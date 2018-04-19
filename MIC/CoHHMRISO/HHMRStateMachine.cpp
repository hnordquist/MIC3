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
// HHMRStateMachine.cpp

#include "stdafx.h"
#include "HHMRInst.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//this is the state machine which controls 
//	what commands to send, 
//	when to send them, and
//	what to expect in return.
//It receives a message for each message returned from an instrument
//as well as a "timer" message on intervals selected in "MIC:Configuration"
LRESULT CHHMRInst::OnReceive(WPARAM WParam, LPARAM LParam)
{
//TRACE("Current State:%d\r\n",m_dCurrent_State);
	
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm.
	CString temp;
	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if (!m_bGoodRegister || m_bBlockWrite)
		m_dCurrent_State = 9900;

/*	if (WParam == TRIGGER_CAMERA_REQUEST)
	{
		if (m_dParallelPortPin && !m_bCameraBlocked)
		{
			//if (current time < suppression end time) then return
			COleDateTime now = COleDateTime::GetCurrentTime();
			if (now < m_pSuppressUntil) 
			{
				m_pSummaryPage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
				temp += " Suppressed";
				m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
				return 0; 
			}
			//set timer to post TRIGGER_CAMERA_START at now + holdoffspan
			SetTimer(TIMER_CAMERA_START,(unsigned)m_pCameraHoldOff.GetTotalSeconds()*1000+1,NULL);
			//set suppression end time at now + m_pCameraSuppress
			m_pSuppressUntil = now + m_pCameraSuppress;
		}
		else
		{
			m_pSummaryPage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
			temp += " Deactivated";
			m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
		}
		return 0;
	}

	else if (WParam == TRIGGER_CAMERA_START_TEST)
	{
		//set parallel port high
		if (m_dParallelPortPin  && !m_bCameraBlocked)
		{
 			//set the pin on
			m_cParallelPort.SetPin(m_dParallelPortPin);

			//tell the world
            m_pSummaryPage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
			temp += " Do";
            m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);

			COleDateTime now = COleDateTime::GetCurrentTime();
			temp = now.Format("%Y.%m.%d %H:%M:%S Test Do");                         
			m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);

			//set timer TIMER_CAMERA_END
			SetTimer(TIMER_CAMERA_END,600,NULL);
			m_pPropertySheet->WriteToFile(TO_CEV, TYPE_COMP, NULL,"53035 Camera Triggered Test");
		}
		return 0;
	}


	else if (WParam == TRIGGER_CAMERA_START)
	{
		//set parallel port high
		if (m_dParallelPortPin && !m_bCameraBlocked)
		{
			try
			{
				//set the pin on
				m_cParallelPort.SetPin(m_dParallelPortPin);
			}
			catch(...)
			{
				return 0;
			}

			//tell the world
			m_pSummaryPage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
			temp += " Do";
			m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);

			//set timer TIMER_CAMERA_END
			SetTimer(TIMER_CAMERA_END,600,NULL);
			m_pPropertySheet->WriteToFile(TO_CEV, TYPE_COMP, NULL,"53030 Camera Triggered");
		}
		return 0;
	}

	else if (WParam == TRIGGER_CAMERA_END)
	{
		//set parallel port low (don't forget to initialize it low)
		if (m_dParallelPortPin && !m_bCameraBlocked)
		{
			m_cParallelPort.UnSetPin(m_dParallelPortPin);
			m_pSummaryPage->GetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
			temp += "ne";
			m_pSummaryPage->SetDlgItemText(IDC_HHMRSUMMARY_CAMERA_TRIG,temp);
		}
		return 0;
	}*/

	else if (WParam == TIMER_TICK)
	{
		//bump time increments to next inquire
		m_dTickToInquire++;
		//bump time from last command
		m_dTick_Count++;
		if ((m_dRetransmit > m_dCommFail) &&
			(m_dCurrent_State != 9010) &&
			(m_dCurrent_State != 9500))
		{		
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dCurrent_State = 8000; //hard fail
		}
	}

	if (m_dCurrent_State < 9000)
		m_bInShutDown = false;
	else
		m_bInShutDown = true;

	if (m_dCurrent_State >= 2000)
		m_bInStartUp = false;
	else
		m_bInStartUp = true;
	//this is the virtual instrument state machine
	switch (m_dCurrent_State)
	{
	//start by sending ^ to force mode to computer
	case 1000:
		if (WParam == RESETUP)
		{
			char resetstring[128];
			char temp[16];

			union UNNAMED {
				int iInteger;
				BYTE bBytes[4];
			} intBytes;

			intBytes.iInteger = GetPrivateProfileInt(m_pName,"COUNTTIMEUNIT",10,m_szIniFile);
//			int iExp = GetPrivateProfileInt(m_pName,"COUNTTIMEEXP",0,m_szIniFile);
//			for (int i = 0; i < (iExp-1); i++)
//				intBytes.iInteger *= 10;
			temp[0] = 'T';
			temp[1] = 'W';
			temp[2] = intBytes.bBytes[2];
			temp[3] = intBytes.bBytes[1];
			temp[4] = intBytes.bBytes[0];
			COM_Transmit(5,temp,m_iNode);
			sprintf(temp,"%c%c[%02x%02x%02x] ",
				temp[0],temp[1],intBytes.bBytes[2],intBytes.bBytes[1],intBytes.bBytes[0]);
			strcpy(resetstring,temp);

			GetPrivateProfileString(m_pName,"PREDELAY","00",temp,sizeof(temp),m_szIniFile);
			sscanf(temp,"%x",&intBytes.iInteger);
			intBytes.iInteger = (intBytes.iInteger+1)*2;

			temp[0] = 'P';
			temp[1] = 'W';
			temp[2] = intBytes.bBytes[1];
			temp[3] = intBytes.bBytes[0];
			COM_Transmit(4,temp,m_iNode);
			sprintf(temp,"%c%c[%02x%02x] ",
				temp[0],temp[1],intBytes.bBytes[1],intBytes.bBytes[0]);
			strcat(resetstring,temp);

			intBytes.iInteger = GetPrivateProfileInt(m_pName,"GATEWIDTH",100,m_szIniFile)*4;
			temp[0] = 'G';
			temp[1] = 'W';
			temp[2] = intBytes.bBytes[1];
			temp[3] = intBytes.bBytes[0];
			COM_Transmit(4,temp,m_iNode);
			sprintf(temp,"%c%c[%02x%02x] ",
				temp[0],temp[1],temp[2],temp[3]);
			strcat(resetstring,temp);

			intBytes.iInteger = GetPrivateProfileInt(m_pName,"HIGHV",100,m_szIniFile);
			temp[0] = 'W';
			temp[1] = 'W';
			temp[2] = intBytes.bBytes[1];
			temp[3] = intBytes.bBytes[0];
			COM_Transmit(4,temp,m_iNode);
			sprintf(temp,"%c%c[%02x%02x]",
				temp[0],temp[1],intBytes.bBytes[1],intBytes.bBytes[0]);
			strcat(resetstring,temp);

			char cBuff[1024];
            strcpy(cBuff,"53040 HHMR Reset: ");
			strcat(cBuff,resetstring);
//			strcat(cBuff,"\r\n");
			m_pPropertySheet->UpdateSummary(cBuff);
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

			if (m_bDoDump)
			{
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
			}
			break;

		}

		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		if (WParam == TIMER_TICK)
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
			COM_Transmit(ISZSTRING,"HX",m_iNode);
			m_dRetransmit++;
			m_pPropertySheet->UpdateSummary("HX - Stop and get status");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53050 HX - Stop and get status (1000)");
			m_dCurrent_State = 1010;
		}


		break;

	//wait for response 
	case 1010:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//got a "?0000000" back 
		if (WParam == HHMR_READ_STATUS)
		{
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1100;//good response so start it up
		}

		//if we didn't get anything back 
		//in the alloted time so assume Front Panel Mode
		if (m_dTick_Count <= m_dTimeOut)
			break;


		m_pButton->put_FaceColor(COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);

		if (m_dCurrent_State != 1100)
			m_dCurrent_State = 1000;//^X transmit //was 1020
	
//		if (m_dRetransmit > m_dCommFail)
//		{
//			m_dRetransmit = 0;
//			m_dCurrent_State = 8000;
//		}
//
		break;

	case 1100:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		{
			COleDateTime cNow = COleDateTime::GetCurrentTime();
			m_pDataStatus->SetDlgItemText(IDC_HHMRDATA_SETUPTIME,cNow.Format("%Y.%m.%d %H:%M:%S"));
		}
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_pPropertySheet->PrepForReceipt(HHMR_READ_PREDELAY,true);
		COM_Transmit(2,"PR",m_iNode);
		m_pPropertySheet->UpdateSummary("PR -- Read Pre-delay");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 PR - Get pre-delay value (1100)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1101;
		break;


	case 1101:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET PRE-DELAY response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_PREDELAY)  
			break;
		m_dCurrent_State = 1102;
		m_dRetransmit = 0;
		//fall through

	case 1102:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_COUNTTIME,true);
		COM_Transmit(2,"TR",m_iNode);
		m_pPropertySheet->UpdateSummary("TR -- Read Count Time");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 TR - Get count time (1100)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1103;
		break;

	case 1103:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1102;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET COUNT TIME response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_COUNTTIME)  
			break;

		if (LParam)
		{
//			m_dInquireInt = (int)((float)LParam/10.0/((float)m_dCycleInt/1000.0))-5;
			//m_dInquireInt = (int)((float)LParam/10.0/((float)m_dCycleInt/1000.0))-m_dInquireIntOffset;
			m_dInquireInt = (int)((float)LParam/10.0/((float)m_dCycleInt/1000.0));
			m_cSpan.SetDateTimeSpan(0,0,0,(int)((float)LParam/10.0)-1);
			m_dSpan = (int)((float)LParam*100.0)-m_dCycleInt;// in mSec; back off one cycle
		}
		else
		{
			double temp = (double)m_dCountTimeUnit;// * pow(10,m_dCountTimeExp-1);
//			m_dInquireInt = (int)(temp/((double)m_dCycleInt/1000.0))-5;
			m_dInquireInt = (int)(temp/((double)m_dCycleInt/1000.0));
			m_cSpan.SetDateTimeSpan(0,0,0,(m_dInquireInt)-1);
			m_dSpan = m_dInquireInt*1000-m_dCycleInt;//back off one inquire cycle to start
			//m_dInquireInt -= m_dInquireIntOffset;
			//the -5 is to give ourself about 1 second
		}

		m_dCurrent_State = 1104;
		m_dRetransmit = 0;
		//fall through

	case 1104:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_GATEWIDTH,true);
		COM_Transmit(2,"GR",m_iNode);
		m_pPropertySheet->UpdateSummary("GR -- Read Gate Width");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 GR - Get GET GATE WIDTH (1100)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1105;
		break;

	case 1105:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1104;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET GATE WIDTH response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_GATEWIDTH)  
			break;

		m_dCurrent_State = 1106;
		m_dRetransmit = 0;
		//fall through

	case 1106:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_HV_SP,true);
		COM_Transmit(2,"WS",m_iNode);
		m_pPropertySheet->UpdateSummary("WS -- Read HV Set Point");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 WS - Get GET HV SET POINT (1100)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1107;
		break;

	case 1107:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1106;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET HV SET POINT response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_HV_SP)  
			break;

		m_dCurrent_State = 1108;
		m_dRetransmit = 0;
		//fall through

	case 1108:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_HV,true);
		COM_Transmit(2,"WR",m_iNode);
		m_pPropertySheet->UpdateSummary("WR -- Read High Voltage");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 WR - Get GET HV (1100)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1109;

