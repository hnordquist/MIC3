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

// ComORTEC.cpp

// IPX communications for ORTEC instruments

//DR00245 - Repaired startup problem with IPX communications component
//PJM 13-Feb-2006 MIC 2.001

#include "stdafx.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ComORTEC.h"
#include "resource.h"
#include "NewIPXDialog.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

//<<QA>>These should be dynamically established....
#define IDC_COM_REQUEST_FOR_SPECTRUM_DATA 21113
#define IDC_COM_SEND_RAW_COMMAND_STRING   21114
#define IDC_DSPEC_SPECTRUM_DATA_PROCESSED 21115
#define IDC_COM_INIT_MCB                  21116
#define IDC_IPX_MCB_ERROR                 20007
#define IDC_COMMAND_RESPONSE_PAIR         20001
/////////////////////////////////////////////////////////////////////////////
// CComORTEC

// IPX communications for ORTEC instruments

IMPLEMENT_DYNCREATE(CComORTEC, CWinThread)
/////////////////////////////////////////////////////////////////////////////
//
//              constructor, destructor, and message map
//
/////////////////////////////////////////////////////////////////////////////
CComORTEC::CComORTEC()
{
//	TRACE("CComORTEC::CTOR\n");

	m_pmcb      = NULL;
	m_pspec     = NULL;
	m_hReceiver = NULL;	//init with no receiver to send data to
	m_hWatcher  = NULL;	//init with no watch window to send data to
	m_dNode     = -1;	//not used for ORTEC IPX
	m_pName[0]  = 0;
	m_uRawDataThresholdValue = 0;
	m_pEvent    = NULL;

	strncpy(m_pType, IPX_DISCOVERY_TYPE, sizeof(m_pType));  //for discovery
}

CComORTEC::~CComORTEC()
{	
//	TRACE("CComORTEC::DTOR - NOOP\n");
}

