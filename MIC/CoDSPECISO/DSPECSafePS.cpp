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
// DSPECSafePS.cpp
 

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "DSPECSafePS.h"
#include "DSPECInst.h"
#include "DSPECStrings.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_CLOSE_BUTTON 9000
#define ID_PAUSE_BUTTON 9001
#define ID_PRINT_BUTTON 9002
#define ID_ACKFL_BUTTON	9003

#define IDC_DSPEC_SPECTRUM_DATA_PROCESSED 21115
#define IDC_COMMAND_RESPONSE_PAIR         20001

static UINT uIDC_RECEIVE;  //registered message value

#define STUB

/////////////////////////////////////////////////////////////////////////////
// CDSPECSafePropertySheet

IMPLEMENT_DYNAMIC(CDSPECSafePropertySheet, CPropertySheet)

CDSPECSafePropertySheet::CDSPECSafePropertySheet(
	UINT nIDCaption, 
	CDSPECInst* pParentWnd, 
	char *inifile)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CDSPECSafePropertySheet::CTOR #1\n");
	m_pCaption[0] = NULL;
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishInstance();
}

CDSPECSafePropertySheet::CDSPECSafePropertySheet(
	LPCTSTR pszCaption, 
	CDSPECInst* pParentWnd, 
	char *inifile)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
//	TRACE("CDSPECSafePropertySheet::CTR b\n");
	strcpy(m_pCaption,pszCaption);
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishInstance();
}

void CDSPECSafePropertySheet::FinishInstance()
{
	//TRACE("CDSPECSafePropertySheet::FinishInstance()\n");

	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;  //for the message map

	Data_Tab      = 0;  //data status
	InstSetup_Tab = 0;  //Instrument settings

	m_dToClose				= 0;

	m_szCurrentFileName[0]	= NULL;

	m_szCurrentCHNFileName[0]        = 0;
	m_uLocalNonzero_channels         = 0;
	m_uLocalChannels_above_threshold = 0;
	Data_Tab = 0;
	InstSetup_Tab = 0;

	memset(&m_ReportedParameters,0,sizeof(m_ReportedParameters));

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;
}

COLORREF CDSPECSafePropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

	return m_pCloseButton->SetFaceColor(color);
}

CDSPECSafePropertySheet::~CDSPECSafePropertySheet()
{
//	TRACE("CDSPECSafePropertySheet::DTR\n");

	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;
}

BEGIN_MESSAGE_MAP(CDSPECSafePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CDSPECSafePropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,			OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,			OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,			OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,			OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,				OnToCloseButton)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))		OnReceive)
	ON_MESSAGE(IDC_COMMAND_RESPONSE_PAIR,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))	OnReceiveCommandResponsePair)
	ON_MESSAGE(IDC_IPX_MCB_ERROR,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))			OnIPXMCBError)
	ON_MESSAGE(IDC_EXTENDCLOSE,(LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))				OnExtendClose)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
//
//                    OnIPXMCBError
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::OnIPXMCBError(WPARAM /*WParam*/, LPARAM pmsg)
{
//	TRACE("CDSPECSafePropertySheet::OnIPXMCBError(%s)\n", (char *)pmsg);

	CString cs;
	cs.Format("63380 DSPEC IPX MCB Error: command = %s\n", pmsg);
	WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, cs);

	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cs);

	//delete [] (void *)pmsg;
	CoTaskMemFree((void *)pmsg);
}
//////////////////////////////////////////////////////////////////////////////
//
//                    SetLimits
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::SetLimits(int hvvolth, int hvvoltl)
{
	m_dHVVoltH=hvvolth;
	m_dHVVoltL=hvvoltl;
}
//////////////////////////////////////////////////////////////////////////////
//
//                    UpdateSummary
//
//////////////////////////////////////////////////////////////////////////////

