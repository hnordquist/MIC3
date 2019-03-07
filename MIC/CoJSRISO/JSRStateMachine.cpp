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
// JSRStateMachine.cpp

#include "stdafx.h"
#include "JSRInst.h"
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
LRESULT CJSRInst::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm.
	//<<QA>> PJM: JSR does not have BBM.  Not sure if the above comment is
	//<<QA>> otherwise correct.

	CString temp;
	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if (!m_bGoodRegister || m_bBlockWrite)
		m_dCurrent_State = 9900;

	/*if (WParam == TRIGGER_CAMERA_REQUEST)
	{
		if (m_dParallelPortPin && !m_bCameraBlocked)
		{
			//if (current time < suppression end time) then return
			COleDateTime now = COleDateTime::GetCurrentTime();
			if (now < m_pSuppressUntil) 
			{
				m_pSummaryPage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
				temp += " Suppressed";
				m_pSummaryPage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
				return 0; 
			}
			//set timer to post TRIGGER_CAMERA_START at now + holdoffspan
			SetTimer(TIMER_CAMERA_START,(unsigned)m_pCameraHoldOff.GetTotalSeconds()*1000+1,NULL);
			//set suppression end time at now + m_pCameraSuppress
			m_pSuppressUntil = now + m_pCameraSuppress;
		}
		else
		{
			m_pSummaryPage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
			temp += " Deactivated";
			m_pSummaryPage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
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
			//m_pSummaryPage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);

			COleDateTime now = COleDateTime::GetCurrentTime();
			temp = now.Format("%Y.%m.%d %H:%M:%S Test Do");                         
			m_pSummaryPage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);

			//set timer TIMER_CAMERA_END
			SetTimer(TIMER_CAMERA_END,600,NULL);
			m_pPropertySheet->WriteToFile(TO_CEV, TYPE_COMP, NULL,"53035 Camera Triggered Test\n");
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
			m_pSummaryPage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
			temp += " Do";
			m_pSummaryPage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);

			//set timer TIMER_CAMERA_END
			SetTimer(TIMER_CAMERA_END,600,NULL);
			m_pPropertySheet->WriteToFile(TO_CEV, TYPE_COMP, NULL,"53030 Camera Triggered\n");
		}
		return 0;
	}

	else if (WParam == TRIGGER_CAMERA_END)
	{
//		TRACE("CJSRInst::OnReceive(WParam = TRIGGER_CAMERA_END)\n");

		//set parallel port low (don't forget to initialize it low)
		if (m_dParallelPortPin && !m_bCameraBlocked)
		{
			m_cParallelPort.UnSetPin(m_dParallelPortPin);
			m_pSummaryPage->GetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
			temp += "ne";
			m_pSummaryPage->SetDlgItemText(IDC_JSRSUMMARY_CAMERA_TRIG,temp);
		}

		return 0;
	}

	else */if (WParam == TIMER_TICK)
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
				m_dCurrent_State = 9000; //hard fail
		}
	}

	if (m_dCurrent_State < 9000)
		m_bInShutDown = false;
	else
		m_bInShutDown = true;

	if (m_dCurrent_State >= 1100)
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
			char resetstring[64];
			char temp[16];
			GetPrivateProfileString(m_pName,"COUNTTIMEUNIT","10",temp,sizeof(temp),m_szIniFile);
			while (strlen(temp) < 2)
			{
				temp[2] = temp[1];
				temp[1] = temp[0];
				temp[0] = '0';
			}
			strcpy(resetstring,"U");
			strcat(resetstring,temp);
			GetPrivateProfileString(m_pName,"COUNTTIMEEXP","0",temp,sizeof(temp),m_szIniFile);
			temp[1] = NULL;
			strcat(resetstring,"E");
			strcat(resetstring,temp);
			GetPrivateProfileString(m_pName,"PREDELAY","1",temp,sizeof(temp),m_szIniFile);
			temp[1] = NULL;
			strcat(resetstring,"P");
			strcat(resetstring,temp);
			GetPrivateProfileString(m_pName,"GATEWIDTH","100",temp,sizeof(temp),m_szIniFile);
			while (strlen(temp) < 3)
			{
				temp[2] = temp[1];
				temp[1] = temp[0];
				temp[0] = '0';
			}
			temp[3] = NULL;
			strcat(resetstring,"G");
			strcat(resetstring,temp);
			GetPrivateProfileString(m_pName,"HIGHV","1800",temp,sizeof(temp),m_szIniFile);
			while (strlen(temp) < 4)
			{
				temp[3] = temp[2];
				temp[2] = temp[1];
				temp[1] = temp[0];
				temp[0] = '0';
			}
			temp[4] = NULL;
			strcat(resetstring,"V");
			strcat(resetstring,temp);
			strcat(resetstring,"J");
