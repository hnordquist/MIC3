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
// HHMRSummary.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "HHMRSummary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHHMRSummary property page

IMPLEMENT_DYNCREATE(CHHMRSummary, CPropertyPage)

CHHMRSummary::CHHMRSummary() : CPropertyPage(CHHMRSummary::IDD)
{
}


CHHMRSummary::CHHMRSummary(UINT id, CISO *parent) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
	m_pParent = parent;

	for (int i= IDC_HHMRSUMMARY_FIRST; i <= IDC_HHMRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_HHMRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_bDebug = false;
}

void CHHMRSummary::SetPropertySheet(CHHMRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CHHMRSummary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_HHMRSUMMARY_FIRST) &&
		(index <= IDC_HHMRSUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CHHMRSummary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_HHMRSUMMARY_FIRST) &&
		(index <= IDC_HHMRSUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_HHMRSUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CHHMRSummary::ResetColors()
{
	for (int i= IDC_HHMRSUMMARY_FIRST; i <= IDC_HHMRSUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_HHMRSUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CHHMRSummary::~CHHMRSummary()
{
}

void CHHMRSummary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHMRSummary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHHMRSummary, CPropertyPage)
	//{{AFX_MSG_MAP(CHHMRSummary)
	ON_BN_CLICKED(IDC_GOTO_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_MODIFYPARAMETERS_PAGE, OnGotoModifyParametersPage)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHMRSummary message handlers

void CHHMRSummary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

void CHHMRSummary::OnGotoModifyParametersPage() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

HBRUSH CHHMRSummary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_HHMRSUMMARY_FIRST) && (index <= IDC_HHMRSUMMARY_LAST))
	{
		int iFirstOfList = IDC_HHMRSUMMARY_FIRST; 
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

void CHHMRSummary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if  (
			(((CButton*)GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==1) &&
			(index >= IDC_HHMRSUMMARY_FIRST) && 
			(index <= IDC_HHMRSUMMARY_LAST)
			)
		{
			if (
				(index == IDC_HHMRSUMMARY_COMMAND13) ||
				(index == IDC_HHMRSUMMARY_COMMAND12) ||
				(index == IDC_HHMRSUMMARY_COMMAND11) ||
				(index == IDC_HHMRSUMMARY_COMMAND10) ||
				(index == IDC_HHMRSUMMARY_COMMAND9) ||
				(index == IDC_HHMRSUMMARY_COMMAND8) ||
				(index == IDC_HHMRSUMMARY_COMMAND7) ||
				(index == IDC_HHMRSUMMARY_COMMAND6) ||
				(index == IDC_HHMRSUMMARY_COMMAND5) ||
				(index == IDC_HHMRSUMMARY_COMMAND4) ||
				(index == IDC_HHMRSUMMARY_COMMAND3) ||
				(index == IDC_HHMRSUMMARY_COMMAND2) ||
				(index == IDC_HHMRSUMMARY_COMMAND1))
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

void CHHMRSummary::OnShowWindow(BOOL bShow, UINT nStatus) 
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
