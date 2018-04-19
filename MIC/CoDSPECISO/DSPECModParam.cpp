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
// DSPECModParam.cpp

#include "stdafx.h"
#include "invalid_chars.h"
#include "INI_definitions.h"
#include "resource.h"
#include "colors.h"
#include "ISO_definitions.h"
#include "DSPECModParam.h"
#include "DirDialog.h"
#include "DSPECInst.h"
#include "TimedMessageDialog.h"
#include "DSPECStaticSetupData.h"
#include "DSPECStrings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int COMMFAIL_SPIN_MAX = 99999;
const int COMMFAIL_SPIN_MIN = 1;

const int LONGDWELL_SPIN_MIN = 30;		//seconds
const int LONGDWELL_SPIN_MAX = 99999;

const int MAXPAUSE_SPIN_MIN	 = 0;		//seconds
const int MAXPAUSE_SPIN_MAX	 = 99999;

const int MAXSTATUS_SPIN_MIN = 0;
const int MAXSTATUS_SPIN_MAX = 99999;

const int REALTIMEDWELL_SPIN_MIN = 1;  //  1 second
const int REALTIMEDWELL_SPIN_MAX = 10; // 10 seconds

const int SHORTDWELL_SPIN_MIN = 10;     // 10 seconds
const int SHORTDWELL_SPIN_MAX = 99999;

const int SLOWRESET_SPIN_MIN  = 120;
const int SLOWRESET_SPIN_MAX  = 99999;

const int THRESHOLD_SPIN_MIN = 0;
const int THRESHOLD_SPIN_MAX = 0x7FFFFFFF;

const int TIMEOUT_SPIN_MIN = 0;
const int TIMEOUT_SPIN_MAX = 99999;

const int FILEID_SPIN_MAX = 99;
const int FILEID_SPIN_MIN = 0;

// These are calculated in the constructor by scanning the default strings.
int HV_HIGH_ERRORLIMIT_SPIN_MIN;
int HV_HIGH_ERRORLIMIT_SPIN_MAX;
int HV_LOW_ERRORLIMIT_SPIN_MIN;
int HV_LOW_ERRORLIMIT_SPIN_MAX;
int HV_FAIL_SPIN_MAX;
int HV_FAIL_SPIN_MIN;

#define NO  "No"
#define YES "Yes"

/////////////////////////////////////////////////////////////////////////////
// CDSPECModParam property page

IMPLEMENT_DYNCREATE(CDSPECModParam, CPropertyPage)

CDSPECModParam::CDSPECModParam() : CPropertyPage(CDSPECModParam::IDD)
{
}

CDSPECModParam::CDSPECModParam(
	UINT id, CISO* pParent,bool DoingConfig)
	: m_pParent(pParent), 
	  m_bCollectingParametersForNewISO(DoingConfig), 
	  CPropertyPage(id)
{
//	TRACE("CDSPECModParam::CTOR #2\n");
	m_bAllowFinish = false;
	m_bChange = false;
	m_datasetup = false;
	m_pPS = NULL;
	m_pApplyButton = NULL;

	for (int i= IDC_DSPECMODPARAMS_FIRST; i <= IDC_DSPECMODPARAMS_LAST; i++)
	{
		StaticColors[i-IDC_DSPECMODPARAMS_FIRST] = COLOR_DEFAULT_TEXT;
	}

	if (m_bCollectingParametersForNewISO)
	{
		//See if default section of INI file is already there...
		char testbuf[16];
		int numchars = GetPrivateProfileSection(
			DSPEC_DEFAULT,
			testbuf,
			sizeof(testbuf),
			m_pParent->m_szIniFile);

		//If not, then go write it
		if (numchars == 0)
			WriteIniFileDefaultSection();
	}

	int low, high;

	sscanf(HVVOLTH_DEFAULT,"%d",&high);
	sscanf(HVVOLTL_DEFAULT,"%d",&low);

	HV_HIGH_ERRORLIMIT_SPIN_MIN = low;
	HV_HIGH_ERRORLIMIT_SPIN_MAX = high;

	HV_LOW_ERRORLIMIT_SPIN_MIN = low;
	HV_LOW_ERRORLIMIT_SPIN_MAX = high;

	HV_FAIL_SPIN_MAX = high;
	HV_FAIL_SPIN_MIN = 1;
		
	//{{AFX_DATA_INIT(CDSPECModParam)
	//}}AFX_DATA_INIT
}

CDSPECModParam::~CDSPECModParam()
{
//	TRACE("CDSPECModParam::DTOR\n");
	delete m_pApplyButton;
}

BEGIN_MESSAGE_MAP(CDSPECModParam, CPropertyPage)
	//{{AFX_MSG_MAP(CDSPECModParam)

	ON_BN_CLICKED(IDC_DSPECMODPARAMS_APPLY, OnModparamsApply)
	ON_BN_CLICKED(IDC_DSPECMODPARAMS_RESET, OnModparamsReset)
	ON_BN_CLICKED(IDC_DSPECMODPARAMS_DEFAULT, OnModparamsDefaultBut)
	ON_BN_CLICKED(IDC_DSPECMODPARAMS_BROWSE, OnModparamsBrowse)
	ON_WM_LBUTTONDOWN()
	ON_WM_SHOWWINDOW()

	// COMMFAIL
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_COMMFAIL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_COMMFAIL_SPIN, OnDeltaposModparamsCommfailSpin)

	//DODUMP
	ON_BN_CLICKED(IDC_DSPECMODPARAMS_DODUMP, OnChangeModparamsMost)

	//FILEID
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_FILEID, OnChangeModparamsFileid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_FILEID_SPIN, OnDeltaposModparamsFileidSpin)
	ON_EN_KILLFOCUS(IDC_DSPECMODPARAMS_FILEID, OnKillfocusModparamsFileid)

	//HV_FAIL
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_HV_FAIL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_HV_FAIL_SPIN, OnDeltaposModparamsHVfailSpin)

	//HV_HIGH_ERROR_LIMIT
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT_SPIN, OnDeltaposModparamsHVhigherrorlimitSpin)

	//HV_LOW_ERROR_LIMIT
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT_SPIN, OnDeltaposModparamsHVlowerrorlimitSpin)

	//LONGDWELL
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_LONGDWELL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_LONGDWELL_SPIN, OnDeltaposModparamsLongdwellSpin)

	//MAXCYCLE - hardcoded to 1 second

	//MAXPAUSE
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_MAXPAUSE, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_MAXPAUSE_SPIN, OnDeltaposModparamsMaxpauseSpin)

	//MAXSTATUS
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_MAXSTATUS, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_MAXSTATUS_SPIN, OnDeltaposModparamsMaxstatusSpin)

	//NODE - not a "concept" for IPX

	//PORT
	ON_CBN_SELCHANGE(IDC_DSPECMODPARAMS_PORT, OnChangeModparamsPort)

	//QUIET
	ON_BN_CLICKED(IDC_DSPECMODPARAMS_QUIET_CHECKBOX, OnChangeModparamsMost)

	//REALTIMEDWELL
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_REALTIMEDWELL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_REALTIMEDWELL_SPIN, OnDeltaposModparamsRealtimedwellSpin)

	//SAVELOC
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_SAVELOC, OnChangeModparamsSaveloc)

	//SHORTDWELL
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_SHORTDWELL, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_SHORTDWELL_SPIN, OnDeltaposModparamsShortdwellSpin)

	//SLOW_RESET
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_SLOW_RESET, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_SLOW_RESET_SPIN, OnDeltaposModparamsSlowresetSpin)

	//THRESHOLD
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_THRESHOLD, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_THRESHOLD_SPIN, OnDeltaposModparamsThresholdSpin)

	//TIMEOUT
	ON_EN_CHANGE(IDC_DSPECMODPARAMS_TIMEOUT, OnChangeModparamsMost)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPECMODPARAMS_TIMEOUT_SPIN, OnDeltaposModparamsTimeoutSpin)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////
