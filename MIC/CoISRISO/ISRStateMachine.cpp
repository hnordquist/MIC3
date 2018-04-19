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
//file ISRStateMachine.cpp

#include "stdafx.h"
#include "ISRInst.h"
#include "resource.h"
#include "colors.h"
#include "ISO_definitions.h"
#include "CSO_definitions.h"
#include "ISRSafePS.h"

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
void CISRInst::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//wparam is the type of message and lparam is the checksum good or bad
	//this is true for all messages EXCEPT the battery backed value message
	//in this one case lparam contains the number of bytes in bbm

	//throw away any message which has a bad checksum -- it can't be
	//trusted.  the tick timer will sum up a counter to force a 
	//retransmition if necessary.  The checksum routine will record
	//the message with an error to the proper files
	int i;
	CString temp;

	m_pNow = COleDateTime::GetCurrentTime();
	int dCurrentDay = m_pNow.GetDay();
	if (dCurrentDay != m_dPreviousDay)
	{
		m_dPreviousDay = dCurrentDay;
		m_dBreakSentCount = 0;
		m_dTimeOutCount = 0;
		m_dInvalidTimeCount=0;
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL,"21870 ISR Start new day\n\r");
	}

	//if we couldn't register this instrument 
	//or if disk is full so we are blocking write
	//then lock it up in state 9900 -- don't send anything
	if (!m_bGoodRegister || m_bBlockWrite)
		m_dCurrent_State = 9900;

	//if checksum result in LParam then was it bad
	if ((WParam != IDC_BBM_VALUE) && 
		(WParam != IDC_FILE_VALUE) && 
		(LParam == CHECKSUM_BAD)) 
	{
		m_bChecksumFail = true;
		return;
	}

	if (WParam == ISR_ISAMSR)
	{
		m_bIsAMSR = true;
		return;
	}

	if (
		((WParam == IDC_BADYEAR_VALUE)||(WParam == IDC_TIMEOOT_VALUE))&&
		m_bGoodRegister &&
		((m_dCurrent_State < 8000)||(m_dCurrent_State > 8999))
	   )
	{
		m_bDoingAutoTimeSet = true;
		m_dTickToInquire = m_dInquireInt;
	}

	if (WParam == IDC_BBM_VALUE)
		m_iCurrentBBM = LParam;	//dgp 18 Oct 2006

