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
//DSPECInst.cpp 

//CDSPECInst Class

#include "stdafx.h"
#include "resource.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "ini_definitions.h"
#include "ccbutton.h"
#include "DSPECSafePS.h"
#include "DSPECInst.h"
#include "DSPECDataStat.h"
#include "DSPECModParam.h"
#include "DSPECStaticSetupData.h"
#include "DSPECParameters.h"
#include "colors.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

const char* crlf = "\n";

#define IDC_COM_REQUEST_FOR_SPECTRUM_DATA 21113
#define IDC_COM_SEND_RAW_COMMAND_STRING   21114
#define IDC_DSPEC_SPECTRUM_DATA_PROCESSED 21115
#define IDC_COM_INIT_MCB                  21116
#define IDC_IPX_MCB_ERROR                 20007
#define IDC_COMMAND_RESPONSE_PAIR         20001

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;
//////////////////////////////////////////////////////////////////////////////
//
//                        Constructors
//                        Destructor
//                        Message Map
//
//////////////////////////////////////////////////////////////////////////////
CDSPECInst::CDSPECInst()
{
//	TRACE("CDSPECInst::CTOR #1\n");
	m_pParent = NULL;
	FinishConstruction();
}
CDSPECInst::CDSPECInst(CWnd* Parent, char* inifile)
	: CISO(inifile)
{
//	TRACE("CDSPECInst::CTOR #2\n");
	m_pParent = Parent;  //parent is CMicMgrDlg
	FinishConstruction();
}
void CDSPECInst::FinishConstruction()
{
	m_eCategory = STANDARD_ISO;

	strncpy(m_pType, DSPEC_DISCOVERY_TYPE, sizeof(m_pType));

	CString csComma(",");
	CString csExtensions = 
		CString(CHN_SUFFIX) + csComma + 
		SOH_SUFFIX + csComma +	//DR 353
		PFM_SUFFIX + csComma +	//DR 353
		CEV_SUFFIX;				//DR 353

	strncpy(m_szFileExtensionsList,(LPCTSTR)csExtensions,sizeof(m_szFileExtensionsList));
	//strncpy(m_szFileExtensionsList,CHN_SUFFIX,sizeof(m_szFileExtensionsList));

	m_pSummaryPage		= NULL;
	m_pDataStatus		= NULL;
	m_pSetupData        = NULL;
	m_pModParams		= NULL;
	m_pLegend			= NULL;
	m_pPropertySheet	= NULL;

	m_dCurrent_State = INITIALIZING_1000;

	m_iSecondsElapsedSinceReceivingData = 0;
	m_iSlowResetCount_sec               = 0;
	m_iShortTermTimeouts                = 0;
	m_iShortTermTimeoutRunningCounter   = 0;

	m_iTickToStatus     = 0;

	m_eDwellType            = UNKNOWN_DWELL_0;
	m_iSecToGo              = 0;

	m_bCommsFailure = false;
	m_bHVVoltageLevelFailure = false;
	m_bHVShutdownFailure = false;

	m_bRealtimeWatcherDataQuiet = false;

	memset(&m_SentParameters,0,sizeof(m_SentParameters));
}

CDSPECInst::~CDSPECInst()
{
//	TRACE("CDSPECInst(%s)::DTOR\n", m_pName);

	//tell the world we are shutting down
	char str[512];									//dr00115
	char str2[128];
	if (m_pPropertySheet)
	{
		strcpy(str,"68090 DSPEC COLLECT stopped:");
		unsigned long size = sizeof(str2);
		if (::GetUserName(str2,&size))
		{
			strcat(str," Logon ID:");
			strcat(str,str2);
		}
		strcat(str," MIC User ID:");											//dr00115
		//strcat(str,g_pVal_Name);  //<<QA>> Removed for MICGM.  Put back in?	//dr00115
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
	if (m_pDataStatus)
	{
		bResult = ::DestroyWindow(m_pDataStatus->m_hWnd);
		delete m_pDataStatus;
	}
	if (m_pModParams)
	{
		bResult = ::DestroyWindow(m_pModParams->m_hWnd);
		delete m_pModParams;
	}

	if (m_pSetupData)
	{
		bResult = ::DestroyWindow(m_pSetupData->m_hWnd);
		delete m_pSetupData;
	}

	if (m_pLegend)
	{
		bResult = ::DestroyWindow(m_pLegend->m_hWnd);
		delete m_pLegend;
	}

	//last, destroy the property sheet itself
	if (m_pPropertySheet)
	{
		bResult = ::DestroyWindow(m_pPropertySheet->m_hWnd);
		delete m_pPropertySheet;
	}
}

BEGIN_MESSAGE_MAP(CDSPECInst, CWnd)
	//{{AFX_MSG_MAP(CDSPECInst)
	ON_WM_TIMER()
	ON_MESSAGE(IDC_REQUEST_SETTINGS_UPDATES,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM,LPARAM))OnRequestSettingsUpdates)
	ON_MESSAGE(IDC_UPDATE_PARAMS,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnUpdateParams)
	ON_MESSAGE(IDC_DESIRED_VICE_CURRENT_MISMATCH,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnDesiredViceCurrentMismatch)
	ON_MESSAGE(IDC_FORCE_WRITE_CHN_FILE,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnManualWriteToCHNFile)
	ON_MESSAGE(IDC_CLEAR_AND_RESTART_SPECTRUM,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnClearAndRestartSpectrum)
	ON_MESSAGE(IDC_HV_VOLTAGE_LEVEL_FAIL,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnHVVoltageLevelFail)
	ON_MESSAGE(IDC_HV_VOLTAGE_LEVEL_RECOVER,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnHVVoltageLevelRecover)
	ON_MESSAGE(IDC_HV_SHUTDOWN,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnHVShutdown)
	ON_MESSAGE(IDC_HV_SHUTDOWN_RECOVER,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnHVShutdownRecover)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////
//
//                         OnUpdateParams
//                         UpdateParams
//
//////////////////////////////////////////////////////////////////////////////
//Update Params gets called if initializing an instrument and after modifying it
//NOTE: these are in alphabetical order corresponding to the order of the defaults
//as written (in the case of a non-existing INI file) by CMicDlg::OnInitDialog().

void CDSPECInst::OnUpdateParams(WPARAM reset, LPARAM sendsetupcommands)
{
//	TRACE("CDSPECInst(%s)::OnUpdateParams()\n", m_pName);

	if (reset != 0)
		UpdateParams(true);
	else
		UpdateParams(false);

	if (sendsetupcommands != 0)
	{
		SendSetupCommandsToDSPEC();
	}
}

