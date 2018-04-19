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
// DSPECProppageSetupData.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "colors.h"
#include "DSPECStrings.h"
#include "DSPECProppageSetupData.h"
#include "DSPECStaticSetupData.h"
#include "DSPECSetEdit.h"
#include "DSPECInst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSPECProppageSetupData dialog
IMPLEMENT_DYNCREATE(CDSPECProppageSetupData, CPropertyPage)

CDSPECProppageSetupData::CDSPECProppageSetupData(CWnd *pParent)  
	: CPropertyPage(CDSPECProppageSetupData::IDD)
{
//	TRACE("CDSPECProppageSetupData::CTR_a\n");
	m_pParent = (CDSPECInst *)pParent;
	m_szIniFile[0] = 0;
}

CDSPECProppageSetupData::CDSPECProppageSetupData(CWnd* pParent, char *inifile)
	: CPropertyPage(CDSPECProppageSetupData::IDD)
{
//	TRACE("CDSPECProppageSetupData::CTR_b\n");
	m_pParent = (CDSPECInst *)pParent;
	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));
}


CDSPECProppageSetupData::~CDSPECProppageSetupData()
{
//	TRACE("CDSPECProppageSetupData::DTR\n");
	m_pParent = NULL;
}

void CDSPECProppageSetupData::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSPECProppageSetupData)
	DDX_Control(pDX, IDC_BUTTON_DSPEC_SETTINGS_EDIT_SELECTION, m_cEditSelected);
	DDX_Control(pDX, IDC_DSPEC_SETTINGS_LIST_SET, m_cList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSPECProppageSetupData, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPECProppageSetupData)
	ON_BN_CLICKED(IDC_BUTTON_DSPEC_SETTINGS_EDIT_SELECTION, OnButtonEditselection)
	ON_BN_CLICKED(IDC_BUTTON_DSPEC_SETTINGS_SAVE_ALL_REPORTED, OnButtonSaveallreported)
	ON_BN_CLICKED(IDC_BUTTON_DSPEC_SETTINGS_LOAD_DEFAULT, OnButtonLoaddefault)
	ON_BN_CLICKED(IDC_BUTTON_DSPEC_SETTINGS_REFRESH_REPORTED, OnButtonRefreshreported)
	ON_BN_CLICKED(IDC_BUTTON_DSPEC_SETTINGS_SEND_DESIRED, OnButtonSenddesired)

	ON_NOTIFY(NM_DBLCLK, IDC_DSPEC_SETTINGS_LIST_SET, OnDblclkListSet)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DSPEC_SETTINGS_LIST_SET, OnItemchangedListSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSPECProppageSetupData message handlers

void CDSPECProppageSetupData::DoApply()
{
}

BOOL CDSPECProppageSetupData::OnSetActive()
{
	BOOL breturn = CPropertyPage::OnSetActive();
	if (m_pParent)
	{
		m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);
	}
	return breturn;
}

BOOL CDSPECProppageSetupData::OnWizardFinish()
{
//	ASSERT(FALSE);
	return FALSE;
}

void CDSPECProppageSetupData::ResetColors()
{
	for (int iRow = 0; iRow < m_cList.GetItemCount(); iRow++)
		m_cList.SetItemTxtColor(iRow,1,COLOR_BLACK,false);
	m_cList.Invalidate();
}

