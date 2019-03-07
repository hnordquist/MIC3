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
// Config.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "mic_definitions.h"
#include "globals.h"
#include "resource.h"
#include "validate.h"
#include "config.h"
#include "access.h"
#include "micmgrdlg.h"
#include "deleteins.h"
#include "TimedMessageDialog.h"
#include "NewInst.h"
#include "NewComm.h"
#include "DeleteCom.h"
#include "DirDialog.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog

CConfig::CConfig(CWnd* pParent)
	: CDialog(CConfig::IDD, pParent),
	m_pParent(pParent)
{
	m_bApplied = false;      //from MIC 1.9
	m_bChanged = false;

	m_csTitle = CString(MIC_NAMELONG) + CString(": Configure");
	m_pApplyButton = NULL;
}
CConfig::~CConfig()
{
	delete m_pApplyButton;
}
void CConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig)
	DDX_Control(pDX, IDC_CHECK_USESHORT, m_cCheckUseShort);
	DDX_Control(pDX, IDC_EDIT_LONGNAME, m_cEditLongName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfig, CDialog)
	//{{AFX_MSG_MAP(CConfig)
	ON_BN_CLICKED(IDC_APPLY_BUTTON, OnApplyButton)
	ON_EN_CHANGE(IDC_FACILITYNAME_EDIT, OnChangeFacilitynameEdit)
	ON_BN_CLICKED(IDC_ADDINST_BUTTON, OnAddinstButton)
	ON_BN_CLICKED(IDC_DELINST_BUTTON, OnDelinstButton)
	ON_BN_CLICKED(IDC_ADDCOMM_BUTTON, OnAddcommButton)
	ON_BN_CLICKED(IDC_DELCOMM_BUTTON, OnDelcommButton)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BROWSEDEST_BUTTON, OnBrowsedestButton)
	ON_BN_CLICKED(IDC_CHECK_USESHORT, OnCheckUseshort)
	ON_EN_CHANGE(IDC_EDIT_LONGNAME, OnChangeEditLongname)
	ON_BN_CLICKED(IDC_HIDEWINDOWS_CHECK, OnChangeMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_COLUMN_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DAYSTOKEEPDATA_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DAYSBEFOREDELFILES_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_MAXREPETITION_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DIALOG_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHT_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_WIDTH_SPIN, OnSpinMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_FLAG_SPIN, OnSpinMost)
	ON_BN_CLICKED(IDC_LOGARCHIVEACT_CHECK, OnChangeMost)
	ON_BN_CLICKED(IDC_NOPAUSERESTART_CHECK, OnChangeMost)
	ON_EN_CHANGE(IDC_MAXPERCENTDISKUSE_STATIC, OnChangeMost)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig message handlers
void CConfig::OnSpinMost(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	Changed(true);	
	*pResult = 0;
}
void CConfig::OnChangeMost() 
{
	Changed(true);
}