//
//                        SetText
//                        SetColor
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::SetText(UINT index,CHAR * text)
{
	if ((index >= IDC_DSPECMODPARAMS_FIRST) &&
		(index <= IDC_DSPECMODPARAMS_LAST))
		GetDlgItem(index)->SetWindowText(text);
}

void CDSPECModParam::SetColor(UINT index, COLORREF color)
{
	if ((index >= IDC_DSPECMODPARAMS_FIRST) &&
		(index <= IDC_DSPECMODPARAMS_LAST)) {
	  StaticColors[index-IDC_DSPECMODPARAMS_FIRST] = color;
	  GetDlgItem(index)->Invalidate();
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                        DoApply
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//                            MakeMyPath
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::MakeMyPath(const char *path)
{
	//recursive routine to create all 
	//of the directories down the "path"
	//NOTE: existance of a "file" with the 
	//same name as a node in the
	//path will cause this routine to fail
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];		
	if (_access(path,0)==0) return;
	_splitpath(path,drive,dir,fname,ext);
	if (strlen(dir)==0) return;
	_makepath(path_buffer,drive,dir,NULL,NULL);
	if (path_buffer[strlen(path_buffer)-1] == '\\')
		path_buffer[strlen(path_buffer)-1] = NULL;
	MakeMyPath(path_buffer);
	//make sure directory exists
	CreateDirectory(path_buffer,NULL);
}

void CDSPECModParam::DoApply()
{
	char data_directory[_MAX_PATH];

	bool bCarryOn;

	if (m_bCollectingParametersForNewISO)
	{
		bCarryOn = true;
	}
	else
	{
		bCarryOn = m_pParent->Validate();
	}

	if (bCarryOn)
	{

		SaveValuesToINIFile(m_pParent->m_pName);

		GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,data_directory,sizeof(data_directory));

//		if (_access(data_directory,0)==-1)
//			_mkdir(data_directory);	//this only creates the last element in path
		// Make sure the directory exists
		if (_access(data_directory, 0) != 0)
		{
			//make sure path exists
			MakeMyPath(data_directory);
		}

	}

	//propagate all the new values to the program
	//and update current values from the instrument
	//in order to check limits that may have
	//changed

	if (!m_bCollectingParametersForNewISO)
	{
		m_pParent->PostMessage(IDC_UPDATE_PARAMS,0,0);
		m_pParent->PostMessage(IDC_REQUEST_SETTINGS_UPDATES,0,0);
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                       OnModparamsApply
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnModparamsApply() 
{
//	TRACE("CDSPECModParam::OnModparamsApply()\n");

	//for all the comm device must exist
	if (m_pPS) 
		m_pPS->SetToClose(0);

	char temp[32];
	GetDlgItem(IDC_DSPECMODPARAMS_PORT)->GetWindowText(temp,sizeof(temp));

	GetPrivateProfileString(
		COMMUNICATIONS,temp,"ERROR",temp,sizeof(temp)-1,m_pParent->m_szIniFile);

	//short dwell must be less than long dwell
	int ilongdwell, ishortdwell;

	GetDlgItem(IDC_DSPECMODPARAMS_LONGDWELL)->GetWindowText(temp,sizeof(temp)-1);
	ilongdwell = atoi(temp);
	GetDlgItem(IDC_DSPECMODPARAMS_SHORTDWELL)->GetWindowText(temp,sizeof(temp)-1);
	ishortdwell = atoi(temp);

	if (ishortdwell >= ilongdwell)
	{
		CString csTitle("DSPEC Component: Apply Configuration");
		CString csText("Long dwell time must be larger than short dwell time.");

		new CTimedMessageDialog(
			csTitle, 
			csText,
			m_pParent->m_iDlgCloseMilliseconds/1000, 
			this);

		return;
	}

	DoApply();  //this does the validation
	m_bChange = false;

	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}
//////////////////////////////////////////////////////////////////////////////
//
//              OnModparamsReset - RESET TO LAST APPLIED Button
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnModparamsReset() 
{
//	TRACE("CDSPECModParam::OnModparamsReset()\n");

	if (m_pPS) 
		m_pPS->SetToClose(0);

	m_datasetup = true;

	if (m_bCollectingParametersForNewISO)
	{
		SaveValuesToINIFile(DSPEC_DEFAULT);
	}
	else
	{
		CComboBox *pcomboport = (CComboBox *)GetDlgItem(IDC_DSPECMODPARAMS_PORT);

		pcomboport->EnableWindow(FALSE);

		ResetAllButPort(m_pParent->m_pName);

		char port_name[64];

		GetPrivateProfileString(
			m_pParent->m_pName,PORT,"ERROR",port_name,sizeof(port_name),
			m_pParent->m_szIniFile);

		pcomboport->ResetContent();
		pcomboport->AddString(port_name);
		pcomboport->SelectString(-1,port_name);

		m_bChange = false;
	}
	if (m_pApplyButton)
		m_pApplyButton->EnableWindow(FALSE);
}

//////////////////////////////////////////////////////////////////////////////
//
//             OnModparamsDefaultBut
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnModparamsDefaultBut()
{
	OnModparamsDefault(false);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//             OnModparamsDefault - LOAD DEFAULT VALUES Button
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnModparamsDefault(bool SetPort)
{
//	TRACE("CDSPECModParam::OnModparamsDefault()\n");

	if (m_pPS) 
		m_pPS->SetToClose(0);

	m_datasetup = true;

	ResetAllButPort(DSPEC_DEFAULT);

	if (SetPort || m_bCollectingParametersForNewISO)
	{
	//PORT
		char ini_comm_section[16384],ini_inst_section[16384];  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
		char one_comm_entry[64],iso_name[64],temp_cso_name[64];

		bool bFoundAtLeastOne = false;

		CComboBox *pcomboport = (CComboBox *)GetDlgItem(IDC_DSPECMODPARAMS_PORT);
		pcomboport->ResetContent();
		m_bAllowFinish = false;

		DWORD dwInst = GetPrivateProfileSection(
			INSTRUMENTS,
			ini_inst_section,
			sizeof(ini_inst_section), 
			m_pParent->m_szIniFile);

		DWORD dwComm = GetPrivateProfileSection(
			COMMUNICATIONS,
			ini_comm_section,
			sizeof(ini_comm_section), 
			m_pParent->m_szIniFile);

		if (dwComm > 0)
		{
			char *at = ini_comm_section;
			
			while (strlen(at) > 0)
			{
				strcpy(one_comm_entry,at);

				//Only present IPX-type CSO's that have not 
				//been assigned to another ISO.

				char *cso_type = 1 + strchr(one_comm_entry,'=');

				//if ((strcmp(cso_type,IPXNAME)) == 0)  //if an IPX-type CSO
				{
					bool used = false;
					char *cso_name = strtok(one_comm_entry, "=");
					if (dwInst > 0)
					{
						//go through existing ISO's to see if anyone is using this port
						char *inst_at = ini_inst_section;

						while(strlen(inst_at) > 0)
						{
							char *inst_type = 1 + strchr(inst_at,'=');

							strcpy(iso_name,inst_at);
							strtok(iso_name,"=");

							if (strlen(iso_name) > 0)
							{
								GetPrivateProfileString(
									iso_name,
									PORT,
									"",
									temp_cso_name,
									sizeof(temp_cso_name),
									m_pParent->m_szIniFile);

								//An IPX CSO can be associated with a WATCH-type pseudo
								//instrument as well as with a real DSPEC.  So if this instrument
								//is a Watch window attached to an IPX CSO, then we can still 
								//use the CSO for comms with a DSPEC.
								if (0 != strcmp(inst_type, "WATCH")) //no test if watch-type inst
								{
									if (strcmp(cso_name,temp_cso_name) == 0)
									{
										used = true;
										break;
									}
								}
							}
							inst_at += strlen(inst_at) + 1;
						}
					}
					if (!used)
					{
						pcomboport->AddString(cso_name);
						bFoundAtLeastOne = true;
						m_bAllowFinish = true;
					}
				}
				at += strlen(at)+1;
			}
		}

		if (bFoundAtLeastOne)
		{
			pcomboport->SetCurSel(0);
			ResetItem(DSPEC_DEFAULT,PORT,"ERROR",IDC_DSPECMODPARAMS_PORT);
		}
		else
		{
			//No port available.  Message box will be presented by MicDlg.
			//mic.ini has incomplete info on this ISO.  MicDlg will figure
			//this out and tell the operator that a new ISO was not
			//created.  mic.ini will need manual editing.  We could do the
			//clean up of the ini file here, but that would be inconsistent
			//with the other ISO types.  Perhaps MICGM must take care of
			//that.
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                       OnSetActive
//
//////////////////////////////////////////////////////////////////////////////
BOOL CDSPECModParam::OnSetActive() 
{
	//TRACE("CDSPECModParam::OnSetActive()\n");

	BOOL result = CPropertyPage::OnSetActive();

	if (m_bCollectingParametersForNewISO) 
	{
		SetDlgItemText(IDC_DSPECMODPARAMS_STATICTEXT,"");
		m_pApplyButton->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DSPECMODPARAMS_RESET)->SetWindowText("Save As Default");
		m_pPS->SetWizardButtons(PSWIZB_FINISH);

        if (!m_datasetup)
		{
			OnModparamsDefault(true);
			if (m_pPS)
			{
				CString sCaption(m_pParent->m_pName);
				TC_ITEM tcItem;
				tcItem.mask = TCIF_TEXT;
				tcItem.pszText = (LPTSTR)((LPCTSTR)sCaption);
				m_pPS->GetTabControl()->SetItem(0, &tcItem );
			}
		}
		if (m_bAllowFinish)
			m_pPS->SetWizardButtons(PSWIZB_FINISH );
		else
			m_pPS->SetWizardButtons(PSWIZB_DISABLEDFINISH );
	}
	else
	{
		SetDlgItemText(
			IDC_DSPECMODPARAMS_STATICTEXT,
			"Changes will NOT take effect until you click on \"Apply\".");

		if (m_pApplyButton)
			m_pApplyButton->EnableWindow(m_bChange?TRUE:FALSE);
	}

	return result;
}

void CDSPECModParam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pPS) m_pPS->SetToClose(0);
	CPropertyPage::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////
//
//                   WriteIniFileDefaultSection
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::WriteIniFileDefaultSection()
{
//	TRACE("CDSPECInst::WriteIniFileDefaultSection()\n");

	char *inifile = m_pParent->m_szIniFile;

	WritePrivateProfileSection(DSPEC_DEFAULT,"",inifile);

	//MIC local settings
	WritePrivateProfileString(DSPEC_DEFAULT,COMMFAIL,COMMFAIL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,DO_DUMP,DO_DUMP_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,FILE_ID,FILEID_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,FLAGCOMM,FLAGCOMM_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,FLAGOTHR,FLAGOTHR_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,FLAGTIME,FLAGTIME_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,HV_FAIL,HV_FAIL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,HVVOLTH,HVVOLTH_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,HVVOLTL,HVVOLTL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,LONGDWELL_SEC, LONGDWELL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,MAXPAUSE,MAXPAUSE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,MAXSTATUS,MAXSTATUS_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,PORT,"",inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,QUIET,QUIET_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,REALTIMEDWELL_SEC,REALTIMEDWELL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SAVE_LOC,SAVELOC_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SHORTDWELL_SEC,SHORTDWELL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SLOW_RESET,SLOW_RESET_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,THRESHOLD,THRESHOLD_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,TIMEOUT,TIMEOUT_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SHOW_SPECTRA,SHOW_SPECTRA_DEFAULT,inifile);

	//DSPEC Instrument settings (across-the-wire settings)
	WritePrivateProfileString(DSPEC_DEFAULT,SET_BLRE_AUTOMATIC_ENABLE,SET_BLRE_AUTOMATIC_ENABLE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_CORRECTION_FLAT,SET_CORRECTION_FLAT_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_ADJUSTMENT,SET_GAIN_ADJUSTMENT_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_CHANNEL,SET_GAIN_CHANNEL_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_COARSE,SET_GAIN_COARSE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_CONVERSION,SET_GAIN_CONVERSION_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_FINE,SET_GAIN_FINE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_POLARITY,SET_GAIN_POLARITY_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_STABILIZATION_ENABLE,SET_GAIN_STABILIZATION_ENABLE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GAIN_WIDTH,SET_GAIN_WIDTH_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_GATE,SET_GATE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_HV,SET_HV_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_HV_ENABLE,SET_HV_ENABLE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_HV_POLARITY,SET_HV_POLARITY_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_MODE_STABILIZATION,SET_MODE_STABILIZATION_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_LLD,SET_LLD_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_PZ,SET_PZ_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_PZ_AUTOMATIC_ENABLE,SET_PZ_AUTOMATIC_ENABLE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_SHAP_CUSP,SET_SHAP_CUSP_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_SHAP_FLAT,SET_SHAP_FLAT_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_SHAP_RISE,SET_SHAP_RISE_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_SHUTDOWN,SET_SHUTDOWN_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_ULD,SET_ULD_DEFAULT,inifile);
	WritePrivateProfileString(DSPEC_DEFAULT,SET_ZDT_ENABLE,SET_ZDT_ENABLE_DEFAULT,inifile);
}


//////////////////////////////////////////////////////////////////////////////
//
//                        SaveItem
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::SaveItem(char *szName, 
							  char *szItem, 
							  int dDlgItem, 
							  bool FromCheckBox)
{
	char temp[MAX_PATH+1];
	if (FromCheckBox)
		strcpy(temp,(((CButton*)GetDlgItem(dDlgItem))->GetState()&0001)?"1":"0");
	else
		GetDlgItem(dDlgItem)->GetWindowText(temp,sizeof(temp));
	while (temp[strlen(temp)-1]=='\\')
		temp[strlen(temp)-1]=NULL;
	WritePrivateProfileString(szName, szItem, temp, m_pParent->m_szIniFile);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  SaveValuesToINIFile
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::SaveValuesToINIFile(char *ploc)
{
//	TRACE("CDSPECModParam::SaveValuesToINIFile()\n");
	char szTemp[_MAX_PATH];

	SaveItem(ploc, COMMFAIL,IDC_DSPECMODPARAMS_COMMFAIL);
	SaveItem(ploc, DO_DUMP,	IDC_DSPECMODPARAMS_DODUMP,true);
	SaveItem(ploc, FILE_ID,	IDC_DSPECMODPARAMS_FILEID);

	if (m_bCollectingParametersForNewISO)
	{
		// Just to place the entries into the body of the .INI file
		WritePrivateProfileString(ploc,FLAGCOMM,"0",m_pParent->m_szIniFile);
		WritePrivateProfileString(ploc,FLAGOTHR,"0",m_pParent->m_szIniFile);
		WritePrivateProfileString(ploc,FLAGTIME,"0",m_pParent->m_szIniFile);
	}

	SaveItem(ploc, HV_FAIL,		IDC_DSPECMODPARAMS_HV_FAIL);
	SaveItem(ploc, HVVOLTH,		IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT);
	SaveItem(ploc, HVVOLTL,		IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT);
	SaveItem(ploc, LONGDWELL_SEC,	IDC_DSPECMODPARAMS_LONGDWELL);
	SaveItem(ploc, MAXPAUSE,		IDC_DSPECMODPARAMS_MAXPAUSE);
	SaveItem(ploc, MAXSTATUS,		IDC_DSPECMODPARAMS_MAXSTATUS);

	//Get the name of the port (there is no "node")
	GetDlgItemText(IDC_DSPECMODPARAMS_PORT,szTemp,sizeof(szTemp));

	SaveItem(ploc,PORT,           IDC_DSPECMODPARAMS_PORT);
	SaveItem(ploc,QUIET,          IDC_DSPECMODPARAMS_QUIET_CHECKBOX,true);
	SaveItem(ploc,REALTIMEDWELL_SEC,IDC_DSPECMODPARAMS_REALTIMEDWELL);

	GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,szTemp,sizeof(szTemp));
	while ((szTemp[strlen(szTemp)-1]=='\\')||
		   (szTemp[strlen(szTemp)-1]=='/' )  ) 
	{
		szTemp[strlen(szTemp)-1] = NULL;
		SetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,szTemp);
	}
	SaveItem(ploc, SAVE_LOC, IDC_DSPECMODPARAMS_SAVELOC);
	SaveItem(ploc, SHORTDWELL_SEC, IDC_DSPECMODPARAMS_SHORTDWELL);
	SaveItem(ploc, SLOW_RESET,     IDC_DSPECMODPARAMS_SLOW_RESET);
	SaveItem(ploc, THRESHOLD,      IDC_DSPECMODPARAMS_THRESHOLD);
	SaveItem(ploc, TIMEOUT,        IDC_DSPECMODPARAMS_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////////
//
//                       ResetItem
//
//////////////////////////////////////////////////////////////////////////////
//reset float item for a spinner with a non-automatic buddy
void CDSPECModParam::ResetItem(char *szName,
							   char *szItem,
							   char*szDefault,
							   int dItem,
							   int dMin, 
							   int dMax, 
							   int dTextItem)
{
	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	float ftemp = (float)atof(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos((int)(ftemp*10.0));
	sprintf(temp,"%1.1f",ftemp);
	GetDlgItem(dTextItem)->SetWindowText(temp);
}

//reset int item for a spinner with a non-automatic buddy
void CDSPECModParam::ResetItemI(char *szName,char *szItem,char*szDefault,
						   int dItem,int /*dMin*/, int dMax, int dTextItem)
{
	int pAccel [] =
	{
		0,1,
		2,10,
		4,100,
		6,1000,
		8,10000,
	};

	char temp[32];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(0,32000);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(0);
	((CSpinButtonCtrl*)GetDlgItem(dItem))->SetAccel(5, (struct _UDACCEL *)pAccel );
	GetDlgItem(dTextItem)->SetWindowText(temp);
	_itoa(dMax,temp,10);
	((CEdit*)GetDlgItem(dTextItem))->SetLimitText(strlen(temp));
}

//reset item for a spinner with an automatic buddy
void CDSPECModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem,int dMin, int dMax)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetRange(dMin,dMax);
		((CSpinButtonCtrl*)GetDlgItem(dItem))->SetPos(dtemp);
		char temp[32];
		_itoa(dMax,temp,10);
		((CEdit*)((CSpinButtonCtrl*)GetDlgItem(dItem))->GetBuddy())->SetLimitText(strlen(temp));
}

//reset item for a check box
void CDSPECModParam::ResetItem(char *szName,char *szItem, int dDefault,int dItem)
{
	int dtemp = GetPrivateProfileInt(szName,szItem,dDefault,m_pParent->m_szIniFile);
	((CButton*)GetDlgItem(dItem))->SetCheck(dtemp);
}

//reset item for a simple edit field
void CDSPECModParam::ResetItem(char *szName, char *szItem, char *szDefault, int dItem)
{
	 // <<QA>> This looks like a no-op for the DSPEC
	char temp[MAX_PATH+1];
	//int nSelect = CB_ERR;
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	switch(dItem)
	{
		case IDC_DSPECMODPARAMS_PORT:
			((CComboBox*)GetDlgItem(dItem))->SelectString(-1, temp);
			break;

		default:;
	}
	GetDlgItem(dItem)->SetWindowText(temp);
}


void CDSPECModParam::ResetItem(char *szName, char *szItem, char *szDefault,
						  int dItemSpin, int dItem)
{
	char temp[3];
	GetPrivateProfileString(szName,szItem,szDefault,temp,sizeof(temp),m_pParent->m_szIniFile);
	if (strlen(temp)==1)
	{
		temp[1] = temp[0];
		temp[0] = '0';
		temp[2] = NULL;
	}
	SetDlgItemText(dItem,temp);
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetRange(FILEID_SPIN_MIN,FILEID_SPIN_MAX);
	((CSpinButtonCtrl*)GetDlgItem(dItemSpin))->SetPos(value);
	((CEdit*)GetDlgItem(dItem))->SetLimitText(2);
}
//////////////////////////////////////////////////////////////////////////////
//
//                       SetPropertySheet
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::SetPropertySheet(CPropertySheet *pPS)
{
	m_pPS = (CTPropertySheet*)pPS;
}
//////////////////////////////////////////////////////////////////////////////
//
//             ResetAllButPort
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::ResetAllButPort(char *ini_section_name)
{
//	TRACE("CDSPECModParam::ResetAllButPort()\n");

	int iDefault;

		//COMMFAIL
		ResetItemI(ini_section_name, 
			COMMFAIL, 
			COMMFAIL_DEFAULT, 
			IDC_DSPECMODPARAMS_COMMFAIL_SPIN,
			COMMFAIL_SPIN_MIN, 
			COMMFAIL_SPIN_MAX, 
			IDC_DSPECMODPARAMS_COMMFAIL);

		//DO_DUMP
		sscanf(DO_DUMP_DEFAULT,"%d",&iDefault);
		ResetItem(ini_section_name,
			DO_DUMP, 
			iDefault, 
			IDC_DSPECMODPARAMS_DODUMP);

		//FILEID
		ResetItem(ini_section_name,
			FILE_ID, 
			FILEID_DEFAULT,
			IDC_DSPECMODPARAMS_FILEID_SPIN,
			IDC_DSPECMODPARAMS_FILEID);

		//HV_FAIL
		ResetItemI(ini_section_name, 
			HV_FAIL, 
			HV_FAIL_DEFAULT, 
			IDC_DSPECMODPARAMS_HV_FAIL_SPIN,
			HV_FAIL_SPIN_MIN, 
			HV_FAIL_SPIN_MAX, 
			IDC_DSPECMODPARAMS_HV_FAIL);

		//HV_HIGH_ERROR_LIMIT
		ResetItemI(ini_section_name, 
			HVVOLTH, 
			HVVOLTH_DEFAULT, 
			IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT_SPIN,
			HV_HIGH_ERRORLIMIT_SPIN_MIN, 
			HV_HIGH_ERRORLIMIT_SPIN_MAX, 
			IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT);

		//HV_LOW_ERROR_LIMIT
		ResetItemI(ini_section_name, 
			HVVOLTL, 
			HVVOLTL_DEFAULT, 
			IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT_SPIN,
			HV_LOW_ERRORLIMIT_SPIN_MIN, 
			HV_LOW_ERRORLIMIT_SPIN_MAX, 
			IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT);

		//LONGDWELL
		ResetItemI(ini_section_name, 
			LONGDWELL_SEC, 
			LONGDWELL_DEFAULT, 
			IDC_DSPECMODPARAMS_LONGDWELL_SPIN,
			LONGDWELL_SPIN_MIN, 
			LONGDWELL_SPIN_MAX, 
			IDC_DSPECMODPARAMS_LONGDWELL);

		//MAXCYCLE - constant at 1 second for the DSPEC

		//MAXPAUSE
		ResetItemI(ini_section_name, 
			MAXPAUSE, 
			MAXPAUSE_DEFAULT, 
			IDC_DSPECMODPARAMS_MAXPAUSE_SPIN,
			MAXPAUSE_SPIN_MIN, 
			MAXPAUSE_SPIN_MAX, 
			IDC_DSPECMODPARAMS_MAXPAUSE);

		//MAXSTATUS
		ResetItemI(ini_section_name, 
			MAXSTATUS, 
			MAXSTATUS_DEFAULT, 
			IDC_DSPECMODPARAMS_MAXSTATUS_SPIN,
			MAXSTATUS_SPIN_MIN, 
			MAXSTATUS_SPIN_MAX, 
			IDC_DSPECMODPARAMS_MAXSTATUS);

		//PORT
		// Constant after the creation of the ISO.

		//QUIET
		sscanf(QUIET_DEFAULT,"%d",&iDefault);
		ResetItem(ini_section_name,
			QUIET,
			iDefault, 
			IDC_DSPECMODPARAMS_QUIET_CHECKBOX);

		//REALTIMEDWELL
		ResetItemI(ini_section_name, 
			REALTIMEDWELL_SEC,
			REALTIMEDWELL_DEFAULT,
			IDC_DSPECMODPARAMS_REALTIMEDWELL_SPIN, 
			REALTIMEDWELL_SPIN_MIN,
			REALTIMEDWELL_SPIN_MAX, 
			IDC_DSPECMODPARAMS_REALTIMEDWELL);

		//SAVE_LOC
		ResetItem(ini_section_name,
			SAVE_LOC, 
			SAVELOC_DEFAULT, 
			IDC_DSPECMODPARAMS_SAVELOC);

		//SHORTDWELL
		ResetItemI(ini_section_name, 
			SHORTDWELL_SEC, 
			SHORTDWELL_DEFAULT,
			IDC_DSPECMODPARAMS_SHORTDWELL_SPIN, 
			SHORTDWELL_SPIN_MIN,
			SHORTDWELL_SPIN_MAX, 
			IDC_DSPECMODPARAMS_SHORTDWELL);

		//SLOWRESET
		ResetItemI(ini_section_name, 
			SLOW_RESET, 
			SLOW_RESET_DEFAULT,
			IDC_DSPECMODPARAMS_SLOW_RESET_SPIN, 
			SLOWRESET_SPIN_MIN,
			SLOWRESET_SPIN_MAX, 
			IDC_DSPECMODPARAMS_SLOW_RESET);

		//THRESHOLD
		ResetItemI(ini_section_name, 
			THRESHOLD, 
			THRESHOLD_DEFAULT,
			IDC_DSPECMODPARAMS_THRESHOLD_SPIN, 
			THRESHOLD_SPIN_MIN, 
			THRESHOLD_SPIN_MAX,
			IDC_DSPECMODPARAMS_THRESHOLD);

		//TIMEOUT
		ResetItemI(ini_section_name, 
			TIMEOUT, 
			TIMEOUT_DEFAULT,
			IDC_DSPECMODPARAMS_TIMEOUT_SPIN, 
			TIMEOUT_SPIN_MIN, 
			TIMEOUT_SPIN_MAX,
			IDC_DSPECMODPARAMS_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////////
//
//                      DoSpinI
//                      DoBuddySpin
//                      DoSpin
//                      DoSpinID
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::DoSpinI(NMHDR* pNMHDR, 
							 LRESULT* pResult, 
							 int MIN, int MAX, 
							 int DLGITEM)
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	char temp[16];								
	GetDlgItem(DLGITEM)->GetWindowText(temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
		iNew = MAX; 

	if (iNew < MIN) 
		iNew = MIN;

	sprintf(temp,"%d",iNew);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CDSPECModParam::DoBuddySpinI(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
								int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{
	if (m_pPS)m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	char temp[16];								
	GetDlgItemText(DLGITEM,temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > MAX)
		iNew = MAX; 

	if (iNew < MIN) 
		iNew = MIN;
	
	sprintf(temp,"%d",iNew);
	SetDlgItemText(DLGITEM,temp);

	char temp2[16];
	int buddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
	sscanf(temp2,"%d",&buddyvalue);
	if (Above)
	{
		if (iNew > buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (iNew < buddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}
void CDSPECModParam::DoSpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];								
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);
	*pResult = 0;
}

void CDSPECModParam::DoBuddySpin(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, 
							int DLGITEM, int DLGITEMBUDDY, int SPINNER, bool Above)
{

	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (iNew > MAX)
	{
		iNew = MAX; 
		pNMUpDown->iDelta = MAX-pNMUpDown->iPos;
	}
	if (iNew < MIN) 
	{
		iNew = MIN;
		pNMUpDown->iDelta = MIN-pNMUpDown->iPos;
	}
	
	char temp[8];
	char temp2[8];
	float fvalue = (float)iNew/(float)10.0;
	sprintf(temp,"%1.1f",(float)iNew/10.0);
	GetDlgItem(DLGITEM)->SetWindowText(temp);

	float fbuddyvalue;
	GetDlgItemText(DLGITEMBUDDY,temp2,sizeof(temp2));
	sscanf(temp2,"%f",&fbuddyvalue);
	if (Above)
	{
		if (fvalue > fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY, temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}
	else
	{
		if (fvalue < fbuddyvalue)
		{
			SetDlgItemText(DLGITEMBUDDY,temp);
			((CSpinButtonCtrl*)GetDlgItem(SPINNER))->SetPos(iNew);
		}
	}

	*pResult = 0;
}

void CDSPECModParam::DoSpinID(NMHDR* pNMHDR, LRESULT* pResult, int MIN, int MAX, int DLGITEM)
{

	if (m_pPS) 
		m_pPS->SetToClose(0);

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNew = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (iNew > MAX)
	{
		iNew = MAX;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MAX;
	}
	if (iNew < MIN)
	{
		iNew = MIN;
		pNMUpDown->iDelta = 0;
		pNMUpDown->iPos = MIN;
	}
	char temp[3];
	_itoa(iNew,temp,10);
	while (strlen(temp) < 2)
	{
		temp[2] = NULL;
		temp[1] = temp[0];
		temp[0] = '0';
	}

	SetDlgItemText(DLGITEM,temp);

	CString Text;
	GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,Text);

//	if "DSPEC" at end of name
	Text.MakeReverse();
	switch (Text.Find("CEPSD"))
	{
	case 0:
		Text.MakeReverse();
		break;
	case 1:	
		if (
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-1);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "DSPEC";
			else
				Text += "\\DSPEC";
		}
		break;
	case 2:
		if (
			(
			((Text[0] >= '0') && (Text[0] <= '9')) ||
			((Text[0] >= 'A') && (Text[0] <= 'Z')) ||
			((Text[0] >= 'a') && (Text[0] <= 'z')) 
			) && (
			((Text[1] >= '0') && (Text[1] <= '9')) ||
			((Text[1] >= 'A') && (Text[1] <= 'Z')) ||
			((Text[1] >= 'a') && (Text[1] <= 'z')) 
			)
		   )
		{
			Text.MakeReverse();
			Text = Text.Left(Text.GetLength()-2);
		}
		else
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "DSPEC";
			else
				Text += "\\DSPEC";
		}
		break;
	default:
		{
			Text.MakeReverse();
			if (Text.Right(1) == "\\")
				Text += "DSPEC";
			else
				Text += "\\DSPEC";
		}
	}

	*pResult = 0;
	Text += temp;
	SetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,Text);
}


void CDSPECModParam::OnModparamsBrowse() 
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	CDirDialog DirDialog(this);// = CDirDialog();
	char temp[_MAX_DIR+1];
	GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,temp,sizeof(temp)-1);
	DirDialog.m_strTitle = "Select Directory to save files in.";
	if (DirDialog.DoBrowse())
	{
		CString Id;
		GetDlgItemText(IDC_DSPECMODPARAMS_FILEID,Id);
		Id.TrimLeft();
		CString Text(DirDialog.m_strPath);
		CString Name("DSPEC"+Id);
		if (Text.Find(Name,0)==-1)
		{
			if (Text.Right(1) == '\\')
				Text += "DSPEC";
			else
			{
				Text += "\\";
				Text += "DSPEC";
			}
			Text += Id;
		}
		SetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC, Text);
	}
}




void CDSPECModParam::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if (m_pPS)
		m_pPS->SetToClose(0);
}

void CDSPECModParam::OnDeltaposModparamsMostSpin(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
//	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(TRUE);
	}
	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////////
//
//                      COMMFAIL
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsCommfailSpin(NMHDR* pNMHDR, 
													 LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		COMMFAIL_SPIN_MIN, 
		COMMFAIL_SPIN_MAX, 
		IDC_DSPECMODPARAMS_COMMFAIL);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      DODUMP
//
//////////////////////////////////////////////////////////////////////////////
//
// Uses only OnChangeModParamsMost()
//
//////////////////////////////////////////////////////////////////////////////
//
//                      FILEID
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsFileidSpin(NMHDR* pNMHDR, 
												   LRESULT* pResult) 
{
	DoSpinID(pNMHDR, pResult, FILEID_SPIN_MIN, FILEID_SPIN_MAX, 
		IDC_DSPECMODPARAMS_FILEID);
}

void CDSPECModParam::OnChangeModparamsFileid()
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(true);
	}
}

