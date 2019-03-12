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
// ISRSafePS.cpp : implementation file
//

#include "stdafx.h"
#include "ISO_definitions.h"
#include "colors.h"
#include "ccbutton.h"
#include "JulianT.h"
#include "ISRSafePS.h"
#include "ISRInst.h"
#include "INI_definitions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CISRPropertySheet

IMPLEMENT_DYNAMIC(CISRPropertySheet, CPropertySheet)

CISRPropertySheet::CISRPropertySheet(
	UINT nIDCaption, 
	CISRInst* pParentWnd)
	:CPropertySheet(nIDCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	m_pCaption[0] = NULL;
	FinishConstruction();
}

CISRPropertySheet::CISRPropertySheet(
	LPCTSTR pszCaption, 
	CISRInst* pParentWnd)
	:CPropertySheet(pszCaption, pParentWnd, 0),
	m_pParent(pParentWnd)
{
	strncpy(m_pCaption,pszCaption,sizeof(m_pCaption));
	FinishConstruction();
}

void CISRPropertySheet::FinishConstruction()
{
	uIDC_RECEIVE = m_pParent->m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	General_Tab		= NULL;
	Data_Tab		= NULL;
	UserProg_Tab	= NULL;

	m_bFirstHV		= true;
	m_bHighVoltOk   = true;
	m_bBatteryOk	= true;
	m_bP5VoltOk		= true;
	m_bBatteryTolOk	= true;
	m_bP15VoltOk	= true;
	m_bM15VoltOk	= true;
	m_bACPowerOk	= true;
	m_bACPowerLoss	= false;
	m_bBBMOk		= true;
	m_bLonOk		= true;
	m_bNewFileName	= false;
	m_bDataFiltered	= false;
	m_bTimeError	= false;

	m_dLowHV = m_dHighHV = m_dCurHV	= 0;
	m_bFirstHV = true;
	m_fLowBV = m_fCurBV = m_fHighBV = 0.0;
	m_bFirstBV = true;
	m_fLowP15 = m_fCurP15 = m_fHighP15 = 0.0;
	m_bFirstP15 = true;
	m_fLowM15 = m_fCurM15 = m_fHighM15 = 0.0;
	m_bFirstM15 = true;
	m_fLowP5 = m_fCurP5 = m_fHighP5 = 0.0;
	m_bFirstP5 = true;

	m_dToClose = 0;
	m_bStartOfFile = true;
	m_dPrevISRHour	= 25; //larger than the largest possible so will see "new" file

	m_szCurrentFileName[0] = NULL;
	m_szPreviousGTime[0] = NULL;
	m_dCurrentDay = -1;

	m_dAt			= 0;
	m_dAcquireRecordCount = 0;
	m_dMessageRecordCount = 0;
	m_bCouldNotOpenBinaryFile = false;

	m_pCloseButton	= NULL;
	m_pPauseButton	= NULL;
	m_pPrintButton	= NULL;
	m_pAckFlagsButton	= NULL;
	m_pToClose	= NULL;
	m_pImageList	= NULL;

	m_uiStatusRecTime = 0;		//DR00406
	m_uiMonStatRecTime = 0;		//DR00406
}

CISRPropertySheet::~CISRPropertySheet()
{
	delete m_pCloseButton;
	delete m_pPauseButton;
	delete m_pPrintButton;
	delete m_pAckFlagsButton;
	delete m_pToClose;
	delete m_pImageList;
}

COLORREF CISRPropertySheet::SetButtonColor(COLORREF color)
{
	if (color == COLOR_PAUSE)
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
	else
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);

	return m_pCloseButton->SetFaceColor(color);
}


void CISRPropertySheet::SetLimits(
	float batvolth, float batvoltl,
	float sup_p15h, float sup_p15l,
	float sup_m15h, float sup_m15l,
	float sup_p05h, float sup_p05l,
	int dHighVoltH, int dHighVoltL,
	int time_err,	char* Id)
{	
	m_fBatVoltH=batvolth;
	m_fBatVoltL=batvoltl;
	m_fSup_P15H=sup_p15h;
	m_fSup_P15L=sup_p15l;
	m_fSup_M15H=sup_m15h;
	m_fSup_M15L=sup_m15l;
	m_fSup_P05H=sup_p05h;
	m_fSup_P05L=sup_p05l;
	m_dHighVoltH = dHighVoltH;
	m_dHighVoltL = dHighVoltL;
	m_dTime_Err=(long)time_err;
	strcpy(m_szCurrentID,Id);
}

BEGIN_MESSAGE_MAP(CISRPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CISRPropertySheet)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CLOSE_BUTTON, OnCloseButton)
	ON_BN_CLICKED(ID_PAUSE_BUTTON, OnPauseButton)
	ON_BN_CLICKED(ID_ACKFL_BUTTON, OnAckButton)
	ON_BN_CLICKED(ID_PRINT_BUTTON, OnPrintButton)
	ON_BN_CLICKED(ID_TOCLOSE, OnToCloseButton)
	ON_MESSAGE(IDC_EXTENDCLOSE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnExtendClose)  //MIC_CB_028 to MIC 1.907 PJM 1/26/05
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM)) OnReceiveOOB)
END_MESSAGE_MAP()

void CISRPropertySheet::UpdateSummary(char *Message)
{
	CString Data;

	CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND5,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND6,Data);
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND4,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND5,Data);
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND3,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND4,Data);
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND3,Data);
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND1,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND2,Data);
	summarypage->SetDlgItemText(IDC_ISRSUMMARY_COMMAND1,Message);

	if (strstr(Message,"LONG BREAK SET"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		m_cCommStatus = "Comm Status Unknown";
		SetChecksumDisplay();
		generalpage->SetColor(IDC_ISRSETUP_CHECKSUM,COLOR_RED);
		SetTabColor(IDC_ISRSETUP_TAB,COLOR_RED);
	}

	if (strstr(Message,"LOCAL LONG BREAK"))
	{
		COleDateTime Now = COleDateTime::GetCurrentTime();
		CString DateTime;
		DateTime = Now.Format("Local Long Break Sent: %d/%H:%M:%S");
		m_cCheckSumMsg = DateTime;
		SetChecksumDisplay();
		generalpage->SetColor(IDC_ISRSETUP_CHECKSUM,COLOR_RED);
		SetTabColor(IDC_ISRSETUP_TAB,COLOR_RED);
	}
}

void CISRPropertySheet::SetChecksumDisplay()
{
	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CString cString;
	cString = m_cCheckSumMsg;;
	cString += " ";
	cString += m_cCommStatus;
	generalpage->SetDlgItemText(IDC_ISRSETUP_CHECKSUM,cString);
}

void CISRPropertySheet::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
	char* cbuff = (char*)LParam;
	char cbuff2[1024];
	char cbuff3[1024];
	cbuff2[1023] = NULL;

	try
	{
		strcpy(cbuff2,"22560 OOB: ");
		int length2 = strlen(cbuff2) + (int)WParam;
		strncat(cbuff2,cbuff,(int)WParam);
		cbuff2[length2] = NULL;

		//build the display message
		strncpy(cbuff3,cbuff,(int)WParam);
		cbuff3[(int)WParam] = NULL;
		m_cCommStatus = cbuff3;
		SetChecksumDisplay();
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP,NULL,"22570 Bad LParam Address Received (OOB)\n\r");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"22570 Bad LParam Address Received (OOB)\n\r");
		CoTaskMemFree((void *)LParam);
		return;
	}

	strcat(cbuff2,"\n\r");

	WriteToFile(TO_PFM, TYPE_COMP, NULL, cbuff2);
	UpdateSummary(cbuff2);
	if (m_pParent->m_bDoDump)
		WriteToFile(TO_DMP,TYPE_DUMP, NULL, cbuff2);

	CoTaskMemFree((void *)LParam);
}


void CISRPropertySheet::OnReceive(WPARAM WParam, LPARAM LParam)
{
	char* cbuff =(char*)LParam;
	char c;
	try
	{
		c = cbuff[0];
	}
	catch(...)
	{
		WriteToFile(TO_PFM,TYPE_COMP, NULL,"22580 Bad LParam Address Received (1002)\n\r");
		if (m_pParent->m_bDoDump)
			WriteToFile(TO_DMP,TYPE_DUMP, NULL,"22580 Bad LParam Address Received (1002)\n\r");
		CoTaskMemFree((void *)LParam);
		return;
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
//			((m_dWaitingForResponse)&&(m_dAt >= m_dWaitingForResponse)) ||
		 		((m_pAccumulator[m_dAt]==EOL1)&&(m_pAccumulator[m_dAt-1]==EOL2)) ||
				((m_pAccumulator[m_dAt]==EOL2)&&(m_pAccumulator[m_dAt-1]==EOL1))
				)
			) 
			{
				short message_type = UNKNOWN_REC;
				char* cbuff2;
//				if (m_dWaitingForResponse)
//				{
//					cbuff2 = (char*)malloc(m_dAt+4);
//					strcpy(cbuff2,UNKNOWN_REC_CHAR);
//					memmove(&cbuff2[2],m_pAccumulator,m_dAt+1);
//					cbuff2[m_dAt+3] = NULL;
//				}
//				else
//				{
					cbuff2 = (char*)malloc(m_dAt+2);
					memmove(cbuff2,m_pAccumulator,m_dAt+1);
					cbuff2[m_dAt+1] = NULL;
//				}

				if (m_pParent->m_bDoDump)
				{
					char cBuff[1024];
					strcpy(cBuff,"22581 ");
					strcat(cBuff,cbuff2);
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);//dump file
				}

				UpdateSummary(cbuff2);
			
				sscanf(cbuff2,"%2hx",&message_type);

				switch(message_type)
				{
				case ISRSTATUS_RECORD:		StatusRec(cbuff2);	break;
				case ISRACQUIRE_RECORD:		AcquirRec(cbuff2);	break;
				case ISREND_DUMP_RECORD:	DumpEnd(cbuff2);	break;
				case ISREND_DUMP2_RECORD:	DumpEnd2(cbuff2);	break;
				case ISREND_DUMP3_RECORD:	DumpEnd3(cbuff2);	break;
				case ISRBUSY_RECORD:		BusyRec(cbuff2);	break;
				case ISRDUMP_OK_RECORD:		DumpOk(cbuff2);		break;
				case ISRDUMP_OK2_RECORD:	DumpOk2(cbuff2);	break;
				case ISRBATTERY_RECORD:		BatteryRec(cbuff2);	break;
				case ISRMON_STATUS_RECORD:	MonStatRec(cbuff2);	break;
				case ISRTRIG_SIG_RECORD:	TrigSigRec(cbuff2);	break;
				case ISRMONITOR_ID2_RECORD:	ID2_Record(cbuff2);	break;
				case ISRANALYZE_OK_RECORD:	AnalyzeOk(cbuff2);	break;
				case ISRINFORMATION_RECORD:	InformRec(cbuff2);	break;
//				case UNKNOWN_REC:			ResponseRec(cbuff2);break;

				default:
					if (strstr(cbuff2,"?0"))
						Echo_0_Rec(cbuff2);
					else if (strstr(cbuff2,"?1"))
						Echo_1_Rec(cbuff2);
					else if (strstr(cbuff2,"?2"))
						Echo_2_Rec(cbuff2);
					else if (strstr(cbuff2,"?A"))
						Echo_0_Rec(cbuff2);
					else if (strstr(cbuff2,"?a"))
						Echo_0_Rec(cbuff2);
					else if (strstr(cbuff2,"?B"))
						Echo_1_Rec(cbuff2);
					else if (strstr(cbuff2,"?b"))
						Echo_1_Rec(cbuff2);
					else if (strstr(cbuff2,"?C"))
						Echo_2_Rec(cbuff2);
					else if (strstr(cbuff2,"?c"))
						Echo_2_Rec(cbuff2);

				}							
				//delete [] cbuff2;
				m_dAt = 0;
			}
			else
			{
				m_dAt++;
				if (m_dAt >= ACCUMULATOR_SIZE) m_dAt = 0;
			}
		}
	}
	CoTaskMemFree((void *)LParam);
}

void CISRPropertySheet::Echo_0_Rec(char * Record)
{
	if (toupper(Record[1])=='A')
	{
		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		generalpage->SetDlgItemText(IDC_ISRSETUP_INSTTYPE,"AMSR");
		m_pParent->PostMessage(uIDC_RECEIVE,ISR_ISAMSR,CHECKSUM_GOOD);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISR_Q0_RECORD,CHECKSUM_GOOD);
}

void CISRPropertySheet::Echo_1_Rec(char * Record)
{
	if (toupper(Record[1])=='B')
	{
		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		generalpage->SetDlgItemText(IDC_ISRSETUP_INSTTYPE,"AMSR");
		m_pParent->PostMessage(uIDC_RECEIVE,ISR_ISAMSR,CHECKSUM_GOOD);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISR_Q1_RECORD,CHECKSUM_GOOD);
}

void CISRPropertySheet::Echo_2_Rec(char * Record)
{
	if (toupper(Record[1])=='C')
	{
		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		generalpage->SetDlgItemText(IDC_ISRSETUP_INSTTYPE,"AMSR");
		m_pParent->PostMessage(uIDC_RECEIVE,ISR_ISAMSR,CHECKSUM_GOOD);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISR_Q2_RECORD,CHECKSUM_GOOD);
}

void CISRPropertySheet::ResponseRec(char * /*Record*/)
{
	m_pParent->PostMessage(uIDC_RECEIVE,UNKNOWN_REC,CHECKSUM_GOOD);
}

char *CISRPropertySheet::StripLeadingBlanks(char *source)
{
	char *At = source;
	while ((At[0] == ' ') && (At[0] != NULL))At++;
	return At;
}

