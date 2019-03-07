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
// SetupData.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "SetupData.h"
#include "SetEdit.h"
#include "Instrument.h"
#include "ISO_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupData dialog
IMPLEMENT_DYNCREATE(CSetupData, CPropertyPage)

CSetupData::CSetupData() : CPropertyPage(CSetupData::IDD)
{
}

CSetupData::CSetupData(CWnd *pParent, char *inifile)  
	: CPropertyPage(CSetupData::IDD),
	m_pParent(pParent)
{
//	TRACE("CSetupData::CTR\n");
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	m_pList = new CListCtrlStyled();
}
/*
CSetupData::CSetupData(UINT id, CWnd* pParent, bool bDoingConfig)
	: CPropertyPage(id)
{
	m_pParent = pParent;
	m_pName[0] = NULL;
	m_bDoingConfig = bDoingConfig;
	m_pPS = NULL;
	//{{AFX_DATA_INIT(CSetupData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
*/

CSetupData::~CSetupData()
{
//	TRACE("CSetupData::DTR\n");
	delete m_pList;
}

void CSetupData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupData)
	DDX_Control(pDX, IDC_BUTTON_EDITSELECTION, m_cEditSelected);
	DDX_Control(pDX, IDC_LIST_SET, *m_pList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupData, CPropertyPage)
	//{{AFX_MSG_MAP(CSetupData)
	ON_BN_CLICKED(IDC_BUTTON_EDITSELECTION, OnButtonEditselection)
	ON_BN_CLICKED(IDC_BUTTON_SAVEALL, OnButtonSaveall)
	ON_BN_CLICKED(IDC_BUTTON_LOADDEFAULT, OnButtonLoaddefault)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SET, OnDblclkListSet)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SET, OnItemchangedListSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupData message handlers

void CSetupData::DoApply()
{
}

BOOL CSetupData::OnSetActive()
{
	return CPropertyPage::OnSetActive();
}

BOOL CSetupData::OnWizardFinish()
{
	return FALSE;
}

void CSetupData::ResetColors()
{
	for (int iRow = 0; iRow < m_pList->GetItemCount(); iRow++)
	{
		m_pList->SetItemTxtColor(iRow,1,COLOR_BLACK,false);
		m_pList->SetItemSelectedTxtColor(iRow,1,COLOR_BLACK,false);
	}
	m_pList->Invalidate();
}

void CSetupData::UpdateColors(COLORREF* cColors)
{
	for (int iRow = 0; iRow < m_pList->GetItemCount(); iRow++)
	{
		m_pList->SetItemTxtColor(iRow,1,cColors[iRow],false);
		m_pList->SetItemSelectedTxtColor(iRow,1,cColors[iRow],false);
	}
	m_pList->Invalidate();
}

void CSetupData::AddSetParam(char*szDesc,char*szIniName,char*szSetting,char*szCommand,unsigned int uLength)
{
	LVITEM	itemData;
	itemData.iItem = m_pList->GetItemCount();
	itemData.iSubItem = 0;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	if (szDesc[0] == '-')
	{
		char szTemp[256];
		strcpy(szTemp,"        ");
		strcat(szTemp,&(szDesc[1]));
		itemData.pszText = szTemp;
		itemData.cchTextMax = strlen(szTemp);
		itemData.iIndent = 1;
	}
	else
	{
		itemData.pszText = szDesc;
		itemData.cchTextMax = strlen(szDesc);
		itemData.iIndent = 0;
	}
	itemData.iImage = 0;
	itemData.lParam = uLength;
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
		if (szDesc[0] == '-')
			m_pList->SetRowStyle(itemData.iItem, LIS_TXTCOLOR | LIS_BGCOLOR, false);
		else
			m_pList->SetRowStyle(itemData.iItem, LIS_BOLD | LIS_TXTCOLOR | LIS_BGCOLOR, false);

	}
	else
	{
		if (szDesc[0] == '-')
			m_pList->SetRowStyle(itemData.iItem,LIS_TXTCOLOR, false);
		else
			m_pList->SetRowStyle(itemData.iItem,LIS_BOLD | LIS_TXTCOLOR, false);
	}
	m_pList->SetRowTxtColor(itemData.iItem,COLOR_BLACK,false);
}

