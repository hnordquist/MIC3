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
// HHMRModParam.cpp

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "HHMRModParam.h"
#include "DirDialog.h"
#include "HHMRInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SLOW_RESET_SPIN_MIN	0
#define SLOW_RESET_SPIN_MAX	9999
#define SLOW_RESET_SPIN_DEF	900

#define MAX_TIMEOUT_SPIN_MIN	2
#define MAX_TIMEOUT_SPIN_MAX	99
#define MAX_TIMEOUT_SPIN_DEF	20

#define REALSTHRESH_SPIN_MIN 1
#define REALSTHRESH_SPIN_MAX 9999
#define REALSTHRESH_SPIN_DEF 50

#define TOTALSTHRESH_SPIN_MIN 1
#define TOTALSTHRESH_SPIN_MAX 9999
#define TOTALSTHRESH_SPIN_DEF 50

#define TOTALS1THRESH_SPIN_MIN 1
#define TOTALS1THRESH_SPIN_MAX 9999
#define TOTALS1THRESH_SPIN_DEF 50

#define TOTALSLTHRESH_SPIN_MIN 0
#define TOTALSLTHRESH_SPIN_MAX 9999
#define TOTALSLTHRESH_SPIN_DEF 10

#define SIGMATHRESH_SPIN_MIN 1
#define SIGMATHRESH_SPIN_MAX 32000
#define SIGMATHRESH_SPIN_DEF 50

#define GATEWIDTH_SPIN_MIN 1
#define GATEWIDTH_SPIN_MAX 999
#define GATEWIDTH_SPIN_DEF 50

#define HIGHV_SPIN_MIN 500
#define HIGHV_SPIN_MAX 4999
#define HIGHV_SPIN_DEF 1800

#define MAXCOMPRESS_SPIN_MIN 1
#define MAXCOMPRESS_SPIN_MAX 999
#define MAXCOMPRESS_SPIN_DEF 25

#define CT_UNITS_SPIN_MIN 0
#define CT_UNITS_SPIN_MAX 99998
#define CT_UNITS_SPIN_DEF "16"

//#define CT_EXP_SPIN_MIN 0
//#define CT_EXP_SPIN_MAX 9
//#define CT_EXP_SPIN_DEF "1"

#define PREDELAY_DEF 1

#define HIGHVOLTH_SPIN_DEF  "4999"
#define HIGHVOLTH_SPIN_MAX	4999
#define HIGHVOLTH_SPIN_MIN	0
#define HIGHVOLTL_SPIN_MAX  4999
#define HIGHVOLTL_SPIN_MIN  0
#define HIGHVOLTL_SPIN_DEF  "0"

#define NODE_SPIN_MAX		32767
#define NODE_SPIN_MIN		-1
#define NODE_SPIN_DEF		-1

#define MAXCYCLE_SPIN_MIN	100
#define MAXCYCLE_SPIN_MAX	2000
#define MAXCYCLE_SPIN_DEF	300

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define DODUMP_DEF			1
#define LOGMII_DEF			1
#define LOGFILTER_DEF		1

#define FILEID_SPIN_MAX	99
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\HHMR01"
#define PORT_DEF			"ERROR"

#define UPDATE_MESSAGE	(WM_USER+3333)
//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;
///////////////////////////////////////////////////////////////////////////
// CHHMRModParam property page

IMPLEMENT_DYNCREATE(CHHMRModParam, CPropertyPage)

CHHMRModParam::CHHMRModParam() : CPropertyPage(CHHMRModParam::IDD)
{
}


CHHMRModParam::CHHMRModParam(UINT id, CISO* pParent, bool DoingConfig) 
	: CPropertyPage(id)
{
	m_pParent = pParent;
	m_bChange = false;
	m_bDataSetup = false;
	m_bCollectingParametersForNewISO = DoingConfig;
	m_pPS = NULL;
	m_pApplyButton = NULL;
	//m_pApplyButton = NULL;
	//m_pApplyHHMRButton = NULL;  //<<QA>> later

	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;

	for (int i= IDC_HHMRMODPARAMS_FIRST; i <= IDC_HHMRMODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_HHMRMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			HHMR_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}
}

CHHMRModParam::~CHHMRModParam()
{
	delete m_pApplyButton;
	delete m_pApplyHHMRButton;
}

void CHHMRModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHMRModParam)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHHMRModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CHHMRModParam)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_APPLYHHMR, OnModparamsApplyHHMR)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_HIGHVOLTL_SPIN, OnDeltaposModparamsHighvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_HIGHVOLTH_SPIN, OnDeltaposModparamsHighvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_CBN_SELCHANGE(IDC_HHMRMODPARAMS_PORT, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_CT_UNITS, OnChangeModparamsCT)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_SAVELOC, OnChangeModparamsSaveLoc)
	ON_EN_KILLFOCUS(IDC_HHMRMODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_HIGHVOLTL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_REALSTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_TOTALSTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_TOTALS1THRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_TOTALSLTHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_SIGMATHRESH_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_MAXCOMPRESS_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_LOGFILTER, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_FILEID, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_HHMRMODPARAMS_RESET, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_SLOW_RESET_SPIN,OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HHMRMODPARAMS_MAX_TIMEOUT_SPIN,OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_SLOW_RESET,OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_NODE,		OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_MAXCYCLE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_REALSTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_TOTALSTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_TOTALSLTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_TOTALS1THRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_TOTALSLTHRESH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_SIGMATHRESH,OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_MAXCOMPRESS, OnChangeModparamsMost)
	ON_CBN_SELCHANGE(IDC_HHMRMODPARAMS_PREDELAY, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_GATEWIDTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_HIGHV, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_HIGHVOLTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_SAVELOC,OnChangeModparamsFileID)
	ON_EN_CHANGE(IDC_HHMRMODPARAMS_PORT,OnChangeModparamsPort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//	ON_EN_CHANGE(IDC_HHMRMODPARAMS_CT_EXP, OnChangeModparamsCT)

void CHHMRModParam::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_HHMRMODPARAMS_FIRST) &&
		(index <= IDC_HHMRMODPARAMS_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

void CHHMRModParam::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_HHMRMODPARAMS_FIRST) &&
		(index <= IDC_HHMRMODPARAMS_LAST)) {
	  StaticColors[index-IDC_HHMRMODPARAMS_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CHHMRModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	char temp2[2*MAX_PATH];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
	strcpy(temp2,"53370 HHMR COLLECT CONFIG CHANGE - ");
	strcat(temp2,szItem);
	strcat(temp2," = ");
	strcat(temp2,temp);

	if (m_pPS && !m_bCollectingParametersForNewISO)
		((CHHMRPropertySheet*)m_pPS)->WriteToFile(TO_PFM,TYPE_COMP,NULL,temp2);
}

void CHHMRModParam::DoApply()
{
	//get all the data from the controls and save them
//	TRACE("CHHMRModParam::DoApply\n");

	char Name[_MAX_PATH];
	char Temp[_MAX_PATH];

	strncpy(Name, m_pParent->GetName(),sizeof(Name)-1);

	if (GetDlgItemInt(IDC_HHMRMODPARAMS_MAX_TIMEOUT) < MAX_TIMEOUT_SPIN_MIN)
		SetDlgItemInt(IDC_HHMRMODPARAMS_MAX_TIMEOUT,MAX_TIMEOUT_SPIN_MIN);
	if (GetDlgItemInt(IDC_HHMRMODPARAMS_MAX_TIMEOUT) > MAX_TIMEOUT_SPIN_MAX)
		SetDlgItemInt(IDC_HHMRMODPARAMS_MAX_TIMEOUT,MAX_TIMEOUT_SPIN_MAX);
	
	if (GetDlgItemInt(IDC_HHMRMODPARAMS_SLOW_RESET) < SLOW_RESET_SPIN_MIN)
		SetDlgItemInt(IDC_HHMRMODPARAMS_SLOW_RESET,SLOW_RESET_SPIN_MIN);
	if (GetDlgItemInt(IDC_HHMRMODPARAMS_SLOW_RESET) > SLOW_RESET_SPIN_MAX)
		SetDlgItemInt(IDC_HHMRMODPARAMS_SLOW_RESET,SLOW_RESET_SPIN_MAX);

	SaveItem(Name,"HIGHVOLTH",	IDC_HHMRMODPARAMS_HIGHVOLTH);
	SaveItem(Name,"HIGHVOLTL",	IDC_HHMRMODPARAMS_HIGHVOLTL);
	SaveItem(Name,"SLOW_RESET",	IDC_HHMRMODPARAMS_SLOW_RESET);
	SaveItem(Name, NODE ,		IDC_HHMRMODPARAMS_NODE);
	SaveItem(Name,"MAXCYCLE",	IDC_HHMRMODPARAMS_MAXCYCLE);
	SaveItem(Name, MAXPAUSE ,	IDC_HHMRMODPARAMS_MAXPAUSE);
	SaveItem(Name,"COMMFAIL",	IDC_HHMRMODPARAMS_MAX_TIMEOUT);
	SaveItem(Name,"REALSTHRESH", IDC_HHMRMODPARAMS_REALSTHRESH);
	SaveItem(Name,"TOTALSTHRESH", IDC_HHMRMODPARAMS_TOTALSTHRESH);
	SaveItem(Name,"TOTALS1THRESH",IDC_HHMRMODPARAMS_TOTALS1THRESH);
	SaveItem(Name,"TOTALSLOWTHRESH",IDC_HHMRMODPARAMS_TOTALSLTHRESH);
	SaveItem(Name,"SIGMATHRESH", IDC_HHMRMODPARAMS_SIGMATHRESH);
	SaveItem(Name,"MAXCOMPRESS",  IDC_HHMRMODPARAMS_MAXCOMPRESS);
	SaveItem(Name,"COUNTTIMEUNIT",IDC_HHMRMODPARAMS_CT_UNITS);
//	SaveItem(Name,"COUNTTIMEEXP", IDC_HHMRMODPARAMS_CT_EXP);
	SaveItem(Name,"DO_DUMP",	IDC_HHMRMODPARAMS_DODUMP,	true);
	SaveItem(Name,"LOG_FILTER",	IDC_HHMRMODPARAMS_LOGFILTER, true);
//	SaveItem(Name,"LOG_MII",	IDC_HHMRMODPARAMS_LOGMII,	true);
	SaveItem(Name, FILE_ID,	IDC_HHMRMODPARAMS_FILEID);
	SaveItem(Name,"PORT",		IDC_HHMRMODPARAMS_PORT);

	GetDlgItemText(IDC_HHMRMODPARAMS_CT_UNITS,Temp,sizeof(Temp));
	WritePrivateProfileString(Name, "COUNTTIMEUNIT", Temp, m_pParent->m_szIniFile);
//	GetDlgItemText(IDC_HHMRMODPARAMS_CT_EXP,Temp,sizeof(Temp));
//	WritePrivateProfileString(Name, "COUNTTIMEEXP",Temp,m_pParent->m_szIniFile);
	int select = ((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PREDELAY))->GetCurSel();
	sprintf(Temp,"%0X",select);
	WritePrivateProfileString(Name, "PREDELAY",Temp,m_pParent->m_szIniFile);
	GetDlgItemText(IDC_HHMRMODPARAMS_GATEWIDTH,Temp,sizeof(Temp));
	WritePrivateProfileString(Name,"GATEWIDTH",Temp,m_pParent->m_szIniFile);
	GetDlgItemText(IDC_HHMRMODPARAMS_HIGHV,Temp,sizeof(Temp));
	WritePrivateProfileString(Name,"HIGHV",Temp,m_pParent->m_szIniFile);
	
	GetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,Temp,sizeof(Temp));
	while ((Temp[strlen(Temp)-1]=='\\')||
		   (Temp[strlen(Temp)-1]=='/' )  ) 
	{
		Temp[strlen(Temp)-1] = NULL;
		SetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,Temp);
	}
	SaveItem(Name,SAVE_LOC,	IDC_HHMRMODPARAMS_SAVELOC);
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
		m_pApplyButton->EnableWindow(FALSE);
	}
}

