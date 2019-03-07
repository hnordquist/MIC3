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

// validate.cpp : implementation file
//

#include "stdafx.h"
#include "validate.h"
#include "TimedMessageDialog.h"
#include "mic_definitions.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// validate dialog

//CValidate::CValidate(CWnd* pParent, char *inifilename, int dlg_close_milliseconds, bool bSuperUser)
CValidate::CValidate(CWnd* pParent, bool bSuperUser)
	: CDialog(CValidate::IDD, pParent)
{
	//TRACE("CValidate::CTOR\n");

	m_bSuperUser = bSuperUser;

	//{{AFX_DATA_INIT(validate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CValidate::~CValidate()
{
	//TRACE("CValidate::DTOR\n");
}

void CValidate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(validate)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValidate, CDialog)
	//{{AFX_MSG_MAP(CValidate)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CValidate::Encrypt(char *input, char *output, char seed){
	if (seed == NULL)
	  output[0] = (char)(GetTickCount() % 26 + '@');
	else
      output[0] = seed;
	for (unsigned i = 0; i < strlen(input); i++) 
	  output[i+1] = (char)((output[i] + input[i]) % 26 + '@');
	output[strlen(input)] = '\0';
}

bool CValidate::Test(char *input, char *password) {
	bool result = false;
	char *temp = (char*)calloc(max(strlen(input),strlen(password))+1,sizeof(char));
	Encrypt(input, temp, password[0]);
	if (strcmp(temp,password)==0)
	  result = true;
	delete [] temp;
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// validate message handlers

void CValidate::OnOK() 
{
	bool ok;
	char buffname[256];
	char buffpass[256];
	char IniData[1024];
	char IniName[256];
	GetDlgItem(IDC_USERNAME_EDIT)->GetWindowText(buffname,sizeof(buffname));
	GetDlgItem(IDC_PASSWORD_EDIT)->GetWindowText(buffpass,sizeof(buffpass));
//	while (strlen(buffpass)<PASSWORDLEN) strcat(buffpass," ");

	ok = false;
	if (GetPrivateProfileSection(m_bSuperUser?"SuperUsers":"Users",
		IniData,sizeof(IniData),g_szIniFile)>0)
	{
      char *at = IniData;		//char pointer to step through buffpass
      while (strlen(at)>0) 
	  {
		strcpy(IniName,at);
		char *IniPassword = strchr(at,'=')+1;//at
		IniName[strcspn(IniName,"=")]='\0';
		while(strlen(buffpass)<MIC_PASSWORDLEN)strcat(buffpass," ");
		if ((strcmp(IniName,buffname)==0) && Test(buffpass,IniPassword)) 
		{
			ok = true;
			break;
		}
		at = at + strlen(at)+1;
	  }

	  //check superuser if failed user
	  if (!m_bSuperUser && !ok)
	  {
		IniData[0] = NULL;
		if (GetPrivateProfileSection("SuperUsers",IniData,sizeof(IniData),g_szIniFile)>0)
		{
		  char *at = IniData;		//char pointer to step through buffpass
		  while (strlen(at)>0) 
		  {
			strcpy(IniName,at);
			char *IniPassword = strchr(at,'=')+1;//at
			IniName[strcspn(IniName,"=")]='\0';
			while(strlen(buffpass)<MIC_PASSWORDLEN)strcat(buffpass," ");
			if ((strcmp(IniName,buffname)==0) &&
				Test(buffpass,IniPassword)) 
			{
				ok = true;
				break;
			}
			at = at + strlen(at)+1;
		  }
		}
	  }
	} 

	strcpy(g_pVal_Name,buffname);

//#ifdef _DEBUG
//	ok = true;
//#endif
	if (ok) 
	{
		g_cLastGoodValidate = COleDateTime::GetCurrentTime();//DR00161
		CDialog::OnOK();
	}
	else
	{
		ShowWindow(SW_HIDE);
		CString csTitle = CString(MIC_NAMESHORT) + CString(": Validate User");
		CString csText("Invalid USERNAME or PASSWORD entered!");
		new CTimedMessageDialog(csTitle,csText,g_iDlgCloseMillisec/1000,this);

		CDialog::OnCancel();
	}
}

char* CValidate::GetUserName()				//DR00115 -- I didn't find any
{											//DR00115 -- implementation of 
	if (strlen(g_pVal_Name) == 0)			//DR00115 -- this function, so
		strcpy(g_pVal_Name,"Unknown");		//DR00115 -- I added one
	return g_pVal_Name;						//DR00115
}											//DR00115

void CValidate::OnCancel() 
{
	GetDlgItem(IDC_USERNAME_EDIT)->GetWindowText(g_pVal_Name,sizeof(g_pVal_Name));
	CDialog::OnCancel();
}

BOOL CValidate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (strlen(g_pVal_Name)>0)
	{
		GetDlgItem(IDC_USERNAME_EDIT)->SetWindowText(g_pVal_Name);
		GetDlgItem(IDC_PASSWORD_EDIT)->SetFocus();
	}
	else
		GetDlgItem(IDC_USERNAME_EDIT)->SetFocus();

	COleDateTime cNow = COleDateTime::GetCurrentTime();	//DR00161
	COleDateTimeSpan cSpan(0,0,2,0);					//DR00161
	COleDateTime cTemp = cNow - cSpan;					//DR00161
	if ((g_cLastGoodValidate >= cTemp) && !m_bSuperUser)//DR00161
	{													//DR00161
		g_cLastGoodValidate = cNow;						//DR00161
		CDialog::OnOK();								//DR00161
	}													//DR00161
	else												//DR00161
		SetTimer(1,g_iDlgCloseMillisec,NULL);
	
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CValidate::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		CDialog::OnCancel();
	}
	CDialog::OnTimer(nIDEvent);
}
