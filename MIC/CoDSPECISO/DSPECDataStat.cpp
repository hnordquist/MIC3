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
// DSPECDataStat.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "DSPECDataStat.h"
#include "ORTECSpectrum.h"
#include "DSPECStrings.h"
#include "DSPECInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STUB

/////////////////////////////////////////////////////////////////////////////
// CDSPECDataStat property page

IMPLEMENT_DYNCREATE(CDSPECDataStat, CPropertyPage)

CDSPECDataStat::CDSPECDataStat() : CPropertyPage(CDSPECDataStat::IDD)
{
//	TRACE("CDSPECDataStat::CTR_a\n");
}

CDSPECDataStat::CDSPECDataStat(UINT id, CWnd* pParent, char *inifile) : CPropertyPage(id)
{
//	TRACE("CDSPECDataStat::CTR_b\n");
	m_pPropertySheet = NULL;

	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));
	m_pParent = (CDSPECInst *)pParent;  //The instrument

	//Get the initial value of the gain conversion
	//from the INI file.
	char buf[128];
	GetPrivateProfileString(m_pParent->m_pName, SET_GAIN_CONVERSION,
				SET_GAIN_CONVERSION_DEFAULT,buf,sizeof(buf),m_szIniFile);
	sscanf(buf,"%u",&m_uGainConversion);

	//initialize all displayed (cstatic) text to the
	//default color (black)
	for (int i= IDC_DSPECDATASTAT_FIRST; i <= IDC_DSPECDATASTAT_LAST; i++)
	{
		StaticColors[i-IDC_DSPECDATASTAT_FIRST] = COLOR_DEFAULT_TEXT;
	}
	//{{AFX_DATA_INIT(CGenlStat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void CDSPECDataStat::SetPropertySheet(CDSPECSafePropertySheet *PropertySheet)
{
	m_pPropertySheet = PropertySheet;
}


void CDSPECDataStat::SetText(int index, char * text)
{
	//set the cstatic to the provided text
	if ((index >= IDC_DSPECDATASTAT_FIRST) &&
		(index <= IDC_DSPECDATASTAT_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

char *CDSPECDataStat::GetText(int index, char * buff, int length)
{
	//get the text from a cstatic
	//used when print to file button is selected
	buff[0] = NULL;
	if ((index >= IDC_DSPECDATASTAT_FIRST) &&
		(index <= IDC_DSPECDATASTAT_LAST))
		GetDlgItem(index)->GetWindowText(buff,length-1);
	return buff;
}

void CDSPECDataStat::SetColor(int index, COLORREF color)
{
	//set the current color of a cstatic
	//it will get updated via onctlcolor()
	if ((index >= IDC_DSPECDATASTAT_FIRST) &&
		(index <= IDC_DSPECDATASTAT_LAST)) {
	  StaticColors[index-IDC_DSPECDATASTAT_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}

void CDSPECDataStat::ResetColors()
{
	for (int i= IDC_DSPECDATASTAT_FIRST; i <= IDC_DSPECDATASTAT_LAST; i++)
	{
		StaticColors[i-IDC_DSPECDATASTAT_FIRST] = COLOR_DEFAULT_TEXT;
	}
}

CDSPECDataStat::~CDSPECDataStat()
{
//	TRACE("CDSPECDataStat::DTR\n");
}

void CDSPECDataStat::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDataStat)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSPECDataStat, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPECDataStat)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_DSPECBUTTON_CLEAR_AND_RESTART, OnClearAndRestart)
	ON_BN_CLICKED(IDC_DSPECBUTTON_WRT_CHN_FILE, OnWriteCHNFile)
	ON_BN_CLICKED(IDC_DSPEC_CHECK_SHOW, OnShowSpectra)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDSPECDataStat::OnClearAndRestart()
{
//	TRACE("CDSPECDataStat::OnClearAndRestart()\n");

	if (m_pPropertySheet->m_pParent->IsPaused())
	{
		STUB;	//do nothing - better to disable and enable button
	}
	else
	{
		m_pPropertySheet->m_pParent->PostMessage(IDC_CLEAR_AND_RESTART_SPECTRUM,0,0);
	}
}

void CDSPECDataStat::OnWriteCHNFile()
{
//	TRACE("CDSPECDataStat::OnWriteCHNFile()\n");

	if (m_pPropertySheet->m_pParent->IsPaused())
	{
		STUB;  //do nothing - better to disable and enable button
	}
	else
	{
		m_pPropertySheet->m_pParent->PostMessage(IDC_FORCE_WRITE_CHN_FILE,0,0);
	}
}

HBRUSH CDSPECDataStat::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//get the id of the control 
	UINT index = pWnd->GetDlgCtrlID();

	//set the font and color of that control
	if ((index >= IDC_DSPECDATASTAT_FIRST) && (index <= IDC_DSPECDATASTAT_LAST))
	{
		int iFirstOfList = IDC_DSPECDATASTAT_FIRST; 
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

void CDSPECDataStat::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPropertySheet)m_pPropertySheet->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

void CDSPECDataStat::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);	
}

