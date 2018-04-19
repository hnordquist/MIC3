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
// ModParam.cpp

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "Instrument.h"
#include "resource.h"
#include "colors.h"
#include "ModParam.h"
#include "DirDialog.h"
#include "GrandSetup.h"
#include "GrandSetupData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GRAND_DEFAULT		"GRAND Default"
#define TIME_ERR_SPIN_MIN	1
#define TIME_ERR_SPIN_MAX	10000
#define TIME_ERR_SPIN_DEF	60

#define SLOW_RESET_SPIN_MIN	0
#define SLOW_RESET_SPIN_MAX	10000
#define SLOW_RESET_SPIN_DEF	900

#define BATVOLTH_SPIN_DEF	"50.0"
#define BATVOLTH_SPIN_MAX	500
#define BATVOLTH_SPIN_MIN	0
#define BATVOLTL_SPIN_MAX	500
#define BATVOLTL_SPIN_MIN	0
#define BATVOLTL_SPIN_DEF	"0.0"

#define SUP_P15H_SPIN_DEF	"24.0"
#define SUP_P15H_SPIN_MAX	240
#define SUP_P15H_SPIN_MIN	0
#define SUP_P15L_SPIN_MAX	240
#define SUP_P15L_SPIN_MIN	0
#define SUP_P15L_SPIN_DEF	"0.0"

#define FCHVERR_DEF			5
#define FCHVERR_MAX			100
#define FCHVERR_MIN			0

#define ICHVERR_DEF			10
#define ICHVERR_MAX			100
#define ICHVERR_MIN			0


#define SUP_M15H_SPIN_DEF	"0.0"
#define SUP_M15H_SPIN_MAX	0
#define SUP_M15H_SPIN_MIN	-240
#define SUP_M15L_SPIN_MAX	0
#define SUP_M15L_SPIN_MIN	-240
#define SUP_M15L_SPIN_DEF	"-24.0"

#define SUP_P05H_SPIN_DEF   "9.9"
#define SUP_P05H_SPIN_MAX	99
#define SUP_P05H_SPIN_MIN	0
#define SUP_P05L_SPIN_MAX	99
#define SUP_P05L_SPIN_MIN	0
#define SUP_P05L_SPIN_DEF   "0.0"

#define NODE_SPIN_MAX		32767
#define NODE_SPIN_MIN		-1
#define NODE_SPIN_DEF		-1

#define MAXCYCLE_SPIN_MIN	500
#define MAXCYCLE_SPIN_MAX	32767
#define MAXCYCLE_SPIN_DEF	1000

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define MAXINQUIRE_SPIN_MIN 2
#define MAXINQUIRE_SPIN_MAX 32767
#define MAXINQUIRE_SPIN_DEF 10

#define MAXSTATUS_SPIN_MIN	0
#define MAXSTATUS_SPIN_MAX	32767
#define MAXSTATUS_SPIN_DEF	30

#define COMMFAIL_SPIN_MIN	1
#define COMMFAIL_SPIN_MAX	99
#define COMMFAIL_SPIN_DEF	5

#define MAXBBM_SPIN_MIN		0
#define MAXBBM_SPIN_MAX		500000
#define MAXBBM_SPIN_DEF		"1000"

#define TIMEOUT_SPIN_MIN	2	
#define	TIMEOUT_SPIN_MAX	99
#define TIMEOUT_SPIN_DEF	5

#define DODUMP_DEF			1
#define LOGMII_DEF			1
#define LOGFILTER_DEF		1
#define AUTOTIME_DEF		1
#define BINARY_BBM_DEF		0
#define LBREXTEND_DEF		0

#define AUTOTIMEDAILY_DEF	0
#define AUTOTIMEDAILYAT_DEF	"4:30:30"

#define FILEID_SPIN_MAX	99
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\GRAND01"
#define PORT_DEF			"ERROR"
/////////////////////////////////////////////////////////////////////////////
// CModParam property page

IMPLEMENT_DYNCREATE(CModParam, CPropertyPage)

CModParam::CModParam() : CPropertyPage(CModParam::IDD)
{
}


CModParam::CModParam(UINT id, CISO* pParent, char *inifile, bool DoingConfig) 
: CPropertyPage(id)
{
	m_bAllowFinish = false;
	m_pParent = pParent;
	m_bChange = false;
	m_pName[0] = NULL;
	m_pGrandSetup = NULL;
	m_pTitle[0] = NULL;
	m_datasetup = false;
	m_bCollectingParametersForNewISO = DoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));

	for (int i= IDC_MODPARAMS_FIRST; i <= IDC_MODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_MODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			GRAND_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();

	}
	//<<QA>> should this happen when collecting parameters for new ISO??
	//<<QA>> I suspect not.
	m_pGrandSetup = new CGrandSetup(this, m_szIniFile);
}

CModParam::~CModParam()
{
	delete m_pApplyButton;
	delete m_pGrandSetup;
}


