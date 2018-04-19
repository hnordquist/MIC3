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

// User: Moore        Date: 11/14/05   Time: 3:19p
// Repaired DR00159 Not all drives are selectable in the Copy Files

// 
// CopyFiles.cpp : implementation file
//
#include "stdafx.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "globals.h"
#include "FileMgr.h"
#include "CopyFiles.h"
#include "DoFileCopy.h"
#include "TimedMessageDialog.h"
#include "dirdialog.h"
#include  <io.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyFiles dialog
UINT uiQueryCancelAutoPlay;													//DR00175


CCopyFiles::CCopyFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CCopyFiles::IDD, pParent)
{
//	TRACE("CCopyFiles::CTOR\n");
	uiQueryCancelAutoPlay = RegisterWindowMessage(TEXT("QueryCancelAutoPlay"));		//DR00175

	//{{AFX_DATA_INIT(CCopyFiles)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCopyFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyFiles)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCopyFiles, CDialog)
	//{{AFX_MSG_MAP(CCopyFiles)
	ON_WM_TIMER()
	ON_WM_DEVICECHANGE()											 		//DR00175
	ON_BN_CLICKED(IDC_DRIVE1_RADIO, OnDrive1Radio)
	ON_BN_CLICKED(IDC_DRIVE2_RADIO, OnDrive2Radio)
	ON_BN_CLICKED(IDC_DRIVE3_RADIO, OnDrive3Radio)
	ON_BN_CLICKED(IDC_DRIVE4_RADIO, OnDrive4Radio)
	ON_BN_CLICKED(IDC_DRIVE5_RADIO, OnDrive5Radio)
	ON_BN_CLICKED(IDC_DRIVE6_RADIO, OnDrive6Radio)
	ON_BN_CLICKED(IDC_DRIVE7_RADIO, OnDrive7Radio)
	ON_BN_CLICKED(IDC_DRIVE8_RADIO, OnDrive8Radio)
	ON_EN_CHANGE(IDC_INSPECTION_EDIT, OnChangeInspectionEdit)
	ON_EN_CHANGE(IDC_INSPECTORID_EDIT, OnChangeInspectoridEdit)
	ON_EN_CHANGE(IDC_INSPECTORNAME_EDIT, OnChangeInspectornameEdit)
	ON_BN_CLICKED(IDC_DRIVEDEF_RADIO, OnDrivedefRadio)
	ON_BN_CLICKED(IDC_TILLCLOSE_BUTTON, OnTillcloseButton)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_CF_BROWSE, OnButtonCfBrowse)					//DR00159
	ON_REGISTERED_MESSAGE(uiQueryCancelAutoPlay, OnQueryCancelAutoPlay)		//DR00175
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopyFiles message handlers

LRESULT CCopyFiles::OnQueryCancelAutoPlay(WPARAM wParam,LPARAM lParam)		//DR00175
{																			//DR00175
	return TRUE;															//DR00175
}																			//DR00175

BOOL CCopyFiles::OnDeviceChange(UINT nEventType, DWORD dwData)				//DR00175
{																			//DR00175
	if (nEventType == DBT_DEVICEARRIVAL)									//DR00175
	{																		//DR00175
		PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)dwData;			//DR00175
		char cDisk;															//DR00175
		unsigned int mCheck = 1;											//DR00175
		cDisk = 'A';														//DR00175
		while (!(pbv->dbcv_unitmask & mCheck) && (mCheck < 0x2000000) )		//DR00175
		{																	//DR00175
			cDisk++;														//DR00175
			mCheck = mCheck<<1;												//DR00175
		}																	//DR00175
		m_pDestination.Format("%c:\\",cDisk);								//DR00175
																			//DR00175
		GetDlgItem(IDC_DRIVEDEF_RADIO)->SetWindowText(m_pDestination);		//DR00175
																			//DR00175
		//set the default radio button on all others off					//DR00175
		((CButton*)GetDlgItem(IDC_DRIVEDEF_RADIO))->SetCheck(1);			//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE1_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE2_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE3_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE4_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE5_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE6_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE7_RADIO))->SetCheck(0);				//DR00175
		((CButton*)GetDlgItem(IDC_DRIVE8_RADIO))->SetCheck(0);				//DR00175
																			//DR00175
		//save the new destination											//DR00175
		WritePrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,m_pDestination,g_szIniFile);	//DR00175
	}																		//DR00175
