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
// DataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DataStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataStat property page

IMPLEMENT_DYNCREATE(CDataStat, CPropertyPage)

CDataStat::CDataStat() : CPropertyPage(CDataStat::IDD)
{
}

CDataStat::CDataStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;

	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void
CDataStat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void
CDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void
CDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_DATA_FIRST) &&
		(index <= IDC_DATA_LAST)) {
	  StaticColors[index-IDC_DATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CDataStat::ResetColors()
{
	for (int i= IDC_DATA_FIRST; i <= IDC_DATA_LAST; i++)
	{
		StaticColors[i-IDC_DATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CDataStat::~CDataStat()
{
}

void CDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataStat message handlers
HBRUSH CDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();
	//set the font and color of that control
	if ((index >= IDC_DATA_FIRST) && (index <= IDC_DATA_LAST))
	{
		int iFirstOfList = IDC_DATA_FIRST; 
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
void CDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CDataStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
	
}

int CDataStat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}