//m_dCurrent_State = 1150;
		break;

	case 1109:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1108;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET HV response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_HV)  
			break;

		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		m_dCurrent_State = 1150;
		break;
		
	case 1150://wait a short time
		if (m_dTick_Count >= (600.0/(float)m_dCycleInt))
		{
			//set the number of times command has been sent
			m_dRetransmit = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 1160;
		}
		break;

	case 1160:
		//send the start command
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);

		m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
		COM_Transmit(1,"S",m_iNode);

		m_cStart = COleDateTime::GetCurrentTime();
		m_cFinish = m_cStart + m_cSpan; 

		m_dStart = GetTickCount();
		m_dFinish = m_dStart + m_dSpan;

		m_pPropertySheet->m_LastStart = COleDateTime::GetCurrentTime();
		m_pPropertySheet->UpdateSummary("S -- Start");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53140 S - Start");
		//won't repeat this one, so don't need to count retransmits 
		//incr the number of times command has been sent
		//m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1161;
		break;

	case 1161:
		if (m_dTick_Count > 10)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1165;
		}
		break;

	case 1165://read status to see if it started
		if (m_dTick_Count >= (600.0/(float)m_dCycleInt))
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			m_dTick_Count = 0;
			m_dCurrent_State = 1170;
			m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
			COM_Transmit(1,"X",m_iNode);
			m_pPropertySheet->UpdateSummary("X -- Read Status");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53140 X - Read Status");
		}
		break;

	case 1170:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1000;
			m_pPropertySheet->UpdateSummary("Timeout on receive initial status response");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53150 HHMR Timeout on receive initial status response");
		} 

		//break out and wait for correct response
		if (WParam == HHMR_READ_STATUS)  
		{
			m_pButton->put_FaceColor(COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);

			m_dRetransmit = 0;
			m_dTick_Count = 0;
			if (LParam == 0)//1 is running
                m_dCurrent_State = 1000;//total reset, couldn't start HHMR
			else//0 is counting
			{
				m_dCurrent_State = 2000;//looks good
				m_dTickToInquire = 0;
			}
		}
		break;
		

	//state 2000 is the "normal" resting state
	case 2000:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		//tell the buttons we are now waiting for expected data record
		//and start a fast query to find when we are done--do this as
		//fast as possible so as to not waste any time 
