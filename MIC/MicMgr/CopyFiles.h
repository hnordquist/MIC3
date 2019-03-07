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

// User: Moore        Date: 11/14/05   Time: 3:19p
// Repaired DR00159 Not all drives are selectable in the Copy Files


#ifndef COPYFILES_H
#define COPYFILES_H

// CopyFiles.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCopyFiles dialog

class CCopyFiles : public CDialog
{
// Construction
public:
	CCopyFiles(CWnd* pParent = NULL);
	FILE* m_pIPFile;
// Dialog Data
	//{{AFX_DATA(CCopyFiles)
	enum { IDD = IDD_COPYFILES1_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	public:
	virtual int DoModal(CFileMgr* pFileMgrParent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFileMgr* m_pFileMgrParent;
	COleDateTime theTime;
	CString m_pDateTime;
	CString m_pInspectorName;
	CString m_pInspectorID;
	CString m_pInspection;
	CString m_pDestination;
	CString m_pPrevious;
	HICON m_hInspectState;
	HICON m_hDumpState;
	HICON m_hMediaState;
	HICON m_hDestState;
	HICON m_hIconYellow1;
	HICON m_hIconYellow2;
	HICON m_hIconRed;
	HICON m_hIconGreen;
	void InspectionIconUpdate();
	int   m_dTotalBytesinBBM;
	int	  m_dToClose;
	bool  m_bDoingDoFileCopy;

	// Generated message map functions
	//{{AFX_MSG(CCopyFiles)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnDrive1Radio();
	afx_msg void OnDrive2Radio();
	afx_msg void OnDrive3Radio();
	afx_msg void OnDrive4Radio();
	afx_msg void OnDrive5Radio();
	afx_msg void OnDrive6Radio();
	afx_msg void OnDrive7Radio();
	afx_msg void OnDrive8Radio();
	afx_msg void OnChangeInspectionEdit();
	afx_msg void OnChangeInspectoridEdit();
	afx_msg void OnChangeInspectornameEdit();
	afx_msg void OnDrivedefRadio();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTillcloseButton();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnButtonCfBrowse();  //DR00159
	afx_msg LRESULT OnQueryCancelAutoPlay(WPARAM wParam, LPARAM lParam);//DR00175
	//}}AFX_MSG
	void OnDriveRadio(UINT,int);
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);	//DR00175
	void TestDrive();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // COPYFILES_H
