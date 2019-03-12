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
// JSRSafePS.cpp : implementation file
//

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "JSRSafePS.h"
#include "JSRInst.h"
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
// CJSRPropertySheet

IMPLEMENT_DYNAMIC(CJSRPropertySheet, CPropertySheet)

CJSRPropertySheet::CJSRPropertySheet(
	UINT nIDCaption, 
	CJSRInst* pParentWnd) 
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	FinishConstruction();
}

CJSRPropertySheet::CJSRPropertySheet(
	LPCTSTR pszCaption, 
	CJSRInst* pParentWnd) 
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	FinishConstruction();
}

void CJSRPropertySheet::FinishConstruction()
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
	m_fLastAux = 0.0;
	m_fInquireInt = 0.0;

	m_bRegisteredZero = false;
	m_bZeroTransitionUp = false;
	m_bZeroTransitionDown = false;

	m_PreviousSetup.SetStatus(COleDateTime::null);
	strcpy(m_cCameraRealorTotals,"RT1");
//	m_lCameraThreshold = 50.000;

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;

	//m_LastStart.SetDateTime(1952,1,1,0,0,0);
	m_LastStart = COleDateTime::GetCurrentTime();
}

CJSRPropertySheet::~CJSRPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}

COLORREF CJSRPropertySheet::SetButtonColor(COLORREF color)
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

/*void CJSRPropertySheet::SetCamera()
{
	char buff[16];
	//m_pParent->SetCamera(true);
	GetPrivateProfileString(
		m_pParent->m_pName,
		"CAMERA_PORT","NONE",buff,sizeof(buff),m_pParent->m_szIniFile);
	buff[0] = (char)toupper(buff[0]);
	m_bCameraDo = (buff[0] == 'P');
}*/

void CJSRPropertySheet::SetLimits(
		double lRealThresh,
		double lTotalThresh,
		double lTotal1Thresh,
		double lTotalLThresh,
		double lSigmaThresh,
		int	dMaxCompress,
		int	dCountTimeUnit,
		int dCountTimeExp,
		int	dHighV,
		int	dGateWidth,
		int	dPreDelay,
		int	dHighVoltH, int dHighVoltL,
	//	char cCameraRealorTotals[4],
		//bool bCameraDo,
		char * pId)
{	
	m_lRealThresh  = lRealThresh;
	m_lTotalThresh  = lTotalThresh;
	m_lTotal1Thresh  = lTotal1Thresh;
	m_lTotalLThresh  = lTotalLThresh;
	m_lSigmaThresh = lSigmaThresh;
	m_dMaxCompress = dMaxCompress;
	m_dCountTimeUnit = dCountTimeUnit;
	m_dCountTimeExp = dCountTimeExp;
	m_dHighV = dHighV;
	m_dGateWidth = dGateWidth;
	m_dPreDelay = dPreDelay;
	m_dHighVoltH = dHighVoltH;
	m_dHighVoltL = dHighVoltL;
	//strcpy(m_cCameraRealorTotals,cCameraRealorTotals);
	//m_bCameraDo = bCameraDo;
	strcpy(m_szCurrentID,pId);
}




BEGIN_MESSAGE_MAP(CJSRPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CJSRPropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
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
// CJSRPropertySheet message handlers
void CJSRPropertySheet::UpdateSummary(char *Message)
{
	CString Data;
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND12,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND13,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND11,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND12,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND10,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND11,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND9,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND10,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND8,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND9,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND7,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND8,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND6,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND7,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_COMMAND1,Message);

}

LRESULT CJSRPropertySheet::OnReceiveOOB(WPARAM /*WParam*/, LPARAM LParam)
{
	CoTaskMemFree((void *)LParam);
	return 0;
}

LRESULT CJSRPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
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
		if (cbuff[index] == NULL) 
		{
			m_dAt = 0;
		}
		else
		{
			m_pAccumulator[m_dAt] = (char)cbuff[index];
			if	(
				(m_dAt>=1) &&
				(
		 		((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) ||
				((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1))
				)
			) 
			{
//				short message_type = UNKNOWN_REC;
				char* cbuff2;
				cbuff2 = (char*)malloc(m_dAt+2);
				memmove(cbuff2,m_pAccumulator,m_dAt+1);
				cbuff2[m_dAt+1] = NULL;
				char cBuffx[2048];

				if (m_pParent->m_bDoDump)
				{
					strcpy(cBuffx,"53390 ");
					strcat(cBuffx,cbuff2);
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuffx);//dump file
				}

				UpdateSummary(cbuff2);
//				int j = strlen(cbuff2);
				switch(strlen(cbuff2))
				{
				case JSR_STATUS_RECORD_LENGTH:	StatusRec(cbuff2);	break;
				case JSR_STATUS_RECORD_ERR_LENGTH: StatusRecErr(cbuff2);break;
				case JSR_HV_RECORD_LENGTH:		HighVRec(cbuff2);	break;
				case JSR_VERSION_RECORD_LENGTH:	VersionRec(cbuff2);	break;
				case JSR_SETUP_RECORD_LENGTH:	SetupRec(cbuff2);	break;
				case JSR_SETUP_RECORD_LENGTH2:  SetupRec(cbuff2);	break;
				case JSR_AUX_RECORD_LENGTH:		AuxRec(cbuff2);		break;
				case JSR_READ_RECORD_LENGTH:	ReadRec(cbuff2);	break;
				case 0: break;
				default:
					m_pParent->PostMessage(uIDC_RECEIVE,JSR_UNKNOWN_RECORD,0);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, "53400 Unknown Message Received from JSR\n");
					if (m_pParent->m_bDoDump)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53400 Unknown Message Received from JSR\n");
					strcpy(cBuffx,"53410 ");
					strcat(cBuffx,cbuff2);
					WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuffx);
				}							
				delete [] cbuff2;
				m_dAt = 0;
			}
			else
			{
				m_dAt++;
				if (m_dAt >= ACCUMSIZE) m_dAt = 0;
			}
		}
	}
	CoTaskMemFree((void *)LParam);
	return 0;
}