//record 21 Status SR Record 
//	updates IDC_ISRSETUP_* on GeneralStatPage
void CISRPropertySheet::StatusRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRSTATUS_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;

	if (CheckSum == CHECKSUM_GOOD)
	{

		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		
		m_dMessageRecordCount++;

		char temp[64];
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		bool CurrentRed = false;
		bool CurrentLow = false;
		bool CurrentHigh= false;
		CJulianTime Julian(temp);

		bool bStale = true;

		//if this record has a newer time stamp then process it		//DR00406
		if (m_uiStatusRecTime < Julian.TotalSeconds())				//DR00406
		{															//DR00406

			m_uiStatusRecTime = Julian.TotalSeconds();				//DR00406

			//build date/time string
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			generalpage->SetDlgItemText(IDC_ISRSETUP_TIME,temp);
		
			//status
			strncpy(temp,&Record[12],2);
			temp[2] = NULL;
			short Status;
			sscanf(temp,"%2hx",&Status);

			//bit 7 acpower
			if (Status & AS_ACPOWER)
			{
	//			m_pParent->m_pButton->SetOthr(true,true);
	//			m_pParent->FlagOthr(1,true);			
				strcpy(temp,"Fail");
				generalpage->SetColor(IDC_ISRSETUP_ACPOWER,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_ACPOWER,COLOR_RED);
				if (m_bACPowerOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22590 STATUS CHANGE - External POWER Off (21)\n\r");
				m_bACPowerOk = false;

				General_Tab |= 0x0001;
			}
			else 
			{
				strcpy(temp,"Ok");
	//			generalpage->SetColor(IDC_DATA_ACPOWER,COLOR_BLACK);
	//			summarypage->SetColor(IDC_ISRSUMMARY_ACPOWER,COLOR_BLACK);
				if (!m_bACPowerOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22600 STATUS CHANGE - External POWER On (21)\n\r");
				m_bACPowerOk = true;

	//			General_Tab &= ~(0x0001);
			}

			//ac power changed since last status record 
			if (Status & AS_ACLOSS)
			{
				strcat(temp," - with loss since last status record");

				if (!m_bACPowerLoss)
				{
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22610 STATUS CHANGE - External POWER LOSS Since last status record (21)\n\r");
					m_bACPowerLoss = true;
				}
			}
			else
				m_bACPowerLoss = false;
					
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_ACPOWER,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_ACPOWER,StripLeadingBlanks(temp));
		
			//bit 6 battery 
			if (Status & AS_BATTERY) 
			{
	//			m_pParent->m_pButton->SetOthr(true,true);
				m_pParent->FlagOthr(1,true);			
				generalpage->SetDlgItemText(IDC_ISRSETUP_BATTERY,"Low");
				generalpage->SetColor(IDC_ISRSETUP_BATTERY,COLOR_RED);
				summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERYOK,"Low");
				summarypage->SetColor(IDC_ISRSUMMARY_BATTERYOK,COLOR_RED);
				if (m_bBatteryOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22620 STATUS CHANGE - BATTERY LOW  (21)\n\r");
				m_bBatteryOk = false;
				General_Tab |= 0x0002;
			}
			else
			{
				generalpage->SetDlgItemText(IDC_ISRSETUP_BATTERY,"Ok");
				summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERYOK,"Ok");
	//			datapage->SetColor(IDC_DATA_BATTERYOK,COLOR_BLACK);
	//			summarypage->SetColor(IDC_ISRSUMMARY_BATTERYOK,COLOR_BLACK);
				if (!m_bBatteryOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22630 STATUS CHANGE - BATTERY OK  (21)\n\r");
				m_bBatteryOk = true;
	//			General_Tab &= ~(0x0002);
			}


			//baud rate
			strncpy(temp,&Record[14],5);
			temp[5] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_BAUD,StripLeadingBlanks(temp));

			//Port setting
			strncpy(temp,&Record[19],2);
			temp[2] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_PORTSET,StripLeadingBlanks(temp));
		
			//count time
			strncpy(temp,&Record[21],8);
			temp[8] = temp[7];
			temp[7] = '.';
			temp[9] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_COUNTT,StripLeadingBlanks(temp));

			//pre-delay
			strncpy(temp,&Record[29],7);
			temp[7] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_PREDELAY,StripLeadingBlanks(temp));

			//Gate width
			strncpy(temp,&Record[36],7);
			temp[7] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_GATEWIDTH,StripLeadingBlanks(temp));

			//HV SetPoint
			strncpy(temp,&Record[43],4);
			temp[4] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_HVSETPOINT,StripLeadingBlanks(temp));

			//HIGH voltage
			strncpy(temp,&Record[47],4);
			temp[4] = NULL;
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
				generalpage->SetColor(IDC_ISRSETUP_HV2,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_HIGHVOLT2,COLOR_RED);
				if (m_bHighVoltOk)
	 				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22640 STATUS CHANGE - HIGH VOLTAGE OUT OF TOLERANCE (21)\n\r");
				m_bHighVoltOk = false;
				General_Tab |= 0x0010;
			}
			else
			{
				CurrentRed = false;
				if (!m_bHighVoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22650 STATUS CHANGE - HIGH VOLTAGE IN TOLERANCE (21)\n\r");
				m_bHighVoltOk = true;
	//			General_Tab &= ~(0x0010);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_dLowHV <= m_dHighVoltL) || (m_dLowHV >= m_dHighVoltH))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_HV1,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_HIGHVOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_dHighHV <= m_dHighVoltL) || (m_dHighHV >= m_dHighVoltH))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_HV3,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_HIGHVOLT3,COLOR_RED);
			}

			sprintf(temp,"%d",m_dLowHV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT1,temp);
			generalpage->SetDlgItemText(IDC_ISRSETUP_HV1,temp);
			sprintf(temp,"%d",m_dCurHV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT2,temp);
			generalpage->SetDlgItemText(IDC_ISRSETUP_HV2,temp);
			sprintf(temp,"%d",m_dHighHV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT3,temp);
			generalpage->SetDlgItemText(IDC_ISRSETUP_HV3,temp);

			//Battery voltage
			strncpy(temp,&Record[51],4);
			temp[4] = NULL;
			m_fCurBV=(float)atof(temp);
			if (m_bFirstBV)
			{
				m_bFirstBV = false;
				m_fLowBV = m_fHighBV = m_fCurBV;
			}
			if (m_fCurBV < m_fLowBV)
			{
				CurrentLow = true;
				m_fLowBV = m_fCurBV;
			}
			else
				CurrentLow = false;
			if (m_fCurBV > m_fHighBV)
			{
				CurrentHigh = true;
				m_fHighBV= m_fCurBV;
			}
			else
				CurrentHigh = false;
			if ((m_fCurBV <= m_fBatVoltL) || (m_fCurBV >= m_fBatVoltH))
			{
				CurrentRed = true;
				m_pParent->FlagOthr(1,true);			
				generalpage->SetColor(IDC_ISRSETUP_BV2,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_BATTERY2,COLOR_RED);
				if (m_bBatteryTolOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22660 STATUS CHANGE - BATTERY OUT OF TOLERANCE (21)\n\r");
				m_bBatteryTolOk = false;
				General_Tab |= 0x0010;
			}
			else
			{
				CurrentRed = false;
				if (!m_bBatteryTolOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22670 STATUS CHANGE - BATTERY IN TOLERANCE (21)\n\r");
				m_bBatteryTolOk = true;
	//			General_Tab &= ~(0x0010);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowBV <= m_fBatVoltL) || (m_fLowBV >= m_fBatVoltH))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_BV1,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_BATTERY1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighBV <= m_fBatVoltL) || (m_fHighBV >= m_fBatVoltH))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_BV3,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_BATTERY3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowBV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERY1,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_BV1,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fCurBV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERY2,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_BV2,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fHighBV);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERY3,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_BV3,StripLeadingBlanks(temp));
			
			//+5 volt
			strncpy(temp,&Record[55],4);
			temp[4] = NULL;
			m_fCurP5=(float)atof(temp);
			if (m_bFirstP5)
			{
				m_bFirstP5 = false;
				m_fLowP5 = m_fHighP5 = m_fCurP5;
			}
			if (m_fCurP5 < m_fLowP5)
			{
				CurrentLow = true;
				m_fLowP5 = m_fCurP5;
			}
			else
				CurrentLow = false;
			if (m_fCurP5 > m_fHighP5)
			{
				CurrentHigh = true;
				m_fHighP5= m_fCurP5;
			}
			else
				CurrentHigh = false;
			if ((m_fCurP5 <= m_fSup_P05L) || (m_fCurP5 >= m_fSup_P05H))
			{
				CurrentRed = true;
				m_pParent->FlagOthr(1,true);			
				generalpage->SetColor(IDC_ISRSETUP_P5VOLT2,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P5VOLT2,COLOR_RED);
				if (m_bP5VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22680 STATUS CHANGE - PLUS 5 VOLT SUPPLY OUT OF TOLERANCE (21)\n\r");
				m_bP5VoltOk = false;
				General_Tab |= 0x0020;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP5VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22690 STATUS CHANGE - PLUS 5 VOLT SUPPLY NOT OUT OF TOLERANCE (21)\n\r");
				m_bP5VoltOk = true;
	//			General_Tab &= ~(0x0020);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowP5 <= m_fSup_P05L) || (m_fLowP5 >= m_fSup_P05H))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_P5VOLT1,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P5VOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighP5 <= m_fSup_P05L) || (m_fHighP5 >= m_fSup_P05H))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_P5VOLT3,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P5VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP5);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P5VOLT1,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P5VOLT1,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fCurP5);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P5VOLT2,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P5VOLT2,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fHighP5);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P5VOLT3,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P5VOLT3,StripLeadingBlanks(temp));

			//+15 volts
			strncpy(temp,&Record[59],4);
			temp[4] = NULL;
			m_fCurP15=(float)atof(temp);
			if (m_bFirstP15)
			{
				m_bFirstP15 = false;
				m_fLowP15 = m_fHighP15 = m_fCurP15;
			}
			if (m_fCurP15 < m_fLowP15)
			{
				CurrentLow = true;
				m_fLowP15 = m_fCurP15;
			}
			else
				CurrentLow = false;
			if (m_fCurP15 > m_fHighP15)
			{
				CurrentHigh = true;
				m_fHighP15= m_fCurP15;
			}
			else
				CurrentHigh = false;
			if ((m_fCurP15 <= m_fSup_P15L) || (m_fCurP15 >= m_fSup_P15H))
			{
				CurrentRed = true;
				m_pParent->FlagOthr(1,true);			
				generalpage->SetColor(IDC_ISRSETUP_P15VOLT2,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P15VOLT2,COLOR_RED);
				if (m_bP15VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22700 STATUS CHANGE - PLUS 15 VOLT SUPPLY OUT OF TOLERANCE (21)\n\r");
				m_bP15VoltOk = false;
				General_Tab |= 0x0040;
			}
			else
			{
				CurrentRed = false;
				if (!m_bP15VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22710 STATUS CHANGE - PLUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (21)\n\r");
				m_bP15VoltOk = true;
	//			General_Tab &= ~(0x0040);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowP15 <= m_fSup_P15L) || (m_fLowP15 >= m_fSup_P15H))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_P15VOLT1,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P15VOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighP15 <= m_fSup_P15L) || (m_fHighP15 >= m_fSup_P15H))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_P15VOLT3,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_P15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowP15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P15VOLT1,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P15VOLT1,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fCurP15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P15VOLT2,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P15VOLT2,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fHighP15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_P15VOLT3,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_P15VOLT3,StripLeadingBlanks(temp));

			//-15 volts
			strncpy(temp,&Record[63],5);
			temp[5] = NULL;
			m_fCurM15=(float)atof(temp);
			if (m_bFirstM15)
			{
				m_bFirstM15 = false;
				m_fLowM15 = m_fHighM15 = m_fCurM15;
			}
			if (m_fCurM15 < m_fLowM15)
			{
				CurrentLow = true;
				m_fLowM15 = m_fCurM15;
			}
			else
				CurrentLow = false;
			if (m_fCurM15 > m_fHighM15)
			{
				CurrentHigh = true;
				m_fHighM15= m_fCurM15;
			}
			else
				CurrentHigh = false;
			if ((m_fCurM15 <= m_fSup_M15L) || (m_fCurM15 >= m_fSup_M15H))
			{
				CurrentRed = true;
				m_pParent->FlagOthr(1,true);			
				generalpage->SetColor(IDC_ISRSETUP_M15VOLT2,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_M15VOLT2,COLOR_RED);
				if (m_bM15VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22720 STATUS CHANGE - MINUS 15 VOLT SUPPLY OUT OF TOLERANCE (21)\n\r");
				m_bM15VoltOk = false;
				General_Tab |= 0x0080;
			}
			else
			{
				CurrentRed = false;
				if (!m_bM15VoltOk)
					WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22730 STATUS CHANGE - MINUS 15 VOLT SUPPLY NOT OUT OF TOLERANCE (21)\n\r");
				m_bM15VoltOk = true;
	//			General_Tab &= ~(0x0080);
			}

			if (
				CurrentRed && CurrentLow &&
				((m_fLowM15 <= m_fSup_M15L) || (m_fLowM15 >= m_fSup_M15H))
			   )
			{
				generalpage->SetColor(IDC_ISRSETUP_M15VOLT1,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_M15VOLT1,COLOR_RED);
			}

			if (
				CurrentRed && CurrentHigh &&
				((m_fHighM15 <= m_fSup_M15L) || (m_fHighM15 >= m_fSup_M15H))
				)
			{
				generalpage->SetColor(IDC_ISRSETUP_M15VOLT3,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_M15VOLT3,COLOR_RED);
			}

			sprintf(temp,"%5.1f",m_fLowM15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_M15VOLT1,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_M15VOLT1,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fCurM15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_M15VOLT2,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_M15VOLT2,StripLeadingBlanks(temp));
			sprintf(temp,"%5.1f",m_fHighM15);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_M15VOLT3,StripLeadingBlanks(temp));
			generalpage->SetDlgItemText(IDC_ISRSETUP_M15VOLT3,StripLeadingBlanks(temp));

			//temperature ON board
			strncpy(temp,&Record[68],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_TEMPONN,StripLeadingBlanks(temp));

			//temperature OFF board
			strncpy(temp,&Record[71],3);
			temp[3] = NULL;
			generalpage->SetDlgItemText(IDC_ISRSETUP_TEMPOFF,StripLeadingBlanks(temp));

			if (General_Tab) SetTabColor(IDC_ISRSETUP_TAB,COLOR_RED);

			bStale = false;										//DR00406
		}														//DR00406

		char cBuff[1024];
		strcpy(cBuff,"22740 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);	//DR00406
//		summarypage->Invalidate();
//		generalpage->Invalidate();
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRSTATUS_RECORD,CheckSum);
}

//record 20 Acquire SR Record
void CISRPropertySheet::AcquirRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRACQUIRE_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		//get pointers to page 2 and summary page
//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;

		char temp[32];
		char show[32];
		char *test;
 		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		AcquireTime[m_dAcquireRecordCount].Init(temp);
		//CJulianTime Julian(temp);
		
		//save time in binary to acquire record
		ISRAcquireRec[m_dAcquireRecordCount].JulianSeconds = 
			AcquireTime[m_dAcquireRecordCount].TotalSeconds();

		//skip Julian time aux byte for now
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIMEAUX4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIMEAUX3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIMEAUX2,temp);
		temp[0] = Record[12];
		temp[1] = NULL;
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIMEAUX1,temp);

		//get status byte 
		short Status;
		strncpy(temp,&Record[13],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status);

		//save status in binary to acquire record
		ISRAcquireRec[m_dAcquireRecordCount].StatusByte = Status;

		//move all the date displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_DATADATE3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATADATE4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATADATE2,temp,sizeof(temp));	
		datapage->SetDlgItemText(IDC_ISRDATA_DATADATE3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATADATE1,temp,sizeof(temp));	
		datapage->SetDlgItemText(IDC_ISRDATA_DATADATE2,temp);
		datapage->SetDlgItemText(IDC_ISRDATA_DATADATE1,AcquireTime[m_dAcquireRecordCount].Date());
	
		//move all the time displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIME3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIME4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIME2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIME3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_DATATIME1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIME2,temp);
		datapage->SetDlgItemText(IDC_ISRDATA_DATATIME1,AcquireTime[m_dAcquireRecordCount].Time());
	
		//move all the Battery displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_BATTERY3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_BATTERY4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_BATTERY2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_BATTERY3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_BATTERY1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_BATTERY2,temp);
		if (ISRAcquireRec[m_dAcquireRecordCount].StatusByte && AS_BATTERY)
			strcpy(temp,"LOW");
		else
			strcpy(temp,"OK");
		strcat(temp," - ");
		if (ISRAcquireRec[m_dAcquireRecordCount].StatusByte && AS_ACPOWER)
			strcat(temp,"BAD");
		else
			strcat(temp,"OK");
		datapage->SetDlgItemText(IDC_ISRDATA_BATTERY1,temp);
//			(ISRAcquireRec[m_dAcquireRecordCount].StatusByte && AS_BATTERY)?"LOW":"OK");

		//need to do this out of order so that we may
		//decide how to correctly do the others
		//move all the elapsed time displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_ETIME3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ETIME4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_ETIME2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ETIME3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_ETIME1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ETIME2,temp);
		strncpy(temp,&Record[76],8);
		temp[8] = temp[7];
		temp[7] = '.';
		temp[9] = NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		datapage->SetDlgItemText(IDC_ISRDATA_ETIME1,test);
		ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime = atof(test);
		bool bDoingRaw = ((CButton*)(datapage->GetDlgItem(IDC_ISRRADIO_RAW)))->GetCheck()==1;

		if (ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime <= 0)
			bDoingRaw = true;
		
		//move all the Totals displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_TOTALS3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_TOTALS4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_TOTALS2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_TOTALS3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_TOTALS1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_TOTALS2,temp);
		strncpy(temp,&Record[15],11);
		temp[11] = NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		ISRAcquireRec[m_dAcquireRecordCount].TotalsCount = atof(test);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_ISRDATA_TOTALS1,test);
		else
		{
			sprintf(show,"%11.4e",atof(test)/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_TOTALS1,show);
		}

//		while (temp[0] == ' ') memmove(temp,&temp[1],11);
//		datapage->SetDlgItemText(IDC_ISRDATA_TOTALS1,temp);
//		ISRAcquireRec[m_dAcquireRecordCount].TotalsCount = atof(temp);

		//move all the Aux 1 Totals rate displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS2,temp);
		strncpy(temp,&Record[26],10);
		temp[10]=NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		ISRAcquireRec[m_dAcquireRecordCount].Totals2Count = atof(test);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS1,test);
		else
		{
			sprintf(show,"%11.4e",atof(test)/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS1,show);
		}

//		while (temp[0] == ' ') memmove(temp,&temp[1],10);
//		datapage->SetDlgItemText(IDC_ISRDATA_AUX1TOTALS1,temp);
//		ISRAcquireRec[m_dAcquireRecordCount].Totals2Count = atof(temp);
	
		//move all the Aux 2 displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS2,temp);
		strncpy(temp,&Record[36],10);
		temp[10]=NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		ISRAcquireRec[m_dAcquireRecordCount].Totals3Count = atof(test);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS1,test);
		else
		{
			sprintf(show,"%11.4e",atof(test)/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS1,show);
		}

//		while (temp[0] == ' ') memmove(temp,&temp[1],10);
//		datapage->SetDlgItemText(IDC_ISRDATA_AUX2TOTALS1,temp);
//		//set the acquire record value
//		ISRAcquireRec[m_dAcquireRecordCount].Totals3Count = atof(temp);
	
		//move all the Reals + Accidentals plays down one
		datapage->GetDlgItemText(IDC_ISRDATA_REALACC3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REALACC4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_REALACC2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REALACC3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_REALACC1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REALACC2,temp);
		strncpy(temp,&Record[46],15);
		temp[15]=NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		ISRAcquireRec[m_dAcquireRecordCount].RealAccCount = atof(test);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_ISRDATA_REALACC1,test);
		else
		{
			sprintf(show,"%11.4e",atof(test)/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_REALACC1,show);
		}

//		while (temp[0] == ' ') memmove(temp,&temp[1],15);
//		datapage->SetDlgItemText(IDC_ISRDATA_REALACC1,temp);
//		//set the acquire record value
//		ISRAcquireRec[m_dAcquireRecordCount].RealAccCount = atof(temp);
	
		//move all the Accidentals displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ACCIDENT4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ACCIDENT3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_ACCIDENT2,temp);
		strncpy(temp,&Record[61],15);
		temp[15]=NULL;
		test = temp;
		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		ISRAcquireRec[m_dAcquireRecordCount].AccidenCount = atof(test);
		if (bDoingRaw)
			datapage->SetDlgItemText(IDC_ISRDATA_ACCIDENT1,test);
		else
		{
			sprintf(show,"%11.4e",atof(test)/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_ACCIDENT1,show);
		}

	
		//move all the Accidentals displays down one
		datapage->GetDlgItemText(IDC_ISRDATA_REAL3,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REAL4,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_REAL2,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REAL3,temp);
		datapage->GetDlgItemText(IDC_ISRDATA_REAL1,temp,sizeof(temp));
		datapage->SetDlgItemText(IDC_ISRDATA_REAL2,temp);
//		strncpy(temp,&Record[61],15);
//		temp[15]=NULL;
//		test = temp;
//		while ((test[0] == ' ')&&(test[0]!=NULL))test++;
		double Reals = ISRAcquireRec[m_dAcquireRecordCount].RealAccCount - ISRAcquireRec[m_dAcquireRecordCount].AccidenCount;
		if (bDoingRaw) 
		{
			sprintf(show,"%d",(int)Reals);
			datapage->SetDlgItemText(IDC_ISRDATA_REAL1,show);
		}
		else
		{
			sprintf(show,"%11.4e",Reals/(double)ISRAcquireRec[m_dAcquireRecordCount].ElapsedTime);
			datapage->SetDlgItemText(IDC_ISRDATA_REAL1,show);
		}

	
		//if not in dumplast sequence then write this record to ISR array
		m_dMessageRecordCount++;
		if (m_pParent->m_bInDumpBBM)
		{
//			WriteToFile(&Julian,&AcquireRec);//acquire rec 20
			m_dAcquireRecordCount++;
			if (m_dAcquireRecordCount >= NUMBEROFACQUIRERECS) 
				m_dAcquireRecordCount = NUMBEROFACQUIRERECS-1;
		}
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRACQUIRE_RECORD,CheckSum);	
}

