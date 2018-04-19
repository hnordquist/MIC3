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
// DSPEC50DataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DSPEC50DataStat.h"
#include "ORTECSpectrum.h"
#include "DSPEC50Strings.h"
#include "DSPEC50Inst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STUB

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50DataStat property page

IMPLEMENT_DYNCREATE(CDSPEC50DataStat, CPropertyPage)

CDSPEC50DataStat::CDSPEC50DataStat() : CPropertyPage(CDSPEC50DataStat::IDD)
{
//	TRACE("CDSPEC50DataStat::CTR_a\n");
}

CDSPEC50DataStat::CDSPEC50DataStat(UINT id, CWnd* pParent, char *inifile) : CPropertyPage(id)
{
//	TRACE("CDSPEC50DataStat::CTR_b\n");
	m_pPropertySheet = NULL;

	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));
	m_pParent = (CDSPEC50Inst *)pParent;  //The instrument

	//Get the initial value of the gain conversion
	//from the INI file.
	char buf[128];
	GetPrivateProfileString(m_pParent->m_pName, SET_GAIN_CONVERSION,
				SET_GAIN_CONVERSION_DEFAULT,buf,sizeof(buf),m_szIniFile);
	sscanf(buf,"%u",&m_uGainConversion);

	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_DSPEC50DATASTAT_FIRST; i <= IDC_DSPEC50DATASTAT_LAST; i++)
	{
		StaticColors[i-IDC_DSPEC50DATASTAT_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CDSPEC50DataStat::SetPropertySheet(CDSPEC50SafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void CDSPEC50DataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_DSPEC50DATASTAT_FIRST) &&
		(index <= IDC_DSPEC50DATASTAT_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *CDSPEC50DataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_DSPEC50DATASTAT_FIRST) &&
		(index <= IDC_DSPEC50DATASTAT_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void CDSPEC50DataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_DSPEC50DATASTAT_FIRST) &&
		(index <= IDC_DSPEC50DATASTAT_LAST)) {
	  StaticColors[index-IDC_DSPEC50DATASTAT_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CDSPEC50DataStat::ResetColors()
{
	for (int i= IDC_DSPEC50DATASTAT_FIRST; i <= IDC_DSPEC50DATASTAT_LAST; i++)
	{
		StaticColors[i-IDC_DSPEC50DATASTAT_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CDSPEC50DataStat::~CDSPEC50DataStat()
{
//	TRACE("CDSPEC50DataStat::DTR\n");
}

void CDSPEC50DataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSPEC50DataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPEC50DataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_DSPEC50BUTTON_CLEAR_AND_RESTART, OnClearAndRestart)
	ON_BN_CLICKED(IDC_DSPEC50BUTTON_WRT_CHN_FILE, OnWriteCHNFile)
	ON_BN_CLICKED(IDC_DSPEC50_CHECK_SHOW, OnShowSpectra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDSPEC50DataStat::OnClearAndRestart()
{
//	TRACE("CDSPEC50DataStat::OnClearAndRestart()\n");

	if (m_pPropertySheet->m_pParent->IsPaused())
	{
		STUB;	//do nothing - better to disable and enable button
	}
	else
	{
		m_pPropertySheet->m_pParent->PostMessage(IDC_CLEAR_AND_RESTART_SPECTRUM,0,0);
	}
}

void CDSPEC50DataStat::OnWriteCHNFile()
{
//	TRACE("CDSPEC50DataStat::OnWriteCHNFile()\n");

	if (m_pPropertySheet->m_pParent->IsPaused())
	{
		STUB;  //do nothing - better to disable and enable button
	}
	else
	{
		m_pPropertySheet->m_pParent->PostMessage(IDC_FORCE_WRITE_CHN_FILE,0,0);
	}
}

HBRUSH CDSPEC50DataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();

	//set the font and color of that control
	if ((index >= IDC_DSPEC50DATASTAT_FIRST) && (index <= IDC_DSPEC50DATASTAT_LAST))
	{
		int iFirstOfList = IDC_DSPEC50DATASTAT_FIRST; 
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
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	}
	return hbr;
}

void CDSPEC50DataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CDSPEC50DataStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);	
}

BOOL CDSPEC50DataStat::OnInitDialog() 
{
//	TRACE("CDSPEC50DataStat::OnInitDialog()\n");

	CPropertyPage::OnInitDialog();

	m_XYCtrl.SubclassDlgItem(IDC_CUSTOM_XYCTRL,this);

	m_XYCtrl.SetTitle("");
	m_XYCtrl.SetLeftLabel("");
	m_XYCtrl.SetBottomLabel("");
	m_XYCtrl.SetNumChannels(m_uGainConversion);

	CString cs;
	cs.Format("Realtime Raw Data (%u points)",m_uGainConversion);
	SetDlgItemText(IDC_STATIC_REALTIME_RAW_DATA, cs);

	if (m_pPropertySheet)
	{
		int iDefault;
		sscanf(SHOW_SPECTRA_DEFAULT, "%d",&iDefault);
		m_iShowSpectra = 
			GetPrivateProfileInt(
			m_pPropertySheet->m_pParent->m_pName,SHOW_SPECTRA,iDefault,m_szIniFile);
	}
	if (m_iShowSpectra)
		SetDlgItemText(IDC_DSPEC50_CHECK_SHOW,"Hide Spectrum.\nNeeds Supervisor\naccess to change.");
	else
		SetDlgItemText(IDC_DSPEC50_CHECK_SHOW,"Show Spectrum.\nNeeds Supervisor\naccess to change.");

	((CButton*)GetDlgItem(IDC_DSPEC50_CHECK_SHOW))->SetCheck(m_iShowSpectra);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSPEC50DataStat::SetGainConversion(UINT gc)
{
	m_uGainConversion = gc;

	if (::IsWindow(m_XYCtrl.m_hWnd))
		m_XYCtrl.SetNumChannels(m_uGainConversion);

	CString cs;
	cs.Format("Realtime Raw Data (%u points)",m_uGainConversion);
	SetDlgItemText(IDC_STATIC_REALTIME_RAW_DATA, cs);
}

int CDSPEC50DataStat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}
void CDSPEC50DataStat::OnShowSpectra()
{
	if (m_pParent->Validate(true)) //superuser = true
	{
		if (m_iShowSpectra==1)
			m_iShowSpectra = 0;
		else
			m_iShowSpectra = 1;

		char szTemp[16];
		WritePrivateProfileString(m_pPropertySheet->m_pParent->m_pName,SHOW_SPECTRA,
			_itoa(m_iShowSpectra,szTemp,10),m_szIniFile);
	}
	ShowWindow(SW_SHOW);

	if (m_iShowSpectra != 0)
		SetDlgItemText(IDC_DSPEC50_CHECK_SHOW,"Hide Spectrum.\nNeeds Supervisor\naccess to change.");
	else
	{
		SetDlgItemText(IDC_DSPEC50_CHECK_SHOW,"Show Spectrum.\nNeeds Supervisor\naccess to change.");
		m_XYCtrl.Reset();
	}

	((CButton*)GetDlgItem(IDC_DSPEC50_CHECK_SHOW))->SetCheck(m_iShowSpectra);
}

void CDSPEC50DataStat::IssuePrint(FILE *outfile)
{
	//fprintf(outfile,"*****************************Begin CDSPEC50DataStat::IssuePrint()\n");

	CString cs;
	fprintf(outfile,"DATA STATUS\n");

	GetDlgItemText(IDC_DSPEC50DATASTAT_DWELL_TYPE,cs);
	fprintf(outfile,"%+25s: %s\n","Dwell Type",cs.GetString()); // gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_SECS_TIL_END,cs);
	fprintf(outfile,"%+25s: %s\n","Sec. Until End of Dwell",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_NON_ZERO_CHANS,cs);
	fprintf(outfile,"%+25s: %s\n","Non-Zero Channels",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_ABV_THRESHOLD,cs);
	fprintf(outfile,"%+25s: %s\n","Chnls Above Threshold",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_CURRENT_HV,cs);
	fprintf(outfile,"%+25s: %s\n","Current High Voltage",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_HV_OVERLOAD,cs);
	fprintf(outfile,"%+25s: %s\n","HV Overload",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPEC50DATASTAT_SHUTDOWN,cs);				//dgp 1Aug2007
	fprintf(outfile,"%+25s: %s\n","Shutdown",cs.GetString());	// gjk //dgp 1Aug2007

	GetDlgItemText(IDC_DSPEC50DATASTAT_SPARE3,cs);				//dgp 1Aug2007
	fprintf(outfile,"%+25s: %s\n","Max Peak Cnt",cs.GetString());//gjk//dgp 1Aug2007dp

	int state = ((CButton *)(GetDlgItem(IDC_DSPEC50_CHECK_SHOW)))->GetCheck();
	if (state == 1)
		cs = "No";
	else
		cs = "Yes";

	fprintf(outfile,"%+25s: %s\n","Spectrum Hidden",cs.GetString()); //gjk

	fprintf(outfile,"\n");

	//fprintf(outfile,"*******************End CDSPEC50DataStat::IssuePrint()\n");
}

bool CDSPEC50DataStat::ProcessSpectrumBufferForRealTimeGraphic(CORTECSpectrum *pspec)
{
	//TRACE("CDSPEC50DataStat::ProcessSpectrumBufferForGraphic()\n");

	static unsigned long cThresholdLineColor = COLOR_CYAN;//COLOR_GREEN;

	m_XYCtrl.Reset();
	UINT *buf = pspec->RawData();

	SetDlgItemInt(IDC_DSPEC50DATASTAT_SPARE3,pspec->MaximumValue(),FALSE);

	m_XYCtrl.DrawThresholdLine(cThresholdLineColor, pspec->Threshold());
	m_XYCtrl.DrawHorizontalGridLines(COLOR_BLACK);

	if (m_iShowSpectra != 0)
	{
		for (int i = 0; i < pspec->NumChans(); i++)
			m_XYCtrl.AddPoint(i,buf[i]);
	}
	return true;
}
