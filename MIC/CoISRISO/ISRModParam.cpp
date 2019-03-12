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
// ISRModParam.cpp : implementation file
//

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "resource.h"
#include "colors.h"
#include "ISRModParam.h"
#include "DirDialog.h"
#include "ISRInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIME_ERR_SPIN_MIN	0
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

#define HIGHVOLTH_SPIN_DEF  "4999"
#define HIGHVOLTH_SPIN_MAX	4999
#define HIGHVOLTH_SPIN_MIN	0
#define HIGHVOLTL_SPIN_MAX  4999
#define HIGHVOLTL_SPIN_MIN  0
#define HIGHVOLTL_SPIN_DEF  "0"

#define SUP_P15H_SPIN_DEF	"24.0"
#define SUP_P15H_SPIN_MAX	240
#define SUP_P15H_SPIN_MIN	0
#define SUP_P15L_SPIN_MAX	240
#define SUP_P15L_SPIN_MIN	0
#define SUP_P15L_SPIN_DEF	"0.0"

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
#define NODE_SPIN_DEF		1

#define MAXCYCLE_SPIN_MIN	100
#define MAXCYCLE_SPIN_MAX	32767
#define MAXCYCLE_SPIN_DEF	1000

#define MAXPAUSE_SPIN_MIN	0
#define MAXPAUSE_SPIN_MAX	99999
#define MAXPAUSE_SPIN_DEF	"60"

#define MAXINQUIRE_SPIN_MIN 1
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

#define AUTOTIMEDAILY_DEF	0
#define AUTOTIMEDAILYAT_DEF	"4:30:30"

#define FILEID_SPIN_MAX	99
#define FILEID_SPIN_MIN	0
#define FILEID_SPIN_DEF	"01"

#define SAVELOC_DEF			"C:\\DATA\\ISR01"
#define PORT_DEF			"ERROR"
/////////////////////////////////////////////////////////////////////////////
// CISRModParam property page

IMPLEMENT_DYNCREATE(CISRModParam, CPropertyPage)

CISRModParam::CISRModParam() : CPropertyPage(CISRModParam::IDD)
{
}

CISRModParam::CISRModParam(
	UINT id, 
	CISO* pParent, 
	bool bCollectingParametersForNewISO ) 
	: CPropertyPage(id)
{
	m_bAllowFinish = false;
	m_pParent = pParent;
	m_bChange = false;
	m_datasetup = false;
	m_bCollectingParametersForNewISO = bCollectingParametersForNewISO;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	for (int i= IDC_ISRMODPARAMS_FIRST; i <= IDC_ISRMODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_ISRMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			ISR_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}
}

CISRModParam::~CISRModParam()
{
	delete m_pApplyButton;
}