void CHHMRModParam::OnModparamsApplyHHMR()
{
	//Button caption:  "Apply New HHMR Configuration To Instrument && MIC"
	if (m_pParent->Validate())
	{
		char *pName = m_pParent->m_pName;
		char *file = m_pParent->m_szIniFile;
		char temp[32];

		//if it doesn't have these in it then give it a set
		/*GetPrivateProfileString(pName,"CAMERA_PORT","",temp,sizeof(temp),file);
		if (strlen(temp)==0)
		{
			WritePrivateProfileString(pName,"CAMERA_PORT","LPT2",file);
			WritePrivateProfileString(pName,"CAMERA_PIN","2",file);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_TYPE","RT1",file);
//			WritePrivateProfileString(pName,"CAMERA_TRIGGER_THRESHOLD_RATE","50.00",m_pParent->m_szIniFile);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_HOLDOFF","0:0:0:10",file);
			WritePrivateProfileString(pName,"CAMERA_TRIGGER_SUPPRESS","0:0:5:0",file);
		}*/

		//get each of the HHMR specific dialog item and save them in the ini file
		GetDlgItemText(IDC_HHMRMODPARAMS_CT_UNITS,temp,sizeof(temp));
		if (strlen(temp) < 1)
		{
			strcpy(temp,CT_UNITS_SPIN_DEF);
			SetDlgItemText(IDC_HHMRMODPARAMS_CT_UNITS,temp);
		}
		WritePrivateProfileString(pName, "COUNTTIMEUNIT", temp, file);

		//get exponential of count time
//		GetDlgItemText(IDC_HHMRMODPARAMS_CT_EXP,temp,sizeof(temp));
//		if (strlen(temp) < 1)
//		{
//			strcpy(temp, CT_EXP_SPIN_DEF);
//			SetDlgItemText(IDC_HHMRMODPARAMS_CT_EXP,temp);
//		}
//		WritePrivateProfileString(pName, "COUNTTIMEEXP",temp,file);

		//get predelay
		int select = ((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PREDELAY))->GetCurSel();
		sprintf(temp,"%0X",select);
		WritePrivateProfileString(pName,"PREDELAY",temp,file);

		//get gate width
		GetDlgItemText(IDC_HHMRMODPARAMS_GATEWIDTH,temp,sizeof(temp));
		WritePrivateProfileString(pName,"GATEWIDTH",temp,file);

		//get high voltage
		GetDlgItemText(IDC_HHMRMODPARAMS_HIGHV,temp,sizeof(temp));
		WritePrivateProfileString(pName,"HIGHV",temp,file);

		//tell parent instrument to do the jsr configurations
		m_pParent->PostMessage(uIDC_RECEIVE,RESETUP,NULL);
		m_pParent->UpdateParams(true);

		//tell the graphic window to update params
		char szTemp[_MAX_PATH];
		strcpy(szTemp,pName);
		strcat(szTemp," Watcher");
		CWnd * pcWnd = FindWindow(NULL,szTemp);
		if (pcWnd)
			pcWnd->PostMessage(UPDATE_MESSAGE,1,0);

	}
}

