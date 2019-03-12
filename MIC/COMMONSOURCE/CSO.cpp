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
// CSO.cpp 

#include "stdafx.h"
#include "CSO.h"
#include "CSO_definitions.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSO.cpp
////////////////////////////////////////////////
//
//            Constructor and destructor
//
////////////////////////////////////////////////
CCSO::CCSO()
{
//	TRACE("CCSO::CTOR\n");
	m_hReceiver = 0;
	m_hWatcher = 0;
	m_hOOB = 0;
	m_uIDC_RECEIVE = 0;
	m_uIDC_RECEIVE_OOB = 0;
 	m_uIDC_COM_START = 0;
	m_uIDC_COM_STOP = 0;
	m_uIDC_COM_RESET = 0;

	CVersionInfo VersionInfo;
	m_csVersion = VersionInfo.FileVersion();
	m_csVersion.Remove(' ');
	m_csVersion.Replace(',','.');
	CString csName = VersionInfo.InternalName();
	m_csNameVersionDate = csName + " " + m_csVersion + " " + __DATE__;

	//CVersionInfo VersionInfo;
	//CString Name    = VersionInfo.InternalName();
	//CString Version = VersionInfo.FileVersion();
	//Version.Remove(' ');
	//Version.Replace(',','.');
	//Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	//CString cs = 
}

CCSO::~CCSO()
{
//	TRACE("CCSO::DTOR NOOP\n");
}
////////////////////////////////////////////////
//
//            GetLastError
//
////////////////////////////////////////////////
CString * CCSO::GetLastError(){return &m_csLastError;}
////////////////////////////////////////////////
//
//            GetName
//
////////////////////////////////////////////////
char * CCSO::GetName(){return m_pName;}
////////////////////////////////////////////////
//
//            GetName
//
////////////////////////////////////////////////
char * CCSO::GetType(){return m_pType;}
////////////////////////////////////////////////
//
//            PutDialogCloseTime
//
////////////////////////////////////////////////
void CCSO::PutDialogCloseTime(int milliseconds)
{m_iDlgClose = milliseconds;}
////////////////////////////////////////////////
//
//            PutIniFileName
//
////////////////////////////////////////////////
void CCSO::PutIniFileName(char *inifile)
{strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));}
////////////////////////////////////////////////
//
//            WatchDogSend
//
////////////////////////////////////////////////
void CCSO::WatchDogSend(){;}
////////////////////////////////////////////////
//
//            WatchDogOk
//
////////////////////////////////////////////////
void CCSO::WatchDogOk(bool set){m_bWatchDogOk = set;}
////////////////////////////////////////////////
//
//            SetThresholdValue
//
//  Only needed for DSPEC at this time...
//
////////////////////////////////////////////////
void CCSO::SetThresholdValue(UINT)
{
}
////////////////////////////////////////////////
//
//     SetWindowsRegisteredMessageStrings
//
////////////////////////////////////////////////
void CCSO::SetWindowsRegisteredMessageStrings(
	ULONG ulRECEIVE, 
	ULONG ulRECEIVE_OOB,
	ULONG ulSTART,
	ULONG ulSTOP,
	ULONG ulRESET)
{
//	TRACE("CCSO::SetWindowsRegisteredMessageStrings()\n");

	m_uIDC_RECEIVE         = (UINT)ulRECEIVE;
	m_uIDC_RECEIVE_OOB     = (UINT)ulRECEIVE_OOB;
	m_uIDC_COM_START       = (UINT)ulSTART;
	m_uIDC_COM_STOP        = (UINT)ulSTOP;
	m_uIDC_COM_RESET       = (UINT)ulRESET;

	SetLocalRegisteredMessageValues();
}