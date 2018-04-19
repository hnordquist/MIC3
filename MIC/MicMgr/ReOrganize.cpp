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
// ReOrganize.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "globals.h"
#include "ReOrganize.h"
#include "RenameISO.h"
#include <process.h>
#include <errno.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReOrganize dialog


CReOrganize::CReOrganize(CWnd* pParent)
	: CDialog(CReOrganize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReOrganize)
	//}}AFX_DATA_INIT

	for (int iIndex = 0; iIndex < MAX_INSTRUMENTS; iIndex++)
		m_iOriginalNodes[iIndex] = m_iNewNodes[iIndex] = -1;
}


void CReOrganize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReOrganize)
	DDX_Control(pDX, IDC_TREE_VIEW, m_cTreeView);
	DDX_Control(pDX, IDC_CHECK_HIDEWATCH, m_cHideWatch);
	DDX_Control(pDX, IDC_LIST1, m_cListBox);
	DDX_Control(pDX, IDC_MOVE_UP, m_cMoveUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_cMoveDown);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReOrganize, CDialog)
	//{{AFX_MSG_MAP(CReOrganize)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDOK2, OnOk2)
	ON_BN_CLICKED(IDC_CHECK_HIDEWATCH, OnCheckHidewatch)
	ON_BN_CLICKED(IDC_RENAME_SELECTED, OnRenameSelected)
	ON_BN_CLICKED(IDC_RENAME_REVERT, OnRenameRevert)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, OnButtonRegister)
	//}}AFX_MSG_MAP
	ON_MESSAGE(TV_PREDROP,OnPreDrop)
	ON_MESSAGE(TV_POSTDROP,OnPostDrop)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReOrganize message handlers

void CReOrganize::DoReorg()
{
	CString cNameString[MAX_INSTRUMENTS];
	CString cKeyString[MAX_INSTRUMENTS];
//	int iIndex = 0;

	//if we have instruments moved to another com port
	if (m_cTreeView.m_bItemChanged)
	{
		//get each of the com ports in turn
		HTREEITEM hRootItem = m_cTreeView.GetRootItem();
		while (hRootItem != NULL)
		{
			//get the name of this com port
			CString cCommItem = m_cTreeView.GetItemText(hRootItem);
			//get the first child under it
			HTREEITEM hChildItem = m_cTreeView.GetChildItem(hRootItem);
			//and do all of the children
			while (hChildItem != NULL)
			{
				//get the child's name
				CString cInstItem = m_cTreeView.GetItemText(hChildItem);
				//update the ini instrument's port to this com object
				WritePrivateProfileString(cInstItem,"PORT",cCommItem,g_szIniFile);
				//get the handle of the next child
				hChildItem = m_cTreeView.GetNextItem(hChildItem,TVGN_NEXT);
			}
			//get the handle of the next com object
			hRootItem = m_cTreeView.GetNextItem(hRootItem,TVGN_NEXT);
		}
	}

	//write out the instruments in the correct order
	for (int i = 0; i < m_cListBox.GetCount(); i++)
	{
		char szKeyString[128];
		char szInstTypeString[32];
		char szTemp[32];
		m_cListBox.GetText(i,szKeyString);
		//get the instrument type
		GetPrivateProfileString("Instruments",m_cOriginalNames[i],"",szInstTypeString,sizeof(szInstTypeString),g_szIniFile);
		if (m_iOriginalNodes[i] != m_iNewNodes[i])
			WritePrivateProfileString(m_cOriginalNames[i],NODE,_itoa(m_iNewNodes[i],szTemp,10),g_szIniFile);
		if (szKeyString != m_cOriginalNames[i])
		{
			//reserve enough for a large section out of the ini file
			char szSection[32768]; //MIC_CB_028 to MIC 1.907 PJM 1/20/05

			//get all the ISO section data
			GetPrivateProfileSection(m_cOriginalNames[i],szSection,sizeof(szSection),g_szIniFile);
			//write it all out using the new name
			WritePrivateProfileSection(szKeyString,szSection,g_szIniFile);
			//delete the old name
			WritePrivateProfileString(m_cOriginalNames[i],NULL,NULL,g_szIniFile);
#ifdef ALLOWTRIGGERHOLDOFF
			char szExclusionName[_MAX_PATH];
			strcpy(szExclusionName,m_cOriginalNames[i]);
			strcat(szExclusionName,"_EXCLUSIONS");
			GetPrivateProfileSection(szExclusionName,szSection,sizeof(szSection),g_szIniFile);
			WritePrivateProfileString(szExclusionName,NULL,NULL,g_szIniFile);
			strcpy(szExclusionName,szKeyString);
			strcat(szExclusionName,"_EXCLUSIONS");
			WritePrivateProfileSection(szExclusionName,szSection,g_szIniFile);
#endif
		}
		//save the name and type
		(cNameString)[i] = szInstTypeString;
		(cKeyString)[i] = szKeyString;
	}

	//delete the "[INSTRUMENT]" section
	WritePrivateProfileString(INSTRUMENTS,NULL,NULL,g_szIniFile);

	//write the new "[INSTRUMENT]" section
	for (int i = 0; i < m_cListBox.GetCount(); i++)
		WritePrivateProfileString(INSTRUMENTS,cKeyString[i],cNameString[i],g_szIniFile);
}