//	if (WParam == IDC_FILE_VALUE)
//		m_dCurrent_FILES = LParam;

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
			(m_dCurrent_State != 9500) &&
			(m_dCurrent_State != 1110) &&
			(m_dCurrent_State != 1100))
		{		
				m_dRetransmit = 0;
				m_dTick_Count =	0;
				m_dTimeOutCount++;
				m_dCurrent_State = 1000; //was 9000
		}
	}

	if (
		(WParam == FORCEDATETIME)&&
		(m_bGoodRegister)&&
		((m_dCurrent_State < 7000)||(m_dCurrent_State > 8999))
	   )
	{
		m_dSave_State = m_dCurrent_State;
		m_dCurrent_State = 8000;
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
	//start by sending ? to discover what mode we are in
	case 1000:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		if (WParam == TIMER_TICK)
		{
			m_pButton->SetErrorIcon_Flag();
			m_pButton->put_FaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(m_bInHardFail?COLOR_ERROR:COLOR_WAIT,true);
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"?\r",m_iNode);
			m_pPropertySheet->UpdateSummary("? - Show Current Mode Commanded");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21880 ? - Show Current Mode Commanded\n\r");
			m_dCurrent_State = 1010;
		}
		break;

	//wait for response 
	case 1010:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//got the expected "?0" record back
		//we are in monitor mode
		if (WParam == ISR_Q0_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1100;//good response
		}

		//got a "?1" back 
		//we are in PSR mode
		if (WParam == ISR_Q1_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1060;
		}

		//got a "?2" back
		//we are in JSR mode
		if (WParam == ISR_Q2_RECORD)
		{
			m_dTimeOutCount = 0;
			m_dCurrent_State = 1070;
		}

		//if we didn't get anything back 
		//in the alloted time so go and
		//transmit second ?
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dCurrent_State = 1030;//second transmit
		}

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1000;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		break;


	case 1030:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send second ? command
		if (WParam == TIMER_TICK)
		{
			m_dTickToInquire = m_dInquireInt;
			m_dTickToStatus  = 0;
			m_dTick_Count = 0;
			COM_Transmit(ISZSTRING,"?\r",m_iNode);
			m_pPropertySheet->UpdateSummary("? - Show Current Mode Commanded");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21890 ? - Show Current Mode Commanded\n\r");
			m_dRetransmit++;
			m_dCurrent_State = 1040;
		}
		break;

	case 1040:

		//got the expected "?0" record back
		//we are in monitor mode
		if (WParam == ISR_Q0_RECORD)
		{
			m_dRetransmit = 0;
			m_bInHardFail = false;
			m_dCurrent_State = 1100;//good response
			m_dTimeOutCount = 0;
		}

		//got a "?1" back 
		//we are in PSR mode
		if (WParam == ISR_Q1_RECORD)
		{
			m_dCurrent_State = 1060;
			m_dTimeOutCount = 0;
		}

		//got a "?2" back
		//we are in JSR mode
		if (WParam == ISR_Q2_RECORD)
		{
			m_dCurrent_State = 1070;
			m_dTimeOutCount = 0;
		}

		//if we didn't get anything back go directly to hard fail
		if (m_dTick_Count > m_dTimeOut)
		{
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

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1000;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		break;

	//we are in PSR mode so command mode back to monitor mode
	case 1060:
		//wait here if paused
		if (m_bPause) 
		{
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//send the string which should bring a grand3 back to monitor mode
		COM_Transmit(ISZSTRING,"*50C\r",m_iNode);
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21900 *50C - PSR to Monitor Mode Commanded\n\r");
		m_pPropertySheet->UpdateSummary("50 - Enter Monitor Mode Commanded");
		m_dTick_Count = 0;
		m_dCurrent_State = 1000;
		break;

	//we are in JSR mode so command mode back to monitor mode
	case 1070:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		COM_Transmit(ISZSTRING,"F\r",m_iNode);
		m_pPropertySheet->UpdateSummary("F - Enter PSR Mode Commanded");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21910 F - JSR to PSR Mode Commanded\n\r");
		m_dTick_Count = 0;
		m_dCurrent_State = 1060;//restart
		break;

	//got past the ?,?,response sequence, now need to send a dumplast
	case 1100:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//we are in a dumplast sequence
		m_bInDumpBBM = true;
		//send the command
		COM_Transmit(ISZSTRING,"6\r",m_iNode);
		m_pPropertySheet->UpdateSummary("6 - DUMPLAST");
		if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21920 6 - DUMPLAST\n\r");
		//incr the number of times command has been sent
		m_dRetransmit++;
		//start the tick count at 0 so we can detect time out
		m_dTick_Count = 0;
		//next state will wait for a good response
		m_dCurrent_State = 1110;
		break;

	//wait here for a response to the dumplast
	case 1110:
		//don't go into long break if from here if not responding
		if (m_dRetransmit > m_dCommFail )
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;

			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;

			//don't save any more acquire records -- until next dumpbbm
			m_bInDumpBBM = false;

			//prepare for the dumpstat command
			m_dRetransmit = 0;
			//go to that state
			m_dCurrent_State = 1200;//go to dump stat anyway
//			m_dCurrent_State = 1120;//send an Inquire2 sequence

			break;
		}

		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1100;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"21930 ISR Timeout on receive DUMPLAST response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"21940 ISR Timeout on receive DUMPLAST response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL,"21950 ISR Timeout on receive DUMPLAST response\n\r");

		} 

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1100;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		//beak out and wait for correct response
		if ((WParam != ISREND_DUMP_RECORD) && 
			(WParam != ISREND_DUMP2_RECORD) &&
			(WParam != ISREND_DUMP3_RECORD) 
			)
			break;

		//got the end_dump_record 
		m_dTimeOutCount = 0;
		//do we need to save these records -- the only way we need to is if
		//we sent a dumpok and then didn't get a dumpok response back and 
		//consequently are resetting this instrument
		if (
			//we are not in initial startup and
			(m_dPrevISRTime) &&
			//we did get some records in the dumplast and
			(m_pPropertySheet->m_dAcquireRecordCount) &&
			//those records date time are after the latest recorded
			(m_dPrevISRTime < m_pPropertySheet->ISRAcquireRec[0].JulianSeconds)
		   )
		{
			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			for (i = 0;i<temp;i++)
			{
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"21960 Acquire Record from DUMPLAST being written in ISR file (15)\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21960 Acquire Record from DUMPLAST being written in ISR file (15)\n\r");
				//issue warning if out of sequence
				if (m_dPrevISRTime > m_pPropertySheet->ISRAcquireRec[i].JulianSeconds)
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"21970 Acquire Record Out Of Order (15)\n\r");
				//update the "most recent" acquire record time
				m_dPrevISRTime = m_pPropertySheet->ISRAcquireRec[i].JulianSeconds;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				POSITION pos = m_ExclusionList.GetHeadPosition();
				if (pos)
				{
					for (int i=0;i < m_ExclusionList.GetCount();i++)
					{
						EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
						if (m_dPrevISRTime > sExclusionRec.uiStartTime && 
							m_dPrevISRTime < sExclusionRec.uiEndTime)
						{
							bAccept = false;
						}
					}
				}
#endif
				if (bAccept)
				{

					//write this record in the bid file
					if (m_pPropertySheet->WriteToISRFile(
						&(m_pPropertySheet->AcquireTime[i]),
						&m_pPropertySheet->ISRAcquireRec[i]) == false)
					{
						m_pPropertySheet->WriteToFile(TO_PFM | TO_CEV, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"21980 Acquire Record from DUMPLAST failed to write to ISR file (15)\n\r");
						if (m_bDoDump)
							m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21980 Acquire Record from DUMPLAST failed to write to ISR file(15)\n\r");
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevISRTime);
					m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"21990 Acquire Record from DUMPLAST in an EXCLUSION time (15)\n\r");
					if (m_bDoDump)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "21990 Acquire Record from DUMPLAST in an EXCLUSION time (15)\n\r");
				}
#endif
			}
		}
		m_pPropertySheet->m_dAcquireRecordCount = 0;
		m_pPropertySheet->m_dMessageRecordCount = 0;

		//don't save any more acquire records -- until next dumpbbm
		m_bInDumpBBM = false;

		//so... we are no longer in dump last sequence
