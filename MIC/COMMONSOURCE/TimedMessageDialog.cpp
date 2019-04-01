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

// TimedMessageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TimedMessageDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimedMessageDialog dialog


CTimedMessageDialog::CTimedMessageDialog(
	const char *title,
	const char *message,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedMessageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimedMessageDialog)
	m_MessageStatic = _T("");
	//}}AFX_DATA_INIT
	m_MessageStatic = CString(message);
	m_iTimeToGoInSeconds = seconds;
	m_csTitle = title;

	Create(IDD_MESSAGE_DIALOG,NULL);
//	SetWindowText(title);
//	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

CTimedMessageDialog::CTimedMessageDialog(
	CString& cstitle,
	CString& csmessage,
	int   seconds,
	CWnd* pParent /*=NULL*/)
	: CDialog(CTimedMessageDialog::IDD, pParent)
{
	m_MessageStatic = csmessage;
	m_iTimeToGoInSeconds = seconds;
	m_csTitle = cstitle;

	Create(IDD_MESSAGE_DIALOG,NULL);
//	SetWindowText(cstitle);
//	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

CTimedMessageDialog::~CTimedMessageDialog()
{
}

void CTimedMessageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimedMessageDialog)
	DDX_Text(pDX, IDC_MESSAGE_STATIC, m_MessageStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimedMessageDialog, CDialog)
	//{{AFX_MSG_MAP(CTimedMessageDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimedMessageDialog message handlers

void CTimedMessageDialog::OnTimer(UINT nIDEvent) 
{
	TRACE("ONTIMER_HIGHEST_LEVEL %d\n",m_iTimeToGoInSeconds);

	if (nIDEvent == 98765)
	{
		TRACE("ONTIMER\n");
		if (--m_iTimeToGoInSeconds <= 0)
		{
			PostMessage(WM_CLOSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CTimedMessageDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	SetTimer(98765,1000,NULL);
}

void CTimedMessageDialog::OnOK() 
{
	DestroyWindow();
	delete this;
}

void CTimedMessageDialog::OnClose() 
{	
	CDialog::OnClose();
	DestroyWindow();
	delete this;
}

BOOL CTimedMessageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(m_csTitle);
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
