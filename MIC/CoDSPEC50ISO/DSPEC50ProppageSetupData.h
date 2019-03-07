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

#ifndef DSPEC50PROPPAGESETUPDATA_H
#define DSPEC50PROPPAGESETUPDATA_H

// DSPEC50ProppageSetupData.h : header file
//
#include <io.h>
#include <direct.h>

#include "DSPEC50SafePS.h"
//#include "TPropertySheet.h"
#include "ListCtrlStyled.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50ProppageSetupData property page

class CDSPEC50ProppageSetupData : public CPropertyPage
{
	DECLARE_DYNCREATE(CDSPEC50ProppageSetupData)

// Construction
public:
//	CDSPEC50ProppageSetupData(unsigned int id, CWnd* pParent, bool DoingConfig);
	CDSPEC50ProppageSetupData(CWnd* pParent = NULL);
	CDSPEC50ProppageSetupData(CWnd* pParent, char *infile);
	~CDSPEC50ProppageSetupData();

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void ResetColors();
//	CTPropertySheet *m_pPS;
	CDSPEC50SafePropertySheet *m_pPS;
	void IssuePrint(FILE *outfile);
	CDSPEC50Inst *m_pParent;

// property page Data
	//{{AFX_DATA(CDSPEC50ProppageSetupData)
	enum { IDD = IDD_DSPEC50_SETTINGS_DIALOG };
	CButton	m_cEditSelected;
	CListCtrlStyled	m_cList;  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPEC50ProppageSetupData)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();

	void AddSetParam(
		char*szDesc,
		char*szIniName,
		char*szSetting,
		char*szCommand,
		char*szAllowedValues,
		unsigned int uLength);

	void CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch=true);

protected:
	char m_pName[32];
//	CWnd* m_pParent;
	char m_szIniFile[_MAX_PATH];
	void SaveAll(bool bDoingConfig);

	// Generated message map functions
	//{{AFX_MSG(CDSPEC50ProppageSetupData)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditselection();
	afx_msg void OnButtonSaveallreported();
	afx_msg void OnButtonLoaddefault();
	afx_msg void OnButtonRefreshreported();
	afx_msg void OnButtonSenddesired();
	afx_msg void OnDblclkListSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // DSPEC50PROPPAGESETUPDATA_H