void CReOrganize::OnOK() 
{
	// TODO: Add extra validation here
	DoReorg();
	CDialog::OnOK();
}

void CReOrganize::OnOk2() 
{
	// TODO: Add your control notification handler code here
	DoReorg();
	CDialog::OnCancel();	
}

void CReOrganize::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}


BOOL CReOrganize::MoveRow(int from, int to)
{
	if (from == to && from < 0 || to < 0)
		return FALSE;
	if (CopyRow(from, to))
	{
		if (from > to)
			m_cListBox.DeleteString(from + 1);
		else
			m_cListBox.DeleteString(from);
		return TRUE;
	}
	return FALSE;
}

BOOL CReOrganize::CopyRow(int from, int to)
{
	if (from == to || from < 0 || to < 0)
		return FALSE;
	CString rString;
	m_cListBox.GetText(from,rString);
	m_cListBox.InsertString(to, rString);
	return TRUE;
}

void CReOrganize::OnMoveDown() 
{
	// TODO: Add your control notification handler code here
	int index = m_cListBox.GetCurSel();
	if (index != LB_ERR)
	{
		if (index < m_cListBox.GetCount()-1)
		{
			if (MoveRow(index, index + 2))
			{
				CString cTemp;
				cTemp = m_cOriginalNames[index+1];
				m_cOriginalNames[index+1] = m_cOriginalNames[index];
				m_cOriginalNames[index]   = cTemp;

				int iTemp = m_iNewNodes[index+1];
				m_iNewNodes[index+1] = m_iNewNodes[index];
				m_iNewNodes[index] = iTemp;
			}
			m_cListBox.SetCurSel(index+1);
		}
	}
}

void CReOrganize::OnMoveUp() 
{
	// TODO: Add your control notification handler code here
	int index = m_cListBox.GetCurSel();
	if ((index > 0) && (index != LB_ERR))
	{
		if (MoveRow(index, index - 1))
		{
			CString cTemp;
			cTemp = m_cOriginalNames[index-1];
			m_cOriginalNames[index-1] = m_cOriginalNames[index];
			m_cOriginalNames[index] = cTemp;

			int iTemp = m_iNewNodes[index-1];
			m_iNewNodes[index-1] = m_iNewNodes[index];
			m_iNewNodes[index] = iTemp;
		}
		m_cListBox.SetCurSel(index-1);
	}
}

void CReOrganize::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
	if (m_cListBox.GetCurSel() != LB_ERR )
	{
		m_cMoveDown.EnableWindow(TRUE);
		m_cMoveUp.EnableWindow(TRUE);
	}
	else
	{
		m_cMoveDown.EnableWindow(FALSE);
		m_cMoveUp.EnableWindow(FALSE);
	}
}


BOOL CReOrganize::OnInitDialog() 
{
	CImageList          *pImageList;
	CBitmap             bitmap;
	CDialog::OnInitDialog();
//	UINT                nID;
	
	// TODO: Add extra initialization here
	int iIndex = 0;
	//don't get from the array instead get from the INI file
//	for (int instrument = 0; instrument < g_dCount; instrument ++) 
//	{
//		CString rString;
//		g_pInstruments[instrument]->m_pButton->GetWindowText(rString);
//		m_cListBox.InsertString(iIndex++,rString);
//	}


	pImageList = new CImageList();
	pImageList->Create(32, 16, ILC_MASK, 6, 4);

//	for (nID = IDB_BMTREEFIRST; nID <= IDB_BMTREELAST; nID++)  // load bitmaps for dog, bird and fish
//	{
//	}

	bitmap.LoadBitmap(IDB_BITMAP2);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP3);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP4);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP5);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP6);
	pImageList->Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();

	m_cTreeView.SetImageList(pImageList, TVSIL_NORMAL);

	char buff[32768];					//for getting stuff from ini

	for (int i = 0; i < MAX_INSTRUMENTS; i++)
		m_cOriginalNames[i] = "ERROR";

	HTREEITEM hItem = NULL;
	HTREEITEM hAllCommItems[MAX_COMM];
	for (int iIndex = 0; iIndex < MAX_COMM; iIndex++)
		hAllCommItems[iIndex] = NULL;
	
	//get from ini
