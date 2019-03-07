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
// COM_Utilities.cpp

#include "stdafx.h"
#include "COM_Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int Convert_BSTR_to_CString(
	BSTR& bstr_in, 
	CString& cs_out)
{
	UINT length = 0;

	if (bstr_in != NULL)
		length = SysStringLen(bstr_in);

	if (length != 0)
	{
		UINT count = 1 + SysStringLen(bstr_in);
		char *mbstr = new char[count * sizeof(char)];
		wcstombs(mbstr,bstr_in,count);
		cs_out = mbstr;
		delete [] mbstr;
	}
	else
	{
		cs_out.Empty();
	}

	SysFreeString(bstr_in);

	return (int)length;
}

int Convert_BSTR_to_Character_String(
	BSTR& bstr_in, 
	char *string_out, 
	int maxchars)
{
	UINT length = 0;

	if (bstr_in != NULL)
		length = SysStringLen(bstr_in);

	if (length != 0)
	{
		UINT count = 1 + length;
		char *mbstr = new char[count * sizeof(char)];
		wcstombs(mbstr,bstr_in,count);
		strncpy(string_out,mbstr,maxchars);
		delete [] mbstr;
	}
	else
	{
		string_out[0] = 0;
	}

	SysFreeString(bstr_in);

	return (int)strlen(string_out);
}


int Convert_Character_String_to_BSTR(char *string_in, BSTR& bstr_out)
{
	//end user must free the BSTR objects with SysFreeString(bstr);

	UINT length = 0;

	if (string_in != NULL)
		length = mbstowcs(NULL,string_in,MB_CUR_MAX);

	if (length != 0)
	{
		int count = 1 + length;//mbstowcs(NULL,string_in,MB_CUR_MAX);
		wchar_t *pwcharName = new wchar_t[count * sizeof(wchar_t)];
		mbstowcs(pwcharName,string_in,count);
		bstr_out = SysAllocString(pwcharName);
		delete [] pwcharName;
	}
	else
	{
		wchar_t *pwchar = NULL;
		bstr_out = SysAllocString(pwchar);
	}
	
	return (int)length;
}

int Convert_CString_to_BSTR(CString& cstring_in, BSTR& bstr_out)
{
	//end user must free the BSTR objects with SysFreeString(bstr);

	int length = 0;

	if (!cstring_in.IsEmpty())
	{
		const char *mbstr1 = (LPCTSTR)(cstring_in);
		int count = 1 + mbstowcs(NULL,mbstr1,MB_CUR_MAX);
		wchar_t *wcstr = new wchar_t[count * sizeof(wchar_t)];
		mbstowcs(wcstr,mbstr1,count);
		bstr_out = SysAllocString(wcstr);
		delete[] wcstr;
		length = (int)SysStringLen(bstr_out); 
	}
	else
	{
		wchar_t *pwchar = NULL;
		bstr_out = SysAllocString(pwchar);
	}

	return length;
}