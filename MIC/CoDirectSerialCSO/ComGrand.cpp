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
// ComGrand.cpp
// Direct serial communications 

#include "stdafx.h"
#include "CSO_definitions.h"
#include "INI_definitions.h"
#include "ComGrand.h"
#include "Resource.h"
#include "NewSerialComm.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ASYNC_IO
#define OPENPORT "OPENPORT"
#define CLOSEPORT "CLOSEPORT"

//added this to help Mike Browne 
//he was experiencing some problems
//#define BYPASSCHECKS
#undef BYPASSCHECKS

//windows registered message values
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CComGrand
// simple serial port communications

IMPLEMENT_DYNCREATE(CComGrand, CWinThread)

//constructor
CComGrand::CComGrand()
{
//	TRACE("CComGrand::CTOR()\n");

	//start with no handle to com object
	m_hComm = NULL;
	lpCommProp = NULL;
	lpdcb = NULL;

	m_bDone = false;
	//init not in local long break
	m_bInLocalBreak = false;

	m_bLastDUMPBBMBinary = false;
	m_bStopped = false;

	strncpy(m_pType, DIRECT_SERIAL_DISCOVERY_TYPE, sizeof(m_pType));
}


BEGIN_MESSAGE_MAP(CComGrand, CCSO)
	//{{AFX_MSG_MAP(CComGrand)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_START,DoStart)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_RESET,DoReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



CComGrand::~CComGrand()
{
//	TRACE("CComGrand::DTOR(), pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());
	//free up the memory used
	if (lpCommProp)
		free(lpCommProp);
	if (lpdcb)	
		free(lpdcb);
}

