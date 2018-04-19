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
// StateMachine.cpp

#include "stdafx.h"
#include "instrument.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "colors.h"

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
void  CInstrument::OnReceive(WPARAM WParam, LPARAM LParam)
{
//TRACE("OnReceive::m_dCurrent_State = %d\n",m_dCurrent_State);
	
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files

	int i = 0;								//dgp 26 Oct 2006

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		m_dInvalidTimeCount= 0;
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"17270 GRAND Start new day\r\n");
	}

	if (WParam == ECHO_RECORD)
	{
		m_IsMiniGrand = 1;
	}
	else if (WParam == ECHO_2_REC)
	{
		m_IsMiniGrand = 0;
	}
	else if (WParam == ECHO_5_REC)
	{
		m_IsMiniGrand = 1;
	}
	else if (WParam == ECHO_4_REC)
	{
		m_IsMiniGrand = 0;
		WParam = ECHO_RECORD;
	}
	else if (WParam == ECHO_3_REC)
	{
		m_IsMiniGrand = 1;
	}
	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if ((m_bGoodRegister == false) || (m_bBlockWrite == true))
		m_dCurrent_State = 9900;

	if (
		((WParam == IDC_BADYEAR_VALUE)||(WParam == IDC_TIMEOOT_VALUE))&&
		m_bGoodRegister &&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_bDoingAutoTimeSet = true;
		m_dTickToInquire = m_dInquireInt;
	}

	//if checksum result in LParam then was it bad
	if ((WParam != IDC_BBM_VALUE) && (LParam == CHECKSUM_BAD)) 
	{
		m_bChecksumFail = true;
		return;
	}

	if (WParam == IDC_BBM_VALUE)
		m_iCurrentBBM = LParam;

	//count the time since the last send of any kind
	//      the time since the last inquire2
	//      the time since the last dumpstat (iff != 0)
	if (WParam == TIMER_TICK)
	{
		//bump time increments to next inquire
		m_dTickToInquire++;
		//bump time increments to next status
		if (m_dStatusInt)m_dTickToStatus++;
		//bump time from last command
		m_dTick_Count++;
		if ((m_dRetransmit > m_dCommFail) &&
			(m_dCurrent_State != 9010) &&
			(m_dCurrent_State != 9500))
		{		
			if (m_dCurrent_State == 1210)
			{
				m_dRetransmit = 0;
				m_dTick_Count = 0;
				m_bChecksumFail = false;

				//set next state to "send analyze command"
				m_dCurrent_State = 1300;
			}
			else
			{
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dTimeOutCount++;
				m_dCurrent_State = 9000; //hard fail
			}
		}
	}

	if (
		(WParam == FORCEDATETIME)&&
		(m_bGoodRegister)&&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_dSave_State = m_dCurrent_State;
		switch (m_IsMiniGrand)
		{
		case 0:
			m_dCurrent_State = 8500;
			break;
		case 1:
			m_dCurrent_State = 8000;
			break;
		case -1:
		default:
			m_dCurrent_State = m_dSave_State;
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
	//start by sending echo,echo,echo
	case 1000:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		if (WParam == TIMER_TICK)
		{
			m_pButton->SetErrorIcon_Flag();
			m_pButton->put_FaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT);
			if (m_pPropertySheet)
	 			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT,true);
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"ECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dCurrent_State = 1010;
		}
		break;

	//wait for response 
	case 1010:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//something came back but didn't understand anything that came back
		if (WParam == ECHO_2_REC)
		{
			m_dCurrent_State = 1060;
			m_dTimeOutCount = 0;
		}

		//got a "??" back (mini-grand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got the expected "TIME = <space>" record back
		//this must be a MiniGRAND
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		//got a "G GAIN" back from a GIII, go wait for a short while
		if (WParam == ECHO_6_REC)
		{
			m_dRetransmit = 0;
			m_dCurrent_State = 1015;
			m_dTimeOutCount = 0;
			m_dTick_Count = 0;
		}

		//got a "MG" response							//DGP
		if (WParam == ECHO_5_REC)						//DGP			
		{
			m_dRetransmit = 0;							//DGP
			m_bInHardFail = false;						//DGP
			m_dCurrent_State = 1100;//good response		//DGP
			m_dTimeOutCount = 0;						//DGP
		}												//DGP

		//if we didn't get anything back go to transmit second ECHO
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//second transmit
		}
		break;

	case 1015:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (WParam == TIMER_TICK)
		{
			m_dTick_Count++ ;
			if (m_dTick_Count > 5)
				m_dCurrent_State = 1000;
		}
		break;

	case 1020:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send second echo command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"ECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 1025;
		}
		break;

	//wait for response 
	case 1025:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//something came back but didn't understand anything that came back
		if (WParam == ECHO_2_REC)
		{
			m_dCurrent_State = 1060;
			m_dTimeOutCount = 0;
		}

		//got a "??" back (mini-grand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		//got a "MG" response							//DGP
		if (WParam == ECHO_5_REC)						//DGP			
		{												//DGP
			m_dRetransmit = 0;							//DGP
			m_bInHardFail = false;						//DGP
			m_dCurrent_State = 1100;//good response		//DGP
			m_dTimeOutCount = 0;						//DGP
		}												//DGP

		//if we didn't get anything back go to transmit second ECHO
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//third transmit
		}
		break;

	case 1030:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send second echo command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"ECHO\r",m_iNode);
			UpdateSummary("ECHO");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ECHO\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 1040;
		}
		break;

	case 1040:
		//something came back but didn't understand anything that came back
		if (WParam == ECHO_2_REC)
		{
			m_dCurrent_State = 1060;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		if (WParam == ECHO_5_REC)
		{
			m_dRetransmit = 0;							//DGP
			m_bInHardFail = false;						//DGP
			m_dCurrent_State = 1100;//good response		//DGP
			m_dTimeOutCount = 0;						//DGP
		}

		//got a "??" back (mini-grand)
		if (WParam == ECHO_3_REC)
		{
			m_dCurrent_State = 1070;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//got the expected "TIME =" record back
		if (WParam == ECHO_RECORD)
		{
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			break;
		}

		//if we didn't get anything back go directly to hard fail
		if (m_dTick_Count > m_dTimeOut)
		{
//			if (m_dBreakSentCount >= m_dCommFail)
			if (m_dRetransmit >= m_dCommFail)
			{
				m_dRetransmit = 0;
				m_dCurrent_State = 9500; //long hard fail
				m_dTick_Count = 0;
				m_dTimeOutCount++;
			}
			else
			{
				m_dCurrent_State = 9000; //hard fail
				m_dTimeOutCount++;
			}
		}
		break;

	//didn't understand what came back so try to reset a grand3
	case 1060:
		//wait here if paused
		if (m_bPause) 
		{
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//send the string which should bring a grand3 back to monitor mode
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		UpdateSummary("END");
		m_dCurrent_State = 1061;
		break;

	case 1061:
		COM_Transmit(ISZSTRING,"USER\r",m_iNode);
		UpdateSummary("USER");
		m_dCurrent_State = 1062;
		break;

	case 1062:
		COM_Transmit(ISZSTRING,"YES\r",m_iNode);
		UpdateSummary("YES");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "END\r\nUSER\r\nYES\r\n");
		m_dTick_Count = 0;
		m_dCurrent_State = 1000;//restart
		break;

	//got a ?? so we are working on a mini and can get into monitor with a "90"
	case 1070:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
//			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		COM_Transmit(ISZSTRING,"90\r",m_iNode);
		UpdateSummary("90");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "90\r\n");
		m_dTick_Count = 0;
		m_dCurrent_State = 1071;//restart
		break;

	case 1071:
		if (m_bPause)
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (m_dTick_Count >= 3)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1000;
		}
		break;

	case 1111:
		if (m_bPause)
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		if (m_dTick_Count >= 2)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
		}
		break;
	
	//got past the echo,echo,response sequence, now need to send a dumplast
	case 1100:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpLast = true;				// DGP 16 Oct 2006 changed true to mean true

		//send the command
		COM_Transmit(ISZSTRING,"DUMPLAST\r",m_iNode);
		UpdateSummary("DUMPLAST");
		if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPLAST\r\n");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1110;
		break;

	//wait here for a response to the dumplast
	case 1110:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			WriteToFileEx(292,TO_PFM, TYPE_COMP, NULL,"17340 GRAND Timeout on receive DUMPLAST response\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL,"GRAND Timeout on receive DUMPLAST response\r\n");
			m_dTimeOutCount++;
		} 
		//beak out and wait for correct response
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
		{
			//
			// Jim Halberg and Peggy Moore 8/22/2002
			//
			if (WParam == ECHO_3_REC) // got "??" ?
			{
				COM_Transmit(ISZSTRING,"90\r",m_iNode);
				UpdateSummary("90");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "90\r\n");
				m_dCurrent_State = 1111;
			}
			//
			// end JH and PM changes
			//
			break;
		}
		//got it 
		m_dTimeOutCount = 0;
		//do we need to save these records -- the only way we need to is if
		//we sent a dumpok and then didn't get a dumpok response back and 
		//consequently are resetting this instrument
		//temp = m_dAcquireRecordCount;
		if (
			//we are not in initial startup and
			(m_dPrevBIDTime) &&
			//we did get some records in the dumplast and
			(m_dAcquireRecordCount) &&
			//those records date time are after the latest recorded
			(m_dPrevBIDTime < AcquireRec[0].JulianSeconds)
		   )
		{
			for (i = 0;i<m_dAcquireRecordCount;i++)
			{
				WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"17370 Acquire Record from DUMPLAST written in BID file (15)\r\n");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record from DUMPLAST written in BID file (15)\r\n");
				//issue warning if out of sequence
				if (m_dPrevBIDTime > AcquireRec[i].JulianSeconds)
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"17380 Acquire Record Out Of Order (15)\r\n");
				//update the "most recent" acquire record time
				m_dPrevBIDTime = AcquireRec[i].JulianSeconds;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				POSITION pos = m_ExclusionList.GetHeadPosition();
				if (pos)
				{
					for (int i=0;i < m_ExclusionList.GetCount();i++)
					{
						EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
						if (m_dPrevBIDTime > sExclusionRec.uiStartTime && 
							m_dPrevBIDTime < sExclusionRec.uiEndTime)
						{
							bAccept = false;
						}
					}
				}
