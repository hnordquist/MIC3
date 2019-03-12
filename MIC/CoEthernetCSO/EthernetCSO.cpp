/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

� 2019 Triad National Security, LLC. All rights reserved.

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
// DirectSerialCSO.cpp

#include "stdafx.h"
#include "CoEthernetCSO.h"
#include "EthernetCSO.h"
#include "NewEthernetComm.h"
#include "ini_definitions.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//local windows registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB; 
static UINT uIDC_COM_START;
static UINT uIDC_COM_STOP;
static UINT uIDC_COM_RESET;

/////////////////////////////////////////////////////////////////////////////
// CEthernetCSO

//1
STDMETHODIMP CEthernetCSO::CollectParametersForNewCSO(
	/*[out]*/ BSTR *pbstrNameOut, 
	/*[out]*/ VARIANT_BOOL *pvbHaveNewCSO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString csNameOut;

	bool bHaveNewCSO = true;// m_pcomm->CollectParametersForNewCSO(csNameOut);

	if (bHaveNewCSO)
	{
		*pvbHaveNewCSO = VARIANT_TRUE;
		const char *string_in = LPCTSTR(csNameOut);
		BSTR bstr_out;
		Convert_Character_String_to_BSTR((char *)string_in, bstr_out);
		*pbstrNameOut = bstr_out;
	}
	else
	{
		*pvbHaveNewCSO = VARIANT_FALSE;
		*pbstrNameOut = BSTR("");
	}
	return S_OK;
}

//2
STDMETHODIMP CEthernetCSO::EnableHeartbeat(/*[in]*/VARIANT_BOOL vbEnable)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->WatchDogOk(vbEnable == VARIANT_TRUE);

	return S_OK;
}

//3
STDMETHODIMP CEthernetCSO::get_CSOTypeString(BSTR *pbstrType)
{
//	TRACE("CEthernetCSO::get_CSOTypeString()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		char *string_in = NULL;//m_pcomm->GetType();
	Convert_Character_String_to_BSTR(string_in, *pbstrType);
	return S_OK;
}

//4
STDMETHODIMP CEthernetCSO::get_LastError(/*[out]*/ BSTR *pbstrErrorString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	BSTR bstr_out;
	CString *pcs = NULL;// m_pcomm->GetLastError();
	const char *pch = LPCTSTR(*pcs);
	Convert_Character_String_to_BSTR((char *)pch, bstr_out);
	pbstrErrorString = &bstr_out;

	return S_OK;
}

//5
STDMETHODIMP CEthernetCSO::get_Name(/*[out]*/ BSTR *pbstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		char *string_in = NULL;//m_pcomm->GetName();
	Convert_Character_String_to_BSTR(string_in, *pbstrName);

	return S_OK;
}

//6
STDMETHODIMP CEthernetCSO::get_VersionString(/*[out]*/ BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
		*pbstrVersion = NULL;// m_pcomm->m_csNameVersionDate.AllocSysString(); //SCR 118
	return S_OK;

	//CVersionInfo VersionInfo;
	//CString Name    = VersionInfo.InternalName();
	//CString Version = VersionInfo.FileVersion();
	//Version.Remove(' ');
	//Version.Replace(',','.');
	//Version.Format("%c.%c.%c.%c",Version[0],Version[3],Version[6],Version[9]);
	//CString cs = Name + " " + Version + " " + __DATE__;
	//Convert_CString_to_BSTR(cs,*pbstrVersion);
	//return S_OK;
}


//7
STDMETHODIMP CEthernetCSO::put_DialogAutoCloseTime(/*[in]*/ ULONG seconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	//m_pcomm->PutDialogCloseTime(1000*(int)seconds);
	return S_OK;
}

//8
STDMETHODIMP CEthernetCSO::put_INIPathname(/*[in]*/ BSTR bstrINIPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrINIPathname,string_out, sizeof(string_out));
	//m_pcomm->PutIniFileName(string_out);

	return S_OK;
}

//9
STDMETHODIMP CEthernetCSO::put_Name(/*[in]*/BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrName, string_out, sizeof(string_out));
	//strncpy(m_pcomm->m_pName,string_out,sizeof(m_pcomm->m_pName));

	return S_OK;
}

