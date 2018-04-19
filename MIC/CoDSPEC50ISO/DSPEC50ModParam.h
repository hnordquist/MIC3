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
// DSPEC50ModParam.h

#ifndef DSPEC50MODPARAM_H
#define DSPEC50MODPARAM_H

#include "TPropertySheet.h"
#include "ccbutton.h"
#include "resource.h"
#include "ISO.h"
/////////////////////////////////////////////////////////////////////////////
// CDSPEC50ModParam dialog
class CDSPEC50ModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPEC50ModParam)

// Construction
public:
	CDSPEC50ModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CDSPEC50ModParam();
	~CDSPEC50ModParam();

	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void SetPropertySheet(CPropertySheet *pPS);

	void SaveSetup();
	bool m_bCollectingParametersForNewISO;
	bool m_bSaved;
	bool m_bAllowFinish;
	CTPropertySheet *m_pPS;
	CYellowButton	*m_pApplyButton;

	COLORREF StaticColors[IDC_DSPEC50MODPARAMS_LAST-IDC_DSPEC50MODPARAMS_FIRST+1];
	void IssuePrint(FILE *outfile);

	void SaveValuesToINIFile(char *name);
	void ResetAllButPort(char *ini_section_name);
	void WriteIniFileDefaultSection();

// Dialog Data
	//{{AFX_DATA(CDSPEC50ModParam)
	enum { IDD = IDD_DSPEC50MODPARAMS_PAGE };
//	CString	m_cSaveLoc;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDSPEC50ModParam)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnInitDialog();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();

protected:
	bool m_datasetup;
	bool m_bChange;
	CISO* m_pParent;
	void MakeMyPath(const char *path);

	void DoSpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void DoSpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM);
	void DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);

	void ResetItem(char *szName, char *szItem, char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItemI(char *szName, char *szItem, char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem, int dMin, int dMax);
	void ResetItem(char *szName, char *szItem, int dDefault,
		int dItem);
	void ResetItem(char *szName,char *szItem, char *szDefault, 
		int dItem);
	void SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox = false);
	void ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem);

public:
	// Generated message map functions
	//{{AFX_MSG(CDSPEC50ModParam)
	afx_msg void OnModparamsReset();
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);

	//Commfail
	afx_msg void OnDeltaposModparamsCommfailSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Fileid
	afx_msg void OnChangeModparamsFileid();
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusModparamsFileid();
	//HV fail
	afx_msg void OnDeltaposModparamsHVfailSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//HV high error limit
	afx_msg void OnDeltaposModparamsHVhigherrorlimitSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//HV low error limit
	afx_msg void OnDeltaposModparamsHVlowerrorlimitSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Longdwell
	afx_msg void OnDeltaposModparamsLongdwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Maxcycle is constant at 1 second
	//Maxpause
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Maxstatus
	afx_msg void OnDeltaposModparamsMaxstatusSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Node
	//Port
	afx_msg void OnChangeModparamsPort();
	//Realtimedwell
	afx_msg void OnDeltaposModparamsRealtimedwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Saveloc
	afx_msg void OnChangeModparamsSaveloc();
	//Shortdwell
	afx_msg void OnDeltaposModparamsShortdwellSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Slowreset
	afx_msg void OnDeltaposModparamsSlowresetSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Threshold
	afx_msg void OnDeltaposModparamsThresholdSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//Timeout
	afx_msg void OnDeltaposModparamsTimeoutSpin(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMost();

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // DSPEC50MODPARAM_H
