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
// HHMRSafePS.cpp : implementation file
//

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "HHMRSafePS.h"
#include "HHMRInst.h"
#include "INI_definitions.h"

#include <math.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CHHMRPropertySheet

IMPLEMENT_DYNAMIC(CHHMRPropertySheet, CPropertySheet)

CHHMRPropertySheet::CHHMRPropertySheet(
	UINT nIDCaption, 
	CHHMRInst* pParentWnd,
	char *inifile) 
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	FinishConstruction();
}

CHHMRPropertySheet::CHHMRPropertySheet(
	LPCTSTR pszCaption, 
	CHHMRInst* pParentWnd,
	char *inifile) 
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	strcpy(m_szIniFile, inifile);
	FinishConstruction();
}

void CHHMRPropertySheet::FinishConstruction()
{
	uIDC_RECEIVE     = m_pParent->m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	Data_Tab		= NULL;

	m_bFirstHV		= true;
	m_bHighVoltOk   = true;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;

	m_dLowHV = m_dHighHV = m_dCurHV	= 0;
	m_bFirstHV = true;

	m_bOverThresh = false;
	m_dBackgroundCount = 0;

	m_dToClose = 0;

	m_szCurrentFileName[0] = NULL;

	m_dAt			= 0;
	m_dAcquireRecordCount = 0;
	m_dMessageRecordCount = 0;
	m_fLastAux1 = 0.0;
	m_fLastAux2 = 0.0;
	m_fInquireInt = 0.0;

	m_bRegisteredZero = false;
	m_bZeroTransitionUp = false;
	m_bZeroTransitionDown = false;

	m_PreviousSetup.SetStatus(COleDateTime::null);
	strcpy(m_cCameraRealorTotals,"RT1");

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;
	m_iBytesToWaitFor = sizeof(m_pAccumulator)-1;
	m_iTypeOfMessageExpected = 0;
	m_bHVSetPointStatusFail = m_bCountTimeStatusFail = 
		m_bPreDelayStatusFail = m_bGateWidthStatusFail = false;

	//m_LastStart.SetDateTime(1952,1,1,0,0,0);
	m_LastStart = COleDateTime::GetCurrentTime();
}

CHHMRPropertySheet::~CHHMRPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}

COLORREF CHHMRPropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		//set compress count at a high enough number to 
		//force next data point to be the first of a set.
		m_dCompressCount = 999999;
	}
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
	return m_pCloseButton->SetFaceColor(color);
}

/*void CHHMRPropertySheet::SetCamera()
{
	char buff[16];
	m_pParent->SetCamera(true);
	GetPrivateProfileString(
		m_pParent->m_pName,
		"CAMERA_PORT","NONE",buff,sizeof(buff),m_pParent->m_szIniFile);
	buff[0] = (char)toupper(buff[0]);
	m_bCameraDo = (buff[0] == 'P');
}*/

void CHHMRPropertySheet::SetLimits(
		double lRealThresh,
		double lTotalThresh,
		double lTotal1Thresh,
		double lTotalLThresh,
		double lSigmaThresh,
		int	dMaxCompress,
		int	dCountTimeUnit,
//		int dCountTimeExp,
		int	dHighV,
		int	dGateWidth,
		int	dPreDelay,
		int	dHighVoltH, int dHighVoltL,
	//	char cCameraRealorTotals[4],
	//	bool bCameraDo,
		char * pId)
{	
	m_lRealThresh  = lRealThresh;
	m_lTotalThresh  = lTotalThresh;
	m_lTotal1Thresh  = lTotal1Thresh;
	m_lTotalLThresh  = lTotalLThresh;
	m_lSigmaThresh = lSigmaThresh;
	m_dMaxCompress = dMaxCompress;
	m_dCountTimeUnit = dCountTimeUnit;
//	m_dCountTimeExp = dCountTimeExp;
	m_dHighV = dHighV;
	m_dGateWidth = dGateWidth;
	m_dPreDelay = dPreDelay;
	m_dHighVoltH = dHighVoltH;
	m_dHighVoltL = dHighVoltL;
	//strcpy(m_cCameraRealorTotals,cCameraRealorTotals);
	//m_bCameraDo = bCameraDo;
	strcpy(m_szCurrentID,pId);
}




BEGIN_MESSAGE_MAP(CHHMRPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CHHMRPropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON,	OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON,	OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON,	OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON,	OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE,		OnToCloseButton)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE,OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, OnReceiveOOB)
	ON_MESSAGE(IDC_EXTENDCLOSE, OnExtendClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHHMRPropertySheet message handlers
void CHHMRPropertySheet::UpdateSummary(char *Message)
{
	CString Data;
	CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND12,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND13,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND11,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND12,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND10,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND11,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND9,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND10,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND8,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND9,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND7,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND8,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND6,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND7,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_HHMRSUMMARY_COMMAND1,Message);

}

LRESULT CHHMRPropertySheet::OnReceiveOOB(WPARAM /*WParam*/, LPARAM LParam)
{
	CoTaskMemFree((void *)LParam);
	return 0;
}

void CHHMRPropertySheet::PrepForReceipt(int iTypeOfMessageExpected, bool bClearAccumulator)
{
	switch(iTypeOfMessageExpected) {
	case HHMR_READ_STATUS:		m_iBytesToWaitFor = HHMR_READ_STATUS_LENGTH;break;
	case HHMR_READ_AUX_SCALERS:	m_iBytesToWaitFor = HHMR_READ_AUX_SCALERS_LENGTH;break;
	case HHMR_READ_HV_SP:		m_iBytesToWaitFor = HHMR_READ_HV_SP_LENGTH;break;
	case HHMR_READ_HV:			m_iBytesToWaitFor = HHMR_READ_HV_LENGTH;break;
	case HHMR_READ_COUNTTIME:	m_iBytesToWaitFor = HHMR_READ_COUNTTIME_LENGTH;break;
	case HHMR_READ_T_TA_A:		m_iBytesToWaitFor = HHMR_READ_T_TA_A_LENGTH;break;
	case HHMR_READ_PREDELAY:		m_iBytesToWaitFor = HHMR_READ_PREDELAY_LENGTH;break;
	case HHMR_READ_GATEWIDTH:	m_iBytesToWaitFor = HHMR_READ_GATEWIDTH_LENGTH;break;
	default:// HHMR_UNKNOWN_RECORD 
		m_iBytesToWaitFor = sizeof(m_pAccumulator)-1;
	}

	m_iTypeOfMessageExpected = iTypeOfMessageExpected;
	if (bClearAccumulator)
		m_dAt = 0;
}

LRESULT CHHMRPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	char* cbuff =(char*)LParam;
	char c;
	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"53380 Bad LParam Address Received (1002)");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"53380 Bad LParam Address Received (1002)");
		CoTaskMemFree((void *)LParam);
		return 0;
	}

	for (unsigned int index = 0; index < WParam; index ++)
	{
		m_pAccumulator[m_dAt] = (char)cbuff[index];//(char)(cbuff[index] & 0x7f);
		if (m_dAt >= (m_iBytesToWaitFor-1))
		{
			if (m_pParent->m_bDoDump)
			{
				char cBuffx[2048];
				strcpy(cBuffx,"53390");
				for (int i = 0; i <= m_dAt; i++)
				{
					char cTemp[8];
					sprintf(cTemp," [%02x]",m_pAccumulator[i]);
					strcat(cBuffx,cTemp);
				}
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuffx);
			}


			switch(m_iTypeOfMessageExpected) {
			case HHMR_READ_STATUS://bytes:1	type: get status response
				StatusRec(m_pAccumulator);
				break;
			case HHMR_READ_AUX_SCALERS://bytes:10	type: read aux scalers
				AuxRec(m_pAccumulator);
				break;
			case HHMR_READ_HV_SP://bytes 2	type: Get HV set point
				HVSetPoint(m_pAccumulator);
				break;
			case HHMR_READ_HV://bytes 2	type: Get HV reading
				HVValue(m_pAccumulator);
				break;
			case HHMR_READ_COUNTTIME://bytes 3    type: Get count time
				CountTime(m_pAccumulator);
				break;
			case HHMR_READ_T_TA_A://bytes 21	type: Read Totals,real+accidentals,accidentals
				T_TA_AValues(m_pAccumulator);
				break;
			case HHMR_READ_PREDELAY://bytes 2	type: Get Pre-delay
				PreDelay(m_pAccumulator);
				break;
			case HHMR_READ_GATEWIDTH://bytes 2	type: Get Gate width
				GateWidth(m_pAccumulator);
				break;
			default:
				{
					char cBuffx[2048];
					m_pParent->PostMessage(uIDC_RECEIVE,HHMR_UNKNOWN_RECORD,0);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, "53400 Unknown Message Received from HHMR");
					if (m_pParent->m_bDoDump)
		                WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53400 Unknown Message Received from HHMR");
					strcpy(cBuffx,"53410 ");
					for (int i = 0; i < m_dAt; i++)
					{
						char cTemp[8];
						sprintf(cTemp," [%02x]",m_pAccumulator[i]);
						strcat(cBuffx,cTemp);
					}
					WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuffx);
				}
			}
		}
		else
		{
			m_dAt++;
			if (m_dAt >= ACCUMSIZE) m_dAt = 0;
		}
	}

	CoTaskMemFree((void *)LParam);
	return 0;

}


