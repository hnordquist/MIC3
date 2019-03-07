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
	ON_BN_CLICKED(IDC_RADIO_RAW, &CHHMRDataStat::OnBnClickedRadioRaw)
	ON_BN_CLICKED(IDC_RADIO_RATES, &CHHMRDataStat::OnBnClickedRadioRates)
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
	currStatus = RATES;
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


void CHHMRDataStat::OnBnClickedRadioRaw()
{
	// TODO: Add your control notification handler code here
	if (currStatus == RATES)
		RefreshView(RAW);
	currStatus = RAW;
}


void CHHMRDataStat::OnBnClickedRadioRates()
{
	// TODO: Add your control notification handler code here
	if (currStatus == RAW)
		RefreshView(RATES);
	currStatus = RATES;
}

void CHHMRDataStat::RefreshView(short toView)
{
	//We are switching from raw to rates or vice versa
	char* temp = new char[32];
	//get data and make it the right data view.
	//First column
	if (GetText(IDC_HHMRDATA_ETIME1, temp, sizeof(temp)) > 0)
	{
		//Store elapsed time for conversion
		double  etime = atof(temp);
		//Totals
		if (GetText(IDC_HHMRDATA_TOTALS1, temp, sizeof(temp)) > 0)
		{
			double totals = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(totals*etime));
				SetText(IDC_HHMRDATA_TOTALS1, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(totals / etime));
				SetText(IDC_HHMRDATA_TOTALS1, temp);
			}
		}
		
		//Reals+ accidentals
		if (GetText(IDC_HHMRDATA_REALACC1, temp, sizeof(temp)) > 0)
		{
			double reacc = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(reacc*etime));
				SetText(IDC_HHMRDATA_REALACC1, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(reacc / etime));
				SetText(IDC_HHMRDATA_REALACC1, temp);
			}
		}
		//Reals --doh, this is actually labeled accidentals
		if (GetText(IDC_HHMRDATA_ACCIDENT1, temp, sizeof(temp)) > 0)
		{
			double reals = atof(temp);
			//convert reals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(reals*etime));
				SetText(IDC_HHMRDATA_ACCIDENT1, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(reals / etime));
				SetText(IDC_HHMRDATA_ACCIDENT1, temp);
			}
		}
		//Aux1
		if (GetText(IDC_HHMRDATA_AUX1TOTALS1, temp, sizeof(temp)) > 0)
		{
			double aux1 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX1TOTALS1, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(aux1 / etime));
				SetText(IDC_HHMRDATA_AUX1TOTALS1, temp);
			}
		}
		//Aux2
		if (GetText(IDC_HHMRDATA_AUX2TOTALS1, temp, sizeof(temp)) > 0)
		{
			double aux2 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX2TOTALS1, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(aux2 / etime));
				SetText(IDC_HHMRDATA_AUX2TOTALS1, temp);
			}
		}
		//Second column
		if (GetText(IDC_HHMRDATA_ETIME2, temp, sizeof(temp)) > 0)
		{
			//Store elapsed time for conversion
			double etime2 = atoi(temp);
			//Totals
			if (GetText(IDC_HHMRDATA_TOTALS2, temp, sizeof(temp)) > 0)
			{
				double totals2 = atoi(temp);
				//convert totals
				if (toView == RAW)
				{
					sprintf(temp, "%d", (int)(totals2*etime2));
					SetText(IDC_HHMRDATA_TOTALS2, temp);
				}
				else
				{
					sprintf(temp, "%11.4e", (double)(totals2 / etime2));
					SetText(IDC_HHMRDATA_TOTALS2, temp);
				}
			}
			
			//Reals+ accidentals
			if (GetText(IDC_HHMRDATA_REALACC2, temp, sizeof(temp)) > 0)
			{
				double reacc2 = atof(temp);
				//convert totals
				if (toView == RAW)
				{
					sprintf(temp, "%d", (int)(reacc2*etime2));
					SetText(IDC_HHMRDATA_REALACC2, temp);
				}
				else
				{
					sprintf(temp, "%11.4e", (double)(reacc2 / etime2));
					SetText(IDC_HHMRDATA_REALACC2, temp);
				}
			}
			//Reals
			if (GetText(IDC_HHMRDATA_ACCIDENT2, temp, sizeof(temp)) > 0)
			{
				double accident2 = atof(temp);
				//convert totals
				if (toView == RAW)
				{
					sprintf(temp, "%d", (int)(accident2*etime2));
					SetText(IDC_HHMRDATA_ACCIDENT2, temp);
				}
				else
				{
					sprintf(temp, "%11.4e", (double)(accident2 / etime2));
					SetText(IDC_HHMRDATA_ACCIDENT2, temp);
				}
			}
			//Aux1
			if (GetText(IDC_HHMRDATA_AUX1TOTALS2, temp, sizeof(temp)) > 0)
			{
				double aux12 = atoi(temp);
				//convert totals
				if (toView == RAW)
				{
					temp = "0";
					SetText(IDC_HHMRDATA_AUX1TOTALS2, temp);
					temp = new char[32];
				}
				else
				{
					sprintf(temp, "%11.4e", (double)aux12 / etime2);
					SetText(IDC_HHMRDATA_AUX1TOTALS2, temp);
				}
			}
			//Aux2
			if (GetText(IDC_HHMRDATA_AUX2TOTALS2, temp, sizeof(temp)) > 0)
			{
				double aux22 = atof(temp);
				//convert totals
				if (toView == RAW)
				{
					temp = "0";
					SetText(IDC_HHMRDATA_AUX2TOTALS2, temp);
					temp = new char[32];
				}
				else
				{
					sprintf(temp, "%11.4e", (double)(aux22 / etime2));
					SetText(IDC_HHMRDATA_AUX2TOTALS2, temp);
				}
			}
		}
	}
	//Third column
	if (GetText(IDC_HHMRDATA_ETIME3, temp, sizeof(temp)) > 0)
	{
		//Store elapsed time for conversion
		double etime3 = atoi(temp);
		//Totals
		if (GetText(IDC_HHMRDATA_TOTALS3, temp, sizeof(temp)) > 0)
		{
			double totals3 = atoi(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(totals3*etime3));
				SetText(IDC_HHMRDATA_TOTALS3, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(totals3 / etime3));
				SetText(IDC_HHMRDATA_TOTALS3, temp);
			}
		}

		//Reals+ accidentals
		if (GetText(IDC_HHMRDATA_REALACC3, temp, sizeof(temp)) > 0)
		{
			double reacc3 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(reacc3*etime3));
				SetText(IDC_HHMRDATA_REALACC3, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(reacc3 / etime3));
				SetText(IDC_HHMRDATA_REALACC3, temp);
			}
		}
		//Reals
		if (GetText(IDC_HHMRDATA_ACCIDENT3, temp, sizeof(temp)) > 0)
		{
			double accident3 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(accident3*etime3));
				SetText(IDC_HHMRDATA_ACCIDENT3, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(accident3 / etime3));
				SetText(IDC_HHMRDATA_ACCIDENT3, temp);
			}
		}
		//Aux1
		if (GetText(IDC_HHMRDATA_AUX1TOTALS3, temp, sizeof(temp)) > 0)
		{
			double aux13 = atoi(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX1TOTALS3, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)aux13 / etime3);
				SetText(IDC_HHMRDATA_AUX1TOTALS3, temp);
			}
		}
		//Aux2
		if (GetText(IDC_HHMRDATA_AUX2TOTALS3, temp, sizeof(temp)) > 0)
		{
			double aux23 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX2TOTALS3, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(aux23 / etime3));
				SetText(IDC_HHMRDATA_AUX2TOTALS3, temp);
			}
		}
	}
	//Fourth column
	if (GetText(IDC_HHMRDATA_ETIME4, temp, sizeof(temp)) > 0)
	{
		//Store elapsed time for conversion
		double etime4 = atoi(temp);
		//Totals
		if (GetText(IDC_HHMRDATA_TOTALS4, temp, sizeof(temp)) > 0)
		{
			double totals4 = atoi(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(totals4*etime4));
				SetText(IDC_HHMRDATA_TOTALS4, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(totals4 / etime4));
				SetText(IDC_HHMRDATA_TOTALS4, temp);
			}
		}

		//Reals+ accidentals
		if (GetText(IDC_HHMRDATA_REALACC4, temp, sizeof(temp)) > 0)
		{
			double reacc4 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(reacc4*etime4));
				SetText(IDC_HHMRDATA_REALACC4, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(reacc4 / etime4));
				SetText(IDC_HHMRDATA_REALACC4, temp);
			}
		}
		//Reals
		if (GetText(IDC_HHMRDATA_ACCIDENT4, temp, sizeof(temp)) > 0)
		{
			double accident4 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				sprintf(temp, "%d", (int)(accident4*etime4));
				SetText(IDC_HHMRDATA_ACCIDENT4, temp);
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(accident4 / etime4));
				SetText(IDC_HHMRDATA_ACCIDENT4, temp);
			}
		}
		//Aux1
		if (GetText(IDC_HHMRDATA_AUX1TOTALS4, temp, sizeof(temp)) > 0)
		{
			double aux14 = atoi(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX1TOTALS4, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)aux14 / etime4);
				SetText(IDC_HHMRDATA_AUX1TOTALS4, temp);
			}
		}
		//Aux2
		if (GetText(IDC_HHMRDATA_AUX2TOTALS4, temp, sizeof(temp)) > 0)
		{
			double aux24 = atof(temp);
			//convert totals
			if (toView == RAW)
			{
				temp = "0";
				SetText(IDC_HHMRDATA_AUX2TOTALS4, temp);
				temp = new char[32];
			}
			else
			{
				sprintf(temp, "%11.4e", (double)(aux24 / etime4));
				SetText(IDC_HHMRDATA_AUX2TOTALS4, temp);
			}
		}
	}
}
