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
// SafePS.cpp
// 
// User: Moore        Date: 1/25/06    Time: 2:38p
// Updated in $/mic 2.001/CoGrandISO
// DP's changes for SCR00235 Processing of 4F DUMPOKRESPONSE.
// Changed version number to 2.001 in the resource file.
// 
// User: Moore        Date: 6/14/05    Time: 4:12p
// Updated in $/MIC 2.0/CoGrandISO
// FIXED 6/14/2005 GRAND ISO: Corrected display 
// of Rel/+;Rel/-;Abs/+;Abs/-.  
// Meaning of the two bits was swapped from what I thought.
// 
// User: Moore        Date: 5/06/05    Time: 10:22a
// Updated in $/MIC 2.0/CoGrandISO
// Added code to fix  DR MIC 0011.  This problem crashes MIC
// 1.9.0.7 and I put it in so that testing MIC GM would not be effected.
// 
#include <sys/types.h>
#include <sys/stat.h>

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "DataWatchDlg.h"
#include "SafePS.h"
#include "Instrument.h"
#include "GrandSetupData.h"
#include "resource.h"
#include "invalid_chars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSafePropertySheet

IMPLEMENT_DYNAMIC(CSafePropertySheet, CPropertySheet)

int CSafePropertySheet::ms_iGRANDPropertySheetCount = 0;

CSafePropertySheet::CSafePropertySheet()
{
}

CSafePropertySheet::CSafePropertySheet(
	UINT nIDCaption, 
	CInstrument* pParentWnd, 
	char *inifile)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CSafePropertySheet::CTOR a\n");
	m_pCaption[0] = NULL;
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishConstruction();
}

CSafePropertySheet::CSafePropertySheet(
	LPCTSTR pszCaption, 
	CInstrument* pParentWnd,
	char *inifile)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CSafePropertySheet::CTOR b\n");
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishConstruction();
}

void CSafePropertySheet::FinishConstruction()
{
//	TRACE("CSafePropertySheet::FinishConstruction\n");
	ms_iGRANDPropertySheetCount++;

	m_eMonitorVers = UNKNOWN;
	m_bUpdatingFromFile = false;
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
	m_pDataWatchDlg = NULL;
	m_bDataWatchDlgOpen = false;
	m_bFirst15Rec	= true;

	m_fLowBV = m_fCurBV = m_fHighBV = 0.0;
	m_bFirstBV = true;
	m_fLowP15 = m_fCurP15 = m_fHighP15 = 0.0;
	m_bFirstP15 = true;
	m_fLowM15 = m_fCurM15 = m_fHighM15 = 0.0;
	m_bFirstM15 = true;
	m_fLowP5 = m_fCurP5 = m_fHighP5 = 0.0;
	m_bFirstP5 = true;

	m_dToClose				= 0;

//	m_bStartOfFile			= true;
	m_dCWStart				= -1; //neither COLD START (0) nor WARM START (1)

	m_szCurrentFileName[0]	= NULL;
	m_szPreviousGTime[0]	= NULL;
	m_dCurrentDay			= -1;
	m_dPreviousBIDTime		= 0xa5a5a5a5;

	m_dBinaryChecksum		= 0;
	m_dBinaryState			= 0;
	m_dBinaryAT				= 0;
	m_usLength				= 0;
	m_usMessageType			= 0;
	m_dRecLengthCount		= 0;
	m_dGivenChecksum		= 0;
	m_dChecksum				= 0;

	m_bCouldNotOpenBinaryFile = false;  ///<<QA>> not in 1.9.0.7.  in 028.  needed?

	m_bFCHVError			= false;
	m_bICHVError			= false;
	m_iFCHVError			= 0;
	m_iICHVError			= 0;
}

COLORREF
CSafePropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
	return m_pCloseButton->SetFaceColor(color);
}

void
CSafePropertySheet::SetHLVoltageDisplay(
	float batvolth, float batvoltl, bool bFirstBV,
	float sup_p15h, float sup_p15l, bool bFirstP15,
	float sup_m15h, float sup_m15l, bool bFirstM15,
	float sup_p05h, float sup_p05l, bool bFirstP5)
{
	char temp[16];

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;

	if (!bFirstBV)
	{
		m_fLowBV = batvoltl;
		m_fHighBV = batvolth;
		m_bFirstBV = false;

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);
	}

	if (!bFirstP5)
	{
		m_fLowP5 = sup_p05l;
		m_fHighP5 = sup_p05h;
		m_bFirstP5 = false;

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMAX,temp);
	}

	if (!bFirstP15)
	{
		m_fLowP15 = sup_p15l;
		m_fHighP15 = sup_p15h;
		m_bFirstP15 = false;

		sprintf(temp,"%5.1f",m_fLowP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMAX,temp);
	}

	if (!bFirstM15)
	{
		m_fLowM15 = sup_m15l;
		m_fHighM15 = sup_m15h;
		m_bFirstM15 = false;

		sprintf(temp,"%5.1f",m_fLowM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMAX,temp);
	}
}

void
CSafePropertySheet::SetLimits(
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

CSafePropertySheet::~CSafePropertySheet()
{
	ms_iGRANDPropertySheetCount--;

	m_pParent->m_pPropertySheet = NULL;

	delete m_pParent->m_pSummaryPage;
	delete m_pParent->m_pGeneralStatPage;
	delete m_pParent->m_pGeneral4StatPage;
	delete m_pParent->m_pDataStatus;
	delete m_pParent->m_pUserProgStatus;
	delete m_pParent->m_pUser4ProgStatus;
	delete m_pParent->m_pCDData;
	delete m_pParent->m_pModParams;
	delete m_pParent->m_pSetupData;
	delete m_pParent->m_pSetup4Data;
	delete m_pParent->m_pLegend;

	m_pParent->m_pSummaryPage = NULL;
	m_pParent->m_pGeneralStatPage = NULL;
	m_pParent->m_pGeneral4StatPage = NULL;
	m_pParent->m_pDataStatus = NULL;
	m_pParent->m_pUserProgStatus = NULL;
	m_pParent->m_pUser4ProgStatus = NULL;
	m_pParent->m_pCDData = NULL;
	m_pParent->m_pModParams = NULL;
	m_pParent->m_pSetupData = NULL;
	m_pParent->m_pSetup4Data = NULL;
	m_pParent->m_pLegend = NULL;

	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;

	delete m_pDataWatchDlg;
}

BEGIN_MESSAGE_MAP(CSafePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSafePropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,	OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,	OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,	OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,	OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,		OnToCloseButton)
	ON_MESSAGE(IDC_EXTENDCLOSE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnExtendClose)
END_MESSAGE_MAP()
//	ON_MESSAGE(IDC_RECEIVE_OOB,		OnReceiveOOB)
//	ON_MESSAGE(IDC_RECEIVE,			OnReceive)

/////////////////////////////////////////////////////////////////////////////
// CSafePropertySheet message handlers
void CSafePropertySheet::UpdateSummary(char *Message)
{
	CString Data;
	bool bChange = false;
	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;

	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_SUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_SUMMARY_COMMAND1,Message);

	if (strstr(Message,"LONG BREAK SET"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		m_cCommStatus = "Comm Status Unknown";
		SetChecksumDisplay();
		bChange = true;
	}

	if (strstr(Message,"LOCAL LONG BREAK"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Local Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		SetChecksumDisplay();
		bChange = true;
	}

	if (bChange)
	{
		if (GetPageIndex(generalpage) >= 0)
		{
			m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
			if (m_pParent->m_iGeneral_Tab == 0x0003)
				SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral_Tab == 0x0002)
				SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
			else if (m_iGeneral_Tab == 0x0001)
				SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
			else if (m_iGeneral_Tab == 0x0000)
				SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);
		}

		if (GetPageIndex(general4page) >= 0)
		{
			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
			if (m_pParent->m_iGeneral4_Tab == 0x0003)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral4_Tab == 0x0002)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
			else if (m_iGeneral4_Tab == 0x0001)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
			else if (m_iGeneral4_Tab == 0x0000)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
		}
	}
}

void CSafePropertySheet::SetChecksumDisplay()
{
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CString cString;
	cString = m_cCheckSumMsg;;
	cString += " ";
	cString += m_cCommStatus;
	if (GetPageIndex(generalpage)>=0)
		generalpage->SetDlgItemText(IDC_GENERAL_CHECKSUM,cString);
	if (GetPageIndex(general4page)>=0) 
		general4page->SetDlgItemText(IDC_GENERAL4_CHECKSUM,cString);
}

void CSafePropertySheet::Reverse(BYTE *sz, int iLength)
{
	BYTE cTemp;
	for (int i = 0; i < (iLength/2); i++)
	{
		cTemp = sz[i];
		sz[i] = sz[iLength - i - 1];
		sz[iLength - i - 1] = cTemp;
	}
}

int CSafePropertySheet::AppendCheckSum(char *pBinaryAt)
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

