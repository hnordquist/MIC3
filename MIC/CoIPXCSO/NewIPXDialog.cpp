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
// NewIPXDialog.cpp : implementation file

#include "stdafx.h"
#include "NewIPXDialog.h"
#include "TimedMessageDialog.h"
#include "ini_definitions.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewIPXDialog dialog


CNewIPXDialog::CNewIPXDialog(char *inifile, int dlgclose, char *name, CWnd* pParent /*=NULL*/)
	: CDialog(CNewIPXDialog::IDD, pParent)
{
//	TRACE("CNewIPXDialog::CTOR\n");
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	m_iDlgCloseMilliseconds = dlgclose;
	m_pchName = name;

	m_pCreateButton = NULL;

	m_csTitle = CString("IPX CSO") + CString(" - ");
	m_csTitle += CString(": Create Comm");

	//{{AFX_DATA_INIT(CNewIPXDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CNewIPXDialog::~CNewIPXDialog()
{
	delete m_pCreateButton;
}

void CNewIPXDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewIPXDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewIPXDialog, CDialog)
	//{{AFX_MSG_MAP(CNewIPXDialog)
	ON_LBN_SELCHANGE(IDC_PORT_LIST, OnSelchangePortList)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewIPXDialog message handlers


void CNewIPXDialog::OnSelchangePortList() 
{
//	TRACE("CNewIPXDialog::OnSelchangePortList()\n");
	// user clicked on one of the port selections in the list box
	// so find out which is the current selection

	KillTimer(1);  //start counting over for closing dialog automatically.
	SetTimer(1,m_iDlgCloseMilliseconds,NULL);

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);
	int cursel = plistbox->GetCurSel();

	if (cursel == LB_ERR)
		m_bHavePort = false;
	else
		m_bHavePort = true;

	CString cs;
	plistbox->GetText(cursel,cs);

	SetDlgItemText(IDC_NAME_STATIC,cs);

	m_pCreateButton->EnableWindow(m_bHavePort);
}

