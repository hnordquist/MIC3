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
// GrandISO.cpp

#include "stdafx.h"
#include "CoGrandISO.h"
#include "GrandISO.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrandISO

//1
STDMETHODIMP CGrandISO::AllowWriteToDisk(VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.BlockWrite(vb == VARIANT_TRUE);
	return S_OK;
}

//2
STDMETHODIMP CGrandISO::AppendGUIValues(BSTR bstrfilename)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	char szFilename[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrfilename, szFilename, sizeof(szFilename));

	FILE *outfile = fopen(szFilename,"at");
	m_inst.IssuePrint(outfile);
	fclose(outfile);

	return S_OK;
}

//3
STDMETHODIMP CGrandISO::CollectParametersForNewISO(
	BSTR *pbstrNameOut,
	VARIANT_BOOL *pvbHaveNewISO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CString csNameOut;

	bool bHaveNewISO = m_inst.CollectParametersForNewISO(csNameOut);

	if (bHaveNewISO)
	{
		*pvbHaveNewISO = VARIANT_TRUE;
		const char *string_in = LPCTSTR(csNameOut);
		BSTR bstr_out;
		Convert_Character_String_to_BSTR((char *)string_in, bstr_out);
		*pbstrNameOut = bstr_out;
	}
	else
	{
		*pvbHaveNewISO = VARIANT_FALSE;
		*pbstrNameOut = BSTR("No input");
	}

	return S_OK;
}

//4
STDMETHODIMP CGrandISO::CreateISO(VARIANT_BOOL *pvbSuccess)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool bresult = m_inst.CreateInstrument();
	*pvbSuccess = bresult?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}


//5
STDMETHODIMP CGrandISO::DriveBBMToZero(
	/*[in]*/ VARIANT_BOOL vbCommand, 
	/*[in]*/ VARIANT_BOOL vbInPowerFail)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.DriveToZero(
		vbCommand == VARIANT_TRUE, 
		vbInPowerFail == VARIANT_TRUE);
	return S_OK;
}

//6
STDMETHODIMP CGrandISO::EstablishButtonKeeperCommunicationsPath(
			/*[in]*/ ULONG ulButtonKeeperHandle, 
			/*[in]*/ ULONG ulCheckButtonRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.EstablishButtonKeeperCommunicationsPath(
		(HWND)ulButtonKeeperHandle, 
		(UINT)ulCheckButtonRegisteredMessageValue);

	return S_OK;
}

//7
STDMETHODIMP CGrandISO::EstablishSpecialUPSCommunicationsPath(
			/*[in]*/ ULONG ulRecipientHandle, 
			/*[in]*/ ULONG ulQueryAllPausedRegisteredMessageValue,
			/*[in]*/ ULONG ulDriveAllToZeroRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.EstablishUPSCommunicationsPath(
		(HWND)ulRecipientHandle, 
		(UINT)ulQueryAllPausedRegisteredMessageValue,
		(UINT)ulDriveAllToZeroRegisteredMessageValue);
	return S_OK;
}

//8
STDMETHODIMP CGrandISO::EstablishValidationCommunicationsPath(
			ULONG ulValidationHandle, 
			ULONG ulValidationRegisteredMessageValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
  	m_inst.EstablishValidationCommunicationsPath(
		(HWND)ulValidationHandle, 
		(UINT)ulValidationRegisteredMessageValue);
	return S_OK;
}

//9
STDMETHODIMP CGrandISO::get_CurrentBBM(ULONG *pCurrentBBM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pCurrentBBM = (ULONG)m_inst.GetCurrentBBM();
	return S_OK;
}

//10
STDMETHODIMP CGrandISO::get_ID(BSTR *pbstrID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *id = m_inst.GetID();
	Convert_Character_String_to_BSTR(id,*pbstrID);
	return S_OK;
}


//11
STDMETHODIMP CGrandISO::get_InShutDown(VARIANT_BOOL *pvbInShutDown)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.InShutDown();
	*pvbInShutDown = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//12
STDMETHODIMP CGrandISO::get_InStartUp(VARIANT_BOOL *pvbInStartUp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.InStartUp();
	*pvbInStartUp = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//13
STDMETHODIMP CGrandISO::get_ISOCategory(ULONG *pCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	*pCategory = (ULONG)m_inst.GetISOCategory();
	return S_OK;
}

//14
STDMETHODIMP CGrandISO::get_ISOTypeString(BSTR *pbstrTypeString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *type = m_inst.GetType();
	Convert_Character_String_to_BSTR(type,*pbstrTypeString);
	return S_OK;
}

//15
STDMETHODIMP CGrandISO::get_IsPaused(VARIANT_BOOL *pvbIsPaused)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	bool b = m_inst.IsPaused();
	*pvbIsPaused = b?VARIANT_TRUE:VARIANT_FALSE;
	return S_OK;
}

