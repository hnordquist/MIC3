/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-040. This software was exported from the United States in accordance with the Export
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


#ifndef DSPEC50DATASTAT_H
#define DSPEC50DATASTAT_H

// DSPEC50DataStat.h

#include "DSPEC50SafePS.h"
#include "XYCtrl.h"
#include "ORTECSpectrum.h"
#include "resource.h"

class CDSPEC50Inst;
/////////////////////////////////////////////////////////////////////////////
// CDSPEC50DATASTAT_H DataStat dialog

class CDSPEC50DataStat : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPEC50DataStat)

// Construction
public:
	CDSPEC50DataStat(UINT id, CWnd* pParent = NULL, char *inifile = NULL);
	CDSPEC50DataStat();
	~CDSPEC50DataStat();

	void SetText(int, char *);
	char *GetText(int, char *,int);
	void SetColor(int, COLORREF);
	void ResetColors();
	void SetPropertySheet(CDSPEC50SafePropertySheet*);
	void SetGainConversion(UINT gc);
	COLORREF StaticColors[IDC_DSPEC50DATASTAT_LAST-IDC_DSPEC50DATASTAT_FIRST+1];
	void IssuePrint(FILE *outfile);

	bool ProcessSpectrumBufferForRealTimeGraphic(CORTECSpectrum *pspec);
	void SetInhibitRealtimeDataButton(bool paused);
	int m_iShowSpectra;
	UINT m_uGainConversion;
	char m_szIniFile[_MAX_PATH];
	CDSPEC50Inst *m_pParent;

	CXYCtrl   m_XYCtrl;

// Dialog Data
	//{{AFX_DATA(CDSPEC50DataStat)
	enum { IDD = IDD_DSPEC50DATASTATUS_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DSPEC50CDataStat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDSPEC50SafePropertySheet *m_pPropertySheet;
	// Generated message map functions
	//{{AFX_MSG(CDSPEC50DataStat)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnClearAndRestart();
	afx_msg void OnWriteCHNFile();
	afx_msg void OnInhibitRealtimeData();
	afx_msg void OnShowSpectra();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DSPEC50DATASTAT_H