/************************************************************************
*	
* this change causes the state machine to go into "fast query" about
* one second before the end of measurement on every cycle.  The prev
* technique would guess when the end was and then would have to adjust
* depending on how busy and how slow the computer running this was.
* consequently sometimes the transition wouldn't occur until way after 
* (as in 3 or 4 seconds) the measurement was complete.
*
************************************************************************/
		if ((m_dFinish <= GetTickCount()) ||
			(GetTickCount() < m_dStart) && (m_cFinish <= COleDateTime::GetCurrentTime()))
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"X",m_iNode);
			//m_dTransmitXCount = 0;
			m_dRetransmit++;
			m_dTick_Count = 0;
			m_dCurrent_State = 2100;
			break;
		}
		break;

	case 2100:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		//if enough time has passed that we may need to reset instrument
		if ((m_dTick_Count >= m_dTimeOut) && (WParam != HHMR_READ_STATUS))
		{
			char cBuff[1024];
			strcpy(cBuff,"53160 Late Response (2100)");

			m_pPropertySheet->UpdateSummary("Late Response (2100)");
			m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, cBuff);
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);


			m_dCurrent_State = 2000; 
			m_dTick_Count = 0;
			break;
		} 

		if (WParam == HHMR_READ_STATUS)
		{
			m_pButton->put_FaceColor(COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);

			m_dRetransmit = 0;
			//if done then go to next state
			if (LParam == 0) //stopped
			{
				m_dCurrent_State=3000;
				//fall through -- don't break
				if ((m_bDoDump)||(m_pSummaryPage->m_bDebug))
					GetLocalTime(&m_Then);
			}
			else
			{
				m_pButton->put_FaceColor(COLOR_WAIT);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);

				m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
				COM_Transmit(1,"X",m_iNode);
				m_dTickToInquire = 0;
				//m_dTransmitXCount++;
				m_dRetransmit++;
				m_dTick_Count = 0;
				//this will cause a "fast" resend of X
				m_dCurrent_State = 2100;
				break;
			}
		}
		else
			break;
		m_dRetransmit++;

	case 3000:	//send read
		//get the data as fast as we can so we can get the psr restarted taking data
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_AUX_SCALERS,true);
		COM_Transmit(1,"Y",m_iNode);
		m_pPropertySheet->UpdateSummary("Y -- Read Scalers");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53170 Y - Get Aux values (3000)");
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//reset retransmit
		m_dRetransmit++;
		//next state will wait for a good response
		m_dCurrent_State = 3100;
		break;

	case 3100:
		//wait for response
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3000;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53180 HHMR Timeout on receive AUX data response");

			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53180 HHMR Timeout on receive AUX data response (3100)");

		} 
		//break out and wait for correct response
		if (WParam != HHMR_READ_AUX_SCALERS)
			break;

		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_dRetransmit = 0;
		m_dCurrent_State = 3200;
		m_dTick_Count = 0;
		//fall through to read aux data

	case 3200:	//send read
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_T_TA_A,true);
		COM_Transmit(1,"R",m_iNode);
		m_pPropertySheet->UpdateSummary("R -- Read T, R+A, & A");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53190 R - Get TOTALS etc. values (3200)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3300;
		break;

	case 3300:
		if (m_dTick_Count > (m_dTimeOut + 1))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3200;
			m_pPropertySheet->UpdateSummary("Timeout on receive DATA response");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53200 HHMR Timeout on receive DATA response");
		} 
		//break out and wait for correct response
		if (WParam != HHMR_READ_T_TA_A)  
			break;

		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		//start another measurement

		m_dTickToInquire = 0;

