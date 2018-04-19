/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

Copyright 2015, Los Alamos National Security, LLC. This software application and associated
material ("The Software") was prepared by the Los Alamos National Security, LLC. (LANS), under
Contract DE-AC52-06NA25396 with the U.S. Department of Energy (DOE). All rights in the software
application and associated material are reserved by DOE on behalf of the Government and LANS
pursuant to the contract.

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
// ISRInst.cpp
#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ccbutton.h"
#include "ISRSafePS.h"
#include "ISRInst.h"
#include "ISRGenlStat.h"
#include "ISRDataStat.h"
#include "ISRUserProg.h"
#include "ISRCDData.h"
#include "ISRModParam.h"
#include "colors.h"

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


// CISRInst
CISRInst::CISRInst()
{
//	TRACE("CISRInst::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}

CISRInst::CISRInst(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CISRInst::CTOR #2\n");
	m_pParent = Parent;
	FinishConstruction();
}
void CISRInst::FinishConstruction()
{
	m_eCategory         = STANDARD_ISO;

	strncpy(m_pType,ISR_DISCOVERY_TYPE, sizeof(m_pType));

	CString csComma(",");
	CString csExtensions = 
		CString(ISR_SUFFIX) + csComma +
		SOH_SUFFIX + csComma +
		PFM_SUFFIX + csComma +	//DR 353
		CEV_SUFFIX;				//DR 353

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));
	//strncpy(m_szFileExtensionsList,ISR_SUFFIX,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pGeneralStatPage	= NULL;
	m_pDataStatus		= NULL;
	m_pUserProgStatus	= NULL;
	m_pCDData			= NULL;
	m_pModParams		= NULL;
	m_pLegend			= NULL;
	m_pPropertySheet	= NULL;

	m_bDriveToZero		= false;
	m_bDoAnalyzeAfterDT0= false;
	m_bInHardFail		= false;
	m_bDoOneDumpStat	= false;//SCR00074

	m_dHardFailCount	= 0;
	m_dSlowReset		= SLOWRESET;
	m_dPrevISRTime		= 0;
	m_dBreakSentCount	= 0;
	m_dTimeOutCount		= 0;
	m_dInvalidTimeCount = 0;
	m_bDoingAutoTimeSet = false;
	m_bAllowAutoTime    = false;
	m_bIsAMSR			= false;

}

CISRInst::~CISRInst()
{
	//tell the world we are shutting down
#ifdef ALLOWTRIGGERHOLDOFF
	WriteExclusion();
#endif
	char str[512];		//dr00115
	char str2[128];
	if (m_pPropertySheet)
	{
		strcpy(str,"22550 ISR COLLECT stopped:");
		unsigned long size = sizeof(str2);
		if (::GetUserName(str2,&size))
		{
			strcat(str," Logon ID:");
			strcat(str,str2);
		}
		strcat(str," MIC User ID:");		//dr00115
		GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"UNKNOWN",str2,sizeof(str2),m_szIniFile);
		strcat(str,str2);														//dr00115
		strcat(str,"\r\n");														//dr00115
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, str);
	}

	//destroy each of the pages in the dialog box for this instrument
	BOOL bResult;  //for debug observation

	if (m_pSummaryPage)
	{
		bResult = ::DestroyWindow(m_pSummaryPage->m_hWnd);
		delete m_pSummaryPage;
	}
	if (m_pGeneralStatPage)
	{
		bResult = ::DestroyWindow(m_pGeneralStatPage->m_hWnd);
		delete m_pGeneralStatPage;
	}
	if (m_pDataStatus)
	{
		bResult = ::DestroyWindow(m_pDataStatus->m_hWnd);
		delete m_pDataStatus;
	}
	if (m_pUserProgStatus)
	{
		bResult = ::DestroyWindow(m_pUserProgStatus->m_hWnd);
		delete m_pUserProgStatus;
	}
	if (m_pCDData)
	{
		bResult = ::DestroyWindow(m_pCDData->m_hWnd);
		delete m_pCDData;
	}
	if (m_pModParams)
	{
		bResult = ::DestroyWindow(m_pModParams->m_hWnd);
		delete m_pModParams;
	}
	if (m_pLegend)
	{
		bResult = ::DestroyWindow(m_pLegend->m_hWnd);
		delete m_pLegend;
	}
	//destroy the property sheet itself
	if (m_pPropertySheet)
	{
		bResult = ::DestroyWindow(m_pPropertySheet->m_hWnd);
		delete m_pPropertySheet;
	}
}

BEGIN_MESSAGE_MAP(CISRInst, CWnd)
	//{{AFX_MSG_MAP(CISRInst)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceive)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