void CSafePropertySheet::AdjustTabsTo(enum MONVER eMonitorVers)
{

//NEED TO SET M_PPARENT->XXXXX to NULL if WE DELETE IT  //<<QA>> DR MIC 0011


//	CSummary*  summarypage		= m_pParent->m_pSummaryPage;
	CGenlStat* generalpage		= m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page	= m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage			= m_pParent->m_pDataStatus;
	CUserProg* userpage			= m_pParent->m_pUserProgStatus;
	CUser4Prog* user4page		= m_pParent->m_pUser4ProgStatus;
	CModParam* modparpage		= m_pParent->m_pModParams;
	CSetupData* setdatapage		= m_pParent->m_pSetupData;
	CSetupData* setdata4page	= m_pParent->m_pSetup4Data;
	CLegend* legend				= m_pParent->m_pLegend;
/*
		m_pSummaryPage->StaticColors = SummaryStaticColors;
		m_pGeneralStatPage->StaticColors = GeneralStaticColors;
		m_pGeneral4StatPage->StaticColors = General4StaticColors;
		m_pDataStatus->StaticColors = DataStaticColors;
		m_pUserProgStatus->StaticColors = UserStaticColors;
		m_pUser4ProgStatus->StaticColors = User4StaticColors;
*/
	//do nothing if we aren't really changing
	if (eMonitorVers == m_eMonitorVers)
		return;

	//it seems to take a lot of time for windows to get
	//caught up to what is about to be commanded so set it now
	//so that it can exit quickly in the if above.
	enum MONVER eCurrent = m_eMonitorVers;
	m_eMonitorVers = eMonitorVers;

	//do we need property sheet to rebuild some of the windows
	bool bReDisplay = false;
	bool bChangeDisp = false;

	//which one is currently displayed
	int iCurrent = GetActiveIndex();
	
	if (eMonitorVers == PRE4)
	{
		//switch to monitor version pre 4 displays
		switch (eCurrent) 
		{
		case UNKNOWN:
			//hide the post4 tabs
			RemovePage(setdata4page);
//DR MIC 0011
delete setdata4page;
m_pParent->m_pSetup4Data = NULL;

			RemovePage(general4page);
//DR MIC 0011
delete general4page;
m_pParent->m_pGeneral4StatPage = NULL;

			RemovePage(user4page);
//DR MIC 0011
delete user4page;
m_pParent->m_pUser4ProgStatus = NULL;

			//make up for general4 not there
			if (iCurrent > 1) iCurrent--;
			//make up for setdata4 not there
			if (iCurrent > 3) iCurrent--;
			//make up for setdata4 not there
			if (iCurrent > 5) iCurrent--;
			bChangeDisp = true;
			break;

		case POST4:
			//hide the post 4 tabs
			RemovePage(legend);
			//DR MIC 0011
			delete legend;
			m_pParent->m_pLegend = NULL;

			RemovePage(modparpage);
			//DR MIC 0011
			delete modparpage;
			m_pParent->m_pModParams = NULL;

			RemovePage(setdata4page);
			//DR MIC 0011
			delete setdata4page;
			m_pParent->m_pSetup4Data = NULL;

			RemovePage(user4page);
			//DR MIC 0011
			delete user4page;
			m_pParent->m_pUser4ProgStatus = NULL;

			RemovePage(datapage);
			//DR MIC 0011
			delete datapage;
			m_pParent->m_pDataStatus = NULL;

			RemovePage(general4page);
			//DR MIC 0011
			delete general4page;
			m_pParent->m_pGeneral4StatPage = NULL;

			//show the pre 4 tabs
			
			//AddPage(generalpage);
			delete generalpage;
			generalpage = new CGenlStat(IDD_GENERALSTAT_PAGE);
			m_pParent->m_pGeneralStatPage = generalpage;
			generalpage->StaticColors = m_pParent->GeneralStaticColors;
			AddPage(generalpage);
			generalpage->SetPropertySheet(this);
			SetActivePage(generalpage);

			m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
			if (m_pParent->m_iGeneral_Tab == 0x0003)
				SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral_Tab == 0x0002)
				SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
			else if (m_iGeneral_Tab == 0x0001)
				SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
			else if (m_iGeneral_Tab == 0x0000)
				SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

			AddPage(datapage);
			SetActivePage(datapage);
			m_iData_Tab = m_pParent->m_iData_Tab;
			if (m_pParent->m_iData_Tab == 0x0003)
				SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
			else if (m_iData_Tab == 0x0002)
				SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
			else if (m_iData_Tab == 0x0001)
				SetTabColor(IDC_DATA_TAB,COLOR_RED);
			else if (m_iData_Tab == 0x0000)
				SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

			delete userpage;
			userpage = new CUserProg(IDD_USERPROGSTAT_PAGE);
			m_pParent->m_pUserProgStatus = userpage;
			userpage->StaticColors = m_pParent->UserStaticColors;
			AddPage(userpage);

			SetActivePage(userpage);
			m_iUserProg_Tab = m_pParent->m_iUserProg_Tab;
			if (m_pParent->m_iUserProg_Tab == 0x0003)
				SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOWRED);
			else if (m_iUserProg_Tab == 0x0002)
				SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOW);
			else if (m_iUserProg_Tab == 0x0001)
				SetTabColor(IDC_USERPROG_TAB,COLOR_RED);
			else if (m_iUserProg_Tab == 0x0000)
				SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);

			//AddPage(setdatapage);
			delete setdatapage;
			setdatapage = new CSetupData(this, m_szIniFile);
			m_pParent->m_pSetupData = setdatapage;
			setdatapage->SetVersion("SET_");
			AddPage(setdatapage);
			setdatapage->SetPropertySheet(this,m_pParent->m_pName);
			SetActivePage(setdatapage);
			m_iInstSetup_Tab = m_pParent->m_iInstSetup_Tab;
			if (m_pParent->m_iInstSetup_Tab == 0x0003)
				SetTabColor(IDC_INSTSETUP_TAB,COLOR_YELLOWRED);
			else if (m_iInstSetup_Tab == 0x0002)
				SetTabColor(IDC_INSTSETUP_TAB,COLOR_YELLOW);
			else if (m_iInstSetup_Tab == 0x0001)
				SetTabColor(IDC_INSTSETUP_TAB,COLOR_RED);
			else if (m_iInstSetup_Tab == 0x0000)
				SetTabColor(IDC_INSTSETUP_TAB,COLOR_GREEN);

			//AddPage(modparpage);
			delete modparpage;
			modparpage = new CModParam(
				IDD_MODPARAMS_PAGE,
				m_pParent,//this, 
				m_szIniFile, 
				false);

			m_pParent->m_pModParams = modparpage;
			AddPage(modparpage);
			modparpage->SetPropertySheet(this,m_pParent->m_pName);
			SetActivePage(modparpage);

			//AddPage(legend);
			delete legend;
			legend = new CLegend(/*IDD_PROPPAGE_LEGEND*/);
			m_pParent->m_pLegend = legend;
			AddPage(legend);
			SetActivePage(legend);

			CGrandSetupData cGrandSetupData;
			char szTemp[64];
			char *pszDescription;
			char *pszIniName;
			char *pszDefault;
			char *pszCommand;
			while ((pszDescription = cGrandSetupData.GetNext())!=NULL)
			{
				pszIniName = cGrandSetupData.GetNext();
				pszDefault = cGrandSetupData.GetNext();
				pszCommand = cGrandSetupData.GetNext();
				GetPrivateProfileString(m_pParent->m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
				modparpage->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
				setdatapage->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			}

			modparpage->OnModparamsReset();
			
			bReDisplay = true;
			bChangeDisp = true;
			break;
		}
	}
	else if (eMonitorVers == POST4)
	{
		//switch to monitor version 4+ displays
		switch (eCurrent) 
		{
		case UNKNOWN:
			//hide the pre4 tabs
			RemovePage(setdatapage);
			//DR MIC 0011
			delete setdatapage;
			m_pParent->m_pSetupData = NULL;

			RemovePage(generalpage);
			//<<QA>> DR MIC 0011
			delete generalpage;
			m_pParent->m_pGeneralStatPage = NULL;

			RemovePage(userpage);
			//<<QA>> DR MIC 0011
			delete userpage;
			m_pParent->m_pUserProgStatus = NULL;

			//make up for general not there
			if (iCurrent > 1) iCurrent--;
			//make up for user not there
			if (iCurrent > 3) iCurrent--;
			//make up for setdata not there
			if (iCurrent > 5) iCurrent--;
			bChangeDisp = true;
			break;

		case PRE4:
			//hide the pre 4 tabs
			RemovePage(legend);
			//<<QA>> DR MIC 0011
			delete legend;
			m_pParent->m_pLegend = NULL;

			RemovePage(modparpage);
			//<<QA>> DR MIC 0011
			delete modparpage;
			m_pParent->m_pModParams = NULL;

			RemovePage(setdatapage);
			//<<QA>> DR MIC 0011
			delete setdatapage;
			m_pParent->m_pSetupData = NULL;

			RemovePage(userpage);
			//<<QA>> DR MIC 0011
			delete userpage;
			m_pParent->m_pUserProgStatus = NULL;

			RemovePage(datapage);
			//<<QA>> DR MIC 0011
			delete datapage;
			m_pParent->m_pDataStatus = NULL;

			RemovePage(generalpage);
			//<<QA>> DR MIC 0011
			delete generalpage;
			m_pParent->m_pGeneralStatPage = NULL;

			//show the post 4 tabs
			delete general4page;
			general4page = new CGenl4Stat(IDD_GENERAL4STAT_PAGE);
			m_pParent->m_pGeneral4StatPage = general4page;
			general4page->StaticColors = m_pParent->General4StaticColors;
			AddPage(general4page);
			general4page->SetPropertySheet(this);
			SetActivePage(general4page);
			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
			if (m_iGeneral4_Tab == 0x0003)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral4_Tab == 0x0002)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
			else if (m_iGeneral4_Tab == 0x0001)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
			else if (m_iGeneral4_Tab == 0x0000)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);

			AddPage(datapage);
			SetActivePage(datapage);
			m_iData_Tab = m_pParent->m_iData_Tab;
			if (m_iData_Tab == 0x0003)
				SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
			else if (m_iData_Tab == 0x0002)
				SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
			else if (m_iData_Tab == 0x0001)
				SetTabColor(IDC_DATA_TAB,COLOR_RED);
			else if (m_iData_Tab == 0x0000)
				SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

			delete user4page;
			user4page = new CUser4Prog(IDD_USER4PROGSTAT_PAGE);
			m_pParent->m_pUser4ProgStatus = user4page;
			user4page->StaticColors = m_pParent->User4StaticColors;
			AddPage(user4page);
			SetActivePage(user4page);
			m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
			if (m_iUser4Prog_Tab == 0x0003)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
			else if (m_iUser4Prog_Tab == 0x0002)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
			else if (m_iUser4Prog_Tab == 0x0001)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
			else if (m_iUser4Prog_Tab == 0x0000)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);

			delete setdata4page;
			setdata4page = new CSetupData(this, m_szIniFile);
			m_pParent->m_pSetup4Data = setdata4page;
			setdata4page->SetVersion("SET4_");
			AddPage(setdata4page);
			setdata4page->SetPropertySheet(this,m_pParent->m_pName);
			SetActivePage(setdata4page);
			m_iInst4Setup_Tab = m_pParent->m_iInst4Setup_Tab;
			if (m_iInst4Setup_Tab == 0x0003)
				SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
			else if (m_iInst4Setup_Tab == 0x0002)
				SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
			else if (m_iInst4Setup_Tab == 0x0001)
				SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
			else if (m_iInst4Setup_Tab == 0x0000)
				SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);

			delete modparpage;
			modparpage = new CModParam(
				IDD_MODPARAMS_PAGE,
				m_pParent,//this, 
				m_szIniFile, 
				false);
			m_pParent->m_pModParams = modparpage;
			AddPage(modparpage);
			modparpage->SetPropertySheet(this,m_pParent->m_pName);
			SetActivePage(modparpage);

			delete legend;
			legend = new CLegend(/*IDD_PROPPAGE_LEGEND*/);
			m_pParent->m_pLegend = legend;
			AddPage(legend);
			SetActivePage(legend);

			{
				CGrandSetupData cGrandSetupData;
				char szTemp[64];
				char *pszDescription;
				char *pszIniName;
				char *pszDefault;
				char *pszCommand;
				while ((pszDescription = cGrandSetupData.GetNewNext())!=NULL)
				{
					pszIniName = cGrandSetupData.GetNewNext();
					pszDefault = cGrandSetupData.GetNewNext();
					pszCommand = cGrandSetupData.GetNewNext();
					GetPrivateProfileString(m_pParent->m_pName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
					modparpage->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
					setdata4page->AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
				}
			}
			modparpage->OnModparamsReset();

			bReDisplay = true;
			bChangeDisp = true;
			break;
		}
	}

	//return to the previously displayed tab
	//if possible else go to associated
	SetActivePage(iCurrent);
}
/*
//record 12
int CSafePropertySheet::BinaryStatusRecOld(BYTE *pBinaryAt)
{
	AdjustTabsTo(PRE4);

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CModParam* modparpage  = m_pParent->m_pModParams;
	CSetupData* setdatapage = m_pParent->m_pSetupData;


#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		UCHAR acq_mode;			//1
		short g_gain1;			//2
		short g_gain2;			//2
		float n_gain;			//4
		UCHAR lld_a;			//1
		UCHAR uld_a;			//1
		UCHAR lld_b;			//1
		UCHAR uld_b;			//1
		float fission_bias;		//4
		float ion_bias;			//4
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} *psBinaryRecord;			//44 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
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

	char temp[64];
	//get julian time and date
	CJulianTime Julian;
	Julian.Init(psBinaryRecord->julian);

	//build date/time string
	strcpy(temp,Julian.Date());
	strcat(temp," ");
	strcat(temp,Julian.Time());
	generalpage->SetDlgItemText(IDC_GENERAL_TIME,temp);
	
	if (psBinaryRecord->status & ACPOWER)
		strcpy(temp,FAIL);
	else 
		strcpy(temp,OK);

	//ac power changed since last status record
	if (psBinaryRecord->status & ACPOWERCH)
	{
		strcat(temp," with loss since last status record");
		
		if (!m_bACPowerLoss)
			m_bACPowerLoss = true;
	}
	else
		m_bACPowerLoss = false;

	datapage->SetDlgItemText(IDC_DATA_ACPOWER,temp);
	summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,temp);
	
	//bit 6 battery 
	if (psBinaryRecord->status & BATTERY) 
	{
		datapage->SetDlgItemText(IDC_DATA_BATTERYOK,LOW);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,LOW);
		m_bBatteryOk = false;
	}
	else
	{
		datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
		m_bBatteryOk = true;
	}

	char temp2[32];
	bool bMatch;
	temp2[0] = NULL;
	sprintf(temp,"%02X",psBinaryRecord->acq_mode);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2)!=0)
		bMatch = false;
	CurrentSetParam("SET_ACQ_MODE12",temp,bMatch);

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
	generalpage->SetDlgItemText(IDC_GENERAL_MODE,temp);

	//gain 1
	sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain1*2)));
	generalpage->SetDlgItemText(IDC_GENERAL_GAMMA1G,temp);

	//gain 2
	sprintf(temp,"%d", (1 << (psBinaryRecord->g_gain2*2)));
	generalpage->SetDlgItemText(IDC_GENERAL_GAMMA2G,temp);

	//neutron gain
	generalpage->GetDlgItem(IDC_GENERAL_ANALOG)->ShowWindow(SW_SHOW);
	sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
	generalpage->SetDlgItemText(IDC_GENERAL_ANALOG,temp);

	//lld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->lld_a);
	generalpage->SetDlgItemText(IDC_GENERAL_LLDA,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_LLD_A",temp,bMatch);
	
	//uld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->uld_a);
	generalpage->SetDlgItemText(IDC_GENERAL_WINDOWA,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	modparpage->CurrentSetParam("SET_ULD_A",temp,bMatch);
	setdatapage->CurrentSetParam("SET_ULD_A",temp,bMatch);

	//lld_b
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->lld_b);
	generalpage->SetDlgItemText(IDC_GENERAL_LLDB,temp);

	GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_LLD_B",temp,bMatch);
	
	//uld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->uld_b);
	generalpage->SetDlgItemText(IDC_GENERAL_WINDOWB,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_ULD_B",temp,bMatch);
	
	float Current;
	Current = psBinaryRecord->fission_bias;
	if (m_bFBFirst)
	{
		m_fFBHigh = m_fFBLow = Current;
		m_bFBFirst = false;
	}
	if (Current < m_fFBLow) m_fFBLow = Current;
	if (Current > m_fFBHigh) m_fFBHigh = Current;
	sprintf(temp,"%6.1f",m_fFBLow);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMIN,temp);

	sprintf(temp,"%6.1f",Current);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVCUR,temp);

	sprintf(temp,"%6.1f",m_fFBHigh);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMAX,temp);

	//ion chamber bias
	Current = psBinaryRecord->ion_bias;
	if (m_dDIRFirst)
	{
		m_fDIRHigh = m_fDIRLow = Current;
		m_dDIRFirst = false;
	}
	if (Current < m_fDIRLow) m_fDIRLow = Current;
	if (Current > m_fDIRHigh) m_fDIRHigh = Current;
	sprintf(temp,"%6.1f",m_fDIRLow);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMIN,temp);

	sprintf(temp,"%6.1f",Current);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVCUR,temp);

	sprintf(temp,"%6.1f",m_fDIRHigh);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMAX,temp);

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
		m_bBatteryTolOk = false;
	}
	else
	{
		CurrentRed = false;
		m_bBatteryTolOk = true;
	}

	sprintf(temp,"%5.1f",m_fLowBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);

	sprintf(temp,"%5.1f",m_fCurBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYCUR,temp);

	sprintf(temp,"%5.1f",m_fHighBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);

	//+5 volts
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
		m_bP5VoltOk = false;
	}
	else
	{
		CurrentRed = false;
		m_bP5VoltOk = true;
	}

	sprintf(temp,"%5.1f",m_fLowP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMIN,temp);

	sprintf(temp,"%5.1f",m_fCurP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTCUR,temp);

	sprintf(temp,"%5.1f",m_fHighP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMAX,temp);

	//+15 volts
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
		m_bP15VoltOk = false;
	}
	else
	{
		CurrentRed = false;
		m_bP15VoltOk = true;
	}

	sprintf(temp,"%5.1f",m_fLowP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMIN,temp);

	sprintf(temp,"%5.1f",m_fCurP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTCUR,temp);

	sprintf(temp,"%5.1f",m_fHighP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMAX,temp);

	//-15 volt
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
		m_bM15VoltOk = false;
	}
	else
	{
		CurrentRed = false;
		m_bM15VoltOk = true;
	}

	sprintf(temp,"%5.1f",m_fLowM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMIN,temp);

	sprintf(temp,"%5.1f",m_fCurM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTCUR,temp);

	sprintf(temp,"%5.1f",m_fHighM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMAX,temp);

	//set to red, yellow, or green if needed
	m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
	if (m_iGeneral_Tab == 0x0003)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
	else if (m_iGeneral_Tab == 0x0002)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
	else if (m_iGeneral_Tab == 0x0001)
		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	else if (m_iGeneral_Tab == 0x0000)
		SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

	m_iData_Tab = m_pParent->m_iData_Tab;
	if (m_iData_Tab == 0x0003)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
	else if (m_iData_Tab == 0x0002)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
	else if (m_iData_Tab == 0x0001)
		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	else if (m_iData_Tab == 0x0000)
		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

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
	UpdateSummary(Record);
	strcat(Record,"\n\r");
	return sizeof(SBinaryRec);
}
*/
/*
//record 12
int CSafePropertySheet::BinaryStatusRec(BYTE *pBinaryAt)
{
	AdjustTabsTo(PRE4);

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CModParam* modparpage  = m_pParent->m_pModParams;
	CSetupData* setdatapage = m_pParent->m_pSetupData;


#pragma pack(1)
	struct SBinaryRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		UCHAR acq_mode;			//1
		unsigned int g_gain1;	//4
		unsigned int g_gain2;	//4
		float n_gain;			//4
		UCHAR lld_a;			//1
		UCHAR uld_a;			//1
		UCHAR lld_b;			//1
		UCHAR uld_b;			//1
		float fission_bias;		//4
		float ion_bias;			//4
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} *psBinaryRecord;			//48 total
#pragma pack()

	psBinaryRecord = (SBinaryRec*)pBinaryAt;
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

	char temp[64];
	//get julian time and date
	CJulianTime Julian;
	Julian.Init(psBinaryRecord->julian);

	//build date/time string
	strcpy(temp,Julian.Date());
	strcat(temp," ");
	strcat(temp,Julian.Time());
	generalpage->SetDlgItemText(IDC_GENERAL_TIME,temp);
	
	if (psBinaryRecord->status & ACPOWER)
		strcpy(temp,FAIL);
	else 
		strcpy(temp,OK);

	//ac power changed since last status record
	if (psBinaryRecord->status & ACPOWERCH)
	{
		strcat(temp," with loss since last status record");
		
		if (!m_bACPowerLoss)
			m_bACPowerLoss = true;
	}
	else
		m_bACPowerLoss = false;

	datapage->SetDlgItemText(IDC_DATA_ACPOWER,temp);
	summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,temp);
	
	//bit 6 battery 
	if (psBinaryRecord->status & BATTERY) 
	{
		datapage->SetDlgItemText(IDC_DATA_BATTERYOK,LOW);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,LOW);
		m_bBatteryOk = false;
	}
	else
	{
		datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
		m_bBatteryOk = true;
	}

	char temp2[32];
	bool bMatch;
	temp2[0] = NULL;
	sprintf(temp,"%02X",psBinaryRecord->acq_mode);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2)!=0)
		bMatch = false;
	CurrentSetParam("SET_ACQ_MODE12",temp,bMatch);

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
	generalpage->SetDlgItemText(IDC_GENERAL_MODE,temp);

	//gain 1
	sprintf(temp,"%d",psBinaryRecord->g_gain1);
	generalpage->SetDlgItemText(IDC_GENERAL_GAMMA1G,temp);

	//gain 2
	sprintf(temp,"%d", psBinaryRecord->g_gain2);
	generalpage->SetDlgItemText(IDC_GENERAL_GAMMA2G,temp);

	//neutron gain
	generalpage->GetDlgItem(IDC_GENERAL_ANALOG)->ShowWindow(SW_SHOW);
	sprintf(temp,"%5.1f",psBinaryRecord->n_gain);
	generalpage->SetDlgItemText(IDC_GENERAL_ANALOG,temp);

	//lld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->lld_a);
	generalpage->SetDlgItemText(IDC_GENERAL_LLDA,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_LLD_A",temp,bMatch);
	
	//uld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->uld_a);
	generalpage->SetDlgItemText(IDC_GENERAL_WINDOWA,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	modparpage->CurrentSetParam("SET_ULD_A",temp,bMatch);
	setdatapage->CurrentSetParam("SET_ULD_A",temp,bMatch);

	//lld_b
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->lld_b);
	generalpage->SetDlgItemText(IDC_GENERAL_LLDB,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_LLD_B",temp,bMatch);
	
	//uld_a
	temp2[0] = NULL;
	sprintf(temp,"%3d",psBinaryRecord->uld_b);
	generalpage->SetDlgItemText(IDC_GENERAL_WINDOWB,temp);
	GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
	bMatch = true;
	if (strcmp(temp,temp2) != 0)
		bMatch = false;
	CurrentSetParam("SET_ULD_B",temp,bMatch);
	
	float Current;
	Current = psBinaryRecord->fission_bias;
	if (m_bFBFirst)
	{
		m_fFBHigh = m_fFBLow = Current;
		m_bFBFirst = false;
	}
	if (Current < m_fFBLow) m_fFBLow = Current;
	if (Current > m_fFBHigh) m_fFBHigh = Current;
	sprintf(temp,"%6.1f",m_fFBLow);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMIN,temp);

	sprintf(temp,"%6.1f",Current);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVCUR,temp);

	sprintf(temp,"%6.1f",m_fFBHigh);
	generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMAX,temp);

	//ion chamber bias
	Current = psBinaryRecord->ion_bias;
	if (m_dDIRFirst)
	{
		m_fDIRHigh = m_fDIRLow = Current;
		m_dDIRFirst = false;
	}
	if (Current < m_fDIRLow) m_fDIRLow = Current;
	if (Current > m_fDIRHigh) m_fDIRHigh = Current;
	sprintf(temp,"%6.1f",m_fDIRLow);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMIN,temp);

	sprintf(temp,"%6.1f",Current);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVCUR,temp);

	sprintf(temp,"%6.1f",m_fDIRHigh);
	generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMAX,temp);

	//battery voltage
	m_fCurBV = psBinaryRecord->battery;
	if (m_bFirstBV)
	{
		m_bFirstBV = false;
		m_fLowBV = m_fHighBV = m_fCurBV;
	}
	if (m_fCurBV < m_fLowBV) 
		m_fLowBV = m_fCurBV;
	if (m_fCurBV > m_fHighBV)
		m_fHighBV= m_fCurBV;
	if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
		m_bBatteryTolOk = false;
	else
		m_bBatteryTolOk = true;

	sprintf(temp,"%5.1f",m_fLowBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);

	sprintf(temp,"%5.1f",m_fCurBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYCUR,temp);

	sprintf(temp,"%5.1f",m_fHighBV);
	summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);

	//+5 volts
	m_fCurP5 = psBinaryRecord->plus5;
	if (m_bFirstP5)
	{
		m_bFirstP5 = false;
		m_fLowP5 = m_fHighP5 = m_fCurP5;
	}
	if (m_fCurP5 < m_fLowP5)
		m_fLowP5 = m_fCurP5;
	if (m_fCurP5 > m_fHighP5)
		m_fHighP5= m_fCurP5;
	if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
		m_bP5VoltOk = false;
	else
		m_bP5VoltOk = true;

	sprintf(temp,"%5.1f",m_fLowP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMIN,temp);
	sprintf(temp,"%5.1f",m_fCurP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTCUR,temp);
	sprintf(temp,"%5.1f",m_fHighP5);
	summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMAX,temp);

	//+15 volts
	m_fCurP15 = psBinaryRecord->plus12;
	if (m_bFirstP15)
	{
		m_bFirstP15 = false;
		m_fLowP15 = m_fHighP15 = m_fCurP15;
	}
	if (m_fCurP15 < m_fLowP15)
		m_fLowP15 = m_fCurP15;
	if (m_fCurP15 > m_fHighP15)
		m_fHighP15= m_fCurP15;

	if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
		m_bP15VoltOk = false;
	else
		m_bP15VoltOk = true;

	sprintf(temp,"%5.1f",m_fLowP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMIN,temp);

	sprintf(temp,"%5.1f",m_fCurP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTCUR,temp);

	sprintf(temp,"%5.1f",m_fHighP15);
	summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMAX,temp);

	//-15 volt
	m_fCurM15 = psBinaryRecord->minus12;
	if (m_bFirstM15)
	{
		m_bFirstM15 = false;
		m_fLowM15 = m_fHighM15 = m_fCurM15;
	}
	if (m_fCurM15 < m_fLowM15)
		m_fLowM15 = m_fCurM15;
	if (m_fCurM15 > m_fHighM15)
		m_fHighM15= m_fCurM15;
	if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
		m_bM15VoltOk = false;
	else
		m_bM15VoltOk = true;

	sprintf(temp,"%5.1f",m_fLowM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMIN,temp);

	sprintf(temp,"%5.1f",m_fCurM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT2,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTCUR,temp);

	sprintf(temp,"%5.1f",m_fHighM15);
	summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
	generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMAX,temp);

	//set to red, yellow, or green if needed
	m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
	if (m_iGeneral_Tab == 0x0003)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
	else if (m_iGeneral_Tab == 0x0002)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
	else if (m_iGeneral_Tab == 0x0001)
		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	else if (m_iGeneral_Tab == 0x0000)
		SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

	m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
	if (m_iGeneral4_Tab == 0x0003)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
	else if (m_iGeneral4_Tab == 0x0002)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
	else if (m_iGeneral4_Tab == 0x0001)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
	else if (m_iGeneral4_Tab == 0x0000)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);

	m_iData_Tab = m_pParent->m_iData_Tab;
	if (m_iData_Tab == 0x0003)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
	else if (m_iData_Tab == 0x0002)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
	else if (m_iData_Tab == 0x0001)
		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	else if (m_iData_Tab == 0x0000)
		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

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
	UpdateSummary(Record);
	strcat(Record,"\n\r");
	return sizeof(SBinaryRec);
}
*/
//record 12
void CSafePropertySheet::StatusRec(char *Record)
{
	AdjustTabsTo(PRE4);

//no display update -- but do add to file as appropriate
	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;


//	int CheckSum;
//	if (LengthOk(Record,STATUS_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[64];
		//get julian time and date

		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());

		generalpage->SetDlgItemText(IDC_GENERAL_TIME,temp);
	
		//status
		strncpy(temp,&Record[12],2);
		temp[2] = NULL;
		short Status;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 32",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		if (Status & ACPOWER)
			strcpy(temp,FAIL);
		else 
			strcpy(temp,OK);

		//ac power changed since last status record
		if (Status & ACPOWERCH)
		{
			strcat(temp," with loss since last status record");
			
			if (!m_bACPowerLoss)
				m_bACPowerLoss = true;
		}
		else
			m_bACPowerLoss = false;

		datapage->SetDlgItemText(IDC_DATA_ACPOWER,temp);
		summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,temp);
	
		//bit 6 battery 
		if (Status & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,ILOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,ILOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}

		strncpy(temp,&Record[14],2);
		temp[2] = NULL;
		char temp2[32];
		temp2[0] = NULL;
		bool bMatch;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ACQ_MODE12","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
			bMatch = false;
		CurrentSetParam("SET_ACQ_MODE12",temp,bMatch);

		short Mode;
