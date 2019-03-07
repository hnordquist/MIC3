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
// SetEdit.cpp

#include "stdafx.h"
#include "resource.h"
#include "SetEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetEdit dialog


CSetEdit::CSetEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CSetEdit::IDD, pParent)
{
	m_uLength = 0;
	//{{AFX_DATA_INIT(CSetEdit)
	m_cDesired = _T("");
	m_cDiscription = _T("");
	m_cReported = _T("");
	m_cIniName = _T("");
	m_cCommand = _T("");
	//}}AFX_DATA_INIT
}

void CSetEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetEdit)
	DDX_Control(pDX, IDC_EDIT1, m_cEdit);
	DDX_Text(pDX, IDC_EDIT1, m_cDesired);
	DDV_MaxChars(pDX, m_cDesired, 128);
	DDX_Text(pDX, IDC_STATIC_DESCRIPTION, m_cDiscription);
	DDV_MaxChars(pDX, m_cDiscription, 128);
	DDX_Text(pDX, IDC_STATIC_REPORTED, m_cReported);
	DDV_MaxChars(pDX, m_cReported, 128);
	DDX_Text(pDX, IDC_STATIC_ININAME, m_cIniName);
	DDX_Text(pDX, IDC_STATIC_COMMAND, m_cCommand);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetEdit, CDialog)
	//{{AFX_MSG_MAP(CSetEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetEdit message handlers

BOOL CSetEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_uLength>0)
		m_cEdit.SetLimitText(m_uLength);
	if (strlen(m_cDesired) == 0)
	{
		m_cEdit.SetWindowText(m_cReported);
		m_cDesired = m_cReported;
	}
	m_cEdit.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