#ifdef _DEBUG
			//Indicate that new hardware commands are being
			//sent to the JSR
			Beep(200,300);	//SCR00114
#endif
			COM_Transmit(ISZSTRING,resetstring,m_iNode);
			m_pPropertySheet->UpdateSummary(resetstring);
			strcat(resetstring,"\n");

			char cBuff[1024];
			strcpy(cBuff,"53040 JSR Parameters reset: ");
			strcat(cBuff,resetstring);
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, cBuff);

			//m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"304 JSR Parameters reset\n");
			//m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,resetstring);

			if (m_bDoDump)
			{
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);
					//m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "304 JSR Parameters reset\n");
					//m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, resetstring);
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
			COM_Transmit(ISZSTRING,"HX",m_iNode);
			m_pPropertySheet->UpdateSummary("HX - In Computer Mode?");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53050 HX - In Computer Mode? (1000)\n");
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
		if (WParam == JSR_STATUS_RECORD)
		{
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
//			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response so start it up
		}

		//if we didn't get anything back 
		//in the alloted time so assume Front Panel Mode
		if (m_dTick_Count <= m_dTimeOut)
			break;

		m_dCurrent_State = 1020;//^X transmit
		//fall through

	case 1020:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		if (WParam == TIMER_TICK)
		{
//			m_pButton->SetFlagChar(FLAG_CHAR,true);
//			m_pButton->put_FaceColor(COLOR_WAIT);
//			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
//			m_dTickToInquire = 0;
//			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"\x08\x18",m_iNode);//^H^X
			m_pPropertySheet->UpdateSummary("^H^X - Terminal Mode?");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53060 ^H^X - Terminal Mode? (1020)\n");
			m_dCurrent_State = 1030;
		}
		break;

	case 1030:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		//got a "10000000" back 
		if (WParam == JSR_STATUS_RECORD)
		{
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
//			m_bInHardFail = false;
			m_dCurrent_State = 1040;//good response so enter computer mode
		}

		//if we didn't get anything back 
		//in the alloted time so assume Front Panel Mode
		if (m_dTick_Count <= m_dTimeOut)
			break;

		//never got anything so attempt reset
		m_dCurrent_State = 1140;//reset JSR with escape character
//		break;
		//fall through

	case 1040:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		if (WParam == TIMER_TICK)
		{
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"^",m_iNode);//^
			m_pPropertySheet->UpdateSummary("^ - In Computer Mode?");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53070 ^ - In Computer Mode? (1040)\n");
			m_dCurrent_State = 1050;
		}
		break;

	case 1050:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//got the expected "V 6.1 17-NOV-95" record back
		//we are in monitor mode
		if (WParam == JSR_VERSION_RECORD)
		{
			m_dRetransmit = 0;
//			m_bInHardFail = false;
			break;
		}

		//got a "10000000" back 
		if (WParam == JSR_STATUS_RECORD)
		{
			m_dRetransmit = 0;
//			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response so start it up
			break;
		}

	case 1060:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}

		if (WParam == TIMER_TICK)
		{
			m_pButton->SetErrorIcon_Flag();
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
//			m_dTickToInquire = 0;
//			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"\x1b",m_iNode);
			m_pPropertySheet->UpdateSummary("<esc> - reset JSR");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53080 <esc> - reset JSR (1060)\n");
			m_dRetransmit++;
			if (m_bInHardFail)
				m_dCurrent_State = 9000;
			else
			{
				m_bInHardFail = true;
				m_dCurrent_State = 1000;
			}
		}
		break;

	case 1100:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		COM_Transmit(ISZSTRING,"J",m_iNode);
		m_pPropertySheet->UpdateSummary("J");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53090 J - Get setup values (1100)\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1110;
		break;

	case 1110:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1000;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53100 JSR Timeout on receive SETUP response\n");
		} 
		//beak out and wait for correct response
		if (WParam != JSR_SETUP_RECORD)  
			break;
		if (LParam)
			m_dInquireInt = (int)((float)LParam/((float)m_dCycleInt/1000.0))-3;
		else
		{
			double temp = (double)m_dCountTimeUnit * pow(10,(double)(m_dCountTimeExp-1));
			m_dInquireInt = (int)(temp/((double)m_dCycleInt/1000.0))-3;
			//the -3 is to give ourself about 1 second
		}
		m_dTick_Count = 0;
		m_dCurrent_State = 1120;
		m_dRetransmit = 0;
