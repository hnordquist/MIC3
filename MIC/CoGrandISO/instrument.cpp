/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

© 2019 Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL),
which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration.

All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration.
The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce,
prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or other materials provided
with the distribution.
3. Neither the name of the "Los Alamos National Security, LLC." nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior
written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS
NATIONAL SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRAT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/
// instrument.cpp

#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "SafePS.h"
#include "instrument.h"
#include "GenlStat.h"
#include "Genl4Stat.h"
#include "DataStat.h"
#include "UserProg.h"
#include "User4Prog.h"
#include "CDData.h"
#include "ModParam.h"
#include "SetupData.h"
#include "GrandSetupData.h"
#include "colors.h"
#include <math.h>
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

//////////////////////////////////////////////////////////////////////////////
//
//               UpdateParams
//
//////////////////////////////////////////////////////////////////////////////
//Update Params gets called if initializing an instrument and after modifying it
bool CInstrument::UpdateParams(bool reseting)
{
//	TRACE("CInstrument::UpdateParams()\n");

	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);
	
	//get the DO_DUMP from the ini file
	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

	//get the LOG_MII from the ini file
	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

	//get do binary dump bbm
	m_bDoBinaryBBM = GetPrivateProfileInt(m_pName,"BINARY_BBM",0,m_szIniFile)==1;

	//get the LOG_FILTER from the ini file
	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

	//get the allow auto time set flag from the ini file
	m_bAllowAutoTime = GetPrivateProfileInt(m_pName,"AUTOTIMESET",1,m_szIniFile)==1;

	//get the automatic daily time set
	m_bAutoTimeDaily = GetPrivateProfileInt(m_pName,"AUTOTIMESETDAILY",0,m_szIniFile)==1;

	//reset the trigger indicating that we have not yet had an earlier inquire2/3
	m_bAutoTimeDailyTrigger = false;

	//get the automatic daily time set time
	int iHour, iMinute, iSecond;
	char cAMPM;
	GetPrivateProfileString(m_pName,"AUTOTIMESETDAILYAT","4:30:30 AM",m_szAutoTimeDailyAt,sizeof(m_szAutoTimeDailyAt),m_szIniFile);
	sscanf(m_szAutoTimeDailyAt,"%d:%d:%d %c",&iHour,&iMinute,&iSecond,&cAMPM);
	if (toupper(cAMPM) == 'P') iHour += 12;
	m_uiSecInDayAutoTime = iHour * 3600 + iMinute * 60 + iSecond;

	//get the SLOWRESET from the ini file
	m_dSlowReset = GetPrivateProfileInt(m_pName,"SLOW_RESET",900,m_szIniFile);
	m_dSlowReset *= 1000;

	//get the port string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	char tempport[32];
	if (reseting) strcpy(tempport,m_pPort);
	bool portchange;
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort)-1,m_szIniFile);
	if (strcmp(tempport,m_pPort)==0)
		portchange = false;
	else
		portchange = true;

	//get the node string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	int tempnode;
	tempnode = m_iNode;
	bool nodechange;
	m_iNode = GetPrivateProfileInt(m_pName,NODE,-1,m_szIniFile);
	if (tempnode == m_iNode)
		nodechange = false;
	else
		nodechange = true;
	
	//get the maximum pause of this instrument
	int maxpause = GetPrivateProfileInt(m_pName,"MAXPAUSE",30000,m_szIniFile);
	if (maxpause < 0) maxpause = 0;
	m_iPauseMax = maxpause * 1000;

	//get the data cycle time
	m_dCycleInt = GetPrivateProfileInt(m_pName,"MAXCYCLE",1000,m_szIniFile);
	if (m_dCycleInt < 500) m_dCycleInt = 500;

	//get the maxinqurie interval
	m_dInquireInt = GetPrivateProfileInt(m_pName,"MAXINQUIRE",10,m_szIniFile);
	if (m_dInquireInt < 1) m_dInquireInt = 1;

	//get the maxstatus interval
	m_dStatusInt = GetPrivateProfileInt(m_pName,"MAXSTATUS",0,m_szIniFile);
	if (m_dStatusInt < 0) m_dStatusInt = 0;

	//get the maximum accumulated bbm byte count 
	m_dMaxBBM = GetPrivateProfileInt(m_pName,"MAXBBM",1000,m_szIniFile);
	if (m_dMaxBBM < 0) m_dMaxBBM = 0;

	//get the comm fail NUMBER OF RETRANSMITS prior to COMMFAIL
	m_dCommFail = GetPrivateProfileInt(m_pName,"COMMFAIL",10,m_szIniFile);
	if (m_dCommFail <= 1) m_dCommFail = 10;

	//get the duration to wait for a response to a command
	m_dTimeOut = GetPrivateProfileInt(m_pName,"TIMEOUT",5,m_szIniFile);
	if (m_dTimeOut <= 1) m_dTimeOut = 5;

	char temp[32];
	//get battery and supply limits
	GetPrivateProfileString(m_pName,"BATVOLTH","30.0",temp,sizeof(temp),m_szIniFile);
	m_fbatvolth = (float)atof(temp);

	GetPrivateProfileString(m_pName,"BATVOLTL","25.0",temp,sizeof(temp),m_szIniFile);
	m_fbatvoltl = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P15H","16.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p15h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P15L","14.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p15l = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_M15H","-14.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_m15h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_M15L","-16.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_m15l = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P05H","6.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p05h = (float)atof(temp);

	GetPrivateProfileString(m_pName,"SUP_P05L","4.0",temp,sizeof(temp),m_szIniFile);
	m_fsup_p05l = (float)atof(temp);
	
	m_dtime_err = GetPrivateProfileInt(m_pName,"TIME_ERR",60,m_szIniFile);

	UpdateStrings();
	if (reseting)
	{
		SetLimits(
			m_fbatvolth, m_fbatvoltl,
			m_fsup_p15h, m_fsup_p15l,
			m_fsup_m15h, m_fsup_m15l,
			m_fsup_p05h, m_fsup_p05l,
			m_dtime_err, m_pID);
//		OnModparamsReset();
		if (!m_bPause)
		{
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		}
		else
		{
			KillTimer(TIMER_PAUSE);
			SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		}
	}
	return true;
}

void CInstrument::UpdateStrings()
{
	//create the string for the delete box
	char temp[32];
	strcpy(m_pFullNameSpaces,m_pName);
	strcat(m_pFullNameSpaces,"  Port:");
	strcat(m_pFullNameSpaces,m_pPort);
	strcat(m_pFullNameSpaces,"  Node:");
	if (m_iNode == -1)
		strcat(m_pFullNameSpaces,"N/A");
	else
		strcat(m_pFullNameSpaces,_itoa(m_iNode,temp,10));

	//create the string for the title bar
	CreateFullNameString();  //in ISO base class
}

//ShowInstrument has been rewritten and is below...


void CInstrument::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"18071 GRAND COLLECT Power Fail Drive BBM to Zero.\r\n");
	}
	PauseInstrument(false);
	m_bDriveToZero = bDriveToZero;
	m_dTickToInquire = m_dInquireInt;
	if (bDriveToZero)
		m_bDoAnalyzeAfterDT0 = true;
}

void CInstrument::IssuePrint(FILE* outfile)
{
	IssuePrint2(outfile);
	fprintf(outfile,"MIC Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object: %s\n",
		m_bGoodRegister?"Yes":"No");
	fprintf(outfile,"  Checksum Error Count: %d\n",m_dChecksumCount);
	fprintf(outfile,"  Communications Error Count: %d\n",m_dCommFail);
//	fprintf(outfile,"  Break Message filter on:");
//	fprintf(outfile,(m_dBreakSentCount > g_dMaxRepetition)?"Yes\n":"No\n");
//	fprintf(outfile,"  Timeout Message filter on:");
//	fprintf(outfile,(m_dTimeOutCount > g_dMaxRepetition)?"Yes\n":"No\n");
//	fprintf(outfile,"  Invalid Time Count filter on:");
//	fprintf(outfile,(m_dInvalidTimeCount > g_dMaxRepetition)?"Yes\n":"No\n");
	CString Text;
	switch(m_dCurrent_State){
	case 1000: Text = "About to send first ECHO. (1000)"; break;
	case 1010: Text = "First ECHO sent.  Waiting for response. (1010)"; break;
	case 1020: Text = "Second ECHO sent.  Waiting for response. (1020)"; break;
	case 1030: Text = "Sending second ECHO. (1030)"; break;
	case 1040: Text = "Third ECHO sent.  Waiting for response. (1040)"; break;
	case 1062:
	case 1061:
	case 1060: Text = "Didn't understand ECHO response.  Sending END, USER, YES. (1060-2)"; break;
	case 1070: Text = "Received a ??.  Sending 90.  (1070)"; break;
	case 1100: Text = "Sending DUMPLAST. (1100)"; break;
	case 1110: Text = "DUMPLAST sent.  Waiting for response. (1110)"; break;
	case 1200: Text = "Sending DUMPSTAT. (1200)"; break;
	case 1210: Text = "DUMPSTAT sent.  Waiting for response. (1210)"; break;
	case 1300: Text = "Sending ANALYZE. (1300)"; break;
	case 1310: Text = "ANALYZE sent.  Waiting for response. (1310)"; break;
	case 2000: Text = "About to send INQUIRE2, DUMPBBM, or DUMPSTAT. (2000)"; break;
	case 2100: Text = "INQUIRE2 sent.  Waiting for response. (2100)"; break;
	case 2200: Text = "DUMPBBM sent.  Waiting for response. (2200)"; break;
	case 2210: Text = "Received ENDDUMP record. If no checksum errors about to send DUMPOK. (2210)";break;
	case 2220: Text = "DUMPOK sent.  Waiting for response. (2220)"; break;
	case 2300: Text = "DUMPSTAT sent.  Waiting for response.  (2300)"; break;
	case 8000:
	case 8010:
	case 8020: 
	case 8500:
	case 8510:
	case 8520:
	case 8530: 
	case 8540: 
	case 8550: Text = "Forcing Instrument to Computer's Time. (8000 - 8530)"; break;
	case 9000: Text = "About to send LONG BREAK SET. (9000)"; break;
	case 9010: Text = "About to clear LONG BREAK SET. (9010)"; break;
	case 9500: Text = "COMMUNICATIONS FAILURE -- In slow (15 minute) reset.  (9500)"; break;
	case 9800:
	case 9810: Text = "WRITE FAILURE on BID file (9800 - 9810)"; break;
	case 9900: Text = "COMMUNICATIONS FAILURE -- Register instrument error or cannot write to disk. (9900)"; break;
	default:
			Text.Format("CURRENT STATE (%d)");
	}
	fprintf(outfile,"  %s\n",Text.GetString());
	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	fprintf(outfile,"========================================================\n");
}

void CInstrument::Reset()
{
	if (IsPaused())
	{
		m_dRetransmit = 0;
		m_bInHardFail = false;
		m_dTimeOutCount = 0;
		m_dCurrent_State = 1000;
#ifdef ALLOWTRIGGERHOLDOFF
		DelExclusion();
#endif
	}
}
///////////////////////////////////////////////////////
//
//            ReadExclusion
//            WriteExclusion
//            AddExclusion
//            DelExclusion
//
//    ALL CONDITIONALLY COMPILED
//
//////////////////////////////////////////////////////
#ifdef ALLOWTRIGGERHOLDOFF
//read the exclusion list from the ini file 
//and put them in the CList -- only called
//during instrument creation
void 
CInstrument::ReadExclusion()
{
	int iIndex;
	char szIndex[8];
	char szExclusionValues[32];
	char szExclusionName[_MAX_PATH];
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	iIndex = 1;
	EXCLUSIONREC ExclusionRec;
	do
	{
		_itoa(iIndex,szIndex,10);
		szExclusionValues[0] = NULL;
		GetPrivateProfileString(szExclusionName,szIndex,"",szExclusionValues,sizeof(szExclusionValues),m_szIniFile);
		if (strlen(szExclusionValues))
		{
//try{
			if (sscanf(szExclusionValues,"%d,%d",&ExclusionRec.uiStartTime,&ExclusionRec.uiEndTime)==2)
				m_ExclusionList.AddTail(ExclusionRec);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 86",MB_OK);
//	AfxMessageBox(szExclusionValues,MB_OK);
//}
		}
		iIndex++;
	}while (strlen(szExclusionValues));
}

//write the exclusion list from the CList
//out to the ini file -- only called
//during instrument destruction
void 
CInstrument::WriteExclusion()
{
	int iIndex;
	char szIndex[8];
	char szValue[16];
	char szExclusionName[_MAX_PATH];
	char szExclusionValues[32];

	//get the name of the exclusions section in the ini file
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	iIndex = 1;
	//get the default minutes
	int iExcludeMin = GetPrivateProfileInt(szExclusionName,"MINUTES",-1,m_szIniFile);
	//clear out the old section
	WritePrivateProfileString(szExclusionName,NULL,NULL,m_szIniFile);
	_itoa(iExcludeMin,szValue,10);
	//start a new copy of the section and save the minutes
	WritePrivateProfileString(szExclusionName,"MINUTES",szValue,m_szIniFile);
	//see if we have any exclusion periods
	POSITION pos = m_ExclusionList.GetHeadPosition();
	//if we do then write them to the new section
	if (pos)
	{
		for (int i=0;i < m_ExclusionList.GetCount();i++)
		{
			_itoa(iIndex,szIndex,10);
			EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
			sprintf(szExclusionValues,"%d,%d",sExclusionRec.uiStartTime,sExclusionRec.uiEndTime);
			WritePrivateProfileString(szExclusionName,szIndex,szExclusionValues,m_szIniFile);
			iIndex++;
		}
	}
}

//add an exclusion period into the CList
//that stores them -- called when ever
//the signal(message) is received
void
CInstrument::AddExclusion(LPARAM lParam)
{
	m_pButton->SetPreNum("Ex BBM: ");
	char szExclusionName[_MAX_PATH];
	strcpy(szExclusionName,m_pName);
	strcat(szExclusionName,"_EXCLUSIONS");
	int iExcludeMin;
	if (lParam >= 0)
	{
		if (lParam == 0)
			iExcludeMin = GetPrivateProfileInt(szExclusionName,"MINUTES",1,m_szIniFile);
		else
			iExcludeMin = lParam;
		SetTimer(TIMER_EXCLUSION,iExcludeMin*60*1000,NULL);
		COleDateTime cOleStart = COleDateTime::GetCurrentTime();
		COleDateTime cOleBase(1952,1,1,0,0,0);
		COleDateTimeSpan cOleJulian;
		cOleJulian = cOleStart - cOleBase;
		EXCLUSIONREC ExclusionRec;
		ExclusionRec.uiStartTime = (unsigned int)cOleJulian.GetTotalSeconds();
		ExclusionRec.uiEndTime = ExclusionRec.uiStartTime + iExcludeMin*60;
		m_ExclusionList.AddTail(ExclusionRec);
		CString cString;
		cString.Format("Exclusion Message: %d to %d, %d min",
			ExclusionRec.uiStartTime,ExclusionRec.uiEndTime,iExcludeMin);
		char szString[128];
		strcpy(szString,cString);
		PostToWatch(szString);
		UpdateSummary(szString);
	}
	else
	{
		char szString[64];
		sprintf(szString,"Bad Exclusion Message, %d min", lParam);
		PostToWatch(szString);
		UpdateSummary(szString);
	}
}

void
CInstrument::DelExclusion()
{
	//remove all of the exclusion periods
	m_ExclusionList.RemoveAll();
	//update the INI file
	WriteExclusion();
	//update the button
	m_pButton->SetPreNum("BBM: ");
	//stop the exclusion timer
	KillTimer(TIMER_EXCLUSION);
	//tell the world
	char szString[128];
	strcpy(szString,"Exclusion Clear Message Received");
	PostToWatch(szString);
	UpdateSummary(szString);
}
#endif

void CInstrument::PauseInstrument(bool pause)
{
//	TRACE("CInstrument::PauseInstrument(%s)\n", pause?"true":"false");

	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);
	//KillTimer(TIMER_UPDATE);

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_pPropertySheet)
		m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"18070 GRAND COLLECT take data stopped.\r\n":
					"18080 GRAND COLLECT take data started.\r\n");
	m_bPause = pause;
	m_dTimeOutCount = 0;
	m_dBreakSentCount = 0;
	m_dInvalidTimeCount = 0;

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		//SetTimer(TIMER_UPDATE,60000,NULL);
//		m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
	}
	else
	{
//		m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
		if (m_dCurrent_State == 9500) 
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			m_dCurrent_State = 1000;
		}
		SetTimer(TIMER_TICK,m_dCycleInt,NULL);
		m_bIsPaused = false;
	}

	// Have the MICDlg review the status of the buttons
	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}


// CInstrument
CInstrument::CInstrument()
{
//	TRACE("CInstrument::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}
CInstrument::CInstrument(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CInstrument::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}

void CInstrument::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;

	strncpy(m_pType, GRAND_DISCOVERY_TYPE, sizeof(m_pType));

	CString csComma(",");
	CString csExtensions = 
		CString(BID_SUFFIX) + csComma + 
		SOH_SUFFIX + csComma +
		PFM_SUFFIX + csComma +	//DR 353
		CEV_SUFFIX;				//DR 353

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));
	//strncpy(m_szFileExtensionsList,BID_SUFFIX,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pGeneralStatPage	= NULL;
	m_pGeneral4StatPage = NULL;
	m_pDataStatus		= NULL;
	m_pUserProgStatus	= NULL;
	m_pUser4ProgStatus	= NULL;
	m_pCDData			= NULL;
	m_pModParams		= NULL;
	m_pSetupData		= NULL;
	m_pSetup4Data		= NULL;
	m_pLegend			= NULL;

	m_pPropertySheet	= NULL;

	m_bDriveToZero		= false;
	m_bDoAnalyzeAfterDT0= false;
	m_bInHardFail		= false;
	m_bGoodRegister		= false;
	m_bDoOneDumpStat	= false;	//SCR00074
	m_bAutoTimeDailyTrigger = false;
	m_dHardFailCount	= 0;
	m_dSlowReset		= SLOWRESET;
	m_dPrevBIDTime		= 0;
	m_dTimeOutCount		= 0;
	m_dInvalidTimeCount = 0;

	m_pComm = NULL;

	m_eMonitorVers = UNKNOWN;

	m_iGeneral_Tab	= 0;
	m_iGeneral4_Tab	= 0;
	m_iData_Tab		= 0;
	m_iUserProg_Tab	= 0;
	m_iUser4Prog_Tab	= 0;
	m_iInstSetup_Tab	= 0;
	m_iInst4Setup_Tab	= 0;

	m_fTNSRHigh		= 0.0;
	m_fTNSRLow		= 0.0;
	m_dTNSRFirst	= true;

	m_fDIRHigh		= 0.0;
	m_fDIRLow		= 0.0;
	m_dDIRFirst		= true;

	m_fFBHigh		= 0.0;
	m_fFBLow		= 0.0;
	m_bFBFirst		= true;

	m_bBatteryOk	= true;
	m_bBatteryTolOk	= true;
	m_bP5VoltOk		= true;
	m_bP15VoltOk	= true;
	m_bM15VoltOk	= true;
	m_bACPowerOk	= true;
	m_bACPowerLoss	= false;
	m_bBBMOk		= true;
	m_bCountRateTamperOk = true;
	m_bRMSSigmaTamperOk = true;
	m_bInMII		= false;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;

	m_bFirst15Rec	= true;

	m_fLowBV = m_fCurBV = m_fHighBV = 0.0;
	m_bFirstBV = true;
	m_fLowP15 = m_fCurP15 = m_fHighP15 = 0.0;
	m_bFirstP15 = true;
	m_fLowM15 = m_fCurM15 = m_fHighM15 = 0.0;
	m_bFirstM15 = true;
	m_fLowP5 = m_fCurP5 = m_fHighP5 = 0.0;
	m_bFirstP5 = true;

//	m_dToClose				= 0;

	m_bStartOfFile			= true;
	m_dCWStart				= -1; //neither COLD START (0) nor WARM START (1)

	m_szCurrentFileName[0]	= NULL;
	m_szPreviousGTime[0]	= NULL;
	m_dCurrentDay			= -1;
	m_dPreviousBIDTime		= 0xa5a5a5a5;

	m_dAt					= 0;
	m_dBinaryAt				= 0;
	m_dAcquireRecordCount	= 0;

	m_dBinaryChecksum		= 0;
	m_dBinaryState			= 0;
	m_dBinaryAT				= 0;
	m_usLength				= 0;
	m_usMessageType			= 0;
	m_dRecLengthCount		= 0;
	m_dGivenChecksum		= 0;
	m_dChecksum				= 0;
	m_bCouldNotOpenBinaryFile = false;

	m_bFCHVError			= false;
	m_bICHVError			= false;
	m_iFCHVError			= 0;
	m_iICHVError			= 0;
	m_dCommFail				= 0;

	m_uiLastStatusRecTime					= 0;		//DR00406
	m_uiLastOffsetRecTime					= 0;		//DR00406
	m_uiLastBatteryRecTime					= 0;		//DR00406
	m_uiLastTripNSRecTime					= 0;		//DR00406
	m_uiLastTripN2RecTime					= 0;		//DR00406
	m_uiLastInformRecTime					= 0;		//DR00406
	m_uiLastDualIonRecTime					= 0;		//DR00406
	m_uiLastGenStatRecTime					= 0;		//DR00406
	m_uiLastMon_A_RecTime					= 0;		//DR00406
	m_uiLastMon_B_RecTime					= 0;		//DR00406
	m_uiLastGeneralConfigRecordTime			= 0;		//DR00406
	m_uiLastInstrumentStatusRecordTime		= 0;		//DR00406
	m_uiLastInstrumentInfoRecordTime		= 0;		//DR00406
	m_uiLastDualCurrentModeConfigRecordTime	= 0;		//DR00406
	m_uiLastTriggerConfigRecordTime			= 0;		//DR00406
	m_uiLastChannel0ConfigRecordTime		= 0;		//DR00406
	m_uiLastChannel1ConfigRecordTime		= 0;		//DR00406
	m_uiLastChannel2ConfigRecordTime		= 0;		//DR00406
	m_uiLastChannel3ConfigRecordTime		= 0;		//DR00406
	m_uiLastChannel4ConfigRecordTime		= 0;		//DR00406
	m_uiLastChannel5ConfigRecordTime		= 0;		//DR00406

	int i;
	for (i = 0; i < sizeof(SummaryStaticColors)/sizeof(COLORREF); i++)
		SummaryStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(GeneralStaticColors)/sizeof(COLORREF); i++)
		GeneralStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(General4StaticColors)/sizeof(COLORREF); i++)
		General4StaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(UserStaticColors)/sizeof(COLORREF); i++)
		UserStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(User4StaticColors)/sizeof(COLORREF); i++)
		User4StaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(DataStaticColors)/sizeof(COLORREF); i++)
		DataStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(SetupStaticColors)/sizeof(COLORREF); i++)
		SetupStaticColors[i] = COLOR_DEFAULT_TEXT;

	for (i = 0; i < sizeof(Setup4StaticColors)/sizeof(COLORREF); i++)
		Setup4StaticColors[i] = COLOR_DEFAULT_TEXT;
	
	for (i = 0; i < (sizeof(m_iMessageLimiter)/sizeof(int)); i++)
		m_iMessageLimiter[i] = m_iMaxRepetition;//0-302 used, see WriteToFileEx

}

CInstrument::~CInstrument()
{
//	TRACE("CInstrument::DTR\n");

	//tell the world we are shutting down
#ifdef ALLOWTRIGGERHOLDOFF
	WriteExclusion();
#endif
	char str[512];									//dr00115
	char str2[128];
	strcpy(str,"18090 GRAND COLLECT stopped:");
	unsigned long size = sizeof(str2);
	if (::GetUserName(str2,&size))
	{
		strcat(str," Logon ID:");
		strcat(str,str2);
	}
	strcat(str," MIC User ID:");					//dr00115
	GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",str2,sizeof(str2),m_szIniFile);
	strcat(str,str2);														//dr00115
	strcat(str,"\r\n");														//dr00115
	WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, str);

	//destroy the button on the main screen dialog
	//if (m_pButton) delete m_pButton;
	
	//destroy the property sheet itself
	if (m_pPropertySheet)
	{
		m_pPropertySheet->EndDialog(0);
		delete m_pPropertySheet; //PropertySheet
	}
}

/* This from MIC 1.9.0.7
BEGIN_MESSAGE_MAP(CInstrument, CWnd)
	//{{AFX_MSG_MAP(CInstrument)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(IDC_RECEIVE_RESULT,	OnReceive)
	ON_MESSAGE(IDC_RECEIVE_OOB,		OnReceiveOOB)
	ON_MESSAGE(IDC_RECEIVE,			OnReceiveData)
END_MESSAGE_MAP()
*/
BEGIN_MESSAGE_MAP(CInstrument, CWnd)
	//{{AFX_MSG_MAP(CInstrument)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE           (IDC_RECEIVE_RESULT, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceiveOOB)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceiveData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstrument message handlers

void CInstrument::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEvent)
	{
	//max pause time ran out so we need to un-pause this instrument
	case TIMER_PAUSE:	
		PauseInstrument(false);
		break;

	//during pause update the network information 1 per minute
	//case TIMER_UPDATE:
	//	m_pButton->Redraw(false);
	//	break;

	//normal timer increment -- need to send this to the state machine
	case TIMER_TICK:
		PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
		break;

#ifdef ALLOWTRIGGERHOLDOFF
	//turn off the EXCLUSION flag
	case TIMER_EXCLUSION:
		m_pButton->SetPreNum("BBM: ");
		KillTimer(TIMER_EXCLUSION);
		break;
#endif
	}
	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEvent);
}

bool CInstrument::CreateInstrument()
{
//	TRACE("CInstrument::CreateInstrument()\n");

//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	m_bPause = true;
	m_IsMiniGrand = -1;
	m_dBreakSentCount = 0;
	m_bIsPaused = true;
	m_iCurrentBBM = 0;

	m_bGoodRegister = false;

	if (!UpdateParams(false))  //<<QA>> is this needed?  Comm detection is different.
		return false;

	CString cs = AfxRegisterWndClass(NULL, NULL, NULL, NULL);

	BOOL result = CreateEx(
			0,               //DWORD dwExStyle, 
			(LPCTSTR)cs,     //LPCTSTR lpszClassName, 
			m_pName,         //LPCTSTR lpszWindowName, 
			0,               //DWORD dwStyle, 
			CRect(0,0,0,0),  //const RECT& rect, 
			NULL,            //CWnd* pParentWnd, 
			0                //UINT nID, 
			);				//LPVOID lpParam = NULL);

	if (result) 
	{
		char bbmstring[] = {"BBM: "};
		BSTR bstr;
		Convert_Character_String_to_BSTR(bbmstring, bstr);
		m_pButton->put_PreNumericString(bstr);

		m_pButton->put_DisplayNumericString(VARIANT_TRUE, 10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		m_pButton->put_Numeric(0);

		//get the time flag status
		m_iFlagTime = GetPrivateProfileInt(m_pName,FLAGTIME,-1,m_szIniFile);
		if (m_iFlagTime == 1)
		{
			WriteToFileEx(307,TO_PFM,TYPE_COMP,NULL,"30003 Icon turned on from previous execution of MIC [TIME icon on]\r\n");
			m_pButton->ShowIcon_Clock(VARIANT_TRUE);
		}
		//get the other flag status (frown face is "other")
		m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		if (m_iFlagOthr == 1)
		{
			WriteToFileEx(308,TO_PFM,TYPE_COMP,NULL,"30004 Icon turned on from previous execution of MIC [FROWN FACE icon on]\r\n");
			m_pButton->ShowIcon_Frown(VARIANT_TRUE);
		}
		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,"FLAGCOMM",-1,m_szIniFile);
		if (m_iFlagComm == 1)
		{
			WriteToFileEx(309,TO_PFM,TYPE_COMP,NULL,"30005 Icon turned on from previous execution of MIC [FLAG icon on]\r\n");
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
		}

#ifdef ALLOWTRIGGERHOLDOFF
		ReadExclusion();
#endif

	//set the time to start data collecting 
	SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);

	SetLimits(m_fbatvolth, m_fbatvoltl,
				m_fsup_p15h, m_fsup_p15l,
				m_fsup_m15h, m_fsup_m15l,
				m_fsup_p05h, m_fsup_p05l,
				m_dtime_err, m_pID);
//			OnModparamsReset();

	m_dCurrent_State = 1000;
	m_dTick_Count = 0;
	m_dTickToInquire = 0;

	VARIANT_BOOL vbResult;

	HWND m_hWndSafe = GetSafeHwnd();

	m_pComm->RegisterISO(
			(ULONG)m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_FALSE,  // ~OOB
			&vbResult);

	m_bGoodRegister = (vbResult == VARIANT_TRUE);

	m_pComm->RegisterISO(
			(ULONG)m_hWnd,
			m_iNode+OOBFLAG,
			STANDARD_ISO,
			VARIANT_TRUE,	// OOB
			NULL);			//don't care about the return value here

	m_dRetransmit		= 0;		//no retransmitions yet
	m_dLongBreak		= 0;		//not in long break
	m_bdoingBBM			= true;		//start by doing dump bbm
	m_bdumpOK			= false;	//don't have a good dumpbbm yet
	m_bbbmVal			= false;	//don't have a bbm value yet
	m_bChecksumFail		= false;	//not checksum fail yet
	m_dChecksumCount	= 0;		//no checksum fails yet
	m_bInHardFail		= false;	//not in hard fail of any type yet
	m_bInitInitTimeOut	= true;		//need to initialize initial time out
	m_bInDumpLast		= false;	//not in dumplast sequence yet

	m_pNow = COleDateTime::GetCurrentTime();
	m_dPreviousDay = m_pNow.GetDay();

		//SCR 118
		//EstablishVersionString();

	//tell the files that mic is starting for this instrument

	WriteToFile(TO_CEV | TO_PFM, 
		(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);// GRAND COLLECT started

	}  //if m_bGoodRegister
	
	return m_bGoodRegister;
}

//////////////////////////////////////////////////////////////////////////////
//
//                  CollectParametersForNewISO
//
//////////////////////////////////////////////////////////////////////////////
bool CInstrument::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CInstrument::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	CModParam modparam_page(IDD_MODPARAMS_PAGE,
	                             this,        //parent
								 m_szIniFile, 
								 true);       //Temporary ISO = true

	modparam_page.SetPropertySheet(&tps,m_pName);
	tps.AddPage(&modparam_page);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,GRAND_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}

void CInstrument::SetLocalRegisteredMessageValues()
{
//	TRACE("CInstrument::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}




void CInstrument::SetLimits(
	float batvolth, float batvoltl,
	float sup_p15h, float sup_p15l,
	float sup_m15h, float sup_m15l,
	float sup_p05h, float sup_p05l,
	int time_err,	char* Id)
{	
	m_fBatVoltH=batvolth;
	m_fBatVoltL=batvoltl;
	m_fSup_P15H=sup_p15h;
	m_fSup_P15L=sup_p15l;
	m_fSup_M15H=sup_m15h;
	m_fSup_M15L=sup_m15l;
	m_fSup_P05H=sup_p05h;
	m_fSup_P05L=sup_p05l;
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

void CInstrument::UpdateSummary(char *Message)
{
	CString cString(Message);
	cString.TrimRight("\r\n");
	if (m_pPropertySheet)
		m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cString));

	m_cSummaryRecent[5] = m_cSummaryRecent[4];
	m_cSummaryRecent[4] = m_cSummaryRecent[3];
	m_cSummaryRecent[3] = m_cSummaryRecent[2];
	m_cSummaryRecent[2] = m_cSummaryRecent[1];
	m_cSummaryRecent[1] = m_cSummaryRecent[0];
	m_cSummaryRecent[0] = cString;
}

void CInstrument::SetChecksumDisplay()
{
	CString cString;
	cString = m_cCheckSumMsg;
	cString += " ";
	cString += m_cCommStatus;
	SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,cString);
	SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,cString);
	if (m_pPropertySheet)
	{
		if (m_pGeneralStatPage)
			m_pGeneralStatPage->SetDlgItemText(IDC_GENERAL_CHECKSUM,cString);
		if (m_pGeneral4StatPage) 
			m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM,cString);
	}
}

void CInstrument::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
	char* cbuff = (char*)LParam;
	char cbuff2[1024];
	char cbuff3[1024];
	cbuff2[1023] = NULL;
	//int length = min(sizeof(cbuff2)-1,(int)WParam);

	try
	{
		strcpy(cbuff2,"15320 OOB: ");
		int length2 = strlen(cbuff2) + (int)WParam;
		strncat(cbuff2,cbuff,(int)WParam);
		cbuff2[length2] = NULL;

		strncpy(cbuff3,cbuff,(int)WParam);
		cbuff3[(int)WParam] = NULL;
		m_cCommStatus = cbuff3;
		SetChecksumDisplay();
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP,NULL,"15310 Bad LParam Address Received (OOB)");
		if (m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"Bad LParam Address Received (OOB)");
		return;
	}

	UpdateSummary(cbuff2);
	strcat(cbuff2,"\r\n");

	WriteToFile(TO_PFM, TYPE_COMP, NULL, cbuff2);

	if (m_bDoDump)
		WriteToFile(TO_DMP,TYPE_DUMP, NULL, cbuff2);

	//delete [] (char*)LParam;
	CoTaskMemFree((void *)LParam);
}


CString CInstrument::GetString(PAGETYPE ePageType, UINT uiDlgItem)
{
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  return SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST];
		break;
	case GENERALPAGE:
		if ((uiDlgItem >= IDC_GENERAL_FIRST) && (uiDlgItem <= IDC_GENERAL_LAST)) 
		  return GeneralStrings[uiDlgItem-IDC_GENERAL_FIRST];
		break;
	case GENERAL4PAGE:
		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
		  return GeneralStrings[uiDlgItem-IDC_GENERAL4_FIRST];
		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  return User4Strings[uiDlgItem-IDC_USER4PROG_FIRST];
		break;
	case USERPAGE:
		if ((uiDlgItem >= IDC_USERPROG_FIRST) && (uiDlgItem <= IDC_USERPROG_LAST)) 
		  return UserStrings[uiDlgItem-IDC_USERPROG_FIRST];
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  return DataStrings[uiDlgItem-IDC_DATA_FIRST];
		break;
	}
	return CString("");
}


void CInstrument::SetString(PAGETYPE ePageType,UINT uiDlgItem,CString cString)
{
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST] = cString;
		break;
	case GENERALPAGE:
		if ((uiDlgItem >= IDC_GENERAL_FIRST) && (uiDlgItem <= IDC_GENERAL_LAST)) 
		  GeneralStrings[uiDlgItem-IDC_GENERAL_FIRST] = cString;
		break;
	case GENERAL4PAGE:
		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
		  General4Strings[uiDlgItem-IDC_GENERAL4_FIRST] = cString;
		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  User4Strings[uiDlgItem-IDC_USER4PROG_FIRST] = cString;
		break;
	case USERPAGE:
		if ((uiDlgItem >= IDC_USERPROG_FIRST) && (uiDlgItem <= IDC_USERPROG_LAST)) 
		  UserStrings[uiDlgItem-IDC_USERPROG_FIRST] = cString;
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  DataStrings[uiDlgItem-IDC_DATA_FIRST] = cString;
		break;
	}
}

void CInstrument::SetColor(PAGETYPE ePageType,UINT uiDlgItem,COLORREF rgbColor)
{
	if ((rgbColor == COLOR_RED) || (rgbColor == COLOR_YELLOW))
	{
//		WriteToFileEx(306,TO_PFM,TYPE_COMP,NULL,"30005 SQUARE icon on\r\n");
		//m_pButton->SetQuestChar(CHICK_CHAR,false);
		//m_pButton->SetQuest(true,true);
		m_pButton->ShowIcon_Square(VARIANT_TRUE);
	}

	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		  SummaryStaticColors[uiDlgItem-IDC_SUMMARY_FIRST] = rgbColor;
		break;
	case GENERALPAGE:
		if ((uiDlgItem >= IDC_GENERAL_FIRST) && (uiDlgItem <= IDC_GENERAL_LAST)) 
		  GeneralStaticColors[uiDlgItem-IDC_GENERAL_FIRST] = rgbColor;
		break;
	case GENERAL4PAGE:
		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
		  General4StaticColors[uiDlgItem-IDC_GENERAL4_FIRST] = rgbColor;
		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		  User4StaticColors[uiDlgItem-IDC_USER4PROG_FIRST] = rgbColor;
		break;
	case USERPAGE:
		if ((uiDlgItem >= IDC_USERPROG_FIRST) && (uiDlgItem <= IDC_USERPROG_LAST)) 
		  UserStaticColors[uiDlgItem-IDC_USERPROG_FIRST] = rgbColor;
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		  DataStaticColors[uiDlgItem-IDC_DATA_FIRST] = rgbColor;
		break;
	}
}

void CInstrument::OnReceiveData(WPARAM WParam, LPARAM LParam)
{
	unsigned char* cbuff =(unsigned char*)LParam;
	char* temp_at;
	unsigned char c;
	if (WParam == 0)
		return;
	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"15330 Bad LParam Address Received (1000)");
		if (m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"15330 Bad LParam Address Received (1000)");
		CoTaskMemFree((void *)LParam);
		return;
	}

	for (unsigned int index = 0; index < WParam; index ++)
	{
		m_pAccumulator[m_dAt] = (char)(cbuff[index] & 0x7f);

		//m_dAt can get reset externally
		if (m_dAt == 0) 
		{
			m_dBinaryAt = 0;
			m_dBinaryChecksum = 0;
			m_dBinaryState = 0;
		}

		//do we have a "special" record containing multiple binary records
		if ((m_dAt > 1) && (m_pAccumulator[0] == '3') && (m_pAccumulator[1] == '3'))
		{
			//compute checksum
			m_dBinaryChecksum += (unsigned char)cbuff[index];
			
			//protect the rest of memory
			if (m_dBinaryAt >= BINARYACCUMULATOR_SIZE)
			{
				m_dBinaryAt = 0;
				m_dBinaryState = 0;
				m_dBinaryChecksum = 0;
			}

			//save the current byte
			m_pBinaryAccumulator[m_dBinaryAt] = (BYTE)cbuff[index];
			m_dAt++;
			switch (m_dBinaryState)
			{
			case 0:
			case 1://get individual record length
				m_usLength = m_pBinaryAccumulator[m_dBinaryAt];
				m_dBinaryState = 2;
				break;
			case 2://get record type
				m_usMessageType = m_pBinaryAccumulator[m_dBinaryAt];

				if	(
					((m_usMessageType == 0x0015) && (m_usLength == 0x0025)) ||//verified
					((m_usMessageType == 0x00ef) && (m_usLength == 0x00be)) ||//verified
					((m_usMessageType == 0x0012) && (m_usLength == 0x002e)) ||//verified was 0x2a
					((m_usMessageType == 0x0012) && (m_usLength == 0x002a)) ||
					((m_usMessageType == 0x0013) && (m_usLength == 0x0035)) ||//verified
					((m_usMessageType == 0x0018) && (m_usLength == 0x0008)) ||//verified
					((m_usMessageType == 0x001a) && (m_usLength == 0x001c)) ||//verified was 0x0f
					((m_usMessageType == 0x001a) && (m_usLength == 0x000f)) ||
					((m_usMessageType == 0x001b) && (m_usLength == 0x002b)) ||//verified was 0x15
					((m_usMessageType == 0x001b) && (m_usLength == 0x0015)) ||
					((m_usMessageType == 0x001c) && (m_usLength == 0x0039)) ||//verified was 0x05
					((m_usMessageType == 0x001c) && (m_usLength == 0x0005)) ||
					((m_usMessageType == 0x001d) && (m_usLength == 0x0000)) ||
					//((m_usMessageType == 0x1e) && (m_usLength == 0x00)) || never
					//((m_usMessageType == 0x1f) && (m_usLength == 0x00)) || never
					((m_usMessageType == 0x0030) && (m_usLength == 0x000d)) ||//verified
					((m_usMessageType == 0x0031) && (m_usLength == 0x002b)) ||//verified
					//m_usMessageType == 0x004e) && (m_usLength == 0012)) never
					((m_usMessageType == 0x0041) && (m_usLength == 0x002a)) ||
					((m_usMessageType == 0x0042) && (m_usLength == 0x0025)) ||
					((m_usMessageType == 0x0043) && (m_usLength == 0x0018)) ||
					((m_usMessageType == 0x0044) && (m_usLength == 0x0018)) ||
					((m_usMessageType == 0x0045) && (m_usLength == 0x0028)) ||
					((m_usMessageType == 0x0046) && (m_usLength == 0x001b)) ||
					((m_usMessageType == 0x0047) && (m_usLength == 0x001b)) ||
					((m_usMessageType == 0x0048) && (m_usLength == 0x001b)) ||
					((m_usMessageType == 0x0049) && (m_usLength == 0x001b)) ||
					((m_usMessageType == 0x004a) && (m_usLength == 0x001b)) ||
					((m_usMessageType == 0x004b) && (m_usLength == 0x001b)) 
					//4d "analyze response2" should never come as binary message
					)
				{
					m_dBinaryState = 3;
					m_dRecLengthCount = 0;
//TRACE("SafePS::OnReceive ML: %02x  MT: %02x\n",m_usLength, m_usMessageType);
				}
				else if (m_usMessageType == 0xbe)
					m_usLength = m_usMessageType;
				else
				{
					m_dBinaryState = 0;
					m_dAt = 0;
					m_dBinaryAt = 0;
					m_dBinaryState = 0;
					m_dBinaryChecksum = 0;
					CString cString;
					cString.Format("15340 Apparent Comm Fault During DUMPBBBM MsgType=%02x MsgLength=%02x\r\n",m_usMessageType,m_usLength);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, (LPSTR)LPCSTR(cString));
					PostToWatch((LPSTR)LPCSTR(cString));
				}
				break;
			case 3://get full record and first checksum byte
				m_dRecLengthCount++;
				//where's the beef
				if ((m_usLength == 0xBE) && (m_usMessageType == 0xEF))
				{
					m_dBinaryState = 4;//done go get checksum
								//compute checksum
					m_dBinaryChecksum -= (unsigned char)cbuff[index];

					m_dGivenChecksum = m_pBinaryAccumulator[m_dBinaryAt];
				}
				else if (m_dRecLengthCount >= m_usLength)
					m_dBinaryState = 0;
				break;
			case 4://get checksum byte 2
				//compute checksum
				m_dBinaryChecksum -= (unsigned char)cbuff[index];
				m_dBinaryChecksum += '3';
				m_dBinaryChecksum += '3';

				//combine the two bytes of checksum
				m_dGivenChecksum = (m_dGivenChecksum << 8) + m_pBinaryAccumulator[m_dBinaryAt];

				//get ready for a new message
				m_dBinaryState = 0;
				m_pAccumulator[0] = ' ';//cause a failure of the "33" check
				m_pAccumulator[1] = ' ';

				//did we get a good 33 message
				if ((m_dBinaryChecksum & 0xffff) == m_dGivenChecksum)
				{

					if ((m_dBinaryAt >= 4) &&
						(m_pBinaryAccumulator[m_dBinaryAt-3]==0xBE) &&
						(m_pBinaryAccumulator[m_dBinaryAt-2]==0xEF) 
						)
					{
COleDateTime cStart = COleDateTime::GetCurrentTime();
//DWORD dwOldPriorityClass = GetPriorityClass(m_hWnd);
//if (m_bDoDump)
//	WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Bumping priority while processing DUMPBBBM\r\n");
//DWORD dwNewPriorityClass = SetPriorityClass(m_hWnd,HIGH_PRIORITY_CLASS);
						bool bDone = false;
						//call each message handler for type
						int dBinaryAt=0;
						while (!bDone)
						{
							short message_type = UNKNOWN_REC;
							short message_length = 0;
							message_type = (short)(m_pBinaryAccumulator[dBinaryAt+1]);
							message_length = (short)(m_pBinaryAccumulator[dBinaryAt]);
							//TRACE("CInstrument::OnReceive message_type:%d\n",message_type);
							switch(message_type)
							{
							case STATUS_RECORD:			//appendix a1	12			
								if (message_length == 0x002e)
									dBinaryAt += BinaryStatusRec(&m_pBinaryAccumulator[dBinaryAt]);	
								else
									dBinaryAt += BinaryStatusRecOld(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case OFFSET_RECORD:			//appendix a2	13			
								dBinaryAt += BinaryOffsetRec(&m_pBinaryAccumulator[dBinaryAt]);	
								break;	
//							case ID_RECORD:				//appendix a3	14				
//								dBinaryAt += BinaryID_Record(&m_pBinaryAccumulator[dBinaryAt]);	
//								break;	
							case ACQUIRE_RECORD:		//appendix a4	15		
								dBinaryAt += BinaryAcquirRec(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case BATTERY_RECORD:		//appendix a7	18		
								dBinaryAt += BinaryBatteryRec(&m_pBinaryAccumulator[dBinaryAt]);
								break;
//							case TRIP_NEU_STAT_RECORD:	//appendix a8	19	
//								dBinaryAt += BinaryTripNSRec(&m_pBinaryAccumulator[dBinaryAt]);	
//								break;	
							case DUAL_ION_STAT_RECORD:	//appendix a9	1a	
								if (message_length == 0x001c)
									dBinaryAt += BinaryDualIonRec(&m_pBinaryAccumulator[dBinaryAt]);
								else
									dBinaryAt += BinaryDualIonRecOld(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case GEN_STATUS_RECORD:		//appendix a10	1b	
								if (message_length == 0x002b)
									dBinaryAt += BinaryGenStatRec(&m_pBinaryAccumulator[dBinaryAt]);
								else
									dBinaryAt += BinaryGenStatRecOld(&m_pBinaryAccumulator[dBinaryAt]);
								break;	
							case MONITOR_A_RECORD:		//appendix a11	1c		
								if (message_length == 0x0039)
									dBinaryAt += BinaryMon_A_Rec(&m_pBinaryAccumulator[dBinaryAt]);	
								else
									dBinaryAt += BinaryMon_A_RecOld(&m_pBinaryAccumulator[dBinaryAt]);
								break;
//							case TRIP_NEU2_RECORD:		//appendix a12	1d		
//								dBinaryAt += BinaryTripN2Rec(&m_pBinaryAccumulator[dBinaryAt]);	
//								break;
							case MONITOR_ID2_RECORD:	//appendix a13	1e	
								dBinaryAt += BinaryID2_Record(&m_pBinaryAccumulator[dBinaryAt]);
								break;
							case ANALYZE_OK_RECORD:		//appendix a14	1f	
								dBinaryAt += BinaryAnalyzeOk(&m_pBinaryAccumulator[dBinaryAt]);	
								break;
							case INFORMATION_RECORD:	//appendix a15	30	
								dBinaryAt += BinaryInformRec(&m_pBinaryAccumulator[dBinaryAt]);	
								break;
							case MONITOR_B_RECORD:		//appendix a16	31		
								dBinaryAt += BinaryMon_B_Rec(&m_pBinaryAccumulator[dBinaryAt]);	
								break;	
							
							//monitor version 4.10 and above only
							case MONITOR_ID3_RECORD:	//4e
								dBinaryAt += BinaryID3_Record(&m_pBinaryAccumulator[dBinaryAt]); 
								break;	//4e
//							case ANALYZE_OK3_RECORD:	//4d
//								dBinaryAt += BinaryAnalyzeOk3(&m_pBinaryAccumulator[dBinaryAt]);
//								break;	//4d 
							case GENERAL_CONFIG_RECORD:	//41
								dBinaryAt += BinaryGeneralConfigRecord(&m_pBinaryAccumulator[dBinaryAt]);	
								break;	//41
							case INSTRUMENT_STATUS_RECORD: //42
								dBinaryAt += BinaryInstrumentStatusRecord(&m_pBinaryAccumulator[dBinaryAt]); 
								break;//42
							case INSTRUMENT_INFO_RECORD:   //43
								dBinaryAt += BinaryInstrumentInfoRecord(&m_pBinaryAccumulator[dBinaryAt]); 
								break; //43
							case DUAL_CURRENTMODE_CONFIG_RECORD: //44
								dBinaryAt += BinaryDualCurrentModeConfigRecord(&m_pBinaryAccumulator[dBinaryAt]); 
								break;//44
							case TRIGGER_CONFIG_RECORD:	//45
								dBinaryAt += BinaryTriggerConfigRecord(&m_pBinaryAccumulator[dBinaryAt]); 
								break; //45
							case CHANNEL0_CONFIG_RECORD: //46
							case CHANNEL1_CONFIG_RECORD: //47
							case CHANNEL2_CONFIG_RECORD: //48
							case CHANNEL3_CONFIG_RECORD: //49
							case CHANNEL4_CONFIG_RECORD: //4a
							case CHANNEL5_CONFIG_RECORD: //4b
						case CHANNEL6_CONFIG_RECORD: //4c
								dBinaryAt += BinaryChannelNConfigRecord(&m_pBinaryAccumulator[dBinaryAt],message_type); 
								break; 
							//end of monitor version 4 and above only

							default:
								{
									CString cString;
									cString.Format("15350 Message Type Unknown: Message Type=%02x\r\n",message_type);
									WriteToFile(TO_PFM, TYPE_COMP, NULL, (LPSTR)LPCSTR(cString));
									PostToWatch((LPSTR)LPCSTR(cString));
									bDone = true;
									m_dBinaryAt = 0;
									m_dAt = 0;
								}
							}
							//account for beef and checksum
							//if we are that close then we are done
							if ((dBinaryAt+4) >= m_dBinaryAt)
							{
								bDone = true;
								m_dBinaryAt = 0;
								m_dAt=0;
							}
						}
	//added to look at cause of delay
	if (m_bDoDump)
	{
		COleDateTime cEnd = COleDateTime::GetCurrentTime();
		COleDateTimeSpan cSpan;
		cSpan = cEnd - cStart;
		if (cSpan.GetTotalSeconds() >= 2.0)
		{
			char szTemp[128];
			sprintf(szTemp,"SYSTEM DELAY Processing DUMPBBBM took %.0f sec. wallclock\r\n",cSpan.GetTotalSeconds());
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, szTemp);
		}
	}
//	if (dwOldPriorityClass)
//	{
//		SetPriorityClass(m_hWnd,dwOldPriorityClass);
//		if (m_bDoDump)
//			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Resetting priority back down\r\n");
//	}


					}

					// Identify the trivial case of hex {33,33,be,ef,02,13}.
					// Be sure m_dAt and other variables are zeroed out for
					// next entry to OnReceive.
					//
					else if ((m_dBinaryAt < 4) &&
						(m_pBinaryAccumulator[m_dBinaryAt-3]==0xBE) &&
						(m_pBinaryAccumulator[m_dBinaryAt-2]==0xEF) 
						)
					{
						m_dBinaryAt = 0;
						m_dAt = 0;
						m_dBinaryState = 0;
						m_dBinaryChecksum = 0;
					}
					////////////////////////////////////////////////////////////
					PostMessage(IDC_RECEIVE_RESULT,END_DUMP2_RECORD,true);
					m_dNumberOfAcquires = -1;
				}
				else
				{
					char Record[] = "Binary 33 record\r\n";
		
//				//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//					CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//					CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//				//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//				//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//				//	CModParam* modparpage  = m_pParent->m_pModParams;

					COleDateTime Now = COleDateTime::GetCurrentTime();
					CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S\n");
					DateTime += Record;

					m_cGENERAL_CHECKSUM_text = DateTime;
					m_iGeneral_Tab |= 0x0001;
					SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
					SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
					SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,m_cGENERAL_CHECKSUM_text);
					SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,m_cGENERAL_CHECKSUM_text);

					if (m_pPropertySheet)
					{
						if (m_pGeneralStatPage)
						{
							m_pGeneralStatPage->SetDlgItemText(IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
							m_pGeneralStatPage->SetColor(IDC_GENERAL_CHECKSUM,COLOR_RED);
							m_pPropertySheet->SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
						}
						if (m_pGeneral4StatPage)
						{
							m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
							m_pGeneral4StatPage->SetColor(IDC_GENERAL4_CHECKSUM,COLOR_RED);
							m_pPropertySheet->m_iGeneral4_Tab |= 0x0001;
								
							if (m_pPropertySheet->m_iGeneral4_Tab == 0x0003)
								m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
							else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0002)
								m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
							else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0001)
								m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
							else if (m_pPropertySheet->m_iGeneral4_Tab == 0x0000)
								m_pPropertySheet->SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
						}
					}


					WriteToFile(TO_PFM, TYPE_COMP, NULL, "15360 CHECKSUM ERROR\r\n");
					char cBuff[1024];
					strcpy(cBuff,"15370 ");
					strcat(cBuff,Record);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

					if (m_bDoDump)
					{
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "CHECKSUM ERROR\r\n");
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
					}
					m_dBinaryAt = 0;
					m_dAt=0;
				}
			}	
			m_dBinaryAt++;
			//clean up data pointers // get them ready for another message
			//m_dBinaryAt = 0;
			//m_dAt = 0;
		}
		else if (
			(m_dAt>=1) &&
			((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) ||
			((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1)) 
		   )
		{
			//clean up from backward instrument
			if ((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) 
			{
				m_pAccumulator[m_dAt]=EOL2;
				m_pAccumulator[m_dAt-1]=EOL1;
			}

			//*test here if length is correct for particular type of message
			//*if it is ok then continue below 
			//*if it is < correct length then assume 
			//*	we got a \r\n or \r\n in binary and continue with the for loop
			//*if it is > correct length then assume
			//*	 we dropped a packet -- send to the handler function and let it
			//*  tell everyone we have an error on this message
			short message_type = UNKNOWN_REC;
			int iStart;
			char* cbuff2;// = (char*)malloc(m_dAt+2);
			iStart = 0;
			while (!isprint(m_pAccumulator[iStart]) && (iStart < m_dAt)) iStart++;

			cbuff2 = (char*)malloc(m_dAt-iStart+2);
			memmove(cbuff2,&m_pAccumulator[iStart],m_dAt-iStart+1);
			cbuff2[m_dAt-iStart+1] = NULL;

/*
			if (m_pAccumulator[0] == NULL)
			{
				cbuff2 = (char*)malloc(m_dAt+1);
				memmove(cbuff2,m_pAccumulator+1,m_dAt);
				cbuff2[m_dAt] = NULL;
			}
			else
			{
				cbuff2 = (char*)malloc(m_dAt+2);
				memmove(cbuff2,m_pAccumulator,m_dAt+1);
				cbuff2[m_dAt+1] = NULL; //in case we want to handle like string
			}
*/
			//*if binary convert to ascii for further processing
			//*cbuff2 = convert(cbuff2);
			//*			if binary type message then
			//*				malloc enough space for ascii of message type
			//*				convert cbuff2's data to ascii
			//*				delete the old cbuff2's memory
			//*				return pointer to new malloc
			//*			else
			//*				return old cbuff2's pointer
			//*
			//if (((CInstrument*)m_pParent)->m_bDoDump)
			//	WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff2);//dump file
			
			UpdateSummary(cbuff2);
//try{
			sscanf(cbuff2,"%2hx",&message_type);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 87",MB_OK);
//	AfxMessageBox(cbuff2,MB_OK);
//}

			//* add new message handlers for new binary messages which are now in ascii
			switch(message_type)
			{
			case STATUS_RECORD:				StatusRec(cbuff2);					break;	//12
			case OFFSET_RECORD:				OffsetRec(cbuff2);					break;	//13
			case ID_RECORD:					ID_Record(cbuff2);					break;	//14
			case ACQUIRE_RECORD:			AcquirRec(cbuff2);					break;	//15

			case END_DUMP_RECORD:			DumpEnd(cbuff2);					break;	//16
			case END_DUMP2_RECORD:			DumpEnd2(cbuff2);					break;	//61

			case DUMP_OK_RECORD:			DumpOk(cbuff2);						break;	//17
			case BATTERY_RECORD:			BatteryRec(cbuff2);					break;	//18
			case TRIP_NEU_STAT_RECORD:		TripNSRec(cbuff2);					break;	//19
			case DUAL_ION_STAT_RECORD:		DualIonRec(cbuff2);					break;	//1a
			case GEN_STATUS_RECORD:			GenStatRec(cbuff2);					break;	//1b
			case MONITOR_A_RECORD:			Mon_A_Rec(cbuff2);					break;	//1c
			case TRIP_NEU2_RECORD:			TripN2Rec(cbuff2);					break;	//1d
			case MONITOR_ID2_RECORD:		ID2_Record(cbuff2);					break;	//1e
			case ANALYZE_OK_RECORD:			AnalyzeOk(cbuff2);					break;	//1f
			case INFORMATION_RECORD:		InformRec(cbuff2);					break;	//30
			case MONITOR_B_RECORD:			Mon_B_Rec(cbuff2);					break;	//31
			case ANALYZE_OK3_RECORD:		AnalyzeOk3Record(cbuff2);			break;	//4d
			case MONITOR_ID3_RECORD:		ID3_Record(cbuff2);					break;	//4e
			case DUMP_OK3_RECORD:			DumpOk3_Record(cbuff2);				break;	//4f
			case GENERAL_CONFIG_RECORD:		GeneralConfigRecord(cbuff2);		break;	//41
			case INSTRUMENT_STATUS_RECORD:	InstrumentStatusRecord(cbuff2);		break;	//42
			case INSTRUMENT_INFO_RECORD:	InstrumentInfoRecord(cbuff2);		break;	//43
			case DUAL_CURRENTMODE_CONFIG_RECORD: DualCurrentModeConfigRecord(cbuff2); break;//44
			case TRIGGER_CONFIG_RECORD:		TriggerConfigRecord(cbuff2);		break;	//45
			case CHANNEL0_CONFIG_RECORD: 
			case CHANNEL1_CONFIG_RECORD: 
			case CHANNEL2_CONFIG_RECORD: 
			case CHANNEL3_CONFIG_RECORD: 
			case CHANNEL4_CONFIG_RECORD: 
			case CHANNEL5_CONFIG_RECORD: 
			case CHANNEL6_CONFIG_RECORD: 
				ChannelNConfigRecord(cbuff2,message_type);
				break; //46
			default:
				if ((temp_at = strstr(cbuff2,"TIME ="))!=NULL)
				{
					if (temp_at[11] == ' ')// || (strstr(cbuff2,"G1=")==NULL)
						Echo_Rec(cbuff2);
					else
					{
						if (strstr(cbuff2,"FILTERED POINTS=")!=NULL)
							Echo_2_Rec(cbuff2);
						else
							Echo_4_Rec(cbuff2);
					}
				}
				else if (strstr(cbuff2,"??")!=NULL)
					Echo_3_Rec(cbuff2);
				else if (strstr(cbuff2,"MG Setup")!=NULL)
					Echo_3_Rec(cbuff2);
				//modified this to catch a startup problem 
				//observed in the field with MiniGRANDs
				else if ((strlen(cbuff2)==0) ||
					     (strstr(cbuff2,"ECHO")!=NULL) ||
					     (strstr(cbuff2,"END")!=NULL) ||
						 (strstr(cbuff2,"USER")!=NULL) ||
						 (strstr(cbuff2,"YES")!=NULL) ||
						 (strstr(cbuff2,"90")!=NULL))
				{
					//do nothing we received a blank line or an echo
				}
				else if (strstr(cbuff2,"MG Monitor")!=NULL)
					Echo_5_Rec(cbuff2);				
				else if (strstr(cbuff2,"tpc=")!=NULL)
					Echo_5_Rec(cbuff2);
				else if (strstr(cbuff2,"G GAIN")!=NULL)
					Echo_6_Rec(cbuff2);
				//put these back in to fast restart a GRAND III
				else if ((strstr(cbuff2,"GRAND UNIT")!=NULL) ||
						 (strstr(cbuff2,"BEGIN VERSION")!=NULL) ||
						 (strstr(cbuff2,"YR.MO.DY")!=NULL) ||
						 (strstr(cbuff2,"MODE=")!=NULL) ||
						 (strstr(cbuff2,"RS232")!=NULL) ||
						 (strstr(cbuff2,"CHANGE/EXAMINE")!=NULL) ||
						 (strstr(cbuff2,"ERASE")!=NULL) ||
						 (strstr(cbuff2,"DUMP BATTERY")!=NULL) ||
						 (strstr(cbuff2,"NEUTRON CHANS")!=NULL) ||
						 (strstr(cbuff2,"GAMMA CHANS")!=NULL) ||
						 (strstr(cbuff2,"DO NOT FILTER")!=NULL) ||
						 (strstr(cbuff2,"HOURS BETWEEN")!=NULL) ||
						 (strstr(cbuff2,"MINUTES BETWEEN")!=NULL) ||
						 (strstr(cbuff2,"NEUTRON THRESHOLDS")!=NULL) ||
						 (strstr(cbuff2,"GAMMA THRESHOLDS")!=NULL) ||
						 (strstr(cbuff2,"FILTERING =")!=NULL) ||
						 (strstr(cbuff2,"NEUT FILTER LIMIT:")!=NULL) ||
						 (strstr(cbuff2,"GAMMA FILTER LIMIT")!=NULL) ||
						 (strstr(cbuff2,"GAMMA SIGMA")!=NULL) ||
						 (strstr(cbuff2,"EXIT MII IF")!=NULL) ||
						 (strstr(cbuff2,"ENTER MII IF")!=NULL) ||
						 (strstr(cbuff2,"WHEN NOT FILTERING")!=NULL) ||
						 (strstr(cbuff2,"LOCAL BACKGROUND")!=NULL) ||
						 (strstr(cbuff2,"TRIGGER CONDITION=")!=NULL) ||
						 (strstr(cbuff2,"SYNC TIME =")!=NULL) ||
						 (strstr(cbuff2,"PERFORM CHAN")!=NULL) ||
						 (strstr(cbuff2,"POSSIBLE TAMPER")!=NULL) ||
						 (strstr(cbuff2,"POINTS TO USE IN")!=NULL)) 
					Echo_2_Rec(cbuff2);
				else	//I took this out--if we don't know what we've got //put back in
						//then don't tell the program we do know.
//					Echo_2_Rec(cbuff2);
					if (m_bDoDump)										//per Bob 12 Jan 2007
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, cbuff2);	//per Bob 12 Jan 2007

			}							
			delete [] cbuff2;
			m_dAt = 0;
		}
		else
		{
			m_dAt++;
			if (m_dAt >= ACCUMULATOR_SIZE) m_dAt = 0;
		}
	}

	//free( (BYTE*)LParam);
	CoTaskMemFree((void *)LParam);
}

void CInstrument::Echo_6_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);

	PostMessage(IDC_RECEIVE_RESULT,ECHO_6_REC,CHECKSUM_GOOD);
}

void CInstrument::Echo_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	m_IsMiniGrand = 1;
	PostMessage(IDC_RECEIVE_RESULT,ECHO_RECORD,CHECKSUM_GOOD);
}

void CInstrument::Echo_5_Rec(char *Record)				
{																	
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_5_REC,CHECKSUM_GOOD);	
}																	

void CInstrument::Echo_4_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_4_REC,CHECKSUM_GOOD);
}

void CInstrument::Echo_3_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_3_REC,CHECKSUM_GOOD);
}

void CInstrument::Echo_2_Rec(char *Record)
{
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,ECHO_2_REC,CHECKSUM_GOOD);
}

void CInstrument::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

int CInstrument::AppendCheckSum(char *pBinaryAt)
{
    unsigned short sum;		/* sum of all bytes in checksum */
	sum = 0;
	for (unsigned int i=0; i < strlen(pBinaryAt); i++)
		sum = (unsigned short)(sum + pBinaryAt[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	char cTemp[4];
	sprintf(cTemp,"%02hx",sum);
	strcat(pBinaryAt,cTemp);
	return sum;
}


void CInstrument::AdjustTabsTo(enum MONVER eMonitorVers)
{
	if (m_pPropertySheet)
	{
		m_pPropertySheet->AdjustTabsTo(eMonitorVers);
		switch (eMonitorVers)
		{
		case POST4:
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_GRAND,"Go To\nMG Setup\nPage");
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_USERSTAT,"Go To\nTrigger &&\nCh. Params\nPage");
			break;
		case PRE4:
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_GRAND,"Go To\nGRAND Setup Page");
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_USERSTAT,"Go To\nMonitor Parameters Page");
			break;
		default:;
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_GRAND,"Go To\nGRAND Setup Page");
			m_pSummaryPage->SetDlgItemText(IDC_GOTO_USERSTAT,"Go To\nMonitor Parameters Page");
		};
/*
		//we keep the color info up here so tell each page where to find it
		m_pSummaryPage->StaticColors = SummaryStaticColors;
		m_pGeneralStatPage->StaticColors = GeneralStaticColors;
		m_pGeneral4StatPage->StaticColors = General4StaticColors;
		m_pDataStatus->StaticColors = DataStaticColors;
		m_pUserProgStatus->StaticColors = UserStaticColors;
		m_pUser4ProgStatus->StaticColors = User4StaticColors;
//		m_pSetupData->StaticColors = SetupStaticColors;
//		m_pSetup4Data->StaticColors = Setup4StaticColors;
*/
	}

	m_eMonitorVers = eMonitorVers;
}

//record 12
int CInstrument::BinaryStatusRecOld(BYTE *pBinaryAt)
{
	AdjustTabsTo(PRE4);

	bool bStale = true;											//DR00406

	struct SBinaryStatusRecOld* psBinaryRecord;

	psBinaryRecord = (SBinaryStatusRecOld*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->g_gain1),2);
	Reverse((BYTE*)&(psBinaryRecord->g_gain2),2);
	Reverse((BYTE*)&(psBinaryRecord->n_gain),4);
	Reverse((BYTE*)&(psBinaryRecord->fission_bias),4);
	Reverse((BYTE*)&(psBinaryRecord->ion_bias),4);
	Reverse((BYTE*)&(psBinaryRecord->battery),4);
	Reverse((BYTE*)&(psBinaryRecord->plus5),4);
	Reverse((BYTE*)&(psBinaryRecord->plus12),4);
	Reverse((BYTE*)&(psBinaryRecord->minus12),4);

	memcpy(&m_sBinaryStatusRecOld,psBinaryRecord,sizeof(m_sBinaryStatusRecOld));

	//DR00406 moved this up to here
	char Record[1024];
	sprintf(Record,"12%10u%02x%02x%07d%07d%5.1f%3d%3d%3d%3d%6.1f%6.1f%4.1f%4.1f%4.1f%5.1f",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		psBinaryRecord->acq_mode,
		psBinaryRecord->g_gain1,
		psBinaryRecord->g_gain2,
		psBinaryRecord->n_gain,
		psBinaryRecord->lld_a,
		psBinaryRecord->uld_a,
		psBinaryRecord->lld_b,
		psBinaryRecord->uld_b,
		psBinaryRecord->fission_bias,
		psBinaryRecord->ion_bias,
		psBinaryRecord->battery,
		psBinaryRecord->plus5,
		psBinaryRecord->plus12,
		psBinaryRecord->minus12
		);
	AppendCheckSum(Record);

	char temp[64];
	//get julian time and date
	CJulianTime Julian;
	Julian.Init(psBinaryRecord->julian);

	if (m_uiLastStatusRecTime < Julian.TotalSeconds())			//DR00406
	{															//DR00406
		m_uiLastStatusRecTime = Julian.TotalSeconds();			//DR00406

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		SetString(GENERALPAGE,IDC_GENERAL_TIME,temp);

		if (psBinaryRecord->status & ACPOWER)
		{
			strcpy(temp,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			m_iData_Tab |= 0x0001;
		}
		else 
		{
			strcpy(temp,OK);
		}

		//ac power changed since last status record
		if (psBinaryRecord->status & ACPOWERCH)
		{
			strcat(temp," with loss since last status record");
			
			if (!m_bACPowerLoss)
			{
				WriteToFileEx(0,TO_CEV | TO_PFM, TYPE_INST, &Julian,"15410 STATUS CHANGE - EXTERNAL POWER LOSS Since last status record (12)\r\n");
				m_bACPowerLoss = true;
			}
		}
		else
			m_bACPowerLoss = false;

		SetString(DATAPAGE,IDC_DATA_ACPOWER,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp);
		
		//bit 6 battery 
		if (psBinaryRecord->status & BATTERY) 
		{
		
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(1,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15420 STATUS CHANGE - BATTERY LOW  (12)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
				WriteToFileEx(1,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15430 STATUS CHANGE - BATTERY OK  (12)\r\n");
			m_bBatteryOk = true;
		}

		char temp2[32];
		bool bMatch;
		temp2[0] = NULL;
		sprintf(temp,"%02X",psBinaryRecord->acq_mode);
		GetPrivateProfileString(m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_MODE,COLOR_YELLOW);
			WriteToFileEx(2,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15440 SETUP FAILURE - ACQ_MODE (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_ACQ_MODE12,"SET_ACQ_MODE12",temp,bMatch);

		temp[0] = NULL;
		if (psBinaryRecord->acq_mode & 0x07) 
		{
			strcat(temp,"Neut:");
			if (psBinaryRecord->acq_mode & 0x01)
				strcat(temp,"a");
			if (psBinaryRecord->acq_mode & 0x02)
				strcat(temp,"b");
			if (psBinaryRecord->acq_mode & 0x04)
				strcat(temp,"c");
			strcat(temp," ");
		}
		if (psBinaryRecord->acq_mode & 0x18)
		{
			strcat(temp,"Gam:");
			if (psBinaryRecord->acq_mode & 0x08)
				strcat(temp,"1");
			if (psBinaryRecord->acq_mode & 0x10)
				strcat(temp,"2");
		}
		SetString(GENERALPAGE,IDC_GENERAL_MODE,temp);

		//gain 1
		sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain1*2)));
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_GAMMA1G,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15450 SETUP FAILURE - GAM_GAIN1 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//gain 2
		sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain2*2)));
	////	sprintf(temp,"%d", psBinaryRecord->g_gain2);
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN2","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_GAMMA2G,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15460 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//neutron gain
	//	generalpage->GetDlgItem(IDC_GENERAL_ANALOG)->ShowWindow(SW_SHOW);
		sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
		SetString(GENERALPAGE,IDC_GENERAL_ANALOG,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_NEU_GAIN","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_ANALOG,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15470 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//lld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->lld_a);
		SetString(GENERALPAGE,IDC_GENERAL_LLDA,temp);
		GetPrivateProfileString(m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_LLDA,COLOR_YELLOW);
			WriteToFileEx(3,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15480 SETUP FAILURE - LLD A (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_LLD_A,"SET_LLD_A",temp,bMatch);
		
		//uld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->uld_a);
		SetString(GENERALPAGE,IDC_GENERAL_WINDOWA,temp);
		GetPrivateProfileString(m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_WINDOWA,COLOR_YELLOW);
			WriteToFileEx(4,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15490 SETUP FAILURE - ULD A (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}

		//lld_b
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->lld_b);
		SetString(GENERALPAGE,IDC_GENERAL_LLDB,temp);
		GetPrivateProfileString(m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_LLDB,COLOR_YELLOW);
			WriteToFileEx(5,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15500 SETUP FAILURE - LLD B (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_LLD_B,"SET_LLD_B",temp,bMatch);
		
		//uld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->uld_b);
		SetString(GENERALPAGE,IDC_GENERAL_WINDOWB,temp);
		GetPrivateProfileString(m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_WINDOWB,COLOR_YELLOW);
			WriteToFileEx(6,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15510 SETUP FAILURE - ULD B (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_ULD_B,"SET_ULD_B",temp,bMatch);
		
		float Current;
		Current = psBinaryRecord->fission_bias;
		if (m_bFBFirst)
		{
			m_fFBHigh = m_fFBLow = Current;
			m_bFBFirst = false;
		}
		if (Current < m_fFBLow) m_fFBLow = Current;
		if (Current > m_fFBHigh) m_fFBHigh = Current;
	////	sprintf(temp,"%6.1f / %6.1f / %6.1f",m_fDIRLow,Current,m_fDIRHigh);
	////	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHV,temp);
		sprintf(temp,"%6.1f",m_fFBLow);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fFBHigh);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp);

		//ion chamber bias
		Current = psBinaryRecord->ion_bias;
		if (m_dDIRFirst)
		{
			m_fDIRHigh = m_fDIRLow = Current;
			m_dDIRFirst = false;
		}
		if (Current < m_fDIRLow) m_fDIRLow = Current;
		if (Current > m_fDIRHigh) m_fDIRHigh = Current;
	////	sprintf(temp,"%6.1f / %6.1f / %6.1f",m_fDIRLow,Current,m_fDIRHigh);
	////	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHV,temp);
		sprintf(temp,"%6.1f",m_fDIRLow);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fDIRHigh);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp);

		//battery voltage
		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;
		m_fCurBV = psBinaryRecord->battery;
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV) 
		{
			m_fLowBV = m_fCurBV;
			CurrentLow = true;
		}
		if (m_fCurBV > m_fHighBV)
		{
			m_fHighBV= m_fCurBV;
			CurrentHigh= true;
		}
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{

			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryTolOk)
			{
				WriteToFileEx(7,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15520 STATUS CHANGE - BATTERY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryTolOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bBatteryTolOk)
				WriteToFileEx(7,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15530 STATUS CHANGE - BATTERY IN TOLERANCE (12)\r\n");
			m_bBatteryTolOk = true;
	//		m_iGeneral_Tab &= ~(0x0010);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fCurBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);

		//+5 volts
		//	strncpy(temp,&Record[45],4);
		//	temp[4] = NULL;
		//m_fCurP5=(float)atof(temp);
		m_fCurP5 = psBinaryRecord->plus5;
		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			CurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			CurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			CurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			CurrentRed = true;
				
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
			if (m_bP5VoltOk)
			{
				WriteToFileEx(8,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15540 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bP5VoltOk = false;

			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP5VoltOk)
				WriteToFileEx(8,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15550 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bP5VoltOk = true;
	//		m_iGeneral_Tab &= ~(0x0020);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP5 <= m_fSup_P05L) || (m_fLowP5 >= m_fSup_P05H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP5 <= m_fSup_P05L) || (m_fHighP5 >= m_fSup_P05H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp);

		//+15 volts
	//	//strncpy(temp,&Record[49],4);
	//	//temp[4] = NULL;
	//	//m_fCurP15=(float)atof(temp);
		m_fCurP15 = psBinaryRecord->plus12;
		if (m_bFirstP15)
		{
			m_bFirstP15 = false;
			m_fLowP15 = m_fHighP15 = m_fCurP15;
		}
		if (m_fCurP15 < m_fLowP15)
		{
			CurrentLow = true;
			m_fLowP15 = m_fCurP15;
		}
		else
			CurrentLow = false;
		if (m_fCurP15 > m_fHighP15)
		{
			CurrentHigh = true;
			m_fHighP15= m_fCurP15;
		}
		else
			CurrentHigh = false;

		if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
		{
			CurrentRed = true;			
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,COLOR_RED);
			if (m_bP15VoltOk)
			{
				WriteToFileEx(9,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15560 STATUS CHANGE - PLUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bP15VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP15VoltOk)
				WriteToFileEx(9,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15570 STATUS CHANGE - PLUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bP15VoltOk = true;
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP15 <= m_fSup_P15L) || (m_fLowP15 >= m_fSup_P15H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP15 <= m_fSup_P15L) || (m_fHighP15 >= m_fSup_P15H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,COLOR_RED);
		}
		sprintf(temp,"%5.1f",m_fLowP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,temp);

		//-15 volt
		//strncpy(temp,&Record[53],5);
		//temp[5] = NULL;
		//m_fCurM15=(float)atof(temp);
		m_fCurM15 = psBinaryRecord->minus12;
		if (m_bFirstM15)
		{
			m_bFirstM15 = false;
			m_fLowM15 = m_fHighM15 = m_fCurM15;
		}
		if (m_fCurM15 < m_fLowM15)
		{
			CurrentLow = true;
			m_fLowM15 = m_fCurM15;
		}
		else
			CurrentLow = false;
		if (m_fCurM15 > m_fHighM15)
		{
			CurrentHigh = true;
			m_fHighM15= m_fCurM15;
		}
		else
			CurrentHigh = false;
		if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
		{
			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,COLOR_RED);
			if (m_bM15VoltOk)
			{
				WriteToFileEx(10,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15580 STATUS CHANGE - MINUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bM15VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bM15VoltOk)
				WriteToFileEx(10,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15590 STATUS CHANGE - MINUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bM15VoltOk = true;
	//		m_iGeneral_Tab &= ~(0x0080);
		}

		if (CurrentRed && CurrentLow && (m_fLowM15 <= m_fSup_M15L))// || (m_fLowM15 >= m_fSup_M15H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighM15 >= m_fSup_M15H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,temp);

	////	strncpy(temp,&Record[58],4);
	////	temp[4] = NULL;
	////	generalpage->SetDlgItemText(IDC_GENERAL_ROM,temp);

		//set to red, yellow, or green if needed
	//	if (m_iGeneral_Tab == 0x0003)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
	//	else if (m_iGeneral_Tab == 0x0001)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	//	else if (m_iGeneral_Tab == 0x0002)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
	//	else
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

	////	SetTabColor(IDC_GENERAL_TAB,m_iGeneral_Tab?COLOR_RED:COLOR_GREEN);
	//	if (m_iData_Tab)
	//		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	////	else
	////		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);


		UpdateSummary(Record);
		strcat(Record,"\r\n");

//	if (m_pPropertySheet)
//		m_pPropertySheet->StatusRecOld(Record);

		bStale = false;											//DR00406
	}															//DR00406
	else														//DR00406
		strcat(Record,"\r\n");									//DR00406

	PostToWatch(Record);

	char cBuff[1024];
	strcpy(cBuff,"15600 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);		//DR00406

	//send to the dump file
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,STATUS_RECORD,CHECKSUM_GOOD);
	return sizeof(SBinaryStatusRecOld);
}

//record 12
int CInstrument::BinaryStatusRec(BYTE *pBinaryAt)
{
	AdjustTabsTo(PRE4);

	bool bStale = true;											//DR00406

	struct SBinaryStatusRec* psBinaryRecord;

	psBinaryRecord = (SBinaryStatusRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->g_gain1),4);
	Reverse((BYTE*)&(psBinaryRecord->g_gain2),4);
	Reverse((BYTE*)&(psBinaryRecord->n_gain),4);
	Reverse((BYTE*)&(psBinaryRecord->fission_bias),4);
	Reverse((BYTE*)&(psBinaryRecord->ion_bias),4);
	Reverse((BYTE*)&(psBinaryRecord->battery),4);
	Reverse((BYTE*)&(psBinaryRecord->plus5),4);
	Reverse((BYTE*)&(psBinaryRecord->plus12),4);
	Reverse((BYTE*)&(psBinaryRecord->minus12),4);

	memcpy(&m_sBinaryStatusRec,psBinaryRecord,sizeof(m_sBinaryStatusRec));

	//DR00406 moved this up to here
	char Record[1024];
	sprintf(Record,"12%10u%02x%02x%07d%07d%5.1f%3d%3d%3d%3d%6.1f%6.1f%4.1f%4.1f%4.1f%5.1f",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		psBinaryRecord->acq_mode,
		psBinaryRecord->g_gain1,
		psBinaryRecord->g_gain2,
		psBinaryRecord->n_gain,
		psBinaryRecord->lld_a,
		psBinaryRecord->uld_a,
		psBinaryRecord->lld_b,
		psBinaryRecord->uld_b,
		psBinaryRecord->fission_bias,
		psBinaryRecord->ion_bias,
		psBinaryRecord->battery,
		psBinaryRecord->plus5,
		psBinaryRecord->plus12,
		psBinaryRecord->minus12
		);
	AppendCheckSum(Record);

	char temp[64];
	//get julian time and date
	//strncpy(temp,&Record[2],10);
	//temp[10]=NULL;
	CJulianTime Julian;
	Julian.Init(psBinaryRecord->julian);

	if (m_uiLastStatusRecTime < Julian.TotalSeconds())			//DR00406
	{															//DR00406
		m_uiLastStatusRecTime = Julian.TotalSeconds();			//DR00406

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		SetString(GENERALPAGE,IDC_GENERAL_TIME,temp);
		
		if (psBinaryRecord->status & ACPOWER)
		{
	////	m_pParent->m_pButton->SetOthr(true,true);
	////		m_pParent->FlagOthr(1,true);			
			strcpy(temp,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
	////		if (m_bACPowerOk)
	////			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"15390 STATUS CHANGE - AC POWER Off (12)\r\n");//commented out
	////		m_bACPowerOk = false;
			m_iData_Tab |= 0x0001;
		}
		else 
		{
			strcpy(temp,OK);
	////		datapage->SetColor(IDC_DATA_ACPOWER,COLOR_BLACK);
	////		summarypage->SetColor(IDC_SUMMARY_ACPOWER,COLOR_BLACK);
	////		if (!m_bACPowerOk)
	////			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"15400 STATUS CHANGE - AC POWER On (12)\r\n");//commented out
	////		m_bACPowerOk = true;

	////		m_iData_Tab &= ~(0x0001);
		}

		//ac power changed since last status record
		if (psBinaryRecord->status & ACPOWERCH)
		{
			strcat(temp," with loss since last status record");
			
			if (!m_bACPowerLoss)
			{
				WriteToFileEx(11,TO_CEV | TO_PFM, TYPE_INST, &Julian,"15410 STATUS CHANGE - EXTERNAL POWER LOSS Since last status record (12)\r\n");
				m_bACPowerLoss = true;
			}
		}
		else
			m_bACPowerLoss = false;

		SetString(DATAPAGE,IDC_DATA_ACPOWER,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp);
		
		//bit 6 battery 
		if (psBinaryRecord->status & BATTERY) 
		{			
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(12,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15420 STATUS CHANGE - BATTERY LOW  (12)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
				WriteToFileEx(12,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15430 STATUS CHANGE - BATTERY OK  (12)\r\n");
			m_bBatteryOk = true;
		}

		char temp2[32];
		bool bMatch;
		temp2[0] = NULL;
		sprintf(temp,"%02X",psBinaryRecord->acq_mode);
		GetPrivateProfileString(m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_MODE,COLOR_YELLOW);
			WriteToFileEx(13,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15440 SETUP FAILURE - ACQ_MODE (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_ACQ_MODE12,"SET_ACQ_MODE12",temp,bMatch);

		temp[0] = NULL;
		if (psBinaryRecord->acq_mode & 0x07) 
		{
			strcat(temp,"Neut:");
			if (psBinaryRecord->acq_mode & 0x01)
				strcat(temp,"a");
			if (psBinaryRecord->acq_mode & 0x02)
				strcat(temp,"b");
			if (psBinaryRecord->acq_mode & 0x04)
				strcat(temp,"c");
			strcat(temp," ");
		}
		if (psBinaryRecord->acq_mode & 0x18)
		{
			strcat(temp,"Gam:");
			if (psBinaryRecord->acq_mode & 0x08)
				strcat(temp,"1");
			if (psBinaryRecord->acq_mode & 0x10)
				strcat(temp,"2");
		}
		SetString(GENERALPAGE,IDC_GENERAL_MODE,temp);

		//gain 1
	////	sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain1*2)));
		sprintf(temp,"%d",psBinaryRecord->g_gain1);
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_GAMMA1G,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15450 SETUP FAILURE - GAM_GAIN1 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//gain 2
	////	sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain2*2)));
		sprintf(temp,"%d", psBinaryRecord->g_gain2);
		SetString(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN2","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_GAMMA2G,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15460 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//neutron gain
	//	generalpage->GetDlgItem(IDC_GENERAL_ANALOG)->ShowWindow(SW_SHOW);
		sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
		SetString(GENERALPAGE,IDC_GENERAL_ANALOG,temp);
	////don't put this back in
	////	temp2[0] = NULL;
	////	sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
	////	GetPrivateProfileString(m_pParent->m_pName,"SET_NEU_GAIN","",temp2,sizeof(temp2),m_szIniFile);
	////	if (strcmp(temp,temp2)!=0)
	////	{
	////		generalpage->SetColor(IDC_GENERAL_ANALOG,COLOR_YELLOW);
	////		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15470 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////		m_iGeneral_Tab |= 0x8000;
	////	}

		//lld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->lld_a);
		SetString(GENERALPAGE,IDC_GENERAL_LLDA,temp);
		GetPrivateProfileString(m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_LLDA,COLOR_YELLOW);
			WriteToFileEx(14,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15480 SETUP FAILURE - LLD A (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_LLD_A,"SET_LLD_A",temp,bMatch);
		
		//uld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->uld_a);
		SetString(GENERALPAGE,IDC_GENERAL_WINDOWA,temp);
		GetPrivateProfileString(m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_WINDOWA,COLOR_YELLOW);
			WriteToFileEx(15,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15490 SETUP FAILURE - ULD A (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
	//	modparpage->CurrentSetParam(SET_ULD_A,"SET_ULD_A",temp,bMatch);
	//	setdatapage->CurrentSetParam(SET_ULD_A,"SET_ULD_A",temp,bMatch);

		//lld_b
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->lld_b);
		SetString(GENERALPAGE,IDC_GENERAL_LLDB,temp);
		GetPrivateProfileString(m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_LLDB,COLOR_YELLOW);
			WriteToFileEx(16,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15500 SETUP FAILURE - LLD B (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_LLD_B,"SET_LLD_B",temp,bMatch);
		
		//uld_a
		temp2[0] = NULL;
		sprintf(temp,"%3d",psBinaryRecord->uld_b);
		SetString(GENERALPAGE,IDC_GENERAL_WINDOWB,temp);
		GetPrivateProfileString(m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(GENERALPAGE,IDC_GENERAL_WINDOWB,COLOR_YELLOW);
			WriteToFileEx(17,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15510 SETUP FAILURE - ULD B (12)\r\n");
			m_iGeneral_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_ULD_B,"SET_ULD_B",temp,bMatch);
		
		float Current;
		Current = psBinaryRecord->fission_bias;
		if (m_bFBFirst)
		{
			m_fFBHigh = m_fFBLow = Current;
			m_bFBFirst = false;
		}
		if (Current < m_fFBLow) m_fFBLow = Current;
		if (Current > m_fFBHigh) m_fFBHigh = Current;
	////	sprintf(temp,"%6.1f / %6.1f / %6.1f",m_fDIRLow,Current,m_fDIRHigh);
	////	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHV,temp);
		sprintf(temp,"%6.1f",m_fFBLow);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fFBHigh);
		SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp);

		//ion chamber bias
		Current = psBinaryRecord->ion_bias;
		if (m_dDIRFirst)
		{
			m_fDIRHigh = m_fDIRLow = Current;
			m_dDIRFirst = false;
		}
		if (Current < m_fDIRLow) m_fDIRLow = Current;
		if (Current > m_fDIRHigh) m_fDIRHigh = Current;
	////	sprintf(temp,"%6.1f / %6.1f / %6.1f",m_fDIRLow,Current,m_fDIRHigh);
	////	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHV,temp);
		sprintf(temp,"%6.1f",m_fDIRLow);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fDIRHigh);
		SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp);

		//battery voltage
		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;
		m_fCurBV = psBinaryRecord->battery;
		if (m_bFirstBV)
		{
			m_bFirstBV = false;
			m_fLowBV = m_fHighBV = m_fCurBV;
		}
		if (m_fCurBV < m_fLowBV) 
		{
			m_fLowBV = m_fCurBV;
			CurrentLow = true;
		}
		if (m_fCurBV > m_fHighBV)
		{
			m_fHighBV= m_fCurBV;
			CurrentHigh= true;
		}
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		{
			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
			if (m_bBatteryTolOk)
			{
				WriteToFileEx(18,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15520 STATUS CHANGE - BATTERY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryTolOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bBatteryTolOk)
				WriteToFileEx(18,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15530 STATUS CHANGE - BATTERY IN TOLERANCE (12)\r\n");
			m_bBatteryTolOk = true;
	//		m_iGeneral_Tab &= ~(0x0010);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fCurBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);

		//+5 volts
		//	strncpy(temp,&Record[45],4);
		//	temp[4] = NULL;
		//m_fCurP5=(float)atof(temp);
		m_fCurP5 = psBinaryRecord->plus5;
		if (m_bFirstP5)
		{
			m_bFirstP5 = false;
			m_fLowP5 = m_fHighP5 = m_fCurP5;
		}
		if (m_fCurP5 < m_fLowP5)
		{
			CurrentLow = true;
			m_fLowP5 = m_fCurP5;
		}
		else
			CurrentLow = false;
		if (m_fCurP5 > m_fHighP5)
		{
			CurrentHigh = true;
			m_fHighP5= m_fCurP5;
		}
		else
			CurrentHigh = false;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		{
			CurrentRed = true;			
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
			if (m_bP5VoltOk)
			{
				WriteToFileEx(19,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15540 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bP5VoltOk = false;

			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP5VoltOk)
				WriteToFileEx(19,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15550 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bP5VoltOk = true;
	//		m_iGeneral_Tab &= ~(0x0020);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP5 <= m_fSup_P05L) || (m_fLowP5 >= m_fSup_P05H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP5 <= m_fSup_P05L) || (m_fHighP5 >= m_fSup_P05H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP5);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp);

		//+15 volts
		//strncpy(temp,&Record[49],4);
		//temp[4] = NULL;
		//m_fCurP15=(float)atof(temp);
		m_fCurP15 = psBinaryRecord->plus12;
		if (m_bFirstP15)
		{
			m_bFirstP15 = false;
			m_fLowP15 = m_fHighP15 = m_fCurP15;
		}
		if (m_fCurP15 < m_fLowP15)
		{
			CurrentLow = true;
			m_fLowP15 = m_fCurP15;
		}
		else
			CurrentLow = false;
		if (m_fCurP15 > m_fHighP15)
		{
			CurrentHigh = true;
			m_fHighP15= m_fCurP15;
		}
		else
			CurrentHigh = false;

		if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
		{
			CurrentRed = true;			
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,COLOR_RED);
			if (m_bP15VoltOk)
			{
				WriteToFileEx(20,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15560 STATUS CHANGE - PLUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bP15VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
			m_iGeneral4_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bP15VoltOk)
				WriteToFileEx(20,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15570 STATUS CHANGE - PLUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bP15VoltOk = true;
	//		m_iGeneral_Tab &= ~(0x0040);
	//		m_iGeneral4_Tab &= ~(0x0040);
		}

		if (
			CurrentRed && CurrentLow &&
			((m_fLowP15 <= m_fSup_P15L) || (m_fLowP15 >= m_fSup_P15H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,COLOR_RED);
		}

		if (
			CurrentRed && CurrentHigh &&
			((m_fHighP15 <= m_fSup_P15L) || (m_fHighP15 >= m_fSup_P15H))
		   )
		{
			SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,COLOR_RED);
		}
		sprintf(temp,"%5.1f",m_fLowP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighP15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,temp);

		//-15 volt
		//strncpy(temp,&Record[53],5);
		//temp[5] = NULL;
		//m_fCurM15=(float)atof(temp);
		m_fCurM15 = psBinaryRecord->minus12;
		if (m_bFirstM15)
		{
			m_bFirstM15 = false;
			m_fLowM15 = m_fHighM15 = m_fCurM15;
		}
		if (m_fCurM15 < m_fLowM15)
		{
			CurrentLow = true;
			m_fLowM15 = m_fCurM15;
		}
		else
			CurrentLow = false;
		if (m_fCurM15 > m_fHighM15)
		{
			CurrentHigh = true;
			m_fHighM15= m_fCurM15;
		}
		else
			CurrentHigh = false;
		if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
		{
			CurrentRed = true;
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,COLOR_RED);
			if (m_bM15VoltOk)
			{
				WriteToFileEx(21,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15580 STATUS CHANGE - MINUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
				FlagOthr(1,true);
			}
			m_bM15VoltOk = false;
			m_iGeneral_Tab |= 0x0001;
		}
		else
		{
			CurrentRed = false;
			if (!m_bM15VoltOk)
				WriteToFileEx(21,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15590 STATUS CHANGE - MINUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
			m_bM15VoltOk = true;
	//		m_iGeneral_Tab &= ~(0x0080);
		}

		if (CurrentRed && CurrentLow && (m_fLowM15 <= m_fSup_M15L))// || (m_fLowM15 >= m_fSup_M15H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,COLOR_RED);
		}

		if (CurrentRed && CurrentHigh && (m_fHighM15 >= m_fSup_M15H))
		{
			SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,COLOR_RED);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,COLOR_RED);
		}

		sprintf(temp,"%5.1f",m_fLowM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighM15);
		SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp);
		SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,temp);


	////	strncpy(temp,&Record[58],4);
	////	temp[4] = NULL;
	////	generalpage->SetDlgItemText(IDC_GENERAL_ROM,temp);

		//set to red, yellow, or green if needed
	//	if (m_iGeneral_Tab)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	//	if ((m_iGeneral_Tab == 0x003)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
	//	else if (m_iGeneral_Tab == 0x0001)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	//	else if (m_iGeneral_Tab == 0x0002)
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
	//	else
	//		SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);
	//
	//	if ((m_iData_Tab == 0x003)
	//		SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
	//	else if (m_iData_Tab == 0x0001)
	//		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	//	else if (m_iData_Tab == 0x0002)
	//		SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
	//	else
	//		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

	////	SetTabColor(IDC_GENERAL_TAB,m_iGeneral_Tab?COLOR_RED:COLOR_GREEN);
	//	if (m_iData_Tab)
	//		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	////	else
	////		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

		UpdateSummary(Record);
		strcat(Record,"\r\n");

		if (m_pPropertySheet)
			m_pPropertySheet->StatusRec(Record);

		bStale = false;											//DR00406
	}															//DR00406
	else														//DR00406
		strcat(Record,"\r\n");									//DR00406

	PostToWatch(Record);

	char cBuff[1024];
	strcpy(cBuff,"15600 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);		//DR00406

	//send to the dump file
	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,STATUS_RECORD,CHECKSUM_GOOD);

	return sizeof(SBinaryStatusRec);
}

//record 12
void CInstrument::StatusRec(char *Record)
{
	
/*
121662050644 0 041943044194304-29253255  0255  2  12.1  -0.528.05.2215.1-14.6e
xxttttttttttxxttxxxxxxxtttttttxxxxxtttxxxtttxxxttttttxxxxxxttttxxxxttttxxxxxTT
2   10      2 2 7      7      5    3  3  3  3  6     6     4   4   4   5    2
*/

//	if (Record[30] == '-')
//	{
//		char szTemp[82];
//		for (int i = 0; i < 30; i++)
//			szTemp[i] = Record[i];
//		for (i = 31; i < 77; i++)
//			szTemp[i-1] = Record[i];
//		szTemp[76] = NULL;
//		AppendCheckSum(szTemp);
//		strcpy(Record,szTemp);
//		strcat(Record,"\r\n");
//	}
	bool bStale = true;
	int CheckSum;
	if (LengthOk(Record,STATUS_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		char temp[64];
		//get julian time and date

		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		if (m_uiLastStatusRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastStatusRecTime = Julian.TotalSeconds();			//DR00406

			m_cStatusRec = Record;									//DR00406
			
			SetString(GENERALPAGE,IDC_GENERAL_TIME,temp);
		
			//status
			strncpy(temp,&Record[12],2);
			temp[2] = NULL;
			short Status;
//try{
			sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 88",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

			if (Status & ACPOWER)
			{
	////		m_pParent->m_pButton->SetOthr(true,true);
	////			m_pParent->FlagOthr(1,true);			
				strcpy(temp,FAIL);
				SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
	////			if (m_bACPowerOk)
	////				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"15610 STATUS CHANGE - AC POWER Off (12)\r\n");//commented out
	////			m_bACPowerOk = false;

				m_iData_Tab |= 0x0001;
			}
			else 
			{
				strcpy(temp,OK);
	////			datapage->SetColor(IDC_DATA_ACPOWER,COLOR_BLACK);
	////			summarypage->SetColor(IDC_SUMMARY_ACPOWER,COLOR_BLACK);
	////			if (!m_bACPowerOk)
	////				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"15620 STATUS CHANGE - AC POWER On (12)\r\n");//commented out
	////			m_bACPowerOk = true;
	////			m_iData_Tab &= ~(0x0001);
			}

			//ac power changed since last status record
			if (Status & ACPOWERCH)
			{
				strcat(temp," with loss since last status record");
				
				if (!m_bACPowerLoss)
				{
					WriteToFileEx(22,TO_CEV | TO_PFM, TYPE_INST, &Julian,"15630 STATUS CHANGE - EXTERNAL POWER LOSS Since last status record (12)\r\n");
					m_bACPowerLoss = true;
				}
			}
			else
				m_bACPowerLoss = false;

			SetString(DATAPAGE,IDC_DATA_ACPOWER,temp);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp);
		
			//bit 6 battery 
			if (Status & BATTERY) 
			{			
				SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
				SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
				SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
				if (m_bBatteryOk)
				{
					WriteToFileEx(23,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15640 STATUS CHANGE - BATTERY LOW  (12)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryOk = false;
				m_iData_Tab |= 0x0001;
			}
			else
			{
				SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
	////			datapage->SetColor(IDC_DATA_BATTERYOK,COLOR_BLACK);
				SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
	////			summarypage->SetColor(IDC_SUMMARY_BATTERYOK,COLOR_BLACK);
				if (!m_bBatteryOk)
					WriteToFileEx(23,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15650 STATUS CHANGE - BATTERY OK  (12)\r\n");
				m_bBatteryOk = true;
	//			m_iData_Tab &= ~(0x0002);
			}

			strncpy(temp,&Record[14],2);
			temp[2] = NULL;
			char temp2[32];
			temp2[0] = NULL;
			bool bMatch;
			GetPrivateProfileString(m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2)!=0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_MODE,COLOR_YELLOW);
				WriteToFileEx(24,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15660 SETUP FAILURE - ACQ_MODE (12)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ACQ_MODE12,"SET_ACQ_MODE12",temp,bMatch);

			short Mode;
//try{
			sscanf(temp,"%2hx",&Mode);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 89",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

			temp[0] = NULL;
			if (Mode & 0x07) 
			{
				strcat(temp,"Neut:");
				if (Mode & 0x01)
					strcat(temp,"a");
				if (Mode & 0x02)
					strcat(temp,"b");
				if (Mode & 0x04)
					strcat(temp,"c");
				strcat(temp," ");
			}
			if (Mode & 0x18)
			{
				strcat(temp,"Gam:");
				if (Mode & 0x08)
					strcat(temp,"1");
				if (Mode & 0x10)
					strcat(temp,"2");
			}
			SetString(GENERALPAGE,IDC_GENERAL_MODE,temp);

			//gain 1
			strncpy(temp,&Record[16],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp);
	//don't put this back in
	////		GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
	////		if (strcmp(temp,temp2)!=0)
	////		{
	////			generalpage->SetColor(IDC_GENERAL_GAMMA1G,COLOR_YELLOW);
	////			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15670 SETUP FAILURE - GAM_GAIN1 (12)\r\n");
	////			m_iGeneral_Tab |= 0x8000;
	////		}

			//gain 2
			strncpy(temp,&Record[23],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp);
	////don't put this back in
	////		GetPrivateProfileString(m_pParent->m_pName,"SET_GAM_GAIN2","",temp2,sizeof(temp2),m_szIniFile);
	////		if (strcmp(temp,temp2)!=0)
	////		{
	////			generalpage->SetColor(IDC_GENERAL_GAMMA2G,COLOR_YELLOW);
	////			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15680 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////			m_iGeneral_Tab |= 0x8000;
	////		}

			//neutron gain
	//		SetString(GENERALPAGE,IDC_GENERAL_ANALOG		)->ShowWindow(SW_SHOW);
			strncpy(temp,&Record[30],5);
			temp[5] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_ANALOG,temp);
	////don't put this back in
	////		GetPrivateProfileString(m_pParent->m_pName,"SET_NEU_GAIN","",temp2,sizeof(temp2),m_szIniFile);
	////		if (strcmp(temp,temp2)!=0)
	////		{
	////			generalpage->SetColor(IDC_GENERAL_ANALOG,COLOR_YELLOW);
	////			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "15690 SETUP FAILURE - GAM_GAIN2 (12)\r\n");
	////			m_iGeneral_Tab |= 0x8000;
	////		}

			//lld_a
			strncpy(temp,&Record[35],3);
			temp[3] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_LLDA,temp);
			GetPrivateProfileString(m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2)!=0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_LLDA,COLOR_YELLOW);
				WriteToFileEx(25,TO_CEV|TO_PFM, TYPE_INST, &Julian, "15700 SETUP FAILURE - LLD_A (12)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_LLD_A,"SET_LLD_A",temp,bMatch);

			//uld_a
			strncpy(temp,&Record[38],3);
			temp[3] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_WINDOWA,temp);
			GetPrivateProfileString(m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2)!=0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_WINDOWA,COLOR_YELLOW);
				WriteToFileEx(26,TO_CEV|TO_PFM, TYPE_INST, &Julian, "15710 SETUP FAILURE - ULD_A (12)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ULD_A,"SET_ULD_A",temp,bMatch);

			//lld_b
			strncpy(temp,&Record[41],3);
			temp[3] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_LLDB,temp);
			GetPrivateProfileString(m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2)!=0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_LLDB,COLOR_YELLOW);
				WriteToFileEx(27,TO_CEV|TO_PFM, TYPE_INST, &Julian, "15720 SETUP FAILURE - LLD_B (12)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_LLD_B,"SET_LLD_B",temp,bMatch);

			//uld_b
			strncpy(temp,&Record[44],3);
			temp[3] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_WINDOWB,temp);
			GetPrivateProfileString(m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2)!=0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_WINDOWB,COLOR_YELLOW);
				WriteToFileEx(28,TO_CEV|TO_PFM, TYPE_INST, &Julian, "15730 SETUP FAILURE - ULD_B (12)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_ULD_B,"SET_ULD_B",temp,bMatch);

			//fission bias
			float Current;
			strncpy(temp,&Record[47],6);
			temp[6] = NULL;
			Current = (float)atof(temp);
			if (m_bFBFirst)
			{
				m_fFBHigh = m_fFBLow = Current;
				m_bFBFirst = false;
			}
			if (Current < m_fFBLow) m_fFBLow = Current;
			if (Current > m_fFBHigh) m_fFBHigh = Current;
			sprintf(temp,"%6.1f",m_fFBLow);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp);
			sprintf(temp,"%6.1f",Current);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp);
			sprintf(temp,"%6.1f",m_fFBHigh);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp);

			//ion chamber bias
			strncpy(temp,&Record[53],6);
			temp[6] = NULL;
			Current = (float)atof(temp);
			if (m_dDIRFirst)
			{
				m_fDIRHigh = m_fDIRLow = Current;
				m_dDIRFirst = false;
			}
			if (Current < m_fDIRLow) m_fDIRLow = Current;
			if (Current > m_fDIRHigh) m_fDIRHigh = Current;
			sprintf(temp,"%6.1f",m_fDIRLow);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp);
			sprintf(temp,"%6.1f",Current);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp);
			sprintf(temp,"%6.1f",m_fDIRHigh);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp);

			//battery voltage
			strncpy(temp,&Record[59],4);
			temp[4] = NULL;
			bool CurrentRed = false;
			bool CurrentLow = false;
			bool CurrentHigh= false;
			m_fCurBV = (float)atof(temp);
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV < m_fLowBV) 
			{
				m_fLowBV = m_fCurBV;
				CurrentLow = true;
			}
			if (m_fCurBV > m_fHighBV)
			{
				m_fHighBV= m_fCurBV;
				CurrentHigh= true;
			}
			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				CurrentRed = true;
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryTolOk)
				{
					WriteToFileEx(29,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15740 STATUS CHANGE - BATTERY OUT OF TOLERANCE (12)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryTolOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bBatteryTolOk)
					WriteToFileEx(29,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15750 STATUS CHANGE - BATTERY IN TOLERANCE (12)\r\n");
				m_bBatteryTolOk = true;
	//			m_iGeneral_Tab &= ~(0x0010);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);

			sprintf(temp,"%5.1f",m_fCurBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);

			sprintf(temp,"%5.1f",m_fHighBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);

			//+5 volts
			strncpy(temp,&Record[63],4);
			temp[4] = NULL;

			m_fCurP5 = (float)atof(temp);
			if (m_bFirstP5)
			{
				m_bFirstP5 = false;
				m_fLowP5 = m_fHighP5 = m_fCurP5;
			}
			if (m_fCurP5 < m_fLowP5)
			{
				CurrentLow = true;
				m_fLowP5 = m_fCurP5;
			}
			else
				CurrentLow = false;
			if (m_fCurP5 > m_fHighP5)
			{
				CurrentHigh = true;
				m_fHighP5= m_fCurP5;
			}
			else
				CurrentHigh = false;
			if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			{
				CurrentRed = true;			
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
				if (m_bP5VoltOk)
				{
					WriteToFileEx(30,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15760 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
					FlagOthr(1,true);
				}
				m_bP5VoltOk = false;

				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP5VoltOk)
					WriteToFileEx(30,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15770 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
				m_bP5VoltOk = true;
	//			m_iGeneral_Tab &= ~(0x0020);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowP5 <= m_fSup_P05L) || (m_fLowP5 >= m_fSup_P05H))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighP5 <= m_fSup_P05L) || (m_fHighP5 >= m_fSup_P05H))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp);

			sprintf(temp,"%5.1f",m_fCurP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp);

			sprintf(temp,"%5.1f",m_fHighP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp);

			//+15 volts
			strncpy(temp,&Record[67],4);
			temp[4] = NULL;
			m_fCurP15=(float)atof(temp);
			if (m_bFirstP15)
			{
				m_bFirstP15 = false;
				m_fLowP15 = m_fHighP15 = m_fCurP15;
			}
			if (m_fCurP15 < m_fLowP15)
			{
				CurrentLow = true;
				m_fLowP15 = m_fCurP15;
			}
			else
				CurrentLow = false;
			if (m_fCurP15 > m_fHighP15)
			{
				CurrentHigh = true;
				m_fHighP15= m_fCurP15;
			}
			else
				CurrentHigh = false;

			if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
			{
				CurrentRed = true;			
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,COLOR_RED);
				if (m_bP15VoltOk)
				{
					WriteToFileEx(31,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15780 STATUS CHANGE - PLUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
					FlagOthr(1,true);
				}
				m_bP15VoltOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP15VoltOk)
					WriteToFileEx(31,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15790 STATUS CHANGE - PLUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
				m_bP15VoltOk = true;
	//			m_iGeneral_Tab &= ~(0x0040);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowP15 <= m_fSup_P15L) || (m_fLowP15 >= m_fSup_P15H))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighP15 <= m_fSup_P15L) || (m_fHighP15 >= m_fSup_P15H))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,COLOR_RED);
			}
			sprintf(temp,"%5.1f",m_fLowP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,temp);

			sprintf(temp,"%5.1f",m_fCurP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,temp);

			sprintf(temp,"%5.1f",m_fHighP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,temp);

			//-15 volt
			strncpy(temp,&Record[71],5);
			temp[5] = NULL;
			m_fCurM15=(float)atof(temp);
			//m_fCurM15 = psBinaryRecord->minus12;
			if (m_bFirstM15)
			{
				m_bFirstM15 = false;
				m_fLowM15 = m_fHighM15 = m_fCurM15;
			}
			if (m_fCurM15 <= m_fLowM15)
			{
				CurrentLow = true;
				m_fLowM15 = m_fCurM15;
			}
			else
				CurrentLow = false;
			if (m_fCurM15 >= m_fHighM15)
			{
				CurrentHigh = true;
				m_fHighM15= m_fCurM15;
			}
			else
				CurrentHigh = false;
			if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
			{
				CurrentRed = true;
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,COLOR_RED);
				if (m_bM15VoltOk)
				{
					WriteToFileEx(32,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15800 STATUS CHANGE - MINUS 12 VOLT SUPPLY OUT OF TOLERANCE (12)\r\n");
					FlagOthr(1,true);
				}
				m_bM15VoltOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bM15VoltOk)
					WriteToFileEx(32,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15810 STATUS CHANGE - MINUS 12 VOLT SUPPLY NOT OUT OF TOLERANCE (12)\r\n");
				m_bM15VoltOk = true;
	//			m_iGeneral_Tab &= ~(0x0080);
			}

			if (CurrentRed && CurrentLow && (m_fLowM15 <= m_fSup_M15L))// || (m_fLowM15 >= m_fSup_M15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighM15 >= m_fSup_M15H))// || (m_fHighM15 >= m_fSup_M15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,temp);

			sprintf(temp,"%5.1f",m_fCurM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,temp);

			sprintf(temp,"%5.1f",m_fHighM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,temp);

			if (m_pPropertySheet)
				m_pPropertySheet->StatusRec(Record);

			bStale = false;								//DR00406
		}												//DR00406

		char cBuff[1024];
		strcpy(cBuff,"15820 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//Status Record 12			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,STATUS_RECORD,CheckSum);
}

//record 13
int CInstrument::BinaryOffsetRec(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		short offsets0[12];		//2*12
		short offsets1[12];		//2*12
	} *psBinaryRecord;			//55 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);

	for (int i = 0; i < 12; i++)
	{
		Reverse((BYTE*)&(psBinaryRecord->offsets0[i]),sizeof(short));
		Reverse((BYTE*)&(psBinaryRecord->offsets1[i]),sizeof(short));
	}
	char Record[1024];
	sprintf(Record,"13%10u%02x%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d"
						"%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d%4d",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		psBinaryRecord->offsets0[0],psBinaryRecord->offsets0[1],psBinaryRecord->offsets0[2],
		psBinaryRecord->offsets0[3],psBinaryRecord->offsets0[4],psBinaryRecord->offsets0[5],
		psBinaryRecord->offsets0[6],psBinaryRecord->offsets0[7],psBinaryRecord->offsets0[8],
		psBinaryRecord->offsets0[9],psBinaryRecord->offsets0[10],psBinaryRecord->offsets0[11],
		psBinaryRecord->offsets1[0],psBinaryRecord->offsets1[1],psBinaryRecord->offsets1[2],
		psBinaryRecord->offsets1[3],psBinaryRecord->offsets1[4],psBinaryRecord->offsets1[5],
		psBinaryRecord->offsets1[6],psBinaryRecord->offsets1[7],psBinaryRecord->offsets1[8],
		psBinaryRecord->offsets1[9],psBinaryRecord->offsets1[10],psBinaryRecord->offsets1[11]
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);

	OffsetRec(Record);
	return sizeof(SBinaryRec);
}

//record 13
void CInstrument::OffsetRec(char *Record)
{
	int CheckSum;
	if (LengthOk(Record,OFFSET_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bStale = true;											//DR00406

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastOffsetRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastOffsetRecTime = Julian.TotalSeconds();			//DR00406

			m_cOffsetRec = Record;									//DR00406
			
			//added to general dialog box 
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			SetString(GENERALPAGE,IDC_GENERAL_OFFSET,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_OFFSET,temp);

			if (m_pPropertySheet)
				m_pPropertySheet->OffsetRec(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"15840 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//Offset Record 13	//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}

	PostMessage(IDC_RECEIVE_RESULT,OFFSET_RECORD,CheckSum);
}

//record 14
void CInstrument::ID_Record(char *Record)
{
	int CheckSum;
	if (LengthOk(Record,ID_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cIDRec = Record;

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
		ts += HALF_SECOND;

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		if ((ts.GetTotalSeconds() >= m_dTime_Err ) ||
			(ts.GetTotalSeconds() <= -m_dTime_Err))
		{
			FlagTime(1,false);			
			SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
			m_iData_Tab |= 0x0001;
		} 

		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}

		//get status byte 
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 90",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	
		//bit 7 ac power
		if (Status & ACPOWER)
		{			
			SetString(DATAPAGE,IDC_DATA_ACPOWER,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,FAIL);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk)
			{
				WriteToFileEx(33,TO_CEV | TO_PFM, TYPE_INST, &Julian,"15850 STATUS CHANGE - External Power Off (14)\r\n");
				FlagOthr(1,true);
			}
			m_bACPowerOk = false;

			m_iData_Tab |= 0x0001;
		}
		else 
		{
			SetString(DATAPAGE,IDC_DATA_ACPOWER,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,OK);
			if (!m_bACPowerOk)
				WriteToFileEx(33,TO_CEV | TO_PFM, TYPE_INST, &Julian,"15860 STATUS CHANGE - External Power On (14)\r\n");
			m_bACPowerOk = true;
		}
	
		//bit 6 battery 
		if (Status & BATTERY) 
		{			
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(34,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15870 STATUS CHANGE - BATTERY LOW (14)\r\n");
				FlagOthr(1,true);
			}
			m_bBatteryOk = false;

			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
				WriteToFileEx(34,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15880 STATUS CHANGE - BATTERY OK (14)\r\n");
			m_bBatteryOk = true;
		}

		//bit 5 tamper alarm
		if (Status & TAMPER)
		{
			if (m_bCountRateTamperOk)
				WriteToFileEx(35,TO_PFM, TYPE_INST, &Julian, "15890 STATUS CHANGE - COUNT RATE TAMPER (14)\r\n");
			m_bCountRateTamperOk = false;
		}
		else
		{
			if (!m_bCountRateTamperOk)
				WriteToFileEx(35,TO_PFM, TYPE_INST, &Julian, "15900 STATUS CHANGE - NO COUNT RATE TAMPER (14)\r\n");
			m_bCountRateTamperOk = true;
		}

		//bit 4 RMS Sigma Tamper
		if (Status & RMSTAMPER)
		{
			if (m_bRMSSigmaTamperOk)
				WriteToFileEx(36,TO_PFM, TYPE_INST, &Julian, "15910 STATUS CHANGE - RMS SIGMA TAMPER (14)\r\n");
			m_bRMSSigmaTamperOk = false;
		}
		else
		{
			if (!m_bRMSSigmaTamperOk)
				WriteToFileEx(36,TO_PFM, TYPE_INST, &Julian, "15920 STATUS CHANGE - NO RMS SIGMA TAMPER (14)\r\n");
			m_bRMSSigmaTamperOk = true;
		}

		//bit 3 bbm
		if (Status & BBMERROR)
		{
			SetString(DATAPAGE,IDC_DATA_BBMOK,"Error in Read or Write");
			SetColor(DATAPAGE,IDC_DATA_BBMOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,"Error in Read or Write");
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
				WriteToFileEx(37,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15930 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (14)\r\n");
			m_bBBMOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BBMOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,OK);
			if (!m_bBBMOk)
				WriteToFileEx(37,TO_CEV | TO_PFM, TYPE_INST, &Julian, "15940 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (14)\r\n");
			m_bBBMOk = true;
		}

		//bit 2 startup
		if (Status & CWSTART)
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_COLD);
			if (m_dCWStart != 0)
				WriteToFileEx(38,TO_PFM, TYPE_INST, &Julian, "15950 STATUS CHANGE - COLD START (14)\r\n");
			m_dCWStart = 0;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_NORMAL);
			if (m_dCWStart != 1)
				WriteToFileEx(38,TO_PFM, TYPE_INST, &Julian, "15960 STATUS CHANGE - NORMAL START (14)\r\n");
			m_dCWStart = 1;
		}
	
		//bit 0 in MII
		if (Status & INMII) 
		{
			m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			SetString(DATAPAGE,IDC_DATA_OOMII,INMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,INMII_TXT);
			if (!m_bInMII && m_bLogMII)
				WriteToFileEx(39,TO_PFM, TYPE_INST, &Julian, "15970 STATUS CHANGE - IN MEASUREMENT INTERVAL OF INTEREST [BELL icon on] (14)\r\n");
			m_bInMII = true;
		}
		else
		{
			m_pButton->ShowIcon_Bell(VARIANT_FALSE);
			SetString(DATAPAGE,IDC_DATA_OOMII,NOTINMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,NOTINMII_TXT);
			if (m_bInMII && m_bLogMII)
				WriteToFileEx(39,TO_PFM, TYPE_INST, &Julian, "15980 STATUS CHANGE - NOT IN MEASUREMENT INTERVAL OF INTEREST [BELL icon off] (14)\r\n");
			m_bInMII = false;
		}

		//for use in the next few data items
		char temp2[16];

		//get Main Unit ID
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		SetString(DATAPAGE,IDC_DATA_UNITID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_DATA_UNITID,COLOR_YELLOW);
			WriteToFileEx(40,TO_CEV|TO_PFM, TYPE_INST, &Julian, "15990 SETUP FAILURE - UNIT ID (14)\r\n");
			m_iData_Tab |= 0x0002;
		}
				
		//get board 1 ID
		strncpy(temp,&Record[16],3);
		temp[3]=NULL;
		SetString(DATAPAGE,IDC_DATA_UNITID2,temp);
		temp2[0] = NULL;
		bool bMatch;
		GetPrivateProfileString(m_pName,"SET_BOARD1_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_DATA_UNITID2,COLOR_YELLOW);
			WriteToFileEx(41,TO_CEV|TO_PFM, TYPE_INST, &Julian, "16000 SETUP FAILURE - BOARD1 ID (14)\r\n");
			m_iData_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_BOARD1_ID,"SET_BOARD1_ID",temp,bMatch);

		//get board 2 ID
		strncpy(temp,&Record[19],3);
		temp[3]=NULL;
		SetString(DATAPAGE,IDC_DATA_UNITID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_BOARD2_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_GENERAL_IDS,COLOR_YELLOW);
			WriteToFileEx(42,TO_CEV|TO_PFM, TYPE_INST, &Julian, "16010 SETUP FAILURE - BOARD2 ID (14)\r\n");
			m_iData_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_BOARD2_ID,"SET_BOARD2_ID",temp,bMatch);

		strncpy(temp,&Record[22],7);
		temp[7] = NULL;
		SetString(DATAPAGE,IDC_DATA_USERID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pName,"SET_USER_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_DATA_USERID,COLOR_YELLOW);
			WriteToFileEx(43,TO_CEV|TO_PFM, TYPE_INST, &Julian, "16020 SETUP FAILURE - USER ID (14)\r\n");
			m_iData_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_USER_ID,"SET_USER_ID",temp,bMatch);

		//get bbm
		strncpy(temp,&Record[29],6);
		temp[6]=NULL;

		//set all the displays
		SetString(DATAPAGE,IDC_DATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);

		//send message to parent saying we got this type of record
		//use second parameter as flag for good or bad checksum
		if (m_pPropertySheet)
			m_pPropertySheet->ID_Record(Record);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,ID_RECORD,CheckSum);	
}

//remove the high order byte(s) of the exponent
//usually make the exponent 2 digits
char* CInstrument::TrunkExp(char* szString, int iDigits)
{
	if ((iDigits == 1) || (iDigits == 2))
	{
		char *szAt;
		//find the exponent flag
		if ((szAt=strpbrk(szString,"gGfFeE"))!=0)
			strcpy(&(szString[strlen(szString)-3]),&(szString[strlen(szString)-iDigits]));
	}
	return szString;
}


//record 15
int CInstrument::BinaryAcquirRec(BYTE *pBinaryAt)
{
	//expect this binary record
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;
		UCHAR type;
		unsigned int julian;
		UCHAR status;
		float rates[5];
		float g_unc[2];
		unsigned int elapsed_time;
	} *psBinaryRecord;
#pragma pack()

	
	//get the address of a binary record
	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	//big endian to little endian the time
	Reverse((BYTE*)&(psBinaryRecord->julian),4);

	//big endian to little endian the rates
	for (int i = 0; i < 5; i++)
		Reverse((BYTE*)&(psBinaryRecord->rates[i]),4);
	for (int i = 0; i < 2; i++)
		Reverse((BYTE*)&(psBinaryRecord->g_unc[i]),4);

	//big endian to little endian the elapsed time
	Reverse((BYTE*)&(psBinaryRecord->elapsed_time),4);

	//area to build the full text record
	char Record[1024];

	//areas to mess with the individual rates
	char szTemp0[3][32];
	char szTemp1[2][32];
	char szTemp2[2][32];

	//format the neutron rates
	for (int i = 0; i < 2; i++)
	{
		if (fabs((double)(psBinaryRecord->rates[3+i])) < 1.0e-8)
			psBinaryRecord->rates[3+i] = 0.0;
		if (fabs((double)(psBinaryRecord->g_unc[i])) < 1.0e-8)
			psBinaryRecord->g_unc[i] = 0.0;

		//correct for - rate!!!!!
		sprintf(szTemp1[i],"%#10.3e",psBinaryRecord->rates[3+i]);
		if (strlen(szTemp1[i]) > 10)
			sprintf(szTemp1[i], "%#10.2e",psBinaryRecord->rates[3+i]);

		sprintf(szTemp2[i],"%#9.2e",psBinaryRecord->g_unc[i]);
		if (strlen(szTemp2[i]) > 9)
			sprintf(szTemp2[i],"%#9.1e",psBinaryRecord->g_unc[i]);

		//only use the last digit of the exponent
		TrunkExp(szTemp1[i],1);
		TrunkExp(szTemp2[i],1);
	}

	//SCR00236: Large numbers need e-format representation for 
	//binary BBM data conversions.
	//format the gamma rates
	for (int i = 0; i < 3; i++)
	{
		if (psBinaryRecord->rates[i] >= 999999.95)
		{
			sprintf(szTemp0[i],"%10.3e",psBinaryRecord->rates[i]);
			TrunkExp(szTemp0[i],1);
		}
		else
			sprintf(szTemp0[i],"%8.1f",psBinaryRecord->rates[i]);
	}

	//pre-SCR00236
	//format the gamma rates
	//for (int i = 0; i < 3; i++)
	//{
	//	if (psBinaryRecord->rates[i] >= 1000000)
	//		sprintf(szTemp0[i],"%8.0f",psBinaryRecord->rates[i]);
	//	else
	//		sprintf(szTemp0[i],"%8.1f",psBinaryRecord->rates[i]);
	//}

	//build the text record
	sprintf(Record,"15%10u%02x%s%s%s%s%s%s%s%5u",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		szTemp0[0],
		szTemp0[1],
		szTemp0[2],
		szTemp1[0],
		szTemp2[0],
		szTemp1[1],
		szTemp2[1],
		psBinaryRecord->elapsed_time
		);

	//add checksum
	/*int check =*/ AppendCheckSum(Record);

	//add the two bytes for authentication
	strcat(Record,"--");
	
	//send to the first tab page
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	//tell the watch window
	PostToWatch(Record);

//test for Bob//////////////////////////////////////////////
//
// if an acquire record's length isn't ok then prints out
// a hex version of it in the dump file (if active)
// this can occur if a bad binary float is handed us. 
//
////////////////////////////////////////////////////////////
	if (m_bDoDump)
	{
		if (!LengthOk(Record,ACQUIRE_RECORD_LENGTH))
		{
				CString cTemp;
				cTemp.Empty();
				char szTemp[16];
				char* pChar = (char*)psBinaryRecord;
				for (int i = 0; i < sizeof(SBinaryRec); i++)
				{
					unsigned char ucChar = *pChar++;
					sprintf(szTemp,"%02x",ucChar);
					cTemp += szTemp;
				}
				cTemp += "\r\n";
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, (char*)LPCSTR(cTemp));//dump file
		}
	}
///////////////////////////////////////////////////////////////////////

	//handle the record with the "text" version handler
	AcquirRec(Record,true);

	//return the number of bytes the binary record
	return sizeof(SBinaryRec);
}


//record 15
void CInstrument::AcquirRec(char *Record, bool bIgnoreAuth)
{

	if (m_pPropertySheet)
		m_pPropertySheet->AcquirRec(Record,bIgnoreAuth);

	int CheckSum;
	if (LengthOk(Record,ACQUIRE_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cAcquireRec[0] = m_cAcquireRec[1];
		m_cAcquireRec[1] = m_cAcquireRec[2];
		m_cAcquireRec[2] = m_cAcquireRec[3];
		m_cAcquireRec[3] = Record;

		char temp[32];

 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		int AcquireRecordTarget = m_dAcquireRecordCount;
		AcquireTime[AcquireRecordTarget].Init(temp);
		
		//save time in binary to acquire record
		AcquireRec[AcquireRecordTarget].JulianSeconds = 
			AcquireTime[AcquireRecordTarget].TotalSeconds();

		//get status byte 
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 91",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//save status in binary to acquire record
		AcquireRec[AcquireRecordTarget].StatusByte = Status;
		//move all the date displays down one
		SetString(DATAPAGE,IDC_DATA_DATADATE4,
			GetString(DATAPAGE,IDC_DATA_DATADATE3));
		SetString(DATAPAGE,IDC_DATA_DATADATE3,
			GetString(DATAPAGE,IDC_DATA_DATADATE2));
		SetString(DATAPAGE,IDC_DATA_DATADATE2,
			GetString(DATAPAGE,IDC_DATA_DATADATE1));
		SetString(DATAPAGE,IDC_DATA_DATADATE1,AcquireTime[AcquireRecordTarget].Date());
	
		//move all the time displays down one
		SetString(DATAPAGE,IDC_DATA_DATATIME4,
			GetString(DATAPAGE,IDC_DATA_DATATIME3));
		SetString(DATAPAGE,IDC_DATA_DATATIME3,
			GetString(DATAPAGE,IDC_DATA_DATATIME2));
		SetString(DATAPAGE,IDC_DATA_DATATIME2,
			GetString(DATAPAGE,IDC_DATA_DATATIME1));
		SetString(DATAPAGE,IDC_DATA_DATATIME1,AcquireTime[AcquireRecordTarget].Time());
	
		//move all the Neutron A count rate displays down one
		SetString(DATAPAGE,IDC_DATA_PULSEACNT4,
			GetString(DATAPAGE,IDC_DATA_PULSEACNT3));
		SetString(DATAPAGE,IDC_DATA_PULSEACNT3,
			GetString(DATAPAGE,IDC_DATA_PULSEACNT2));
		SetString(DATAPAGE,IDC_DATA_PULSEACNT2,
			GetString(DATAPAGE,IDC_DATA_PULSEACNT1));
		strncpy(temp,&Record[14],8);
		temp[8]=NULL;
		SetString(DATAPAGE,IDC_DATA_PULSEACNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChA = (float)atof(temp);
	
		//move all the Neutron B count rate displays down one
		SetString(DATAPAGE,IDC_DATA_PULSEBCNT4,
			GetString(DATAPAGE,IDC_DATA_PULSEBCNT3));
		SetString(DATAPAGE,IDC_DATA_PULSEBCNT3,
			GetString(DATAPAGE,IDC_DATA_PULSEBCNT2));
		SetString(DATAPAGE,IDC_DATA_PULSEBCNT2,
			GetString(DATAPAGE,IDC_DATA_PULSEBCNT1));
		strncpy(temp,&Record[22],8);
		temp[8]=NULL;
		SetString(DATAPAGE,IDC_DATA_PULSEBCNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChB = (float)atof(temp);
	
		//move all the Neutron C count rate displays down one
		SetString(DATAPAGE,IDC_DATA_PULSECCNT4,
			GetString(DATAPAGE,IDC_DATA_PULSECCNT3));
		SetString(DATAPAGE,IDC_DATA_PULSECCNT3,
			GetString(DATAPAGE,IDC_DATA_PULSECCNT2));
		SetString(DATAPAGE,IDC_DATA_PULSECCNT2,
			GetString(DATAPAGE,IDC_DATA_PULSECCNT1));
		strncpy(temp,&Record[30],8);
		temp[8]=NULL;
		SetString(DATAPAGE,IDC_DATA_PULSECCNT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].NeutronChC = (float)atof(temp);
	
		//move all the gross gammas 1 displays down one
		SetString(DATAPAGE,IDC_DATA_GAMMA1GR4,
			GetString(DATAPAGE,IDC_DATA_GAMMA1GR3));
		SetString(DATAPAGE,IDC_DATA_GAMMA1GR3,
			GetString(DATAPAGE,IDC_DATA_GAMMA1GR2));
		SetString(DATAPAGE,IDC_DATA_GAMMA1GR2,
			GetString(DATAPAGE,IDC_DATA_GAMMA1GR1));
		strncpy(temp,&Record[38],8);
		temp[8]=NULL;
		SetString(DATAPAGE,IDC_DATA_GAMMA1GR1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh1 = (float)atof(temp);
	
		//move all the gamma 1 sigma displays down one
		SetString(DATAPAGE,IDC_DATA_GAMMA1S4,
			GetString(DATAPAGE,IDC_DATA_GAMMA1S3));
		SetString(DATAPAGE,IDC_DATA_GAMMA1S3,
			GetString(DATAPAGE,IDC_DATA_GAMMA1S2));
		SetString(DATAPAGE,IDC_DATA_GAMMA1S2,
			GetString(DATAPAGE,IDC_DATA_GAMMA1S1));
		strncpy(temp,&Record[46],7);
		temp[7]=NULL;
		SetString(DATAPAGE,IDC_DATA_GAMMA1S1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh1Sigma = (float)atof(temp);
	
		//move all the gross gamma 2 displays down one
		SetString(DATAPAGE,IDC_DATA_GAMMA2GR4,
			GetString(DATAPAGE,IDC_DATA_GAMMA2GR3));
		SetString(DATAPAGE,IDC_DATA_GAMMA2GR3,
			GetString(DATAPAGE,IDC_DATA_GAMMA2GR2));
		SetString(DATAPAGE,IDC_DATA_GAMMA2GR2,
			GetString(DATAPAGE,IDC_DATA_GAMMA2GR1));
		strncpy(temp,&Record[53],8);
		temp[8]=NULL;
		SetString(DATAPAGE,IDC_DATA_GAMMA2GR1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh2 = (float)atof(temp);
	
		//move all the sigma 2 displays down one
		SetString(DATAPAGE,IDC_DATA_GAMMA2S4,
			GetString(DATAPAGE,IDC_DATA_GAMMA2S3));
		SetString(DATAPAGE,IDC_DATA_GAMMA2S3,
			GetString(DATAPAGE,IDC_DATA_GAMMA2S2));
		SetString(DATAPAGE,IDC_DATA_GAMMA2S2,
			GetString(DATAPAGE,IDC_DATA_GAMMA2S1));
		strncpy(temp,&Record[61],7);
		temp[7]=NULL;
		SetString(DATAPAGE,IDC_DATA_GAMMA2S1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].GammaCh2Sigma = (float)atof(temp);
	
		//move all the time data acquired displays down one
		SetString(DATAPAGE,IDC_DATA_LENGTHT4,
			GetString(DATAPAGE,IDC_DATA_LENGTHT3));
		SetString(DATAPAGE,IDC_DATA_LENGTHT3,
			GetString(DATAPAGE,IDC_DATA_LENGTHT2));
		SetString(DATAPAGE,IDC_DATA_LENGTHT2,
			GetString(DATAPAGE,IDC_DATA_LENGTHT1));
		strncpy(temp,&Record[68],5);
		temp[5]=NULL;
		SetString(DATAPAGE,IDC_DATA_LENGTHT1,temp);
		//set the acquire record value
		AcquireRec[AcquireRecordTarget].CountTime = (short)atoi(temp);

		//get the checksum byte
		strncpy(temp,&Record[73],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&AcquireCS[AcquireRecordTarget]);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 92",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//get the authentication byte
		strncpy(temp,&Record[75],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&AcquireAUTH[AcquireRecordTarget]);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 93",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		AcquireAUTHIgnore[AcquireRecordTarget] = bIgnoreAuth;

		//this is WRONG! we need to save them incase there was a problem
		//during the handshake with the instrument and it thinks it did
		//a good dumpbbm but we don't.  In this case the first acquire 
		//record of the dumplast will be later than what we have stored
		//as the most recent acquire record -- this is checked in the
		//state machine so basically do the same when in dump last and
		//when not in dump last.  DGP 26 Oct 2006

		//if not in dumplast sequence then add this record to BID file
//		if (m_bInDumpLast)
//		{
//			m_dPreviousBIDTime = AcquireRec[AcquireRecordTarget].JulianSeconds;
//		}
//		else
//		{
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
//		}
		m_bFirst15Rec = false;

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,ACQUIRE_RECORD,CheckSum);
}

//record 16
void CInstrument::DumpEnd(char *Record)
{


	int CheckSum;
	if (LengthOk(Record,END_DUMP_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	m_dNumberOfAcquires = -1;

	if (m_pPropertySheet)
		m_pPropertySheet->DumpEnd(Record);

	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,END_DUMP_RECORD,CheckSum);
}

//record 61
void CInstrument::DumpEnd2(char *Record)
{
	if (m_pPropertySheet)
		m_pPropertySheet->DumpEnd2(Record);

	int CheckSum = CHECKSUM_BAD;
	if (strlen(Record) == END_DUMP2_RECORD_LENGTH+1)
	{
		if (LengthOk(Record,END_DUMP2_RECORD_LENGTH+1))
			CheckSum = CHECKSUM_GOOD;
		char temp[3];
		if (CheckSum == CHECKSUM_GOOD)
		{
			temp[0] = Record[2];
			temp[1] = Record[3];
			temp[2] = NULL;
//try{
			sscanf(temp,"%d",&m_dNumberOfAcquires);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 94",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		}
	}
	else
	{
		if (LengthOk(Record,END_DUMP2_RECORD_LENGTH))
			CheckSum = Checksum(Record);
		else
			CheckSum = CHECKSUM_BAD;
		char temp[2];
		if (CheckSum == CHECKSUM_GOOD)
		{
			temp[0] = Record[2];
			temp[1] = NULL;
//try{
			sscanf(temp,"%x",&m_dNumberOfAcquires);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 95",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		}
		else
			m_dNumberOfAcquires = -1;
	}

	if (m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	PostMessage(IDC_RECEIVE_RESULT,END_DUMP2_RECORD,CheckSum);
}


//record 17
void CInstrument::DumpOk(char *Record)
{
	//don't know if pre or post 4.1 so don't:
	//AdjustTabsTo(PRE4);

	if (m_pPropertySheet)
		m_pPropertySheet->DumpOk(Record);

	int CheckSum;
	if (LengthOk(Record,DUMP_OK_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[8];

		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		bbm = atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);
		SetString(DATAPAGE,IDC_DATA_BYTESMEM,temp);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

		//check checksum
			//send message to instrument if bad
		//send message to parent saying we got this record
		//use second parameter as flag for good or bad checksum
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,DUMP_OK_RECORD,CheckSum);
}

//record 18
int CInstrument::BinaryBatteryRec(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;		//1
		UCHAR type;			//1
		unsigned int julian;//4
		float battery;		//4
	} *psBinaryRecord;		//10 total
#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->battery),4);

	char Record[1024];
	sprintf(Record,"18%10u%4.1f",
		psBinaryRecord->julian,
		psBinaryRecord->battery
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");

	//send to the first tab page
	PostToWatch(Record);

	BatteryRec(Record);
	return sizeof(SBinaryRec);
}

//record 18
void CInstrument::BatteryRec(char *Record)
{
	int CheckSum;
	if (LengthOk(Record,BATTERY_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bStale = true;											//DR00406

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastBatteryRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastBatteryRecTime = Julian.TotalSeconds();			//DR00406

			m_cBatteryRec = Record;

			CString cTemp(Julian.Date());
			cTemp += " ";
			cTemp += Julian.Time();
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYTIME,cTemp);
	
			//battery voltage
			strncpy(temp,&Record[12],4);
			temp[4] = NULL;
			bool CurrentRed = false;
			bool CurrentLow = false;
			bool CurrentHigh= false;
			m_fCurBV=(float)atof(temp);
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV < m_fLowBV) 
			{
				m_fLowBV = m_fCurBV;
				CurrentLow = true;
			}
			if (m_fCurBV > m_fHighBV)
			{
				m_fHighBV= m_fCurBV;
				CurrentHigh= true;
			}
			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				CurrentRed = true;
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
				m_iGeneral_Tab |= 0x0001;
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,COLOR_RED);
				m_iGeneral4_Tab |= 0x0001;
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryTolOk)
				{
					WriteToFileEx(44,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16050 STATUS CHANGE - BATTERY OUT OF TOLERANCE (18)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryTolOk = false;
			}
			else
			{
				CurrentRed = false;
				if (!m_bBatteryTolOk)
					WriteToFileEx(44,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16060 STATUS CHANGE - BATTERY IN TOLERANCE (18)\r\n");
				m_bBatteryTolOk = true;
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
			   )
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,temp);

			sprintf(temp,"%5.1f",m_fCurBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,temp);

			sprintf(temp,"%5.1f",m_fHighBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,temp);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16090 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);		//DR00406

		if (m_pPropertySheet)
			m_pPropertySheet->BatteryRec(Record);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,BATTERY_RECORD,CheckSum);
}

//record 19 GRANDIII ONLY
void CInstrument::TripNSRec(char *Record)
{
/*
19166205461632  12.1LLDAWDWBEXTC255  0255  2-2925a7
19166205336232  12.1LLDAWDWBEXTC255  0255  2-2925a4
xxttttttttttxxttttttxxxxttttxxxxtttxxxtttxxxtttttTT
2 10        2 6     4   4   4   3  3  3  3  5    2
*/
	int CheckSum;
	if (LengthOk(Record,TRIP_NEU_STAT_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		bool bStale = true;											//DR00406

		char temp[32];


		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastTripNSRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastTripNSRecTime = Julian.TotalSeconds();			//DR00406

			m_cTripNSRec = Record;									//DR00406

			float Current;
			strncpy(temp,&Record[14],6);
			temp[6] = NULL;
			Current = (float)atof(temp);
			if (m_dTNSRFirst)
			{
				m_fTNSRHigh = m_fTNSRLow = Current;
				m_dTNSRFirst = false;
			}
			if (Current < m_fTNSRLow) m_fTNSRLow = Current;
			if (Current > m_fTNSRHigh) m_fTNSRHigh = Current;
			sprintf(temp,"%6.1f",m_fTNSRLow);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp);

			sprintf(temp,"%6.1f",Current);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp);

			sprintf(temp,"%6.1f",m_fTNSRHigh);
			SetString(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp);

			strcpy(temp,"Neutron A:");
			strncat(temp,&Record[20],4);
			strcat(temp,", B:");
			strncat(temp,&Record[24],4);
			strcat(temp,", C:");
			strncat(temp,&Record[28],4);
			SetString(GENERALPAGE,IDC_GENERAL_PULSE,temp);

			char temp2[64];
	//		temp2[0] = NULL;
	//		temp[0] = Record[20];
	//		temp[1] = NULL;
			strncpy(temp,&Record[20],12);
			temp[12] = NULL;
			bool bMatch;
			GetPrivateProfileString(m_pName,"SET_PULSE_CONF","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_PULSE,COLOR_YELLOW);
				m_iGeneral_Tab |= 0x0002;
				WriteToFileEx(45,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16100 SETUP FAILURE - PULSE INPUT CONF (19)\r\n");
				bMatch = false;
			}	
			CurrentSetParam(SET_PULSE_CONF,"SET_PULSE_CONF",temp,bMatch);

			if (strcmp("EXTAEXTBEXTC",temp)==0)
			{
			}
			else
			{

				strncpy(temp,&Record[32],3);
				temp[3] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_LLDA,temp);

				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_LLDA,COLOR_YELLOW);
					m_iGeneral_Tab |= 0x0002;
					WriteToFileEx(46,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16110 SETUP FAILURE - LLD A (19)\r\n");
					bMatch = false;
				}
				CurrentSetParam(SET_LLD_A,"SET_LLD_A",temp,bMatch);
		
				strncpy(temp,&Record[38],3);
				temp[3] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_LLDB,temp);

				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_LLDB,COLOR_YELLOW);
					m_iGeneral_Tab |= 0x0002;
					WriteToFileEx(47,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16120 SETUP FAILURE - LLD B (19)\r\n");
					bMatch = false;
				}
				CurrentSetParam(SET_LLD_B,"SET_LLD_B",temp,bMatch);
		
				strncpy(temp,&Record[35],3);
				temp[3] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_WINDOWA,temp);
				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_WINDOWA,COLOR_YELLOW);
					m_iGeneral_Tab |= 0x0002;
					WriteToFileEx(48,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16130 SETUP FAILURE - ULD A (19)\r\n");
					bMatch = false;
				}
				CurrentSetParam(SET_ULD_A,"SET_ULD_A",temp,bMatch);

				strncpy(temp,&Record[41],3);
				temp[3] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_WINDOWB,temp);

				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_WINDOWB,COLOR_YELLOW);
					m_iGeneral_Tab |= 0x0002;
					WriteToFileEx(49,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16140 SETUP FAILURE - ULD B (19)\r\n");
					bMatch = false;
				}
				CurrentSetParam(SET_ULD_B,"SET_ULD_B",temp,bMatch);
		
				strncpy(temp,&Record[44],5);
				temp[5] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_ANALOG,temp);
				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_ANALOG_GAIN","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_ANALOG,COLOR_YELLOW);
					m_iGeneral_Tab |= 0x0002;
					WriteToFileEx(50,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16150 SETUP FAILURE - ANALOG PULSE CHAN GAIN (19)\r\n");
					bMatch = false;
				}
				CurrentSetParam(SET_ANALOG_GAIN,"SET_ANALOG_GAIN",temp,bMatch);
			}

			if (m_pPropertySheet)
				m_pPropertySheet->TripNSRec(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16160 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//TripNSRec 19	//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,TRIP_NEU_STAT_RECORD,CheckSum);
}

bool CInstrument::CurrentSetParam(UINT uiIniName, CString cIniName,CString cValue,bool bMatch)
{
	return CurrentSetParam(uiIniName, (LPSTR)LPCSTR(cIniName),(LPSTR)LPCSTR(cValue),bMatch);
}

bool CInstrument::CurrentSetParam(UINT uiIniName, char*szIniName,char*szValue,bool bMatch)
{
	//carefull here, (bMatch == false) means to go ahead and see if szValue matches ini value
	//whereas (bMatch == true) means we assume they've already been shown to match
	bool bReturn = true;
	if (uiIniName >= SET_BASE && uiIniName <= SET_TOP)
	{
		SetupStrings[uiIniName-SET_BASE] = szValue;
	}
	else if (uiIniName >= SET4_BASE && uiIniName <= SET4_TOP)
	{
		Setup4Strings[uiIniName-SET4_BASE] = szValue;
	}

	char szIniValue[128];
	GetPrivateProfileString(m_pName,szIniName,"",szIniValue,sizeof(szIniValue),m_szIniFile);
	if (uiIniName >= SET_BASE && uiIniName <= SET_TOP)
	{
		SetupINIStrings[uiIniName-SET_BASE] = szIniValue;
	}
	else if (uiIniName >= SET4_BASE && uiIniName <= SET4_TOP)
	{
		Setup4INIStrings[uiIniName-SET4_BASE] = szIniValue;
	}

	if (m_pPropertySheet)
		m_pPropertySheet->CurrentSetParam(szIniName,szValue,bMatch,false);

	m_cSetParamMatchText = szIniValue;
	if (!bMatch)
	{
		if (strcmp(szIniValue,szValue)!=0)
		{
			int iInstSetupTab;
			bReturn = false;
			if (uiIniName >= SET_BASE && uiIniName <= SET_TOP)
			{
				//m_pButton->SetQuestChar(CHICK_CHAR,false);
				//m_pButton->SetQuest(true,true);
				m_pButton->ShowIcon_Square(VARIANT_TRUE);

				iInstSetupTab = m_iInstSetup_Tab;
				m_iInstSetup_Tab |= 0x0001;
				SetupStaticColors[uiIniName-SET_BASE] = COLOR_RED;
				if (m_pPropertySheet && iInstSetupTab != m_iInstSetup_Tab)
				{
					m_pPropertySheet->SetTabColor(IDC_INSTSETUP_TAB, COLOR_RED);
					m_pPropertySheet->Invalidate(false);
				}
			}
			else if ((uiIniName >= SET4_BASE) && (uiIniName <= SET4_TOP))
			{
				//m_pButton->SetQuestChar(CHICK_CHAR,false);
				//m_pButton->SetQuest(true,true);
				m_pButton->ShowIcon_Square(VARIANT_TRUE);
				iInstSetupTab = m_iInst4Setup_Tab;
				m_iInst4Setup_Tab |= 0x0001;
				Setup4StaticColors[uiIniName-SET4_BASE] = COLOR_RED;
				if (m_pPropertySheet && iInstSetupTab != m_iInst4Setup_Tab)
				{
					m_pPropertySheet->SetTabColor(IDC_INST4SETUP_TAB, COLOR_RED);
					m_pPropertySheet->Invalidate(false);
				}
			}
		}
	}
	return bReturn;
}

//record 1a
int CInstrument::BinaryDualIonRecOld(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		float bias;				//4
		UCHAR gain_mode[2];		//2
		BYTE max_gain[2];		//2	
		BYTE gain[2];			//2
		UCHAR dis_mode;			//1
	} *psBinaryRecord;			//17 total 
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->bias),4);

	char Record[1024];
	
	sprintf(Record,"1a%10u00%6.1f%c%c%7d%7d%7d%7d%c",
		psBinaryRecord->julian,
		psBinaryRecord->bias,
		(psBinaryRecord->gain_mode[0]==0)?'A':'M',
		(psBinaryRecord->gain_mode[1]==0)?'A':'M',
		1<<(psBinaryRecord->max_gain[0]*2),
		1<<(psBinaryRecord->max_gain[1]*2),
		1<<(psBinaryRecord->gain[0]*2),
		1<<(psBinaryRecord->gain[1]*2),
		(psBinaryRecord->dis_mode==0)?'I':'R'
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	DualIonRec(Record, true);

	return sizeof(SBinaryRec);
}

//record 1a
int CInstrument::BinaryDualIonRec(BYTE *pBinaryAt)
{
#pragma pack(1)
	/*
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		float bias;				//4
		UCHAR gain_mode[2];		//2
		BYTE max_gain[2];		//2	
		BYTE gain[2];			//2
		UCHAR dis_mode;			//1
	} *psBinaryRecord;			//17 total 
	*/
	struct SBinaryRec {
		UCHAR length;				//1
		UCHAR type;					//1
		unsigned int uiJulian;		//4
		UCHAR ucBoardID;			//1
		float fBias;				//4
		UCHAR ucGainMode[2];		//2
		unsigned int uiMaxGain[2];	//8
		unsigned int uiGain[2];		//8
		UCHAR ucDisconnectMode;		//1
	} *psBinaryRecord;				//30 total
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->uiJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->fBias),4);
	Reverse((BYTE*)&(psBinaryRecord->uiMaxGain[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiMaxGain[1]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiGain[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->uiGain[1]),4);

	char Record[1024];
	/*
	sprintf(Record,"1a%10u00%6.1f%c%c%7d%7d%7d%7d%c",
		psBinaryRecord->julian,
		psBinaryRecord->bias,
		(psBinaryRecord->gain_mode[0]==0)?'A':'M',
		(psBinaryRecord->gain_mode[1]==0)?'A':'M',
		1<<(psBinaryRecord->max_gain[0]*2),
		1<<(psBinaryRecord->max_gain[1]*2),
		1<<(psBinaryRecord->gain[0]*2),
		1<<(psBinaryRecord->gain[1]*2),
		(psBinaryRecord->dis_mode==0)?'I':'R'
		);
	*/
	sprintf(Record,"1a%10u%02x%6.1f%c%c%7d%7d%7d%7d%c",
		psBinaryRecord->uiJulian,		//%10u
		psBinaryRecord->ucBoardID,		//%02x
		psBinaryRecord->fBias,			//%6.1f
		psBinaryRecord->ucGainMode[0],	//%c
		psBinaryRecord->ucGainMode[1],	//%c
		psBinaryRecord->uiMaxGain[0],	//%7d
		psBinaryRecord->uiMaxGain[1],	//%7d
		psBinaryRecord->uiGain[0],		//%7d
		psBinaryRecord->uiGain[1],		//%7d
		psBinaryRecord->ucDisconnectMode//%c
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	DualIonRec(Record, false);
	
	return sizeof(SBinaryRec);
}


//record 1a
void CInstrument::DualIonRec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,DUAL_ION_STAT_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		bool bStale = true;											//DR00406

		char temp[32];
		char temp2[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastDualIonRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastDualIonRecTime = Julian.TotalSeconds();			//DR00406

			m_cDualIonRec = Record;									//DR00406

			//set both dates and times
			strcpy(temp,Julian.Date());
			SetString(GENERALPAGE,IDC_GENERAL_G1DATE,temp);
			SetString(GENERALPAGE,IDC_GENERAL_G2DATE,temp);

			strcpy(temp,Julian.Time());
			SetString(GENERALPAGE,IDC_GENERAL_G1TIME,temp);
			SetString(GENERALPAGE,IDC_GENERAL_G2TIME,temp);

			//board id 00 
			//"00" if from binary message
			if (!bBinarySource)
			{
				//handle board ID
			}

			//Ion Chamber Bias
			float Current;
			strncpy(temp,&Record[14],6);
			temp[6] = NULL;
			Current = (float)atof(temp);
			if (m_dDIRFirst)
			{
				m_fDIRHigh = m_fDIRLow = Current;
				m_dDIRFirst = false;
			}
			if (Current < m_fDIRLow) m_fDIRLow = Current;
			if (Current > m_fDIRHigh) m_fDIRHigh = Current;
			sprintf(temp,"%6.1f",m_fDIRLow);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp);

			sprintf(temp,"%6.1f",Current);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp);

			sprintf(temp,"%6.1f",m_fDIRHigh);
			SetString(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp);

			bool bMatch;
			//gain mode ch 1
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA1M,(Record[20]=='A')?"Automatic":"Manual");
			temp2[0] = NULL;
			temp[0] = Record[20];
			temp[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_GAMMA1_MODE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[20] != temp2[0])
			{
				SetColor(GENERALPAGE,IDC_GENERAL_GAMMA1M,COLOR_YELLOW);
				WriteToFileEx(51,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16180 SETUP FAILURE - GAMMA 1 MODE (1a)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA1_MODE,"SET_GAMMA1_MODE",temp,bMatch);

			//gain mode ch 2
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA2M,(Record[21]=='A')?"Automatic":"Manual");
			temp2[0] = NULL;
			temp[0] = Record[21];
			temp[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_GAMMA2_MODE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[21] != temp2[0])
			{
				SetColor(GENERALPAGE,IDC_GENERAL_GAMMA2M,COLOR_YELLOW);
				WriteToFileEx(52,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16190 SETUP FAILURE - GAMMA 2 MODE (1a)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA2_MODE,"SET_GAMMA2_MODE",temp,bMatch);
			
			//max gain0
			strncpy(temp,&Record[22],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_MAXG1,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_MAX_GAIN0","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_MAXG1,COLOR_YELLOW);
				WriteToFileEx(53,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16200 SETUP FAILURE - GAMMA MAX GAIN0 (1a)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_MAX_GAIN0,"SET_MAX_GAIN0",temp,bMatch);
			//max gain1
			strncpy(temp,&Record[29],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_MAXG2,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_MAX_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_MAXG2,COLOR_YELLOW);
				WriteToFileEx(54,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16210 SETUP FAILURE - GAMMA MAX GAIN1 (1a)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_MAX_GAIN1,"SET_MAX_GAIN1",temp,bMatch);
		
			//gain
			strncpy(temp,&Record[36],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp);
			strncpy(temp,&Record[43],7);
			temp[7] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp);
		
			//disconnect mode
			strncpy(temp,&Record[50],1);
			temp[1] = NULL;
			SetString(GENERALPAGE,IDC_GENERAL_DISCM,(Record[50]=='I')?"Internal":"Remote");
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_DISCONNECT_MODE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(GENERALPAGE,IDC_GENERAL_DISCM,COLOR_YELLOW);
				WriteToFileEx(55,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16220 SETUP FAILURE - DISCONNECT MODE (1a)\r\n");
				m_iGeneral_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_DISCONNECT_MODE,"SET_DISCONNECT_MODE",temp,bMatch);

			if (m_pPropertySheet)
				m_pPropertySheet->DualIonRec(Record, bBinarySource);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16230 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//dual ion record 1a //DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,DUAL_ION_STAT_RECORD,CheckSum);
}

//record 1b
int CInstrument::BinaryGenStatRecOld(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} *psBinaryRecord;			//23 total
#pragma pack()
	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->battery),4);
	Reverse((BYTE*)&(psBinaryRecord->plus5),4);
	Reverse((BYTE*)&(psBinaryRecord->plus12),4);
	Reverse((BYTE*)&(psBinaryRecord->minus12),4);
	char szRecord[512];

	sprintf(szRecord,"1b%10u00%02x00.0000000000000000000000%4.1f%4.2f%4.1f%5.1f0000",
		psBinaryRecord->julian,
		psBinaryRecord->status,
		psBinaryRecord->battery,
		psBinaryRecord->plus5,
		psBinaryRecord->plus12,
		psBinaryRecord->minus12
		);
	
/*
	sprintf(szRecord,"1b%10u%02x%02x%c%c%c%c%c%07d%02x%02x%c%05d%c%c%c%4.1f%4.1f%4.1f%5.1f%04X",
		psBinaryRecord->uiJulian,
		psBinaryRecord->ucUnitID,
		psBinaryRecord->ucStatus,
		psBinaryRecord->cVersion[0],
		psBinaryRecord->cVersion[1],
		psBinaryRecord->cVersion[2],
		psBinaryRecord->cVersion[3],
		psBinaryRecord->cVersion[4],
		psBinaryRecord->ulInspectorID,
		psBinaryRecord->cBoard1Type,
		psBinaryRecord->cBoard2Type,
		psBinaryRecord->cAcqMode,
		(psBinaryRecord->ulBaudRate > 99999)?psBinaryRecord->ulBaudRate/10:psBinaryRecord->ulBaudRate,
		psBinaryRecord->cFrame[0],
		psBinaryRecord->cFrame[1],
		psBinaryRecord->cXOn,
		psBinaryRecord->fBattery,
		psBinaryRecord->fPlus5,
		psBinaryRecord->fPlus12,
		psBinaryRecord->fMinus12,
		psBinaryRecord->uiCodeCksum
		);
*/
	AppendCheckSum(szRecord);
	UpdateSummary(szRecord);
	strcat(szRecord,"\r\n");	
	PostToWatch(szRecord);
	GenStatRec(szRecord, true);

	return sizeof(SBinaryRec);
}

//record 1b
int CInstrument::BinaryGenStatRec(BYTE *pBinaryAt)
{
#pragma pack(1)
/*
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} *psBinaryRecord;			//23 total
*/
  struct SBinaryRec {
		UCHAR length;				//1
		UCHAR type;					//1
		unsigned int uiJulian;		//4
		UCHAR ucUnitID;				//1
		UCHAR ucStatus;				//1
		char  cVersion[5];			//5
		unsigned int ulInspectorID;	//4
		char  cBoard1Type;			//1
		char  cBoard2Type;			//1
		char  cAcqMode;				//1
		unsigned int ulBaudRate;	//4
		char  cFrame[2];			//2
		char  cXOn;					//1
		float fBattery;				//4
		float fPlus5;				//4
		float fPlus12;				//4
		float fMinus12;				//4
		unsigned short uiCodeCksum;	//2
	} *psBinaryRecord;				//45 total

	AdjustTabsTo(PRE4);

#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->uiJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->ulInspectorID),4);
	Reverse((BYTE*)&(psBinaryRecord->ulBaudRate),4);
	Reverse((BYTE*)&(psBinaryRecord->fBattery),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus5),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus12),4);
	Reverse((BYTE*)&(psBinaryRecord->fMinus12),4);
	Reverse((BYTE*)&(psBinaryRecord->uiCodeCksum),2);
	char szRecord[512];

	sprintf(szRecord,"1b%10u%02x%02x%c%c%c%c%c%07d%02x%02x%c%05d%c%c%c%4.1f%4.1f%4.1f%5.1f%04X",
		psBinaryRecord->uiJulian,
		psBinaryRecord->ucUnitID,
		psBinaryRecord->ucStatus,
		psBinaryRecord->cVersion[0],
		psBinaryRecord->cVersion[1],
		psBinaryRecord->cVersion[2],
		psBinaryRecord->cVersion[3],
		psBinaryRecord->cVersion[4],
		psBinaryRecord->ulInspectorID,
		psBinaryRecord->cBoard1Type,
		psBinaryRecord->cBoard2Type,
		psBinaryRecord->cAcqMode,
		(psBinaryRecord->ulBaudRate > 99999)?psBinaryRecord->ulBaudRate/10:psBinaryRecord->ulBaudRate,
		psBinaryRecord->cFrame[0],
		psBinaryRecord->cFrame[1],
		psBinaryRecord->cXOn,
		psBinaryRecord->fBattery,
		psBinaryRecord->fPlus5,
		psBinaryRecord->fPlus12,
		psBinaryRecord->fMinus12,
		psBinaryRecord->uiCodeCksum
		);

	AppendCheckSum(szRecord);
	UpdateSummary(szRecord);
	strcat(szRecord,"\r\n");	
	PostToWatch(szRecord);
	GenStatRec(szRecord, false);

	return sizeof(SBinaryRec);
}


//record 1b
void CInstrument::GenStatRec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,GEN_STATUS_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;

		bool bStale = true;											//DR00406
		
		char temp[64];
		char temp2[64];
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastGenStatRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastGenStatRecTime = Julian.TotalSeconds();			//DR00406

			m_cGenStatRec = Record;

			//build date/time string
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			SetString(GENERALPAGE,IDC_GENERAL_TIME,temp);

			bool bMatch;
		
			//main unit id
			//in HEX on a GRANDIII in dec on a MiniGRAND
			if (!bBinarySource)
			{
				strncpy(temp,&Record[12],2);
				temp[2] = NULL;
				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					int iTemp;
//try{
					sscanf(temp,"%x",&iTemp);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 96",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

					sprintf(temp,"%02d",iTemp);
					if (strcmp(temp,temp2) != 0)
					{
						SetColor(DATAPAGE,IDC_DATA_UNITID,COLOR_YELLOW);
						WriteToFileEx(56,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16250 SETUP FAILURE - UNIT ID(1b)\r\n");
						m_iData_Tab |= 0x0002;
						bMatch = false;
					}
				}
				SetString(DATAPAGE,IDC_DATA_UNITID,temp);
				CurrentSetParam(SET_UNIT_ID,"SET_UNIT_ID",temp,bMatch);
			}
		
			//status
			strncpy(temp,&Record[14],2);
			temp[2] = NULL;
			short Status;
//try{
			sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 97",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
			if (Status & ACPOWER)
			{
				strcpy(temp,FAIL);
				SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
				m_iData_Tab |= 0x0001;
			}
			else 
				strcpy(temp,OK);

			//ac power changed since last status record
			if (Status & ACPOWERCH)
			{
				strcat(temp," with loss since last status record");
		
				if (!m_bACPowerLoss)
				{
					WriteToFileEx(57,TO_CEV | TO_PFM, TYPE_INST, &Julian,"16280 STATUS CHANGE - EXTERNAL POWER LOSS Since last status record (1b)\r\n");
					m_bACPowerLoss = true;
				}
			}
			else
				m_bACPowerLoss = false;

			SetString(DATAPAGE,IDC_DATA_ACPOWER,temp);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp);

			//bit 6 battery 
			if (Status & BATTERY) 
			{			
				SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
				SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
				SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
				if (m_bBatteryOk)
				{
					WriteToFileEx(58,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16290 STATUS CHANGE - BATTERY LOW (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryOk = false;
				m_iData_Tab |= 0x0002;
			}
			else
			{
				SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
				SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
				if (!m_bBatteryOk)
					WriteToFileEx(58,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16300 STATUS CHANGE - BATTERY OK (1b)\r\n");
				m_bBatteryOk = true;
			}

			//Version: 00.00
			if (!bBinarySource)
			{
				strncpy(temp,&Record[16],5);
				temp[5] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_GRANDP,temp);
				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_VERSION","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_GRANDP,COLOR_YELLOW);
					WriteToFileEx(59,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16310 SETUP FAILURE - VERSION (1b)\r\n");
					m_iGeneral_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_VERSION,"SET_VERSION",temp,bMatch);
			}

			//Insp ID: 0000000
			//exclude if from binary record
			if (!bBinarySource)
			{
				strncpy(temp,&Record[21],7);
				temp[7] = NULL;
				SetString(DATAPAGE,IDC_DATA_USERID,temp);
				temp2[0] = NULL;
				GetPrivateProfileString(m_pName,"SET_USER_ID","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(DATAPAGE,IDC_DATA_USERID,COLOR_YELLOW);
					WriteToFileEx(60,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16320 SETUP FAILURE - USERID (1b)\r\n");
					m_iData_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_USER_ID,"SET_USER_ID",temp,bMatch);
			}

			//Board 1 type: FF
			//exclude if from binary record
			if (!bBinarySource)
			{
				strncpy(temp,&Record[28],2);
				temp[2] = NULL;
				SetString(DATAPAGE,IDC_DATA_UNITID2,temp);
				GetPrivateProfileString(m_pName,"SET_BOARD1_ID","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(DATAPAGE,IDC_DATA_UNITID2,COLOR_YELLOW);
					WriteToFileEx(61,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16330 SETUP FAILURE - BOARD1 (1b)\r\n");
					m_iData_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_BOARD1_ID,"SET_BOARD1_ID",temp,bMatch);
			}

			//Board 2 type: FF
			//exclude if from binary record
			if (!bBinarySource)
			{
				strncpy(temp,&Record[30],2);
				temp[2] = NULL;
				SetString(DATAPAGE,IDC_DATA_UNITID3,temp);		
				GetPrivateProfileString(m_pName,"SET_BOARD2_ID","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(DATAPAGE,IDC_DATA_UNITID3,COLOR_YELLOW);
					WriteToFileEx(62,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16340 SETUP FAILURE - BOARD2 (1b)\r\n");
					m_iData_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_BOARD2_ID,"SET_BOARD2_ID",temp,bMatch);
			}
		
			//acq mode
			if (!bBinarySource)
			{
				if (Record[32]=='0') strcpy(temp,"Gamma Only");
				else if (Record[32]== '1') strcpy(temp,"Neutron Only");
				else strcpy(temp,"Gamma & Neutron");
				temp[0] = Record[32];
				temp[1] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_MODE2,temp);
				GetPrivateProfileString(m_pName,"SET_ACQ_MODE1B","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				//if (strcmp(temp,temp2) != 0)
				if (Record[32] != temp2[0])
				{
					SetColor(GENERALPAGE,IDC_GENERAL_MODE2,COLOR_YELLOW);
					WriteToFileEx(63,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16350 SETUP FAILURE - ACQ MODE (1b)\r\n");
					m_iGeneral_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_ACQ_MODE1B,"SET_ACQ_MODE1B",temp,bMatch);
			}

			//baud rate
			//exclude if from binary source

			//port setting
			//exclude if from binary source

			//XON
			//exclude if from binary source

			//battery voltage
			strncpy(temp,&Record[41],4);
			temp[4] = NULL;
			m_fCurBV=(float)atof(temp);
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV < m_fLowBV) 
			{
				m_fLowBV = m_fCurBV;
				CurrentLow = true;
			}
			if (m_fCurBV > m_fHighBV)
			{
				m_fHighBV= m_fCurBV;
				CurrentHigh= true;
			}
			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				CurrentRed = true;
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryTolOk)
				{
					WriteToFileEx(64,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16360 STATUS CHANGE - BATTERY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryTolOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bBatteryTolOk)
					WriteToFileEx(64,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16370 STATUS CHANGE - BATTERY IN TOLERANCE (1b)\r\n");
				m_bBatteryTolOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowBV <= m_fBatVoltL))// || (m_fLowBV >= m_fBatVoltH))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighBV >= m_fBatVoltH))// || (m_fHighBV >= m_fBatVoltH))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_BATTERYMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp);
			sprintf(temp,"%5.1f",m_fCurBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp);
			sprintf(temp,"%5.1f",m_fHighBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp);

			//+5 volts
			strncpy(temp,&Record[45],4);
			temp[4] = NULL;
			m_fCurP5=(float)atof(temp);
			if (m_bFirstP5)
			{
				m_bFirstP5 = false;
				m_fLowP5 = m_fHighP5 = m_fCurP5;
			}
			if (m_fCurP5 < m_fLowP5)
			{
				CurrentLow = true;
				m_fLowP5 = m_fCurP5;
			}
			else
				CurrentLow = false;
			if (m_fCurP5 > m_fHighP5)
			{
				CurrentHigh = true;
				m_fHighP5= m_fCurP5;
			}
			else
				CurrentHigh = false;
			if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			{
				CurrentRed = true;			
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
				if (m_bP5VoltOk)
				{
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16380 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bP5VoltOk = false;

				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP5VoltOk)
					WriteToFileEx(65,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16390 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bP5VoltOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighP5 >= m_fSup_P05H))// || (m_fHighP5 >= m_fSup_P05H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp);

			//+15 volts
			strncpy(temp,&Record[49],4);
			temp[4] = NULL;
			m_fCurP15=(float)atof(temp);
			if (m_bFirstP15)
			{
				m_bFirstP15 = false;
				m_fLowP15 = m_fHighP15 = m_fCurP15;
			}
			if (m_fCurP15 < m_fLowP15)
			{
				CurrentLow = true;
				m_fLowP15 = m_fCurP15;
			}
			else
				CurrentLow = false;
			if (m_fCurP15 > m_fHighP15)
			{
				CurrentHigh = true;
				m_fHighP15= m_fCurP15;
			}
			else
				CurrentHigh = false;

			if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
			{
				CurrentRed = true;			
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,COLOR_RED);
				if (m_bP15VoltOk)
				{
					WriteToFileEx(66,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16400 STATUS CHANGE - PLUS 15 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bP15VoltOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP15VoltOk)
					WriteToFileEx(66,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16410 STATUS CHANGE - PLUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bP15VoltOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowP15 <= m_fSup_P15L))// || (m_fLowP15 >= m_fSup_P15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighP15 >= m_fSup_P15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,temp);

			//-15 volt
			strncpy(temp,&Record[53],5);
			temp[5] = NULL;
			m_fCurM15=(float)atof(temp);
			if (m_bFirstM15)
			{
				m_bFirstM15 = false;
				m_fLowM15 = m_fHighM15 = m_fCurM15;
			}
			if (m_fCurM15 < m_fLowM15)
			{
				CurrentLow = true;
				m_fLowM15 = m_fCurM15;
			}
			else
				CurrentLow = false;
			if (m_fCurM15 > m_fHighM15)
			{
				CurrentHigh = true;
				m_fHighM15= m_fCurM15;
			}
			else
				CurrentHigh = false;
			if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
			{
				CurrentRed = true;
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,COLOR_RED);
				if (m_bM15VoltOk)
				{
					WriteToFileEx(67,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16420 STATUS CHANGE - MINUS 15 VOLT SUPPLY OUT OF TOLERANCE (1b)\r\n");
					FlagOthr(1,true);
				}
				m_bM15VoltOk = false;
				m_iGeneral_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bM15VoltOk)
					WriteToFileEx(67,TO_CEV | TO_PFM, TYPE_INST, &Julian, "16430 STATUS CHANGE - MINUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (1b)\r\n");
				m_bM15VoltOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowM15 <= m_fSup_M15L))// || (m_fLowM15 >= m_fSup_M15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighM15 >= m_fSup_M15H))// || (m_fHighM15 >= m_fSup_M15H))
			{
				SetColor(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp);
			SetString(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,temp);

			//rom checksum
			if (!bBinarySource)
			{
				strncpy(temp,&Record[58],4);
				temp[4] = NULL;
				SetString(GENERALPAGE,IDC_GENERAL_ROM,temp);

				GetPrivateProfileString(m_pName,"SET_1B_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(GENERALPAGE,IDC_GENERAL_ROM,COLOR_YELLOW);
					WriteToFileEx(68,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16440 SETUP FAILURE - 1B ROM CHECKSUM (1b)\r\n");
					m_iGeneral_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_1B_ROM_CHECKSUM,"SET_1B_ROM_CHECKSUM",temp,bMatch);
			}
		
			if (m_pPropertySheet)
				m_pPropertySheet->GenStatRec(Record, bBinarySource);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16450 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff,bStale);//general stat record 1b //DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,GEN_STATUS_RECORD,CheckSum);
}


//record 1c
int CInstrument::BinaryMon_A_RecOld(BYTE *pBinaryAt)
{
#pragma pack(1)

	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR acq_mode;			//1
	} *psBinaryRecord;			//7 total
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	char Record[1024];
						
	sprintf(Record,"1c%10u00.00 000000000000000.0000000.0000000.0000000.000000000.00000.0000.00000000.0000000.0%02x0000",
		psBinaryRecord->julian,
		psBinaryRecord->acq_mode
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	Mon_A_Rec(Record, true);
	
	return sizeof(SBinaryRec);
}

//record 1c
int CInstrument::BinaryMon_A_Rec(BYTE *pBinaryAt)
{
#pragma pack(1)
/*
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR acq_mode;			//1
	} *psBinaryRecord;			//7 total
*/
	struct SBinaryRec {
		UCHAR length;					//1
		UCHAR type;						//1
		unsigned int usJulian;			//4
		char  cVersion[5];				//5
		unsigned short int usAcqTime;	//2
		UCHAR ucImmSize;				//1
		UCHAR ucImmSaveSize;			//1
		float fThresholds[5];			//20
		UCHAR ucChangingEntryCount;		//1
		float fChangingSigma;			//4
		UCHAR ucThresholdEntryCount;	//1
		float fThresholdMult;			//4
		UCHAR ucThresholdExitCount;		//1
		UCHAR ucBkgdSize;				//1
		UCHAR ucBkgdStart;				//1
		float fTamperSigma;				//4
		float fTamperCount;				//4
		char  cAcqMode;					//1
		unsigned short int usCodeCksum;	//2
	} *psBinaryRecord;					//57 total

#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;

	Reverse((BYTE*)&(psBinaryRecord->usJulian),4);
	Reverse((BYTE*)&(psBinaryRecord->usAcqTime),2);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[0]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[1]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[2]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[3]),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholds[4]),4);
	Reverse((BYTE*)&(psBinaryRecord->fChangingSigma),4);
	Reverse((BYTE*)&(psBinaryRecord->fThresholdMult),4);
	Reverse((BYTE*)&(psBinaryRecord->fTamperSigma),4);
	Reverse((BYTE*)&(psBinaryRecord->fTamperCount),4);
	Reverse((BYTE*)&(psBinaryRecord->usCodeCksum),2);
	char Record[1024];
						
//	sprintf(Record,"1c%10u00.00 000000000000000.0000000.0000000.0000000.000000000.00000.0000.00000000.0000000.0%02x0000",
//		psBinaryRecord->julian,
//		psBinaryRecord->acq_mode
//		);

	sprintf(Record,"1c%10u%c%c%c%c%c%5d%02d%02d%8.1f%8.1f%8.1f%10.1f%10.1f%2d%3.1f%2d%3.1f%2d%02d%02d%3.1f%8.1f%02x%04X",
		psBinaryRecord->usJulian,				//%10u
		psBinaryRecord->cVersion[0],			//%c
		psBinaryRecord->cVersion[1],			//%c
		psBinaryRecord->cVersion[2],			//%c
		psBinaryRecord->cVersion[3],			//%c
		psBinaryRecord->cVersion[4],			//%c
		psBinaryRecord->usAcqTime,				//%05d
		psBinaryRecord->ucImmSize,				//%02d
		psBinaryRecord->ucImmSaveSize,			//%02d
		psBinaryRecord->fThresholds[0],			//%8.1f
		psBinaryRecord->fThresholds[1],			//%8.1f
		psBinaryRecord->fThresholds[2],			//%8.1f
		psBinaryRecord->fThresholds[3],			//%10.3f
		psBinaryRecord->fThresholds[4],			//%10.3f
		psBinaryRecord->ucChangingEntryCount,	//%2d
		psBinaryRecord->fChangingSigma,			//%3.1f
		psBinaryRecord->ucThresholdEntryCount,	//%2d
		psBinaryRecord->fThresholdMult,			//%3.1f
		psBinaryRecord->ucThresholdExitCount,	//%2d
		psBinaryRecord->ucBkgdSize,				//%02d
		psBinaryRecord->ucBkgdStart,			//%02d	
		psBinaryRecord->fTamperSigma,			//%3.1f
		psBinaryRecord->fTamperCount,			//%8.1f
		psBinaryRecord->cAcqMode,				//%02x
		psBinaryRecord->usCodeCksum				//%04x
		);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");	
	PostToWatch(Record);
	Mon_A_Rec(Record, false);

	return sizeof(SBinaryRec);
}

//record 1c
void CInstrument::Mon_A_Rec(char *Record, bool bBinarySource)
{

	int CheckSum;
	if (LengthOk(Record,MONITOR_A_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		bool bStale = true;											//DR00406

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		m_cMon_A_Rec = Record;

		if (m_uiLastMon_A_RecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastMon_A_RecTime = Julian.TotalSeconds();			//DR00406

			//build date/time string
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			SetString(USERPAGE,IDC_USERPROG_TIME,temp);

			bool bMatch;
			
			char temp2[32];
			char temp3[32];

			//user program version number
			if (!bBinarySource)
			{
				strncpy(temp,&Record[12],5);
				temp[5] = NULL;
				SetString(USERPAGE,IDC_USERPROG_GRANDUV,temp);

				GetPrivateProfileString(m_pName,"SET_GRAND_USERVER","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_GRANDUV,COLOR_YELLOW);

					WriteToFileEx(69,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16470 SETUP FAILURE - GRAND USER VERSION (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_GRAND_USERVER,"SET_GRAND_USERVER",temp,bMatch);
			}
		
			//user select data acquire time
			if (!bBinarySource)
			{
				strncpy(temp,&Record[17],5);
				temp[5] = NULL;
				SetString(USERPAGE,IDC_USERPROG_USER,temp);

				GetPrivateProfileString(m_pName,"SET_ACQUIRE_TIME","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_USER,COLOR_YELLOW);
					WriteToFileEx(70,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16480 SETUP FAILURE - ACQUIRE TIME (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_ACQUIRE_TIME,"SET_ACQUIRE_TIME",temp,bMatch);

			}
		
			//Immediate Buffer Size / save size
			if (!bBinarySource)
			{
				strcpy(temp,"Total: ");
				strncat(temp,&Record[22],2);
				temp3[0] = Record[22];
				temp3[1] = Record[23];
				temp3[2] = NULL;
				GetPrivateProfileString(m_pName,"SET_BUFFER_TOTAL","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[22],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_IMMEDIATE,COLOR_YELLOW);
					WriteToFileEx(71,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16490 SETUP FAILURE - BUFFER TOTAL (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_BUFFER_TOTAL,"SET_BUFFER_TOTAL",temp3,bMatch);

				strcat(temp,", Save: ");
				strncat(temp,&Record[24],2);
				temp3[0] = Record[24];
				temp3[1] = Record[25];
				temp3[2] = NULL;
				SetString(USERPAGE,IDC_USERPROG_IMMEDIATE,temp);

				GetPrivateProfileString(m_pName,"SET_BUFFER_SAVE","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[24],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_IMMEDIATE,COLOR_YELLOW);
					WriteToFileEx(72,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16500 SETUP FAILURE - BUFFER SAVE (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_BUFFER_SAVE,"SET_BUFFER_SAVE",temp3,bMatch);
			}
		
			if (!bBinarySource)
			{
				//neutron A threshold
				strncpy(temp,&Record[26],8);
				temp[8] = NULL;
				SetString(USERPAGE,IDC_USERPROG_PULSEAT,temp);

				GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSEATHRESH","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_PULSEAT,COLOR_YELLOW);
					WriteToFileEx(73,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16510 SETUP FAILURE - NEUTRON PULSE A THRESHOLD (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_NEUTRON_PULSEATHRESH,"SET_NEUTRON_PULSEATHRESH",temp,bMatch);
		
				//neutron B threshold
				strncpy(temp,&Record[34],8);
				temp[8] = NULL;
				SetString(USERPAGE,IDC_USERPROG_PULSEBT,temp);
				GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSEBTHRESH","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_PULSEBT,COLOR_YELLOW);
					WriteToFileEx(74,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16520 SETUP FAILURE - NEUTRON PULSE B THRESHOLD (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_NEUTRON_PULSEBTHRESH,"SET_NEUTRON_PULSEBTHRESH",temp,bMatch);
			
				//neutron C threshold
				strncpy(temp,&Record[42],8);
				temp[8] = NULL;
				SetString(USERPAGE,IDC_USERPROG_PULSECT,temp);
				GetPrivateProfileString(m_pName,"SET_NEUTRON_PULSECTHRESH","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_PULSECT,COLOR_YELLOW);
					WriteToFileEx(75,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16530 SETUP FAILURE - NEUTRON PULSE C THRESHOLD (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_NEUTRON_PULSECTHRESH,"SET_NEUTRON_PULSECTHRESH",temp,bMatch);
			
				//gamma 1 threshold
				strncpy(temp,&Record[50],10);
				temp[10] = NULL;
				SetString(USERPAGE,IDC_USERPROG_GAMMA1T,temp);
				GetPrivateProfileString(m_pName,"SET_GAMMA_1THRESH","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_GAMMA1T,COLOR_YELLOW);
					WriteToFileEx(76,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16540 SETUP FAILURE - GAMMA 1 THRESHOLD (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_GAMMA_1THRESH,"SET_GAMMA_1THRESH",temp,bMatch);
			
				//gamma 2 threshold
				strncpy(temp,&Record[60],10);
				temp[10] = NULL;
				SetString(USERPAGE,IDC_USERPROG_GAMMA2T,temp);
				GetPrivateProfileString(m_pName,"SET_GAMMA_2THRESH","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_GAMMA2T,COLOR_YELLOW);
					WriteToFileEx(77,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16550 SETUP FAILURE - GAMMA 2 THRESHOLD (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_GAMMA_2THRESH,"SET_GAMMA_2THRESH",temp,bMatch);
			
				//no filter and enter mii
				strncpy(temp,&Record[70],2);
				temp[2] = NULL;
				char temp3[4];
				temp3[0] = Record[70];
				temp3[1] = Record[71];
				temp3[2] = NULL;
				strcat(temp," runs > ");
				strncat(temp,&Record[72],3);
				strcat(temp," sigma from background");
				SetString(USERPAGE,IDC_USERPROG_NOFILTER,temp);
				GetPrivateProfileString(m_pName,"SET_NOFILTERENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[70],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_NOFILTER,COLOR_YELLOW);
					WriteToFileEx(78,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16560 SETUP FAILURE - NO FILTER ENTER MII RUNS (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_NOFILTERENTERMII_RUNS,"SET_NOFILTERENTERMII_RUNS",temp3,bMatch);

				temp3[0] = Record[72];
				temp3[1] = Record[73];
				temp3[2] = Record[74];
				temp3[4] = NULL;
				GetPrivateProfileString(m_pName,"SET_NOFILTERENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[72],temp2,3) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_NOFILTER,COLOR_YELLOW);
					WriteToFileEx(79,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16570 SETUP FAILURE - NO FILTER ENTER MII SIGMA (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_NOFILTERENTERMII_SIGMA,"SET_NOFILTERENTERMII_SIGMA",temp3,bMatch);
			
				//mii enter - consecutive runs
				strncpy(temp,&Record[75],2);
				temp[2] = NULL;
				temp3[0] = Record[75];
				temp3[1] = Record[76];
				temp3[2] = NULL;
				strcat(temp," runs > ");
				strncat(temp,&Record[77],3);
				strcat(temp," sigma from background");
				SetString(USERPAGE,IDC_USERPROG_MIIENTER,temp);
				GetPrivateProfileString(m_pName,"SET_ENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[75],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_MIIENTER,COLOR_YELLOW);
					WriteToFileEx(80,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16580 SETUP FAILURE - ENTER MII RUNS (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_ENTERMII_RUNS,"SET_ENTERMII_RUNS",temp3,bMatch);

				temp3[0] = Record[77];
				temp3[1] = Record[78];
				temp3[2] = Record[79];
				temp3[3] = NULL;
				GetPrivateProfileString(m_pName,"SET_ENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[77],temp2,3) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_MIIENTER,COLOR_YELLOW);
					WriteToFileEx(81,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16590 SETUP FAILURE - ENTER MII SIGMA (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_ENTERMII_SIGMA,"SET_ENTERMII_SIGMA",temp3,bMatch);
			
				//mii exit
				strncpy(temp,&Record[80],2);
				temp[2] = NULL;
				temp3[0] = Record[80];
				temp3[1] = Record[81];
				temp3[2] = NULL;
				strcat(temp," runs < MII threshold");
				SetString(USERPAGE,IDC_USERPROG_MIIEXIT,temp);
				GetPrivateProfileString(m_pName,"SET_MII_EXIT","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[80],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_MIIEXIT,COLOR_YELLOW);
					WriteToFileEx(82,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16600 SETUP FAILURE - MII EXIT (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_MII_EXIT,"SET_MII_EXIT",temp3,bMatch);
			
				//local background
				strcpy(temp,"First ");
				strncat(temp,&Record[82],2);
				temp3[0] = Record[82];
				temp3[1] = Record[83];
				temp3[2] = NULL;
				GetPrivateProfileString(m_pName,"SET_LOCAL_BACKGROUND_FIRST","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[82],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_LOCALBACK,COLOR_YELLOW);
					WriteToFileEx(83,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16610 SETUP FAILURE - LOCAL BACKGROUND FIRST(1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_LOCAL_BACKGROUND_FIRST,"SET_LOCAL_BACKGROUND_FIRST",temp3,bMatch);

			
				strcat(temp," of last ");
				strncat(temp,&Record[84],2);
				temp3[0] = Record[84];
				temp3[1] = Record[85];
				temp3[2] = NULL;
				strcat(temp," runs");
				GetPrivateProfileString(m_pName,"SET_LOCAL_BACKGROUND_LAST","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strncmp(&Record[84],temp2,2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_LOCALBACK,COLOR_YELLOW);
					WriteToFileEx(84,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16620 SETUP FAILURE - LOCAL BACKGROUND LAST(1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_LOCAL_BACKGROUND_LAST,"SET_LOCAL_BACKGROUND_LAST",temp3,bMatch);

			}
		
			//acquisition mode
			strncpy(temp,&Record[97],2);
			temp[2] = NULL;
			short Status;
			short Status2;
//try{
			sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 98",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
			temp2[0] = NULL;
			Status2 = 0;
			GetPrivateProfileString(m_pName,"SET_ACQ_MODE1C","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strlen(temp2))
			{
//try{
				sscanf(temp2,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 99",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
			}
			if ((Status & 0x0001) != (Status2 & 0x0001))
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEAF,COLOR_YELLOW);
				WriteToFileEx(85,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16630 SETUP FAILURE - PULSEA FLAG (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			SetString(USERPAGE,IDC_USERPROG_PULSEAF,(Status & 0x0001)?YES:NO);

			if ((Status & 0x0002) != (Status2 & 0x0002))
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEBF,COLOR_YELLOW);
				WriteToFileEx(86,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16640 SETUP FAILURE - PULSEB FLAG (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			SetString(USERPAGE,IDC_USERPROG_PULSEBF,(Status & 0x0002)?YES:NO);

			if ((Status & 0x0004) != (Status2 & 0x0004))
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSECF,COLOR_YELLOW);
				WriteToFileEx(87,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16650 SETUP FAILURE - PULSEC FLAG (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			SetString(USERPAGE,IDC_USERPROG_PULSECF,(Status & 0x0004)?YES:NO);

			if ((Status & 0x0008) != (Status2 & 0x0008))
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA1F,COLOR_YELLOW);
				WriteToFileEx(88,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16660 SETUP FAILURE - GAMMA1 FLAG (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			SetString(USERPAGE,IDC_USERPROG_GAMMA1F,(Status & 0x0008)?YES:NO);

			if ((Status & 0x0010) != (Status2 & 0x0010))
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA2F,COLOR_YELLOW);
				WriteToFileEx(89,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16670 SETUP FAILURE - GAMMA2 FLAG (1c)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			SetString(USERPAGE,IDC_USERPROG_GAMMA2F,(Status & 0x0010)?YES:NO);
			CurrentSetParam(SET_ACQ_MODE1C,"SET_ACQ_MODE1C",temp,bMatch);
		
			//rom checksum
			if (!bBinarySource)
			{
				strncpy(temp,&Record[99],4);
				temp[4] = NULL;
				SetString(USERPAGE,IDC_USERPROG_ROMCHECK,temp);

				GetPrivateProfileString(m_pName,"SET_1C_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
				bMatch = true;
				if (strcmp(temp,temp2) != 0)
				{
					SetColor(USERPAGE,IDC_USERPROG_ROMCHECK,COLOR_YELLOW);
					WriteToFileEx(90,TO_CEV|TO_PFM,TYPE_INST,&Julian,"16680 SETUP FAILURE - ROM CHECKSUM (1c)\r\n");
					m_iUserProg_Tab |= 0x0002;
					bMatch = false;
				}
				CurrentSetParam(SET_1C_ROM_CHECKSUM,"SET_1C_ROM_CHECKSUM",temp,bMatch);
			}
			
			if (m_pPropertySheet)
				m_pPropertySheet->Mon_A_Rec(Record, bBinarySource);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16690 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//mon_a_rec 1c	//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_A_RECORD,CheckSum);	
}

//record 1d
void CInstrument::TripN2Rec(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,TRIP_NEU2_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		bool bStale = true;											//DR00406

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastTripN2RecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastTripN2RecTime = Julian.TotalSeconds();			//DR00406

			m_cTripN2Rec = Record;

			if (m_pPropertySheet)
				m_pPropertySheet->TripN2Rec(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"16700 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//tripn2rec 1d	//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}

	PostMessage(IDC_RECEIVE_RESULT,TRIP_NEU2_RECORD,CheckSum);
}

//record 1e
//binary version should NEVER be received
int CInstrument::BinaryID2_Record(BYTE *pBinaryAt)
{

#pragma pack(1)
	struct SBinaryRec {	
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR main_id;			//1
		UCHAR status1;			//1
		UCHAR status2;			//1
		ULONG bbm_used;			//4
	} *psBinaryRecord;			//13 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->bbm_used),4);
	char Record[1024];
	sprintf(Record,"1e%10u%02x%02x%02x%6u",
		psBinaryRecord->julian,
		psBinaryRecord->main_id,
		psBinaryRecord->status1,
		psBinaryRecord->status2,
		psBinaryRecord->bbm_used
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	ID2_Record(Record);
	
	return sizeof(SBinaryRec);
}

//record 1e
void CInstrument::ID2_Record(char *Record)
{
	//don't know if pre 4.1 or post 4.1 so don't:
	//AdjustTabsTo(PRE4);

	int CheckSum;
	if (LengthOk(Record,MONITOR_ID2_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cID2_Record = Record;

		bool AnyChange = false;

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (Julian.Year() < 1990)
		{
			SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
			PostMessage(IDC_RECEIVE_RESULT,IDC_BADYEAR_VALUE,Julian.Year());
			WriteToFileEx(91,TO_CEV|TO_PFM,TYPE_COMP,&Julian,"16720 Year less then 1990");
		}

		//post invalid time to CEV if necessary
		if (m_szPreviousGTime[0] != NULL) 
		{
			if (strcmp(m_szPreviousGTime,temp)>0)
			{
				m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);
				WriteToFileEx(92,TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			}
			else
			{
				m_dInvalidTimeCount = 0;
			}
		}

		strcpy(m_szPreviousGTime,temp);

		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"16740 ");
			strcat(cBuff,Record);
			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTimeSpan ts(0,0,0,1);//add one second for transmission delay
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		ts = t1 - (t2-ts);  // Subtract COleDateTimes to find the span
		ts += HALF_SECOND;

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		//select only the  first response after the target time
		bool bDoBecauseOfDaily = false;
		if (m_bAutoTimeDaily)
		{
			unsigned uiSecInDay = t2.GetHour() * 3600 + t2.GetMinute() * 60 + t2.GetSecond();
			if (uiSecInDay < m_uiSecInDayAutoTime)
				m_bAutoTimeDailyTrigger = true;
			else
			{
				if (m_bAutoTimeDailyTrigger) bDoBecauseOfDaily = true;
				m_bAutoTimeDailyTrigger = false;
			}
		}

		
		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			//if not doing because of daily check then
			//turn on all the lights 
			if (m_bAutoTimeDaily)
			{
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_YELLOW);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_YELLOW);
				m_iData_Tab |= 0x0002;
				if (bDoBecauseOfDaily)
				{
					FlagTime(0,false);			
					SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_DEFAULT_TEXT);
					SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_DEFAULT_TEXT);
					m_iData_Tab &= 0xfffd;
					PostMessage(IDC_RECEIVE_RESULT,IDC_TIMEOOT_VALUE,(LPARAM)ts.GetTotalSeconds()); // added typecast gjk
					char cBuff[128];
					sprintf(cBuff,"16755 DOING DAILY TIME SET. Delta: %.0f\r\n",ts.GetTotalSeconds());
					WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, &Julian, cBuff);
				}
			}
			else
			{
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
				m_iData_Tab |= 0x0001;
			}
			if (!m_bTimeError)
			{
				if (WriteToFileEx(93,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16750 STATUS CHANGE - Time Out of Tolerance (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);//id2 record
			}
			m_bTimeError = true;
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
			if (m_bTimeError)
			{
				if (WriteToFileEx(94,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16760 STATUS CHANGE - Time In Tolerance (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);
			}
			m_bTimeError = false;
		}

//TRACE("time span:%f %f\r\n",ts.GetSeconds(),(float)m_dTime_Err);

		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}
		SetString(DATAPAGE,IDC_DATA_INQUIRETIME,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp);

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		SetString(DATAPAGE,IDC_DATA_UNITID,temp);//11
		GetPrivateProfileString(m_pName,
			(m_eMonitorVers==PRE4)?"SET_UNIT_ID":"SET4_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_DATA_UNITID,COLOR_YELLOW);
			WriteToFileEx(95,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16770 SETUP FAILURE - UNIT ID (1e)\r\n");
			m_iData_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam((m_eMonitorVers==PRE4)?SET_UNIT_ID:SET4_UNIT_ID,(m_eMonitorVers==PRE4)?"SET_UNIT_ID":"SET4_UNIT_ID",temp,bMatch);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status1);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 100",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 101",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{			
			SetString(DATAPAGE,IDC_DATA_ACPOWER,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,FAIL);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk) 
			{
				WriteToFileEx(96,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16780 STATUS CHANGE - External Power Off (1e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}
			m_bACPowerOk = false;
			m_iData_Tab |= 0x0001;
		}
		else 
		{
			SetString(DATAPAGE,IDC_DATA_ACPOWER,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,OK);
			if (!m_bACPowerOk)
			{
				WriteToFileEx(96,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16790 STATUS CHANGE - External Power On (1e)\r\n");
				AnyChange = true;
			}
			m_bACPowerOk = true;
		}

		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);

			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(97,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16800 STATUS CHANGE - BATTERY LOW (1e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBatteryOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
			{
				WriteToFileEx(97,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16810 STATUS CHANGE - BATTERY OK (1e)\r\n");
				AnyChange = true;
			}
			m_bBatteryOk = true;
		}


		//-1 is unknown
		//0 is GRAND III
		//1 is MiniGRAND
		if (m_IsMiniGrand != 1)//isn't MiniGRAND
		{
			//bit 5 tamper alarm
			if (Status1 & TAMPER)
			{
				if (m_bCountRateTamperOk)
				{
					WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
						"16820 STATUS CHANGE - COUNT RATE TAMPER (1e)\r\n");
					AnyChange = true;
				}

				m_bCountRateTamperOk = false;
			}
			else
			{
				if (!m_bCountRateTamperOk)
				{
					WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
						"16830 STATUS CHANGE - NO COUNT RATE TAMPER (1e)\r\n");
					AnyChange = true;
				}
				m_bCountRateTamperOk = true;
			}
		}
		else //is MiniGRAND
		{
			//bit 5 tamper alarm
			if (Status1 & TAMPER)
			{
				if (m_bCountRateTamperOk)
				{
					WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
						"16821 STATUS CHANGE - Changing Signal (1e)\r\n");
					AnyChange = true;
				}

				m_bCountRateTamperOk = false;
			}
			else
			{
				if (!m_bCountRateTamperOk)
				{
					WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
						"16831 STATUS CHANGE - No Changing Signal (1e)\r\n");
					AnyChange = true;
				}
				m_bCountRateTamperOk = true;
			}
		}

		if (m_IsMiniGrand != 1)
		{
			//bit 4 RMS Sigma Tamper
			if (Status1 & RMSTAMPER)
			{
				if (m_bRMSSigmaTamperOk)
				{
					WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian,
						"16840 STATUS CHANGE - RMS SIGMA TAMPER (1e)\r\n");
					AnyChange = true;
				}
				m_bRMSSigmaTamperOk = false;
			}
			else
			{
				if (!m_bRMSSigmaTamperOk)
				{
					WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian, 
						"16850 STATUS CHANGE - NO RMS SIGMA TAMPER (1e)\r\n");
					AnyChange = true;
				}
				m_bRMSSigmaTamperOk = true;
			}
		}
		else //is MiniGRAND
		{
			//bit 4 RMS Sigma Tamper
			if (Status1 & RMSTAMPER)
			{
				if (m_bRMSSigmaTamperOk)
				{
					WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian,
						"16841 STATUS CHANGE - Threshold Event (1e)\r\n");
					AnyChange = true;
				}
				m_bRMSSigmaTamperOk = false;
			}
			else
			{
				if (!m_bRMSSigmaTamperOk)
				{
					WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian, 
						"16851 STATUS CHANGE - No Threshold Event (1e)\r\n");
					AnyChange = true;
				}
				m_bRMSSigmaTamperOk = true;
			}
		}

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"16870");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"16880");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"16890");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"16900");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");

			
			SetString(DATAPAGE,IDC_DATA_BBMOK,temp);
			SetColor(DATAPAGE,IDC_DATA_BBMOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
			{
				char temp2[256];
				strcpy(temp2,cBuff);
				strcat(temp2,temp);
				strcat(temp2,"\r\n");
				if (WriteToFileEx(100,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16860 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (1e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, temp2);//specific failure
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBBMOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BBMOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,OK);
			if (!m_bBBMOk)
			{
				WriteToFileEx(100,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16910 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (1e)\r\n");
				AnyChange = true;
			}
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_COLD);
			if (m_dCWStart != 0)
			{
				WriteToFileEx(101,TO_PFM, TYPE_INST, &Julian, 
					"16920 STATUS CHANGE - COLD START (1e)\r\n");
				AnyChange = true;
			}
			m_dCWStart = 0;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_NORMAL);
			if (m_dCWStart != 1)
			{
				WriteToFileEx(101,TO_PFM, TYPE_INST, &Julian, 
					"16930 STATUS CHANGE - NORMAL START (1e)\r\n");
				AnyChange = true;
			}
			m_dCWStart = 1;
		}
	
		//bit 0 mii
		if (Status1 & INMII)
		{
			m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			SetString(DATAPAGE,IDC_DATA_OOMII,INMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,INMII_TXT);
			if (!m_bInMII && m_bLogMII)
			{
				WriteToFileEx(102,TO_PFM, TYPE_INST, &Julian, 
					"16940 STATUS CHANGE - In MEASUREMENT INTERVAL OF INTEREST [BELL icon on] (1e)\r\n");
				AnyChange = true;
			}
			m_bInMII = true;
		}
		else
		{
			m_pButton->ShowIcon_Bell(VARIANT_FALSE);
			SetString(DATAPAGE,IDC_DATA_OOMII,NOTINMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,NOTINMII_TXT);
			if (m_bInMII && m_bLogMII)
			{
				WriteToFileEx(102,TO_PFM, TYPE_INST, &Julian, 
					"16950 STATUS CHANGE - Out of MEASUREMENT INTERVAL OF INTEREST [BELL icon off] (1e)\r\n");
				AnyChange = true;
			}
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			SetString(DATAPAGE,IDC_DATA_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(103,TO_PFM, TYPE_INST, &Julian,
					"16960 STATUS CHANGE - DATA NOT FILTERED (1e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = true;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(103,TO_PFM, TYPE_INST, &Julian, 
					"16970 STATUS CHANGE - DATA FILTERED (1e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = false;
		}

		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"16980 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}
	
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

		//get bbm
		strncpy(temp,&Record[18],6);
		temp[6]=NULL;
		SetString(DATAPAGE,IDC_DATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);

		if (m_pPropertySheet)
			m_pPropertySheet->ID2_Record(Record);	
		
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_ID2_RECORD,CheckSum);	
}

//record 1f
//should never get this as binary
int CInstrument::BinaryAnalyzeOk(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;
		UCHAR type;
		unsigned int bbm_used;
	} *psBinaryRecord;
#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->bbm_used),4);
	char Record[1024];
	sprintf(Record,"1f%6u",	psBinaryRecord->bbm_used);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	AnalyzeOk(Record);
	return sizeof(SBinaryRec);
}

//record 1f
void CInstrument::AnalyzeOk(char *Record)
{
	//don't know if we will get 4.10+ records so don't do:
	//AdjustTabsTo(PRE4);

	int CheckSum;
	if (LengthOk(Record,ANALYZE_OK_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		m_cAnalyzeOkRec = Record;

		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		int bbm=atoi(temp);
		m_pButton->put_Numeric((ULONG)bbm);
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,bbm);

		if (m_pPropertySheet)
			m_pPropertySheet->AnalyzeOk(Record);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,ANALYZE_OK_RECORD,CheckSum);
}

//record 30
int CInstrument::BinaryInformRec(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;
		UCHAR type;
		unsigned int julian;
		UCHAR flag_byte1;
		UCHAR flag_byte2;
		UCHAR flag_byte3;
		UCHAR flag_byte4;
		UCHAR status;
		UCHAR days;
		UCHAR hr;
		UCHAR min;
		UCHAR sec;
	} *psBinaryRecord;
#pragma pack()
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	char Record[1024];
	
	if (psBinaryRecord->flag_byte1 & 0x80)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02x%02u:%02u:%02u",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status,
			(psBinaryRecord->hr  > 99)?99:psBinaryRecord->hr,
			(psBinaryRecord->min > 99)?99:psBinaryRecord->min,
			(psBinaryRecord->sec > 99)?99:psBinaryRecord->sec
			);
	} else if (psBinaryRecord->flag_byte1 & 0x40)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%3d       ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->days
			);
	} else if (psBinaryRecord->flag_byte1 & 0x20)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02x        ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status
			);
	} else if (psBinaryRecord->flag_byte1 & 0x10)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x          ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4
			);
	} else if (psBinaryRecord->flag_byte1 & 0x08)
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x          ",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4
			);
	} else 
	{
		sprintf(Record,"30%10u%02x%02x%02x%02x%02X%02X%02X%02X%02X",
			psBinaryRecord->julian,
			psBinaryRecord->flag_byte1,
			psBinaryRecord->flag_byte2,
			psBinaryRecord->flag_byte3,
			psBinaryRecord->flag_byte4,
			psBinaryRecord->status,
			psBinaryRecord->days,
			psBinaryRecord->hr, 
			psBinaryRecord->min,
			psBinaryRecord->sec
			);
	}

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	InformRec(Record);

	return sizeof(SBinaryRec);
}


//record 30
void CInstrument::InformRec(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,INFORMATION_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bStale = true;											//DR00406

		char temp[32];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		if (m_uiLastInformRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastInformRecTime = Julian.TotalSeconds();			//DR00406

			m_cInformRec = Record;									//DR00406

			if (m_pPropertySheet)									//DR00406
				m_pPropertySheet->InformRec(Record);				//DR00406

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"17010 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//InformRec	//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}

	PostMessage(IDC_RECEIVE_RESULT,INFORMATION_RECORD,CheckSum);
}

//record 31
int CInstrument::BinaryMon_B_Rec(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE length;
		BYTE type;
		unsigned int julian;
		BYTE filter_method;
		short btwn_offsets;
		short max_btwn_offsets;
		BYTE btwn_batt_recs;
		BYTE filter_enable;
		float ll[5];
		float gamma_mult;
		BYTE trig_mode;
		BYTE sync_enable;
		BYTE sync_time[3];
		BYTE sync_ll;
		BYTE sync_ul;
		BYTE tamper_en;
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(PRE4);

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->btwn_offsets),2);
	Reverse((BYTE*)&(psBinaryRecord->max_btwn_offsets),2);

	for (int i = 0; i < 5; i++)
		Reverse((BYTE*)&(psBinaryRecord->ll[i]),4);

	Reverse((BYTE*)&(psBinaryRecord->gamma_mult),4);

	char Record[1024];

	char szTemp[32];

	sprintf(Record,"31%10u%1u%3u%3u%2u%1u",
		psBinaryRecord->julian,
		psBinaryRecord->filter_method,
		psBinaryRecord->btwn_offsets,
		psBinaryRecord->max_btwn_offsets,
		psBinaryRecord->btwn_batt_recs,
		psBinaryRecord->filter_enable);

	for (int i = 0; i < 3; i++)
	{
		if (psBinaryRecord->ll[i] >= 10.000)
			sprintf(szTemp,"%5.1f",psBinaryRecord->ll[i]);
		else
			sprintf(szTemp,"%5.3f",psBinaryRecord->ll[i]);

		strcat(Record,szTemp);
	}

	for (int i = 3; i < 5; i++)
	{
		if (psBinaryRecord->ll[i] >= 1000.000)
			sprintf(szTemp,"%7.1f",psBinaryRecord->ll[i]);
		else
			sprintf(szTemp,"%7.3f",psBinaryRecord->ll[i]);

		strcat(Record,szTemp);
	}

	if (psBinaryRecord->gamma_mult >= 100.000)
		sprintf(szTemp,"%6.1f",psBinaryRecord->gamma_mult);
	else
		sprintf(szTemp,"%6.3f",psBinaryRecord->gamma_mult);

	strcat(Record,szTemp);

	sprintf(szTemp,"%1u%1u%02u%02u%02u%2u%1u%1u",
		psBinaryRecord->trig_mode,
		psBinaryRecord->sync_enable,
		psBinaryRecord->sync_time[0],
		psBinaryRecord->sync_time[1],
		psBinaryRecord->sync_time[2],
		psBinaryRecord->sync_ll,
		psBinaryRecord->sync_ul,
		psBinaryRecord->tamper_en);

	strcat(Record,szTemp);

	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);

	Mon_B_Rec(Record);

	return sizeof(SBinaryRec);
} 

//record 31
void CInstrument::Mon_B_Rec(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,MONITOR_B_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(PRE4);

		bool bStale = true;											//DR00406

		char temp[32];
		char temp2[32];

		bool bMatch;
		char temp3[4];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (m_uiLastOffsetRecTime < Julian.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastOffsetRecTime = Julian.TotalSeconds();			//DR00406

			m_cMon_B_Rec = Record;

			//build date/time string
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			SetString(USERPAGE,IDC_USERPROG_TIME,temp);
			
			//filter method
			SetString(USERPAGE,IDC_USERPROG_FILTERMETH,
				(Record[12]=='1')?"Average Point":"First Point");
			temp3[0] = Record[12];
			temp3[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_FILTER_METHOD","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[12] != temp2[0])
			{
				SetColor(USERPAGE,IDC_USERPROG_FILTERMETH,COLOR_YELLOW);
				WriteToFileEx(104,TO_CEV|TO_PFM,TYPE_INST,&Julian,"17030 SETUP FAILURE - FILTER METHOD (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_FILTER_METHOD,"SET_FILTER_METHOD",temp3,bMatch);

			//take offset
			strncpy(temp,&Record[13],3);
			temp[3] = NULL;
			SetString(USERPAGE,IDC_USERPROG_TAKEOFF,temp);

			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_TAKE_OFFSET","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_TAKEOFF,COLOR_YELLOW);
				WriteToFileEx(105,TO_CEV|TO_PFM,TYPE_INST,&Julian,"17040 SETUP FAILURE - TAKE OFFSET (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_TAKE_OFFSET,"SET_TAKE_OFFSET",temp,bMatch);
		
			//maximum take offset
			strncpy(temp,&Record[16],3);
			temp[3] = NULL;
			SetString(USERPAGE,IDC_USERPROG_TAKEOFFMAX,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_TAKEOFFSETMAX","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_TAKEOFFMAX,COLOR_YELLOW);
				WriteToFileEx(106,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17050 SETUP FAILURE - TAKE OFFSET MAX (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_TAKEOFFSETMAX,"SET_TAKEOFFSETMAX",temp,bMatch);
		
			//battery records
			strncpy(temp,&Record[19],2);
			temp[2] = NULL;
			SetString(USERPAGE,IDC_USERPROG_LOGBATT,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_LOG_BATTERY","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_LOGBATT,COLOR_YELLOW);
				WriteToFileEx(107,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17060 SETUP FAILURE - LOG BATTERY (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_LOG_BATTERY,"SET_LOG_BATTERY",temp,bMatch);
		
			//filtering enabled
			SetString(USERPAGE,IDC_USERPROG_FILTERENAB,
				(Record[21]=='0')?NO:YES);
			temp2[0] = NULL;
			temp3[0] = Record[21];
			temp3[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_FILTERING_ENABLED","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[21] != temp2[0])
			{
				SetColor(USERPAGE,IDC_USERPROG_FILTERENAB,COLOR_YELLOW);
				WriteToFileEx(108,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17070 SETUP FAILURE - FILTERING ENABLED (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_FILTERING_ENABLED,"SET_FILTERING_ENABLED",temp3,bMatch);
		
			//Neutron lower limit A
			strncpy(temp,&Record[22],5);
			temp[5] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSEAL,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_NEUTRON_A_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEAL,COLOR_YELLOW);
				WriteToFileEx(109,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17080 SETUP FAILURE - NEUTRON A LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_A_LOWERLIMIT,"SET_NEUTRON_A_LOWERLIMIT",temp,bMatch);

			//Neutron lower limit B
			strncpy(temp,&Record[27],5);
			temp[5] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSEBL,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_NEUTRON_B_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSEBL,COLOR_YELLOW);
				WriteToFileEx(110,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17090 SETUP FAILURE - NEUTRON B LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_B_LOWERLIMIT,"SET_NEUTRON_B_LOWERLIMIT",temp,bMatch);

			//Neutron lower limit C
			strncpy(temp,&Record[32],5);
			temp[5] = NULL;
			SetString(USERPAGE,IDC_USERPROG_PULSECL,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_NEUTRON_C_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_PULSECL,COLOR_YELLOW);
				WriteToFileEx(111,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17100 SETUP FAILURE - NEUTRON C LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_NEUTRON_C_LOWERLIMIT,"SET_NEUTRON_C_LOWERLIMIT",temp,bMatch);
		
			//Gamma lower limit	1	
			strncpy(temp,&Record[37],7);
			temp[7] = NULL;
			SetString(USERPAGE,IDC_USERPROG_GAMMA1L,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_GAMMA_1_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA1L,COLOR_YELLOW);
				WriteToFileEx(112,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17110 SETUP FAILURE - GAMMA 1 LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA_1_LOWERLIMIT,"SET_GAMMA_1_LOWERLIMIT",temp,bMatch);

			//Gamma lower limit	2	
			strncpy(temp,&Record[44],7);
			temp[7] = NULL;
			SetString(USERPAGE,IDC_USERPROG_GAMMA2L,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_GAMMA_2_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_GAMMA2L,COLOR_YELLOW);
				WriteToFileEx(113,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17120 SETUP FAILURE - GAMMA 2 LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_GAMMA_2_LOWERLIMIT,"SET_GAMMA_2_LOWERLIMIT",temp,bMatch);
		
			//trigger mode
			switch(Record[57])
			{
				case '0':strcpy(temp,"Threshold Only");	break;
				case '1':strcpy(temp,"Change Only");	break;
				case '2':strcpy(temp,"Threshold or Change"); break;
				case '3':strcpy(temp,"Threshold and Change");break;
				default: strcpy(temp,"Unknown");
			}
			SetString(USERPAGE,IDC_USERPROG_TRIGGERMODE,temp);
			temp2[0] = NULL;
			temp3[0] = Record[57];
			temp3[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_TRIGGER_MODE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[57] != temp2[0])
			{
				SetColor(USERPAGE,IDC_USERPROG_TRIGGERMODE,COLOR_YELLOW);
				WriteToFileEx(114,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17130 SETUP FAILURE - TRIGGER MODE (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_TRIGGER_MODE,"SET_TRIGGER_MODE",temp3,bMatch);

			//Sync enabled
			switch(Record[58])
			{
			case '0':strcpy(temp,"No");break;
			case '1':strcpy(temp,"Yes");break;
			//case '2':strcpy(temp,"Hourly");break;  //for future plans...
			//case '3':strcpy(temp,"Daily");break;
			//case '4':strcpy(temp,"Both");break;
			default: strcpy(temp,"Unknown");
			}
			SetString(USERPAGE,IDC_USERPROG_SYNCE,temp);
			temp2[0] = NULL;
			temp3[0] = Record[58];
			temp3[1] = NULL;
			GetPrivateProfileString(m_pName,"SET_SYNC_ENABLED","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (Record[58] != temp2[0])
			{
				SetColor(USERPAGE,IDC_USERPROG_SYNCE,COLOR_YELLOW);
				WriteToFileEx(115,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17140 SETUP FAILURE - SYNC ENABLED (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_SYNC_ENABLED,"SET_SYNC_ENABLED",temp3,bMatch);
		
			//Sync time
			strncpy(temp,&Record[59],2);
			temp[2] = NULL;
			strcat(temp,":");
			strncat(temp,&Record[61],2);
			strcat(temp,":");
			strncat(temp,&Record[63],2);
			SetString(USERPAGE,IDC_USERPROG_SYNCT,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_SYNC_TIME","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_SYNCT,COLOR_YELLOW);
				WriteToFileEx(116,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17150 SETUP FAILURE - SYNC TIME (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_SYNC_TIME,"SET_SYNC_TIME",temp,bMatch);
		
			//Sync lower limit
			strncpy(temp,&Record[65],2);
			temp[2] = NULL;
			SetString(USERPAGE,IDC_USERPROG_SYNCL,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_SYNC_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_SYNCL,COLOR_YELLOW);
				WriteToFileEx(117,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17160 SETUP FAILURE - SYNC LOWER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_SYNC_LOWERLIMIT,"SET_SYNC_LOWERLIMIT",temp,bMatch);
		
			//Sync upper limit
			strncpy(temp,&Record[67],1);
			temp[1] = 0;
			SetString(USERPAGE,IDC_USERPROG_SYNCU,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pName,"SET_SYNC_UPPERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				SetColor(USERPAGE,IDC_USERPROG_SYNCU,COLOR_YELLOW);
				WriteToFileEx(118,TO_CEV|TO_PFM,TYPE_INST, &Julian, "17170 SETUP FAILURE - SYNC UPPER LIMIT (31)\r\n");
				m_iUserProg_Tab |= 0x0002;
				bMatch = false;
			}
			CurrentSetParam(SET_SYNC_UPPERLIMIT,"SET_SYNC_UPPERLIMIT",temp,bMatch);

			if (m_pPropertySheet)
				m_pPropertySheet->Mon_B_Rec(Record);
	
			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"17180 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//MON_B_RECORD

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file

	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_B_RECORD,CheckSum);	
}

//binary version should NEVER be received
int CInstrument::BinaryID3_Record(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {	
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR main_id;			//1
		UCHAR status1;			//1
		UCHAR status2;			//1
		ULONG bbm_used;			//4
	} *psBinaryRecord;			//13 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	Reverse((BYTE*)&(psBinaryRecord->julian),4);
	Reverse((BYTE*)&(psBinaryRecord->bbm_used),4);
	char Record[1024];
	sprintf(Record,"1e%10u%02x%02x%02x%10u",
		psBinaryRecord->julian,
		psBinaryRecord->main_id,
		psBinaryRecord->status1,
		psBinaryRecord->status2,
		psBinaryRecord->bbm_used
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\r\n");
	PostToWatch(Record);
	ID3_Record(Record);

	return sizeof(SBinaryRec);
}

//record 4d
void CInstrument::AnalyzeOk3Record(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,ANALYZE_OK3_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);
		m_cAnalyzeOk3Rec = Record;

		char temp[16];
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		//int bbm=atoi(temp);
		unsigned long ulbbm;
//try{
		sscanf(temp,"%d",&ulbbm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 102",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		m_pButton->put_Numeric(ulbbm);

		if (m_pPropertySheet)
			m_pPropertySheet->AnalyzeOk3Record(Record);

		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,ANALYZE_OK_RECORD,CheckSum);
}

//record 4f
void CInstrument::DumpOk3_Record(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,DUMP_OK3_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);
		char temp[16];

		//get bbm bytes
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
//try{
		sscanf(temp,"%d",&ulbbm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 103",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		if (m_pButton)
			m_pButton->put_Numeric(ulbbm);
		SetString(DATAPAGE,IDC_DATA_BYTESMEM,temp);

		if (m_pPropertySheet)
			m_pPropertySheet->DumpOk3_Record(Record);

		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	}
	PostMessage(IDC_RECEIVE_RESULT,DUMP_OK_RECORD,CheckSum);
}

//record 4e
void CInstrument::ID3_Record(char *Record)
{

	int CheckSum;
	LengthOk(Record,MONITOR_ID3_RECORD_LENGTH);
	if (true)
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);
		
		m_cID3_Record = Record;

		bool AnyChange = false;
	
		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		if (Julian.Year() < 1990)
		{
			SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
			PostMessage(IDC_RECEIVE_RESULT,IDC_BADYEAR_VALUE,Julian.Year());
			WriteToFileEx(119,TO_CEV|TO_PFM,TYPE_COMP,&Julian,"16720 Year less then 1990");
		}

		//post invalid time to CEV if necessary
		if (m_szPreviousGTime[0] != NULL) 
		{
			if (strcmp(m_szPreviousGTime,temp)>0)
			{
				m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);
				WriteToFileEx(120,TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
			}
			else
			{
				m_dInvalidTimeCount = 0;
			}
		}

		strcpy(m_szPreviousGTime,temp);

		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"16740 ");
			strcat(cBuff,Record);
			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
		ts += HALF_SECOND;

		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		bool bDoBecauseOfDaily = false;
		if (m_bAutoTimeDaily)
		{
			unsigned uiSecInDay = t2.GetHour() * 3600 + t2.GetMinute() * 60 + t2.GetSecond();
			if (uiSecInDay < m_uiSecInDayAutoTime)
				m_bAutoTimeDailyTrigger = true;
			else
			{
				if (m_bAutoTimeDailyTrigger) bDoBecauseOfDaily = true;
				m_bAutoTimeDailyTrigger = false;
			}
		}

		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			if (m_bAutoTimeDaily)
			{
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_YELLOW);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_YELLOW);
				m_iData_Tab |= 0x0002;
				if (bDoBecauseOfDaily)
				{
					FlagTime(0,false);			
					SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_DEFAULT_TEXT);
					SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_DEFAULT_TEXT);
					PostMessage(IDC_RECEIVE_RESULT,IDC_TIMEOOT_VALUE,(LPARAM)ts.GetTotalSeconds()); // added typecast gjk
					char cBuff[128];
					sprintf(cBuff,"16755 DOING DAILY TIME SET. Delta: %.0f\r\n",ts.GetTotalSeconds());
					WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, &Julian, cBuff);
				}
			}
			else
			{
				FlagTime(1,false);			
				SetColor(DATAPAGE,IDC_DATA_INQUIRETIME,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,COLOR_RED);
				m_iData_Tab |= 0x0001;
			}
			if (!m_bTimeError)
			{
				if (WriteToFileEx(121,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16750 STATUS CHANGE - Time Out of Tolerance (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);//id2 record
			}
			m_bTimeError = true;
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
			if (m_bTimeError)
			{
				if (WriteToFileEx(121,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16760 STATUS CHANGE - Time In Tolerance (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian);
			}
			m_bTimeError = false;
		}
		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}
		SetString(DATAPAGE,IDC_DATA_INQUIRETIME,temp);
		SetString(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp);

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		SetString(DATAPAGE,IDC_DATA_UNITID,temp);
		GetPrivateProfileString(m_pName,"SET4_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);//no scr 2 Jan 2008 dgp
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
		{
			SetColor(DATAPAGE,IDC_DATA_UNITID,COLOR_YELLOW);
			WriteToFileEx(122,TO_CEV|TO_PFM,TYPE_INST, &Julian, "16770 SETUP FAILURE - UNIT ID (4e)\r\n");//no scr 2 Jan 2008 dgp
			m_iData_Tab |= 0x0002;
			bMatch = false;
		}
		CurrentSetParam(SET_UNIT_ID,"SET4_UNIT_ID",temp,bMatch);//no scr 2 Jan 2008 dgp
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status1);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 104",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 105",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{
			SetString(DATAPAGE,IDC_DATA_ACPOWER,FAIL);
			SetColor(DATAPAGE,IDC_DATA_ACPOWER,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,FAIL);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk) 
			{
				WriteToFileEx(123,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16780 STATUS CHANGE - External Power Off (4e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}
			m_bACPowerOk = false;
			m_iData_Tab |= 0x0001;
		}
		else 
		{
			SetString(DATAPAGE,IDC_DATA_ACPOWER,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,OK);
			if (!m_bACPowerOk)
			{
				WriteToFileEx(123,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16790 STATUS CHANGE - External Power On (4e)\r\n");
				AnyChange = true;
			}
			m_bACPowerOk = true;
		}
	
		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,ILOW);
			SetColor(DATAPAGE,IDC_DATA_BATTERYOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,ILOW);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFileEx(124,TO_CEV | TO_PFM, TYPE_INST, &Julian,
					"16800 STATUS CHANGE - BATTERY LOW (4e)\r\n");
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBatteryOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BATTERYOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,OK);
			if (!m_bBatteryOk)
			{
				WriteToFileEx(124,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16810 STATUS CHANGE - BATTERY OK (4e)\r\n");
				AnyChange = true;
			}
			m_bBatteryOk = true;

		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
		{
			if (m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"16821 STATUS CHANGE - Changing Signal (4e)\r\n");
				AnyChange = true;
			}

			m_bCountRateTamperOk = false;
		}
		else
		{
			if (!m_bCountRateTamperOk)
			{
				WriteToFileEx(98,TO_PFM, TYPE_INST, &Julian, 
					"16831 STATUS CHANGE - No Changing Signal (4e)\r\n");
				AnyChange = true;
			}
			m_bCountRateTamperOk = true;
		}

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
		{
			if (m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian,
					"16841 STATUS CHANGE - Threshold Event (4e)\r\n");
				AnyChange = true;
			}

			m_bRMSSigmaTamperOk = false;
		}
		else
		{
			if (!m_bRMSSigmaTamperOk)
			{
				WriteToFileEx(99,TO_PFM, TYPE_INST, &Julian, 
					"16851 STATUS CHANGE - No Threshold Event (4e)\r\n");
				AnyChange = true;
			}

			m_bRMSSigmaTamperOk = true;
		}

		//bit 3 bbm
		if (Status1 & BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & BBMWRITEERROR_FLAG)
			{
				strcpy(temp,BBMWRITEERROR);
				strcpy(cBuff,"16870");
			}
			else if (Status2 & BBMNUMOFBYTES_FLAG)
			{
				strcpy(temp,BBMNUMOFBYTES);
				strcpy(cBuff,"16880");
			}
			else if (Status2 & BBMISFULLEDER_FLAG)
			{
				strcpy(temp,BBMISFULLEDER);
				strcpy(cBuff,"16890");
			}
			else
			{
				strcpy(temp,BBMINVALIDOPC);
				strcpy(cBuff,"16900");
			}
			strcat(cBuff," STATUS CHANGE (cont) - ");
		
			SetString(DATAPAGE,IDC_DATA_BBMOK,temp);
			SetColor(DATAPAGE,IDC_DATA_BBMOK,COLOR_RED);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp);
			SetColor(SUMMARYPAGE,IDC_SUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
			{
				char temp2[256];
				strcpy(temp2,cBuff);
				strcat(temp2,temp);
				strcat(temp2,"\r\n");
				if (WriteToFileEx(127,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16860 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (4e)\r\n"))
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, temp2);//specific failure
				AnyChange = true;
				FlagOthr(1,true);
			}

			m_bBBMOk = false;
			m_iData_Tab |= 0x0001;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_BBMOK,OK);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BBMOK,OK);
			if (!m_bBBMOk)
			{
				WriteToFileEx(127,TO_CEV | TO_PFM, TYPE_INST, &Julian, 
					"16910 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (4e)\r\n");
				AnyChange = true;
			}

			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_COLD);
			if (m_dCWStart != 0)
			{
				WriteToFileEx(128,TO_PFM, TYPE_INST, &Julian, 
					"16920 STATUS CHANGE - COLD START (4e)\r\n");
				AnyChange = true;
			}

			m_dCWStart = 0;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_OPERATION,START_NORMAL);
			if (m_dCWStart != 1)
			{
				WriteToFileEx(128,TO_PFM, TYPE_INST, &Julian, 
					"16930 STATUS CHANGE - NORMAL START (4e)\r\n");
				AnyChange = true;
			}

			m_dCWStart = 1;

		}
	
		//bit 1 reserved

		//bit 0 mii
		if (Status1 & INMII)
		{
			m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			SetString(DATAPAGE,IDC_DATA_OOMII,INMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,INMII_TXT);
			if (!m_bInMII && m_bLogMII)
			{
				WriteToFileEx(129,TO_PFM, TYPE_INST, &Julian, 
					"16940 STATUS CHANGE - In MEASUREMENT INTERVAL OF INTEREST [BELL icon on] (4e)\r\n");
				AnyChange = true;
			}

			m_bInMII = true;
		}
		else
		{
			m_pButton->ShowIcon_Bell(VARIANT_FALSE);
			SetString(DATAPAGE,IDC_DATA_OOMII,NOTINMII_TXT);
			SetString(SUMMARYPAGE,IDC_SUMMARY_OOMII,NOTINMII_TXT);
			if (m_bInMII && m_bLogMII)
			{
				WriteToFileEx(129,TO_PFM, TYPE_INST, &Julian, 
					"16950 STATUS CHANGE - Out of MEASUREMENT INTERVAL OF INTEREST [BELL icon off] (4e)\r\n");
				AnyChange = true;
			}

			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			SetString(DATAPAGE,IDC_DATA_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(130,TO_PFM, TYPE_INST, &Julian,
					"16960 STATUS CHANGE - DATA NOT FILTERED (4e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = true;
		}
		else
		{
			SetString(DATAPAGE,IDC_DATA_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_bLogFilter)
			{
				WriteToFileEx(130,TO_PFM, TYPE_INST, &Julian, 
					"16970 STATUS CHANGE - DATA FILTERED (4e)\r\n");
				AnyChange = true;
			}

			m_bDataFiltered = false;
		}

		if (m_pPropertySheet)
			m_pPropertySheet->ID3_Record(Record);

		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"18700 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);//dump file
	
		//get bbm
		strncpy(temp,&Record[18],10);
		temp[10]=NULL;
		SetString(DATAPAGE,IDC_DATA_BYTESMEM,temp);
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		//int bbm=atoi(temp);
		unsigned long ulbbm;
//try{
		sscanf(temp,"%d",&ulbbm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 106",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		m_pButton->put_Numeric(ulbbm);
	
		PostMessage(IDC_RECEIVE_RESULT,IDC_BBM_VALUE,ulbbm);
	}
	PostMessage(IDC_RECEIVE_RESULT,MONITOR_ID3_RECORD,CheckSum);	
}

//record 41 (post monitor version 4 only)
int
CInstrument::BinaryGeneralConfigRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;				//1
		BYTE ucOpcode;				//1
		unsigned int uiJulianTime;	//4
		unsigned short usCountTime;	//2
		BYTE ucBiasTypeConf;		//1
		float fBiasSetPoint;		//4
		BYTE ucBaudFrame;			//1
		BYTE ucModeFlags;			//1
		BYTE ucModeFlags2;			//1
		BYTE ucBatRecInt;			//1
		BYTE ucFilterCont;			//1
		BYTE ucImmBuffSize;			//1
		BYTE ucImmBuffSaveSize;		//1
		BYTE ucLocalBGSize;			//1
		BYTE ucLocalBGEnd;			//1
		unsigned short usStatusRecInterval; //2
		unsigned int uiInspectorID;	//4
		BYTE ucUnitID;				//1
		BYTE ucTimeSyncState;		//1
		BYTE ucTimeSyncHourlyMin;	//1
		BYTE ucTimeSyncHourlySec;	//1
		BYTE ucTimeSyncDailyHrs;	//1
		BYTE ucTimeSyncDailyMin;	//1
		BYTE ucTimeSyncDailySec;	//1
		BYTE ucTimeSyncLowerTol;	//1
		BYTE ucTimeSyncUpperTol;	//1
		BYTE ucFirmwareVersion[5];	//5
		unsigned short usFirmwareCS;//2
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	Reverse((BYTE*)&(psBinaryRecord->usCountTime),2);
	Reverse((BYTE*)&(psBinaryRecord->fBiasSetPoint),4);
	Reverse((BYTE*)&(psBinaryRecord->uiInspectorID),4);
	Reverse((BYTE*)&(psBinaryRecord->usStatusRecInterval),2);
	Reverse((BYTE*)&(psBinaryRecord->usFirmwareCS),2);
	//build the text version
	char Record[128];
	sprintf(Record,"%02X%10u%5u%02X%#6.1f%02X%02X%02X%2u%01X%2u%2u%2u%2u%5u%7u%02X%01X%02u%02u%2u%02u%02u%2u%2u%c%c%c%c%c%04X",
		psBinaryRecord->ucOpcode,				//1
		psBinaryRecord->uiJulianTime,			//4
		psBinaryRecord->usCountTime,			//2
		psBinaryRecord->ucBiasTypeConf,			//1
		psBinaryRecord->fBiasSetPoint,			//4
		psBinaryRecord->ucBaudFrame,			//1
		psBinaryRecord->ucModeFlags,			//1
		psBinaryRecord->ucModeFlags2,			//1
		psBinaryRecord->ucBatRecInt,			//1
		psBinaryRecord->ucFilterCont,		  	//1
		psBinaryRecord->ucImmBuffSize,			//1
		psBinaryRecord->ucImmBuffSaveSize,		//1
		psBinaryRecord->ucLocalBGSize,			//1
		psBinaryRecord->ucLocalBGEnd,			//1
		psBinaryRecord->usStatusRecInterval,	//2
		psBinaryRecord->uiInspectorID,			//4
		psBinaryRecord->ucUnitID,				//1
		psBinaryRecord->ucTimeSyncState,		//1
		psBinaryRecord->ucTimeSyncHourlyMin,	//1
		psBinaryRecord->ucTimeSyncHourlySec,	//1
		psBinaryRecord->ucTimeSyncDailyHrs,		//1
		psBinaryRecord->ucTimeSyncDailyMin,		//1
		psBinaryRecord->ucTimeSyncDailySec,		//1
		psBinaryRecord->ucTimeSyncLowerTol,		//1
		psBinaryRecord->ucTimeSyncUpperTol,		//1
		psBinaryRecord->ucFirmwareVersion[0],	//1
		psBinaryRecord->ucFirmwareVersion[1],	//1
		psBinaryRecord->ucFirmwareVersion[2],	//1
		psBinaryRecord->ucFirmwareVersion[3],	//1
		psBinaryRecord->ucFirmwareVersion[4],	//1
		psBinaryRecord->usFirmwareCS);			//2

	//add a good checksum
	AppendCheckSum(Record);
	//send it to the summary page
	UpdateSummary(Record);
	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate formatting
	char cBuff[1024];
	if (strlen(Record) == GENERAL_CONFIG_RECORD_LENGTH)
	{

		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		GeneralConfigRecord(Record);

	}
	else
	{
		sprintf(cBuff,"18123 BINARY GENERAL CONFIG RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(131,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18124 BINARY GENERAL CONFIG RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}
	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 42 (post monitor version 4 only)
int  
CInstrument::BinaryInstrumentStatusRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;				//1
		BYTE ucOpcode;				//1
		unsigned int uiJulianTime;	//4
		float fBatteryVoltage;		//4
		float fPlus5Voltage;		//4
		float fPlus12Voltage;		//4
		float fMinus12Voltage;		//4
		BYTE ucACPowerStatus;		//1
		float fBiasVoltage;			//4
		float fIonChamberVoltage;	//4
		float fOnBoardTemp;			//4
		float fOffBoardTemp;		//4
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	Reverse((BYTE*)&(psBinaryRecord->fBatteryVoltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus5Voltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fPlus12Voltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fMinus12Voltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fBiasVoltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fIonChamberVoltage),4);
	Reverse((BYTE*)&(psBinaryRecord->fOnBoardTemp),4);
	Reverse((BYTE*)&(psBinaryRecord->fOffBoardTemp),4);
	//build the text version
	char Record[64];
	sprintf(Record,"%02X%10u%#4.1f%#4.1f%#4.1f%#5.1f%01X%#6.1f%#5.1f%#5.1f%#5.1f",
		psBinaryRecord->ucOpcode,				//1
		psBinaryRecord->uiJulianTime,			//4
		psBinaryRecord->fBatteryVoltage,		//4
		psBinaryRecord->fPlus5Voltage,			//4
		psBinaryRecord->fPlus12Voltage,			//4
		psBinaryRecord->fMinus12Voltage,		//4
		psBinaryRecord->ucACPowerStatus,		//1
		psBinaryRecord->fBiasVoltage,			//4
		psBinaryRecord->fIonChamberVoltage,		//4
		psBinaryRecord->fOnBoardTemp,			//4
		psBinaryRecord->fOffBoardTemp);			//4
		
	//add a good checksum
	AppendCheckSum(Record);

	//send it to the summary page
	UpdateSummary(Record);

	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate formatting
	char cBuff[1024];
	if (strlen(Record) == INSTRUMENT_STATUS_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		InstrumentStatusRecord(Record);
	}
	else
	{
		sprintf(cBuff,"18210 BINARY INSTRUMENT STATUS RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(132,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18220 BINARY INSTRUMENT STATUS RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}
	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 43 (post monitor version 4 only)
int  
CInstrument::BinaryInstrumentInfoRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;				//1
		BYTE ucOpcode;				//1
		unsigned int uiJulianTime;	//4
		BYTE ucMPBID[6];			//6
		BYTE ucMPBXilinxVersion;	//1
		BYTE ucDCMSerialNumber;		//1
		BYTE ucDCMBoardType;		//1
		BYTE ucDCMXilinxVersion;	//1
		BYTE ucDCMActelVersion;		//1
		BYTE ucDCMPresence;			//1
		BYTE ucTPCSerialNumber;		//1
		BYTE ucTPCBoardType;		//1
		BYTE ucTPCXilinxVersion;	//1
		BYTE ucTPCPresence;			//1
		BYTE ucPSSerialNumber;		//1
		BYTE ucPSBoardVersion;		//1
		BYTE ucPSBoardType;			//1
		BYTE ucPSXilinxVersion;		//1
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	//build the text version
	char Record[64];
	sprintf(Record,"%02X%10u%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%01X%02X%02X%02X%01X%02X%02X%02X%02X",
//		psBinaryRecord->ucSize,					//1
		psBinaryRecord->ucOpcode,				//1
		psBinaryRecord->uiJulianTime,			//4
		psBinaryRecord->ucMPBID[0],				//1
		psBinaryRecord->ucMPBID[1],				//1
		psBinaryRecord->ucMPBID[2],				//1
		psBinaryRecord->ucMPBID[3],				//1
		psBinaryRecord->ucMPBID[4],				//1
		psBinaryRecord->ucMPBID[5],				//1
		psBinaryRecord->ucMPBXilinxVersion,		//1
		psBinaryRecord->ucDCMSerialNumber,		//1
		psBinaryRecord->ucDCMBoardType,			//1
		psBinaryRecord->ucDCMXilinxVersion,		//1
		psBinaryRecord->ucDCMActelVersion,		//1
		psBinaryRecord->ucDCMPresence,			//1--
		psBinaryRecord->ucTPCSerialNumber,		//1
		psBinaryRecord->ucTPCBoardType,			//1
		psBinaryRecord->ucTPCXilinxVersion,		//1
		psBinaryRecord->ucTPCPresence,			//1--
		psBinaryRecord->ucPSSerialNumber,		//1
		psBinaryRecord->ucPSBoardVersion,		//1
		psBinaryRecord->ucPSBoardType,			//1
		psBinaryRecord->ucPSXilinxVersion		//1
		);
	//add a good checksum
	AppendCheckSum(Record);

	//send it to the summary page
	UpdateSummary(Record);

	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate the formatting
	char cBuff[1024];
	if (strlen(Record) == INSTRUMENT_INFO_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		InstrumentInfoRecord(Record);

	}
	else
	{
		sprintf(cBuff,"18310 BINARY INSTRUMENT INFO RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(133,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18320 BINARY INSTRUMENT INFO RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}
	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 44 (post monitor version 4 only)
int  
CInstrument::BinaryDualCurrentModeConfigRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;					//1
		BYTE ucOpcode;					//1
		unsigned int uiJulianTime;		//4
		BYTE ucOffsetMode;				//1
		unsigned short usNominalOffsetInterval;//2
		unsigned short usMaximumOffsetInterval;//2
		BYTE ucGammaChan0GainMode;		//1
		BYTE ucGammaChan0MaxGain;		//1
		BYTE ucGammaChan0PresGainIndex;	//1
		BYTE ucGammaChan1GainMode;		//1
		BYTE ucGammaChan1MaxGain;		//1
		BYTE ucGammaChan1PresGainIndex;	//1
		float fIonChamberHVSetPoint;	//4
		BYTE ucIonChamberHVConfState;	//1
		float fGammaUncertaintyMult;	//4
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	Reverse((BYTE*)&(psBinaryRecord->usNominalOffsetInterval),2);
	Reverse((BYTE*)&(psBinaryRecord->usMaximumOffsetInterval),2);
	Reverse((BYTE*)&(psBinaryRecord->fIonChamberHVSetPoint),4);
	Reverse((BYTE*)&(psBinaryRecord->fGammaUncertaintyMult),4);
	//build the text version
	char Record[64];
	sprintf(Record,"%02X%10u%1X%3u%3u%1u%2u%2u%1u%2u%2u%#5.1f%1u%#6.3f",
		psBinaryRecord->ucOpcode,				
		psBinaryRecord->uiJulianTime,			
		psBinaryRecord->ucOffsetMode,
		psBinaryRecord->usNominalOffsetInterval,
		psBinaryRecord->usMaximumOffsetInterval,
		psBinaryRecord->ucGammaChan0GainMode,
		psBinaryRecord->ucGammaChan0MaxGain,
		psBinaryRecord->ucGammaChan0PresGainIndex,
		psBinaryRecord->ucGammaChan1GainMode,
		psBinaryRecord->ucGammaChan1MaxGain,
		psBinaryRecord->ucGammaChan1PresGainIndex,
		psBinaryRecord->fIonChamberHVSetPoint,
		psBinaryRecord->ucIonChamberHVConfState,
		psBinaryRecord->fGammaUncertaintyMult
		);

	//add a good checksum
	AppendCheckSum(Record);

	//send it to the summary page
	UpdateSummary(Record);

	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate the formatting
	char cBuff[1024];
	if (strlen(Record) == DUAL_CURRENTMODE_CONFIG_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		DualCurrentModeConfigRecord(Record);

	}
	else
	{
		sprintf(cBuff,"18410 BINARY DUAL CURRENT MODE CONFIGURATION RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(134,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18420 BINARY DUAL CURRENT MODE CONFIGURATION RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}
	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 45 (post monitor version 4 only)
int  
CInstrument::BinaryTriggerConfigRecord(BYTE *pBinaryAt)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;					//1
		BYTE ucOpcode;					//1
		unsigned int uiJulianTime;		//4
		BYTE ucTriggerPin4Logic;		//1
		BYTE ucTriggerPin4ChanType[8];	//8
		BYTE ucTriggerPin5Logic;		//1
		BYTE ucTriggerPin5ChanType[8];	//8
		BYTE ucTriggerPin6Logic;		//1
		BYTE ucTriggerPin6ChanType[8];	//8
		BYTE ucTriggerPin7Logic;		//1
		BYTE ucTriggerPin7ChanType[8];	//8
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	//build the text version
	char Record[128];
	sprintf(Record,"%02X%10u"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X"
		"%1X%02X%02X%02X%02X%02X%02X%02X%02X",
		psBinaryRecord->ucOpcode,					
		psBinaryRecord->uiJulianTime,				
		psBinaryRecord->ucTriggerPin4Logic,			
		psBinaryRecord->ucTriggerPin4ChanType[0],	
		psBinaryRecord->ucTriggerPin4ChanType[1],	
		psBinaryRecord->ucTriggerPin4ChanType[2],	
		psBinaryRecord->ucTriggerPin4ChanType[3],	
		psBinaryRecord->ucTriggerPin4ChanType[4],	
		psBinaryRecord->ucTriggerPin4ChanType[5],	
		psBinaryRecord->ucTriggerPin4ChanType[6],	
		psBinaryRecord->ucTriggerPin4ChanType[7],	
		psBinaryRecord->ucTriggerPin5Logic,			
		psBinaryRecord->ucTriggerPin5ChanType[0],	
		psBinaryRecord->ucTriggerPin5ChanType[1],	
		psBinaryRecord->ucTriggerPin5ChanType[2],	
		psBinaryRecord->ucTriggerPin5ChanType[3],	
		psBinaryRecord->ucTriggerPin5ChanType[4],	
		psBinaryRecord->ucTriggerPin5ChanType[5],	
		psBinaryRecord->ucTriggerPin5ChanType[6],	
		psBinaryRecord->ucTriggerPin5ChanType[7],	
		psBinaryRecord->ucTriggerPin6Logic,			
		psBinaryRecord->ucTriggerPin6ChanType[0],	
		psBinaryRecord->ucTriggerPin6ChanType[1],	
		psBinaryRecord->ucTriggerPin6ChanType[2],	
		psBinaryRecord->ucTriggerPin6ChanType[3],	
		psBinaryRecord->ucTriggerPin6ChanType[4],	
		psBinaryRecord->ucTriggerPin6ChanType[5],	
		psBinaryRecord->ucTriggerPin6ChanType[6],	
		psBinaryRecord->ucTriggerPin6ChanType[7],	
		psBinaryRecord->ucTriggerPin7Logic,			
		psBinaryRecord->ucTriggerPin7ChanType[0],	
		psBinaryRecord->ucTriggerPin7ChanType[1],	
		psBinaryRecord->ucTriggerPin7ChanType[2],	
		psBinaryRecord->ucTriggerPin7ChanType[3],	
		psBinaryRecord->ucTriggerPin7ChanType[4],	
		psBinaryRecord->ucTriggerPin7ChanType[5],	
		psBinaryRecord->ucTriggerPin7ChanType[6],	
		psBinaryRecord->ucTriggerPin7ChanType[7]);	

	//add a good checksum
	AppendCheckSum(Record);
	//send it to the summary page
	UpdateSummary(Record);
	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate the formatting
	char cBuff[1024];
	if (strlen(Record) == TRIGGER_CONFIG_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		TriggerConfigRecord(Record);
	}
	else
	{
		sprintf(cBuff,"18510 BINARY TRIGGER CONFIG RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(135,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18520 BINARY TRIGGER CONFIG RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}

	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 46 through 4c (post monitor version 4 only)
int  
CInstrument::BinaryChannelNConfigRecord(BYTE *pBinaryAt,short message_type)
{
#pragma pack(1)
	struct SBinaryRec {
		BYTE ucSize;					//1
		BYTE ucOpcode;					//1
		unsigned int uiJulianTime;		//4
		BYTE ucUsedForAnalysis;			//1
		BYTE ucThreshold1TypeDir;		//1
		float fThreshold1Value;			//4
		BYTE ucThreshold1EntryExitCnt;	//1
		BYTE ucThreshold2TypeDir;		//1
		float fThreshold2Value;			//4
		BYTE ucThreshold2EntryExitCnt;	//1
		float fChangingSigMultiplier;	//4
		BYTE ucChangingSigEntryCnt;		//1
		float fFilterLimit;				//4
		BYTE ucHysteresis;				//1
	} *psBinaryRecord;
#pragma pack()

	AdjustTabsTo(POST4);

	//set a record pointer to the record data
	psBinaryRecord = (SBinaryRec*)pBinaryAt;
	//Reverse the appropriate fields
	Reverse((BYTE*)&(psBinaryRecord->uiJulianTime),4);
	Reverse((BYTE*)&(psBinaryRecord->fThreshold1Value),4);
	Reverse((BYTE*)&(psBinaryRecord->fThreshold2Value),4);
	Reverse((BYTE*)&(psBinaryRecord->fChangingSigMultiplier),4);
	Reverse((BYTE*)&(psBinaryRecord->fFilterLimit),4);

	//build the text version
	char Record[64];
	sprintf(Record,"%02X%10u%1X"
						"%1X%9.1f%02X"
						"%1X%9.1f%02X"
						"%#4.1f%01X%#7.4f%3u",
		psBinaryRecord->ucOpcode,					
		psBinaryRecord->uiJulianTime,		
		psBinaryRecord->ucUsedForAnalysis,			
		psBinaryRecord->ucThreshold1TypeDir,		
		psBinaryRecord->fThreshold1Value,			
		psBinaryRecord->ucThreshold1EntryExitCnt,	
		psBinaryRecord->ucThreshold2TypeDir,		
		psBinaryRecord->fThreshold2Value,			
		psBinaryRecord->ucThreshold2EntryExitCnt,	
		psBinaryRecord->fChangingSigMultiplier,	
		psBinaryRecord->ucChangingSigEntryCnt,		
		psBinaryRecord->fFilterLimit,				
		psBinaryRecord->ucHysteresis);			

	//add a good checksum
	AppendCheckSum(Record);

	//send it to the summary page
	UpdateSummary(Record);

	//add the end of line stuff
	strcat(Record,"\r\n");

	//validate the formatting
	char cBuff[1024];
	if (strlen(Record) == CHANNELN_CONFIG_RECORD_LENGTH)
	{
		//send it to the watch window
		PostToWatch(Record);

		//send it to the text version handler
		ChannelNConfigRecord(Record,message_type);
	}
	else
	{
		sprintf(cBuff,"18610 BINARY CHANNEL CONFIG RECORD FAILED FORMAT (Formatted): %s",Record);
		if (WriteToFileEx(136,TO_PFM,TYPE_COMP,NULL,cBuff))
		{
			sprintf(cBuff,"18620 BINARY CHANNEL CONFIG RECORD FAILED FORMAT (Source   ): ");
			char cTemp[1024];
			for (int iIndex = 0; iIndex < sizeof(SBinaryRec); iIndex++)
			{
				sprintf(cTemp,"%02x ",*(pBinaryAt+iIndex));
				strcat(cBuff,cTemp);		
			}
			strcat(cBuff,"\r\n");
			WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff);
		}
	}
	//tell caller how long it was
	return sizeof(SBinaryRec);
}

//record 41 (post monitor version 4 only)
void
CInstrument::GeneralConfigRecord(char *Record)
{

	int CheckSum;
	bool bChange;
	
	if (LengthOk(Record,GENERAL_CONFIG_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);

		bool bStale = true;											//DR00406

		CString cFull(Record);
		CString cPart;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		if (m_uiLastGeneralConfigRecordTime < cjTime.TotalSeconds())	//DR00406
		{																//DR00406
			m_uiLastGeneralConfigRecordTime = cjTime.TotalSeconds();	//DR00406

			m_cGeneralConfigRecord = Record;							//DR00406

			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			SetString(GENERAL4PAGE,IDC_GENERAL4_TIME,cPart);
			CurrentSetParam(SET4_GCR_TIME,"SET4_GCR_TIME",cPart,true);	
			
			cPart = cFull.Mid(12,5);
			SetString(GENERAL4PAGE,IDC_GENERAL4_COUNTTIME,cPart);
			if (!CurrentSetParam(SET4_COUNT_TIME,"SET4_COUNT_TIME",cPart,false))
			{
				WriteToFileEx(137,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18101 SETUP FAILURE - COUNT_TIME (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_COUNTTIME,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;//turn on yellow
			}
			//Ascii::411661355073    1141234.095FC5A904252015 3    5 1196462A25030 72040 5 304.10474B4A
			//Binar::411661355373    1141234.095FC5A904252015 3    5 1196462A25030 72040 5 304.10474B4d
			cPart = cFull.Mid(17,1);
			if (!CurrentSetParam(SET4_CONF_STATE,"SET4_CONF_STATE",cPart,false))
				WriteToFileEx(138,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18103 SETUP FAILURE - CONF_STATE (41)\r\n");

			cPart = cFull.Mid(18,1);
	//cPart = 'x';
			if (!CurrentSetParam(SET4_BIAS_TYPE,"SET4_BIAS_TYPE",cPart,false))
				WriteToFileEx(139,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18102 SETUP FAILURE - BIAS_TYPE (41)\r\n");

			cPart = cFull.Mid(19,6);
			SetString(GENERAL4PAGE,IDC_GENERAL4_FCHVSETPOINT,cPart);
			if (!CurrentSetParam(SET4_BIAS_SETPOINT,"SET4_BIAS_SETPOINT",cPart,false))
			{
				WriteToFileEx(140,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18104 SETUP FAILURE - BIAS_SETPOINT (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_FCHVSETPOINT,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(25,1);
			CurrentSetParam(SET4_BAUD_RATE,"SET4_BAUD_RATE",cPart,false);

			cPart = cFull.Mid(26,1);
			CurrentSetParam(SET4_FRAME,"SET4_FRAME",cPart,false);

			cPart = cFull.Mid(27,2);
			SetString(GENERAL4PAGE,IDC_GENERAL4_MODE,cPart);
			if (!CurrentSetParam(SET4_MODE_FLAGS1,"SET4_MODE_FLAGS1",cPart,false))
			{
				WriteToFileEx(141,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18105 SETUP FAILURE - MODE_FLAGS1 (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_MODE,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(29,2);
			SetString(GENERAL4PAGE,IDC_GENERAL4_MODE2,cPart);
			if (!CurrentSetParam(SET4_MODE_FLAGS2,"SET4_MODE_FLAGS2",cPart,false))
			{
				WriteToFileEx(142,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18106 SETUP FAILURE - MODE_FLAGS2 (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_MODE2,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(31,2);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTINT,cPart);
			if (!CurrentSetParam(SET4_BATT_INTER,"SET4_BATT_INTER",cPart,false))
			{
				WriteToFileEx(143,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18107 SETUP FAILURE - BATT_INTER (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTINT,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(33,1);
			if (!CurrentSetParam(SET4_FILTER_CONT,"SET4_FILTER_CONT",cPart,false))
				WriteToFileEx(144,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18108 SETUP FAILURE - FILTER_CONT (41)\r\n");

			cPart = cFull.Mid(34,2);
			if (!CurrentSetParam(SET4_IMMBUFF_SZ,"SET4_IMMBUFF_SZ",cPart,false))
				WriteToFileEx(145,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18109 SETUP FAILURE - IMMBUFF_SZ (41)\r\n");

			cPart = cFull.Mid(36,2);
			if (!CurrentSetParam(SET4_IMMBUFFSAVE_SZ,"SET4_IMMBUFFSAVE_SZ",cPart,false))
				WriteToFileEx(146,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18110 SETUP FAILURE - IMMBUFFSAVE_SZ (41)\r\n");

			cPart = cFull.Mid(38,2);
			if (!CurrentSetParam(SET4_LOCBG_SZ,"SET4_LOCBG_SZ",cPart,false))
				WriteToFileEx(147,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18111 SETUP FAILURE - LOCBG_SZ (41)\r\n");

			cPart = cFull.Mid(40,2);
			if (!CurrentSetParam(SET4_LOCBG_END,"SET4_LOCBG_END",cPart,false))
				WriteToFileEx(148,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18112 SETUP FAILURE - LOCBG_END (41)\r\n");

			cPart = cFull.Mid(42,5);
			SetString(GENERAL4PAGE,IDC_GENERAL4_STATINT,cPart);
			if (!CurrentSetParam(SET4_STATUS_INT,"SET4_STATUS_INT",cPart,false))
			{
				WriteToFileEx(149,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18113 SETUP FAILURE - STATUS_INT (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_STATINT,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(47,7);
			if (!CurrentSetParam(SET4_INSP_ID,"SET4_INSP_ID",cPart,false))
				WriteToFileEx(150,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18114 SETUP FAILURE - INST_ID (41)\r\n");

			cPart = cFull.Mid(54,2);
			if (!CurrentSetParam(SET4_UNIT_ID,"SET4_UNIT_ID",cPart,false))
				WriteToFileEx(151,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18115 SETUP FAILURE - UNIT_ID (41)\r\n");

			cPart = cFull.Mid(56,1);
			SetString(GENERAL4PAGE,IDC_GENERAL4_TSSTATE,cPart);
			if (!CurrentSetParam(SET4_TIMESYNC_STATE,"SET4_TIMESYNC_STATE",cPart,false))
			{
				WriteToFileEx(152,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18116 SETUP FAILURE - TIMESYNC_STATE (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_TSSTATE,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(57,4);
			SetString(GENERAL4PAGE,IDC_GENERAL4_TSHOURLY,cPart);
			if (!CurrentSetParam(SET4_TIMESYNC_HOUR,"SET4_TIMESYNC_HOUR",cPart,false))
			{
				WriteToFileEx(153,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18117 SETUP FAILURE - TIMESYNC_HOUR (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_TSHOURLY,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(61,6);
			SetString(GENERAL4PAGE,IDC_GENERAL4_TSDAILY,cPart);
			if (!CurrentSetParam(SET4_TIMESYNC_DAILY,"SET4_TIMESYNC_DAILY",cPart,false))
			{
				WriteToFileEx(154,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18118 SETUP FAILURE - TIMESYNC_DAILY (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_TSDAILY,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(67,2);
			SetString(GENERAL4PAGE,IDC_GENERAL4_TSLOWER,cPart);
			if (!CurrentSetParam(SET4_TIMESYNC_LOWTOL,"SET4_TIMESYNC_LOWTOL",cPart,false))
			{
				WriteToFileEx(155,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18119 SETUP FAILURE - TIMESYNC_LOWTOL (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_TSLOWER,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			cPart = cFull.Mid(69,2);
			SetString(GENERAL4PAGE,IDC_GENERAL4_TSUPPER,cPart);
			if (!CurrentSetParam(SET4_TIMESYNC_UPPTOL,"SET4_TIMESYNC_UPPTOL",cPart,false))
			{
				WriteToFileEx(156,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18120 SETUP FAILURE - TIMESYNC_UPPTOL (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_TSUPPER,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}
			
			cPart = cFull.Mid(71,5);
			SetString(GENERAL4PAGE,IDC_GENERAL4_GRANDP,cPart);
			if (!CurrentSetParam(SET4_FIRMWARE_VERSION,"SET4_FIRMWARE_VERSION",cPart,false))
			{
				WriteToFileEx(157,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18121 SETUP FAILURE - FIRMWARE_VERSION (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GRANDP,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}
			
			cPart = cFull.Mid(76,4);
			SetString(GENERAL4PAGE,IDC_GENERAL4_ROM,cPart);
			if (!CurrentSetParam(SET4_FIRMWARE_CHKSUM,"SET4_FIRMWARE_CHKSUM",cPart,false))
			{
				WriteToFileEx(158,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18122 SETUP FAILURE - FIRMWARE_CHKSUM (41)\r\n");
				SetColor(GENERAL4PAGE,IDC_GENERAL4_ROM,COLOR_YELLOW);
				bChange = true;
				m_iGeneral4_Tab |= 0x0002;
			}

			if (m_pPropertySheet)
				m_pPropertySheet->GeneralConfigRecord(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"18100 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);

	}
	//m_pParent->PostMessage(IDC_RECEIVE_RESULT,GENERAL_CONFIG_RECORD,CheckSum);
	//PJM 4/27/2005
	PostMessage(IDC_RECEIVE_RESULT,GENERAL_CONFIG_RECORD,CheckSum);	
}


//record 42 (post monitor version 4 only)
void
CInstrument::InstrumentStatusRecord(char *Record)
{

	int CheckSum;
	
	if (LengthOk(Record,INSTRUMENT_STATUS_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);

		bool bStale = true;											//DR00406

		CString cFull(Record);
		CString cPart;
		char temp[32];
		bool bChange = false;

		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;

		//julian time
		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		if (m_uiLastInstrumentStatusRecordTime < cjTime.TotalSeconds())	//DR00406
		{																//DR00406
			m_uiLastInstrumentStatusRecordTime = cjTime.TotalSeconds();	//DR00406

			m_cInstrumentStatusRecord = Record;

			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			CurrentSetParam(SET4_ISR_TIME,"SET4_ISR_TIME",cPart,true);	
			SetString(GENERAL4PAGE,IDC_GENERAL4_ISRTIME,cPart);

			//battery voltage
			cPart = cFull.Mid(12,4);
			CurrentSetParam(SET4_BAT_VOLT,"SET4_BAT_VOLT",cPart,true);
			m_fCurBV=(float)atof(cPart);
			CurrentRed = false;
			CurrentLow = false;
			CurrentHigh = false;
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV <= m_fLowBV) 
			{
				m_fLowBV = m_fCurBV;
				CurrentLow = true;
			}
			if (m_fCurBV >= m_fHighBV)
			{
				m_fHighBV= m_fCurBV;
				CurrentHigh= true;
			}

			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				CurrentRed = true;
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryTolOk)
				{
					WriteToFileEx(159,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16360 STATUS CHANGE - BATTERY OUT OF TOLERANCE (42)\r\n");
					FlagOthr(1,true);
				}
				m_bBatteryTolOk = false;
				m_iGeneral4_Tab |= 0x0001;
				bChange = true;
			}
			else
			{
				CurrentRed = false;
				if (!m_bBatteryTolOk)
					WriteToFileEx(159,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16370 STATUS CHANGE - BATTERY IN TOLERANCE (42)\r\n");
				m_bBatteryTolOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowBV <= m_fBatVoltL))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighBV >= m_fBatVoltH))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,COLOR_RED);
			}
			
			sprintf(temp,"%5.1f",m_fLowBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,temp);
			sprintf(temp,"%5.1f",m_fCurBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,temp);
			sprintf(temp,"%5.1f",m_fHighBV);
			SetString(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,temp);

			//+5 volts
			cPart = cFull.Mid(16,4);
			CurrentSetParam(SET4_P5READ_VOLT,"SET4_P5READ_VOLT",cPart,true);	
			m_fCurP5=(float)atof(cPart);
			CurrentLow = false;
			CurrentHigh = false;
			CurrentRed = false;
			if (m_bFirstP5)
			{
				m_bFirstP5 = false;
				m_fLowP5 = m_fHighP5 = m_fCurP5;
			}
			if (m_fCurP5 <= m_fLowP5)
			{
				CurrentLow = true;
				m_fLowP5 = m_fCurP5;
			}
			if (m_fCurP5 >= m_fHighP5)
			{
				CurrentHigh = true;
				m_fHighP5= m_fCurP5;
			}
			
			if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			{
				CurrentRed = true;			
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P5VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,COLOR_RED);
				if (m_bP5VoltOk)
				{
					WriteToFileEx(160,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16380 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (42)\r\n");
					FlagOthr(1,true);
				}
				m_bP5VoltOk = false;
				m_iGeneral4_Tab |= 0x0001;
				bChange = true;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP5VoltOk)
					WriteToFileEx(160,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16390 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (42)\r\n");
				m_bP5VoltOk = true;
			}

			if (CurrentRed && CurrentLow && (m_fLowP5 <= m_fSup_P05L))// || (m_fLowP5 >= m_fSup_P05H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighP5 >= m_fSup_P05L))// || (m_fHighP5 >= m_fSup_P05H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P5VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighP5);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMAX,temp);

			//plus 12 volts
			cPart = cFull.Mid(20,4);
			CurrentSetParam(SET4_P12READ_VOLT,"SET4_P12READ_VOLT",cPart,true);	
			//+15 volts
			//strncpy(temp,&Record[49],4);
			//temp[4] = NULL;
			CurrentRed = false;
			CurrentLow = false;
			CurrentHigh = false;
			m_fCurP15=(float)atof(cPart);
			if (m_bFirstP15)
			{
				m_bFirstP15 = false;
				m_fLowP15 = m_fHighP15 = m_fCurP15;
			}
			if (m_fCurP15 <= m_fLowP15)
			{
				CurrentLow = true;
				m_fLowP15 = m_fCurP15;
			}
			if (m_fCurP15 >= m_fHighP15)
			{
				CurrentHigh = true;
				m_fHighP15= m_fCurP15;
			}

			if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
			{
				CurrentRed = true;			
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,COLOR_RED);
				if (m_bP15VoltOk)
				{
					WriteToFileEx(161,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16400 STATUS CHANGE - PLUS 15 VOLT SUPPLY OUT OF TOLERANCE (42)\r\n");
					FlagOthr(1,true);
				}
				m_bP15VoltOk = false;
				m_iGeneral4_Tab |= 0x0001;
				bChange = true;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP15VoltOk)
					WriteToFileEx(161,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16410 STATUS CHANGE - PLUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (42)\r\n");
				m_bP15VoltOk = true;
				////m_iGeneral4_Tab &= ~(0x0001);
			}

			if (CurrentRed && CurrentLow &&	(m_fLowP15 <= m_fSup_P15L))// || (m_fLowP15 >= m_fSup_P15H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighP15 >= m_fSup_P15L))// || (m_fHighP15 >= m_fSup_P15H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P15VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighP15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMAX,temp);

			//-12 volts
			cPart = cFull.Mid(24,5);
			CurrentSetParam(SET4_M12READ_VOLT,"SET4_M12READ_VOLT",cPart,true);	
			m_fCurM15=(float)atof(cPart);
			CurrentRed = false;
			CurrentHigh = false;
			CurrentLow = false;
			if (m_bFirstM15)
			{
				m_bFirstM15 = false;
				m_fLowM15 = m_fHighM15 = m_fCurM15;
			}
			if (m_fCurM15 <= m_fLowM15)
			{
				CurrentLow = true;
				m_fLowM15 = m_fCurM15;
			}
			if (m_fCurM15 >= m_fHighM15)
			{
				CurrentHigh = true;
				m_fHighM15= m_fCurM15;
			}
			if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
			{
				CurrentRed = true;
				SetColor(GENERAL4PAGE,IDC_GENERAL4_M15VOLTCUR,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,COLOR_RED);
				if (m_bM15VoltOk)
				{
					WriteToFileEx(162,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16420 STATUS CHANGE - MINUS 15 VOLT SUPPLY OUT OF TOLERANCE (42)\r\n");
					FlagOthr(1,true);
				}
				m_bM15VoltOk = false;
				m_iGeneral4_Tab |= 0x0001;
			}
			else
			{
				CurrentRed = false;
				if (!m_bM15VoltOk)
					WriteToFileEx(162,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "16430 STATUS CHANGE - MINUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (42)\r\n");
				m_bM15VoltOk = true;
				////m_iGeneral_Tab &= ~(0x0080);
			}

			if (CurrentRed && CurrentLow && (m_fLowM15 <= m_fSup_M15L))// || (m_fLowM15 >= m_fSup_M15H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMIN,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,COLOR_RED);
			}

			if (CurrentRed && CurrentHigh && (m_fHighM15 >= m_fSup_M15H))// || (m_fHighM15 >= m_fSup_M15H))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMAX,COLOR_RED);
				SetColor(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMIN,temp);
			sprintf(temp,"%5.1f",m_fCurM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_M15VOLTCUR,temp);
			sprintf(temp,"%5.1f",m_fHighM15);
			SetString(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp);
			SetString(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMAX,temp);
	////should be displayed?**************************************************************
			//ac power status
			cPart = cFull.Mid(29,1);
			CurrentSetParam(SET4_ACPOWER_STAT,"SET4_ACPOWER_STAT",cPart,false);	

			//bias read voltage
			cPart = cFull.Mid(30,6);
			CurrentSetParam(SET4_BIASREAD_VOLT,"SET4_BIASREAD_VOLT",cPart,true);	

			float fCurrent;
			float fCurrentSet;
			float fFCHVTol;
			fFCHVTol = (float)GetPrivateProfileInt(m_pName,"FCHVERR",5,m_szIniFile);
			fFCHVTol = fFCHVTol / 100.0f;
			//CString cTemp;
			GetPrivateProfileString(m_pName,"SET4_BIAS_SETPOINT","",temp,sizeof(temp),m_szIniFile);
			fCurrent = (float)atof(cPart);
			if (strlen(temp))
				fCurrentSet = (float)atof(temp);
			else
				fCurrentSet = fCurrent;
			if (m_bFBFirst)
			{
				m_fFBHigh = m_fFBLow = fCurrent;
				m_bFBFirst = false;
			}
			if (fCurrent < m_fFBLow) m_fFBLow = fCurrent;
			if (fCurrent > m_fFBHigh) m_fFBHigh = fCurrent;
			sprintf(temp,"%6.1f",m_fFBLow);
			SetString(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMIN,temp);

			sprintf(temp,"%6.1f",fCurrent);
			SetString(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVCUR,temp);

			//if ((fCurrent < (1.0-fFCHVTol)*fCurrentSet) || (fCurrent > (1.0+fFCHVTol)*fCurrentSet))

			if (((fCurrent < (1.0-fFCHVTol)*fCurrentSet) || 
				 (fCurrent > (1.0+fFCHVTol)*fCurrentSet)   )
				&&(fCurrentSet > 50.0))						//dgp 16 dec 2004 added > 50.0 1.9.0.8
			{
				if (!m_bFCHVError)
				{
					CString cTemp;
					cTemp.Format("18201 STATUS CHANGE - FC HV > %2.0f%% ERROR (42)\r\n",fFCHVTol*100.0);
					WriteToFileEx(163,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				m_bFCHVError = true;
				SetColor(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVCUR,COLOR_RED);
				m_iGeneral4_Tab |= 0x0001;
				bChange = true;
			}
			else
			{
				if (m_bFCHVError)
				{
					CString cTemp;
					cTemp.Format("18202 STATUS CHANGE - FC HV < %2.0f%% ERROR (42)\r\n",fFCHVTol*100.0);
					WriteToFileEx(163,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				m_bFCHVError = false;
			}
				
			sprintf(temp,"%6.1f",m_fFBHigh);
			SetString(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMAX,temp);

			//ic read voltage
			cPart = cFull.Mid(36,5);
			CurrentSetParam(SET4_IONREAD_VOLT,"SET4_IONREAD_VOLT",cPart,true);
			float fICHVTol;
			fICHVTol = (float)GetPrivateProfileInt(m_pName,"ICHVERR",10,m_szIniFile);
			fICHVTol = fICHVTol / 100.0f;

			GetPrivateProfileString(m_pName,"SET4_ICHV_SETPOINT","",temp,sizeof(temp),m_szIniFile);
	//		SetString(GENERAL4PAGE,IDC_GENERAL4_ICHVSETPOINT,temp);
			fCurrent = (float)atof(cPart);
			if (strlen(temp))
				fCurrentSet = (float)atof(temp);
			else
				fCurrentSet = fCurrent;
			if (m_dDIRFirst)
			{
				m_fDIRHigh = m_fDIRLow = fCurrent;
				m_dDIRFirst = false;
			}
			if (fCurrent < m_fDIRLow) m_fDIRLow = fCurrent;
			if (fCurrent > m_fDIRHigh) m_fDIRHigh = fCurrent;
			sprintf(temp,"%6.1f",m_fDIRLow);
			SetString(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMIN,temp);

			sprintf(temp,"%6.1f",fCurrent);
			SetString(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR,temp);
			//if ((fCurrent < (1.0-fICHVTol)*fCurrentSet) || (fCurrent > (1.0+fICHVTol)*fCurrentSet))

			if (((fCurrent < (1.0-fICHVTol)*fCurrentSet) || 
				 (fCurrent > (1.0+fICHVTol)*fCurrentSet)   )
				 && (fCurrentSet > 50))							//dgp 16 dec 2004 added > 50.0 1.9.0.8
			{
				if (!m_bICHVError)
				{
					CString cTemp;
					cTemp.Format("18203 STATUS CHANGE - IC HV > %2.0f%% ERROR (42)\r\n",fICHVTol*100.0);
					WriteToFileEx(164,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				m_bICHVError = true;
				SetColor(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR,COLOR_RED);
				m_iGeneral4_Tab |= 0x0001;
				bChange = true;
			}
			else
			{
				if (m_bICHVError)
				{
					CString cTemp;
					cTemp.Format("18204 STATUS CHANGE - IC HV < %2.0f%% ERROR (42)\r\n",fICHVTol*100.0);
					WriteToFileEx(164,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				m_bICHVError = false;
			}

			sprintf(temp,"%6.1f",m_fDIRHigh);
			SetString(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMAX,temp);

			//on board temperature
			cPart = cFull.Mid(41,5);
			CurrentSetParam(SET4_ONBOARDREAD_TEMP,"SET4_ONBOARDREAD_TEMP",cPart,true);	

			cPart = cFull.Mid(46,5);
			CurrentSetParam(SET4_OFFBOARDREAD_TEMP,"SET4_OFFBOARDREAD_TEMP",cPart,true);	

			if (m_pPropertySheet)
				m_pPropertySheet->InstrumentStatusRecord(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"18200 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff, bStale);			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}

	PostMessage(IDC_RECEIVE_RESULT,INSTRUMENT_STATUS_RECORD,CheckSum);	
}

//record 43 (post monitor version 4 only)
void
CInstrument::InstrumentInfoRecord(char *Record)
{

	int CheckSum;
	if (LengthOk(Record,INSTRUMENT_INFO_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);

		bool bStale = true;											//DR00406

		CString cFull(Record);
		CString cPart;
		bool bChange = false;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		if (m_uiLastInstrumentInfoRecordTime < cjTime.TotalSeconds())	//DR00406
		{																//DR00406
			m_uiLastInstrumentInfoRecordTime = cjTime.TotalSeconds();	//DR00406

			m_cInstrumentInfoRecord = Record;

			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			CurrentSetParam(SET4_IIR_TIME,"SET4_IIR_TIME",cPart,true);	

			cPart = cFull.Mid(12,12);
			CurrentSetParam(SET4_MPB_ID,"SET4_MPB_ID",cPart,false);	

			cPart = cFull.Mid(24,2);
			CurrentSetParam(SET4_MPBXILINX_VER,"SET4_MPBXILINX_VER",cPart,false);	

			cPart = cFull.Mid(26,2);
			CurrentSetParam(SET4_DCMSER_NUM,"SET4_DCMSER_NUM",cPart,false);	

			cPart = cFull.Mid(28,2);
			CurrentSetParam(SET4_DCMBRD_TYPE,"SET4_DCMBRD_TYPE",cPart,false);	

			cPart = cFull.Mid(30,2);
			CurrentSetParam(SET4_DCMXILINX_VER,"SET4_DCMXILINX_VER",cPart,false);	

			cPart = cFull.Mid(32,2);
			CurrentSetParam(SET4_DCMACTEL_VER,"SET4_DCMACTEL_VER",cPart,false);	

			cPart = cFull.Mid(34,1);
			CurrentSetParam(SET4_DCMPRESENCE,"SET4_DCMPRESENCE",cPart,false);	

			cPart = cFull.Mid(35,2);
			CurrentSetParam(SET4_TPCSER_NUM,"SET4_TPCSER_NUM",cPart,false);	

			cPart = cFull.Mid(37,2);
			CurrentSetParam(SET4_TPCBRD_TYPE,"SET4_TPCBRD_TYPE",cPart,false);	

			cPart = cFull.Mid(39,2);
			CurrentSetParam(SET4_TPCXILINX_VER,"SET4_TPCXILINX_VER",cPart,false);	

			cPart = cFull.Mid(41,1);
			CurrentSetParam(SET4_TPCPRESENCE,"SET4_TPCPRESENCE",cPart,false);	

			cPart = cFull.Mid(42,2);
			CurrentSetParam(SET4_PSSER_NUM,"SET4_PSSER_NUM",cPart,false);	

			cPart = cFull.Mid(44,2);
			CurrentSetParam(SET4_PSBRD_VER,"SET4_PSBRD_VER",cPart,false);	

			cPart = cFull.Mid(46,2);
			CurrentSetParam(SET4_PSBRD_TYPE,"SET4_PSBRD_TYPE",cPart,false);	

			cPart = cFull.Mid(48,2);
			CurrentSetParam(SET4_PSXILINX_VER,"SET4_PSXILINX_VER",cPart,false);	

			if (m_pPropertySheet)
				m_pPropertySheet->InstrumentInfoRecord(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"18300 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}

	PostMessage(IDC_RECEIVE_RESULT,INSTRUMENT_INFO_RECORD,CheckSum);	
}

//record 44 (post monitor version 4 only)
void
CInstrument::DualCurrentModeConfigRecord(char *Record)
{

	int CheckSum;
	
	if (LengthOk(Record,DUAL_CURRENTMODE_CONFIG_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		AdjustTabsTo(POST4);

		bool bStale = true;											//DR00406

		CString cFull(Record);
		CString cPart;

		bool bChange = false;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		if (m_uiLastDualCurrentModeConfigRecordTime < cjTime.TotalSeconds())			//DR00406
		{															//DR00406
			m_uiLastDualCurrentModeConfigRecordTime = cjTime.TotalSeconds();			//DR00406

			m_cDualCurrentModeConfigRecord = Record;


			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			CurrentSetParam(SET4_DCMCR_TIME,"SET4_DCMCR_TIME",cPart,true);	
			SetString(GENERAL4PAGE,IDC_GENERAL4_G1DATE,cjTime.Date());
			SetString(GENERAL4PAGE,IDC_GENERAL4_G2DATE,cjTime.Time());

			cPart = cFull.Mid(12,1);
			if (!CurrentSetParam(SET4_OFFSET_MODE,"SET4_OFFSET_MODE",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_DISCM,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(165,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18401 SETUP FAILURE - OFFSET_MODE (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_DISCM,cPart);
			
			cPart = cFull.Mid(13,3);
			if (!CurrentSetParam(SET4_NOMOS_INTERVAL,"SET4_NOMOS_INTERVAL",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_G1TIME,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(166,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18402 SETUP FAILURE - NOM_OFFSET_INTERVAL (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_G1TIME,cPart);

			cPart = cFull.Mid(16,3);
			if (!CurrentSetParam(SET4_MAXOS_INTERVAL,"SET4_MAXOS_INTERVAL",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_G2TIME,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(167,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18403 SETUP FAILURE - MAX_OFFSET_INTERVAL (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_G2TIME,cPart);

			cPart = cFull.Mid(19,1);
			bool bG0GMAuto = cPart == '0';// SCR 526 
			if (!CurrentSetParam(SET4_G0_GAINMODE,"SET4_G0_GAINMODE",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GAMMA1G,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(168,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18404 SETUP FAILURE - GAMMA0_GAIN_MODE (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_GAMMA1G,cPart);

			cPart = cFull.Mid(20,2);
			if (!CurrentSetParam(SET4_G0_MAXFIX,"SET4_G0_MAXFIX",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GAMMA1M,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(169,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18405 SETUP FAILURE - GAMMA0_MAX_FIX (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_GAMMA1M,cPart);

			cPart = cFull.Mid(22,2);
			if (!CurrentSetParam(SET4_G0_PRES,"SET4_G0_PRES",cPart,bG0GMAuto))// SCR 526
			{
				if (!bG0GMAuto)// SCR 526
				{
					SetColor(GENERAL4PAGE,IDC_GENERAL4_MAXG1,COLOR_YELLOW);
					m_iGeneral4_Tab |= 0x0002;
					WriteToFileEx(170,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18406 SETUP FAILURE - GAMMA1_PRESGAIN1 (44)\r\n");
				}
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_MAXG1,cPart);

			cPart = cFull.Mid(24,1);
			bool bG1GMAuto = cPart == '0';// SCR 526
			if (!CurrentSetParam(SET4_G1_GAINMODE,"SET4_G1_GAINMODE",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GAMMA2G,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(170,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18406 SETUP FAILURE - GAMMA1_GAIN_MODE (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_GAMMA2G,cPart);

			cPart = cFull.Mid(25,2);
			if (!CurrentSetParam(SET4_G1_MAXFIX,"SET4_G1_MAXFIX",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GAMMA2M,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(171,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18407 SETUP FAILURE - GAMMA1_MAX_FIX (44)\r\n");
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_GAMMA2M,cPart);

			cPart = cFull.Mid(27,2);
			if (!CurrentSetParam(SET4_G1_PRES,"SET4_G1_PRES",cPart,bG1GMAuto))// SCR 526
			{
				if (!bG1GMAuto)// SCR 526
				{
					SetColor(GENERAL4PAGE,IDC_GENERAL4_MAXG2,COLOR_YELLOW);
					m_iGeneral4_Tab |= 0x0002;
					WriteToFileEx(170,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18406 SETUP FAILURE - GAMMA1_PRESGAIN2 (44)\r\n");
				}
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_MAXG2,cPart);

			CString cTemp;
			cPart = cFull.Mid(29,5);
			float fICHVErr;
			fICHVErr = (float)GetPrivateProfileInt(m_pName,"ICHVERR",10,m_szIniFile);
			fICHVErr = fICHVErr/100.0f;
			if (!CurrentSetParam(SET4_ICHV_SETPOINT,"SET4_ICHV_SETPOINT",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_ICHVSETPOINT,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
				WriteToFileEx(172,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18410 SETUP FAILURE - IC_HV_SETPOINT (44)\r\n");
			}

			SetString(GENERAL4PAGE,IDC_GENERAL4_ICHVSETPOINT,cPart);
			cTemp = GetString(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR);
			if (cTemp.GetLength())
			{
				double dCurrent = atof(cTemp);
				double dCurrentSet = atof(cPart);
				if ((dCurrent < (1.0-fICHVErr)*dCurrentSet) || (dCurrent > (1.0+fICHVErr)*dCurrentSet))
				{
					SetColor(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR,COLOR_RED);
					m_iGeneral4_Tab |= 0x0001;
					bChange = true;
					if (!m_bICHVError)
					{
						CString cTemp;
						cTemp.Format("18408 STATUS CHANGE - IC HV > %2.0f%% ERROR (44)\r\n",fICHVErr*100.0);
						WriteToFileEx(173,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
						m_bICHVError = true;
					}
				}
				else
				{
					if (m_bICHVError)
					{
						CString cTemp;
						cTemp.Format("18409 STATUS CHANGE - IC HV < %2.0f%% ERROR (44)\r\n",fICHVErr*100.0);
						WriteToFileEx(173,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
						m_bICHVError = false;
					}
				}
			}

			cPart = cFull.Mid(34,1);
			if (!CurrentSetParam(SET4_ICHV_CONFIG,"SET4_ICHV_CONFIG",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_HVCONFIG,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_HVCONFIG,cPart);

			cPart = cFull.Mid(35,6);
			if (!CurrentSetParam(SET4_GUNCERT_MULT,"SET4_GUNCERT_MULT",cPart,false))
			{
				SetColor(GENERAL4PAGE,IDC_GENERAL4_GAMMAUNMULT,COLOR_YELLOW);
				m_iGeneral4_Tab |= 0x0002;
			}
			SetString(GENERAL4PAGE,IDC_GENERAL4_GAMMAUNMULT,cPart);

			if (m_pPropertySheet)
				m_pPropertySheet->DualCurrentModeConfigRecord(Record);

			bStale = false;											//DR00406
		}															//DR00406
	
		char cBuff[1024];
		strcpy(cBuff,"18400 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff, bStale);			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);

	}
	PostMessage(IDC_RECEIVE_RESULT,DUAL_CURRENTMODE_CONFIG_RECORD,CheckSum);	
}

//record 45 (post monitor version 4 only)
void
CInstrument::TriggerConfigRecord(char *Record)
{

	int CheckSum;

	if (LengthOk(Record,TRIGGER_CONFIG_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		AdjustTabsTo(POST4);

		bool bStale = true;											//DR00406

		bool bChange = false;
		bool bDoYellow = false;
	
		CString cFull(Record);
		CString cPart;
		CString cPart2;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		if (m_uiLastTriggerConfigRecordTime < cjTime.TotalSeconds())	//DR00406
		{																//DR00406
			m_uiLastTriggerConfigRecordTime = cjTime.TotalSeconds();	//DR00406

			m_cTriggerConfigRecord = Record;

			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			CurrentSetParam(SET4_TCR_TIME,"SET4_TCR_TIME",cPart,true);
			SetString(USER4PAGE,IDC_USER4PROG_TIME,cPart);

			cPart = cFull.Mid(12,1);
			if (!CurrentSetParam(SET4_TRIGGER4_LOGIC,"SET4_TRIGGER4_LOGIC",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;
				SetColor(USER4PAGE,IDC_USER4PROG_LOGIC1,COLOR_YELLOW);
				WriteToFileEx(174,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18501 SETUP FAILURE - TRIGGER_4_LOGIC (45)\r\n");
			}
			SetString(USER4PAGE,IDC_USER4PROG_LOGIC1,cPart);
			
			cPart = cFull.Mid(13,16);
			if (!CurrentSetParam(SET4_TRIGGER4_CHTYPE,"SET4_TRIGGER4_CHTYPE",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;

				while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
				WriteToFileEx(175,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18502 SETUP FAILURE - TRIGGER_4_CHAN_TYPE (45)\r\n");
			}
			else
				bDoYellow = false;

			cPart2 = cPart[0];
			cPart2 += "/";
			cPart2 += cPart[1];
			if (bDoYellow)
				if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE1,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE1,cPart2);

			cPart2 = cPart[2];
			cPart2 += "/";
			cPart2 += cPart[3];
			if (bDoYellow)
				if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE2,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE2,cPart2);

			cPart2 = cPart[4];
			cPart2 += "/";
			cPart2 += cPart[5];
			if (bDoYellow)
				if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE3,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE3,cPart2);

			cPart2 = cPart[6];
			cPart2 += "/";
			cPart2 += cPart[7];
			if (bDoYellow)
				if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE4,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE4,cPart2);

			cPart2 = cPart[8];
			cPart2 += "/";
			cPart2 += cPart[9];
			if (bDoYellow)
				if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE5,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE5,cPart2);

			cPart2 = cPart[10];
			cPart2 += "/";
			cPart2 += cPart[11];
			if (bDoYellow)
				if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE6,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE6,cPart2);

			cPart2 = cPart[12];
			cPart2 += "/";
			cPart2 += cPart[13];
			if (bDoYellow)
				if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE7,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE7,cPart2);

			cPart2 = cPart[14];
			cPart2 += "/";
			cPart2 += cPart[15];
			if (bDoYellow)
				if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
					SetColor(USER4PAGE,IDC_USER4PROG_1CHTYPE8,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_1CHTYPE8,cPart2);
			
			cPart = cFull.Mid(29,1);
			if (!CurrentSetParam(SET4_TRIGGER5_LOGIC,"SET4_TRIGGER5_LOGIC",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;
				SetColor(USER4PAGE,IDC_USER4PROG_LOGIC2,COLOR_YELLOW);
				WriteToFileEx(176,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18503 SETUP FAILURE - TRIGGER_5_LOGIC (45)\r\n");
			}
			else
				bDoYellow = false;
			SetString(USER4PAGE,IDC_USER4PROG_LOGIC2,cPart);
			
			cPart = cFull.Mid(30,16);
			if (!CurrentSetParam(SET4_TRIGGER5_CHTYPE,"SET4_TRIGGER5_CHTYPE",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;

				while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
				WriteToFileEx(177,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18504 SETUP FAILURE - TRIGGER_5_CHAN_TYPE (45)\r\n");
			}
			else
				bDoYellow = false;
			cPart2 = cPart[0];
			cPart2 += "/";
			cPart2 += cPart[1];
			if (bDoYellow)
				if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE1,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE1,cPart2);

			cPart2 = cPart[2];
			cPart2 += "/";
			cPart2 += cPart[3];
			if (bDoYellow)
				if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE2,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE2,cPart2);

			cPart2 = cPart[4];
			cPart2 += "/";
			cPart2 += cPart[5];
			if (bDoYellow)
				if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE3,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE3,cPart2);

			cPart2 = cPart[6];
			cPart2 += "/";
			cPart2 += cPart[7];
			if (bDoYellow)
				if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE4,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE4,cPart2);

			cPart2 = cPart[8];
			cPart2 += "/";
			cPart2 += cPart[9];
			if (bDoYellow)
				if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE5,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE5,cPart2);

			cPart2 = cPart[10];
			cPart2 += "/";
			cPart2 += cPart[11];
			if (bDoYellow)
				if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE6,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE6,cPart2);

			cPart2 = cPart[12];
			cPart2 += "/";
			cPart2 += cPart[13];
			if (bDoYellow)
				if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE7,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE7,cPart2);

			cPart2 = cPart[14];
			cPart2 += "/";
			cPart2 += cPart[15];
			if (bDoYellow)
				if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
					SetColor(USER4PAGE,IDC_USER4PROG_2CHTYPE8,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_2CHTYPE8,cPart2);
			
			cPart = cFull.Mid(46,1);
			if (!CurrentSetParam(SET4_TRIGGER6_LOGIC,"SET4_TRIGGER6_LOGIC",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;
				SetColor(USER4PAGE,IDC_USER4PROG_LOGIC3,COLOR_YELLOW);
				WriteToFileEx(178,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18505 SETUP FAILURE - TRIGGER_6_LOGIC (45)\r\n");
			}
			else
				bDoYellow = false;
			SetString(USER4PAGE,IDC_USER4PROG_LOGIC3,cPart);
			
			cPart = cFull.Mid(47,16);
			if (!CurrentSetParam(SET4_TRIGGER6_CHTYPE,"SET4_TRIGGER6_CHTYPE",cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				bChange = true;
				bDoYellow = true;
				while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
				WriteToFileEx(179,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18506 SETUP FAILURE - TRIGGER_6_CHAN_TYPE (45)\r\n");
			}
			else
				bDoYellow = false;
			cPart2 = cPart[0];
			cPart2 += "/";
			cPart2 += cPart[1];
			if (bDoYellow)
				if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE1,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE1,cPart2);

			cPart2 = cPart[2];
			cPart2 += "/";
			cPart2 += cPart[3];
			if (bDoYellow)
				if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE2,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE2,cPart2);

			cPart2 = cPart[4];
			cPart2 += "/";
			cPart2 += cPart[5];
			if (bDoYellow)
				if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE3,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE3,cPart2);

			cPart2 = cPart[6];
			cPart2 += "/";
			cPart2 += cPart[7];
			if (bDoYellow)
				if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE4,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE4,cPart2);

			cPart2 = cPart[8];
			cPart2 += "/";
			cPart2 += cPart[9];
			if (bDoYellow)
				if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE5,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE5,cPart2);

			cPart2 = cPart[10];
			cPart2 += "/";
			cPart2 += cPart[11];
			if (bDoYellow)
				if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE6,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE6,cPart2);
			
			cPart2 = cPart[12];
			cPart2 += "/";
			cPart2 += cPart[13];
			if (bDoYellow)
				if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE7,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE7,cPart2);

			cPart2 = cPart[14];
			cPart2 += "/";
			cPart2 += cPart[15];
			if (bDoYellow)
				if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
					SetColor(USER4PAGE,IDC_USER4PROG_3CHTYPE8,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_3CHTYPE8,cPart2);
			
			cPart = cFull.Mid(63,1);
			if (!CurrentSetParam(SET4_TRIGGER7_LOGIC,"SET4_TRIGGER7_LOGIC",cPart,false))
			{
				bChange = true;
				bDoYellow = true;
				m_iUser4Prog_Tab |= 0x0002;
				SetColor(USER4PAGE,IDC_USER4PROG_LOGIC4,COLOR_YELLOW);
				WriteToFileEx(180,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18507 SETUP FAILURE - TRIGGER_7_LOGIC (45)\r\n");
			}
			else
				bDoYellow = false;
			SetString(USER4PAGE,IDC_USER4PROG_LOGIC4,cPart);
			
			cPart = cFull.Mid(64,16);
			if (!CurrentSetParam(SET4_TRIGGER7_CHTYPE,"SET4_TRIGGER7_CHTYPE",cPart,false))
			{
				bChange = true;
				bDoYellow = true;
				m_iUser4Prog_Tab |= 0x0002;
				while (m_cSetParamMatchText.GetLength() < 16) m_cSetParamMatchText += ' ';
				WriteToFileEx(181,TO_CEV | TO_PFM, TYPE_INST, &cjTime, "18508 SETUP FAILURE - TRIGGER_7_CHAN_TYPE (45)\r\n");
			}
			else
				bDoYellow = false;
			cPart2 = cPart[0];
			cPart2 += "/";
			cPart2 += cPart[1];
			if (bDoYellow)
				if ((m_cSetParamMatchText[0] != cPart[0]) || (m_cSetParamMatchText[1] != cPart[1]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE1,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE1,cPart2);

			cPart2 = cPart[2];
			cPart2 += "/";
			cPart2 += cPart[3];
			if (bDoYellow)
				if ((m_cSetParamMatchText[2] != cPart[2]) || (m_cSetParamMatchText[3] != cPart[3]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE2,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE2,cPart2);

			cPart2 = cPart[4];
			cPart2 += "/";
			cPart2 += cPart[5];
			if (bDoYellow)
				if ((m_cSetParamMatchText[4] != cPart[4]) || (m_cSetParamMatchText[5] != cPart[5]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE3,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE3,cPart2);

			cPart2 = cPart[6];
			cPart2 += "/";
			cPart2 += cPart[7];
			if (bDoYellow)
				if ((m_cSetParamMatchText[6] != cPart[6]) || (m_cSetParamMatchText[7] != cPart[7]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE4,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE4,cPart2);

			cPart2 = cPart[8];
			cPart2 += "/";
			cPart2 += cPart[9];
			if (bDoYellow)
				if ((m_cSetParamMatchText[8] != cPart[8]) || (m_cSetParamMatchText[9] != cPart[9]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE5,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE5,cPart2);

			cPart2 = cPart[10];
			cPart2 += "/";
			cPart2 += cPart[11];
			if (bDoYellow)
				if ((m_cSetParamMatchText[10] != cPart[10]) || (m_cSetParamMatchText[11] != cPart[11]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE6,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE6,cPart2);

			cPart2 = cPart[12];
			cPart2 += "/";
			cPart2 += cPart[13];
			if (bDoYellow)
				if ((m_cSetParamMatchText[12] != cPart[12]) || (m_cSetParamMatchText[13] != cPart[13]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE7,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE7,cPart2);

			cPart2 = cPart[14];
			cPart2 += "/";
			cPart2 += cPart[15];
			if (bDoYellow)
				if ((m_cSetParamMatchText[14] != cPart[14]) || (m_cSetParamMatchText[15] != cPart[15]))
					SetColor(USER4PAGE,IDC_USER4PROG_4CHTYPE8,COLOR_YELLOW);
			SetString(USER4PAGE,IDC_USER4PROG_4CHTYPE8,cPart2);

			if (m_pPropertySheet)
				m_pPropertySheet->TriggerConfigRecord(Record);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"18500 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);			//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}
	PostMessage(IDC_RECEIVE_RESULT,TRIGGER_CONFIG_RECORD,CheckSum);	
}

//record 46 through 4b (post monitor version 4 only)
void
CInstrument::ChannelNConfigRecord(char *Record,short message_type)
{

	int CheckSum;
	if (LengthOk(Record,CHANNELN_CONFIG_RECORD_LENGTH))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bStale = true;											//DR00406

		AdjustTabsTo(POST4);

		bool bChange; 

		char cChan = '0';
		int  iChan = 0;
		int  uiChan = 0;

		//iChan = chan offset into SET4_xxxx
		//uiChan = chan offset into IDC_USER4PROG_XXXXX
		CString cFull(Record);
		CString cPart;
		CString cName;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);

		switch(message_type) {
			case CHANNEL0_CONFIG_RECORD: 
				cChan = '0'; iChan = 00; uiChan = 0;
				if (m_uiLastChannel0ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel0ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel0ConfigRecord = Record;
				}
				break;
			case CHANNEL1_CONFIG_RECORD: 
				cChan = '1'; iChan = 12; uiChan = 10;
				if (m_uiLastChannel1ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel1ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel1ConfigRecord = Record;
				}
				break;
			case CHANNEL2_CONFIG_RECORD: 
				cChan = '2'; iChan = 24; uiChan = 20;
				if (m_uiLastChannel2ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel2ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel2ConfigRecord = Record;
				}
				break;
			case CHANNEL3_CONFIG_RECORD: 
				cChan = '3'; iChan = 36; uiChan = 30;
				if (m_uiLastChannel3ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel3ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel3ConfigRecord = Record;
				}
				break;
			case CHANNEL4_CONFIG_RECORD: 
				cChan = '4'; iChan = 48; uiChan = 40;
				if (m_uiLastChannel4ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel4ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel4ConfigRecord = Record;
				}
				break;
			case CHANNEL5_CONFIG_RECORD: 
				cChan = '5'; iChan = 60; uiChan = 50;
				if (m_uiLastChannel5ConfigRecordTime < cjTime.TotalSeconds())	//DR00406
				{																//DR00406
					m_uiLastChannel5ConfigRecordTime = cjTime.TotalSeconds();	//DR00406
					bStale = false;												//DR00406
					m_cChannel5ConfigRecord = Record;
				}
				break;
		}

		//note to self -- can't mess with iChan 'cause it is used as the index into 
		if (!bStale)
		{

			cPart = cjTime.Date();
			cPart += " ";
			cPart += cjTime.Time();
			cName = "SET4_CH0_TIME";
			cName.SetAt(7,cChan);
			CurrentSetParam(SET4_CH0_TIME+iChan,cName,cPart,true);

			cPart = cFull.Mid(12,1);
			cName = "SET4_CH0_USED";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(SET4_CH0_USED+iChan,cName,cPart,false))
			{
				m_iUser4Prog_Tab |= 0x0002;
				SetColor(USER4PAGE,IDC_USER4PROG_CH0USED+uiChan/10,COLOR_YELLOW);
				CString cTemp;
				cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18601+uiChan,cName.Right(8),message_type);
				WriteToFileEx(182+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
			}
			bool bEnable;
			if (cPart == '1')
			{
				CString cTemp;
				cTemp.Format("Ch %c",cChan);
				SetString(USER4PAGE,IDC_USER4PROG_CH0USED+uiChan/10,cTemp);
				bEnable = true;
			}
			else
			{
				SetString(USER4PAGE,IDC_USER4PROG_CH0USED+uiChan/10,"Not Used");
				bEnable = false;
			}

			if (bEnable)
			{

				cPart = cFull.Mid(13,1);
				cName = "SET4_CH0_T1TYPEDIR";
				cName.SetAt(7,cChan);
				CString cTemp;
				if (!CurrentSetParam(SET4_CH0_T1TYPEDIR+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C00+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18602+uiChan,cName.Right(13),message_type);
					WriteToFileEx(192+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				if (cPart[0] == '0')				
					cPart = "Abs/-";				//dgp 16 dec 2004 (I had bit meaning swapped) 1.9.0.8
				else if (cPart[0] == '1')
					cPart = "Rel/-";				//dgp 16 dec 2004			
				else if (cPart[0] == '2')
					cPart = "Abs/+";				//dgp 16 dec 2004
				else if (cPart[0] == '3')
					cPart = "Rel/+";				//dgp 16 dec 2004
				SetString(USER4PAGE,IDC_USER4PROG_C00+uiChan,cPart);
 
				cPart = cFull.Mid(14,9);
				cName = "SET4_CH0_T1VALUE";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_T1VALUE+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C01+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18603+uiChan,cName.Right(11),message_type);
					WriteToFileEx(202+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			cPart.TrimLeft();
				SetString(USER4PAGE,IDC_USER4PROG_C01+uiChan,cPart);

				cPart = cFull.Mid(23,2);
				cName = "SET4_CH0_T1EEV";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_T1EEV+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C02+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18604+uiChan,cName.Right(9),message_type);
					WriteToFileEx(212+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			unsigned short ival1,ival2;
	//			sscanf(cPart,"%1x%1x",&ival1,&ival2);
	//			cPart.Format("%d/%d",ival1,ival2);
				SetString(USER4PAGE,IDC_USER4PROG_C02+uiChan,cPart);

				cPart = cFull.Mid(25,1);
				cName = "SET4_CH0_T2TYPEDIR";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_T2TYPEDIR+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C03+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18605+uiChan,cName.Right(13),message_type);
					WriteToFileEx(222+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				if (cPart[0] == '0')				
					cPart = "Abs/-";				//dgp 16 dec 2004 (I had bit meaning swapped) 1.9.0.8
				else if (cPart[0] == '1')
					cPart = "Rel/-";				//dgp 16 dec 2004			
				else if (cPart[0] == '2')
					cPart = "Abs/+";				//dgp 16 dec 2004
				else if (cPart[0] == '3')
					cPart = "Rel/+";				//dgp 16 dec 2004
				SetString(USER4PAGE,IDC_USER4PROG_C03+uiChan,cPart);
	//			Setup4Strings[iChan+80] = cPart;

				cPart = cFull.Mid(26,9);
				cName = "SET4_CH0_T2VALUE";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_T2VALUE+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C04+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18606+uiChan,cName.Right(11),message_type);
					WriteToFileEx(232+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			cPart.TrimLeft();
				SetString(USER4PAGE,IDC_USER4PROG_C04+uiChan,cPart);
				Setup4Strings[iChan+81] = cPart;

				cPart = cFull.Mid(35,2);
				cName = "SET4_CH0_T2EEV";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_T2EEV+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C05+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18607+uiChan,cName.Right(9),message_type);
					WriteToFileEx(242+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			sscanf(cPart,"%1x%1x",&ival1,&ival2);
	//			cPart.Format("%d/%d",ival1,ival2);
				SetString(USER4PAGE,IDC_USER4PROG_C05+uiChan,cPart);

				cPart = cFull.Mid(37,4);
				cName = "SET4_CH0_CHANGMULT";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_CHANGMULT+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C06+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18608+uiChan,cName.Right(13),message_type);
					WriteToFileEx(252+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			cPart.TrimLeft();
				SetString(USER4PAGE,IDC_USER4PROG_C06+uiChan,cPart);

				cPart = cFull.Mid(41,1);
				cName = "SET4_CH0_CHANGENTCNT";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_CHANGENTCNT+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C07+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18609+uiChan,cName.Right(15),message_type);
					WriteToFileEx(262+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
				SetString(USER4PAGE,IDC_USER4PROG_C07+uiChan,cPart);

				cPart = cFull.Mid(42,7);
				cName = "SET4_CH0_FILTERLIM";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_FILTERLIM+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C08+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18710+uiChan,cName.Right(13),message_type);
					WriteToFileEx(272+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			cPart.TrimLeft();
				SetString(USER4PAGE,IDC_USER4PROG_C08+uiChan,cPart);

				cPart = cFull.Mid(49,3);
				cName = "SET4_CH0_HYSTER";
				cName.SetAt(7,cChan);
				if (!CurrentSetParam(SET4_CH0_HYSTER+iChan,cName,cPart,false))
				{
					m_iUser4Prog_Tab |= 0x0002;
					bChange = true;
					SetColor(USER4PAGE,IDC_USER4PROG_C09+uiChan,COLOR_YELLOW);
					cTemp.Format("%d SETUP FAILURE - %s (%2x)\r\n",18711+uiChan,cName.Right(10),message_type);
					WriteToFileEx(282+iChan,TO_CEV | TO_PFM, TYPE_INST, &cjTime, (LPSTR)LPCSTR(cTemp));
				}
	//			cPart.TrimLeft();
	//			cPart += "%";
				SetString(USER4PAGE,IDC_USER4PROG_C09+uiChan,cPart);
			}
			else
			{
				cPart = "";
				SetString(USER4PAGE,IDC_USER4PROG_C00+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C01+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C02+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C03+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C04+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C05+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C06+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C07+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C08+uiChan,cPart);
				SetString(USER4PAGE,IDC_USER4PROG_C09+uiChan,cPart);
			}

			if (m_pPropertySheet)
				m_pPropertySheet->ChannelNConfigRecord(Record, message_type);
		}																		//DR00406

		char cBuff[1024];
		strcpy(cBuff,"18600 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM,TYPE_COMP,NULL,cBuff,bStale);						//DR00406

		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}
	PostMessage(IDC_RECEIVE_RESULT,CHANNELN_CONFIG_RECORD,CheckSum);	
}

int 
CInstrument::Checksum(char* Record)
{
    unsigned short nbytes;	/* # bytes to include in checksum calculation */
    unsigned short sum;		/* sum of all bytes in checksum */
	unsigned short rec_sum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	sum = 0;
    nbytes = (unsigned short)(strlen(Record)-4);//don't count lf/cr nor checksum
	//adjust for authentication in acquire record
	if (strncmp(ACQUIRE_RECORD_CHAR,Record,2)==0) 
	{
		//and cause a checksum error if wrong length received
		nbytes -= 2;//adjust for acquire record's auth byte
	}

	//don't bother with checking checksum if length is wrong
	for (int i=0; i<nbytes; i++)
		sum = (unsigned short)(sum + Record[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
//try{
	sscanf(&Record[nbytes],"%2hx",&rec_sum);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 107",MB_OK);
//	AfxMessageBox(&Record[nbytes],MB_OK);
//}
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

////	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
////	CDataStat* datapage    = m_pParent->m_pDataStatus;
////	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
////	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S ");
	DateTime += Record;
//	if (GetPageIndex(generalpage) >= 0)
//	{
		SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
		SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
		m_iGeneral_Tab |= 0x0001;
//		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
//	}
//	if (GetPageIndex(general4page) >= 0)
//	{
		SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
		SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
		m_iGeneral4_Tab |= 0x0001;
//		if (m_iGeneral4_Tab == 0x0003)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
//		else if (m_iGeneral4_Tab == 0x0002)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
//		else if (m_iGeneral4_Tab == 0x0001)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
//		else if (m_iGeneral4_Tab == 0x0000)
//			SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
//	}

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "17190 CHECKSUM ERROR\r\n");
	char cBuff[1024];
	strcpy(cBuff,"17200 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "17190 CHECKSUM ERROR\r\n");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CInstrument::LengthOk(char* Record, int length)
{
	//Check the length of the record	
	if (strlen(Record) == (unsigned)length)
		return true;

	//must have failed the length check

////	CSummary*  summarypage = m_pParent->m_pSummaryPage;
////	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
////	CDataStat* datapage    = m_pParent->m_pDataStatus;
////	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
////	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Length: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);

	SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
	SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
	m_iGeneral_Tab |= 0x0001;

	SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
	SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
	m_iGeneral4_Tab |= 0x0001;

//	generalpage->GetDlgItem(IDC_GENERAL_CHECKSUM)->Invalidate();
////	m_pParent->FlagComm(1,false);			
//	SetTabColor(IDC_GENERAL_TAB,COLOR_RED);

	WriteToFile(TO_PFM, TYPE_COMP, NULL, "17210 LENGTH ERROR\r\n");
	char cBuff[1024];
	strcpy(cBuff,"17220 ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

	if (m_bDoDump)
	{
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "17210 LENGTH ERROR\r\n");
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}
//debug to test ILON problem -- force pause on length error
//	AfxGetMainWnd()->PostMessage(IDC_FORCE_PAUSE, NULL, NULL);
//end debug to test ILON problem
    return false;
}

//this function should go into the parent class
void 
CInstrument::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cSuffix
	)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += cType;
				break;
			case 'I':
				{
					int iLength = cID.GetLength();
					while (iLength < 3)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'i':
				{
					int iLength = cID.GetLength();
					while (iLength < 2)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'y':
				cTemp.Format("%04d",iYear);
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",iMonth);
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",iDay);
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",iHour);
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",iMinute);
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",iSecond);
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}

bool CInstrument::WriteToBIDFile(CJulianTime* Time, AcquireRecord* AcquireRec, 
								   short AcquireCS, short AcquireAUTH, bool bIgnoreAuth)
{
	FILE *File;
	fpos_t position;
	struct AcquireHeader AcquireHead;
//	CGenlStat* generalpage;
//	CGenl4Stat* general4page;
	if (m_bBlockWrite) return false;
	
	//build filename
	if (m_bUseShortFilename)
	{
		sprintf(m_szCurrentBIDFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
	////			'0' + (Time->Year() % 10),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,BID_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"MG",
			m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,BID_SUFFIX);
		strcpy(m_szCurrentBIDFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (_access(m_szCurrentBIDFileName,0) != 0)
	{
////		WriteToFile(TO_DMP, TYPE_DUMP, NULL, "17221 ACCESS 1 FAILED\r\n");
		//make sure path exists
		MakeMyPath(m_szCurrentBIDFileName);

		//build archive file name
		CString ArchiveName;
		if (m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_szSaveLoc,	
				m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
	////			'0' + (Time->Year()%10),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,BID_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_csLongNameFormatString,cTemp,"MG",
				m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,BID_SUFFIX);
		}
		//if the file is in the archive directory
		if (_access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentBIDFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts > 0) && (File == NULL))
	{
		File = fopen(m_szCurrentBIDFileName,"ab+");
		if (File == NULL)
		{
			Sleep(200);
		}
		Attempts--;
	}

	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				memset( &AcquireHead, ' ', sizeof(AcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(AcquireHeader)-4);
				memcpy(AcquireHead.SizeOfHeader,temp,4);

				//Fix for new version number formatting inside of binary file
				//not converting to a float for the review software.
				//Incompatible format for binary file came out "2.0.0".
				//Compatible format is " 2000".
				//Prior to 2.0.0.0, the version number was formatted "1.907".
				//This code reformats the newer version format to look like the old.
				//SK uses a float to read the value from the binary file.
				//Fix provided by DP and incorporated by PJM 8/5/2005
				//
				//sprintf(temp,"%5s",m_pParent->m_csVersion);
				short sA,sB,sC,sD;
//try{
				sscanf(m_csVersion,"%d.%d.%d.%d",(int *)&sA, (int *)&sB, (int *)&sC, (int *)&sD);// added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 108",MB_OK);
//	AfxMessageBox(m_csVersion,MB_OK);
//}
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(AcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(AcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time->Year()%100);
				memcpy(AcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time->Month());
				memcpy(AcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time->Day());
				memcpy(AcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time->Year());
				memcpy(AcquireHead.unused2,temp,4);
				//write header
				fwrite(&AcquireHead,sizeof(AcquireHeader),1,File);

			}
		}
		
		if (m_bFirst15Rec)
		{
			m_dPreviousBIDTime = (short)(((AcquireCS - AcquireAUTH) & 0x000000ff) << 8);
			m_bFirst15Rec = false;
		}
		short auth = (short)((m_dPreviousBIDTime >> 8) & 0x000000ff);
		auth = (short)((auth + AcquireCS) & 0x000000ff);
		if ((auth == AcquireAUTH) || bIgnoreAuth)
		{
			//good auth
			AcquireRec->StatusByte |= 0x0002;
		}
		else
		{
			//bad auth
			AcquireRec->StatusByte &= 0x00fd;

			//convert the time of the 15 record to 
			//a date time string
			COleDateTime AuthError(
				Time->Year(),Time->Month(),Time->Day(),
				Time->Hour(),Time->Minute(),Time->Second());
			CString DateTime;
			char cBuff[1024];
			if (AcquireAUTH == AcquireCS)
			{
				DateTime = AuthError.Format("Auth. Mismatch: %Y/%m/%d %H:%M:%S Apparent Restart ") ;
				m_dPreviousBIDTime = 0;
				strcpy(cBuff,"17231 ");
			}
			else
			{
				DateTime = AuthError.Format("Auth. Failure: %Y/%m/%d %H:%M:%S ");
				strcpy(cBuff,"17230 ");
			}

			//append the 15+juliantime to the string
			CString Continue;
			Continue.Format("15%010u",AcquireRec->JulianSeconds);
			DateTime += Continue;
			
			//place the string on the general tab page
			SetString(GENERALPAGE,IDC_GENERAL_CHECKSUM,(char*)LPCSTR(DateTime));
			SetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,DateTime);
			if (AcquireAUTH != AcquireCS)
			{
				m_cGENERAL_CHECKSUM_text = DateTime;
				m_iGeneral_Tab = 0x0001;
				m_iGeneral4_Tab |= 0x0001;
				SetColor(GENERALPAGE,IDC_GENERAL_CHECKSUM,COLOR_RED);
				SetColor(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,COLOR_RED);
			}

			//add the new line and cr for the PFM/CEV files
			DateTime += "\r\n";

			//write the error to the pfm and cev files
			strcat(cBuff,(char*)LPCSTR(DateTime));
			WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, cBuff);

			//write the error to the dump file if required
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
		}

		//send record to the BID file
		int iWriteResult;
		if ((iWriteResult = fwrite(AcquireRec,sizeof(AcquireRecord),1,File)) != 1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			if ((iWriteResult = fwrite(AcquireRec,sizeof(AcquireRecord),1,File)) != 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				if ((iWriteResult = fwrite(AcquireRec,sizeof(AcquireRecord),1,File)) != 1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					//m_pButton->SetDiskChar('<',false);
					//m_pButton->SetDisk(true,true);
					//m_pButton->SetFlash(true);
					m_pButton->SetDiskIcon_Floppy();
					m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pButton->put_Flash(VARIANT_TRUE);
					//write the error to the pfm and cev files
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "17240 Could not write to BID file [DISK icon on] \r\n");
					//close the file anyway
					fclose(File);
					//save the fact that we couildn't so that we can tell PFM/CEV when good
					m_bCouldNotOpenBinaryFile = true;
					//return write failure 
					return false;
				}
			}
		}
		if ((iWriteResult == 1) && m_bCouldNotOpenBinaryFile)
		{
			m_bCouldNotOpenBinaryFile = false;
			WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "17250 Successful write to BID file\r\n");
		}
		//close the bid file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//save the fact that we couildn't so that we can tell PFM/CEV when good
		m_bCouldNotOpenBinaryFile = true;
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "17260 Could not open BID file [DISK icon on] \r\n");
	}
	return false;
}

void CInstrument::ReConfig()
{
	//reset any item that may have been changed by the "config dialog"

	//reset all ofthe max message repetitions
	for (int i = 0; i < sizeof(m_iMessageLimiter)/sizeof(int);i++)
		m_iMessageLimiter[i] = m_iMaxRepetition;
}

bool CInstrument::WriteToFileEx(
	unsigned uiIndex, int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale)
{
	bool bReturn = false;
	if (uiIndex < sizeof(m_iMessageLimiter)/sizeof(int))
	{
		if (m_iMessageLimiter[uiIndex] > 1)
		{
			WriteToFile(ToWhom, Type, Time, Msg, bStale);
			bReturn = true;
		}
		else if (m_iMessageLimiter[uiIndex] == 1)
		{
			//strip off right \r\n if we can
			CString cNewMsg(Msg);
			cNewMsg.TrimRight("\r\n");
			cNewMsg += " (further occurrences not included)\r\n";
			WriteToFile(ToWhom, Type, Time, (LPSTR)LPCSTR(cNewMsg), bStale);
			bReturn = true;
		}

		m_iMessageLimiter[uiIndex]--;
		if (m_iMessageLimiter[uiIndex] < 0)
			m_iMessageLimiter[uiIndex] = 0;
	}
	return bReturn;
}

void CInstrument::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale)		//DR00406
{
	FILE *File;
	char Message[2048];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime();
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_bBlockWrite) 
		return;

	//EstablishVersionString();

	if (Msg)
	{
		unsigned uLength = strlen(Msg);
		if (uLength >= 2)
		{
			if ((Msg[uLength-1] == 0x0d) && (Msg[uLength-2] == 0x0a))
			{
				Msg[uLength-1] = 0x0a;
				Msg[uLength-2] = 0x0d;
			}
		}
	}

	//default to the provided grand time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100)  Year	= 1900;//COleDateTime limits year 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year 100-9999
		if (Month	< 1)	Month	= 1;
		if (Day		< 1)	Day		= 1;
		if (Hour	< 0)	Hour	= 0;
		if (Minute	< 0)	Minute	= 0;
		if (Second	< 0)	Second	= 0;
		TG = COleDateTime(Year, Month, Day, Hour, Minute, Second);
	}
	//if that wasn't available then get the computer time
	//this is the case in all TYPE_COMP messages
	else
	{
		TG = COleDateTime::GetCurrentTime();
	}

	if (ToWhom == TO_DMP)
	{
		if (m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_szSaveLoc,	
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	////			'0' + (TC.GetYear()%10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"MG",
				m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		if (m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_szSaveLoc,	
				m_pID,
				((TG.GetYear()-1990)<10)?
					((TG.GetYear()-1990<0)?'#':'0'+(TG.GetYear()-1990)):
					'A'+(TG.GetYear()-2000),
////				'0' + (TG.Year() % 10),
				((TG.GetMonth()<10)?
					'0'+(TG.GetMonth()):
					'A'+(TG.GetMonth()-10)),
				((TG.GetDay()  <10)?
					'0'+(TG.GetDay()):
					'A'+(TG.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,m_csLongNameFormatString,m_szSaveLoc,"MG",
				m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,".");
			strcpy(m_szCurrentFileName,cResult);
		}
	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\r\n":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\r\n":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS Grand Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - G = xxxx\r"
		//computer time
		//grand time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G 17330 Grand Time %04d.%02d.%02d %02d:%02d:%02d C 727 Computer Time   C - G = %.0f seconds\r\n",
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 17280 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\r\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C GRAND COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 17290 GRAND COLLECT Version %s started\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_csVersion.GetString());
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C GRAND COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 17300 GRAND COLLECT Version %s started from abnormal shutdown\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_csVersion.GetString());
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 17310 File %s deleted\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 17320 Unknown TYPE %s\r\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+dmp
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
////		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (_access(fn,0) != 0)
		{
////			if (_access(ArchiveFileNameEx,0)!=-1)
////				MoveFileEx(ArchiveFileNameEx,fn,
////					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			MakeMyPath(fn);
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
		return;
	}

	//if to cev
	if (ToWhom & TO_CEV)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += CEV_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);
			
			if (m_bUseShortFilename)
			{
				//build archive path\name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_szSaveLoc,
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
////				'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_csLongNameFormatString,cTemp,"MG",
					m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);
			if ((position==0) && (m_dCurrent_State >= 2000))
			{
				for (int i = 0; i < (sizeof(m_iMessageLimiter)/sizeof(int)); i++)
					m_iMessageLimiter[i] = m_iMaxRepetition;//0-302 used, see WriteToFileEx

				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_bFCHVError	= false;
				m_bICHVError	= false;
				m_dBreakSentCount = 0;
				m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}

			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{																				//DR00406
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+pfm
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			//check if it is in the archive directory
			if (m_bUseShortFilename)
			{
				//build file name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_szSaveLoc,
				m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	////			'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,PFM_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_csLongNameFormatString,cTemp,"MG",
					m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,PFM_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"ab+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position );
			if ((position==0) && (m_dCurrent_State >= 2000))
			{
				m_bDoOneDumpStat = true;			//SCR00074
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
				m_bCountRateTamperOk = true;
				m_bRMSSigmaTamperOk  = true;
				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_bFCHVError	= false;
				m_bICHVError	= false;
				m_dBreakSentCount = 0;
				m_dTimeOutCount = 0;
				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send string
			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{																				//DR00406
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);
			//	close pfm
			fclose(File);
		}
	}
}

void CInstrument::GetStrings(PAGETYPE ePageType, UINT uiDlgItem, CString& cString, char* cFlag)
{
	COLORREF rgbColor = COLOR_BLACK;
	*cFlag = ' ';
	switch (ePageType)
	{
	case SUMMARYPAGE:
		if ((uiDlgItem >= IDC_SUMMARY_FIRST) && (uiDlgItem <= IDC_SUMMARY_LAST)) 
		{
		  rgbColor = SummaryStaticColors[uiDlgItem-IDC_SUMMARY_FIRST];
		  cString = SummaryStrings[uiDlgItem-IDC_SUMMARY_FIRST];
		}
		break;
	case GENERALPAGE:
		if ((uiDlgItem >= IDC_GENERAL_FIRST) && (uiDlgItem <= IDC_GENERAL_LAST)) 
		{
		  rgbColor = GeneralStaticColors[uiDlgItem-IDC_GENERAL_FIRST];
		  cString = GeneralStrings[uiDlgItem-IDC_GENERAL_FIRST];
		}
		break;
	case GENERAL4PAGE:
		if ((uiDlgItem >= IDC_GENERAL4_FIRST) && (uiDlgItem <= IDC_GENERAL4_LAST)) 
		{
		  rgbColor = General4StaticColors[uiDlgItem-IDC_GENERAL4_FIRST];
		  cString = General4Strings[uiDlgItem-IDC_GENERAL4_FIRST];
		}
		break;
	case USER4PAGE:
		if ((uiDlgItem >= IDC_USER4PROG_FIRST) && (uiDlgItem <= IDC_USER4PROG_LAST)) 
		{
		  rgbColor = User4StaticColors[uiDlgItem-IDC_USER4PROG_FIRST];
		  cString = User4Strings[uiDlgItem-IDC_USER4PROG_FIRST];
		}
		break;
	case USERPAGE:
		if ((uiDlgItem >= IDC_USERPROG_FIRST) && (uiDlgItem <= IDC_USERPROG_LAST)) 
		{
		  rgbColor = UserStaticColors[uiDlgItem-IDC_USERPROG_FIRST];
		  cString = UserStrings[uiDlgItem-IDC_USERPROG_FIRST];
		}
		break;
	case DATAPAGE:
		if ((uiDlgItem >= IDC_DATA_FIRST) && (uiDlgItem <= IDC_DATA_LAST)) 
		{
		  rgbColor = DataStaticColors[uiDlgItem-IDC_DATA_FIRST];
		  cString = DataStrings[uiDlgItem-IDC_DATA_FIRST];
		}
		break;
	}

	if (rgbColor == COLOR_YELLOW) *cFlag = '#';
	else if (rgbColor != COLOR_DEFAULT_TEXT) *cFlag = '*';
}

void CInstrument::SummaryPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3;//, cTemp4;

	fprintf(outfile,"SUMMARY\n");
	fprintf(outfile," GRAND\\MiniGRAND Setup\n");

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERY3,temp3,&cTemp3);

	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1.GetString(),cTemp1,temp2.GetString(),cTemp2,temp3.GetString(),cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P15VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P15VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P15VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_M15VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_M15VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_M15VOLT3,temp3,&cTemp3);
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT1,temp1,&cTemp1);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT2,temp2,&cTemp2);
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_P5VOLT3,temp3,&cTemp3);
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s%c %s%c %s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	fprintf(outfile," Data Status\n");
	GetStrings(SUMMARYPAGE,IDC_SUMMARY_OOMII,temp,&cTemp);
	fprintf(outfile,"   Measurement Interval of Interest: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_ACPOWER,temp,&cTemp);
	fprintf(outfile,"                     External Power: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BATTERYOK,temp,&cTemp);
	fprintf(outfile,"                            Battery: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_BBMOK,temp,&cTemp);
	fprintf(outfile,"           Battery Backed Up Memory: %s%c\n",LPCTSTR(temp),cTemp);

	GetStrings(SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME,temp,&cTemp);/*\n            */
	fprintf(outfile,"  Last INQUIRE Status (Y.M.D H:M:S): %s%c\n",LPCTSTR(temp),cTemp);

	fprintf(outfile,"  Recent Commands and Responses:\n");
//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND6,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[5].GetString());

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND5,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[4].GetString());

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND4,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[3].GetString());

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND3,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[2].GetString());

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND2,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[1].GetString());

//	GetStrings(SUMMARYPAGE,IDC_SUMMARY_COMMAND1,temp,&cTemp);
	fprintf(outfile,"  %s\n",m_cSummaryRecent[0].GetString());
}

void CInstrument::GeneralPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3;//, cTemp4;

	fprintf(outfile,"GRAND SETUP\n");
	GetStrings(GENERALPAGE,IDC_GENERAL_TIME,temp,&cTemp);
	fprintf(outfile,"  Last GENERAL STATUS Record (Y.M.D H:M:S): %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MODE,temp,&cTemp);
	fprintf(outfile,"                          Operational Mode: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MODE2,temp,&cTemp);
	fprintf(outfile,"                          Operational Mode: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_OFFSET,temp,&cTemp);
	fprintf(outfile,"                        Last Offset Record: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_FCHIGHVMAX,temp3,&cTemp3);
	fprintf(outfile," Pulse Counter HV Bias (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1.GetString(),cTemp1,temp2.GetString(),cTemp2,temp3.GetString(),cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_ICHIGHVMAX,temp3,&cTemp3);
	fprintf(outfile,"   Ion Chamber HV Bias (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_BATTERYMAX,temp3,&cTemp3);
	fprintf(outfile,"       Battery Voltage (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_P15VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_P15VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_P15VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"       +12 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_M15VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_M15VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_M15VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"       -12 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTMIN,temp1,&cTemp1);
	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTCUR,temp2,&cTemp2);
	GetStrings(GENERALPAGE,IDC_GENERAL_P5VOLTMAX,temp3,&cTemp3);
	fprintf(outfile,"        +5 Volt Supply (Min/Cur/Max): %7s%c %7s%c %7s%c\n\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3);

	GetStrings(GENERALPAGE,IDC_GENERAL_PULSE,temp,&cTemp);
	fprintf(outfile," Pulse Input Configuration: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_LLDA,temp,&cTemp);
	fprintf(outfile,"   LLDA: %10s%c    ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_WINDOWA,temp,&cTemp);
	fprintf(outfile," Window A: %10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_LLDB,temp,&cTemp);
	fprintf(outfile,"   LLDB: %10s%c    ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_WINDOWB,temp,&cTemp);
	fprintf(outfile," Window B: %10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_ANALOG,temp,&cTemp);
	fprintf(outfile," Analog Pulse Channel Gain: %s%c\n\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_DISCM,temp,&cTemp);
	fprintf(outfile," Ion Chamber Disconnect Mode: %s%c\n",temp.GetString(),cTemp);

	fprintf(outfile,"                          Gamma1           Gamma2\n");
	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA1G,temp,&cTemp);
	fprintf(outfile,"                Gain: %11s%c     ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA2G,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA1M,temp,&cTemp);
	fprintf(outfile,"                Mode: %11s%c     ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GAMMA2M,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MAXG1,temp,&cTemp);
	fprintf(outfile,"            Max Gain: %11s%c     ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_MAXG2,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G1DATE,temp,&cTemp);
	fprintf(outfile,"  Date Last Dual Ion: %11s%c     ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G2DATE,temp,&cTemp);
	fprintf(outfile,"%10s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G1TIME,temp,&cTemp);
	fprintf(outfile,"  Time Last Dual Ion: %11s%c     ",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_G2TIME,temp,&cTemp);
	fprintf(outfile,"%10s%c\n\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_GRANDP,temp,&cTemp);
	fprintf(outfile," GRAND Program Version Number: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_ROM,temp,&cTemp);
	fprintf(outfile," ROM Checksum: %s%c\n",temp.GetString(),cTemp);

	GetStrings(GENERALPAGE,IDC_GENERAL_CHECKSUM,temp,&cTemp);
	fprintf(outfile," Checksum, Length, or Authentication Error Received in Message:%c\n %s\n",cTemp,temp.GetString());
}

void CInstrument::General4PageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;

	fprintf(outfile,"MG SETUP\n");
	fprintf(outfile,"Most Recent General Status Record, Instrument Status Record, and Battery Record\n");

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTINT,temp,&cTemp);
	fprintf(outfile,"                      Battery Interval (min.): %s %c\n",temp.GetString(),cTemp);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_STATINT,temp,&cTemp);
	fprintf(outfile,"                       Status Interval (min.): %s %c\n",temp.GetString(),cTemp);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHVSETPOINT,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_FCHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"PC HV Set Point %6s%c Pulse Counter HV Bias: %6s%c %6s%c %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHVSETPOINT,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ICHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"IC HV Set Point %6s%c   Ion Chamber HV Bias: %6s%c %6s%c %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_COUNTTIME,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYMAX,temp4,&cTemp4);
	fprintf(outfile,"    Count Time: %6s%c       Battery Voltage: %6s%c %6s%c %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MODE,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P15VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P15VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 1: %6s%c       +12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MODE2,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_M15VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_M15VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 2: %6s%c       -12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMIN,temp2,&cTemp2);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTCUR,temp3,&cTemp3);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_P5VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"                              +5 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_OFFSET,temp1,&cTemp1);
	fprintf(outfile,"       Offset Date & Time: %s\n",temp1.GetString());               

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ISRTIME,temp1,&cTemp1);
	fprintf(outfile,"          ISR Date & Time: %s\n",temp1.GetString());

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TIME,temp1,&cTemp1);
	fprintf(outfile,"          GSR Date & Time: %s\n",temp1.GetString());

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_BATTERYTIME,temp1,&cTemp1);
	fprintf(outfile,"      Battery Date & Time: %s\n",temp1.GetString());

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSSTATE,temp1,&cTemp1);
	fprintf(outfile,"          Time Sync State: %s%c\n",temp1.GetString(),cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSHOURLY,temp1,&cTemp1);
	fprintf(outfile,"              Hourly Time: %s%c\n",temp1.GetString(),cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSDAILY,temp1,&cTemp1);
	fprintf(outfile,"               Daily Time: %s%c\n",temp1.GetString(),cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSUPPER,temp1,&cTemp1);
	fprintf(outfile,"          Upper Tolerance: %s%c\n",temp1.GetString(),cTemp1);
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_TSLOWER,temp1,&cTemp1);
	fprintf(outfile,"          Lower Tolerance: %s%c\n",temp1.GetString(),cTemp1);
	
	fprintf(outfile,"Most Recent Instrument Status and Dual Current Mode Records\n");
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G1DATE,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G2DATE,temp2,&cTemp2);  //DP 10/14/04
	fprintf(outfile,"     Dual Ion Date & Time: %s %s\n",temp1.GetString(),temp2.GetString());

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMAUNMULT,temp1,&cTemp1);
	fprintf(outfile," Gamma Uncert. Multiplier: %s%c\n",temp1.GetString(),cTemp1);
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA1G,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA2G,temp2,&cTemp2);
	fprintf(outfile,"                           Gamma1   Gamma2\n");
	fprintf(outfile,"                Gain Mode: %6s%c  %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2);
	
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA1M,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GAMMA2M,temp2,&cTemp2);
	fprintf(outfile,"       Max or Fixed Index: %6s%c  %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MAXG1,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_MAXG2,temp2,&cTemp2);
	fprintf(outfile,"       Present Gain Index: %6s%c  %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_DISCM,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_HVCONFIG,temp2,&cTemp2);
	fprintf(outfile,"              Offset Mode: %6s%c         HV Config: %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G1TIME,temp1,&cTemp1);
	GetStrings(GENERAL4PAGE,IDC_GENERAL4_G2TIME,temp2,&cTemp2);
	fprintf(outfile,"         Nominal Interval: %6s%c  Maximum Interval: %6s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_GRANDP,temp1,&cTemp1);
	fprintf(outfile,"  GRAND Program Version Num: %6s%c\n",
		temp1.GetString(), cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_ROM,temp1,&cTemp1);
	fprintf(outfile,"               ROM Checksum: %6s%c\n",
		temp1.GetString(), cTemp1);

	GetStrings(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM,temp,&cTemp1);
	fprintf(outfile," Checksum, Length, or Authentication Error Received in Message: %c\n  %s\n",cTemp1,temp.GetString());
}

void CInstrument::DataPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cChar;
	fprintf(outfile,"DATA STATUS\n");
	GetStrings(DATAPAGE,IDC_DATA_INQUIRETIME,temp,&cChar);
	fprintf(outfile," Last INQUIRE Status (Y.M.D H:M:S):\n %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_UNITID,temp,&cChar);
	fprintf(outfile,"                      Main Unit ID: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_ACPOWER,temp,&cChar);
	fprintf(outfile,"            External Power Present: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_OPERATION,temp,&cChar);
	fprintf(outfile,"                         Operation: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_BYTESMEM,temp,&cChar);
	fprintf(outfile,"     Bytes of Data in GRAND Memory: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_DATAFILTER,temp,&cChar);
	fprintf(outfile,"                Data Filter Status: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_OOMII,temp,&cChar);
	fprintf(outfile,"   Measurement Interval of Interest: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_BBMOK,temp,&cChar);
	fprintf(outfile,"          Battery Backed Up Memory: %s%c\n",temp.GetString(),cChar);

	GetStrings(DATAPAGE,IDC_DATA_BATTERYOK,temp,&cChar);
	fprintf(outfile,"                           Battery: %s%c\n",temp.GetString(),cChar);

	fprintf(outfile,"Most Recent ACQUIRE Records\n");

	fprintf(outfile," Date of Record (Y/M/D): %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_DATADATE1).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATADATE2).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATADATE3).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATADATE4).GetString());

	fprintf(outfile," Time of Record (H:M:S): %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_DATATIME1).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATATIME2).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATATIME3).GetString(),
		GetString(DATAPAGE,IDC_DATA_DATATIME4).GetString());

	fprintf(outfile,"     Pulse A Count Rate: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_PULSEACNT1).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEACNT2).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEACNT3).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEACNT4).GetString());

	fprintf(outfile,"     Pulse B Count Rate: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_PULSEBCNT1).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEBCNT2).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEBCNT3).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSEBCNT4).GetString());

	fprintf(outfile,"     Pulse C Count Rate: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_PULSECCNT1).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSECCNT2).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSECCNT3).GetString(),
		GetString(DATAPAGE,IDC_DATA_PULSECCNT4).GetString());

	fprintf(outfile,"   Gamma 1 Gross Gammas: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_GAMMA1GR1).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1GR2).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1GR3).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1GR4).GetString());

	fprintf(outfile,"Gamma 1 Sigma/Int. Temp: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_GAMMA1S1).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1S2).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1S3).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA1S4).GetString());

	fprintf(outfile,"   Gamma 2 Gross Gammas: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_GAMMA2GR1).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2GR2).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2GR3).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2GR4).GetString());

	fprintf(outfile,"Gamma 2 Sigma/Ext. Temp: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_GAMMA2S1).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2S2).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2S3).GetString(),
		GetString(DATAPAGE,IDC_DATA_GAMMA2S4).GetString());

	fprintf(outfile," Duration Data Acquired: %10s %10s %10s %10s\n",
		GetString(DATAPAGE,IDC_DATA_LENGTHT1).GetString(),
		GetString(DATAPAGE,IDC_DATA_LENGTHT2).GetString(),
		GetString(DATAPAGE,IDC_DATA_LENGTHT3).GetString(),
		GetString(DATAPAGE,IDC_DATA_LENGTHT4).GetString());
}

void CInstrument::UserPageIssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;
	fprintf(outfile,"MONITOR PARAMETERS\n");
//	GetDlgItemText(IDC_USERPROG_TIME,temp);
	fprintf(outfile," Last USER PROGRAM STATUS Record (Y.M.D H:M:S): %s\n",
		GetString(USERPAGE,IDC_USERPROG_TIME).GetString());

	GetStrings(USERPAGE,IDC_USERPROG_USER,temp,&cTemp);
	fprintf(outfile," User Selected Data Acquire Time (secs): %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_IMMEDIATE,temp,&cTemp);
	fprintf(outfile," Immediate Buffer Size / Save Size: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_LOCALBACK,temp,&cTemp);
	fprintf(outfile,"       Local Background Definition: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_NOFILTER,temp,&cTemp);
	fprintf(outfile," No Filter and Enter MII Condition: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_FILTERMETH,temp,&cTemp);
	fprintf(outfile,"                     Filter Method: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_MIIENTER,temp,&cTemp);
	fprintf(outfile,"               MII Enter Condition: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_MIIEXIT,temp,&cTemp);
	fprintf(outfile,"                MII Exit Condition: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_FILTERENAB,temp,&cTemp);
	fprintf(outfile,"                 Filtering Enabled: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TRIGGERMODE,temp,&cTemp);
	fprintf(outfile,"                      Trigger Mode: %s%c\n",temp.GetString(),cTemp);

	fprintf(outfile,"                       Pulse A    Pulse B    Pulse C    Gamma 1    Gamma 2\n");

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAF,temp,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBF,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECF,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1F,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2F,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filter & MII Active:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp.GetString(),cTemp,temp1.GetString(),cTemp1,temp2.GetString(),cTemp2,temp3.GetString(),cTemp3,temp4.GetString(),cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAT,temp ,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBT,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECT,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1T,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2T,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile,"           Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp.GetString(), cTemp, temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_PULSEAL,temp ,&cTemp);
	GetStrings(USERPAGE,IDC_USERPROG_PULSEBL,temp1,&cTemp1);
	GetStrings(USERPAGE,IDC_USERPROG_PULSECL,temp2,&cTemp2);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA1L,temp3,&cTemp3);
	GetStrings(USERPAGE,IDC_USERPROG_GAMMA2L,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filtering Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n\n",
		temp.GetString(), cTemp, temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCE,temp,&cTemp);
	fprintf(outfile,"               Syncronization Enabled: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCT,temp,&cTemp);
	fprintf(outfile,"       Syncronization time (hh:mm:ss): %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCL,temp,&cTemp);
	fprintf(outfile," Syncronization Lower Limit (seconds): %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_SYNCU,temp,&cTemp);
	fprintf(outfile," Syncronization Upper Limit (minutes): %s%c\n\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TAKEOFF,temp,&cTemp);
	fprintf(outfile,"        Take Offsets Every (hours): %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_TAKEOFFMAX,temp,&cTemp);
	fprintf(outfile,"   Maximum Between Offsets (hours): %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_LOGBATT,temp,&cTemp);
	fprintf(outfile,"  Log Battery Info Every (minutes): %s%c\n\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_GRANDUV,temp,&cTemp);
	fprintf(outfile," GRAND User Program Version Number: %s%c\n",temp.GetString(),cTemp);

	GetStrings(USERPAGE,IDC_USERPROG_ROMCHECK,temp,&cTemp);
	fprintf(outfile,"                      ROM Checksum: %s%c\n",temp.GetString(),cTemp);

}

void CInstrument::User4PageIssuePrint(FILE *outfile)
{
	CString temp0, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
	char cTemp0, cTemp1, cTemp2, cTemp3, cTemp4, cTemp5, cTemp6, cTemp7, cTemp8, cTemp9;

	fprintf(outfile,"TRIGGER PARAMETERS\n");
	fprintf(outfile,"  Most Recent Trigger Configuration Record\n");
	fprintf(outfile,"  Date/Time of Record (Y.M.D H:M:S): %s\n",
		GetString(USER4PAGE,IDC_USER4PROG_TIME).GetString());

	fprintf(outfile,"             Logic     Channel / Type\n");
	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC1,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_1CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 4: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1.GetString(), cTemp1,	temp2.GetString(), cTemp2,	temp3.GetString(), cTemp3,	temp4.GetString(), cTemp4,
		temp5.GetString(), cTemp5,	temp6.GetString(), cTemp6,	temp7.GetString(), cTemp7,	temp8.GetString(), cTemp8,
		temp9.GetString(), cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC2,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_2CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 5: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4,
		temp5.GetString(), cTemp5, temp6.GetString(), cTemp6, temp7.GetString(), cTemp7, temp8.GetString(), cTemp8,
		temp9.GetString(), cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC3,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_3CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 6: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4,
		temp5.GetString(), cTemp5, temp6.GetString(), cTemp6, temp7.GetString(), cTemp7, temp8.GetString(), cTemp8,
		temp9.GetString(), cTemp9);

	GetStrings(USER4PAGE,IDC_USER4PROG_LOGIC4,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE1,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE2,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE3,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE4,temp5,&cTemp5);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE5,temp6,&cTemp6);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE6,temp7,&cTemp7);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE7,temp8,&cTemp8);
	GetStrings(USER4PAGE,IDC_USER4PROG_4CHTYPE8,temp9,&cTemp9);
	fprintf(outfile,"      Pin 7: %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c %3s%c\n",
		temp1.GetString(), cTemp1, temp2.GetString(), cTemp2, temp3.GetString(), cTemp3, temp4.GetString(), cTemp4,
		temp5.GetString(), cTemp5, temp6.GetString(), cTemp6, temp7.GetString(), cTemp7, temp8.GetString(), cTemp8,
		temp9.GetString(), cTemp9);

	fprintf(outfile,"  Most Recent Channel Config. Records\n");

	GetStrings(USER4PAGE,IDC_USER4PROG_CH0USED,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH1USED,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH2USED,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH3USED,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH4USED,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_CH5USED,temp5,&cTemp5);
	if (temp0.GetLength()==0)temp0 = "Ch 0";
	if (temp1.GetLength()==0)temp1 = "Ch 1";
	if (temp2.GetLength()==0)temp2 = "Ch 2";
	if (temp3.GetLength()==0)temp3 = "Ch 3";
	if (temp4.GetLength()==0)temp4 = "Ch 4";
	if (temp5.GetLength()==0)temp5 = "Ch 5";
	fprintf(outfile,"                         %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C00,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C10,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C20,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C30,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C40,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C50,temp5,&cTemp5);
	fprintf(outfile,"     Thresh 1 Type::Dir: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C01,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C11,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C21,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C31,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C41,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C51,temp5,&cTemp5);
	fprintf(outfile,"         Thresh 1 Value: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C02,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C12,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C22,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C32,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C42,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C52,temp5,&cTemp5);
	fprintf(outfile,"   Thresh 1 Entry::Exit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C03,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C13,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C23,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C33,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C43,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C53,temp5,&cTemp5);
	fprintf(outfile,"     Thresh 2 Type::Dir: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C04,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C14,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C24,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C34,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C44,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C54,temp5,&cTemp5);
	fprintf(outfile,"         Thresh 2 Value: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C05,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C15,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C25,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C35,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C45,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C55,temp5,&cTemp5);
	fprintf(outfile,"   Thresh 2 Entry::Exit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C06,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C16,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C26,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C36,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C46,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C56,temp5,&cTemp5);
	fprintf(outfile,"  Chng. Sig. Multiplier: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C07,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C17,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C27,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C37,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C47,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C57,temp5,&cTemp5);
	fprintf(outfile,"   Chng. Sig. Entry Pt.: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C08,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C18,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C28,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C38,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C48,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C58,temp5,&cTemp5);
	fprintf(outfile,"           Filter Limit: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);

	GetStrings(USER4PAGE,IDC_USER4PROG_C09,temp0,&cTemp0);
	GetStrings(USER4PAGE,IDC_USER4PROG_C19,temp1,&cTemp1);
	GetStrings(USER4PAGE,IDC_USER4PROG_C29,temp2,&cTemp2);
	GetStrings(USER4PAGE,IDC_USER4PROG_C39,temp3,&cTemp3);
	GetStrings(USER4PAGE,IDC_USER4PROG_C49,temp4,&cTemp4);
	GetStrings(USER4PAGE,IDC_USER4PROG_C59,temp5,&cTemp5);
	fprintf(outfile,"             Hysteresis: %9s%c  %9s%c  %9s%c  %9s%c  %9s%c  %9s%c\n",
		temp0.GetString(), cTemp0, temp1.GetString(), cTemp1,
		temp2.GetString(), cTemp2, temp3.GetString(), cTemp3,
		temp4.GetString(), cTemp4, temp5.GetString(), cTemp5);
}

void CInstrument::SetupPageIssuePrint(FILE* outfile)
{
	CGrandSetupData cSetupData;
	for (int iIndex = 0; iIndex <= (SET_TOP - SET_BASE); iIndex++)
	{
		cSetupData.SetItteratorLineTo(iIndex);
		fprintf(outfile,"%35s: %s\n",cSetupData.GetNext(),SetupStrings[iIndex].GetString());
	}
}
void CInstrument::Setup4PageIssuePrint(FILE* outfile)
{
	CGrandSetupData cSetupData;
	bool bAtChConfRecords = false;

	fprintf(outfile, "Instrument Settings\n");
                                                                    
	fprintf(outfile,"  Item: Reported(instrument) :: Desired(INI file)\n");
	for (int iIndex = 0; iIndex <= (SET4_TOP - SET4_BASE); iIndex++)
	{
		cSetupData.SetNewItteratorLineTo(iIndex);
		fprintf(outfile,"%35s: %s :: %s\n",cSetupData.GetNewNext(),Setup4Strings[iIndex].GetString(), Setup4INIStrings[iIndex].GetString());
	}
}

void CInstrument::IssuePrint2(FILE* outfile)
{
	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	fprintf(outfile,"* = Displayed in RED (reading out of tolerance)\n");
	fprintf(outfile,"# = Displayed in YELLOW (configuration doesn't match)\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	SummaryPageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	if (m_eMonitorVers == PRE4)
	{
		GeneralPageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}
	if (m_eMonitorVers == POST4)
	{
		General4PageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}

	DataPageIssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());

	if (m_eMonitorVers == PRE4)
	{
		UserPageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}
	if (m_eMonitorVers == POST4)
	{
		User4PageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}

	if (m_eMonitorVers == PRE4)
	{
		SetupPageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}
	if (m_eMonitorVers == POST4)
	{
		Setup4PageIssuePrint(outfile);
		fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	}
//
//	modparpage->IssuePrint(outfile);
//	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime);
//
	fprintf(outfile,"\nCURRENT CONFIGURATION (from INI file)\n");
	char cIniData[32768];
	if (GetPrivateProfileSection(m_pName,cIniData,sizeof(cIniData),m_szIniFile)>0)
	{
		char *at = cIniData;		//char pointer to step through buffpass
		while (strlen(at)>0) 
		{
			fprintf(outfile,"  %s\n",at);
			at = at + strlen(at)+1;
		}
	}
}

void CInstrument::FlagTime(int val, bool redraw)
{
	CISO::FlagTime(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(303,TO_PFM,TYPE_COMP,NULL,
			"30000 INQUIRE2 Response Record caused [TIME icon on] \r\n");
	}
}

void CInstrument::FlagComm(int val, bool redraw)
{
	CISO::FlagComm(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(304,TO_PFM,TYPE_COMP,NULL,
			"30001 A communications problem caused [FLAG, SKULL, or BOMB icon on]\r\n");
	}
}

void CInstrument::FlagOthr(int val, bool redraw)
{
	CISO::FlagOthr(val,redraw);

	if (val == 1)
	{
		WriteToFileEx(305,TO_PFM,TYPE_COMP,NULL,
			"30002 A power or BBM problem caused [FROWN FACE icon on] \r\n");
	}
}


void CInstrument::UpdateDisplay()
{
//do these if we know PRE4
	if ((m_eMonitorVers == PRE4) || (m_eMonitorVers == UNKNOWN))
	{
		if (m_cStatusRec.GetLength())
			m_pPropertySheet->StatusRec((LPSTR)LPCSTR(m_cStatusRec));

		if (m_cTripNSRec.GetLength())
			m_pPropertySheet->TripNSRec((LPSTR)LPCSTR(m_cTripNSRec));

		if (m_cDualIonRec.GetLength())
			m_pPropertySheet->DualIonRec((LPSTR)LPCSTR(m_cDualIonRec));

		if (m_cGenStatRec.GetLength())
			m_pPropertySheet->GenStatRec((LPSTR)LPCSTR(m_cGenStatRec));

		if (m_cMon_A_Rec.GetLength())
			m_pPropertySheet->Mon_A_Rec((LPSTR)LPCSTR(m_cMon_A_Rec));

		if (m_cMon_B_Rec.GetLength())
			m_pPropertySheet->Mon_B_Rec((LPSTR)LPCSTR(m_cMon_B_Rec));
	}
//do these if we know POST4
	else if ((m_eMonitorVers == POST4) || (m_eMonitorVers == UNKNOWN))
	{
		if (m_cID3_Record.GetLength())
			m_pPropertySheet->ID3_Record((LPSTR)LPCSTR(m_cID3_Record));

		if (m_cGeneralConfigRecord.GetLength())
			m_pPropertySheet->GeneralConfigRecord((LPSTR)LPCSTR(m_cGeneralConfigRecord));

		if (m_cInstrumentStatusRecord.GetLength())
			m_pPropertySheet->InstrumentStatusRecord((LPSTR)LPCSTR(m_cInstrumentStatusRecord));

		if (m_cInstrumentInfoRecord.GetLength())
			m_pPropertySheet->InstrumentInfoRecord((LPSTR)LPCSTR(m_cInstrumentInfoRecord));

		if (m_cDualCurrentModeConfigRecord.GetLength())
			m_pPropertySheet->DualCurrentModeConfigRecord((LPSTR)LPCSTR(m_cDualCurrentModeConfigRecord));

		if (m_cTriggerConfigRecord.GetLength())
			m_pPropertySheet->TriggerConfigRecord((LPSTR)LPCSTR(m_cTriggerConfigRecord));

		if (m_cChannel0ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel0ConfigRecord),CHANNEL0_CONFIG_RECORD);

		if (m_cChannel1ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel1ConfigRecord),CHANNEL1_CONFIG_RECORD);

		if (m_cChannel2ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel2ConfigRecord),CHANNEL2_CONFIG_RECORD);

		if (m_cChannel3ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel3ConfigRecord),CHANNEL3_CONFIG_RECORD);

		if (m_cChannel4ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel4ConfigRecord),CHANNEL4_CONFIG_RECORD);

		if (m_cChannel5ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel5ConfigRecord),CHANNEL5_CONFIG_RECORD);

		if (m_cChannel6ConfigRecord.GetLength())
			m_pPropertySheet->ChannelNConfigRecord((LPSTR)LPCSTR(m_cChannel6ConfigRecord),CHANNEL6_CONFIG_RECORD);
	}

//always do these
	if (m_cInformRec.GetLength())
		m_pPropertySheet->InformRec((LPSTR)LPCSTR(m_cInformRec));

//	if (m_cAnalyzeOkRec.GetLength())
//		m_pPropertySheet->AnalyzeOkRec((LPSTR)LPCSTR(m_cAnalyzeOkRec));

	if (m_cID2_Record.GetLength())
		m_pPropertySheet->ID2_Record((LPSTR)LPCSTR(m_cID2_Record));

	if (m_cTripN2Rec.GetLength())
		m_pPropertySheet->TripN2Rec((LPSTR)LPCSTR(m_cTripN2Rec));

	if (m_cBatteryRec.GetLength())
		m_pPropertySheet->BatteryRec((LPSTR)LPCSTR(m_cBatteryRec));

	for (int i = 0; i < 4; i++)
	{
		if (m_cAcquireRec[i].GetLength())
			m_pPropertySheet->AcquirRec((LPSTR)LPCSTR(m_cAcquireRec[i]),true);
	}

	if (m_cIDRec.GetLength())
		m_pPropertySheet->ID_Record((LPSTR)LPCSTR(m_cIDRec));

	if (m_cOffsetRec.GetLength())
		m_pPropertySheet->OffsetRec((LPSTR)LPCSTR(m_cOffsetRec));

//	m_pPropertySheet->UpdateTabs();

}
	
void CInstrument::ShowInstrument(bool show)
{
	//if we already have 10 windows open don't allow any more
	if (CSafePropertySheet::ms_iGRANDPropertySheetCount < 10)
	{
		//correct for possible misscount
		if (CSafePropertySheet::ms_iGRANDPropertySheetCount < 0) 
			CSafePropertySheet::ms_iGRANDPropertySheetCount = 0;
		//create and open a new window
		if (!m_pPropertySheet)
		{
			m_pPropertySheet   = new CSafePropertySheet(LPCTSTR(m_pFullNameSpaces),this, m_szIniFile);

			//create each of the pages we'll need in the property sheet
			m_pSummaryPage     = new CSummary(IDD_SUMMARY, this);
			m_pGeneralStatPage = new CGenlStat(IDD_GENERALSTAT_PAGE);
			m_pGeneral4StatPage= new CGenl4Stat(IDD_GENERAL4STAT_PAGE);
			m_pDataStatus      = new CDataStat(IDD_DATASTATUS_PAGE);
			m_pUserProgStatus  = new CUserProg(IDD_USERPROGSTAT_PAGE);
			m_pUser4ProgStatus = new CUser4Prog(IDD_USER4PROGSTAT_PAGE);
			m_pSetupData	   = new CSetupData(m_pPropertySheet, m_szIniFile);
			m_pSetup4Data	   = new CSetupData(m_pPropertySheet, m_szIniFile);
			m_pModParams       = new CModParam(IDD_MODPARAMS_PAGE,this,m_szIniFile,false);
			m_pLegend		   = new CLegend(/*IDD_PROPPAGE_LEGEND*/);

			m_pSetupData->SetVersion("SET_");
			m_pSetup4Data->SetVersion("SET4_");

			//we keep the color info up here so tell each page where to find it
			m_pSummaryPage->StaticColors = SummaryStaticColors;
			m_pGeneralStatPage->StaticColors = GeneralStaticColors;
			m_pGeneral4StatPage->StaticColors = General4StaticColors;
			m_pDataStatus->StaticColors = DataStaticColors;
			m_pUserProgStatus->StaticColors = UserStaticColors;
			m_pUser4ProgStatus->StaticColors = User4StaticColors;
//			m_pSetupData->StaticColors = SetupStaticColors;
//			m_pSetup4Data->StaticColors = Setup4StaticColors;

			m_pPropertySheet->AddPage(m_pSummaryPage);
			m_pPropertySheet->AddPage(m_pGeneralStatPage);
			m_pPropertySheet->AddPage(m_pGeneral4StatPage);
			m_pPropertySheet->AddPage(m_pDataStatus);
			m_pPropertySheet->AddPage(m_pUserProgStatus);
			m_pPropertySheet->AddPage(m_pUser4ProgStatus);
			m_pPropertySheet->AddPage(m_pSetupData);
			m_pPropertySheet->AddPage(m_pSetup4Data);
			m_pPropertySheet->AddPage(m_pModParams);
			m_pPropertySheet->AddPage(m_pLegend);

			m_pPropertySheet->Create( this, //NULL, //DR00142
				WS_POPUP | WS_CAPTION | DS_MODALFRAME
				| WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU,
				0);

			m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
			m_pGeneralStatPage->SetPropertySheet(m_pPropertySheet);
			m_pGeneral4StatPage->SetPropertySheet(m_pPropertySheet);
			m_pDataStatus->SetPropertySheet(m_pPropertySheet);
			m_pUserProgStatus->SetPropertySheet(m_pPropertySheet);
			m_pUser4ProgStatus->SetPropertySheet(m_pPropertySheet);
			m_pSetupData->SetPropertySheet(m_pPropertySheet,m_pName);
			m_pSetup4Data->SetPropertySheet(m_pPropertySheet,m_pName);
			m_pModParams->SetPropertySheet(m_pPropertySheet,m_pName);
			//m_pLegend->m_pPropertySheet = m_pPropertySheet;

			//necessary because CPropertySheet doesn't creat a page until 
			//  it needs to display it.  So force it to create them all
			for (int i = m_pPropertySheet->GetPageCount();i>0;i--)
				m_pPropertySheet->SetActivePage(i-1);

			//set the summary page "scrolling" information
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND6,m_cSummaryRecent[5]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND5,m_cSummaryRecent[4]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND4,m_cSummaryRecent[3]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND3,m_cSummaryRecent[2]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND2,m_cSummaryRecent[1]);
			m_pSummaryPage->SetDlgItemText(IDC_SUMMARY_COMMAND1,m_cSummaryRecent[0]);

			char szTemp[MAX_PATH];

			GetModuleFileName(AfxGetInstanceHandle(),szTemp,sizeof(szTemp));
			char *c = strrchr(szTemp,'\\');//find the last "\"
			*c = '\0';						//make it null
			strcat(szTemp,"\\");
			
			CString cTemp(szTemp);
			cTemp += m_pName;
			cTemp += ".rtf";
			if (_access(cTemp,4) == 0)
				m_pLegend->SetText((LPSTR)LPCSTR(cTemp));
			else
			{

				cTemp = szTemp;
				cTemp += "GRAND.rtf";
				if (_access(cTemp,4) == 0)
					m_pLegend->SetText((LPSTR)LPCSTR(cTemp));
			}

			//set the button
			ULONG RGBValue;
			m_pButton->get_FaceColor(&RGBValue);
			ULONG numeric;
			m_pButton->get_Numeric(&numeric);

			m_pPropertySheet->m_pCloseButton->SetFaceColor(RGBValue,false);
			m_pPropertySheet->m_pCloseButton->SetNumeric(numeric,false);
			if (RGBValue == COLOR_PAUSE)
				m_pPropertySheet->m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
			else
				m_pPropertySheet->m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

			szTemp[0] = NULL;
			CGrandSetupData cGrandSetupData;
			char *pszDescription;
			char *pszIniName;
			char *pszDefault;
			char *pszCommand;
			while ((pszDescription = cGrandSetupData.GetNext())!=NULL)
			{
				pszIniName = cGrandSetupData.GetNext();
				pszDefault = cGrandSetupData.GetNext();
				pszCommand = cGrandSetupData.GetNext();
				GetPrivateProfileString(m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
				m_pModParams->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
				m_pSetupData->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			}
			while ((pszDescription = cGrandSetupData.GetNewNext())!=NULL)
			{
				pszIniName = cGrandSetupData.GetNewNext();
				pszDefault = cGrandSetupData.GetNewNext();
				pszCommand = cGrandSetupData.GetNewNext();
				GetPrivateProfileString(m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
				m_pModParams->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
				m_pSetup4Data->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			}

			m_pPropertySheet->SetLimits(
				m_fbatvolth, m_fbatvoltl,
				m_fsup_p15h, m_fsup_p15l,
				m_fsup_m15h, m_fsup_m15l,
				m_fsup_p05h, m_fsup_p05l,
				m_dtime_err, m_pID);

			m_pPropertySheet->SetHLVoltageDisplay(
				m_fHighBV, m_fLowBV, m_bFirstBV,
				m_fHighP15, m_fLowP15, m_bFirstP15,
				m_fHighM15, m_fLowM15, m_bFirstM15,
				m_fHighP5, m_fLowP5, m_bFirstP5);

			m_pGeneralStatPage->SetDlgItemText(IDC_GENERAL_CHECKSUM,GetString(GENERALPAGE,IDC_GENERAL_CHECKSUM));
			m_pGeneral4StatPage->SetDlgItemText(IDC_GENERAL4_CHECKSUM,GetString(GENERAL4PAGE,IDC_GENERAL4_CHECKSUM));

			m_pModParams->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(m_bPause?TRUE:FALSE);
	
			UpdateStrings();

			UpdateDisplay();

			if (m_eMonitorVers==POST4)
				m_pSetup4Data->UpdateColors(Setup4StaticColors);
			else if (m_eMonitorVers==PRE4)
				m_pSetupData->UpdateColors(SetupStaticColors);

			m_pModParams->OnModparamsReset();

			m_pPropertySheet->UpdateTabs();

			m_pPropertySheet->Invalidate(false);
		}

		m_pPropertySheet->SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	else
	{
		AfxMessageBox("Close a GRAND Instrument window and try again");
	}
}

void
CInstrument::AckButton()
{
	FlagComm(0,false);
	FlagOthr(0,false);
	FlagTime(0,false);
	m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pButton->ShowIcon_Square(VARIANT_FALSE);
	m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pButton->put_Flash(VARIANT_FALSE);

	m_iGeneral_Tab	= 0;
	m_iGeneral4_Tab	= 0;
	m_iData_Tab		= 0;
	m_iUserProg_Tab	= 0;
	m_iUser4Prog_Tab	= 0;
	m_iInstSetup_Tab	= 0;
	m_iInst4Setup_Tab	= 0;

	int i;
	for (i = 0; i < sizeof(SummaryStaticColors)/sizeof(COLORREF); i++)
		SummaryStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(GeneralStaticColors)/sizeof(COLORREF); i++)
		GeneralStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(General4StaticColors)/sizeof(COLORREF); i++)
		General4StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(UserStaticColors)/sizeof(COLORREF); i++)
		UserStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(User4StaticColors)/sizeof(COLORREF); i++)
		User4StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(DataStaticColors)/sizeof(COLORREF); i++)
		DataStaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(Setup4StaticColors)/sizeof(COLORREF); i++)
		Setup4StaticColors[i] = COLOR_DEFAULT_TEXT;
	for (i = 0; i < sizeof(SetupStaticColors)/sizeof(COLORREF); i++)
		SetupStaticColors[i] = COLOR_DEFAULT_TEXT;

	WriteToFile(TO_PFM,TYPE_COMP,NULL,"30006 Flags acknowledged by user. [FLAGS all off]\r\n");
}
