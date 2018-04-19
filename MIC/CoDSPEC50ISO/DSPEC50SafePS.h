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
// DSPEC50SafePS.h

#ifndef DSPEC50SAFEPS_H
#define DSPEC50SAFEPS_H

#include "CCButton.h"
#include "ORTECSpectrum.h"
#include "DSPEC50Parameters.h"
#include "JulianT.h"

class CDSPEC50Inst;
/////////////////////////////////////////////////////////////////////////////
// CDSPEC50SafePropertySheet
class CDSPEC50SafePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDSPEC50SafePropertySheet)

// Construction
public:
	CDSPEC50SafePropertySheet(
		UINT nIDCaption, 
		CDSPEC50Inst* pParentWnd, 
		char *inifile);

	CDSPEC50SafePropertySheet(
		LPCTSTR pszCaption, 
		CDSPEC50Inst* pParentWnd, 
		char *inifile);

	void FinishInstance();

	COLORREF SetButtonColor(COLORREF Color);

	void WriteToFile(
		int ToWhom, 
		int Type, 
		CJulianTime* Time = NULL, 
		const char* Message = NULL);

	void WriteSetupFailureToFile(int number, char *msg);

	bool WriteToCHNFile(CORTECSpectrum *pspec, char tag);
	void MakeMyPath(const char *);

	void SetLimits(int hvvolth, int hvvoltl);

	void CleanUpString(
		char *destination, 
		char *source, 
		int destination_length,
		int digits_after_decimal = -1);

	void SetTabColor(int Tab, COLORREF Color);

	void IssuePrint(FILE* outfile);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPEC50SafePropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDSPEC50SafePropertySheet();
	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CDSPEC50Inst		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentCHNFileName[_MAX_PATH];
	char			m_szIniFile[_MAX_PATH];

	virtual void	SetToClose(int set);
	virtual void	UpdateSummary(char *);
	void			UpdateDataStatus();

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

	DSPEC50_PARAMETERS m_ReportedParameters;

protected:
	int Data_Tab;
	int InstSetup_Tab;

	UINT    m_uLocalNonzero_channels;
	UINT    m_uLocalChannels_above_threshold;

	void CurrentSetParam(char*szIniName,char*szValue,bool bMatch);

	// Limits variables
	int m_dHVVoltH;
	int m_dHVVoltL;

	char  m_pCaption[256];

	//{{AFX_MSG(CDSPEC50SafePropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceiveCommandResponsePair(WPARAM command, LPARAM response);
	afx_msg void OnIPXMCBError(WPARAM WParam, LPARAM LParam);
	afx_msg void OnExtendClose(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



};

#endif // DSPEC50SAFEPS_H