//void CHHMRPropertySheet::StatusRecErr(char * Record)
//{
//	char *SubSection;
//	SubSection = Record + strlen(Record) - HHMR_STATUS_RECORD_LENGTH;
//	StatusRec(SubSection);
//}

void CHHMRPropertySheet::StatusRec(BYTE *pAccumulator)
{
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
	char Record[9];
	for (unsigned char i = (unsigned char)0x80, j = 8; j > 0; i = (unsigned char)(i >> 1), j--)
	{
		Record[j-1] = (pAccumulator[0] & i)?'1':'0';
	}
	Record[8] = NULL;
	UpdateSummary(Record);

	char temp[32];
	COleDateTime Now = COleDateTime::GetCurrentTime();
	strcpy(temp,Now.Format("%Y.%m.%d %H:%M:%S"));
    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSTIME,temp);

    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSA,(Record[0]=='1')?"Started":"Stopped");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSB,(Record[1]=='1')?"Yes":"No");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSC,(Record[2]=='1')?"Yes":"No");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSD,(Record[3]=='1')?"Yes":"No");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSE,(Record[4]=='1')?"Yes":"No");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSF,(Record[5]=='1')?"Yes":"No");
//    datapage->SetDlgItemText(IDC_HHMRDATA_STATUSG,(Record[6]=='1')?"Yes":"No");
    //datapage->SetDlgItemText(IDC_HHMRDATA_STATUSH,(Record[7]=='1')?"Yes":"No");

	if (Record[0] == '1' ) //running
        m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_STATUS,1);
	else	//stopped
        m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_STATUS,0);
}

void CHHMRPropertySheet::HVSetPoint(BYTE *pAccumulator)
{
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	char temp[8];
	//get the duration
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = pAccumulator[0];
	uByteInt.bBytes[0] = pAccumulator[1];

	sprintf(temp,"%4d",uByteInt.iValue);
	datapage->SetDlgItemText(IDC_HHMRDATA_SETUP_HV,temp);
	UpdateSummary(temp);
	if (uByteInt.iValue != m_dHighV)
	{
		m_bHVSetPointStatusFail = true;
        datapage->SetColor(IDC_HHMRDATA_SETUP_HV,COLOR_RED);
		m_pParent->FlagOthr(1,true);			
        summarypage->SetColor(IDC_HHMRSUMMARY_CONFIGURATION,COLOR_RED);
        SetTabColor(IDC_HHMRDATA_TAB,COLOR_RED);
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"NOT THE SAME"); 
	}
	else
		m_bHVSetPointStatusFail = false;

	if (!m_bHVSetPointStatusFail && !m_bCountTimeStatusFail && 
		!m_bPreDelayStatusFail && !m_bGateWidthStatusFail)
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"IDENTICAL");

	m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_HV_SP,0);
};

void CHHMRPropertySheet::CountTime(BYTE *pAccumulator)
{
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	char temp[16];
	//get the duration
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = pAccumulator[0];
	uByteInt.bBytes[1] = pAccumulator[1];
	uByteInt.bBytes[0] = pAccumulator[2];

	sprintf(temp,"%.1f",((float)uByteInt.iValue)/10.0);
//	temp[strlen(temp)-3] = temp[strlen(temp)-1];
//	temp[strlen(temp)-2] = NULL;
	datapage->SetDlgItemText(IDC_HHMRDATA_SETUP_COUNT,temp);
	UpdateSummary(temp);
	double fInquireInt = atof(temp);
	if ((int(fInquireInt*10.0) != m_dCountTimeUnit) && (m_dCountTimeUnit > 0.0))
	{
		m_bCountTimeStatusFail = true;
        datapage->SetColor(IDC_HHMRDATA_SETUP_COUNT,COLOR_RED);
		m_pParent->FlagOthr(1,true);			
        summarypage->SetColor(IDC_HHMRSUMMARY_CONFIGURATION,COLOR_RED);
        SetTabColor(IDC_HHMRDATA_TAB,COLOR_RED);
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"NOT THE SAME"); 
	}
	else
		m_bCountTimeStatusFail = false;

	if (!m_bHVSetPointStatusFail && !m_bCountTimeStatusFail && 
		!m_bPreDelayStatusFail && !m_bGateWidthStatusFail)
		summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"IDENTICAL");

	m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_COUNTTIME,uByteInt.iValue);
};

void CHHMRPropertySheet::PreDelay(BYTE *pAccumulator)
{
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	char temp[8];
	//get the duration
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = pAccumulator[0];
	uByteInt.bBytes[0] = pAccumulator[1];

	sprintf(temp,"%.2f",((float)uByteInt.iValue)/4.0);
	datapage->SetDlgItemText(IDC_HHMRDATA_SETUP_PRE,temp);
	UpdateSummary(temp);

	if	((uByteInt.iValue/2 - 1) != m_dPreDelay)
	{
		m_bPreDelayStatusFail = true;
        datapage->SetColor(IDC_HHMRDATA_SETUP_PRE,COLOR_RED);
		m_pParent->FlagOthr(1,true);			
        summarypage->SetColor(IDC_HHMRSUMMARY_CONFIGURATION,COLOR_RED);
        SetTabColor(IDC_HHMRDATA_TAB,COLOR_RED);
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"NOT THE SAME"); 
	}
	else
		m_bPreDelayStatusFail = false;

	if (!m_bHVSetPointStatusFail && !m_bCountTimeStatusFail && 
		!m_bPreDelayStatusFail && !m_bGateWidthStatusFail)
		summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"IDENTICAL");

	m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_PREDELAY,0);

};

void CHHMRPropertySheet::GateWidth(BYTE *pAccumulator)
{
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	char temp[8];
	//get the duration
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = pAccumulator[0];
	uByteInt.bBytes[0] = pAccumulator[1];

	uByteInt.iValue++;
	
	sprintf(temp,"%.0f",((float)uByteInt.iValue)/4.0);
	datapage->SetDlgItemText(IDC_HHMRDATA_SETUP_GATE,temp);
	UpdateSummary(temp);

	if	((uByteInt.iValue/4) != m_dGateWidth)
	{
		m_bGateWidthStatusFail = true;
        datapage->SetColor(IDC_HHMRDATA_SETUP_GATE,COLOR_RED);
		m_pParent->FlagOthr(1,true);			
        summarypage->SetColor(IDC_HHMRSUMMARY_CONFIGURATION,COLOR_RED);
        SetTabColor(IDC_HHMRDATA_TAB,COLOR_RED);
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"NOT THE SAME"); 
	}
	else
		m_bGateWidthStatusFail = false;

	if (!m_bHVSetPointStatusFail && !m_bCountTimeStatusFail && 
		!m_bPreDelayStatusFail && !m_bGateWidthStatusFail)
		summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,"IDENTICAL");

	m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_GATEWIDTH,0);

};

//totals, reals etc
void CHHMRPropertySheet::T_TA_AValues(BYTE *pAccumulator)
{
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;

//	//save time in binary acquire record
	COleDateTime Base(1952,1,1,0,0,0);
	COleDateTimeSpan Difference = (m_LastStart - Base);
	Difference += HALF_SECOND;
	CJulianTime Jtemp;
	Jtemp.Init((unsigned)(Difference.GetTotalSeconds()));

    HHMRAcquireRec.JulianSeconds = (unsigned)Difference.GetTotalSeconds();
    HHMRAcquireRec.StatusByte = NULL;
    HHMRAcquireRec.Totals2Count = m_fLastAux1;
    HHMRAcquireRec.Totals3Count = m_fLastAux2;

	char temp[32];
	char show[128];

    datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR2,temp);
    datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"");

	COleDateTime Now = COleDateTime::GetCurrentTime();
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_INQUIRETIME,Now.Format("%Y.%m.%d %H:%M:%S"));
	
	//skip Julian time aux byte for now
    datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATADATE4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATADATE3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATADATE2,temp);
	strcpy(temp,m_LastStart.Format("%Y.%m.%d"));
	strcpy(show,temp);
	strcat(show," ");
    datapage->SetDlgItemText(IDC_HHMRDATA_DATADATE1,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATATIME4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATATIME3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_DATATIME2,temp);

	strcpy(temp,m_LastStart.Format("%H:%M:%S"));
	strcat(show,temp);