void CDSPECSafePropertySheet::UpdateSummary(char *pchMessageInput)
{
	CDSPECSummary*  summarypage  = m_pParent->m_pSummaryPage;
	char ifmt[] = "%d";
	CString cs;

	if (pchMessageInput != NULL)
	{
		//
		// update Recent Commands and Responses panel on summary page
		//
		summarypage->GetDlgItemText(IDC_DSPECSUMMARY_COMMAND5, cs);
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND6, cs);

		summarypage->GetDlgItemText(IDC_DSPECSUMMARY_COMMAND4, cs);
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND5, cs);

		summarypage->GetDlgItemText(IDC_DSPECSUMMARY_COMMAND3, cs);
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND4, cs);

		summarypage->GetDlgItemText(IDC_DSPECSUMMARY_COMMAND2, cs);
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND3, cs);

		summarypage->GetDlgItemText(IDC_DSPECSUMMARY_COMMAND1, cs);
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND2, cs);

		cs = pchMessageInput;
		//CTime t = CTime::GetCurrentTime();
		//CString s = t.Format( " %Y.%m.%d %H.%M.%S" );
		//cs += s;

		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_COMMAND1,cs);
	}
	//
	// update data status pane of summary page
	//
	//non-zero channels
	cs.Format(ifmt,m_uLocalNonzero_channels);
	summarypage->SetDlgItemText(IDC_DSPECSUMMARY_NONZERO_CHANS, cs);

	//channels above threshold
	cs.Format(ifmt,m_uLocalChannels_above_threshold);
	summarypage->SetDlgItemText(IDC_DSPECSUMMARY_ABOVE_THRESHOLD, cs);

	//dwell type
	if (!m_pParent->DwellSettingIsShort())
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_DWELLTYPE,LONG_STRING);
	else
		summarypage->SetDlgItemText(IDC_DSPECSUMMARY_DWELLTYPE,SHORT_STRING);

	//seconds until end of dwell
	cs.Format(ifmt, m_pParent->m_iSecToGo);
	summarypage->SetDlgItemText(IDC_DSPECSUMMARY_SECS_TO_END_DWELL, cs);

	//Latest CHN file written
	summarypage->SetDlgItemText(IDC_DSPECSUMMARY_LAST_CHN_FILE, m_szCurrentCHNFileName);
	//
	// update DSPEC Setup pane of summary page
	//
	summarypage->SetDlgItemText(
		IDC_DSPECSUMMARY_UNIT_ID,
		m_ReportedParameters.network_id);

	summarypage->SetDlgItemText(
		IDC_DSPECSUMMARY_CURRENT_HV, 
		m_ReportedParameters.hv_volts);

	summarypage->SetDlgItemText(
		IDC_DSPECSUMMARY_HV_OVERLOAD,
		m_ReportedParameters.hv_overload);

	summarypage->SetDlgItemText(
		IDC_DSPECSUMMARY_SHUT_ACTUAL,
		m_ReportedParameters.shut_actual);
}
//////////////////////////////////////////////////////////////////////////////
//
//                            UpdateDataStatus
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::UpdateDataStatus()
{
	CString cs;
	char ifmt[] = "%d";
	CDSPECDataStat* datastatpage = m_pParent->m_pDataStatus;

	//non-zero channels
	cs.Format(ifmt,m_uLocalNonzero_channels);
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_NON_ZERO_CHANS, cs);

	//channels above threshold
	cs.Format(ifmt,m_uLocalChannels_above_threshold);
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_ABV_THRESHOLD, cs);

	//dwell type
	if (m_pParent->DwellSettingIsShort())
		datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_DWELL_TYPE,SHORT_STRING);
	else
		datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_DWELL_TYPE,LONG_STRING);

	//seconds until end of dwell
	cs.Format(ifmt, m_pParent->m_iSecToGo);
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_SECS_TIL_END, cs);

	//update close button TTG value (Time To Go for this dwell period)
	m_pCloseButton->SetNumeric(m_pParent->m_iSecToGo, true);

	//current high voltage
	cs = m_ReportedParameters.hv_volts;
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_CURRENT_HV,cs);

	//current hv overloaded status
	cs = m_ReportedParameters.hv_overload;
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_HV_OVERLOAD, cs);

	//shutdown status
	cs = m_ReportedParameters.shut_actual;
	datastatpage->SetDlgItemText(IDC_DSPECDATASTAT_SHUTDOWN, cs);
}
//////////////////////////////////////////////////////////////////////////////
//
//                          OnReceiveCommandResponsePair
//
//         The "cracker" for status data coming from the DSPEC via the CSO
//
///////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::OnReceiveCommandResponsePair(WPARAM wparam, LPARAM lparam)
{
	//TRACE("CDSPECSafePropertySheet::OnReceiveCommandResponsePair()\n");

	const int  MAX_IN    = 64;
	const char SEPARATOR = '.';

	char command[MAX_IN];
	char response[MAX_IN];

	strncpy(command,(char *)wparam,MAX_IN - 1);
	strncpy(response,(char *)lparam,MAX_IN - 1);

	//delete [] (void *)wparam;
	//delete [] (void *)lparam;
	CoTaskMemFree((void *)wparam);
	CoTaskMemFree((void *)lparam);

	int commandlen = strlen(command);

	CDSPECSummary* summarypage  = m_pParent->m_pSummaryPage;
	CDSPECDataStat* datastatpage = m_pParent->m_pDataStatus;

	bool bMatch = true;

	m_pParent->AcknowledgeInputFromCSO();

	char *to, *from;
	char ini_value[64];

	const int storagesize = DPLEN;

	char substring[DPLEN];
	memset(substring,0,sizeof(substring));

	//make incoming command all caps for comparisons below
	for (int i = 0; i < commandlen; i++)
		command[i] = (char)toupper(command[i]);

	//TRACE("Command-Response Pair = %s, %s\n", command, response);
	UpdateSummary(command);
	UpdateSummary(response);

///////////////////////////////////////////////////////////////////////
//   THESE ARE IN ALPHABETICAL ORDER  (except for the first one)     //
///////////////////////////////////////////////////////////////////////

	if (0 == strcmp(command,"SHOW_ACTIVE_DEVICES"))  //recovery msg from CSO
		strncpy(m_ReportedParameters.active_devices, response, storagesize);

	else if (0 == strcmp(command,LIST_CORRECTION_FLAT))
		strncpy(m_ReportedParameters.correction_flat_list, response, storagesize);

	else if (0 == strcmp(command,LIST_GAIN_COARSE))
		strncpy(m_ReportedParameters.gain_coarse_list, response, storagesize);

	else if (0 == strcmp(command,LIST_GAIN_CONV))
		strncpy(m_ReportedParameters.gain_conversion_list, response, storagesize);

	else if (0 == strcmp(command,LIST_GAIN_FINE))
		strncpy(m_ReportedParameters.gain_fine_list, response, storagesize);

	else if (0 == strcmp(command,LIST_HV))
		strncpy(m_ReportedParameters.hv_list, response, storagesize);

	else if (0 == strcmp(command,LIST_SHAP_CUSP))
		strncpy(m_ReportedParameters.shap_cusp_list, response, storagesize);

	else if (0 == strcmp(command,LIST_SHAP_FLAT))
		strncpy(m_ReportedParameters.shap_flat_list, response, storagesize);

	else if (0 == strcmp(command,LIST_SHAP_RISE))
		strncpy(m_ReportedParameters.shap_rise_list, response, storagesize);

	else if (0 == strcmp(command,LIST_SHUTDOWN))
		strncpy(m_ReportedParameters.shutdown_list, response, storagesize);

	else if (0 == strcmp(command,SHOW_BLRE_AUTOMATIC))
		{
			if (0 == strcmp(DOLLARIT,response))
				strcpy(m_ReportedParameters.blre_automatic_enable,ENABLE);
			else if (0 == strcmp(DOLLARIF,response))
				strcpy(m_ReportedParameters.blre_automatic_enable,DISABLE);
			else
				strcpy(m_ReportedParameters.blre_automatic_enable,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_BLRE_AUTOMATIC_ENABLE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.blre_automatic_enable,ini_value))
			{
				WriteSetupFailureToFile(65410, SET_BLRE_AUTOMATIC_ENABLE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_BLRE_AUTOMATIC_ENABLE, 
				m_ReportedParameters.blre_automatic_enable, bMatch);
		}
	else if (0 == strcmp(command,SHOW_CORRECTION_FLAT))
		{
			CleanUpString(m_ReportedParameters.correction_flat,&response[10],storagesize,5);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_CORRECTION_FLAT,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.correction_flat,ini_value))
			{
				WriteSetupFailureToFile(65415, SET_CORRECTION_FLAT);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_CORRECTION_FLAT, m_ReportedParameters.correction_flat, bMatch);
		}

	else if (0 == strcmp(command,SHOW_DATE))
		{
			to   = m_ReportedParameters.date;
			from = &response[3];

			*to++ = *from++; //dd
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //mm
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //yy
			*to++ = *from;
			*to = 0;
		}
	else if (0 == strcmp(command,SHOW_DATE_START))
		{
			to   = m_ReportedParameters.date_start;
			from = &response[3];

			*to++ = *from++; //dd
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //mm
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //yy
			*to++ = *from;
			*to = 0;
		}
	else if (0 == strcmp(command,SHOW_DEVICE))
		{
			memcpy(m_ReportedParameters.device, &response[2], 3);
			m_ReportedParameters.device[3] = 0;
		}
	else if (0 == strcmp(command,SHOW_GAIN_ADJUSTMENT))
		{
/*
			//GAIN_ADJ 00000nnn
			//GAIN_ADJ -00000nnn
			CleanUpString(m_ReportedParameters.gain_adjustment,&response[9],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_ADJUSTMENT,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_adjustment,ini_value))
			{
				//generalpage->SetColor(IDC_GENERAL_MODE,COLOR_YELLOW);
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, 
					"65400 SETUP FAILURE - SET_GAIN_ADJUSTMENT\n");
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_ADJUSTMENT, m_ReportedParameters.gain_adjustment, bMatch);
*/
		}
	else if (0 == strcmp(command,SHOW_GAIN_CHANNEL))
		{
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.gain_channel,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_CHANNEL,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_channel,ini_value))
			{
				WriteSetupFailureToFile(65420, SET_GAIN_CHANNEL);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_CHANNEL, m_ReportedParameters.gain_channel, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_COARSE))
		{
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.gain_coarse,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_COARSE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_coarse,ini_value))
			{
				WriteSetupFailureToFile(65425, SET_GAIN_COARSE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_COARSE, m_ReportedParameters.gain_coarse, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_CONVERSION))
		{
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.gain_conversion,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_CONVERSION,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_conversion,ini_value))
			{
				WriteSetupFailureToFile(65430, SET_GAIN_CONVERSION);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_CONVERSION, m_ReportedParameters.gain_conversion, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_FINE))
		{
			//GAIN_FINE 0000000000000.5
			//MIC_CB_028 to MIC 1.907 PJM 1/19/05 (captured change 
			//from three to four digits)
			// four digits after the decimal point
			CleanUpString(m_ReportedParameters.gain_fine,&response[10],storagesize, 4);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_FINE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_fine,ini_value))
			{
				WriteSetupFailureToFile(65435, SET_GAIN_FINE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_FINE, m_ReportedParameters.gain_fine, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_POLARITY))
		{
			if (0 == strcmp(DOLLARFNEG,response))
				strcpy(m_ReportedParameters.gain_polarity,NEGATIVE);
			else if (0 == strcmp(DOLLARFPOS,response))
				strcpy(m_ReportedParameters.gain_polarity,POSITIVE);
			else
				strcpy(m_ReportedParameters.gain_polarity,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_POLARITY,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_polarity,ini_value))
			{
				WriteSetupFailureToFile(65440, SET_GAIN_POLARITY);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_POLARITY, m_ReportedParameters.gain_polarity, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_STABILIZATION))
		{
			if (0 == strcmp(DOLLARIT,response))
				strcpy(m_ReportedParameters.gain_stabilization_enable,ENABLE);
			else if (0 == strcmp(DOLLARIF,response))
				strcpy(m_ReportedParameters.gain_stabilization_enable,DISABLE);
			else
				strcpy(m_ReportedParameters.gain_stabilization_enable,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_STABILIZATION_ENABLE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_stabilization_enable,ini_value))
			{
				WriteSetupFailureToFile(65445, SET_GAIN_STABILIZATION_ENABLE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_STABILIZATION_ENABLE, m_ReportedParameters.gain_stabilization_enable, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GAIN_WIDTH))
		{
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.gain_width,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_GAIN_WIDTH,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gain_width,ini_value))
			{
				WriteSetupFailureToFile(65450, SET_GAIN_WIDTH);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GAIN_WIDTH, m_ReportedParameters.gain_width, bMatch);
		}
	else if (0 == strcmp(command,SHOW_GATE))
		{
			//char DOLLARFANT[] = "$FANT";
			//char DOLLARFCOI[] = "$FCOI";
			//char DOLLARFOFF[] = "$FOFF";

			if (0 == strcmp(DOLLARFOFF,response))
				strcpy(m_ReportedParameters.gate,OFF);
			else if (0 == strcmp(DOLLARFCOI,response))
				strcpy(m_ReportedParameters.gate,COINCIDENT);
			else if (0 == strcmp(DOLLARFANT,response))
				strcpy(m_ReportedParameters.gate,ANTICOINCIDENT);
			else
				strcpy(m_ReportedParameters.gate,"ERROR");


			GetPrivateProfileString(m_pParent->m_pName,
				SET_GATE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.gate,ini_value))
			{
				WriteSetupFailureToFile(65455, SET_GATE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_GATE, m_ReportedParameters.gate, bMatch);
		}
	else if (0 == strcmp(command,SHOW_HV))
		{
			//save the whole string
			strncpy(m_ReportedParameters.hv, response,DPLEN);

			//
			// volts - must check for out of tolerance for "drift" and
			//         must check for catastrophically low
			//
			memcpy(substring,&response[2],5);
			substring[5] = 0;
			CleanUpString(m_ReportedParameters.hv_volts,substring,DPLEN);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_HV,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.hv_volts,ini_value))
			{
				WriteSetupFailureToFile(65460, SET_HV);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_HV, m_ReportedParameters.hv_volts, bMatch);

			int volts_in;
			sscanf(m_ReportedParameters.hv_volts,"%d",&volts_in);

			static bool b_prior_write_to_file_was_hvvolt_out_of_tolerance = false;
		
			if ((volts_in <= m_dHVVoltL) || (volts_in >= m_dHVVoltH))
			{
				// out of tolerance

				m_pParent->FlagOthr(1,true);
				summarypage->SetColor(IDC_DSPECSUMMARY_CURRENT_HV,COLOR_RED);
				datastatpage->SetColor(IDC_DSPECDATASTAT_CURRENT_HV,COLOR_RED);
				Data_Tab |= 0x8000;

				if (!b_prior_write_to_file_was_hvvolt_out_of_tolerance)
				{
						WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62640 STATUS CHANGE - HIGH VOLTAGE OUT OF DRIFT TOLERANCE\n");
				}

				b_prior_write_to_file_was_hvvolt_out_of_tolerance = true;
			}
			else	// within tolerance
			{
				if (b_prior_write_to_file_was_hvvolt_out_of_tolerance)
				{
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62650 STATUS CHANGE - HIGH VOLTAGE WITHIN DRIFT TOLERANCE\n");
				}
				b_prior_write_to_file_was_hvvolt_out_of_tolerance = false;
			}

			//determine if the volts_in value is catastrophically low - let parent know
			//either way - but only once

			static bool b_prior_voltage_level_ok = true;

			if ( volts_in < m_pParent->m_iHVFail )  //catastrophically low
			{
				if (b_prior_voltage_level_ok)               //not reported before
				{
//					TRACE("PostMessage(IDC_HV_VOLTAGE_LEVEL_FAIL)\n");

					WriteToFile(
						TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62660 STATUS CHANGE - FAILURE: HIGH VOLTAGE LEVEL TOO LOW\n");

					m_pParent->PostMessage(IDC_HV_VOLTAGE_LEVEL_FAIL,0,0);
					b_prior_voltage_level_ok = false;
					m_pParent->FlagOthr(1,true);
					summarypage->SetColor(IDC_DSPECSUMMARY_CURRENT_HV,COLOR_RED);
					datastatpage->SetColor(IDC_DSPECDATASTAT_CURRENT_HV,COLOR_RED);
					Data_Tab |= 0x8000;
				}
			}
			else  //not catastrophically low 
			{
				if(!b_prior_voltage_level_ok)  //transition from failure to non-failure
				{
//					TRACE("PostMessage(IDC_HV_VOLTAGE_LEVEL_RECOVER)\n");

					WriteToFile(
						TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62670 STATUS CHANGE - RECOVERY: HIGH VOLTAGE LEVEL OK\n");

					m_pParent->PostMessage(IDC_HV_VOLTAGE_LEVEL_RECOVER,0,0);
					b_prior_voltage_level_ok = true;
				}
			}

			//
			// end volts
			//

			//get status value
			memcpy(substring, &response[7],5); //"sssss" for status
			int status, value;
			sscanf(substring,"%d", &status);

			//overload
			value = status & 0x2; //bit 1 for overloaded
			if (value == 0)
			{
				strcpy(m_ReportedParameters.hv_overload, OVERLOAD);
				m_pParent->FlagOthr(1,true);
				summarypage->SetColor(IDC_DSPECSUMMARY_HV_OVERLOAD,COLOR_RED);
				datastatpage->SetColor(IDC_DSPECDATASTAT_HV_OVERLOAD,COLOR_RED);
				Data_Tab |= 0x8000;
			}
			else
			{
				strcpy(m_ReportedParameters.hv_overload, NORMAL);
			}

			//enabled
			value = status & 0x4; //bit 2 for enabled
			if (value == 0)
				strcpy(m_ReportedParameters.hv_enable, DISABLE);
			else
				strcpy(m_ReportedParameters.hv_enable, ENABLE);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_HV_ENABLE,"",ini_value,sizeof(ini_value),m_szIniFile);

			bMatch = true;
			if (0 != strcmp(m_ReportedParameters.hv_enable,ini_value))
			{
				WriteSetupFailureToFile(65465, SET_HV_ENABLE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_HV_ENABLE, m_ReportedParameters.hv_enable, bMatch);
		}

	else if (0 == strcmp(command,SHOW_HV_POLARITY))
		{
			if (0 == strcmp(DOLLARFNEG,response))
				strcpy(m_ReportedParameters.hv_polarity,NEGATIVE);
			else if (0 == strcmp(DOLLARFPOS,response))
				strcpy(m_ReportedParameters.hv_polarity,POSITIVE);
			else
				strcpy(m_ReportedParameters.hv_polarity,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_HV_POLARITY,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.hv_polarity,ini_value))
			{
				WriteSetupFailureToFile(65470, SET_HV_POLARITY);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_HV_POLARITY, m_ReportedParameters.hv_polarity, bMatch);
		}
	else if (0 == strcmp(command,SHOW_LIVE))  //don't care
		{
		}
	else if (0 == strcmp(command,SHOW_LIVE_PRESET))  //don't care
		{	
		}
	else if (0 == strcmp(command,SHOW_LLD))
		{
			//$Cnnnnnccc
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.lld,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_LLD,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.lld,ini_value))
			{
				WriteSetupFailureToFile(65475, SET_LLD);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_LLD, m_ReportedParameters.lld, bMatch);
		}
	else if (0 == strcmp(command,SHOW_MODE_STABILIZATION))
		{
			if (0 == strcmp(DOLLARFGAUSS,response))
				strcpy(m_ReportedParameters.stabilization_mode,GAUSS);
			else if (0 == strcmp(DOLLARFPOINT,response))
				strcpy(m_ReportedParameters.stabilization_mode,POINT);
			else
				strcpy(m_ReportedParameters.stabilization_mode,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_MODE_STABILIZATION,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.stabilization_mode,ini_value))
			{
				WriteSetupFailureToFile(65480, SET_MODE_STABILIZATION);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_MODE_STABILIZATION, 
				m_ReportedParameters.stabilization_mode, bMatch);
		}
	else if (0 == strcmp(command,SHOW_NETWORK_ADDRESS))
		{
			memcpy(m_ReportedParameters.network_address, &response[2],20);
			m_ReportedParameters.network_address[20] = 0;
		}
	else if (0 == strcmp(command,SHOW_NETWORK_ID))
		{		
			memcpy(m_ReportedParameters.network_id, &response[2],10);
			m_ReportedParameters.network_id[10] = 0;
		}

	else if (0 == strcmp(command,SHOW_PZ))
		{
			if (NULL != strstr(response,PZ_ENA))
				strcpy(m_ReportedParameters.pz_enable,ENABLE);
			else if (NULL != strstr(response,PZ_DIS))
				strcpy(m_ReportedParameters.pz_enable,DISABLE);
			else
				strcpy(m_ReportedParameters.pz_enable,"ERROR");

			CleanUpString(m_ReportedParameters.pz,&response[7],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_PZ,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.pz,ini_value))
			{
				WriteSetupFailureToFile(65485, SET_PZ);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_PZ, m_ReportedParameters.pz, bMatch);
		}
	else if (0 == strcmp(command,SHOW_PZ_AUTOMATIC))
		{
			if (0 == strcmp(DOLLARIT,response))
				strcpy(m_ReportedParameters.pz_automatic,ENABLE);
			else if (0 == strcmp(DOLLARIF,response))
				strcpy(m_ReportedParameters.pz_automatic,DISABLE);
			else
				strcpy(m_ReportedParameters.pz_automatic,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_PZ_AUTOMATIC_ENABLE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.pz_automatic,ini_value))
			{
				WriteSetupFailureToFile(65490, SET_PZ_AUTOMATIC_ENABLE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_PZ_AUTOMATIC_ENABLE, m_ReportedParameters.pz_automatic, bMatch);
		}
	else if (0 == strcmp(command,SHOW_SHAP_CUSP))
		{
			CleanUpString(m_ReportedParameters.shap_cusp,&response[10],storagesize,1);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_SHAP_CUSP,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.shap_cusp,ini_value))
			{
				WriteSetupFailureToFile(65495, SET_SHAP_CUSP);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}
			CurrentSetParam(SET_SHAP_CUSP, m_ReportedParameters.shap_cusp, bMatch);
		}
	else if (0 == strcmp(command,SHOW_SHAP_FLAT))
		{
			CleanUpString(m_ReportedParameters.shap_flat,&response[10],storagesize,1);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_SHAP_FLAT,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.shap_flat,ini_value))
			{
				WriteSetupFailureToFile(65500, SET_SHAP_FLAT);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}
			CurrentSetParam(SET_SHAP_FLAT, m_ReportedParameters.shap_flat, bMatch);
		}
	else if (0 == strcmp(command,SHOW_SHAP_RISE))
		{
			CleanUpString(m_ReportedParameters.shap_rise,&response[10],storagesize,1);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_SHAP_RISE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.shap_rise,ini_value))
			{
				WriteSetupFailureToFile(65505, SET_SHAP_RISE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}
			CurrentSetParam(SET_SHAP_RISE, m_ReportedParameters.shap_rise, bMatch);
		}
	else if (0 == strcmp(command,SHOW_SHUTDOWN))
		{
			//$F1ORT or $FTTL
			if (0 == strcmp(DOLLARF1ORT,response))
				strcpy(m_ReportedParameters.shutdown_type_selected,ORTEC);
			else if (0 == strcmp(DOLLARF0TTL,response))
				strcpy(m_ReportedParameters.shutdown_type_selected,TTL);
			else
				strcpy(m_ReportedParameters.shutdown_type_selected,"ERROR");

			GetPrivateProfileString(m_pParent->m_pName,
				SET_SHUTDOWN,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.shutdown_type_selected,ini_value))
			{
				WriteSetupFailureToFile(65510, SET_SHUTDOWN);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}

			CurrentSetParam(SET_SHUTDOWN, m_ReportedParameters.shutdown_type_selected, bMatch);
		}
	else if (0 == strcmp(command,SHOW_SHUT_ACTUAL))
		{
			static bool b_prior_shutdown_ok = true;

			if (0 == strcmp(DOLLARIT,response))
			{
				strcpy(m_ReportedParameters.shut_actual,ACTIVE);
				if (b_prior_shutdown_ok)
				{
					m_pParent->PostMessage(IDC_HV_SHUTDOWN,0,0);
					b_prior_shutdown_ok = false;

					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62680 STATUS CHANGE - FAILURE: HV SHUTDOWN ACTIVE\n");

					summarypage->SetColor(IDC_DSPECSUMMARY_SHUT_ACTUAL,COLOR_RED);
					datastatpage->SetColor(IDC_DSPECDATASTAT_SHUTDOWN,COLOR_RED);
					Data_Tab |= 0x8000;
				}
			}
			else if (0 == strcmp(DOLLARIF,response))
			{
				strcpy(m_ReportedParameters.shut_actual,INACTIVE);
				if (!b_prior_shutdown_ok)
				{
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, 
						"62690 STATUS CHANGE - RECOVERY: HV SHUTDOWN INACTIVE\n");

					m_pParent->PostMessage(IDC_HV_SHUTDOWN_RECOVER,0,0);
					b_prior_shutdown_ok = true;
				}
			}
			else
				strcpy(m_ReportedParameters.shut_actual,"ERROR");
		}
	else if (0 == strcmp(command,SHOW_TIME))
		{
			to   = m_ReportedParameters.time;
			from = &response[3];

			*to++ = *from++; //hh
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //mm
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //ss
			*to++ = *from;
			*to = 0;
		}
	else if (0 == strcmp(command,SHOW_TIME_START))
		{
			to   = m_ReportedParameters.time_start;
			from = &response[3];

			*to++ = *from++; //hh
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //mm
			*to++ = *from++;
			from++;
			*to++ = SEPARATOR;
			*to++ = *from++; //ss
			*to++ = *from;
			*to = 0;
		}
	else if (0 == strcmp(command,SHOW_TRUE))  //don't care
		{
		}
	else if (0 == strcmp(command,SHOW_ULD))
		{
			//$Cnnnnnccc
			response[7]=0;  //wipe out checksum
			CleanUpString(m_ReportedParameters.uld,&response[2],storagesize);

			GetPrivateProfileString(m_pParent->m_pName,
				SET_ULD,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.uld,ini_value))
			{
				WriteSetupFailureToFile(65515, SET_ULD);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}
			CurrentSetParam(SET_ULD, m_ReportedParameters.uld, bMatch);
		}
	else if (0 == strcmp(command,SHOW_VERSION))  //don't care
		{
		}
	else if (0 == strcmp(command,SHOW_ZDT))
		{
			if (0 == strcmp(DOLLARIT,response))
				strcpy(m_ReportedParameters.zdt_enable, ENABLE);
			else if (0 == strcmp(DOLLARIF,response))
				strcpy(m_ReportedParameters.zdt_enable,DISABLE);
			else
				strcpy(m_ReportedParameters.zdt_enable,"ERROR");
			
			GetPrivateProfileString(m_pParent->m_pName,
				SET_ZDT_ENABLE,"",ini_value,sizeof(ini_value),m_szIniFile);

			if (0 != strcmp(m_ReportedParameters.zdt_enable,ini_value))
			{
				WriteSetupFailureToFile(65520, SET_ZDT_ENABLE);
				InstSetup_Tab |= 0x8000;
				bMatch = false;
			}
			CurrentSetParam(SET_ZDT_ENABLE, m_ReportedParameters.zdt_enable, bMatch);
		}
	else
		{
//			ASSERT(FALSE);  //for now
		}

	if (Data_Tab)
		SetTabColor(IDC_DATA_TAB,COLOR_RED);
	if (InstSetup_Tab)
		SetTabColor(IDC_INSTSETUP_TAB, COLOR_RED);
}
//////////////////////////////////////////////////////////////////////////////
//
//                          CleanUpString
//
///////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::CleanUpString(char *destination, 
											char *source, 
											int destination_length,
											int digits_after_decimal)
{
	size_t source_length = strlen(source);

	char *firstchar    = source;
	char *decimalpoint = strchr(source,'.');
	char *nullpos = source + source_length;  //terminating null

	char *minussign = strchr(source,'-');
	if (minussign == firstchar)
		firstchar++;

	bool bleadingzero = false;

	memset(destination,0,destination_length);
	int allowed_chars = destination_length - 1;

	//does the source string have a decimal place?
	if (decimalpoint != NULL)
	{
		for (; firstchar < decimalpoint; firstchar++)
		{
			if (*firstchar != '0')
				break;
			else
				bleadingzero = true;
		}
		if (bleadingzero && (firstchar == decimalpoint))
			firstchar--;
	}
	else
	{
		//no decimal point - assume integer
		for (; firstchar < nullpos; firstchar++)
		{
			if (*firstchar != '0')
				break;
			else
				bleadingzero = true;
		}
		if (bleadingzero && (firstchar == nullpos))
			firstchar--;
	}

	if (minussign != NULL)
	{
		*destination++ = '-';
		destination_length--;
	}

	size_t numchars = strlen(firstchar);
	if (numchars >= (size_t)destination_length)
		numchars = destination_length - 1;

	// clean up any trailing zeroes
	if (decimalpoint)  //don't do this for integers!
	{
		char *lastchar= strchr(firstchar,0) - 1;

		for (;;)
		{
			if ((lastchar != decimalpoint) && (*lastchar == '0'))
				*lastchar-- = 0;
			else
				break;
		}
	}

	if ((digits_after_decimal > 0) && (digits_after_decimal < 10) )  //allow 1 - 9
	{
			if (!decimalpoint)
			{
				strncat(firstchar, ".", 1);  //append the dp
				numchars++;
				for (int i = 0; i < digits_after_decimal; i++)
				{
					if (numchars >= (size_t)allowed_chars)
						break;
					strncat(firstchar, "0", 1);
					numchars++;
				}
			}
			else
			{
				float fnum;
				int i,total;
				unsigned int unum;

				sscanf(firstchar,"%f",&fnum);

				float faddfactor = 0.5;
				float fmulfactor = 1.0;

				for (i = 0; i < digits_after_decimal; i++)
				{
					faddfactor /= 10.0;
					fmulfactor *= 10.0;
				}

				fnum += faddfactor;
				fnum *= fmulfactor;

				unum = (unsigned int)(fnum);  //truncate
				fnum = (float)unum;
				fnum /= fmulfactor;

				int digits_to_the_left = 0;
				float fleft = 1.0;
				
				for (;;)
				{
					if (fnum < fleft)
					{
						break;
					}
					else
					{
						fleft *= 10.0;
						digits_to_the_left++;

						if (digits_to_the_left >= 5)  //need some limit here
							break;
					}
				}

				if (digits_to_the_left == 0)  //leading zero
					digits_to_the_left = 1;

				total = digits_to_the_left + 1 + digits_after_decimal;

				char fmt[16], temp[16];;
				memset(fmt,0,16);

				fmt[0] = '%';							//"%"
				sprintf(temp,"%d",total);
				strncat(fmt,temp,3);					//"%7"
				strncat(fmt,".",1);						//"%7."
				sprintf(temp,"%d",digits_after_decimal);
				strncat(fmt,temp,3);					//"%7.5"
				strncat(fmt,"f",1);						//"%7.5f"

				sprintf(firstchar,fmt,fnum);
			}
	}
	memcpy(destination, firstchar, numchars);
}
//////////////////////////////////////////////////////////////////////////////
//
//                          OnReceive
//
///////////////////////////////////////////////////////////////////////////////
//
//         The "cracker" for spectrum data coming from the DSPEC via the CSO
//
////////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//TRACE("CDSPECSafePropertySheet::OnReceive(), LParam = %u\n", LParam);
	CDSPECDataStat* datapage = m_pParent->m_pDataStatus;

	DWELL_TYPE dwelltype = (DWELL_TYPE)WParam;  //see ORTECSpectrum.h

	CORTECSpectrum *pspec = (CORTECSpectrum *)LParam;

	switch(dwelltype)
	{
		case REALTIME_DWELL_1:
		{
			m_uLocalNonzero_channels = pspec->NonZeroChannels();
			m_uLocalChannels_above_threshold = pspec->ChannelsAboveThreshold();

			datapage->ProcessSpectrumBufferForRealTimeGraphic(pspec);

			if (m_uLocalChannels_above_threshold != 0)
				m_pParent->m_pButton->ShowIcon_Bell(VARIANT_TRUE);
			else
				m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);

			break;
		}

		case LONG_DWELL_3:
		{
			if (0 != pspec->ChannelsAboveThreshold())
			{
				WriteToCHNFile(pspec,'N');
			}
			m_pParent->PostMessage(
				uIDC_RECEIVE, 
				(WPARAM)IDC_DSPEC_SPECTRUM_DATA_PROCESSED, 
				(LPARAM)pspec);

			break;
		}
		case SHORT_DWELL_2:
		{
			m_pParent->PostMessage(
				uIDC_RECEIVE, 
				(WPARAM)IDC_DSPEC_SPECTRUM_DATA_PROCESSED, 
				(LPARAM)pspec);
			break;
		}

		case IMMEDIATE_DWELL_4:
		{
			m_pParent->PostMessage(
				uIDC_RECEIVE, 
				(WPARAM)IDC_DSPEC_SPECTRUM_DATA_PROCESSED, 
				(LPARAM)pspec);
			break;
		}

		default:
		{
			STUB;  // <<QA>> put and error message in a file somewhere
		}

	} //end switch

	//The CSO owns the CORTECSpectrum object.  Never delete it.
	//delete [] (char*)LParam;
	return;
}

