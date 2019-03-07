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
// DSPEC50Summary.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DSPEC50Summary.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50Summary property page

IMPLEMENT_DYNCREATE(CDSPEC50Summary, CPropertyPage)


//////////////////////////////////////////////////////////////////////////////
//
//  Constructors, Destructor, DDX, Message Map
//
//////////////////////////////////////////////////////////////////////////////
CDSPEC50Summary::CDSPEC50Summary() : CPropertyPage(CDSPEC50Summary::IDD)
{
////	TRACE("CDSPEC50Summary::CTOR #1\n");
}
CDSPEC50Summary::CDSPEC50Summary(UINT id, CISO* parent) : CPropertyPage(id)
{
//	TRACE("CDSPEC50Summary::CTOR #2\n");

	m_pPropertySheet = NULL;
	m_pParent = parent;

	ResetColors();

	//{{AFX_DATA_INIT(CDSPEC50Summary)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CDSPEC50Summary::~CDSPEC50Summary()
{
//		TRACE("CDSPEC50Summary::DTR\n");
}

void CDSPEC50Summary::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSPEC50Summary)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDSPEC50Summary, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPEC50Summary)
	ON_BN_CLICKED(IDC_GOTO_DSPEC50_DATASTAT, OnGotoDatastat)
	ON_BN_CLICKED(IDC_GOTO_DSPEC50_SETUP, OnGotoSetup)
	//ON_BN_CLICKED(IDC_GOTO_DSPEC50_USERSTAT, OnGotoUserstat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()





void CDSPEC50Summary::SetPropertySheet(CDSPEC50SafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}

void CDSPEC50Summary::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_DSPEC50SUMMARY_FIRST) &&
		(index <= IDC_DSPEC50SUMMARY_LAST)) 
		GetDlgItem(index)->SetWindowText(text);
}