//		m_bInDumpBBM = false;
		//prepare for the dumpstat command
		m_dRetransmit = 0;
		//go to that state
		m_dCurrent_State = 1200;
		//fall through to do the next transmit iff end dump received


	case 1120:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		m_dRetransmit++;
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		COM_Transmit(ISZSTRING,"4\r",m_iNode);
		m_pPropertySheet->UpdateSummary("4 - INQUIRE2");
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22000 4 - INQUIRE2\n\r");
			m_dRetransmit++;
		m_dCurrent_State = 1130; 
		m_dTick_Count = 0;
		break;

	case 1130:
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 1120;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"22010 ISR Timeout on receive INQUIRE2 response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL,"22020 ISR Timeout on receive INQUIRE2 response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL,"22030 ISR Timeout on receive INQUIRE2 response\n\r");
		} 

		if (WParam == ISRMONITOR_ID2_RECORD)
		{
			m_dRetransmit = 0;
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 1200;
			m_dTimeOutCount = 0;
		}

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1120;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		break;


	//transmit the dumpstat command
	case 1200:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send it
		COM_Transmit(ISZSTRING,"3\r",m_iNode);
		m_pPropertySheet->UpdateSummary("3 - DUMPSTAT");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22040 3 - DUMPSTAT\n\r");
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

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1200;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		//wait here if we don't have a end dump record
		if ((WParam != ISREND_DUMP_RECORD) && 
			(WParam != ISREND_DUMP2_RECORD) &&
			(WParam != ISREND_DUMP3_RECORD) 
			)
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
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}
		//send the command
		COM_Transmit(ISZSTRING,"5\r",m_iNode);
		m_pPropertySheet->UpdateSummary("5 - ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22050 5 - ANALYZE\n\r");
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
			m_pPropertySheet->m_dAt = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22060 ISR Timeout on receive ANALYZE response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22070 ISR Timeout on receive ANALYZE response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22080 ISR Timeout on receive ANALYZE response\n\r");
		}

				//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 1300;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		//wait here
		if (WParam != ISRANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		//"normal" operations
		m_dTimeOutCount = 0;
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		if (m_bDoAnalyzeAfterDT0)
		{
			m_bDoAnalyzeAfterDT0 = false;
			m_dTickToInquire = m_dInquireInt;
		}
		m_dCurrent_State = 2000;
		//fall through to next state

	//state 2000 is the "normal" resting state
	case 2000:

		//we can't have an initial initialization time out any longer
		m_bInitInitTimeOut = false;
		m_dHardFailCount = 0;
		m_dBreakSentCount = 0;
		//if we are paused then don't let state change, just
		//wait for next message to arrive by exiting
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			break;
		}

		//if we are driving to zero (power fail or copy data) then 
		//we need to empty the immediate buffer on the instrument
		//by sending an analyze command
		if (m_bDoAnalyzeAfterDT0)
		{
			//send this only once
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"5\r",m_iNode);
			m_pPropertySheet->UpdateSummary("5 - ANALYZE");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22090 5 - ANALYZE\n\r");

			m_dRetransmit = 0;
			m_dTick_Count = 0;
			m_bChecksumFail = false;
			m_dCurrent_State = 1310;
			break;
		}

		//if enough time has passed that we need to do
		//another inquire2 then do it
		if (m_dTickToInquire >= m_dInquireInt)  
		{
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"4\r",m_iNode);
			m_pPropertySheet->UpdateSummary("4 - INQUIRE2");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22100 4 - INQUIRE2\n\r");

			m_dRetransmit++;
			m_dCurrent_State = 2100; 
			m_dTick_Count = 0;
			break;
			
		}
		

		//if StatusInterval is greater than 0 (e.g. we do want to do dumpstats)
		//and enough time has passed then do it.
		if (((m_dStatusInt > 0)&&(m_dTickToStatus >= m_dStatusInt)) || m_bDoOneDumpStat)	//SCR00074
		{
			m_bDoOneDumpStat = false;														//SCR00074
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"3\r",m_iNode);
			m_pPropertySheet->UpdateSummary("3 - DUMPSTAT");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22110 3 - DUMPSTAT\n\r");
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
			m_pButton->put_FaceColor(COLOR_WAIT);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
			COM_Transmit(ISZSTRING,"8\r",m_iNode);
			m_pPropertySheet->UpdateSummary("8 - DUMPBBM");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22120 8 - DUMPBBM\n\r");
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			m_bInDumpBBM = true;
			m_dRetransmit++;
			m_dCurrent_State = 2200; 
			m_dTick_Count = 0;