/*																			//DR00175
	//at this point we don't really care if the media has been removed		//DR00175
	else if (nEventType == DBT_DEVICEREMOVECOMPLETE)						//DR00175
	{																		//DR00175
		PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)dwData;			//DR00175
		char cDisk;															//DR00175
		unsigned int mCheck = 1;											//DR00175
		cDisk = 'A';														//DR00175
		while (!(pbv->dbcv_unitmask & mCheck)&& (mCheck < 0x2000000))		//DR00175
		{ 																	//DR00175
			cDisk++;														//DR00175
			mCheck = mCheck<<1;												//DR00175
		}																	//DR00175
		CString cMsg;														//DR00175
	}																		//DR00175
*/																			//DR00175
	return TRUE;															//DR00175
}																			//DR00175

void CCopyFiles::OnTimer(UINT nIDEvent) 
{
	// hide this window if the user has selected to do the file copy
	int temp = 0;
	int i;
	//are all instruments paused
	bool AllPaused;
	
	if (m_bDoingDoFileCopy)
	{
		ShowWindow(SW_HIDE);
		KillTimer(1);
		KillTimer(2);
		m_dToClose = 0;
	}

	//flash icons depending on state
	switch (nIDEvent) 
	{
	case 1:	
		//waiting for information in the three inspector cedits
		if (m_hInspectState == m_hIconYellow1)
		{
			m_hInspectState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_1))->SetIcon(m_hInspectState);
		}
		else if
			(m_hInspectState == m_hIconYellow2)
		{
			m_hInspectState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_1))->SetIcon(m_hInspectState);
		}
		//waiting for dumping data from bbm in instruments to finish	
		if (m_hDestState == m_hIconYellow1)
		{
			m_hDestState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_3))->SetIcon(m_hDestState);
		}
		else if
			(m_hDestState == m_hIconYellow2)
		{
			m_hDestState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_3))->SetIcon(m_hDestState);
		}
	
		//test the drive to see if it is accessible
		TestDrive();
		break;
		
	case 2:
		//initial timer to wait until information
		//comes back from instruments has expired
		//then do the "normal" timer to update the
		//progress indicator
		KillTimer(2);
		SetTimer(3,2000,NULL);

	case 3:
		temp = 0;
		//are all instruments paused
		AllPaused = true;
		VARIANT_BOOL vbInShutDown, vbInStartUp, vbIsPaused;
		ULONG ulCurrentBBM;

		for (i = 0; i < g_iInstCount; i++)
		{
			//if ((g_pIISO[i]) && 
			//	(g_dInstType[i] != WATCH) &&
			//	(g_dInstType[i] != APC_UPS) &&
			//	(g_dInstType[i] != AQUILA_UPS) &&
			//	(g_dInstType[i] != BELKIN_UPS) )
			if (g_eISOCategory[i] == STANDARD_ISO)
			{
				//int Current_State = g_pIISO[i]->m_dCurrent_State;
				//not in 
				//if ((Current_State < 9000) && (Current_State >= 1100))
				g_pIISO[i]->get_InShutDown(&vbInShutDown);
				g_pIISO[i]->get_InStartUp(&vbInStartUp);

				if ((vbInShutDown == VARIANT_FALSE) &&
					 (vbInStartUp == VARIANT_FALSE))
				{
					g_pIISO[i]->get_CurrentBBM(&ulCurrentBBM);
					temp += ulCurrentBBM;

					g_pIISO[i]->get_IsPaused(&vbIsPaused);

					if (vbIsPaused == VARIANT_FALSE) 
						AllPaused = false;
				}
			}
		}

		//bytes in bbm
		if(temp > m_dTotalBytesinBBM)
			m_dTotalBytesinBBM = temp;

		//if not all paused and there is still bytes 
		//in bbm then update the progress bar
		if (temp && !AllPaused)
		{
			if (m_dTotalBytesinBBM!=temp)
				((CProgressCtrl*)GetDlgItem(IDC_DUMP_PROGRESS))->SetPos((100*(m_dTotalBytesinBBM-temp))/m_dTotalBytesinBBM);
			m_hDumpState = m_hIconRed;
			((CStatic*)GetDlgItem(IDC_ICON_2))->SetIcon(m_hDumpState);
		}

		//if all paused then we are done waiting
		if (AllPaused)
		{
			((CProgressCtrl*)GetDlgItem(IDC_DUMP_PROGRESS))->SetPos(100);
			SetDlgItemText(IDC_DUMP_MESSAGE,
				"Dumping Data Complete - MIC will automatically resume when done with copy");
			m_hDumpState = m_hIconGreen;
			((CStatic*)GetDlgItem(IDC_ICON_2))->SetIcon(m_hDumpState);
		}
		break;
	case 4:
		{
			if (m_dToClose*1000 >= g_iDlgCloseMillisec)
				OnCancel();
			else
			{
				char temp[32];
				int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
				int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
				sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);

				GetDlgItem(IDC_TILLCLOSE_BUTTON)->SetWindowText(temp);
				if (m_hDumpState == m_hIconGreen)m_dToClose++;
			}
		}
	}

	//iaea has different rules to go by
	//if iaea then we MUST have information 
	//in the three inspector cedits,
	//the rest of us only the the other
	//information in order to continue
	if (g_bInIAEA)
		if ((m_hDumpState == m_hIconGreen) &&
			(m_hDestState == m_hIconGreen) &&
			(m_hMediaState== m_hIconGreen) &&
			(m_hInspectState== m_hIconGreen)) 
			GetDlgItem(IDOK)->EnableWindow(true);
		else
		{
			GetDlgItem(IDOK)->EnableWindow(false);
		}
	else
		if ((m_hDumpState == m_hIconGreen) &&
			(m_hDestState == m_hIconGreen) &&
			(m_hMediaState== m_hIconGreen))
			GetDlgItem(IDOK)->EnableWindow(true);
		else
		{
			GetDlgItem(IDOK)->EnableWindow(false);
		}
	
	CDialog::OnTimer(nIDEvent);
}