BEGIN_MESSAGE_MAP(CISRModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CISRModParam)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ISRMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_HIGHVOLTL_SPIN, OnDeltaposModparamsHighvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_HIGHVOLTH_SPIN, OnDeltaposModparamsHighvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_BATVOLTL_SPIN, OnDeltaposModparamsBatvoltlSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_BATVOLTH_SPIN, OnDeltaposModparamsBatvolthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_FILEID_SPIN,   OnDeltaposModparamsFileidSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_M15H_SPIN, OnDeltaposModparamsSupM15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_M15L_SPIN, OnDeltaposModparamsSupM15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_P05H_SPIN, OnDeltaposModparamsSupP05hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_P05L_SPIN, OnDeltaposModparamsSupP05lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_P15H_SPIN, OnDeltaposModparamsSupP15hSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SUP_P15L_SPIN, OnDeltaposModparamsSupP15lSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_MAXBBM_SPIN, OnDeltaposModparamsMaxbbmSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_NODE_SPIN, OnDeltaposModparamsMostSpin)
	ON_CBN_SELCHANGE(IDC_ISRMODPARAMS_PORT, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SAVELOC, OnChangeModparamsSaveLoc)
	ON_EN_KILLFOCUS(IDC_ISRMODPARAMS_FILEID, OnKillfocusModparamsFileid)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_DATETIME, OnModparamsDatetime)
	ON_BN_CLICKED(IDC_CHECK_ALLOWAUTODATE, OnCheckAllowautodate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_MAXSTATUS_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_MAXINQUIRE_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_MAXCYCLE_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_LOGFILTER, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_FILEID, OnChangeModparamsMost)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_DODUMP, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_COMMFAIL_SPIN, OnDeltaposModparamsMostSpin)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_LOAD_DEFAULT_VALUES, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_ISRMODPARAMS_RESET_TO_LAST_APPLIED, OnModparamsReset)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_TIMEOUT_SPIN, OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_TIMEOUT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_TIME_ERR_SPIN, OnDeltaposModparamsMostSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ISRMODPARAMS_SLOW_RESET_SPIN,OnDeltaposModparamsMostSpin)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_TIME_ERR, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SLOW_RESET,OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_P15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_P15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_P05L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_P05H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_M15L, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_SUP_M15H, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_NODE,		OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_MAXSTATUS, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_MAXINQUIRE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_MAXCYCLE, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_MAXBBM,	OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_COMMFAIL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_BATVOLTL, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_BATVOLTH, OnChangeModparamsMost)
	ON_EN_CHANGE(IDC_ISRMODPARAMS_PORT,		OnChangeModparamsPort)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, OnDatetimechangeDatetimepicker1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CISRModParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISRModParam)
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_cDateTimePicker);
	//}}AFX_DATA_MAP
}

void CISRModParam::DoApply()
{
	char *Name = m_pParent->m_pName;
	char Temp[_MAX_PATH];
	//get all the stuff and save it

	bool bCarryOn;

	if (m_bCollectingParametersForNewISO)
	{
		bCarryOn = true;
	}
	else
	{
		bCarryOn = m_pParent->Validate();
	}

	if (bCarryOn)
	{
		SaveItem(Name,"HIGHVOLTH",	IDC_ISRMODPARAMS_HIGHVOLTH);
		SaveItem(Name,"HIGHVOLTL",	IDC_ISRMODPARAMS_HIGHVOLTL);
		SaveItem(Name,"BATVOLTH",	IDC_ISRMODPARAMS_BATVOLTH);
		SaveItem(Name,"BATVOLTL",	IDC_ISRMODPARAMS_BATVOLTL);
		SaveItem(Name,"SUP_P15H",	IDC_ISRMODPARAMS_SUP_P15H);
		SaveItem(Name,"SUP_P15L",	IDC_ISRMODPARAMS_SUP_P15L);
		SaveItem(Name,"SUP_M15H",	IDC_ISRMODPARAMS_SUP_M15H);
		SaveItem(Name,"SUP_M15L",	IDC_ISRMODPARAMS_SUP_M15L);	
		SaveItem(Name,"SUP_P05H",	IDC_ISRMODPARAMS_SUP_P05H);
		SaveItem(Name,"SUP_P05L",	IDC_ISRMODPARAMS_SUP_P05L);
		SaveItem(Name,"TIME_ERR",	IDC_ISRMODPARAMS_TIME_ERR);
		SaveItem(Name,"SLOW_RESET",	IDC_ISRMODPARAMS_SLOW_RESET);
		SaveItem(Name,"NODE",		IDC_ISRMODPARAMS_NODE);
		SaveItem(Name,"MAXCYCLE",	IDC_ISRMODPARAMS_MAXCYCLE);
		SaveItem(Name,"MAXPAUSE",	IDC_ISRMODPARAMS_MAXPAUSE);
		SaveItem(Name,"DO_DUMP",	IDC_ISRMODPARAMS_DODUMP,	true);
		SaveItem(Name,"AUTOTIMESET",IDC_CHECK_ALLOWAUTODATE,	true);
		SaveItem(Name,"LOG_FILTER",	IDC_ISRMODPARAMS_LOGFILTER, true);
	//	SaveItem(Name,"LOG_MII",	IDC_ISRMODPARAMS_LOGMII,	true);
		SaveItem(Name,"PORT",		IDC_ISRMODPARAMS_PORT);
		SaveItem(Name,"MAXINQUIRE",	IDC_ISRMODPARAMS_MAXINQUIRE);
		SaveItem(Name,"MAXSTATUS",	IDC_ISRMODPARAMS_MAXSTATUS);
		SaveItem(Name,"COMMFAIL",	IDC_ISRMODPARAMS_COMMFAIL);
		SaveItem(Name,"TIMEOUT",	IDC_ISRMODPARAMS_TIMEOUT);
		SaveItem(Name,"MAXBBM",		IDC_ISRMODPARAMS_MAXBBM);
		SaveItem(Name,FILE_ID,	IDC_ISRMODPARAMS_FILEID);

		m_cDateTimePicker.GetWindowText(Temp,sizeof(Temp));
		CTime timeTime;
		DWORD dwResult = m_cDateTimePicker.GetTime(timeTime);
		WritePrivateProfileString(Name,"AUTOTIMESETDAILY",(dwResult & 0x00000001)?"0":"1",m_pParent->m_szIniFile);
		WritePrivateProfileString(Name,"AUTOTIMESETDAILYAT",Temp,m_pParent->m_szIniFile);

	
		GetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,Temp,sizeof(Temp));
		while ((Temp[strlen(Temp)-1]=='\\')||
			   (Temp[strlen(Temp)-1]=='/' )  ) 
		{
			Temp[strlen(Temp)-1] = NULL;
			SetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,Temp);
		}
		SaveItem(Name,SAVE_LOC,	IDC_ISRMODPARAMS_SAVELOC);
		if (_access(Temp,0)==-1)
		{
			_mkdir(Temp);
			strcat(Temp,"\\archive");
			_mkdir(Temp);
		}

		if (m_bCollectingParametersForNewISO)
		{
			WritePrivateProfileString(Name,"FLAGCOMM","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGOTHR","0",m_pParent->m_szIniFile);
			WritePrivateProfileString(Name,"FLAGTIME","0",m_pParent->m_szIniFile);
		}

		//propagate all the new values to the program
		if (!m_bCollectingParametersForNewISO)
			m_pParent->UpdateParams(true);

	}//end bCarryOn
}