BOOL CComGrand::InitInstance()
{
//	TRACE("CComGrand::InitInstance() - NOOP\n");
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

bool CComGrand::Register(HWND receiver_handle, 
					bool /*out_of_bounds*/, 
					int /*node*/)
{
//	TRACE("CComGrand::Register\n");

	if (m_hReceiver != NULL)
		return false;

	m_hReceiver = receiver_handle;
	return true;
}

bool CComGrand::RegisterWatch(HWND watcher_handle, int /*node*/)
{
//	TRACE("CComGrand::RegisterWatch(watcher_handle = %d)\n", int(watcher_handle));

	//only allow one watcher
	if (m_hWatcher != NULL) 
		return false;

	m_hWatcher = watcher_handle;

	return true;
}

void CComGrand::SetLocalRegisteredMessageValues()
{
//	TRACE("CComGrand::SetLocalRegisteredMessageValues()\n");

	uIDC_COM_START   = m_uIDC_COM_START;
	uIDC_COM_STOP    = m_uIDC_COM_STOP;
	uIDC_COM_RESET   = m_uIDC_COM_RESET;
	uIDC_RECEIVE     = m_uIDC_RECEIVE;
	uIDC_RECEIVE_OOB = m_uIDC_RECEIVE_OOB;
}

//initialize the serial port
//from information in the INI file
bool CComGrand::SetUpPort(char * name)
{
//	TRACE("CComGrand::SetUpPort()\n");

	SetLocalRegisteredMessageValues();

	unsigned BaudRate;
	unsigned DataBits;
	strcpy(m_pName,name);
	//allow long break on this port?
	char temp[32];
	GetPrivateProfileString(name,LONGBREAK,"NO",temp,sizeof(temp),m_szIniFile);
	_strupr(temp);
	if ((strcmp(temp,"YES")==0) || (strcmp(temp,"1")==0))
		m_bLongBreak = true;
	else
		m_bLongBreak = false;
	//get the baudrate, databits, stopbits, parity, 
	//and physical port from INI file
	m_dBaudRate = GetPrivateProfileInt(name,BAUDRATE,9600,m_szIniFile);
	m_dDataBits = GetPrivateProfileInt(name,DATABITS,7,m_szIniFile);
	m_dStopBits = GetPrivateProfileInt(name,STOPBITS,1,m_szIniFile);
	GetPrivateProfileString(name,PARITY,"EVEN",
		m_pParity,sizeof(m_pParity),m_szIniFile);
	GetPrivateProfileString(name,PORT,"PORT_ERR",
		m_pPort,sizeof(m_pPort),m_szIniFile);

	//convert baudrate to "internal" form
	switch (m_dBaudRate) 
	{
	case 300:	BaudRate = BAUD_075;	break;
	case 600:	BaudRate = BAUD_600;	break;
	case 1200:	BaudRate = BAUD_1200;	break;
	case 1800:	BaudRate = BAUD_1800;	break;
	case 2400:	BaudRate = BAUD_2400;	break;
	case 4800:	BaudRate = BAUD_4800;	break;
	case 7200:	BaudRate = BAUD_7200;	break;
	case 9600:	BaudRate = BAUD_9600;	break;
	case 14400:	BaudRate = BAUD_14400;	break;
	case 19200:	BaudRate = BAUD_19200;	break;
	case 38400:	BaudRate = BAUD_38400;	break;
	case 56000:	BaudRate = BAUD_56K;	break;
	case 57600:	BaudRate = BAUD_57600;	break;
	case 115200:BaudRate = BAUD_115200;	break;
	case 128000:BaudRate = BAUD_128K;	break;
	default:
			BaudRate = BAUD_9600; 
	}

	//convert databits to "internal" form
	switch (m_dDataBits) {
	case 5:		DataBits = DATABITS_5;	break;
	case 6:		DataBits = DATABITS_6;	break;
	case 7:		DataBits = DATABITS_7;	break;
	case 8:		DataBits = DATABITS_8;	break;
	case 16:	DataBits = DATABITS_16;	break;
	default:
		DataBits = DATABITS_7;
	}

	//necessay to resolve ports com10 and above
	//but also works on com1 - com9
#ifdef BYPASSCHECKS
	strcpy(temp,m_pPort);
#else
	strcpy(temp,"\\\\.\\");
	strcat(temp,m_pPort);
#endif
	//if we already have an open com close it first
	if (m_hComm)
		CloseHandle(m_hComm);
	if (lpCommProp)
		free(lpCommProp);
	if (lpdcb)
		free(lpdcb);

	//open the physical the port by name
	if ((m_hComm = CreateFile(temp,//m_pPort,	//port name
		GENERIC_READ | GENERIC_WRITE, 	//read and write
		0,								//not shared (can't on com)
		NULL,							//default security attributes
		OPEN_EXISTING,					//open existing com port
#ifdef ASYNC_IO
		FILE_FLAG_OVERLAPPED,			//async i/o
#else
		NULL,							//sync i/o
#endif
		NULL))==INVALID_HANDLE_VALUE)	//template file -- not used on com
		return false;
	
	//configure the port
	//get the size of the actual commprop packet
	//need to do this twice, once to get the size of COMMPROP
	//and then to get the actual data
	lpCommProp = (LPCOMMPROP)malloc(sizeof(COMMPROP));
	lpCommProp->wPacketLength = sizeof(COMMPROP);
	GetCommProperties(m_hComm, lpCommProp);
	unsigned short dwSize = lpCommProp->wPacketLength;
	free(lpCommProp);

	//get the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(dwSize);
	lpCommProp->wPacketLength = dwSize;
	if (GetCommProperties(m_hComm, lpCommProp) == 0)		//scr00498 
	{														//scr00498
		free(lpCommProp);									//scr00498
		lpCommProp = NULL;									//scr00498
		return false;										//scr00498
	}														//scr00498

#ifndef BYPASSCHECKS	
	//verify that we have a valid packet
	if ((lpCommProp->wPacketVersion != 0xffff) && //win95
		(lpCommProp->wPacketVersion != 0x0002) && //NT		//scr00498
		(lpCommProp->wPacketVersion != 0x0100))				//scr00498
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}
/*
PST_FAX 33
PST_LAT 257
PST_MODEM 6
PST_NETWORK_BRIDGE 256
PST_PARALLELPORT 2
PST_RS232 1
PST_RS422 3
PST_RS423 4
PST_RS449 5
PST_SCANNER 34
PST_TCPIP_TELNET 258
PST_UNSPECIFIED 0
PST_X25 259
*/
	//verify that we have a serial port
	if (lpCommProp->dwServiceMask != SP_SERIALCOMM)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}
	else
		if ((lpCommProp->dwProvSubType != PST_RS232) &&
			(lpCommProp->dwProvSubType != PST_UNSPECIFIED) &&	//scr00498
			(lpCommProp->dwProvSubType != PST_MODEM))			//scr00498
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}

	//verify we can have the baud rate we need
	if (lpCommProp->dwMaxBaud != BAUD_USER)		//user settable?
		if (lpCommProp->dwMaxBaud < BaudRate)	//nope-is it at what we need?
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}
#endif


	//Made changes to upgrade from Baseline MIC_CB_028 
	//to MIC 1.9.0.7.
	//PJM 01/19/2005
	//
	//get device control block
	lpdcb = (DCB*)malloc(sizeof(DCB));
	GetCommState(m_hComm,lpdcb);

	//set baud rate
	if (lpCommProp->dwSettableParams & SP_BAUD)
		lpdcb->BaudRate = m_dBaudRate;

	//force binary -- must be true for Win32 API
	lpdcb->fBinary = true;

	//set parity checking 
	if (lpCommProp->dwSettableParams & SP_PARITY_CHECK)
	{
		if ((m_pParity[0] =='N')||(m_pParity[0] == 'n'))
			lpdcb->fParity = false;
		else
			lpdcb->fParity = true;
	}

	//set clear to send and data set ready flow control off
	lpdcb->fOutxCtsFlow = false;
	lpdcb->fOutxDsrFlow = false;

	//set DTR off
	lpdcb->fDtrControl = DTR_CONTROL_ENABLE;
	lpdcb->fDsrSensitivity = false;

	//set xoff and xon to non-functioning state
	lpdcb->fOutX = false;
	lpdcb->fInX  = false;

	//set parity error replace char flag
	if (strcmp(m_pParity,"NONE") == 0)
	{
		//no place to put it if parity is true
		lpdcb->fErrorChar = false;
		//clear the parity replacement char
		lpdcb->ErrorChar = NULL;
	}
	else
	{
		lpdcb->fErrorChar = true;
		//set replacement char for parity errors
		lpdcb->ErrorChar = '#';
	}

	//make sure we can receive NULL bytes
	lpdcb->fNull = false;

	//no RTS flow control
	//this seems backwards -- but was needed 
	//to get the grands to talk
	lpdcb->fRtsControl = RTS_CONTROL_ENABLE;

	//always abort on error
	lpdcb->fAbortOnError = TRUE;

	//set data size
	if (lpCommProp->dwSettableParams & SP_DATABITS)
		lpdcb->ByteSize = (unsigned char)m_dDataBits;

	//set parity
	if (strcmp(m_pParity,"EVEN")==0)
		lpdcb->Parity = EVENPARITY;
	else if (strcmp(m_pParity,"ODD")==0)
		lpdcb->Parity = ODDPARITY;
	else if (strcmp(m_pParity,"NONE")==0)
		lpdcb->Parity = NOPARITY;
	else if (strcmp(m_pParity,"MARK")==0)
		lpdcb->Parity = MARKPARITY;
	else if (strcmp(m_pParity,"SPACE")==0)
		lpdcb->Parity = SPACEPARITY;
	else
		lpdcb->Parity = EVENPARITY;

	//set stop bits
	switch (m_dStopBits) {
	case 1: lpdcb->StopBits = ONESTOPBIT; break;//one
	case 2: lpdcb->StopBits = TWOSTOPBITS;break;//two
	case 15:lpdcb->StopBits = ONE5STOPBITS;break;//one and a half
	default:lpdcb->StopBits = ONESTOPBIT; break;//one
	}


	//set the port to what we want
	BOOL Result = SetCommState(m_hComm, lpdcb);

	//The SetCommMask function specifies a set of events 
	//to be monitored for a communications device
	SetCommMask(m_hComm, EV_RXCHAR | EV_BREAK | EV_ERR );

	//create an event object for the com to wait on
	m_OverlapR.Internal = 0;
	m_OverlapR.InternalHigh = 0;
	m_OverlapR.Offset = 0;
	m_OverlapR.OffsetHigh = 0;
	m_OverlapR.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_OverlapT.Internal = 0;
	m_OverlapT.InternalHigh = 0;
	m_OverlapT.Offset = 0;
	m_OverlapT.OffsetHigh = 0;
	m_OverlapT.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	return Result?true:false;
}
// End of PJM 01/19/05