void CHHMRModParam::OnModparamsApply() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (m_pParent->Validate()) 
	{
		DoApply();
		//tell the graphic window to update params
		char szTemp[_MAX_PATH];
		strcpy(szTemp,m_pParent->m_pName);
		strcat(szTemp," Watcher");
		CWnd * pcWnd = FindWindow(NULL,szTemp);
		if (pcWnd)
			pcWnd->PostMessage(UPDATE_MESSAGE,1,0);

		m_bChange = false;
		m_pApplyButton->EnableWindow(FALSE);
	}
}

//reset float item for a spinner with a non-automatic buddy
void CHHMRModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
{
	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	float ftemp = (float)atof(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos((int)(ftemp*10.0));
	sprintf(temp,"%1.1f",ftemp);
	SetDlgItemText(dTextItem,temp);
}

//reset int item for a spinner with a non-automatic buddy
void CHHMRModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
						   int dItem,int dMin, int dMax, int dTextItem)
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
//	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
//	GetDlgItem(dTextItem)->SetWindowText(temp);
//	_itoa(dMax,temp,10);
//	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));

	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange32(dMin,dMax);
//	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(atoi(szDefault));
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	_itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));

}

//reset item for a spinner with an automatic buddy
void CHHMRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CHHMRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a list box
void CHHMRModParam::ResetItemL(char *szName,char *szItem, int /*dDefault*/, int dItem)
{
	char temp[4];
	int dtemp;
	//should change the "1" to _itoa(dDefault)
	GetPrivateProfileString(szName,szItem,"1",temp,sizeof(temp),m_pParent->m_szIniFile);
	sscanf(temp,"%x",&dtemp);
	((CComboBox*)GetDlgItem(dItem))->SetCurSel(dtemp);
}

//reset item for a simple edit field
void CHHMRModParam::ResetItem(char *szName,char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_HHMRMODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, szDefault);
		//WritePrivateProfileString(szName, szItem, szDefault, m_pParent->m_szIniFile);
		break;
	default:; 
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}

void CHHMRModParam::SetPropertySheet(CPropertySheet *pPS)//, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
}


