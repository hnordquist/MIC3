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
// ISRSummary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "ISRSummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CISRSummary property page

IMPLEMENT_DYNCREATE(CISRSummary, CPropertyPage)

CISRSummary::CISRSummary() : CPropertyPage(CISRSummary::IDD)
{
}


CISRSummary::CISRSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_ISRSUMMARY_FIRST; i <= IDC_ISRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_ISRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CISRSummary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CISRSummary::~CISRSummary()
{
}

BEGIN_MESSAGE_MAP(CISRSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CISRSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_ISR_SETUP_PAGE, OnGotoISRSetupPage)
	ON_BN_CLICKED(IDC_GOTO_ISR_MONITOR_PARAMS_PAGE, OnGotoMonitorParametersPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CISRSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CISRSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CISRSummary::SetPropertySheet(CISRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CISRSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_ISRSUMMARY_FIRST) &&
		(index <= IDC_ISRSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CISRSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_ISRSUMMARY_FIRST) &&
		(index <= IDC_ISRSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_ISRSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CISRSummary::ResetColors()
{
	for (int i= IDC_ISRSUMMARY_FIRST; i <= IDC_ISRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_ISRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}


void CISRSummary::OnGotoISRSetupPage()
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CISRSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CISRSummary::OnGotoMonitorParametersPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(3);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CISRSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_ISRSUMMARY_FIRST) && (index <= IDC_ISRSUMMARY_LAST))
	{
		int iFirstOfList = IDC_ISRSUMMARY_FIRST; 
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

void CISRSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_ISRSUMMARY_FIRST) && (index <= IDC_ISRSUMMARY_LAST))
		{
			if ((index == IDC_ISRSUMMARY_COMMAND6) ||
				(index == IDC_ISRSUMMARY_COMMAND5) ||
				(index == IDC_ISRSUMMARY_COMMAND4) ||
				(index == IDC_ISRSUMMARY_COMMAND3) ||
				(index == IDC_ISRSUMMARY_COMMAND2) ||
				(index == IDC_ISRSUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'I';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}
	CPropertyPage::OnLButtonDown(nFlags, point);
}