//record 16
void CISRPropertySheet::DumpEnd(char *Record)
{
	int CheckSum;
	if (LengthOk(ISREND_DUMP_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	m_pParent->m_dNumberOfAcquires = -1;
	m_pParent->PostMessage(uIDC_RECEIVE,ISREND_DUMP_RECORD,CheckSum);
}

//record 24
void CISRPropertySheet::BusyRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRBUSY_RECORD_LENGTH, Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
		m_pParent->PostMessage(uIDC_RECEIVE,ISRBUSY_RECORD,
		(toupper(Record[2]) == 'B')?(CheckSum+BUSY):(CheckSum+NOTBUSY));
	else
		m_pParent->PostMessage(uIDC_RECEIVE,ISRBUSY_RECORD,CheckSum);
}


//record 61
void CISRPropertySheet::DumpEnd2(char *Record)
{
	int CheckSum;
	if (LengthOk(ISREND_DUMP2_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	char temp[2];
	if (CheckSum == CHECKSUM_GOOD)
	{
		temp[0] = Record[2];
		temp[1] = NULL;
		sscanf(temp,"%1x",&(m_pParent->m_dNumberOfAcquires));
	}
	else
	{
		m_pParent->m_dNumberOfAcquires = -1;
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISREND_DUMP2_RECORD,CheckSum);
}

//record 62
void CISRPropertySheet::DumpEnd3(char *Record)
{
	int CheckSum;
	if (LengthOk(ISREND_DUMP3_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	char temp[3];
	if (CheckSum == CHECKSUM_GOOD)
	{
		temp[0] = Record[2];
		temp[1] = Record[3];
		temp[2] = NULL;
		sscanf(temp,"%2x",&m_pParent->m_dNumberOfMessages);
	}
	else
	{
		m_pParent->m_dNumberOfMessages = -1;
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISREND_DUMP3_RECORD,CheckSum);
}

//record 17
void CISRPropertySheet::DumpOk(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRDUMP_OK_RECORD_LENGTH,Record)) 
//	if (LengthOk(ISRDUMP_OK_RECORD_LENGTH,Record) ||
//		LengthOk(ISRDUMP_OK_RECORD_LENGTH+1,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[8];

//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		bbm = atoi(temp);
		m_pCloseButton->SetPreNum("BBM:",false);
		m_pCloseButton->SetNumeric(bbm,true);

		BSTR bstr;
		Convert_Character_String_to_BSTR("BBM: ",bstr);
		m_pParent->m_pButton->put_PreNumericString(bstr);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
		datapage->SetDlgItemInt(IDC_DATA_BYTESMEM,bbm);
	
		//check checksum
			//send message to instrument if bad
		//send message to parent saying we got this record
		//use second parameter as flag for good or bad checksum
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRDUMP_OK_RECORD,CheckSum);
}

//record 19
void CISRPropertySheet::DumpOk2(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRDUMP_OK2_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

		char temp[32];

//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		//get bbm bytes
		int bbm;
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		bbm = atoi(temp);
		int files = 0;
		strncpy(temp,&Record[8],2);
		temp[2] = NULL;
		sscanf(temp,"%x",&files);
		sprintf(temp,"F:%d B:",files);
		m_pCloseButton->SetPreNum(temp,false);
		m_pCloseButton->SetNumeric(bbm,true);

		//m_pParent->m_pButton->SetPreNum(temp,false);
		//m_pParent->m_pButton->SetNumeric(bbm,true);
		BSTR bstr;
		Convert_Character_String_to_BSTR(temp,bstr);
		m_pParent->m_pButton->put_PreNumericString(bstr);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);

		sprintf(temp,"%d, Files: %d",bbm,files);
		datapage->SetDlgItemText(IDC_DATA_BYTESMEM,temp);	
		//check checksum
			//send message to instrument if bad
		//send message to parent saying we got this record
		//use second parameter as flag for good or bad checksum
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_FILE_VALUE,files);
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRDUMP_OK2_RECORD,CheckSum);
}

//record 18 Battery Record
void CISRPropertySheet::BatteryRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRBATTERY_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];

//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		
		m_dMessageRecordCount++;
		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		
		char cBuff[1024];
		strcpy(cBuff,"22750 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRBATTERY_RECORD,CheckSum);
}