bool CSetupData::CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch,bool bTextOnly)
{
	//find item's row
	CString cString;
	m_cMatchString = "";
	for (int iIndex = 0; iIndex < m_pList->GetItemCount(); iIndex++)
	{
		cString = m_pList->GetItemText(iIndex,3);
		if (cString == szIniName)
		{
			char szText[256];

			m_pList->GetItemText(iIndex,2,szText,sizeof(szText));
			if (!bMatch)
				if (strcmp(szText,szCurrent)==0)
					bMatch = true;
			m_cMatchString = szText;
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
			if (!bTextOnly)
			{
				if (!bMatch)
				{
					m_pList->SetItemTxtColor(iIndex,1,COLOR_RED,false);		
					m_pList->SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
					m_pList->SetItemSelectedTxtColor(iIndex,1,COLOR_RED,false);
				}
				else
				{
					if (m_pList->GetTxtColor(iIndex,1)!=COLOR_RED)
					{
						m_pList->SetItemTxtColor(iIndex,1,COLOR_BLACK,false);
						m_pList->SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
						m_pList->SetItemSelectedTxtColor(iIndex,1,COLOR_BLACK,false);
					}
				}
			}
			break;//break out of the for loop
		}
	}
	return bMatch;
}

BOOL CSetupData::OnInitDialog() 
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
	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	//GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	
//	CString rString;
//	GetParent()->GetWindowText(rString);
//	SetWindowText(rString);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupData::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
	m_pPS = (CTPropertySheet*)pPS;
	if (pName)
	{
	//	strcpy(m_pTitle,pName);
		strcpy(m_pName,pName);
	}
}
void CSetupData::OnButtonEditselection() 
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
		CSetEdit cSetEdit;
		cSetEdit.m_cDesired = szDesired;
		cSetEdit.m_cDiscription = szDescription;
		cSetEdit.m_cReported = szReported;
		cSetEdit.m_uLength = uLength;
		cSetEdit.m_cIniName = szIniName;
		cSetEdit.m_cCommand = szCommand;

		//if user is happy with results then
		if (cSetEdit.DoModal() == IDOK)
		{
			//save the new string
			strcpy(szDesired,cSetEdit.m_cDesired);

			//make sure it is long enough
			while ((unsigned)cSetEdit.m_cDesired.GetLength() < uLength)
				cSetEdit.m_cDesired = " " + cSetEdit.m_cDesired;


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
			char szTemp[256];
			strcpy(szTemp,"\"");
			strcat(szTemp,szDesired);
			strcat(szTemp,"\"");
			WritePrivateProfileString(m_pName,szIniName,szTemp,m_szIniFile);

			//write to PFM file
			strcpy(szTemp,"18999 User Changed ");
			strcat(szTemp,szIniName);
			strcat(szTemp," to ");
			strcat(szTemp,szDesired);
			strcat(szTemp,"\n");
			((CSafePropertySheet*)m_pPS)->m_pParent->WriteToFile(TO_PFM,TYPE_COMP,NULL,szTemp);

			//turn off red if showing
			if (strcmp(szDesired,szReported)!=0)
			{
				m_pList->SetItemTxtColor(nItem,1,COLOR_RED,false);		
				m_pList->SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
				m_pList->SetItemSelectedTxtColor(nItem,1,COLOR_RED,true);
			}
			else
			{
				if (m_pList->GetTxtColor(nItem,1)!=COLOR_RED)
				{
					m_pList->SetItemTxtColor(nItem,1,COLOR_BLACK,false);
					m_pList->SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
					m_pList->SetItemSelectedTxtColor(nItem,1,COLOR_BLACK,true);
				}
			}
		}
	}
}

