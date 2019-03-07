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
// GrandSetup.cpp

#include "stdafx.h"
#include "resource.h"
#include "colors.h"
#include "GrandSetup.h"
#include "GrandSetupData.h"
#include "ListCtrlStyled.h"
#include "SetEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrandSetup dialog

CGrandSetup::CGrandSetup(CWnd* pParent, char *inifile)
	: CDialog(CGrandSetup::IDD, pParent)
{
//	TRACE("CGrandSetup::CTR\n");
	m_pParent = pParent;
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	m_pList = new CListCtrlStyled();
	//{{AFX_DATA_INIT(CGrandSetup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CGrandSetup::~CGrandSetup()
{
//	TRACE("CGrandSetup::CTR\n");
	delete m_pList;
}


void CGrandSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrandSetup)
	DDX_Control(pDX, IDC_BUTTON_EDITSELECTION, m_cEditSelected);
	DDX_Control(pDX, IDC_LIST_SET, *m_pList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGrandSetup, CDialog)
	//{{AFX_MSG_MAP(CGrandSetup)
	ON_BN_CLICKED(IDC_BUTTON_EDITSELECTION, OnButtonEditselection)
	ON_BN_CLICKED(IDC_BUTTON_SAVEALL, OnButtonSaveall)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT, OnButtonLoadDefault)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, OnButtonHide)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SET, OnDblclkListSet)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SET, OnItemchangedListSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CGrandSetup::OnButtonHide()
{
	ShowWindow(SW_HIDE);
}

void CGrandSetup::AddSetParam(char*szDesc, char*szIniName, char*szSetting, char*szCommand,unsigned uLength)
{
	
	LVITEM	itemData;
	itemData.iItem = m_pList->GetItemCount();
	itemData.iSubItem = 0;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szDesc;
	itemData.cchTextMax = strlen(szDesc);
	itemData.iImage = 0;
	itemData.lParam = uLength;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT | LVIF_PARAM;
	m_pList->InsertItem(&itemData);

	itemData.iSubItem = 2;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szSetting;
	itemData.cchTextMax = strlen(szSetting);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_pList->SetItem(&itemData);
	
	itemData.iSubItem = 3;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szIniName;
	itemData.cchTextMax = strlen(szIniName);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_pList->SetItem(&itemData);

	itemData.iSubItem = 4;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szCommand;
	itemData.cchTextMax = strlen(szCommand);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_pList->SetItem(&itemData);

	if (itemData.iItem & 0x00000001)
	{
		m_pList->SetRowBgColor(itemData.iItem,COLOR_LIGHT_GREEN, false );
		m_pList->SetRowStyle(itemData.iItem, LIS_TXTCOLOR | LIS_BGCOLOR, false);
	}
	else
	{
//		m_pList->SetRowBgColor(itemData.iItem,COLOR_LIGHT_GREEN, false );
		m_pList->SetRowStyle(itemData.iItem,LIS_TXTCOLOR, false);
	}
	m_pList->SetRowTxtColor(itemData.iItem,COLOR_BLACK,false);
}

void CGrandSetup::OnButtonLoadDefault()
{
	CWaitCursor wait;
	CGrandSetupData cGrandSetupData;
	char szTemp[64];
	char *pszDescription;
	char *pszIniName;
	char *pszDefault;
	char *pszCommand;
	m_pList->SetRedraw(false);
	if (strcmp(m_szVersion,"SET_")==0)
	{
		while ((pszDescription = cGrandSetupData.GetNext())!=NULL)
		{
			pszIniName = cGrandSetupData.GetNext();
			pszDefault = cGrandSetupData.GetNext();
			pszCommand = cGrandSetupData.GetNext();
			GetPrivateProfileString(m_szName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
		}
	}
	if (strcmp(m_szVersion,"SET4_")==0)
	{
		while ((pszDescription = cGrandSetupData.GetNewNext())!=NULL)
		{
			pszIniName = cGrandSetupData.GetNewNext();
			pszDefault = cGrandSetupData.GetNewNext();
			pszCommand = cGrandSetupData.GetNewNext();
			GetPrivateProfileString(m_szName,pszIniName,"",szTemp,sizeof(szTemp),m_szIniFile);
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
			AddSetParam(pszDescription,pszIniName,szTemp,pszCommand,strlen(pszDefault));
		}
	}
	m_pList->SetRedraw(true);
}

void CGrandSetup::CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch, bool bTextOnly)
{
	//find item's row
	CString cString;
	m_pList->SetRedraw(false);

	for (int iIndex = 0; iIndex < m_pList->GetItemCount(); iIndex++)
	{
		cString = m_pList->GetItemText(iIndex,3);
		if (cString == szIniName)
		{
			m_pList->SetItemStyle(iIndex,1,LIS_TXTCOLOR,true);
			//set iSubItem 1
			LVITEM	itemData;
			itemData.iItem = iIndex;
			itemData.iSubItem = 1;
			itemData.state = LVIS_SELECTED;
			itemData.stateMask = LVIS_SELECTED;
			itemData.pszText = szCurrent;
			itemData.cchTextMax = strlen(szCurrent);
			itemData.iImage = 0;
			itemData.lParam = 1;
			itemData.iIndent = 0;
			itemData.mask = LVIF_TEXT;
			m_pList->SetItem(&itemData);

			char szTemp[64];
			m_pList->GetItemText(iIndex,2,szTemp,sizeof(szTemp));
			if (!bTextOnly)
			{
				if (!bMatch)
					if (strcmp(szTemp,szCurrent)==0)
						bMatch = true;
				if (bMatch)
				{
					m_pList->SetItemTxtColor(iIndex,1,COLOR_BLACK,false);
					m_pList->SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
					m_pList->SetItemSelectedTxtColor(iIndex,1,COLOR_BLACK,false);
				}
				else
				{
					m_pList->SetItemTxtColor(iIndex,1,COLOR_RED,false);		
					m_pList->SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
					m_pList->SetItemSelectedTxtColor(iIndex,1,COLOR_RED,false);
				}
			}
			break;
		}
	}
	m_pList->SetRedraw(true);
}