//watch for the drive media to be inserted
void CCopyFiles::TestDrive()
{
	UINT oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	//check for read and write
	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   
	char ext[_MAX_EXT];
	_splitpath(m_pDestination,drive,dir,fname,ext);
	bool good;
	if (strlen(drive) == 0)
		good = (_access(dir,2)==0);
	else
		good = (_access(drive,2)==0);
	if (good)
	{
		if (m_hMediaState != m_hIconGreen)
		{
			m_dToClose = 0;
			m_hMediaState = m_hIconGreen;
			((CStatic*)GetDlgItem(IDC_ICON_4))->SetIcon(m_hMediaState);
		}
	}
	else
	{
		if (m_hMediaState == m_hIconYellow1)
			m_hMediaState = m_hIconYellow2;
		else
			m_hMediaState = m_hIconYellow1;
		((CStatic*)GetDlgItem(IDC_ICON_4))->SetIcon(m_hMediaState);
	}

	SetErrorMode(oldErrorMode);
}

int CCopyFiles::DoModal(CFileMgr* pParent) 
{
//	TRACE("CCopyFiles::DoModal\n");

	//save the parent
	m_pFileMgrParent = pParent;
	return CDialog::DoModal();
}

BOOL CCopyFiles::OnInitDialog() 
{
//	TRACE("CCopyFiles::OnInitDialog\n");
	CDialog::OnInitDialog();
	char buff[_MAX_PATH];

	//are we under IAEA rules
	GetPrivateProfileString(CONFIGURATION,MODE,"IAEA",buff,sizeof(buff),g_szIniFile);

	if (strcmp("IAEA",buff)==0)
		g_bInIAEA = true;
	else
		g_bInIAEA = false;

	SetDlgItemText(IDC_IAEA_STATIC,buff);

	//get the previous IP date
	GetPrivateProfileString(CONFIGURATION,IPDATE,"NONE",buff,sizeof(buff),g_szIniFile);
	m_pPrevious = buff;
	SetDlgItemText(IDC_PREVIOUS_STATIC,buff);

	//setup the icons to the initial state
//	m_hIconYellow1	= AfxGetApp()->LoadIcon(IDI_ICON_YELLOWR);
//	m_hIconYellow2	= AfxGetApp()->LoadIcon(IDI_ICON_YELLOWR2);
	m_hIconYellow1	= AfxGetApp()->LoadIcon(IDI_ICON11);
	m_hIconYellow2	= AfxGetApp()->LoadIcon(IDI_ICON12);
	m_hIconRed		= AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconGreen	= AfxGetApp()->LoadIcon(IDI_ICON_GREENT);
	m_hInspectState	= (g_bInIAEA)?m_hIconYellow1:m_hIconGreen;
	m_hDumpState	= m_hIconRed;
	m_hMediaState	= m_hIconYellow1;
	((CStatic*)GetDlgItem(IDC_ICON_1))->SetIcon(m_hInspectState);
	((CStatic*)GetDlgItem(IDC_ICON_2))->SetIcon(m_hDumpState);
	((CStatic*)GetDlgItem(IDC_ICON_4))->SetIcon(m_hMediaState);

	//set the facility name
	SetDlgItemText(IDC_FACILITYNAME_STATIC,g_sFacilityName);

	//the date and time of this IP 
	theTime = COleDateTime::GetCurrentTime();
	m_pDateTime = theTime.Format("%A, %d %B %Y %H:%M:%S");
	CString s = theTime.Format("%A, %d %B %Y" );
	SetDlgItemText(IDC_TODAYSDATE_STATIC,s);

	//what drives/types do we have
	int index = 0;

	//we had a problem because I was using IDC_DRIVE1_RADIO+index
	//this only works if the resource IDs are in numerical order
	//somehow they got out of order so I rewrote the code to use
	//the following array.  Now we don't care what order the ID 
	//are in.

	// This is the fix for DR MIC 0007 - Filecopy crashes when 
	// A: is not physically present.  PJM 6/6/2005
	unsigned int uiDriveResources[8];
	uiDriveResources[0] = IDC_DRIVE1_RADIO;
	uiDriveResources[1] = IDC_DRIVE2_RADIO;
	uiDriveResources[2] = IDC_DRIVE3_RADIO;
	uiDriveResources[3] = IDC_DRIVE4_RADIO;
	uiDriveResources[4] = IDC_DRIVE5_RADIO;
	uiDriveResources[5] = IDC_DRIVE6_RADIO;
	uiDriveResources[6] = IDC_DRIVE7_RADIO;
	uiDriveResources[7] = IDC_DRIVE8_RADIO;

	for (int i=0; ((i<26) && (index < 8)); ++i)
	{
		sprintf(buff,"%c: ",'A'+i);
		char tbuff[8];
		strcpy(tbuff,buff);
		tbuff[2] = '\\';
		int ret = GetDriveType(tbuff);
		switch (ret)
		{
		case DRIVE_UNKNOWN:
			strcat(buff,"(unknown)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		case DRIVE_NO_ROOT_DIR: 
			break;
		case DRIVE_REMOVABLE:
			strcat(buff,"(removable)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		case DRIVE_FIXED:
			strcat(buff,"(fixed)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		case DRIVE_REMOTE:
			strcat(buff,"(remote)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		case DRIVE_CDROM:
			{
				DWORD dwSectorsPerCluster;     // sectors per cluster
				DWORD dwBytesPerSector;        // bytes per sector
				DWORD dwNumberOfFreeClusters;  // free clusters
				DWORD dwTotalNumberOfClusters;  // total clusters
				GetDiskFreeSpace(tbuff,
					&dwSectorsPerCluster,
					&dwBytesPerSector,
					&dwNumberOfFreeClusters,
					&dwTotalNumberOfClusters);
				if (dwNumberOfFreeClusters)
				{
					strcat(buff,"(cd-r/rw)");
					GetDlgItem(uiDriveResources[index])->EnableWindow(true);
				}
				else
				{
					strcat(buff,"(cd)");
					GetDlgItem(uiDriveResources[index])->EnableWindow(false);
				}
				SetDlgItemText(uiDriveResources[index],buff);
				index++;
			}
			break;
		case DRIVE_RAMDISK:
			strcat(buff,"(ram disk)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		default:
			strcat(buff,"(Unknown)");
			SetDlgItemText(uiDriveResources[index],buff);
			index++;
			break;
		}
	}

	//get the default selection
	GetPrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,"UNKNOWN",buff,sizeof(buff),g_szIniFile);
	if (strcmp("UNKNOWN",buff)==0)
	{
		GetDlgItem(IDC_DRIVEDEF_RADIO)->ShowWindow(SW_HIDE);
		m_hDestState = m_hIconYellow1;
		((CStatic*)GetDlgItem(IDC_ICON_3))->SetIcon(m_hDestState);
		m_pDestination.Empty();
	}
	else
	{
		SetDlgItemText(IDC_DRIVEDEF_RADIO,buff);
		m_hDestState = m_hIconGreen;
		((CStatic*)GetDlgItem(IDC_ICON_3))->SetIcon(m_hDestState);
		m_pDestination = buff;
		((CButton*)GetDlgItem(IDC_DRIVEDEF_RADIO))->SetCheck(1);
	}

	//make the usable ones visible
	for (;index < 8;index++)  //<<QA>>
		GetDlgItem(uiDriveResources[index])->ShowWindow(SW_HIDE);
	//for (;index <= 7;index++)  //<<QA>>
	//	GetDlgItem(IDC_DRIVE1_RADIO+index)->ShowWindow(SW_HIDE);
	
	//place the insertion point in the "inspector name" cedit
	GetDlgItem(IDC_INSPECTORNAME_EDIT)->SetFocus();

	//disable the ok window
	GetDlgItem(IDOK)->EnableWindow(false);

	//start getting the bbm
	m_dTotalBytesinBBM = 0;
	VARIANT_BOOL vbInShutDown;
	VARIANT_BOOL vbInPowerFail = VARIANT_FALSE;
	ULONG ulCurrentBBM;

	for (int i = 0; i < g_iInstCount;i++)
	{
		if (g_pIISO[i])
		{
			g_pIISO[i]->get_InShutDown(&vbInShutDown);
			if (vbInShutDown == VARIANT_FALSE)
			{
				g_pIISO[i]->DriveBBMToZero(VARIANT_TRUE,vbInPowerFail);
				g_pIISO[i]->get_CurrentBBM(&ulCurrentBBM);
				m_dTotalBytesinBBM += ulCurrentBBM;
			}
		}
	}

	//0 to 100 percent on the progress bar
	((CProgressCtrl*)GetDlgItem(IDC_DUMP_PROGRESS))->SetRange(0,100);

	//not yet
	m_bDoingDoFileCopy = false;

	//update/toggle icons every 1/2 second
	SetTimer(1,500,NULL);	
	//don't update progress for 4 seconds then every 2 seconds
	//the four seconds will allow for initial update of bbm 
	//on instruments which are currently paused
	SetTimer(2,(m_dTotalBytesinBBM==0)?4000:2000,NULL);
	SetTimer(4,1000,NULL);
	m_dToClose = 0;
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//respond to a drive radio button being selected
void CCopyFiles::OnDriveRadio(UINT RadioButton, int Count)
{
	m_dToClose = 0;
	//update the icons
	//we will always have a destination selected
	if (m_hDestState != m_hIconGreen)
	{
		m_hDestState = m_hIconGreen;
		((CStatic*)GetDlgItem(IDC_ICON_3))->SetIcon(m_hDestState);
	}
	//start with the belief that there is no media in selected drive
	m_hMediaState = m_hIconYellow1;
	((CStatic*)GetDlgItem(IDC_ICON_4))->SetIcon(m_hMediaState);
	//get the destination from the button selected
	GetDlgItemText(RadioButton,m_pDestination);
	//count is the number of characters of the check box 
	//title to keep -- the default can be long so in its case
	//we call with count == 0;
	if (Count)
		m_pDestination = m_pDestination.Left(Count) + "\\";
	GetDlgItem(IDOK)->EnableWindow(false);
	//can we write to the drive
	TestDrive();
}

//catch a drive radio button
void CCopyFiles::OnDrive1Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE1_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive2Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE2_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive3Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE3_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive4Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE4_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive5Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE5_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive6Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE6_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive7Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE7_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrive8Radio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVE8_RADIO,2);
}

//catch a drive radio button
void CCopyFiles::OnDrivedefRadio() 
{
	// TODO: Add your control notification handler code here
	OnDriveRadio(IDC_DRIVEDEF_RADIO,0);
}

//catch change of inspection cedit
void CCopyFiles::OnChangeInspectionEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_INSPECTION_EDIT, m_pInspection);
	InspectionIconUpdate();
	m_dToClose = 0;
}

//catch change of inspector id cedit
void CCopyFiles::OnChangeInspectoridEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_INSPECTORID_EDIT, m_pInspectorID);
	InspectionIconUpdate();
	m_dToClose = 0;
}