void CHHMRModParam::ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem)
{
	char temp[3];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
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

void CHHMRModParam::OnModparamsReset() 
{
//	TRACE("CHHMRModParam::OnModparamsReset\n");

	if (m_pPS)m_pPS->SetToClose(0);

	m_bDataSetup = true;

	if (!m_bCollectingParametersForNewISO)
	{
		GetDlgItem(IDC_HHMRMODPARAMS_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_HHMRMODPARAMS_NODE)->EnableWindow(FALSE);
	}

	ResetItem(m_pParent->m_pName,FILE_ID, FILEID_SPIN_DEF,
		IDC_HHMRMODPARAMS_FILEID_SPIN,IDC_HHMRMODPARAMS_FILEID);

	ResetItem(m_pParent->m_pName,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_HHMRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"COMMFAIL",MAX_TIMEOUT_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAX_TIMEOUT_SPIN,MAX_TIMEOUT_SPIN_MIN,MAX_TIMEOUT_SPIN_MAX);

	ResetItem(m_pParent->m_pName, NODE ,NODE_SPIN_DEF,
		IDC_HHMRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
	
	ResetItem(m_pParent->m_pName,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"DO_DUMP", DODUMP_DEF, IDC_HHMRMODPARAMS_DODUMP);

	ResetItem(m_pParent->m_pName,"LOG_FILTER", LOGFILTER_DEF, IDC_HHMRMODPARAMS_LOGFILTER);

//	ResetItem(m_pParent->m_pName,"LOG_MII", LOGMII_DEF, IDC_HHMRMODPARAMS_LOGMII);

	ResetItem(m_pParent->m_pName,SAVE_LOC, SAVELOC_DEF, IDC_HHMRMODPARAMS_SAVELOC);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->ResetContent();

		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->AddString(buff2);

			at = at + strlen(at)+1;
		}

		((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->SetCurSel(0);
	}

	ResetItem(m_pParent->m_pName,PORT, PORT_DEF, IDC_HHMRMODPARAMS_PORT);

	ResetItemI(m_pParent->m_pName,MAXPAUSE,
		MAXPAUSE_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXPAUSE_SPIN,
		MAXPAUSE_SPIN_MIN,
		MAXPAUSE_SPIN_MAX,
		IDC_HHMRMODPARAMS_MAXPAUSE);

	ResetItemI(m_pParent->m_pName,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_HHMRMODPARAMS_HIGHVOLTH);

	ResetItemI(m_pParent->m_pName,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_HHMRMODPARAMS_HIGHVOLTL);

	ResetItem(m_pParent->m_pName,"REALSTHRESH",
		REALSTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_REALSTHRESH_SPIN,
		REALSTHRESH_SPIN_MIN,
		REALSTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALSTHRESH",
		TOTALSTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALSTHRESH_SPIN,
		TOTALSTHRESH_SPIN_MIN,
		TOTALSTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALS1THRESH",
		TOTALS1THRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALS1THRESH_SPIN,
		TOTALS1THRESH_SPIN_MIN,
		TOTALS1THRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"TOTALSLOWTHRESH",
		TOTALSLTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALSLTHRESH_SPIN,
		TOTALSLTHRESH_SPIN_MIN,
		TOTALSLTHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"SIGMATHRESH",
		SIGMATHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_SIGMATHRESH_SPIN,
		SIGMATHRESH_SPIN_MIN,
		SIGMATHRESH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"GATEWIDTH",
		GATEWIDTH_SPIN_DEF,
		IDC_HHMRMODPARAMS_GATEWIDTH_SPIN,
		GATEWIDTH_SPIN_MIN,
		GATEWIDTH_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"HIGHV",
		HIGHV_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHV_SPIN,
		HIGHV_SPIN_MIN,
		HIGHV_SPIN_MAX);

	ResetItem(m_pParent->m_pName,"MAXCOMPRESS",
		MAXCOMPRESS_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXCOMPRESS_SPIN,
		MAXCOMPRESS_SPIN_MIN,
		MAXCOMPRESS_SPIN_MAX);

	ResetItemI(m_pParent->m_pName,"COUNTTIMEUNIT",
		CT_UNITS_SPIN_DEF,
		IDC_HHMRMODPARAMS_CT_UNITS_SPIN,
		CT_UNITS_SPIN_MIN,
		CT_UNITS_SPIN_MAX,
		IDC_HHMRMODPARAMS_CT_UNITS);

//	ResetItemI(m_pParent->m_pName,"COUNTTIMEEXP",
//		CT_EXP_SPIN_DEF,
//		IDC_HHMRMODPARAMS_CT_EXP_SPIN,
//		CT_EXP_SPIN_MIN,
//		CT_EXP_SPIN_MAX,
//		IDC_HHMRMODPARAMS_CT_EXP);

	ResetItemL(m_pParent->m_pName,"PREDELAY",
		PREDELAY_DEF,
		IDC_HHMRMODPARAMS_PREDELAY);

	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CHHMRModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

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
	*pResult = 0;
}

void CHHMRModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
								int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

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
	sscanf(temp2,"%d",&buddyvalue);
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

void CHHMRModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS)m_pPS->SetToClose(0);

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

void CHHMRModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

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
	sscanf(temp2,"%f",&fbuddyvalue);
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


void CHHMRModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

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
	GetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,Text);

//	if "HHMR" at end then...
	Text.MakeReverse();
	switch (Text.Find("RMH"))
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
				Text += "HHMR";
			else
				Text += "\\HHMR";
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
				Text += "HHMR";
			else
				Text += "\\HHMR";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "HHMR";
			else
				Text += "\\HHMR";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,Text);
}

void CHHMRModParam::OnDeltaposModparamsHighvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTL_SPIN_MIN, HIGHVOLTL_SPIN_MAX, 
		IDC_HHMRMODPARAMS_HIGHVOLTL, IDC_HHMRMODPARAMS_HIGHVOLTH,IDC_HHMRMODPARAMS_HIGHVOLTH_SPIN, true);
}