BEGIN_MESSAGE_MAP(CComORTEC, CWinThread)
	//{{AFX_MSG_MAP(CComORTEC)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_START,DoStart)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_RESET,DoReset)
	ON_THREAD_MESSAGE(IDC_COM_REQUEST_FOR_SPECTRUM_DATA, DoGetSpectrumDataForReceiver)
	ON_THREAD_MESSAGE(IDC_COM_SEND_RAW_COMMAND_STRING, DoSendRawCommandDataToMCA)
	ON_THREAD_MESSAGE(IDC_COM_INIT_MCB, InitMCB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
//
//                             InitInstance
//
/////////////////////////////////////////////////////////////////////////////
BOOL CComORTEC::InitInstance()
{
//	TRACE("CComORTEC::InitInstance() NOOP\n");
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//
//             Register - <<QA>> Do we need this?
//
/////////////////////////////////////////////////////////////////////////////
bool CComORTEC::Register(HWND receiver_handle, 
					bool /*out_of_bounds*/, 
					int /*node*/)
{
//	TRACE("CComORTEC::Register\n");

	if (m_hReceiver != NULL)
		return false;

	m_hReceiver = receiver_handle;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
//
//             RegisterWatch - <<QA>> Do we need this?
//
/////////////////////////////////////////////////////////////////////////////
bool CComORTEC::RegisterWatch(HWND watcher_handle, int /*node*/)
{
//	TRACE("CComORTEC::RegisterWatch(watcher_handle = %d)\n", int(watcher_handle));

	//only allow one watcher
	if (m_hWatcher != NULL) 
		return false;

	m_hWatcher = watcher_handle;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
//
//              SetLocalRegisteredMessageValues
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::SetLocalRegisteredMessageValues()
{
//	TRACE("CComORTEC::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}
/////////////////////////////////////////////////////////////////////////////
//
//                             SetUpPort
//
/////////////////////////////////////////////////////////////////////////////
bool CComORTEC::SetUpPort(char*,unsigned,unsigned,unsigned,char*,bool)
{
	//Don't use this version for ORTECIPX
//	ASSERT(FALSE);
	return false;
}

bool CComORTEC::SetUpPort(char *name)
{
//	TRACE("CComORTEC(%s)::SetUpPort(), pid:%u tid:0x%X\n",name,_getpid(),GetCurrentThreadId());

	SetLocalRegisteredMessageValues();

	strcpy(m_pName,name);

	bool breturn = false;

	m_iCMcbSuccess = -1;			 //Tri-state flag

	m_pEvent = new CEvent(FALSE, TRUE);// Create a non-signaled, manual-reset event 
//	ASSERT(m_pEvent != NULL);

	int count_ms = 1000;
	CSingleLock csl(m_pEvent);

	//DR00245
	for(int i = 0; i < 10; i++)
	{
TRACE("\n\nSending\n\n");
		if (::PostThreadMessage(m_nThreadID,IDC_COM_INIT_MCB,0,0))  //Execute InitMCB()
			break;

		Sleep(50);
	}

	csl.Lock(count_ms);     // wait for signal event or timeout
	csl.Unlock();

	if (m_iCMcbSuccess == -1)
	{
		breturn = false;
		m_csLastError = CString("IPX CSO creation timed out");
	}
	else if (m_iCMcbSuccess == 0)
	{
		breturn = false;
		m_csLastError = CString("Could not locate IPX DLL files");
	}
	else if (m_iCMcbSuccess == 1)
	{
		breturn = true;
		m_csLastError = CString("IPX CSO creation successful");
	}

	delete m_pEvent;
	m_pEvent = NULL;

	return breturn;
}
/////////////////////////////////////////////////////////////////////////////
//
//                             InitMCB
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::InitMCB(WPARAM /*W*/, LPARAM/*L*/)
{
//	TRACE("CComORTEC(%s)::InitMCB() pid:%u tid:0x%X\n", m_pName, _getpid(),GetCurrentThreadId());

	m_pspec = new CORTECSpectrum(CS_MAX_CHANNELS);

//	ASSERT(0 != strlen(m_pName));

	//the IPX index is the numeric string at the beginning of the name
	char index_number[32];
	DWORD index;
	strncpy(index_number,m_pName,sizeof(index_number));
	strtok(index_number," ");
	sscanf(index_number,"%d",&index);

	m_iCMcbSuccess = 1;  //assume true

	try
	{
		//This will throw if the ORTEC DLL's are not loadable
		m_pmcb  = new CORTECMcb(m_pName, index, m_pspec, (void *)this);
	}
	catch(...)
	{
		m_iCMcbSuccess = 0;
	}

	//signal the event that we're done
	if (m_pEvent)
		m_pEvent->SetEvent();
}
/////////////////////////////////////////////////////////////////////////////
//
//                             DoShutdown
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoShutdown(WPARAM, LPARAM)
{
//	TRACE("CComORTEC::DoShutdown()\n");

	if (m_pmcb)
		m_pmcb->Close();

	delete m_pspec;
	m_pspec = NULL;

	delete m_pmcb;
	m_pmcb = NULL;

	AfxEndThread(0);
}
/////////////////////////////////////////////////////////////////////////////
//
//                             ExitInstance
//
/////////////////////////////////////////////////////////////////////////////
int CComORTEC::ExitInstance()
{	
//	TRACE("CComORTEC(%s)::ExitInstance()\n", m_pName);
	// TODO:  perform any per-thread cleanup here
	return CCSO::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
//
//          UnRegister and UnRegisterWatch - <<QA>> do we need these??
//
/////////////////////////////////////////////////////////////////////////////
bool CComORTEC::UnRegister(int /*node*/)
{
	if (m_hReceiver != NULL)
	{
		m_hReceiver = NULL;
		return true;
	}
	return false;
}

bool CComORTEC::UnRegisterWatch(int /*node*/)
{
	if (m_hWatcher != NULL) 
	{
		m_hWatcher = NULL;
		return true;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////
//
//                             DoStart
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoStart(WPARAM /*WWord*/, LPARAM /*LWord*/)
{
//	TRACE("CComORTEC(%s)::DoStart() pid:%u tid:0x%X\n", m_pName, _getpid(),GetCurrentThreadId());

	m_pmcb->Open();
	m_pmcb->StartAcquire();
}
/////////////////////////////////////////////////////////////////////////////
//
//                             DoStop
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoStop()
{
//	TRACE("CComORTEC(%s)::DoStop() STUB pid:%u tid:0x%X\n", m_pName, _getpid(),GetCurrentThreadId());
}
/////////////////////////////////////////////////////////////////////////////
//
//                             DoReset
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoReset(WPARAM /*WWord*/, LPARAM /*LWord*/)
{
//	TRACE("CComORTEC(%s)::DoReset(), pid:%u tid:0x%X\n", m_pName, _getpid(),GetCurrentThreadId());

	SendToWatch(ISZSTRING, "IDC_COM_RESET Received by CSO", -1);

//#ifdef _DEBUG
//			static DWORD freq = 200;
//			freq += 100;
//			Beep(freq,300); // <<QA>>
//#endif

	m_pmcb->Close(); //Clear the channels(aka memory) on the DSPEC

	m_pspec->ResetDateTime();	//Reset time on the spectrum object
	m_pspec->SetThreshold(m_uRawDataThresholdValue);

	bool success = m_pmcb->Open();  //And restart

//	TRACE("CComORTEC(%s)::DoReset() - Success on Open() = %d\n",m_pName, success);

	if (success)
	{
		if (m_pmcb->StartAcquire())
		{
			m_pmcb->SendCommand("SHOW_ACTIVE_DEVICES");  //tickle it
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
//
//                             Transmit
//
/////////////////////////////////////////////////////////////////////////////
int CComORTEC::Transmit(int /*bytes*/,char *pch,int /*node*/,bool Quiet)
{
//	TRACE("CComORTEC::Transmit(%s)\n", pch);

	if (m_pmcb)
		m_pmcb->SetQuiet(Quiet);

	UINT message = 0;
	WPARAM wParam = 0;
	LPARAM lParam = 0;
	bool bSendToWatch = false;

	if (0 == (strcmp(pch,GET_REALTIME_DWELL_SPECTRUM)))
	{
		message = IDC_COM_REQUEST_FOR_SPECTRUM_DATA;
		wParam = (WPARAM)REALTIME_DWELL_1;
		bSendToWatch = !Quiet;  //only for realtime dwell
	}

	else if (0 == (strcmp(pch,GET_SHORT_DWELL_SPECTRUM)))
	{
		message = IDC_COM_REQUEST_FOR_SPECTRUM_DATA;
		wParam = (WPARAM)SHORT_DWELL_2;
		bSendToWatch = true;
	}

	else if (0 == (strcmp(pch,GET_LONG_DWELL_SPECTRUM)))
	{
		message = IDC_COM_REQUEST_FOR_SPECTRUM_DATA;
		wParam = (WPARAM)LONG_DWELL_3;
		bSendToWatch = true;
	}

	else if (0 == (strcmp(pch,GET_IMMEDIATE_DWELL_SPECTRUM)))
	{
		message = IDC_COM_REQUEST_FOR_SPECTRUM_DATA;
		wParam = (WPARAM)IMMEDIATE_DWELL_4; //i.e., do it NOW.
		bSendToWatch = true;
	}

	else
	{
		//Raw input.  From the watch window edit box, for example.

		int len = strlen(pch);
		//char *data = new char[len+1];
		char *data = (char *)CoTaskMemAlloc(len + 1);

		if (data != NULL)
		{
			message = IDC_COM_SEND_RAW_COMMAND_STRING;
			memcpy(data,pch,len);
			data[len] = 0;
			lParam = (LPARAM)data;
		}
	}

	::PostThreadMessage(m_nThreadID, message, wParam, lParam);

	if (bSendToWatch)
		SendToWatch(ISZSTRING,pch,-1);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//                             CollectParametersForNewCSO
//
/////////////////////////////////////////////////////////////////////////////
bool CComORTEC::CollectParametersForNewCSO(CString& csNameOut)
{
//	TRACE("CComORTEC()::CollectParametersForNewCSO()\n");

	//This CSO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new CSO from that data.

	//Collect the data for the new communications object.
	CNewIPXDialog dlg(m_szIniFile,m_iDlgClose,m_pName);
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(COMMUNICATIONS,m_pName,IPX_DISCOVERY_TYPE,m_szIniFile);
		return true;
	}
	else
	{
		csNameOut.Empty();
		return false;
	}

}
/////////////////////////////////////////////////////////////////////////////
//
//                     DoSendRawCommandDataToMCA
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoSendRawCommandDataToMCA(WPARAM /*unused*/, LPARAM pchr)
{
//	TRACE("CComORTEC::DoSendRawCommandDataToMCA(%s)\n", (char *)pchr);

	char *p = (char *)pchr;
	bool success = m_pmcb->SendCommand(p);

	if (!success && m_hReceiver)
	{
		char *msgcopy = (char *)CoTaskMemAlloc(256);

		if (msgcopy != NULL)
			strncpy(msgcopy,p,255);

		::PostMessage(m_hReceiver,IDC_IPX_MCB_ERROR,0,(LPARAM)msgcopy);
	}
	CoTaskMemFree((void *)pchr);
}
/////////////////////////////////////////////////////////////////////////////
//
//                     CommandAndResponse
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::CommandAndResponse(char *cmd, char *response)
{
//	TRACE("CComORTEC::CommandAndResponse(%s, %s)\n", cmd, response);

	//nobody to send to...
	if (m_hReceiver == NULL)  //will leak if this isn't here
		return;

	char crlf[3] = {0x0d, 0x0a, 0};

	int cmd_length      = strlen(cmd);
	int response_length = strlen(response);

	if (cmd_length == 0 || response_length == 0)
		return;

	strtok(cmd, crlf);  //zero out cr or lf
	strtok(cmd, crlf);  //there might be both
	cmd_length = strlen(cmd);

	char *cmd_buf = (char *)CoTaskMemAlloc(cmd_length+1);
	if (cmd_buf == NULL)
		return;
 
	response_length--;  //don't send the return character at the end of the response.

	char *response_buf = (char *)CoTaskMemAlloc(response_length +1);
	if (response_buf == NULL)
	{
		CoTaskMemFree((void *)cmd_buf);
		return;
	}

	memcpy(cmd_buf,cmd,cmd_length);
	cmd_buf[cmd_length] = 0;
	memcpy(response_buf,response,response_length);
	response_buf[response_length] = 0;

	::PostMessage(m_hReceiver,IDC_COMMAND_RESPONSE_PAIR, (WPARAM)cmd_buf, (LPARAM)response_buf);
}

/////////////////////////////////////////////////////////////////////////////
//
//                             DoGetSpectrumDataForReceiver
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::DoGetSpectrumDataForReceiver(WPARAM dwelltype, LPARAM /*lparam*/)
{
//	TRACE("CComORTEC::DoSendDataToReceiver(), pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());

	DWELL_TYPE type = (DWELL_TYPE)dwelltype;

#ifdef _DEBUG
	CString cs("");

	if (type == IMMEDIATE_DWELL_4)
		cs = "IMMEDIATE_DWELL";
	else if (type == SHORT_DWELL_2)
		cs = "SHORT_DWELL";
	else if (type == LONG_DWELL_3)
		cs = "LONG_DWELL";

	if (!cs.IsEmpty())
		TRACE("CComORTEC(%s)::DoSendDataToReceiver() datatype = %s\n", m_pName, cs);
#endif

	m_pmcb->GetSpectrum();
	m_pspec->EvaluateRawData();
	m_pspec->SetDwellType(type);

	// Send to property sheet
	if (m_hReceiver)
		::PostMessage(m_hReceiver,m_uIDC_RECEIVE,(WPARAM)dwelltype,(LPARAM)m_pspec);
}

/////////////////////////////////////////////////////////////////////////////
//
//                             SetThresholdValue
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::SetThresholdValue(UINT u)
{
//	TRACE("CComORTEC::SetThresholdValue(%u)\n",u);
	m_uRawDataThresholdValue = u;

	if (m_pspec)
		m_pspec->SetThreshold(u);
}
/////////////////////////////////////////////////////////////////////////////
//
//                             SetThresholdValue
//
/////////////////////////////////////////////////////////////////////////////
void CComORTEC::SendToWatch(int Length, char* Output, int /*node*/)
{
	if (NULL != m_hWatcher)
	{
		int dlength = (Length==ISZSTRING)?strlen(Output):Length;
		unsigned char *Data = (unsigned char*)CoTaskMemAlloc((ULONG)(dlength + 2));
		if (Data != 0)
		{
			memmove(Data,Output,dlength);
			if (Data[0]==0x0d) Data[0]= NULL;
			Data[dlength] = 0x0a;
			Data[dlength+1]=NULL;
			::PostMessage(m_hWatcher,m_uIDC_RECEIVE,WPARAM(dlength + 1),LPARAM(Data));
		}
	}
}