//initialize the serial port
//from parameter information
bool CComGrand::SetUpPort(
	char* szPort, 
	unsigned BaudRate, 
	unsigned DataBits, 
	unsigned StopBits, 
	char* szParity, 
	bool bLongBreak)
{
//	TRACE("CComGrand::SetUpPort()\n");
	//allow long break on this port?
	m_bLongBreak = bLongBreak;
	//get the baudrate, databits, stopbits, parity, 
	//and physical port from INI file
	m_dBaudRate = BaudRate;
	m_dDataBits = DataBits;
	m_dStopBits = StopBits;
	strcpy(m_pParity,szParity);
	strcpy(m_pPort,szPort);

	m_OverlapT.hEvent = NULL;
	m_OverlapR.hEvent = NULL;

	//convert baudrate to "internal" form
	switch (m_dBaudRate) {
	case 300:	BaudRate = BAUD_075;	break;
	case 600:	BaudRate = BAUD_600;	break;
	case 1200:	BaudRate = BAUD_1200;	break;
	case 1800:	BaudRate = BAUD_1800;	break;
	case 2400:	BaudRate = BAUD_2400;	break;
	case 4800:	BaudRate = BAUD_4800;	break;
	case 7200:	BaudRate = BAUD_7200;	break;
	case 9600:	BaudRate = BAUD_9600;	break;
	case 14400:	BaudRate = BAUD_14400;	break;
	case 19200:	BaudRate = BAUD_19200;	break;
	case 38400:	BaudRate = BAUD_38400;	break;
	case 56000:	BaudRate = BAUD_56K;	break;
	case 57600:	BaudRate = BAUD_57600;	break;
	case 115200:BaudRate = BAUD_115200;	break;
	case 128000:BaudRate = BAUD_128K;	break;
	default:
			BaudRate = BAUD_9600; 
	}

	//convert databits to "internal" form
	switch (m_dDataBits) {
	case 5:		DataBits = DATABITS_5;	break;
	case 6:		DataBits = DATABITS_6;	break;
	case 7:		DataBits = DATABITS_7;	break;
	case 8:		DataBits = DATABITS_8;	break;
	case 16:	DataBits = DATABITS_16;	break;
	default:
		DataBits = DATABITS_7;
	}

	//necessay to resolve ports com10 and above
	//but also works on com1 - com9
	char temp[MAX_PATH];
	strcpy(temp,"\\\\.\\");
	strcat(temp,m_pPort);

	//if we already have an open com close it first
	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}

	if (m_hComm)
	{
		CloseHandle(m_hComm);
		Sleep(500);
	}

	//open the physical the port by name
	if ((m_hComm = CreateFile(temp,//m_pPort,	//port name
		GENERIC_READ | GENERIC_WRITE, 	//read and write
		0,								//not shared (can't on com)
		NULL,							//default security attributes
		OPEN_EXISTING,					//open existing com port
#ifdef ASYNC_IO
		FILE_FLAG_OVERLAPPED,			//async i/o
#else
		NULL,							//sync i/o
#endif
		NULL))==INVALID_HANDLE_VALUE)	//template file -- not used on com
		return false;
	
	//configure the port
	//get the size of the actual commprop packet
	//need to do this twice, once to get the size of COMMPROP
	//and then to get the actual data
	lpCommProp = (LPCOMMPROP)malloc(sizeof(COMMPROP));
	lpCommProp->wPacketLength = sizeof(COMMPROP);
	GetCommProperties(m_hComm, lpCommProp);
	unsigned short dwSize = lpCommProp->wPacketLength;
	free(lpCommProp);

	//get the actual commprop
	lpCommProp = (LPCOMMPROP)malloc(dwSize);
	lpCommProp->wPacketLength = dwSize;
	if (GetCommProperties(m_hComm, lpCommProp) == 0)		//scr00498
	{														//scr00498
		free(lpCommProp);									//scr00498
		lpCommProp = NULL;									//scr00498
		return false;										//scr00498
	}														//scr00498

