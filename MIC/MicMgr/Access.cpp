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

// Access.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "mic_definitions.h"
#include "validate.h"
#include "Access.h"
#include "Userinfo.h"
#include "TimedMessageDialog.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAccess dialog

// constructor 
CAccess::CAccess(CWnd* pParent)
	: CDialog(CAccess::IDD, pParent)
{
	//TRACE("CAccess::CTR\n");

	//{{AFX_DATA_INIT(CAccess)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAccess::~CAccess()
{
//	TRACE("CAccess::DTR\n");
}


void CAccess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccess)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAccess, CDialog)
	//{{AFX_MSG_MAP(CAccess)
	ON_LBN_SELCHANGE(IDC_USER_LIST, OnSelchangeUserList)
	ON_BN_CLICKED(IDC_NEW_BUTTON, OnNewButton)
	ON_BN_CLICKED(IDC_EDIT_BUTTON, OnEditButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_LBN_DBLCLK(IDC_USER_LIST, OnDblclkUserList)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccess message handlers

void CAccess::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void CAccess::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

//track the currently selected item in the list box
void CAccess::OnSelchangeUserList() 
{
	if(((CListBox*)GetDlgItem(IDC_USER_LIST))->GetCurSel()==LB_ERR) 
	{
        GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);
        GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);
	} 
	else 
	{
        GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(true);
	}
}

//add a new name when requested to do so
void CAccess::OnNew()
{
  int selected;
  char Name[256];Name[0]=NULL;
  char Password[256];Password[0]=NULL;

  //get the new user information
  CUserInfo UserInfo(this,1,Name,Password);
  if (UserInfo.DoModal()==IDOK) {
	//create new item with name and pw
	if(((CListBox*)GetDlgItem(IDC_USER_LIST))->FindStringExact(-1,Name)
		== LB_ERR) 
	{
	  //add the new info to the list box
	  selected = ((CListBox*)GetDlgItem(IDC_USER_LIST))->AddString(Name);
	  char *temp = (char*)malloc(256);
	  ((CListBox *)GetDlgItem(IDC_USER_LIST))->SetItemDataPtr(selected, temp);
	  //force the saved password to length
	  while(strlen(Password)<MIC_PASSWORDLEN)strcat(Password," ");
	  //mangle the password
	  CValidate Valid(this);
	  Valid.Encrypt(Password,temp);
	  //save the password and user name to the INI file
	  WritePrivateProfileString(USERS,Name,temp,g_szIniFile);
	  ((CListBox *)GetDlgItem(IDC_USER_LIST))->SetCurSel(selected);
	  //update the display
	  OnSelchangeUserList();
	} 
	else 
	{
		//get rid of the timer (to time out this display)
		//we don't want it to time out while the subordinate 
		//dialog box is being displayed
		KillTimer(1);
		//display a time-out'able error dialog
		CString csTitle(MIC_NAMELONG);
		csTitle += ": Access Control";
		CString csText("A user by that name already exists!");
		new CTimedMessageDialog(csTitle,csText,1000*g_iDlgCloseMillisec,this);
		//<<QA>>MsgBox.DoModal(LPCTSTR (CString(MIC_NAMELONG)+CString(": Access Control")),
		//	"A user by that name already exists!");
		//reset this dialog box's timeout timer
		SetTimer(1,g_iDlgCloseMillisec,NULL);
	}
  }
}

void CAccess::OnNewButton() 
{
	//get a new user/password combo
	if (g_bHideWindows) 
		ShowWindow(SW_HIDE);
	//validate we have an authorized user
	CValidate Valid(this);
	if (Valid.DoModal() == IDOK) 
	  OnNew();
	ShowWindow(SW_SHOW);  
}