//record 22 ISR MONITOR PARAMETERS STATUS RECORD (USER PROGRAM STATUS)
void CISRPropertySheet::MonStatRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRMON_STATUS_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{

//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		
		char temp[64];

		m_dMessageRecordCount++;
		//get julian time and date
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		bool bStale = true;											//DR00406

		//if this record has a newer time stamp then process it		//DR00406
		if (m_uiMonStatRecTime < Julian.TotalSeconds())				//DR00406
		{															//DR00406
			m_uiMonStatRecTime = Julian.TotalSeconds();				//DR00406

			//build date/time string
			strcpy(temp,Julian.Date());
			strcat(temp," ");
			strcat(temp,Julian.Time());
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TIME,temp);

	//		//Monitor version number
	//		strncpy(temp,&Record[12],5);
	//		temp[5] = NULL;
	//		userpage->SetDlgItemText(IDC_ISRUSERPROG_ISRUV,temp);
		
			//Battery Check Interval
			strncpy(temp,&Record[17],2);
			temp[2] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_LOGBATT,temp);

			//Status Check Interval
			strncpy(temp,&Record[19],4);
			temp[4] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_STATCHECK,temp);

			//Filterning
			short ChanUse;
			strncpy(temp,&Record[23],2);
			temp[2]=NULL;
			sscanf(temp,"%2hx",&ChanUse);
			strcpy(temp,(ChanUse & 0x0010)?"Yes / ":"No / ");
			strcat(temp,(ChanUse & 0x0001)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_REALSF, temp);
			strcpy(temp,(ChanUse & 0x0020)?"Yes / ":"No / ");
			strcat(temp,(ChanUse & 0x0002)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TOTALSF,temp);
			strcpy(temp,(ChanUse & 0x0040)?"Yes / ":"No / ");
			strcat(temp,(ChanUse & 0x0004)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX1F,  temp);
			strcpy(temp,(ChanUse & 0x0080)?"Yes / ":"No / ");
			strcat(temp,(ChanUse & 0x0008)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX2F,  temp);

			strncpy(temp,&Record[25],2); 
			temp[2]=NULL;
			sscanf(temp,"%2hx",&ChanUse);
			userpage->SetDlgItemText(IDC_ISRUSERPROG_REALS3ST, (ChanUse & 0x0001)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TOTALS3ST,(ChanUse & 0x0002)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX13ST,  (ChanUse & 0x0004)?"Yes":"No");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX23ST,  (ChanUse & 0x0008)?"Yes":"No");

			//Channel 0 (reals)
			//High Threshold
			strncpy(temp,&Record[27],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_REALSTH,temp);

			//Low Threshold
			strncpy(temp,&Record[33],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_REALSTL,temp);

			//Channel 1 (totals)
			//High Threshold
			strncpy(temp,&Record[39],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TOTALSTH,temp);

			//Low Threshold
			strncpy(temp,&Record[45],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TOTALSTL,temp);

			//Channel 2 (aux 1)
			//High Threshold
			strncpy(temp,&Record[51],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX1TH,temp);

			//Low Threshold
			strncpy(temp,&Record[57],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX1TL,temp);

			//Channel 3 (aux 2)
			//High Threshold
			strncpy(temp,&Record[63],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX2TH,temp);

			//Low Threshold
			strncpy(temp,&Record[69],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_AUX2TL,temp);

			//Immediate byffer size
			strncpy(temp,"Total: ",7);
			strncpy(&temp[7],&Record[75],2);
			strncpy(&temp[9],", Save: ",8);
			strncpy(&temp[17],&Record[77],2);
			temp[19] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_IMMEDIATE,temp);

			//Local background
			strcpy(temp,"First ");
			strncpy(&temp[6],&Record[79],2);
			strncpy(&temp[8]," of last ",9);
			strncpy(&temp[17],&Record[81],2);
			temp[19] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_LOCALBACK,temp);

			//Filter Enabled
			userpage->SetDlgItemText(IDC_ISRUSERPROG_FILTERENAB,
				(Record[83]=='0')?"No":"Yes");

			//Filter Method
			userpage->SetDlgItemText(IDC_ISRUSERPROG_FILTERMETH,
				(Record[84]=='0')?"1st Point":"Average Point");

			//No Filter and Enter MII Condition
			strncpy(temp,&Record[85],2);
			strncpy(&temp[2]," runs > ",8);
			strncpy(&temp[10],&Record[87],3);
			strncpy(&temp[13]," sigma from background",22);
			temp[35] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_NOFILTER,temp);

			char *test;
			//trigger 0
			//Trigger Enabled
			strcpy(temp,"0 - ");
			if (Record[90] == '0')
				strcat(temp,"No");
			else
				strcat(temp,"Yes");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGCHENAB0,temp);

			//Trigger Type
			if (Record[91]=='0')
				strcpy(temp,"Level ");
			else
				strcpy(temp,"Pulse ");

			//Trigger polarity
			if (Record[92]=='0')
				strcat(temp,"Low");
			else
				strcat(temp,"High");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR0,temp);

			//Trigger delay
			strncpy(temp,&Record[93],3);
			temp[3] = NULL;
			strcat(temp," & ");
			//Trigger pulse width
			strncat(temp,&Record[96],3);
			temp[9] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH0,temp);

			//Trigger High threshold
			strncpy(temp,&Record[99],6);
			temp[6] = NULL;
			test = temp;
			while ((test[0] == ' ') && (test[0] != NULL)) test++;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGHIGHT0,test);

			//trigger 1
			//Trigger Enabled
			strcpy(temp,"1 - ");
			if (Record[105] == '0')
				strcat(temp,"No");
			else
				strcat(temp,"Yes");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGCHENAB1,temp);

			//Trigger Type
			if (Record[106]=='0')
				strcpy(temp,"Level ");
			else
				strcpy(temp,"Pulse ");

			//Trigger polarity
			if (Record[107]=='0')
				strcat(temp,"Low");
			else
				strcat(temp,"High");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR1,temp);

			//Trigger delay
			strncpy(temp,&Record[108],3);
			temp[3] = NULL;
			strcat(temp," & ");
			//Trigger pulse width
			strncat(temp,&Record[111],3);
			temp[9] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH1,temp);

			//Trigger High threshold
			strncpy(temp,&Record[114],6);
			temp[6] = NULL;
			test = temp;
			while ((test[0] == ' ') && (test[0] != NULL)) test++;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGHIGHT1,test);

			//trigger 2
			//Trigger Enabled
			strcpy(temp,"2 - ");
			if (Record[120] == '0')
				strcat(temp,"No");
			else
				strcat(temp,"Yes");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGCHENAB2,temp);

			//Trigger Type
			if (Record[121]=='0')
				strcpy(temp,"Level ");
			else
				strcpy(temp,"Pulse ");

			//Trigger polarity
			if (Record[122]=='0')
				strcat(temp,"Low");
			else
				strcat(temp,"High");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR2,temp);

			//Trigger delay
			strncpy(temp,&Record[123],3);
			temp[3] = NULL;
			strcat(temp," & ");
			//Trigger pulse width
			strncat(temp,&Record[126],3);
			temp[9] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH2,temp);

			//Trigger High threshold
			strncpy(temp,&Record[129],6);
			temp[6] = NULL;
			test = temp;
			while ((test[0] == ' ') && (test[0] != NULL)) test++;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGHIGHT2,test);


			//trigger 3
			//Trigger Enabled
			strcpy(temp,"3 - ");
			if (Record[135] == '0')
				strcat(temp,"No");
			else
				strcat(temp,"Yes");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGCHENAB3,temp);

			//Trigger Type
			if (Record[136]=='0')
				strcpy(temp,"Level ");
			else
				strcpy(temp,"Pulse ");

			//Trigger polarity
			if (Record[137]=='0')
				strcat(temp,"Low");
			else
				strcat(temp,"High");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR3,temp);

			//Trigger delay
			strncpy(temp,&Record[138],3);
			temp[3] = NULL;
			strcat(temp," & ");
			//Trigger pulse width
			strncat(temp,&Record[141],3);
			temp[9] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH3,temp);

			//Trigger High threshold
			strncpy(temp,&Record[144],6);
			temp[6] = NULL;
			test = temp;
			while ((test[0] == ' ') && (test[0] != NULL)) test++;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGHIGHT3,test);

			//trigger 4
			//Trigger Enabled
			strcpy(temp,"4 - ");
			if (Record[150] == '0')
				strcat(temp,"No");
			else
				strcat(temp,"Yes");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGCHENAB4,temp);

			//Trigger Type
			if (Record[151]=='0')
				strcpy(temp,"Level ");
			else
				strcpy(temp,"Pulse ");

			//Trigger polarity
			if (Record[152]=='0')
				strcat(temp,"Low");
			else
				strcat(temp,"High");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR4,temp);

			//Trigger delay
			strncpy(temp,&Record[153],3);
			temp[3] = NULL;
			strcat(temp," & ");
			//Trigger pulse width
			strncat(temp,&Record[156],3);
			temp[9] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH4,temp);

			//Trigger High threshold
			strncpy(temp,&Record[159],6);
			temp[6] = NULL;
			test = temp;
			while ((test[0] == ' ') && (test[0] != NULL)) test++;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_TRIGHIGHT4,test);

			//A/T test totals threshold
			strncpy(temp,&Record[165],6);
			temp[6] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_ATTESTTH,temp);

			//A/T test bias limit
			strncpy(temp,&Record[171],4);
			temp[4] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_ATTESTBIAS,temp);

			//A/T Test Sigma Limit
			strncpy(temp,&Record[175],3);
			temp[3] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_ATTESTLIMIT,temp);

			//Synchronization enable & interval
			if (Record[178]=='0')
				strcpy(temp,"No - ");
			else
				strcpy(temp,"Yes - ");
			strcat(temp,(Record[185]=='1')?"Daily":"Hourly");
			userpage->SetDlgItemText(IDC_ISRUSERPROG_SYNCE,temp);

			//Sync time
			strncpy(temp,&Record[179],2);
			temp[2] = ':';
			strncpy(&temp[3],&Record[181],2);
			temp[5] = ':';
			strncpy(&temp[6],&Record[183],2);
			temp[8] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_SYNCT,temp);

			//sync lower limit
			strncpy(temp,&Record[186],2);
			temp[2] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_SYNCL,temp);

			//sync upper limit
			strncpy(temp,&Record[188],2);
			temp[2] = NULL;
			userpage->SetDlgItemText(IDC_ISRUSERPROG_SYNCU,temp);

	//		//ROM checksum
	//		strncpy(temp,&Record[190],4);
	//		temp[4] = NULL;
	//		userpage->SetDlgItemText(IDC_ISRUSERPROG_ROMCHECK,temp);

			bStale = false;											//DR00406
		}															//DR00406

		char cBuff[1024];
		strcpy(cBuff,"22760 ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff, bStale);//record 22		//DR00406
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRMON_STATUS_RECORD,CheckSum);
}

//record 23
void CISRPropertySheet::TrigSigRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRTRIG_SIG_RECORD_LENGTH,Record))
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
//		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;

		char temp[64];

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		short State;
		strncpy(temp,&Record[13],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&State);
		if (State & 0x0080)
			strcpy(temp,"Active ");
		else
			strcpy(temp,"Inactive "); 

		strcat(temp, Julian.Date());
		strcat(temp," ");
		strcat(temp, Julian.Time());
		
		UINT OutCtrl1,OutCtrl2;
		switch (State & 0x000f) {
		case 0:
			OutCtrl1 = IDC_ISRSETUP_CH0A;
			OutCtrl2 = IDC_ISRSETUP_CH0B;
			break;
		case 1:
			OutCtrl1 = IDC_ISRSETUP_CH1A;
			OutCtrl2 = IDC_ISRSETUP_CH1B;
			break;
		case 2:
			OutCtrl1 = IDC_ISRSETUP_CH2A;
			OutCtrl2 = IDC_ISRSETUP_CH2B;
			break;
		case 3:
			OutCtrl1 = IDC_ISRSETUP_CH3A;
			OutCtrl2 = IDC_ISRSETUP_CH3B;
			break;
		case 4:
			OutCtrl1 = IDC_ISRSETUP_CH4A;
			OutCtrl2 = IDC_ISRSETUP_CH4B;
			break;
		default:
			OutCtrl1 = 0;
			OutCtrl2 = 0;
		}

		if (OutCtrl1)
		{
			char Temp1[64];
			generalpage->GetDlgItemText(OutCtrl1,Temp1,sizeof(Temp1));
			generalpage->SetDlgItemText(OutCtrl2,Temp1);
			generalpage->SetDlgItemText(OutCtrl1,temp);
		}

		m_dMessageRecordCount++;

		char cBuff[1024];
		strcpy(cBuff,"22770 ");
		strcat(cBuff,Record);
		WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, cBuff);//record 23
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRTRIG_SIG_RECORD,CheckSum);	
}