#ifndef BYPASSCHECKS	
	//verify that we have a valid packet
	if ((lpCommProp->wPacketVersion != 0xffff) && //win95
		(lpCommProp->wPacketVersion != 0x0002) && //NT		//scr00498
		(lpCommProp->wPacketVersion != 0x0100))				//scr00498
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}

	//verify that we have a serial port
	if (lpCommProp->dwServiceMask != SP_SERIALCOMM)
	{
		free(lpCommProp);
		lpCommProp = NULL;
		return false;
	}
	else
		if ((lpCommProp->dwProvSubType != PST_RS232) &&
			(lpCommProp->dwProvSubType != PST_UNSPECIFIED) &&	//scr00498
			(lpCommProp->dwProvSubType != PST_MODEM))			//scr00498
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}

	//verify we can have the baud rate we need
	if (lpCommProp->dwMaxBaud != BAUD_USER)		//user settable?
		if (lpCommProp->dwMaxBaud < BaudRate)	//nope-is it at what we need?
		{
			free(lpCommProp);
			lpCommProp = NULL;
			return false;
		}
#endif
	//get device control block
	lpdcb = (DCB*)malloc(sizeof(DCB));
	GetCommState(m_hComm,lpdcb);

	//set baud rate
	if (lpCommProp->dwSettableParams & SP_BAUD)

		lpdcb->BaudRate = m_dBaudRate;

	//force binary
	lpdcb->fBinary = true;

	//set parity checking 
	if (lpCommProp->dwSettableParams & SP_PARITY_CHECK)
	{
		if (strcmp(m_pParity,"NONE")==0)
			lpdcb->fParity = false;
		else
			lpdcb->fParity = true;
	}

	//set clear to send and data set ready flow control off
	lpdcb->fOutxCtsFlow = false;
	lpdcb->fOutxDsrFlow = false;

	//set DTR on
	lpdcb->fDtrControl = DTR_CONTROL_ENABLE;

	//set xoff and xon to non-functioning state
	lpdcb->fOutX = false;
	lpdcb->fInX  = false;

	//set parity error replace char flag
	lpdcb->fErrorChar = true;

	//make sure we can receive NULL bytes
	lpdcb->fNull = false;

	//no RTS flow control
	//this seems backwards -- but was needed 
	//to get the grands to talk
	lpdcb->fRtsControl = RTS_CONTROL_ENABLE;

	//always abort on error
	lpdcb->fAbortOnError = true;

	//set data size
	if (lpCommProp->dwSettableParams & SP_DATABITS)
		lpdcb->ByteSize = (unsigned char)m_dDataBits;

	//set parity
	if (strcmp(m_pParity,"EVEN")==0)
		lpdcb->Parity = EVENPARITY;
	else if (strcmp(m_pParity,"ODD")==0)
		lpdcb->Parity = ODDPARITY;
	else if (strcmp(m_pParity,"NONE")==0)
		lpdcb->Parity = NOPARITY;
	else if (strcmp(m_pParity,"MARK")==0)
		lpdcb->Parity = MARKPARITY;
	else
		lpdcb->Parity = EVENPARITY;

	//set stop bits
	switch (m_dStopBits) {
	case 1: lpdcb->StopBits = ONESTOPBIT; break;//one
	case 2: lpdcb->StopBits = TWOSTOPBITS;break;//two
	case 15:lpdcb->StopBits = ONE5STOPBITS;break;//one and a half
	default:lpdcb->StopBits = ONESTOPBIT; break;//one
	}

	//set replacement char for parity errors
	lpdcb->ErrorChar = '#';

	//set the port to what we want
	BOOL Result = SetCommState(m_hComm, lpdcb);

	//The SetCommMask function specifies a set of events 
	//to be monitored for a communications device
	SetCommMask(m_hComm, EV_RXCHAR | EV_BREAK | EV_ERR );

	//create an event object for the com to wait on
	m_OverlapR.Internal = 0;
	m_OverlapR.InternalHigh = 0;
	m_OverlapR.Offset = 0;
	m_OverlapR.OffsetHigh = 0;
	m_OverlapR.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_OverlapT.Internal = 0;
	m_OverlapT.InternalHigh = 0;
	m_OverlapT.Offset = 0;
	m_OverlapT.OffsetHigh = 0;
	m_OverlapT.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	return Result?true:false;
}