/////////////////////////////////////////////////////////////////////////////
// CGrandSetup message handlers

BOOL CGrandSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set a Global Style
	//
	m_pList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	// Columns Creation
	//
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 280;
	lvColumn.pszText = "Description";
	m_pList->InsertColumn(0,&lvColumn);
	lvColumn.cx = 140;
	lvColumn.pszText = "Reported (instrument)";
	m_pList->InsertColumn(1,&lvColumn);
	lvColumn.cx = 140;
	lvColumn.pszText = "Desired (INI file)";
	m_pList->InsertColumn(2,&lvColumn);
	lvColumn.cx = 200;
	lvColumn.pszText = "INI Name";
	m_pList->InsertColumn(3,&lvColumn);
	lvColumn.cx = 500;
	lvColumn.pszText = "Command";
	m_pList->InsertColumn(4,&lvColumn);
	
	// Set a BackGround color
	//
//	m_pList->SetTextBkColor( RGB(217,245,245) );
	m_pList->SetTextColor(COLOR_BLACK);

	// *******************************************
	// START SPECIFIC CListCtrlStyled methods !!!!!
	// *******************************************
/*
	for (int i = 0; i < iRows; i++)
	{
		if (i & 0x00000001)
		{
			m_pList->SetRowBgColor(i,COLOR_LIGHT_GREEN, false );
			m_pList->SetRowStyle(i, LIS_TXTCOLOR | LIS_BGCOLOR);
		}
		else
		{
			m_pList->SetRowStyle(i,LIS_TXTCOLOR);
		}
		m_pList->SetRowTxtColor(i,COLOR_BLACK,false);
	}
*/
	m_pList->SetColStyle(1,LIS_BOLD|LIS_TXTCOLOR);

	//
	m_pList->SetHighlightColor(COLOR_LIGHT_BLUE_GREEN);
	m_pList->SetHighlightTextColor(COLOR_BLACK);
	
	CString rString;
	GetParent()->GetWindowText(rString);
	SetWindowText(rString);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGrandSetup::OnOK() 
{
	GetParent()->PostMessage(WM_COMMAND,MAKEWPARAM(IDC_BUTTON_SETUP,BN_CLICKED),NULL);
}