void CConfig::OnApplyButton() 
{
//	ASSERT(m_bChanged);
	char buff[256];

	CString cs_long_format;
	m_cEditLongName.GetWindowText(cs_long_format);
	if ((m_cCheckUseShort.GetCheck() == 0) && (cs_long_format.IsEmpty()))
	{
		char title[] = {"MIC Configuration"};
		char message[] = {"Please enter long form for filenames.\n\tOR\nCheck User Short Form box."};
		new CTimedMessageDialog(title,message,g_iDlgCloseMillisec/1000,m_pParent);
		return;
	}

	CValidate Valid(this);

	if (g_bHideWindows)  //hide if minimized
		ShowWindow(SW_HIDE);

	//verify authorization and do
	if (Valid.DoModal() == IDOK) 
	{
		m_bApplied = true;
		//get the maximum number of repetitions for an entry in pfm
		g_iMaxRepetition = GetPrivateProfileInt(CONFIGURATION,MAX_REP,0,g_szIniFile);
		if (g_iMaxRepetition < 2) 
		{
			g_iMaxRepetition = 10;
			WritePrivateProfileString(CONFIGURATION,MAX_REP,"10",g_szIniFile);
		}
		//get the default location to copy to
		GetDlgItemText(IDC_DESTINATIONNAME_EDIT,buff,sizeof(buff));
		//is destination a directory
		DWORD result = GetFileAttributes(buff);
		if ((result & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			CString csText("Invalid Destination.  Changes Not Applied");
			new CTimedMessageDialog(m_csTitle, csText,1000*g_iDlgCloseMillisec,this);
			return;
		}

		//save the destination in ini file
		WritePrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,buff,g_szIniFile);

		//set facility name in the ini file
		GetDlgItem(IDC_FACILITYNAME_EDIT)->GetWindowText(buff,sizeof(buff));
		strcpy(g_sFacilityName,buff);
	    WritePrivateProfileString(CONFIGURATION,FACILITYNAME,g_sFacilityName,g_szIniFile);

		//Set the text in the title bar of the main MIC dialog
		m_pParent->SetWindowText(
			CString(g_sFacilityName) +
			CString(" - ") +
			CString(MIC_NAMESHORT) + 
			CString(" - ") + 
			g_csVersion);

		//set hide windows in global and ini
		if(((CButton*)GetDlgItem(IDC_HIDEWINDOWS_CHECK))->GetCheck())
			g_bHideWindows = true;
		else
			g_bHideWindows = false;
		WritePrivateProfileString(CONFIGURATION,HIDEWINDOWS,g_bHideWindows?"1":"0",g_szIniFile);

		if (m_cCheckUseShort.GetCheck() == 1)
		{
			g_bUseShortName = true;
		}
		else
		{
			g_bUseShortName = false;
			m_cEditLongName.GetWindowText(g_csLongName);
			WritePrivateProfileString(CONFIGURATION,LONGNAMEFORM,g_csLongName,g_szIniFile);
		}

		WritePrivateProfileString(CONFIGURATION,DOSHORTNAME,g_bUseShortName?"Yes":"No",g_szIniFile);

		if(((CButton*)GetDlgItem(IDC_NOPAUSERESTART_CHECK))->GetCheck())
			g_bNoPauseRestart = true;
		else
			g_bNoPauseRestart = false;
		WritePrivateProfileString(CONFIGURATION,FASTSTARTUP,g_bNoPauseRestart?"1":"0",g_szIniFile);

		if(((CButton*)GetDlgItem(IDC_LOGARCHIVEACT_CHECK))->GetCheck())
			g_blogarchiveact = true;
		else
			g_blogarchiveact = false;
		WritePrivateProfileString(CONFIGURATION,LOGARCHIVEACT,g_blogarchiveact?"1":"0",g_szIniFile);

		//set button columns
		GetDlgItem(IDC_INSTCOLUMNS_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,BUTTONSWIDE,buff,g_szIniFile);
		g_iButtonColumns = atoi(buff);

		//set days to keep data
		//later modified to be minutes between archive (size and age of files) checks
		GetDlgItem(IDC_DAYSTOKEEPDATA_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,MINUTESTOCHECKARCHIVE,buff,g_szIniFile);
		g_iMinutesToCheckArchive = atoi(buff);

		//set days before deleting files
		GetDlgItem(IDC_DAYSBEFOREDELFILES_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,DAYSBEFOREDELFILES,buff,g_szIniFile);
		g_iDaysUntilDeleteFiles = atoi(buff);

		//set max percent of disk use
		GetDlgItem(IDC_MAXPERCENTDISKUSE_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,MAXPERCENTDISKUSE,buff,g_szIniFile);
		g_fMaxPercentDiskUse = (float)atof(buff);

		//set the maximum repetitions of selected messages
		GetDlgItem(IDC_MAXREPETITION_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,"MAX_REP",buff,g_szIniFile);
		g_iMaxRepetition = atoi(buff);
		
		//set instrument button width
		GetDlgItem(IDC_WIDTH)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,BUTTONWIDTH,buff,g_szIniFile);
		g_iButtonWidth = atoi(buff);

		//set instrument button height
		GetDlgItem(IDC_HEIGHT)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,BUTTONHEIGHT,buff,g_szIniFile);
		g_iButtonHeight = atoi(buff);

		//set instrument button flag size
		GetDlgItem(IDC_FLAG)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,BUTTONFLAGSIZE,buff,g_szIniFile);
		g_iButtonFlagSize = atoi(buff);

		//reorganize the maindialog display
		((CMicMgrDlg*)m_pParent)->ReorgDialogLayout();

		//set time to keep dialog windows open in seconds
		GetDlgItem(IDC_DIALOG_STATIC)->GetWindowText(buff,sizeof(buff));
		WritePrivateProfileString(CONFIGURATION,DIALOGCLOSE,buff,g_szIniFile);
		g_iDlgCloseMillisec = atoi(buff)*1000;
		KillTimer(1);
		SetTimer(1,g_iDlgCloseMillisec,NULL);

		//tell the instruments about the changes relevant to ISO's

		ULONG ul_new_auto_close_time = (ULONG)(g_iDlgCloseMillisec/1000);
		BSTR bstr_new_long_format_string;
		Convert_CString_to_BSTR(g_csLongName,bstr_new_long_format_string);
		VARIANT_BOOL vb_new_use_short_filename = 
			g_bUseShortName?VARIANT_TRUE:VARIANT_FALSE;
		ULONG ul_new_max_repetitions = (ULONG)g_iMaxRepetition;

		for (int i = 0; i < g_iInstCount; i++)
		{
			g_pIISO[i]->put_DialogAutoCloseTime(ul_new_auto_close_time);
			g_pIISO[i]->put_LongNameFormatString(bstr_new_long_format_string);
			g_pIISO[i]->UseShortFilename(vb_new_use_short_filename);
			g_pIISO[i]->put_MaximumRetryRepetitions(ul_new_max_repetitions);
		}

		Changed(false);
	}
	else
	{
		//Validate did not happen
		//Keep the changes and allow apply to happen
		Changed(true);
	}

	ShowWindow(SW_SHOW);
}

