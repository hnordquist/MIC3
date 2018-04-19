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
// TestInst.h

#if !defined(AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
#define AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TestInst.h : header file
//
#include <commctrl.h>
#include <afxtempl.h>
#include "CSO.h"
#include "ISO.h"
#include "TestDlg.h"

#define WATCH_DISCOVERY_TYPE "WATCH"

/////////////////////////////////////////////////////////////////////////////
// CTestInst window

class CTestInst : public CISO
{
	// Construction
public:
	CTestInst(CWnd *parent, char *inifile);
	CTestInst();
	virtual ~CTestInst();

// Attributes
public:

	virtual bool  CollectParametersForNewISO(CString& csNameOut);
	virtual bool  CreateInstrument(void);
	virtual bool  UnRegInstrument();
	virtual void  ShowInstrument(bool);
	virtual void  PauseInstrument(bool){;}
	virtual bool  UpdateParams(bool){return true;}
	virtual void  UpdateStrings(){;}
	virtual void  DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	virtual void  IssuePrint(FILE* outfile){;}
	virtual void  Reset(){;}
	virtual void  BlockWrite(bool bw){m_bBlockWrite = bw;}
	virtual char *GetSaveLoc(){return NULL;}
	virtual void  SetLocalRegisteredMessageValues();

	CTestDlg	*m_pTestDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestInst)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CTestInst)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTINST_H__0B89D6C1_9D71_11D1_B39F_006008A70A6A__INCLUDED_)