#endif
				if (bAccept)
				{
					//write this record in the bid file
					if (WriteToBIDFile(
						&(AcquireTime[i]),
						&(AcquireRec[i]),
						AcquireCS[i],
						AcquireAUTH[i],
						AcquireAUTHIgnore[i]) == false)
					{
						UpdateSummary("Could not open BID file on DUMPLAST");
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevBIDTime);
					UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					WriteToFile(TO_PFM, TYPE_INST, &(AcquireTime[i]),"17390 Acquire Record from DUMPLAST in an EXCLUSION time (15)\r\n");
					if (m_bDoDump)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record from DUMPLAST in an EXCLUSION time (15)\r\n");
				}
#endif
			}
		}
////		else
////		{
////			//save the time of the latest received  
////			//acquire record for authentication
////			if (temp)
////				m_pPropertySheet->m_dLastAcquireTime = m_pPropertySheet->AcquireRec[temp-1].JulianSeconds;
////		}
		if (m_dAcquireRecordCount) 
			m_dPreviousBIDTime = AcquireRec[m_dAcquireRecordCount-1].JulianSeconds;

		m_dAcquireRecordCount = 0;

		//again this is WRONG! the state machine will decide			//dgp 26 Oct 2006
		//don't save any more acquire records -- until next dumpbbm
		//m_bInDumpLast = true;											//dgp 26 Oct 2006
		m_bInDumpLast = false;											//dgp 26 Oct 2006

		//so... we are no longer in dump last sequence