void CDSPECProppageSetupData::AddSetParam(
	char*szDesc,
	char*szIniName,
	char*szSetting,
	char*szCommand,
	char*szAllowedValues,
	unsigned int uLength)
{
	//TRACE("CDSPECProppageSetupData::AddSetParam()\n");

	LVITEM	itemData;
	itemData.iItem = m_cList.GetItemCount();
	itemData.iSubItem = 0;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szDesc;
	itemData.cchTextMax = strlen(szDesc);
	itemData.iImage = 0;
	itemData.lParam = uLength;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT | LVIF_PARAM;
	m_cList.InsertItem(&itemData);

	itemData.iSubItem = 2;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szSetting;
	itemData.cchTextMax = strlen(szSetting);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_cList.SetItem(&itemData);
	
	itemData.iSubItem = 3;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szIniName;
	itemData.cchTextMax = strlen(szIniName);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_cList.SetItem(&itemData);

	itemData.iSubItem = 4;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szCommand;
	itemData.cchTextMax = strlen(szCommand);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_cList.SetItem(&itemData);

	itemData.iSubItem = 5;
	itemData.state = LVIS_SELECTED;
	itemData.stateMask = LVIS_SELECTED;
	itemData.pszText = szAllowedValues;
	itemData.cchTextMax = strlen(szAllowedValues);
	itemData.iImage = 0;
	itemData.lParam = 1;
	itemData.iIndent = 0;
	itemData.mask = LVIF_TEXT;
	m_cList.SetItem(&itemData);

	if (itemData.iItem & 0x00000001)
	{
		m_cList.SetRowBgColor(itemData.iItem, RGB(170,240,170) , false );
		m_cList.SetRowStyle(itemData.iItem, LIS_TXTCOLOR | LIS_BGCOLOR, false);
	}
	else
	{
		m_cList.SetRowStyle(itemData.iItem,LIS_TXTCOLOR, false);
	}
	m_cList.SetRowTxtColor(itemData.iItem,COLOR_BLACK,false);
}

void CDSPECProppageSetupData::CurrentSetParam(char*szIniName,char*szCurrent,bool bMatch)
{
	//find item's row
	CString cString;
	for (int iIndex = 0; iIndex < m_cList.GetItemCount(); iIndex++)
	{
		cString = m_cList.GetItemText(iIndex,3);
		if (cString == szIniName)
		{
			m_cList.SetItemStyle(iIndex,1,LIS_TXTCOLOR,true);
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
			m_cList.SetItem(&itemData);

			//CString desired = m_cList.GetItemText(iIndex,2);
			//if (desired != szCurrent)
			if (!bMatch)
			{
				m_cList.SetItemTxtColor(iIndex,1,COLOR_RED,false);		
				m_cList.SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
				m_cList.SetItemSelectedTxtColor(iIndex,1,COLOR_RED,false);
				//if (m_pParent)
				//	m_pParent->PostMessage(IDC_DESIRED_VICE_CURRENT_MISMATCH,0,0);
			}
			else
			{
				m_cList.SetItemTxtColor(iIndex,1,COLOR_BLACK,false);
				m_cList.SetItemSelectedStyle(iIndex,1,LIS_TXTCOLOR,false);
				m_cList.SetItemSelectedTxtColor(iIndex,1,COLOR_BLACK,false);
			}
			break;

		}
	}
}

