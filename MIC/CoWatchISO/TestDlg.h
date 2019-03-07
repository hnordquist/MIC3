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
// TestDlg.h

#if !defined(AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
#define AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TestDlg.h : header file
//
#include "CCButton.h"
#include "../Interfaces/ICSO.h"
#include "../Interfaces/IISO.h"
#include "resource.h"
#include "EditSend.h"
#include <afxcmn.h>

class CTestInst;

#define MAX_CEDIT_HIGH 120000
#define MAX_CEDIT_DECR 5000

#define COMMANDSTACKSIZE 10

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg : public CDialog
{
	//DECLARE_DYNAMIC(CTestDlg)

// Construction
public:
	CTestDlg(CWnd* pParent, ICSO* pComm, int Node );
// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_WATCH_DIALOG };
	CProgressCtrl   m_Progress;
	CSpinButtonCtrl m_EditSpin;
	CEdit           m_EditDelay;
	CEditSend	    m_pSend;
	CEdit           m_pReceive;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestDlg();

protected:
	CTestInst* m_pParent;
	ICSO    *m_pComm;
	FILE    *m_Finfile;
	long	m_dFileSize;
	int		m_dTotalBytesRead;
	int		m_dBytesRemainingToSend;
	int		m_dBytesSent;
	bool	m_bDoingTransfer;
	int		m_dNode;
	bool	m_bNl;
	int		m_dLength;
	char	m_cLastChar;
	char	m_cTempBuf[2048];
	bool	m_bIn33;
	int		m_dIn33Count;
	int		m_dPostBEEFCount;
	bool	m_bInitialized;
	CColorButton *m_pCloseButton;
	CFont        EditFont;
	CRect        m_OldRect;
	CScrollBar   m_Grip;
	CString m_cLastCommands[COMMANDSTACKSIZE];
	int		m_iLastCommand;

	int DLG_COM_Transmit(int ilength,char *output,int Node, bool Quiet = false);  //<<QA>>

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	virtual void OnOK();
	afx_msg void OnTestCloseButton();
	afx_msg void OnTestPrintButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnTestClearButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTestSendfileButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditDelay();
	afx_msg void OnRadioMost();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReceive(WPARAM, LPARAM);
	afx_msg void OnReceiveOOB(WPARAM,LPARAM);
	afx_msg void OnSysChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnSpecialKey(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__0B89D6C2_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