//try{
		sscanf(temp,"%2hx",&Mode);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 33",MB_OK);
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
		generalpage->SetDlgItemText(IDC_GENERAL_MODE,temp);

		//gain 1
		strncpy(temp,&Record[16],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA1G,temp);

		//gain 2
		strncpy(temp,&Record[23],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA2G,temp);

		//neutron gain
//		generalpage->GetDlgItem(IDC_GENERAL_ANALOG_STAT	)->ShowWindow(SW_SHOW);
//		generalpage->GetDlgItem(IDC_GENERAL_ANALOG		)->ShowWindow(SW_SHOW);
		strncpy(temp,&Record[30],5);
		temp[5] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_ANALOG,temp);

		//lld_a
		strncpy(temp,&Record[35],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_LLDA,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
			bMatch = false;
		CurrentSetParam("SET_LLD_A",temp,bMatch);

		//uld_a
		strncpy(temp,&Record[38],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_WINDOWA,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
			bMatch = false;
		CurrentSetParam("SET_ULD_A",temp,bMatch);

		//lld_b
		strncpy(temp,&Record[41],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_LLDB,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
			bMatch = false;
		CurrentSetParam("SET_LLD_B",temp,bMatch);

		//uld_b
		strncpy(temp,&Record[44],3);
		temp[3] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_WINDOWB,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2)!=0)
			bMatch = false;
		CurrentSetParam("SET_ULD_B",temp,bMatch);

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
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMIN,temp);
		sprintf(temp,"%6.1f",Current);
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVCUR,temp);
		sprintf(temp,"%6.1f",m_fFBHigh);
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMAX,temp);

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
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMIN,temp);
		sprintf(temp,"%6.1f",Current);
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVCUR,temp);
		sprintf(temp,"%6.1f",m_fDIRHigh);
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMAX,temp);
		CWnd *pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVMIN); if (pdiw) pdiw->UpdateWindow();
		pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVCUR); if (pdiw) pdiw->UpdateWindow();
		pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVMAX); if (pdiw) pdiw->UpdateWindow();

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
//			m_pParent->FlagOthr(1,true);
			CurrentRed = true;
			m_bBatteryTolOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bBatteryTolOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYCUR,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);

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
			m_bP5VoltOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMAX,temp);

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
			m_bP15VoltOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bP15VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMAX,temp);

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
//			m_pParent->FlagOthr(1,true);
			m_bM15VoltOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bM15VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMIN,temp);

		sprintf(temp,"%5.1f",m_fCurM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTCUR,temp);

		sprintf(temp,"%5.1f",m_fHighM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMAX,temp);

		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

		char cBuff[1024];
		strcpy(cBuff,"15820 ");
		strcat(cBuff,Record);

	}
}
/*
//record 13
int CSafePropertySheet::BinaryOffsetRec(BYTE *pBinaryAt)
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
	strcat(Record,"\n\r");

	OffsetRec(Record);
	return sizeof(SBinaryRec);
}
*/
//record 13
void CSafePropertySheet::OffsetRec(char *Record)
{
//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
//	int CheckSum;
//	if (LengthOk(Record,OFFSET_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
	
		//added to general dialog box 
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		if (GetPageIndex(generalpage)>=0)
			generalpage->SetDlgItemText(IDC_GENERAL_OFFSET,temp);
		if (GetPageIndex(general4page)>=0)
			general4page->SetDlgItemText(IDC_GENERAL4_OFFSET,temp);

	}
}