BOOL CDSPECDataStat::OnInitDialog() 
{
//	TRACE("CDSPECDataStat::OnInitDialog()\n");

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
		SetDlgItemText(IDC_DSPEC_CHECK_SHOW,"Hide Spectrum.\nNeeds Supervisor\naccess to change.");
	else
		SetDlgItemText(IDC_DSPEC_CHECK_SHOW,"Show Spectrum.\nNeeds Supervisor\naccess to change.");

	((CButton*)GetDlgItem(IDC_DSPEC_CHECK_SHOW))->SetCheck(m_iShowSpectra);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSPECDataStat::SetGainConversion(UINT gc)
{
	m_uGainConversion = gc;

	if (::IsWindow(m_XYCtrl.m_hWnd))
		m_XYCtrl.SetNumChannels(m_uGainConversion);

	CString cs;
	cs.Format("Realtime Raw Data (%u points)",m_uGainConversion);
	SetDlgItemText(IDC_STATIC_REALTIME_RAW_DATA, cs);
}

int CDSPECDataStat::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	return 0;
}
void CDSPECDataStat::OnShowSpectra()
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
		SetDlgItemText(IDC_DSPEC_CHECK_SHOW,"Hide Spectrum.\nNeeds Supervisor\naccess to change.");
	else
	{
		SetDlgItemText(IDC_DSPEC_CHECK_SHOW,"Show Spectrum.\nNeeds Supervisor\naccess to change.");
		m_XYCtrl.Reset();
	}

	((CButton*)GetDlgItem(IDC_DSPEC_CHECK_SHOW))->SetCheck(m_iShowSpectra);
}

void CDSPECDataStat::IssuePrint(FILE *outfile)
{
	//fprintf(outfile,"*****************************Begin CDSPECDataStat::IssuePrint()\n");

	CString cs;
	fprintf(outfile,"DATA STATUS\n");

	GetDlgItemText(IDC_DSPECDATASTAT_DWELL_TYPE,cs);
	fprintf(outfile,"%+25s: %s\n","Dwell Type",cs.GetString()); // gjk

	GetDlgItemText(IDC_DSPECDATASTAT_SECS_TIL_END,cs);
	fprintf(outfile,"%+25s: %s\n","Sec. Until End of Dwell",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPECDATASTAT_NON_ZERO_CHANS,cs);
	fprintf(outfile,"%+25s: %s\n","Non-Zero Channels",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPECDATASTAT_ABV_THRESHOLD,cs);
	fprintf(outfile,"%+25s: %s\n","Chnls Above Threshold",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPECDATASTAT_CURRENT_HV,cs);
	fprintf(outfile,"%+25s: %s\n","Current High Voltage",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPECDATASTAT_HV_OVERLOAD,cs);
	fprintf(outfile,"%+25s: %s\n","HV Overload",cs.GetString()); //gjk

	GetDlgItemText(IDC_DSPECDATASTAT_SHUTDOWN,cs);				//dgp 1Aug2007
	fprintf(outfile,"%+25s: %s\n","Shutdown",cs.GetString());	// gjk //dgp 1Aug2007

	GetDlgItemText(IDC_DSPECDATASTAT_SPARE3,cs);				//dgp 1Aug2007
	fprintf(outfile,"%+25s: %s\n","Max Peak Cnt",cs.GetString());//gjk//dgp 1Aug2007dp

	int state = ((CButton *)(GetDlgItem(IDC_DSPEC_CHECK_SHOW)))->GetCheck();
	if (state == 1)
		cs = "No";
	else
		cs = "Yes";

	fprintf(outfile,"%+25s: %s\n","Spectrum Hidden",cs.GetString()); //gjk

	fprintf(outfile,"\n");

	//fprintf(outfile,"*******************End CDSPECDataStat::IssuePrint()\n");
}

bool CDSPECDataStat::ProcessSpectrumBufferForRealTimeGraphic(CORTECSpectrum *pspec)
{
	//TRACE("CDSPECDataStat::ProcessSpectrumBufferForGraphic()\n");

	static unsigned long cThresholdLineColor = COLOR_CYAN;//COLOR_GREEN;

	m_XYCtrl.Reset();
	UINT *buf = pspec->RawData();

	SetDlgItemInt(IDC_DSPECDATASTAT_SPARE3,pspec->MaximumValue(),FALSE);

	m_XYCtrl.DrawThresholdLine(cThresholdLineColor, pspec->Threshold());
	m_XYCtrl.DrawHorizontalGridLines(COLOR_BLACK);

	if (m_iShowSpectra != 0)
	{
		for (int i = 0; i < pspec->NumChans(); i++)
			m_XYCtrl.AddPoint(i,buf[i]);
	}
	return true;
}
