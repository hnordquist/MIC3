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
// RenameISO.cpp

#include "stdafx.h"
#include "resource.h"
#include "RenameISO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameISO dialog


CRenameISO::CRenameISO(CWnd* pParent /*=NULL*/)
	: CDialog(CRenameISO::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameISO)
	m_cNewName = _T("");
	m_cCurrentName = _T("");
	m_cOriginalName = _T("");
	m_iNewNode = 0;
	m_cExistingNode = _T("");
	m_cOriginalNode = _T("");
	//}}AFX_DATA_INIT
}


void CRenameISO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameISO)
	DDX_Text(pDX, IDC_EDIT_NEWNAME, m_cNewName);
	DDV_MaxChars(pDX, m_cNewName, 127);
	DDX_Text(pDX, IDC_STATIC_CURRENTNAME, m_cCurrentName);
	DDX_Text(pDX, IDC_STATIC_ORIGINALNAME, m_cOriginalName);
	DDX_Text(pDX, IDC_EDIT_NEWNODE, m_iNewNode);
	DDV_MinMaxInt(pDX, m_iNewNode, -1, 65535);  //MIC_CB_028 to MIC 1.907 PJM 1/20/05
	DDX_Text(pDX, IDC_STATIC_EXISTINGNODE, m_cExistingNode);
	DDX_Text(pDX, IDC_STATIC_ORIGINALNODE, m_cOriginalNode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameISO, CDialog)
	//{{AFX_MSG_MAP(CRenameISO)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameISO message handlers