//catch change of inspector name cedit
void CCopyFiles::OnChangeInspectornameEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	GetDlgItemText(IDC_INSPECTORNAME_EDIT,m_pInspectorName);
	InspectionIconUpdate();
	m_dToClose = 0;
}

//update the inspection icon
void CCopyFiles::InspectionIconUpdate()
{
	//if any of the three are empty then
	//we need to have a yellow icon
	if (m_pInspectorName.IsEmpty() ||
		m_pInspectorID.IsEmpty() ||
		m_pInspection.IsEmpty())
	{
		if (m_hInspectState==m_hIconGreen)
		{
			m_hInspectState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_1))->SetIcon(m_hInspectState);
			if (g_bInIAEA) 
				GetDlgItem(IDOK)->EnableWindow(false);
		}
	}
	//else all have something in them so
	//change it to green icon if it isn't there already
	else
	{
		if (m_hInspectState!=m_hIconGreen)
		{
			m_hInspectState = m_hIconGreen;
			((CStatic*)GetDlgItem(IDC_ICON_1))->SetIcon(m_hInspectState);
		}
	}
}

//go do the actual copy 
void CCopyFiles::OnOK() 
{

	//the system's date and time to be
	//the date and time in the IP, SG, and MN file names
	COleDateTime Today = COleDateTime::GetCurrentTime();
	CString Date;
	CString m_pIPName;
	//iaea format and other format
	if (g_bInIAEA)
		Date = Today.Format("%b%y%d");
	else
		Date = Today.Format("%Y_%b_%d_%H%M");

	//do the actual file copy
	CDoFileCopy FileCopy(this);
	m_bDoingDoFileCopy = true;
	m_dToClose = 0;
	if (FileCopy.DoModal(m_pDestination,m_pPrevious,m_pDateTime,
		m_pInspectorName,m_pInspectorID,m_pInspection)!=IDOK)
	{
		//cancel out of it so restart the icon update timers
		m_bDoingDoFileCopy = false;
		m_dToClose = 0;
		SetTimer(1,500,NULL);	
		SetTimer(2,500,NULL);
		SetTimer(4,1000,NULL);
		//and show the window again
		ShowWindow(SW_SHOW);
	}
	else
	{
		//exited normally from the copy activity and
		//we halted all the instruments on the way in so
		//attempt to immediately restart them all
		VARIANT_BOOL vbInPowerFail = VARIANT_FALSE;

		for (int i = 0; i < g_iInstCount; i++)
		{
			if (g_pIISO[i])
				g_pIISO[i]->DriveBBMToZero(VARIANT_FALSE,vbInPowerFail);
		}
		CDialog::OnOK();
	}
}

