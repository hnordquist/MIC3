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
//DeleteIns.cpp 

#include "stdafx.h"
#include "resource.h"
#include "DeleteIns.h"
#include "resource.h"
#include "validate.h"
#include "globals.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteIns dialog
// delete instrument

CDeleteIns::CDeleteIns(CWnd* pParent)
	: CDialog(CDeleteIns::IDD, pParent)
{
	m_dSelect = -1;
	m_dCount  =  0;
	m_pDeleteButton = NULL;
}
CDeleteIns::~CDeleteIns()
{
	delete m_pDeleteButton;
}


void CDeleteIns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteIns)
	DDX_Control(pDX, IDC_DELETE_LIST, m_DeleteList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteIns, CDialog)
	//{{AFX_MSG_MAP(CDeleteIns)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_DELETE_LIST, OnSelchangeDeleteList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteIns message handlers

void CDeleteIns::OnOK() 
{
	CValidate Valid(this);

    if (g_bHideWindows)
		ShowWindow(SW_HIDE);

	if (Valid.DoModal() == IDOK) 
	{
		//get the selection -- parent will do the 
		//instrument delete for us 
		int temp;
		if ((temp = m_DeleteList.GetCurSel()) != LB_ERR)
			m_dSelect = m_DeleteList.GetItemData(temp);

		BSTR bstr;
		g_pIISO[m_dSelect]->get_Name(&bstr);
		UINT count = 1 + SysStringLen(bstr);

		char *name = (char *)CoTaskMemAlloc((ULONG)count);
		Convert_BSTR_to_Character_String(bstr,name,count);

		AfxGetMainWnd()->PostMessage(IDC_DELETE_INST, NULL,(WPARAM)name);

		CDialog::OnOK();
	}
	//show this window again
	ShowWindow(SW_SHOW);
}

void CDeleteIns::AddString(char* Data)  //<<QA>> delete this
{
	//add an instrument to the list
	//this is done after instantiating the dialog
	//box and before do modal is called
	char *data = (char *)malloc(strlen (Data)+1);
	strcpy(data,Data);
	m_DeleteData[m_dCount] = data;
	m_dCount++;
}

BOOL CDeleteIns::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(false);
	SetTimer(1,g_iDlgCloseMillisec,NULL);

	//set the tab stops
	int tabs[2] = {60,85};
	m_DeleteList.SetTabStops(2,tabs);	
	
	char string_out[64];
	int index;
	BSTR bstr;

	for (int i = 0; i < g_iInstCount; i++)
	{
		g_pIISO[i]->get_FullName(&bstr);
		Convert_BSTR_to_Character_String(bstr,string_out,sizeof(string_out));
		index = m_DeleteList.AddString(string_out);
		m_DeleteList.SetItemData(index,i);
	}
	
	//create delete button
	m_pDeleteButton = new CYellowButton("Delete",IDOK,g_szIniFile,this);

	return TRUE;
}

void CDeleteIns::OnDestroy() 
{
	//on quit get rid of all the data
	CDialog::OnDestroy();
	for (int i = 0; i < m_dCount; i++)
		delete [] m_DeleteData[i];
}

void CDeleteIns::OnTimer(UINT nIDEvent) 
{
	//catch the timer to auto-close this dialog box
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDeleteIns::OnSelchangeDeleteList() 
{
	//enable "ok" button if valid current selection
	if (m_DeleteList.GetCurSel() != LB_ERR)
		m_pDeleteButton->EnableWindow(TRUE);
	else
		GetDlgItem(IDOK)->EnableWindow(FALSE);
}
