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
// WatchISO.h

#ifndef __WATCHISO_H_
#define __WATCHISO_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"
//#include "../Interfaces/ICSO.h"
//#include "../Interfaces/ICB.h"
#include "testinst.h"

/////////////////////////////////////////////////////////////////////////////
// CWatchISO
class ATL_NO_VTABLE CWatchISO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWatchISO, &CLSID_WatchISO>,
	public IDispatchImpl<IISO, &IID_IISO, &LIBID_COWATCHISOLib>
{
public:
	CWatchISO()
	{
	}
	~CWatchISO()
	{
	}

	CTestInst m_inst;

DECLARE_REGISTRY_RESOURCEID(IDR_WATCHISO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CWatchISO)
	COM_INTERFACE_ENTRY(IISO)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IISO
public:

	STDMETHOD(AllowWriteToDisk)(/*[in]*/ VARIANT_BOOL vb);
	STDMETHOD(AppendGUIValues)(/*[in]*/ BSTR bstrfilename);

	STDMETHOD(CollectParametersForNewISO)(
		/*[out]*/ BSTR *pbstrNameOut, 
		/*[out]*/ VARIANT_BOOL *pvbCancelled);

	STDMETHOD(CreateISO)(/*[out]*/ VARIANT_BOOL *pvbSuccess);

	STDMETHOD(DriveBBMToZero)(/*[in]*/ VARIANT_BOOL vbCommand, /*[in]*/ VARIANT_BOOL vbInPowerFail);

	STDMETHOD(EstablishButtonKeeperCommunicationsPath)(
			/*[in]*/ ULONG ulButtonKeeperHandle,
			/*[in]*/ ULONG ulCheckButtonRegisteredMessageValue);

	STDMETHOD(EstablishSpecialUPSCommunicationsPath)(
			/*[in]*/ ULONG ulSendToHandle, 
			/*[in]*/ ULONG ulQueryAllPausedRegisteredMessageValue,
			/*[in]*/ ULONG ulDriveAllToZeroRegisteredMessageValue);

	STDMETHOD(EstablishValidationCommunicationsPath)(
			/*[in]*/ ULONG ulValidationHandle, 
			/*[in]*/ ULONG ulValidationRegisteredMessageValue);

	STDMETHOD(get_CurrentBBM)(/*[out]*/ ULONG *pCurrentBBM);
	STDMETHOD(get_ID)(/*[out]*/ BSTR *pbstrID);
	STDMETHOD(get_InShutDown)(/*[out]*/ VARIANT_BOOL *pvbInShutDown);
	STDMETHOD(get_InStartUp)(/*[out]*/ VARIANT_BOOL *pvbInStartUp);
	STDMETHOD(get_ISOCategory)(/*[out]*/ ULONG *pCategory);
	STDMETHOD(get_ISOTypeString)(/*[out]*/ BSTR *pTypeString);
	STDMETHOD(get_IsPaused)(/*[out]*/ VARIANT_BOOL *pvbIsPaused);
	STDMETHOD(get_FullName)(/*[out]*/ BSTR *pFullName);
	STDMETHOD(get_LastError)(/*[out]*/ BSTR *pbstrErrorString);

	STDMETHOD(get_LocationAndExtensionsForDataFiles)(
		BSTR *pbstrLocation,
		BSTR *pbstrExtensions);

	STDMETHOD(get_Name)(/*[out]*/ BSTR *pName);
	STDMETHOD(get_VersionString)(/*[out]*/ BSTR *pbstrVersion);
	STDMETHOD(PauseInstrument)(/*[in]*/ VARIANT_BOOL vbPauseCommand);
	STDMETHOD(put_DialogAutoCloseTime)(/*[in]*/ ULONG seconds);
	STDMETHOD(put_ICBPointer)(/*[in]*/IUnknown* pICB);
	STDMETHOD(put_ICSOPointer)(/*[in]*/IUnknown* pICSO);
	STDMETHOD(put_INIPathname)(/*[in]*/ BSTR pathname);
	STDMETHOD(put_LongNameFormatString)(/*[in]*/ BSTR bstrFormatString);
	STDMETHOD(put_MaximumRetryRepetitions)(/*[in]*/ ULONG uNumberRepetitions);
	STDMETHOD(put_Name)(/*[in]*/BSTR bstrName);
	STDMETHOD(put_NormalShutdown)(VARIANT_BOOL vbNormalShutdown);
	STDMETHOD(put_UTILPathname)(/*[in]*/ BSTR bstrUTILPathname);

	STDMETHOD(put_WindowsRegisteredMessageStrings)(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET);

	STDMETHOD(ResetISO)();
	STDMETHOD(ShowISOGUI)(/*[in]*/ VARIANT_BOOL vbCommand);
	STDMETHOD(ShutdownISO)();
	STDMETHOD(UPSGoDumb)();
	STDMETHOD(UseShortFilename)(/*[in]*/ VARIANT_BOOL vbCommand);
};

#endif //__WATCHISO_H_
