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
// GenlStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "GenlStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenlStat property page

IMPLEMENT_DYNCREATE(CGenlStat, CPropertyPage)

CGenlStat::CGenlStat(): CPropertyPage(CGenlStat::IDD)
{
}


CGenlStat::CGenlStat(UINT id) : CPropertyPage(id)// CPropertyPage(CGenlStat::IDD)
{
	m_pPropertySheet = NULL;
	//initialize all displayed (cstatic) text to the
	//default color (black)
//	for (int i= IDC_GENERAL_FIRST; i <= IDC_GENERAL_LAST; i++)
//	{
//		StaticColors[i-IDC_GENERAL_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CGenlStat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CGenlStat::SetText(int index,CHAR* text)
{
	if ((index >= IDC_GENERAL_FIRST) &&
		(index <= IDC_GENERAL_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CGenlStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_GENERAL_FIRST) &&
		(index <= IDC_GENERAL_LAST)) {
	  StaticColors[index-IDC_GENERAL_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CGenlStat::ResetColors()
{
	for (int i= IDC_GENERAL_FIRST; i <= IDC_GENERAL_LAST; i++)
	{
		StaticColors[i-IDC_GENERAL_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CGenlStat::~CGenlStat()
{
}

void CGenlStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenlStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenlStat, CPropertyPage)
	//{{AFX_MSG_MAP(CGenlStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenlStat message handlers

HBRUSH CGenlStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_GENERAL_FIRST) && (index <= IDC_GENERAL_LAST))
	{
		int iFirstOfList = IDC_GENERAL_FIRST; 
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

void CGenlStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CGenlStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString *csversion = m_pPropertySheet->GetVersionString();
	CString cs = "Version " + *csversion + " " + __DATE__;
	SetDlgItemText(IDC_GENERAL_STATIC_ADVERT, cs);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG_GRAND, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG_GRAND, "Release Build");
#endif

	
}