//	strcat(show," Possible Tamper Indication");

    datapage->SetDlgItemText(IDC_HHMRDATA_DATATIME1,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_ETIME3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ETIME4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_ETIME2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ETIME3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_ETIME1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ETIME2,temp);

	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	double dValue;

	char Record[64];
	
	//get the duration
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = pAccumulator[0];
	uByteInt.bBytes[1] = pAccumulator[1];
	uByteInt.bBytes[0] = pAccumulator[2];

	sprintf(temp,"%6d   ",uByteInt.iValue);
	strcpy(Record,temp);

	uByteInt.bBytes[3] = pAccumulator[3];
	uByteInt.bBytes[2] = pAccumulator[4];
	uByteInt.bBytes[1] = pAccumulator[5];
	uByteInt.bBytes[0] = pAccumulator[6];
	dValue = uByteInt.iValue * 256;
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = NULL;
	uByteInt.bBytes[0] = pAccumulator[7];
	dValue += uByteInt.iValue;
	
	sprintf(temp,"%10.0f   ",dValue);
	strcat(Record,temp);

	uByteInt.bBytes[3] = pAccumulator[8];
	uByteInt.bBytes[2] = pAccumulator[9];
	uByteInt.bBytes[1] = pAccumulator[10];
	uByteInt.bBytes[0] = pAccumulator[11];
	dValue = uByteInt.iValue * 256;
	dValue *= 256;
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = pAccumulator[12];
	uByteInt.bBytes[0] = pAccumulator[13];
	dValue += uByteInt.iValue;
	
	sprintf(temp,"%10.0f   ",dValue);
	strcat(Record,temp);

	uByteInt.bBytes[3] = pAccumulator[14];
	uByteInt.bBytes[2] = pAccumulator[15];
	uByteInt.bBytes[1] = pAccumulator[16];
	uByteInt.bBytes[0] = pAccumulator[17];
	dValue = uByteInt.iValue * 256;
	dValue *= 256;
	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = pAccumulator[18];
	uByteInt.bBytes[0] = pAccumulator[19];
	dValue += uByteInt.iValue;
	
	sprintf(temp,"%10.0f",dValue);
	strcat(Record,temp);

	UpdateSummary(Record);
	
	
	//Start new parse algorithm
	char rates[sizeof(Record)];
	strncpy(rates,&Record[0],sizeof(Record));
	
	int i = 0;
	char parseValue[32];
	while (rates[i] != '\0')
	{
		//Skip white space
		while (rates[i] == ' ')
		{
			;
			i++;
		}
		int j = 0;
		while (rates[i] >= '0' && rates[i] <= '9')
		{
			parseValue[j] = rates[i];
			j++;
			i++;
		}
		//count time
		HHMRAcquireRec.ElapsedTime = atoi(parseValue);
		sprintf(temp, "%.1f", HHMRAcquireRec.ElapsedTime / 10.0);
		datapage->SetDlgItemText(IDC_HHMRDATA_ETIME1, temp);

		//totals
		while (rates[i] == ' ')
		{
			;
			i++;
		}
		j = 0;
		while (rates[i] >= '0' && rates[i] <= '9')
		{
			parseValue[j] = rates[i];
			j++;
			i++;
		}

		//Store totals
		HHMRAcquireRec.TotalsCount = atoi(parseValue);

		datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS3, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS4, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS2, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS3, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS1, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS2, temp);
		bool bDoingRaw = ((CButton*)(datapage->GetDlgItem(IDC_RADIO_RAW)))->GetCheck() == 1;
		if (HHMRAcquireRec.ElapsedTime <= 0)
			bDoingRaw = true;
		
		m_bZeroTransitionUp = false;
		m_bZeroTransitionDown = false;
		if ((HHMRAcquireRec.TotalsCount / (double)HHMRAcquireRec.ElapsedTime) < m_lTotalLThresh)
		{
			//set cstatic
			summarypage->SetDlgItemText(IDC_HHMRSUMMARY_ZEROVALUE, show);
			//set to red
			summarypage->SetColor(IDC_HHMRSUMMARY_ZEROVALUE, COLOR_RED);
			//if not currently in sequence below threshold
			if (!m_bRegisteredZero)
			{
				m_bZeroTransitionDown = true;
				m_bRegisteredZero = true;
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53420 STATUS CHANGE - RECEIVING TOTALS COUNT < LOW THRESHOLD");
			}
		}
		else
		{
			//if currently in sequence below threshold
			if (m_bRegisteredZero)
			{
				m_bZeroTransitionUp = true;
				m_bRegisteredZero = false;
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53430 STATUS CHANGE - RECEIVING TOTALS COUNT >= LOW THRESHOLD");
			}
		}

		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS1, parseValue);
		else
		{
			// Added * 10.0 to adjust for ElapsedTime in increments of 0.1 seconds
			sprintf(show, "%11.4e", 10.0 * (HHMRAcquireRec.TotalsCount / (double)HHMRAcquireRec.ElapsedTime));
			datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS1, show);
		}

		//get reals+accidentals
		datapage->GetDlgItemText(IDC_HHMRDATA_REALACC3, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_REALACC4, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_REALACC2, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_REALACC3, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_REALACC1, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_REALACC2, temp);

		//reals+accidentals
		while (rates[i] == ' ')
		{
			;
			i++;
		}
		j = 0;
		while (rates[i] >= '0' && rates[i] <= '9')
		{
			parseValue[j] = rates[i];
			j++;
			i++;
		}

		HHMRAcquireRec.RealAccCount = atoi(parseValue);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_HHMRDATA_REALACC1, parseValue);
		else
		{
			// Added * 10.0 to adjust for ElapsedTime in increments of 0.1 seconds
			sprintf(show, "%11.4e", 10.0 * (HHMRAcquireRec.RealAccCount));
			datapage->SetDlgItemText(IDC_HHMRDATA_REALACC1, show);
		}
		datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT3, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT4, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT2, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT3, temp);
		datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT1, temp, sizeof(temp));
		datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT2, temp);
		//accidentals
		while (rates[i] == ' ')
		{
			;
			i++;
		}
		j = 0;
		while (rates[i] >= '0' && rates[i] <= '9')
		{
			parseValue[j] = rates[i];
			j++;
			i++;
		}
		HHMRAcquireRec.AccidenCount = atoi(parseValue);
		double Reals = HHMRAcquireRec.RealAccCount - HHMRAcquireRec.AccidenCount;
		if (bDoingRaw)
			datapage->SetDlgItemInt(IDC_HHMRDATA_ACCIDENT1, (int)Reals);
		else
		{
			sprintf(show, "%11.4e", 10.0*(Reals / (double)HHMRAcquireRec.ElapsedTime));
			datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT1, show);
		}

		if (!bDoingRaw)
		{
			sprintf(show, "%11.4e", 10.0*((double)HHMRAcquireRec.Totals2Count / (double)HHMRAcquireRec.ElapsedTime));
			datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS1, show);

			sprintf(show, "%11.4e", 10.0* ((double)HHMRAcquireRec.Totals3Count / (double)HHMRAcquireRec.ElapsedTime));
			datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS1, show);
		}
		//consume trailing white space
		while (rates[i] == ' ')
		{
			;
			i++;
		}

	}

	/*strncpy(temp, &Record[0], 7);
	temp[7]=NULL;
	char*at = temp;
	char*i;
	char*j;
	while ((*at == ' ')&&(at<&temp[13]))at++;
	for(i=at,j=at;i<&temp[13];i++)
	{
		if(*i!=' ')
		{
			*j=*i;
			j++;
		}
	}
	*j=NULL;
    HHMRAcquireRec.ElapsedTime = atof(at);
	sprintf(temp,"%.1f",HHMRAcquireRec.ElapsedTime/10.0);
    datapage->SetDlgItemText(IDC_HHMRDATA_ETIME1,temp);
	
	//get totals
    datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS2,temp);
	strncpy(temp,&Record[10],13);
	temp[13]=NULL;
	at = temp;
	while ((*at == ' ')&&(at<&temp[13]))at++;
	for(i=at,j=at;i<&temp[13];i++)
	{
		if(*i!=' ')
		{
			*j=*i;
			j++;
		}
	}
	*j=NULL;
	bool bDoingRaw = ((CButton*)(datapage->GetDlgItem(IDC_RADIO_RAW)))->GetCheck()==1;
    if (HHMRAcquireRec.ElapsedTime <= 0)
		bDoingRaw = true;

    HHMRAcquireRec.TotalsCount = atof(at);

	m_bZeroTransitionUp = false;
	m_bZeroTransitionDown = false;
    if ((HHMRAcquireRec.TotalsCount/(double)HHMRAcquireRec.ElapsedTime) < m_lTotalLThresh)
	{
		//set cstatic
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_ZEROVALUE,show);
		//set to red
        summarypage->SetColor(IDC_HHMRSUMMARY_ZEROVALUE,COLOR_RED);
		//if not currently in sequence below threshold
		if (!m_bRegisteredZero)
		{
			m_bZeroTransitionDown = true;
			m_bRegisteredZero = true;
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53420 STATUS CHANGE - RECEIVING TOTALS COUNT < LOW THRESHOLD");
		}
	}
	else
	{
		//if currently in sequence below threshold
		if (m_bRegisteredZero)
		{
			m_bZeroTransitionUp = true;
			m_bRegisteredZero = false;
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53430 STATUS CHANGE - RECEIVING TOTALS COUNT >= LOW THRESHOLD");
		}
	}
	
	if (bDoingRaw)
        datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS1,at);
	else
	{
// Added * 10.0 to adjust for ElapsedTime in increments of 0.1 seconds
        sprintf(show,"%11.4e",10.0 * (HHMRAcquireRec.TotalsCount/(double)HHMRAcquireRec.ElapsedTime));
        datapage->SetDlgItemText(IDC_HHMRDATA_TOTALS1,show);
	}

	//get reals+accidentals
    datapage->GetDlgItemText(IDC_HHMRDATA_REALACC3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_REALACC4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_REALACC2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_REALACC3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_REALACC1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_REALACC2,temp);
	strncpy(temp,&Record[26],13);
	temp[13]=NULL;
	at=temp;
	while ((*at == ' ')&&(at<&temp[13]))at++;
	for(i=at,j=at;i<&temp[13];i++)
	{
		if(*i!=' ')
		{
			*j=*i;
			j++;
		}
	}
	*j=NULL;
	if (bDoingRaw)
        datapage->SetDlgItemText(IDC_HHMRDATA_REALACC1,at);
	else
	{
        sprintf(show,"%11.4e",10.0*(atof(at)/(double)HHMRAcquireRec.ElapsedTime));
        datapage->SetDlgItemText(IDC_HHMRDATA_REALACC1,show);
	}
    HHMRAcquireRec.RealAccCount = atof(at);

    datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT2,temp);
	strncpy(temp,&Record[42],13);
	temp[13]=NULL;
	at=temp;
	while ((*at == ' ')&&(at<&temp[13]))at++;
	for(i=at,j=at;i<&temp[13];i++)
	{
		if(*i!=' ')
		{
			*j=*i;
			j++;
		}
	}
	*j=NULL;
    HHMRAcquireRec.AccidenCount = atof(at);
    double Reals = HHMRAcquireRec.RealAccCount - HHMRAcquireRec.AccidenCount;
	if (bDoingRaw)
        datapage->SetDlgItemInt(IDC_HHMRDATA_ACCIDENT1,(int)Reals);
	else
	{
        sprintf(show,"%11.4e",10.0*(Reals/(double)HHMRAcquireRec.ElapsedTime));
        datapage->SetDlgItemText(IDC_HHMRDATA_ACCIDENT1,show);
	}

	if (!bDoingRaw)
	{
        sprintf(show,"%11.4e",10.0*((double)HHMRAcquireRec.Totals2Count/(double)HHMRAcquireRec.ElapsedTime));
        datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS1,show);

        sprintf(show,"%11.4e",10.0* ((double)HHMRAcquireRec.Totals3Count/(double)HHMRAcquireRec.ElapsedTime));
        datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS1,show);
	}
	*/

	bool dotrigger = false;
	if (strchr(m_cCameraRealorTotals,'R'))
	{
        if (m_lRealThresh < ((HHMRAcquireRec.RealAccCount-HHMRAcquireRec.AccidenCount)/HHMRAcquireRec.ElapsedTime))
			dotrigger = true;
	}

	if (strchr(m_cCameraRealorTotals,'T'))
	{
        if (m_lTotalThresh < (HHMRAcquireRec.TotalsCount/HHMRAcquireRec.ElapsedTime))
			dotrigger = true;
	}
	if (strchr(m_cCameraRealorTotals,'1'))
	{
        if (m_lTotal1Thresh < (HHMRAcquireRec.Totals2Count/HHMRAcquireRec.ElapsedTime))
			dotrigger = true;
	}

	if (dotrigger && m_bCameraDo)
	{
		m_pParent->PostMessage(uIDC_RECEIVE,TRIGGER_CAMERA_REQUEST,0);
        summarypage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,Now.Format("%Y.%m.%d %H:%M:%S Request"));
	}

    WriteToHHMRFile(&Jtemp,&HHMRAcquireRec);
    m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_T_TA_A,0);
}