//JSR Status  Record 
//	updates IDC_JSRSETUP_* on GeneralStatPage

void CJSRPropertySheet::ReadRec(char * Record)
{
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;

//	//save time in binary acquire record
	COleDateTime Base(1952,1,1,0,0,0);
	COleDateTimeSpan Difference = (m_LastStart - Base);
	Difference += HALF_SECOND;
	CJulianTime Jtemp;
	Jtemp.Init((unsigned)(Difference.GetTotalSeconds()));
	JSRAcquireRec.JulianSeconds = (unsigned)Difference.GetTotalSeconds();
	JSRAcquireRec.StatusByte = NULL;
	JSRAcquireRec.Totals2Count = m_fLastAux;
	JSRAcquireRec.Totals3Count = NULL;

	char temp[32];
	char show[128];

	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR2,temp);
	datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"");

	COleDateTime Now = COleDateTime::GetCurrentTime();
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_INQUIRETIME,Now.Format("%Y.%m.%d %H:%M:%S"));
	
	//skip Julian time aux byte for now
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATADATE4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATADATE3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATADATE2,temp);
	strcpy(temp,m_LastStart.Format("%Y.%m.%d"));
	strcpy(show,temp);
	strcat(show," ");
	datapage->SetDlgItemText(IDC_JSRDATA_DATADATE1,temp);

	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATATIME4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATATIME3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_DATATIME2,temp);
	strcpy(temp,m_LastStart.Format("%H:%M:%S"));
	strcat(show,temp);
	strcat(show," Possible Tamper Indication");
	datapage->SetDlgItemText(IDC_JSRDATA_DATATIME1,temp);
	
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ETIME4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ETIME3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ETIME2,temp);

	//get the duration
	strncpy(temp,&Record[0],7);
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
	datapage->SetDlgItemText(IDC_JSRDATA_ETIME1,at);
	JSRAcquireRec.ElapsedTime = atof(at);
	
	//get totals
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_TOTALS4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_TOTALS3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_TOTALS2,temp);
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
	if (JSRAcquireRec.ElapsedTime <= 0)
		bDoingRaw = true;

	JSRAcquireRec.TotalsCount = atof(at);

	m_bZeroTransitionUp = false;
	m_bZeroTransitionDown = false;
	if ((JSRAcquireRec.TotalsCount/(double)JSRAcquireRec.ElapsedTime) < m_lTotalLThresh)
	{
		//set cstatic
		summarypage->SetDlgItemText(IDC_JSRSUMMARY_ZEROVALUE,show);
		//set to red
		summarypage->SetColor(IDC_JSRSUMMARY_ZEROVALUE,COLOR_RED);
		//if not currently in sequence below threshold
		if (!m_bRegisteredZero)
		{
			m_bZeroTransitionDown = true;
			m_bRegisteredZero = true;
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53420 STATUS CHANGE - RECEIVING TOTALS COUNT < LOW THRESHOLD\n");
		}
	}
	else
	{
		//if currently in sequence below threshold
		if (m_bRegisteredZero)
		{
			m_bZeroTransitionUp = true;
			m_bRegisteredZero = false;
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Jtemp, "53430 STATUS CHANGE - RECEIVING TOTALS COUNT >= LOW THRESHOLD\n");
		}
	}
	
	if (bDoingRaw)
		datapage->SetDlgItemText(IDC_JSRDATA_TOTALS1,at);
	else
	{
		sprintf(show,"%11.4e",JSRAcquireRec.TotalsCount/(double)JSRAcquireRec.ElapsedTime);
		datapage->SetDlgItemText(IDC_JSRDATA_TOTALS1,show);
	}

	//get reals+accidentals
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_REALACC4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_REALACC3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_REALACC2,temp);
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
		datapage->SetDlgItemText(IDC_JSRDATA_REALACC1,at);
	else
	{
		sprintf(show,"%11.4e",atof(at)/(double)JSRAcquireRec.ElapsedTime);
		datapage->SetDlgItemText(IDC_JSRDATA_REALACC1,show);
	}
	JSRAcquireRec.RealAccCount = atof(at);
	
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ACCIDENT4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ACCIDENT3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_ACCIDENT2,temp);
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
	JSRAcquireRec.AccidenCount = atof(at);
	double Reals = JSRAcquireRec.RealAccCount - JSRAcquireRec.AccidenCount;
	if (bDoingRaw)