bool CDSPECSafePropertySheet::WriteToCHNFile(CORTECSpectrum *pspec, char tag)
{

	//DR 267: Corrected problem that the 61000 message in the PFM and 
	//CEV files does not print the CHN filename.
	//pjm 8 March 2006 (for ESP 14 Rev 1 and eventually for B2R1)

//	TRACE("CDSPECSafePropertySheet::WriteToCHNFile()\n");

//#define ENDLINE "\n"  //this still double spaces in the CEV and PFM files.

	const char* crlf = "\n";

	// if unable to write to disk, bail
	if (m_pParent->m_bBlockWrite) 
		return false;

	bool success;

	CDatTim *Time = pspec->DateTime();

	CString cs, csCurrentCHNFileName;

	// Made changes to upgrade from Baseline MIC_CB_028 to MIC 1.9.0.7.
	// PJM 1/19/2005 - captured code that builds the short name
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentCHNFileName,"%s\\%s%c%c%c%c%02d%02d.%3s",

				m_pParent->m_szSaveLoc,	//%s
				                        //
				m_pParent->m_pID,       //%s

				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),  //%c #1

				((Time->Mon()<10)?
					'0'+(Time->Mon()):
					'A'+(Time->Mon()-10)),    //%c #2

				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day() - 10)),  //%c #3

