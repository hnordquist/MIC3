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
#ifndef COMORTEC_H
#define COMORTEC_H

// ComORTEC.h

#include "mcbcio32/mcbcio32.h"
#include "ORTECMcb.h"
#include "ORTECSpectrum.h"
#include "CSO.h"
#include <Afxmt.h>

#define IPX_DISCOVERY_TYPE "ORTECIPX"

// CComORTEC thread

class CComORTEC : public CCSO
{
	DECLARE_DYNCREATE(CComORTEC)

protected:
	CComORTEC();           // protected constructor used by dynamic creation

public:
	CORTECSpectrum	*m_pspec;
	CORTECMcb		*m_pmcb;
	int				m_dNode;  //Not used for ORTEC IPX.  Is set to -1 in the constructor.

	UINT m_uRawDataThresholdValue;

	virtual bool CollectParametersForNewCSO(CString& csNameOut);
	virtual void DoReset(WPARAM,LPARAM);
	virtual void DoStart(WPARAM,LPARAM);
	virtual void DoShutdown(WPARAM,LPARAM);
	virtual void DoStop();

	//<<QA>> These weren't in the orginal MIC_GM version.
	virtual bool Register(
		HWND receiver_handle,bool bout_of_bounds = false,int node = -1);
	virtual bool RegisterWatch(HWND watcher_handle,int node = -1);
	virtual bool UnRegister(int node = -1);
	virtual bool UnRegisterWatch(int node = -1);
	virtual void SendToWatch(int Length, char* Output, int /*node*/);
	//end <<QA>>

	virtual void SetLocalRegisteredMessageValues();
	virtual int Transmit(int bytes,char *,int node,bool Quiet=false);
	virtual bool SetUpPort(char *);
	virtual bool SetUpPort(char*,unsigned,unsigned,unsigned,char*,bool); //not used

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComORTEC)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CComORTEC();

	void	DoGetSpectrumDataForReceiver(WPARAM, LPARAM);
	void	DoSendRawCommandDataToMCA(WPARAM, LPARAM);
	void	CommandAndResponse(char *cmd, char *response); //for the CSO to forward to the cracker
	void	SetThresholdValue(UINT u);
	void	InitMCB(WPARAM, LPARAM);

protected:
	char m_szLastSend[64];
	int  m_iCMcbSuccess;

	CEvent *m_pEvent;

	// Generated message map functions
	//{{AFX_MSG(CComORTEC)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //COMORTEC_H