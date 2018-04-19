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
// DirectSerialCSO.h

#ifndef __DIRECTSERIALCSO_H_
#define __DIRECTSERIALCSO_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"
#include "ComGrand.h"

/////////////////////////////////////////////////////////////////////////////
// CDirectSerialCSO
class ATL_NO_VTABLE CDirectSerialCSO : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDirectSerialCSO, &CLSID_DirectSerialCSO>,
	public IDispatchImpl<ICSO, &IID_ICSO, &LIBID_CODIRECTSERIALCSOLib>
{
public:
	CDirectSerialCSO()
	{
//		TRACE("CDirectSerialCSO()::CTOR\n");
		m_pcomm = (CComGrand*)AfxBeginThread(RUNTIME_CLASS( CComGrand ));
	}
	~CDirectSerialCSO()
	{
//		TRACE("CDirectSerialCSO::DTOR - NOOP\n");
	}

	CComGrand *m_pcomm;

DECLARE_REGISTRY_RESOURCEID(IDR_DIRECTSERIALCSO)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CDirectSerialCSO)
	COM_INTERFACE_ENTRY(ICSO)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICSO
public:

	STDMETHOD(CollectParametersForNewCSO)(
		/*[out]*/ BSTR *nameout,
		/*[out]*/ VARIANT_BOOL *pvbHaveNewCSO );

	STDMETHOD(EnableHeartbeat)(/*[in]*/VARIANT_BOOL bEnable);
	STDMETHOD(get_CSOTypeString)(/*[out]*/ BSTR *pType);
	STDMETHOD(get_LastError)(/*[out]*/ BSTR *errorstring);
	STDMETHOD(get_Name)(/*[out]*/ BSTR *pName);
	STDMETHOD(get_VersionString)(/*[out]*/ BSTR *pName);
	STDMETHOD(put_DialogAutoCloseTime)(/*[in]*/ ULONG seconds);
	STDMETHOD(put_INIPathname)(/*[in]*/ BSTR pathname);
	STDMETHOD(put_Name)(/*[in]*/BSTR pathname);

	STDMETHOD(put_WindowsRegisteredMessageStrings)(
			/*[in]*/ ULONG ulRECEIVE,
			/*[in]*/ ULONG ulRECEIVE_OOB,
			/*[in]*/ ULONG ulSTART,
			/*[in]*/ ULONG ulSTOP,
			/*[in]*/ ULONG ulRESET);

	STDMETHOD(put_ThresholdValue)(/*[in]*/ ULONG threshold_value);

	STDMETHOD(RegisterISO)(
			/*[in]*/ ULONG receiver_handle,
			/*[in]*/ ULONG Node,
			/*[in]*/ ISO_FUNCTIONAL_CATEGORY cat,
			/*[in]*/ VARIANT_BOOL vb_OOB, 
			/*[out]*/ VARIANT_BOOL *pvbResult);

	STDMETHOD(ResetCSO)();
	STDMETHOD(SendHeartbeat)();
	STDMETHOD(SendToWatch)(/*[in]*/ BSTR message, /*[in]*/ ULONG Node);
	STDMETHOD(SetUpPort)(/*[in]*/ BSTR portname, /*[out]*/ VARIANT_BOOL *pvbResult);
	STDMETHOD(ShutdownCSO)();
	STDMETHOD(StartCSO)();

	STDMETHOD(Transmit)(
			/*[in]*/ ULONG length,
			/*[in]*/ BLOB data, 
			/*[in]*/ ULONG node, 
			/*[in]*/ VARIANT_BOOL vbQuiet, 
			/*[out]*/ ULONG *pBytesWritten);

	STDMETHOD(UnRegisterISO)(/*[in]*/ ULONG Node, /*[in]*/ ISO_FUNCTIONAL_CATEGORY cat);
};

#endif //__GENERICCSO_H_
