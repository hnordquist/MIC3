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
//
// HideDlg.cpp : implementation file
//
#include "stdafx.h"
#include "globals.h"
#include "HideDlg.h"
#include "DoFileCopy.h"
#include "ISO_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHideDlg dialog


CHideDlg::CHideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHideDlg::IDD, pParent)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CHideDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHideDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHideDlg, CDialog)
	//{{AFX_MSG_MAP(CHideDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_HIDDEN_EXTEND, OnHiddenExtend)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideDlg message handlers

void CHideDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (((CDoFileCopy*)m_pParent)->m_dNewMediaState == 2) 
			OnOK();

		m_dToClose++;

		char temp[32];
		int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
		int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
		sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

		SetDlgItemText(ID_HIDDEN_EXTEND,temp);
		if ((minutes == 0)&&(seconds==0)) OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CHideDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	ModifyStyle(WS_VISIBLE,NULL,NULL);
//	ShowWindow(SW_HIDE);
//	MoveWindow(0,0,1,1,false);

	m_dToClose = 0;

	char temp[32];
	int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
	int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
	sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

	SetDlgItemText(ID_HIDDEN_EXTEND,temp);
	SetTimer(1,500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHideDlg::OnHiddenExtend() 
{
	m_dToClose -= (BUMP_EXTEND*2);

	if ((g_iDlgCloseMillisec/1000 - m_dToClose/2) > MAX_EXTEND) 
		m_dToClose = g_iDlgCloseMillisec/500-(MAX_EXTEND * 2);

	char temp[32];
	int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
	int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
	sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

	SetDlgItemText(ID_HIDDEN_EXTEND,temp);
}

void CHideDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_dToClose = 0;
	CDialog::OnLButtonDown(nFlags, point);
}