//		datapage->SetDlgItemText(IDC_JSRDATA_ACCIDENT1,at);
		datapage->SetDlgItemInt(IDC_JSRDATA_ACCIDENT1,(int)Reals);
	else
	{
//		sprintf(show,"%11.4e",atof(at)/(double)JSRAcquireRec.ElapsedTime);
		sprintf(show,"%11.4e",Reals/(double)JSRAcquireRec.ElapsedTime);
		datapage->SetDlgItemText(IDC_JSRDATA_ACCIDENT1,show);
	}

	if (!bDoingRaw)
	{
		sprintf(show,"%11.4e",(double)JSRAcquireRec.Totals2Count/(double)JSRAcquireRec.ElapsedTime);
		datapage->SetDlgItemText(IDC_JSRDATA_AUX1TOTALS1,show);
	}


	bool dotrigger = false;
	if (strchr(m_cCameraRealorTotals,'R'))
	{
		if (m_lRealThresh < ((JSRAcquireRec.RealAccCount-JSRAcquireRec.AccidenCount)/JSRAcquireRec.ElapsedTime))
			dotrigger = true;
	}

	if (strchr(m_cCameraRealorTotals,'T'))
	{
		if (m_lTotalThresh < (JSRAcquireRec.TotalsCount/JSRAcquireRec.ElapsedTime))
			dotrigger = true;
	}
	if (strchr(m_cCameraRealorTotals,'1'))
	{
		if (m_lTotal1Thresh < (JSRAcquireRec.Totals2Count/JSRAcquireRec.ElapsedTime))
			dotrigger = true;
	}

	if (dotrigger && m_bCameraDo)
	{
		m_pParent->PostMessage(uIDC_RECEIVE,TRIGGER_CAMERA_REQUEST,0);
		summarypage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,Now.Format("%Y.%m.%d %H:%M:%S Request"));
	}

	WriteToJSRFile(&Jtemp,&JSRAcquireRec);
	m_pParent->PostMessage(uIDC_RECEIVE,JSR_READ_RECORD,0);
}

void CJSRPropertySheet::StatusRecErr(char * Record)
{
	char *SubSection;
	SubSection = Record + strlen(Record) - JSR_STATUS_RECORD_LENGTH;
	StatusRec(SubSection);
}

void CJSRPropertySheet::StatusRec(char * Record)
{

//	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CJSRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CJSRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CJSRModParam* modparpage  = m_pParent->m_pModParams;

	char temp[32];
	COleDateTime Now = COleDateTime::GetCurrentTime();
	strcpy(temp,Now.Format("%Y.%m.%d %H:%M:%S"));
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSTIME,temp);

	datapage->SetDlgItemText(IDC_JSRDATA_STATUSA,(Record[0]=='1')?"Stopped":"Started");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSB,(Record[1]=='1')?"Yes":"No");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSC,(Record[2]=='1')?"Yes":"No");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSD,(Record[3]=='1')?"Yes":"No");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSE,(Record[4]=='1')?"Yes":"No");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSF,(Record[5]=='1')?"Yes":"No");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSG,(Record[6]=='1')?"Single":"Multi");
	datapage->SetDlgItemText(IDC_JSRDATA_STATUSH,(Record[7]=='1')?"Yes":"No");

	if (Record[0] == '1')
		m_pParent->PostMessage(uIDC_RECEIVE,JSR_STATUS_RECORD,1);
	else
		m_pParent->PostMessage(uIDC_RECEIVE,JSR_STATUS_RECORD,0);
}

//JSR HV Record
void CJSRPropertySheet::HighVRec(char * Record)
{
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;

	char temp[32];
	bool CurrentRed = false;
	bool CurrentLow = false;
	bool CurrentHigh= false;
	
	COleDateTime Now = COleDateTime::GetCurrentTime();
	strcpy(temp,Now.Format("%Y.%m.%d %H:%M:%S"));
	datapage->SetDlgItemText(IDC_JSRDATA_HVTIME,temp);


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
		datapage->SetColor(IDC_JSRDATA_HV,COLOR_RED);
		summarypage->SetColor(IDC_JSRSUMMARY_HIGHVOLT2,COLOR_RED);
		if (m_bHighVoltOk)
 			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "53440 STATUS CHANGE - HIGH VOLTAGE OUT OF TOLERANCE (21)\n");
		m_bHighVoltOk = false;
		Data_Tab |= 0x0010;
	}
	else
	{
		CurrentRed = false;
		if (!m_bHighVoltOk)
			WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "53450 STATUS CHANGE - HIGH VOLTAGE IN TOLERANCE (21)\n");
		m_bHighVoltOk = true;
	}

	if (
		CurrentRed && CurrentLow &&
		((m_dLowHV <= m_dHighVoltL) || (m_dLowHV >= m_dHighVoltH))
	   )
	{
		datapage->SetColor(IDC_JSRDATA_HV,COLOR_RED);
		summarypage->SetColor(IDC_JSRSUMMARY_HIGHVOLT1,COLOR_RED);
	}

	if (
		CurrentRed && CurrentHigh &&
		((m_dHighHV <= m_dHighVoltL) || (m_dHighHV >= m_dHighVoltH))
	   )
	{
		summarypage->SetColor(IDC_JSRSUMMARY_HIGHVOLT3,COLOR_RED);
	}

	sprintf(temp,"%d",m_dLowHV);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT1,temp);
	sprintf(temp,"%d",m_dCurHV);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT2,temp);
	datapage->SetDlgItemText(IDC_JSRDATA_HV,temp);
	sprintf(temp,"%d",m_dHighHV);
	summarypage->SetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT3,temp);
	
	if (Data_Tab)
		SetTabColor(IDC_JSRDATA_TAB,COLOR_RED);

	m_pParent->PostMessage(uIDC_RECEIVE,JSR_HV_RECORD,NULL);	
}