////		m_bInDumpLast = false;
		//prepare for the dumpstat command
		m_dRetransmit = 0;
		//go to that state
		m_dCurrent_State = 1200;
		//fall through to do the next transmit iff end dump received

	//transmit the dumpstat command
	case 1200:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send it
		COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
		UpdateSummary("DUMPSTAT");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPSTAT\r\n");
		//count the number of times we've sent this command
		m_dRetransmit++;
		//reset the tick count so that we can time out
		m_dTick_Count = 0;
		//set the next state to wait for a dumpstat response
		m_dCurrent_State = 1210;
		//start with no checksum failures
		m_bChecksumFail = false;
		break;

	//wait for a good response for the dumpstat command
	case 1210:

		//timeout if we've waited here too long
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			m_dTimeOutCount++;
			break;
		}

		//wait here if we don't have a end dump record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;
		
		//got a good end dump record
		m_dTimeOutCount = 0;
		if (m_bChecksumFail)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			break;
		}

		//got a good response 
		//so prepare for the next state
		m_dRetransmit = 0;
		m_dTick_Count = 0;
		m_bChecksumFail = false;

		//set next state to "send analyze command"
		m_dCurrent_State = 1300;
		//fall through to immediately transmit the command

	//send the analyze command
	case 1300:
		//wait here if pausing
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE );
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		UpdateSummary("ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 1310;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		break;

	//wait for a response to the analyze command
	case 1310:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1300;
			m_dAt = 0;
			m_dTimeOutCount++;
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(293,TO_PFM, TYPE_COMP, NULL, "17420 GRAND Timeout on receive ANALYZE response\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17430 GRAND Timeout on receive ANALYZE response (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on receive ANALYZE response\r\n");
		}
		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			///////////////////////////////////////////////////////////////
			//
			// Change by Jim Halbig and Peggy Moore 8/22/2002.
			//
			// Kick the miniGRAND out of setup mode and back into
			// monitor mode.  Then go transmit ECHO...
			//
			m_dCurrent_State = 1070; //1000;
			//
			//////////////////////////////////////////////////////////////
			m_dTimeOutCount = 0;
		}

		// received someting we can't understand
		// so assume it is a grand3 and restart
		if (WParam == ECHO_2_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 2000;
		if (m_bDoAnalyzeAfterDT0)
		{
			m_bDoAnalyzeAfterDT0 = false;
			m_dTickToInquire = m_dInquireInt;
////			break;
		}
////		else
////			m_dCurrent_State = 1400;
		//else fall through to next state
/*
	case 1400:
		//wait here if pausing
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->SetFaceColor(COLOR_PAUSE,true);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send the command
		COM_Transmit(ISZSTRING,"INQUIRE2\r",m_iNode);
		if (m_pPropertySheet)
			m_pPropertySheet->UpdateSummary("INQUIRE2");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "17440 INQUIRE2\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 1410;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		break;

	case 1410:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1400;
			m_pPropertySheet->m_dAt = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < g_dMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "17450 GRAND Timeout on receive INQUIRE2 response\r\n");
			if (m_dTimeOutCount == g_dMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "17460 GRAND Timeout on receive INQUIRE2 response (msg filter on)\r\n");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "17450 GRAND Timeout on receive INQUIRE2 response\r\n");
		}
		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_dTimeOutCount = 0;
		}

		// received someting we can't understand
		// so assume it is a grand3 and restart
		if (WParam == ECHO_2_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != MONITOR_ID2_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_dCurrent_State = 2000;
		m_bChecksumFail = false;
		//fall through to next state
*/
	//state 2000 is the "normal" resting state
	case 2000:

		//we can't have an initial initialization time out any longer
		m_bInitInitTimeOut = false;
		m_dHardFailCount = 0;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		//if we are paused then don't let state change, just
		//wait for next message to arrive by exiting
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//if we are driving to zero (power fail or copy data) then 
		//we need to empty the immediate buffer on the instrument
		//by sending an analyze command
		if (m_bDoAnalyzeAfterDT0)
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
			UpdateSummary("ANALYZE");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");

			m_dRetransmit = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 1310;
			break;
		}

		//if enough time has passed that we need to do
		//another inquire2 then do it
		if (m_dTickToInquire >= m_dInquireInt)  
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"INQUIRE2\r",m_iNode);
			UpdateSummary("INQUIRE2");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "INQUIRE2\r\n");

			m_dRetransmit++;
			m_dCurrent_State = 2100; 
			m_dTick_Count = 0;
			break;
			
		} 

		//if StatusInterval is greater than 0 (e.g. we do want to do dumpstats)
		//and enough time has passed then do it.
		if ((((m_dStatusInt > 0)&&(m_dTickToStatus >= m_dStatusInt)))||m_bDoOneDumpStat)	//SCR00074
		{
			m_bDoOneDumpStat = false;							//SCR00074
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"DUMPSTAT\r",m_iNode);
			UpdateSummary("DUMPSTAT");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPSTAT\r\n");
			m_dRetransmit++;
			m_dCurrent_State = 2300; 
			m_dTick_Count = 0;
			break;
		}

		//if we aren't going to do an inquire2 or a dumpstat then are we 
		//in the process of doing a dumpbbm--if so then continue with another
		//if the bytes in bbm has been driven to 0 then doingbbm will be false
		if (m_bdoingBBM) 
		{
			CString cCommand;
			if (m_bDoBinaryBBM)
				cCommand = "DUMPBBBM\r";
			else
				cCommand = "DUMPBBM\r";
			m_pButton->put_FaceColor(COLOR_WAIT);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,(char*)LPCSTR(cCommand),m_iNode);
			UpdateSummary(m_bDoBinaryBBM?"DUMPBBBM":"DUMPBBM");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, m_bDoBinaryBBM?"DUMPBBBM\r\n":"DUMPBBM\r\n");
			m_dAcquireRecordCount = 0;
			m_bInDumpLast = false;				//DGP 26 Oct 2006 Don't really need this here
			m_dRetransmit++;
			m_dCurrent_State = 2200; 
			m_dTick_Count = 0;
			break;//redundant
		} 
		break;

	case 2100:
		//inquire2 has just been sent 
		//normally two messages will be received in response
		//the first is the bbm value and the second is the id2 record
		//in the case of a mini-grand a "??" could also be receive iff
		//the grand has been taken out of monitor mode
		//all other messages are ignored and the state will eventually 
		//time out

		//have we timed out yet--we can timeout on the message we are waiting for
		//so DON'T break just incase
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(294,TO_PFM, TYPE_COMP, NULL, "17520 GRAND Timeout on INQUIRE2 command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17530 GRAND Timeout on INQUIRE2 command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on INQUIRE2 command\r\n");
			m_dAt = 0;
		}

		//adjust the instruments clock if needed
		if (
			((WParam == IDC_BADYEAR_VALUE) && m_bAllowAutoTime) ||
			((WParam == IDC_TIMEOOT_VALUE) && m_bAutoTimeDaily)
		   )
		{
			m_bDoingAutoTimeSet = true;
			m_dTickToInquire = m_dInquireInt;
			m_dSave_State = 2000;
			m_dCurrent_State = (m_IsMiniGrand)?8000:8500;
			break;
		}