BEGIN_MESSAGE_MAP(CModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CModParam)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_MODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_MODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_BATVOLTL_SPIN, OnDeltaposModparamsBatvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_BATVOLTH_SPIN, OnDeltaposModparamsBatvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_M15L_SPIN, OnDeltaposModparamsSupM15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_M15H_SPIN, OnDeltaposModparamsSupM15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P05H_SPIN, OnDeltaposModparamsSupP05hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P05L_SPIN, OnDeltaposModparamsSupP05lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P15H_SPIN, OnDeltaposModparamsSupP15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SUP_P15L_SPIN, OnDeltaposModparamsSupP15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXBBM_SPIN, OnDeltaposModparamsMaxbbmSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_FCHVERR_SPIN,  OnDeltaposModparamsFCHVErrSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_ICHVERR_SPIN,  OnDeltaposModparamsICHVErrSpin)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXINQUIRE, OnChangeModparamsMaxInq)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXCYCLE, OnChangeModparamsMaxCycle)
	ON_BN_CLICKED(IDC_MODPARAMS_LOGMII, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_FILEID, OnChangeModparamsFileID)
	ON_CBN_SELCHANGE(IDC_MODPARAMS_PORT, OnChangeModparamsPort)
	ON_EN_CHANGE(IDC_MODPARAMS_SAVELOC, OnChangeModparamsSaveloc)
	ON_EN_KILLFOCUS(IDC_MODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_BN_CLICKED(IDC_MODPARAMS_DATETIME, OnModparamsDatetime)
	ON_BN_CLICKED(IDC_CHECK_ALLOWAUTODATE, OnCheckAllowautodate)
	ON_BN_CLICKED(IDC_CHECK_BINARY, OnCheckBinary)
	ON_BN_CLICKED(IDC_CHECK_LBREXTEND, OnCheckLbrextend)
	ON_BN_CLICKED(IDC_BUTTON_SETUP, OnButtonSetup)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXSTATUS_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXINQUIRE_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_MODPARAMS_LOGFILTER, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_MODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_COMMFAIL_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_MODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_MODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_TIMEOUT_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_TIMEOUT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_TIME_ERR_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MODPARAMS_SLOW_RESET_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_MODPARAMS_TIME_ERR, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SLOW_RESET, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P05L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_P05H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_M15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_SUP_M15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_NODE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXSTATUS, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_MAXBBM, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_COMMFAIL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_BATVOLTL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_BATVOLTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_ICHVERR, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_MODPARAMS_FCHVERR, OnChangeModparamsMost)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, OnDatetimechangeDatetimepicker1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CModParam::HideSetParam()
{
	if (m_pGrandSetup)
		m_pGrandSetup->ShowWindow(SW_HIDE);
}

void CModParam::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_MODPARAMS_FIRST) &&
		(index <= IDC_MODPARAMS_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

void CModParam::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_MODPARAMS_FIRST) &&
		(index <= IDC_MODPARAMS_LAST)) {
	  StaticColors[index-IDC_MODPARAMS_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModParam)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_cDateTimePicker);
	//}}AFX_DATA_MAP
}



/////////////////////////////////////////////////////////////////////////////
// CModParam message handlers

HBRUSH CModParam::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_szIniFile);
}

void CModParam::DoApply()
{
	char Name[_MAX_PATH];
	char Temp[_MAX_PATH];

	//get all the stuff and save it

	strncpy(Name, m_pParent->GetName(),sizeof(Name)-1);

	SaveItem(Name,"BATVOLTH",	IDC_MODPARAMS_BATVOLTH);
	SaveItem(Name,"BATVOLTL",	IDC_MODPARAMS_BATVOLTL);
	SaveItem(Name,"SUP_P15H",	IDC_MODPARAMS_SUP_P15H);
	SaveItem(Name,"SUP_P15L",	IDC_MODPARAMS_SUP_P15L);
	SaveItem(Name,"SUP_M15H",	IDC_MODPARAMS_SUP_M15H);
	SaveItem(Name,"SUP_M15L",	IDC_MODPARAMS_SUP_M15L);	
	SaveItem(Name,"SUP_P05H",	IDC_MODPARAMS_SUP_P05H);
	SaveItem(Name,"SUP_P05L",	IDC_MODPARAMS_SUP_P05L);
	SaveItem(Name,"TIME_ERR",	IDC_MODPARAMS_TIME_ERR);
	SaveItem(Name,"SLOW_RESET",	IDC_MODPARAMS_SLOW_RESET);
	SaveItem(Name, NODE,		IDC_MODPARAMS_NODE);
	SaveItem(Name,"MAXCYCLE",	IDC_MODPARAMS_MAXCYCLE);
	SaveItem(Name,"MAXPAUSE",	IDC_MODPARAMS_MAXPAUSE);
	SaveItem(Name,"DO_DUMP",	IDC_MODPARAMS_DODUMP,	true);
	SaveItem(Name,"AUTOTIMESET",IDC_CHECK_ALLOWAUTODATE,true);
	SaveItem(Name,"LOG_FILTER",	IDC_MODPARAMS_LOGFILTER,true);
	SaveItem(Name,"LOG_MII",	IDC_MODPARAMS_LOGMII,	true);
	SaveItem(Name, PORT,		IDC_MODPARAMS_PORT);
	SaveItem(Name,"MAXINQUIRE",	IDC_MODPARAMS_MAXINQUIRE);
	SaveItem(Name,"MAXSTATUS",	IDC_MODPARAMS_MAXSTATUS);
	SaveItem(Name,"COMMFAIL",	IDC_MODPARAMS_COMMFAIL);
	SaveItem(Name,"TIMEOUT",	IDC_MODPARAMS_TIMEOUT);
	SaveItem(Name,"MAXBBM",		IDC_MODPARAMS_MAXBBM);
	SaveItem(Name, FILE_ID,		IDC_MODPARAMS_FILEID);
	SaveItem(Name,"BINARY_BBM",	IDC_CHECK_BINARY,true);
	SaveItem(Name,"LBREXTEND",	IDC_CHECK_LBREXTEND,true);
	SaveItem(Name,"ICHVERR",    IDC_MODPARAMS_ICHVERR);
	SaveItem(Name,"FCHVERR",    IDC_MODPARAMS_FCHVERR);
	
	m_cDateTimePicker.GetWindowText(Temp,sizeof(Temp));
	CTime timeTime;
	DWORD dwResult = m_cDateTimePicker.GetTime(timeTime);
	WritePrivateProfileString(Name,"AUTOTIMESETDAILY",(dwResult & 0x00000001)?"0":"1",m_szIniFile);
	WritePrivateProfileString(Name,"AUTOTIMESETDAILYAT",Temp,m_szIniFile);

	GetDlgItemText(IDC_MODPARAMS_SAVELOC,Temp,sizeof(Temp));
	while ((Temp[strlen(Temp)-1]=='\\')||
		   (Temp[strlen(Temp)-1]=='/' )  ) 
	{
		Temp[strlen(Temp)-1] = NULL;
		SetDlgItemText(IDC_MODPARAMS_SAVELOC,Temp);
	}
	SaveItem(Name,SAVE_LOC,IDC_MODPARAMS_SAVELOC);
	if (_access(Temp,0)==-1)
	{
		_mkdir(Temp);
		strcat(Temp,"\\archive");
		_mkdir(Temp);
	}

	//propagate all the new values to the program
	if (!m_bCollectingParametersForNewISO)
	{
		m_pParent->UpdateParams(true);	
	}
	else
	{
		m_pGrandSetup->SaveSetup();
	}

}