bool CDSPECInst::UpdateParams(bool reseting)
{
//	TRACE("CDSPECInst(%s)::UpdateParams()\n", m_pName);
	int iDefault;

	// COMMFAIL - get the comm fail NUMBER OF retries prior to COMMFAIL
	sscanf(COMMFAIL_DEFAULT, "%d",&iDefault);
	m_iCommFail = GetPrivateProfileInt(m_pName,COMMFAIL,iDefault,m_szIniFile);
	if (m_iCommFail <= 1)
		m_iCommFail = iDefault;

	// DO_DUMP
	m_bDoDump = GetPrivateProfileInt(m_pName,DO_DUMP,0,m_szIniFile)==1;

	// FILEID
	GetPrivateProfileString(m_pName,FILE_ID,FILEID_DEFAULT,m_pID,sizeof(m_pID),m_szIniFile);

	// HV_FAIL
	sscanf(HV_FAIL_DEFAULT, "%d",&iDefault);
	m_iHVFail = GetPrivateProfileInt(m_pName,HV_FAIL,iDefault,m_szIniFile);

	// LONGDWELL_SEC
	sscanf(LONGDWELL_DEFAULT, "%d",&iDefault);
	m_iLongdwell_sec = GetPrivateProfileInt(m_pName,LONGDWELL_SEC,iDefault,m_szIniFile);

	// MAXCYCLE
	m_iCycleInt_ms = 1000;   //Force to 1 second

	// MAXPAUSE - get the maximum pause of this instrument
	sscanf(MAXPAUSE_DEFAULT, "%d",&iDefault);
	int maxpause_sec = GetPrivateProfileInt(m_pName,MAXPAUSE,iDefault,m_szIniFile);
	if (maxpause_sec < 0) 
		maxpause_sec = 0;
//	m_iPauseMax_ms = maxpause_sec * 1000;	//dgp 18 Oct 2006
	m_iPauseMax = maxpause_sec * 1000;	//dgp 18 Oct 2006

	// MAXSTATUS - get the maxstatus interval
	sscanf(MAXSTATUS_DEFAULT, "%d",&iDefault);
	int status_interval_minutes = GetPrivateProfileInt(m_pName,MAXSTATUS,iDefault,m_szIniFile);
	if (status_interval_minutes < 0) 
		status_interval_minutes = iDefault;
	m_iStatusInt_sec = status_interval_minutes * 60; //comes in in minutes, need seconds

	// SAVE_LOC - get the file location to save data at
	GetPrivateProfileString(m_pName,SAVE_LOC,SAVELOC_DEFAULT,m_szSaveLoc,sizeof(m_szSaveLoc),m_szIniFile);

	// PORT - get the port string (really the ipx CSO string) from the ini file
	// (The IPX "port" is tied directly with a specific instrument.  Therfore, we
	// cannot ever "reset" the port.)
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort)-1,m_szIniFile);

	// IPX has no "node"

	//quiet
	sscanf(QUIET_DEFAULT, "%d",&iDefault);
	int iquiet = GetPrivateProfileInt(m_pName,QUIET,iDefault,m_szIniFile);
	m_bRealtimeWatcherDataQuiet = (iquiet != 0);

	// REALTIMEDWELL_SEC
	// get the interval for getting data for the realtime graphic
	sscanf(REALTIMEDWELL_DEFAULT, "%d",&iDefault);
	m_iRealtimedwell_sec = GetPrivateProfileInt(m_pName,REALTIMEDWELL_SEC,iDefault,m_szIniFile);
	if (m_iRealtimedwell_sec < 1)
		m_iRealtimedwell_sec = iDefault;

	// SHORTDWELL_SEC
	sscanf(SHORTDWELL_DEFAULT, "%d",&iDefault);
	m_iShortdwell_sec = GetPrivateProfileInt(m_pName,SHORTDWELL_SEC,iDefault,m_szIniFile);

	//SLOW_RESET
	sscanf(SLOW_RESET_DEFAULT, "%d",&iDefault);
	m_iSlowReset_sec = GetPrivateProfileInt(m_pName,SLOW_RESET,iDefault,m_szIniFile);
	if (m_iSlowReset_sec <= 0)
		m_iSlowReset_sec = iDefault;

	// THRESHOLD
	sscanf(THRESHOLD_DEFAULT, "%d",&iDefault);
	int ithreshold = GetPrivateProfileInt(m_pName,THRESHOLD,iDefault,m_szIniFile);
	if (ithreshold < 0)
		ithreshold = iDefault;
	m_ulThreshold = (ULONG)ithreshold;
	m_pComm->put_ThresholdValue(m_ulThreshold);

	// TIMEOUT
	sscanf(TIMEOUT_DEFAULT, "%d",&iDefault);
	m_iTimeOut = GetPrivateProfileInt(m_pName,TIMEOUT,iDefault,m_szIniFile);
	if (m_iTimeOut <=0 ) 
		m_iTimeOut = iDefault;

	char buf[128];
	int sizebuf = sizeof(buf);

	//
	// the following are the high and low error limits for the readings
	// from the instrument.
	//

	// High Limit for high voltage - to indicate drift, not catastrophe
		GetPrivateProfileString(m_pName,HVVOLTH,HVVOLTH_DEFAULT,
				buf,sizebuf,m_szIniFile);
		sscanf(buf,"%d",&m_ihvvolth);

	// Low Limit for high voltage - to indicate drift, not catastrophe
		GetPrivateProfileString(m_pName,HVVOLTL,HVVOLTL_DEFAULT,
				buf,sizebuf,m_szIniFile);
		sscanf(buf,"%d",&m_ihvvoltl);
	//
	// the following are the hardware settings for the DSPEC instrument
	//
//	SendShapeCommandsToDSPEC(reseting);				//SCR000178
//	SendStandardGainCommandsToDSPEC(reseting);		//SCR000178
//	SendHVCommandsToDSPEC(reseting);				//SCR000178
//	SendPoleZeroCommandsToDSPEC(reseting);			//SCR000178
//	SendOtherCommandsToDSPEC(reseting);				//SCR000178
//	SendStabilizationCommandsToDSPEC(reseting);  	//SCR000178

	//
	// Finished with INI file reading for now...
	//
	if (reseting)
	{
		UpdateStrings();

		m_pPropertySheet->SetLimits(m_ihvvolth, m_ihvvoltl);

		m_pModParams->OnModparamsReset();

		if (!m_bPause)
		{
			//Instrument's background all-purpose timer - usually 1 second
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,m_iCycleInt_ms,NULL);

			KillTimer(TIMER_REALTIME_DWELL);
			SetTimer(TIMER_REALTIME_DWELL,m_iRealtimedwell_sec*1000,NULL);
		}
		else
		{
			//Maximum time the instrument may be paused - usually 10 minutes
			KillTimer(TIMER_PAUSE);
			SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////
//
//               SendSetupCommandsToDSPEC()
//				from MIC 1.907a
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendSetupCommandsToDSPEC()  //For MIC  1.907a
{
//#ifdef _DEBUG
//	Beep(1000,1000);
//#endif

	m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,
			"67275 DSPEC --Sending configuration commands to DSPEC--\n");
	bool reseting = true;  //this is a don't care.  Not used.
	SendShapeCommandsToDSPEC(reseting);
	SendStandardGainCommandsToDSPEC(reseting);
	SendHVCommandsToDSPEC(reseting);
	SendPoleZeroCommandsToDSPEC(reseting);
	SendOtherCommandsToDSPEC(reseting);
	SendStabilizationCommandsToDSPEC(reseting);  //do stabilization setup last
}