void CAccess::OnEditButton() 
{
	//get selected index
	int selected;
	if ((selected =	((CListBox*)GetDlgItem(IDC_USER_LIST))->GetCurSel())
		  != LB_ERR) 
	{
		//validate we have an authorized user
	    if (g_bHideWindows) ShowWindow(SW_HIDE);
		CValidate Valid(this);
	    if (Valid.DoModal() == IDOK) 
		{
	  		//get at index text and make cstring
			char Name[256];
			char Namebackup[256];
			((CListBox*)GetDlgItem(IDC_USER_LIST))->GetText(selected,Name);
			strcpy(Namebackup,Name);
			//get at index string data
			char Password[256];
			char *abuff = 
			  (char*)((CListBox*)GetDlgItem(IDC_USER_LIST))->GetItemDataPtr(selected);
			strcpy(Password,abuff);
			//dialog to get user/password
			CUserInfo UserInfo(this,0,Name,Password);
			if (UserInfo.DoModal()==IDOK) {
				//delete old pw
				delete [] abuff;
				//delete old record from listbox
		        ((CListBox*)GetDlgItem(IDC_USER_LIST))->DeleteString(selected);
				//delete old record from ini file
				WritePrivateProfileString(USERS,Namebackup,NULL,g_szIniFile);
				//create new item with name and pw
				selected = ((CListBox*)GetDlgItem(IDC_USER_LIST))->AddString(Name);
				//push to 20 characters long and encrypt it
				while(strlen(Password)<MIC_PASSWORDLEN)strcat(Password," ");
				char *temp = (char*)malloc(256);
				CValidate Valid(this);
	            Valid.Encrypt(Password,temp);
				//save the new userid password in the list box
				((CListBox *)GetDlgItem(IDC_USER_LIST))->SetItemDataPtr(selected, temp);
				//save the new userid and encrypted password in the ini file
				WritePrivateProfileString(USERS,Name,temp,g_szIniFile);
				//force a selection
				((CListBox *)GetDlgItem(IDC_USER_LIST))->SetCurSel(selected);

			}
		}
		ShowWindow(SW_SHOW);
	}
}

void CAccess::OnDeleteButton() 
{
	CValidate Valid(this);
    if (g_bHideWindows) ShowWindow(SW_HIDE);
	int selected;
	//if something is selected
	if ((selected = ((CListBox*)GetDlgItem(IDC_USER_LIST))->GetCurSel())
		    != LB_ERR) {
	  //validate the current user 
	  if (Valid.DoModal() == IDOK) 
	  {
		//delete the selection from the list box
		delete [] (char*)((CListBox*)GetDlgItem(IDC_USER_LIST))->GetItemDataPtr(selected);
		char Name[256];
		//get the selection from the list box
		((CListBox*)GetDlgItem(IDC_USER_LIST))->GetText(selected,Name);
		//delete the selection from the ini file
		WritePrivateProfileString(USERS,Name,NULL,g_szIniFile);
		((CListBox*)GetDlgItem(IDC_USER_LIST))->DeleteString(selected);
	  }
	}
	ShowWindow(SW_SHOW);  
	//update the user list
    OnSelchangeUserList();
}

void CAccess::OnDblclkUserList() 
{
	OnEditButton();
}

int CAccess::CountUsers()
{
	char buff[2048];
	int index;
	//get all the user/passwords from the ini file
	//getprivateprofilesection returns null delimited string with 2 null end
	index = 0;
    if (GetPrivateProfileSection(USERS,buff,sizeof(buff),g_szIniFile)>0)
	{
	  //did get something	
      char *at = buff;		//char pointer to step through buff
	  char *at2;
      while (strlen(at)>0) 
	  {
		at2=strtok(at,"=");
		if (at2) 
		{
		  index++;
		  at = at + strlen(at)+1;
		}
		at = at + strlen(at)+1;
	  }
	} 
	return index;
}

BOOL CAccess::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char buff[2048];
	int index;
	//get all the user/passwords from the ini file
	//getprivateprofilesection returns null delimited string with 2 null end
    if (GetPrivateProfileSection(USERS,buff,sizeof(buff),g_szIniFile)>0)
	{
	  //did get something	
      char *at = buff;		//char pointer to step through buff
	  char *at2;
      while (strlen(at)>0) 
	  {
		at2=strtok(at,"=");
		if (at2) 
		{
		  index = ((CListBox *)GetDlgItem(IDC_USER_LIST))->
			  AddString(at2);
		  at = at + strlen(at)+1;
		  char *temp = (char*)malloc(256);
		  strcpy(temp,at);
		  ((CListBox *)GetDlgItem(IDC_USER_LIST))->
			  SetItemDataPtr(index, temp);
		}
		at = at + strlen(at)+1;
	  }
	} 
	else 
	{
	  //no currenly defined so get a new user/password
	  OnNew();
	}
    GetDlgItem(IDC_DELETE_BUTTON)->EnableWindow(false);
    GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(false);

	//close the dialog box automatically at end of timer
	SetTimer(1,g_iDlgCloseMillisec,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAccess::DestroyWindow() 
{
	int result;
	//get how many entries in the list and delete them all
	if ((result = ((CListBox*)GetDlgItem(IDC_USER_LIST))->GetCount())
		!=LB_ERR) 
	  for (int i=0;i<result;i++)
		delete [] (char*)((CListBox*)GetDlgItem(IDC_USER_LIST))->GetItemDataPtr(i);
	return CDialog::DestroyWindow();
}

void CAccess::OnTimer(UINT nIDEvent) 
{
	// catch the time out timer and destroy the window 
	if (nIDEvent == 1)
		CDialog::OnCancel();
	CDialog::OnTimer(nIDEvent);
}
