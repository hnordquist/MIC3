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
// Config.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "globals.h"
#include "ports.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfig dialog

CComPortList::CComPortList(CWnd* pParent, CString ** p)
	: CDialog(CComPortList::IDD, pParent)
{
	theListOfPorts = p;
}
CComPortList::~CComPortList()
{

}
void CComPortList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfig)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PORTLIST, meinpops);
}

BEGIN_MESSAGE_MAP(CComPortList, CDialog)
	//{{AFX_MSG_MAP(CConfig)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfig message handlers

void CComPortList::OnOK() 
{
	CDialog::OnOK();
}


BOOL CComPortList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//clock to update the total bbm
	SetTimer(2,1000,NULL);

	//clock to auto close the dialog
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	if (theListOfPorts != NULL)
	{
		for (int myi = 0; myi < MAX_COMM; myi++)
		{
			if (theListOfPorts[myi])
			{
				LVITEM lvI;
				::ZeroMemory(&lvI, sizeof(LVITEM));
				lvI.pszText   = theListOfPorts[myi]->GetBuffer(); 
				lvI.mask      = LVIF_TEXT;
				meinpops.InsertItem(&lvI);
			}
		}
		//meinpops.
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


//catch timer messages
void CComPortList::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 2)
	{
		ULONG ulong;
		int iTotal = 0;
		for (int i = 0; i < g_iInstCount; i++)
		{
			g_pIISO[i]->get_CurrentBBM(&ulong);
			iTotal += (int)ulong;
		}
		SetDlgItemInt(IDC_STATIC_BBMTOTAL,iTotal);
	}

	//if this is the timeout event, then cancel this dialog
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}