/*
		//this is the processing of the "automatic daily time check"
		//it will do only on the first inquire2 response after the
		//time transition and if there is an error on that response
		if (m_bAutoTimeDaily)
		{
			//what is the tod 
			COleDateTime cNow=COleDateTime::GetCurrentTime();
			//what is the total seconds into the day
			unsigned int uiSecInDay = cNow.GetHour() * 3600 + cNow.GetMinute() * 60 + cNow.GetSecond();
			//if we have an inquire response prior to the required time
			if (uiSecInDay < m_uiSecInDayAutoTime) m_bAutoTimeDailyTrigger = true;
			//else it is > or = and we have a time out of tolerance
			else
			{
				//this if makes sure we only do the first one after the transition
				if ((WParam == IDC_TIMEOOT_VALUE) && m_bAutoTimeDailyTrigger)
				{
					m_bDoingAutoTimeSet = true;
					m_dTickToInquire = m_dInquireInt;
					m_dSave_State = 2000;
					m_dCurrent_State = (m_IsMiniGrand)?8000:8500;
					m_bAutoTimeDailyTrigger = false;
					break;
				}
				m_bAutoTimeDailyTrigger = false;
			}
		}
*/
		//got the expected bbm value message -- we should receive the 
		//MONITOR_ID2_RECORD message very shortly
		if (WParam == IDC_BBM_VALUE)
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if ((LParam > (m_bdoingBBM?0:m_dMaxBBM)) ||
				((LParam > 0) && m_bDriveToZero))
			{
				m_bdoingBBM=true;
				PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
			}
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) && (!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_dTimeOutCount = 0;
		}

		//we got a "??" from a mini-grand so try to reset it
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//received the expected monitor_id2 record so go back to "rest" state
		if ((WParam == MONITOR_ID2_RECORD)||(WParam == MONITOR_ID3_RECORD))
		{
			m_dRetransmit = 0;
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
		}

		break;

	case 2200:
		//we just sent a DUMPBBM command
		//time out?  we may time out on the specific message we need so
		//don't break--instead continue to see if we got it
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dCurrent_State = 2000;
			m_dAt = 0;
			m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(295,TO_PFM, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"17540 GRAND Timeout on DUMPBBBM command\r\n":"17550 GRAND Timeout on DUMPBBM command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, 
//				m_bDoBinaryBBM?"17560 GRAND Timeout on DUMPBBBM command (msg filter on)\r\n":"17570 GRAND Timeout on DUMPBBM command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, 
				m_bDoBinaryBBM?"17540 GRAND Timeout on DUMPBBBM command\r\n":"GRAND Timeout on DUMPBBM command\r\n");
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dAcquireRecordCount = 0;
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1070;

			//caution here -- we'll allow the next if to 
			//break us out
		}

		//wait for end dumpbbm record
		if ((WParam != END_DUMP_RECORD) && (WParam != END_DUMP2_RECORD))
			break;

		//got an end dump record so we can continue with the dumpbbm sequence
		m_dTimeOutCount = 0;
		m_dCurrent_State = 2210;
		m_bInDumpLast = false;					// DGP 16 Oct 2006
		//fall through iff end_dump_record received
		//this allows "quick" looping on the expected dumpbbm/

	case 2210:
		//just received a end dump record and possibly a bunch of other records
		//one or more of which may have a bad checksum
		//if we are paused in the middle of a dumpbbm sequence this is 
		//NOT a good state to wait in.   

		//if the dumpbbm terminated in the "new" end of dump then 
		//we MUST have same number of acquire records that the 
		//instrument thinks it sent
		if ((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_dAcquireRecordCount))
		{
			m_dCurrent_State = 2000;
			m_bChecksumFail = false;
			m_dAcquireRecordCount = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			WriteToFile(TO_PFM, TYPE_INST, NULL, "17580 INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL, "INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\r\n");
			m_dRetransmit = 0;
			break;
		}

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;

			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount <= 5)
			{
				//go back to get another dumpbbm
				m_dCurrent_State = 2000;
				//reset the checksum failure flag
				m_bChecksumFail = false;
				//we are going to ask for a new dumpbbm so 
				//get rid of all the existing acquire records
				m_dAcquireRecordCount = 0;
				//indumplast actually means "not" in dumpbbm //changed DGP 16 Oct2006
				//the next dumpbbm will set this to false
				m_bInDumpLast = false;					// DGP 16 Oct 2006
				//jump out of state machine
				break;
			}
			else 
			{
				m_bChecksumFail = false;
				FlagComm(1,true);
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "17590 HARD CHECKSUM ERROR\r\n");
			}
		}

		//if count was <= 5 we break out of case and don't do the 
		//following stuff
		//reset the checksum count in preparation for the next hard checksum error
		m_dChecksumCount = 0;

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"DUMPOK\r",m_iNode);
		UpdateSummary("DUMPOK");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK\r\n");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2220:
		//we just sent a DUMPOK message so we need the dumpok response record
		//we should receive two messages here m_bdumpOK and bbm value

		//got a bbm value so test the bbm value for empty and
		//set the flag to continue the dump and the got bbm value flag
		if ((WParam == IDC_BBM_VALUE))
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if (LParam > 0)
				m_bdoingBBM = true;
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero) && (!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
			m_bbbmVal = true;
		}

		//don't fail on waiting for dump ok response
		//assume that the instrument got at least one
		//of the dumpok messages -- need the check for
		//bbm value message first so we don't loose it
		if (m_dRetransmit > m_dCommFail)
		{
			WParam = DUMP_OK_RECORD;
			WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL, 
				"17610 GRAND Multiple Timeout on DUMPOK -- assuming ok\r\n");
			UpdateSummary("DUMPOK -- assuming ok");