void CHHMRModParam::OnDeltaposModparamsHighvolthSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);;
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTH_SPIN_MIN, HIGHVOLTH_SPIN_MAX, 
		IDC_HHMRMODPARAMS_HIGHVOLTH, IDC_HHMRMODPARAMS_HIGHVOLTL, IDC_HHMRMODPARAMS_HIGHVOLTL_SPIN, false);
}

void CHHMRModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_HHMRMODPARAMS_MAXPAUSE);
}

void CHHMRModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_HHMRMODPARAMS_FILEID);
}

void CHHMRModParam::OnModparamsBrowse() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_HHMRMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("HHMR"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "HHMR";
			else
				Text += "\\HHMR";
			Text += Id;
		}
		SetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC, Text);

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnModparamsDefault(bool SetPort) 
{
	if (m_pPS)m_pPS->SetToClose(0);

	m_bDataSetup = true;

	ResetItem(HHMR_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_HHMRMODPARAMS_FILEID_SPIN, IDC_HHMRMODPARAMS_FILEID);

	ResetItem(HHMR_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_HHMRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"COMMFAIL",MAX_TIMEOUT_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAX_TIMEOUT_SPIN,MAX_TIMEOUT_SPIN_MIN,MAX_TIMEOUT_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_HHMRMODPARAMS_DODUMP);

	ResetItem(HHMR_DEFAULT,"LOG_FILTER",LOGFILTER_DEF,IDC_HHMRMODPARAMS_LOGFILTER);

	ResetItem(HHMR_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_HHMRMODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(HHMR_DEFAULT,"NODE",NODE_SPIN_DEF,
			IDC_HHMRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
		
		char buff[16384];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			
			((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->ResetContent();
			
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->AddString(buff2);

				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_HHMRMODPARAMS_PORT))->SetCurSel(0);
	
		ResetItem(HHMR_DEFAULT,PORT,buff2,IDC_HHMRMODPARAMS_PORT);
	}

	ResetItemI(HHMR_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_HHMRMODPARAMS_MAXPAUSE);

	ResetItemI(HHMR_DEFAULT,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_HHMRMODPARAMS_HIGHVOLTH);

	ResetItemI(HHMR_DEFAULT,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_HHMRMODPARAMS_HIGHVOLTL);

	ResetItem(HHMR_DEFAULT,"REALSTHRESH",
		REALSTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_REALSTHRESH_SPIN,
		REALSTHRESH_SPIN_MIN,
		REALSTHRESH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"TOTALSTHRESH",
		TOTALSTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALSTHRESH_SPIN,
		TOTALSTHRESH_SPIN_MIN,
		TOTALSTHRESH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"TOTALS1THRESH",
		TOTALS1THRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALS1THRESH_SPIN,
		TOTALS1THRESH_SPIN_MIN,
		TOTALS1THRESH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"TOTALSLOWTHRESH",
		TOTALSLTHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_TOTALSLTHRESH_SPIN,
		TOTALSLTHRESH_SPIN_MIN,
		TOTALSLTHRESH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"SIGMATHRESH",
		SIGMATHRESH_SPIN_DEF,
		IDC_HHMRMODPARAMS_SIGMATHRESH_SPIN,
		SIGMATHRESH_SPIN_MIN,
		SIGMATHRESH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"GATEWIDTH",
		GATEWIDTH_SPIN_DEF,
		IDC_HHMRMODPARAMS_GATEWIDTH_SPIN,
		GATEWIDTH_SPIN_MIN,
		GATEWIDTH_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"HIGHV",
		HIGHV_SPIN_DEF,
		IDC_HHMRMODPARAMS_HIGHV_SPIN,
		HIGHV_SPIN_MIN,
		HIGHV_SPIN_MAX);

	ResetItem(HHMR_DEFAULT,"MAXCOMPRESS",
		MAXCOMPRESS_SPIN_DEF,
		IDC_HHMRMODPARAMS_MAXCOMPRESS_SPIN,
		MAXCOMPRESS_SPIN_MIN,
		MAXCOMPRESS_SPIN_MAX);

	ResetItemI(HHMR_DEFAULT,"COUNTTIMEUNIT",
		CT_UNITS_SPIN_DEF,
		IDC_HHMRMODPARAMS_CT_UNITS_SPIN,
		CT_UNITS_SPIN_MIN,
		CT_UNITS_SPIN_MAX,
		IDC_HHMRMODPARAMS_CT_UNITS);

//	ResetItemI(HHMR_DEFAULT,"COUNTTIMEEXP",
//		CT_EXP_SPIN_DEF,
//		IDC_HHMRMODPARAMS_CT_EXP_SPIN,
//		HIGHVOLTL_SPIN_MIN,
//		HIGHVOLTL_SPIN_MAX,
//		IDC_HHMRMODPARAMS_CT_EXP);

	ResetItemL(HHMR_DEFAULT,"PREDELAY",
		PREDELAY_DEF,
		IDC_HHMRMODPARAMS_PREDELAY);

}


BOOL CHHMRModParam::OnSetActive() 
{
//	TRACE("CHHMRModParam::OnSetActive\n");

	if (m_bCollectingParametersForNewISO)
	{
		if (m_pApplyButton)
			m_pApplyButton->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_HHMRMODPARAMS_RESET)->ShowWindow(SW_HIDE);

		if (!m_bDataSetup) 
		{
			OnModparamsDefault();

		    if (m_pPS)
			{
				CString sCaption(m_pParent->m_pName);
				TC_ITEM tcItem;
				tcItem.mask = TCIF_TEXT;
				tcItem.pszText = (LPTSTR)((LPCTSTR)sCaption);
				m_pPS->GetTabControl()->SetItem(0, &tcItem );
				m_pPS->m_bSameNextFinish = false;
			}
		}

		//GetDlgItem(IDC_HHMRMODPARAMS_APPLY)->EnableWindow(false);
		SetDlgItemText(IDC_STATIC_MESSAGE,"These items will not be applied to the HHMR instrument until "
			"initial setup is completed.");
		GetDlgItem(IDC_HHMRMODPARAMS_APPLYHHMR)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_HHMRMODPARAMS_APPLYHHMR)->EnableWindow(false);
		GetDlgItem(IDC_HHMRMODPARAMS_STATICTEXT)->SetWindowText("");
	}
	else  //!m_bCollectingParametersForNewISO
	{
		m_pApplyButton->EnableWindow(m_bChange);
		//GetDlgItem(IDC_HHMRMODPARAMS_APPLYHHMR)->EnableWindow(false);
		GetDlgItem(IDC_HHMRMODPARAMS_APPLYHHMR)->EnableWindow(m_pParent->m_bPause);
	}
	return CPropertyPage::OnSetActive();
}
BOOL CHHMRModParam::OnWizardFinish()
{
	//	ASSERT(m_bCollectingParametersForNewISO);

	DoApply();
	return CPropertyPage::OnWizardFinish();
}
void CHHMRModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS)m_pPS->SetToClose(0);