//record 1e
void CISRPropertySheet::ID2_Record(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRMONITOR_ID2_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		bool AnyChange = false;

		CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//		CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
		CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//		CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//		CISRModParam* modparpage  = m_pParent->m_pModParams;
		
		char temp[64];
//		Data_Tab = 0;

		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);
		if (Julian.Year() < 1990)
		{
//			m_pParent->PostMessage(uIDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
			m_pParent->PostMessage(uIDC_RECEIVE,IDC_BADYEAR_VALUE,Julian.Year());
		}
		//post invalid time to CEV and PFM if necessary
		if (m_szPreviousGTime[0] != NULL) 
		{
			if (strcmp(m_szPreviousGTime,temp)>0)// string 1 lexographic greater then 2
			{
				m_pParent->m_dInvalidTimeCount++;
				CJulianTime JulianT(m_szPreviousGTime);

				if (m_pParent->m_dInvalidTimeCount < m_pParent->m_iMaxRepetition)
					WriteToFile(TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
				if (m_pParent->m_dInvalidTimeCount == m_pParent->m_iMaxRepetition)
				{
					WriteToFile(TO_CEV|TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
					WriteToFile(TO_CEV|TO_PFM, TYPE_INST,&JulianT,"22780 INVALID TIME Filter On\n\r");
				}
			
				//WriteToFile(TO_CEV | TO_PFM, TYPE_INVTIME, &JulianT, (char*)(&Julian));//id2 record 
//				m_pParent->m_pButton->SetTime(true,false);
				m_pParent->FlagTime(1,false);			
				datapage->SetColor(IDC_ISRDATA_INQUIRETIME,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_INQUIRETIME,COLOR_RED);
				Data_Tab |= 0x0020;
			}
			else
			{
				m_pParent->m_dInvalidTimeCount = 0;
			}
		}
		strcpy(m_szPreviousGTime,temp);
		if ((m_bStartOfFile)||(Julian.Day()!=m_dCurrentDay))
		{
			char cBuff[1024];
			strcpy(cBuff,"22800 ");
			strcat(cBuff,Record);
			WriteToFile(TO_CEV | TO_PFM, TYPE_GID2, &Julian, cBuff);//id2 record if start of file
			m_bStartOfFile = false;
			m_dCurrentDay = Julian.Day();
		}

		COleDateTime t1( Julian.Year(), Julian.Month(), Julian.Day(), 
			      Julian.Hour(), Julian.Minute(), Julian.Second() );
		COleDateTime t2 = COleDateTime::GetCurrentTime();
		COleDateTimeSpan ts = t1 - t2;  // Subtract 2 COleDateTimes
		ts += HALF_SECOND;
		strcpy(temp,Julian.Date());
		strcat(temp," ");
		strcat(temp,Julian.Time());


		//select only the  first response after the target time
		bool bDoBecauseOfDaily = false;
		if (m_pParent->m_bAutoTimeDaily)
		{
			unsigned uiSecInDay = t2.GetHour() * 3600 + t2.GetMinute() * 60 + t2.GetSecond();
			if (uiSecInDay < m_pParent->m_uiSecInDayAutoTime)
				m_pParent->m_bAutoTimeDailyTrigger = true;
			else
			{
				if (m_pParent->m_bAutoTimeDailyTrigger) bDoBecauseOfDaily = true;
				m_pParent->m_bAutoTimeDailyTrigger = false;
			}
		}

		if ((ts.GetTotalSeconds() > m_dTime_Err ) ||
			(ts.GetTotalSeconds() < -m_dTime_Err ))
		{
			if (m_pParent->m_bAutoTimeDaily)
			{
				m_pParent->FlagTime(1,false);			
				datapage->SetColor(IDC_ISRDATA_INQUIRETIME,COLOR_YELLOW);
				summarypage->SetColor(IDC_ISRSUMMARY_INQUIRETIME,COLOR_YELLOW);
				Data_Tab |= 0x0010;
				if (bDoBecauseOfDaily)
				{
					m_pParent->FlagTime(0,false);			
					datapage->SetColor(IDC_ISRDATA_INQUIRETIME,COLOR_DEFAULT_TEXT);
					summarypage->SetColor(IDC_ISRSUMMARY_INQUIRETIME,COLOR_DEFAULT_TEXT);
					m_pParent->PostMessage(uIDC_RECEIVE,IDC_TIMEOOT_VALUE,(long)ts.GetTotalSeconds());
					char cBuff[128];
					sprintf(cBuff,"16755 DOING DAILY TIME SET. Delta: %.0f\r\n",ts.GetTotalSeconds());
					WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, &Julian, cBuff);
				}
			}
			else
			{
				m_pParent->FlagTime(1,false);			
				datapage->SetColor(IDC_ISRDATA_INQUIRETIME,COLOR_RED);
				summarypage->SetColor(IDC_ISRSUMMARY_INQUIRETIME,COLOR_RED);
				Data_Tab |= 0x0010;
			}
			if (!m_bTimeError)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22810 STATUS CHANGE - Time Out of Tolerance (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian,"22830 Computer's Time (1e)\n\r");//id2 record
			}
			m_bTimeError = true;
		
		} 

		if ((ts.GetTotalSeconds() < m_dTime_Err ) &&
			(ts.GetTotalSeconds() > -m_dTime_Err ))
		{
//			datapage->SetColor(IDC_ISRDATA_INQUIRETIME,COLOR_BLACK);
//			summarypage->SetColor(IDC_ISRSUMMARY_INQUIRETIME,COLOR_BLACK);
//			Data_Tab &= ~(0x0008);
			if (m_bTimeError)
			{
//				AnyChange = true;
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22820 STATUS CHANGE - Time In Tolerance (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_TIME, &Julian,"22840 Computer's Time (1e)\n\r");
			}
			m_bTimeError = false;
		}

		long days = ts.GetDays();
		if (days > 0)
			strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		else if (days == 0)
			strcat(temp,ts.Format(" Delta: %H:%M:%S"));
		else 
		{
			sprintf(temp,"%s Delta: %ld days, %02d:%02d:%02d", temp,
				days, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());
		}

//		strcat(temp,ts.Format(" Delta: %D days, %H:%M:%S"));
		datapage->SetDlgItemText(IDC_ISRDATA_INQUIRETIME,temp);
		summarypage->SetDlgItemText(IDC_ISRSUMMARY_INQUIRETIME,temp);


		//get Main Unit ID
		strncpy(temp,&Record[12],2);
		temp[2]=NULL;
		datapage->SetDlgItemText(IDC_ISRDATA_UNITID,temp);
		
		//get status byte 1 and 2
		short Status1, Status2;
		strncpy(temp,&Record[14],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status1);
		strncpy(temp,&Record[16],2);
		temp[2]=NULL;
		sscanf(temp,"%2hx",&Status2);
	
		//bit 7 acpower
		if (Status1 & ID21_ACPOWER)
		{
			m_pParent->FlagOthr(1,true);			
			datapage->SetDlgItemText(IDC_ISRDATA_ACPOWER,"Fail");
			datapage->SetColor(IDC_ISRDATA_ACPOWER,COLOR_RED);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_ACPOWER,"Fail");
			summarypage->SetColor(IDC_ISRSUMMARY_ACPOWER,COLOR_RED);
			if (m_bACPowerOk) 
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22850 STATUS CHANGE - External Power Off (1e)\n\r");
				AnyChange = true;
			}
			m_bACPowerOk = false;
			Data_Tab |= 0x0001;
		}
		else 
		{
			datapage->SetDlgItemText(IDC_ISRDATA_ACPOWER,"Ok");
//			datapage->SetColor(IDC_ISRDATA_ACPOWER,COLOR_BLACK);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_ACPOWER,"Ok");
//			summarypage->SetColor(IDC_ISRSUMMARY_ACPOWER,COLOR_BLACK);
			if (!m_bACPowerOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian,"22860 STATUS CHANGE - External Power On (1e)\n\r");
				AnyChange = true;
			}
			m_bACPowerOk = true;
//			Data_Tab &= ~(0x0001);
		}
	
		//bit 6 battery
		if (Status1 & ID21_BATTERY) 
		{
			m_pParent->FlagOthr(1,true);			
			datapage->SetDlgItemText(IDC_ISRDATA_BATTERYOK,"Low");
			datapage->SetColor(IDC_ISRDATA_BATTERYOK,COLOR_RED);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERYOK,"Low");
			summarypage->SetColor(IDC_ISRSUMMARY_BATTERYOK,COLOR_RED);
			if (m_bBatteryOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22870 STATUS CHANGE - BATTERY LOW (1e)\n\r");
				AnyChange = true;
			}

			m_bBatteryOk = false;
			Data_Tab |= 0x0002;
		}
		else
		{
			datapage->SetDlgItemText(IDC_ISRDATA_BATTERYOK,"Ok");
//			datapage->SetColor(IDC_ISRDATA_BATTERYOK,COLOR_BLACK);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BATTERYOK,"Ok");
//			summarypage->SetColor(IDC_ISRSUMMARY_BATTERYOK,COLOR_BLACK);
			if (!m_bBatteryOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22880 STATUS CHANGE - BATTERY OK (1e)\n\r");
				AnyChange = true;
			}
			m_bBatteryOk = true;

//			Data_Tab &= ~(0x0002);
		}

		//bit 3 bbm
		if (Status1 & ID21_BBMERROR)
		{
			char cBuff[1024];
			if (Status2 & ID22_BBMWRITEERR)
			{
				strcpy(cBuff,"22900");
				strcpy(temp,ID22_BBMWRITEERR_TXT);
			}
			else if (Status2 & ID22_BBMNUMBBAD)
			{
				strcpy(cBuff,"22901");
				strcpy(temp,ID22_BBMNUMBBAD_TXT);
			}
			else if (Status2 & ID22_BBMFILLED)
			{
				strcpy(cBuff,"22902");
				strcpy(temp,ID22_BBMFILLED_TXT);
			}
			else
			{
				strcpy(cBuff,"22903");
				strcpy(temp,ID22_BBMINVOPCOD_TXT);
			}

			m_pParent->FlagOthr(1,true);			
			datapage->SetDlgItemText(IDC_ISRDATA_BBMOK,temp);
			datapage->SetColor(IDC_ISRDATA_BBMOK,COLOR_RED);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BBMOK,temp);
			summarypage->SetColor(IDC_ISRSUMMARY_BBMOK,COLOR_RED);
			if (m_bBBMOk)
			{
				strcat(cBuff," STATUS CHANGE (cont) - ");
				strcat(cBuff,temp);
				strcat(cBuff,"\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22890 STATUS CHANGE - BATTERY BACKED UP MEMORY ERROR (1e)\n\r");
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, cBuff);//specific failure
				AnyChange = true;
			}

			m_bBBMOk = false;
			Data_Tab |= 0x0004;
		}
		else
		{
			datapage->SetDlgItemText(IDC_ISRDATA_BBMOK,"Ok");
//			datapage->SetColor(IDC_ISRDATA_BBMOK,COLOR_BLACK);
			summarypage->SetDlgItemText(IDC_ISRSUMMARY_BBMOK,"Ok");
//			summarypage->SetColor(IDC_ISRSUMMARY_BBMOK,COLOR_BLACK);
			if (!m_bBBMOk)
			{
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, &Julian, "22910 STATUS CHANGE - NO BATTERY BACKED UP MEMORY ERROR (1e)\n\r");
				AnyChange = true;
			}

			m_bBBMOk = true;
//			Data_Tab &= ~(0x0004);
		}
	
		//bit 7 of sw 2 filter/no filter
		if (Status2 & ID22_FILTER)
		{
			datapage->SetDlgItemText(IDC_ISRDATA_DATAFILTER,"Not Filtered");
			if (m_bDataFiltered && m_pParent->m_bLogFilter)
			{
				WriteToFile(TO_PFM, TYPE_INST, &Julian, "22920 STATUS CHANGE - DATA NOT FILTERED (1e)\n\r");
				AnyChange = true;
			}

			m_bDataFiltered = false;
		}
		else
		{
			datapage->SetDlgItemText(IDC_ISRDATA_DATAFILTER,"Filtered");
			if (!m_bDataFiltered && m_pParent->m_bLogFilter)
			{
				WriteToFile(TO_PFM, TYPE_INST, &Julian, "22930 STATUS CHANGE - DATA FILTERED (1e)\n\r");
				AnyChange = true;
			}

			m_bDataFiltered = true;
		}

		//LON ok
		if (Status2 & ID22_LONOK)
		{
			datapage->SetDlgItemText(IDC_ISRDATA_LONOK,"Error");
			if (m_bLonOk)
			{
				WriteToFile(TO_PFM, TYPE_INST, &Julian, "22940 STATUS CHANGE - LON ERROR (1e)\n\r");
				AnyChange = true;
			}
			m_bLonOk = false;
			Data_Tab |= 0x0008;
		}
		else
		{
			datapage->SetDlgItemText(IDC_ISRDATA_LONOK,"Ok");
			if (!m_bLonOk)
			{
				WriteToFile(TO_PFM, TYPE_INST, &Julian, "22950 STATUS CHANGE - LON OK (1e)\n\r");
				AnyChange = true;
			}
			m_bLonOk = true;
		}


		//write to pfm file if there was a change in the status bits
		if (AnyChange)
		{
			char cBuff[1024];
			strcpy(cBuff,"22960 ");
			strcat(cBuff,Record);
			WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//any change to status
		}
	
		//get bbm
		strncpy(temp,&Record[18],6);
		temp[6]=NULL;
		int bbm=atoi(temp);
		datapage->SetDlgItemInt(IDC_ISRDATA_BYTESMEM,bbm);
		m_pCloseButton->SetNumeric(bbm,true);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
	
		if (Data_Tab != 0)
			SetTabColor(IDC_ISRDATA_TAB,COLOR_RED);

//		datapage->Invalidate();
//		summarypage->Invalidate();
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRMONITOR_ID2_RECORD,CheckSum);	
}

//record 1f
void CISRPropertySheet::AnalyzeOk(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRANALYZE_OK_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[8];
		strncpy(temp,&Record[2],6);
		temp[6] = NULL;
		int bbm=atoi(temp);
		m_pCloseButton->SetNumeric(bbm,true);
		m_pParent->m_pButton->put_Numeric((ULONG)bbm);
		m_pParent->PostMessage(uIDC_RECEIVE,IDC_BBM_VALUE,bbm);
	}
	m_pParent->PostMessage(uIDC_RECEIVE,ISRANALYZE_OK_RECORD,CheckSum);
}

//record 30
void CISRPropertySheet::InformRec(char *Record)
{
	int CheckSum;
	if (LengthOk(ISRINFORMATION_RECORD_LENGTH,Record)) 
		CheckSum = Checksum(Record);
	else
		CheckSum = CHECKSUM_BAD;
	if (CheckSum == CHECKSUM_GOOD)
	{
		char temp[32];

		m_dMessageRecordCount++;
		//get julian time
		strncpy(temp,&Record[2],10);
		temp[10]=NULL;
		CJulianTime Julian(temp);

		char cBuff[1024];
		strcpy(cBuff,"22970 INFORMATION RECORD - ");
		strcat(cBuff,Record);
		WriteToFile(TO_PFM, TYPE_INST, &Julian, cBuff);//InformRec
	}

	m_pParent->PostMessage(uIDC_RECEIVE,ISRINFORMATION_RECORD,CheckSum);
}

int 
CISRPropertySheet::Checksum(char* Record)
{
    unsigned short nbytes;	/* # bytes to include in checksum calculation */
    unsigned short sum;		/* sum of all bytes in checksum */
	unsigned short rec_sum;
    /* checksum is a one byte sum of all bytes up to the checksum byte */
	sum = 0;
    nbytes = (unsigned short)(strlen(Record)-4);//don't count lf/cr nor checksum
	sscanf(&Record[nbytes],"%2hx",&rec_sum);
	for (int i=0; i < nbytes; i++)
		sum = (unsigned short)(sum + Record[i]);
	sum &= 0x00ff;	/* keep only relevant part of checksum */
	if (rec_sum == sum)
		/* calculated checksum matches checksum in record */
		return CHECKSUM_GOOD;

//	CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CISRModParam* modparpage  = m_pParent->m_pModParams;

	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Checksum: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);
	generalpage->SetDlgItemText(IDC_ISRSETUP_CHECKSUM,(char*)LPCTSTR(DateTime));
	generalpage->SetColor(IDC_ISRSETUP_CHECKSUM,COLOR_RED);
