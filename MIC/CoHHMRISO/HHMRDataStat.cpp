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
// HHMRDataStat.cpp : implementation file
//

#include "stdafx.h"
#include "colors.h"
#include "HHMRDataStat.h"
#include "HHMRInst.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHHMRDataStat property page

IMPLEMENT_DYNCREATE(CHHMRDataStat, CPropertyPage)

CHHMRDataStat::CHHMRDataStat() : CPropertyPage(CHHMRDataStat::IDD)
{
}

CHHMRDataStat::CHHMRDataStat(UINT id, CISO *parent) : CPropertyPage(id)
{
	for (int i= IDC_HHMRDATA_FIRST; i <= IDC_HHMRDATA_LAST; i++)
	{
		StaticColors[i-IDC_HHMRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	m_pParent = parent;
}


void
CHHMRDataStat::SetText(int index,CHAR * text)
{
	if ((index >= IDC_HHMRDATA_FIRST) &&
		(index <= IDC_HHMRDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *
CHHMRDataStat::GetText(int index, char * buff, int length)
{
	buff[0] = NULL;
	if ((index >= IDC_HHMRDATA_FIRST) &&
		(index <= IDC_HHMRDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}
void
CHHMRDataStat::SetPropertySheet(CHHMRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CHHMRDataStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_HHMRDATA_FIRST) &&
		(index <= IDC_HHMRDATA_LAST)) 
	{
	  StaticColors[index-IDC_HHMRDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CHHMRDataStat::ResetColors()
{
	for (int i= IDC_HHMRDATA_FIRST; i <= IDC_HHMRDATA_LAST; i++)
	{
		StaticColors[i-IDC_HHMRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CHHMRDataStat::~CHHMRDataStat()
{
}

void CHHMRDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHHMRDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHHMRDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CHHMRDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	//ON_BN_CLICKED(IDC_BUTTON_WATCH, OnButtonWatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHMRDataStat message handlers
HBRUSH CHHMRDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_HHMRDATA_FIRST) && (index <= IDC_HHMRDATA_LAST))
	{
		int iFirstOfList = IDC_HHMRDATA_FIRST; 
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

void CHHMRDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CHHMRDataStat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_RADIO_RAW))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_RATES))->SetCheck(1);
	
	return TRUE;
}

void CHHMRDataStat::OnButtonWatch() 
{
	// TODO: Add your control notification handler code here
	char szTemp[_MAX_PATH];
	strcpy(szTemp,m_pPropertySheet->m_pParent->m_pName);
	strcat(szTemp," Watcher");
	CWnd * pcWnd = FindWindow(NULL,szTemp);
	if (pcWnd == NULL)
	{
		char szNewPathName[_MAX_PATH];
		char szDrive[_MAX_DRIVE];
		char szDir[_MAX_DIR];
		char szFName[_MAX_FNAME];
		char szExt[_MAX_EXT];
		//m_pPropertySheet->m_pParent->m_szIniFile
		_splitpath(m_pPropertySheet->m_pParent->m_szIniFile,szDrive,szDir,szFName,szExt);
		_makepath(szNewPathName,szDrive,szDir,"HHMRWatch",".exe");
		/*if (_spawnl(_P_NOWAIT,szNewPathName,"HHMRWatch",m_pPropertySheet->m_pParent->m_pName,NULL) == -1)
			SetDlgItemText(IDC_BUTTON_WATCH,"Watch Data Graphically (exe missing)");
		else
			SetDlgItemText(IDC_BUTTON_WATCH,"Watch Data Graphically");*/
	}
	else
	{
		pcWnd->ShowWindow(SW_RESTORE);
		pcWnd->SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOMOVE);
	}
}

BOOL CHHMRDataStat::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}