void CSetupData::OnButtonLoaddefault()
{
	CWaitCursor wait;
	char szTemp[64];
	char szBuffer[32768];
//	char szValue[64];
	if (strcmp(m_szVersion,"SET_")==0)
	{
		GetPrivateProfileString(INSTRUMENTS,m_pName,"",szTemp,sizeof(szTemp),m_szIniFile);
		if (strlen(szTemp))
		{
			strcat(szTemp, " Default");
			GetPrivateProfileSection(szTemp,szBuffer,sizeof(szBuffer),m_szIniFile);
			char *at = szBuffer;
			while (strlen(at) > 0)
			{
				strcpy(szTemp,at);
				if (memcmp(szTemp,"SET_",4)==0)
				{
					char *szResult = strchr(szTemp,'=');
					szResult[0] = NULL;
					szResult++;
					
					//find the row
					CString cString;
					for (int iIndex = 0; iIndex < m_pList->GetItemCount(); iIndex++)
					{
						cString = m_pList->GetItemText(iIndex,3);
						if (cString == szTemp)
						{
							//change the ini file
							WritePrivateProfileString(m_pName,szTemp,szResult,m_szIniFile);
							if (szResult[0] == '\"')
							{
								szResult[strlen(szResult)-1]=NULL;
								szResult++;
							}
							//change the value in the list box
							LVITEM	itemData;
							itemData.iItem = iIndex;
							itemData.iSubItem = 2;
							itemData.state = LVIS_SELECTED;
							itemData.stateMask = LVIS_SELECTED;
							itemData.pszText = szResult;
							itemData.cchTextMax = strlen(szResult);
							itemData.iImage = 0;
							itemData.lParam = strlen(szResult);
							itemData.iIndent = 0;
							itemData.mask = LVIF_TEXT;
							m_pList->SetItem(&itemData);

								//set the color
							break;//out of for loop
						}
					}
				}
				at = at + strlen(at)+1;
			}

		}
	}

	if (strcmp(m_szVersion,"SET4_")==0)
	{
		GetPrivateProfileString(INSTRUMENTS,m_pName,"",szTemp,sizeof(szTemp),m_szIniFile);
		if (strlen(szTemp))
		{
			strcat(szTemp, " Default");
			GetPrivateProfileSection(szTemp,szBuffer,sizeof(szBuffer),m_szIniFile);
			char *at = szBuffer;
			while (strlen(at) > 0)
			{
				strcpy(szTemp,at);
				if (memcmp(szTemp,"SET4_",4)==0)
				{
					char *szResult = strchr(szTemp,'=');
					szResult[0] = NULL;
					szResult++;
					
					//find the row
					CString cString;
					for (int iIndex = 0; iIndex < m_pList->GetItemCount(); iIndex++)
					{
						cString = m_pList->GetItemText(iIndex,3);
						if (cString == szTemp)
						{
							//change the ini file
							WritePrivateProfileString(m_pName,szTemp,szResult,m_szIniFile);
							if (szResult[0] == '\"')
							{
								szResult[strlen(szResult)-1]=NULL;
								szResult++;
							}
							//change the value in the list box
							LVITEM	itemData;
							itemData.iItem = iIndex;
							itemData.iSubItem = 2;
							itemData.state = LVIS_SELECTED;
							itemData.stateMask = LVIS_SELECTED;
							itemData.pszText = szResult;
							itemData.cchTextMax = strlen(szResult);
							itemData.iImage = 0;
							itemData.lParam = strlen(szResult);
							itemData.iIndent = 0;
							itemData.mask = LVIF_TEXT;
							m_pList->SetItem(&itemData);

								//set the color
							break;//out of for loop
						}
					}
				}
				at = at + strlen(at)+1;
			}
		}
	}

	strcpy(szTemp,"18998 User Changed all DESIRED in INI file to DEFAULT\r\n");
	((CSafePropertySheet*)m_pPS)->m_pParent->WriteToFile(TO_PFM,TYPE_COMP,NULL,szTemp);

}

void CSetupData::OnButtonSaveall() 
{
	SaveAll(false);
	char szTemp[64];
	strcpy(szTemp,"18997 User Changed all DESIRED in INI file to REPORTED\r\n");
	((CSafePropertySheet*)m_pPS)->m_pParent->WriteToFile(TO_PFM,TYPE_COMP,NULL,szTemp);
}

void CSetupData::SaveAll(bool bDoingConfig)
{
	CWaitCursor wait;
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
		WritePrivateProfileString(m_pName,szIniName,szTemp,m_szIniFile);

		m_pList->SetItemTxtColor(iRow,1,COLOR_BLACK,false);
		m_pList->SetItemSelectedStyle(iRow,1,LIS_TXTCOLOR,false);
		m_pList->SetItemSelectedTxtColor(iRow,1,COLOR_BLACK,false);
	}
	m_pList->Invalidate();
}
void CSetupData::OnOK() 
{
//	GetParent()->PostMessage(WM_COMMAND,MAKEWPARAM(IDC_BUTTON_SETUP,BN_CLICKED),NULL);
}

void CSetupData::OnDblclkListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	OnButtonEditselection();	
	*pResult = 0;
}

void CSetupData::OnItemchangedListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (m_pList->GetSelectedCount())
	{
		m_cEditSelected.EnableWindow(true);
	}
	else
	{
		m_cEditSelected.EnableWindow(false);
	}
	
	*pResult = 0;
}
/*
void CSetupData::IssuePrint(FILE* outfile, UINT iWidth)
{
	fprintf(outfile,"INSTRUMENT SETUP\n");
	fprintf(outfile,"  %*s:  %s\n",
		iWidth,"Description","Reported");
                                            
	for (int iRow = 0; iRow < m_pList->GetItemCount(); iRow++)
	{
		char szDescription[128];
		char szReported[128];
		char szDesired[128];
		char szIniName[128];
		char cChar;
		UINT uLength;

		LVITEM lvItem;

		COLORREF color = m_pList->GetTxtColor(iRow,1);
		if (color == COLOR_YELLOW) cChar = '#';
		else if (color != COLOR_DEFAULT_TEXT) cChar = '*';
		else cChar = ' ';

		//get the description of the item
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = iRow;
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

		fprintf(outfile,"  %*s:  \"%s\"%c\n",
			iWidth,szDescription,szReported, cChar);
	}
}
*/