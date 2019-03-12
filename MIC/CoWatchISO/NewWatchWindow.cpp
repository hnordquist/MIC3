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
// NewWatchWindow.cpp

#include "stdafx.h"
#include "NewWatchWindow.h"
#include "INI_definitions.h"
#include "testinst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewWatchWindow dialog

CNewWatchWindow::CNewWatchWindow(						 
		char * szIniFile,
		int iDlgClose,
		char *name,
		CWnd* pParent /*=NULL*/)
	: CDialog(CNewWatchWindow::IDD, pParent)
{
//	TRACE("CNewWatchWindow::COTR\n");
	strncpy(m_szIniFile, szIniFile, sizeof(m_szIniFile));
	m_iDlgCloseInMilliseconds = iDlgClose;
	strncpy(m_pName, name, sizeof(m_pName));

	//{{AFX_DATA_INIT(CNewWatchWindow)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CNewWatchWindow::~CNewWatchWindow()
{
//	TRACE("CNewWatchWindow::DTOR - NOOP\n");
}


void CNewWatchWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewWatchWindow)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewWatchWindow, CDialog)
	//{{AFX_MSG_MAP(CNewWatchWindow)
	ON_LBN_SELCHANGE(IDC_COMMNAMES_LIST, OnSelchangeCommnamesList)
	ON_EN_CHANGE(IDC_NODE_EDIT, OnChangeNodeEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_NODE_SPIN, OnDeltaposNodeSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewWatchWindow message handlers

void CNewWatchWindow::OnSelchangeCommnamesList() 
{
	int index;
	if((index=((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->GetCurSel())==LB_ERR)
	{
		m_pCommName[0] = NULL;
	}
	else
	{
		((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->GetText(index,m_pCommName);
		//((CButton *)GetDlgItem(IDOK))->EnableWindow(true);
		m_pCreateButton->EnableWindow(TRUE);
	}
}

void CNewWatchWindow::OnChangeNodeEdit() 
{
	//don't let the user place a number larger than 32767 in the node
	//as above--we'll have to revisit this when we add support for 
	//ethernet
	CString Value;
	GetDlgItemText(IDC_NODE_EDIT,Value);
	if (atoi(Value) > 32767)
		SetDlgItemText(IDC_NODE_EDIT,"32767");
	else if (atoi(Value) == 0)
		SetDlgItemText(IDC_NODE_EDIT,"0");
	else if (atoi(Value) <= -1)
		SetDlgItemText(IDC_NODE_EDIT, "-1");
}

void CNewWatchWindow::OnDeltaposNodeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CNewWatchWindow::OnCancel() 
{
//	TRACE("CNewWatchWindow::OnCancel()\n");
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CNewWatchWindow::OnOK() 
{
//	TRACE("CNewWatchWindow::OnOk()\n");
	//find the equals 
	char* key = strtok(m_pCommName,"=");

	//get the node
	CString Node;
	GetDlgItemText(IDC_NODE_EDIT,Node);

	WritePrivateProfileString(INSTRUMENTS,m_pName,WATCH_DISCOVERY_TYPE,m_szIniFile);

	//create this instrument's section and add the port and node entries
	WritePrivateProfileString(m_pName,PORT,key,m_szIniFile);
	WritePrivateProfileString(m_pName,NODE,Node,m_szIniFile);
	
	CDialog::OnOK();
}

BOOL CNewWatchWindow::OnInitDialog() 
{
//	TRACE("CNewWatchWindow::OnInitDialog()\n");

	CDialog::OnInitDialog();
	
	char buff[512];
	//get all of the communications objects and place
	//their names in the list box
	//m_bAllowFinish = false;
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
		sizeof(buff), m_szIniFile)>0)
	{
		char *at = buff;
		while (strlen(at) > 0)
		{
			((CListBox*)GetDlgItem(IDC_COMMNAMES_LIST))->AddString(at);
			//m_bAllowFinish = true;
			at = at + strlen(at)+1;
		}
	}

	SetWindowText(m_pName);

	//limit the user input to 5 digits
	((CEdit*)GetDlgItem(IDC_NODE_EDIT))->LimitText(5);

	//limit the spinner to what the ILON needs
	//we will have to revisit this when we add
	//support for ethernet
	((CSpinButtonCtrl*)GetDlgItem(IDC_NODE_SPIN))->SetRange(-1,32767);

	SetDlgItemText(IDC_NODE_EDIT, "-1");

	//GetDlgItem(IDOK)->EnableWindow(FALSE);

	//create the create button
	m_pCreateButton = new CYellowButton(
		"Create...",
		IDOK,
		m_szIniFile,
		this);


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