//////////////////////////////////////////////////////////////////////////////
//
//               UpdateStrings
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::UpdateStrings()
{
	CreateFullNameString();
	m_pPropertySheet->SetWindowText(m_pFullName);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  ShowInstrument
//
//////////////////////////////////////////////////////////////////////////////			
void CDSPECInst::ShowInstrument(bool show)
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

//////////////////////////////////////////////////////////////////////////////
//
//                 DriveToZero
//
//////////////////////////////////////////////////////////////////////////////			
void CDSPECInst::DriveToZero(bool /*bDriveToZero*/, bool /*bInPwrFail*/)
{
	//DSPEC doesn't have a BBM to drive to zero
	PauseInstrument(true);
}
//////////////////////////////////////////////////////////////////////////////
//
//                IssuePrint
//
//////////////////////////////////////////////////////////////////////////////			
void CDSPECInst::IssuePrint(FILE* outfile)
{
//	TRACE("CDSPECInst(%s)::IssuePrint()\n", m_pName);

	//fprintf(outfile,"****************Begin CDSPECInst::IssuePrint()\n");

	if (m_pPropertySheet)
		m_pPropertySheet->IssuePrint(outfile);

	fprintf(outfile,"DSPEC Instrument Status:\n");
	fprintf(outfile,"  Registered with communications object: %s\n",
		m_bGoodRegister?"Yes":"No");

	fprintf(outfile,"  Communications Error Count: %d\n",m_iCommFail);

	CString Text;
	fprintf(outfile,"  Current State: ");
	switch(m_dCurrent_State)
	{
		case 1000: Text = "INITIALIZING_1000";                             break;
		case 2000: Text = "WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000";      break;
		case 2010: Text = "EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010";      break;
		case 3000: Text = "WAITING_WITHIN_LONG_DWELL_INTERVAL_3000";       break;
		case 3010: Text = "EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010";       break;
		case 4000: Text = "EXPECTING_DATA_FOR_FORCED_CHN_FILE_WRITE_4000"; break;
		case 9000: Text = "SHORT_TERM_HARD_FAIL_9000";                     break;
		case 9500: Text = "LONG_TERM_HARD_FAIL_9500";                      break;
		case 9900: Text = "LOCKED_UP_9900";                                break;

		default: Text = "UNKNOWN STATE";
	}
	fprintf(outfile,"%s\n\n",Text.GetString()); // gjk

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"%s ===== %s ================\n\n",m_pName,datetime.GetString()); //gjk
	fprintf(outfile,"====================================================================\n");
	fprintf(outfile,"====================================================================\n");

	//fprintf(outfile,"***************************End CDSPECInst::IssuePrint()\n");
}
//////////////////////////////////////////////////////////////////////////////
//
//                       Reset
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::Reset()
{
//	TRACE("CDSPECInst(%s)::Reset()\n",m_pName);
	if (IsPaused())
	{
		SetStateAndButtonColor(INITIALIZING_1000);
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                       PauseInstrument
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::PauseInstrument(bool pause)
{
//	TRACE("CDSPECInst(%s)::PauseInstrument(%s)\n", m_pName, pause?"true":"false");

	//////////////////////
	//
	// always do these
	//
	//////////////////////
	// Stop everything....

	KillAllTimers();

	if (m_bPause != pause)
	{
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, 
			(pause)?"68070 DSPEC COLLECT take data stopped.\n":
					"68080 DSPEC COLLECT take data started.\n");

		m_bPause = pause;
	}

	///////////////////////
	//
	// if pausing, do these
	//
	///////////////////////
	if (m_bPause)
	{
		m_pButton->put_FaceColor((ULONG)COLOR_PAUSE);
		m_pPropertySheet->SetButtonColor(COLOR_PAUSE);  //this changes text, too

		//Maximum time the instrument may be paused - usually 10 min
//		SetTimer(TIMER_PAUSE,m_iPauseMax_ms,NULL);	//dgp 18 Oct 2006
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006
	}
	///////////////////////////////////////////////
	//
	// if un-pausing and not in shutdown, do these
	//
	///////////////////////////////////////////////
	else  //unpause - that is, carry on
	{
		//Always update status when coming out of a pause period
		PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);

		///////////////////////////////////////////////
		//
		// if un-pausing and not in shutdown, do these
		//
		//////////////////////////////////////////////
		// Reset everything - sledge hammer approach
		// As long as we aren't in shutdown, that is
		if (!m_bInShutDown)
		{
			m_iSecondsElapsedSinceReceivingData = 0;
			m_iSlowResetCount_sec               = 0;
			m_iTickToStatus                     = 0;

			SetTimer(TIMER_TICK,m_iCycleInt_ms,NULL);  //1-second allpurpose timer
			SetTimer(TIMER_REALTIME_DWELL,m_iRealtimedwell_sec*1000,NULL);

			//reset DSPEC if initializing.
			//StartNewShortDwell(m_dCurrent_State == INITIALIZING_1000);
			if (m_dCurrent_State == INITIALIZING_1000) //<<QA>> for 1.907a
				StartNewShortDwell(true);
			else
				StartNewShortDwell(false);

			SetStateAndButtonColor(WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000);
		}
		///////////////////////////////////////////////
		//
		// if un-pausing and in shutdown, do this
		//
		//////////////////////////////////////////////
		else  //in shutdown
		{
			//Let the state changing function set the button color
			//back to where it belongs.  Setting the current state to the
			//current state is a little convoluted, but it executes
			//the code in the function that sets the button face color
			//properly.
			SetStateAndButtonColor(m_dCurrent_State);
		}
	}

	////////////////////
	//
	// always do these
	//
	////////////////////
	// Have the MICDlg review the status of the buttons

	if (m_hButtonKeeperHandle)
		::PostMessage(m_hButtonKeeperHandle,m_uWM_CHECKDATABUTTON,0,0);
}
//////////////////////////////////////////////////////////////////////////////
//
//                              OnTimer
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnTimer(UINT nIDEvent) 
{
	//TRACE("CDSPECInst::OnTimer() ");

	//if we have a timer tick then notify the state machine
	//if we have a pause timer rundown then UN-pause this instrument
	switch (nIDEvent)
	{
	//max pause time ran out so we need to un-pause this instrument
	//Maximum time the instrument may be paused - usually 10 minutes
	case TIMER_PAUSE:
		//TRACE("CDSPECInst::OnTimer() TIMER_PAUSE\n");
		this->PauseInstrument(false);
		break;

	//normal timer increments -- need to send these to the state machine
	//Instrument's background timer - always 1 second for DSPEC
	case TIMER_TICK:
		//TRACE("CDSPECInst::OnTimer() TIMER_TICK\n");
		this->PostMessage(uIDC_RECEIVE,IDC_TIMER_TICK,0);
		break;
		
	//Time to provide updated spectrum buffer for real time graphic
	case TIMER_REALTIME_DWELL:
		//TRACE("CDSPECInst::OnTimer() iTIMER_DSPEC_REALTIME_DWELL\n");
		this->PostMessage(uIDC_RECEIVE,IDC_TIMER_REALTIME_DWELL,0);
		break;

	//Time to provide spectrum buffer for analysis
	case TIMER_ANALYSIS_DWELL:
		//TRACE("CDSPECInst::OnTimer() iTIMER_DSPEC_ANALYSISDWELL\n");
		this->PostMessage(uIDC_RECEIVE,IDC_TIMER_ANALYSIS_DWELL,0);
		break;
	}

	//let the system handle all other timer messages 
	CWnd::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////
//
//                 CreateInstrument - 3 signatures
//
//////////////////////////////////////////////////////////////////////////////
bool CDSPECInst::CreateInstrument()
{
//	TRACE("CDSPECInst::CreateInstrument_1()\n");
//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	m_bPause  = true;

	//find if IPX CSO exists - if it doesn't return false;
	if (!UpdateParams(false))
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
			);  

	if (result) 
	{	
		//start with the instrument paused and set the face color for the button
		m_pButton->put_DisplayNumericString(VARIANT_TRUE,10);
		m_pButton->put_FaceColor(COLOR_PAUSE);
		m_pButton->put_Flash(VARIANT_FALSE);
		m_pButton->put_Numeric(0);

		BSTR bstr;
		Convert_Character_String_to_BSTR("TTG: ",bstr); //"TTG" = time to go (on dwell)
		m_pButton->put_PreNumericString(bstr);

		//get the time flag status
		m_iFlagTime = GetPrivateProfileInt(m_pName,FLAGTIME,-1,m_szIniFile);
		if (m_iFlagTime == 1)
			m_pButton->ShowIcon_Clock(VARIANT_TRUE);

		//get the other flag status
		//m_iFlagOthr = GetPrivateProfileInt(m_pName,FLAGOTHR,-1,m_szIniFile);
		//if (m_iFlagOthr == 1)
		//	m_pButton->ShowIcon_Frown(VARIANT_TRUE);

		//get the comm flag status
		m_iFlagComm = GetPrivateProfileInt(m_pName,FLAGCOMM,-1,m_szIniFile);
		if (m_iFlagComm == 1)
			m_pButton->ShowIcon_Error(VARIANT_TRUE);


		//create each of the pages we'll need in the property sheet
		m_pSummaryPage     = new CDSPECSummary(IDD_DSPECSUMMARY,this);
		m_pDataStatus      = new CDSPECDataStat(IDD_DSPECDATASTATUS_PAGE,this,m_szIniFile);
		m_pModParams       = new CDSPECModParam(IDD_DSPECMODPARAMS_PAGE,this);
		m_pSetupData	   = new CDSPECProppageSetupData(this,m_szIniFile);
		m_pLegend		   = new CLegend();

		//create the user interface dialog box for this instrument
		m_pPropertySheet   = new CDSPECSafePropertySheet(LPCTSTR(m_pName),this, m_szIniFile);

		m_pPropertySheet->AddPage(m_pSummaryPage);
		m_pPropertySheet->AddPage(m_pDataStatus);
		m_pPropertySheet->AddPage(m_pSetupData);
		m_pPropertySheet->AddPage(m_pModParams);
		m_pPropertySheet->AddPage(m_pLegend);

		DWORD dwStyle = 
			WS_POPUP | WS_CAPTION | DS_MODALFRAME |
			WS_MINIMIZEBOX|WS_OVERLAPPED|WS_SYSMENU;

		m_pPropertySheet->Create(this,dwStyle,0);

		m_pSummaryPage->SetPropertySheet(m_pPropertySheet);
		m_pDataStatus->SetPropertySheet(m_pPropertySheet);
		m_pSetupData->SetPropertySheet(m_pPropertySheet,m_pName);
		m_pModParams->SetPropertySheet(m_pPropertySheet);

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
			cTemp += "DSPEC.rtf";
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

		//Maximum time the instrument may be paused - usually 10 minutes
//		SetTimer(TIMER_PAUSE,m_iPauseMax_ms,NULL);	//dgp 18 Oct 2006
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);	//dgp 18 Oct 2006

		m_pPropertySheet->SetLimits(m_ihvvolth,m_ihvvoltl);

		m_pModParams->OnModparamsReset();

		SetStateAndButtonColor(INITIALIZING_1000);

		//register this instrument with its communications object
		VARIANT_BOOL vbResult;

		m_pComm->RegisterISO(
			(ULONG)m_pPropertySheet->m_hWnd,
			m_iNode,
			STANDARD_ISO,
			VARIANT_FALSE,  //not OOB
			&vbResult);

		m_bGoodRegister = (vbResult == VARIANT_TRUE);

		Query_CSO();  //tickle the CSO

		m_pNow = COleDateTime::GetCurrentTime();
		m_iPreviousDay = m_pNow.GetDay();

		//SCR 118
		//EstablishVersionString();

		//tell the files that mic is starting for this instrument

		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, 
			(m_bNormalShutdown)?TYPE_START:TYPE_ABNORMAL);

