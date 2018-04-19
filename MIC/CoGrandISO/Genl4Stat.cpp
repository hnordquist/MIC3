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
// Genl4Stat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "Genl4Stat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenl4Stat property page

IMPLEMENT_DYNCREATE(CGenl4Stat, CPropertyPage)

CGenl4Stat::CGenl4Stat() : CPropertyPage(CGenl4Stat::IDD)
{
}

CGenl4Stat::CGenl4Stat(UINT id) : CPropertyPage(id)// CPropertyPage(CGenl4Stat::IDD)
{
	m_pPropertySheet = NULL;
	//initialize all displayed (cstatic) text to the
	//default color (black)
//	for (int i= IDC_GENERAL4_FIRST; i <= IDC_GENERAL4_LAST; i++)
//	{
//		StaticColors[i-IDC_GENERAL4_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CGenl4Stat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CGenl4Stat::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CGenl4Stat::SetText(int index,CHAR* text)
{
	if ((index >= IDC_GENERAL4_FIRST) &&
		(index <= IDC_GENERAL4_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CGenl4Stat::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_GENERAL4_FIRST) &&
		(index <= IDC_GENERAL4_LAST)) {
	  StaticColors[index-IDC_GENERAL4_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void
CGenl4Stat::ResetColors()
{
	for (int i= IDC_GENERAL4_FIRST; i <= IDC_GENERAL4_LAST; i++)
	{
		StaticColors[i-IDC_GENERAL4_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CGenl4Stat::~CGenl4Stat()
{
}

void CGenl4Stat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenl4Stat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGenl4Stat, CPropertyPage)
	//{{AFX_MSG_MAP(CGenl4Stat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenl4Stat message handlers

HBRUSH CGenl4Stat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_GENERAL4_FIRST) && (index <= IDC_GENERAL4_LAST))
	{
		int iFirstOfList = IDC_GENERAL4_FIRST; 
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

//		if (StaticColors[(index-IDC_GENERAL4_FIRST)]==COLOR_RED)
//		{
//			pDC->SetBkColor(StaticColors[index-IDC_GENERAL4_FIRST]);
//			pDC->SetTextColor(COLOR_WHITE);
//		} 
//		else
//		{
//			pDC->SetTextColor(StaticColors[index-IDC_GENERAL4_FIRST]);
//		}
//		pDC->SelectStockObject(SYSTEM_FIXED_FONT);

//		pDC->SetTextColor(StaticColors[index-IDC_GENERAL4_FIRST]);
//		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
//	else
//		pDC->SetTextColor(COLOR_DEFAULT_TEXT);

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CGenl4Stat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CGenl4Stat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here

	CString *csversion = m_pPropertySheet->GetVersionString();
	CString cs = "Version " + *csversion + " " + __DATE__;
	SetDlgItemText(IDC_GENERAL_STATIC_ADVERT, cs);

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif

	
}
/*
void CGenl4Stat::GetStrings(int nId, CString &szName, char*szFlag)
{
	if (StaticColors[nId - IDC_GENERAL4_FIRST] == COLOR_YELLOW) szFlag[0] = '#';
	else if (StaticColors[nId - IDC_GENERAL4_FIRST] != COLOR_DEFAULT_TEXT) szFlag[0] = '*';
	else szFlag[0] = ' ';
	GetDlgItemText(nId,szName);
}

void CGenl4Stat::IssuePrint(FILE *outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;

	fprintf(outfile,"MG SETUP\n");
	fprintf(outfile,"Most Recent General Status Record, Instrument Status Record, and Battery Record\n");

	GetStrings(IDC_GENERAL4_BATTINT,temp,&cTemp);
	fprintf(outfile,"                   Battery Interval (min.): %s %c\n",temp,cTemp);

	GetStrings(IDC_GENERAL4_STATINT,temp,&cTemp);
	fprintf(outfile,"                    Status Interval (min.): %s %c\n",temp,cTemp);

	GetStrings(IDC_GENERAL4_FCHVSETPOINT,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_FCHIGHVMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_FCHIGHVCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_FCHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"PC HV Set Point %6s%c Pulse Counter HV Bias: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_ICHVSETPOINT,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_ICHIGHVMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_ICHIGHVCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_ICHIGHVMAX,temp4,&cTemp4);
	fprintf(outfile,"IC HV Set Point %6s%c   Ion Chamber HV Bias: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_COUNTTIME,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_BATTERYMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_BATTERYCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_BATTERYMAX,temp4,&cTemp4);
	fprintf(outfile,"    Count Time: %6s%c       Battery Voltage: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_MODE,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_P15VOLTMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_P15VOLTCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_P15VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 1: %6s%c       +12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_MODE2,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_M15VOLTMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_M15VOLTCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_M15VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"   Mode Flag 2: %6s%c       -12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp1, cTemp1, temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_P5VOLTMIN,temp2,&cTemp2);
	GetStrings(IDC_GENERAL4_P5VOLTCUR,temp3,&cTemp3);
	GetStrings(IDC_GENERAL4_P5VOLTMAX,temp4,&cTemp4);
	fprintf(outfile,"                              -12 Volt Supply: %6s%c %6s%c %6s%c\n",
		temp2, cTemp2, temp3, cTemp3, temp4, cTemp4);

	GetStrings(IDC_GENERAL4_ISRTIME,temp1,&cTemp1);
	fprintf(outfile,"          ISR Date & Time: %s\n",temp1);               

	GetStrings(IDC_GENERAL4_TIME,temp1,&cTemp1);
	fprintf(outfile,"          GSR Date & Time: %s\n",temp1);               

	GetStrings(IDC_GENERAL4_OFFSET,temp1,&cTemp1);
	fprintf(outfile,"      Battery Date & Time: %s\n",temp1);               

	GetStrings(IDC_GENERAL4_TSSTATE,temp1,&cTemp1);
	fprintf(outfile,"          Time Sync State: %s%c\n",temp1,cTemp1);               

	GetStrings(IDC_GENERAL4_TSHOURLY,temp1,&cTemp1);
	fprintf(outfile,"              Hourly Time: %s%c\n",temp1,cTemp1);               

	GetStrings(IDC_GENERAL4_TSDAILY,temp1,&cTemp1);
	fprintf(outfile,"               Daily Time: %s%c\n",temp1,cTemp1);               

	GetStrings(IDC_GENERAL4_TSUPPER,temp1,&cTemp1);
	fprintf(outfile,"          Upper Tolerance: %s%c\n",temp1,cTemp1); 
	
	GetStrings(IDC_GENERAL4_TSLOWER,temp1,&cTemp1);
	fprintf(outfile,"          Lower Tolerance: %s%c\n",temp1,cTemp1); 
	
	GetStrings(IDC_GENERAL4_G1DATE,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_G1TIME,temp2,&cTemp2);
	fprintf(outfile,"     Dual Ion Date & Time: %s %s\n",temp1,temp2);               
	
	GetStrings(IDC_GENERAL4_GAMMA1G,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_GAMMA2G,temp2,&cTemp2);
	fprintf(outfile,"                           Gamma1   Gamma2\n");
	fprintf(outfile,"                Gain Mode: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);
	
	GetStrings(IDC_GENERAL4_GAMMA1M,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_GAMMA2M,temp2,&cTemp2);
	fprintf(outfile,"       Max or Fixed Index: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(IDC_GENERAL4_MAXG1,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_MAXG2,temp2,&cTemp2);
	fprintf(outfile,"       Present Gain Index: %6s%c  %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(IDC_GENERAL4_DISCM,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_HVCONFIG,temp2,&cTemp2);
	fprintf(outfile,"              Offset Mode: %6s%c         HV Config: %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(IDC_GENERAL4_G1TIME,temp1,&cTemp1);
	GetStrings(IDC_GENERAL4_G2TIME,temp2,&cTemp2);
	fprintf(outfile,"         Nominal Interval: %6s%c  Maximum Interval: %6s%c\n",
		temp1, cTemp1, temp2, cTemp2);

	GetStrings(IDC_GENERAL4_GRANDP,temp1,&cTemp1);
	fprintf(outfile,"  GRAND Program Version Num: %6s%c\n",
		temp1, cTemp1);

	GetStrings(IDC_GENERAL4_ROM,temp1,&cTemp1);
	fprintf(outfile,"               ROM Checksum: %6s%c\n",
		temp1, cTemp1);

	GetStrings(IDC_GENERAL4_CHECKSUM,temp,&cTemp1);
	fprintf(outfile," Checksum, Length, or Authentication Error Received in Message: %c\n  %s\n",cTemp1,temp);
}

 */

