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
// DSPEC50SetEdit.h : header file

#ifndef DSPEC50SETEDIT_H
#define DSPEC50SETEDIT_H

#include "resource.h"

class CDSPEC50SetEdit : public CDialog
{
// Construction
public:
	CDSPEC50SetEdit(CWnd* pParent = NULL);   // standard constructor
	unsigned int m_uLength;
// Dialog Data
	//{{AFX_DATA(CDSPEC50SetEdit)
	enum { IDD = IDD_DSPEC50_DIALOG_EDIT };
	CEdit           m_cEdit;
	CSpinButtonCtrl m_cSpinButton;
	CComboBox       m_cComboBox;
	CString	m_cDesired;
	CString	m_cDiscription;
	CString	m_cReported;
	CString	m_cIniName;
	CString	m_cCommand;
	CString m_cAllowedValues;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPEC50SetEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void PopulateComboBox();
	void InitializeEditSpinBuddyPair();

	void CleanUpString(
		char *destination,
		char *source, 
		int destination_length,
		int digits_after_decimal = -1);

	bool m_bUsingComboBox;
	bool m_bIntegerOnly;

	double m_dSpinMax;
	double m_dSpinMin;
	double m_dSpinStepSize;
	int    m_iRightOfDecimal;

	// Generated message map functions
	//{{AFX_MSG(CDSPEC50SetEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg void OnDeltaposDSPEC50BuddySpin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // DSEPCSETEDIT_H