//HHMR HV Record
void CHHMRPropertySheet::HVValue(BYTE * bAccumulator)
{
    CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
	char Record[8];
	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

	uByteInt.bBytes[3] = NULL;
	uByteInt.bBytes[2] = NULL;
	uByteInt.bBytes[1] = bAccumulator[0];
	uByteInt.bBytes[0] = bAccumulator[1];

	sprintf(Record,"%4d",uByteInt.iValue);

	UpdateSummary(Record);

	char temp[32];
	bool CurrentRed = false;
	bool CurrentLow = false;
	bool CurrentHigh= false;
	
	COleDateTime Now = COleDateTime::GetCurrentTime();
	strcpy(temp,Now.Format("%Y.%m.%d %H:%M:%S"));
	datapage->SetDlgItemText(IDC_HHMRDATA_HVTIME,temp);

	COleDateTime Base(1952,1,1,0,0,0);
	COleDateTimeSpan Difference = (m_LastStart - Base);
	Difference += HALF_SECOND;

	CJulianTime Julian;
	Julian.Init((unsigned)(Difference.GetTotalSeconds()));

	strncpy(temp,Record,4);
	temp[4] = NULL;
	
	//HIGH voltage
	m_dCurHV = atoi(temp);
	if (m_bFirstHV)
	{
		m_bFirstHV = false;
		m_dLowHV = m_dHighHV = m_dCurHV;
	}
	if (m_dCurHV < m_dLowHV)
	{
		CurrentLow = true;
		m_dLowHV = m_dCurHV;
	}
	else
		CurrentLow = false;
	if (m_dCurHV > m_dHighHV) 
	{
		CurrentHigh = true;
		m_dHighHV= m_dCurHV;
	}
	else
		CurrentHigh = false;

	if ((m_dCurHV < m_dHighVoltL) || (m_dCurHV > m_dHighVoltH))
	{
		CurrentRed = true;
		m_pParent->FlagOthr(1,true);			
        datapage->SetColor(IDC_HHMRDATA_HV,COLOR_RED);
        summarypage->SetColor(IDC_HHMRSUMMARY_HIGHVOLT2,COLOR_RED);
		if (m_bHighVoltOk)
 			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "53440 STATUS CHANGE - HIGH VOLTAGE OUT OF TOLERANCE (21)");
		m_bHighVoltOk = false;
		Data_Tab |= 0x0010;
	}
	else
	{
		CurrentRed = false;
		if (!m_bHighVoltOk)
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "53450 STATUS CHANGE - HIGH VOLTAGE IN TOLERANCE (21)");
		m_bHighVoltOk = true;
	}

	if (
		CurrentRed && CurrentLow &&
		((m_dLowHV <= m_dHighVoltL) || (m_dLowHV >= m_dHighVoltH))
	   )
	{
        datapage->SetColor(IDC_HHMRDATA_HV,COLOR_RED);
        summarypage->SetColor(IDC_HHMRSUMMARY_HIGHVOLT1,COLOR_RED);
	}

	if (
		CurrentRed && CurrentHigh &&
		((m_dHighHV <= m_dHighVoltL) || (m_dHighHV >= m_dHighVoltH))
	   )
	{
		summarypage->SetColor(IDC_HHMRSUMMARY_HIGHVOLT3,COLOR_RED);
	}

	sprintf(temp,"%d",m_dLowHV);
    summarypage->SetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT1,temp);
	sprintf(temp,"%d",m_dCurHV);
    summarypage->SetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT2,temp);
    datapage->SetDlgItemText(IDC_HHMRDATA_HV,temp);
	sprintf(temp,"%d",m_dHighHV);
    summarypage->SetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT3,temp);

	if (Data_Tab)
		SetTabColor(IDC_HHMRDATA_TAB,COLOR_RED);

    m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_HV,NULL); 
}

//HHMR AUX Record
void CHHMRPropertySheet::AuxRec(BYTE* bAccumulator)
{
	char Record[32];

	union UNNAMED {
		int iValue;
		BYTE bBytes[4];
	} uByteInt;

//	memcpy(uByteInt.bBytes,bAccumulator,4);

	uByteInt.bBytes[3] = bAccumulator[0];
	uByteInt.bBytes[2] = bAccumulator[1];
	uByteInt.bBytes[1] = bAccumulator[2];
	uByteInt.bBytes[0] = bAccumulator[3];

	m_fLastAux1 = uByteInt.iValue*256 + (BYTE)bAccumulator[4];

	uByteInt.bBytes[3] = bAccumulator[5];
	uByteInt.bBytes[2] = bAccumulator[6];
	uByteInt.bBytes[1] = bAccumulator[7];
	uByteInt.bBytes[0] = bAccumulator[8];

	m_fLastAux2 = uByteInt.iValue*256 + (BYTE)bAccumulator[9];

	char temp[32];

    CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
    datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS2,temp);
	sprintf(temp,"%.0f",m_fLastAux1);
	datapage->SetDlgItemText(IDC_HHMRDATA_AUX1TOTALS1,temp);


    datapage->GetDlgItemText(IDC_HHMRDATA_AUX2TOTALS3,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS4,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_AUX2TOTALS2,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS3,temp);
    datapage->GetDlgItemText(IDC_HHMRDATA_AUX2TOTALS1,temp,sizeof(temp));
    datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS2,temp);
	sprintf(temp,"%.0f",m_fLastAux2);
	datapage->SetDlgItemText(IDC_HHMRDATA_AUX2TOTALS1,temp);

	sprintf(Record,"%10.0f %10.0f",m_fLastAux1,m_fLastAux2);
	UpdateSummary(Record);

    m_pParent->PostMessage(uIDC_RECEIVE,HHMR_READ_AUX_SCALERS,NULL);
}