//10
STDMETHODIMP CEthernetCSO::put_WindowsRegisteredMessageStrings(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET)
{
//	TRACE("CEthernetCSO::put_WindowsRegisteredMessageStrings()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->SetWindowsRegisteredMessageStrings(ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET);

	//we need some of the unique message values locally at this level, too.
	uIDC_RECEIVE         = ulRECEIVE;
	uIDC_RECEIVE_OOB     = ulRECEIVE_OOB;
	uIDC_COM_START       = ulSTART;
	uIDC_COM_STOP        = ulSTOP;
	uIDC_COM_RESET       = ulRESET;

	return S_OK;
}

//11
STDMETHODIMP CEthernetCSO::put_ThresholdValue(/*[in]*/ ULONG threshold_value)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	// TODO: Add your implementation code here
	//m_pcomm->SetThresholdValue((UINT)threshold_value);
	return S_OK;
}

//12
STDMETHODIMP CEthernetCSO::RegisterISO(
			/*[in]*/ ULONG receiver_handle,
			/*[in]*/ ULONG Node,
			/*[in]*/ ISO_FUNCTIONAL_CATEGORY cat,
			/*[in]*/ VARIANT_BOOL vb_OOB, 
			/*[out]*/ VARIANT_BOOL *pvbResult)
{
//	TRACE("CEthernetCSO::RegisterISO\n");

	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	bool bresult = false;
	bool boutofbounds = (vb_OOB == VARIANT_TRUE);

	/*if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
	{
		bresult = m_pcomm->Register(
			(HWND)receiver_handle, 
			boutofbounds, 
			(int)Node);
	}
	else if (cat == WATCH_ISO)
	{
		bresult = m_pcomm->RegisterWatch(
			(HWND)receiver_handle,  
			(int)Node);
	}
	if (pvbResult != NULL)
		*pvbResult = bresult?VARIANT_TRUE:VARIANT_FALSE;*/

	return S_OK;
}

//13
STDMETHODIMP CEthernetCSO::ResetCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->PostThreadMessage(uIDC_COM_RESET,0,0);

	return S_OK;
}

//14
STDMETHODIMP CEthernetCSO::SendHeartbeat()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->WatchDogSend();

	return S_OK;
}

//15
STDMETHODIMP CEthernetCSO::SendToWatch(/*[in]*/ BSTR message, /*[in]*/ ULONG Node)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[128];
	Convert_BSTR_to_Character_String(message,string_out,sizeof(string_out));
	int bytes = strlen(string_out);  //<<QA>> is this checking really necessary??
	if (bytes > sizeof(string_out))
		bytes = sizeof(string_out);
	//m_pcomm->SendToWatch(bytes, string_out, int(Node));

	return S_OK;
}

//16
STDMETHODIMP CEthernetCSO::SetUpPort(
	/*[in]*/ BSTR portname,
	/*[out]*/ VARIANT_BOOL *pvbResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[32];
	Convert_BSTR_to_Character_String(portname,string_out,sizeof(string_out));
	bool success = true;// m_pcomm->SetUpPort(string_out);
	*pvbResult = success ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

//17
STDMETHODIMP CEthernetCSO::ShutdownCSO()
{
//	TRACE("CEthernetCSO::ShutdownCSO()\n");
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->DoStop();
	//m_pcomm->PostThreadMessage(uIDC_COM_STOP,0,0);

	return S_OK;
}

//18
STDMETHODIMP CEthernetCSO::StartCSO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// TODO: Add your implementation code here
	//m_pcomm->PostThreadMessage(uIDC_COM_START,0,0);

	return S_OK;
}

//19
STDMETHODIMP CEthernetCSO::Transmit(
			/*[in]*/ ULONG length,
			/*[in]*/ BLOB data, 
			/*[in]*/ ULONG ulnode, 
			/*[in]*/ VARIANT_BOOL vbQuiet, 
			/*[out]*/ ULONG *pBytesWritten)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		int bytes = 0;/* m_pcomm->Transmit(
		(int)length,                   //ilength (must allow for -1)
		(char*)data.pBlobData,         // Output 
		(int)ulnode,                   // Node 
		(vbQuiet == VARIANT_TRUE ));   // looking for a simple bool
		*/

	*pBytesWritten = bytes;

	return S_OK;
}

//20
STDMETHODIMP CEthernetCSO::UnRegisterISO(/*[in]*/ ULONG Node, /*[in]*/ ISO_FUNCTIONAL_CATEGORY cat)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	/*if ((cat == STANDARD_ISO) || (cat == UPS_ISO))
		m_pcomm->UnRegister((int)Node);
	else if (cat == WATCH_ISO)
		m_pcomm->UnRegisterWatch((int)Node);*/

	return S_OK;
}