//
//               UpdateParams
//
//////////////////////////////////////////////////////////////////////////////
//Update Params gets called if initializing an instrument and after modifying it
bool CISRInst::UpdateParams(bool reseting)
{
	//get the file id to be used
	GetPrivateProfileString(m_pName,FILE_ID,"",m_pID,sizeof(m_pID),m_szIniFile);

	//get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,"C:\\DATA\\ISR01",m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);
	
	//get the DO_DUMP from the ini file
	m_bDoDump = GetPrivateProfileInt(m_pName,"DO_DUMP",0,m_szIniFile)==1;

	//get the LOG_MII from the ini file
	m_bLogMII = GetPrivateProfileInt(m_pName,"LOG_MII",1,m_szIniFile)==1;

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

	//get the LOG_FILTER from the ini file
	m_bLogFilter = GetPrivateProfileInt(m_pName,"LOG_FILTER",1,m_szIniFile)==1;

	//get the SLOWRESET from the ini file
	m_dSlowReset = GetPrivateProfileInt(m_pName,"SLOW_RESET",900,m_szIniFile);
	m_dSlowReset *= 1000;

	//get the port string from the ini file
	//if we are resetting and it has changed then we must unregister
	//and reregister
	char tempport[32];
	if (reseting) strcpy(tempport,m_pPort);
	bool portchange;
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort),m_szIniFile);
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
	m_iNode = GetPrivateProfileInt(m_pName,NODE,0,m_szIniFile);
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

	//get the max inquire interval
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
	
	//get the high voltage high limit
	m_dHighVoltH = GetPrivateProfileInt(m_pName,"HIGHVOLTH",2000,m_szIniFile);

	//get the high voltage low limit
	m_dHighVoltL = GetPrivateProfileInt(m_pName,"HIGHVOLTL",1000,m_szIniFile);

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

	if (reseting)
	{
		UpdateStrings();
		m_pPropertySheet->SetLimits(
			m_fbatvolth, m_fbatvoltl,
			m_fsup_p15h, m_fsup_p15l,
			m_fsup_m15h, m_fsup_m15l,
			m_fsup_p05h, m_fsup_p05l,
			m_dHighVoltH, m_dHighVoltL,
			m_dtime_err, m_pID);
		m_pModParams->OnModparamsReset();
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


void CISRInst::UpdateStrings()
{
	CreateFullNameString();
	m_pPropertySheet->SetWindowText(m_pFullName);
}

void CISRInst::ShowInstrument(bool show)
{
	//if attempting to show instrument then do it and bring it to top
	if (show) 
	{
		m_pPropertySheet->ShowWindow(SW_SHOW);
		m_pPropertySheet->BringWindowToTop();
	} 
	else
		m_pPropertySheet->ShowWindow(SW_HIDE);
}

void CISRInst::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
	if (bInPwrFail)
	{
		m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, 
			"22525 ISR COLLECT Power Fail Drive BBM to Zero.\n\r");
	}

	if (m_bIsAMSR && bInPwrFail)
	{
		PauseInstrument(true);
	}
	else
	{
		PauseInstrument(false);
		m_bDriveToZero = bDriveToZero;
		if (bDriveToZero)
			m_bDoAnalyzeAfterDT0 = true;
	}
}

void CISRInst::IssuePrint(FILE* outfile)
{
	if (m_pPropertySheet)
		m_pPropertySheet->IssuePrint(outfile);
	fprintf(outfile,"MIC Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object:");
	fprintf(outfile,m_bGoodRegister?"Yes\n":"No\n");
	fprintf(outfile,"  Checksum Error Count: %d\n",m_dChecksumCount);
	fprintf(outfile,"  Communications Error Count: %d\n",m_dCommFail);
	fprintf(outfile,"  Break Message filter on:");
	fprintf(outfile,(m_dBreakSentCount > m_iMaxRepetition)?"Yes\n":"No\n");
	fprintf(outfile,"  Invalid Time Count filter on:");
	fprintf(outfile,(m_dInvalidTimeCount > m_iMaxRepetition)?"Yes\n":"No\n");
	fprintf(outfile,"  Timeout Message filter on:");
	fprintf(outfile,(m_dTimeOutCount > m_iMaxRepetition)?"Yes\n":"No\n");
	CString Text;
	switch(m_dCurrent_State){
	case 1000: Text = "About to send first ?. (1000)"; break;
	case 1010: Text = "First ? sent.  Waiting for response. (1010)"; break;
	case 1030: Text = "Sending second ?. (1030)"; break;
	case 1040: Text = "Second ? sent.  Waiting for response. (1040)"; break;
	case 1060: Text = "Sending *50C - PSR to Monitor Mode. (1060)"; break;
	case 1070: Text = "Received a ?2.  Sending F - JSR to PSR mode.  (1070)"; break;
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
	case 8020: Text = "Forcing Instrument to Computer's Time. (8000 - 8020)"; break;
	case 9000: Text = "About to send LONG BREAK SET. (9000)"; break;
	case 9010: Text = "About to clear LONG BREAK SET. (9010)"; break;
	case 9500: Text = "COMMUNICATIONS FAILURE -- In slow (15 minute) reset.  (9500)"; break;
	case 9800:
	case 9810: Text = "WRITE FAILURE on ISR file (9800 - 9810)"; break;
	case 9900: Text = "COMMUNICATIONS FAILURE -- Register instrument error or cannot write to disk. (9900)"; break;
	default:
		Text = "UNKNOWN STATE";
	}
	fprintf(outfile,"  %s\n",Text.GetString());
		COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	fprintf(outfile,"%s ===== %s ================\n",m_pName,datetime.GetString());
	fprintf(outfile,"========================================================\n");
}