/********************************************************************************
*
*	NOTE: To speed things up further I've added code to see if the instruments
*	settings have changed only one in 10 measuremnts.  This will significantly
*	help make dead time between mesasuements smaller.
*
*********************************************************************************/

		m_iCycleCount++;
		if (m_iCycleCount > 10)
		{
			m_dCurrent_State = 1100;
			m_iCycleCount = 0;
		}
		else
			m_dCurrent_State = 1160;

/*
		m_pPropertySheet->PrepForReceipt(HHMR_UNKNOWN_RECORD,true);
		COM_Transmit(1,"S",m_iNode);
		m_pPropertySheet->m_LastStart = COleDateTime::GetCurrentTime();
		m_dTickToInquire = 0;
		m_pPropertySheet->UpdateSummary("S -- Restart");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53210 S - Start (3300)");

		m_dRetransmit = 0;
		m_dTick_Count = 0;
		//get rest of data
		m_dCurrent_State = 3401;
*/
		break;

	case 3401://wait a reasonable time or not
//		if (m_dTick_Count >= (1000.0/(float)m_dCycleInt))
//		if (m_dTick_Count > (100.0/(float)m_dCycleInt))

		if (m_dTick_Count > 10)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3400;
			m_dRetransmit = 0;
		}
		else 
			break;
		//fall through iff if above is true

	case 3400:
		//send the read status command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
		COM_Transmit(1,"X",m_iNode);
		m_pPropertySheet->UpdateSummary("X -- Read Status (3400)");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53220 X - get status (3400)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3410;
		//get the system time
		GetSystemTime(&m_sStartTime);
		break;

	case 3410:
		//if we don't get a response in a reasonable time try to reset
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_pPropertySheet->UpdateSummary("Timeout on receive status response");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53230 HHMR Timeout on receive status response");
		} 

		if (WParam == HHMR_READ_STATUS)
		{
			char szTemp[32];
			m_dTick_Count = 0;
			if (LParam == 0) //it didn't start
			{
				//try again
				m_dCurrent_State = 3400;

				//immediately try again if not started
				m_pPropertySheet->UpdateSummary("S -- Start");
				m_pPropertySheet->PrepForReceipt(HHMR_READ_STATUS,true);
				COM_Transmit(1,"S",m_iNode);
				//incr the number of times command has been sent

			}
			else	//it did start
			{
				m_dRetransmit = 0;
				m_dCurrent_State = 3500;//looks good

				if ((m_bDoDump)||(m_pSummaryPage->m_bDebug))
				{
					SYSTEMTIME Now;
					GetLocalTime(&Now);
					sprintf(szTemp,"53260 Dead(msec): %d",CalculateDiff(&Now,&m_Then));

					SYSTEMTIME sStartTime;
					GetSystemTime(&sStartTime);
					int iDiff = CalculateDiff(&sStartTime,&m_sStartTime);


					//put in debug window if required
					if (m_pSummaryPage->m_bDebug)
						m_pSummaryPage->SetDlgItemText(IDC_STATIC_NOTE3,szTemp);
					//put in dump file if required
					if (m_bDoDump)
					{
						//strcat(szTemp,"\n");
						m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,szTemp);
					}
					sprintf(szTemp,"53270 Time: %d",iDiff);
					//put in window if required
					if (m_pSummaryPage->m_bDebug)
						m_pPropertySheet->UpdateSummary(szTemp);

					if (m_bDoDump)
					{
						//strcat(szTemp,"\n");
						m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,szTemp);
					}
				}
			}
		}

		break;

	//we've started the HHMR now we want to read some values back
	case 3500:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_pPropertySheet->PrepForReceipt(HHMR_READ_PREDELAY,true);
		COM_Transmit(2,"PR",m_iNode);
		m_pPropertySheet->UpdateSummary("PR -- Read Pre-delay");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 PR - Get pre-delay value (3500)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3501;
		break;

	case 3501:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3500;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET PRE-DELAY response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_PREDELAY)  
			break;
		m_dCurrent_State = 3502;
		m_dRetransmit = 0;
		//fall through

	case 3502:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_COUNTTIME,true);
		COM_Transmit(2,"TR",m_iNode);
		m_pPropertySheet->UpdateSummary("TR -- Read Count Time");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 TR - Get count time (3502)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3503;
		break;

	case 3503:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3502;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET COUNT TIME response");
		} 
		//break out and wait for correct response
        if (WParam != HHMR_READ_COUNTTIME)  
			break;