//			break;//redundant
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
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22130 ISR Timeout on INQUIRE2 command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22140 ISR Timeout on INQUIRE2 command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22130 ISR Timeout on INQUIRE2 command\n\r");
			m_pPropertySheet->m_dAt = 0;
		}

		if (
			//(WParam == IDC_BADYEAR_VALUE) && m_bAllowAutoTime
			((WParam == IDC_BADYEAR_VALUE) && m_bAllowAutoTime) ||
			((WParam == IDC_TIMEOOT_VALUE) && m_bAutoTimeDaily)
           )
		{
			m_bDoingAutoTimeSet = true;
			m_dTickToInquire = m_dInquireInt;
			m_dSave_State = 2000;
			m_dCurrent_State = 8000;
			break;
		}

		//got the expected bbm value message -- we should receive the 
		//MONITOR_ID2_RECORD message very shortly
		if (WParam == IDC_BBM_VALUE)
		{
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if ((LParam > (m_bdoingBBM?0:m_dMaxBBM)) ||
				((LParam > 0) && m_bDriveToZero))
			{
				m_bdoingBBM=true;
				PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
			}
			else
			{
				m_bdoingBBM = false;
				if ((m_bDriveToZero)&&(!m_bDoAnalyzeAfterDT0))
				{
					m_bPause = true;
					m_bDriveToZero = false;
				}
			}
		}

		//received the expected monitor_id2 record so go back to "rest" state
		if (WParam == ISRMONITOR_ID2_RECORD)
		{
			m_dRetransmit = 0;
			m_dTickToInquire = 0;
			m_dTick_Count = 0;
			m_dCurrent_State = 2000;
			m_dTimeOutCount = 0;
		}

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 2000;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		break;

	case 2200:
		//we just sent a DUMPBBM command
		//time out?  we may time out on the specific message we need so
		//don't break--instead continue to see if we got it
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_bInDumpBBM = false;
			m_dCurrent_State = 2000;
			m_pPropertySheet->m_dAt = 0;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22160 ISR Timeout on DUMPBBM command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22170 ISR Timeout on DUMPBBM command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22160 ISR Timeout on DUMPBBM command\n\r");
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
//		if (WParam == ECHO_3_REC)
//		{
//			m_bInDumpBBM = false;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
//			m_dTick_Count = 0;
//			m_dRetransmit = 0;
//			m_dCurrent_State = 1070;
//			//caution here -- we'll allow the next if to 
//			//break us out
//		}
		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 2000;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		//wait for end dumpbbm record
		if ((WParam != ISREND_DUMP_RECORD) && 
			(WParam != ISREND_DUMP2_RECORD) &&
			(WParam != ISREND_DUMP3_RECORD) 
			)
			break;

		//got an end of dump record so we can continue with the dumpbbm sequence
		m_dLastCommand = WParam;
		m_dCurrent_State = 2210;
//		m_bInDumpBBM = false;
		m_dTimeOutCount = 0;
		//fall through iff end_dump_record received
		//this allows "quick" looping on the expected dumpbbm/

	case 2210:
		//just received a end dump record and possibly a bunch of other records
		//one or more of which may have a bad checksum
		//if we are paused in the middle of a dumpbbm sequence 
		//this is not a good state to wait in.  
		if (m_dLastCommand == ISREND_DUMP2_RECORD)
		{
			//if the dumpbbm terminated in the "new" end of dump then 
			//we MUST have same number of acquire records that the 
			//instrument thinks it sent
			m_dTimeOutCount = 0;
			if ((m_dNumberOfAcquires >=0) && (m_dNumberOfAcquires != m_pPropertySheet->m_dAcquireRecordCount))
			{
				m_dCurrent_State = 2000;
				m_bChecksumFail = false;
				m_pPropertySheet->m_dAcquireRecordCount = 0;
				m_pPropertySheet->m_dMessageRecordCount = 0;
				m_bInDumpBBM = false;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, NULL, "22180 INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22180 INSUFFICIENT NUMBER OF ACQUIRE RECORDS RECEIVED\n\r");
				m_dRetransmit = 0;
				break;
			}
		}
		if (
			(m_dLastCommand == ISREND_DUMP3_RECORD) 
			)
		{
			if ((m_dNumberOfMessages >=0) && 
				(m_dNumberOfMessages != m_pPropertySheet->m_dMessageRecordCount))
			{
				m_dCurrent_State = 2000;
				m_dTimeOutCount = 0;
				m_bChecksumFail = false;
				m_pPropertySheet->m_dMessageRecordCount = 0;
				m_pPropertySheet->m_dAcquireRecordCount = 0;
				m_bInDumpBBM = false;
				m_pPropertySheet->WriteToFile(TO_PFM,TYPE_INST,NULL,"22190 INSUFFICIENT NUMBER OF RECORDS RECEIVED\n\r"); 
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22190 INSUFFICIENT NUMBER OF RECORDS RECEIVED\n\r");
				m_dRetransmit = 0;
				break;
			}
		}
		//if we received ANY checksum errors during the dumpbbm then
		//we want to break out and ask for a retransmit via another dumpbbm
		//but if the number of checksum fails is at hard-coded value then
		//record the hard fail and continue with the next dump
		if (m_bChecksumFail) 
		{ 
			//count this failure
			m_dChecksumCount++;
			//go back to get another dumpbbm
			m_dCurrent_State = 2000;
			//reset the checksum failure flag
			m_bChecksumFail = false;
			//we are going to ask for a new dumpbbm so 
			//get rid of all the existing acquire records
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			//indumpbbm actually means in dumpbbm
			//the next dumpbbm will set this to true
			m_bInDumpBBM = false;

			//hard coded if more than 5 checksum failures 
			//then give up and move on
			if (m_dChecksumCount <= 5)
			{
				break;
			}
			else 
			{
				FlagComm(1,true);
				m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "22200 HARD CHECKSUM ERROR\n\r");
			}
		}

		//if count was <= 5 we break out of case and don't do the 
		//following stuff
		//reset the checksum count in preparation for the next hard checksum error
		m_dChecksumCount = 0;

		//either we got a good BBMDUMP or we had excessive checksum errors
		//in both cases we need to continue
		m_pButton->put_FaceColor(COLOR_WAIT);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"9\r",m_iNode);
		m_pPropertySheet->UpdateSummary("9 - DUMPOK");

		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22210 9 - DUMPOK\n\r");
		m_dRetransmit++;
		m_dCurrent_State = 2220;
		//set these to prime the next state
		m_bdumpOK = false;
		m_bbbmVal = false;
		break;

	case 2220:
		//we just sent a DUMPOK message so we need the dumpok response record
		//we should receive two messages here m_bdumpOK and bbm value

		//got a good dumpok but need to receive the bbm value before we 
		//continue.  so just set one of the flags
		if ((WParam == ISRDUMP_OK_RECORD) || (WParam == ISRDUMP_OK2_RECORD))
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_bdumpOK = true;
			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			bool bDidWriteThemAll = true;
			m_dTimeOutCount = 0;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			for (i = 0;i<temp;i++)
			{
				//issue warning if out of sequence
				if (m_dPrevISRTime && 
					(m_dPrevISRTime > m_pPropertySheet->ISRAcquireRec[i].JulianSeconds))
				{
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"22220 Acquire Record Out Of Order (15)\n\r");
				}
				//update the "most recent" acquire record time
				m_dPrevISRTime = m_pPropertySheet->ISRAcquireRec[i].JulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;

				bool bAccept = true;
