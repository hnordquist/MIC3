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
//aboutdlg.cpp

#include "stdafx.h"
#include "AboutDlg.h"
#include "globals.h"
#include "mic_definitions.h"
#include "version.h"
#include "newcomm.h"
#include "newinst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetTimer(1,g_iDlgCloseMillisec,NULL);

/*
	CVersionInfo VersionInfo;
	CString Name    = VersionInfo.InternalName();
	CString Version = VersionInfo.FileVersion();
	CString PrivateBuild = VersionInfo.PrivateBuild();

	Version.Remove(' ');
	Version.Replace(',','.');

	//Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	//CString cs = Name + " " + Version + " - " + PrivateBuild +" - " + __DATE__;
	CString cs = CString(Name) +" " + Version + " "  + __DATE__;
	SetDlgItemText(IDC_STATIC_VERSION, cs);
*/
	SetDlgItemText(IDC_STATIC_VERSION, g_csNameVersionDate);  //SCR 118

#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "");
#endif

	CNewComm cNewComm(this);									//SCR000118
	CNewInst cNewInst(this);									//SCR000118
	CString cs;													//SCR000118
	CString csOut("REGISTERED MIC COMPONENTS\r\n\r\nCSOs (communications):\r\n");//SCR000118
	bool bDoGUI = false;										//SCR000118

	bool bOK = cNewComm.DoDynamicDiscovery(bDoGUI, &cs);		//SCR000118
	if (bOK)													//SCR000118
		csOut += cs;											//SCR000118

	csOut += "\r\nISOs (instruments):\r\n";									//SCR000118

	cs.Empty();													//SCR000118
	bOK = cNewInst.DoDynamicDiscovery(bDoGUI, &cs);				//SCR000118
	if (bOK)													//SCR000118
		csOut += cs;											//SCR000118

	csOut += "\r\nOTHERS:\r\n";

	csOut += g_csColorButtonVersionString;						//SCR000118

	SetDlgItemText(IDC_EDIT1,csOut);							//SCR000118

	return TRUE;
}

void CAboutDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnOK();
	}
	CDialog::OnTimer(nIDEvent);
}