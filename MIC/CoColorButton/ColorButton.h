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
// ColorButton.h

#ifndef __COLORBUTTON_H_
#define __COLORBUTTON_H_

#include "resource.h"       // main symbols
#include "COM_Utilities.h"

/////////////////////////////////////////////////////////////////////////////
// CColorButton
class ATL_NO_VTABLE CColorButton : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColorButton, &CLSID_ColorButton>,
	public IDispatchImpl<ICB, &IID_ICB, &LIBID_COCOLORBUTTONLib>
{
public:
	CColorButton()
	{
	}

	HWND m_hwnd;  //the handle for MIC's ccbutton object

DECLARE_REGISTRY_RESOURCEID(IDR_COLORBUTTON)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CColorButton)
	COM_INTERFACE_ENTRY(ICB)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// ICB
public:

	STDMETHOD(get_FaceColor)(/*[out]*/ ULONG *pRGBvalue);
	STDMETHOD(get_Numeric)(/*[out]*/ ULONG *numeric);
	STDMETHOD(put_DisplayNumericString)(/*[in]*/ VARIANT_BOOL vb, /*[in]*/ ULONG uRadix);
	STDMETHOD(put_FaceColor)(/*[in]*/ ULONG RGBcolor);
	STDMETHOD(put_Flash)(/*[in]*/ VARIANT_BOOL vb);
	STDMETHOD(put_Numeric)(/*[in]*/ ULONG newValue);
	STDMETHOD(put_PreNumericString)(/*[in]*/ BSTR string);
	STDMETHOD(SetDiskIcon_Floppy)();
	STDMETHOD(SetDiskIcon_Hourglass)();
	STDMETHOD(SetErrorIcon_Bomb)();
	STDMETHOD(SetErrorIcon_Flag)();
	STDMETHOD(SetErrorIcon_Skull)();
	STDMETHOD(SetMICObjectHandle)(/*[in]*/ ULONG handle);
	STDMETHOD(ShowIcon_Bell)(/*[in]*/ VARIANT_BOOL vb);			//was SetBell(bool)
	STDMETHOD(ShowIcon_Clock)(/*[in]*/ VARIANT_BOOL vb);		//was SetTime(bool)
	STDMETHOD(ShowIcon_Disk)(/*[in]*/ VARIANT_BOOL vb);			//was SetDisk(bool)
	STDMETHOD(ShowIcon_Error)(/*[in]*/ VARIANT_BOOL vb);		//was SetFlag(bool)
	STDMETHOD(ShowIcon_Frown)(/*[in]*/ VARIANT_BOOL vb);		//was SetOthr(bool)
	STDMETHOD(ShowIcon_Square)(/*[in]*/ VARIANT_BOOL vb);
	STDMETHOD(get_VersionString)(/*[out]*/BSTR *pbstrVersion);
};

#endif //__COLORBUTTON_H_