#ifdef ALLOWTRIGGERHOLDOFF
				if (!m_ExclusionList.IsEmpty())
				{
					EXCLUSIONREC sExclusionRec = m_ExclusionList.GetHead();
					if (sExclusionRec.uiEndTime < m_dPrevISRTime)
						m_ExclusionList.RemoveHead();
					POSITION pos = m_ExclusionList.GetHeadPosition();
					if (pos)
					{
						for (int i=0; i < m_ExclusionList.GetCount(); i++)
						{
							EXCLUSIONREC sExclusionRec = m_ExclusionList.GetNext(pos);
							if (m_dPrevISRTime > sExclusionRec.uiStartTime &&
								m_dPrevISRTime < sExclusionRec.uiEndTime)
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
					if (m_pPropertySheet->WriteToISRFile(
						&(m_pPropertySheet->AcquireTime[i]),
						&m_pPropertySheet->ISRAcquireRec[i]) == false)
					{
						//we failed on this write		
						m_pPropertySheet->UpdateSummary("Could not open ISR file");
						m_dCurrent_State = 2221;
						m_pButton->put_FaceColor(COLOR_CYAN);
						m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
						m_dTick_Count = 0;
						m_dTimeOutCount = 0;
						bDidWriteThemAll = false;
						//break out of the for loop
						break;
					}
					else
					{
						//we succeeded on this write
					//	m_pPropertySheet->m_dPreviousISRTime = m_pPropertySheet->AcquireRec[i].JulianSeconds;
					}
				}
#ifdef ALLOWTRIGGERHOLDOFF
				else
				{
					CString cMessage;
					cMessage.Format("Excluded Record: %d",m_dPrevISRTime);
					m_pPropertySheet->UpdateSummary((LPSTR)LPCSTR(cMessage));
					PostToWatch((LPSTR)LPCSTR(cMessage));
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"22230 Acquire Record in an EXCLUSION time (15)\n\r");
					if (m_bDoDump)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22230 Acquire Record in an EXCLUSION time (15)\n\r");
				}
#endif
			}
			if (bDidWriteThemAll)
				m_pPropertySheet->m_dAcquireRecordCount = 0;
			//break out of the case
			if (m_dCurrent_State == 2221)
				break;
		}

		// received a ?? == somehow a mini-grand 
		// went back into user mode.  so we need
		// to restart