////			if (m_bDoDump)
////				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK -- assuming ok\r\n");
		}

		//got a good dumpok but need to receive the bbm value before we 
		//continue.  so just set one of the flags
		if (WParam == DUMP_OK_RECORD) 
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_bdumpOK = true;
			int iTemp = m_dAcquireRecordCount;
			bool bDidWriteThemAll = true;
			unsigned int uLastBIDTime;
			uLastBIDTime = 0;
			m_dTimeOutCount = 0;
			for (i = 0;i<iTemp;i++)
			{
				//issue warning if out of sequence
				if (m_dPrevBIDTime && 
					(m_dPrevBIDTime > AcquireRec[i].JulianSeconds))
				{
					WriteToFile(TO_PFM, TYPE_INST, &(AcquireTime[i]),"17620 Acquire Record Out Of Order (15)\r\n");
				}
				//update the "most recent" acquire record time
				//this is local only NOT the PropertySheet value used in WriteToBIDFile
				m_dPrevBIDTime = AcquireRec[i].JulianSeconds;
				uLastBIDTime = m_dPrevBIDTime;

				//record we are attempting to write
				m_iLastRecordWritten = i;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				if (!m_ExclusionList.IsEmpty())
				{
					EXCLUSIONREC sExclusionRec = m_ExclusionList.GetHead();
					if (sExclusionRec.uiEndTime < m_dPrevBIDTime)
						m_ExclusionList.RemoveHead();
					POSITION pos = m_ExclusionList.GetHeadPosition();
					if (pos)
					{
						for (int i=0; i < m_ExclusionList.GetCount(); i++)
						{
							EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
							if (m_dPrevBIDTime > sExclusionRec.uiStartTime &&
								m_dPrevBIDTime < sExclusionRec.uiEndTime)
							{
								bAccept = false;
							}
						}
					}
				}
#endif
				if (bAccept)
				{
					//write this record in the bid file
					if (WriteToBIDFile(
							&AcquireTime[i],
							&AcquireRec[i],
							AcquireCS[i],
							AcquireAUTH[i], 
							AcquireAUTHIgnore[i])==false)
					{
						//we failed on this write		
						UpdateSummary("Could not open BID file");
						m_dCurrent_State = 2221;
						m_pButton->put_FaceColor(COLOR_CYAN);
						if (m_pPropertySheet)
							m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
						m_dTick_Count = 0;
						m_dTimeOutCount = 0;
						bDidWriteThemAll = false;
						break;
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevBIDTime);
					PostToWatch((LPSTR)LPCSTR(cMessage));
					UpdateSummary((LPSTR)LPCSTR(cMessage));
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"17630 Acquire Record in an EXCLUSION time (15)\r\n");
					if (m_bDoDump)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Acquire Record in an EXCLUSION time (15)\r\n");
				}
#endif
			}

			//if we did manage to write them and 
			//we did have some record 15s to write 
			//then reset the propertysheets info
			if (bDidWriteThemAll && (iTemp > 0))
			{
				m_dAcquireRecordCount = 0;
				m_dPreviousBIDTime = uLastBIDTime;
			}

			if (m_dCurrent_State == 2221) break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dAcquireRecordCount = 0;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1070;
			break;
		}

		//time out if we need to but check this message anyway by
		//not doing a break in the if above
		if ((m_dTick_Count > m_dTimeOut) && (WParam != DUMP_OK_RECORD))
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2230;//retransmit as necessary
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(296,TO_PFM, TYPE_COMP, NULL, "17640 GRAND Timeout on DUMPOK command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17650 GRAND Timeout on DUMPOK command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on DUMPOK command\r\n");
			break;
		}

		//iff we received both messages then go back to the "rest" state
		if (m_bdumpOK && m_bbbmVal) 
		{
			m_dCurrent_State = 2000;
			if (m_bdoingBBM)
				PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
		}

		break;

		//could not open bid file state handler
	case 2221:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;
