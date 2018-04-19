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
//FileMgr.h
#ifndef FILEMGR_H
#define FILEMGR_H

// FileMgr.h : header file
//

#include "COM_Utilities.h"

struct FileRec 
{
	FILETIME Date;
	bool Deleted;
	int Size;
	char *PathName;
	FileRec* Older;
	FileRec* Newer;
};

class CMicMgrDlg;

/////////////////////////////////////////////////////////////////////////////
// CFileMgr thread

class CFileMgr : public CWinThread
{
	DECLARE_DYNCREATE(CFileMgr)
protected:
	CFileMgr();           // protected constructor used by dynamic creation

// Attributes
public:
	virtual ~CFileMgr();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileMgr)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	void DoTimeCheck(WPARAM,LPARAM);
	void DoCopyData(WPARAM,LPARAM);
	void DoArchiveCheck();
	int  m_dTimeInCopy;
	int  m_dTimeFromAge;
	void DoShutdown(WPARAM,LPARAM);

	void SetParentPointer(CMicMgrDlg *p);
	void SetIDC_COM_STOPRegisteredMessageValue(UINT value);

	CMicMgrDlg *m_pMicMgrDlgParent;

// Implementation
protected:

	FILE* OutFile;
	FileRec* Head;
	FileRec* CreateRec(char* Path,WIN32_FIND_DATA FindFileData);
	void Insert(FileRec* NewRec, FileRec* At);
	void CleanUp(FileRec* At);
	__int64 VisitSize(FileRec* At, __int64 BytesToDelete, FILE* OutFile = NULL);
	void VisitAge (FileRec* At, COleDateTime TodayMx, FILE* OutFile = NULL);

	bool m_bDoingCopyFile;
	bool m_bDoingAgeFile;
	// Generated message map functions
	//{{AFX_MSG(CFileMgr)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // FILEMGR_H