void
CHHMRPropertySheet::MakeMyPath(const char* path)
{
//	SHCreateDirectoryEx(m_hWnd,path,NULL);

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
//	_mkdir(path_buffer);
	CreateDirectory(path_buffer,NULL);
}

//this function should go into the parent class
void  CHHMRPropertySheet::BuildFileName(CString &cResult,
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

bool CHHMRPropertySheet::WriteToHHMRFile(
	CJulianTime* Time, 
	HHMRAcquireRecord* AcquireRec)
{
	FILE *File;
	fpos_t position;
	bool DoNewRecord;
	struct HHMRAcquireHeader HHMRAcquireHead;

	//get the summary page pointer and the data page pointer
	CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;

	//initialize to don't do a new record
	DoNewRecord = false;
	
	//bail out if we can't write to the disk
	if (m_pParent->m_bBlockWrite) return false;
	
	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentHHMRFileName,"%s\\%s%c%c%c%c%02d.%3s",
				((CHHMRInst*)m_pParent)->m_szSaveLoc,	
				((CHHMRInst*)m_pParent)->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,HMR_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"SR",
			m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,HMR_SUFFIX);
		strcpy(m_szCurrentHHMRFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (_access(m_szCurrentHHMRFileName,0)!=0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentHHMRFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				((CHHMRInst*)m_pParent)->m_szSaveLoc,	
				((CHHMRInst*)m_pParent)->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
	//			'0' + (Time->Year() % 10),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,HMR_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_pParent->m_csLongNameFormatString,cTemp,"SR",
				m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,HMR_SUFFIX);
		}
		//if the file is in the archive directory
		if (_access(ArchiveName,0)==0)
		{
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentHHMRFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			DoNewRecord = true;
		}
	}

	//repeatedly try to open the file but don't go beyond about 4 seconds
	//someone else may be reading the file
	//open for append binary
	int Attempts = 5;
	File = NULL;
	while ((Attempts >= 0) && (File == NULL))
	{
		File = fopen(m_szCurrentHHMRFileName,"ab+");
		if (File == NULL) 
		{
			Sleep(200);
		}
		Attempts--;
	}

	//if we managed to open the file 
	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		//we may be at the beginning of a new file (new day)
		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				char temp2[32];
				memset( &HHMRAcquireHead, ' ', sizeof(HHMRAcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(HHMRAcquireHeader)-4);
				memcpy(HHMRAcquireHead.SizeOfHeader,temp,4);

				sprintf(temp,"%7s",m_pParent->m_csVersion);
				//Remove the dots
				int j = 0;
				for (int i = 0; i < 7; ++i)
				{
					if (temp[i] != '.')
					{
						temp2[j] = temp[i];
						j++;
					}
				}
				temp2[j]='\0';
					
				//short sA,sB,sC,sD;
				// Were being typed from short to int, causing runtime error....... hn 4/18/2018
				//sscanf(m_pParent->m_csVersion,"%d.%d.%d.%d",(short*)&sA, (short*)&sB, (short*)&sC, (short*)&sD); // added typecast gjk
				//sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				memcpy(HHMRAcquireHead.Version,temp2,5);

				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(HHMRAcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time->Year()%100);
				memcpy(HHMRAcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time->Month());
				memcpy(HHMRAcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time->Day());
				memcpy(HHMRAcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time->Year());
				memcpy(HHMRAcquireHead.unused2,temp,4);
				//write header
				fwrite(&HHMRAcquireHead,sizeof(HHMRAcquireHeader),1,File);
				DoNewRecord = true;
				m_bRegisteredZero = false;
			}
		}
		fclose(File);

		//again open the file again only this time "update binary"
		Attempts = 80;
		File = NULL;
		while ((Attempts >= 0) && (File == NULL))
		{
			File = fopen(m_szCurrentHHMRFileName,"rb+");
			if (File == NULL) Sleep(250);
			Attempts--;
		}
		
		//if reals greater than real theshold
//		if ((AcquireRec->TotalsCount/AcquireRec->ElapsedTime) < m_lTotalLowThresh)
		if (m_bZeroTransitionUp || m_bZeroTransitionDown)
		{
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,(m_bZeroTransitionDown)?"T<Low T.Thres":"T>=Low T.Tresh");
			if (summarypage->m_bDebug)
				summarypage->SetDlgItemText(IDC_STATIC_NOTE2,(m_bZeroTransitionDown)?"T<Low T.Thres":"T>=Low T.Tresh");
			m_bZeroTransitionUp = false;
			m_bZeroTransitionDown = false;
			DoNewRecord = true;
			m_dBackgroundCount = 0;
			m_bOverThresh = true;
		}
		else if (((AcquireRec->RealAccCount - 
			AcquireRec->AccidenCount) / AcquireRec->ElapsedTime) >= m_lRealThresh)
		{
			//we want a new record written all the time we are above RealThresh rate
			DoNewRecord = true;
			//reset the background counter
			m_dBackgroundCount = 0;
			//over this threshold (used below to null out summed records
			m_bOverThresh = true;
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"R >= R Thresh");
			if (summarypage->m_bDebug)
				summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"R >= R Thresh");
		}
		else if ((AcquireRec->TotalsCount/AcquireRec->ElapsedTime) >= m_lTotalThresh)
		{
			//we want a new record written all the time we are above RealThresh rate
			DoNewRecord = true;
			//reset the background counter
			m_dBackgroundCount = 0;
			//over this threshold (used below to null out summed records
			m_bOverThresh = true;
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"T >= T Thresh");
			if (summarypage->m_bDebug)
				summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"T >= T Thresh");
		}
		else if ((AcquireRec->Totals2Count/AcquireRec->ElapsedTime) >= m_lTotal1Thresh)
		{
			//we want a new record written all the time we are above RealThresh rate
			DoNewRecord = true;
			//reset the background counter
			m_dBackgroundCount = 0;
			//over this threshold (used below to null out summed records
			m_bOverThresh = true;
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"T1 >= T1 Thresh");
			//if (summarypage->m_bDebug)
			//	summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"T1 >= T1 Thresh");
		}
		else
		{
			//if previous was an overthreshold then we need to force this one to 
			//start a new record
			if (m_bOverThresh)
				DoNewRecord = true;
			//we can now reset the overthreshold to false
			m_bOverThresh = false;
			//if this backbround is the first than we need to do a new record
			if ((++m_dBackgroundCount)==1)
			{
				DoNewRecord = true;
				datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"New Record");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"First BG Data");
			}
			else
			{
				datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"New Record");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"Not First BG Data");
			}
		}
		
		//do a new record if we have reached the maximum number of compressed records
		if (m_dCompressCount >= m_dMaxCompress)
		{
			DoNewRecord = true;
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,(m_dCompressCount >= 999999)?"New Record":"Max Compr Met");
			if (summarypage->m_bDebug)
				summarypage->SetDlgItemText(IDC_STATIC_NOTE2,(m_dCompressCount >= 999999)?"New Record":"Max Compr Met");
		}

		//is Sigmas > sigma thresh
		if (!DoNewRecord)
		{
			//do a new record if totals is above threshold
			double TRate = AcquireRec->TotalsCount / AcquireRec->ElapsedTime;
			double TRateSigma =  sqrt(AcquireRec->TotalsCount) / AcquireRec->ElapsedTime;
			double SumTRate = SumAcquireRec.TotalsCount / SumAcquireRec.ElapsedTime;
			double SumTRateSigma = sqrt(SumAcquireRec.TotalsCount) / SumAcquireRec.ElapsedTime;
			double Sigmas = fabs(TRate - SumTRate) / sqrt(TRateSigma*TRateSigma + SumTRateSigma*SumTRateSigma);
			if (Sigmas > m_lSigmaThresh)
			{
				DoNewRecord = true;
				m_bOverThresh = true;
				datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"Over Sigma");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE4,"Over Sigma");
			}
			else
			{
				datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"New Record");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE4,"New Not Over Thr");
			}
		}
		else
		{
			if (summarypage->m_bDebug)
				summarypage->SetDlgItemText(IDC_STATIC_NOTE4,"No Over Thr check");
		}

		if (DoNewRecord)
		{
//			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,"New Rec");
			//if overthreshold 'cause of RealThreshold Rate or because of 
			//the sigma test then we don't want to save the record
			if (m_bOverThresh)
			{
				memset(&SumAcquireRec, 0x00, sizeof(HHMRAcquireRecord));
				//we have no records in the sumacquirerec
				m_dCompressCount = 0;
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Did new Cleared Old");
			}
			else
			{
				memcpy(&SumAcquireRec,AcquireRec,sizeof(HHMRAcquireRecord));
				//we only have one record in the sumacquirerec
				m_dCompressCount = 1;
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Did new Saved Old");
			}


			//send record to end of file
			fseek(File,0,SEEK_END);
			if (fwrite(AcquireRec,sizeof(HHMRAcquireRecord),1,File)!= 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(100);
				if (fwrite(AcquireRec,sizeof(HHMRAcquireRecord),1,File)!=1)
				{
					//if failed it could be because MicXfer is trying to
					//clear the archive bit, so wait a short time and try
					//again.
					Sleep(200);
					if (fwrite(AcquireRec,sizeof(HHMRAcquireRecord),1,File)!=1)
					{
						//if failed on write then show a file write problem
						//on the instrument's button
						m_pParent->m_pButton->SetDiskIcon_Hourglass();
						m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
						m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
						WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53470 Could not write to HHMR file");
						//close the file anyway
						fclose(File);
						//return write failure 
						return false;
					}
				}
			}
		}
		else //do not do a new record
		{
			CString temp;
			m_dCompressCount++;
			temp.Format("Filtered: %d",m_dCompressCount);
			datapage->SetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,temp);
			
			SumAcquireRec.AccidenCount	+= AcquireRec->AccidenCount;
			SumAcquireRec.ElapsedTime	+= AcquireRec->ElapsedTime;
			SumAcquireRec.RealAccCount	+= AcquireRec->RealAccCount;
			SumAcquireRec.TotalsCount	+= AcquireRec->TotalsCount;
			SumAcquireRec.Totals2Count	+= AcquireRec->Totals2Count;
			SumAcquireRec.Totals3Count	+= AcquireRec->Totals3Count;
			fseek( File, -(signed)sizeof(HHMRAcquireRecord), SEEK_END);
			//send record to file
			if (summarypage->m_bDebug)
			{
				CString tempstring;
				tempstring.Format("Adding to rec: %d",m_dCompressCount);
				summarypage->SetDlgItemText(IDC_STATIC_NOTE5,tempstring);
			}
			
			if (fwrite(&SumAcquireRec,sizeof(HHMRAcquireRecord),1,File)!= 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(50);
				if (fwrite(&SumAcquireRec,sizeof(HHMRAcquireRecord),1,File)!=1)
				{
					//if failed it could be because MicXfer is trying to
					//clear the archive bit, so wait a short time and try
					//again.
					Sleep(200);
					if (fwrite(&SumAcquireRec,sizeof(HHMRAcquireRecord),1,File)!=1)
					{
						//if failed on write then show a file write problem
						//on the instrument's button
						m_pParent->m_pButton->SetDiskIcon_Hourglass();
						m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
						m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
						WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53480 Could not write to HHMR file");
						//close the file anyway
						fclose(File);
						//return write failure 
						return false;
					}
				}
			}
		}
		//close file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pParent->m_pButton->SetDiskIcon_Hourglass();
		m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53490 Could not open HHMR file");
		if (summarypage->m_bDebug)
			summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Error Open HHMR");
	}
	return false;
}


void CHHMRPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg)
{
	FILE *File;
	char Message[512];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime();
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_pParent->m_bBlockWrite) 
		return;

	//default to the provided HHMR time
	if (Time)
	{
		int Year,Month,Day,Hour,Minute,Second;
		Year = Time->Year();
		Month = Time->Month();
		Day = Time->Day();
		Hour = Time->Hour();
		Minute = Time->Minute();
		Second = Time->Second();
		if (Year	< 100 )	Year	= 1900;
		if (Year	> 9999)	Year	= 9999;
		if (Month	< 1)	Month	= 1;
		if (Month	> 12)	Month	= 12;
		if (Day		< 1)	Day		= 1;
		if (Day		> 31)	Day		= 31;
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
				((CHHMRInst*)m_pParent)->m_szSaveLoc,	
				((CHHMRInst*)m_pParent)->m_pID,
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
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(
				cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"SR",
				m_pParent->m_pID,
				TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");

			strcpy(m_szCurrentFileName,cResult);
		}
	}
	else
	{
		//	build new file name and save it
		if (m_pParent->m_bUseShortFilename)
		{
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				((CHHMRInst*)m_pParent)->m_szSaveLoc,	
				((CHHMRInst*)m_pParent)->m_pID,
				((TG.GetYear()-1990)<10)?
					((TG.GetYear()-1990<0)?'#':'0'+(TG.GetYear()-1990)):
					'A'+(TG.GetYear()-2000),
	//			'0' + (TG.GetYear() % 10),
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

			BuildFileName(
				cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"SR",
				m_pParent->m_pID,
				TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,".");

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
		//	message = Time::YY.MM.DD HH:MM:SS J (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?" ":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?" ":Msg);
		break;

	case TYPE_NEWDAY:
		//	message = Time::YY.MM.DD HH:MM:SS I New Day, Setup= (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I %s = Setup, New Day",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n":Msg);
		break;


	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS HHMR Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - I = xxxx\r"
		//HHMR time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 53500 HHMR Time %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - I = %.0f seconds",
			TG.GetYear(),TG.GetMonth(),TG.GetDay(),TG.GetHour(),TG.GetMinute(),TG.GetSecond(),
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 53510 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C HHMR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53520 HHMR COLLECT Version %s started.",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C HHMR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53530 HHMR COLLECT Version %s started from abnormal shutdown.",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53540 File %s deleted.",
		TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53550 Unknown TYPE %s",
			TC.GetYear(),	
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),		TC.GetMinute(),	TC.GetSecond(),
			Msg);
	}

	//if to dmp do the write to todays file and get out
	if (ToWhom == TO_DMP)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += DMP_SUFFIX;
//		CString ArchiveFileNameEx = ArchiveFileName + DMP_SUFFIX;
		if (_access(fn,0) != 0)
		{
//			if (_access(ArchiveFileNameEx,0)!=-1)
//				MoveFileEx(ArchiveFileNameEx,fn,
//					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			//make sure path exists
			m_pParent->MakeMyPath(fn);
		}
		
		if ((File = fopen(fn,"at+")) != NULL)
		{
			//	send string
			fprintf(File,"%s\n",Message);
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
			m_pParent->MakeMyPath(fn);

			if (m_pParent->m_bUseShortFilename)
			{
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				((CHHMRInst*)m_pParent)->m_szSaveLoc,
				((CHHMRInst*)m_pParent)->m_pID,
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
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = ((CHHMRInst*)m_pParent)->m_szSaveLoc;
				cTemp += "\\archive\\";

				BuildFileName(
					ArchiveFileName,
					m_pParent->m_csLongNameFormatString,
					cTemp,
					"SR",
					m_pParent->m_pID,
					TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
			}
			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position);
			if (position==0)
			{
//d				m_bStartOfFile	= true;
//				m_bBatteryOk	= true;
//				m_bBatteryTolOk	= true;
//				m_bP15VoltOk	= true;
//				m_bM15VoltOk	= true;
//				m_bACPowerOk	= true;
//				m_bACPowerLoss	= false;
//				m_bBBMOk		= true;
//				m_bCountRateTamperOk = true;
//				m_bRMSSigmaTamperOk  = true;
//				m_bInMII		= false;
				m_bDataFiltered	= false;
//				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}

			//	send string
			fprintf(File,"%s\n",Message);
			//	close cev
			fclose(File);
		}
	}

	//if to pfm
	if (ToWhom & TO_PFM)
	{
		//	open filename+cev
		CString fn(m_szCurrentFileName);
		fn += PFM_SUFFIX;

		CString ArchiveFileNameExt = ArchiveFileName + PFM_SUFFIX;

		//if fn does not exist (it may have been moved or we have a new day)
		if (_access(fn,0) != 0)
		{
			//make sure path exists
			m_pParent->MakeMyPath(fn);

			//check if it is in the archive directory
			//build file name
			if (m_pParent->m_bUseShortFilename)
			{
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				((CHHMRInst*)m_pParent)->m_szSaveLoc,
				((CHHMRInst*)m_pParent)->m_pID,
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
				BuildFileName(ArchiveFileName,
					m_pParent->m_csLongNameFormatString,//g_cLongName,
					cTemp,"SR",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,PFM_SUFFIX);
			}
			//if it exists in the subdirectory "archive" then move it and use it
			if (_access(ArchiveFileName,0)==0)
				MoveFileEx(ArchiveFileName,fn,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
		}

		if ((File = fopen(fn,"at+")) != NULL)
		{
			//if new file do stuff
			fseek(File,0,SEEK_END);
			fgetpos( File, &position);
			if (position==0)
			{
//d				m_bStartOfFile	= true;
//				m_bBatteryOk	= true;
//				m_bBatteryTolOk	= true;
//				m_bP15VoltOk	= true;
//				m_bM15VoltOk	= true;
//				m_bACPowerOk	= true;
//				m_bACPowerLoss	= false;
//				m_bBBMOk		= true;
//				m_bCountRateTamperOk = true;
//				m_bRMSSigmaTamperOk  = true;
//				m_bInMII		= false;
				m_bDataFiltered	= false;
//				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send string
			fprintf(File,"%s\n",Message);
			//	close pfm
			fclose(File);
		}
	}

}


int CHHMRPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCloseButton = new CColorButton(false, m_pParent->m_szIniFile);
	m_pPauseButton = new CButton();
	m_pAckFlagsButton = new CButton();
	m_pPrintButton = new CButton();
	m_pToClose = new CButton();
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);

	return 0;
}