BOOL CDSPECProppageSetupData::OnInitDialog() 
{
//	TRACE("CDSPECProppageSetupData::OnInitDialog()\n");

	CDialog::OnInitDialog();
	
	// Set a Global Style
	//
	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	// Columns Creation
	//
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 280;
	lvColumn.pszText = "Description";
	m_cList.InsertColumn(0,&lvColumn);
	lvColumn.cx = 140;
	lvColumn.pszText = "Reported (instrument)";
	m_cList.InsertColumn(1,&lvColumn);
	lvColumn.cx = 140;
	lvColumn.pszText = "Desired (INI file)";
	m_cList.InsertColumn(2,&lvColumn);
	lvColumn.cx = 200;
	lvColumn.pszText = "INI Name";
	m_cList.InsertColumn(3,&lvColumn);
	lvColumn.cx = 500;
	lvColumn.pszText = "Command";
	m_cList.InsertColumn(4,&lvColumn);
	lvColumn.cx = 200;
	lvColumn.pszText = "Allowed Values";
	m_cList.InsertColumn(5,&lvColumn);

	m_cList.SetTextColor( COLOR_BLACK);

	m_cList.SetColStyle(1,LIS_BOLD|LIS_TXTCOLOR);

	m_cList.SetHighlightColor( RGB(160,210,210) );
	m_cList.SetHighlightTextColor( COLOR_BLACK );
	
//	CString rString;
//	GetParent()->GetWindowText(rString);
//	SetWindowText(rString);
	//
	// fill in the list control with data from the INI file.
	// If the data is not in this instrument's part of the INI file, get it out
	// of the default part and write it into the instrument part.
	//

	// This was in the CDSPECInst Create...
	CDSPECStaticSetupData cDSPECSetupData;
	char szTemp[64];
	char *pszDescription,*pszIniName,*pszDefault,*pszCommand,*pszAllowedValues;

	bool b_new_ini_values = false;

	while ((pszDescription = cDSPECSetupData.GetNext())!=NULL)
	{
		pszIniName = cDSPECSetupData.GetNext();
		pszDefault = cDSPECSetupData.GetNext();
		pszCommand = cDSPECSetupData.GetNext();
		pszAllowedValues  = cDSPECSetupData.GetNext();

		GetPrivateProfileString(m_pName,pszIniName,"ERROR",szTemp,sizeof(szTemp),m_szIniFile);
		if (0 == strcmp(szTemp, "ERROR"))
		{
//#ifdef _DEBUG
//			static DWORD freq = 200;
//			freq += 100;
//			Beep(freq,300);
//#endif
			//Data was not there.  Go look for it in the DSPEC_DEFAULT section
			GetPrivateProfileString(DSPEC_DEFAULT,pszIniName,"ERROR",szTemp,sizeof(szTemp),m_szIniFile);
			WritePrivateProfileString(m_pName,pszIniName,szTemp,m_szIniFile);
			b_new_ini_values = true;
		}
		AddSetParam(
			pszDescription,
			pszIniName,
			szTemp,
			pszCommand,
			pszAllowedValues,
			strlen(pszDefault));
	}
	// end This was in the CDSPECInst Create... 

	if (b_new_ini_values && m_pParent)
		m_pParent->PostMessage(IDC_UPDATE_PARAMS, 0, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSPECProppageSetupData::SetPropertySheet(CPropertySheet *pPS, char *pName)
{
//	m_pPS = (CTPropertySheet*)pPS;
	m_pPS = (CDSPECSafePropertySheet*)pPS;
	if (pName)
	{
		strcpy(m_pName,pName);
	}
}
void CDSPECProppageSetupData::OnButtonEditselection() 
{
	POSITION pos = m_cList.GetFirstSelectedItemPosition();
	if (pos > 0)
	{
		char szDescription[128];
		char szReported[128];
		char szDesired[128];
		char szIniName[128];
		char szCommand[128];
		char szAllowedValues[128];
		UINT uLength;

		LVITEM lvItem;

		//get the description of the item
		int nItem = m_cList.GetNextSelectedItem(pos);
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = nItem;
		lvItem.iSubItem = 0;
		lvItem.pszText = szDescription;
		lvItem.cchTextMax = sizeof(szDescription);
		m_cList.GetItem(&lvItem);
		uLength = lvItem.lParam;

		//get the actual value the instrument has been reporting
		lvItem.iSubItem = 1;
		lvItem.pszText = szReported;
		lvItem.cchTextMax = sizeof(szReported)-1;
		m_cList.GetItem(&lvItem);

		//get the desired value from the ini file
		lvItem.iSubItem = 2;
		lvItem.pszText = szDesired;
		lvItem.cchTextMax = sizeof(szDesired)-1;
		m_cList.GetItem(&lvItem);
		
		//get the ini name 
		lvItem.iSubItem = 3;
		lvItem.pszText = szIniName;
		lvItem.cchTextMax = sizeof(szIniName)-1;
		m_cList.GetItem(&lvItem);

		//get the command string 
		lvItem.iSubItem = 4;
		lvItem.pszText = szCommand;
		lvItem.cchTextMax = sizeof(szCommand)-1;
		m_cList.GetItem(&lvItem);

		//get the allowed values
		lvItem.iSubItem = 5;
		lvItem.pszText = szAllowedValues;
		lvItem.cchTextMax = sizeof(szAllowedValues)-1;
		m_cList.GetItem(&lvItem);

		/* MIC 1.907a for Rokkasho does not allow this editing
		//do the edit dialog
		CDSPECSetEdit cSetEdit;
		cSetEdit.m_cDesired = szDesired;
		cSetEdit.m_cDiscription = szDescription;
		cSetEdit.m_cReported = szReported;
		cSetEdit.m_uLength = uLength;
		cSetEdit.m_cIniName = szIniName;
		cSetEdit.m_cCommand = szCommand;
		cSetEdit.m_cAllowedValues = szAllowedValues;

		//if user is happy with results AND changed the value, then
		if( (cSetEdit.DoModal() == IDOK)  && (cSetEdit.m_cDesired != szDesired))
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
			m_cList.SetItem(&itemData);

			//write to ini file
			char szTemp[128];
			strcpy(szTemp,"\"");
			strcat(szTemp,szDesired);
			strcat(szTemp,"\"");
			WritePrivateProfileString(m_pName,szIniName,szTemp,m_szIniFile);
			//WritePrivateProfileString(m_pName,szIniName,szDesired,m_szIniFile);

			//Tell the parent to apply the value we changed 
			//and to get the new instrument settings
			if (m_pParent)
			{
				m_pParent->PostMessage(IDC_UPDATE_PARAMS, 0, 0);
				m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);
			}

			//turn off red if showing
			if (strcmp(szDesired,szReported)!=0)
			{
				m_cList.SetItemTxtColor(nItem,1,COLOR_RED,false);		
				m_cList.SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
				m_cList.SetItemSelectedTxtColor(nItem,1,COLOR_RED,true);
			}
			else
			{
				m_cList.SetItemTxtColor(nItem,1,COLOR_BLACK,false);
				m_cList.SetItemSelectedStyle(nItem,1,LIS_TXTCOLOR,false);
				m_cList.SetItemSelectedTxtColor(nItem,1,COLOR_BLACK,true);
			}
		}
		*/
	}
}

void CDSPECProppageSetupData::OnButtonLoaddefault()
{
	//Not used for MIC 1.907a
	/*
	char szTemp[64];
	char szBuffer[16384];
//	char szValue[64];
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
				for (int iIndex = 0; iIndex < m_cList.GetItemCount(); iIndex++)
				{
					cString = m_cList.GetItemText(iIndex,3);
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
						m_cList.SetItem(&itemData);

							//set the color
						break;//out of for loop
					}
				}
			}
			at = at + strlen(at)+1;
		}
		if (m_pParent)
		{
			m_pParent->PostMessage(IDC_UPDATE_PARAMS, 0, 0);
			m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);
		}
	}
	*/
}

void CDSPECProppageSetupData::OnButtonSaveallreported() 
{
	//Not used for MIC 1.907a
	if (m_pParent->Validate(true)) //superuser = true
	{
		for (int iRow = 0; iRow < m_cList.GetItemCount(); iRow++)
		{
			char szDescription[128];
			char szReported[128];
			char szDesired[128];
			char szIniName[128];
			UINT uLength;

			LVITEM lvItem;

			//get the description of the item
			lvItem.mask = LVIF_TEXT | LVIF_PARAM;
			lvItem.iItem = iRow;
			lvItem.iSubItem = 0;
			lvItem.pszText = szDescription;
			lvItem.cchTextMax = sizeof(szDescription);
			m_cList.GetItem(&lvItem);
			uLength = lvItem.lParam;

			//get the actual value the instrument has been reporting
			lvItem.iSubItem = 1;
			lvItem.pszText = szReported;
			lvItem.cchTextMax = sizeof(szReported)-1;
			m_cList.GetItem(&lvItem);

			//get the desired value from the ini file
			lvItem.iSubItem = 2;
			lvItem.pszText = szDesired;
			lvItem.cchTextMax = sizeof(szDesired)-1;
			m_cList.GetItem(&lvItem);
			
			//get the ini name 
			lvItem.iSubItem = 3;
			lvItem.pszText = szIniName;
			lvItem.cchTextMax = sizeof(szIniName)-1;
			m_cList.GetItem(&lvItem);

			//update the desired to match reported
			//if (!bDoingConfig)
				strcpy(szDesired,szReported);

			//update the display
			LVITEM itemData;
			itemData.iItem = iRow;
			itemData.iSubItem = 2;
			itemData.state = LVIS_SELECTED;
			itemData.stateMask = LVIS_SELECTED;
			itemData.pszText = szDesired;
			itemData.cchTextMax = strlen(szDesired);
			itemData.iImage = 0;
			itemData.lParam = uLength;
			itemData.iIndent = 0;
			itemData.mask = LVIF_TEXT;
			m_cList.SetItem(&itemData);

			//write to ini file
	//		char szTemp[128];
	//		strcpy(szTemp,"\"");
	//		strcat(szTemp,szDesired);
	//		strcat(szTemp,"\"");
			WritePrivateProfileString(m_pName,szIniName,szDesired,m_szIniFile);
			

			m_cList.SetItemTxtColor(iRow,1,COLOR_BLACK,false);
			m_cList.SetItemSelectedStyle(iRow,1,LIS_TXTCOLOR,false);
			m_cList.SetItemSelectedTxtColor(iRow,1,COLOR_BLACK,false);
		}
		m_cList.Invalidate();

		if (m_pParent)
		{
			m_pParent->PostMessage(IDC_UPDATE_PARAMS, 0, 0);
			m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);
		}
	}
}
void CDSPECProppageSetupData::OnOK() 
{
}

void CDSPECProppageSetupData::OnDblclkListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	OnButtonEditselection();	
	*pResult = 0;
}