//JSR Version Record 
void CJSRPropertySheet::VersionRec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,JSR_VERSION_RECORD,NULL);
}

//JSR AUX Record
void CJSRPropertySheet::AuxRec(char * Record)
{
//	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CJSRModParam* modparpage  = m_pParent->m_pModParams;
	char temp[32];
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS3,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_AUX1TOTALS4,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS2,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_AUX1TOTALS3,temp);
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS1,temp,sizeof(temp));
	datapage->SetDlgItemText(IDC_JSRDATA_AUX1TOTALS2,temp);
	strncpy(temp,&Record[0],13);
	temp[13]=NULL;
	char*at=temp;
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
	datapage->SetDlgItemText(IDC_JSRDATA_AUX1TOTALS1,at);

	m_fLastAux = atof(at);

	m_pParent->PostMessage(uIDC_RECEIVE,JSR_AUX_RECORD,NULL);
}

//JSR SETUP Record 
void CJSRPropertySheet::SetupRec(char * Record)
{
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CJSRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CJSRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CJSRModParam* modparpage  = m_pParent->m_pModParams;

	bool bStatusFail = false;
	char temp[32];
	COleDateTime Now = COleDateTime::GetCurrentTime();
	if ((m_PreviousSetup.GetStatus() != COleDateTime::valid) || (Now.GetDay() != m_PreviousSetup.GetDay()))
	{
		m_PreviousSetup = Now;
		m_PreviousSetup.SetStatus(COleDateTime::valid);
//		WriteToFile(TO_CEV|TO_PFM,TYPE_COMP,NULL,"Start New Day\n");
		char cBuff[1024];
		strcpy(cBuff,"53460 ");
		strcat(cBuff,Record);
		cBuff[strlen(Record) + 4] = NULL;
		WriteToFile(TO_CEV|TO_PFM,TYPE_NEWDAY,NULL,cBuff);
	}
	strcpy(temp,Now.Format("%Y.%m.%d %H:%M:%S"));
	datapage->SetDlgItemText(IDC_JSRDATA_SETUPTIME,temp);

	temp[0] = Record[0];
	temp[1] = '.';
	temp[2] = Record[1];
	temp[3] = NULL;
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_PRE,temp);
	if	(
		((int)(atof(temp) * 2.0) - 1 ) != m_dPreDelay
		)
	{
		bStatusFail = true;
		datapage->SetColor(IDC_JSRDATA_SETUP_PRE,COLOR_RED);
	}

	temp[0] = Record[2];
	temp[1] = '.';
	temp[2] = Record[3];
	temp[3] = 'E';
	temp[4] = Record[4];
	temp[5] = NULL;
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_COUNT,temp);
	double fInquireInt = atof(temp);
	m_fInquireInt = m_dCountTimeUnit / 10.0;
	for (int i = 0; i < m_dCountTimeExp; i++)
		m_fInquireInt *= 10.0;
//	m_fInquireInt = (double)m_dCountTimeUnit;
	if ((fInquireInt != m_fInquireInt)&&(m_fInquireInt > 0.0))
	{
		bStatusFail = true;
		datapage->SetColor(IDC_JSRDATA_SETUP_COUNT,COLOR_RED);
	}

	strncpy(temp,&Record[5],3);
	temp[3] = NULL;
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_GATE,temp);
	if (atoi(temp) != m_dGateWidth)	
	{
		bStatusFail = true;
		datapage->SetColor(IDC_JSRDATA_SETUP_GATE,COLOR_RED);
	}

	strncpy(temp,&Record[8],4);
	temp[4] = NULL;
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_HV,temp);
	if (atoi(temp) != m_dHighV)
	{
		bStatusFail = true;
		datapage->SetColor(IDC_JSRDATA_SETUP_HV,COLOR_RED);
	}

	strncpy(temp,&Record[12],2);
	temp[2] = NULL;
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_MULTI,temp);

	if (Record[18] == 'K')
	{
		strncpy(temp,&Record[14],5);
		temp[5] = NULL;
	}
	else
	{
		strncpy(temp,&Record[14],4);
		temp[4] = NULL;
	}
	datapage->SetDlgItemText(IDC_JSRDATA_SETUP_BAUD,temp);
	m_pParent->PostMessage(uIDC_RECEIVE,JSR_SETUP_RECORD,(LPARAM)(float)fInquireInt);

	if (bStatusFail)
	{
		m_pParent->FlagOthr(1,true);			
		summarypage->SetColor(IDC_JSRSUMMARY_CONFIGURATION,COLOR_RED);
		SetTabColor(IDC_JSRDATA_TAB,COLOR_RED);
		summarypage->SetDlgItemText(IDC_JSRSUMMARY_CONFIGURATION,"NOT THE SAME"); 
	}
	else
	{
		summarypage->SetDlgItemText(IDC_JSRSUMMARY_CONFIGURATION,"IDENTICAL");
	}
}