void CISRModParam::OnModparamsApply() 
{
	if (m_pPS)
		m_pPS->SetToClose(0);

	DoApply();  //this does the validation
	m_bChange = false;
	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CISRModParam::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	//The needed function OnClose is virtual in both the CTPropertySheet
	//and in CISRPropertySheet.  So one or the other gets called.
	m_pPS = (CTPropertySheet*)pPS;
}

void CISRModParam::OnModparamsReset() 
{
	if (m_pPS)m_pPS->SetToClose(0);

	m_datasetup = true;
	char *name = m_pParent->m_pName;

	if (m_bCollectingParametersForNewISO)
	{
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_ISRMODPARAMS_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ISRMODPARAMS_NODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
	}

	ResetItem(name,FILE_ID, FILEID_SPIN_DEF,
		IDC_ISRMODPARAMS_FILEID_SPIN,IDC_ISRMODPARAMS_FILEID);

	ResetItem(name, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_ISRMODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_ISRMODPARAMS_BATVOLTH);

	ResetItem(name, "BATVOLTL", BATVOLTL_SPIN_DEF,
		IDC_ISRMODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
		IDC_ISRMODPARAMS_BATVOLTL);

	ResetItem(name,"SUP_P15H",SUP_P15H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P15H);

	ResetItem(name,"SUP_P15L",SUP_P15L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P15L);

	ResetItem(name,"SUP_M15H",SUP_M15H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_M15H);

	ResetItem(name,"SUP_M15L",SUP_M15L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_M15L);	

	ResetItem(name,"SUP_P05H",SUP_P05H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P05H);

	ResetItem(name,"SUP_P05L",SUP_P05L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P05L);

	ResetItem(name,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_ISRMODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(name,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_ISRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(name,"NODE",NODE_SPIN_DEF,
		IDC_ISRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
	
	ResetItem(name,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(name,"DO_DUMP", DODUMP_DEF, IDC_ISRMODPARAMS_DODUMP);

	ResetItem(name,"LOG_FILTER", LOGFILTER_DEF, IDC_ISRMODPARAMS_LOGFILTER);

//	ResetItem(name,"LOG_MII", LOGMII_DEF, IDC_ISRMODPARAMS_LOGMII);
	ResetItem(name,"AUTOTIMESET", AUTOTIME_DEF, IDC_CHECK_ALLOWAUTODATE);

	char szTemp[16];
	GetPrivateProfileString(name,"AUTOTIMESETDAILYAT","04:30:30 AM",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	int iHour,iMinute,iSecond;
	char cAMPM;
	sscanf(szTemp,"%d:%d:%d %c",&iHour,&iMinute,&iSecond,&cAMPM);
	if (cAMPM == 'P') iHour += 12;
	CTime cTime(2000,1,1,iHour,iMinute,iSecond);
	m_cDateTimePicker.SetTime(&cTime);
	bool bDoAuto = GetPrivateProfileInt(name,"AUTOTIMESETDAILY",0,m_pParent->m_szIniFile)==1;
	if (!bDoAuto)
		m_cDateTimePicker.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE,0);

	ResetItem(name,SAVE_LOC, SAVELOC_DEF, IDC_ISRMODPARAMS_SAVELOC);

	char buff[16384];
	char buff2[64];
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_pParent->m_szIniFile)>0)
	{
		char *at = buff;
		int sel = ((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->GetCurSel();
		((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->ResetContent();

		m_bAllowFinish = false;
		
		while (strlen(at) > 0)
		{
			strcpy(buff2,at);
			strtok(buff2,"=");
			((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->AddString(buff2);
			m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}

		((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->SetCurSel(sel);
	}

	ResetItem(name,"PORT", PORT_DEF, IDC_ISRMODPARAMS_PORT);

	ResetItem(name,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(name,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(name,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_ISRMODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(name,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_ISRMODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(name,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,IDC_ISRMODPARAMS_MAXPAUSE);

	ResetItemI(name,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,IDC_ISRMODPARAMS_MAXBBM);

	ResetItemI(name,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_ISRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_ISRMODPARAMS_HIGHVOLTH);

	ResetItemI(name,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_ISRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_ISRMODPARAMS_HIGHVOLTL);

	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

void CISRModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CISRModParam::OnModparamsDefault(bool SetPort) 
{
	if (m_pPS)m_pPS->SetToClose(0);
	m_datasetup = true;

	ResetItem(ISR_DEFAULT, FILE_ID, FILEID_SPIN_DEF,  
		IDC_ISRMODPARAMS_FILEID_SPIN, IDC_ISRMODPARAMS_FILEID);

	ResetItem(ISR_DEFAULT, "BATVOLTH", BATVOLTH_SPIN_DEF,  
		IDC_ISRMODPARAMS_BATVOLTH_SPIN, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX,
		IDC_ISRMODPARAMS_BATVOLTH);

	ResetItem(ISR_DEFAULT, "BATVOLTL", BATVOLTL_SPIN_DEF,
		IDC_ISRMODPARAMS_BATVOLTL_SPIN, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX,
		IDC_ISRMODPARAMS_BATVOLTL);

	ResetItem(ISR_DEFAULT,"SUP_P15H",SUP_P15H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P15H_SPIN,SUP_P15H_SPIN_MIN,SUP_P15H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P15H);

	ResetItem(ISR_DEFAULT,"SUP_P15L",SUP_P15L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P15L_SPIN,SUP_P15L_SPIN_MIN,SUP_P15L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P15L);

	ResetItem(ISR_DEFAULT,"SUP_M15H",SUP_M15H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_M15H_SPIN,SUP_M15H_SPIN_MIN,SUP_M15H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_M15H);

	ResetItem(ISR_DEFAULT,"SUP_M15L",SUP_M15L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_M15L_SPIN,SUP_M15L_SPIN_MIN,SUP_M15L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_M15L);	

	ResetItem(ISR_DEFAULT,"SUP_P05H",SUP_P05H_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P05H_SPIN,SUP_P05H_SPIN_MIN,SUP_P05H_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P05H);

	ResetItem(ISR_DEFAULT,"SUP_P05L",SUP_P05L_SPIN_DEF,
		IDC_ISRMODPARAMS_SUP_P05L_SPIN,SUP_P05L_SPIN_MIN,SUP_P05L_SPIN_MAX,
		IDC_ISRMODPARAMS_SUP_P05L);

	ResetItem(ISR_DEFAULT,"TIME_ERR",TIME_ERR_SPIN_DEF,
		IDC_ISRMODPARAMS_TIME_ERR_SPIN,TIME_ERR_SPIN_MIN,TIME_ERR_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"SLOW_RESET",SLOW_RESET_SPIN_DEF,
		IDC_ISRMODPARAMS_SLOW_RESET_SPIN,SLOW_RESET_SPIN_MIN,SLOW_RESET_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"MAXCYCLE",MAXCYCLE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXCYCLE_SPIN,MAXCYCLE_SPIN_MIN,MAXCYCLE_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"DO_DUMP",DODUMP_DEF,IDC_ISRMODPARAMS_DODUMP);

	ResetItem(ISR_DEFAULT,"LOG_FILTER",LOGFILTER_DEF,IDC_ISRMODPARAMS_LOGFILTER);

//	ResetItem(ISR_DEFAULT,"LOG_MII",LOGMII_DEF,IDC_ISRMODPARAMS_LOGMII);
	ResetItem(ISR_DEFAULT,"AUTOTIMESET",AUTOTIME_DEF,IDC_CHECK_ALLOWAUTODATE);

	char szTemp[16];
	GetPrivateProfileString(ISR_DEFAULT,"AUTOTIMESETDAILYAT","04:30:30 AM",szTemp,sizeof(szTemp),m_pParent->m_szIniFile);
	int iHour,iMinute,iSecond;
	char cAMPM;
	sscanf(szTemp,"%d:%d:%d %c",&iHour,&iMinute,&iSecond,&cAMPM);
	if (cAMPM == 'P') iHour += 12;
	CTime cTime(2000,1,1,iHour,iMinute,iSecond);
	m_cDateTimePicker.SetTime(&cTime);
	bool bDoAuto = GetPrivateProfileInt(ISR_DEFAULT,"AUTOTIMESETDAILY",0,m_pParent->m_szIniFile)==1;
	if (!bDoAuto)
		m_cDateTimePicker.SendMessage(DTM_SETSYSTEMTIME,GDT_NONE,0);

	ResetItem(ISR_DEFAULT,SAVE_LOC,SAVELOC_DEF,IDC_ISRMODPARAMS_SAVELOC);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
		ResetItem(ISR_DEFAULT,"NODE",NODE_SPIN_DEF,
			IDC_ISRMODPARAMS_NODE_SPIN,NODE_SPIN_MIN,NODE_SPIN_MAX);
		
		char buff[16384];
		char buff2[32];
		if (GetPrivateProfileSection(COMMUNICATIONS,buff,
			sizeof(buff), m_pParent->m_szIniFile)>0)
		{
			char *at = buff;
			((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->ResetContent();
			m_bAllowFinish = false;			
			while (strlen(at) > 0)
			{
				strcpy(buff2,at);
				strtok(buff2,"=");
				((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->AddString(buff2);
				m_bAllowFinish = true;
				at = at + strlen(at)+1;
			}
		}
		((CComboBox*)GetDlgItem(IDC_ISRMODPARAMS_PORT))->SetCurSel(0);
	
		ResetItem(ISR_DEFAULT,"PORT",PORT_DEF,IDC_ISRMODPARAMS_PORT);
	}

	ResetItem(ISR_DEFAULT,"MAXINQUIRE",MAXINQUIRE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXINQUIRE_SPIN,MAXINQUIRE_SPIN_MIN,MAXINQUIRE_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"MAXSTATUS",MAXSTATUS_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXSTATUS_SPIN,MAXSTATUS_SPIN_MIN,MAXSTATUS_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"COMMFAIL",COMMFAIL_SPIN_DEF,
		IDC_ISRMODPARAMS_COMMFAIL_SPIN,COMMFAIL_SPIN_MIN,COMMFAIL_SPIN_MAX);

	ResetItem(ISR_DEFAULT,"TIMEOUT",TIMEOUT_SPIN_DEF,
		IDC_ISRMODPARAMS_TIMEOUT_SPIN,TIMEOUT_SPIN_MIN,TIMEOUT_SPIN_MAX);

	ResetItemI(ISR_DEFAULT,"MAXPAUSE",MAXPAUSE_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXPAUSE_SPIN,MAXPAUSE_SPIN_MIN,MAXPAUSE_SPIN_MAX,
		IDC_ISRMODPARAMS_MAXPAUSE);

	ResetItemI(ISR_DEFAULT,"MAXBBM",MAXBBM_SPIN_DEF,
		IDC_ISRMODPARAMS_MAXBBM_SPIN,MAXBBM_SPIN_MIN,MAXBBM_SPIN_MAX,
		IDC_ISRMODPARAMS_MAXBBM);

	ResetItemI(ISR_DEFAULT,"HIGHVOLTH",HIGHVOLTH_SPIN_DEF,
		IDC_ISRMODPARAMS_HIGHVOLTH_SPIN,HIGHVOLTH_SPIN_MIN,HIGHVOLTH_SPIN_MAX,
		IDC_ISRMODPARAMS_HIGHVOLTH);

	ResetItemI(ISR_DEFAULT,"HIGHVOLTL",HIGHVOLTL_SPIN_DEF,
		IDC_ISRMODPARAMS_HIGHVOLTL_SPIN,HIGHVOLTL_SPIN_MIN,HIGHVOLTL_SPIN_MAX,
		IDC_ISRMODPARAMS_HIGHVOLTL);
}

BOOL CISRModParam::OnSetActive() 
{
	BOOL result = CPropertyPage::OnSetActive();

	if (m_pPS)
		m_pPS->SetToClose(0);

	if (m_bCollectingParametersForNewISO)
	{
		SetDlgItemText(IDC_ISRMODPARAMS_STATICTEXT,"");

		if (m_pApplyButton)
			m_pApplyButton->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_ISRMODPARAMS_RESET_TO_LAST_APPLIED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->ShowWindow(SW_HIDE);

		if (!m_datasetup) 
		{
			OnModparamsDefault();

		    if (m_pPS)
			{
				CString sCaption(m_pParent->m_pName);
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
		SetDlgItemText(IDC_ISRMODPARAMS_STATICTEXT,"Changes will NOT take effect until you click on \"Apply\".");
		GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->ShowWindow(SW_SHOW);
		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange?TRUE:FALSE);
	}
	return result;
}

void CISRModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS)m_pPS->SetToClose(0);
//	CWnd* pWnd;
//	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
//	{
//		UINT index = pWnd->GetDlgCtrlID();
//		if ((index >= IDC_ISRMODPARAMS_FIRST) && (index <= IDC_ISRMODPARAMS_LAST))
//		{
//			MessageBox("Got One",MB_OK);
//		}
//	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CISRModParam::OnWizardFinish() 
{
//	TRACE("CISRModParam::OnWizardFinish\n");

//	ASSERT(m_bCollectingParametersForNewISO);

	//Put the name of the instrument in the ini file
	WritePrivateProfileString(
		INSTRUMENTS,m_pParent->m_pName,ISR_DISCOVERY_TYPE,m_pParent->m_szIniFile);
	//Put the parameters for the instrument into the INI file
	DoApply();  //This does the validate

	return CPropertyPage::OnWizardFinish();
}

BOOL CISRModParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_ISRMODPARAMS_APPLY, 
		m_pParent->m_szIniFile,
		this);
	
	return TRUE;
}

void CISRModParam::WriteIniFileDefaultSection()
{
//	TRACE("CISRModParam::WriteIniFileDefaultSection()\n");

	char *inifile = m_pParent->m_szIniFile;

	WritePrivateProfileSection(ISR_DEFAULT,"",inifile);

	WritePrivateProfileString(ISR_DEFAULT,"HIGHVOLTH", "1800",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"HIGHVOLTL", "1600",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SLOW_RESET", "900",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"BATVOLTH", "30.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"BATVOLTL", "10.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_P15H", "20.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_P15L", "10.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_M15H", "-10.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_M15L","-20.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_P05H", "6.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SUP_P05L", "4.0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"TIME_ERR", "60",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"SLOW_RESET", "900",inifile);
	WritePrivateProfileString(ISR_DEFAULT,NODE, "-1",inifile);
	WritePrivateProfileString(ISR_DEFAULT,PORT, "",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"FLAGCOMM", "0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"FLAGOTHR", "0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"FLAGTIME","0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"MAXCYCLE","1000",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"MAXPAUSE","600",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"DO_DUMP","0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"LOG_FILTER","1",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"LOG_MII","1",inifile);
	WritePrivateProfileString(ISR_DEFAULT,SAVE_LOC,"C:\\DATA\\ISR01",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"MAXINQUIRE","10",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"MAXSTATUS","0",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"COMMFAIL","5",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"TIMEOUT","5",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"MAXBBM","1000",inifile);
	WritePrivateProfileString(ISR_DEFAULT,FILE_ID,"01",inifile);
	WritePrivateProfileString(ISR_DEFAULT,"AUTOTIMESET","1",inifile);

	WritePrivateProfileString(ISR_DEFAULT,"AUTOTIMESETDAILY",AUTOTIMEDAILY_DEF,inifile);
	WritePrivateProfileString(ISR_DEFAULT,"AUTOTIMESETDAILYAT",AUTOTIMEDAILYAT_DEF,inifile);
}


//reset float item for a spinner with a non-automatic buddy
void CISRModParam::ResetItem(char *szName,char *szItem,char*szDefault,int dItem,int dMin, int dMax, int dTextItem)
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
void CISRModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
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
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	_itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));
}

//reset item for a spinner with an automatic buddy
void CISRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CISRModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CISRModParam::ResetItem(char *szName,char *szItem, char *szDefault, int dItem)
{
	char temp[MAX_PATH+1];
//	int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
	case IDC_ISRMODPARAMS_PORT:
		((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
		break;
	default:; 
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}

void CISRModParam::ResetItem(char *szName, char *szItem, char *szDefault,
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


void CISRModParam::DoSpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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

void CISRModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
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

void CISRModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CISRModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS) m_pPS->SetToClose(0);
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


void CISRModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
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
	GetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,Text);

//	if "ISR" at end then...
	Text.MakeReverse();
	switch (Text.Find("RSI"))
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
				Text += "ISR";
			else
				Text += "\\ISR";
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
				Text += "ISR";
			else
				Text += "\\ISR";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "ISR";
			else
				Text += "\\ISR";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,Text);
}

void CISRModParam::OnDeltaposModparamsHighvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTL_SPIN_MIN, HIGHVOLTL_SPIN_MAX, 
		IDC_ISRMODPARAMS_HIGHVOLTL, IDC_ISRMODPARAMS_HIGHVOLTH,IDC_ISRMODPARAMS_HIGHVOLTH_SPIN, true);
}

void CISRModParam::OnDeltaposModparamsHighvolthSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	DoBuddySpinI(pNMHDR, pResult, HIGHVOLTH_SPIN_MIN, HIGHVOLTH_SPIN_MAX, 
		IDC_ISRMODPARAMS_HIGHVOLTH, IDC_ISRMODPARAMS_HIGHVOLTL, IDC_ISRMODPARAMS_HIGHVOLTL_SPIN, false);
}

void CISRModParam::OnDeltaposModparamsBatvolthSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTH_SPIN_MIN, BATVOLTH_SPIN_MAX, 
		IDC_ISRMODPARAMS_BATVOLTH, IDC_ISRMODPARAMS_BATVOLTL,IDC_ISRMODPARAMS_BATVOLTL_SPIN, false);
}

void CISRModParam::OnDeltaposModparamsBatvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, BATVOLTL_SPIN_MIN, BATVOLTL_SPIN_MAX, 
		IDC_ISRMODPARAMS_BATVOLTL, IDC_ISRMODPARAMS_BATVOLTH,IDC_ISRMODPARAMS_BATVOLTH_SPIN, true);
}

void CISRModParam::OnDeltaposModparamsSupM15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15H_SPIN_MIN, SUP_M15H_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_M15H, IDC_ISRMODPARAMS_SUP_M15L, IDC_ISRMODPARAMS_SUP_M15L_SPIN, false);
}

void CISRModParam::OnDeltaposModparamsSupM15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_M15L_SPIN_MIN, SUP_M15L_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_M15L, IDC_ISRMODPARAMS_SUP_M15H, IDC_ISRMODPARAMS_SUP_M15H_SPIN,true);
}

void CISRModParam::OnDeltaposModparamsSupP05hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05H_SPIN_MIN, SUP_P05H_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_P05H, IDC_ISRMODPARAMS_SUP_P05L, IDC_ISRMODPARAMS_SUP_P05L_SPIN, false);
}