////			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			for (int i = m_iLastRecordWritten; i < m_dAcquireRecordCount; i++)
			{
				//issue warning if out of sequence
				if (m_dPrevBIDTime && 
					(m_dPrevBIDTime > AcquireRec[i].JulianSeconds))
				{
					WriteToFile(TO_PFM, TYPE_INST, &AcquireTime[i],"17660 Acquire Record Out Of Order (15)\r\n");
				}
				//update the "most recent" acquire record time
				m_dPrevBIDTime = AcquireRec[i].JulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the bid file
				if (WriteToBIDFile(
						&AcquireTime[i],
						&AcquireRec[i],
						AcquireCS[i],
						AcquireAUTH[i], 
						AcquireAUTHIgnore[i])==false)
				{
					//still can't write records so break out of the for loop and try again
					bThisTimeFailed = true;
					UpdateSummary("Still can not open BID file");
					break;
				}
////				else
////				{
////					//ok so just continue
////					m_pPropertySheet->m_dPreviousBIDTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
////				}
			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				UpdateSummary("Still can not open BID file -- giving up");
				m_pButton->SetDiskIcon_Floppy();
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 9800;
			}
			//got a good write this time
			if (!bThisTimeFailed)
			{
				//get ready for the next block of records
				m_dPreviousBIDTime = m_dPrevBIDTime;
				//setup to add new records in the acquirerecord array
				m_dAcquireRecordCount = 0;
				//set the main button color back to something appropriate
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//set the property sheet's button color back
				if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
				//turn off flash
				m_pButton->put_Flash(VARIANT_FALSE);
				//set the disk icon back to the normal disk
				m_pButton->SetDiskIcon_Floppy();
				//turn off the disk icon
				m_pButton->ShowIcon_Disk(VARIANT_FALSE);
				//go back to the normal state
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 2000;
				//if we still have bbm then post a message to trigger 
				//getting more bbm immediately
				if (m_bdoingBBM)
					PostMessage(IDC_RECEIVE_RESULT,TIMER_TICK,CHECKSUM_GOOD);
			}
		}
		break;

	case 2230:

		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;
			//reset the checksum failure flag
			m_bChecksumFail = false;
			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount > 5)
			{
				FlagComm(1,true);
				WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "17670 HARD CHECKSUM ERROR ON DUMPBBMOK\r\n");
			}
		}

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"DUMPOK\r",m_iNode);
		UpdateSummary("DUMPOK Retransmit");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "DUMPOK Retransmit\r\n");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2300:
		//we just sent a dumpstat command

		//got the end dump record from the dumpstat command
		//so return to the "rest" state
		if ((WParam == END_DUMP_RECORD) || (WParam == END_DUMP2_RECORD))
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_dRetransmit = 0;
			m_dTickToStatus = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dAcquireRecordCount = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
			break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dAcquireRecordCount = 0;
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
			break;
		}

		//time out if we need to but check this record just in case
		//if timeout then return to the "rest" state -- next timer tick will trigger retransmit
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dAt = 0;
			m_bInDumpLast = false;					// DGP 16 Oct 2006
			m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;

//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(297,TO_PFM, TYPE_COMP, NULL, "17690 GRAND Timeout on DUMPSTAT command\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17700 GRAND Timeout on DUMPSTAT command (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on DUMPSTAT command\r\n");
			m_dCurrent_State = 2000;
		}
		break;


	case 8000:
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		UpdateSummary("ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 8001;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		//reset tick counter
		m_dTick_Count = 0;
		break;

	//wait for a response to the analyze command
	case 8001:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 8000;
			m_dAt = 0;
			m_dTimeOutCount++;
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(298,TO_PFM, TYPE_COMP, NULL, "17720 GRAND Timeout on receive ANALYZE response\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17730 GRAND Timeout on receive ANALYZE response (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on receive ANALYZE response\r\n");
		}
		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			///////////////////////////////////////////////////////////////
			//
			// Change by Jim Halbig and Peggy Moore 8/22/2002.
			//
			// Kick the miniGRAND out of setup mode and back into
			// monitor mode.  Then go transmit ECHO...
			//
			m_dCurrent_State = 1070; //1000;
			//
			//////////////////////////////////////////////////////////////
			m_dTimeOutCount = 0;
		}

		// received someting we can't understand
		// so assume it is a grand3 and restart
		if (WParam == ECHO_2_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 8009;
		//fall through to send end

	case 8009:
////		if (m_bPause) 
////		{
			KillTimer(TIMER_TICK);
			SetTimer(TIMER_TICK,1000,NULL);
			if (!m_bDoingAutoTimeSet)
				m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
			m_dTick_Count=0;
////		}

		m_dCurrent_State = 8010;
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		UpdateSummary("END");
		WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL,"17740 Forcing Instrument to Computer Time\r\n");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL,"END\r\n");
		break;

	case 8010:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8020;
			m_dTick_Count = 0;
			m_pToday = COleDateTime::GetCurrentTime();
			m_pToday += COleDateTimeSpan(0,0,0,1);//add one second for transmission delays
