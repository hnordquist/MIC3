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
// RegisterByCategory.cpp

#include "stdafx.h"

#include "RegisterByCategory.h"
#include "../Interfaces/CATIDS.h"  //Category guids

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////////
//
//                 RegisterThisComponentWithItsCategory
//
///////////////////////////////////////////////////////////////////////////////// 
HRESULT RegisterThisComponentWithItsCategory(
		REGISTRATION_CATEGORY cat, 
		GUID myCLSID)
{
	bool bCategoryRegistered;

	HRESULT hr;

	if (FAILED(hr = Initialize()))
		return hr;
	if (FAILED(hr = TestRegistrationOfCategory(cat, &bCategoryRegistered)))
		return hr;

	if (!bCategoryRegistered)
	{
		if (FAILED(hr = RegisterCategory(cat,true)))
			return hr;
	}

	return RegisterWithPreExistingCategory(cat,myCLSID,true);
}
/////////////////////////////////////////////////////////////////////////////////
//
//                 UnRegisterThisComponentFromItsCategory
//
///////////////////////////////////////////////////////////////////////////////// 
HRESULT UnRegisterThisComponentFromItsCategory(		
	REGISTRATION_CATEGORY cat, 
	GUID myCLSID)
{
	HRESULT hr;

	if (FAILED(hr = Initialize()))
		return hr;

	return RegisterWithPreExistingCategory(cat,myCLSID,false);
}
/////////////////////////////////////////////////////////////////////////////
//
//                  Initialize()
//
/////////////////////////////////////////////////////////////////////////////
HRESULT Initialize(void)
{
	g_pICatInformation = NULL;
	g_pICatRegister = NULL;

	// Initialize COM Library.
	OleInitialize(NULL) ;

	// Create the standard COM Category manager.
	HRESULT hr = ::CoCreateInstance(CLSID_StdComponentCategoriesMgr,
	                                NULL, 
	                                CLSCTX_ALL,
	                                IID_ICatInformation, 
	                                (void**)&g_pICatInformation) ;

	if (FAILED(hr))
	{
		AfxMessageBox("Could not create component category manager.", hr) ;
		// Unintialize COM Library
		OleUninitialize();
	}

	// Get the Category Registration interface.
	hr = g_pICatInformation->QueryInterface(
		IID_ICatRegister,
	    (void**)&g_pICatRegister);

	if (FAILED(hr))
	{
		AfxMessageBox("Attempt to query for ICatRegister failed.");
		OleUninitialize();
	}
	return hr;
}
/////////////////////////////////////////////////////////////////////////////
//
//                  TestRegistrationOfCategory()
//
/////////////////////////////////////////////////////////////////////////////
HRESULT TestRegistrationOfCategory(
	REGISTRATION_CATEGORY cat, 
	bool *psuccess)
{
	///////////////////////////////////////////////////////////////////////
	//See if our categories are registered
	///////////////////////////////////////////////////////////////////////
	// Get an enumerator for the categories.
	IEnumCATEGORYINFO* pIEnumCATEGORYINFO = NULL ;
	
	HRESULT hr = g_pICatInformation->EnumCategories(
		::GetUserDefaultLCID(),
		&pIEnumCATEGORYINFO) ;

	if (FAILED(hr))
	{
		AfxMessageBox("ListCategories: ICatInformation::EnumCategories failed.");
		return hr;
	}

	char szDescription[128] ;
	CATEGORYINFO CategoryInfo ;

	bool bThisCategoryIsRegistered = false;

	// Enumerate the categories.
	while ((hr = pIEnumCATEGORYINFO->Next(1, &CategoryInfo, NULL)) == S_OK)
	{
		// Convert from wchar_t to char.
		::wcstombs(
			szDescription, 
			CategoryInfo.
			szDescription,
	        sizeof(szDescription));

			if (cat == MIC_ISO)
			{
				if (0 == strcmp(szDescription,MIC_ISO_CATID_FRIENDLY_NAME))
					bThisCategoryIsRegistered = true;
			}
			else if (cat == MIC_CSO)
			{
				if (0 == strcmp(szDescription,MIC_CSO_CATID_FRIENDLY_NAME))
					bThisCategoryIsRegistered = true;
			}

			if (bThisCategoryIsRegistered)
				break;
	} 

	*psuccess = bThisCategoryIsRegistered;

	// Release Interfaces.
	if (pIEnumCATEGORYINFO != NULL)
		pIEnumCATEGORYINFO->Release();

	return S_OK;
}
/////////////////////////////////////////////////////////////////////
//
//                   RegisterCategory
//
/////////////////////////////////////////////////////////////////////
HRESULT RegisterCategory(REGISTRATION_CATEGORY cat,bool doit)
{
	HRESULT hr;
	CATEGORYINFO category_info;
	size_t numchars;
	GUID CATID_ThisCategory[1];

	if (cat == MIC_ISO)
	{
		category_info.catid = MIC_ISO_CATID;
		category_info.lcid = LOCALE_SYSTEM_DEFAULT;
		numchars = sizeof(MIC_ISO_CATID_FRIENDLY_NAME);
		::mbstowcs(category_info.szDescription,MIC_ISO_CATID_FRIENDLY_NAME,numchars);
		CATID_ThisCategory[0] = MIC_ISO_CATID;
	}
	else if (cat == MIC_CSO)
	{
		category_info.catid = MIC_CSO_CATID;
		category_info.lcid = LOCALE_SYSTEM_DEFAULT;
		numchars = sizeof(MIC_CSO_CATID_FRIENDLY_NAME);
		::mbstowcs(category_info.szDescription,MIC_CSO_CATID_FRIENDLY_NAME,numchars);
		CATID_ThisCategory[0] = MIC_CSO_CATID;
	}
	else
	{
		hr = S_FALSE;
		return hr;
	}

	if (doit)
	{
		hr = g_pICatRegister->RegisterCategories(1, &category_info) ;
		if (FAILED(hr))
			AfxMessageBox("RegisterCategory: Registering the component category failed.",hr) ;
	}
	else
	{
		hr = g_pICatRegister->UnRegisterCategories(1, CATID_ThisCategory) ;
		if (FAILED(hr))
			AfxMessageBox("UnregisterCategory: Unregistering component category failed.") ;
	}

	return hr;
}
/////////////////////////////////////////////////////////////////////
//
//             RegisterWithPreExistingCategory
//
/////////////////////////////////////////////////////////////////////
HRESULT RegisterWithPreExistingCategory(
		REGISTRATION_CATEGORY cat,
		GUID myCLSID,
		bool doit)
{
	GUID CATID_ThisCategory[1];  // a one-element array

	HRESULT hr;

	if (cat == MIC_CSO)
		CATID_ThisCategory[0] = MIC_CSO_CATID;
	else if (cat == MIC_ISO)
		CATID_ThisCategory[0] = MIC_ISO_CATID; 
	else	
		return S_FALSE;

	if (doit)  //this is the register part
	{
		hr = g_pICatRegister->RegisterClassImplCategories(
				myCLSID,//guid,
				1,
				CATID_ThisCategory); 

		if (FAILED(hr))
		{
			AfxMessageBox("AddComponent: Adding component to category failed.", hr);
		}
	}
	else  //this is the unregister part
	{
		g_pICatRegister->UnRegisterClassImplCategories(
				myCLSID,//guid,
				1,
				CATID_ThisCategory); 

		hr = S_OK;  //don't care if unregister fails.
	}

	return hr;
}