//record 14
void CSafePropertySheet::ID_Record(char *Record)
{
	//all page 2 stuff;
//	int CheckSum;
//	if (LengthOk(Record,ID_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		
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

		datapage->SetDlgItemText(IDC_DATA_INQUIRETIME,temp);

		//get status byte 
		short Status;
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 34",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	
		//bit 7 ac power
		if (Status & ACPOWER)
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,FAIL);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,FAIL);
			m_bACPowerOk = false;
		}
		else 
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,OK);
			m_bACPowerOk = true;
		}
	
		//bit 6 battery 
		if (Status & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,ILOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,ILOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}

		//bit 5 tamper alarm
		if (Status & TAMPER)
			m_bCountRateTamperOk = false;
		else
			m_bCountRateTamperOk = true;

		//bit 4 RMS Sigma Tamper
		if (Status & RMSTAMPER)
			m_bRMSSigmaTamperOk = false;
		else
			m_bRMSSigmaTamperOk = true;

		//bit 3 bbm
		if (Status & BBMERROR)
		{
			datapage->SetDlgItemText(IDC_DATA_BBMOK,"Error in Read or Write");
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,"Error in Read or Write");
			m_bBBMOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BBMOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,OK);
			m_bBBMOk = true;
		}


		//bit 2 startup
		if (Status & CWSTART)
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_COLD);
			m_dCWStart = 0;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_NORMAL);
			m_dCWStart = 1;
		}
	
		//bit 0 in MII
		if (Status & INMII) 
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,INMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,INMII_TXT);
			m_bInMII = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,NOTINMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,NOTINMII_TXT);
			m_bInMII = false;
		}

		//for use in the next few data items
		char temp2[16];

		//get Main Unit ID
		bool bMatch;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		datapage->SetDlgItemText(IDC_DATA_UNITID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_UNIT_ID",temp,bMatch);
				
		//get board 1 ID
		strncpy(temp,&Record[16],3);
		temp[3]=NULL;
		datapage->SetDlgItemText(IDC_DATA_UNITID2,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BOARD1_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_BOARD1_ID",temp,bMatch);

		//get board 2 ID
		strncpy(temp,&Record[19],3);
		temp[3]=NULL;
		datapage->SetDlgItemText(IDC_DATA_UNITID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_BOARD2_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_BOARD2_ID",temp,bMatch);

		strncpy(temp,&Record[22],7);
		temp[7] = NULL;
		datapage->SetDlgItemText(IDC_DATA_USERID,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_USER_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_USER_ID",temp,bMatch);

		//get bbm
		strncpy(temp,&Record[29],6);
		temp[6]=NULL;

		//set all the displays
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);

		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	}
}

//remove the high order byte(s) of the exponent
//usually make the exponent 2 digits
char* CSafePropertySheet::TrunkExp(char* szString, int iDigits)
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
/*
//record 15
int CSafePropertySheet::BinaryAcquirRec(BYTE *pBinaryAt)
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
	for (i = 0; i < 2; i++)
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
	for (i = 0; i < 2; i++)
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
		
		//
		// pjm
		//
		// Format definition for e  double Signed value having the form 
		// [ - ]d.dddd e [sign]ddd where d is a single decimal digit, 
		// dddd is one or more decimal digits, ddd is exactly three decimal digits, 
		// and sign is + or -. 
		//
		// NOTE:  "exactly three" decimal digits for the exponent.

		TrunkExp(szTemp1[i],1);
		TrunkExp(szTemp2[i],1);
	}

	//format the gamma rates
	for (i = 0; i < 3; i++)
	{
		if (psBinaryRecord->rates[i] >= 1000000)
			sprintf(szTemp0[i],"%8.0f",psBinaryRecord->rates[i]);
		else
			sprintf(szTemp0[i],"%8.1f",psBinaryRecord->rates[i]);
	}

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
	//int check  
	AppendCheckSum(Record);

	//add the two bytes for authentication
	strcat(Record,"--");
	strcat(Record,"\n\r");
	
	//send to the first tab page
	UpdateSummary(Record);
	//handle the record with the "text" version handler
	AcquirRec(Record,true);
	//return the number of bytes the binary record
	return sizeof(SBinaryRec);
}
*/
//record 15
void CSafePropertySheet::AcquirRec(char *Record, bool /*bIgnoreAuth*/)
{
	//get pointers to page 2 and summary page
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
	
	char temp[32];
	
	//vars to send to graphic display
	float f1,f2,f3,f4,f5;

 	//make sure we arn't trying to read from the binary
	//file at the moment.  This can occur if dumpbbm is
	//secquence is happening at the same time the graph
	//display is being displayed.
	while(m_bUpdatingFromFile) Sleep(500);

	//get julian time
	strncpy(temp,&Record[2],10);
	temp[10]=NULL;
	CJulianTime jTime(temp);
	
	//get status byte 
	short Status;
	strncpy(temp,&Record[12],2);
	temp[2]=NULL;
//try{
	sscanf(temp,"%2hx",&Status);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 35",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

	//save status in binary to acquire record
	//move all the date displays down one
	datapage->SetDlgItemText(IDC_DATA_DATADATE4,
		datapage->GetText(IDC_DATA_DATADATE3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATADATE3,
		datapage->GetText(IDC_DATA_DATADATE2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATADATE2,
		datapage->GetText(IDC_DATA_DATADATE1,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATADATE1,jTime.Date());

	//move all the time displays down one
	datapage->SetDlgItemText(IDC_DATA_DATATIME4,
		datapage->GetText(IDC_DATA_DATATIME3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATATIME3,
		datapage->GetText(IDC_DATA_DATATIME2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATATIME2,
		datapage->GetText(IDC_DATA_DATATIME1,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_DATATIME1,jTime.Time());

	//move all the Neutron A count rate displays down one
	datapage->SetDlgItemText(IDC_DATA_PULSEACNT4,
		datapage->GetText(IDC_DATA_PULSEACNT3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSEACNT3,
		datapage->GetText(IDC_DATA_PULSEACNT2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSEACNT2,
		datapage->GetText(IDC_DATA_PULSEACNT1,temp,sizeof(temp)));
	strncpy(temp,&Record[14],8);
	temp[8]=NULL;
	datapage->SetDlgItemText(IDC_DATA_PULSEACNT1,temp);
//try{
	sscanf(temp,"%f",&f1);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 36",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	//set the acquire record value

	//move all the Neutron B count rate displays down one
	datapage->SetDlgItemText(IDC_DATA_PULSEBCNT4,
		datapage->GetText(IDC_DATA_PULSEBCNT3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSEBCNT3,
		datapage->GetText(IDC_DATA_PULSEBCNT2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSEBCNT2,
		datapage->GetText(IDC_DATA_PULSEBCNT1,temp,sizeof(temp)));
	strncpy(temp,&Record[22],8);
	temp[8]=NULL;
	datapage->SetDlgItemText(IDC_DATA_PULSEBCNT1,temp);
//try{
		sscanf(temp,"%f",&f2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 37",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	//set the acquire record value

	//move all the Neutron C count rate displays down one
	datapage->SetDlgItemText(IDC_DATA_PULSECCNT4,
		datapage->GetText(IDC_DATA_PULSECCNT3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSECCNT3,
		datapage->GetText(IDC_DATA_PULSECCNT2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_PULSECCNT2,
		datapage->GetText(IDC_DATA_PULSECCNT1,temp,sizeof(temp)));
	strncpy(temp,&Record[30],8);
	temp[8]=NULL;
	datapage->SetDlgItemText(IDC_DATA_PULSECCNT1,temp);
//try{
	sscanf(temp,"%f",&f3);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 38",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	//set the acquire record value

	//move all the gross gammas 1 displays down one
	datapage->SetDlgItemText(IDC_DATA_GAMMA1GR4,
		datapage->GetText(IDC_DATA_GAMMA1GR3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA1GR3,
		datapage->GetText(IDC_DATA_GAMMA1GR2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA1GR2,
		datapage->GetText(IDC_DATA_GAMMA1GR1,temp,sizeof(temp)));
	strncpy(temp,&Record[38],8);
	temp[8]=NULL;
	datapage->SetDlgItemText(IDC_DATA_GAMMA1GR1,temp);
//try{
	sscanf(temp,"%f",&f4);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 39",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	//set the acquire record value

	//move all the gamma 1 sigma displays down one
	datapage->SetDlgItemText(IDC_DATA_GAMMA1S4,
		datapage->GetText(IDC_DATA_GAMMA1S3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA1S3,
		datapage->GetText(IDC_DATA_GAMMA1S2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA1S2,
		datapage->GetText(IDC_DATA_GAMMA1S1,temp,sizeof(temp)));
	strncpy(temp,&Record[46],7);
	temp[7]=NULL;
	datapage->SetDlgItemText(IDC_DATA_GAMMA1S1,temp);
	//set the acquire record value

	//move all the gross gamma 2 displays down one
	datapage->SetDlgItemText(IDC_DATA_GAMMA2GR4,
		datapage->GetText(IDC_DATA_GAMMA2GR3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA2GR3,
		datapage->GetText(IDC_DATA_GAMMA2GR2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA2GR2,
		datapage->GetText(IDC_DATA_GAMMA2GR1,temp,sizeof(temp)));
	strncpy(temp,&Record[53],8);
	temp[8]=NULL;
	datapage->SetDlgItemText(IDC_DATA_GAMMA2GR1,temp);
//try{
	sscanf(temp,"%f",&f5);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 40",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
	//set the acquire record value

	//move all the sigma 2 displays down one
	datapage->SetDlgItemText(IDC_DATA_GAMMA2S4,
		datapage->GetText(IDC_DATA_GAMMA2S3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA2S3,
		datapage->GetText(IDC_DATA_GAMMA2S2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_GAMMA2S2,
		datapage->GetText(IDC_DATA_GAMMA2S1,temp,sizeof(temp)));
	strncpy(temp,&Record[61],7);
	temp[7]=NULL;
	datapage->SetDlgItemText(IDC_DATA_GAMMA2S1,temp);
	//set the acquire record value

	//move all the time data acquired displays down one
	datapage->SetDlgItemText(IDC_DATA_LENGTHT4,
		datapage->GetText(IDC_DATA_LENGTHT3,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_LENGTHT3,
		datapage->GetText(IDC_DATA_LENGTHT2,temp,sizeof(temp)));
	datapage->SetDlgItemText(IDC_DATA_LENGTHT2,
		datapage->GetText(IDC_DATA_LENGTHT1,temp,sizeof(temp)));
	strncpy(temp,&Record[68],5);
	temp[5]=NULL;


	//if mode flag 2 bit 0 == 1 then divide by 10 and show %.1f
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	char szTemp[8];
	general4page->GetDlgItemText(IDC_GENERAL4_MODE2,szTemp,sizeof(szTemp)-1);
	if (strlen(szTemp))
	{
		int iTemp;
		sscanf(szTemp,"%x",&iTemp);
		if (iTemp & 0x01)
		{
			float fTemp;
			sscanf(temp,"%f",&fTemp);
			fTemp /= 10.0;
			sprintf(temp,"%.1f",fTemp);
		}
	}
	datapage->SetDlgItemText(IDC_DATA_LENGTHT1,temp);
	//set the acquire record value

	if (m_bDataWatchDlgOpen)
	{
		m_pDataWatchDlg->SetData(jTime.TotalSeconds(),
			f1, f2, f3, f4, f5, true);
	}
}

//record 16
void CSafePropertySheet::DumpEnd(char *Record)
{
}

//record 61
void CSafePropertySheet::DumpEnd2(char *Record)
{
}

//record 17
void CSafePropertySheet::DumpOk(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,DUMP_OK_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[8];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;


		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		bbm = atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);
	}
}
/*
//record 18
int CSafePropertySheet::BinaryBatteryRec(BYTE *pBinaryAt)
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
	strcat(Record,"\n\r");

	BatteryRec(Record);
	return sizeof(SBinaryRec);
}
*/
//record 18
void CSafePropertySheet::BatteryRec(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,BATTERY_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[32];

		CSummary*  summarypage = m_pParent->m_pSummaryPage;
		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		if (GetPageIndex(general4page)>=0)
		{
			CString cTemp(Julian.Date());
			cTemp += " ";
			cTemp += Julian.Time();
			general4page->SetDlgItemText(IDC_GENERAL4_BATTERYTIME,cTemp);
		}

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
			m_bBatteryTolOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bBatteryTolOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
		if (GetPageIndex(generalpage)>=0)
			generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);
		if (GetPageIndex(general4page)>=0)
			general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMIN,temp);

		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
		if (GetPageIndex(generalpage)>=0)
			generalpage->SetDlgItemText(IDC_GENERAL_BATTERYCUR,temp);
		if (GetPageIndex(general4page)>=0)
			general4page->SetDlgItemText(IDC_GENERAL4_BATTERYCUR,temp);

		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
		if (GetPageIndex(generalpage)>=0)
			generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);
		if (GetPageIndex(general4page)>=0)
			general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMAX,temp);

		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

		m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
		if (m_iGeneral4_Tab == 0x0003)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral4_Tab == 0x0002)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
		else if (m_iGeneral4_Tab == 0x0001)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
		else if (m_iGeneral4_Tab == 0x0000)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
	}
}

//record 19 GRANDIII ONLY
void CSafePropertySheet::TripNSRec(char *Record)
{
	AdjustTabsTo(PRE4);

//	int CheckSum;
//	if (LengthOk(Record,TRIP_NEU_STAT_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

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
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fTNSRHigh);
		generalpage->SetDlgItemText(IDC_GENERAL_FCHIGHVMAX,temp);

		strcpy(temp,"Neutron A:");
		strncat(temp,&Record[20],4);
		strcat(temp,", B:");
		strncat(temp,&Record[24],4);
		strcat(temp,", C:");
		strncat(temp,&Record[28],4);
		generalpage->SetDlgItemText(IDC_GENERAL_PULSE,temp);

		char temp2[64];
//		temp2[0] = NULL;
//		temp[0] = Record[20];
//		temp[1] = NULL;
		strncpy(temp,&Record[20],12);
		temp[12] = NULL;
		bool bMatch;
		GetPrivateProfileString(m_pParent->m_pName,"SET_PULSE_CONF","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_PULSE_CONF",temp,bMatch);

		if (strcmp("EXTAEXTBEXTC",temp)==0)
		{
			generalpage->GetDlgItem(IDC_GENERAL_LLDA_STAT	)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_LLDB_STAT	)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_LLDA		)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_LLDB		)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWA_STAT)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWB_STAT)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWA		)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWB		)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_ANALOG_STAT	)->ShowWindow(SW_HIDE);
			generalpage->GetDlgItem(IDC_GENERAL_ANALOG		)->ShowWindow(SW_HIDE);
		}
		else
		{
			generalpage->GetDlgItem(IDC_GENERAL_LLDA_STAT	)->ShowWindow(SW_SHOW);
			generalpage->GetDlgItem(IDC_GENERAL_LLDB_STAT	)->ShowWindow(SW_SHOW);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWA_STAT)->ShowWindow(SW_SHOW);
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWB_STAT)->ShowWindow(SW_SHOW);
			generalpage->GetDlgItem(IDC_GENERAL_ANALOG_STAT	)->ShowWindow(SW_SHOW);
			generalpage->GetDlgItem(IDC_GENERAL_LLDA		)->ShowWindow(SW_SHOW);

			strncpy(temp,&Record[32],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_LLDA,temp);

			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_A","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_LLD_A",temp,bMatch);
	
			generalpage->GetDlgItem(IDC_GENERAL_LLDB		)->ShowWindow(SW_SHOW);

			strncpy(temp,&Record[38],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_LLDB,temp);

			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_LLD_B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_LLD_B",temp,bMatch);
	
			generalpage->GetDlgItem(IDC_GENERAL_WINDOWA		)->ShowWindow(SW_SHOW);

			strncpy(temp,&Record[35],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_WINDOWA,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_A","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_ULD_A",temp,bMatch);

			generalpage->GetDlgItem(IDC_GENERAL_WINDOWB		)->ShowWindow(SW_SHOW);

			strncpy(temp,&Record[41],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_WINDOWB,temp);

			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_ULD_B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_ULD_B",temp,bMatch);
	
			generalpage->GetDlgItem(IDC_GENERAL_ANALOG		)->ShowWindow(SW_SHOW);
			strncpy(temp,&Record[44],5);
			temp[5] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_ANALOG,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_ANALOG_GAIN","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_ANALOG_GAIN",temp,bMatch);
		}

		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);
	}
}

bool CSafePropertySheet::CurrentSetParam(CString cIniName,CString cValue,bool bMatch,bool bTextOnly)
{
	return CurrentSetParam((LPSTR)LPCSTR(cIniName),(LPSTR)LPCSTR(cValue),bMatch,bTextOnly);
}

bool CSafePropertySheet::CurrentSetParam(char*szIniName,char*szValue,bool bMatch, bool bTextOnly)
{
//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CModParam* modparpage  = m_pParent->m_pModParams;
	CSetupData* setdata4page = m_pParent->m_pSetup4Data;
	CSetupData* setdatapage = m_pParent->m_pSetupData;

	modparpage->CurrentSetParam(szIniName,szValue,bMatch,true);
	bool bReturn = true;
	if (GetPageIndex(setdatapage)>=0)
	{
		if (setdatapage->CurrentSetParam(szIniName,szValue,bMatch,bTextOnly)==false)
		{
			if (!bTextOnly)
				m_iInstSetup_Tab |= 0x0001;
			bReturn = false;
		}
	}

	if (GetPageIndex(setdata4page)>=0)
	{
		if (setdata4page->CurrentSetParam(szIniName,szValue,bMatch,bTextOnly)==false)
		{
			if (!bTextOnly)
				m_iInst4Setup_Tab |= 0x0001;
			bReturn = false;
		}
	}
	return bReturn;
}

//record 1a
void CSafePropertySheet::DualIonRec(char *Record, bool bBinarySource)
{
	AdjustTabsTo(PRE4);

//	int CheckSum;
//	if (LengthOk(Record,DUAL_ION_STAT_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];
		char temp2[32];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//set both dates and times
		strcpy(temp,Julian.Date());
		generalpage->SetDlgItemText(IDC_GENERAL_G1DATE,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_G2DATE,temp);

		strcpy(temp,Julian.Time());
		generalpage->SetDlgItemText(IDC_GENERAL_G1TIME,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_G2TIME,temp);

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
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMIN,temp);

		sprintf(temp,"%6.1f",Current);
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVCUR,temp);

		sprintf(temp,"%6.1f",m_fDIRHigh);
		generalpage->SetDlgItemText(IDC_GENERAL_ICHIGHVMAX,temp);
		CWnd *pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVMIN); if (pdiw) pdiw->UpdateWindow();
		pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVCUR); if (pdiw) pdiw->UpdateWindow();
		pdiw = GetDlgItem(IDC_GENERAL_ICHIGHVMAX); if (pdiw) pdiw->UpdateWindow();

		bool bMatch;
		//gain mode ch 1
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA1M,(Record[20]=='A')?"Automatic":"Manual");
		temp2[0] = NULL;
		temp[0] = Record[20];
		temp[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA1_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[20] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_GAMMA1_MODE",temp,bMatch);

		//gain mode ch 2
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA2M,(Record[21]=='A')?"Automatic":"Manual");
		temp2[0] = NULL;
		temp[0] = Record[21];
		temp[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA2_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[21] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_GAMMA2_MODE",temp,bMatch);
		
		//max gain0
		strncpy(temp,&Record[22],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_MAXG1,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_MAX_GAIN0","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_MAX_GAIN0",temp,bMatch);
		//max gain1
		strncpy(temp,&Record[29],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_MAXG2,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_MAX_GAIN1","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_MAX_GAIN1",temp,bMatch);
	
		//gain
		strncpy(temp,&Record[36],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA1G,temp);
		strncpy(temp,&Record[43],7);
		temp[7] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_GAMMA2G,temp);
	
		//disconnect mode
		strncpy(temp,&Record[50],1);
		temp[1] = NULL;
		generalpage->SetDlgItemText(IDC_GENERAL_DISCM,(Record[50]=='I')?"Internal":"Remote");
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_DISCONNECT_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_DISCONNECT_MODE",temp,bMatch);

		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

	}
}
/*
//record 1b
int CSafePropertySheet::BinaryGenStatRecOld(BYTE *pBinaryAt)
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
	
	AppendCheckSum(szRecord);
	UpdateSummary(szRecord);
	strcat(szRecord,"\n\r");	
	GenStatRec(szRecord, true);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1b
int CSafePropertySheet::BinaryGenStatRec(BYTE *pBinaryAt)
{
#pragma pack(1)
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
	strcat(szRecord,"\n\r");	
	GenStatRec(szRecord, false);

	return sizeof(SBinaryRec);
}
*/
//record 1b
void CSafePropertySheet::GenStatRec(char *Record, bool bBinarySource)
{
	AdjustTabsTo(PRE4);

//	int CheckSum;
//	if (LengthOk(Record,GEN_STATUS_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];
		char temp2[64];
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		generalpage->SetDlgItemText(IDC_GENERAL_TIME,temp);

		bool bMatch;
	
		//main unit id
		//in HEX on a GRANDIII in dec on a MiniGRAND
		if (!bBinarySource)
		{
			strncpy(temp,&Record[12],2);
			temp[2] = NULL;
			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
			{
				int iTemp;
//try{
				sscanf(temp,"%x",&iTemp);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 41",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
				sprintf(temp,"%02d",iTemp);
				if (strcmp(temp,temp2) != 0)
					bMatch = false;
			}
			datapage->SetDlgItemText(IDC_DATA_UNITID,temp);
			CurrentSetParam("SET_UNIT_ID",temp,bMatch);
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
//	AfxMessageBox("Error at 42",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		if (Status & ACPOWER)
			strcpy(temp,FAIL);
		else 
			strcpy(temp,OK);

		//ac power changed since last status record
		if (Status & ACPOWERCH)
		{
			strcat(temp," with loss since last status record");
	
			if (!m_bACPowerLoss)
				m_bACPowerLoss = true;
		}
		else
			m_bACPowerLoss = false;

		datapage->SetDlgItemText(IDC_DATA_ACPOWER,temp);
		summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,temp);

		//bit 6 battery 
		if (Status & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,ILOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,ILOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}

		//Version: 00.00
		if (!bBinarySource)
		{
			strncpy(temp,&Record[16],5);
			temp[5] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_GRANDP,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_VERSION","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_VERSION",temp,bMatch);
		}

		//Insp ID: 0000000
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[21],7);
			temp[7] = NULL;
			datapage->SetDlgItemText(IDC_DATA_USERID,temp);
			temp2[0] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_USER_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_USER_ID",temp,bMatch);
		}

		//Board 1 type: FF
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[28],2);
			temp[2] = NULL;
			datapage->SetDlgItemText(IDC_DATA_UNITID2,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_BOARD1_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_BOARD1_ID",temp,bMatch);
		}

		//Board 2 type: FF
		//exclude if from binary record
		if (!bBinarySource)
		{
			strncpy(temp,&Record[30],2);
			temp[2] = NULL;
			datapage->SetDlgItemText(IDC_DATA_UNITID3,temp);		
			GetPrivateProfileString(m_pParent->m_pName,"SET_BOARD2_ID","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_BOARD2_ID",temp,bMatch);
		}
	
		//acq mode
		if (!bBinarySource)
		{
			if (Record[32]=='0') strcpy(temp,"Gamma Only");
			else if (Record[32]== '1') strcpy(temp,"Neutron Only");
			else strcpy(temp,"Gamma & Neutron");
			temp[0] = Record[32];
			temp[1] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_MODE2,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_ACQ_MODE1B","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			//if (strcmp(temp,temp2) != 0)
			if (Record[32] != temp2[0])
				bMatch = false;
			CurrentSetParam("SET_ACQ_MODE1B",temp,bMatch);
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
			m_fLowBV = m_fCurBV;
		if (m_fCurBV > m_fHighBV)
			m_fHighBV= m_fCurBV;
		if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			m_bBatteryTolOk = false;
		else
			m_bBatteryTolOk = true;

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMIN,temp);
		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYCUR,temp);
		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_BATTERYMAX,temp);

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
			m_fLowP5 = m_fCurP5;
		if (m_fCurP5 > m_fHighP5)
			m_fHighP5= m_fCurP5;
		if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			m_bP5VoltOk = false;
		else
			m_bP5VoltOk = true;

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P5VOLTMAX,temp);

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
			m_fLowP15 = m_fCurP15;
		if (m_fCurP15 > m_fHighP15)
			m_fHighP15= m_fCurP15;
		if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
			m_bP15VoltOk = false;
		else
			m_bP15VoltOk = true;

		sprintf(temp,"%5.1f",m_fLowP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_P15VOLTMAX,temp);

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
			m_fLowM15 = m_fCurM15;
		if (m_fCurM15 > m_fHighM15)
			m_fHighM15= m_fCurM15;
		if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
		{
//			m_pParent->FlagOthr(1,true);
			m_bM15VoltOk = false;
		}
		else
			m_bM15VoltOk = true;

		sprintf(temp,"%5.1f",m_fLowM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT2,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
		generalpage->SetDlgItemText(IDC_GENERAL_M15VOLTMAX,temp);

		//rom checksum
		if (!bBinarySource)
		{
			strncpy(temp,&Record[58],4);
			temp[4] = NULL;
			generalpage->SetDlgItemText(IDC_GENERAL_ROM,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_1B_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_1B_ROM_CHECKSUM",temp,bMatch);
		}
	
		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	}
}
/*
//record 1c
int CSafePropertySheet::BinaryMon_A_RecOld(BYTE *pBinaryAt)
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
	strcat(Record,"\n\r");	
	Mon_A_Rec(Record, true);

	return sizeof(SBinaryRec);
}
*/
/*
//record 1c
int CSafePropertySheet::BinaryMon_A_Rec(BYTE *pBinaryAt)
{
#pragma pack(1)
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
	strcat(Record,"\n\r");	
	Mon_A_Rec(Record, false);
	
	return sizeof(SBinaryRec);
}
*/
//record 1c
void CSafePropertySheet::Mon_A_Rec(char *Record, bool bBinarySource)
{
	AdjustTabsTo(PRE4);

//	int CheckSum;
//	if (LengthOk(Record,MONITOR_A_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[64];

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
//		CUserProg* userpage = (CUserProg*)GetPage(3);

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		userpage->SetDlgItemText(IDC_USERPROG_TIME,temp);

		bool bMatch;
		
		char temp2[32];
		char temp3[32];

		//user program version number
		if (!bBinarySource)
		{
			strncpy(temp,&Record[12],5);
			temp[5] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_GRANDUV,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_GRAND_USERVER","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_GRAND_USERVER",temp,bMatch);
		}
	
		//user select data acquire time
		if (!bBinarySource)
		{
			strncpy(temp,&Record[17],5);
			temp[5] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_USER,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_ACQUIRE_TIME","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
//			if (strcmp(temp,temp2) != 0)
//				m_iUserProg_Tab |= 0x8000;
			CurrentSetParam("SET_ACQUIRE_TIME",temp,bMatch);

		}
	
		//Immediate Buffer Size / save size
		if (!bBinarySource)
		{
			strcpy(temp,"Total: ");
			strncat(temp,&Record[22],2);
			temp3[0] = Record[22];
			temp3[1] = Record[23];
			temp3[2] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_BUFFER_TOTAL","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[22],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_BUFFER_TOTAL",temp3,bMatch);

			strcat(temp,", Save: ");
			strncat(temp,&Record[24],2);
			temp3[0] = Record[24];
			temp3[1] = Record[25];
			temp3[2] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_IMMEDIATE,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_BUFFER_SAVE","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[24],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_BUFFER_SAVE",temp3,bMatch);

		}
	
		if (!bBinarySource)
		{
			//neutron A threshold
			strncpy(temp,&Record[26],8);
			temp[8] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_PULSEAT,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_PULSEATHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_NEUTRON_PULSEATHRESH",temp,bMatch);
	
			//neutron B threshold
			strncpy(temp,&Record[34],8);
			temp[8] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_PULSEBT,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_PULSEBTHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_NEUTRON_PULSEBTHRESH",temp,bMatch);
		
			//neutron C threshold
			strncpy(temp,&Record[42],8);
			temp[8] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_PULSECT,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_PULSECTHRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_NEUTRON_PULSECTHRESH",temp,bMatch);
		
			//gamma 1 threshold
			strncpy(temp,&Record[50],10);
			temp[10] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_GAMMA1T,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA_1THRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_GAMMA_1THRESH",temp,bMatch);
		
			//gamma 2 threshold
			strncpy(temp,&Record[60],10);
			temp[10] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_GAMMA2T,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA_2THRESH","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_GAMMA_2THRESH",temp,bMatch);
		
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
			userpage->SetDlgItemText(IDC_USERPROG_NOFILTER,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_NOFILTERENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[70],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_NOFILTERENTERMII_RUNS",temp3,bMatch);

			temp3[0] = Record[72];
			temp3[1] = Record[73];
			temp3[2] = Record[74];
			temp3[4] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_NOFILTERENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[72],temp2,3) != 0)
				bMatch = false;
			CurrentSetParam("SET_NOFILTERENTERMII_SIGMA",temp3,bMatch);
		
			//mii enter - consecutive runs
			strncpy(temp,&Record[75],2);
			temp[2] = NULL;
			temp3[0] = Record[75];
			temp3[1] = Record[76];
			temp3[2] = NULL;
			strcat(temp," runs > ");
			strncat(temp,&Record[77],3);
			strcat(temp," sigma from background");
			userpage->SetDlgItemText(IDC_USERPROG_MIIENTER,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_ENTERMII_RUNS","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[75],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_ENTERMII_RUNS",temp3,bMatch);

			temp3[0] = Record[77];
			temp3[1] = Record[78];
			temp3[2] = Record[79];
			temp3[3] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_ENTERMII_SIGMA","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[77],temp2,3) != 0)
				bMatch = false;
			CurrentSetParam("SET_ENTERMII_SIGMA",temp3,bMatch);
		
			//mii exit
			strncpy(temp,&Record[80],2);
			temp[2] = NULL;
			temp3[0] = Record[80];
			temp3[1] = Record[81];
			temp3[2] = NULL;
			strcat(temp," runs < MII threshold");
			userpage->SetDlgItemText(IDC_USERPROG_MIIEXIT,temp);
			GetPrivateProfileString(m_pParent->m_pName,"SET_MII_EXIT","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[80],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_MII_EXIT",temp3,bMatch);
		
			//local background
			strcpy(temp,"First ");
			strncat(temp,&Record[82],2);
			temp3[0] = Record[82];
			temp3[1] = Record[83];
			temp3[2] = NULL;
			GetPrivateProfileString(m_pParent->m_pName,"SET_LOCAL_BACKGROUND_FIRST","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[82],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_LOCAL_BACKGROUND_FIRST",temp3,bMatch);

		
			strcat(temp," of last ");
			strncat(temp,&Record[84],2);
			temp3[0] = Record[84];
			temp3[1] = Record[85];
			temp3[2] = NULL;
			strcat(temp," runs");
			GetPrivateProfileString(m_pParent->m_pName,"SET_LOCAL_BACKGROUND_LAST","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strncmp(&Record[84],temp2,2) != 0)
				bMatch = false;
			CurrentSetParam("SET_LOCAL_BACKGROUND_LAST",temp3,bMatch);

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
//	AfxMessageBox("Error at 43",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		temp2[0] = NULL;
		Status2 = 0;
		GetPrivateProfileString(m_pParent->m_pName,"SET_ACQ_MODE1C","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strlen(temp2))
		{
//try{
			sscanf(temp2,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 44",MB_OK);
//	AfxMessageBox(temp2,MB_OK);
//}
		}
		if ((Status & 0x0001) != (Status2 & 0x0001))
			bMatch = false;
		userpage->SetDlgItemText(IDC_USERPROG_PULSEAF,(Status & 0x0001)?YES:NO);

		if ((Status & 0x0002) != (Status2 & 0x0002))
			bMatch = false;
		userpage->SetDlgItemText(IDC_USERPROG_PULSEBF,(Status & 0x0002)?YES:NO);

		if ((Status & 0x0004) != (Status2 & 0x0004))
			bMatch = false;
		userpage->SetDlgItemText(IDC_USERPROG_PULSECF,(Status & 0x0004)?YES:NO);

		if ((Status & 0x0008) != (Status2 & 0x0008))
			bMatch = false;
		userpage->SetDlgItemText(IDC_USERPROG_GAMMA1F,(Status & 0x0008)?YES:NO);

		if ((Status & 0x0010) != (Status2 & 0x0010))
			bMatch = false;
		userpage->SetDlgItemText(IDC_USERPROG_GAMMA2F,(Status & 0x0010)?YES:NO);
		CurrentSetParam("SET_ACQ_MODE1C",temp,bMatch);
	
		//rom checksum
		if (!bBinarySource)
		{
			strncpy(temp,&Record[99],4);
			temp[4] = NULL;
			userpage->SetDlgItemText(IDC_USERPROG_ROMCHECK,temp);

			GetPrivateProfileString(m_pParent->m_pName,"SET_1C_ROM_CHECKSUM","",temp2,sizeof(temp2),m_szIniFile);
			bMatch = true;
			if (strcmp(temp,temp2) != 0)
				bMatch = false;
			CurrentSetParam("SET_1C_ROM_CHECKSUM",temp,bMatch);
		}
		
		m_iUserProg_Tab = m_pParent->m_iUserProg_Tab;
		if (m_pParent->m_iUserProg_Tab == 0x0003)
			SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOWRED);
		else if (m_iUserProg_Tab == 0x0002)
			SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOW);
		else if (m_iUserProg_Tab == 0x0001)
			SetTabColor(IDC_USERPROG_TAB,COLOR_RED);
		else if (m_iUserProg_Tab == 0x0000)
			SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);
	}
}


