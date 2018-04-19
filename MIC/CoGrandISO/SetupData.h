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
// SetupData.h

#if !defined(AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_)
#define AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <io.h>
#include <direct.h>
#include "resource.h"
#include "TPropertySheet.h"
#include "ListCtrlStyled.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupData property page

class CSetupData : public CPropertyPage
{
	DECLARE_DYNCREATE(CSetupData)

// Construction
public:
	CSetupData();
	CSetupData(CWnd* pParent, char *inifile);
	~CSetupData();

	void SetPropertySheet(CPropertySheet *pPS, char *pName = NULL);
	void ResetColors();
	void UpdateColors(COLORREF* cColors);
	CTPropertySheet *m_pPS;
	void SetVersion(char* szVersion){strcpy(m_szVersion,szVersion);};
//	void IssuePrint(FILE *outfile,UINT iWidth=32);
	CString GetMatchString(){return m_cMatchString;};

// property page Data
	//{{AFX_DATA(CSetupData)
	enum { IDD = IDD_GRANDSETUP_DIALOG };
	CButton	m_cEditSelected;
	CListCtrlStyled* m_pList;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupData)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DoApply();
	void AddSetParam(char*szDesc,char*szIniName,char*szSetting,char*szCommand,unsigned int uLength);
	bool CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch=true, bool bTextOnly=true);

	char m_szIniFile[_MAX_PATH];

protected:
	CString m_cMatchString;
	char m_szVersion[16];
	char m_pName[32];
	CWnd* m_pParent;
	void SaveAll(bool bDoingConfig);

	// Generated message map functions
	//{{AFX_MSG(CSetupData)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonEditselection();
	afx_msg void OnButtonSaveall();
	afx_msg void OnButtonLoaddefault();
	afx_msg void OnDblclkListSet(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSet(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDATA_H__9B6BF85B_A45B_4102_A7EB_807B89700300__INCLUDED_)