//		break; //fall through

	case 1120:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
//		//we are in a dumplast sequence
//		m_bInDumpLast = false;
		//send the command
		COM_Transmit(ISZSTRING,"Y",m_iNode);
		m_pPropertySheet->UpdateSummary("Y");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53110 Y - Get actual HV (1120)\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1130;
		break;

	case 1130:
		if (m_dTick_Count > (m_dTimeOut+3))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			m_pPropertySheet->UpdateSummary("JSR Timeout on receive HV");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53120 JSR Timeout on receive actual HV response\n");
		} 
		//beak out and wait for correct response
		if (WParam != JSR_HV_RECORD)  
			break;

		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_dCurrent_State = 1140;

//		break; //fall through
	case 1140:
		if (m_bPause) 
		{
			m_dCurrent_State = 1000;
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = false;
		//send the command
		COM_Transmit(ISZSTRING,"Z&",m_iNode);
		m_pPropertySheet->m_LastStart = COleDateTime::GetCurrentTime();
		m_pPropertySheet->UpdateSummary("Z&&");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53130 Z& - Zero and Auto (1140)\n");
		m_dTickToInquire = 0;
		m_dRetransmit = 0;
		m_dTick_Count = 0;
		//next state will wait for a good response
//		m_dCurrent_State = 2000;
		//fall through
		m_dCurrent_State = 1150;
		break;

	case 1150://wait a short time
//		if (m_dTick_Count >= (1000.0/(float)m_dCycleInt))
		if (m_dTick_Count >= (600.0/(float)m_dCycleInt))//dgp
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1160;
		}
		break;

	case 1160:
		//send the read status command
		COM_Transmit(ISZSTRING,"X",m_iNode);
		m_pPropertySheet->UpdateSummary("X");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53140 X - get status (3400)\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1170;
		break;

	case 1170:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1000;
			m_pPropertySheet->UpdateSummary("Timeout on receive initial status response");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53150 JSR Timeout on receive initial status response\n");
		} 

		//break out and wait for correct response
		if (WParam == JSR_STATUS_RECORD)  
		{
			m_dRetransmit = 0;
			m_dTick_Count = 0;
			if (LParam == 1)
				m_dCurrent_State = 1000;//total reset, couldn't start jsr
			else
				m_dCurrent_State = 2000;//looks good
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
		if  (
			(WParam == TIMER_TICK)	&&
			(m_dTickToInquire >= m_dInquireInt) 
			)
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT);
			COM_Transmit(ISZSTRING,"X",m_iNode);
			m_dTransmitXCount = 0;
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
//		if (m_dTickToInquire >= (m_dInquireInt + 8))  
		if ((m_dTick_Count >= m_dTimeOut) && (WParam != JSR_STATUS_RECORD))
		{
			char cBuff[1024];
			strcpy(cBuff,"53160 Late Response (2100)\n");

			m_pPropertySheet->UpdateSummary("Late Response (2100)");
			m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL, cBuff);
		//	m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"Late Response - Resetting JSR (2100)\n");

			if (m_bDoDump)
			//	m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Late Response - Resetting JSR (2100)\n");
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, cBuff);


			m_dCurrent_State = 2000; 
			m_dTick_Count = 0;
			break;
		} 

		if (WParam == JSR_STATUS_RECORD)
		{
			m_dRetransmit = 0;
			if (LParam == 1)
			{
				m_dCurrent_State=3000;
				//fall through -- don't break
				if ((m_bDoDump)||(m_pSummaryPage->m_bDebug))
					GetLocalTime(&m_Then);
			}
			else
			{
				COM_Transmit(ISZSTRING,"X",m_iNode);
				m_dTickToInquire = 0;
				m_dTransmitXCount++;
				m_dRetransmit++;
				m_dTick_Count = 0;
				m_dCurrent_State = 2100;
				break;
			}
		}
		else
			break;