//				((Time->Hour()  <10)?
//					'0'+(Time->Hour()):
//					'A'+(Time->Hour() - 10)),  //%c #4
				('A'+Time->Hour()),

				Time->Min(),    //%02d #1
				Time->Sec(),    //%02d #2
				                // .
				CHN_SUFFIX      //%3s
				);

				csCurrentCHNFileName = CString(m_szCurrentCHNFileName);
	}
	else
	{
		csCurrentCHNFileName = CString(m_szCurrentCHNFileName);

		BuildFileName(csCurrentCHNFileName,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"DS",
				m_pParent->m_pID,
				Time->Year(),
				Time->Mon(),
				Time->Day(),
				Time->Hour(),
				Time->Min(),
				Time->Sec(),
				"");

		csCurrentCHNFileName += '.' + CString(CHN_SUFFIX);
		strncpy(
			m_szCurrentCHNFileName,
			LPCTSTR(csCurrentCHNFileName),
			sizeof(m_szCurrentCHNFileName));
	}
	// Make sure the directory exists
	if (_access(m_pParent->m_szSaveLoc, 0) != 0)
	{
		//make sure path exists
		MakeMyPath(m_pParent->m_szSaveLoc);
		//_mkdir(m_pParent->m_szSaveLoc); //dgp -- this only makes last dir element in path
	}

	if (pspec->Write(m_szCurrentCHNFileName))
	{
		success = true;
		cs =  "61000 Write CHN file: "  + csCurrentCHNFileName + crlf;//"\n\r";

	}
	else //bad write
	{
		success = false;
		cs = "61100 Write CHN file FAILED: " + csCurrentCHNFileName + crlf;//"\n\r";
	}

	WriteToFile( TO_CEV | TO_PFM, TYPE_COMP, NULL, cs);

	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cs);

	cs = csCurrentCHNFileName + crlf;

	if (!success)
		cs = "FAILED " + cs;

	cs = "Write file: " + cs;
	const char *pch = (LPCTSTR)cs;

	UpdateSummary((char *)pch);
	m_pParent->PostToWatch((char *)pch);

	return success;
}

