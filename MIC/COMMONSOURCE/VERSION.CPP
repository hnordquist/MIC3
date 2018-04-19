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
// version.cpp
 
#include <stdafx.h>
#include "Version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
     
CVersionInfo::CVersionInfo(): m_VerData(NULL)
{
    DWORD   hInfo;
    DWORD   dwLen;
	char name[_MAX_PATH];
    struct 
	{
        WORD    first;
        WORD    second;
    } *lpBuffer;
     
    HMODULE hModule = AfxGetInstanceHandle();
    GetModuleFileName(hModule, m_name, sizeof(m_name));

    dwLen = GetFileVersionInfoSize(m_name, &hInfo);

    m_VerData = (void *)new char[dwLen];

    GetFileVersionInfo(m_name, hInfo,  dwLen, m_VerData);

    VerQueryValue(m_VerData,"\\VarFileInfo\\Translation",
            (LPVOID*)&lpBuffer,(unsigned int *) &dwLen);
      
    wsprintf(name, "\\StringFileInfo\\%04x%04x\\", 
                    lpBuffer->first, lpBuffer->second);
    m_sSubBlockHead = name;
}

CVersionInfo::~CVersionInfo()
{
    delete m_VerData;
}
     
CString CVersionInfo::GetValue(CString const sSubBlock)
{
    CString sRet;
    unsigned int uSize;
    LPCTSTR lpBuffer;

    CString sBlock = m_sSubBlockHead + sSubBlock; 

    BOOL bRet = VerQueryValue(m_VerData, 
                    (char *)((LPCTSTR) sBlock), 
                    (void**)&lpBuffer, &uSize);
     
    if (bRet)
        sRet = lpBuffer;
     
    return sRet;
}