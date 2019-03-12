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
// User4Prog.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "User4Prog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUser4Prog property page

IMPLEMENT_DYNCREATE(CUser4Prog, CPropertyPage)

CUser4Prog::CUser4Prog() : CPropertyPage(CUser4Prog::IDD)
{
}

CUser4Prog::CUser4Prog(UINT id) : CPropertyPage(id)// CPropertyPage(CUser4Prog::IDD)
{
	m_pPropertySheet = NULL;
//	for (int i= IDC_USER4PROG_FIRST; i <= IDC_USER4PROG_LAST; i++)
//	{
//		StaticColors[i-IDC_USER4PROG_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CUser4Prog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CUser4Prog::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CUser4Prog::SetText(int index,CHAR * text)
{
	if ((index >= IDC_USER4PROG_FIRST) && (index <= IDC_USER4PROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CUser4Prog::ResetColors()
{
	for (int i= IDC_USER4PROG_FIRST; i <= IDC_USER4PROG_LAST; i++)
	{
		StaticColors[i-IDC_USER4PROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void
CUser4Prog::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_USER4PROG_FIRST) &&
		(index <= IDC_USER4PROG_LAST)) {
	  StaticColors[index-IDC_USER4PROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CUser4Prog::~CUser4Prog()
{
}

void CUser4Prog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUser4Prog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUser4Prog, CPropertyPage)
	//{{AFX_MSG_MAP(CUser4Prog)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUser4Prog message handlers
HBRUSH CUser4Prog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_USER4PROG_FIRST) && (index <= IDC_USER4PROG_LAST))
	{
//		pDC->SetTextColor(StaticColors[index-IDC_USER4PROG_FIRST]);
//		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
		int iFirstOfList = IDC_USER4PROG_FIRST; 
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

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
void CUser4Prog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CUser4Prog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
	
}