//record 1d
void CSafePropertySheet::TripN2Rec(char *Record)
{
}
/*
//record 1e
//binary version should NEVER be received
int CSafePropertySheet::BinaryID2_Record(BYTE *pBinaryAt)
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
	strcat(Record,"\n\r");
	ID2_Record(Record);
	
	return sizeof(SBinaryRec);
}
*/
//record 1e
void CSafePropertySheet::ID2_Record(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,MONITOR_ID2_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		//all page 2 stuff;// CPropertySheet
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];

		//get julian time
//		strncpy(temp,&Record[2],10);
//		temp[10]=NULL;
//		CJulianTime Julian(temp);

		//post invalid time to CEV if necessary
//		if (m_szPreviousGTime[0] != NULL) 
//		{
//			if (strcmp(m_szPreviousGTime,temp)>0)
//				m_pParent->FlagTime(1,false);			
//		}
//
//		strcpy(m_szPreviousGTime,temp);
//
//		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
//		{
//			m_bStartOfFile = false;
//			m_dCurrentDay = Julian.Day();
//		}
//
//		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
//			      Julian.Hour(), Julian.Minute(), Julian.Second() );
//		COleDateTime t2 = COleDateTime::GetCurrentTime();
//		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
//		ts += HALF_SECOND;
//
//		strcpy(temp,Julian.Date());
//		strcat(temp," ");
//		strcat(temp,Julian.Time());
//
//		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
//			(ts.GetTotalSeconds() < -m_dTime_Err ))
//		{
//			m_pParent->FlagTime(1,false);			
//			m_bTimeError = true;
//		} 

//		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
//			(ts.GetTotalSeconds() > -m_dTime_Err ))
//			m_bTimeError = false;
//		long days = ts.GetDays();
//		if (days > 0)
//			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
//		else if (days == 0)
//			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
//		else 
//		{
//			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
//				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
//		}
	
		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,
			m_pParent->GetString(CInstrument::SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME));
		
		datapage->SetDlgItemText(IDC_DATA_INQUIRETIME,
			m_pParent->GetString(CInstrument::DATAPAGE,IDC_DATA_INQUIRETIME));

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		datapage->SetDlgItemText(IDC_DATA_UNITID,temp);//11
		GetPrivateProfileString(m_pParent->m_pName,
			(m_eMonitorVers==PRE4)?"SET_UNIT_ID":"SET4_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_UNIT_ID",temp,bMatch);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status1);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 45",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 46",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,FAIL);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,FAIL);
			m_bACPowerOk = false;
		}
		else 
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,OK);
			m_bACPowerOk = true;
		}

		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,ILOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,ILOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
			m_bCountRateTamperOk = false;
		else
			m_bCountRateTamperOk = true;

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
			m_bRMSSigmaTamperOk = false;
		else
			m_bRMSSigmaTamperOk = true;

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

			datapage->SetDlgItemText(IDC_DATA_BBMOK,temp);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,temp);

			m_bBBMOk = false;
//			m_iData_Tab |= 0x0004;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BBMOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,OK);
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_COLD);
			m_dCWStart = 0;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_NORMAL);
			m_dCWStart = 1;
		}
	
		//bit 0 mii
		if (Status1 & INMII)
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,INMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,INMII_TXT);
			m_bInMII = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,NOTINMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,NOTINMII_TXT);
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			datapage->SetDlgItemText(IDC_DATA_DATAFILTER,DATAFILTER_NO);
			if (!m_bDataFiltered && m_pParent->m_bLogFilter)
				m_bDataFiltered = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_DATAFILTER,DATAFILTER_YES);
			if (m_bDataFiltered && m_pParent->m_bLogFilter)
				m_bDataFiltered = false;
		}

		//get bbm
		strncpy(temp,&Record[18],6);
		temp[6]=NULL;
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
	
		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_pParent->m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	}
}
/*
//record 1f
//should never get this as binary
int CSafePropertySheet::BinaryAnalyzeOk(BYTE *pBinaryAt)
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
	sprintf(Record,"1f%6u",
		psBinaryRecord->bbm_used
		);
	AppendCheckSum(Record);
	UpdateSummary(Record);
	strcat(Record,"\n\r");
	AnalyzeOk(Record);
	return sizeof(SBinaryRec);
}
*/
//record 1f
void CSafePropertySheet::AnalyzeOk(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,ANALYZE_OK_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
	}
}
/*
//record 30
int CSafePropertySheet::BinaryInformRec(BYTE *pBinaryAt)
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
	strcat(Record,"\n\r");
	InformRec(Record);

	return sizeof(SBinaryRec);
}
*/
//record 30
void CSafePropertySheet::InformRec(char *Record)
{
}
/*
//record 31
int CSafePropertySheet::BinaryMon_B_Rec(BYTE *pBinaryAt)
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

	for (i = 0; i < 3; i++)
	{
		if (psBinaryRecord->ll[i] >= 10.000)
			sprintf(szTemp,"%5.1f",psBinaryRecord->ll[i]);
		else
			sprintf(szTemp,"%5.3f",psBinaryRecord->ll[i]);

		strcat(Record,szTemp);
	}

	for (i = 3; i < 5; i++)
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
	strcat(Record,"\n\r");

	Mon_B_Rec(Record);

	return sizeof(SBinaryRec);
} 
*/
//record 31
void CSafePropertySheet::Mon_B_Rec(char *Record)
{
	AdjustTabsTo(PRE4);

//	int CheckSum;
//	if (LengthOk(Record,MONITOR_B_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];
		char temp2[32];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		bool bMatch;
		char temp3[4];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		//build date/time string
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());
		userpage->SetDlgItemText(IDC_USERPROG_TIME,temp);
		
		//filter method
		userpage->SetDlgItemText(IDC_USERPROG_FILTERMETH,
			(Record[12]=='1')?"Average Point":"First Point");
		temp3[0] = Record[12];
		temp3[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_FILTER_METHOD","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[12] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_FILTER_METHOD",temp3,bMatch);

		//take offset
		strncpy(temp,&Record[13],3);
		temp[3] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_TAKEOFF,temp);

		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_TAKE_OFFSET","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_TAKE_OFFSET",temp,bMatch);
	
		//maximum take offset
		strncpy(temp,&Record[16],3);
		temp[3] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_TAKEOFFMAX,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_TAKEOFFSETMAX","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_TAKEOFFSETMAX",temp,bMatch);
	
		//battery records
		strncpy(temp,&Record[19],2);
		temp[2] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_LOGBATT,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_LOG_BATTERY","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_LOG_BATTERY",temp,bMatch);
	
		//filtering enabled
		userpage->SetDlgItemText(IDC_USERPROG_FILTERENAB,
			(Record[21]=='0')?NO:YES);
		temp2[0] = NULL;
		temp3[0] = Record[21];
		temp3[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_FILTERING_ENABLED","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[21] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_FILTERING_ENABLED",temp3,bMatch);
	
		//Neutron lower limit A
		strncpy(temp,&Record[22],5);
		temp[5] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_PULSEAL,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_A_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_NEUTRON_A_LOWERLIMIT",temp,bMatch);

		//Neutron lower limit B
		strncpy(temp,&Record[27],5);
		temp[5] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_PULSEBL,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_B_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_NEUTRON_B_LOWERLIMIT",temp,bMatch);

		//Neutron lower limit C
		strncpy(temp,&Record[32],5);
		temp[5] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_PULSECL,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_NEUTRON_C_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_NEUTRON_C_LOWERLIMIT",temp,bMatch);
	
		//Gamma lower limit	1	
		strncpy(temp,&Record[37],7);
		temp[7] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_GAMMA1L,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA_1_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_GAMMA_1_LOWERLIMIT",temp,bMatch);

		//Gamma lower limit	2	
		strncpy(temp,&Record[44],7);
		temp[7] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_GAMMA2L,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_GAMMA_2_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_GAMMA_2_LOWERLIMIT",temp,bMatch);
	
		//trigger mode
		switch(Record[57])
		{
			case '0':strcpy(temp,"Threshold Only");	break;
			case '1':strcpy(temp,"Change Only");	break;
			case '2':strcpy(temp,"Threshold or Change"); break;
			case '3':strcpy(temp,"Threshold and Change");break;
			default: strcpy(temp,"Unknown");
		}
		userpage->SetDlgItemText(IDC_USERPROG_TRIGGERMODE,temp);
		temp2[0] = NULL;
		temp3[0] = Record[57];
		temp3[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_TRIGGER_MODE","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[57] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_TRIGGER_MODE",temp3,bMatch);

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
		userpage->SetDlgItemText(IDC_USERPROG_SYNCE,temp);
		temp2[0] = NULL;
		temp3[0] = Record[58];
		temp3[1] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_SYNC_ENABLED","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (Record[58] != temp2[0])
			bMatch = false;
		CurrentSetParam("SET_SYNC_ENABLED",temp3,bMatch);
	
		//Sync time
		strncpy(temp,&Record[59],2);
		temp[2] = NULL;
		strcat(temp,":");
		strncat(temp,&Record[61],2);
		strcat(temp,":");
		strncat(temp,&Record[63],2);
		userpage->SetDlgItemText(IDC_USERPROG_SYNCT,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_SYNC_TIME","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_SYNC_TIME",temp,bMatch);
	
		//Sync lower limit
		strncpy(temp,&Record[65],2);
		temp[2] = NULL;
		userpage->SetDlgItemText(IDC_USERPROG_SYNCL,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_SYNC_LOWERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_SYNC_LOWERLIMIT",temp,bMatch);
	
		//Sync upper limit
		strncpy(temp,&Record[67],1);
		temp[1] = 0;
		userpage->SetDlgItemText(IDC_USERPROG_SYNCU,temp);
		temp2[0] = NULL;
		GetPrivateProfileString(m_pParent->m_pName,"SET_SYNC_UPPERLIMIT","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_SYNC_UPPERLIMIT",temp,bMatch);
	
		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_pParent->m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

		m_iUserProg_Tab = m_pParent->m_iUserProg_Tab;
		if (m_pParent->m_iUserProg_Tab == 0x0003)
			SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOWRED);
		else if (m_iUserProg_Tab == 0x0002)
			SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOW);
		else if (m_iUserProg_Tab == 0x0001)
			SetTabColor(IDC_USERPROG_TAB,COLOR_RED);
		else if (m_iUserProg_Tab == 0x0000)
			SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);
	}
}

//record 4d
void CSafePropertySheet::AnalyzeOk3Record(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,ANALYZE_OK3_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
	}
}

//record 4f
void CSafePropertySheet::DumpOk3_Record(char *Record)
{
//	int CheckSum;
//	if (LengthOk(Record,DUMP_OK3_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[16];
//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//get bbm bytes
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
		strncpy(temp,&Record[2],10);
		temp[10] = NULL;
		//bbm = (int)atoi(temp);
//try{
		sscanf(temp,"%d",&ulbbm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 47",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		m_pCloseButton->SetNumeric(ulbbm,true);
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);
	}
}

//record 4e
void CSafePropertySheet::ID3_Record(char *Record)
{
//	AdjustTabsTo(POST4);

//	int CheckSum;
//	LengthOk(Record,MONITOR_ID3_RECORD_LENGTH);
//	if (true)
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		//all page 2 stuff;// CPropertySheet
		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
	
		char temp[64];

		//get julian time
//		strncpy(temp,&Record[2],10);
//		temp[10]=NULL;
//		CJulianTime Julian(temp);
//
//		if (Julian.Year() < 1990)
//		{
//			datapage->SetColor(IDC_DATA_INQUIRETIME,COLOR_RED);
//		}
//
//		//post invalid time to CEV if necessary
//		if (m_szPreviousGTime[0] != NULL) 
//		{
//			if (strcmp(m_szPreviousGTime,temp)>0)
//			{
//				CJulianTime JulianT(m_szPreviousGTime);
//				datapage->SetColor(IDC_DATA_INQUIRETIME,COLOR_RED);
//				summarypage->SetColor(IDC_SUMMARY_INQUIRETIME,COLOR_RED);
//			}
//		}
//
//		strcpy(m_szPreviousGTime,temp);
//
//		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
//		{
//			m_bStartOfFile = false;
//			m_dCurrentDay = Julian.Day();
//		}
//
//		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
//			      Julian.Hour(), Julian.Minute(), Julian.Second() );
//		COleDateTime t2 = COleDateTime::GetCurrentTime();
//		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
//		ts += HALF_SECOND;
//
//		strcpy(temp,Julian.Date());
//		strcat(temp," ");
//		strcat(temp,Julian.Time());
//
//		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
//			(ts.GetTotalSeconds() < -m_dTime_Err ))
//			m_bTimeError = true;
//
//		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
//			(ts.GetTotalSeconds() > -m_dTime_Err ))
//			m_bTimeError = false;
//
//		long days = ts.GetDays();
//		if (days > 0)
//			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
//		else if (days == 0)
//			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
//		else 
//		{
//			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
//				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
//		}
//		datapage->SetDlgItemText(IDC_DATA_INQUIRETIME,temp);
//		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,temp);

		summarypage->SetDlgItemText(IDC_SUMMARY_INQUIRETIME,
			m_pParent->GetString(CInstrument::SUMMARYPAGE,IDC_SUMMARY_INQUIRETIME));
		
		datapage->SetDlgItemText(IDC_DATA_INQUIRETIME,
			m_pParent->GetString(CInstrument::DATAPAGE,IDC_DATA_INQUIRETIME));

		bool bMatch;

		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		char temp2[8];
		temp2[0] = NULL;
		datapage->SetDlgItemText(IDC_DATA_UNITID,temp);
		GetPrivateProfileString(m_pParent->m_pName,"SET_UNIT_ID","",temp2,sizeof(temp2),m_szIniFile);
		bMatch = true;
		if (strcmp(temp,temp2) != 0)
			bMatch = false;
		CurrentSetParam("SET_UNIT_ID",temp,bMatch);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
//try{
			sscanf(temp,"%2hx",&Status1);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 48",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
//try{
		sscanf(temp,"%2hx",&Status2);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 49",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}

		//bit 7 acpower
		if (Status1 & ACPOWER)
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,FAIL);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,FAIL);
			m_bACPowerOk = false;
		}
		else 
		{
			datapage->SetDlgItemText(IDC_DATA_ACPOWER,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_ACPOWER,OK);
			m_bACPowerOk = true;
		}
	
		//bit 6 battery
		if (Status1 & BATTERY) 
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,ILOW);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,ILOW);
			m_bBatteryOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BATTERYOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BATTERYOK,OK);
			m_bBatteryOk = true;
		}
	
		//bit 5 tamper alarm
		if (Status1 & TAMPER)
			m_bCountRateTamperOk = false;
		else
			m_bCountRateTamperOk = true;

		//bit 4 RMS Sigma Tamper
		if (Status1 & RMSTAMPER)
			m_bRMSSigmaTamperOk = false;
		else
			m_bRMSSigmaTamperOk = true;

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

			datapage->SetDlgItemText(IDC_DATA_BBMOK,temp);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,temp);

			m_bBBMOk = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_BBMOK,OK);
			summarypage->SetDlgItemText(IDC_SUMMARY_BBMOK,OK);
			m_bBBMOk = true;
		}
	
		//bit 2 startup
		if (Status1 & CWSTART) 
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_COLD);
			m_dCWStart = 0;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OPERATION,START_NORMAL);
			m_dCWStart = 1;
		}
	
		//bit 1 reserved

		//bit 0 mii
		if (Status1 & INMII)
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,INMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,INMII_TXT);
			m_bInMII = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_OOMII,NOTINMII_TXT);
			summarypage->SetDlgItemText(IDC_SUMMARY_OOMII,NOTINMII_TXT);
			m_bInMII = false;
		}
	
		//bit 7 of sw 2
		if (Status2 & DATAFILTER_FLAG)
		{
			datapage->SetDlgItemText(IDC_DATA_DATAFILTER,DATAFILTER_NO);
			m_bDataFiltered = true;
		}
		else
		{
			datapage->SetDlgItemText(IDC_DATA_DATAFILTER,DATAFILTER_YES);
			m_bDataFiltered = false;
		}

		//get bbm
		strncpy(temp,&Record[18],10);
		temp[10]=NULL;
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);
		// SCR00235 Processing of 4F DUMPOKRESPONSE
		unsigned long ulbbm;