BOOL CNewIPXDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	TRACE("CNewIPXDialog::OnInitDialog()\n");

	m_pCreateButton = new CYellowButton("Create",IDOK,m_szIniFile,this);

	bool bPortInfoFound = DiscoverIPXPortInformation();

	if(bPortInfoFound)
	{
		PopulateIPXDetectorList();
		m_bHavePort = false;
		m_pCreateButton->EnableWindow(m_bHavePort);
		SetTimer(1,m_iDlgCloseMilliseconds,NULL);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

bool CNewIPXDialog::DiscoverIPXPortInformation()
{
//	TRACE("CNewIPXDialog::DiscoverIPXPortInformation()\n");

	//get the name of the ini file with the IPX port information
	char mccon32_ini_name[_MAX_PATH];
	GetPrivateProfileString("IPX","CONFIG_FILE","",
		mccon32_ini_name,sizeof(mccon32_ini_name),m_szIniFile);

	if (strlen(mccon32_ini_name) == 0)
	{
			char path[_MAX_PATH];
			CString configfile;//"CONFIG_FILE=");
			UINT numchars = GetWindowsDirectory(path,_MAX_PATH);
			configfile += CString(path);
			configfile += "\\";
			configfile += CString("MCBCON32.INI");
			strcpy(mccon32_ini_name, (char *)(LPCSTR(configfile)));
	}

	bool success = false;

	if (access(mccon32_ini_name,0)!=0)  //if the file does not exist... 
		{
			CString substring("IPX Setup Error");
			CString csText;

			csText = CString ("Could not find IPX configuration file. "\
						"Run ORTEC MCB configuration program "\
						"McbCon32.exe to create IPX config file.");

			new CTimedMessageDialog(substring, csText,m_iDlgCloseMilliseconds/1000,this);
		}

	else if (access(mccon32_ini_name,4)==0) //if the file is readable
		{
			//read the whole thing and write into our MIC.INI file
			char ipx_ini_section[1024];

			//DETS section
			memset(ipx_ini_section,0,sizeof(ipx_ini_section));
			GetPrivateProfileSection("DETS",ipx_ini_section,sizeof(ipx_ini_section),mccon32_ini_name);
			WritePrivateProfileSection("IPX_DETECTORS",ipx_ini_section,m_szIniFile);

			//GENERAL section
			memset(ipx_ini_section,0,sizeof(ipx_ini_section));
			GetPrivateProfileSection("GENERAL",ipx_ini_section,sizeof(ipx_ini_section),mccon32_ini_name);
			WritePrivateProfileSection("IPX_GENERAL",ipx_ini_section,m_szIniFile);

			//Determine if there are any IPX ports in the system.
			m_iNumber_of_IPX_detectors = GetPrivateProfileInt("IPX_DETECTORS","MAX",0,m_szIniFile);

			if (m_iNumber_of_IPX_detectors != 0)
			success = true;
		}

	return success;
}

void CNewIPXDialog::PopulateIPXDetectorList()
{
//	TRACE("CNewIPXDialog::PopulateIPXDetectorList()\n");

	//clear the port list
	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);
	if (0 != plistbox->GetCount())
	{
		do
		{
			plistbox->DeleteString(0);
		} while (0 != plistbox->GetCount());
	}

	if (m_iNumber_of_IPX_detectors <= 0)  //not supposed to be zero if we get this far.
		return;

	char keynamebuf[32] = "NAME00x";
	char detectornamebuf[32];
		char temp[32];

	//<<QA>> this breaks if val is greater than 9...
	for (int val = 1; val <= m_iNumber_of_IPX_detectors; val++)
	{
		keynamebuf[6] = (char)(val + '0');
		GetPrivateProfileString("IPX_DETECTORS",keynamebuf,"",
			detectornamebuf,sizeof(detectornamebuf),m_szIniFile);

		//only show those that are available by
		//checking if a communications object by this name already exists
		GetPrivateProfileString(COMMUNICATIONS,detectornamebuf,"ERROR",temp,sizeof(temp)-1,m_szIniFile);
		if (strcmp(temp,"ERROR")==0)
			plistbox->AddString(detectornamebuf);
	}
}


void CNewIPXDialog::OnOK() //This is for the "create" button
{
//	TRACE("CNewIPXDialog::OnOK()\n");

	char name_from_static_control[32];
	char buff2[32];
	CString csText;
	
	GetDlgItemText(
		IDC_NAME_STATIC,
		name_from_static_control,
		sizeof(name_from_static_control));

	if (m_pchName[0] == ';')
	{
		csText = CString("A \"Name\" for a communications object cannot start with \";\".");
		new CTimedMessageDialog(m_csTitle, csText,m_iDlgCloseMilliseconds/1000,this);
		return;
	}

	//comm objects cannot have the same name
	GetPrivateProfileString(
		COMMUNICATIONS,
		m_pchName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("A Communications object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_iDlgCloseMilliseconds/1000,this);
		return;
	}

	//because we are using INI sections to save the information
	//we cannot have the same name as an instrument section either
	GetPrivateProfileString(
		INSTRUMENTS,
		m_pchName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("An Instrument object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_iDlgCloseMilliseconds/1000,this);
		return;
	}

	//get the port from the port list
	//For an IPX CSO, the "port" is the the index in the list that is 
	//provided to us in the MCBCON32.INI file.  The first index is 1, not 0.

	int port;
	char szIPXnode[32];

	//get port number from the first character(s) of the name string
	//port number and node are the same for IPX

	memcpy(szIPXnode,name_from_static_control,sizeof(szIPXnode));
	strtok(szIPXnode," ");  //chop off at space -  "1 DSPP-359" turns into "1"
	sscanf(szIPXnode,"%d",&port);

	//<<QA>> danger here........  
	memcpy(m_pchName,name_from_static_control,32);  //<<QA>> have this "size" as an input.
	
	CDialog::OnOK();
}

void CNewIPXDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CNewIPXDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	
	CDialog::OnTimer(nIDEvent);
}