#define CR
#undef CR
	case 3000:	//send read
		//send the command
#ifdef CR
		COM_Transmit(ISZSTRING,"CR",m_iNode);//dgp
		m_pPropertySheet->UpdateSummary("CR");//dgp
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53170 CR - Get Aux value (3000)\n");
#else
		COM_Transmit(ISZSTRING,"C",m_iNode);//dgp
		m_pPropertySheet->UpdateSummary("C");//dgp
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53170 C - Get Aux value (3000)\n");
#endif
		//incr the number of times command has been sent
		m_dRetransmit++;
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
			m_dCurrent_State = 1100;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53180 JSR Timeout on receive AUX data response\n");

			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53180 Timeout on receive AUX data response (3100)\n");

		} 
		//break out and wait for correct response
		if (WParam != JSR_AUX_RECORD)
			break;

		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_dRetransmit = 0;
#ifdef CR
		m_dCurrent_State = 3300;//dgp
		m_dTick_Count = 0;//dgp
		break;//dgp
#endif
//		break;
		//fall through to read aux data

	case 3200:	//send read
		//send the command
		COM_Transmit(ISZSTRING,"R",m_iNode);
		m_pPropertySheet->UpdateSummary("R");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53190 R - Get values (3200)\n");
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
			m_dCurrent_State = 2100;
			m_pPropertySheet->UpdateSummary("Timeout on receive DATA response");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53200 JSR Timeout on receive DATA response\n");
		} 
		//break out and wait for correct response
		if (WParam != JSR_READ_RECORD)  
			break;

		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		//start another measurement
		COM_Transmit(ISZSTRING,"Z&",m_iNode);
		m_pPropertySheet->m_LastStart = COleDateTime::GetCurrentTime();
		m_dTickToInquire = 0;
		m_pPropertySheet->UpdateSummary("Z&&");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53210 Z& - Zero and Auto (3300)\n");

		m_dRetransmit = 0;
		m_dTick_Count = 0;
		//get rest of data
		m_dCurrent_State = 3401;
		break;

	case 3401://wait a reasonable time or not
//		if (m_dTick_Count >= (1000.0/(float)m_dCycleInt))
//		if (m_dTick_Count > (100.0/(float)m_dCycleInt))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 3400;
			m_dRetransmit = 0;
		}
//		break;

	case 3400:
		//send the read status command
		COM_Transmit(ISZSTRING,"X",m_iNode);
		m_pPropertySheet->UpdateSummary("X");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53220 X - get status (3400)\n");
