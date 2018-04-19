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
#pragma once

#include <afxtempl.h>
typedef struct InitialStatusData
{
	typedef enum {eUnknown, eISO, eCSO, eWatch, eHiddenWatch, eCOMCategory4, eCOMCategory5, eMaxCOMCategory} COMCategory;

	// this preserves enough information to generate a unique hash key and give the user some useful diagnostic messages.
	COMCategory Type;
	INT Node;
	DWORD StatusTickTime;

	CString Name, Version, Err;
	CString Port;//, Node; //StatusMessages;
	CString kHashKey;

	InitialStatusData()
	{
		Type = eUnknown;
		Node = -1;
		StatusTickTime = ::GetTickCount();
	}

	void SetHashKey()
	{
		kHashKey.Format("%s %s %s %lu",COMCategoryString[Type], Name, Version, StatusTickTime);
	}

	void PrepareCSOStatus(TCHAR* name, BSTR version, BSTR err, TCHAR* port)
	{
		Name = name;
		Port = port;
		//Node = -1;
		Version = version;
		Err = err;
		Type = eCSO;
		SetHashKey ();
	}

	void PrepareISOStatus(TCHAR* name, BSTR version, BSTR err, int node)
	{
		Name = name;
		Err = err;
		Version = version;
		Type = eISO;
		Node = node;
		SetHashKey ();
	}

	static const TCHAR* COMCategoryString[eMaxCOMCategory];
} InitialStatusDataStruct;

const TCHAR* InitialStatusData::COMCategoryString[InitialStatusDataStruct::eMaxCOMCategory] = {"Unknown", "ISO", "CSO", "Watch", "Hidden Watch", "COMCategory4", "COMCategory5"};


class CCOMStatusMap : public CTypedPtrMap<CMapStringToPtr, CString, InitialStatusDataStruct*> 
{

	public:

	CCOMStatusMap() {}
	~CCOMStatusMap() {}

	InitialStatusDataStruct* GetNewCSO(TCHAR* name, BSTR version, BSTR err)
	{
		CHAR port[256];
		InitialStatusDataStruct* p = new InitialStatusDataStruct();
		LPSTR f = port;
		GetPrivateProfileString(name, PORT,"PORT_ERR", f, sizeof(port), g_szIniFile);
		p->PrepareCSOStatus(name, version, err, port);
		SetAt(p->kHashKey, p);
		return p;
	}

	InitialStatusDataStruct* GetNewISO(TCHAR* name, BSTR version, BSTR err)
	{
		InitialStatusDataStruct* p = new InitialStatusDataStruct();
		int iInstNode = GetPrivateProfileInt(name,NODE,-1,g_szIniFile);
		p->PrepareISOStatus(name, version, err, iInstNode);
		SetAt(p->kHashKey, p);
		return p;
	}

	void PrepareNoticeStrings(CString& CSOs, CString& ISOs)
	{
		POSITION pos = GetStartPosition();
		while(pos != NULL)
		try
		{
			InitialStatusDataStruct* p;
			CString hk, temp;
			// Gets key (hk) and value (p)
			GetNextAssoc(pos, hk, p);

			if (p->Type == InitialStatusDataStruct::eCSO)
			{
				temp.Format("CSO \'%s\' assigned to port %s\r\n   %16s", p->Name, p->Port, p->Version);
				if (CSOs.GetLength() < 1)
					CSOs = "\r\nCSOs with errors:\r\n";
				CSOs.Append("\r\n");
				CSOs.Append(temp);
				CSOs.Append("\r\n");
			}
			else if (p->Type == InitialStatusDataStruct::eISO)
			{
				temp.Format("ISO \'%s\' at node %d, -- see other message box", p->Name, p->Node);
				if (ISOs.GetLength() < 1)
					ISOs = "\r\n\r\nISOs with errors:\r\n";
				ISOs.Append("\r\n");
				ISOs.Append(temp);
				ISOs.Append("\r\n");
			}
		}
		catch (...)
		{
		}
	}

	void ClearMap()
	{
		POSITION pos;
		for (pos = GetStartPosition(); pos != NULL;)
		{
			CString key;
			InitialStatusDataStruct* p = NULL;
			GetNextAssoc(pos, key, (InitialStatusDataStruct*&)p);
			if (p != NULL)
				delete p;
		}

	}


};