//		if (LParam)
//			m_dInquireInt = (int)((float)LParam/10.0/((float)m_dCycleInt/1000.0))-5;
//		else
//		{
//			double temp = (double)m_dCountTimeUnit;// * pow(10,m_dCountTimeExp-1);
//			m_dInquireInt = (int)(temp/((double)m_dCycleInt/1000.0))-5;
//		}
//		//adjust for how busy the system is
//		if (m_dTransmitXCount == 0)
//			m_dInquireIntAdjust -= 5;
//		else if (m_dTransmitXCount == 1)
//			m_dInquireIntAdjust -= 2; 
//		else if (m_dTransmitXCount < 5)
//			m_dInquireIntAdjust--;
//		else if ((m_dTransmitXCount > 30)&&(m_dTransmitXCount < 80))
//			m_dInquireIntAdjust++;
//		else if (m_dTransmitXCount >= 80)
//			m_dInquireIntAdjust += (m_dTransmitXCount/30);
//
//		m_dInquireInt += m_dInquireIntAdjust;

		m_dCurrent_State = 3504;
		m_dRetransmit = 0;
		//fall through

	case 3504:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_GATEWIDTH,true);
		COM_Transmit(2,"GR",m_iNode);
		m_pPropertySheet->UpdateSummary("GR -- Read Gate Width");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 GR - Get GET GATE WIDTH (3504)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3505;
		break;

	case 3505:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3504;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET GATE WIDTH response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_GATEWIDTH)  
			break;

		m_dCurrent_State = 3506;
		m_dRetransmit = 0;
		//fall through

	case 3506:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_HV_SP,true);
		COM_Transmit(2,"WS",m_iNode);
		m_pPropertySheet->UpdateSummary("WS -- Read HV Set Point");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 WS - Get GET HV SET POINT (3506)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3507;
		break;

	case 3507:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3506;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET HV SET POINT response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_HV_SP)  
			break;

		m_dCurrent_State = 3508;
		m_dRetransmit = 0;
		//fall through

	case 3508:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		m_pPropertySheet->PrepForReceipt(HHMR_READ_HV,true);
		COM_Transmit(2,"WR",m_iNode);
		m_pPropertySheet->UpdateSummary("WR -- Read High Voltage");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 WR - Get GET HV (3508)");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3509;
		break;

	case 3509:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3508;
            m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 HHMR Timeout on receive GET HV response");
		} 
		//beak out and wait for correct response
        if (WParam != HHMR_READ_HV)  
			break;

		m_dRetransmit = 0;

		m_pButton->put_FaceColor(COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_OK,true);

		m_dCurrent_State = 2000;
		break;

	case 8000:
		FlagComm(1,true);
		m_pButton->put_FaceColor(COLOR_RED);
		//m_pButton->SetErrorIcon_Flag();
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_RED);
		m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP,NULL,"53990 HHMR Resetting serial port");
		COM_Transmit(9,"CLOSEPORT",m_iNode);
		m_dTick_Count = 0;
		m_dCurrent_State = 8100;
		break;

	case 8100:
		if (m_dTick_Count > m_dTimeOut)
		{
			COM_Transmit(8,"OPENPORT",m_iNode);
			m_dCurrent_State = 1000;
		}
		break;

		//long hard fail recovery
	case 9000:							//	900sec/15min
		if ((m_dTick_Count * m_dCycleInt) > m_dSlowReset)//wait 15 minutes before we retry
		{
			m_dTick_Count = 0;
//			m_dHardFailCount = 0;
			m_pButton->SetErrorIcon_Flag();
			m_dCurrent_State = 1000;
		}

		if ((m_dTick_Count % 10) == 0)
		{
			//m_pButton->put_FlagChar(SKULL_CHAR,false);
			m_pButton->SetErrorIcon_Skull();
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
			FlagComm(1,true);			
		}
		break;

	case 9800:
		m_dCurrent_State = 9810;
		PauseInstrument(true);	
//		m_pButton->SetFlagChar(SKULL_CHAR,false);
//		m_pButton->SetFlag(true,false);
		m_pButton->put_FaceColor(COLOR_ERROR);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "53320 HHMR WRITE FAILURE on HHMR file");
		break;

	case 9810:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
		}
		else
		{	
			m_bIsPaused = false;
			m_dCurrent_State = 1000;
		}
		break;

	case 9900:
		if (m_bGoodRegister)
			m_dCurrent_State = 1000;
		else
		{
			m_dCurrent_State = 9900;
			m_pButton->SetErrorIcon_Bomb();
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR);
		}
		break;
	default:
		m_pPropertySheet->UpdateSummary("State Machine Failure");
		m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"53330 State Machine Failure");
		m_dCurrent_State = 1000;
	}
	return 0;
}