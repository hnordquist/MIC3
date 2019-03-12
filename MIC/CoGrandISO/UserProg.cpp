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
// UserProg.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "UserProg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserProg property page

IMPLEMENT_DYNCREATE(CUserProg, CPropertyPage)

CUserProg::CUserProg() : CPropertyPage(CUserProg::IDD)
{
}

CUserProg::CUserProg(UINT id) : CPropertyPage(id)
{
	m_pPropertySheet = NULL;
//	for (int i= IDC_USERPROG_FIRST; i <= IDC_USERPROG_LAST; i++)
//	{
//		StaticColors[i-IDC_USERPROG_FIRST] = COLOR_DEFAULT_TEXT;
//	}
	//{{AFX_DATA_INIT(CUserProg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void
CUserProg::SetPropertySheet(CSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void
CUserProg::SetText(int index,CHAR * text)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void
CUserProg::ResetColors()
{
	for (int i= IDC_USERPROG_FIRST; i <= IDC_USERPROG_LAST; i++)
	{
		StaticColors[i-IDC_USERPROG_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void
CUserProg::SetColor(int index, COLORREF color)
{
	if ((index >= IDC_USERPROG_FIRST) &&
		(index <= IDC_USERPROG_LAST)) {
	  StaticColors[index-IDC_USERPROG_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

CUserProg::~CUserProg()
{
}

void CUserProg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserProg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserProg, CPropertyPage)
	//{{AFX_MSG_MAP(CUserProg)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserProg message handlers
HBRUSH CUserProg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_USERPROG_FIRST) && (index <= IDC_USERPROG_LAST))
	{
//		pDC->SetTextColor(StaticColors[index-IDC_USERPROG_FIRST]);
//		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
		int iFirstOfList = IDC_USERPROG_FIRST; 
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
void CUserProg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet) 
		m_pPropertySheet->SetToClose(0);

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CUserProg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
//	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	// TODO: Add your message handler code here
	
}
/*
void CUserProg::GetStrings(int nId, CString &szName, char*szFlag)
{
	if (StaticColors[nId - IDC_USERPROG_FIRST] == COLOR_YELLOW) szFlag[0] = '#';
	else if (StaticColors[nId - IDC_USERPROG_FIRST] != COLOR_DEFAULT_TEXT) szFlag[0] = '*';
	else szFlag[0] = ' ';
	GetDlgItemText(nId,szName);
}


void CUserProg::IssuePrint(FILE* outfile)
{
	CString temp, temp1, temp2, temp3, temp4;
	char cTemp, cTemp1, cTemp2, cTemp3, cTemp4;
	fprintf(outfile,"MONITOR PARAMETERS\n");
	GetDlgItemText(IDC_USERPROG_TIME,temp);
	fprintf(outfile," Last USER PROGRAM STATUS Record (Y.M.D H:M:S): %s\n",temp);

	GetStrings(IDC_USERPROG_USER,temp,&cTemp);
	fprintf(outfile," User Selected Data Acquire Time (secs): %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_IMMEDIATE,temp,&cTemp);
	fprintf(outfile," Immediate Buffer Size / Save Size: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_LOCALBACK,temp,&cTemp);
	fprintf(outfile,"       Local Background Definition: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_NOFILTER,temp,&cTemp);
	fprintf(outfile," No Filter and Enter MII Condition: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_FILTERMETH,temp,&cTemp);
	fprintf(outfile,"                     Filter Method: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_MIIENTER,temp,&cTemp);
	fprintf(outfile,"               MII Enter Condition: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_MIIEXIT,temp,&cTemp);
	fprintf(outfile,"                MII Exit Condition: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_FILTERENAB,temp,&cTemp);
	fprintf(outfile,"                 Filtering Enabled: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_TRIGGERMODE,temp,&cTemp);
	fprintf(outfile,"                      Trigger Mode: %s%c\n",temp,cTemp);

	fprintf(outfile,"                       Pulse A    Pulse B    Pulse C    Gamma 1    Gamma 2\n");

	GetStrings(IDC_USERPROG_PULSEAF,temp,&cTemp);
	GetStrings(IDC_USERPROG_PULSEBF,temp1,&cTemp1);
	GetStrings(IDC_USERPROG_PULSECF,temp2,&cTemp2);
	GetStrings(IDC_USERPROG_GAMMA1F,temp3,&cTemp3);
	GetStrings(IDC_USERPROG_GAMMA2F,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filter & MII Active:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(IDC_USERPROG_PULSEAT,temp ,&cTemp);
	GetStrings(IDC_USERPROG_PULSEBT,temp1,&cTemp1);
	GetStrings(IDC_USERPROG_PULSECT,temp2,&cTemp2);
	GetStrings(IDC_USERPROG_GAMMA1T,temp3,&cTemp3);
	GetStrings(IDC_USERPROG_GAMMA2T,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile,"           Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(IDC_USERPROG_PULSEAL,temp ,&cTemp);
	GetStrings(IDC_USERPROG_PULSEBL,temp1,&cTemp1);
	GetStrings(IDC_USERPROG_PULSECL,temp2,&cTemp2);
	GetStrings(IDC_USERPROG_GAMMA1L,temp3,&cTemp3);
	GetStrings(IDC_USERPROG_GAMMA2L,temp4,&cTemp4);
	temp.TrimLeft();		temp1.TrimLeft();		temp2.TrimLeft();
	temp3.TrimLeft();		temp4.TrimLeft();
	fprintf(outfile," Filtering Threshold:%9s%c %9s%c %9s%c %9s%c %9s%c\n\n",
		temp,cTemp,temp1,cTemp1,temp2,cTemp2,temp3,cTemp3,temp4,cTemp4);

	GetStrings(IDC_USERPROG_SYNCE,temp,&cTemp);
	fprintf(outfile,"               Syncronization Enabled: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_SYNCT,temp,&cTemp);
	fprintf(outfile,"       Syncronization time (hh:mm:ss): %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_SYNCL,temp,&cTemp);
	fprintf(outfile," Syncronization Lower Limit (seconds): %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_SYNCU,temp,&cTemp);
	fprintf(outfile," Syncronization Upper Limit (minutes): %s%c\n\n",temp,cTemp);

	GetStrings(IDC_USERPROG_TAKEOFF,temp,&cTemp);
	fprintf(outfile,"        Take Offsets Every (hours): %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_TAKEOFFMAX,temp,&cTemp);
	fprintf(outfile,"   Maximum Between Offsets (hours): %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_LOGBATT,temp,&cTemp);
	fprintf(outfile,"  Log Battery Info Every (minutes): %s%c\n\n",temp,cTemp);

	GetStrings(IDC_USERPROG_GRANDUV,temp,&cTemp);
	fprintf(outfile," GRAND User Program Version Number: %s%c\n",temp,cTemp);

	GetStrings(IDC_USERPROG_ROMCHECK,temp,&cTemp);
	fprintf(outfile,"                      ROM Checksum: %s%c\n",temp,cTemp);

}
*/