//try{
		sscanf(temp,"%d",&ulbbm);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 50",MB_OK);
//	AfxMessageBox(temp,MB_OK);
//}
		//int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(ulbbm,true);
	
		m_iData_Tab = m_pParent->m_iData_Tab;
		if (m_pParent->m_iData_Tab == 0x0003)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
		else if (m_iData_Tab == 0x0002)
			SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
		else if (m_iData_Tab == 0x0001)
			SetTabColor(IDC_DATA_TAB,COLOR_RED);
		else if (m_iData_Tab == 0x0000)
			SetTabColor(IDC_DATA_TAB,COLOR_GREEN);
	}
}

//record 41 (post monitor version 4 only)
void
CSafePropertySheet::GeneralConfigRecord(char *Record)
{
	AdjustTabsTo(POST4);

//	int CheckSum;
	bool bChange = false;
	
//	if (LengthOk(Record,GENERAL_CONFIG_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CString cFull(Record);
		CString cPart;

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		general4page->SetDlgItemText(IDC_GENERAL4_TIME,cPart);
		CurrentSetParam("SET4_GCR_TIME",cPart,true);	
		
		cPart = cFull.Mid(12,5);
		general4page->SetDlgItemText(IDC_GENERAL4_COUNTTIME,cPart);
		if (!CurrentSetParam("SET4_COUNT_TIME",cPart,false))
			bChange = true;

		cPart = cFull.Mid(17,1);
		CurrentSetParam("SET4_CONF_STATE",cPart,false);

		cPart = cFull.Mid(18,1);
//cPart = 'x';
		CurrentSetParam("SET4_BIAS_TYPE",cPart,false);

		cPart = cFull.Mid(19,6);
		general4page->SetDlgItemText(IDC_GENERAL4_FCHVSETPOINT,cPart);
		if (!CurrentSetParam("SET4_BIAS_SETPOINT",cPart,false))
			bChange = true;

		cPart = cFull.Mid(25,1);
		CurrentSetParam("SET4_BAUD_RATE",cPart,false);

		cPart = cFull.Mid(26,1);
		CurrentSetParam("SET4_FRAME",cPart,false);

		cPart = cFull.Mid(27,2);
		general4page->SetDlgItemText(IDC_GENERAL4_MODE,cPart);
		if (!CurrentSetParam("SET4_MODE_FLAGS1",cPart,false))
			bChange = true;

		cPart = cFull.Mid(29,2);
		general4page->SetDlgItemText(IDC_GENERAL4_MODE2,cPart);
		if (!CurrentSetParam("SET4_MODE_FLAGS2",cPart,false))
			bChange = true;

		cPart = cFull.Mid(31,2);
		general4page->SetDlgItemText(IDC_GENERAL4_BATTINT,cPart);
		if (!CurrentSetParam("SET4_BATT_INTER",cPart,false))
			bChange = true;

		cPart = cFull.Mid(33,1);
		CurrentSetParam("SET4_FILTER_CONT",cPart,false);

		cPart = cFull.Mid(34,2);
		CurrentSetParam("SET4_IMMBUFF_SZ",cPart,false);

		cPart = cFull.Mid(36,2);
		CurrentSetParam("SET4_IMMBUFFSAVE_SZ",cPart,false);

		cPart = cFull.Mid(38,2);
		CurrentSetParam("SET4_LOCBG_SZ",cPart,false);

		cPart = cFull.Mid(40,2);
		CurrentSetParam("SET4_LOCBG_END",cPart,false);

		cPart = cFull.Mid(42,5);
		general4page->SetDlgItemText(IDC_GENERAL4_STATINT,cPart);
		if (!CurrentSetParam("SET4_STATUS_INT",cPart,false))
			bChange = true;

		cPart = cFull.Mid(47,7);
		CurrentSetParam("SET4_INSP_ID",cPart,false);

		cPart = cFull.Mid(54,2);
		CurrentSetParam("SET4_UNIT_ID",cPart,false);

		cPart = cFull.Mid(56,1);
		general4page->SetDlgItemText(IDC_GENERAL4_TSSTATE,cPart);
		if (!CurrentSetParam("SET4_TIMESYNC_STATE",cPart,false))
			bChange = true;

		cPart = cFull.Mid(57,4);
		general4page->SetDlgItemText(IDC_GENERAL4_TSHOURLY,cPart);
		if (!CurrentSetParam("SET4_TIMESYNC_HOUR",cPart,false))
			bChange = true;

		cPart = cFull.Mid(61,6);
		general4page->SetDlgItemText(IDC_GENERAL4_TSDAILY,cPart);
		if (!CurrentSetParam("SET4_TIMESYNC_DAILY",cPart,false))
			bChange = true;

		cPart = cFull.Mid(67,2);
		general4page->SetDlgItemText(IDC_GENERAL4_TSLOWER,cPart);
		if (!CurrentSetParam("SET4_TIMESYNC_LOWTOL",cPart,false))
			bChange = true;

		cPart = cFull.Mid(69,2);
		general4page->SetDlgItemText(IDC_GENERAL4_TSUPPER,cPart);
		if (!CurrentSetParam("SET4_TIMESYNC_UPPTOL",cPart,false))
			bChange = true;
		
		cPart = cFull.Mid(71,5);
		general4page->SetDlgItemText(IDC_GENERAL4_GRANDP,cPart);
		if (!CurrentSetParam("SET4_FIRMWARE_VERSION",cPart,false))
			bChange = true;
		
		cPart = cFull.Mid(76,4);
		general4page->SetDlgItemText(IDC_GENERAL4_ROM,cPart);
		if (!CurrentSetParam("SET4_FIRMWARE_CHKSUM",cPart,false))
			bChange = true;
	
		if (bChange)
		{
			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
			if (m_iGeneral4_Tab == 0x0003)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral4_Tab == 0x0002)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
			else if (m_iGeneral4_Tab == 0x0001)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
			else if (m_iGeneral4_Tab == 0x0000)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
		}
	}
}

//record 42 (post monitor version 4 only)
void
CSafePropertySheet::InstrumentStatusRecord(char *Record)
{
	AdjustTabsTo(POST4);

//	int CheckSum;
//	
//	if (LengthOk(Record,INSTRUMENT_STATUS_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CString cFull(Record);
		CString cPart;
		char temp[16];
		bool bChange = false;

		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;

		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		//julian time
		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam("SET4_ISR_TIME",cPart,true);	
		general4page->SetDlgItemText(IDC_GENERAL4_ISRTIME,cPart);

		//battery voltage
		cPart = cFull.Mid(12,4);
		CurrentSetParam("SET4_BAT_VOLT",cPart,true);
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
//			m_pParent->FlagOthr(1,true);
			CurrentRed = true;
			m_bBatteryTolOk = false;
			bChange = true;
		}
		else
		{
			CurrentRed = false;
			m_bBatteryTolOk = true;
			//m_iGeneral4_Tab &= ~(0x0001);
		}

		sprintf(temp,"%5.1f",m_fLowBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY1,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMIN,temp);
		sprintf(temp,"%5.1f",m_fCurBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY2,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_BATTERYCUR,temp);
		sprintf(temp,"%5.1f",m_fHighBV);
		summarypage->SetDlgItemText(IDC_SUMMARY_BATTERY3,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_BATTERYMAX,temp);

		//+5 volts
		cPart = cFull.Mid(16,4);
		CurrentSetParam("SET4_P5READ_VOLT",cPart,true);	
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
			m_bP5VoltOk = false;
			bChange = true;
		}
		else
		{
			CurrentRed = false;
			m_bP5VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT1,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P5VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT2,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P5VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP5);
		summarypage->SetDlgItemText(IDC_SUMMARY_P5VOLT3,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P5VOLTMAX,temp);

		//plus 12 volts
		cPart = cFull.Mid(20,4);
		CurrentSetParam("SET4_P12READ_VOLT",cPart,true);	
		//+15 volts
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
			m_bP15VoltOk = false;
			bChange = true;
		}
		else
		{
			CurrentRed = false;
			m_bP15VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT1,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P15VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT2,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P15VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighP15);
		summarypage->SetDlgItemText(IDC_SUMMARY_P15VOLT3,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_P15VOLTMAX,temp);

		//-12 volts
		cPart = cFull.Mid(24,5);
		CurrentSetParam("SET4_M12READ_VOLT",cPart,true);	
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
			m_bM15VoltOk = false;
		}
		else
		{
			CurrentRed = false;
			m_bM15VoltOk = true;
		}

		sprintf(temp,"%5.1f",m_fLowM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT1,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_M15VOLTMIN,temp);
		sprintf(temp,"%5.1f",m_fCurM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT2,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_M15VOLTCUR,temp);
		sprintf(temp,"%5.1f",m_fHighM15);
		summarypage->SetDlgItemText(IDC_SUMMARY_M15VOLT3,temp);
		general4page->SetDlgItemText(IDC_GENERAL4_M15VOLTMAX,temp);
//should be displayed?**************************************************************
		//ac power status
		cPart = cFull.Mid(29,1);
		CurrentSetParam("SET4_ACPOWER_STAT",cPart,false);	

		//bias read voltage
		cPart = cFull.Mid(30,6);
		CurrentSetParam("SET4_BIASREAD_VOLT",cPart,true);	

		float fCurrent;
		float fCurrentSet;
		float fFCHVTol;
		fFCHVTol = (float)GetPrivateProfileInt(m_pParent->m_pName,"FCHVERR",5,m_szIniFile);
		fFCHVTol = fFCHVTol / 100.0f;
		CString cTemp;
		general4page->GetDlgItemText(IDC_GENERAL4_FCHVSETPOINT,cTemp);
		fCurrent = (float)atof(cPart);
		if (cTemp.GetLength())
			fCurrentSet = (float)atof(cTemp);
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
		general4page->SetDlgItemText(IDC_GENERAL4_FCHIGHVMIN,temp);

		sprintf(temp,"%6.1f",fCurrent);
		general4page->SetDlgItemText(IDC_GENERAL4_FCHIGHVCUR,temp);
		if ((fCurrent < (1.0-fFCHVTol)*fCurrentSet) || (fCurrent > (1.0+fFCHVTol)*fCurrentSet))
		{
			m_bFCHVError = true;
			bChange = true;
		}
		else
		{
			m_bFCHVError = false;
		}
			
		sprintf(temp,"%6.1f",m_fFBHigh);
		general4page->SetDlgItemText(IDC_GENERAL4_FCHIGHVMAX,temp);

		//ic read voltage
		cPart = cFull.Mid(36,5);
		CurrentSetParam("SET4_IONREAD_VOLT",cPart,true);
		float fICHVTol;
		fICHVTol = (float)GetPrivateProfileInt(m_pParent->m_pName,"ICHVERR",10,m_szIniFile);
		fICHVTol = fICHVTol / 100.0f;
		general4page->GetDlgItemText(IDC_GENERAL4_ICHVSETPOINT,cTemp);
		fCurrent = (float)atof(cPart);
		if (cTemp.GetLength())
			fCurrentSet = (float)atof(cTemp);
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
		general4page->SetDlgItemText(IDC_GENERAL4_ICHIGHVMIN,temp);

		sprintf(temp,"%6.1f",fCurrent);
		general4page->SetDlgItemText(IDC_GENERAL4_ICHIGHVCUR,temp);
		if ((fCurrent < (1.0-fICHVTol)*fCurrentSet) || (fCurrent > (1.0+fICHVTol)*fCurrentSet))
		{
			m_bICHVError = true;
			bChange = true;
		}
		else
		{
			m_bICHVError = false;
		}

		sprintf(temp,"%6.1f",m_fDIRHigh);
		general4page->SetDlgItemText(IDC_GENERAL4_ICHIGHVMAX,temp);

		//on board temperature
		cPart = cFull.Mid(41,5);
		CurrentSetParam("SET4_ONBOARDREAD_TEMP",cPart,true);	

		cPart = cFull.Mid(46,5);
		CurrentSetParam("SET4_OFFBOARDREAD_TEMP",cPart,true);	

		if (bChange)
		{
			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
			if (m_iGeneral4_Tab == 0x0003)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral4_Tab == 0x0002)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
			else if (m_iGeneral4_Tab == 0x0001)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
			else if (m_iGeneral4_Tab == 0x0000)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
		}
	}
}

//record 43 (post monitor version 4 only)
void
CSafePropertySheet::InstrumentInfoRecord(char *Record)
{
	AdjustTabsTo(POST4);

//	int CheckSum;
//	if (LengthOk(Record,INSTRUMENT_INFO_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CString cFull(Record);
		CString cPart;

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		bool bChange = false;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam("SET4_IIR_TIME",cPart,true);	

		cPart = cFull.Mid(12,12);
		CurrentSetParam("SET4_MPB_ID",cPart,false);	

		cPart = cFull.Mid(24,2);
		CurrentSetParam("SET4_MPBXILINX_VER",cPart,false);	

		cPart = cFull.Mid(26,2);
		CurrentSetParam("SET4_DCMSER_NUM",cPart,false);	

		cPart = cFull.Mid(28,2);
		CurrentSetParam("SET4_DCMBRD_TYPE",cPart,false);	

		cPart = cFull.Mid(30,2);
		CurrentSetParam("SET4_DCMXILINX_VER",cPart,false);	

		cPart = cFull.Mid(32,2);
		CurrentSetParam("SET4_DCMACTEL_VER",cPart,false);	

		cPart = cFull.Mid(34,1);
		CurrentSetParam("SET4_DCMPRESENCE",cPart,false);	

		cPart = cFull.Mid(35,2);
		CurrentSetParam("SET4_TPCSER_NUM",cPart,false);	

		cPart = cFull.Mid(37,2);
		CurrentSetParam("SET4_TPCBRD_TYPE",cPart,false);	

		cPart = cFull.Mid(39,2);
		CurrentSetParam("SET4_TPCXILINX_VER",cPart,false);	

		cPart = cFull.Mid(41,1);
		CurrentSetParam("SET4_TPCPRESENCE",cPart,false);	

		cPart = cFull.Mid(42,2);
		CurrentSetParam("SET4_PSSER_NUM",cPart,false);	

		cPart = cFull.Mid(44,2);
		CurrentSetParam("SET4_PSBRD_VER",cPart,false);	

		cPart = cFull.Mid(46,2);
		CurrentSetParam("SET4_PSBRD_TYPE",cPart,false);	

		cPart = cFull.Mid(48,2);
		CurrentSetParam("SET4_PSXILINX_VER",cPart,false);	
	}
}

//record 44 (post monitor version 4 only)
void
CSafePropertySheet::DualCurrentModeConfigRecord(char *Record)
{
	AdjustTabsTo(POST4);
	
//	int CheckSum;
//	
//	if (LengthOk(Record,DUAL_CURRENTMODE_CONFIG_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		CString cFull(Record);
		CString cPart;

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		bool bChange = false;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam("SET4_DCMCR_TIME",cPart,true);	
		general4page->SetDlgItemText(IDC_GENERAL4_G1DATE,cjTime.Date());
		general4page->SetDlgItemText(IDC_GENERAL4_G2DATE,cjTime.Time());

		cPart = cFull.Mid(12,1);
		CurrentSetParam("SET4_OFFSET_MODE",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_DISCM,cPart);
		
		cPart = cFull.Mid(13,3);
		CurrentSetParam("SET4_NOMOS_INTERVAL",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_G1TIME,cPart);

		cPart = cFull.Mid(16,3);
		CurrentSetParam("SET4_MAXOS_INTERVAL",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_G2TIME,cPart);

		cPart = cFull.Mid(19,1);
		CurrentSetParam("SET4_G0_GAINMODE",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_GAMMA1G,cPart);

		cPart = cFull.Mid(20,2);
		CurrentSetParam("SET4_G0_MAXFIX",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_GAMMA1M,cPart);

		cPart = cFull.Mid(22,2);
		CurrentSetParam("SET4_G0_PRES",cPart,true);
		general4page->SetDlgItemText(IDC_GENERAL4_MAXG1,cPart);

		cPart = cFull.Mid(24,1);
		CurrentSetParam("SET4_G1_GAINMODE",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_GAMMA2G,cPart);

		cPart = cFull.Mid(25,2);
		CurrentSetParam("SET4_G1_MAXFIX",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_GAMMA2M,cPart);

		cPart = cFull.Mid(27,2);
		CurrentSetParam("SET4_G1_PRES",cPart,true);
		general4page->SetDlgItemText(IDC_GENERAL4_MAXG2,cPart);

		CString cTemp;
		cPart = cFull.Mid(29,5);
		float fICHVErr;
		fICHVErr = (float)GetPrivateProfileInt(m_pParent->m_pName,"ICHVERR",10,m_szIniFile);
		fICHVErr = fICHVErr/100.0f;
		CurrentSetParam("SET4_ICHV_SETPOINT",cPart,false);

		general4page->SetDlgItemText(IDC_GENERAL4_ICHVSETPOINT,cPart);
		general4page->GetDlgItemText(IDC_GENERAL4_ICHIGHVCUR,cTemp);
		if (cTemp.GetLength())
		{
			double dCurrent = atof(cTemp);
			double dCurrentSet = atof(cPart);
			if ((dCurrent < (1.0-fICHVErr)*dCurrentSet) || (dCurrent > (1.0+fICHVErr)*dCurrentSet))
			{
				bChange = true;
				if (!m_bICHVError)
					m_bICHVError = true;
			}
			else
			{
				if (m_bICHVError)
					m_bICHVError = false;
			}
		}

		cPart = cFull.Mid(34,1);
		CurrentSetParam("SET4_ICHV_CONFIG",cPart,false);
		general4page->SetDlgItemText(IDC_GENERAL4_HVCONFIG,cPart);

		cPart = cFull.Mid(35,6);
		CurrentSetParam("SET4_GUNCERT_MULT",cPart,false);	
		general4page->SetDlgItemText(IDC_GENERAL4_GAMMAUNMULT,cPart);

		if (bChange)
		{
			m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
			if (m_iGeneral4_Tab == 0x0003)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
			else if (m_iGeneral4_Tab == 0x0002)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
			else if (m_iGeneral4_Tab == 0x0001)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
			else if (m_iGeneral4_Tab == 0x0000)
				SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
		}
	}
}