void CDSPECSafePropertySheet::CurrentSetParam(char*szIniName,char*szValue,bool bMatch)
{
	CDSPECProppageSetupData*  settingspage = m_pParent->m_pSetupData;

	settingspage->CurrentSetParam(szIniName,szValue,bMatch);

	if (!bMatch)
		SetTabColor(IDC_INSTSETUP_TAB,COLOR_RED);
}

int CDSPECSafePropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCloseButton = new CColorButton(false, m_szIniFile);
	m_pPauseButton = new CButton();
	m_pAckFlagsButton = new CButton();
	m_pPrintButton = new CButton();
	m_pToClose = new CButton();
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);
	return 0;
}

void CDSPECSafePropertySheet::OnCloseButton()
{
	CDSPECModParam* modparpage  = m_pParent->m_pModParams;
	modparpage->ShowWindow(SW_HIDE);
	
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CDSPECSafePropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if ((Tab != IDC_DATA_TAB) && (Tab != IDC_INSTSETUP_TAB)) 
		return;

	int tab = 0;

	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;

	switch(Color) 
	{
	case COLOR_RED:
		pTabCtrlItem.iImage = 0;
		break;
	case COLOR_GREEN:
		pTabCtrlItem.iImage = 1;
		break;
	case COLOR_BLUE:
		pTabCtrlItem.iImage = 2;
		break;
	case COLOR_YELLOW:
		pTabCtrlItem.iImage = 3;
		break;
	case COLOR_CYAN:
		pTabCtrlItem.iImage = 4;
		break;
	case COLOR_MAGENTA:
		pTabCtrlItem.iImage = 5;
		break;
	case COLOR_WHITE:
		pTabCtrlItem.iImage = 6;
		break;
	case COLOR_BLACK:
		pTabCtrlItem.iImage = 7;
		break;
	case COLOR_YELLOWRED:
		pTabCtrlItem.iImage = 10;
		break;
	default:
		pTabCtrlItem.iImage = 6;
	}

	switch (Tab) 
	{
		case IDC_DATA_TAB:      tab = 1; break;
		case IDC_INSTSETUP_TAB: tab = 2; break;
	}

	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}