void CHHMRPropertySheet::OnCloseButton()
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CHHMRPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if (Tab != IDC_HHMRDATA_TAB) 
		return;

	int ColorIndex;
	int tab = 0;

	switch(Color) 
	{
	case COLOR_RED:
		ColorIndex = 0;
		break;
	case COLOR_GREEN:
		ColorIndex = 1;
		break;
	case COLOR_BLUE:
		ColorIndex = 2;
		break;
	case COLOR_YELLOW:
		ColorIndex = 3;
		break;
	case COLOR_CYAN:
		ColorIndex = 4;
		break;
	case COLOR_MAGENTA:
		ColorIndex = 5;
		break;
	case COLOR_WHITE:
		ColorIndex = 6;
		break;
	case COLOR_BLACK:
		ColorIndex = 7;
		break;
	default:
		ColorIndex = 6;
	};
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = ColorIndex;
	switch (Tab) 
	{
		case IDC_HHMRDATA_TAB:    tab = 1; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}

void CHHMRPropertySheet::OnAckButton()
{
	m_dToClose = 0;

	//acknowledge the four button flags
	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);

	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	//acknowledge the checksum error

	CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;

	summarypage->ResetColors();
	datapage->ResetColors();

	SetTabColor(IDC_HHMRDATA_TAB,COLOR_GREEN);
	Data_Tab = 0;
	Invalidate();
}

void CHHMRPropertySheet::OnPauseButton()
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