//record 45 (post monitor version 4 only)
void
CSafePropertySheet::TriggerConfigRecord(char *Record)
{
	AdjustTabsTo(POST4);
	
//	int CheckSum;
//
//	if (LengthOk(Record,TRIGGER_CONFIG_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
		CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;
		CSetupData* setdata4page = m_pParent->m_pSetup4Data;
//		CModParam* modparpage  = m_pParent->m_pModParams;
		bool bChange = false;
		bool bDoYellow = false;
	
		CString cFull(Record);
		CString cPart;
		CString cPart2;
		CString cSet;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		CurrentSetParam("SET4_TCR_TIME",cPart,true);
		user4page->SetDlgItemText(IDC_USER4PROG_TIME,cPart);

		cPart = cFull.Mid(12,1);
		if (!CurrentSetParam("SET4_TRIGGER4_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,"OR");
		else if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC1,cPart);
		
		cPart = cFull.Mid(13,16);
		if (!CurrentSetParam("SET4_TRIGGER4_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdata4page->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		unsigned short usValue;
		cPart2 = cPart[0];
//try{
		sscanf(cPart2,"%02x",(unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 51",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE1,"");

		cPart2 = cPart[2];
//try{
		sscanf(cPart2,"%02x",(unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 52",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE2,"");

		cPart2 = cPart[4];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 53",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE3,"");

		cPart2 = cPart[6];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 54",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE4,"");

		cPart2 = cPart[8];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 55",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE5,"");

		cPart2 = cPart[10];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 56",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE6,"");

		cPart2 = cPart[12];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 57",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE7,"");

		cPart2 = cPart[14];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 58",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_1CHTYPE8,"");
		
		cPart = cFull.Mid(29,1);
		if (!CurrentSetParam("SET4_TRIGGER5_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC2,cPart);
		
		cPart = cFull.Mid(30,16);
		if (!CurrentSetParam("SET4_TRIGGER5_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdata4page->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 59",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE1,"");

		cPart2 = cPart[2];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 60",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE2,"");

		cPart2 = cPart[4];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 61",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE3,"");

		cPart2 = cPart[6];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 62",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE4,"");

		cPart2 = cPart[8];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 63",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE5,"");

		cPart2 = cPart[10];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 64",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE6,"");

		cPart2 = cPart[12];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 65",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE7,"");

		cPart2 = cPart[14];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 66",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_2CHTYPE8,"");
		
		cPart = cFull.Mid(46,1);
		if (!CurrentSetParam("SET4_TRIGGER6_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC3,cPart);
		
		cPart = cFull.Mid(47,16);
		if (!CurrentSetParam("SET4_TRIGGER6_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdata4page->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 67",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE1,"");

		cPart2 = cPart[2];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 68",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE2,"");

		cPart2 = cPart[4];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 69",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE3,"");

		cPart2 = cPart[6];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 70",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE4,"");

		cPart2 = cPart[8];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 71",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE5,"");

		cPart2 = cPart[10];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 72",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE6,"");
		
		cPart2 = cPart[12];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 73",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE7,"");

		cPart2 = cPart[14];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 74",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_3CHTYPE8,"");
		
		cPart = cFull.Mid(63,1);
		if (!CurrentSetParam("SET4_TRIGGER7_LOGIC",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
		}
		else
			bDoYellow = false;
		if (cPart == "1")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,"OR");
		else if (cPart == "0")
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,"AND");
		else
			user4page->SetDlgItemText(IDC_USER4PROG_LOGIC4,cPart);
		
		cPart = cFull.Mid(64,16);
		if (!CurrentSetParam("SET4_TRIGGER7_CHTYPE",cPart,false))
		{
			bChange = true;
			bDoYellow = true;
			cSet = setdata4page->GetMatchString();
			while (cSet.GetLength() < 16) cSet += ' ';
		}
		else
			bDoYellow = false;

		cPart2 = cPart[0];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 75",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[1];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE1,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE1,"");

		cPart2 = cPart[2];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 76",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[3];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE2,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE2,"");

		cPart2 = cPart[4];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 77",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[5];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE3,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE3,"");

		cPart2 = cPart[6];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 78",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[7];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE4,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE4,"");

		cPart2 = cPart[8];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 79",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[9];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE5,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE5,"");

		cPart2 = cPart[10];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 80",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[11];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE6,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE6,"");

		cPart2 = cPart[12];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 81",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[13];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE7,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE7,"");

		cPart2 = cPart[14];
//try{
		sscanf(cPart2,"%02x", (unsigned int *)&usValue); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 82",MB_OK);
//	AfxMessageBox(cPart2,MB_OK);
//}
		if (usValue >= 0x08)
		{
			cPart2.Format("%01x",usValue & 0x0007);
			cPart2 += "/";
			cPart2 += cPart[15];
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE8,cPart2);
		}
		else
			user4page->SetDlgItemText(IDC_USER4PROG_4CHTYPE8,"");

		if (bChange)
		{
			m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
			if (m_iUser4Prog_Tab == 0x0003)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
			else if (m_iUser4Prog_Tab == 0x0002)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
			else if (m_iUser4Prog_Tab == 0x0001)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
			else if (m_iUser4Prog_Tab == 0x0000)
				SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);
		}
	}
}

//record 46 through 4c (post monitor version 4 only)
void
CSafePropertySheet::ChannelNConfigRecord(char *Record,short message_type)
{
	AdjustTabsTo(POST4);

//	int CheckSum;
//	if (LengthOk(Record,CHANNELN_CONFIG_RECORD_LENGTH))
//		CheckSum = Checksum(Record);
//	else
//		CheckSum = CHECKSUM_BAD;
//	if (CheckSum == CHECKSUM_GOOD)
	{
		bool bChange = false; 

		char cChan = '0';
		int  iChan = 0;

		switch(message_type) {
			case CHANNEL0_CONFIG_RECORD: 
				cChan = '0'; iChan = 00;break;
			case CHANNEL1_CONFIG_RECORD: 
				cChan = '1'; iChan = 10; break;
			case CHANNEL2_CONFIG_RECORD: 
				cChan = '2'; iChan = 20; break;
			case CHANNEL3_CONFIG_RECORD: 
				cChan = '3'; iChan = 30; break;
			case CHANNEL4_CONFIG_RECORD: 
				cChan = '4'; iChan = 40; break;
			case CHANNEL5_CONFIG_RECORD: 
				cChan = '5'; iChan = 50; break;
			case CHANNEL6_CONFIG_RECORD: 
				cChan = '6'; iChan = 60; break;
		}

//		CSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//		CDataStat* datapage    = m_pParent->m_pDataStatus;
//		CUserProg* userpage    = m_pParent->m_pUserProgStatus;
		CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;
//		CModParam* modparpage  = m_pParent->m_pModParams;

		CString cFull(Record);
		CString cPart;
		CString cName;

		cPart = cFull.Mid(2,10);
		CJulianTime cjTime(cPart);
		cPart = cjTime.Date();
		cPart += " ";
		cPart += cjTime.Time();
		cName = "SET4_CH0_TIME";
		cName.SetAt(7,cChan);
		CurrentSetParam(cName,cPart,true);

		cPart = cFull.Mid(12,1);
		cName = "SET4_CH0_USED";
		cName.SetAt(7,cChan);
		CurrentSetParam(cName,cPart,false);
		bool bEnable;
		if (cPart == '1')
		{
			CString cTemp;
			cTemp.Format("Chan. %c",cChan);
			user4page->SetDlgItemText(IDC_USER4PROG_CH0USED+iChan/10,cTemp);
			bEnable = true;
		}
		else
		{
			user4page->SetDlgItemText(IDC_USER4PROG_CH0USED+iChan/10,"Not Used");
			bEnable = false;
		}

		if (bEnable)
		{

			cPart = cFull.Mid(13,1);
			cName = "SET4_CH0_T1TYPEDIR";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;

			if (cPart[0] == '0')				
				cPart = "Abs/-";				//dgp 16 dec 2004 (I had bit meaning swapped) 1.9.0.8
			else if (cPart[0] == '1')
				cPart = "Rel/-";				//dgp 16 dec 2004			
			else if (cPart[0] == '2')
				cPart = "Abs/+";				//dgp 16 dec 2004
			else if (cPart[0] == '3')
				cPart = "Rel/+";				//dgp 16 dec 2004
/*
			if (cPart[0] == '0')
				cPart = "Abs/-";
			else if (cPart[0] == '1')
				cPart = "Abs/+";
			else if (cPart[0] == '2')
				cPart = "Rel/-";
			else if (cPart[0] == '3')
				cPart = "Rel/+";
*/
			user4page->SetDlgItemText(IDC_USER4PROG_C00+iChan,cPart);
 
			cPart = cFull.Mid(14,9);
			cName = "SET4_CH0_T1VALUE";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			cPart.TrimLeft();
			user4page->SetDlgItemText(IDC_USER4PROG_C01+iChan,cPart);

			cPart = cFull.Mid(23,2);
			cName = "SET4_CH0_T1EEV";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			unsigned short ival1,ival2;
//try{
			sscanf(cPart,"%1x%1x", (unsigned int *)&ival1, (unsigned int *)&ival2); // added typecast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 83",MB_OK);
//	AfxMessageBox(cPart,MB_OK);
//}
			cPart.Format("%d/%d",ival1,ival2);
			user4page->SetDlgItemText(IDC_USER4PROG_C02+iChan,cPart);

			cPart = cFull.Mid(25,1);
			cName = "SET4_CH0_T2TYPEDIR";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;

			if (cPart[0] == '0')
				cPart = "Abs/-";				//dgp 16 dec 2004 (I had bit meaning swapped) 1.9.0.8
			else if (cPart[0] == '1')
				cPart = "Rel/-";				//dgp 16 dec 2004
			else if (cPart[0] == '2')
				cPart = "Abs/+";				//dgp 16 dec 2004
			else if (cPart[0] == '3')			
				cPart = "Rel/+";				//dgp 16 dec 2004
/*
			if (cPart[0] == '0')
				cPart = "Abs/-";
			else if (cPart[0] == '1')
				cPart = "Abs/+";
			else if (cPart[0] == '2')
				cPart = "Rel/-";
			else if (cPart[0] == '3')
				cPart = "Rel/+";
*/
			user4page->SetDlgItemText(IDC_USER4PROG_C03+iChan,cPart);

			cPart = cFull.Mid(26,9);
			cName = "SET4_CH0_T2VALUE";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			cPart.TrimLeft();
			user4page->SetDlgItemText(IDC_USER4PROG_C04+iChan,cPart);

			cPart = cFull.Mid(35,2);
			cName = "SET4_CH0_T2EEV";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
//try{
				sscanf(cPart,"%1x%1x", (unsigned int *)&ival1, (unsigned int *)&ival2); //added type cast gjk
//}
//catch(...)
//{
//	AfxMessageBox("Error at 84",MB_OK);
//	AfxMessageBox(cPart,MB_OK);
//}
			cPart.Format("%d/%d",ival1,ival2);
			user4page->SetDlgItemText(IDC_USER4PROG_C05+iChan,cPart);

			cPart = cFull.Mid(37,4);
			cName = "SET4_CH0_CHANGMULT";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			cPart.TrimLeft();
			user4page->SetDlgItemText(IDC_USER4PROG_C06+iChan,cPart);

			cPart = cFull.Mid(41,1);
			cName = "SET4_CH0_CHANGENTCNT";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			user4page->SetDlgItemText(IDC_USER4PROG_C07+iChan,cPart);

			cPart = cFull.Mid(42,7);
			cName = "SET4_CH0_FILTERLIM";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			cPart.TrimLeft();
			user4page->SetDlgItemText(IDC_USER4PROG_C08+iChan,cPart);

			cPart = cFull.Mid(49,3);
			cName = "SET4_CH0_HYSTER";
			cName.SetAt(7,cChan);
			if (!CurrentSetParam(cName,cPart,false))
				bChange = true;
			cPart.TrimLeft();
			cPart += "%";
			user4page->SetDlgItemText(IDC_USER4PROG_C09+iChan,cPart);

			if (bChange)
			{
				m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
				if (m_iUser4Prog_Tab == 0x0003)
					SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
				else if (m_iUser4Prog_Tab == 0x0002)
					SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
				else if (m_iUser4Prog_Tab == 0x0001)
					SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
				else if (m_iUser4Prog_Tab == 0x0000)
					SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);
			}
		}
		else
		{
			CString cPart("");
			user4page->SetDlgItemText(IDC_USER4PROG_C00+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C01+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C02+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C03+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C04+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C05+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C06+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C07+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C08+iChan,cPart);
			user4page->SetDlgItemText(IDC_USER4PROG_C09+iChan,cPart);
		}
	}
}

int 
CSafePropertySheet::Checksum(char* Record)
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
//	AfxMessageBox("Error at 85",MB_OK);
//	AfxMessageBox(&Record[nbytes],MB_OK);
//}
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

//	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
//	CDataStat* datapage    = m_pParent->m_pDataStatus;
//	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("CheckSum: %d/%H:%M:%S ");
	DateTime += Record;
	if (GetPageIndex(generalpage) >= 0)
	{
		generalpage->SetDlgItemText(IDC_GENERAL_CHECKSUM,(char*)LPCTSTR(DateTime));
		m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
		if (m_pParent->m_iGeneral_Tab == 0x0003)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral_Tab == 0x0002)
			SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
		else if (m_iGeneral_Tab == 0x0001)
			SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
		else if (m_iGeneral_Tab == 0x0000)
			SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);
	}

	if (GetPageIndex(general4page) >= 0)
	{
		general4page->SetDlgItemText(IDC_GENERAL4_CHECKSUM,(char*)LPCTSTR(DateTime));
		m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
//		m_iGeneral4_Tab |= 0x0001;
		if (m_iGeneral4_Tab == 0x0003)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
		else if (m_iGeneral4_Tab == 0x0002)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
		else if (m_iGeneral4_Tab == 0x0001)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
		else if (m_iGeneral4_Tab == 0x0000)
			SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);
	}

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CSafePropertySheet::LengthOk(char* Record, int length)
{
	//Check the length of the record	
	if (strlen(Record) == (unsigned)length)
		return true;

    return false;
}

int CSafePropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pCloseButton = new CColorButton();
	m_pCloseButton->SetBase(10);
	m_pCloseButton->SetPreNum("BBM: ");

	m_pPauseButton = new CButton();
	m_pAckFlagsButton = new CButton();
	m_pPrintButton = new CButton();
	m_pToClose = new CButton();
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);
	return 0;
}

void CSafePropertySheet::OnCloseButton()
{
	KillTimer(1);
	::PostMessage(m_hWnd,WM_SYSCOMMAND,SC_CLOSE,NULL);
}

void CSafePropertySheet::UpdateTabs()
{
	m_iGeneral4_Tab = m_pParent->m_iGeneral4_Tab;
	if (m_iGeneral4_Tab == 0x0003)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOWRED);
	else if (m_iGeneral4_Tab == 0x0002)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_YELLOW);
	else if (m_iGeneral4_Tab == 0x0001)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_RED);
	else if (m_iGeneral4_Tab == 0x0000)
		SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);

	m_iGeneral_Tab = m_pParent->m_iGeneral_Tab;
	if (m_iGeneral_Tab == 0x0003)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOWRED);
	else if (m_iGeneral_Tab == 0x0002)
		SetTabColor(IDC_GENERAL_TAB,COLOR_YELLOW);
	else if (m_iGeneral_Tab == 0x0001)
		SetTabColor(IDC_GENERAL_TAB,COLOR_RED);
	else if (m_iGeneral_Tab == 0x0000)
		SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);

	m_iData_Tab = m_pParent->m_iData_Tab;
	if (m_iData_Tab == 0x0003)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOWRED);
	else if (m_iData_Tab == 0x0002)
		SetTabColor(IDC_DATA_TAB,COLOR_YELLOW);
	else if (m_iData_Tab == 0x0001)
		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	else if (m_iData_Tab == 0x0000)
		SetTabColor(IDC_DATA_TAB,COLOR_GREEN);

	m_iUserProg_Tab = m_pParent->m_iUserProg_Tab;
	if (m_iUserProg_Tab == 0x0003)
		SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOWRED);
	else if (m_iUserProg_Tab == 0x0002)
		SetTabColor(IDC_USERPROG_TAB,COLOR_YELLOW);
	else if (m_iUserProg_Tab == 0x0001)
		SetTabColor(IDC_USERPROG_TAB,COLOR_RED);
	else if (m_iUserProg_Tab == 0x0000)
		SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);

	m_iUser4Prog_Tab = m_pParent->m_iUser4Prog_Tab;
	if (m_iUser4Prog_Tab == 0x0003)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOWRED);
	else if (m_iUser4Prog_Tab == 0x0002)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_YELLOW);
	else if (m_iUser4Prog_Tab == 0x0001)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_RED);
	else if (m_iUser4Prog_Tab == 0x0000)
		SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);

	m_iInstSetup_Tab = m_pParent->m_iInstSetup_Tab;
	if (m_iInstSetup_Tab == 0x0003)
		SetTabColor(IDC_INSTSETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInstSetup_Tab == 0x0002)
		SetTabColor(IDC_INSTSETUP_TAB,COLOR_YELLOW);
	else if (m_iInstSetup_Tab == 0x0001)
		SetTabColor(IDC_INSTSETUP_TAB,COLOR_RED);
	else if (m_iInstSetup_Tab == 0x0000)
		SetTabColor(IDC_INSTSETUP_TAB,COLOR_GREEN);

	m_iInst4Setup_Tab = m_pParent->m_iInst4Setup_Tab;
	if (m_iInst4Setup_Tab == 0x0003)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOWRED);
	else if (m_iInst4Setup_Tab == 0x0002)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_YELLOW);
	else if (m_iInst4Setup_Tab == 0x0001)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_RED);
	else if (m_iInst4Setup_Tab == 0x0000)
		SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);
}