void CDSPECSafePropertySheet::OnAckButton()
{
	m_dToClose = 0;

	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);
	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	CDSPECSummary*  summarypage = m_pParent->m_pSummaryPage;
	CDSPECDataStat* datapage    = m_pParent->m_pDataStatus;
	CDSPECProppageSetupData*  setdatapage = m_pParent->m_pSetupData;

	summarypage->ResetColors();
	datapage->ResetColors();
	setdatapage->ResetColors();

	SetTabColor(IDC_DATA_TAB,COLOR_GREEN);       //Data Status
	SetTabColor(IDC_INSTSETUP_TAB,COLOR_GREEN);  //Inst. Settings.

	Data_Tab = 0x0000;
	InstSetup_Tab = 0x0000;

	Invalidate();
}

void CDSPECSafePropertySheet::OnPauseButton()
{
	m_dToClose = 0;

	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
	}
}

void CDSPECSafePropertySheet::IssuePrint(FILE* outfile)
{
	//fprintf(outfile,"************Begin CDSPECSafePropertySheet::IssuePrint()\n");

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");

	CDSPECSummary*  summarypage = m_pParent->m_pSummaryPage;
	CDSPECDataStat* datapage    = m_pParent->m_pDataStatus;
	CDSPECModParam* modparpage  = m_pParent->m_pModParams;
	CDSPECProppageSetupData* setuppage  = m_pParent->m_pSetupData;

	fprintf(outfile,"* = Displayed in RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	summarypage->IssuePrint(outfile);
	datapage->IssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	modparpage->IssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	setuppage->IssuePrint(outfile);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"\nCURRENT CONFIGURATION (from INI file)\n");
	char cIniData[16384];  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
	if (GetPrivateProfileSection(m_pParent->m_pName,cIniData,sizeof(cIniData),m_pParent->m_szIniFile)>0)
	{
		char *at = cIniData;		//char pointer to step through buffpass
		while (strlen(at)>0) 
		{
			fprintf(outfile,"  %s\n",at);
			at = at + strlen(at)+1;
		}
	}

	fprintf(outfile,"\n");

	//fprintf(outfile,"*********END CDSPECSafePropertySheet::IssuePrint()\n");
}