void CCopyFiles::OnCancel() 
{
	// canceling out of this dialog and we halted
	//all of the instruments on the way in so this
	//time we want to restart them all
	VARIANT_BOOL vbInPowerFail = VARIANT_FALSE;

	for (int i = 0; i < g_iInstCount; i++)
	{
		if (g_pIISO[i])
			g_pIISO[i]->DriveBBMToZero(VARIANT_FALSE,vbInPowerFail);
	}
	CDialog::OnCancel();
}


//int CCopyFiles::DoModal(CFileMgr* pParent) 
//{
	// TODO: Add your specialized code here and/or call the base class
	//save the parent
	//m_pParent = pParent;
//	return CDialog::DoModal();
//}

void CCopyFiles::OnTillcloseButton() 
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;
	if (g_iDlgCloseMillisec/1000 - m_dToClose > MAX_EXTEND) 
		m_dToClose = g_iDlgCloseMillisec/1000 - MAX_EXTEND;
	int minutes = (g_iDlgCloseMillisec/1000-m_dToClose)/60;
	int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose) % 60;
	sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);
	GetDlgItem(IDC_TILLCLOSE_BUTTON)->SetWindowText(temp);
}

void CCopyFiles::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_dToClose = 0;

	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		CString cstitle("Action Indicator Icon");
		CString csmessage;
		csmessage.Empty();

		KillTimer(4);

		switch(index)
		{
		case IDC_ICON_1:
			csmessage = CString("This icon prompts you to Enter Inspector Data.");
			break;
		case IDC_ICON_2:
			csmessage = CString("This icon informs you when dumping is completed.");
			break;
		case IDC_ICON_3:
			csmessage = CString("This icon is a reminder to verify the destination that the files are being copied to.");
			break;
		case IDC_ICON_4:
			csmessage = CString("This icon informs you when it is time to Insert Destination Media.");
			break;
		case IDC_ICON_5:
		case IDC_ICON_6:
		case IDC_ICON_7:
		case IDC_ICON_8:
			csmessage = CString("This icon is not active on this screen.");
			break;
		}

		if (!csmessage.IsEmpty())
		{
			new CTimedMessageDialog(cstitle,csmessage,g_iDlgCloseMillisec/1000,this);
		}

		SetTimer(4,1000,NULL);  //<<QA>> do we still need this?
	}
	CDialog::OnLButtonDown(nFlags, point);
}
void CCopyFiles::OnButtonCfBrowse() 
{
	CDirDialog DirDialog(this);
	DirDialog.m_strTitle = "Select Default Destination for Copy:";
	if (g_bHideWindows)
		ShowWindow(SW_HIDE);
	if (DirDialog.DoBrowse())
	{
		//get the path from the dialog
		m_pDestination = CString(DirDialog.m_strPath);
		GetDlgItem(IDC_DRIVEDEF_RADIO)->SetWindowText(m_pDestination);

		//set the default radio button on all others off
		((CButton*)GetDlgItem(IDC_DRIVEDEF_RADIO))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_DRIVE1_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE2_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE3_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE4_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE5_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE6_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE7_RADIO))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_DRIVE8_RADIO))->SetCheck(0);

		//save the new destination
		WritePrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,m_pDestination,g_szIniFile);
	}
	ShowWindow(SW_SHOW);
}