//16
STDMETHODIMP CGrandISO::get_FullName(BSTR *pbstrFullName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *fullname = m_inst.GetFullName();
	Convert_Character_String_to_BSTR(fullname,*pbstrFullName);
	return S_OK;
}

//17
STDMETHODIMP CGrandISO::get_LastError(BSTR *pbstrErrorString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CString *pcs = m_inst.GetLastError();
	Convert_CString_to_BSTR(*pcs,*pbstrErrorString);
	return S_OK;
}

//18
STDMETHODIMP CGrandISO::get_LocationAndExtensionsForDataFiles(
	BSTR *pbstrLocation,
	BSTR *pbstrExtensions)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *string_in = m_inst.GetSaveLoc();
	Convert_Character_String_to_BSTR(string_in,*pbstrLocation);
	string_in = m_inst.GetFileExtensionsList();
	Convert_Character_String_to_BSTR(string_in,*pbstrExtensions);
	return S_OK;
}

//19
STDMETHODIMP CGrandISO::get_Name(BSTR *pbstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char *name = m_inst.GetName();
	Convert_Character_String_to_BSTR(name,*pbstrName);
	return S_OK;
}

//20
STDMETHODIMP CGrandISO::get_VersionString(BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	//CVersionInfo VersionInfo;
	*pbstrVersion = m_inst.m_csNameVersionDate.AllocSysString(); //SCR 118
	return S_OK;
}

//21
STDMETHODIMP CGrandISO::PauseInstrument(VARIANT_BOOL vbPauseCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PauseInstrument(vbPauseCommand == VARIANT_TRUE);
	return S_OK;
}

//22
STDMETHODIMP CGrandISO::put_DialogAutoCloseTime(ULONG seconds)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutDialogAutoCloseTime(1000*(int)seconds);
	return S_OK;
}

//23
STDMETHODIMP CGrandISO::put_ICBPointer(IUnknown* pICB)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutICBPointer((ICB *)pICB);
	return S_OK;
}

//24
STDMETHODIMP CGrandISO::put_ICSOPointer(IUnknown* pICSO)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutICSOPointer((ICSO*)pICSO);
	return S_OK;
}

//25
STDMETHODIMP CGrandISO::put_INIPathname(BSTR bstrINIPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrINIPathname,string_out,sizeof(string_out));
	m_inst.PutINIPathname(string_out);
	return S_OK;
}

//26
STDMETHODIMP CGrandISO::put_LongNameFormatString(BSTR bstrFormatString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[128];
	Convert_BSTR_to_Character_String(
		bstrFormatString,string_out, sizeof(string_out));
	CString cs = string_out;
	m_inst.PutLongNameFormatString(cs);
	return S_OK;
}

//27
STDMETHODIMP CGrandISO::put_MaximumRetryRepetitions(ULONG uNumberRepetitions)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutMaxRepetition((int)uNumberRepetitions);
	return S_OK;
}

//28
STDMETHODIMP CGrandISO::put_Name(BSTR bstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(bstrName, string_out,sizeof(string_out));
	m_inst.PutName(string_out);
	return S_OK;
}

//29
STDMETHODIMP CGrandISO::put_NormalShutdown(VARIANT_BOOL vbNormalShutdown)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutNormalShutdown(VARIANT_TRUE == vbNormalShutdown);
	return S_OK;
}

//30
STDMETHODIMP CGrandISO::put_UTILPathname(BSTR bstrUTILPathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	char string_out[MAX_PATH];
	Convert_BSTR_to_Character_String(
		bstrUTILPathname,string_out,sizeof(string_out));
	m_inst.PutUTILPathname(string_out);
	return S_OK;
}

//31
STDMETHODIMP CGrandISO::put_WindowsRegisteredMessageStrings(
	/*[in]*/ ULONG ulRECEIVE,
	/*[in]*/ ULONG ulRECEIVE_OOB,
	/*[in]*/ ULONG ulSTART,
	/*[in]*/ ULONG ulSTOP,
	/*[in]*/ ULONG ulRESET)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.SetWindowsRegisteredMessageStrings(
		ulRECEIVE,
		ulRECEIVE_OOB,
		ulSTART,
		ulSTOP,
		ulRESET);
	return S_OK;
}

//32
STDMETHODIMP CGrandISO::ResetISO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.Reset();
	return S_OK;
}

//33
STDMETHODIMP CGrandISO::ShowISOGUI(VARIANT_BOOL vbCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.ShowInstrument(vbCommand == VARIANT_TRUE);
	return S_OK;
}

//34
STDMETHODIMP CGrandISO::ShutdownISO()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.UnRegInstrument();
	return S_OK;
}

//35
STDMETHODIMP CGrandISO::UPSGoDumb()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.GoDumb();
	return S_OK;
}

//36
STDMETHODIMP CGrandISO::UseShortFilename(VARIANT_BOOL vbCommand)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_inst.PutUseShortFilename(vbCommand == VARIANT_TRUE);
	return S_OK;
}