//		if (!m_bNormalShutdown)  //MIC 1.907a //MIC 1.907b
//		{
			SendSetupCommandsToDSPEC();
//		}
	}  //if result
	
	return m_bGoodRegister;
}

//////////////////////////////////////////////////////////////////////////////
//
//                        CollectParametersForNewISO
//
//////////////////////////////////////////////////////////////////////////////
bool CDSPECInst::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CDSPECInst::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	//
	CTPropertySheet tps("",m_iDlgCloseMilliseconds,this);
	CDSPECModParam modparam_page(IDD_DSPECMODPARAMS_PAGE,this,true);

	modparam_page.SetPropertySheet(&tps);
	tps.AddPage(&modparam_page);
	tps.SetWizardMode();

	if (tps.DoModal()==ID_WIZFINISH)  //don't process the IDCANCEL??
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(INSTRUMENTS,m_pName,DSPEC_DISCOVERY_TYPE,m_szIniFile);
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
void CDSPECInst::SetLocalRegisteredMessageValues()
{
//	TRACE("CISRInst::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

//////////////////////////////////////////////////////////////////////////////
//
//                         SendShapeCommandsToDSPEC
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendShapeCommandsToDSPEC(bool /*resetting*/)
{
	char buf[128], cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(buf);

	// Shape - cusp factor

		GetPrivateProfileString(m_pName,SET_SHAP_CUSP,
			SET_SHAP_CUSP_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.shap_cusp,buf,DPLEN);
		sprintf(cmd,format,SET_SHAP_CUSP,buf);
		TransmitAndUpdateSummary(cmd);

	// Shape - flattop width

		GetPrivateProfileString(m_pName,SET_SHAP_FLAT,SET_SHAP_FLAT_DEFAULT,
			buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.shap_flat,buf,DPLEN);
		sprintf(cmd,format,SET_SHAP_FLAT,buf);
		TransmitAndUpdateSummary(cmd);

	// Shape - flattop correction

		GetPrivateProfileString(m_pName,SET_CORRECTION_FLAT,
			SET_CORRECTION_FLAT_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.correction_flat,buf,DPLEN);
		sprintf(cmd,format,SET_CORRECTION_FLAT,buf);
		TransmitAndUpdateSummary(cmd);

	// Shape - Risetime

		GetPrivateProfileString(m_pName,SET_SHAP_RISE,
			SET_SHAP_RISE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.shap_rise,buf,DPLEN);
		sprintf(cmd,format,SET_SHAP_RISE,buf);
		TransmitAndUpdateSummary(cmd);
}

//////////////////////////////////////////////////////////////////////////////
//
//                         SendStandardGainCommandsToDSPEC
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendStandardGainCommandsToDSPEC(bool /*resetting*/)
{
	char buf[128], cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(buf);

	// Gain Conversion

		GetPrivateProfileString(m_pName, SET_GAIN_CONVERSION,
				SET_GAIN_CONVERSION_DEFAULT,buf,sizebuf,m_szIniFile);

		TransmitAndUpdateSummary(STOP); //This is necessary 5/5/03 pjm

		strncpy(m_SentParameters.gain_conversion,buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_CONVERSION,buf);
		TransmitAndUpdateSummary(cmd);

		if (m_pDataStatus) //for XYCtrl's x-axis
		{
			UINT gc;
			sscanf(m_SentParameters.gain_conversion,"%u",&gc);
			m_pDataStatus->SetGainConversion(gc);
		}

	// Gain coarse

		GetPrivateProfileString(m_pName, SET_GAIN_COARSE,
			SET_GAIN_COARSE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_coarse,buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_COARSE,buf);
		TransmitAndUpdateSummary(cmd);

	// Gain fine

		GetPrivateProfileString(m_pName, SET_GAIN_FINE,
			SET_GAIN_FINE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_fine,buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_FINE,buf);
		TransmitAndUpdateSummary(cmd);

	//Gain polarity

		GetPrivateProfileString(m_pName, SET_GAIN_POLARITY,
			SET_GAIN_POLARITY_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_polarity,buf,DPLEN);

		if (0 == strcmp(buf, POSITIVE))
			TransmitAndUpdateSummary(SET_GAIN_POSITIVE);
		else
			TransmitAndUpdateSummary(SET_GAIN_NEGATIVE);  // assume negative
}
//////////////////////////////////////////////////////////////////////////////
//
//                         SendStabilizationCommandsToDSPEC
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendStabilizationCommandsToDSPEC(bool /*resetting*/)
{
	char from_ini_buf[128], cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(from_ini_buf);

	//Before GAIN STABILIZATION can be enabled, we set these values:
	//Gauss or Point mode
	//Gain Center Channel
	//Gain Width

	// First step:  Disable gain stabilization before setting any parameters

		TransmitAndUpdateSummary(DISABLE_GAIN_STABILIZATION);

	// Second step:  Set Gauss or Point Mode

		GetPrivateProfileString(m_pName,SET_MODE_STABILIZATION,
				SET_MODE_STABILIZATION_DEFAULT,from_ini_buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.stabilization_mode,from_ini_buf,DPLEN);

		if (0 == strcmp(from_ini_buf, GAUSS))
				TransmitAndUpdateSummary(SET_MODE_GAUSS);
		else
			TransmitAndUpdateSummary(SET_MODE_POINT);  // assume POINT

	// Third step:  Set center channel for the stabilizer gain peak

		GetPrivateProfileString(m_pName,SET_GAIN_CHANNEL,
				SET_GAIN_CHANNEL_DEFAULT,from_ini_buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_channel,from_ini_buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_CHANNEL,from_ini_buf);
		TransmitAndUpdateSummary(cmd);

	// Fourth step:  Set Stabilizer Gain Peak Width

		GetPrivateProfileString(m_pName,SET_GAIN_WIDTH,
				SET_GAIN_WIDTH_DEFAULT,from_ini_buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_width,from_ini_buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_WIDTH,from_ini_buf);
		TransmitAndUpdateSummary(cmd);
/*
	// Extra step: Set stabilization adjustment - (Is this needed?? <<QA>>)

		GetPrivateProfileString(m_pName,SET_GAIN_ADJUSTMENT,SET_GAIN_ADJUSTMENT_DEFAULT,
				from_ini_buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_adjustment,from_ini_buf,DPLEN);
		sprintf(cmd,format,SET_GAIN_ADJUSTMENT,from_ini_buf);
		TransmitAndUpdateSummary(cmd);
*/

	// Last step:  Re-enable gain stabilization (or not depending on ini value)
	// We disabled it at step #1.  Only need to re-enable it or leave it disabled.

		GetPrivateProfileString(m_pName,SET_GAIN_STABILIZATION_ENABLE,
				SET_GAIN_STABILIZATION_ENABLE_DEFAULT,
				from_ini_buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gain_stabilization_enable,from_ini_buf,DPLEN);

		if (0 == strcmp(from_ini_buf, ENABLE))
			TransmitAndUpdateSummary(ENABLE_GAIN_STABILIZATION);
}

//////////////////////////////////////////////////////////////////////////////
//
//                         SendHVCommandsToDSPEC
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendHVCommandsToDSPEC(bool /*resetting*/)
{
	char buf[128], cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(buf);

	//HV Polarity

	GetPrivateProfileString(
		m_pName,SET_HV_POLARITY,SET_HV_POLARITY_DEFAULT,
		buf,sizebuf,m_szIniFile);

	strncpy(m_SentParameters.hv_polarity,buf,DPLEN);

	if (0 == strcmp(buf, POSITIVE))
		TransmitAndUpdateSummary(SET_HV_POSITIVE);
	else
		TransmitAndUpdateSummary(SET_HV_NEGATIVE);  // assume negative

	//HV voltage

	GetPrivateProfileString(
		m_pName,SET_HV,SET_HV_DEFAULT,buf,sizebuf,m_szIniFile);

	strncpy(m_SentParameters.hv,buf,DPLEN);
	sprintf(cmd,format,SET_HV,buf);
	TransmitAndUpdateSummary(cmd);

	//HV enable

	GetPrivateProfileString(m_pName,SET_HV_ENABLE,
		SET_HV_ENABLE_DEFAULT,buf,sizebuf,m_szIniFile);

	strncpy(m_SentParameters.hv_enable,buf,DPLEN);

	if (0 == strcmp(buf, ENABLE))
		TransmitAndUpdateSummary(ENABLE_HV);
	else
		TransmitAndUpdateSummary(DISABLE_HV);
}
//////////////////////////////////////////////////////////////////////////////
//
//                         SendPoleZeroCommandsToDSPEC
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendPoleZeroCommandsToDSPEC(bool /*resetting*/)
{
	char buf[128], cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(buf);

	//Set Pole Zero value

		GetPrivateProfileString(
			m_pName,SET_PZ,SET_PZ_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.pz,buf,DPLEN);
		sprintf(cmd,format,SET_PZ,buf);
		TransmitAndUpdateSummary(cmd);

	//Pole Zero Automatic -> Preamp Type

		GetPrivateProfileString(m_pName,SET_PZ_AUTOMATIC_ENABLE,
				SET_PZ_AUTOMATIC_ENABLE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.pz_automatic,buf,DPLEN);

		if (0 == strcmp(buf, DISABLE))
			TransmitAndUpdateSummary(DISABLE_PZ_AUTOMATIC);
		else
			// assume enabled (resistive feedback)
			TransmitAndUpdateSummary(ENABLE_PZ_AUTOMATIC);

}
//////////////////////////////////////////////////////////////////////////////
//
//                        SendOtherCommandsToDSPEC()
//
/////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SendOtherCommandsToDSPEC(bool /*resetting*/)
{
	char buf[128],cmd[64];
	char format[] = "%s %-s";
	int sizebuf = sizeof(buf);

	//Automatic determination of baseline restorer time constant

		GetPrivateProfileString(m_pName,SET_BLRE_AUTOMATIC_ENABLE,
				SET_BLRE_AUTOMATIC_ENABLE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.blre_automatic_enable,buf,DPLEN);

		if (0 == (strcmp(buf, DISABLE)))
			TransmitAndUpdateSummary(DISABLE_BLRE_AUTOMATIC);
		else
			TransmitAndUpdateSummary(ENABLE_BLRE_AUTOMATIC); //assume enable

	//Shutdown type (TTL or ORTEC)

		GetPrivateProfileString(m_pName,SET_SHUTDOWN,
				SET_SHUTDOWN_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.shutdown,buf,DPLEN);

		if (0 == (strcmp(buf, TTL)))
			TransmitAndUpdateSummary(SET_SHUTDOWN_TTL);
		else
			TransmitAndUpdateSummary(SET_SHUTDOWN_ORTEC); //assume ORTEC

	//Lower level Discriminator

		GetPrivateProfileString(m_pName,SET_LLD,
				SET_LLD_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.lld,buf,DPLEN);
		sprintf(cmd,format,SET_LLD,buf);
		TransmitAndUpdateSummary(cmd);

	//Upper level Discriminator

		GetPrivateProfileString(m_pName,SET_ULD,
				SET_ULD_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.uld,buf,DPLEN);
		sprintf(cmd,format,SET_ULD,buf);
		TransmitAndUpdateSummary(cmd);

	//Zero Dead Time mode

		GetPrivateProfileString(m_pName,SET_ZDT_ENABLE,
				SET_ZDT_ENABLE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.zdt_enable,buf,DPLEN);

		if (0 == (strcmp(buf, ENABLE)))
			TransmitAndUpdateSummary(ENABLE_ZDT);
		else
			TransmitAndUpdateSummary(DISABLE_ZDT); //assume disable

	//ADC gate

		GetPrivateProfileString(m_pName,SET_GATE,
				SET_GATE_DEFAULT,buf,sizebuf,m_szIniFile);

		strncpy(m_SentParameters.gate,buf,DPLEN);

		if (0 == (strcmp(buf, ANTICOINCIDENT)))
			TransmitAndUpdateSummary(SET_GATE_ANTICOINCIDENT);
		else if(0 == (strcmp(buf,COINCIDENT)))
			TransmitAndUpdateSummary(SET_GATE_COINCIDENT);
		else
			TransmitAndUpdateSummary(SET_GATE_OFF);  //assume off
}
//////////////////////////////////////////////////////////////////////////////
//
//              TransmitAndUpdateSummary
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::TransmitAndUpdateSummary(char *command)
{
	COM_Transmit(ISZSTRING, command, m_iNode);

	if (m_pPropertySheet)
	{
		m_pPropertySheet->UpdateSummary(command);

		if (m_bDoDump)
		{
			string thestring = command + string(crlf);
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, thestring.c_str());
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//              AcknowledgeInputFromCSO
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::AcknowledgeInputFromCSO()
{
	//TRACE("CDSPECInst::AcknowledgeInputFromCSO()\n");
	m_iSecondsElapsedSinceReceivingData = 0;
	m_bCommsFailure = false;
}

#ifdef ALLOWTRIGGERHOLDOFF
void
CDSPECInst::AddExclusion(LPARAM)
{
//	COleDateTime cOleStart = COleDateTime::GetCurrentTime();
//	COleDateTime cOleBase(1952,1,1,0,0,0);
//	COleDateTimeSpan cOleJulian;
//	cOleJulian = cOleStart - cOleBase;
//	EXCLUSIONREC ExclusionRec;
//	ExclusionRec.uiStartTime = cOleJulian.GetTotalSeconds();
//	ExclusionRec.uiEndTime = ExclusionRec.uiStartTime + 60;
//	m_ExclusionList.AddTail(ExclusionRec);
}
void
CDSPECInst::DelExclusion()
{
//	//remove all of the exclusion periods
//	m_ExclusionList.RemoveAll();
//	//update the INI file
//	WriteExclusion();
//	//update the button
//	m_pButton->SetPreNum("BBM: ");
//	//stop the exclusion timer
//	KillTimer(TIMER_EXCLUSION);
}
#endif

void CDSPECInst::GetListsFromInstrument()
{
//	TRACE("CDSPECInst(%s)::GetListsFromInstrument()\n", m_pName);

	TransmitAndUpdateSummary(LIST_CORRECTION_FLAT);
	TransmitAndUpdateSummary(LIST_GAIN_COARSE);
	TransmitAndUpdateSummary(LIST_GAIN_CONV);
	TransmitAndUpdateSummary(LIST_GAIN_FINE);
	TransmitAndUpdateSummary(LIST_HV);
	TransmitAndUpdateSummary(LIST_SHAP_CUSP);
	TransmitAndUpdateSummary(LIST_SHAP_FLAT);
	TransmitAndUpdateSummary(LIST_SHAP_RISE);
	TransmitAndUpdateSummary(LIST_SHUTDOWN);
}

bool CDSPECInst::DwellSettingIsShort()
{
	return ( (m_eDwellType == SHORT_DWELL_2) ? true : false );
}
bool CDSPECInst::DwellSettingIsLong()
{
	return ( (m_eDwellType == LONG_DWELL_3) ? true : false );
}
//////////////////////////////////////////////////////////////////////////////
//
//                    OnReceive
//
//////////////////////////////////////////////////////////////////////////////
//
//this is the state machine which controls 
//	what commands to send, 
//	when to send them, and
//	what to expect in return.
//  It also processes "timer" message on intervals selected in "MIC:Configuration"

void CDSPECInst::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//TRACE("CDSPECInst::OnReceive()\n");
	//
	// Always need the time of day, regardless of what state we are in.
	//
	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_iPreviousDay)
	{
		m_iPreviousDay = dCurrentDay;
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,
			"67270 DSPEC Start new day\n");
	}

	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	//<<QA>> need to do more to process this "locked-up" state
	if (!m_bGoodRegister || m_bBlockWrite)
	{
		SetStateAndButtonColor(LOCKED_UP_9900);
	}

	AdjustShutdownAndStartupBooleans();

	if (m_bPause)
	{
		//Don't do anything if paused.  Should not even get here
		//Since all of the timers that post messages to OnReceive
		//are turned off.
		ASSERT(FALSE);
	}
	else if (WParam == IDC_TIMER_TICK)  //Instrument's background timer is always 1 second for DSPEC
	{
		if (m_iStatusInt_sec != 0)
			m_iTickToStatus++;

		//seconds to go for the dwell
		if (m_bInShutDown)
			m_iSecToGo = 0;
		else if (m_iSecToGo > 0)
			m_iSecToGo--;

		m_pButton->put_Numeric((ULONG)m_iSecToGo);

		if (++m_iSecondsElapsedSinceReceivingData >= m_iTimeOut)
		{
			if (!m_bCommsFailure)
			{
				m_pPropertySheet->WriteToFile(
				TO_PFM | TO_CEV, TYPE_COMP, NULL,
				"63370 DSPEC Communications Failure\n");
			}

			m_bCommsFailure = true;
		}

		if (!m_bInShutDown)
		{
			//kick the CSO if there has been no traffic for (m_iTimeOut - 1) seconds
			//maybe there has been no need for data from the DSPEC, so this is a
			//pseudo-heartbeat.

			if (m_iSecondsElapsedSinceReceivingData == m_iTimeOut - 1)
				Query_CSO();

			//process possible failure
			if (m_bCommsFailure||m_bHVVoltageLevelFailure||m_bHVShutdownFailure)
			{
				// not quite dead yet - set button to yellow
				SetStateAndButtonColor(SHORT_TERM_HARD_FAIL_9000);

				//Communications failure is based on one timeout period already elapsed.
				//Other failures are edge triggered.
				if (m_bCommsFailure)
					m_iShortTermTimeouts = 1;
				else
					m_iShortTermTimeouts = 0;

				m_iShortTermTimeoutRunningCounter = 0;
			}

			//Time to update status?
			if ((m_iStatusInt_sec != 0) && (m_iTickToStatus >= m_iStatusInt_sec))
			{
				PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);

				CString cs("68060 DSPEC Periodic Status Request\n");

				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, cs);

				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cs);

				m_iTickToStatus = 0;
			}

		} //end if(!m_bInShutDown)
	} //end WParam == IDC_TIMER_TICK

	else if (WParam == IDC_TIMER_REALTIME_DWELL)  //from our local OnTimer
	{
		//Tell CSO to get the current spectrum for realtime display.

		if (!m_bInShutDown)
		{
			COM_Transmit(ISZSTRING,GET_REALTIME_DWELL_SPECTRUM,m_iNode,m_bRealtimeWatcherDataQuiet);

			if (!m_bRealtimeWatcherDataQuiet)
				m_pPropertySheet->UpdateSummary(GET_REALTIME_DWELL_SPECTRUM);

			m_pPropertySheet->UpdateDataStatus();

			//Throw out any piled up requests for real time dwell buffers
			//PeekMessage.... <<QA>>
		}
	}
	//else if (WParam == IDC_TIMER_ANALYSIS_DWELL)  //from our local OnTimer
	//{
		// Timer tells us it is time to ask for either a long dwell spectrum
		// or a short dwell spectrum.
		// See further processing below in the state machine's switch statement
		// to determine which type of spectrum we want to ask for.
	//}

	AdjustShutdownAndStartupBooleans();  //Do this right before the switch and upon return

	//TRACE("m_dCurrent_State = %d\n", m_dCurrent_State);
	//this is the virtual instrument state machine
	switch (m_dCurrent_State)
	{
		case INITIALIZING_1000:

			StartNewShortDwell(true);
			SetStateAndButtonColor(WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000);
			break;

		case WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000:

			if (WParam == IDC_TIMER_ANALYSIS_DWELL)
			{
				//Time to go get a spectrum buffer
				TransmitAndUpdateSummary(GET_SHORT_DWELL_SPECTRUM);
				SetStateAndButtonColor(EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010);
			}
			break;

		case EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010:

			if (WParam == IDC_DSPEC_SPECTRUM_DATA_PROCESSED) // 21115 from CDSPECSafePropertySheet
			{
				CORTECSpectrum *pspec = (CORTECSpectrum *)LParam;
				if (0  == pspec->ChannelsAboveThreshold())
				{
					StartNewShortDwell(true);             //nothing interesting here, start again
					SetStateAndButtonColor(WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000);
				}
				else
				{
					//Let's go take a more serious look
					//Expand Short Dwell Into Long Dwell
					KillTimer(TIMER_ANALYSIS_DWELL);

					m_eDwellType = LONG_DWELL_3;
					m_iSecToGo = m_iLongdwell_sec - m_iShortdwell_sec;
					m_pButton->put_Numeric((ULONG)m_iSecToGo);

					SetTimer(TIMER_ANALYSIS_DWELL,(m_iSecToGo*1000), NULL);
					SetStateAndButtonColor(WAITING_WITHIN_LONG_DWELL_INTERVAL_3000);
				}
			}
			break;

		case WAITING_WITHIN_LONG_DWELL_INTERVAL_3000:

			if (WParam == IDC_TIMER_ANALYSIS_DWELL)  // from OnTimer()
			{
				//Request Long Dwell Data
				TransmitAndUpdateSummary(GET_LONG_DWELL_SPECTRUM);
				SetStateAndButtonColor(EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010);
			}
			break;

		case EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010:

			if (WParam == IDC_DSPEC_SPECTRUM_DATA_PROCESSED) // from CDSPECSafePropertySheet
			{
				// The property sheet has received and saved the .CHN file.
				// Always go to short dwell after a long dwell, just to do a quick look.
				// If there is interesting data in the short dwell, then this state machine
				// will expand that short dwell back into a long dwell.
				StartNewShortDwell(true);
				SetStateAndButtonColor(WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000);
			}
			break;

		case EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000:

			if (WParam == IDC_DSPEC_SPECTRUM_DATA_PROCESSED) //from CDSPECSafePropertySheet
			{
				CORTECSpectrum *pspec = (CORTECSpectrum *)LParam;
				m_pPropertySheet->WriteToCHNFile(pspec, m_cCurrent_Tag);
				SetStateAndButtonColor(m_dSave_State);
			}
			break;

		case SHORT_TERM_HARD_FAIL_9000:			//9000 and above are "in shutdown"
			if (WParam == IDC_TIMER_TICK)
			{
				//
				// recovered?
				//
				bool bRecovered = 
					!m_bCommsFailure           && 
					!m_bHVVoltageLevelFailure  && 
					!m_bHVShutdownFailure;

				if (bRecovered)
				{
					RecoverFromFailure();
				}

				// not recovered - do some further calculations
				else  
				{
					bool bDoReset = false;
					bool bTransitionToLongTermHardFail = false;

					m_iShortTermTimeoutRunningCounter++;

					if (m_iShortTermTimeoutRunningCounter >= m_iTimeOut)
					{
						m_iShortTermTimeouts++;
						m_iShortTermTimeoutRunningCounter = 0;
						bDoReset = true;
					}

					if (m_iShortTermTimeouts >= m_iCommFail)
					{
						bTransitionToLongTermHardFail = true;
						// No need to do a reset if we are going into hard fail.
						bDoReset = false;
					}

					if (bTransitionToLongTermHardFail)
					{
						KillTimer(TIMER_REALTIME_DWELL);
						KillTimer(TIMER_ANALYSIS_DWELL);
						m_iSlowResetCount_sec = 0;
						SetStateAndButtonColor(LONG_TERM_HARD_FAIL_9500);
						m_pButton->SetErrorIcon_Skull();
						FlagComm(1,true);
					}

					if (bDoReset)
					{
						m_pComm->ResetCSO();
						PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);  // get the new status
					}
				}
			}
			break;

		case LONG_TERM_HARD_FAIL_9500:
			{
				//
				// recovered?
				//
				bool bRecovered = 
					!m_bCommsFailure           && 
					!m_bHVVoltageLevelFailure  && 
					!m_bHVShutdownFailure;

				if (bRecovered)
				{
					RecoverFromFailure();
				}
				else	// not recovered
				{
					FlagComm(1,true);  //put the skull and crossbones back up

					if (m_iSlowResetCount_sec == 0)
					{
						COM_Transmit(ISZSTRING,"INITIALIZE",m_iNode);

						m_pPropertySheet->WriteToFile(
							TO_PFM | TO_CEV, TYPE_COMP, 
							NULL,"63340 DSPEC Sending INITIALIZE command\n");
					}

					m_iSlowResetCount_sec++;
//					TRACE("SRC:%d ",m_iSlowResetCount_sec);
					if (m_iSlowResetCount_sec >= m_iSlowReset_sec)
						m_iSlowResetCount_sec = 0;

					//hardcoded, every minute, try to talk
					static int sixty_seconds_counter;
					if (++sixty_seconds_counter >= 60)
					{
//						TRACE("\nResetting at 60 seconds\n");
						m_pPropertySheet->WriteToFile(
							TO_PFM | TO_CEV, TYPE_COMP, 
							NULL,"63350 Attempting 60-sec reset\n");

						m_pComm->ResetCSO();
						PostMessage(IDC_UPDATE_PARAMS,FALSE,TRUE);	//MIC 1.907a
						PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);  // get the new status
						sixty_seconds_counter = 0;
					}
					TRACE("60 Sec Counter %d\n",sixty_seconds_counter);
				}
				break;
			}
		case LOCKED_UP_9900:
			{
				break;
			}
		default:
			{
				m_pPropertySheet->UpdateSummary("State Machine Failure");
				m_pPropertySheet->WriteToFile(
					TO_PFM | TO_CEV, TYPE_COMP, NULL,"63330 DSPEC State Machine Failure\n");
				break;
			}
	}//end switch

	AdjustShutdownAndStartupBooleans();  //Do this right before the switch and upon return
}
//////////////////////////////////////////////////////////////////////////////
//
//                  RecoverFromFailure
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::RecoverFromFailure()
{
//	TRACE("CDSPECInst(%s)::RecoverFromFailure()\n", m_pName);

	m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, 
			NULL,"63360 DSPEC Recovered\n");

	// If we are recovering from a hard fail, then change skull to flag
	if (m_dCurrent_State == LONG_TERM_HARD_FAIL_9500)
	{
		m_pButton->SetErrorIcon_Flag();
		FlagComm(1,true);
	}

	//MIC 1.907a change
	//PostMessage(IDC_UPDATE_PARAMS, FALSE, 0);	//Reapply the ini values to the reset DSPEC
	PostMessage(IDC_UPDATE_PARAMS, FALSE, TRUE);	//Reapply the ini values to the reset DSPEC
	PostMessage(IDC_CLEAR_AND_RESTART_SPECTRUM,0,0);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  OnClearAndRestartSpectrum
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnClearAndRestartSpectrum(WPARAM /*w*/,LPARAM /*l*/)
{
	StartNewShortDwell(true);
	SetStateAndButtonColor(WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  OnManualWriteToCHNFile
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnManualWriteToCHNFile(WPARAM,LPARAM)
{
//	TRACE("CDSPECInst::OnManualWriteToCHNFile()\n");

	TransmitAndUpdateSummary(GET_IMMEDIATE_DWELL_SPECTRUM);
	m_cCurrent_Tag = 'M';

	//The active state gets saved off and restored after the spectrum
	//is collected.

	m_dSave_State = m_dCurrent_State;
	SetStateAndButtonColor(EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  StartNewShortDwell
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::StartNewShortDwell(bool resetmemory)
{
//	TRACE("CDSPECInst(%s)::StartNewShortDwell(resetmemory = %d)\n", m_pName, int(resetmemory));

	if (resetmemory)  //do not reset when coming out of a pause interval?
		m_pComm->ResetCSO();

	BOOL b;
	b = KillTimer(TIMER_ANALYSIS_DWELL);
	b = KillTimer(TIMER_REALTIME_DWELL);
	b = KillTimer(TIMER_TICK);

	m_eDwellType = SHORT_DWELL_2;
	m_iSecToGo   = m_iShortdwell_sec;
	m_pButton->put_Numeric((ULONG)m_iSecToGo);

	UINT u;
	u = SetTimer(TIMER_ANALYSIS_DWELL,(m_iShortdwell_sec * 1000),NULL);
	u = SetTimer(TIMER_REALTIME_DWELL,m_iRealtimedwell_sec*1000,NULL);
	u = SetTimer(TIMER_TICK,m_iCycleInt_ms,NULL);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  AdjustShutdownAndStartupBooleans
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::AdjustShutdownAndStartupBooleans()
{
	if (m_dCurrent_State < SHORT_TERM_HARD_FAIL_9000)
		m_bInShutDown = false;
	else
		m_bInShutDown = true;

	if (m_dCurrent_State >= FIRST_NON_STARTUP_STATE_1100)
		m_bInStartUp = false;
	else
		m_bInStartUp = true;
}

//////////////////////////////////////////////////////////////////////////////
//
//                   OnDesiredViceCurrentMismatch
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnDesiredViceCurrentMismatch(WPARAM,LPARAM)
{
	m_pPropertySheet->SetTabColor(IDC_INSTSETUP_TAB,COLOR_RED);
}
//////////////////////////////////////////////////////////////////////////////
//
//                   OnRequestSettingsUpdates
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnRequestSettingsUpdates(WPARAM ignore_pause_state, LPARAM /*l*/)
{
//	TRACE("CDSPECInst(%s)::OnRequestSettingsUpdates()\n",m_pName);

	if (!m_bPause || ignore_pause_state)
	{
		//In alphabetical order

		TransmitAndUpdateSummary(SHOW_BLRE_AUTOMATIC);
		TransmitAndUpdateSummary(SHOW_CORRECTION_FLAT);
		TransmitAndUpdateSummary(SHOW_GAIN_ADJUSTMENT);
		TransmitAndUpdateSummary(SHOW_GAIN_CHANNEL);
		TransmitAndUpdateSummary(SHOW_GAIN_COARSE);
		TransmitAndUpdateSummary(SHOW_GAIN_CONVERSION);
		TransmitAndUpdateSummary(SHOW_GAIN_FINE);
		TransmitAndUpdateSummary(SHOW_GAIN_POLARITY);
		TransmitAndUpdateSummary(SHOW_GAIN_STABILIZATION);
		TransmitAndUpdateSummary(SHOW_GAIN_WIDTH);
		TransmitAndUpdateSummary(SHOW_GATE);
		TransmitAndUpdateSummary(SHOW_HV);
		TransmitAndUpdateSummary(SHOW_HV_POLARITY);
		TransmitAndUpdateSummary(SHOW_LLD);
		TransmitAndUpdateSummary(SHOW_SHUTDOWN);
		TransmitAndUpdateSummary(SHOW_MODE_STABILIZATION);
		TransmitAndUpdateSummary(SHOW_NETWORK_ID);
		TransmitAndUpdateSummary(SHOW_PZ);
		TransmitAndUpdateSummary(SHOW_PZ_AUTOMATIC);
		TransmitAndUpdateSummary(SHOW_SHAP_CUSP);
		TransmitAndUpdateSummary(SHOW_SHAP_FLAT);
		TransmitAndUpdateSummary(SHOW_SHAP_RISE);
		TransmitAndUpdateSummary(SHOW_SHUT_ACTUAL);
		TransmitAndUpdateSummary(SHOW_ULD);
		TransmitAndUpdateSummary(SHOW_ZDT);

		// Establish time for "latest update of reported values"
		m_cLatestReportedColumnUpdate = COleDateTime::GetCurrentTime();
		//<<QA>> Do the "elapsed time since reported were refreshed" static text
		//<<QA>> in OnTimer
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                   UnRegInstrument
//
//////////////////////////////////////////////////////////////////////////////
//disconnect this instrument from the communications object
//it is registered with
bool CDSPECInst::UnRegInstrument()
{
	if (m_pComm && m_bGoodRegister)
		m_pComm->UnRegisterISO(m_iNode,m_eCategory);

	if (IsWindow(m_hWnd))
		KillAllTimers();

	return true;
}
//////////////////////////////////////////////////////////////////////////////
//
//                   SetStateAndButtonColor
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::SetStateAndButtonColor(DSPEC_STATE ds)
{
//	TRACE("CDSPECInst(%s)::SetStateAndButtonColor(%d)\n", m_pName, int(ds));

	bool bChange = false;

	if (m_dCurrent_State != ds)
	{
		bChange = true;
		m_dCurrent_State = ds;
	}

	if (m_pPropertySheet && bChange)
	{
		CString dcs(" DSPEC Change State: ");
		CString cs;

		switch(ds)
		{
		case INITIALIZING_1000:                             cs = "62000" + dcs + "INITIALIZING_1000";                         break;
		case FIRST_NON_STARTUP_STATE_1100:                  cs = "620xx" + dcs + "FIRST_NON_STARTUP_STATE_1100";              break;
		case WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000:      cs = "62010" + dcs + "WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000";  break;
		case EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010:      cs = "62020" + dcs + "EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010";  break;
		case WAITING_WITHIN_LONG_DWELL_INTERVAL_3000:       cs = "62030" + dcs + "WAITING_WITHIN_LONG_DWELL_INTERVAL_3000";   break;
		case EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010:       cs = "62040" + dcs + "EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010";   break;
		case EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000:		cs = "62050" + dcs + "EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000"; break;
		case SHORT_TERM_HARD_FAIL_9000:                     cs = "62060" + dcs + "SHORT_TERM_HARD_FAIL_9000";                 break;
		case LONG_TERM_HARD_FAIL_9500:                      cs = "62070" + dcs + "LONG_TERM_HARD_FAIL_9500";                  break;
		case LOCKED_UP_9900:                                cs = "62080" + dcs + "LOCKED_UP_9900";                            break;

		default: 
			cs = "62100" + dcs + "Unknown State";
		}

		cs += crlf;

		//dgp added per TW 31 July 2007
		if ((ds != WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000) && (ds != EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010))
			m_pPropertySheet->WriteToFile( TO_PFM, TYPE_COMP, NULL, cs);

		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cs);
	}

	if (!m_bPause)
	{
		if (m_dCurrent_State < SHORT_TERM_HARD_FAIL_9000)
		{
			//GREEN
			m_pButton->put_FaceColor((ULONG)COLOR_OK);
			m_pPropertySheet->SetButtonColor(COLOR_OK);
		}
		else if (m_dCurrent_State == SHORT_TERM_HARD_FAIL_9000)
		{
			//YELLOW
			m_pButton->put_FaceColor((ULONG)COLOR_WAIT);
			m_pPropertySheet->SetButtonColor(COLOR_WAIT);
		}
		else if (m_dCurrent_State >= LONG_TERM_HARD_FAIL_9500)
		{
			//RED
			m_pButton->put_FaceColor((ULONG)COLOR_ERROR);
			m_pPropertySheet->SetButtonColor(COLOR_ERROR);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                   Query_CSO
//
//////////////////////////////////////////////////////////////////////////////
//
// The purpose of this function is to force a response out of the DSPEC.
// The message SHOW_NETWORK_ID is used because the reply is short.  The 
// value of the reply is not significant.
//
void CDSPECInst::Query_CSO()
{
	static unsigned int querycount;
	char String[64];
	
	sprintf(String,"%s Query_CSO %u", SHOW_NETWORK_ID, ++querycount);

	COM_Transmit(ISZSTRING, SHOW_NETWORK_ID, m_iNode);

	m_pPropertySheet->UpdateSummary(String);

	if (m_bDoDump)
	{
		strcat(String, crlf);
		m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, String);
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                   OnHVVoltageLevelFail
//                   OnHVVoltageLevelRecover
//                   OnHVShutdown
//                   OnHVShutdownRecover
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::OnHVVoltageLevelFail(WPARAM /*wparam*/,LPARAM /*lparam*/)
{
//	TRACE("CDSPECInst::OnHVVoltageLevelFail()\n");
	m_bHVVoltageLevelFailure = true;
}

void CDSPECInst::OnHVVoltageLevelRecover(WPARAM /*wparam*/,LPARAM /*lparam*/)
{
//	TRACE("CDSPECInst::OnHVVoltageLevelRecover()\n");
	m_bHVVoltageLevelFailure = false;
}

void CDSPECInst::OnHVShutdown(WPARAM /*wparam*/,LPARAM /*lparam*/)
{
//	TRACE("CDSPECInst::OnHVShutdown()\n");
	m_bHVShutdownFailure = true;
}

void CDSPECInst::OnHVShutdownRecover(WPARAM /*wparam*/,LPARAM /*lparam*/)
{
//	TRACE("CDSPECInst::OnHVShutdownRecover()\n");
	m_bHVShutdownFailure = false;
}
//////////////////////////////////////////////////////////////////////////////
//
//                        KillAllTimers
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECInst::KillAllTimers() 
{
	KillTimer(TIMER_PAUSE);
	KillTimer(TIMER_TICK);
	KillTimer(TIMER_REALTIME_DWELL);
	KillTimer(TIMER_ANALYSIS_DWELL);
}