void CConfig::OnOK() 
{
	//if someting changed then apply change
	if (m_bChanged) 
		OnApplyButton();

	CDialog::OnOK();
}

void CConfig::Changed(bool change)
{
	m_bChanged = change;

	CString Text;
	GetDlgItemText(IDC_FACILITYNAME_EDIT,Text);
	bool bValidFacility = !Text.IsEmpty();

	if (m_pApplyButton)
	{
		if (bValidFacility && change)
			m_pApplyButton->EnableWindow(TRUE);
		else
			m_pApplyButton->EnableWindow(FALSE);
	}
}

BOOL CConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char buff[_MAX_PATH];
	
	//set facility window
	GetDlgItem(IDC_FACILITYNAME_EDIT)->SetWindowText(g_sFacilityName);
	((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->SetLimitText(32);
	
	//set check box for hide windows
	((CButton*)GetDlgItem(IDC_HIDEWINDOWS_CHECK))->SetCheck(g_bHideWindows);
	
	//set check box for log archive actions
	((CButton*)GetDlgItem(IDC_LOGARCHIVEACT_CHECK))->SetCheck(g_blogarchiveact);

	//set check box for nopauserestart
	((CButton*)GetDlgItem(IDC_NOPAUSERESTART_CHECK))->SetCheck(g_bNoPauseRestart);

	//set check box for use short name
	m_cCheckUseShort.SetCheck(g_bUseShortName?1:0);
	m_cEditLongName.EnableWindow(!g_bUseShortName);
	if (g_bUseShortName)
		GetDlgItem(IDC_STATIC_EXAMPLE)->SetWindowText("IIYMDHMS");
	else
		m_cEditLongName.SetWindowText(g_csLongName);

	//set initial buttonswide count
	sprintf(buff,"%d",g_iButtonColumns);
	GetDlgItem(IDC_INSTCOLUMNS_STATIC)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_COLUMN_SPIN))->SetRange(1,20);

	//set initial seconds before closing dialog box
	sprintf(buff,"%d",g_iDlgCloseMillisec/1000);
	GetDlgItem(IDC_DIALOG_STATIC)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_DIALOG_SPIN))->SetRange(10,999);

	//set initial maximum percent of disk use
	if (g_fMaxPercentDiskUse > 99.99f) 
		g_fMaxPercentDiskUse = 99.99f;
	sprintf(buff,"%5.2f",g_fMaxPercentDiskUse);
	GetDlgItem(IDC_MAXPERCENTDISKUSE_STATIC)->SetWindowText(buff);

	//set initial minutes to check archive
	sprintf(buff,"%d",g_iMinutesToCheckArchive);
	GetDlgItem(IDC_DAYSTOKEEPDATA_STATIC)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_DAYSTOKEEPDATA_SPIN))->SetRange(1,999);

	//set initial days before deleting files
	sprintf(buff,"%d",g_iDaysUntilDeleteFiles);
	GetDlgItem(IDC_DAYSBEFOREDELFILES_STATIC)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_DAYSBEFOREDELFILES_SPIN))->SetRange(1,999);

	sprintf(buff,"%d",g_iMaxRepetition);
	GetDlgItem(IDC_MAXREPETITION_STATIC)->SetWindowText(buff);
	((CSpinButtonCtrl*)GetDlgItem(IDC_MAXREPETITION_SPIN))->SetRange(2,9999);

	//set initial Width of instrument buttons
	sprintf(buff,"%d",g_iButtonWidth);
	GetDlgItem(IDC_WIDTH)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_WIDTH_SPIN))->SetRange(1,1999);

	//set initial Height of instrument buttons
	sprintf(buff,"%d",g_iButtonHeight);
	GetDlgItem(IDC_HEIGHT)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_HEIGHT_SPIN))->SetRange(1,1999);

	//set initial flag size of instrument buttons
	sprintf(buff,"%d",g_iButtonFlagSize);
	GetDlgItem(IDC_FLAG)->SetWindowText(buff);
	//set the range to something reasonable
	((CSpinButtonCtrl*)GetDlgItem(IDC_FLAG_SPIN))->SetRange(1,999);

	//set the default path to save files during copy
	GetPrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,DEFAULTCOPYPATH_DEFAULT,buff,sizeof(buff),g_szIniFile);
	GetDlgItem(IDC_DESTINATIONNAME_EDIT)->SetWindowText(buff);