void CComGrand::DoShutdown(WPARAM, LPARAM)
{
//	TRACE("CComGrand::DoShutdown()\n");

	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}

	AfxEndThread(0);  //This calls the DTOR
}


int CComGrand::ExitInstance()
{
//	TRACE("CComGrand::ExitInstance()\n");
	//close comm port 
	CloseHandle(m_hComm);
	return CCSO::ExitInstance();
}

bool CComGrand::UnRegister(int /*node*/)
{
	if (m_hReceiver != NULL)
	{
		m_hReceiver = NULL;
		return true;
	}
	return false;
}

bool CComGrand::UnRegisterWatch(int /*node*/)
{
	if (m_hWatcher != NULL) 
	{
		m_hWatcher = NULL;
		return true;
	}
	return false;
}

void CComGrand::WatchDogSend()
{
	//TRACE("CComGrand::WatchDogSend()\n");
	if (m_bWatchDogOk)
	{
		//send ok to watchdog - ILON only
	}
	if (m_bStopped)
		SendToWatch(12,"Comm STOPPED",0);  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
}

void CComGrand::DoStart(WPARAM /*WWord*/, LPARAM /*LWord*/)
{
	//TRACE("CComGrand::DoStart()\n");


	if ((m_hComm == NULL) || (m_hComm == INVALID_HANDLE_VALUE))
		return;


	//enough bytes for approximately 10 seconds of data
	BYTE inbuff[10240];
	DWORD nBytesRead, dwEvent, dwError;
	COMSTAT cs;
	m_bDone = false;
	//begin read loop here
	while (!m_bDone) 
	{
		//hang reads here 
		if (WaitCommEvent(m_hComm, &dwEvent, NULL))
		{
			//clear the "error" flags. actually the char received
			ClearCommError(m_hComm, &dwError, &cs);
			//if the event out of waitcom says we have a char and there is one
			if ((dwEvent & EV_RXCHAR) && cs.cbInQue) 
			{
				//go get the number of characters the port says we have
				//up to the size of the buffer
				if (!ReadFile(m_hComm, inbuff, min(cs.cbInQue,sizeof(inbuff)),
					&nBytesRead, &m_OverlapR)) 
				{
					//if for some reason we can't do a read of that many 
					//characters or any other error occurs on the read
					//then do a time-out dialog box and continue
					ClearCommError(m_hComm, &dwError, &cs);
				}
				else 
				{
					//if we actually got some characters to read
					if (nBytesRead)
					{
						//MIC_CB_028 to MIC 1.907 PJM 1/19/05
						if (strcmp(m_szLastSend,(const char *)inbuff)==0)
						{
#ifdef COM_LOOP_BACK_MODE_MESSAGE
							char szTemp[64];
							strcpy(szTemp,"WARNING: COM Loop-back mode detected.\r");
							if (m_hReceiver != NULL)
							{
								char* Data = (char*)CoTaskMemAlloc(strlen(szTemp)+1);
								strcpy(Data,szTemp);
								::PostMessage(m_hReceiver, m_uIDC_RECEIVE_OOB,
									(WPARAM)strlen(szTemp)+1,(LPARAM)Data);
							}
							if (m_hWatcher != NULL)

							{
								char* Data = (char *)CoTaskMemAlloc(strlen(szTemp)+1);
								strcpy(Data,szTemp);
								::PostMessage(m_hWatcher, m_uIDC_RECEIVE,
									(WPARAM)strlen(szTemp),(LPARAM)Data);
							}
#endif
						}

						//give all of the buffer to the virtual instrument.
						if (m_hReceiver != NULL)
						{
							char* Data = (char*)CoTaskMemAlloc(nBytesRead);
							memmove(Data,inbuff,nBytesRead);
							::PostMessage(m_hReceiver,m_uIDC_RECEIVE,nBytesRead,(LPARAM)Data);
						}
						//give all of the buffer to the watcher if there is one
						if (m_hWatcher != NULL)
						{
							char* Data = (char*)CoTaskMemAlloc(nBytesRead);
							memmove(Data,inbuff,nBytesRead);
							::PostMessage(m_hWatcher,m_uIDC_RECEIVE,nBytesRead,(LPARAM)Data);
						}

					}//end if (nBytesRead
				}//if (file read ELSE
			}//end if ((dwEvent

		}//end if (WaitCommEvent
		else
		{
			ClearCommError(m_hComm, &dwError, &cs);
			/*DWORD dWord =*/ ::GetLastError();
		}
	}//end while not done
	PurgeComm(m_hComm, PURGE_RXCLEAR);
}