//	m_pParent->FlagComm(1,false);			

	SetTabColor(IDC_ISRSETUP_TAB,COLOR_RED);

	char cBuff[1024];
	strcpy(cBuff,"22980 CHECKSUM ERROR -- ");
	strcat(cBuff,Record);
	
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);
	//WriteToFile(TO_PFM, TYPE_COMP, NULL, Record);

	if (m_pParent->m_bDoDump)
	{
		WriteToFile(TO_DMP,TYPE_DUMP,NULL,cBuff);
	//	WriteToFile(TO_DMP, TYPE_DUMP, NULL, "CHECKSUM ERROR -- \n\r");
	//	WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
	}

    return CHECKSUM_BAD;	       /* checksums do not match */
}

bool 
CISRPropertySheet::LengthOk(int length, char* Record)
{
	if (strlen(Record) == (unsigned)length)
		return true;

//	CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
//	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
//	CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CISRModParam* modparpage  = m_pParent->m_pModParams;
	COleDateTime Now = COleDateTime::GetCurrentTime();
	CString DateTime = Now.Format("Length: %d/%H:%M:%S\n");
	DateTime += Record;
	DateTime = DateTime.Left(DateTime.GetLength()-2);
/*
	generalpage->SetDlgItemText(IDC_ISRSETUP_CHECKSUM,(char*)LPCTSTR(DateTime));
	generalpage->SetColor(IDC_ISRSETUP_CHECKSUM,COLOR_RED);
	generalpage->GetDlgItem(IDC_ISRSETUP_CHECKSUM)->Invalidate();
//	m_pParent->FlagComm(1,false);			
	SetTabColor(IDC_ISRSETUP_TAB,COLOR_RED);
*/
	char cBuff[1024];
	strcpy(cBuff,"22990 LENGTH ERROR -- ");
	strcat(cBuff,Record);
	WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

//	WriteToFile(TO_PFM, TYPE_COMP, NULL, "LENGTH ERROR\n\r");
//	WriteToFile(TO_PFM, TYPE_COMP, NULL, Record);

	if (m_pParent->m_bDoDump)
	{
		//WriteToFile(TO_DMP, TYPE_DUMP, NULL, "LENGTH ERROR\n\r");
		//WriteToFile(TO_DMP, TYPE_DUMP, NULL, Record);
		WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
	}

    return false;
}

void CISRPropertySheet::BuildFileName(CString &cResult,
	CString cFormat,
	CString cPath,
	CString cType,
	CString cID,
	int iYear,
	int iMonth,
	int iDay,
	int iHour,
	int iMinute,
	int iSecond,
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
	bool bHaveEscape = 0;
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

bool CISRPropertySheet::WriteToISRFile(CJulianTime* Time, ISRAcquireRecord* AcquireRec)
{
	FILE *File;
	fpos_t position;
	struct ISRAcquireHeader ISRAcquireHead;
	if (m_pParent->m_bBlockWrite) return false;
	
	//build filename
	if (m_pParent->m_bUseShortFilename)
	{
		sprintf(m_szCurrentISRFileName,"%s\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
				((Time->Year()-1990)<10)?
					((Time->Year()-1990<0)?'#':'0'+(Time->Year()-1990)):
					'A'+(Time->Year()-2000),
				((Time->Month()<10)?
					'0'+(Time->Month()):
					'A'+(Time->Month()-10)),
				((Time->Day()  <10)?
					'0'+(Time->Day()):
					'A'+(Time->Day()  -10)),
				'A',0,ISR_SUFFIX);
	}
	else
	{
		CString cResult;
		BuildFileName(cResult,m_pParent->m_csLongNameFormatString,m_pParent->m_szSaveLoc,"SR",
			m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,ISR_SUFFIX);
		strcpy(m_szCurrentISRFileName,cResult);
	}

	//is the file there
	// '0' checks for existance only
	if (_access(m_szCurrentISRFileName,0)!=0)
	{
		//make sure path exists
		m_pParent->MakeMyPath(m_szCurrentISRFileName);

		//build archive file name
		CString ArchiveName;
		if (m_pParent->m_bUseShortFilename)
		{
			ArchiveName.Format("%s\\archive\\%s%c%c%c%c%02d.%3s",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
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
				'A',0,ISR_SUFFIX);
		}
		else
		{
			CString cTemp;
			cTemp = m_pParent->m_szSaveLoc;
			cTemp += "\\archive\\";
			BuildFileName(ArchiveName,m_pParent->m_csLongNameFormatString,cTemp,"SR",
				m_pParent->m_pID,Time->Year(),Time->Month(),Time->Day(),0,0,0,ISR_SUFFIX);
		}
		//if the file is in the archive directory
		if (_access(ArchiveName,0)==0)
			//then move it to the working directory
			MoveFileEx(ArchiveName,m_szCurrentISRFileName,
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
	}

	int Attempts = 5;
	File = NULL;
	while ((Attempts > 0) && (File == NULL))
	{
		File = fopen(m_szCurrentISRFileName,"ab+");
		if (File == NULL)
		{
			Sleep(200);
		}
		Attempts--;
	}

	if (File != NULL)
	{
		//get file position
		fseek(File,0,SEEK_END);

		if (fgetpos( File, &position )==0)
		{
			//if new file (e.g. no header there yet)
			if (position == 0)
			{
				//initialize header
				char temp[32];
				memset( &ISRAcquireHead, ' ', sizeof(ISRAcquireHeader));

				m_pParent->m_bDoOneDumpStat = true;	//SCR00074

				//fill header
				//size of header beyond first four bytes
				sprintf(temp,"%4d",sizeof(ISRAcquireHeader)-4);
				memcpy(ISRAcquireHead.SizeOfHeader,temp,4);

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
				sscanf(m_pParent->m_csVersion,"%d.%d.%d.%d",(int*)&sA,(int*)&sB,(int*)&sC, (int*)&sD); // added typecast gjk
				sprintf(temp,"%2d%1d%1d%1d",sA,sB,sC,sD);  //no decimal point
				//End fix of 8/5/2005

				memcpy(ISRAcquireHead.Version,temp,5);
				sprintf(temp,"%3s",m_szCurrentID);
				memcpy(ISRAcquireHead.StationId,temp,3);
				sprintf(temp,"%3d",Time->Year()%100);
				memcpy(ISRAcquireHead.Year,temp,3);
				sprintf(temp,"%3d",Time->Month());
				memcpy(ISRAcquireHead.Month,temp,3);
				sprintf(temp,"%3d",Time->Day());
				memcpy(ISRAcquireHead.Day,temp,3);
				sprintf(temp,"%4d",Time->Year());
				memcpy(ISRAcquireHead.unused2,temp,4);
				//write header
				fwrite(&ISRAcquireHead,sizeof(ISRAcquireHeader),1,File);
			}
		}
		//<<QA>> there is muchmore than this in the GRAND
		//send record to file
		int iWriteResult;
		if ((iWriteResult = fwrite(AcquireRec,sizeof(ISRAcquireRecord),1,File))!= 1)
		{
			//if failed it could be because MicXfer is trying to
			//clear the archive bit, so wait a short time and try
			//again.
			Sleep(100);
			if ((iWriteResult = fwrite(AcquireRec,sizeof(ISRAcquireRecord),1,File))!=1)
			{
				//if failed it could be because MicXfer is trying to
				//clear the archive bit, so wait a short time and try
				//again.
				Sleep(200);
				if ((iWriteResult = fwrite(AcquireRec,sizeof(ISRAcquireRecord),1,File))!=1)
				{
					//if failed on write then show a file write problem
					//on the instrument's button
					m_pParent->m_pButton->SetDiskIcon_Hourglass();
					m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
					m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
					//write the error to the pfm and cev files
					WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "23000 Could not write to ISR file\n\r");
					//close the file anyway
					fclose(File);
					//return write failure 
					return false;
				}
			}
		}
		if ((iWriteResult == 1) && m_bCouldNotOpenBinaryFile)
		{
			m_bCouldNotOpenBinaryFile = false;
			WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "23010 Successful write to ISR file\n\r");
		}
		//close file
		fclose(File);
		//return good write
		return true;
	}
	else
	{
		//save the fact that we couildn't so that we can tell PFM/CEV when good
		m_bCouldNotOpenBinaryFile = true;
		//couldn't open the file so...
		//show a file write problem
		//on the instrument's button
		m_pParent->m_pButton->ShowIcon_Disk(VARIANT_TRUE);
		m_pParent->m_pButton->put_Flash(VARIANT_TRUE);
		WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, "23020 Could not open ISR file\n\r");
	}
	return false;
}


void CISRPropertySheet::WriteToFile(int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale)
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

	//default to the provided ISR time
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
		//	build new file name and save it
		if (m_pParent->m_bUseShortFilename)
		{
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
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
				'A',0);
		}
		else
		{
			CString cResult;
			BuildFileName(cResult,
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
		if (m_pParent->m_bUseShortFilename)
		{
			//	build new file name and save it
			sprintf(m_szCurrentFileName,"%s\\%s%c%c%c%c%02d.",
				m_pParent->m_szSaveLoc,	
				m_pParent->m_pID,
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
			BuildFileName(cResult,
				m_pParent->m_csLongNameFormatString,
				m_pParent->m_szSaveLoc,
				"SR",
				m_pParent->m_pID,
				TG.GetYear(),TG.GetMonth(),TG.GetDay(),0,0,0,".");
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
		//	message = Time::YY.MM.DD HH:MM:SS I (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I %s",
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond(),
			Msg[0]==NULL?"\n\r":Msg);
		break;

	case TYPE_COMP:
		//	message = MICTIME::YY.MM.DD HH:MM:SS C (MESSAGE==NULL)?\r:MESSAGE
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C %s",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),TC.GetMinute(),TC.GetSecond(),Msg[0]==NULL?"\n\r":Msg);
		break;

	case TYPE_GID2:
	case TYPE_TIME:
		//	message = Time::YY.MM.DD HH:MM:SS ISR Time   "+
		//					"YY.MM.DD HH:MM:SS Computer Time   C - I = xxxx\r"
		//computer time
		//ISR time
		TS = TC - TG;  // Subtract 2 COleDateTimes
		TS += HALF_SECOND;
		sprintf(Message,
			"%04d.%02d.%02d %02d:%02d:%02d I 22791 ISR Time   %04d.%02d.%02d %02d:%02d:%02d Computer Time   C - I = %.0f seconds\n\r",
			TG.GetYear(),TG.GetMonth(),TG.GetDay(),TG.GetHour(),TG.GetMinute(),TG.GetSecond(),
			TC.GetYear(),TC.GetMonth(),TC.GetDay(),TC.GetHour(),TC.GetMinute(),TC.GetSecond(),
			TS.GetTotalSeconds());
		break;

	case TYPE_INVTIME:
		//	message = "INVALID TIME  "+
		//		"Previous Record Time Saved::YY.MM.DD HH:MM:SS "+
		//		"Current Record Time Time::YY.MM.DD HH:MM:SS\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d I 22790 INVALID TIME %04d.%02d.%02d %02d:%02d:%02d\n\r",
			((CJulianTime*)Msg)->Year(),	((CJulianTime*)Msg)->Month(),	((CJulianTime*)Msg)->Day(),
			((CJulianTime*)Msg)->Hour(),		((CJulianTime*)Msg)->Minute(),	((CJulianTime*)Msg)->Second(),
			TG.GetYear(),
			TG.GetMonth(),	TG.GetDay(),
			TG.GetHour(),		TG.GetMinute(),	TG.GetSecond());
		break;

	case TYPE_START:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C ISR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 21830 ISR COLLECT Version %s started\n\r",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_ABNORMAL:
	//  message = MICTIME::YY.MM.DD HH:MM:SS C ISR COLLECT Version %s<VERSION> started\r"
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 21840 ISR COLLECT Version %s started from abnormal shutdown\n\r",
			TC.GetYear(),		
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			m_pParent->m_csVersion.GetString());
		break;

	case TYPE_DELETE:
	//	message = MICNOW::YY.MM.DD HH:MM:SS C file %s<MESSAGE> deleted\r"

		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 21850 File %s deleted\n\r",
			TC.GetYear(),
			TC.GetMonth(),	TC.GetDay(),
			TC.GetHour(),	TC.GetMinute(),	TC.GetSecond(),
			Msg);
		break;

	//just in case
	default:
		sprintf(Message,"%04d.%02d.%02d %02d:%02d:%02d C 21860 Unknown TYPE %s\n\r",
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

		if (_access(fn,0) != 0)
		{
			//make sure path exists
			m_pParent->MakeMyPath(fn);
		}
		
		if ((File = fopen(fn,"ab+")) != NULL)  //MIC_CB_028 to MIC 1.907 PJM 1/26/05
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
				'A',0,CEV_SUFFIX);
			}
			else
			{
				CString cTemp;
				cTemp = m_pParent->m_szSaveLoc;
				cTemp += "\\archive\\";
				BuildFileName(ArchiveFileName,m_pParent->m_csLongNameFormatString,cTemp,"SR",
					m_pParent->m_pID,TC.GetYear(),TC.GetMonth(),TC.GetDay(),0,0,0,CEV_SUFFIX);
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
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
//				m_bCountRateTamperOk = true;
//				m_bRMSSigmaTamperOk  = true;
//				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_pParent->m_dBreakSentCount = 0;
				m_pParent->m_dTimeOutCount = 0;

//				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}

			//	send string
			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{																				//DR00406
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);
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
				BuildFileName(ArchiveFileName,m_pParent->m_csLongNameFormatString,cTemp,"SR",
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
				m_bStartOfFile	= true;
				m_bBatteryOk	= true;
				m_bBatteryTolOk	= true;
				m_bP15VoltOk	= true;
				m_bM15VoltOk	= true;
				m_bACPowerOk	= true;
				m_bACPowerLoss	= false;
				m_bBBMOk		= true;
//				m_bCountRateTamperOk = true;
//				m_bRMSSigmaTamperOk  = true;
//				m_bInMII		= false;
				m_bDataFiltered	= false;
				m_pParent->m_dBreakSentCount = 0;
				m_pParent->m_dTimeOutCount = 0;
//				m_dCWStart = -1;//neither COLD START (0) nor WARM START (1)
			}
			//	send string
			//	send stale string															//DR00406
			if (bStale)																		//DR00406
			{																				//DR00406
				unsigned iLen = strlen(Message);											//DR00406
				char ch1 = Message[iLen-1];													//DR00406
				char ch2 = Message[iLen-2];													//DR00406
				if ((ch1 == '\r') || (ch1 == '\n')) Message[iLen-1] = NULL;					//DR00406
				if ((ch2 == '\r') || (ch2 == '\n')) Message[iLen-2] = NULL;					//DR00406
				fprintf(File,"%s %s",Message,"Note: A newer message has been received.\r\n");	//DR00406
			}																				//DR00406
			else																			//DR00406
				fprintf(File,"%s",Message);

			//	close pfm
			fclose(File);
		}
	}

}


int CISRPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pCloseButton = new CColorButton(false, m_pParent->m_szIniFile);
	m_pCloseButton->SetBase(10);
	m_pCloseButton->SetPreNum("BBM: ");

	m_pPauseButton = new CButton();
	m_pAckFlagsButton = new CButton();
	m_pPrintButton = new CButton();
	m_pToClose = new CButton();
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);
	return 0;
}

void CISRPropertySheet::OnCloseButton()
{
	//<<QA>> GRAND model hides the modparams page here, too.
	ShowWindow(SW_HIDE);
	KillTimer(1);
}

void CISRPropertySheet::SetTabColor(int Tab,COLORREF Color)
{
	//<<QA>> this is different in the GRAND model.
	if ((Tab != IDC_ISRSETUP_TAB) && 
		(Tab != IDC_ISRDATA_TAB) &&
		(Tab != IDC_ISRUSERPROG_TAB)) return;
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
	case IDC_ISRSETUP_TAB: tab = 1; break;
	case IDC_ISRDATA_TAB:    tab = 2; break;
	case IDC_ISRUSERPROG_TAB:tab = 3; break;
	};
	GetTabControl()->SetItem(tab,&pTabCtrlItem);
	GetTabControl()->Invalidate();
}

void CISRPropertySheet::OnAckButton()
{
	m_dToClose = 0;

	//acknowledge the four button flags
	m_pParent->m_pButton->ShowIcon_Bell(VARIANT_FALSE);
	m_pParent->FlagComm(0,false);
	m_pParent->FlagOthr(0,false);
	m_pParent->FlagTime(0,false);

	m_pParent->m_pButton->ShowIcon_Disk(VARIANT_FALSE);
	m_pParent->m_pButton->put_Flash(VARIANT_FALSE);

	CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CISRDataStat* datapage    = m_pParent->m_pDataStatus;
//	CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
//	CISRModParam* modparpage  = m_pParent->m_pModParams;

//	generalpage->SetColor(IDC_GENERAL_CHECKSUM,COLOR_BLACK);
	summarypage->ResetColors();
	generalpage->ResetColors();
	datapage->ResetColors();

	SetTabColor(IDC_ISRSETUP_TAB,COLOR_GREEN);
	SetTabColor(IDC_ISRDATA_TAB,COLOR_GREEN);
	SetTabColor(IDC_ISRUSERPROG_TAB,COLOR_GREEN);

	Data_Tab = 0x0000;
	General_Tab = 0x0000;
	UserProg_Tab = 0x0000;

	Invalidate();
}

void CISRPropertySheet::OnPauseButton()
{
	m_dToClose = 0;
	CISRModParam* modparpage = m_pParent->m_pModParams;
	if (m_pParent->TogglePause())
	{
		m_pPauseButton->SetWindowText(COLLECTINGBEGIN);
		modparpage->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(TRUE);
	}
	else
	{
		m_pPauseButton->SetWindowText(COLLECTINGPAUSE);
		modparpage->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(FALSE);
	}
}