void CSafePropertySheet::SetTabColor(int Tab,COLORREF Color)
{

	CSummary*  summarypage =	m_pParent->m_pSummaryPage;
	CGenlStat* generalpage =	m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page =	m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    =	m_pParent->m_pDataStatus;
	CUserProg* userpage    =	m_pParent->m_pUserProgStatus;
	CUser4Prog* user4page    =	m_pParent->m_pUser4ProgStatus;
	CModParam* modparpage  =	m_pParent->m_pModParams;
	CSetupData* setdatapage =	m_pParent->m_pSetupData;
	CSetupData* setdata4page =	m_pParent->m_pSetup4Data;
	CLegend* legend =			m_pParent->m_pLegend;

	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	switch(Color) {
	case COLOR_RED:
		pTabCtrlItem.iImage = 0;
		break;
	case COLOR_GREEN:
		pTabCtrlItem.iImage = 1;
		break;
	case COLOR_BLUE:
		pTabCtrlItem.iImage = 2;
		break;
	case COLOR_YELLOW:
		pTabCtrlItem.iImage = 3;
		break;
	case COLOR_CYAN:
		pTabCtrlItem.iImage = 4;
		break;
	case COLOR_MAGENTA:
		pTabCtrlItem.iImage = 5;
		break;
	case COLOR_WHITE:
		pTabCtrlItem.iImage = 6;
		break;
	case COLOR_BLACK:
		pTabCtrlItem.iImage = 7;
		break;
	case COLOR_YELLOWRED:
		pTabCtrlItem.iImage = 10;
		break;
	default:
		pTabCtrlItem.iImage = 6;
	};

	int iTab = -1;
	switch (Tab) {
	case IDC_GENERAL_TAB:
		iTab = GetPageIndex(generalpage);
		break;
	case IDC_DATA_TAB:
		iTab = GetPageIndex(datapage);
		break;
	case IDC_USERPROG_TAB:
		iTab = GetPageIndex(userpage);
		break;
	case IDC_USER4PROG_TAB:
		iTab = GetPageIndex(user4page);
		break;
	case IDC_INSTSETUP_TAB:
		iTab = GetPageIndex(setdatapage);
		break;
	case IDC_INST4SETUP_TAB:
		iTab = GetPageIndex(setdata4page);
		break;
	case IDC_GENERAL4_TAB:
		iTab = GetPageIndex(general4page);
		break;
	};

	if (iTab >= 0)
	{
		GetTabControl()->SetItem(iTab,&pTabCtrlItem);
		GetTabControl()->Invalidate();
	}
}

void CSafePropertySheet::OnAckButton()
{
	m_dToClose = 0;
	//acknowledge the four button flags
	//acknowledge the checksum error

	CSummary*  summarypage = m_pParent->m_pSummaryPage;
	CGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CGenl4Stat* general4page = m_pParent->m_pGeneral4StatPage;
	CDataStat* datapage    = m_pParent->m_pDataStatus;
	CUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CUser4Prog* user4page    = m_pParent->m_pUser4ProgStatus;
//	CModParam* modparpage  = m_pParent->m_pModParams;
	CSetupData*  setdatapage = m_pParent->m_pSetupData;
	CSetupData*  setdata4page = m_pParent->m_pSetup4Data;
	
	summarypage->ResetColors();
	if (GetPageIndex(generalpage)>=0)
		generalpage->ResetColors();
	if (GetPageIndex(general4page)>=0)
		general4page->ResetColors();
	datapage->ResetColors();
	if (GetPageIndex(userpage)>=0)
		userpage->ResetColors();
	if (GetPageIndex(user4page)>=0)
		user4page->ResetColors();
			
	SetTabColor(IDC_GENERAL_TAB,COLOR_GREEN);   //GRAND Setup
	SetTabColor(IDC_GENERAL4_TAB,COLOR_GREEN);	//GRAND ver 4 Setup
	SetTabColor(IDC_DATA_TAB,COLOR_GREEN);      //Data Status
	SetTabColor(IDC_USERPROG_TAB,COLOR_GREEN);  //Monitor Parameters
	SetTabColor(IDC_USER4PROG_TAB,COLOR_GREEN);  //Monitor Parameters
	SetTabColor(IDC_INSTSETUP_TAB,COLOR_GREEN); //Inst. Settings
	SetTabColor(IDC_INST4SETUP_TAB,COLOR_GREEN);//inst 4 settings

	m_iData_Tab = 0x0000;
	m_iGeneral_Tab = 0x0000;
	m_iGeneral4_Tab = 0x0000;
	m_iUserProg_Tab = 0x0000;
	m_iUser4Prog_Tab = 0x0000;
	m_iInstSetup_Tab = 0x0000;
	m_iInst4Setup_Tab = 0x0000;

	m_pParent->AckButton();

	if (GetPageIndex(setdatapage)>=0)
		setdatapage->ResetColors();
	if (GetPageIndex(setdata4page)>=0)
		setdata4page->ResetColors();

	Invalidate();
}

void CSafePropertySheet::OnPauseButton()
{
	m_dToClose = 0;
	CModParam* modparpage = m_pParent->m_pModParams;
	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
		modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
	}
}

void CSafePropertySheet::OnPrintButton()
{
/*
	char *sMsg;
	int nBytesRead;

#ifdef _DEBUG
	//test generation of General Configuration Record message
	//411661096173    1141234.095FC5A904252015 3    5 1196462A25030 72040 5 304.10474B4D
	sMsg = (char*)malloc(85);
	nBytesRead = 84;
	if (sMsg)
	{
		strcpy(sMsg,"41");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"11111");
		strcat(sMsg,"FF");
		strcat(sMsg,"2222.2");
		strcat(sMsg,"00");
		strcat(sMsg,"FF");
		strcat(sMsg,"EE");
		strcat(sMsg,"33");
		strcat(sMsg,"D");
		strcat(sMsg,"44");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77");
		strcat(sMsg,"88888");
		strcat(sMsg,"9999999");
		strcat(sMsg,"CC");
		strcat(sMsg,"B");
		strcat(sMsg,"1234");
		strcat(sMsg,"HHMMSS");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77.88");
		strcat(sMsg,"ABCD");
		strcat(sMsg,"dc");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of Instrument status record
	//42166109617312.4 4.911.9-12.001342.0260.0 80.1240.2D0
	sMsg = (char*)malloc(56);
	nBytesRead = 55;
	if (sMsg)
	{
		strcpy(sMsg,"42");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"12.3");
		strcat(sMsg,"45.6");
		strcat(sMsg,"78.9");
		strcat(sMsg,"-12.3");
		strcat(sMsg,"1");
		strcat(sMsg,"1234.5");
		strcat(sMsg,"123.4");
		strcat(sMsg,"456.7");
		strcat(sMsg,"123.4");
		strcat(sMsg,"1d");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of instrument information record
	//4316610961740000089131A921FDDF01031FFFF041FF04FF01D6
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"43");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"aaaaaaaaaaaa");
		strcat(sMsg,"11");
		strcat(sMsg,"22");
		strcat(sMsg,"33");
		strcat(sMsg,"44");
		strcat(sMsg,"55");
		strcat(sMsg,"66");
		strcat(sMsg,"77");
		strcat(sMsg,"88");
		strcat(sMsg,"99");
		strcat(sMsg,"aa");
		strcat(sMsg,"bb");
		strcat(sMsg,"cc");
		strcat(sMsg,"dd");
		strcat(sMsg,"ee");
		strcat(sMsg,"98");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of Dual Current Mode Configuration Record
	//4416610961741 45128111 5111 8255.01 0.002D4
	sMsg = (char*)malloc(46);
	nBytesRead = 45;
	if (sMsg)
	{
		strcpy(sMsg,"44");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"A");
		strcat(sMsg,"123");
		strcat(sMsg,"123");
		strcat(sMsg,"4");
		strcat(sMsg,"56");
		strcat(sMsg,"78");
		strcat(sMsg,"9");
		strcat(sMsg,"12");
		strcat(sMsg,"34");
		strcat(sMsg,"567.8");
		strcat(sMsg,"9");
		strcat(sMsg,"1.2345");
		strcat(sMsg,"61");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test generation of trigger configuration record
	//451661096174187000000000000001818200000000000008090A0000000000018092A100000000009E
	sMsg = (char*)malloc(85);
	nBytesRead = 84;
	if (sMsg)
	{
		strcpy(sMsg,"45");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"A");
		strcat(sMsg,"aabbccddeeffaabb");
		strcat(sMsg,"B");
		strcat(sMsg,"1122334455667788");
		strcat(sMsg,"C");
		strcat(sMsg,"AABBCCDDEEFFGGHH");
		strcat(sMsg,"D");
		strcat(sMsg,"9988776655443322");
		strcat(sMsg,"98");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test chan 0 configuration record
	//46166109617401   1234.5122    100.03A 0.1F 0.0020120A1
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"46");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"1");
		strcat(sMsg,"B");
		strcat(sMsg,"1234567.8");
		strcat(sMsg,"CC");
		strcat(sMsg,"D");
		strcat(sMsg,"9876543.2");
		strcat(sMsg,"EE");
		strcat(sMsg,"12.3");
		strcat(sMsg,"F");
		strcat(sMsg,"65.4321");
		strcat(sMsg,"321");
		strcat(sMsg,"ed");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test chan 1 configuration record
	//47166109617412    100.03A1    555.0CD 1.53 0.2000 809E
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"47");
		strcat(sMsg,"7234567890");
		strcat(sMsg,"7");
		strcat(sMsg,"7");
		strcat(sMsg,"7234567.8");
		strcat(sMsg,"7C");
		strcat(sMsg,"7");
		strcat(sMsg,"7876543.2");
		strcat(sMsg,"7E");
		strcat(sMsg,"72.3");
		strcat(sMsg,"7");
		strcat(sMsg,"75.4321");
		strcat(sMsg,"721");
		strcat(sMsg,"c8");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 2 configuration record
	//48166109617412    100.03A2    100.03A 1.53 0.2000 807F
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"48");
		strcat(sMsg,"8234567890");
		strcat(sMsg,"8");
		strcat(sMsg,"8");
		strcat(sMsg,"8234567.8");
		strcat(sMsg,"8C");
		strcat(sMsg,"8");
		strcat(sMsg,"8876543.2");
		strcat(sMsg,"8E");
		strcat(sMsg,"82.3");
		strcat(sMsg,"8");
		strcat(sMsg,"85.4321");
		strcat(sMsg,"821");
		strcat(sMsg,"d5");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 3 configuration record
	//49166109617403   5432.1342    100.03A 9.0D 0.0200125B5
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"49");
		strcat(sMsg,"9999999999");
		strcat(sMsg,"9");
		strcat(sMsg,"9");
		strcat(sMsg,"9999999.9");
		strcat(sMsg,"99");
		strcat(sMsg,"9");
		strcat(sMsg,"9999999.9");
		strcat(sMsg,"99");
		strcat(sMsg,"99.9");
		strcat(sMsg,"9");
		strcat(sMsg,"99.9999");
		strcat(sMsg,"999");
		strcat(sMsg,"63");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 4 configuration record
	//4A166109617412    100.03A3    105.61175.06 0.0200 809B
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"4a");
		strcat(sMsg,"aaaaaaaaaa");
		strcat(sMsg,"a");
		strcat(sMsg,"a");
		strcat(sMsg,"aaaaaaa.a");
		strcat(sMsg,"aa");
		strcat(sMsg,"a");
		strcat(sMsg,"aaaaaaa.a");
		strcat(sMsg,"aa");
		strcat(sMsg,"aa.a");
		strcat(sMsg,"a");
		strcat(sMsg,"aa.aaaa");
		strcat(sMsg,"aaa");
		strcat(sMsg,"bb");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
	//test chan 5 configuration record
	sMsg = (char*)malloc(57);
	nBytesRead = 56;
	if (sMsg)
	{
		strcpy(sMsg,"4b");
		strcat(sMsg,"bbbbbbbbbb");
		strcat(sMsg,"b");
		strcat(sMsg,"b");
		strcat(sMsg,"bbbbbbb.b");
		strcat(sMsg,"bb");
		strcat(sMsg,"b");
		strcat(sMsg,"bbbbbbb.b");
		strcat(sMsg,"bb");
		strcat(sMsg,"bb.b");
		strcat(sMsg,"b");
		strcat(sMsg,"bb.bbbb");
		strcat(sMsg,"bbb");
		strcat(sMsg,"ea");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}

	//test battery record
	sMsg = (char*)malloc(21);
	nBytesRead = 20;
	if (sMsg)
	{
		strcpy(sMsg,"18");
		strcat(sMsg,"1234567890");
		strcat(sMsg,"98.7");
		strcat(sMsg,"4c");
		strcat(sMsg,"\r\n");
		PostMessage(IDC_RECEIVE,nBytesRead,(LPARAM)sMsg);
	}
#endif
*/
	FILE* outfile;
	int iAt;
	CString cCaption(m_pParent->m_pName);
	while ((iAt = cCaption.FindOneOf(INVALIDCHARS)) >= 0) 
		cCaption.SetAt(iAt,'_');
	
	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",cCaption,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);
	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");

		m_pParent->IssuePrint(outfile);

		fclose(outfile);
	}
}

void CSafePropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	m_dToClose = 0;
	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
		CModParam* modparpage = m_pParent->m_pModParams;
		if (m_pParent->IsPaused())
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(true);
		else
			modparpage->GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CSafePropertySheet::OnExtendClose(WPARAM,LPARAM)
{
	m_dToClose = 0;
}

void CSafePropertySheet::OnToCloseButton()
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND) //120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}

void CSafePropertySheet::DisplayTodayData()
{
	m_bUpdatingFromFile = true;
	if (m_pDataWatchDlg)
	{
		if (m_bDataWatchDlgOpen)
		{
			return;
		}
		else
		{
			m_pDataWatchDlg->EndDialog(0);
			delete m_pDataWatchDlg;
			m_pDataWatchDlg = NULL;
		}
	}
	
	m_pDataWatchDlg = new CDataWatchDlg();
	m_pDataWatchDlg->SetDlgTitle("GRAND Data", this);
	m_pDataWatchDlg->SetGraphTitles("Neutron A", "Neutron B", 
		"Neutron C", "Gamma 1", "Gamma 2");
	m_pDataWatchDlg->Create(IDD_DATAWATCH_DIALOG,NULL);
	m_bDataWatchDlgOpen = true;

	//build today file name
	//open file and read header
		//read and add data to end of file
		//close file

	COleDateTime cNow = COleDateTime::GetCurrentTime();
	COleDateTime cBase(1952,1,1,0,0,0);
	COleDateTimeSpan cSpan = cNow - cBase;
	CJulianTime Time(cSpan.GetTotalSeconds());

	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentBIDFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
	////			'0' + (Time->Year() % 10),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,BID_SUFFIX);
	}
	else
	{
		CString cResult;
		m_pParent->BuildFileName(cResult,m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"MG",
			m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,BID_SUFFIX);
		strcpy(m_szCurrentBIDFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (_access(m_szCurrentBIDFileName,0) != 0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentBIDFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time.Year()-1990)<10)?
					((Time.Year()-1990<0)?'#':'0'+(Time.Year()-1990)):
					'A'+(Time.Year()-2000),
				((Time.Month()<10)?
					'0'+(Time.Month()):
					'A'+(Time.Month()-10)),
				((Time.Day()  <10)?
					'0'+(Time.Day()):
					'A'+(Time.Day()  -10)),
				'A',0,BID_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			m_pParent->BuildFileName(ArchiveName,m_pParent->m_csLongNameFormatString,cTemp,"MG",
				m_pParent->m_pID,Time.Year(),Time.Month(),Time.Day(),0,0,0,BID_SUFFIX);
		}
		//if the file is in the archive directory
		if (_access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentBIDFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	CFile cFile;
	CFileException ex;
	if (cFile.Open(m_szCurrentBIDFileName,CFile::modeRead|CFile::shareDenyNone,&ex))
	{
		DWORD dwLength = (DWORD)cFile.GetLength();// added type cast gjk
		bool bFail = false;
		if (dwLength > (sizeof(AcquireHeader) + 3600*sizeof(AcquireRecord)))
			cFile.Seek(sizeof(AcquireHeader) + 3600*sizeof(AcquireRecord), CFile::begin);
		else if (dwLength > sizeof(AcquireHeader))
			cFile.Seek(sizeof(AcquireHeader), CFile::begin);
		else 
			bFail = true;

		if (!bFail)
		{
			//while not eof read and display data
			AcquireRecord sAcquireRecord;

			UINT nBytesRead;
			while ((nBytesRead = cFile.Read(&sAcquireRecord,sizeof(AcquireRecord))) > 0)
			{
				m_pDataWatchDlg->SetData(sAcquireRecord.JulianSeconds,
					sAcquireRecord.NeutronChA,
					sAcquireRecord.NeutronChB,
					sAcquireRecord.NeutronChC,
					sAcquireRecord.GammaCh1,
					sAcquireRecord.GammaCh2, cFile.GetPosition()==dwLength?true:false);
			}
		}
		cFile.Close();
	}
	m_bUpdatingFromFile = false;
}

void CSafePropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			m_dToClose = 0;
			PostMessage(WM_SYSCOMMAND,SC_CLOSE,NULL);
			KillTimer(1);
		} 
		else
		{
			char temp[32];
			int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
			int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

HBRUSH CSafePropertySheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertySheet::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CSafePropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
	{
		DestroyWindow();
		delete (this);
	}
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CSafePropertySheet::OnInitDialog() 
{
	EnableStackedTabs(FALSE);
	BOOL bResult = CPropertySheet::OnInitDialog();

	CRect WindowRect;
	CRect FinalRect;
	GetWindowRect(&WindowRect);
	WindowRect.bottom += 60;
	MoveWindow(&WindowRect);
	int Interspace = 6;
	int ButtonWidth = (WindowRect.right - WindowRect.left)/5 - 6*Interspace;
	if (ButtonWidth < 100) 
		ButtonWidth = 116;
	else
		ButtonWidth = 157;

	FinalRect = WindowRect;
	ScreenToClient(FinalRect);
	int Y1 = FinalRect.bottom - 60;
	int Y2 = FinalRect.bottom - 10;

	m_pCloseButton->Create( "Close",  
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER ,
		  CRect(Interspace,  Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	m_pPauseButton->Create(COLLECTINGBEGIN,
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags",
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status To File",
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

	char temp[32];

	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->Create(temp,
		  WS_CHILD | WS_TABSTOP | WS_VISIBLE | 
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER,
		  CRect(5*Interspace + 4*ButtonWidth,Y1,
				5*Interspace + 5*ButtonWidth,Y2),
		this,
		ID_TOCLOSE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND);
	}
	
	GetTabControl( )->SetImageList(m_pImageList);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(2,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(3,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(4,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(5,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(6,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(7,&pTabCtrlItem);

	SetTimer(1,1000,NULL);

	return bResult;
}

BOOL CSafePropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CSafePropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CSafePropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(IDC_RECEIVE_RESULT,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CSafePropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}