void CComGrand::DoStop()
{
//	TRACE("CComGrand::DoStop()\n");
	//flag dostart to exit now
	m_bDone = true;
	if ((m_hComm) && (m_hComm != INVALID_HANDLE_VALUE))
	{
		PurgeComm(m_hComm,PURGE_RXCLEAR);
		PurgeComm(m_hComm,PURGE_TXCLEAR);
		CloseHandle(m_hComm);
		if (m_OverlapR.hEvent)
			CloseHandle(m_OverlapR.hEvent);
		if (m_OverlapT.hEvent)
			CloseHandle(m_OverlapT.hEvent);
		WaitForSingleObject(m_hComm,1000);
	}
	m_hComm = NULL;
}

void CComGrand::DoReset(WPARAM /*WWord*/, LPARAM /*LWord*/)
{
//	TRACE("CComGrand::DoReset()\n");

	if (lpCommProp)
	{
		free(lpCommProp);
		lpCommProp = NULL;
	}
	if (lpdcb)
	{
		free(lpdcb);
		lpdcb = NULL;
	}

		//wait a couple of seconds
		//before we attempt to reestablish 
		//the connection -- this is safe 'cause 
		//doreset runs in this thread 
		Sleep(2000);
		//reset the port and restart
		if (SetUpPort(m_pName))
		{
			::PostThreadMessage(m_nThreadID,uIDC_COM_START,0,0);
			SendToWatch(14,"Comm RESTARTED",0);
			m_bStopped = false;
		}
		else
		{
			m_hComm = NULL;
			SendToWatch(19,"Comm RESTART FAILED",0);
		}
}