void CISRInst::Reset()
{
	if (IsPaused())
	{
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
void CISRInst::ReadExclusion()
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
		itoa(iIndex,szIndex,10);
		szExclusionValues[0] = NULL;
		GetPrivateProfileString(szExclusionName,szIndex,"",szExclusionValues,sizeof(szExclusionValues),m_szIniFile);
		if (strlen(szExclusionValues))
		{
			if (sscanf(szExclusionValues,"%d,%d",&ExclusionRec.uiStartTime,&ExclusionRec.uiEndTime)==2)
				m_ExclusionList.AddTail(ExclusionRec);
		}
		iIndex++;
	}while (strlen(szExclusionValues));
}

//write the exclusion list from the CList
//out to the ini file -- only called
//during instrument destruction
void CISRInst::WriteExclusion()
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
	itoa(iExcludeMin,szValue,10);
	//start a new copy of the section and save the minutes
	WritePrivateProfileString(szExclusionName,"MINUTES",szValue,m_szIniFile);
	//see if we have any exclusion periods
	POSITION pos = m_ExclusionList.GetHeadPosition();
	//if we do then write them to the new section
	if (pos)
	{
		for (int i=0;i < m_ExclusionList.GetCount();i++)
		{
			itoa(iIndex,szIndex,10);
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
void CISRInst::AddExclusion(LPARAM lParam)
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
		m_pPropertySheet->UpdateSummary(szString);
	}
	else
	{
		char szString[64];
		sprintf(szString,"Bad Exclusion Message, %d min", lParam);
		PostToWatch(szString);
		m_pPropertySheet->UpdateSummary(szString);
	}
}

