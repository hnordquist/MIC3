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
// JSRSummary.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "JSRSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJSRSummary property page

IMPLEMENT_DYNCREATE(CJSRSummary, CPropertyPage)

CJSRSummary::CJSRSummary() : CPropertyPage(CJSRSummary::IDD)
{
}


CJSRSummary::CJSRSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_JSRSUMMARY_FIRST; i <= IDC_JSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_JSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bDebug = false;
}

void CJSRSummary::SetPropertySheet(CJSRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CJSRSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_JSRSUMMARY_FIRST) &&
		(index <= IDC_JSRSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CJSRSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_JSRSUMMARY_FIRST) &&
		(index <= IDC_JSRSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_JSRSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CJSRSummary::ResetColors()
{
	for (int i= IDC_JSRSUMMARY_FIRST; i <= IDC_JSRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_JSRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CJSRSummary::~CJSRSummary()
{
}

void CJSRSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJSRSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJSRSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CJSRSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_MODIFYPARAMETERS_PAGE, OnGotoModifyParametersPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJSRSummary message handlers

void CJSRSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CJSRSummary::OnGotoModifyParametersPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CJSRSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_JSRSUMMARY_FIRST) && (index <= IDC_JSRSUMMARY_LAST))
	{
		int iFirstOfList = IDC_JSRSUMMARY_FIRST; 
		if (StaticColors[index-iFirstOfList]==COLOR_RED)
		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(COLOR_WHITE);
		}
		else if (StaticColors[(index-iFirstOfList)]==COLOR_YELLOW)
		{
			pDC->SetBkColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_RED);
		}
		else
		{
			pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}

	return hbr;
}

void CJSRSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if  (
			(((CButton*)GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==1) &&
			(index >= IDC_JSRSUMMARY_FIRST) && 
			(index <= IDC_JSRSUMMARY_LAST)
			)
		{
			if (
				(index == IDC_JSRSUMMARY_COMMAND13) ||
				(index == IDC_JSRSUMMARY_COMMAND12) ||
				(index == IDC_JSRSUMMARY_COMMAND11) ||
				(index == IDC_JSRSUMMARY_COMMAND10) ||
				(index == IDC_JSRSUMMARY_COMMAND9) ||
				(index == IDC_JSRSUMMARY_COMMAND8) ||
				(index == IDC_JSRSUMMARY_COMMAND7) ||
				(index == IDC_JSRSUMMARY_COMMAND6) ||
				(index == IDC_JSRSUMMARY_COMMAND5) ||
				(index == IDC_JSRSUMMARY_COMMAND4) ||
				(index == IDC_JSRSUMMARY_COMMAND3) ||
				(index == IDC_JSRSUMMARY_COMMAND2) ||
				(index == IDC_JSRSUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'J';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CJSRSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	char Debug[16];
	GetPrivateProfileString(
		CONFIGURATION,"DEBUG","FALSE",Debug,sizeof(Debug),
		m_pParent->m_szIniFile);

	if (toupper(Debug[0]) == 'T')
		m_bDebug = true;
	else
		m_bDebug = false;

	if (m_bDebug)
	{
		GetDlgItem(IDC_STATIC_LASTDATA)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_NOTE5)->ShowWindow(SW_SHOW);

		//<<QA>> hide the version info
	}
	else
	{
		GetDlgItem(IDC_STATIC_LASTDATA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_NOTE5)->ShowWindow(SW_HIDE);

		//<<QA>> Show the version info
	}
}