//		if (WParam == ECHO_3_REC)
//		{
//			m_dTick_Count = 0;
//			m_dRetransmit = 0;
//			m_bInDumpBBM = false;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
//			m_dCurrent_State = 1070;
//			break;
//		}

		//time out if we need to but check this message anyway by
		//not doing a break in the if
		if ((m_dTick_Count > m_dTimeOut) && 
			(WParam != ISRDUMP_OK_RECORD) && 
			(WParam != ISRDUMP_OK2_RECORD) &&
			(WParam != IDC_FILE_VALUE) &&
			(WParam != IDC_BBM_VALUE)
			)
		{
			m_dTick_Count = 0;
			m_dCurrent_State = 2210;//retransmit as necessary
//			m_pPropertySheet->m_dAt = 0;
//			m_bInDumpBBM = false;
//			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22240 ISR Timeout on DUMPOK command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22250 ISR Timeout on DUMPOK command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22240 ISR Timeout on DUMPOK command\n\r");
			break;
		}

		//can only get a 17 if there are no other files
		//to be xfered from the pc104
		if (WParam == ISRDUMP_OK_RECORD)
			m_dNumberOfFiles = 0;

		//then we must be in catch-up mode
		if (WParam == IDC_FILE_VALUE)
			m_dNumberOfFiles = LParam;

		//got a bbm value so test the bbm value for empty and
		//set the flag to continue the dump and the got bbm value flag
		if (WParam == IDC_BBM_VALUE)
		{
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			if ((LParam > 0) || (m_dNumberOfFiles > 0))
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

		//iff we received both messages then go back to the "rest" state
		if (m_bdumpOK && m_bbbmVal) 
		{
			if ((m_dNumberOfFiles == 0) && (m_iCurrentBBM == 0))	//dgp 18 Oct 2006
				m_bdoingBBM = false;
			m_dCurrent_State = 2000;
			PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
		}
		break;

	case 2221:
		if (m_dTick_Count > m_dTimeOut)
		{
			bool bThisTimeFailed = false;
			m_dTimeOutCount++;
			m_dTick_Count = 0;
//			int temp = m_pPropertySheet->m_dAcquireRecordCount;
			for (int i = m_iLastRecordWritten; i < m_pPropertySheet->m_dAcquireRecordCount; i++)
			{
				//issue warning if out of sequence
				if (m_dPrevISRTime && 
					(m_dPrevISRTime > m_pPropertySheet->ISRAcquireRec[i].JulianSeconds))
				{
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, &(m_pPropertySheet->AcquireTime[i]),"22260 Acquire Record Out Of Order (15)\n\r");
				}
				//update the "most recent" acquire record time
				//m_dPrevISRTime = m_pPropertySheet->ISRAcquireRec[i].JulianSeconds;
				//record we are attempting to write
				m_iLastRecordWritten = i;
				//write this record in the isr file
				if (m_pPropertySheet->WriteToISRFile(
					&(m_pPropertySheet->AcquireTime[i]),
					&m_pPropertySheet->ISRAcquireRec[i]) == true)
				{
					//ok so just continue
					m_dPrevISRTime = m_pPropertySheet->ISRAcquireRec[i].JulianSeconds;
				}
				else
				{
					//still can't write records so break out of the for loop and try again
					bThisTimeFailed = true;
					m_pPropertySheet->UpdateSummary("Still can not open BID file");
					break;
				}
			}
			//if we failed and we've retried as many as possible then
			//bail out of trying and lock up the 
			if ((m_dTimeOutCount >= m_dCommFail) && bThisTimeFailed)
			{
				m_pPropertySheet->UpdateSummary("Still can not open ISR file -- giving up");
				m_pButton->SetDiskIcon_Floppy();//SetDiskChar('<',true);
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 9800;
			}
			//got a good write this time
			if (!bThisTimeFailed)
			{
				//setup to add new records in the acquirerecord array
				m_pPropertySheet->m_dAcquireRecordCount = 0;
				//set the main button color back to something appropriate
				m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
				//set the property sheet's button color back
				m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
				//turn off flash
				m_pButton->put_Flash(VARIANT_FALSE);
				//set the disk icon back to the normal disk
				m_pButton->SetDiskIcon_Floppy();//SetDiskChar('<',true);
				//turn off the disk icon
				m_pButton->ShowIcon_Disk(VARIANT_FALSE);//SetDisk(false,true);
				//go back to the normal state
				m_dTimeOutCount = 0;
				m_dTick_Count = 0;
				m_dCurrent_State = 2000;
				//if we still have bbm then post a message to trigger 
				//getting more bbm immediately
				if (m_bdoingBBM)
					PostMessage(m_uIDC_RECEIVE,TIMER_TICK,CHECKSUM_GOOD);
			}
		}
		break;

	case 2300:
		//we just sent a dumpstat command

		//got the end dump record from the dumpstat command
		//so return to the "rest" state
		if ((WParam == ISREND_DUMP_RECORD) || 
			(WParam == ISREND_DUMP2_RECORD)||
			(WParam == ISREND_DUMP3_RECORD)
			)
		{
			m_dTimeOutCount = 0;
			m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
			m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
			m_dRetransmit = 0;
			m_dTickToStatus = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			m_dCurrent_State = 2000;
			break;
		}

		//time out if we need to but check this record just in case
		//if timeout then return to the "rest" state -- next timer tick will trigger retransmit
		if (m_dTick_Count > m_dTimeOut)
		{
			m_dTick_Count = 0;
			m_pPropertySheet->m_dAt = 0;
			m_bInDumpBBM = false;
			m_pPropertySheet->m_dAcquireRecordCount = 0;
			m_pPropertySheet->m_dMessageRecordCount = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22270 ISR Timeout on DUMPSTAT command\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22280 ISR Timeout on DUMPSTAT command (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22270 ISR Timeout on DUMPSTAT command\n\r");
			m_dCurrent_State = 2000;
		}

		//if busy go into special wait state
		if (WParam == ISRBUSY_RECORD)
		{
			m_dSave_State = 2000;
			m_dCurrent_State = 7000;
			m_pButton->get_FaceColor(&m_crSave_Color);
		}

		break;

	case 7000: //start up of extended wait for instrument to not be busy
		//set timer to once per second
		SetTimer(TIMER_TICK,1000,NULL);
		m_dTick_Count = 0;
		m_dRetransmit = 0;
		m_dBusyCount = 0;
		m_dCurrent_State = 7100;
		m_pButton->put_FaceColor(COLOR_CYAN);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
		break;

	case 7100: //wait for time to send the still busy query
		//catch pause if needed
		if (m_bPause) 
		{
			//don't set the button's colors unless we are just going into pause
			if (!m_bIsPaused)
			{
				m_pButton->put_FaceColor(COLOR_PAUSE);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
			}
			m_bIsPaused = true;
			break;
		} 

		if (WParam == TIMER_TICK)
		{
			m_dBusyCount++;
//			if (m_dTick_Count > m_dInquireInt)
			if (m_dTick_Count > 2)
			{
				m_dTick_Count = 0;
				m_dRetransmit++;
				m_pButton->put_FaceColor(COLOR_WAIT);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_WAIT,true);
				COM_Transmit(ISZSTRING,"0\r",m_iNode);
				m_pPropertySheet->UpdateSummary("0 - STILL BUSY?");
				if (m_dBusyCount == 1)
					m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, NULL, "22290 First 0 - STILL BUSY? sent\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22300 0 - STILL BUSY?\n\r");
				m_dCurrent_State = 7200;
			}
		}

		//set to the max time to empty the entire
		//bbm via a 9600 baud connection approx 1 hr
		//60sec*60min
		if (m_dBusyCount > 3600)
		{
			m_dTick_Count = 0;
			m_dRetransmit = 0;
			m_dBusyCount = 0;
			m_dCurrent_State = 9000;
			m_dTimeOutCount++;
			m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_INST, NULL, "22310 0 - STILL BUSY? Busy too long -- doing LBR\n\r");
		}

		break;

	case 7200:

		if (WParam == TIMER_TICK)
		{
			m_dBusyCount++;

			//no response received in 
			//m_dTImeOut then send it again
			//if we've done this multiple
			//times then a restart will be
			//done via the step prior to 
			//the switch statement
			if (m_dTick_Count > m_dTimeOut)
			{
				m_dTick_Count = 0;
				m_dCurrent_State = 7100;
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_INST, NULL, "22320 Timeout on 0 - STILL BUSY?\n\r");
				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22320 Timeout on 0 - STILL BUSY?\n\r");
			}
		}
		//LParam can be 
		//	CHECKSUMGOOD == 1 == good checksum and not busy
		//	CHECKSUMBAD  == 0 == checksum is bad and don't know about busy
		//  CHECKSUMGOOD + BUSY == 3 == good checksum and busy
		if (WParam == ISRBUSY_RECORD)
		{
			//if not busy
			if (LParam == CHECKSUM_GOOD)
			{
				//reset to the old color
				m_pButton->put_FaceColor(m_crSave_Color);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(m_crSave_Color,true);
				//reset to the old state
				m_dCurrent_State = m_dSave_State;
				//restore the timer
				SetTimer(TIMER_TICK,m_dCycleInt,NULL);
				//reinit the time to next query counter
				m_dTick_Count = 0;
				//reset the re-transmit counts
				m_dRetransmit = 0;
				//reset the busy count of seconds
				m_dBusyCount=0;
			}

			//if still busy
			if (LParam == (CHECKSUM_GOOD + BUSY))
			{
				//make sure the buttons show the write color
				m_pButton->put_FaceColor(COLOR_CYAN);
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_CYAN,true);
				//reset the time to next query counter
				m_dTick_Count = 0;
				//go back and send another query
				m_dCurrent_State = 7100;
			}
		}

		break;

	case 8000:
		if (m_bPause || m_bDoingAutoTimeSet)
		{
			SetTimer(TIMER_TICK,1000,NULL);
			if (m_bPause) 
				m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
		}

		//send the command
		COM_Transmit(ISZSTRING,"5\r",m_iNode);
		m_pPropertySheet->UpdateSummary("5 - ANALYZE");
		//update the dmp file
		if (m_bDoDump)
			m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22330 5 - ANALYZE\n\r");
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
			m_pPropertySheet->m_dAt = 0;
			m_dTimeOutCount++;
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22340 ISR Timeout on receive ANALYZE response\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22350 ISR Timeout on receive ANALYZE response (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22340 ISR Timeout on receive ANALYZE response\n\r");
		}

		//wait here
		if (WParam != ISRANALYZE_OK_RECORD)
			break;
		
		//got through the initialization sequence so set up for 
		//"normal" operations
		m_dTimeOutCount = 0;
		m_pButton->put_FaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK);
		m_pPropertySheet->m_pCloseButton->SetFaceColor((m_bPause)?COLOR_PAUSE:COLOR_OK,true);
		m_dRetransmit = 0;
		m_bChecksumFail = false;
		m_dCurrent_State = 8009;
		//fall through to send the end command
	
	case 8009:
		/*
		if (m_bPause) 
		{
			SetTimer(TIMER_TICK,1000,NULL);
			m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;
		}

		if (m_bDoingAutoTimeSet)
		{
			SetTimer(TIMER_TICK,1000,NULL);
			if (m_dStatusInt) m_dTickToStatus=0;
		}
		*/
		m_dCurrent_State = 8010;
		m_dTick_Count = 0;
		COM_Transmit(ISZSTRING,"1\r",m_iNode);
		m_pPropertySheet->UpdateSummary("1 - END Enter PSR Mode Commanded");
		m_pPropertySheet->WriteToFile(TO_PFM|TO_CEV, TYPE_COMP, NULL,"22360 Forcing Instrument to Computer Time\n\r");
		break;

	case 8010://wait 1 second
		if (WParam == TIMER_TICK)
			m_dCurrent_State = 8020;
		break;

	case 8020://wait until next 1 second
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8030;
			m_dTick_Count = 0;
			m_pToday = COleDateTime::GetCurrentTime();
			temp = m_pToday.Format("%m%d%y%H%M%S");
			temp = "KW" + temp + "\r";
			COM_Transmit(ISZSTRING,(char*)LPCSTR(temp),m_iNode);
			m_pPropertySheet->UpdateSummary((char*)LPCSTR(temp));
		}
		break;
	
	case 8030://wait 1 second
		if (WParam == TIMER_TICK)
			m_dCurrent_State = 8040;
		break;

	case 8040://wait until next 1 second
		if (WParam == TIMER_TICK)
		{
			m_dCurrent_State = 8050;
			COM_Transmit(ISZSTRING,"*50C\r",m_iNode);
			m_pPropertySheet->UpdateSummary("*50C - Enter Monitor Mode Commanded");
			m_dTick_Count = 0;
		}
		break;

	case 8050://wait 1 second
		if (WParam == TIMER_TICK)
			m_dCurrent_State = 8060;
		break;

	case 8060://wait until next 1 second
		if (WParam == TIMER_TICK)
		{
			//get rid of the old 1 sec timer
			KillTimer(TIMER_TICK);
			//if manual timer set then don't 
			//force an inquire 2 immediately
			//after this -- hold off on stat also
			if (!m_bDoingAutoTimeSet)
				m_dTickToInquire=0;
			if (m_dStatusInt)m_dTickToStatus=0;

			//if we were not paused and were doing
			//an automatic time set then we reset the
			//timer to 1 sec so set it back to the
			//correct value
 			if (!m_bPause || m_bDoingAutoTimeSet)
			{
				SetTimer(TIMER_TICK,m_dCycleInt,NULL);
				m_bIsPaused = false;
			}
			
			//go back to where we were
			m_dCurrent_State = m_dSave_State;

			//no matter what we don't need
			//to do another auto time set
			m_bDoingAutoTimeSet = false;
		}
		break;

	case 9000:
		//hard fail trying to communicate with instrument
		m_bInHardFail = true;
		m_bInDumpBBM = false;
		m_pPropertySheet->m_dAcquireRecordCount = 0;
		m_pPropertySheet->m_dMessageRecordCount = 0;
		//if initialization and hard fail then send the message
		if (m_bInitInitTimeOut)
		{
			if (m_dTimeOutCount < m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22370 ISR Timeout on ISR initialization\n\r");
			if (m_dTimeOutCount == m_iMaxRepetition)
				m_pPropertySheet->WriteToFile(TO_PFM, TYPE_COMP, NULL, "22380 ISR Timeout on ISR initialization (msg filter on)\n\r");
			if (m_bDoDump)
				m_pPropertySheet->WriteToFile(TO_DMP, TYPE_COMP, NULL, "22370 ISR Timeout on ISR initialization\n\r");
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
				m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
				FlagComm(1,true);			
				bool Local = false;
				bool HeldOffLocal = false;
				if (m_dRetransmit < (m_dCommFail-1))
				{
					COM_Transmit(ISZSTRING,LONGBREAKSET"\r",m_iNode);
					m_pPropertySheet->UpdateSummary("LONG BREAK SET");
					m_dBreakSentCount++;
				}
				else
				{
					if (COM_Transmit(ISZSTRING,LOCALLONGBREAK"\r",m_iNode)==-1)
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK held off");
						HeldOffLocal = true;
					}
					else
					{
						m_pPropertySheet->UpdateSummary("LOCAL LONG BREAK");
						m_dBreakSentCount++;
					}
					Local = true;
				}

				if (m_dBreakSentCount < m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22390 Local Break held off (start)\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"22400 Local Break sent (start)\n\r":"22410 Break sent (start)\n\r");
				}
				if (m_dBreakSentCount == m_iMaxRepetition)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22420 Local Break held off (start) (msg filter on)\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, (Local)?"22430 Local Break sent (start) (msg filter on)\n\r":"22440 Break sent (start) (msg filter on)\n\r");
				}

				if (m_bDoDump)
				{
					if (HeldOffLocal)
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22450 Local LONG BREAK SET held off\n\r");
					else
						m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, (Local)?"22460 Local LONG BREAK SET\n\r":"22470 LONG BREAK SET\n\r");
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
				COM_Transmit(ISZSTRING,LONGBREAKCLR"\r",m_iNode);
				m_pPropertySheet->UpdateSummary("LONG BREAK CLEAR");
				if (m_dBreakSentCount < m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22480 Break sent (end)\n\r");
				if (m_dBreakSentCount == m_iMaxRepetition)
					m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22490 Break sent (end) (msg filter on)\n\r");

				if (m_bDoDump)
					m_pPropertySheet->WriteToFile(TO_DMP, TYPE_DUMP, NULL, "22500 LONG BREAK CLEAR\n\r");
			}
			m_dLongBreak = 0;
			//this is how many timer ticks to wait until we continue with the reset
			if ((m_dTick_Count * m_dCycleInt) > 3000)//wait a minimum 3 seconds or longer
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
			m_pButton->SetErrorIcon_Flag();//SetFlagChar(FLAG_CHAR,true);
			m_dCurrent_State = 1000;
		}
		if (m_dTick_Count % 10 == 0)
		{
			m_pButton->SetErrorIcon_Skull();//SetFlagChar(SKULL_CHAR,false);
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
			FlagComm(1,true);			
		}
		break;

	case 9800:
		m_dCurrent_State = 9810;
		//shut down the instrument
		PauseInstrument(true);	
		//but set it to attempt again in the future
		SetTimer(TIMER_PAUSE,m_iPauseMax,NULL);
		m_pButton->put_FaceColor(COLOR_ERROR);
		m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22510 WRITE FAILURE on ISR file\n\r");
		break;

	case 9810:
		if (m_bPause) 
		{
			m_bIsPaused = true;
			m_pButton->put_FaceColor(COLOR_PAUSE);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_PAUSE,true);
		}
		else
		{	
			m_bIsPaused = false;
			PauseInstrument(false);
			m_dTick_Count = INT_MAX-1;
			m_dTimeOutCount = 0;
			m_pPropertySheet->WriteToFile(TO_CEV | TO_PFM, TYPE_COMP, NULL, "22520 Attempt restart from WRITE FAILURE on ISR file\n\r");
			m_dCurrent_State = 2221;
		}
		break;

	case 9900:
		if (m_bGoodRegister)
			m_dCurrent_State = 1000;
		else
		{
			m_dCurrent_State = 9900;
			m_pButton->SetErrorIcon_Bomb();
			m_pButton->ShowIcon_Error(VARIANT_TRUE);//(true,false);
			m_pButton->put_FaceColor(COLOR_ERROR);
			m_pPropertySheet->m_pCloseButton->SetFaceColor(COLOR_ERROR,true);
		}
		break;
	default:
		m_dCurrent_State = 1000;
	}
}