//this function should go into the parent class
void  CJSRPropertySheet::BuildFileName(CString &cResult,
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

bool CJSRPropertySheet::WriteToJSRFile(
	CJulianTime* Time, 
	JSRAcquireRecord* AcquireRec)
{
	FILE *File;
	fpos_t position;
	bool DoNewRecord;
	struct JSRAcquireHeader JSRAcquireHead;

	//get the summary page pointer and the data page pointer
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;

	//initialize to don't do a new record
	DoNewRecord = false;
	
	//bail out if we can't write to the disk
	if (m_pParent->m_bBlockWrite) return false;
	
	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentJSRFileName,"%s\\%s%c%c%c%c%02d.%3s",
				((CJSRInst*)m_pParent)->m_szSaveLoc,	
				((CJSRInst*)m_pParent)->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,JSR_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"SR",
			m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,JSR_SUFFIX);
		strcpy(m_szCurrentJSRFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (_access(m_szCurrentJSRFileName,0)!=0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentJSRFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				((CJSRInst*)m_pParent)->m_szSaveLoc,	
				((CJSRInst*)m_pParent)->m_pID,
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
				'A',0,JSR_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_pParent->m_csLongNameFormatString,cTemp,"SR",
				m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,JSR_SUFFIX);
		}
		//if the file is in the archive directory
		if (_access(ArchiveName,0)==0)
		{
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentJSRFileName,
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
		File = fopen(m_szCurrentJSRFileName,"ab+");
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
				memset( &JSRAcquireHead, ' ', sizeof(JSRAcquireHeader));

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(JSRAcquireHeader)-4);
				memcpy(JSRAcquireHead.SizeOfHeader,temp,4);

				//Fix for new version number formatting inside of binary file
				//not converting to a float for the review software.
				//Incompatible format for binary file came out "2.0.0".
				//Compatible format is " 2000".
				//Prior to 2.0.0.0, the version number was formatted "1.907".
				//This code reformats the newer version format to look like the old.
				//SK uses a float to read the value from the binary file.
				//Fix provided by DP and incorporated by PJM 8/5/2005
				//
				//sprintf(temp,"%5s",m_pParent->m_csVersion);
				short sA,sB,sC,sD;
				sscanf(m_pParent->m_csVersion,"%d.%d.%d.%d",&sA,&sB,&sC,&sD);
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(JSRAcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(JSRAcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time->Year()%100);
				memcpy(JSRAcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time->Month());
				memcpy(JSRAcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time->Day());
				memcpy(JSRAcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time->Year());
				memcpy(JSRAcquireHead.unused2,temp,4);
				//write header
				fwrite(&JSRAcquireHead,sizeof(JSRAcquireHeader),1,File);
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
			File = fopen(m_szCurrentJSRFileName,"rb+");
			if (File == NULL) Sleep(250);
			Attempts--;
		}
		
		//if reals greater than real theshold
//		if ((AcquireRec->TotalsCount/AcquireRec->ElapsedTime) < m_lTotalLowThresh)
		if (m_bZeroTransitionUp || m_bZeroTransitionDown)
		{
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,(m_bZeroTransitionDown)?"T<Low T.Thres":"T>=Low T.Tresh");
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
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"R >= R Thresh");
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
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"T >= T Thresh");
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
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"T1 >= T1 Thresh");
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
				datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"New Record");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"First BG Data");
			}
			else
			{
				datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"New Record");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE2,"Not First BG Data");
			}
		}
		
		//do a new record if we have reached the maximum number of compressed records
		if (m_dCompressCount >= m_dMaxCompress)
		{
			DoNewRecord = true;
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,(m_dCompressCount >= 999999)?"New Record":"Max Compr Met");
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
				datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"Over Sigma");
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE4,"Over Sigma");
			}
			else
			{
				datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"New Record");
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
//			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,"New Rec");
			//if overthreshold 'cause of RealThreshold Rate or because of 
			//the sigma test then we don't want to save the record
			if (m_bOverThresh)
			{
				memset(&SumAcquireRec, 0x00, sizeof(JSRAcquireRecord));
				//we have no records in the sumacquirerec
				m_dCompressCount = 0;
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Did new Cleared Old");
			}
			else
			{
				memcpy(&SumAcquireRec,AcquireRec,sizeof(JSRAcquireRecord));
				//we only have one record in the sumacquirerec
				m_dCompressCount = 1;
				if (summarypage->m_bDebug)
					summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Did new Saved Old");
			}


			//send record to end of file
			fseek(File,0,SEEK_END);
			if (fwrite(AcquireRec,sizeof(JSRAcquireRecord),1,File)!= 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(100);
				if (fwrite(AcquireRec,sizeof(JSRAcquireRecord),1,File)!=1)
				{
					//if failed it could be because MicXfer is trying to
					//clear the archive bit, so wait a short time and try
					//again.
					Sleep(200);
					if (fwrite(AcquireRec,sizeof(JSRAcquireRecord),1,File)!=1)
					{
						//if failed on write then show a file write problem
						//on the instrument's button
						m_pParent->m_pButton->SetDiskIcon_Hourglass();
						m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
						m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
						WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53470 Could not write to JSR file\n");
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
			datapage->SetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,temp);
			
			SumAcquireRec.AccidenCount	+= AcquireRec->AccidenCount;
			SumAcquireRec.ElapsedTime	+= AcquireRec->ElapsedTime;
			SumAcquireRec.RealAccCount	+= AcquireRec->RealAccCount;
			SumAcquireRec.TotalsCount	+= AcquireRec->TotalsCount;
			SumAcquireRec.Totals2Count	+= AcquireRec->Totals2Count;
			SumAcquireRec.Totals3Count	+= AcquireRec->Totals3Count;
			fseek( File, -(signed)sizeof(JSRAcquireRecord), SEEK_END);
			//send record to file
			if (summarypage->m_bDebug)
			{
				CString tempstring;
				tempstring.Format("Adding to rec: %d",m_dCompressCount);
				summarypage->SetDlgItemText(IDC_STATIC_NOTE5,tempstring);
			}
			
			if (fwrite(&SumAcquireRec,sizeof(JSRAcquireRecord),1,File)!= 1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(50);
				if (fwrite(&SumAcquireRec,sizeof(JSRAcquireRecord),1,File)!=1)
				{
					//if failed it could be because MicXfer is trying to
					//clear the archive bit, so wait a short time and try
					//again.
					Sleep(200);
					if (fwrite(&SumAcquireRec,sizeof(JSRAcquireRecord),1,File)!=1)
					{
						//if failed on write then show a file write problem
						//on the instrument's button
						m_pParent->m_pButton->SetDiskIcon_Hourglass();
						m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
						m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
						WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53480 Could not write to JSR file\n");
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
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "53490 Could not open JSR file\n");
		if (summarypage->m_bDebug)
			summarypage->SetDlgItemText(IDC_STATIC_NOTE5,"Error Open JSR");
	}
	return false;
}

void CJSRPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg)
{
	FILE *File;
	char Message[512];
	fpos_t position;
	Message[0] = NULL;
	COleDateTime TC = COleDateTime::GetCurrentTime(); 		//computer time
	COleDateTime TG;
	COleDateTimeSpan TS;
	CString ArchiveFileName;

	if (m_pParent->m_bBlockWrite) 
		return;

	//default to the provided JSR time
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
				((CJSRInst*)m_pParent)->m_szSaveLoc,	
				((CJSRInst*)m_pParent)->m_pID,
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
				((CJSRInst*)m_pParent)->m_szSaveLoc,	
				((CJSRInst*)m_pParent)->m_pID,
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
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d J %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\n":Msg);
		break;

	case TYPE_NEWDAY:
		//	message = Time::YY.MM.DD HH:MM:SS J New Day, Setup= (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d J %s = Setup, New Day\n",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),	TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n":Msg);
		break;


	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS JSR Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - J = xxxx\r"
		//JSR time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d J 53500 JSR Time %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - I = %.0f seconds\n",
			TG.GetYear(),TG.GetMonth(),TG.GetDay(),TG.GetHour(),TG.GetMinute(),TG.GetSecond(),
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 53510 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C JSR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53520 JSR COLLECT Version %s started\n",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C JSR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53530 JSR COLLECT Version %s started from abnormal shutdown\n",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion);
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53540 File %s deleted\n",
		TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 53550 Unknown TYPE %s\n",
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
		
		if ((File = fopen(fn,"ab+")) != NULL)
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
			m_pParent->MakeMyPath(fn);

			if (m_pParent->m_bUseShortFilename)
			{
				ArchiveFileName.Format("%s\\archive\\%s%c%c%c%c%02d.%s",
				((CJSRInst*)m_pParent)->m_szSaveLoc,
				((CJSRInst*)m_pParent)->m_pID,
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
				cTemp = ((CJSRInst*)m_pParent)->m_szSaveLoc;
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

		if ((File = fopen(fn,"ab+")) != NULL)
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
			fprintf(File,Message);
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
				((CJSRInst*)m_pParent)->m_szSaveLoc,
				((CJSRInst*)m_pParent)->m_pID,
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

		if ((File = fopen(fn,"ab+")) != NULL)
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
			fprintf(File,Message);
			//	close pfm
			fclose(File);
		}
	}

}


int CJSRPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CJSRPropertySheet::OnCloseButton()
{
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CJSRPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	if (Tab != IDC_JSRDATA_TAB) 
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
//	case IDC_JSRSETUP_TAB:   tab = 1; break;
	case IDC_JSRDATA_TAB:    tab = 1; break;
//	case IDC_JSRUSERPROG_TAB:tab = 3; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}

void CJSRPropertySheet::OnAckButton()
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

	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CJSRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CJSRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CJSRModParam* modparpage  = m_pParent->m_pModParams;

//	generalpage->SetColor(IDC_GENERAL_CHECKSUM,COLOR_BLACK);
	summarypage->ResetColors();
//	generalpage->ResetColors();
	datapage->ResetColors();

//	SetTabColor(IDC_JSRSETUP_TAB,COLOR_GREEN);
	SetTabColor(IDC_JSRDATA_TAB,COLOR_GREEN);
	Data_Tab = 0;
//	General_Tab = 0;
//	UserProg_Tab = 0;
//	SetTabColor(IDC_JSRUSERPROG_TAB,COLOR_GREEN);
	Invalidate();
}

void CJSRPropertySheet::OnPauseButton()
{
	m_dToClose = 0;
//	CJSRModParam* modparpage = m_pParent->m_pModParams;
	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
//		modparpage->GetDlgItem(IDC_JSRMODPARAMS_DATETIME)->EnableWindow(true);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
//		modparpage->GetDlgItem(IDC_JSRMODPARAMS_DATETIME)->EnableWindow(false);
	}
}

