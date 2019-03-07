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
// TestInst.cpp

#include "stdafx.h"
#include "resource.h"
#include "TestInst.h"
#include "TestDlg.h"
#include "colors.h"
#include "INI_definitions.h"
#include "NewWatchWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CTestInst
//////////////////////////////////////////////////////////////////////////////
//
//   Constructors, Destructor, Message Map

CTestInst::CTestInst(void)
{
//	TRACE("CTestInst::CTOR #1\n");
	//m_pParent = NULL;
	m_pTestDlg= NULL;
	strncpy(m_pType,WATCH_DISCOVERY_TYPE, sizeof(m_pType));
	memset(m_szFileExtensionsList,0,sizeof(m_szFileExtensionsList));
	m_eCategory = WATCH_ISO;
}

CTestInst::CTestInst(CWnd* Parent, char *inifile) : CISO(inifile)
{
//	TRACE("CTestInst::CTOR #2\n");
	//m_pParent = Parent;
	m_pTestDlg= NULL;
	strncpy(m_pType, WATCH_DISCOVERY_TYPE, sizeof(m_pType));
	memset(m_szFileExtensionsList,0,sizeof(m_szFileExtensionsList));
	m_eCategory = WATCH_ISO;
}

CTestInst::~CTestInst()
{
//	TRACE("CTestInst::DTOR\n");
	//destroy the dialog box
	if (m_pTestDlg)
	{
		m_pTestDlg->DestroyWindow();
		delete m_pTestDlg;
		m_pTestDlg = NULL;
	}
}

BEGIN_MESSAGE_MAP(CTestInst, CWnd)
	//{{AFX_MSG_MAP(CTestInst)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
//
//                 CreateInstrument - 3 signatures
//
//////////////////////////////////////////////////////////////////////////////
bool CTestInst::CreateInstrument()
{
//	TRACE("CTestInst::CreateInstrument #1(%s)\n",m_pName);
//	ASSERT(m_pName != NULL);
//	ASSERT(m_pButton != NULL);

	//<<QA>>char buff[32];
	//char port[32];

	//get the node string from the ini file
	//<<QA>>GetPrivateProfileString(m_pName,NODE,"",buff,sizeof(buff),m_szIniFile);
	//<<QA>>m_iNode = atoi(buff);
	m_iNode = GetPrivateProfileInt(m_pName,NODE,-1,m_szIniFile);


	//get the port string from the ini file
	GetPrivateProfileString(m_pName,PORT,"",m_pPort,sizeof(m_pPort),m_szIniFile);

	//create CTestInst object -- we won't actually have a window
	//the window will be in a non-modal dialog box 
	CString cs = AfxRegisterWndClass(NULL, NULL, NULL, NULL);

	BOOL result = CreateEx(
			0,               //DWORD dwExStyle, 
			(LPCTSTR)cs,     //LPCTSTR lpszClassName, 
			m_pName,         //LPCTSTR lpszWindowName, 
			0,               //DWORD dwStyle, 
			CRect(0,0,0,0),  //const RECT& rect, 
			NULL,            //CWnd* pParentWnd, 
			0                //UINT nID, 
			);               //LPVOID lpParam = NULL);

	if (result) 
	{
		//Start with the instrument paused and set the face color of the button
		//Don't care about the radix input
		m_pButton->put_DisplayNumericString(VARIANT_FALSE,0);
		m_pButton->put_FaceColor(COLOR_WHITE);

		//create the user interface dialog box for this instrument
		m_pTestDlg = new CTestDlg(this,m_pComm,m_iNode);
		BOOL B = m_pTestDlg->Create(IDD_WATCH_DIALOG,this);

		CreateFullNameString();
		m_pTestDlg->SetWindowText(m_pFullName);

		VARIANT_BOOL vbresult;

		m_pComm->RegisterISO(
			(ULONG)m_pTestDlg->m_hWnd, //[in] ULONG receiver_handle, 
			(ULONG)m_iNode,                 //[in] ULONG Node,
			WATCH_ISO,                      //[in] ISO_FUNCTIONAL_CATEGORY cat,
			VARIANT_FALSE,                  //[in] VARIANT_BOOL vb_OOB, 
			&vbresult);                     //[out] VARIANT_BOOL *pvbResult);

		//SCR 118
		//EstablishVersionString();

		result = (vbresult == VARIANT_TRUE);
	}

	return result?true:false;
}

bool CTestInst::UnRegInstrument()
{
	//This is different from the base class version
	//of UnRegInstrument();
//	TRACE("CTestInst::UnRegInstrument()\n");
	if (m_pComm)
		m_pComm->UnRegisterISO((ULONG)m_iNode, WATCH_ISO);
	return true;
}

void CTestInst::DriveToZero(bool bDriveToZero, bool bInPwrFail)
{
//	TRACE("CTestInst::DriveToZero()\n");
	if (bInPwrFail)
	{
		char szBuff[128];
		strcpy(szBuff,"\r\nCOLLECT Power Fail Drive BBM to Zero.\r\n");
		unsigned char* tbuff = (unsigned char*)CoTaskMemAlloc(strlen(szBuff)+1);
		//m_pTestDlg->OnReceiveOOB((strlen(szBuff)+1),(LPARAM)tbuff);
		m_pTestDlg->SendMessage(uIDC_RECEIVE_OOB,(strlen(szBuff)+1),(LPARAM)tbuff);
	}
	PauseInstrument(bDriveToZero);
}
			
void CTestInst::ShowInstrument(bool show)
{
//	TRACE("CTestInst::ShowInstrument()\n");

	//if attempting to show instrument then do it and bring it to top
	if (show) 
	{
		m_pTestDlg->ShowWindow(SW_SHOW);
		m_pTestDlg->BringWindowToTop();
	} 
	else
	{
		m_pTestDlg->ShowWindow(SW_HIDE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestInst message handlers
void CTestInst::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////
//
//                           CollectParametersForNewISO
//
//////////////////////////////////////////////////////////////////////////////
bool CTestInst::CollectParametersForNewISO(CString& csNameOut)
{
//	TRACE("CTestInst::CollectParametersForNewISO()\n");

	//This ISO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new ISO from that data.

	bool bReturn;

	//Collect the data for the new instrument.
	CNewWatchWindow dlg(m_szIniFile,m_iDlgCloseMilliseconds,m_pName);

	int result = dlg.DoModal();

	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		WritePrivateProfileString(INSTRUMENTS,m_pName,WATCH_DISCOVERY_TYPE,m_szIniFile);
		bReturn = true;
	}
	else
	{
		csNameOut.Empty();
		bReturn = false;
	}
	
	return bReturn;
}

void CTestInst::SetLocalRegisteredMessageValues()
{
//	TRACE("CTestInst::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

BOOL CTestInst::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::PreTranslateMessage(pMsg);
}