//	((CEdit*)GetDlgItem(IDC_DESTINATIONNAME_EDIT))->SetLimitText(3);

	if (g_iInstCount <= 0 )
		GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(false);
	else
		GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(true);

	if (g_iCommCount <= 0 )
	{
		GetDlgItem(IDC_DELCOMM_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_ADDINST_BUTTON)->EnableWindow(false);
	}
	else
		GetDlgItem(IDC_DELCOMM_BUTTON)->EnableWindow(true);

	//clock to update the total bbm
	SetTimer(2,1000,NULL);

	//clock to auto close the dialog
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//Can't get here if we haven't added access information...
	//CAccess cAccess(this);
	//if (cAccess.CountUsers()==0)
	//{
	//	GetDlgItem(IDC_DELCOMM_BUTTON)->EnableWindow(false);
	//	GetDlgItem(IDC_ADDINST_BUTTON)->EnableWindow(false);
	//	GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(false);
	//	GetDlgItem(IDC_ADDCOMM_BUTTON)->EnableWindow(false);
	//}

	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_APPLY_BUTTON,
		g_szIniFile,
		this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//catch a change to the facility name edit control 
void CConfig::OnChangeFacilitynameEdit() 
{
	CString Text;
	GetDlgItemText(IDC_FACILITYNAME_EDIT,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDFILENAMECHARS) != -1)
		{
			((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_FACILITYNAME_EDIT))->SetSel(posStart,posEnd);
	}
	Changed(true);
}

//add instrument button selected
void CConfig::OnAddinstButton() 
{
//	TRACE("CConfig::OnAddinstButton()\n");

	//hide the current window
    if (g_bHideWindows)
	      ShowWindow(SW_HIDE);

	//create an instrument
	CNewInst NewInst(this);
	//kill the timer to dump this window
	KillTimer(1);
	//bring up the new dialog box
	NewInst.DoModal();

	if (g_iInstCount <= 0 )
		GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(false);
	else
		GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(true);
	//reset the timer to kill this window
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	//and show this window
	ShowWindow(SW_SHOW);
}

//delete instrument button selected
void CConfig::OnDelinstButton()
{
//	TRACE("CConfig::OnDelinstButton()\n");

	bool result;

	if (g_iInstCount > 0) 
	{
		CDeleteIns DeleteIns(this);
		if (g_bHideWindows)
	      ShowWindow(SW_HIDE);
		KillTimer(1);

/*
		//get all the instruments
		char string_out[64];
		for (index = 0; index< g_iInstCount; index++)
		{
			g_pIISO[index]->get_FullName(&bstr);
			cu.Convert_BSTR_to_Character_String(bstr,string_out,sizeof(string_out));
			deleteIns.AddString(string_out);
		}
*/

		if (DeleteIns.DoModal() == IDOK) 
		{ 
			if (g_iInstCount == 1)
				result = false;
			else
				result = true;
		}
		else
		{
			if (g_iInstCount <= 0 )
				result = false;
			else
				result = true; 
		}

		GetDlgItem(IDC_DELINST_BUTTON)->EnableWindow(result);

		SetTimer(1,g_iDlgCloseMillisec,NULL);
		ShowWindow(SW_SHOW);
	}
}