void CJSRPropertySheet::IssuePrint(FILE* outfile)
{
	CJSRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CJSRDataStat* datapage    = m_pParent->m_pDataStatus;
	CJSRModParam* modparpage  = m_pParent->m_pModParams;
	CJSRConfig2*  camerapage  = m_pParent->m_pCamera;

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"* = Item is displayed RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"SUMMARY\n");

	fprintf(outfile," Data Status\n");
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_JSRSUMMARY_HIGHVOLT1-IDC_JSRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_JSRSUMMARY_HIGHVOLT2-IDC_JSRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_HIGHVOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_JSRSUMMARY_HIGHVOLT3-IDC_JSRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"     High Voltage (Min / Cur / Max): %8s %8s %8s\n",
		temp1,temp2,temp3);
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_INQUIRETIME,temp1);		//dr00073
	fprintf(outfile,"      Last Data Status(Y.M.D H:M:S): %s\n",temp1);	//dr00073

	summarypage->GetDlgItemText(IDC_JSRSUMMARY_CONFIGURATION,temp1);	//dr00073
	fprintf(outfile,"   MIC vs. Instrument Configuration: %s\n",temp1);	//dr00073

	summarypage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp1);		//dr00073
	fprintf(outfile,"   Last Camera Trigger(Y.M.D H:M:S): %s\n",temp1);	//dr00073

	summarypage->GetDlgItemText(IDC_JSRSUMMARY_ZEROVALUE,temp1);
	fprintf(outfile,"      Totals Below LOW Threshold at: %s\n",temp1);

	fprintf(outfile,"  Recent Commands and Responses:\n");

	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND13,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND12,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND11,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND10,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND9,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND8,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND7,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND6,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND5,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND4,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND3,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND2,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_JSRSUMMARY_COMMAND1,temp);
	temp = temp.SpanExcluding("\n");
	fprintf(outfile,"  %s\n",LPCTSTR(temp));

	if(((CButton*)summarypage->GetDlgItem(IDC_CHECK_ALLOWCLICK))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Allow click on Recent Msg: %s\n",temp);

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"DATA STATUS\n");
	fprintf(outfile," Most Recent Response To Read Status\n");
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSA,temp);
	fprintf(outfile,"             Stopped\\Started: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSD,temp);
	fprintf(outfile,"       Stop Button Activated: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSF,temp);
	fprintf(outfile,"      Start Button Activated: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSB,temp);
	fprintf(outfile,"       Terminated by Timeout: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSE,temp);
	fprintf(outfile,"     Reset Button Activated: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSH,temp);
	fprintf(outfile,"   Readout Button Activated: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSC,temp);
	fprintf(outfile,"         Carry/Borrow Fault: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_STATUSG,temp);
	fprintf(outfile,"           Single/Multi Run: %s\n",temp);

	fprintf(outfile," Most Recent Response To Read Setup\n");
	datapage->GetDlgItemText(IDC_JSRDATA_SETUPTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_PRE,temp);
	fprintf(outfile,"                  Pre-delay: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_HV,temp);
	fprintf(outfile,"                  HV Set At: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_COUNT,temp);
	fprintf(outfile,"                 Count Time: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_MULTI,temp);
	fprintf(outfile,"                 Multi Runs: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_GATE,temp);
	fprintf(outfile,"                 Gate Width: %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_SETUP_BAUD,temp);
	fprintf(outfile,"                  Baud Rate: %s\n",temp);

	fprintf(outfile," Most Recent Response To Read HV\n");
	datapage->GetDlgItemText(IDC_JSRDATA_HVTIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp);
	datapage->GetDlgItemText(IDC_JSRDATA_HV,temp);
	fprintf(outfile,"                High Voltage: %s\n",temp);

	fprintf(outfile," Most Recent ACQUIRE Records Received\n");

	
	if(((CButton*)datapage->GetDlgItem(IDC_RADIO_RAW))->GetCheck()==0)
		temp = "Show Rates";
	else
		temp = "Show Raw";
	fprintf(outfile,"                             %s\n",temp);

	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_DATADATE4,temp4);
	fprintf(outfile,"  Date of Data Record (Y/M/D): %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_DATATIME4,temp4);
	fprintf(outfile,"  Time of Data Record (H:M:S): %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR1,temp1);		//dr00073 -- just moved them down to 
	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR2,temp2);		//dr00073 -- match the dialog
	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR3,temp3);		//dr00073
	datapage->GetDlgItemText(IDC_JSRDATA_FILTERFACTOR4,temp4);		//dr00073
	fprintf(outfile,"                Filter Factor: %11s %11s %11s %11s\n",	//dr00073
		temp1, temp2, temp3, temp4);								//dr00073

	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_TOTALS4,temp4);
	fprintf(outfile,"                       Totals: %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_AUX1TOTALS4,temp4);
	fprintf(outfile,"             Auxillary Totals: %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_REALACC1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_REALACC4,temp4);
	fprintf(outfile,"          Reals + Accidentals: %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_ACCIDENT4,temp4);
	fprintf(outfile,"                  Accidentals: %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	datapage->GetDlgItemText(IDC_JSRDATA_ETIME1,temp1);
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME2,temp2);
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME3,temp3);
	datapage->GetDlgItemText(IDC_JSRDATA_ETIME4,temp4);
	fprintf(outfile,"                 Elapsed Time: %11s %11s %11s %11s\n",
		temp1, temp2, temp3, temp4);

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp,temp1);
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station/File ID: %s\n",temp);				//dr00073
	if(((CButton*)modparpage->GetDlgItem(IDC_JSRMODPARAMS_LOGFILTER))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Log Data Filtering Msgs: %s\n",temp);
	if(((CButton*)modparpage->GetDlgItem(IDC_JSRMODPARAMS_DODUMP))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";

	fprintf(outfile," Error Limits\n");
	fprintf(outfile,"                       High     Low\n");
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_HIGHVOLTH,temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_HIGHVOLTL,temp1);
	fprintf(outfile,"       High Voltage: %6s  %6s\n",temp,temp1);	//dr00073 -- took out battery

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_MAX_TIMEOUT,temp);	//dr00073 -- added
	fprintf(outfile,"# Timeouts before fail: %d\n",temp);			//dr00073 -- added

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_SLOW_RESET,temp);
	fprintf(outfile,"  Slow Reset (Sec.): %6s\n\n",temp);

	fprintf(outfile," General\n");
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp);

	fprintf(outfile," Data Compression Rates\n");

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_TOTALSLTHRESH,temp);
	fprintf(outfile,"      LOW Totals Threshold: %s\n",temp);		//dr00073 -- added

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_TOTALSTHRESH,temp);
	fprintf(outfile,"          Totals Threshold: %s\n",temp);

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_REALSTHRESH,temp);
	fprintf(outfile,"           Reals Threshold: %s\n",temp);

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_TOTALS1THRESH,temp);
	fprintf(outfile,"      Aux Totals Threshold: %s\n",temp);

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_SIGMATHRESH,temp);
	fprintf(outfile," Sigma Threshold 0.1 Units: %s\n",temp);

	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_MAXCOMPRESS,temp);
	fprintf(outfile,"       Maximum Data Points: %s\n",temp);

	fprintf(outfile," JSR Configuration\n");
	modparpage->GetDlgItemText(IDC_STATIC_CT_RESULT,temp);
	fprintf(outfile,"    Count Time (seconds): %s\n",temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_PREDELAY,temp);
	fprintf(outfile,"       Pre-Delay (uSec.): %s\n",temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_GATEWIDTH,temp);
	fprintf(outfile,"      Gate Width (uSec.): %s\n",temp);
	modparpage->GetDlgItemText(IDC_JSRMODPARAMS_HIGHV,temp);
	fprintf(outfile,"    High Voltage (Volts): %s\n",temp);

	/*fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
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
	fprintf(outfile,"  Parallel Printer Port: %s\n",temp);
	fprintf(outfile,"             Pin Number: %d\n",camerapage->GetDlgItemInt(IDC_CAMERAPIN));
	fprintf(outfile," Camera Trigger Configuration\n");
	temp = "";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGREALS))->GetCheck()==1)
		temp += "Reals ";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGTOTALS))->GetCheck()==1)
		temp += "Totals ";
	if (((CButton*)camerapage->GetDlgItem(IDC_CAMERATRIGTOTALS1))->GetCheck()==1)
		temp += "Totals 1";
	fprintf(outfile,"                   Trigger on: %s\n",temp);
	
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT1,temp1);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT2,temp2);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT3,temp3);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT4,temp4);
	temp = temp1 +":"+ temp2 +":"+ temp3 +":"+ temp4;
	fprintf(outfile,"      Trigger Delay (D:H:M:S): %s\n",temp);

	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT5,temp1);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT6,temp2);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT7,temp3);
	camerapage->GetDlgItemText(IDC_JSRCONFIG_EDIT8,temp4);
	temp = temp1 +":"+ temp2 +":"+ temp3 +":"+ temp4;
	fprintf(outfile,"   Trigger Suppress (D:H:M:S): %s\n",temp);*/

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
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
	GetPrivateProfileString(m_pParent->m_pName,"COUNTTIMEEXP","",buff,sizeof(buff),inifile);
	fprintf(outfile," COUNTTIMEEXP=%s\n",buff);
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
	fprintf(outfile,"  CAMERA_TRIGGER_SUPPRESS=%s\n",buff);*/
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime);
}

void CJSRPropertySheet::OnPrintButton()
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

void CJSRPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
//		CJSRModParam* modparpage = m_pParent->m_pModParams;
//		if (m_pParent->IsPaused())
//			modparpage->GetDlgItem(IDC_JSRMODPARAMS_DATETIME)->EnableWindow(true);
//		else
//			modparpage->GetDlgItem(IDC_JSRMODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CJSRPropertySheet::OnToCloseButton()
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

LRESULT CJSRPropertySheet::OnExtendClose(WPARAM, LPARAM)
{
	m_dToClose = 0;
	return 0;
}


void CJSRPropertySheet::OnTimer(UINT nIDEvent) 
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

void CJSRPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it

	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}


BOOL CJSRPropertySheet::OnInitDialog() 
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

BOOL CJSRPropertySheet::DestroyWindow() 
{
	//destroy the imagelist
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CJSRPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

CString *CJSRPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}