void CISRInst::DelExclusion()
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
	m_pPropertySheet->UpdateSummary(szString);
}
#endif
////////////////////////////////////////////////////////////
//
//            PauseInstrument
//
///////////////////////////////////////////////////////////
void  CISRInst::PauseInstrument(bool pause)
{
//	TRACE("CISRInst::PauseInstrument(%s)\n", pause?"true":"false");

	//kill the timer so that it won't unpause us when we don't want
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);

	//if we are changing the pause state then change the button face 
	//and the member variable
	m_pButton->put_FaceColor((pause)?COLOR_PAUSE:COLOR_OK);
	m_pPropertySheet->SetButtonColor((pause)?COLOR_PAUSE:COLOR_OK);
	if (m_bPause != pause)
	{
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"22530 ISR COLLECT take data stopped.\n\r":
					"22540 ISR COLLECT take data started.\n\r");
	}
	m_bPause = pause;
	m_dTimeOutCount = 0;
	m_dBreakSentCount = 0;
	m_dInvalidTimeCount = 0;

	//if we are now paused then set the timer to un-pause us when
	//it expires at end of PauseMax
	if (pause) 
	{
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		m_pModParams->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(TRUE);
	}
	else
	{
		m_pModParams->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(FALSE);
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

void CISRInst::OnTimer(UINT nIDEvent) 
{
	//TRACE("CISRInst::OnTimer(%u)\n", nIDEvent);
	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEvent)
	{
	//max pause time ran out so we need to un-pause this instrument
	case TIMER_PAUSE:	
		PauseInstrument(false);
		break;

	//normal timer increment -- need to send this to the state machine
	case TIMER_TICK:
		PostMessage(uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
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
///////////////////////////////////////////////////////
//
//                 CreateInstrument
//
///////////////////////////////////////////////////////
bool CISRInst::CreateInstrument()
{
//	TRACE("CISRInst::CreateInstrument()\n");
//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	m_bPause  = true;

	if (!UpdateParams(false))
		return false;

	//create CISRInst object -- we won't actually have a window
	//the window will be in a non-modal dialog box 
	CString cs = AfxRegisterWndClass(NULL, NULL, NULL, NULL);

	BOOL result = CreateEx(
			0,               //DWORD dwExStyle, 
			(LPCTSTR)cs,     //LPCTSTR lpszClassName, 
			m_pName,         //LPCTSTR lpszWindowName, 
			0,               //DWORD dwStyle, 
			CRect(0,0,0,0),  //const RECT& rect, 
			NULL,            //CWnd* pParentWnd, 
			0                //UINT nID, 
			);               //LPVOID lpParam = NULL);

	if (result) 
	{	
		m_pButton->put_DisplayNumericString(VARIANT_TRUE,10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		m_pButton->put_Numeric(0);

		BSTR bstr;
		Convert_Character_String_to_BSTR("BBM: ",bstr);
		m_pButton->put_PreNumericString(bstr);

		//get the time flag status
		m_iFlagTime = GetPrivateProfileInt(m_pName,FLAGTIME,-1,m_szIniFile);
		if (m_iFlagTime == 1)
			m_pButton->ShowIcon_Clock(VARIANT_TRUE);

		//get the other flag status
		m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		if (m_iFlagOthr == 1)
			m_pButton->ShowIcon_Frown(VARIANT_TRUE);

		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,FLAGCOMM,-1,m_szIniFile);
		if (m_iFlagComm == 1)
			m_pButton->ShowIcon_Error(VARIANT_TRUE);

		//create each of the pages we'll need in the property sheet
		m_pSummaryPage     = new CISRSummary(IDD_ISRSUMMARY, this);
		m_pGeneralStatPage = new CISRGenlStat(IDD_ISRGENERALSTAT_PAGE);
		m_pDataStatus      = new CISRDataStat(IDD_ISRDATASTATUS_PAGE);
		m_pUserProgStatus  = new CISRUserProg(IDD_ISRUSERPROGSTAT_PAGE);
		m_pModParams       = new CISRModParam(IDD_ISRMODPARAMS_PAGE,this);
		m_pLegend		   = new CLegend();

		//create the user interface dialog box for this instrument
		m_pPropertySheet   = new CISRPropertySheet(LPCTSTR(m_pName),this);

		m_pPropertySheet->AddPage(m_pSummaryPage);
		m_pPropertySheet->AddPage(m_pGeneralStatPage);
		m_pPropertySheet->AddPage(m_pDataStatus);
		m_pPropertySheet->AddPage(m_pUserProgStatus);
		m_pPropertySheet->AddPage(m_pModParams);
		m_pPropertySheet->AddPage(m_pLegend);

		DWORD dwStyle = 
			WS_POPUP | WS_CAPTION | DS_MODALFRAME |
			WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;

		m_pPropertySheet->Create(this,dwStyle,0);

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pGeneralStatPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pUserProgStatus->SetPropertySheet(m_pPropertySheet);
		m_pModParams->SetPropertySheet((CISRPropertySheet*)m_pPropertySheet);

		//necessary because PropertySheet doesn't creat a page until 
		//  it needs to display it.  So force it to create them all
		for (int i = m_pPropertySheet->GetPageCount();i>=0;i--)
			m_pPropertySheet->SetActivePage(i-1);

		char szTemp[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(),szTemp,sizeof(szTemp));
		char *c = strrchr(szTemp,'\\');//find the last "\"
		*c = '\0';						//make it null
		strcat(szTemp,"\\");
		
		CString cTemp(szTemp);
		cTemp += m_pName;
		cTemp += ".rtf";
		if (_access(cTemp,4))
		{
			cTemp = szTemp;
			cTemp += "APC.rtf";
			if (_access(cTemp,4))
			{
				cTemp = 
				"Instrument Button Colors...\n"
				"GREEN_ICON\nNormal state.\n"
				"---------------------------\n"
				"YELLOW_ICON\nA command has been sent from MIC to the instrument and MIC is waiting for a response.\n"
				"---------------------------\n"
				"RED_ICON\nCommunications Failure! A command has been sent and resent but no acceptable response has been received back from the instrument.\n"
				"---------------------------\n"
				"WHITE_ICON\nFLASHING to previous color. The disk is full or the instrument has a failure which is causing data loss\n"
				"---------------------------\n"
				"GRAY_ICON\nMIC has been instructed not to talk to the instrument.  In all cases this state will eventuall revert to a normal operational state.\n"
				"===========================\n"
				"\n"
				"Instrument Button ICONs...\n"
				"SQUARE_ICON Square: One or more of the tabbed pages have a data item out of tolerance or a configuration mismatch.\n"
				"---------------------------\n"
				"CLOCK_ICON Clock: The difference between MIC's time and the instrument's time is greater than the user set tolerance.\n"
				"---------------------------\n"
				"FLAG_ICON Flag:  MIC has sent and resent a command with no acceptable response and consequently commanded a long break reset in an attempt to reinitialize the instrument or a Hard Checksum Error occurred.\n"
				"---------------------------\n"
				"BELL_ICON Bell: The instrument is currently reporting a \"Measurement Interval of Interest\" or equivalent.\n"	
				"---------------------------\n"
				"FACE_ICON Face: The instrument has reported either a power problem or a battery backed up memory problem.\n"	
				"---------------------------\n"
				"FLOPPY_ICON Floppy Disk & Flashing: MIC has stopped writing to disk and talking to the instrument.  This can be caused by the hard drive being too full, the maximum use of the hard drive set too low, or disk write failure.\n"
				"---------------------------\n"
				"SKULL_ICON Skull and Cross Bones: The instrument has repeatedly failed to respond to commands and long break resets.  MIC will attempt to reset it every 15 minutes or instrument is in a state which is causing DATA LOSS.\n"
				"---------------------------\n"
				"BOMB_ICON Bomb: Configuration Error.  MIC is not configured correctly. Edit the MIC.INI file or delete the instrument object and recreate it to correct this problem.\n";
			}
		}
		m_pLegend->SetText((LPSTR)LPCSTR(cTemp));

		UpdateStrings();

#ifdef ALLOWTRIGGERHOLDOFF
	ReadExclusion();
#endif

		//set the time to start data collecting
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);

		m_pPropertySheet->SetLimits(
				m_fbatvolth, m_fbatvoltl,
				m_fsup_p15h, m_fsup_p15l,
				m_fsup_m15h, m_fsup_m15l,
				m_fsup_p05h, m_fsup_p05l,
				m_dHighVoltH,m_dHighVoltL,
				m_dtime_err, m_pID);

		m_pModParams->OnModparamsReset();

		m_dCurrent_State = 1000;
		m_dTick_Count = 0;
		m_dTickToInquire = 0;

		//register this instument with the com object 
		//note that the DIALOGBOX gets registered -- so that it will receive 
		//the data coming from the port.  We may want to change this to a 
		//separate CWndThread window later on if the interface gets sluggish

		VARIANT_BOOL vbResult;

		m_pComm->RegisterISO(
			(ULONG)m_pPropertySheet->m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_FALSE,  //not OOB
			&vbResult);

		m_bGoodRegister = (vbResult == VARIANT_TRUE);

		m_pComm->RegisterISO(
			(ULONG)m_pPropertySheet->m_hWnd,
			m_iNode+OOBFLAG,
			STANDARD_ISO,
			VARIANT_TRUE,  //is OOB
			&vbResult);

		m_dRetransmit		= 0;		//no retransmitions yet
		m_dLongBreak		= 0;		//not in long break
		m_bdoingBBM			= true;		//start by doing dump bbm
		m_bdumpOK			= false;	//don't have a good dumpbbm yet
		m_bbbmVal			= false;	//don't have a bbm value yet
		m_bChecksumFail		= false;	//not checksum fail yet
		m_dChecksumCount	= 0;		//no checksum fails yet
		m_bInHardFail		= false;	//not in hard fail of any type yet
		m_bInitInitTimeOut	= true;	//need to initialize initial time out
		m_bInDumpBBM		= false;	//not in dumpbbm sequence yet

		m_pNow = COleDateTime::GetCurrentTime();
		m_dPreviousDay = m_pNow.GetDay();

		//SCR 118
		//EstablishVersionString();

		//tell the files that mic is starting for this instrument
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, 
			(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);

	} //if(result)
	
	return m_bGoodRegister;
}

bool CISRInst::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CISRInst::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	CISRModParam *pModParam = new CISRModParam(IDD_ISRMODPARAMS_PAGE,this,true);
	pModParam->SetPropertySheet((CISRPropertySheet*)&tps,m_pName);
	tps.AddPage(pModParam);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,ISR_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}

	return bReturn;
}
//////////////////////////////////////////////////////////////////////////////
//
//                 SetLocalRegisteredMessageValues
//
//////////////////////////////////////////////////////////////////////////////
void CISRInst::SetLocalRegisteredMessageValues()
{
//	TRACE("CISRInst::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}