void CModParam::OnModparamsApply() 
{
	/*
	//<<QA>> does not apply for MIC GM?
		//for all the comm device must exist
	if (m_pPS) m_pPS->SetToClose(0);
	char temp[32];
	GetDlgItem(IDC_MODPARAMS_PORT)->GetWindowText(temp,sizeof(temp));
	GetPrivateProfileString(COMMUNICATIONS,temp,"ERROR",temp,sizeof(temp)-1,m_szIniFile);
	if (m_bCollectingParametersForNewISO)
	{
		if (strcmp(temp,"ERROR")==0)
		{
			CTimeMsgDlg MsgBox(this);
			MsgBox.DoModal(LPCTSTR (CString(g_cNAMELONG)+CString(": Apply Configuration")),
				"A \"Communications\" object must be selected for this instrument.");
			return;
		}
	}
	*/

	if (m_pParent->Validate()) 
	{
		DoApply();
		m_bChange = false;
		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(FALSE);
	}
}

//reset float item for a spinner with a non-automatic buddy
void CModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
{
	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_szIniFile);
	float ftemp = (float)atof(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos((int)(ftemp*10.0));
	sprintf(temp,"%1.1f",ftemp);
	GetDlgItem(dTextItem)->SetWindowText(temp);
}

//reset int item for a spinner with a non-automatic buddy
void CModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
						   int dItem,int /*dMin*/, int dMax, int dTextItem)
{
	int pAccel [] =
	{
		0,1,
		2,10,
		4,100,
		6,1000,
		8,10000,
	};

	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_szIniFile);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	_itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));
}

//reset item for a spinner with an automatic buddy
void CModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CModParam::ResetItem(char *szName, char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_szIniFile);
	switch(dItem)
	{
	case IDC_MODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:;
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}

void CModParam::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;

	if (pName)
	{
		strncpy(m_pTitle,pName,sizeof(m_pTitle));
		strncpy(m_pName,pName,sizeof(m_pName));
	}
}


void CModParam::ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem)
{
	char temp[3];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_szIniFile);
	if (strlen(temp)==1)
	{
		temp[1] = temp[0];
		temp[0] = '0';
		temp[2] = NULL;
	}
	SetDlgItemText(dItem,temp);
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetRange(FILEID_SPIN_MIN,FILEID_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetPos(value);
	((CEdit*)GetDlgItem(dItem))->SetLimitText(2);
}