////			m_pPropertySheet->UpdateSummary((char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S")));
			COM_Transmit(ISZSTRING,(char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r")),m_iNode);
			if (m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP,NULL,(char*)LPCSTR(m_pToday.Format("2f %m %d %y %H %M %S\r\n")));
		}
		break;

	case 8020:
		if (WParam == TIMER_TICK)
		{
			COM_Transmit(ISZSTRING,"90\r",m_iNode);
			UpdateSummary("90");
			if (m_bDoDump)
				WriteToFile(TO_DMP,TYPE_DUMP,NULL,"90\r\n");
			m_dTick_Count = 0;
////			if (m_bWasPaused)
////			{
				if (!m_bDoingAutoTimeSet)
					m_dTickToInquire=0;
				if (m_dStatusInt) m_dTickToStatus=0;
				KillTimer(TIMER_TICK);
////			}
			if (!m_bPause || m_bDoingAutoTimeSet)
			{
				SetTimer(TIMER_TICK,m_dCycleInt,NULL);
				m_bIsPaused = false;
			}

			m_dCurrent_State = 8021;//m_dSave_State;
			//no matter what we don't need
			//to do another auto time set
			m_bDoingAutoTimeSet = false;

		}
		break;

	case 8021:
		if (m_dTick_Count >= 2)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = m_dSave_State;
		}
		break;


	case 8500:
		//send the command
		COM_Transmit(ISZSTRING,"ANALYZE\r",m_iNode);
		UpdateSummary("ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL, "ANALYZE\r\n");
		//count the number of times analyze has been sent
		m_dRetransmit++;
		//go to the wait for response state
		m_dCurrent_State = 8501;
		//reset check sum fail to see if we get one on the response
		m_bChecksumFail = false;
		//reset tick counter
		m_dTick_Count = 0;
		break;

	//wait for a response to the analyze command
	case 8501:
		//time out if need be
		if (m_dTick_Count >= m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 8500;
			m_dAt = 0;
			m_dTimeOutCount++;
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(299,TO_PFM, TYPE_COMP, NULL, "17790 GRAND Timeout on receive ANALYZE response\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17800 GRAND Timeout on receive ANALYZE response (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on receive ANALYZE response\r\n");
		}
		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
		if (WParam == ECHO_3_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			///////////////////////////////////////////////////////////////
			//
			// Change by Jim Halbig and Peggy Moore 8/22/2002.
			//
			// if we are a GRAND3 and we get "??", then we are
			// really a MiniGRAND.  So, send the "90" to get the MG out
			// setup mode and back into monitor mode, then go to initial
			// state 1000 to transmit the ECHO.  Then, the m_IsMiniGrand
			// variable will get reset properly, hopefully.
			//
			// Kick the miniGRAND out of setup mode and back into
			// monitor mode.  Then go transmit ECHO...
			//
			m_dCurrent_State = 1070; //1000;
			//
			//////////////////////////////////////////////////////////////
			m_dTimeOutCount = 0;
		}

		// received someting we can't understand
		// so assume it is a grand3 and restart
		if (WParam == ECHO_2_REC)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dCurrent_State = 1000;
			m_dTimeOutCount = 0;
		}

		//wait here
		if (WParam != ANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		m_dTimeOutCount = 0;
		//"normal" operations
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 8509;
		//fall through to send end

	case 8509:
		SetTimer(TIMER_TICK,200,NULL);
		if (!m_bDoingAutoTimeSet)
			m_dTickToInquire=0;
		if (m_dStatusInt)m_dTickToStatus=0;
		m_dTick_Count=0;
		m_dCurrent_State = 8510;
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"END\r",m_iNode);
		UpdateSummary("END");
		WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL,"17810 Forcing Instrument to Computer Time\r\n");
		if (m_bDoDump)
			WriteToFile(TO_DMP, TYPE_DUMP, NULL,"END\r\n");
		break;
		
	case 8510:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8520;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"SETUP\r",m_iNode);
			//UpdateSummary("END, SETUP");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL,"SETUP\r\n");
		}
		break;

	case 8520:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8530;
			m_dTick_Count = 0;
			m_pToday = COleDateTime::GetCurrentTime();
			m_pToday += COleDateTimeSpan( 0, 0, 0, 2 );
			COM_Transmit(ISZSTRING,(char*)LPCSTR(m_pToday.Format("%y\r%m\r%d\r%H\r%M\r%S\r")),m_iNode);
			UpdateSummary((char*)LPCSTR(m_pToday.Format("END, SETUP, %y.%m.%d %H:%M:%S")));
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL,(char*)LPCSTR(m_pToday.Format("%y.%m.%d %H:%M:%S\r\n")));
		}
		break;

	case 8530:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8540;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"END\r",m_iNode);