void CDSPECSafePropertySheet::OnPrintButton()
{
	FILE* outfile;

	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",m_pCaption,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);
	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");

		m_pParent->IssuePrint(outfile); // <<QA>>should be like this for all ISO's

		fclose(outfile);
	}
}

void CDSPECSafePropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	m_dToClose = 0;
	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(TIMER_AUTOCLOSE,1000,NULL);
		UpdateSummary(NULL);

		//if (m_pParent->m_bPause)
		//	m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		//else
		//	m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

	}
	else
		KillTimer(1);

}

void CDSPECSafePropertySheet::OnToCloseButton()
{
//	TRACE("CDSPECSafePropertySheet::OnToCloseButton()\n");

	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND)  //120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}


void CDSPECSafePropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_AUTOCLOSE)
	{
		UpdateSummary(NULL);

		UpdateDataStatus();

		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			m_dToClose = 0;
			CDSPECModParam* modparpage  = m_pParent->m_pModParams;
			modparpage->ShowWindow(SW_HIDE);
			ShowWindow(SW_HIDE);
			KillTimer(TIMER_AUTOCLOSE);
		} 
		else
		{
			char temp[32];
			int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
			int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

HBRUSH CDSPECSafePropertySheet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPropertySheet::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDSPECSafePropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	// TODO: Add your message handler code here and/or call default
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
	{
		CDSPECModParam* modparpage  = m_pParent->m_pModParams;
		modparpage->ShowWindow(SW_HIDE);
		ShowWindow(SW_HIDE);
	}
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}

BOOL CDSPECSafePropertySheet::OnInitDialog() 
{
//	TRACE("CDSPECSafePropertySheet::OnInitDialog()\n");

	//strncpy(m_pName,m_pParent->m_pName,32);

	EnableStackedTabs(FALSE);

	BOOL bResult = CPropertySheet::OnInitDialog();

	CRect WindowRect;
	CRect FinalRect;
	GetWindowRect(&WindowRect);
	WindowRect.bottom += 60;
	MoveWindow(&WindowRect);

	int Interspace = 6;
	int ButtonWidth = (WindowRect.right - WindowRect.left)/5 - 6*Interspace;
	if (ButtonWidth < 100) 
		ButtonWidth = 116;
	else
		ButtonWidth = 157;

	FinalRect = WindowRect;
	ScreenToClient(FinalRect);
	int Y1 = FinalRect.bottom - 60;
	int Y2 = FinalRect.bottom - 10;

	DWORD dwStyle = 
		WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	m_pCloseButton->Create( "Close", dwStyle, 
		CRect(Interspace,  Y1,Interspace + ButtonWidth,Y2),
		this,
		ID_CLOSE_BUTTON);

	m_pCloseButton->SetPreNum("TTG: ",true);

	m_pPauseButton->Create(COLLECTINGBEGIN, dwStyle,
		CRect(2*Interspace + ButtonWidth,Y1,2*Interspace + 2*ButtonWidth,Y2),
		this,
		ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags", dwStyle,
		CRect(3*Interspace + 2*ButtonWidth,Y1,3*Interspace + 3*ButtonWidth,Y2),
		this,
		ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status To File", dwStyle,
		CRect(4*Interspace + 3*ButtonWidth,Y1,4*Interspace + 4*ButtonWidth,Y2),
		this,
		ID_PRINT_BUTTON);

	char temp[32];
	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->Create(temp, dwStyle,
		CRect(5*Interspace + 4*ButtonWidth,Y1,5*Interspace + 5*ButtonWidth,Y2),
		this,
		ID_TOCLOSE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND);
	}
	
	GetTabControl( )->SetImageList(m_pImageList);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);  //Data Status tab
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(2,&pTabCtrlItem);  //Inst. Settings tab

	return bResult;
}