void CDSPEC50Summary::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_DSPEC50SUMMARY_FIRST) &&
		(index <= IDC_DSPEC50SUMMARY_LAST)) 
	{
	  StaticColors[index-IDC_DSPEC50SUMMARY_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CDSPEC50Summary::ResetColors()
{
	for (int i= IDC_DSPEC50SUMMARY_FIRST; i <= IDC_DSPEC50SUMMARY_LAST; i++)
	{
		StaticColors[i-IDC_DSPEC50SUMMARY_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

void CDSPEC50Summary::OnGotoSetup() 
{
	if (m_pPropertySheet)
	{
		m_pPropertySheet->SetActivePage(2);
		m_pPropertySheet->SetToClose(0);
	}
}

void CDSPEC50Summary::OnGotoDatastat() 
{
	if (m_pPropertySheet) 
	{
		m_pPropertySheet->SetActivePage(1);
		m_pPropertySheet->SetToClose(0);
	}
}

//void CDSPEC50Summary::OnGotoUserstat() 
//{
//	if (m_pPropertySheet) 
//	{
//		m_pPropertySheet->SetActivePage(3);
//		m_pPropertySheet->SetToClose(0);
//	}
//}

HBRUSH CDSPEC50Summary::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	UINT index = pWnd->GetDlgCtrlID();
	if ((index >= IDC_DSPEC50SUMMARY_FIRST) && (index <= IDC_DSPEC50SUMMARY_LAST))
	{
		int iFirstOfList = IDC_DSPEC50SUMMARY_FIRST; 
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
			//pDC->SetTextColor(StaticColors[index-iFirstOfList]);
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}

	return hbr;
}

void CDSPEC50Summary::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		if ((index >= IDC_DSPEC50SUMMARY_FIRST) && (index <= IDC_DSPEC50SUMMARY_LAST))
		{
			if ((index == IDC_DSPEC50SUMMARY_COMMAND6) ||
				(index == IDC_DSPEC50SUMMARY_COMMAND5) ||
				(index == IDC_DSPEC50SUMMARY_COMMAND4) ||
				(index == IDC_DSPEC50SUMMARY_COMMAND3) ||
				(index == IDC_DSPEC50SUMMARY_COMMAND2) ||
				(index == IDC_DSPEC50SUMMARY_COMMAND1))
			{
				char Text[256];
				Text[0] = NULL;
				GetDlgItemText(index,Text,sizeof(Text));
				if (strlen(Text))
				{
					strcat(Text,"\"");
					memmove(&Text[2],Text,strlen(Text)+1);
					Text[0] = '"';
					Text[1] = 'G';
					_spawnl(_P_NOWAIT,m_pParent->m_szUtilFile,"MsgUtil.exe",Text,NULL);
				}
			}
		}
	}

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CDSPEC50Summary::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
	// TODO: Add your message handler code here

	CString cs = "Version " + m_pParent->m_csVersion + " " + __DATE__;
	SetDlgItemText(IDC_SUMMARY_STATIC_ADVERT, cs);
#ifdef _DEBUG
	SetDlgItemText(IDC_UPS_SUMMARY_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_SUMMARY_STATIC_DEBUG, "Release Build");
#endif
}

void CDSPEC50Summary::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CPropertyPage::OnTimer(nIDEvent);
}

BOOL CDSPEC50Summary::OnInitDialog() 
{
//	TRACE("CDSPEC50Summary::OnInitDialog()\n");

	CPropertyPage::OnInitDialog();

	return TRUE;
}


void CDSPEC50Summary::IssuePrint(FILE *outfile)
{
	//fprintf(outfile,"************************BEGIN CDSPEC50Summary::IssuePrint()\n");

	CString temp, temp1, temp2, temp3, temp4;

	fprintf(outfile,"\nSUMMARY\n");

	fprintf(outfile," DSPEC50 Settings\n");

	GetDlgItemText(IDC_DSPEC50SUMMARY_UNIT_ID,temp);
	fprintf(outfile,"%+25s: %s\n","Unit Identification",temp.GetString());

	fprintf(outfile," Data Status\n");

	GetDlgItemText(IDC_DSPEC50SUMMARY_NONZERO_CHANS,temp);
	fprintf(outfile,"%+25s: %s\n","Non-Zero Chans",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_ABOVE_THRESHOLD,temp);
	fprintf(outfile,"%+25s: %s\n","Chans Above Threshold",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_DWELLTYPE,temp);
	fprintf(outfile,"%+25s: %s\n","Dwell Type",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_SECS_TO_END_DWELL,temp);
	fprintf(outfile,"%+25s: %s\n","Secs Until End of Dwell",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_LAST_CHN_FILE,temp);
	fprintf(outfile,"%+25s: %s\n","Latest CHN File",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_CURRENT_HV,temp);
	fprintf(outfile,"%+25s: %s\n","Current HV",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_HV_OVERLOAD,temp);
	fprintf(outfile,"%+25s: %s\n","HV Overload",temp.GetString());

	GetDlgItemText(IDC_DSPEC50SUMMARY_SHUT_ACTUAL,temp);
	fprintf(outfile,"%+25s: %s\n","Shutdown", temp.GetString());

	fprintf(outfile," Recent Commands and Responses\n");
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND6,temp);
	fprintf(outfile,"   %s\n",temp.GetString());
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND5,temp);
	fprintf(outfile,"   %s\n",temp.GetString());
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND4,temp);
	fprintf(outfile,"   %s\n",temp.GetString());
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND3,temp);
	fprintf(outfile,"   %s\n",temp.GetString());
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND2,temp);
	fprintf(outfile,"   %s\n",temp.GetString());
	GetDlgItemText(IDC_DSPEC50SUMMARY_COMMAND1,temp);
	fprintf(outfile,"   %s\n",temp.GetString());

	fprintf(outfile,"\n");

	//fprintf(outfile,"************************END CDSPEC50Summary::IssuePrint()\n");
}