void CDSPECModParam::OnKillfocusModparamsFileid() 
{
	char temp[3];
	GetDlgItemText(IDC_DSPECMODPARAMS_FILEID,temp,sizeof(temp));
	while (strlen(temp) < 2)
	{
		temp[2] = temp [1];
		temp[1] = temp [0];
		temp[0] = '0';
	}
	int value = atoi(temp);
	((CSpinButtonCtrl*)GetDlgItem(IDC_DSPECMODPARAMS_FILEID_SPIN))->SetPos(value);
	((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_FILEID))->SetWindowText(temp);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      HV_FAIL
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsHVfailSpin(
	NMHDR* pNMHDR, 
	LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		HV_FAIL_SPIN_MIN, 
		HV_FAIL_SPIN_MAX, 
		IDC_DSPECMODPARAMS_HV_FAIL);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      HV_ERROR_LIMITS_HIGH
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsHVhigherrorlimitSpin(
	NMHDR* pNMHDR, 
	LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CWnd *pwdhigh = GetDlgItem(IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT);
	CWnd *pwdlow = GetDlgItem(IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT);

	char temp[16];								
	pwdhigh->GetWindowText(temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > HV_HIGH_ERRORLIMIT_SPIN_MAX)
		iNew = HV_HIGH_ERRORLIMIT_SPIN_MAX; 

	if (iNew < HV_HIGH_ERRORLIMIT_SPIN_MIN) 
		iNew = HV_HIGH_ERRORLIMIT_SPIN_MIN;

	pwdlow->GetWindowText(temp,sizeof(temp)-1);
	int iLow = atoi(temp);

	sprintf(temp,"%d",iNew);
	pwdhigh->SetWindowText(temp);

	if (iNew < iLow)
	{
		pwdlow->SetWindowText(temp);
	}


	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////////
//
//                      HV_ERROR_LIMITS_LOW
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsHVlowerrorlimitSpin(
	NMHDR* pNMHDR, 
	LRESULT* pResult)
{
	if (m_pPS) m_pPS->SetToClose(0);
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	CWnd *pwdhigh = GetDlgItem(IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT);
	CWnd *pwdlow = GetDlgItem(IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT);

	char temp[16];								
	pwdlow->GetWindowText(temp,sizeof(temp)-1);

	int iNew = atoi(temp) + pNMUpDown->iDelta;

	if (iNew > HV_LOW_ERRORLIMIT_SPIN_MAX)
		iNew = HV_LOW_ERRORLIMIT_SPIN_MAX; 

	if (iNew < HV_LOW_ERRORLIMIT_SPIN_MIN) 
		iNew = HV_LOW_ERRORLIMIT_SPIN_MIN;

	pwdhigh->GetWindowText(temp,sizeof(temp)-1);
	int iHigh = atoi(temp);

	sprintf(temp,"%d",iNew);
	pwdlow->SetWindowText(temp);

	if (iNew > iHigh)
		pwdhigh->SetWindowText(temp);

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////////////
//
//                      LONGDWELL
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsLongdwellSpin(NMHDR* pNMHDR, 
													  LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		LONGDWELL_SPIN_MIN, 
		LONGDWELL_SPIN_MAX, 
		IDC_DSPECMODPARAMS_LONGDWELL);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      MAXPAUSE
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsMaxpauseSpin(NMHDR* pNMHDR, 
													 LRESULT* pResult) 
{
	DoSpinI(pNMHDR, 
		pResult, 
		MAXPAUSE_SPIN_MIN, 
		MAXPAUSE_SPIN_MAX, 
		IDC_DSPECMODPARAMS_MAXPAUSE);
}

//////////////////////////////////////////////////////////////////////////////
//
//                     MAXSTATUS
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsMaxstatusSpin(NMHDR* pNMHDR, 
													 LRESULT* pResult) 
{
	DoSpinI(pNMHDR, 
		pResult, 
		MAXSTATUS_SPIN_MIN, 
		MAXSTATUS_SPIN_MAX, 
		IDC_DSPECMODPARAMS_MAXSTATUS);
}

//////////////////////////////////////////////////////////////////////////////
//
//                      NODE
//
//////////////////////////////////////////////////////////////////////////////
//
// Uses OnDeltaposModparamsMostSpin()
//
//////////////////////////////////////////////////////////////////////////////
//
//                      PORT
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnChangeModparamsPort()
{
	if(m_pPS)m_pPS->SetToClose(0);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      REALTIMEDWELL
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsRealtimedwellSpin(NMHDR* pNMHDR, 
														  LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		REALTIMEDWELL_SPIN_MIN, 
		REALTIMEDWELL_SPIN_MAX, 
		IDC_DSPECMODPARAMS_REALTIMEDWELL);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      SAVELOC
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnChangeModparamsSaveloc() 
{
	CString Text;
	GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,Text);
	if (!Text.IsEmpty())
	{
		int posStart, posEnd;
		((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_SAVELOC))->GetSel(posStart,posEnd);
		if (Text.FindOneOf(INVALIDCHARSSUB) != -1)
		{
			((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_SAVELOC))->Undo();
			posStart--;
			posEnd--;
		}
		((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_SAVELOC))->SetModify(FALSE);
		((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_SAVELOC))->EmptyUndoBuffer();
		((CEdit*)GetDlgItem(IDC_DSPECMODPARAMS_SAVELOC))->SetSel(posStart,posEnd);
	}
	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(true);
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                      SHORTDWELL
//
//////////////////////////////////////////////////////////////////////////////

void CDSPECModParam::OnDeltaposModparamsShortdwellSpin(NMHDR* pNMHDR, 
													   LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		SHORTDWELL_SPIN_MIN, 
		SHORTDWELL_SPIN_MAX, 
		IDC_DSPECMODPARAMS_SHORTDWELL);
}

//////////////////////////////////////////////////////////////////////////////
//
//                      SLOW_RESET
//
//////////////////////////////////////////////////////////////////////////////

void CDSPECModParam::OnDeltaposModparamsSlowresetSpin(NMHDR* pNMHDR, 
													   LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		SLOWRESET_SPIN_MIN, 
		SLOWRESET_SPIN_MAX, 
		IDC_DSPECMODPARAMS_SLOW_RESET);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      THRESHOLD
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsThresholdSpin(NMHDR* pNMHDR, 
													  LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		THRESHOLD_SPIN_MIN, 
		THRESHOLD_SPIN_MAX, 
		IDC_DSPECMODPARAMS_THRESHOLD);
}
//////////////////////////////////////////////////////////////////////////////
//
//                      TIMEOUt
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnDeltaposModparamsTimeoutSpin(NMHDR* pNMHDR, 
													  LRESULT* pResult)
{
	DoSpinI(pNMHDR, 
		pResult, 
		TIMEOUT_SPIN_MIN, 
		TIMEOUT_SPIN_MAX, 
		IDC_DSPECMODPARAMS_TIMEOUT);
}
//////////////////////////////////////////////////////////////////////////////
//
//                  OnChangeModparamsMost
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::OnChangeModparamsMost()
{
	if (m_pPS) 
		m_pPS->SetToClose(0);

	if (!m_bCollectingParametersForNewISO && m_datasetup)
	{
		m_bChange = true;
		m_pApplyButton->EnableWindow(true);
	}
}
//////////////////////////////////////////////////////////////////////////////
//
//                   OnWizardFinish
//
//////////////////////////////////////////////////////////////////////////////
BOOL CDSPECModParam::OnWizardFinish() 
{
//	ASSERT(m_bCollectingParametersForNewISO);

	//Put the name of the instrument in the ini file
	WritePrivateProfileString(
		INSTRUMENTS,m_pParent->m_pName,DSPEC_DISCOVERY_TYPE,m_pParent->m_szIniFile);
	//Put the parameters for the instrument into the INI file
	DoApply();  //This does the validate

	return CPropertyPage::OnWizardFinish();
}
//////////////////////////////////////////////////////////////////////////////
//
//                   OnInitDialog
//
//////////////////////////////////////////////////////////////////////////////
BOOL CDSPECModParam::OnInitDialog() 
{
//	TRACE("CDSPECModParam::OnInitDialog()\n");

	CPropertyPage::OnInitDialog();
	
	m_pApplyButton = new CYellowButton(
		"Apply",
		IDC_DSPECMODPARAMS_APPLY, 
		m_pParent->m_szIniFile,
		this);

	//disable the "NODE" editbox and hide the spinbutton
	GetDlgItem(IDC_DSPECMODPARAMS_NODE)->EnableWindow(false);
	GetDlgItem(IDC_DSPECMODPARAMS_NODE_SPIN)->ShowWindow(SW_HIDE);

//	CString *csversion = m_pParent->GetVersionString();
//	CString cs = "Version " + *csversion + " " + __DATE__;
//	CString cs = "Version " + m_pParent->m_csVersion + " Build Date:" + __DATE__;

//	SetDlgItemText(IDC_STATIC_ADVERT, cs);
/*
#ifdef _DEBUG
	SetDlgItemText(IDC_STATIC_DEBUG, "Debug Build");
#else
	SetDlgItemText(IDC_STATIC_DEBUG, "Release Build");
#endif
*/
	
	return TRUE;
}



//////////////////////////////////////////////////////////////////////////////
//
//                   IssuePrint
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECModParam::IssuePrint(FILE *outfile)
{
//	TRACE("CDSPECModParam::IssuePrint()\n");

	//fprintf(outfile,"*******************Begin CDSPECModParam::IssuePrint()\n");

	CString cs, cs1;
	fprintf(outfile,"\nMODIFY PARAMETERS -- Caution: These values may not have been applied!\n");

	fprintf(outfile," Communications...\n");

	GetDlgItemText(IDC_DSPECMODPARAMS_PORT,cs);
	fprintf(outfile,"%+27s: %s\n","Port",cs.GetString());

	fprintf(outfile," File Output...\n");

	GetDlgItemText(IDC_DSPECMODPARAMS_SAVELOC,cs);
	fprintf(outfile,"%+27s: %s\n","Location",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_FILEID,cs);
	fprintf(outfile,"%+27s: %s\n","Station ID",cs.GetString());

	if(((CButton*)GetDlgItem(IDC_DSPECMODPARAMS_QUIET_CHECKBOX))->GetCheck()==0)
		cs = NO;
	else
		cs = YES;
	fprintf(outfile,"%+35s: %s\n","Quiet Realtime Data Text Displays",cs.GetString());

	if(((CButton*)GetDlgItem(IDC_DSPECMODPARAMS_DODUMP))->GetCheck()==0)
		cs = NO;
	else
		cs = YES;
	fprintf(outfile,"%+27s: %s\n","Do Dump File",cs.GetString());

	fprintf(outfile," General...\n");

	GetDlgItemText(IDC_DSPECMODPARAMS_MAXPAUSE,cs);
	fprintf(outfile,"%+27s: %s\n","Maximum Pause Time (Sec)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_THRESHOLD, cs);
	fprintf(outfile,"%+27s: %s\n","Threshold(Counts)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_SHORTDWELL, cs);
	fprintf(outfile,"%+27s: %s\n","Short Dwell(Sec)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_LONGDWELL, cs);
	fprintf(outfile,"%+27s: %s\n","Long Dwell(Sec)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_REALTIMEDWELL, cs);
	fprintf(outfile,"%+27s: %s\n","Realtime Dwell(Sec)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_COMMFAIL, cs);
	fprintf(outfile,"%+27s: %s\n","# of Timeouts Before Fail",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_TIMEOUT, cs);
	fprintf(outfile,"%+27s: %s\n","Timeout (Sec)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_MAXSTATUS, cs);
	fprintf(outfile,"%+27s: %s\n","Take Status Every(Min)",cs.GetString());

	fprintf(outfile," Error Limits...\n");

	GetDlgItemText(IDC_DSPECMODPARAMS_HV_HIGH_ERRORLIMIT, cs);
	fprintf(outfile,"%+27s: %s\n","HV High Error Limit (v)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_HV_LOW_ERRORLIMIT, cs);
	fprintf(outfile,"%+27s: %s\n","HV Low Error Limit (v)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_HV_FAIL, cs);
	fprintf(outfile,"%+27s: %s\n","HV Fail (v)",cs.GetString());

	GetDlgItemText(IDC_DSPECMODPARAMS_SLOW_RESET, cs);
	fprintf(outfile,"%+27s: %s\n","Slow Reset (Sec)",cs.GetString());

	fprintf(outfile,"\n");

	//fprintf(outfile,"****************End CDSPECModParam::IssuePrint()\n");
}