void CGrandSetup::OnButtonEditselection() 
{
	POSITION pos = m_pList->GetFirstSelectedItemPosition();
	if (pos > 0)
	{
		char szDescription[128];
		char szReported[128];
		char szDesired[128];
		char szIniName[128];
		char szCommand[128];
		UINT uLength;

		LVITEM lvItem;

		//get the description of the item
		int nItem = m_pList->GetNextSelectedItem(pos);
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = nItem;
		lvItem.iSubItem = 0;
		lvItem.pszText = szDescription;
		lvItem.cchTextMax = sizeof(szDescription);
		m_pList->GetItem(&lvItem);
		uLength = lvItem.lParam;

		//get the actual value the instrument has been reporting
		lvItem.iSubItem = 1;
		lvItem.pszText = szReported;
		lvItem.cchTextMax = sizeof(szReported)-1;
		m_pList->GetItem(&lvItem);

		//get the desired value from the ini file
		lvItem.iSubItem = 2;
		lvItem.pszText = szDesired;
		lvItem.cchTextMax = sizeof(szDesired)-1;
		m_pList->GetItem(&lvItem);
		
		//get the ini name 
		lvItem.iSubItem = 3;
		lvItem.pszText = szIniName;
		lvItem.cchTextMax = sizeof(szIniName)-1;
		m_pList->GetItem(&lvItem);

		//get the ini name 
		lvItem.iSubItem = 4;
		lvItem.pszText = szCommand;
		lvItem.cchTextMax = sizeof(szCommand)-1;
		m_pList->GetItem(&lvItem);

		//do the edit dialog
		CSetEdit cSetEdit(this);
		cSetEdit.m_cDesired = szDesired;
		cSetEdit.m_cDiscription = szDescription;
		cSetEdit.m_cReported = szReported;
		cSetEdit.m_uLength = uLength;
		cSetEdit.m_cIniName = szIniName;
		cSetEdit.m_cCommand = szCommand;

		//if user is happy with results then
		if (cSetEdit.DoModal() == IDOK)
		{
			//make sure it is long enough
			while ((unsigned)cSetEdit.m_cDesired.GetLength() < uLength)
				cSetEdit.m_cDesired = " " + cSetEdit.m_cDesired;

			//save the new string
			strcpy(szDesired,cSetEdit.m_cDesired);

			//update the display
			LVITEM itemData;
			itemData.iItem = nItem;
			itemData.iSubItem = 2;
			itemData.state = LVIS_SELECTED;
			itemData.stateMask = LVIS_SELECTED;
			itemData.pszText = szDesired;
			itemData.cchTextMax = strlen(szDesired);
			itemData.iImage = 0;
			itemData.lParam = uLength;
			itemData.iIndent = 0;
			itemData.mask = LVIF_TEXT;
			m_pList->SetItem(&itemData);

			//write to ini file
			char szTemp[128];
			strcpy(szTemp,"\"");
			strcat(szTemp,szDesired);
			strcat(szTemp,"\"");
			WritePrivateProfileString(m_szName,szIniName,szTemp,m_szIniFile);
/*
			//write to PFM file
			strcpy(szTemp,"18999 User Changed ");
			strcat(szTemp,szIniName);
			strcat(szTemp," to ");
			strcat(szTemp,szDesired);
			m_pParent->m_pParent->WriteToFile(TO_PFM,TYPE_COMP,NULL,szTemp);
*/
			//turn off red if showing
			if (strcmp(szDesired,szReported)!=0)
			{
				m_pList->SetItemTxtColor(nItem,1,COLOR_RED,false);		
				m_pList->SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
				m_pList->SetItemSelectedTxtColor(nItem,1,COLOR_RED,true);
			}
			else
			{
				m_pList->SetItemTxtColor(nItem,1,COLOR_BLACK,false);
				m_pList->SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
				m_pList->SetItemSelectedTxtColor(nItem,1,COLOR_BLACK,true);
			}
		}
	}
}

void CGrandSetup::SaveSetup()
{
	SaveAll(true);
}

void CGrandSetup::OnButtonSaveall() 
{
	SaveAll(false);
}

void CGrandSetup::SaveAll(bool bDoingConfig)
{
	CWaitCursor wait;

	m_pList->SetRedraw(false);
	for (int iRow = 0; iRow < m_pList->GetItemCount(); iRow++)
	{
//		char szDescription[128];
		char szReported[128];
		char szDesired[128];
		char szIniName[128];


		//get the description of the item
//		m_pList->GetItemText(iRow,0,szDescription,sizeof(szDescription));

		//get the actual value the instrument has been reporting
		m_pList->GetItemText(iRow,1,szReported,sizeof(szReported));

		//get the desired value from the ini file
		m_pList->GetItemText(iRow,2,szDesired,sizeof(szDesired));
		
		//get the ini name 
		m_pList->GetItemText(iRow,3,szIniName,sizeof(szIniName));

		//update the desired to match reported
		if (!bDoingConfig)
		{
			strcpy(szDesired,szReported);
			//update the display
			m_pList->SetItemText(iRow,2,szDesired);
		}

		//write to ini file
		char szTemp[128];
		strcpy(szTemp,"\"");
		strcat(szTemp,szDesired);
		strcat(szTemp,"\"");
		WritePrivateProfileString(m_szName,szIniName,szTemp,m_szIniFile);

		m_pList->SetItemTxtColor(iRow,1,COLOR_BLACK,false);
		m_pList->SetItemSelectedStyle(iRow,1,LIS_TXTCOLOR,false);
		m_pList->SetItemSelectedTxtColor(iRow,1,COLOR_BLACK,true);
	}
	m_pList->SetRedraw(true);
}

void CGrandSetup::OnDblclkListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	OnButtonEditselection();	
	*pResult = 0;
}

void CGrandSetup::OnItemchangedListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_pList->GetSelectedCount())
		m_cEditSelected.EnableWindow(true);
	else
		m_cEditSelected.EnableWindow(false);
	
	*pResult = 0;
}