void CModParam::OnModparamsReset() 
{
	m_datasetup = true;

	char szTemp[MAX_PATH];

	if (m_bCollectingParametersForNewISO)
	{
		if (strlen(m_pName) == 0)
			strncpy(m_pName,m_pParent->m_pName,sizeof(m_pName));

		strncpy(m_pName,m_pTitle,sizeof(m_pTitle));
		GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
	}
	else
	{
		if (strlen(m_pName) == 0)
			strcpy(m_pName,m_pTitle);

		GetDlgItem(IDC_MODPARAMS_PORT)->EnableWindow(false);
		GetDlgItem(IDC_MODPARAMS_NODE)->EnableWindow(false);
		GetDlgItem(IDC_MODPARAMS_NODE_SPIN)->EnableWindow(false);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
	}

	ResetItem(m_pName,FILE_ID, FILEID_SPIN_DEF,
		IDC_MODPARAMS_FILEID_SPIN,IDC_MODPARAMS_FILEID);

	ResetItem(m_pName, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_MODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTH);

	ResetItem(m_pName, "BATVOLTL", BATVOLTL_SPIN_DEF,
			IDC_MODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
			IDC_MODPARAMS_BATVOLTL);

	ResetItem(m_pName,"SUP_P15H",SUP_P15H_SPIN_DEF,
			IDC_MODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
			IDC_MODPARAMS_SUP_P15H);

	ResetItem(m_pName,"SUP_P15L",SUP_P15L_SPIN_DEF,
			IDC_MODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
			IDC_MODPARAMS_SUP_P15L);

	ResetItem(m_pName,"SUP_M15H",SUP_M15H_SPIN_DEF,
			IDC_MODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
			IDC_MODPARAMS_SUP_M15H);

	ResetItem(m_pName,"SUP_M15L",SUP_M15L_SPIN_DEF,
			IDC_MODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
			IDC_MODPARAMS_SUP_M15L);	

	ResetItem(m_pName,"SUP_P05H",SUP_P05H_SPIN_DEF,
			IDC_MODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
			IDC_MODPARAMS_SUP_P05H);

	ResetItem(m_pName,"SUP_P05L",SUP_P05L_SPIN_DEF,
			IDC_MODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
			IDC_MODPARAMS_SUP_P05L);

	ResetItem(m_pName,"TIME_ERR",TIME_ERR_SPIN_DEF,
			IDC_MODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(m_pName,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
			IDC_MODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(m_pName,NODE,NODE_SPIN_DEF,
			IDC_MODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
		
	ResetItem(m_pName,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
			IDC_MODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(m_pName,"DO_DUMP", DODUMP_DEF, IDC_MODPARAMS_DODUMP);

	ResetItem(m_pName,"LOG_FILTER", LOGFILTER_DEF, IDC_MODPARAMS_LOGFILTER);

	ResetItem(m_pName,"LOG_MII", LOGMII_DEF, IDC_MODPARAMS_LOGMII);

	ResetItem(m_pName,"AUTOTIMESET", AUTOTIME_DEF, IDC_CHECK_ALLOWAUTODATE);

	GetPrivateProfileString(m_pName,"AUTOTIMESETDAILYAT","04:30:30 AM",szTemp,sizeof(szTemp),m_szIniFile);
	int iHour,iMinute,iSecond;
	char cAMPM;
	sscanf(szTemp,"%d:%d:%d %c",&iHour,&iMinute,&iSecond,&cAMPM);
	if (cAMPM == 'P') iHour += 12;
	CTime cTime(2000,1,1,iHour,iMinute,iSecond);
	m_cDateTimePicker.SetTime(&cTime);
	bool bDoAuto = GetPrivateProfileInt(m_pName,"AUTOTIMESETDAILY",0,m_szIniFile)==1;
	if (!bDoAuto)
		m_cDateTimePicker.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE,0);

//Displays a checkbox in the left-hand side of the control which, 
//when cleared, provides a way to have no date currently selected 
//in the control. You can also set this state with DTM_SETSYSTEMTIME wParam = GDT_NONE. 
//Verify the state with DTM_GETSYSTEMTIME. Type: Bool. Default: False.
 
	ResetItem(m_pName,SAVE_LOC, SAVELOC_DEF, IDC_MODPARAMS_SAVELOC);

	ResetItem(m_pName,"BINARY_BBM", BINARY_BBM_DEF,IDC_CHECK_BINARY);

	ResetItem(m_pName,"LBREXTEND", LBREXTEND_DEF,IDC_CHECK_LBREXTEND);

	//<<QA>> what about the new controls? FCHVERR, etc.

	char buff[16384];
	char buff2[64];

	if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff), m_szIniFile)>0)
		{
			char *at = buff;
			int sel = ((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->GetCurSel();
			((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
			((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->SetCurSel(sel);
		}

	ResetItem(m_pName,"PORT", PORT_DEF, IDC_MODPARAMS_PORT);

	ResetItem(m_pName,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
			IDC_MODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(m_pName,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
			IDC_MODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(m_pName,"COMMFAIL",COMMFAIL_SPIN_DEF,
			IDC_MODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(m_pName,"TIMEOUT",TIMEOUT_SPIN_DEF,
			IDC_MODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(m_pName,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
			IDC_MODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,IDC_MODPARAMS_MAXPAUSE);

	ResetItemI(m_pName,"MAXBBM",MAXBBM_SPIN_DEF,
			IDC_MODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,IDC_MODPARAMS_MAXBBM);
			
	ResetItem(m_pName,"ICHVERR",ICHVERR_DEF,
			IDC_MODPARAMS_ICHVERR_SPIN,ICHVERR_MIN,ICHVERR_MAX);

	ResetItem(m_pName,"FCHVERR",FCHVERR_DEF,
			IDC_MODPARAMS_FCHVERR_SPIN,FCHVERR_MIN,FCHVERR_MAX);

	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	char temp[16];								
	GetDlgItem(DLGITEM)->GetWindowText(temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
	}
	
	sprintf(temp,"%d",iNew);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
								int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	char temp[16];								
	GetDlgItemText(DLGITEM,temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
	}
	
	sprintf(temp,"%d",iNew);
	SetDlgItemText(DLGITEM,temp);

	char temp2[16];
	int buddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
//try{
	sscanf(temp2,"%d",&buddyvalue);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 30",MB_OK);
//	AfxMessageBox(temp2,MB_OK);
//}
	if (Above)
	{
		if (iNew > buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (iNew < buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}
void CModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];								
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];
	char temp2[8];
	float fvalue = (float)iNew/(float)10.0;
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);

	float fbuddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
//try{
	sscanf(temp2,"%f",&fbuddyvalue);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 31",MB_OK);
//	AfxMessageBox(temp2,MB_OK);
//}

	if (Above)
	{
		if (fvalue > fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (fvalue < fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}

void CModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (iNew > MAX)
	{
		iNew = MAX;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MAX;
	}
	if (iNew < MIN)
	{
		iNew = MIN;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MIN;
	}
	char temp[3];
	_itoa(iNew,temp,10);
	while (strlen(temp) < 2)
	{
		temp[2] = NULL;
		temp[1] = temp[0];
		temp[0] = '0';
	}

	SetDlgItemText(DLGITEM,temp);

	CString Text;
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);

//	if "GRAND" at end of name
	Text.MakeReverse();
	switch (Text.Find("DNARG"))
	{
	case 0:
		Text.MakeReverse();
		break;
	case 1:	
		if (
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-1);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "GRAND";
			else
				Text += "\\GRAND";
		}
		break;
	case 2:
		if (
			(
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
			) && (
			((Text[1] >= '0') && (Text[1] <= '9')) ||
			((Text[1] >= 'A') && (Text[1] <= 'Z')) ||
			((Text[1] >= 'a') && (Text[1] <= 'z')) 
			)
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-2);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "GRAND";
			else
				Text += "\\GRAND";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "GRAND";
			else
				Text += "\\GRAND";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);
}

void CModParam::OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX, 
		IDC_MODPARAMS_BATVOLTH, IDC_MODPARAMS_BATVOLTL,IDC_MODPARAMS_BATVOLTL_SPIN, false);
}

void CModParam::OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX, 
		IDC_MODPARAMS_BATVOLTL, IDC_MODPARAMS_BATVOLTH,IDC_MODPARAMS_BATVOLTH_SPIN, true);
}

void CModParam::OnDeltaposModparamsSupM15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15H_SPIN_MIN, SUP_M15H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_M15H, IDC_MODPARAMS_SUP_M15L, IDC_MODPARAMS_SUP_M15L_SPIN, false);
}

void CModParam::OnDeltaposModparamsSupM15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15L_SPIN_MIN, SUP_M15L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_M15L, IDC_MODPARAMS_SUP_M15H, IDC_MODPARAMS_SUP_M15H_SPIN,true);
}

void CModParam::OnDeltaposModparamsSupP05hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05H_SPIN_MIN, SUP_P05H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P05H, IDC_MODPARAMS_SUP_P05L, IDC_MODPARAMS_SUP_P05L_SPIN, false);
}