/*
	CWnd* pWnd;

	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_HHMRMODPARAMS_FIRST) && (index <= IDC_HHMRMODPARAMS_LAST))
		{
			MessageBox("Got One",MB_OK);
		}
	}
*/
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CHHMRModParam::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if (m_bCollectingParametersForNewISO && bShow)
	{
		m_pPS->SetWizardButtons(PSWIZB_FINISH);
		//m_pPS->SetWizardButtons(PSWIZB_NEXT);
		//Got rid of camera triggering, so hide the back/next buttons. Only do Finish.

		//m_pPS->GetDlgItem(ID_WIZBACK)->ShowWindow(SW_HIDE);
		//m_pPS->GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_HIDE);
		m_pPS->GetDlgItem(ID_WIZFINISH)->ShowWindow(SW_SHOW);
		m_pPS->GetDlgItem(ID_WIZFINISH)->EnableWindow(TRUE);

		m_pApplyButton->ShowWindow(SW_HIDE);
		m_pApplyHHMRButton->ShowWindow(SW_HIDE);
	}
	else  //SCR00114
	{
		if (m_pApplyHHMRButton)
			m_pApplyHHMRButton->EnableWindow(m_pParent->IsPaused());
	}
	if (m_pPS)m_pPS->SetToClose(0);
}

void CHHMRModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	//NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CHHMRModParam::OnChangeModparamsMost()
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO && m_bDataSetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnChangeModparamsPort()
{
	if (m_pPS)m_pPS->SetToClose(0);
}

void CHHMRModParam::OnChangeModparamsFileID()
{
	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnChangeModparamsCT()
{
	if (m_pPS)m_pPS->SetToClose(0);

//	char units[8];
//	char exp[4];
//	char result[16];
//	GetDlgItemText(IDC_HHMRMODPARAMS_CT_UNITS,units,sizeof(units));
//	while (strlen(units) < 2)
//	{
//		units[2] = NULL;
//		units[1] = units[0];
//		units[0] = '0';
//	}
//	GetDlgItemText(IDC_HHMRMODPARAMS_CT_EXP,exp,sizeof(exp));
//	result[0] = units[0];
//	result[1] = '.';
//	result[2] = units[1];
//	result[3] = 'E';
//	result[4] = exp[0];
//	result[5] = NULL;
//	SetDlgItemText(IDC_STATIC_CT_RESULT,result);

	if (!m_bCollectingParametersForNewISO && m_bDataSetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnChangeModparamsSaveLoc() 
{
	CString Text;
	GetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}

	if (m_pPS)m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CHHMRModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_HHMRMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_HHMRMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_HHMRMODPARAMS_FILEID))->SetWindowText(temp);
}