BOOL CDSPECSafePropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CDSPECSafePropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
		m_dToClose = set;
}
//////////////////////////////////////////////////////////////////////////////
//
//                            WriteSetupFailureToFile
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::WriteSetupFailureToFile(int number, char *msg)
{
	CString cs;
	cs.Format("%d",number);
	cs += " DSPEC SETUP FAILURE - ";
	cs += msg;
	cs += "\n";

	WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, cs);
}
//////////////////////////////////////////////////////////////////////////////
//
//                            WriteToFile
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::WriteToFile(int ToWhom, 
										  int Type, 
										  CJulianTime* Time, 
										  const char* Msg)
{
	//TRACE("CDSPECSafePropertySheet::WriteToFile()\n");

	FILE *File;
	char Message[256];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime(); 		//computer time
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_pParent->m_bBlockWrite) 
		return;

	//default to the provided grand time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100)  Year	= 1900;//COleDateTime limits year 100-9999
		if (Year    > 9999) Year    = 9999;//COleDateTime limits year 100-9999
		if (Month	< 1)	Month	= 1;
		if (Day		< 1)	Day		= 1;
		if (Hour	< 0)	Hour	= 0;
		if (Minute	< 0)	Minute	= 0;
		if (Second	< 0)	Second	= 0;
		TG = COleDateTime(Year, Month, Day, Hour, Minute, Second);
	}
	//if that wasn't available then get the computer time
	//this is the case in all TYPE_COMP messages
	else
	{
		TG = COleDateTime::GetCurrentTime();
	}

	if (ToWhom == TO_DMP)
	{
		
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	//			'0' + (TC.GetYear()%10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(
				cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"DS",
				m_pParent->m_pID,
				TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");

			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((TG.GetYear()-1990)<10)?
					((TG.GetYear()-1990<0)?'#':'0'+(TG.GetYear()-1990)):
					'A'+(TG.GetYear()-2000),
//				'0' + (TG.Year() % 10),
				((TG.GetMonth()<10)?
					'0'+(TG.GetMonth()):
					'A'+(TG.GetMonth()-10)),
				((TG.GetDay()  <10)?
					'0'+(TG.GetDay()):
					'A'+(TG.GetDay()  -10)),
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"DS",
				m_pParent->m_pID,TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,".");

			strcpy(m_szCurrentFileName,cResult);
		}
	}	

	//format string to send
	switch (Type){

	case TYPE_DUMP:
		sprintf(Message,"%4d.%02d.%02d %02d:%02d:%02d %s",
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			Msg);
//		strcpy(Message,Msg);
		break;

	case TYPE_INST:
		//	message = Time::YY.MM.DD HH:MM:SS G (MESSAGE==NULL)?\r:MESSAGE

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\n":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS Grand Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - G = xxxx\r"
		//computer time
		
		//grand time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d G 67330 DSPEC Time %04d.%02d.%02d %02d:%02d:%02d C 727 Computer Time   C - G = %.0f seconds\n",
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 67280 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),	TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond());
		
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C DSPEC COLLECT Version %s<VERSION> started\r"
		
		sprintf(Message,"\r%04d.%02d.%02d %02d:%02d:%02d C 67290 DSPEC COLLECT Version %s started\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C GRAND COLLECT Version %s<VERSION> started\r"
		
		sprintf(Message,"\r%04d.%02d.%02d %02d:%02d:%02d C 67300 DSPEC COLLECT Version %s started from abnormal shutdown\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 67310 File %s deleted\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 67320 Unknown TYPE %s\n",
			TC.GetYear(),	TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+dmp
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
//		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (_access(fn,0) != 0)
		{
//			if (_access(ArchiveFileNameEx,0)!=-1)
//				MoveFileEx(ArchiveFileNameEx,fn,
//					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			MakeMyPath(fn);
		}

		if ((File = fopen(fn,"at")) != NULL)  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
		{
			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
		return;
	}

	//if to cev
	if (ToWhom & TO_CEV)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += CEV_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);
			
			if (m_pParent->m_bUseShortFilename)
			{
				//build archive path\name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
//				'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(
					ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,
					"DS",
					m_pParent->m_pID,
					TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos(File,&position);

			//	send string
			fprintf(File,Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+pfm
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			MakeMyPath(fn);

			//check if it is in the archive directory
			if (m_pParent->m_bUseShortFilename)
			{
				//build file name
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				m_pParent->m_szSaveLoc,
				m_pParent->m_pID,
				((TC.GetYear()-1990)<10)?
					((TC.GetYear()-1990<0)?'#':'0'+(TC.GetYear()-1990)):
					'A'+(TC.GetYear()-2000),
	//			'0' + (TC.GetYear() % 10),
				((TC.GetMonth()<10)?
					'0'+(TC.GetMonth()):
					'A'+(TC.GetMonth()-10)),
				((TC.GetDay()  <10)?
					'0'+(TC.GetDay()):
					'A'+(TC.GetDay()  -10)),
				'A',0,PFM_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_pParent->m_csLongNameFormatString,cTemp,"DS",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,PFM_SUFFIX);
			}

			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at")) != NULL)
		{
			//if new file do stuff
//			fseek(File,0,SEEK_END);
//			fgetpos( File, &position );

			//	send string
			fprintf(File,Message);
			//	close pfm
			fclose(File);
		}
	}

}
//////////////////////////////////////////////////////////////////////////////
//
//                            OnExtendClose
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::OnExtendClose(WPARAM,LPARAM)
{
	m_dToClose = 0;
}
//////////////////////////////////////////////////////////////////////////////
//
//                            MakeMyPath
//
//////////////////////////////////////////////////////////////////////////////
void CDSPECSafePropertySheet::MakeMyPath(const char *path)
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
//////////////////////////////////////////////////////////////////////////////
//
//                           BuildFileName
//
//////////////////////////////////////////////////////////////////////////////
//this function should go into the parent class
void CDSPECSafePropertySheet::BuildFileName(CString &cResult,
	CString cFormat,CString cPath,CString cType,
	CString cID,
	int iYear,	int iMonth,	int iDay,
	int iHour,	int iMinute,int iSecond,
	CString cSuffix
	)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
			case 'T':
				cResult += cType;
				break;
			case 'I':
				{
					int iLength = cID.GetLength();
					while (iLength < 3)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'i':
				{
					int iLength = cID.GetLength();
					while (iLength < 2)
					{
						cResult += '0';
						iLength++;
					}
					cResult += cID;
				}
				break;
			case 'y':
				cTemp.Format("%04d",iYear);
				cResult += cTemp;
				break;
			case 'm':
				cTemp.Format("%02d",iMonth);
				cResult += cTemp;
				break;
			case 'd':
				cTemp.Format("%02d",iDay);
				cResult += cTemp;
				break;
			case 'H':
				cTemp.Format("%02d",iHour);
				cResult += cTemp;
				break;
			case 'M':
				cTemp.Format("%02d",iMinute);
				cResult += cTemp;
				break;
			case 'S':
				cTemp.Format("%02d",iSecond);
				cResult += cTemp;
				break;
			default:
				cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}