//add comm button selected
void CConfig::OnAddcommButton() 
{
	//CNewComm will send message to high level window to add
	//do the new comm dialog box
	if (g_bHideWindows)
		ShowWindow(SW_HIDE);
	KillTimer(1);
	CNewComm NewComm(this);
	if (NewComm.DoModal()==IDOK)
	{
		GetDlgItem(IDC_DELCOMM_BUTTON)->EnableWindow(true);
		GetDlgItem(IDC_ADDINST_BUTTON)->EnableWindow(true);
	}
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	ShowWindow(SW_SHOW);
}

//delete comm button selected
void CConfig::OnDelcommButton() 
{
	//CDelComm will send message to high level window to delete

	//do the delete comm dialog box
	bool result;
	if (g_iCommCount > 0)
	{
		CDeleteCom DeleteCom(this);
		if (g_bHideWindows)
			ShowWindow(SW_HIDE);
		KillTimer(1);
		if (DeleteCom.DoModal()==IDOK)
		{
			if (g_iCommCount == 1 )
				result = false;
			else
				result = true;
		}
		else
		{
			if (g_iCommCount <= 0 )
				result = false;
			else
				result = true; 
		}
		GetDlgItem(IDC_DELCOMM_BUTTON)->EnableWindow(result);
		GetDlgItem(IDC_ADDINST_BUTTON)->EnableWindow(result);

		SetTimer(1,g_iDlgCloseMillisec,NULL);
		ShowWindow(SW_SHOW);
	}
}

//catch timer messages
void CConfig::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 2)
	{
		ULONG ulong;
		int iTotal = 0;
		for (int i = 0; i < g_iInstCount; i++)
		{
			g_pIISO[i]->get_CurrentBBM(&ulong);
			iTotal += (int)ulong;
		}
		SetDlgItemInt(IDC_STATIC_BBMTOTAL,iTotal);
	}

	//if it is the timeout timer then go cancel this dialog
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

//catch browse for destination filename button
void CConfig::OnBrowsedestButton() 
{
	CDirDialog DirDialog(this);
	char temp[_MAX_DIR+1];
	GetDlgItem(IDC_DESTINATIONNAME_EDIT)->GetWindowText(temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Default Destination for Copy:";
	if (g_bHideWindows)
		ShowWindow(SW_HIDE);
	KillTimer(1);
	if (DirDialog.DoBrowse())
	{
		CString Path(DirDialog.m_strPath);
		GetDlgItem(IDC_DESTINATIONNAME_EDIT)->SetWindowText(Path);
		Changed(true);
	}
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	ShowWindow(SW_SHOW);
}

void CConfig::OnCheckUseshort() 
{
	if (m_cCheckUseShort.GetCheck()==1)
	{
		m_cEditLongName.EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_EXAMPLE)->SetWindowText("IIYMDHMS");
	}
	else
	{
		CString cs;
		GetDlgItemText(IDC_EDIT_LONGNAME,cs);
		if (cs.IsEmpty())
		{
			char buff[32];
			GetPrivateProfileString(CONFIGURATION,LONGNAMEFORM,LONGNAMEFORM_DEFAULT,buff,sizeof(buff),g_szIniFile);
			if (0 != strlen(buff))
				m_cEditLongName.SetWindowText(buff);
			else
				m_cEditLongName.SetWindowText(LONGNAMEFORM_DEFAULT);
		}
		m_cEditLongName.EnableWindow(TRUE);
		OnChangeEditLongname();
	}
	Changed(true);
}

void CConfig::OnChangeEditLongname() 
{
	CString cResult,cTemp,cFormat;
	bool bHaveEscape = false;
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	GetDlgItemText(IDC_EDIT_LONGNAME,cFormat);

	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += "MG";
				break;
			case 'I':
				cResult += "999";
				break;
			case 'i':
				cResult += "99";
				break;
			case 'y':
				cTemp.Format("%04d",cNow.GetYear());
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",cNow.GetMonth());
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",cNow.GetDay());
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",cNow.GetHour());
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",cNow.GetMinute());
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",cNow.GetSecond());
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
		{
			cResult += cFormat[iSourceIndex];
		}
	}
	if (cResult.FindOneOf("\\/?*:;><\"")>=0)
		SetDlgItemText(IDC_STATIC_EXAMPLE,"DON'T USE character used!");
	else
		SetDlgItemText(IDC_STATIC_EXAMPLE,cResult);	
	Changed(true);
}