void CDSPECProppageSetupData::OnItemchangedListSet(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_cList.GetSelectedCount())
	{
		m_cEditSelected.EnableWindow(true);
	}
	else
	{
		m_cEditSelected.EnableWindow(false);
	}
	
	*pResult = 0;
}

void CDSPECProppageSetupData::IssuePrint(FILE* outfile)
{
	//fprintf(outfile,"***************Begin CDSPECProppageSetupData::IssuePrint()\n");

	fprintf(outfile,"\nDSPEC INSTRUMENT SETTINGS\n");
	fprintf(outfile,"  %40s:  %s\n",
		"Description","  Reported :: Desired (INI file)");
                                            
	for (int iRow = 0; iRow < m_cList.GetItemCount(); iRow++)
	{
		char szDescription[128];
		char szReported[128];
		char szDesired[128];
//		char szIniName[128];												//dgp 1 aug 2007
		UINT uLength;

		LVITEM lvItem;

		COLORREF color = m_cList.GetTxtColor(iRow,1);

		//get the description of the item
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = iRow;
		lvItem.iSubItem = 0;
		lvItem.pszText = szDescription;
		lvItem.cchTextMax = sizeof(szDescription);
		m_cList.GetItem(&lvItem);
		uLength = lvItem.lParam;

		//get the actual value the instrument has been reporting
		lvItem.iSubItem = 1;
		lvItem.pszText = szReported;
		lvItem.cchTextMax = sizeof(szReported)-1;
		m_cList.GetItem(&lvItem);

		//get the desired value from the ini file
		lvItem.iSubItem = 2;
		lvItem.pszText = szDesired;
		lvItem.cchTextMax = sizeof(szDesired)-1;
		m_cList.GetItem(&lvItem);
		
//		//get the ini name													//dgp 1 aug 2007
//		lvItem.iSubItem = 3;												//dgp 1 aug 2007
//		lvItem.pszText = szIniName;											//dgp 1 aug 2007
//		lvItem.cchTextMax = sizeof(szIniName)-1;							//dgp 1 aug 2007
//		m_cList.GetItem(&lvItem);											//dgp 1 aug 2007

		fprintf(outfile,"  %40s:  %+10s :: %-10s %s\n",							//dgp 1 aug 2007
			szDescription,szReported,szDesired,(color==COLOR_RED)?"*":" ");	//dgp 1 aug 2007
	}

	fprintf(outfile,"\n");

	//fprintf(outfile,"**********End CDSPECProppageSetupData::IssuePrint()\n");
}

void CDSPECProppageSetupData::OnButtonRefreshreported()
{
	if (m_pParent)
	{
		WPARAM ignore_pause_state = 1;
		m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,ignore_pause_state,0);
	}
}

void CDSPECProppageSetupData::OnButtonSenddesired()
{
	/* Not used for MIC 1.907a
	if (m_pParent)
		m_pParent->PostMessage(IDC_UPDATE_PARAMS,TRUE,0);
	*/
}