int CComGrand::Transmit(int Length, char* Output, int /*Node*/, bool Quiet)
{
	//TRACE("CComGrand::Transmit(length = %d, output = %s)\n", Length,Output);
	//output Output to file\com.
	unsigned long BytesWritten = 0;
	CString cResult;
	cResult = "";
	//if any of the commands to the com object then do them
	//SETBAUD LONGBREAKSET LONGBREAKCLR LOCALLONGBREAK
	if (strncmp(Output,"SETBAUD",7)==0)
	{
		unsigned uBaud = atoi(&Output[7]);
		if (SetUpPort(m_pPort, uBaud, 8, 1, "NONE", true))
		{
			::PostThreadMessage(m_nThreadID,uIDC_COM_START,0,0);
			cResult.Format("%s: Ok %d\r\n", Output, uBaud);
		}
		else
		{
			m_hComm = NULL;
			cResult.Format("%s: FAILED %d\r\n",Output, uBaud);
		}
	}
	else if (strncmp(Output,CLOSEPORT,9)==0)
	{
		DoStop();
		SendToWatch(12,"Comm STOPPED");
		m_bStopped = true;
	}
	else if (strncmp(Output,OPENPORT,8)==0)
	{
		::PostThreadMessage(m_nThreadID,uIDC_COM_RESET,0,0);
	}
	else if (strncmp(Output,LONGBREAKSET,12)==0) 
	{
		if (m_hComm==NULL)
		{
			m_bInLocalBreak = false;
			DoStop();
			::PostThreadMessage(m_nThreadID,uIDC_COM_RESET,0,0);
		}
		else 
		{
			//only if comm device allows longbreak
			if (m_bLongBreak) 
			{
				m_bInLocalBreak = true;
				SetCommBreak(m_hComm);
			}
		}
	}
	else if	(strncmp(Output,LOCALLONGBREAK,14)==0)
	{
		//ignore the followon clear long break
		m_bInLocalBreak = false;
		//stop and reset the port
		DoStop();
		::PostThreadMessage(m_nThreadID,uIDC_COM_RESET,0,0);
	} 
	//if command is to terminate a long break then do it
	else if (strncmp(Output,LONGBREAKCLR,12)==0)
	{
		//only if comm device allows longbreak
		if (m_bLongBreak && m_bInLocalBreak)  
		{
			ClearCommBreak(m_hComm);
			m_bInLocalBreak = false;
		}
	}
	else
	{
		//send the command otherwise
		if (m_hComm)
		{
			//save the last send command
			int iLength = (Length==ISZSTRING)?strlen(Output):Length;
			strncpy(m_szLastSend,Output,min(iLength,sizeof(m_szLastSend)-1));
			m_szLastSend[min(iLength,sizeof(m_szLastSend)-1)] = NULL;

			WriteFile (m_hComm, Output, iLength, &BytesWritten, &m_OverlapT);
		}
	}
	//echo the command to the watcher if there is one
	if ((NULL != m_hWatcher) && !Quiet)
	{
		if (strcmp(Output,"DUMPBBM\r")==0)
			m_bLastDUMPBBMBinary = false;
		else if (strcmp(Output,"DUMPBBBM\r")==0)
			m_bLastDUMPBBMBinary = true;
		if ((strcmp(Output,"DUMPOK\r")==0) && m_bLastDUMPBBMBinary)
		{
			char*Data = (char*)CoTaskMemAlloc(4);
			Data[0] = 0x0d;
			Data[1] = 0x0a;
			Data[2] = NULL;
			::PostMessage(m_hWatcher, m_uIDC_RECEIVE,2,(LPARAM)Data);

		}
		if (cResult.GetLength()==0)
		{
			int dlength = (Length==ISZSTRING)?strlen(Output):Length;
			char*Data = (char*)CoTaskMemAlloc(dlength+2);
			memmove(Data,Output,dlength);
			//if (Data[0]==0x0d) Data[0]= NULL;
			m_bLastDUMPBBMBinary = false;
			Data[dlength] = 0x0a;
			Data[dlength+1]=NULL;
			::PostMessage(m_hWatcher,m_uIDC_RECEIVE,dlength+1,(LPARAM)Data);
		}
		else
		{
			int dlength = cResult.GetLength();
			//char*Data = (char*)malloc(dlength+1);
			char*Data = (char*)CoTaskMemAlloc(dlength+1);
			memmove(Data,LPCSTR(cResult),dlength);
			Data[dlength] = NULL;
			::PostMessage(m_hWatcher,m_uIDC_RECEIVE,dlength+1,(LPARAM)Data);
		}
	}
	return (int)BytesWritten;
}

//////////////////////////////////////////////////////////////////////////////
//
//                      CollectParametersForNewCSO
//
//////////////////////////////////////////////////////////////////////////////
bool CComGrand::CollectParametersForNewCSO(CString& csNameOut)
{
//	TRACE("CComGrand::CollectParametersForNewCSO()\n");

	//This CSO is temporary, instatiated solely for the
	//purpose of collecting data to put into the INI file so that
	//the main dialog can pick up the data in the INI file
	//and create an actual new CSO from that data.

	//Collect the data for the new communications object.
	CNewSerialComm dlg(m_szIniFile,m_iDlgClose,m_pName);
	int result = dlg.DoModal();
	if (result == IDOK)
	{
		//the new dialog changed the name - maybe.
		csNameOut = CString(m_pName);
		//add instrument name=typestring to inifile
		WritePrivateProfileString(COMMUNICATIONS,m_pName,DIRECT_SERIAL_DISCOVERY_TYPE,m_szIniFile);
		return true;
	}
	else
	{
		csNameOut.Empty();
		return false;
	}
}

void CComGrand::SendToWatch(int Length, char* Output, int /*node*/)
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