//			m_pPropertySheet->UpdateSummary("END");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL,"END\r\n");
		}
		break;

	case 8540:
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8550;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"USER\r",m_iNode);
//			m_pPropertySheet->UpdateSummary("USER");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL,"USER\r\n");
		}
		break;

	case 8550:
		if (WParam == TIMER_TICK)
		{
			m_dTick_Count = 0;
			if (m_dStatusInt)m_dTickToStatus=0;
			KillTimer(TIMER_TICK);
			if (!m_bPause || m_bDoingAutoTimeSet)
			{
				SetTimer(TIMER_TICK,m_dCycleInt,NULL);
				m_bIsPaused = false;
			}
			m_dCurrent_State = m_dSave_State;
			COM_Transmit(ISZSTRING,"YES\r",m_iNode);
			UpdateSummary("END, USER, YES");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_DUMP, NULL,"YES\r\n");
			//no matter what we don't need
			//to do another auto time set
			m_bDoingAutoTimeSet = false;

		}
		break;

	case 9000:
		//hard fail trying to communicate with instrument
		m_bInHardFail = true;
		m_bInDumpLast = false;					// DGP 16 Oct 2006
		m_dAcquireRecordCount = 0;
		//if initialization and hard fail then send the message
		if (m_bInitInitTimeOut)
		{
//			if (m_dTimeOutCount < g_dMaxRepetition)
			WriteToFileEx(300,TO_PFM, TYPE_COMP, NULL, "17880 GRAND Timeout on GRAND initialization\r\n");
//			if (m_dTimeOutCount == g_dMaxRepetition)
//				WriteToFile(TO_PFM, TYPE_COMP, NULL, "17890 GRAND Timeout on GRAND initialization (msg filter on)\r\n");
			if (m_bDoDump)
				WriteToFile(TO_DMP, TYPE_COMP, NULL, "GRAND Timeout on GRAND initialization\r\n");
			m_bInitInitTimeOut = false;
		}

		//start trying to reset things with a long break 
		//if the comm object won't allow a hard break then 
		//the comm object must catch the command and not do it
		//this command turns on break at the comm object
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak == 0)
			{
				m_pButton->put_FaceColor(COLOR_ERROR);
				if (m_pPropertySheet)
					m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
				FlagComm(1,true);			
				bool Local = false;
				bool HeldOffLocal = false;
				if (m_dRetransmit < (m_dCommFail-1))
				{
					COM_Transmit(ISZSTRING,LONGBREAKSET"\r",m_iNode);
					UpdateSummary("LONG BREAK SET");
					m_dBreakSentCount++;
////					if (m_bDoDump)
////						WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LONGBREAKSET\r\n");
				}
				else
				{
					if (COM_Transmit(ISZSTRING,LOCALLONGBREAK"\r",m_iNode)==-1)
					{
						UpdateSummary("LOCAL LONG BREAK held off");
						HeldOffLocal = true;
////						if (m_bDoDump)
////							WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK held off\r\n");
					}
					else
					{
						UpdateSummary("LOCAL LONG BREAK");
						m_dBreakSentCount++;
////						if (m_bDoDump)
////							WriteToFile(TO_DMP, TYPE_DUMP, NULL,"LOCAL LONG BREAK\r\n");
					}
					Local = true;
				}

//				if (m_dBreakSentCount < g_dMaxRepetition)
//				{
					if (HeldOffLocal)
						WriteToFileEx(301,TO_CEV | TO_PFM, TYPE_COMP, NULL, "17930 Local Break held off (start)\r\n");
					else
						WriteToFileEx(301,TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"17940 Local Break sent (start)\r\n":"17950 Break sent (start)\r\n");
//				}
//				if (m_dBreakSentCount == g_dMaxRepetition)
//				{
//					if (HeldOffLocal)
//						WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, "17960 Local Break held off (start) (msg filter on)\r\n");
//					else
//						WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"17970 Local Break sent (start) (msg filter on)\r\n":"17980 Break sent (start) (msg filter on)\r\n");
//				}

				if (m_bDoDump)
				{
					if (HeldOffLocal)
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, "Local LONG BREAK SET held off\r\n");
					else
						WriteToFile(TO_DMP, TYPE_DUMP, NULL, (Local)?"Local LONG BREAK SET\r\n":"18010 LONG BREAK SET\r\n");
				}

			}
			m_dLongBreak++;
		}

		//wait here for a couple of timer ticks to make 
		//sure a long break is minimum 3 seconds
		if ((m_dLongBreak * m_dCycleInt) > 3000) 
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 9010;
		}
		break;

	case 9010:
		//still in hard fail
		m_bInHardFail = true;

		//send the command to clear the long break
		//then wait in this state for a couple timer cycles
		//to give the instrument time to reset after the long break
		if (WParam == TIMER_TICK)
		{
			if (m_dLongBreak)
			{
				m_dTick_Count = 0;
				COM_Transmit(ISZSTRING,LONGBREAKCLR"\r",m_iNode,false);
				UpdateSummary("LONG BREAK CLEAR");
//				if (m_dBreakSentCount < g_dMaxRepetition)
				WriteToFileEx(302,TO_CEV | TO_PFM, TYPE_COMP, NULL, "18020 Break sent (end)\r\n");
//				if (m_dBreakSentCount == g_dMaxRepetition)
//					WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "18030 Break sent (end) (msg filter on)\r\n");
				if (m_bDoDump)
					WriteToFile(TO_DMP, TYPE_DUMP, NULL, "LONG BREAK CLEAR\r\n");
			}
			m_dLongBreak = 0;
			//this is how many timer ticks to wait until we continue with the reset
			bool bLBRExtend = (GetPrivateProfileInt(m_pName,"LBREXTEND",0,m_szIniFile) != 0);
			int iWait;
			if (bLBRExtend) 
				iWait = 60000;
			else
				iWait = 3000;
			if ((m_dTick_Count * m_dCycleInt) > iWait)//wait a minimum 3 or 60 seconds
			{
				m_dTick_Count = 0;
				m_dHardFailCount++;
				m_dCurrent_State = 1000;
			}
		}
		break;

	//long hard fail recovery
	case 9500:							//	900sec/15min
		if ((m_dTick_Count * m_dCycleInt) > m_dSlowReset)//wait 15 minutes before we retry
		{
			m_dTick_Count = 0;
			m_dHardFailCount = 0;
			//m_pButton->SetFlagChar(FLAG_CHAR,true);
			WriteToFileEx(310, TO_PFM, TYPE_COMP, NULL, "30008 Attempting recovery from communications failure.\r\n");
			m_dCurrent_State = 1000;
		}
		//Tickle Tracker every 10 counts
		if (m_dTick_Count % 10 == 0)
		{
			m_pButton->SetErrorIcon_Skull();
			m_pButton->put_FaceColor(COLOR_ERROR);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);

			if (m_dTick_Count <= 10) //first time through have FlagComm write msg 30001 to PFM
				FlagComm(1,true);
			// <<QA>> The redraw is done in the OnTimer function now.
			//else
			//	m_pButton->Redraw(TRUE);  //other times, just trigger network send
		}
		break;

	case 9800:
		m_dCurrent_State = 9810;
		//shut down this instrument
		PauseInstrument(true);	
		//but set it to attempt again in the future
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		m_pButton->put_FaceColor(COLOR_ERROR);
		if (m_pPropertySheet)
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "18050 WRITE FAILURE on BID file, SUPPORT TERMINATED\r\n");
		break;

	case 9810:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
		}
		else
		{
			m_bIsPaused = false;
			PauseInstrument(false);
			m_dTick_Count = INT_MAX-1;
			m_dTimeOutCount = 0;
			WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "18060 Attempt restart from WRITE FAILURE on BID file\r\n");
			m_dCurrent_State = 2221;
		}
		break;

	case 9900:
		if (m_bGoodRegister)
			m_dCurrent_State = 1000;
		else
		{
			m_dCurrent_State = 9900;
			WriteToFileEx(311,TO_PFM,TYPE_COMP,NULL,"18061 Configuration Failure [BOMB icon on]\r\n");
			m_pButton->SetErrorIcon_Bomb();
			m_pButton->ShowIcon_Error(VARIANT_TRUE);
			m_pButton->put_FaceColor(COLOR_ERROR);
			if (m_pPropertySheet)
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		}
		break;
	default:
		m_dCurrent_State = 1000;
	}
}