//	if (GetPrivateProfileSection(COMMUNICATIONS,buff,
//		   sizeof(buff), m_szIniFile)>0)
//	{
//		//we must have at least one instrument or we wouldn't get here	
//		char *at = buff;				//char pointer to step through buff
//
//		//step through list of communications using a pointer into buff
//		while (strlen(at) > 0) 
//		{
//			//getting communications names from INI file so strip off extra stuff
//			CString At(at);
//			At.TrimRight();
//			char temp[32];
//			strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
//			if ((strlen(temp)>0) && (temp[0] != ';'))
//			{
//				//add the instrument
//
//				TVINSERTSTRUCT tvInsert;
//				tvInsert.hParent = NULL;
//				tvInsert.hInsertAfter = NULL;
//				tvInsert.item.mask = TVIF_TEXT;
//				tvInsert.item.pszText = temp;
//
//				hItem = m_cTreeView.InsertItem(&tvInsert);
//
//			}
// 			at = at + strlen(at)+1;
//		}
//	}
	BSTR bstr;
	char string_out[128];
	//get from global
	for (int iIndex = 0; iIndex < MAX_COMM; iIndex++)
	{
		if (g_pICSO[iIndex])
		{
			g_pICSO[iIndex]->get_Name(&bstr);
			Convert_BSTR_to_Character_String(bstr,string_out,sizeof(string_out));

			hItem = m_cTreeView.InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE,
					string_out, 0, 1, 0, 0, 0, NULL, NULL);

			hAllCommItems[iIndex] = hItem;
		}
	}

	if (GetPrivateProfileSection(INSTRUMENTS,buff,sizeof(buff), g_szIniFile)>0)
	{
		//we must have at least one instrument or we wouldn't get here	
		char *at = buff;				//char pointer to step through buff

		iIndex = 0;
		//step through list of instruments using a pointer into buff
		while ((strlen(at) > 0) && (g_iInstCount < MAX_INSTRUMENTS) ) 
		{
			//getting instrument names from INI file so strip off extra stuff
			CString At(at);
			At.TrimRight();
			char temp[32];
			char temp2[32];
			strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
			if ((strlen(temp)>0) && (temp[0] != ';'))
			{
				//add the instrument to the original array
				m_cOriginalNames[iIndex] = temp;
				//add the instrument's node to the node array
				m_iOriginalNodes[iIndex] = m_iNewNodes[iIndex] = GetPrivateProfileInt(temp,"Node",-1,g_szIniFile);
				//add the instrument to reorg box
				m_cListBox.InsertString(iIndex++,temp);

				//add instrument to comm::inst box
				GetPrivateProfileString(temp,PORT,"",temp2,sizeof(temp2),g_szIniFile);

				BSTR bstr;
				char string_out[128];
				//find correct comm object
				for (int i = 0; i < MAX_COMM; i++)
				{
					hItem = NULL;
					if (NULL != g_pICSO[i])
					{
						g_pICSO[i]->get_Name(&bstr);  //<<QA>> ???
						Convert_BSTR_to_Character_String(bstr,string_out,sizeof(string_out));

						if (strcmp(string_out,temp2)==0)
						{
							hItem = hAllCommItems[i];
							break;
						}
					}
				}
				if (hItem == NULL) //if it wasn't found then create "ERROR" for it and put it there
				{
					hItem = m_cTreeView.InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE,
							"ERROR", 0, 1, 0, 0, 0, NULL, NULL);
				}
				//insert under it, if it was found
				m_cTreeView.InsertItem(TVIF_TEXT|TVIF_SELECTEDIMAGE|TVIF_IMAGE,
					temp, 2, 3, 0, 0, 0, hItem, NULL);


				m_cTreeView.Expand(hItem,TVE_EXPAND);
			}
   			at = at + strlen(at)+1;
		}
	}

	if (g_iInstCount)
	{
		m_cListBox.SetCurSel(0);
		m_cMoveDown.EnableWindow(TRUE);
		m_cMoveUp.EnableWindow(TRUE);
	}
	else
	{
		m_cMoveDown.EnableWindow(FALSE);
		m_cMoveUp.EnableWindow(FALSE);
	}

	GetPrivateProfileString(CONFIGURATION,HIDEWATCH,"No",buff,sizeof(buff),g_szIniFile);
	buff[0] = (char)toupper(buff[0]);
	if (buff[0] == 'Y')
		m_cHideWatch.SetCheck(1);
	else
		m_cHideWatch.SetCheck(0);

	OnPostDrop(NULL,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CReOrganize::OnCheckHidewatch() 
{
	if (m_cHideWatch.GetCheck()==1)
		WritePrivateProfileString(CONFIGURATION,HIDEWATCH,"Yes",g_szIniFile);	
	else
		WritePrivateProfileString(CONFIGURATION,HIDEWATCH,"No",g_szIniFile);
}

void CReOrganize::OnRenameSelected() 
{
	// TODO: Add your control notification handler code here
	int index = m_cListBox.GetCurSel();
	if (index != LB_ERR)
	{
		char szName[256];
		CRenameISO cRenameISO;
		m_cListBox.GetText(index,szName);
		cRenameISO.m_cCurrentName = szName;
		cRenameISO.m_cNewName = szName;
		cRenameISO.m_cOriginalName = m_cOriginalNames[index];
		cRenameISO.m_cExistingNode = _itoa(m_iNewNodes[index],szName,10);
		cRenameISO.m_iNewNode = m_iNewNodes[index];
		cRenameISO.m_cOriginalNode = _itoa(m_iOriginalNodes[index],szName,10);
		if (cRenameISO.DoModal() == IDOK)
		{
			m_cListBox.DeleteString(index);
			m_cListBox.InsertString(index,cRenameISO.m_cNewName);

			m_iNewNodes[index] = cRenameISO.m_iNewNode;
		}
		m_cListBox.SetCurSel(index);
	}
}

void CReOrganize::OnRenameRevert() 
{
	// TODO: Add your control notification handler code here
	int iSelection = m_cListBox.GetCurSel();
	int iCount = m_cListBox.GetCount();
	for (int iIndex = 0; iIndex < iCount; iIndex++)
	{
		m_cListBox.DeleteString(iIndex);
		m_cListBox.InsertString(iIndex,m_cOriginalNames[iIndex]);

		m_iNewNodes[iIndex] = m_iOriginalNodes[iIndex];

	}
	if (iSelection != LB_ERR)
		m_cListBox.SetCurSel(iSelection);
}

void CReOrganize::OnDblclkList1() 
{
	// TODO: Add your control notification handler code here
	OnRenameSelected();	
}

LRESULT CReOrganize::OnPreDrop(WPARAM /*wParam*/, LPARAM lParam)
{
//	return;
//	HTREEITEM hInsItemDrag = (HTREEITEM)wParam;
	m_hComItemDrop = (HTREEITEM)lParam;
//	bool				bNodeConflict = false;
//	TV_ITEM				tvItem;
//	int iDragNode = GetPrivateProfileInt(sztBuffer,"NODE",-1,m_szIniFile);
//	int iDropNode;
//	tvstruct.item.iImage = 4;
	return 0;
/*
	CString cDrag = m_cTreeView.GetItemText(hInsItemDrag);
	int iDropPort = GetPrivateProfileInt(cDrag,"NODE",-2,m_szIniFile);

	if (m_cTreeView.ItemHasChildren(hComItemDrop))
	{
	   HTREEITEM hNextItem;
	   HTREEITEM hChildItem = m_cTreeView.GetChildItem(hComItemDrop);

	   while (hChildItem != NULL)
	   {
		  hNextItem = m_cTreeView.GetNextItem(hChildItem, TVGN_NEXT);
		  CString cInstrument = m_cTreeView.GetItemText(hChildItem);
		  int iChildPort = GetPrivateProfileInt(cInstrument,"NODE",-1,m_szIniFile);
		  if ((iChildPort == iDropPort) || (iChildPort == -1 && iDropPort != -1))
		  {
			  m_cTreeView.SetItemImage(hInsItemDrag,4,4);
			  m_cTreeView.SetItemImage(hChildItem,4,4);
		  }
		  else
		  {
			  m_cTreeView.SetItemImage(hInsItemDrag,2,3);
			  m_cTreeView.SetItemImage(hChildItem,2,3);
		  }
		  hChildItem = hNextItem;
	   }
	}
*/
	return 0;
}

LRESULT CReOrganize::OnPostDrop(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	HTREEITEM	hInstruments[MAX_INSTRUMENTS];
	int			iInsNode[MAX_INSTRUMENTS];
	int			iWatchNode[MAX_INSTRUMENTS];
	bool		bAnySetRed;
	bAnySetRed = false;
	Invalidate();

	HTREEITEM hRootItem = m_cTreeView.GetRootItem();
	if (hRootItem != NULL)
	{
		HTREEITEM hSiblingNextItem;
		HTREEITEM hSiblingItem = hRootItem;
		while (hSiblingItem != NULL)
		{
			if (hSiblingItem == m_hComItemDrop)
				m_cTreeView.Expand(hSiblingItem,TVE_EXPAND);
			hSiblingNextItem = m_cTreeView.GetNextSiblingItem(hSiblingItem);
//			CString cComm = m_cTreeView.GetItemText(hSiblingItem);

//			HTREEITEM hInsItemDrag = (HTREEITEM)wParam;
			if (m_cTreeView.ItemHasChildren(hSiblingItem))
			{

				HTREEITEM hNextItem;
				HTREEITEM hChildItem = m_cTreeView.GetChildItem(hSiblingItem);
				int iChildCount = 0;
				memset(iInsNode,-1,sizeof(iInsNode));
				memset(iWatchNode,-1,sizeof(iWatchNode));
				while (hChildItem != NULL)
				{
					hNextItem = m_cTreeView.GetNextItem(hChildItem, TVGN_NEXT);
					CString cInstrument = m_cTreeView.GetItemText(hChildItem);
					hInstruments[iChildCount] = hChildItem;
					int iInstNode = GetPrivateProfileInt(cInstrument,NODE,-1,g_szIniFile);
					char szType[32];
					GetPrivateProfileString(INSTRUMENTS,cInstrument,"",szType,sizeof(szType),g_szIniFile);
					if (strcmp(szType,"WATCH")==0)
						iWatchNode[iChildCount] = iInstNode;
					else
						iInsNode[iChildCount] = iInstNode;
					hChildItem = hNextItem;
					iChildCount++;
				}
				for (int iIndex = 0; iIndex < iChildCount; iIndex++)
				{
					bool bSetRed = false;
					if (iInsNode[iIndex] != -1)
						for (int iIndex2 = 0; iIndex2 < iChildCount; iIndex2++)
						{
							if ((iInsNode[iIndex2] != -1) &&(iInsNode[iIndex2] == iInsNode[iIndex]) && (iIndex != iIndex2))
								bSetRed = true;
						}
					if (iWatchNode[iIndex] != -1)
						for (int iIndex2 = 0; iIndex2 < iChildCount; iIndex2++)
						{
							if ((iWatchNode[iIndex2] != -1) && (iWatchNode[iIndex2] == iWatchNode[iIndex]) && (iIndex != iIndex2))
								bSetRed = true;
						}
					if (bSetRed)
					{
						bAnySetRed = true;
						m_cTreeView.SetItemImage(hInstruments[iIndex],4,4);
					}
					else
					{
						m_cTreeView.SetItemImage(hInstruments[iIndex],2,3);
					}
				}
			}

			hSiblingItem = hSiblingNextItem;
		}
	}
	if (bAnySetRed)
		AfxMessageBox("WARNING: Node Number Conflict!\nEdit Node number to fix.",MB_OK | MB_ICONEXCLAMATION);
	return 0;
}

void CReOrganize::OnButtonRegister() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDialog(TRUE,"dll",
		NULL,OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_PATHMUSTEXIST,
		"DLL Files (*.dll)|*.dll|All Files (*.*)|*.*||",this);
	if (FileDialog.DoModal()==IDOK)
	{
		if (_spawnl(_P_WAIT,"regsvr32.exe","regsvr32.exe","-s",FileDialog.GetPathName(),NULL)!=0)
		{
			switch (errno) {
			case E2BIG:
				break;
			case EINVAL:
				break;
			case ENOEXEC:
				break;
			case ENOENT:
				break;
			case ENOMEM:
				break;
			default:
				AfxMessageBox("Register DLL Failed");
			}
		}
	}
}