void CHHMRPropertySheet::IssuePrint(FILE* outfile)
{
	CHHMRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CHHMRDataStat* datapage    = m_pParent->m_pDataStatus;
	CHHMRModParam* modparpage  = m_pParent->m_pModParams;
	//CHHMRConfig2*  camerapage  = m_pParent->m_pCamera;

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"* = Item is displayed RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"SUMMARY\n");

	fprintf(outfile," Data Status\n");
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_HHMRSUMMARY_HIGHVOLT1-IDC_HHMRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_HHMRSUMMARY_HIGHVOLT2-IDC_HHMRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_HIGHVOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_HHMRSUMMARY_HIGHVOLT3-IDC_HHMRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"     High Voltage (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(),temp2.GetString(),temp3.GetString());
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_INQUIRETIME,temp1);		
	fprintf(outfile,"      Last Data Status(Y.M.D H:M:S): %s\n",temp1.GetString());

	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_CONFIGURATION,temp1);	
	fprintf(outfile,"   MIC vs. Instrument Configuration: %s\n",temp1.GetString());

	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp1);		
	fprintf(outfile,"   Last Camera Trigger(Y.M.D H:M:S): %s\n",temp1.GetString());

	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_ZEROVALUE,temp1);
	fprintf(outfile,"      Totals Below LOW Threshold at: %s\n",temp1.GetString());

	fprintf(outfile,"  Recent Commands and Responses:\n");

	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND13,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND12,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND11,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND10,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND9,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND8,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND7,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND6,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND5,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND4,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND3,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND2,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_HHMRSUMMARY_COMMAND1,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));

	if(((CButton*)summarypage->GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Allow click on Recent Msg: %s\n",temp.GetString());

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"DATA STATUS\n");
	fprintf(outfile," Most Recent Response To Read Status\n");
	datapage->GetDlgItemText(IDC_HHMRDATA_STATUSTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_STATUSA,temp);
	fprintf(outfile,"             Stopped\\Started: %s\n",temp.GetString());

	fprintf(outfile," Most Recent Response To Read Setup\n");
	datapage->GetDlgItemText(IDC_HHMRDATA_SETUPTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_SETUP_PRE,temp);
	fprintf(outfile,"                  Pre-delay: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_SETUP_HV,temp);
	fprintf(outfile,"                  HV Set At: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_SETUP_COUNT,temp);
	fprintf(outfile,"                 Count Time: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_SETUP_GATE,temp);
	fprintf(outfile,"                 Gate Width: %s\n",temp.GetString());

	fprintf(outfile," Most Recent Response To Read HV\n");
	datapage->GetDlgItemText(IDC_HHMRDATA_HVTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_HHMRDATA_HV,temp);
	fprintf(outfile,"                High Voltage: %s\n",temp.GetString());

	fprintf(outfile," Most Recent ACQUIRE Records Received\n");


	if(((CButton*)datapage->GetDlgItem(IDC_RADIO_RAW))->GetCheck()==0)
		temp = "Show Rates";
	else
		temp = "Show Raw";
	fprintf(outfile,"                             %s\n",temp.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATADATE4,temp4);
	fprintf(outfile,"  Date of Data Record (Y/M/D): %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_DATATIME4,temp4);
	fprintf(outfile,"  Time of Data Record (H:M:S): %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR1,temp1);		
	datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR2,temp2);		
	datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR3,temp3);		
	datapage->GetDlgItemText(IDC_HHMRDATA_FILTERFACTOR4,temp4);		
	fprintf(outfile,"                Filter Factor: %11s %11s %11s %11s\n",	
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_TOTALS4,temp4);
	fprintf(outfile,"                       Totals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_AUX1TOTALS4,temp4);
	fprintf(outfile,"             Auxillary Totals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_REALACC1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_REALACC2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_REALACC3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_REALACC4,temp4);
	fprintf(outfile,"          Reals + Accidentals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_ACCIDENT4,temp4);
	fprintf(outfile,"                  Accidentals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_HHMRDATA_ETIME1,temp1);
	datapage->GetDlgItemText(IDC_HHMRDATA_ETIME2,temp2);
	datapage->GetDlgItemText(IDC_HHMRDATA_ETIME3,temp3);
	datapage->GetDlgItemText(IDC_HHMRDATA_ETIME4,temp4);
	fprintf(outfile,"                 Elapsed Time: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp.GetString(),temp1.GetString());
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station/File ID: %s\n",temp.GetString());
	if(((CButton*)modparpage->GetDlgItem(IDC_HHMRMODPARAMS_LOGFILTER))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Log Data Filtering Msgs: %s\n",temp.GetString());
	if(((CButton*)modparpage->GetDlgItem(IDC_HHMRMODPARAMS_DODUMP))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";

	fprintf(outfile," Error Limits\n");
	fprintf(outfile,"                       High     Low\n");
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_HIGHVOLTH,temp);
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_HIGHVOLTL,temp1);
	fprintf(outfile,"       High Voltage: %6s  %6s\n",temp.GetString(),temp1.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_MAX_TIMEOUT,temp);	
	fprintf(outfile,"# Timeouts before fail: %s\n",temp.GetString());	 // was %d changed to %s gjk 		

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_SLOW_RESET,temp);
	fprintf(outfile,"  Slow Reset (Sec.): %6s\n\n",temp.GetString());

	fprintf(outfile," General\n");
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp.GetString());

	fprintf(outfile," Data Compression Rates\n");

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_TOTALSLTHRESH,temp);
	fprintf(outfile,"      LOW Totals Threshold: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_TOTALSTHRESH,temp);
	fprintf(outfile,"          Totals Threshold: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_REALSTHRESH,temp);
	fprintf(outfile,"           Reals Threshold: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_TOTALS1THRESH,temp);
	fprintf(outfile,"      Aux Totals Threshold: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_SIGMATHRESH,temp);
	fprintf(outfile," Sigma Threshold 0.1 Units: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_MAXCOMPRESS,temp);
	fprintf(outfile,"       Maximum Data Points: %s\n",temp.GetString());

	fprintf(outfile," HHMR Configuration\n");
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_CT_UNITS,temp);
	fprintf(outfile,"    Count Time (0.1 seconds): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_PREDELAY,temp);
	fprintf(outfile,"           Pre-Delay (uSec.): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_GATEWIDTH,temp);
	fprintf(outfile,"          Gate Width (uSec.): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_HHMRMODPARAMS_HIGHV,temp);
	fprintf(outfile,"        High Voltage (Volts): %s\n",temp.GetString());

	/*fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"CAMERA SETUP -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Camera Connection\n");
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERAPORT1))->GetCheck()==1)
		temp = "PORT1";
	else if	(((CButton*)camerapage->GetDlgItem(IDC_CAMERAPORT2))->GetCheck()==1)
		temp = "PORT2";
	else if (((CButton*)camerapage->GetDlgItem(IDC_CAMERAPORT3))->GetCheck()==1)
		temp = "PORT3";
	else 
		temp = "NONE";
	fprintf(outfile,"  Parallel Printer Port: %s\n",temp.GetString());
	fprintf(outfile,"             Pin Number: %d\n",camerapage->GetDlgItemInt(IDC_CAMERAPIN));
	fprintf(outfile," Camera Trigger Configuration\n");
	temp = "";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGREALS))->GetCheck()==1)
		temp += "Reals ";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGTOTALS))->GetCheck()==1)
		temp += "Totals ";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGTOTALS1))->GetCheck()==1)
		temp += "Totals 1";
	fprintf(outfile,"                   Trigger on: %s\n",temp.GetString());
	
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT1,temp1);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT2,temp2);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT3,temp3);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT4,temp4);
	temp = temp1 +":"+ temp2 +":"+ temp3 +":"+ temp4;
	fprintf(outfile,"      Trigger Delay (D:H:M:S): %s\n",temp.GetString());

	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT5,temp1);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT6,temp2);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT7,temp3);
	camerapage->GetDlgItemText(IDC_HHMRCONFIG_EDIT8,temp4);
	temp = temp1 +":"+ temp2 +":"+ temp3 +":"+ temp4;
	fprintf(outfile,"   Trigger Suppress (D:H:M:S): %s\n",temp.GetString());*/

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"CURRENT CONFIGURATION (from INI file)\n");
	char buff[_MAX_PATH];

	char *inifile = m_pParent->m_szIniFile;

	GetPrivateProfileString(m_pParent->m_pName,"HIGHVOLTH","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  HIGHVOLTH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"HIGHVOLTL","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  HIGHVOLTL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SLOW_RESET","",buff,sizeof(buff),inifile);
	fprintf(outfile," SLOW_RESET=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName, NODE ,"",buff,sizeof(buff),inifile);
	fprintf(outfile,"       NODE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXCYCLE","",buff,sizeof(buff),inifile);
	fprintf(outfile,"   MAXCYCLE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXPAUSE","",buff,sizeof(buff),inifile);
	fprintf(outfile,"   MAXPAUSE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"DO_DUMP","",buff,sizeof(buff),inifile);
	fprintf(outfile,"    DO_DUMP=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"LOG_FILTER","",buff,sizeof(buff),inifile);
	fprintf(outfile," LOG_FILTER=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"PORT","",buff,sizeof(buff),inifile);
	fprintf(outfile,"       PORT=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FILE_ID,"",buff,sizeof(buff),inifile);
	fprintf(outfile,"    FILE_ID=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,SAVE_LOC,"",buff,sizeof(buff),inifile);
	fprintf(outfile,"  SAVE_LOC=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"FLAGCOMM","",buff,sizeof(buff),inifile);
	fprintf(outfile," FLAGCOMM=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"FLAGOTHR","",buff,sizeof(buff),inifile);
	fprintf(outfile," FLAGOTHR=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"COUNTTIMEUNIT","",buff,sizeof(buff),inifile);
	fprintf(outfile," COUNTTIMEUNIT=%s\n",buff);
//	GetPrivateProfileString(m_pParent->m_pName,"COUNTTIMEEXP","",buff,sizeof(buff),inifile);
//	fprintf(outfile," COUNTTIMEEXP=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"PREDELAY","",buff,sizeof(buff),inifile);
	fprintf(outfile," PREDELAY=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"GATEWIDTH","",buff,sizeof(buff),inifile);
	fprintf(outfile," SUP-GATEWIDTH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"HIGHV","",buff,sizeof(buff),inifile);
	fprintf(outfile," HIGHV=%s\n",buff);

	GetPrivateProfileString(m_pParent->m_pName,"REALSTHRESH","",buff,sizeof(buff),inifile);
	fprintf(outfile," REALSTHRESH=%s\n",buff);

	GetPrivateProfileString(m_pParent->m_pName,"TOTALSTHRESH","",buff,sizeof(buff),inifile);
	fprintf(outfile," TOTALSTHRESH=%s\n",buff);

	GetPrivateProfileString(m_pParent->m_pName,"TOTALS1THRESH","",buff,sizeof(buff),inifile);
	fprintf(outfile," TOTALS1THRESH=%s\n",buff);

	GetPrivateProfileString(m_pParent->m_pName,"SIGMATHRESH","",buff,sizeof(buff),inifile);
	fprintf(outfile," SIGMATHRESH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXCOMPRESS","",buff,sizeof(buff),inifile);
	fprintf(outfile," MAXCOMPRESS=%s\n",buff);
	/*GetPrivateProfileString(m_pParent->m_pName,"CAMERA_PORT","NONE",buff,sizeof(buff),inifile);
	fprintf(outfile," CAMERA_PORT=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"CAMERA_PIN","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  CAMERA_PIN=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"CAMERA_TRIGGER_TYPE","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  CAMERA_TRIGGER_TYPE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"CAMERA_TRIGGER_HOLDOFF","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  CAMERA_TRIGGER_HOLDOFF=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"CAMERA_TRIGGER_SUPPRESS","",buff,sizeof(buff),inifile);
	fprintf(outfile,"  CAMERA_TRIGGER_SUPPRESS=%s\n",buff);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());*/
}

void CHHMRPropertySheet::OnPrintButton()
{
	FILE* outfile;
	char *at;
	while ((at = strchr(m_pCaption,':'))!=NULL) at[0] = '_';

	m_dToClose = 0;

	CFileDialog FileDialog(FALSE,"txt",m_pCaption,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);

	if (FileDialog.DoModal()!=IDCANCEL)
	{
		CString filename = FileDialog.GetPathName();
		outfile = fopen(filename,"w");
		m_pParent->IssuePrint(outfile);
		fclose(outfile);
	}
}

void CHHMRPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
	}
	else
		KillTimer(1);
}

void CHHMRPropertySheet::OnToCloseButton()
{
	char temp[32];
	m_dToClose -= BUMP_EXTEND;  //Bump_extend = 300 seconds = 5 more minutes

    int seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;

	if (seconds_to_go > MAX_EXTEND) //MAX_EXTEND = 7200 = 120 minutes
	{
		m_dToClose   = (m_pParent->m_iDlgCloseMilliseconds/1000) - MAX_EXTEND;
		seconds_to_go = (m_pParent->m_iDlgCloseMilliseconds/1000) - m_dToClose;
	}

	int minutes = seconds_to_go / 60;
	int seconds = seconds_to_go % 60;

	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
	m_pToClose->SetWindowText(temp);
}

LRESULT CHHMRPropertySheet::OnExtendClose(WPARAM, LPARAM)
{
	m_dToClose = 0;
	return 0;
}


void CHHMRPropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		int idlg_close_seconds = m_pParent->m_iDlgCloseMilliseconds/1000;

		if (m_dToClose >= idlg_close_seconds)
		{
			m_dToClose = 0;
			ShowWindow(SW_HIDE);
			KillTimer(1);
		} 
		else
		{
			char temp[32];
			int minutes = (idlg_close_seconds - m_dToClose)/60;
			int seconds = (idlg_close_seconds - m_dToClose) % 60;
			sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);
			m_pToClose->SetWindowText(temp);
			m_dToClose++;
		}
	}
	CPropertySheet::OnTimer(nIDEvent);
}

void CHHMRPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it

	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}


BOOL CHHMRPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	CRect WindowRect, FinalRect;

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

	//create the colored close button

	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	m_pCloseButton->Create( "Close", dwStyle,
		  CRect(Interspace, Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	//jsrs don't have bbm so turn display off
	m_pCloseButton->SetPreNum("",false);
	m_pCloseButton->SetBase(0,false);

	//create the pause/begin button
	m_pPauseButton->Create(COLLECTINGBEGIN, dwStyle,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	//create the ack button
	m_pAckFlagsButton->Create("Acknowledge Flags",dwStyle,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	//create the snapshot button
	m_pPrintButton->Create("Snapshot Status\nTo File",dwStyle,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

	//create the autoclose button
	char temp[32];
	int minutes = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose)/60;
	int seconds   = (m_pParent->m_iDlgCloseMilliseconds/1000-m_dToClose) % 60;
	sprintf(temp,"Extend Auto\nClose: %d:%02d",minutes,seconds);

	m_pToClose->Create(temp,dwStyle,
		  CRect(5*Interspace + 4*ButtonWidth,Y1,
				5*Interspace + 5*ButtonWidth,Y2),
		  this,
		  ID_TOCLOSE);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND);
		pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND);
	}
	
	//initialize the sum acquire record
	memchr(&SumAcquireRec,NULL,sizeof(SumAcquireRec));
	
	//initialize the count of the number of records in the sum acquire record
	m_dCompressCount = 0;

	//set the image list for the tabs
	//for the jsr only the second tab(#1) has an icon
	GetTabControl( )->SetImageList(m_pImageList);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);

	return bResult;
}

BOOL CHHMRPropertySheet::DestroyWindow() 
{
	//destroy the imagelist
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CHHMRPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

CString *CHHMRPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}