void CModParam::OnDeltaposModparamsSupP05lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05L_SPIN_MIN, SUP_P05L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P05L, IDC_MODPARAMS_SUP_P05H, IDC_MODPARAMS_SUP_P05H_SPIN, true );
}

void CModParam::OnDeltaposModparamsSupP15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15H_SPIN_MIN, SUP_P15H_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P15H, IDC_MODPARAMS_SUP_P15L, IDC_MODPARAMS_SUP_P15L_SPIN, false);
}

void CModParam::OnDeltaposModparamsSupP15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15L_SPIN_MIN, SUP_P15L_SPIN_MAX, 
		IDC_MODPARAMS_SUP_P15L, IDC_MODPARAMS_SUP_P15H, IDC_MODPARAMS_SUP_P15H_SPIN, true);
}

void CModParam::OnDeltaposModparamsMaxbbmSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXBBM_SPIN_MIN, MAXBBM_SPIN_MAX, IDC_MODPARAMS_MAXBBM);
}

void CModParam::OnDeltaposModparamsFCHVErrSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult,FCHVERR_MIN,FCHVERR_MAX,IDC_MODPARAMS_FCHVERR);
}

void CModParam::OnDeltaposModparamsICHVErrSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult,ICHVERR_MIN,ICHVERR_MAX,IDC_MODPARAMS_ICHVERR);
}

void CModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_MODPARAMS_MAXPAUSE);
}

void CModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_MODPARAMS_FILEID);
}

void CModParam::OnModparamsBrowse() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_MODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("GRAND"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "GRAND";
			else
				Text += "\\GRAND";
			Text += Id;
		}
		SetDlgItemText(IDC_MODPARAMS_SAVELOC, Text);
	}
}

void CModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnModparamsDefault(bool SetPort)
{
	if (m_pPS) m_pPS->SetToClose(0);
	m_datasetup = true;

	ResetItem(GRAND_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_MODPARAMS_FILEID_SPIN, IDC_MODPARAMS_FILEID);

	ResetItem(GRAND_DEFAULT, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_MODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTH);

	ResetItem(GRAND_DEFAULT, "BATVOLTL", BATVOLTL_SPIN_DEF,
		IDC_MODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
		IDC_MODPARAMS_BATVOLTL);

	ResetItem(GRAND_DEFAULT,"SUP_P15H",SUP_P15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15H);

	ResetItem(GRAND_DEFAULT,"SUP_P15L",SUP_P15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P15L);

	ResetItem(GRAND_DEFAULT,"SUP_M15H",SUP_M15H_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15H);

	ResetItem(GRAND_DEFAULT,"SUP_M15L",SUP_M15L_SPIN_DEF,
		IDC_MODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
		IDC_MODPARAMS_SUP_M15L);	

	ResetItem(GRAND_DEFAULT,"SUP_P05H",SUP_P05H_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05H);

	ResetItem(GRAND_DEFAULT,"SUP_P05L",SUP_P05L_SPIN_DEF,
		IDC_MODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
		IDC_MODPARAMS_SUP_P05L);

	ResetItem(GRAND_DEFAULT,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_MODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(GRAND_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_MODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);
	
	ResetItem(GRAND_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_MODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(GRAND_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_MODPARAMS_DODUMP);

	ResetItem(GRAND_DEFAULT,"BINARY_BBM",BINARY_BBM_DEF,IDC_CHECK_BINARY);

	ResetItem(GRAND_DEFAULT,"LBREXTEND", LBREXTEND_DEF,IDC_CHECK_LBREXTEND);

	ResetItem(GRAND_DEFAULT,"LOG_FILTER",LOGFILTER_DEF,IDC_MODPARAMS_LOGFILTER);

	ResetItem(GRAND_DEFAULT,"LOG_MII",LOGMII_DEF,IDC_MODPARAMS_LOGMII);

	ResetItem(GRAND_DEFAULT,"AUTOTIMESET",AUTOTIME_DEF,IDC_CHECK_ALLOWAUTODATE);

	char szTemp[16];
	GetPrivateProfileString(GRAND_DEFAULT,"AUTOTIMESETDAILYAT","04:30:30 AM",szTemp,sizeof(szTemp),m_szIniFile);
	int iHour,iMinute,iSecond;
	char cAMPM;
	sscanf(szTemp,"%d:%d:%d %c",&iHour,&iMinute,&iSecond,&cAMPM);
	if (cAMPM == 'P') iHour += 12;
	CTime cTime(2000,1,1,iHour,iMinute,iSecond);
	m_cDateTimePicker.SetTime(&cTime);
	bool bDoAuto = GetPrivateProfileInt(GRAND_DEFAULT,"AUTOTIMESETDAILY",0,m_szIniFile)==1;
	if (!bDoAuto)
		m_cDateTimePicker.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE,0);

//Displays a checkbox in the left-hand side of the control which, 
//when cleared, provides a way to have no date currently selected 
//in the control. You can also set this state with DTM_SETSYSTEMTIME wParam = GDT_NONE. 
//Verify the state with DTM_GETSYSTEMTIME. Type: Bool. Default: False.
 
	ResetItem(GRAND_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_MODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(GRAND_DEFAULT,NODE,NODE_SPIN_DEF,
			IDC_MODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);

		char buff[16384];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_MODPARAMS_PORT))->SetCurSel(0);

		ResetItem(GRAND_DEFAULT,PORT,PORT_DEF,IDC_MODPARAMS_PORT);
	}

	ResetItem(GRAND_DEFAULT,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_MODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(GRAND_DEFAULT,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_MODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(GRAND_DEFAULT,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_MODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(GRAND_DEFAULT,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_MODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(GRAND_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_MODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,IDC_MODPARAMS_MAXPAUSE);

	ResetItemI(GRAND_DEFAULT,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_MODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,IDC_MODPARAMS_MAXBBM);

	ResetItem(GRAND_DEFAULT,"ICHVERR",ICHVERR_DEF,
		IDC_MODPARAMS_ICHVERR_SPIN,ICHVERR_MIN,ICHVERR_MAX);

	ResetItem(GRAND_DEFAULT,"FCHVERR",FCHVERR_DEF,
		IDC_MODPARAMS_FCHVERR_SPIN,FCHVERR_MIN,FCHVERR_MAX);
}

BOOL CModParam::OnSetActive() 
{
	if (m_bCollectingParametersForNewISO) 
	{
		SetDlgItemText(IDC_MODPARAMS_STATICTEXT,"");
//		GetDlgItem(IDC_MODPARAMS_APPLY)->ShowWindow(SW_HIDE);
		if (m_pApplyButton)
			m_pApplyButton->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_MODPARAMS_RESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_MODPARAMS_RESET)->SetWindowText("Save As Default");
		GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(false);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
//		m_pPS->SetWizardButtons(PSWIZB_FINISH);
        if (!m_datasetup)
		{
			OnModparamsDefault();
			if (m_pPS && m_pTitle)
			{
				CString sCaption(m_pTitle);
				TC_ITEM tcItem;
				tcItem.mask = TCIF_TEXT;
				tcItem.pszText = (LPTSTR)((LPCTSTR)sCaption);
				m_pPS->GetTabControl()->SetItem(0, &tcItem );
			}
		}
		if (m_bAllowFinish)
			m_pPS->SetWizardButtons(PSWIZB_FINISH );
		else
			m_pPS->SetWizardButtons(PSWIZB_DISABLEDFINISH );

	}
	else
	{
		SetDlgItemText(IDC_MODPARAMS_STATICTEXT,
			"Changes will NOT take effect until you click on \"Apply\".");
//		GetDlgItem(IDC_MODPARAMS_DATETIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_MODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange);
	}

	return CPropertyPage::OnSetActive();
}

void CModParam::AddSetParam(char*szDesc,char*szIniName,char*szSetting,char*szCommand,unsigned int uLength)
{
	if (m_pGrandSetup)
		m_pGrandSetup->AddSetParam(szDesc,szIniName,szSetting,szCommand,uLength);
}

void CModParam::CurrentSetParam(char*szIniName,char*szCurrent,bool bModParam,bool bTextOnly)
{
	if (m_pGrandSetup)
		m_pGrandSetup->CurrentSetParam(szIniName,szCurrent,bModParam,bTextOnly);
}

void CModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS) m_pPS->SetToClose(0);
//	CWnd* pWnd;
//	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
//	{
//		UINT index = pWnd->GetDlgCtrlID();
//		if ((index >= IDC_MODPARAMS_FIRST) && (index <= IDC_MODPARAMS_LAST))
//		{
//			MessageBox("Got One",MB_OK);
//		}
//	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CModParam::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if (m_bShowingGrandSetupDialog && m_pGrandSetup)
		m_pGrandSetup->ShowWindow(bShow);
}

void CModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CModParam::OnChangeModparamsMaxInq()
{
	int value =	GetDlgItemInt(IDC_MODPARAMS_MAXINQUIRE);
	if (value < MAXINQUIRE_SPIN_MIN)
		SetDlgItemInt(IDC_MODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MIN);
	if (value > MAXINQUIRE_SPIN_MAX)
		SetDlgItemInt(IDC_MODPARAMS_MAXINQUIRE, MAXINQUIRE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnChangeModparamsMaxCycle()
{
	int value =	GetDlgItemInt(IDC_MODPARAMS_MAXCYCLE);
	if (value < MAXCYCLE_SPIN_MIN)
		SetDlgItemInt(IDC_MODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MIN);
	if (value > MAXCYCLE_SPIN_MAX)
		SetDlgItemInt(IDC_MODPARAMS_MAXCYCLE, MAXCYCLE_SPIN_MAX);

	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnChangeModparamsMost()
{
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnChangeModparamsPort()
{
//	if (m_bCollectingParametersForNewISO)
//	{
//		SetDlgItemText(IDC_MODPARAMS_STATICTEXT,"");
//		m_pPS->SetWizardButtons(PSWIZB_NEXT);
//		m_bNextOk = true;
//	}

	if(m_pPS)m_pPS->SetToClose(0);
}

void CModParam::OnChangeModparamsFileID()
{
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnChangeModparamsSaveloc() 
{
	CString Text;
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_MODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_MODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_MODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_MODPARAMS_FILEID))->SetWindowText(temp);
}

BOOL CModParam::OnWizardFinish() 
{
//	ASSERT(m_bCollectingParametersForNewISO);

	DoApply();
	return CPropertyPage::OnWizardFinish();
}

void CModParam::OnModparamsDatetime() 
{
	if (m_pPS)
	{
		m_pPS->SetToClose(0);
		m_pPS->ForceDateTime();
	}		
}

void CModParam::OnCheckAllowautodate() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	
}

void CModParam::OnCheckBinary()
{
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnCheckLbrextend() 
{
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CModParam::OnButtonSetup() 
{
	if (m_bShowingGrandSetupDialog)
	{
		if (m_pGrandSetup)
			m_pGrandSetup->ShowWindow(SW_HIDE);

		m_bShowingGrandSetupDialog = false;
	}
	else
	{
		if (m_pGrandSetup)
			m_pGrandSetup->ShowWindow(SW_SHOW);

		m_bShowingGrandSetupDialog = true;
	}
}

BOOL CModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	if (m_pGrandSetup)
	{
		m_pGrandSetup->Create(IDD_GRANDSETUP_DIALOG,this);
		m_pGrandSetup->ShowWindow(SW_HIDE);
	}
	m_bShowingGrandSetupDialog = false;

	//build the colored apply button
	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_MODPARAMS_APPLY,
		m_szIniFile,
		this);

	//want different behavour if we are creating a new iso
	//<<QA>> I think this should not happen?
	if (m_bCollectingParametersForNewISO)
	{
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
			szTemp[0] = NULL;
			GetPrivateProfileString(GRAND_DEFAULT,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
			if (strlen(szTemp)==0)
				strcpy(szTemp,pszDefault);
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(szTemp));
		}
		while ((pszDescription = cGrandSetupData.GetNewNext())!=NULL)
		{
			pszIniName = cGrandSetupData.GetNewNext();
			pszDefault = cGrandSetupData.GetNewNext();
			pszCommand = cGrandSetupData.GetNewNext();
			szTemp[0] = NULL;
			GetPrivateProfileString(GRAND_DEFAULT,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
			if (strlen(szTemp)==0)
				strcpy(szTemp,pszDefault);
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(szTemp));
		}
	}
//	else
//	{
		GetDlgItem(IDC_BUTTON_SETUP)->ShowWindow(SW_HIDE);
//	}

	if (m_pGrandSetup)
	{
		m_pGrandSetup->SetWindowText(m_pName);
		strcpy(m_pGrandSetup->m_szName,m_pName);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/*
void CModParam::IssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	GetDlgItemText(IDC_MODPARAMS_PORT,temp);
	GetDlgItemText(IDC_MODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp,temp1);
	fprintf(outfile," File Output\n");
	GetDlgItemText(IDC_MODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp);
	GetDlgItemText(IDC_MODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station ID: %s\n",temp);
	if(((CButton*)GetDlgItem(IDC_MODPARAMS_LOGFILTER))->GetCheck()==0)
		temp = NO;
	else
		temp = YES;
	fprintf(outfile,"  Log Data Filtering Msgs: %s\n",temp);
	if(((CButton*)GetDlgItem(IDC_MODPARAMS_DODUMP))->GetCheck()==0)
		temp = NO;
	else
		temp = YES;
	fprintf(outfile,"  Do Dump File: %s\n",temp);
	if(((CButton*)GetDlgItem(IDC_MODPARAMS_LOGMII))->GetCheck()==0)
		temp = NO;
	else
		temp = YES;
	fprintf(outfile,"  Log MII Msgs: %s\n\n",temp);
	fprintf(outfile," Error Flags\n");
	GetDlgItemText(IDC_MODPARAMS_ICHVERR,temp);
	GetDlgItemText(IDC_MODPARAMS_FCHVERR,temp1);
	fprintf(outfile,"           ICHVERR%%:%s    FCHVERR%%:%s\n",temp,temp1);
	fprintf(outfile,"                       High     Low\n");
	GetDlgItemText(IDC_MODPARAMS_BATVOLTH,temp);
	GetDlgItemText(IDC_MODPARAMS_BATVOLTL,temp1);
	fprintf(outfile,"  Battery (24 Volt): %6s  %6s\n",temp,temp1);
	GetDlgItemText(IDC_MODPARAMS_SUP_P15H,temp);
	GetDlgItemText(IDC_MODPARAMS_SUP_P15L,temp1);
	fprintf(outfile,"    +12 Volt Supply: %6s  %6s\n",temp,temp1);
	GetDlgItemText(IDC_MODPARAMS_SUP_M15H,temp);
	GetDlgItemText(IDC_MODPARAMS_SUP_M15L,temp1);
	fprintf(outfile,"    -12 Volt Supply: %6s  %6s\n",temp,temp1);
	GetDlgItemText(IDC_MODPARAMS_SUP_P05H,temp);
	GetDlgItemText(IDC_MODPARAMS_SUP_P05L,temp1);
	fprintf(outfile,"     +5 Volt Supply: %6s  %6s\n",temp,temp1);
	GetDlgItemText(IDC_MODPARAMS_TIME_ERR,temp);
	fprintf(outfile,"  Time Delta (Sec.): %6s\n\n",temp);
	fprintf(outfile," General\n");
	GetDlgItemText(IDC_MODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp);
	GetDlgItemText(IDC_MODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp);
	GetDlgItemText(IDC_MODPARAMS_MAXBBM,temp);
	fprintf(outfile,"           Maximum BBM (Bytes): %s\n",temp);
	if(((CButton*)GetDlgItem(IDC_CHECK_BINARY))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"            Do BINARY DUMPBBBM: %s\n",temp);


	GetDlgItemText(IDC_MODPARAMS_COMMFAIL,temp);
	fprintf(outfile,"  # of Retransmits Before Fail: %s\n",temp);

	if(((CButton*)GetDlgItem(IDC_CHECK_ALLOWAUTODATE))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"    Allow Auto Date & Time Set: %s\n",temp);
	
	GetDlgItemText(IDC_MODPARAMS_MAXINQUIRE,temp);
	fprintf(outfile,"          Inquire Every(units): %s\n",temp);
	GetDlgItemText(IDC_MODPARAMS_TIMEOUT,temp);
	fprintf(outfile,"     Response Time-Out (units): %s\n",temp);
	GetDlgItemText(IDC_MODPARAMS_MAXSTATUS,temp);
	fprintf(outfile,"     Take Status Every (units): %s\n",temp);
}
*/
//////////////////////////////////////////////////////////////////////////////
//
//                   WriteIniFileDefaultSection
//
//////////////////////////////////////////////////////////////////////////////
void CModParam::WriteIniFileDefaultSection()
{
//	TRACE("CModParam::WriteIniFileDefaultSection()\n");

	WritePrivateProfileSection(GRAND_DEFAULT,"",m_szIniFile);

	WritePrivateProfileString(GRAND_DEFAULT,"SLOW_RESET","900",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"BATVOLTH","14.5",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"BATVOLTL","11.3",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_P15H","12.0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_P15L","11.0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_M15H","-11.0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_M15L","-12.0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_P05H","5.3",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SUP_P05L","4.7",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"TIME_ERR","60",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SLOW_RESET","900",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"NODE","-1",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"PORT","",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"FLAGCOMM","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"FLAGOTHR","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"FLAGTIME","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"MAXCYCLE","1000",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"MAXPAUSE","5400",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"DO_DUMP","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"LOG_FILTER","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"LOG_MII","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,SAVE_LOC,"C:\\DATA\\GRAND01",m_szIniFile);

	WritePrivateProfileString(GRAND_DEFAULT,"MAXINQUIRE","30",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"MAXSTATUS","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"COMMFAIL","5",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"AUTOTIMESET","1",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"LBREXTEND","0",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"TIMEOUT","5",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"BINARY_BBM","1",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"MAXBBM","100",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT, FILE_ID,"01",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"ICHVERR","10",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"FCHVERR","5",m_szIniFile);

	WritePrivateProfileString(GRAND_DEFAULT,"AUTOTIMESETDAILY",AUTOTIMEDAILY_DEF,m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"AUTOTIMESETDAILYAT",AUTOTIMEDAILYAT_DEF,m_szIniFile);

	//<<QA>> This writes a lot of data into the mic.ini file.
	CGrandSetupData cGrandSetupData;
	//char szTemp[64];
	char *pszDescription;
	char *pszIniName;
	char *pszDefault;
	char *pszCommand;
	while ((pszDescription = cGrandSetupData.GetNext())!=NULL)
	{
		pszIniName = cGrandSetupData.GetNext();
		pszDefault = cGrandSetupData.GetNext();
		pszCommand = cGrandSetupData.GetNext();
		WritePrivateProfileString(GRAND_DEFAULT,pszIniName,pszDefault,m_szIniFile);
	}
	while ((pszDescription = cGrandSetupData.GetNewNext())!=NULL)
	{
		pszIniName = cGrandSetupData.GetNewNext();
		pszDefault = cGrandSetupData.GetNewNext();
		pszCommand = cGrandSetupData.GetNewNext();
		WritePrivateProfileString(GRAND_DEFAULT,pszIniName,pszDefault,m_szIniFile);
	}

/*
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ACQ_MODE12","\"1F\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ACQ_MODE1B","\"2\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_1B_ROM_CHECKSUM","\"0000\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_LLD_A","\"  0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_LLD_B","\"  0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ULD_A","\"  0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ULD_B","\"  0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_UNIT_ID","\"11\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_BOARD1_ID","\"07\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_BOARD2_ID","\"06\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_USER_ID","\"7777777\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA1_MODE","\"A\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA2_MODE","\"A\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_MAX_GAIN0","\"4194304\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_MAX_GAIN1","\"4194304\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_DISCONNECT_MODE","\"I\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GRAND_USERVER","\"03.76\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_VERSION","\"03.76\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ACQUIRE_TIME","\"    1\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_BUFFER_TOTAL","\"70\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_BUFFER_SAVE","\"50\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_PULSEATHRESH","\"   100.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_PULSEBTHRESH","\"   100.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_PULSECTHRESH","\"   100.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA_1THRESH","\"     100.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA_2THRESH","\"     100.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NOFILTERENTERMII_RUNS","\" 3\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NOFILTERENTERMII_SIGMA","\"3.0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ENTERMII_RUNS","\" 3\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ENTERMII_SIGMA","\"1.5\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_MII_EXIT","\"10\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_LOCAL_BACKGROUND_FIRST","\"25\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_LOCAL_BACKGROUND_LAST","\"30\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_ACQ_MODE1C","\"1f\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_1C_ROM_CHECKSUM","\"0000\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_FILTER_METHOD","\"0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_TRIGGER_MODE","\"1\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_TAKE_OFFSET","\" 24\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_TAKEOFFSETMAX","\" 48\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_LOG_BATTERY","\" 1\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_FILTERING_ENABLED","\"0\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_A_LOWERLIMIT","\"0.200\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_B_LOWERLIMIT","\"0.200\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_NEUTRON_C_LOWERLIMIT","\"0.200\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA_1_LOWERLIMIT","\"  0.020\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_GAMMA_2_LOWERLIMIT","\"  0.020\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_SYNC_ENABLED","\"1\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_SYNC_TIME","\"04:30:30\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_SYNC_LOWERLIMIT","\" 2\"",m_szIniFile);
	WritePrivateProfileString(GRAND_DEFAULT,"SET_SYNC_UPPERLIMIT","\"5\"",m_szIniFile);
*/
}

void CModParam::OnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if (m_pPS) m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	
	*pResult = 0;
}
