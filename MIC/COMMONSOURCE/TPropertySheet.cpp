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
// TPropertySheet.cpp - in CommonSource directory

// DR00169 - Buttons can be crowded and unusable when adding a new instrument.
// DR00169  - 2.001 removed hardcoding from 2.000  PJM 13-Feb-2006
 
#include "stdafx.h"
#include "TPropertySheet.h"
#include "iso_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTPropertySheet

IMPLEMENT_DYNAMIC(CTPropertySheet, CPropertySheet)

CTPropertySheet::CTPropertySheet(UINT nIDCaption, int dlg_close, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
//	TRACE("CTPropertySheet::CTR a\n");
	m_dMyDlgClose = dlg_close;
	m_bSameNextFinish = false;
}

CTPropertySheet::CTPropertySheet(LPCTSTR pszCaption, int dlg_close, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
//	TRACE("CTPropertySheet::CTOR b\n");
	m_dMyDlgClose = dlg_close;
	m_bSameNextFinish = false;
}

CTPropertySheet::~CTPropertySheet()
{
//	TRACE("CTPropertySheet::DTOR\n");
}

BEGIN_MESSAGE_MAP(CTPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTPropertySheet)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDB_TOCLOSE, OnToCloseButton)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//	ON_WM_CLOSE()

void
CTPropertySheet::SameNextFinish()
{
//	TRACE("CTPropertySheet::SameNextFinish()\n");
	m_bSameNextFinish = false;
}

/////////////////////////////////////////////////////////////////////////////
// CTPropertySheet message handlers

void CTPropertySheet::OnToCloseButton()
{
//	TRACE("CTPropertySheet::OnToCloseButton()\n");
	char temp[32];
	m_dToClose -= BUMP_EXTEND;
	if ((m_dMyDlgClose/1000-m_dToClose) > MAX_EXTEND) 
		m_dToClose = m_dMyDlgClose/1000 - MAX_EXTEND;
	int minutes = (m_dMyDlgClose/1000-m_dToClose)/60;
	int seconds   = (m_dMyDlgClose/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	SetDlgItemText(IDB_TOCLOSE,temp);
}


void CTPropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_dToClose >= m_dMyDlgClose/1000)
		{
			m_dToClose = 0;
			KillTimer(1);
			CPropertySheet::PressButton(PSBTN_CANCEL);
		} 
		else
		{
			char temp[32];
			int minutes = (m_dMyDlgClose/1000-m_dToClose)/60;
			int seconds   = (m_dMyDlgClose/1000-m_dToClose) % 60;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			SetDlgItemText(IDB_TOCLOSE,temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

BOOL CTPropertySheet::OnInitDialog() 
{
//	TRACE("CTPropertySheet::OnInitDialog()\n");

/*
	// DR00169 2.001 removed hardcoding from 2.000

  NOTE:  For MICGM, m_bSameNextFinish is always true.  MIC 1.9.0.7 set it false when
  creating a WATCH window; MICGM uses a different mechanism for creating a WATCH window
  that doesn't use the wizard functionality.

	PJM May 18, 2005
*/

	BOOL bResult = CPropertySheet::OnInitDialog();
	SetTimer(1,1000,NULL);
	char temp[32];
	WINDOWPLACEMENT lpwndpl;
	//grow the property sheet 
	GetWindowPlacement(&lpwndpl);
	lpwndpl.rcNormalPosition.bottom += 27;
	// DR00169 2.001 removed hardcoding from 2.000
	SetWindowPlacement(&lpwndpl);

	//we do want the cancel action
	GetDlgItem(IDCANCEL)->GetWindowPlacement(&lpwndpl);
	lpwndpl.rcNormalPosition.bottom += 28;
	// DR00169 2.001 removed hardcoding from 2.000
	GetDlgItem(IDCANCEL)->SetWindowPlacement(&lpwndpl);

	int minutes = (m_dMyDlgClose/1000-m_dToClose)/60;
	int seconds   = (m_dMyDlgClose/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	SetDlgItemText(IDHELP,temp);
	GetDlgItem(IDHELP)->ModifyStyle(NULL,BS_MULTILINE);
	GetDlgItem(IDHELP)->GetWindowPlacement(&lpwndpl);
	lpwndpl.rcNormalPosition.bottom += 28;
	// DR00169 2.001 removed hardcoding from 2.000
	GetDlgItem(IDHELP)->SetWindowPlacement(&lpwndpl);
	GetDlgItem(IDHELP)->SetDlgCtrlID(IDB_TOCLOSE);

	if (m_bSameNextFinish)
	{
		GetDlgItem(ID_WIZNEXT)->GetWindowPlacement(&lpwndpl);
		int offset = lpwndpl.rcNormalPosition.right-lpwndpl.rcNormalPosition.left;
		lpwndpl.rcNormalPosition.bottom += 28;
		lpwndpl.rcNormalPosition.left -= offset;
		lpwndpl.rcNormalPosition.right -= offset;
		// DR00169 2.001 removed hardcoding from 2.000
		GetDlgItem(ID_WIZNEXT)->SetWindowPlacement(&lpwndpl);
		GetDlgItem(ID_WIZBACK)->GetWindowPlacement(&lpwndpl);
		lpwndpl.rcNormalPosition.bottom += 28;
		lpwndpl.rcNormalPosition.left -= offset;
		lpwndpl.rcNormalPosition.right -= offset;
		// DR00169 2.001 removed hardcoding from 2.000
		GetDlgItem(ID_WIZBACK)->SetWindowPlacement(&lpwndpl);
		GetDlgItem(ID_WIZFINISH)->GetWindowPlacement(&lpwndpl);
		lpwndpl.rcNormalPosition.bottom += 28;
		// DR00169 2.001 removed hardcoding from 2.000
		GetDlgItem(ID_WIZFINISH)->SetWindowPlacement(&lpwndpl);
//		GetDlgItem(ID_WIZFINISH)->EnableWindow(false);
	}
	else
	{
		//GetDlgItem(ID_WIZNEXT)->GetWindowPlacement(&lpwndpl);
		//lpwndpl.rcNormalPosition.bottom += 28;
		//GetDlgItem(ID_WIZNEXT)->SetWindowPlacement(&lpwndpl);
		GetDlgItem(ID_WIZBACK)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_WIZNEXT)->ShowWindow(SW_HIDE);
		//lpwndpl.rcNormalPosition.bottom += 28;
		//GetDlgItem(ID_WIZBACK)->SetWindowPlacement(&lpwndpl);
		
		GetDlgItem(ID_WIZFINISH)->GetWindowPlacement(&lpwndpl);
		lpwndpl.rcNormalPosition.bottom += 28;
		GetDlgItem(ID_WIZFINISH)->SetWindowPlacement(&lpwndpl);
//		GetDlgItem(ID_WIZFINISH)->EnableWindow(false);
	}

	for (int i = GetPageCount();i>=0;i--)
		SetActivePage(i-1);

	m_dToClose = 0;

	return bResult;
}

int CTPropertySheet::DoModal() 
{
//	TRACE("CTPropertySheet::DoModal()\n");
	m_dToClose = 0;
	return CPropertySheet::DoModal();
}

int CTPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CTPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)  //MIC_CB_028 to MIC 1.907 PJM 1/20/05
		m_dToClose = set;
}

void CTPropertySheet::ForceDateTime(void)
{
}

void CTPropertySheet::OnClose() 
{
//	TRACE("CTPropertySheet::OnClose()\n");

	// TODO: Add your message handler code here and/or call default
	
	CPropertySheet::OnClose();
}
