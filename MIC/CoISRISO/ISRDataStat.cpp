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
// ISRDataStat.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "ISRDataStat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CISRDataStat property page

IMPLEMENT_DYNCREATE(CISRDataStat, CPropertyPage)

CISRDataStat::CISRDataStat() : CPropertyPage(CISRDataStat::IDD)
{
}

CISRDataStat::CISRDataStat(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;

	for (int i= IDC_ISRDATA_FIRST; i <= IDC_ISRDATA_LAST; i++)
	{
		StaticColors[i-IDC_ISRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CISRDataStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CISRDataStat::SetPropertySheet(CISRPropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CISRDataStat::SetText(int index,CHAR * text)
{
	if ((index >= IDC_ISRDATA_FIRST) &&
		(index <= IDC_ISRDATA_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *CISRDataStat::GetText(int index, char * buff, int length)
{
	buff[0] = NULL;
	if ((index >= IDC_ISRDATA_FIRST) &&
		(index <= IDC_ISRDATA_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void CISRDataStat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_ISRDATA_FIRST) &&
		(index <= IDC_ISRDATA_LAST)) 
	{
	  StaticColors[index-IDC_ISRDATA_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CISRDataStat::ResetColors()
{
	for (int i= IDC_ISRDATA_FIRST; i <= IDC_ISRDATA_LAST; i++)
	{
		StaticColors[i-IDC_ISRDATA_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CISRDataStat::~CISRDataStat()
{
}

BEGIN_MESSAGE_MAP(CISRDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CISRDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_ISRRADIO_RATES, OnRadioRates)
	ON_BN_CLICKED(IDC_ISRRADIO_RAW, OnRadioRaw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CISRDataStat message handlers
HBRUSH CISRDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_ISRDATA_FIRST) && (index <= IDC_ISRDATA_LAST))
	{
		int iFirstOfList = IDC_ISRDATA_FIRST; 
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

void CISRDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

BOOL CISRDataStat::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_ISRRADIO_RAW))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_ISRRADIO_RATES))->SetCheck(1);
	m_bDoingRates = true;
	return TRUE;
}

void CISRDataStat::ToRate(double period, int DlgItem)
{
	char text[32];
	double temp, result;
	GetDlgItemText(DlgItem,text,sizeof(text)-1);
	temp = atof(text);
	if (period != 0.0)
		result = temp/period;
	else
		result = temp;
	sprintf(text,"%11.4e",result);
	SetDlgItemText(DlgItem,text);
}

void CISRDataStat::ToRaw(double period, int DlgItem)
{
	char text[32];
	double temp, result;
	GetDlgItemText(DlgItem,text,sizeof(text)-1);
	temp = atof(text);
	if (period != 0.0)
		result = temp*period;
	else
		result = temp;
	sprintf(text,"%d",(int)result);
	SetDlgItemText(DlgItem,text);
}

void CISRDataStat::OnRadioRates() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	m_bDoingRates = true;
}

void CISRDataStat::OnRadioRaw() 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	m_bDoingRates = false;	
}

void CISRDataStat::IssuePrint(FILE *outfile)
{
	//<QA>>
#ifdef _DEBUG
	AfxMessageBox("CISRDataStat::IssuePrint() unimplemented\n");
#endif
}