void CISRPropertySheet::IssuePrint(FILE* outfile)
{
	CISRSummary*  summarypage = m_pParent->m_pSummaryPage;
	CISRGenlStat* generalpage = m_pParent->m_pGeneralStatPage;
	CISRDataStat* datapage    = m_pParent->m_pDataStatus;
	CISRUserProg* userpage    = m_pParent->m_pUserProgStatus;
	CISRModParam* modparpage  = m_pParent->m_pModParams;

	CString temp, temp1, temp2, temp3, temp4;

	COleDateTime today = COleDateTime::GetCurrentTime();
	CString datetime = today.Format(" %A, %B %d, %Y - %H:%M:%S ");
	fprintf(outfile,"* = Item is displayed RED\n");
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"SUMMARY\n");
	fprintf(outfile," ISR Setup\n");

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_ISRSUMMARY_HIGHVOLT1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_ISRSUMMARY_HIGHVOLT2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_HIGHVOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_ISRSUMMARY_HIGHVOLT3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"     High Voltage (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(),temp2.GetString(),temp3.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_BATTERY1,temp1);
	temp1 += (summarypage->StaticColors[IDC_ISRSUMMARY_BATTERY1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_BATTERY2,temp2);
	temp2 += (summarypage->StaticColors[IDC_ISRSUMMARY_BATTERY2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_BATTERY3,temp3);
	temp3 += (summarypage->StaticColors[IDC_ISRSUMMARY_BATTERY3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P15VOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_ISRSUMMARY_P15VOLT1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P15VOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_ISRSUMMARY_P15VOLT2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P15VOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_ISRSUMMARY_P15VOLT3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_M15VOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_ISRSUMMARY_M15VOLT1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_M15VOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_ISRSUMMARY_M15VOLT2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_M15VOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_ISRSUMMARY_M15VOLT3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P5VOLT1,temp1);
	temp1 += (summarypage->StaticColors[IDC_ISRSUMMARY_P5VOLT1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P5VOLT2,temp2);
	temp2 += (summarypage->StaticColors[IDC_ISRSUMMARY_P5VOLT2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_P5VOLT3,temp3);
	temp3 += (summarypage->StaticColors[IDC_ISRSUMMARY_P5VOLT3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %8s %8s %8s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	fprintf(outfile," Data Status\n");
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_ACPOWER,temp);
	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_ACPOWER-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
//	fprintf(outfile,"                           AC Power: %s\n",temp);
	fprintf(outfile,"                     External Power: %s\n",temp.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_BATTERYOK,temp);
	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_BATTERYOK-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                            Battery: %s\n",temp.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_BBMOK,temp);
	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_BBMOK-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"           Battery Backed Up Memory: %s\n",temp.GetString());

	summarypage->GetDlgItemText(IDC_ISRSUMMARY_INQUIRETIME,temp);
	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_INQUIRETIME-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"   Last INQUIRE Status (Y.M.D H:M:S: %s\n",temp.GetString());

	fprintf(outfile,"  Recent Commands and Responses:\n");
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND6,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND6-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND5,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND5-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND4,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND4-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND3,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND3-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND2,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND2-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));
	summarypage->GetDlgItemText(IDC_ISRSUMMARY_COMMAND1,temp);
	temp = temp.SpanExcluding("\n\r");
//	temp += (summarypage->StaticColors[IDC_ISRSUMMARY_COMMAND1-IDC_ISRSUMMARY_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  %s\n",LPCTSTR(temp));


	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"ISR SETUP\n");
	fprintf(outfile," Most Recent STATUS SR Record\n");

	generalpage->GetDlgItemText(IDC_ISRSETUP_TIME,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_TIME-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"          Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_ACPOWER,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_ACPOWER-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
//	fprintf(outfile,"                           AC Power: %s\n",temp);
	fprintf(outfile,"                     External Power: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_BATTERY,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_BATTERY-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                            Battery: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_BAUD,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_BAUD-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                               Baud: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_PORTSET,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_PORTSET-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                           Port Set: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_HVSETPOINT,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_HVSETPOINT-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"             High Voltage Set Point: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_PREDELAY,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_PREDELAY-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                          Pre-Delay: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_GATEWIDTH,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_GATEWIDTH-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"                         Gate Width: %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_COUNTT,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_COUNTT-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"               Count Time (Seconds): %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_TEMPONN,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_TEMPONN-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"           Temperature On Board (C): %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_TEMPOFF,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_TEMPOFF-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"          Temperature Off Board (C): %s\n",temp.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_HV1,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_HV1-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_HV2,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_HV2-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_HV3,temp3);
	temp3 += (generalpage->StaticColors[IDC_ISRSETUP_HV3-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"     High Voltage (Min / Cur / Max): %s %s %s\n",
		temp1.GetString(),temp2.GetString(),temp3.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_BV1,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_BV1-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_BV2,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_BV2-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_BV3,temp3);
	temp3 += (generalpage->StaticColors[IDC_ISRSETUP_BV3-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  Battery Voltage (Min / Cur / Max): %s %s %s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_P15VOLT1,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_P15VOLT1-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_P15VOLT2,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_P15VOLT2-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_P15VOLT3,temp3);
	temp3 += (generalpage->StaticColors[IDC_ISRSETUP_P15VOLT3-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  +12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_M15VOLT1,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_M15VOLT1-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_M15VOLT2,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_M15VOLT2-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_M15VOLT3,temp3);
	temp3 += (generalpage->StaticColors[IDC_ISRSETUP_M15VOLT3-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"  -12 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_P5VOLT1,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_P5VOLT1-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_P5VOLT2,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_P5VOLT2-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_P5VOLT3,temp3);
	temp3 += (generalpage->StaticColors[IDC_ISRSETUP_P5VOLT3-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile,"   +5 Volt Supply (Min / Cur / Max): %s %s %s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CH0A,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_CH0A-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_CH0B,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_CH0B-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Trigger Signal Records, Most Recent/Previous\n");
	fprintf(outfile," Chan. 0: %s / %s\n",temp1.GetString(),temp2.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CH1A,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_CH1A-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_CH1B,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_CH1B-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Chan. 1: %s / %s\n",temp1.GetString(),temp2.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CH2A,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_CH2A-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_CH2B,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_CH2B-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Chan. 2: %s / %s\n",temp1.GetString(),temp2.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CH3A,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_CH3A-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_CH3B,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_CH3B-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Chan. 3: %s / %s\n", temp1.GetString(), temp2.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CH4A,temp1);
	temp1 += (generalpage->StaticColors[IDC_ISRSETUP_CH4A-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	generalpage->GetDlgItemText(IDC_ISRSETUP_CH4B,temp2);
	temp2 += (generalpage->StaticColors[IDC_ISRSETUP_CH4B-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Sig. Ch: %s / %s\n", temp1.GetString(), temp2.GetString());

	generalpage->GetDlgItemText(IDC_ISRSETUP_CHECKSUM,temp);
	temp += (generalpage->StaticColors[IDC_ISRSETUP_CHECKSUM-IDC_ISRSETUP_FIRST]==COLOR_RED)?"*":" ";
	fprintf(outfile," Checksum or Length Error in Any Message: %s\n",temp.GetString());

	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"DATA STATUS\n");
	fprintf(outfile," Most Recent Response To INQUIRE2\n");
	datapage->GetDlgItemText(IDC_ISRDATA_INQUIRETIME,temp);
	fprintf(outfile,"   Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_UNITID,temp);
	fprintf(outfile,"                Main Unit ID: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_BYTESMEM,temp);
	fprintf(outfile,"     Bytes of Data in Memory: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_ACPOWER,temp);
	fprintf(outfile,"      External Power Present: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_BATTERYOK,temp);
	fprintf(outfile,"                     Battery: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_BBMOK,temp);
	fprintf(outfile,"    Battery Backed Up Memory: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_DATAFILTER,temp);
	fprintf(outfile,"          Data Filter Status: %s\n",temp.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_LONOK,temp);
	fprintf(outfile,"                         LON: %s\n",temp.GetString());
	fprintf(outfile," Most Recent ACQUIRE Records Received\n");

	if(((CButton*)datapage->GetDlgItem(IDC_ISRRADIO_RAW))->GetCheck()==0)
		fprintf(outfile,"      Show Rates\n");
	else
		fprintf(outfile,"      Show Raw\n");

	datapage->GetDlgItemText(IDC_ISRDATA_DATADATE1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_DATADATE2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_DATADATE3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_DATADATE4,temp4);
	fprintf(outfile,"  Date of Data Record (Y/M/D): %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIME1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIME2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIME3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIME4,temp4);
	fprintf(outfile,"  Time of Data Record (H:M:S): %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_DATATIMEAUX4,temp4);
	fprintf(outfile,"   Julian Time Auxillary Byte: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_BATTERY1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_BATTERY2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_BATTERY3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_BATTERY4,temp4);
//	fprintf(outfile,"           Battery - AC Power: %10s %10s %10s %10s\n",
	fprintf(outfile,"     Battery - External Power: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_TOTALS4,temp4);
	fprintf(outfile,"                       Totals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX1TOTALS4,temp4);
	fprintf(outfile,"           Auxillary 1 Totals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_AUX2TOTALS4,temp4);
	fprintf(outfile,"           Auxillary 2 Totals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_REALACC1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_REALACC2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_REALACC3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_REALACC4,temp4);
	fprintf(outfile,"          Reals + Accidentals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_ACCIDENT4,temp4);
	fprintf(outfile,"                  Accidentals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_ISRDATA_REAL1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_REAL2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_REAL3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_REAL4,temp4);
	fprintf(outfile,"                        Reals: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	datapage->GetDlgItemText(IDC_ISRDATA_ETIME1,temp1);
	datapage->GetDlgItemText(IDC_ISRDATA_ETIME2,temp2);
	datapage->GetDlgItemText(IDC_ISRDATA_ETIME3,temp3);
	datapage->GetDlgItemText(IDC_ISRDATA_ETIME4,temp4);
	fprintf(outfile,"                 Elapsed Time: %11s %11s %11s %11s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"Most Recent Monitor Parameters Status Record\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TIME,temp);
	fprintf(outfile," Date & Time (Y.M.D H:M:S): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_IMMEDIATE,temp);
	fprintf(outfile," Immediate Buffer Size / Save Size: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_LOCALBACK,temp);
	fprintf(outfile,"       Local Background Definition: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_NOFILTER,temp);
	fprintf(outfile,"               No Filter Condition: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_FILTERENAB,temp);
	fprintf(outfile,"                 Filtering Enabled: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_FILTERMETH,temp);
	fprintf(outfile,"                     Filter Method: %s\n",temp.GetString());

	fprintf(outfile,"   Trigger 0\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR0,temp);
	fprintf(outfile,"           Trigger Type & Polarity: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH0,temp);
	fprintf(outfile,"  Trigger Delay(Sec) & Width(mSec): %s\n",temp.GetString());

	fprintf(outfile,"   Trigger 1\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR1,temp);
	fprintf(outfile,"           Trigger Type & Polarity: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH1,temp);
	fprintf(outfile,"  Trigger Delay(Sec) & Width(mSec): %s\n",temp.GetString());

	fprintf(outfile,"   Trigger 2\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR2,temp);
	fprintf(outfile,"           Trigger Type & Polarity: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH2,temp);
	fprintf(outfile,"  Trigger Delay(Sec) & Width(mSec): %s\n",temp.GetString());

	fprintf(outfile,"   Trigger 3\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR3,temp);
	fprintf(outfile,"           Trigger Type & Polarity: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH3,temp);
	fprintf(outfile,"  Trigger Delay(Sec) & Width(mSec): %s\n",temp.GetString());

	fprintf(outfile,"   Trigger 4\n");
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR4,temp);
	fprintf(outfile,"           Trigger Type & Polarity: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH4,temp);
	fprintf(outfile,"  Trigger Delay(Sec) & Width(mSec): %s\n",temp.GetString());

//	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGPOLAR,temp);
//	fprintf(outfile,"                  Trigger Polarity: %s\n",temp);
//	userpage->GetDlgItemText(IDC_ISRUSERPROG_TRIGWIDTH,temp);
//	fprintf(outfile,"        Trigger Pulse Width (mSec): %s\n",temp);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TOTALSF,temp1);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX1F,temp2);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX2F,temp3);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_REALSF,temp4);
	fprintf(outfile,"                             %s/%s/%s/%s\n",
		"Totals Ch1","Aux. 1 Ch2","Aux. 2 Ch3","Reals  Ch0");
//												:					
	fprintf(outfile,"  Filtering Threshold/Sigma: %10s %10s %10s %10s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TOTALSTH,temp1);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX1TH,temp2);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX2TH,temp3);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_REALSTH,temp4);
	fprintf(outfile,"             High Threshold: %10s %10s %10s %10s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TOTALSTL,temp1);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX1TL,temp2);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX2TL,temp3);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_REALSTL,temp4);
	fprintf(outfile,"                  Low Limit: %10s %10s %10s %10s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_TOTALS3ST,temp1);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX13ST,temp2);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_AUX23ST,temp3);
	userpage->GetDlgItemText(IDC_ISRUSERPROG_REALS3ST,temp4);
	fprintf(outfile,"      Triggering Sigma Test: %10s %10s %10s %10s\n",
		temp1.GetString(), temp2.GetString(), temp3.GetString(), temp4.GetString());

	userpage->GetDlgItemText(IDC_ISRUSERPROG_ATTESTTH,temp);
	fprintf(outfile,"  A/T Test Totals Threshold: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_ATTESTBIAS,temp);
	fprintf(outfile,"    A/T Test Bias Limit (percent): %s\n",temp.GetString()); // changed % -> percent gjk
	userpage->GetDlgItemText(IDC_ISRUSERPROG_ATTESTLIMIT,temp);
	fprintf(outfile,"       A/T Test Sigma Limit: %s\n",temp.GetString());

	userpage->GetDlgItemText(IDC_ISRUSERPROG_SYNCE,temp);
	fprintf(outfile,"                Synchronization Enabled: %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_SYNCT,temp);
	fprintf(outfile,"        Synchronization Time (hh:mm:ss): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_SYNCL,temp);
	fprintf(outfile,"  Synchronization Lower Limit (Seconds): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_SYNCU,temp);
	fprintf(outfile,"  Synchronization Upper Limit (Minutes): %s\n",temp.GetString());
//	userpage->GetDlgItemText(IDC_ISRUSERPROG_TAKEOFF,temp);
//	fprintf(outfile,"             Take Offsets Every (Hours): %s\n",temp.GetString());
//	userpage->GetDlgItemText(IDC_ISRUSERPROG_TAKEOFFMAX,temp);
//	fprintf(outfile,"        Maximum Between Offsets (Hours): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_STATCHECK,temp);
	fprintf(outfile,"           Status Check Every (Minutes): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_LOGBATT,temp);
	fprintf(outfile,"       Log Battery Info Every (Minutes): %s\n",temp.GetString());
	userpage->GetDlgItemText(IDC_ISRUSERPROG_ISRUV,temp);
	fprintf(outfile,"        ISR User Program Version Number: %s\n",temp.GetString());
//	userpage->GetDlgItemText(IDC_ISRUSERPROG_ROMCHECK,temp);
//	fprintf(outfile,"                           ROM Checksum: %s\n",temp);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"MODIFY PARAMETERS -- Caution: These values may not have been applied!\n");
	fprintf(outfile," Communications\n");
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_PORT,temp);
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_NODE,temp1);
	fprintf(outfile,"  Port: %s\n  Node: %s\n\n",temp.GetString(),temp1.GetString());
	fprintf(outfile," File Output\n");
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SAVELOC,temp);
	fprintf(outfile,"  Location: %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_FILEID,temp);
	fprintf(outfile,"  Station ID: %s\n",temp.GetString());

	if(((CButton*)modparpage->GetDlgItem(IDC_ISRMODPARAMS_LOGFILTER))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"  Log Data Filtering Msgs: %s\n",temp.GetString());

	if(((CButton*)modparpage->GetDlgItem(IDC_ISRMODPARAMS_DODUMP))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes";
	fprintf(outfile,"             Do Dump File: %s\n",temp.GetString());
//	if(((CButton*)modparpage->GetDlgItem(IDC_ISRMODPARAMS_LOGMII))->GetCheck()==0)
//		temp = "No";
//	else
//		temp = "Yes";
//	fprintf(outfile,"  Log MII Msgs: %s\n\n",temp);

	fprintf(outfile," Error Limits\n");
	fprintf(outfile,"                       High     Low\n");
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_BATVOLTH,temp);
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_BATVOLTL,temp1);
	fprintf(outfile,"   Battery (28 Volt): %6s  %6s\n",temp.GetString(),temp1.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_P15H,temp);
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_P15L,temp1);
	fprintf(outfile,"     +15 Volt Supply: %6s  %6s\n", temp.GetString(), temp1.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_M15H,temp);
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_M15L,temp1);
	fprintf(outfile,"     -15 Volt Supply: %6s  %6s\n", temp.GetString(), temp1.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_P05H,temp);
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SUP_P05L,temp1);
	fprintf(outfile,"      +5 Volt Supply: %6s  %6s\n", temp.GetString(), temp1.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_TIME_ERR,temp);
	fprintf(outfile,"  Time Delta (Sec.): %6s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_SLOW_RESET,temp);
	fprintf(outfile,"  Slow Reset (Sec.): %6s\n\n",temp.GetString());
	fprintf(outfile," General\n");
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_MAXCYCLE,temp);
	fprintf(outfile,"     Message Cycle Time (mSec): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_MAXPAUSE,temp);
	fprintf(outfile,"      Maximum Pause Time (Sec): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_MAXBBM,temp);
	fprintf(outfile,"           Maximum BBM (Bytes): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_COMMFAIL,temp);
	fprintf(outfile,"  # of Retransmits Before Fail: %s\n",temp.GetString());

	if(((CButton*)modparpage->GetDlgItem(IDC_CHECK_ALLOWAUTODATE))->GetCheck()==0)
		temp = "No";
	else
		temp = "Yes"; 
	fprintf(outfile,"    Allow Auto Date & Time Set: %s\n",temp.GetString());

	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_MAXINQUIRE,temp);
	fprintf(outfile,"          Inquire Every(units): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_TIMEOUT,temp);
	fprintf(outfile,"     Response Time-Out (units): %s\n",temp.GetString());
	modparpage->GetDlgItemText(IDC_ISRMODPARAMS_MAXSTATUS,temp);
	fprintf(outfile,"     Take Status Every (units): %s\n",temp.GetString());
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
	fprintf(outfile,"CURRENT CONFIGURATION (from INI file)\n");
	char buff[_MAX_PATH];
	GetPrivateProfileString(m_pParent->m_pName,"SLOW_RESET","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SLOW_RESET=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,PORT,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," PORT=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,NODE,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," NODE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,SAVE_LOC,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SAVE_LOC=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FILE_ID,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," FILE_ID=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"LOG_FILTER","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," LOG_FILTER=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"DO_DUMP","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," DO_DUMP=%s\n",buff);
//	GetPrivateProfileString(m_pParent->m_pName,"LOG_MII","",buff,sizeof(buff),m_pParent->m_szIniFile);
//	fprintf(outfile," LOG_MII=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"HIGHVOLTH","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," HIGHVOLTH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"HIGHVOLTL","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," HIGHVOLTL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"BATVOLTH","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," BATVOLTH=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"BATVOLTL","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," BATVOLTL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P15H","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP_P15H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P15L","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP_P15L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_M15H","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP_M15H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_M15L","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP_M15L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P05H","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP_P05H=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"SUP_P05L","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," SUP-P05L=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"TIME_ERR","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," TIME_ERR=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXCYCLE","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," MAXCYCLE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXPAUSE","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," MAXPAUSE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXBBM","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," MAXBBM=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXINQUIRE","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," MAXINQUIRE=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"MAXSTATUS","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," MAXSTATUS=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,"COMMFAIL","",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," COMMFAIL=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FLAGTIME,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," FLAGTIME=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FLAGCOMM,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," FLAGCOMM=%s\n",buff);
	GetPrivateProfileString(m_pParent->m_pName,FLAGOTHR,"",buff,sizeof(buff),m_pParent->m_szIniFile);
	fprintf(outfile," FLAGOTHR=%s\n",buff);
	fprintf(outfile,"%s ===== %s ================\n",m_pCaption,datetime.GetString());
}

void CISRPropertySheet::OnPrintButton()
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

void CISRPropertySheet::OnShowWindow(BOOL bShow, UINT nStatus) 
{
 	CPropertySheet::OnShowWindow(bShow, nStatus);

	if (bShow && nStatus==0) 
	{
		SetActivePage(0);
		m_dToClose = 0;
		SetTimer(1,1000,NULL);
		CISRModParam* modparpage = m_pParent->m_pModParams;

		if (m_pParent->IsPaused())
			modparpage->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(true);
		else
			modparpage->GetDlgItem(IDC_ISRMODPARAMS_DATETIME)->EnableWindow(false);
	}
	else
		KillTimer(1);
}

void CISRPropertySheet::OnToCloseButton()
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

//MIC_CB_028 to MIC 1.907 PJM 1/26/05
void CISRPropertySheet::OnExtendClose(WPARAM,LPARAM)
{
	m_dToClose = 0;
}

void CISRPropertySheet::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1)
	{
		if (m_dToClose >= m_pParent->m_iDlgCloseMilliseconds/1000)
		{
			m_dToClose = 0;
			//<<QA>> grand model hides modparm here too
			ShowWindow(SW_HIDE);
			KillTimer(1);
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


void CISRPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//<<QA>> Grand model hides modparams page, too.
	//watch for alt-f4 to try to close us -- don't let it
	//m_dToClose = 0;  //<<QA>> reset autotimout every time window was moved pjm 6/13/03
	if (nID == SC_CLOSE)
		ShowWindow(SW_HIDE);
	else
  		CPropertySheet::OnSysCommand(nID, lParam);
}


BOOL CISRPropertySheet::OnInitDialog() 
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

	DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE |
		  BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	m_pCloseButton->Create( "Close", dwStyle,
		  CRect(Interspace, Y1,
				Interspace + ButtonWidth,Y2),
		  this,
		  ID_CLOSE_BUTTON);

	m_pPauseButton->Create(COLLECTINGBEGIN, dwStyle,
		  CRect(2*Interspace + ButtonWidth,Y1,
				2*Interspace + 2*ButtonWidth,Y2),
		  this,
		  ID_PAUSE_BUTTON);

	m_pAckFlagsButton->Create("Acknowledge Flags",dwStyle,
		  CRect(3*Interspace + 2*ButtonWidth,Y1,
				3*Interspace + 3*ButtonWidth,Y2),
		  this,
		  ID_ACKFL_BUTTON);

	m_pPrintButton->Create("Snapshot Status\nTo File",dwStyle,
		  CRect(4*Interspace + 3*ButtonWidth,Y1,
				4*Interspace + 4*ButtonWidth,Y2),
		  this,
		  ID_PRINT_BUTTON);

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
	
	GetTabControl()->SetImageList(m_pImageList);
	TC_ITEM pTabCtrlItem;
	pTabCtrlItem.mask = TCIF_IMAGE;
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(1,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(2,&pTabCtrlItem);
	pTabCtrlItem.iImage = 1;
	GetTabControl()->SetItem(3,&pTabCtrlItem);

	return bResult;
}

BOOL CISRPropertySheet::DestroyWindow() 
{
	GetTabControl( )->SetImageList(NULL);
	return CPropertySheet::DestroyWindow();
}

void CISRPropertySheet::SetToClose(int set)
{
	//if (m_dToClose > 0)
		m_dToClose = set;
}

void CISRPropertySheet::ForceDateTime(void)
{
	m_dToClose = 0;
	m_pParent->PostMessage(uIDC_RECEIVE,FORCEDATETIME,CHECKSUM_GOOD);
}

CString *CISRPropertySheet::GetVersionString()
{
	return &m_pParent->m_csVersion;
}
