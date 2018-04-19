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
 
#ifndef NEWETHERNETCOMM_H
#define NEWETHERNETCOMM_H

// file NewEthernetComm.h

#include "resource.h"
#include "ccbutton.h"

class CNewEthernetComm : public CDialog
{
	DECLARE_DYNCREATE(CNewEthernetComm)

// Construction
public:
	CNewEthernetComm(char *cIniFile, int iDlgClose, char *name);
	CNewEthernetComm();
	virtual ~CNewEthernetComm();

// Dialog Data

	//{{AFX_DATA(CNewEthernetComm)
	enum { IDD = IDD_ETHERNET_COLLECT_PARAMETERS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewEthernetComm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CYellowButton *m_pCreateButton;

	int m_dDlgCloseMilliseconds;
	int m_dParity;
	int m_dDataBits;
	int m_dStopBits;
	int m_dBaudRate;
	bool m_bHavePort;

	char m_pName[128];
	char m_szPort[32];
	char m_szIniFile[_MAX_PATH];

	CString m_csTitle;
	//int m_dCurSel_Serial;

	//void PopulateSerialPortList();

	// Generated message map functions
	//{{AFX_MSG(CNewEthernetComm)
	afx_msg void On96e71Button();
	afx_msg void On96n81Button();
	virtual void OnOK();
	afx_msg void OnBaudRadio();
	virtual BOOL OnInitDialog();
	afx_msg void OnDatabitsRadio();
	afx_msg void OnParityRadio();
	afx_msg void OnSelchangePortList();
	afx_msg void OnStopbitsRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // NEWETHERNETCOMM_H