void CISRModParam::OnDeltaposModparamsSupP05lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P05L_SPIN_MIN, SUP_P05L_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_P05L, IDC_ISRMODPARAMS_SUP_P05H, IDC_ISRMODPARAMS_SUP_P05H_SPIN, true );
}

void CISRModParam::OnDeltaposModparamsSupP15hSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15H_SPIN_MIN, SUP_P15H_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_P15H, IDC_ISRMODPARAMS_SUP_P15L, IDC_ISRMODPARAMS_SUP_P15L_SPIN, false);
}

void CISRModParam::OnDeltaposModparamsSupP15lSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	DoBuddySpin(pNMHDR, pResult, SUP_P15L_SPIN_MIN, SUP_P15L_SPIN_MAX, 
		IDC_ISRMODPARAMS_SUP_P15L, IDC_ISRMODPARAMS_SUP_P15H, IDC_ISRMODPARAMS_SUP_P15H_SPIN, true);
}

void CISRModParam::OnDeltaposModparamsMaxbbmSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXBBM_SPIN_MIN, MAXBBM_SPIN_MAX, IDC_ISRMODPARAMS_MAXBBM);
}

void CISRModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);
	DoSpinI(pNMHDR, pResult, MAXPAUSE_SPIN_MIN, MAXPAUSE_SPIN_MAX, IDC_ISRMODPARAMS_MAXPAUSE);
}

void CISRModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_pPS)m_pPS->SetToClose(0);
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, IDC_ISRMODPARAMS_FILEID);
}

void CISRModParam::OnModparamsBrowse() 
{
	if (m_pPS)m_pPS->SetToClose(0);
	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_ISRMODPARAMS_SAVELOC)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_ISRMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("ISR"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "ISR";
			else
				Text += "\\ISR";
			Text += Id;
		}
		
		SetDlgItemText(IDC_ISRMODPARAMS_SAVELOC, Text);
	}
}


void CISRModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if (m_pPS)m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CISRModParam::OnChangeModparamsMost()
{
	if (m_pPS)
		m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CISRModParam::OnChangeModparamsPort()
{
//	ASSERT(m_bCollectingParametersForNewISO);
	if(m_pPS)m_pPS->SetToClose(0);
}

void CISRModParam::OnChangeModparamsFileID()
{
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CISRModParam::OnChangeModparamsSaveLoc() 
{
	CString Text;
	GetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}
	if (m_pPS) m_pPS->SetToClose(0);
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CISRModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_ISRMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_ISRMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_ISRMODPARAMS_FILEID))->SetWindowText(temp);
}



void CISRModParam::OnModparamsDatetime() 
{
	if (m_pPS)
	{
		m_pPS->SetToClose(0);
		m_pPS->ForceDateTime();
	}	
}

void CISRModParam::OnCheckAllowautodate() 
{
	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

void CISRModParam::SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
}

void CISRModParam::OnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult) 
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