//		//incr the number of times command has been sent
//		m_dRetransmit++;
		//use retrans to count bad responses
		m_dRetransmit = 0;
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
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53230 JSR Timeout on receive status response\n");
		} 

		//regardless of time out if we have a bunch of bad responses then reset
		if (WParam == JSR_UNKNOWN_RECORD)
		{
			//user retransmit to count error responses
			m_dRetransmit++;
			if (m_dRetransmit > 4)
			{
				m_dTick_Count = 0;
				m_dRetransmit = 0;
				m_dCurrent_State = 1000;
				m_pPropertySheet->UpdateSummary("Too many bad status responses");
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53240 JSR Too many bad status response\n");
			}
			else
				//immediately ask again if started count time
				COM_Transmit(ISZSTRING,"X",m_iNode);
		}
		//else we must have a status record (WParam == JSR_STATUS_RECORD)  
		else if (WParam == JSR_STATUS_RECORD)
		{
			char szTemp[32];
			m_dRetransmit = 0;
			m_dTick_Count = 0;
			if (LParam == 1)
			{
				//not started yet

				//next state will wait for a good response
				m_dCurrent_State = 3410;

				//calculate the time since first X command
				SYSTEMTIME sStartTime;
				GetSystemTime(&sStartTime);
				int iDiff = CalculateDiff(&sStartTime,&m_sStartTime);

				//don't wait any longer than 600 msec
				if (iDiff > 700)
				{
					m_dCurrent_State = 1000;//total reset, couldn't start jsr
					//don't immediately go to slow reset state
					m_dRetransmit = 0;
					m_pPropertySheet->UpdateSummary("Too much time to restart");
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53250 JSR Too much time to restart\n");
				}
				else
				{
					//immediately ask again if started count time
					COM_Transmit(ISZSTRING,"X",m_iNode);
					//incr the number of times command has been sent
				}
		
			}
			else
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
						strcat(szTemp,"\n");
						m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,szTemp);
					}
					sprintf(szTemp,"53270 Time: %d",iDiff);
					//put in window if required
					if (m_pSummaryPage->m_bDebug)
						m_pPropertySheet->UpdateSummary(szTemp);

					if (m_bDoDump)
					{
						strcat(szTemp,"\n");
						m_pPropertySheet->WriteToFile(TO_DMP,TYPE_DUMP,NULL,szTemp);
					}
				}
			}
		}

		break;

	case 3500:
		//send the command
		COM_Transmit(ISZSTRING,"J",m_iNode);
		m_pPropertySheet->UpdateSummary("J");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53280 J - Get Setup (3500)\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3510;
		break;

	case 3510:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53290 JSR Timeout on receive SETUP response\n");
		} 
		//break out and wait for correct response
		if (WParam != JSR_SETUP_RECORD)  
			break;

		//adjust for how busy the system is
		if (m_dTransmitXCount == 0)
			m_dInquireIntAdjust -= 10;
		else if (m_dTransmitXCount == 1)
			m_dInquireIntAdjust -= 2; 
		else if (m_dTransmitXCount < 5)
			m_dInquireIntAdjust--;
		else if ((m_dTransmitXCount > 30)&&(m_dTransmitXCount < 80))
			m_dInquireIntAdjust++;
		else if (m_dTransmitXCount >= 80)
			m_dInquireIntAdjust += (m_dTransmitXCount/30);

		if (LParam)
			m_dInquireInt = (int)((float)LParam/((float)m_dCycleInt/1000.0))-3;
		else
		{
			double temp = (double)m_dCountTimeUnit * pow(10,double(m_dCountTimeExp-1));
			m_dInquireInt = (int)(temp/((double)m_dCycleInt/1000.0))-3;
		}

		m_dInquireInt += m_dInquireIntAdjust;

		if (m_pSummaryPage->m_bDebug)
		{
			CString temp;
			temp.Format("Xs:%d Wait:%d",m_dTransmitXCount,m_dInquireInt);
			m_pSummaryPage->SetDlgItemText(IDC_STATIC_NOTE1,temp);
		}

		//reset retransmit
		m_dRetransmit = 0;
		//next logical state
		m_dCurrent_State = 3600;
		//fall through to next transmit

	case 3600:
		//send the command
		COM_Transmit(ISZSTRING,"Y",m_iNode);
		m_pPropertySheet->UpdateSummary("Y");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "53300 Y - Get HV\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 3610;
		break;

	case 3610:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			m_pPropertySheet->UpdateSummary("Timeout on receive HV");
			m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"53310 JSR Timeout on receive HV response (3610)\n");
		} 

		//break out and wait for correct response
		if (WParam != JSR_HV_RECORD)  
			break;

		//reset retransmit
		m_dRetransmit = 0;
		//not waiting for a respons
		m_dTick_Count = 0;
		//tell the world we are ok
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		//wait for timeout on "inquire"
		m_dCurrent_State = 2000;
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
			//m_pButton->SetFlagChar(SKULL_CHAR,false);
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
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "53320 WRITE FAILURE on JSR file\n");
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
		m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_COMP, NULL,"53330 State Machine Failure\n");
		m_dCurrent_State = 1000;
	}
	return 0;
}