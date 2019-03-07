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
// HHMRModParam.h

#ifndef HHMRMODPARAM_H
#define HHMRMODPARAM_H

#include <io.h>
#include <direct.h>
#include "TPropertySheet.h"
#include "ISO.h"
#include "SimpleYellowButton.h" 

#define HHMR_DEFAULT "HHMR Default"
///////////////////////////////////////////////////////////////////////////
// CModParam dialog
class CHHMRModParam : public CPropertyPage
{
	DECLARE_DYNCREATE(CHHMRModParam)

// Construction
public:
	CHHMRModParam(UINT id, CISO* pParent, bool DoingConfig = false);
	CHHMRModParam();
	~CHHMRModParam();

//Variables
	bool m_bCollectingParametersForNewISO;
	bool m_bSaved;
	//On configuring a new HHMR, m_pPropertySheet is CTPropertySheet.
	//All other times, m_pPropertySheet is CHHMRPropertySheet.
	CTPropertySheet *m_pPS;
	COLORREF StaticColors[IDC_HHMRMODPARAMS_LAST-IDC_HHMRMODPARAMS_FIRST+1];
	bool m_bDataSetup;
	bool m_bChange;
	CISO* m_pParent;
	CSimpleYellowButton	*m_pApplyButton;
	CSimpleYellowButton *m_pApplyHHMRButton;  

//Functions
	void SetText(UINT, char *);
	void SetColor(UINT, COLORREF);
	void SetPropertySheet(CPropertySheet *pPS);//, char *pName = NULL);
	void IssuePrint(FILE *outfile);
	void WriteIniFileDefaultSection();
	void DoApply();
	void ResetAutoCloseTime();

// Dialog Data
	//{{AFX_DATA(CHHMRModParam)
	enum { IDD = IDD_HHMRMODPARAMS_PAGE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHHMRModParam)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

protected:

	void DoSpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void DoSpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, int DLGITEM);
	void DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM);
	void DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult,int MIN, int MAX, 
		int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above);
	void ResetItem(char *szName,char *szItem,char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItemI(char *szName,char *szItem,char*szDefault,
		int dItem,int dMin, int dMax, int dTextItem);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem,int dMin, int dMax);
	void ResetItem(char *szName,char *szItem, int dDefault,
		int dItem);
	void ResetItem(char *szName,char *szItem, char *szDefault, 
		int dItem);
	void ResetItemL(char *szName,char *szItem, int szDefault, 
		int dItem);
	void SaveItem(char *szName, char *szItem, int dDlgItem, bool FromCheckBox = false);
	void ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem);

	// Generated message map functions
	//{{AFX_MSG(CHHMRModParam)
	afx_msg void OnModparamsApply();
	afx_msg void OnModparamsApplyHHMR();
	afx_msg void OnModparamsBrowse();
	afx_msg void OnDeltaposModparamsHighvoltlSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsHighvolthSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDeltaposModparamsMostSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeModparamsMost();
	afx_msg void OnChangeModparamsCT();
	afx_msg void OnChangeModparamsSaveLoc();
	afx_msg void OnKillfocusModparamsFileid();
	afx_msg void OnChangeModparamsFileID();
	afx_msg void OnChangeModparamsPort();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	public:
	afx_msg void OnModparamsDefaultBut();
	afx_msg void OnModparamsDefault(bool SetPort = true);
	afx_msg void OnModparamsReset();

	DECLARE_MESSAGE_MAP()
};

#endif // HHMRMODPARAM_H