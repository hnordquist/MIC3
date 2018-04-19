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
 

// UserInfo.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "UserInfo.h"
#include "TimedMessageDialog.h"
#include "mic_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserInfo dialog


CUserInfo::CUserInfo(CWnd* pParent, int mode, char* name, char* password )
	: CDialog(CUserInfo::IDD, pParent)
{
	m_dMode = mode;
	m_pcName = name;
	m_pcPassword = password;
	//{{AFX_DATA_INIT(CUserInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUserInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserInfo, CDialog)
	//{{AFX_MSG_MAP(CUserInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserInfo message handlers

BOOL CUserInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char buffa[128];
	char buffb[128];
	GetWindowText(buffa,sizeof(buffa));
	switch(m_dMode){
	case 0:
		sprintf(buffb,buffa,"Edit");
		break;
	case 1:
		sprintf(buffb,buffa,"New");
		break;
	}
	SetWindowText(buffb);

	((CEdit*)GetDlgItem(IDC_USERNAME_EDIT))->SetWindowText(m_pcName);
	((CEdit*)GetDlgItem(IDC_PASSWORD1_EDIT))->SetWindowText(m_pcPassword);
	((CEdit*)GetDlgItem(IDC_PASSWORD2_EDIT))->SetWindowText(m_pcPassword);
	GetDlgItem(IDC_USERNAME_EDIT)->SetFocus();
	return false; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserInfo::OnOK() 
{
	CString name;
	CString password1;
	CString password2;
	((CEdit*)GetDlgItem(IDC_USERNAME_EDIT ))->GetWindowText(name);
	((CEdit*)GetDlgItem(IDC_PASSWORD1_EDIT))->GetWindowText(password1);
	((CEdit*)GetDlgItem(IDC_PASSWORD2_EDIT))->GetWindowText(password2);
	name.TrimLeft();
	name.TrimRight();
	password1.TrimLeft();
	password2.TrimLeft();
	password1.TrimRight();
	password2.TrimRight();

	if (g_bHideWindows) 
		ShowWindow(SW_HIDE);

	CString csTitle(MIC_NAMELONG);
	csTitle += ": Password";

	if (password1 != password2)
	{
		CString csText("Passwords don't match!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//			"Passwords don't match!");
		ShowWindow(SW_SHOW);
		return;
	} 

	if (name.GetLength() == 0) 
	{
		CString csText("A user name must be entered!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//CTimeMsgDlg MsgBox(this, g_dDlgCloseMillisec);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//			"A user name must be entered!");
		ShowWindow(SW_SHOW);
		return;
	}

	if (password1.GetLength() == 0) 
	{
		CString csText("A password must be entered!");
		new CTimedMessageDialog(csTitle,csText,1000 * g_iDlgCloseMillisec, this);
		//CTimeMsgDlg MsgBox(this, g_dDlgCloseMillisec);
		//MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Password")),
		//"A password must be entered!");
		ShowWindow(SW_SHOW);
		return;
	}

	strcpy(m_pcName,name);
	strcpy(m_pcPassword,password1);
	CDialog::OnOK();
}

BOOL CUserInfo::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::DestroyWindow();
}
