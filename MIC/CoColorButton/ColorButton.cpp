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
// ColorButton.cpp

#include "stdafx.h"
#include "CoColorButton.h"
#include "ColorButton.h"
#include "ccbutton_messages.h"
#include "afxconv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton


STDMETHODIMP CColorButton::get_FaceColor(/*[out]*/ ULONG *pRGBvalue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	LRESULT lr = ::SendMessage(m_hwnd, WM_GET_FACE_COLOR, 0, 0);
	*pRGBvalue = ULONG(lr);
	return S_OK;
}
STDMETHODIMP CColorButton::get_Numeric(/*[out]*/ ULONG *pNumeric)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	LRESULT lr = ::SendMessage(m_hwnd, WM_GET_NUMERIC, 0, 0);
	*pNumeric = ULONG(lr);
	return S_OK;
}
STDMETHODIMP CColorButton::put_DisplayNumericString(VARIANT_BOOL vb, ULONG uRadix)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd,WM_DISPLAY_NUMERIC_STRING, WPARAM(vb), (LPARAM)uRadix);
	return S_OK;
}
STDMETHODIMP CColorButton::put_FaceColor(/*[in]*/ ULONG RGBcolor)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_FACE_COLOR, WPARAM(RGBcolor), 0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_Flash(/*[in]*/ VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_FLASH, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_Numeric(/*[in]*/ ULONG newValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_PUT_NUMERIC, WPARAM(newValue),0);
	return S_OK;
}
STDMETHODIMP CColorButton::put_PreNumericString(/*[in]*/ BSTR bstr_in)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//<<QA>> see if this works.  see if this leaks.  If okay , then can
	//not include the COM_Utilities.cpp
	//USES_CONVERSION;
	//char *string_out = W2A(bstr_in);

	char string_out[16];
	Convert_BSTR_to_Character_String(bstr_in, string_out, sizeof(string_out));

	::SendMessage(m_hwnd, WM_PUT_NUMERIC_STRING, WPARAM(string_out),0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetDiskIcon_Floppy()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_DISK_ICON_FLOPPY, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetDiskIcon_Hourglass()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_DISK_ICON_HOURGLASS, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Bomb()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_BOMB, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Flag()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_FLAG, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetErrorIcon_Skull()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SET_ERROR_ICON_SKULL, 0,0);
	return S_OK;
}
STDMETHODIMP CColorButton::SetMICObjectHandle(/*[in]*/ ULONG handle)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	m_hwnd = (HWND)handle;
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Bell(/*[in]*/ VARIANT_BOOL vb)	//was SetBell(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_BELL, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Clock(/*[in]*/ VARIANT_BOOL vb)	//was SetTime(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_CLOCK, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Disk(/*[in]*/ VARIANT_BOOL vb)	//was SetDisk(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_DISK, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Error(/*[in]*/ VARIANT_BOOL vb)	//was SetFlag(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_ERROR, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Frown(/*[in]*/ VARIANT_BOOL vb)		//was SetOthr(bool)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_FROWN, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::ShowIcon_Square(/*[in]*/ VARIANT_BOOL vb)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	::SendMessage(m_hwnd, WM_SHOW_ICON_SQUARE, WPARAM(vb),0);
	return S_OK;
}
STDMETHODIMP CColorButton::get_VersionString(BSTR *pbstrVersion)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	//SCR 118
    DWORD   hInfo;
    DWORD   dwLen;
    char    name[_MAX_PATH];
	char*	VerData;
    CString sRet;
    unsigned int uSize;
    LPCTSTR lpBuffer2;
	LPCTSTR lpBuffer3;
	CString sSubBlockHead;

    struct 
	{
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    HMODULE hModule = AfxGetInstanceHandle();
    GetModuleFileName(hModule, name, sizeof(name));

    dwLen = GetFileVersionInfoSize(name, &hInfo);

    VerData = (char *)new char[dwLen];

    GetFileVersionInfo(name, hInfo,  dwLen, VerData);

    VerQueryValue(VerData,"\\VarFileInfo\\Translation",
            (LPVOID*)&lpBuffer,(unsigned int *) &dwLen);
      
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\", 
                    lpBuffer->first, lpBuffer->second);
    sSubBlockHead = name;


    CString sBlock = sSubBlockHead + "FileVersion"; 

    BOOL bRet = VerQueryValue(VerData, 
                    (char *)((LPCTSTR) sBlock), 
                    (void**)&lpBuffer2, &uSize);
     
	sBlock = sSubBlockHead + "InternalName";
	bRet = VerQueryValue(VerData,
		(char*)((LPCSTR) sBlock),
		(void**)&lpBuffer3, &uSize);


	CString csNameVersionDate;

	csNameVersionDate = lpBuffer3;
	csNameVersionDate += "\t";
	csNameVersionDate += lpBuffer2;
	csNameVersionDate.Replace(", ",".");
	csNameVersionDate += "\t";
	csNameVersionDate += __DATE__;

	Convert_CString_to_BSTR(csNameVersionDate,*pbstrVersion);

	delete VerData;

	return S_OK;
}
