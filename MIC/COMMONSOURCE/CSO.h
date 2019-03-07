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
// CSO.h
 
#ifndef CSO_H
#define CSO_H

class CCSO : public CWinThread
{
// Construction
public:

	CCSO();
	virtual ~CCSO();

// Attributes
public:

	//
	// Pure virtual
	//
	virtual bool CollectParametersForNewCSO(CString& csNameOut) = 0;

	virtual void DoReset(WPARAM,LPARAM) = 0;
	virtual void DoStart(WPARAM,LPARAM) = 0;
	virtual void DoShutdown(WPARAM,LPARAM) = 0;
	virtual void DoStop() = 0;

	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1) = 0;

	virtual bool RegisterWatch(HWND watcher_handle,int node = -1) = 0;
	virtual void SendToWatch(int bytes, char*, int node = -1) = 0;

	virtual void SetLocalRegisteredMessageValues() = 0;
	virtual bool SetUpPort(char *) = 0;

	virtual bool SetUpPort(char *szPort, unsigned BaudRate, unsigned DataBits, 
							unsigned StopBits, char* szParity, bool bLongBreak) = 0;

	virtual int  Transmit(int bytes,char *,int node = -1, bool Quiet=false) = 0;
	virtual bool UnRegister(int node = -1) = 0;
	virtual bool UnRegisterWatch(int node = -1) = 0;

	//
	// Non-pure virtual - have implementations in CSO.cpp
	//
	virtual void SetWindowsRegisteredMessageStrings(
		ULONG ulRECEIVE, 
		ULONG ulRECEIVE_OOB,
		ULONG ulIDC_COM_START,
		ULONG ulIDC_COM_STOP,
		ULONG ulIDC_COM_RESET);

	virtual void SetThresholdValue(UINT u);

	virtual CString *GetLastError();
	virtual char *GetName();
	virtual char *GetType();
	virtual void PutIniFileName(char *inifile);
	virtual void WatchDogSend();
	virtual void WatchDogOk(bool set);
	virtual void PutDialogCloseTime(int milliseconds);

	//
	// CSO Base class variables
	//
	bool	m_bWatchDogOk;
	char    m_szIniFile[_MAX_PATH];
	CString m_csLastError;
	int     m_iDlgClose;
	char    m_pType[32];
	char    m_pName[32];

	HWND    m_hReceiver;
	HWND    m_hWatcher;
	HWND    m_hOOB;

	UINT    m_uIDC_RECEIVE;
	UINT    m_uIDC_RECEIVE_OOB;
	UINT    m_uIDC_COM_START;
	UINT    m_uIDC_COM_STOP;
	UINT    m_uIDC_COM_RESET;
	
	CString	m_csVersion;
	CString	m_csNameVersionDate;
};

#endif //CSO_H