BOOL CHHMRModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	m_pApplyButton = new CSimpleYellowButton(
		"Apply",
		IDC_HHMRMODPARAMS_APPLY,
		this);

	//SCR00114
	m_pApplyHHMRButton = new CSimpleYellowButton(
		"Apply New HHMR\nConfiguration To Instrument && MIC",
		IDC_HHMRMODPARAMS_APPLYHHMR,
		this);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////
//
//                   WriteIniFileDefaultSection
//
//////////////////////////////////////////////////////////////////////////////
void CHHMRModParam::WriteIniFileDefaultSection()
{
//	TRACE("CHHMRModParam::WriteIniFileDefaultSection()\n");

	char *file = m_pParent->m_szIniFile;

	WritePrivateProfileString(HHMR_DEFAULT,"MAXPAUSE","60",file);
	WritePrivateProfileString(HHMR_DEFAULT,"COMMFAIL","20",file);
	WritePrivateProfileString(HHMR_DEFAULT,"DO_DUMP","0",file);
	WritePrivateProfileString(HHMR_DEFAULT,"SLOW_RESET","900",file);
	WritePrivateProfileString(HHMR_DEFAULT,SAVE_LOC,"C:\\DATA\\HHMR01",file);
	WritePrivateProfileString(HHMR_DEFAULT,FILE_ID,"01",file);
	WritePrivateProfileString(HHMR_DEFAULT,"HIGHVOLTH","1803",file);
	WritePrivateProfileString(HHMR_DEFAULT,"HIGHVOLTL","1797",file);
	WritePrivateProfileString(HHMR_DEFAULT, NODE ,"-1",file);
	WritePrivateProfileString(HHMR_DEFAULT, PORT,"",file);
	WritePrivateProfileString(HHMR_DEFAULT,"MAXCYCLE","200",file);
	WritePrivateProfileString(HHMR_DEFAULT,"LOG_FILTER","1",file);
	WritePrivateProfileString(HHMR_DEFAULT, FLAGCOMM ,"0",file);
	WritePrivateProfileString(HHMR_DEFAULT, FLAGOTHR ,"0",file);
	WritePrivateProfileString(HHMR_DEFAULT, FLAGTIME ,"0",file);
	WritePrivateProfileString(HHMR_DEFAULT,"COUNTTIMEUNIT","30",file);
//	WritePrivateProfileString(HHMR_DEFAULT,"COUNTTIMEEXP","1",file);
	WritePrivateProfileString(HHMR_DEFAULT,"PREDELAY","6",file);
	WritePrivateProfileString(HHMR_DEFAULT,"GATEWIDTH","64",file);
	WritePrivateProfileString(HHMR_DEFAULT,"HIGHV","1800",file);
	WritePrivateProfileString(HHMR_DEFAULT,"REALSTHRESH","50",file);
	WritePrivateProfileString(HHMR_DEFAULT,"TOTALSTHRESH","50",file);
	WritePrivateProfileString(HHMR_DEFAULT,"TOTALS1THRESH","50",file);
	WritePrivateProfileString(HHMR_DEFAULT,"SIGMATHRESH","50",file);
	WritePrivateProfileString(HHMR_DEFAULT,"MAXCOMPRESS","25",file);

	/*WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_PIN","2",file);

	WritePrivateProfileString(HHMR_DEFAULT,"Remark: Allowed CAMERA_PORT strings","PORT1, PORT2, PORT3, or NONE",file);
	WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_PORT","NONE",file);

	WritePrivateProfileString(HHMR_DEFAULT,"Remark: Allowed CAMERA_TRIGGER_TYPE","R and/or T and/or 1",file);
	WritePrivateProfileString(HHMR_DEFAULT,"Remark: where R","reals, T = Totals, and 1 = Totals 1",file);

	WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_TRIGGER_TYPE","RT1",file);

	WritePrivateProfileString(HHMR_DEFAULT,"Remark: Format for CAMERA_TRIGGER_HOLDOFF","D:H:M:S",file);
	WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_TRIGGER_HOLDOFF","0:0:0:10",file);

	WritePrivateProfileString(HHMR_DEFAULT,"Remark: Format for CAMERA_TRIGGER_SUPPRESS","D:H:M:S",file);
	WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_TRIGGER_SUPPRESS","0:0:5:0",file);

	WritePrivateProfileString(HHMR_DEFAULT,"CAMERA_TRIGGER_THRESHOLD_RATE","50.00",file);*/

}
