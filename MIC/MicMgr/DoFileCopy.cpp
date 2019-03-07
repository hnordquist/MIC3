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
// DoFileCopy.cpp : implementation file
//

#include "stdafx.h"
#include "INI_definitions.h"
#include "ISO_definitions.h"
#include "globals.h"
#include "colors.h"
#include "DoFileCopy.h"
#include "HideDlg.h"
#include "TimedMessageDialog.h"
#include <io.h>
#include <direct.h>
#include <errno.h>
#include  <dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 11 Oct 2006
// Numerous changes by Dave Pelowitz to clear up these two issues:
// DR 359 Copy function does not allow for long-form names.
// DR 353 Program stops responding when using MIC copy function.
// pjm


class TDeviceEject
{
public:
	typedef enum
	{
		deec_Ok                        =  0,
		deec_UnsupportedOS             =  1,
		deec_ErrorLoadingLibrary       =  2,
		deec_LibraryFunctionMissing    =  3,
		deec_DeviceNotFound            =  4,
		deec_DeviceNotRemovable        =  5,
		deec_DeviceHasProblem          =  6,
		deec_NativeError               =  7,
		deec_ErrorGettingDevNodeStatus =  8,
		deec_ErrorGettingDeviceIdSize  =  9,
		deec_ErrorGettingDeviceId      = 10,
		deec_ErrorGettingChild         = 11,
		deec_ErrorGettingSibling       = 12,
		deec_ErrorLocatingDevNode      = 13,
		deec_ErrorDeviceEject          = 14,
		deec_ErrorEjectVetoed          = 15,
		deec_LAST
	} TDeviceEjectErrorCode;
	TDeviceEjectErrorCode DeviceEjectErrorCode;
	// Fehlercodes des Configuration Managers
	enum
	{
		CR_SUCCESS                  = 0x00,
		CR_DEFAULT                  = 0x01,
		CR_OUT_OF_MEMORY            = 0x02,
		CR_INVALID_POINTER          = 0x03,
		CR_INVALID_FLAG             = 0x04,
		CR_INVALID_DEVNODE          = 0x05,
		CR_INVALID_RES_DES          = 0x06,
		CR_INVALID_LOG_CONF         = 0x07,
		CR_INVALID_ARBITRATOR       = 0x08,
		CR_INVALID_NODELIST         = 0x09,
		CR_DEVNODE_HAS_REQS         = 0x0A,
		CR_INVALID_RESOURCEID       = 0x0B,
		CR_DLVXD_NOT_FOUND          = 0x0C,
		CR_NO_SUCH_DEVNODE          = 0x0D,
		CR_NO_MORE_LOG_CONF         = 0x0E,
		CR_NO_MORE_RES_DES          = 0x0F,
		CR_ALREADY_SUCH_DEVNODE     = 0x10,
		CR_INVALID_RANGE_LIST       = 0x11,
		CR_INVALID_RANGE            = 0x12,
		CR_FAILURE                  = 0x13,
		CR_NO_SUCH_LOGICAL_DEV      = 0x14,
		CR_CREATE_BLOCKED           = 0x15,
		CR_NOT_SYSTEM_VM            = 0x16,
		CR_REMOVE_VETOED            = 0x17,
		CR_APM_VETOED               = 0x18,
		CR_INVALID_LOAD_TYPE        = 0x19,
		CR_BUFFER_SMALL             = 0x1A,
		CR_NO_ARBITRATOR            = 0x1B,
		CR_NO_REGISTRY_HANDLE       = 0x1C,
		CR_REGISTRY_ERROR           = 0x1D,
		CR_INVALID_DEVICE_ID        = 0x1E,
		CR_INVALID_DATA             = 0x1F,
		CR_INVALID_API              = 0x20,
		CR_DEVLOADER_NOT_READY      = 0x21,
		CR_NEED_RESTART             = 0x22,
		CR_NO_MORE_HW_PROFILES      = 0x23,
		CR_DEVICE_NOT_THERE         = 0x24,
		CR_NO_SUCH_VALUE            = 0x25,
		CR_WRONG_TYPE               = 0x26,
		CR_INVALID_PRIORITY         = 0x27,
		CR_NOT_DISABLEABLE          = 0x28,
		CR_FREE_RESOURCES           = 0x29,
		CR_QUERY_VETOED             = 0x2A,
		CR_CANT_SHARE_IRQ           = 0x2B,
		CR_NO_DEPENDENT             = 0x2C,
		CR_SAME_RESOURCES           = 0x2D,
		CR_NO_SUCH_REGISTRY_KEY     = 0x2E,
		CR_INVALID_MACHINENAME      = 0x2F,
		CR_REMOTE_COMM_FAILURE      = 0x30,
		CR_MACHINE_UNAVAILABLE      = 0x31,
		CR_NO_CM_SERVICES           = 0x32,
		CR_ACCESS_DENIED            = 0x33,
		CR_CALL_NOT_IMPLEMENTED     = 0x34,
		CR_INVALID_PROPERTY         = 0x35,
		CR_DEVICE_INTERFACE_ACTIVE  = 0x36,
		CR_NO_SUCH_DEVICE_INTERFACE = 0x37,
		CR_INVALID_REFERENCE_STRING = 0x38,
		CR_INVALID_CONFLICT_LIST    = 0x39,
		CR_INVALID_INDEX            = 0x3A,
		CR_INVALID_STRUCTURE_SIZE   = 0x3B
	};
	DWORD NativeErrorCode;
	TDeviceEject(BOOL AVerbose,BOOL ADebug);
	~TDeviceEject();
	TDeviceEjectErrorCode Eject(PCHAR EjectDevSpec,BOOL EjectDevSpecIsName,PDWORD NativeErrorCode,PDWORD EjectedCount);
	static BOOL GetDriveDeviceId(PCHAR DriveSpec,PCHAR DeviceId,DWORD MaxDeviceIdSize);
private:
	BOOL    Verbose;
	BOOL    Debug;
	HMODULE Lib;
	BOOL    UnloadLib;
	typedef DWORD  DEVINST,*PDEVINST;
	typedef CHAR  *DEVINSTID_A;
	typedef enum
	{
		PNP_VetoTypeUnknown,
		PNP_VetoLegacyDevice,
		PNP_VetoPendingClose,
		PNP_VetoWindowsApp,
		PNP_VetoWindowsService,
		PNP_VetoOutstandingOpen,
		PNP_VetoDevice,
		PNP_VetoDriver,
		PNP_VetoIllegalDeviceRequest,
		PNP_VetoInsufficientPower,
		PNP_VetoNonDisableable,
		PNP_VetoLegacyDriver,
		PNP_VetoInsufficientRights
	} PNP_VETO_TYPE,* PPNP_VETO_TYPE;
	enum
	{
		DN_REMOVABLE = 0x00004000
	};
	typedef DWORD  CONFIGRET;
	typedef CONFIGRET (WINAPI *TCM_Locate_DevNodeA)      (OUT PDEVINST pdnDevInst,IN DEVINSTID_A pDeviceID,OPTIONAL IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Get_Child)            (OUT PDEVINST pdnDevInst,IN DEVINST dnDevInst,IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Get_Sibling)          (OUT PDEVINST pdnDevInst,IN DEVINST DevInst,IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Get_DevNode_Status)   (OUT PULONG pulStatus,OUT PULONG pulProblemNumber,IN DEVINST dnDevInst,IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Get_Device_ID_Size)   (OUT PULONG pulLen,IN DEVINST dnDevInst,IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Get_Device_IDA)       (IN DEVINST dnDevInst,OUT PCHAR Buffer,IN ULONG BufferLen,IN ULONG ulFlags);
	typedef CONFIGRET (WINAPI *TCM_Request_Device_EjectA)(IN DEVINST dnDevInst,OUT PPNP_VETO_TYPE pVetoType,OUT LPSTR pszVetoName,IN ULONG ulNameLength,IN ULONG ulFlags);
	TCM_Locate_DevNodeA       Func_LocateDevNode;
	TCM_Get_Child             Func_GetChild;
	TCM_Get_Sibling           Func_GetSibling;
	TCM_Get_DevNode_Status    Func_GetDevNodeStatus;
	TCM_Get_Device_ID_Size    Func_GetDeviceIDSize;
	TCM_Get_Device_IDA        Func_GetDeviceID;
	TCM_Request_Device_EjectA Func_RequestDeviceEject;

	typedef struct _TEnumDeviceInfo
	{
		struct _TEnumDeviceInfo *Parent;
		DEVINST                  DevInst;
		ULONG                    Status;
		ULONG                    ProblemNumber;
	} TEnumDeviceInfo,* PEnumDeviceInfo;
	TDeviceEjectErrorCode EnumDevices(PEnumDeviceInfo ParentEnumDeviceInfo,PDWORD EjectDeviceFound,PCHAR EjectDevSpec,BOOL EjectDevSpecIsName,DEVINST DevInst,int Indent,PDWORD NativeErrorCode);
	BOOL                  GetFriendlyName(PCHAR DeviceId,PCHAR Name,DWORD MaxNameSize);
};
typedef TDeviceEject * PDeviceEject;

TDeviceEject::TDeviceEject(BOOL AVerbose,BOOL ADebug)
{
	DeviceEjectErrorCode    = deec_Ok;
	NativeErrorCode         = 0;
	Verbose                 = AVerbose;
	Debug                   = ADebug;
	Lib                     = NULL;
	UnloadLib               = FALSE;
	Func_LocateDevNode      = NULL;
	Func_GetChild           = NULL;
	Func_GetSibling         = NULL;
	Func_GetDevNodeStatus   = NULL;
	Func_GetDeviceIDSize    = NULL;
	Func_GetDeviceID        = NULL;
	Func_RequestDeviceEject = NULL;

	BOOL OSOk = FALSE;
	DWORD V = GetVersion();
	if (!(V & 0x80000000))
	{
		BYTE MajorVersion = (BYTE)V;
		if (MajorVersion >= 5)
			OSOk = TRUE;
	}
	if (!OSOk)
		DeviceEjectErrorCode = deec_UnsupportedOS;
	else
	{
		const PCHAR LibFileName = "setupapi.dll";
		Lib = GetModuleHandle(LibFileName);
		if (!Lib)
		{
			Lib = LoadLibrary(LibFileName);
			if (Lib)
				UnloadLib = TRUE;
		}
		if (!Lib)
		{
			DeviceEjectErrorCode = deec_ErrorLoadingLibrary;
			NativeErrorCode      = GetLastError();
		}
		else
		{
			Func_LocateDevNode      = (TCM_Locate_DevNodeA)      GetProcAddress(Lib,"CM_Locate_DevNodeA");
			Func_GetChild           = (TCM_Get_Child)            GetProcAddress(Lib,"CM_Get_Child");
			Func_GetSibling         = (TCM_Get_Sibling)          GetProcAddress(Lib,"CM_Get_Sibling");
			Func_GetDevNodeStatus   = (TCM_Get_DevNode_Status)   GetProcAddress(Lib,"CM_Get_DevNode_Status");
			Func_GetDeviceIDSize    = (TCM_Get_Device_ID_Size)   GetProcAddress(Lib,"CM_Get_Device_ID_Size");
			Func_GetDeviceID        = (TCM_Get_Device_IDA)       GetProcAddress(Lib,"CM_Get_Device_IDA");
			Func_RequestDeviceEject = (TCM_Request_Device_EjectA)GetProcAddress(Lib,"CM_Request_Device_EjectA");
		}
	}
}

TDeviceEject::~TDeviceEject()
{
	if (Lib && UnloadLib)
		FreeLibrary(Lib);
}

BOOL TDeviceEject::GetDriveDeviceId(PCHAR DriveSpec,PCHAR DeviceId,DWORD MaxDeviceIdSize)
{
	BOOL Result = FALSE;
	if (DeviceId && MaxDeviceIdSize)
		DeviceId[0] = '\0';
	if (DriveSpec && DriveSpec[0] && DeviceId && MaxDeviceIdSize)
	{
		HKEY Key;
		LONG L = RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SYSTEM\\MountedDevices",0,KEY_QUERY_VALUE,&Key);
		if (L == ERROR_SUCCESS)
		{
			DWORD Type;
			BYTE  Value[1024];
			DWORD ValueSize = sizeof Value;
			CHAR  ValueName[256];
			strcpy(ValueName,"\\DosDevices\\");
			strncat(ValueName,DriveSpec,sizeof ValueName - 1);
			ValueName[sizeof ValueName - 1] = '\0';
			L = RegQueryValueEx(Key,ValueName,0,&Type,(BYTE *)Value,&ValueSize);
			RegCloseKey(Key);
			if ((L == ERROR_SUCCESS) && (Type == REG_BINARY))
			{
				// Beispielsweise:
				//   \??\SBP2#API-903-95__&1394_Storage_+_Repeater_&LUN0#0050c564500068e2#{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}
				// ->
				//   SBP2\API-903-95__&1394_STORAGE_+_REPEATER_&LUN0\0050C564500068E2
				PWSTR p1 = (PWSTR)Value;
				ValueSize /= sizeof *p1;
				if ((p1[0] == '\\') && (p1[1] == '?') && (p1[2] == '?') && (p1[3] == '\\'))
				{
					Result = TRUE;
					p1        += 4;
					ValueSize -= 4;
					PCHAR p2 = DeviceId;
					for (unsigned int i = 0;i < ValueSize;++i)
					{
						CHAR c = (char)p1[i];
						if ((c == '{') || ((c == '#') && (p1[i + 1] == '{')))
							break;
						if (c == '#')
							c = '\\';
						if ((DWORD)(p2 - DeviceId) < (MaxDeviceIdSize - 1))
							*p2++ = c;
					}
					*p2 = '\0';
				}
			}
		}
	}
	return Result;
}

BOOL TDeviceEject::GetFriendlyName(PCHAR DeviceId,PCHAR Name,DWORD MaxNameSize)
{
	BOOL Result = FALSE;
	if (Name && MaxNameSize)
		Name[0] = '\0';
	if (DeviceId && DeviceId[0] && Name && MaxNameSize)
	{
		HKEY Key;
		CHAR KeyName[1024];
		strcpy(KeyName,"SYSTEM\\CurrentControlSet\\Enum\\");
		strncat(KeyName,DeviceId,sizeof KeyName - 1);
		KeyName[sizeof KeyName - 1] = '\0';
		LONG L = RegOpenKeyEx(HKEY_LOCAL_MACHINE,KeyName,0,KEY_QUERY_VALUE,&Key);
		if (L == ERROR_SUCCESS)
		{
			DWORD Type;
			BYTE  Value[1024];
			DWORD ValueSize = sizeof Value;
			L = RegQueryValueEx(Key,"FriendlyName",0,&Type,(BYTE *)Value,&ValueSize);
			if (L != ERROR_SUCCESS)
				L = RegQueryValueEx(Key,"DeviceDesc",0,&Type,(BYTE *)Value,&ValueSize);
			RegCloseKey(Key);
			if ((L == ERROR_SUCCESS) && (Type == REG_SZ))
			{
				strncpy(Name,(PCHAR)Value,MaxNameSize - 1);
				Name[MaxNameSize - 1] = '\0';
				Result = TRUE;
			}
		}
	}
	return Result;
}

static BOOL CompareWithWildcard(PCHAR s1,PCHAR s2)
{
	if (s1 && s2)
		for (;;)
		{
			if (*s2 == '*')
				return TRUE;
			if (!*s1)
			{
				if (!*s2)
					return TRUE;
				break;
			}
			if (!*s2)
			{
				if (!*s1)
					return TRUE;
				break;
			}
			if (toupper(*s1++) != toupper(*s2++))
				break;
		}
		return FALSE;
}

TDeviceEject::TDeviceEjectErrorCode TDeviceEject::EnumDevices(PEnumDeviceInfo ParentEnumDeviceInfo,PDWORD EjectDeviceFound,PCHAR EjectDevSpec,BOOL EjectDevSpecIsName,DEVINST DevInst,int Indent,PDWORD NativeErrorCode)
{
	TDeviceEjectErrorCode Result = deec_Ok;

	DEVINST ThisDevInst = DevInst;
	while (Result == deec_Ok)
	{
		ULONG Status; // DN_...
		ULONG ProblemNumber;
		CONFIGRET r = Func_GetDevNodeStatus(&Status,&ProblemNumber,ThisDevInst,0);
		if (r == CR_NO_SUCH_DEVNODE)
		{
		}
		else if (r != CR_SUCCESS)
		{
			Result = deec_ErrorGettingDevNodeStatus;      
			*NativeErrorCode = r;
		}
		if (Result == deec_Ok)
		{
			ULONG DeviceIDLen;
			r = Func_GetDeviceIDSize(&DeviceIDLen,ThisDevInst,0);
			if (r != CR_SUCCESS)
			{
				Result = deec_ErrorGettingDeviceIdSize;
				*NativeErrorCode = r;
			}
			else
			{
				PCHAR DeviceID = new CHAR[DeviceIDLen + 1];
				if (DeviceID)
				{
					r = Func_GetDeviceID(ThisDevInst,DeviceID,DeviceIDLen + 1,0);
					if (r != CR_SUCCESS)
					{
						Result = deec_ErrorGettingDeviceId;
						*NativeErrorCode = r;
					}
					else
					{
						CHAR FriendlyName[512];
						if (!GetFriendlyName(DeviceID,FriendlyName,sizeof FriendlyName))
							FriendlyName[0] = '\0';

						if (Verbose)
						{
							for (int i = 0;i < Indent * 2;++i)
								printf(" ");
							printf("%s",DeviceID);

							if (Debug)
							{
								printf(" DevInst: %u",ThisDevInst);
								printf(" Status: 0x%X ",Status);
							}

							if (FriendlyName[0])
								printf(" '%s'",FriendlyName);

							if (Status & DN_REMOVABLE)
								printf(" [REMOVEABLE]");
							if (ProblemNumber)
								printf(" ProblemNumber: %u",ProblemNumber);

							printf("\n");
						}
						else
						{
							if (!EjectDevSpec || !EjectDevSpec[0])
								if (Status & DN_REMOVABLE)
								{
									if (FriendlyName[0])
										printf("'%s' ",FriendlyName);
									printf("'%s' [REMOVEABLE]\n",DeviceID);
								}
						}

						BOOL Found = FALSE;
						if (EjectDevSpec && EjectDevSpec[0])
						{
							if (EjectDevSpecIsName)
							{
								if (FriendlyName[0])
									Found = CompareWithWildcard(FriendlyName,EjectDevSpec);
							}
							else
								Found = CompareWithWildcard(DeviceID,EjectDevSpec);
						}
						if (Found)
						{
							++*EjectDeviceFound;
							DEVINST EjectDevInst = ThisDevInst;
							if (!(Status & DN_REMOVABLE))
							{
								// Dieses Device ist nicht auswerfbar, probiere übergeordneten Eintrag
								for (PEnumDeviceInfo EDI = ParentEnumDeviceInfo;EDI;EDI = EDI->Parent)
								{
									if (EDI->Status & DN_REMOVABLE)
									{
										EjectDevInst  = EDI->DevInst;
										Status        = EDI->Status;
										ProblemNumber = EDI->ProblemNumber;
										break;
									}
								}
							}

							if (!(Status & DN_REMOVABLE))
							{
								Result = deec_DeviceNotRemovable;

								printf("Device");
								if (FriendlyName[0])
									printf(" '%s'",FriendlyName);
								printf(" [%s] not removable!\n",DeviceID);
							}
							else
							{
								if (ProblemNumber)
								{
									Result = deec_DeviceHasProblem;
									*NativeErrorCode = ProblemNumber;
								}
								else
								{
									printf("Ejecting ");
									if (FriendlyName[0])
										printf(" '%s'",FriendlyName);
									printf(" [%s]...",DeviceID);

									PNP_VETO_TYPE VetoType;
									CHAR          VetoName[MAX_PATH];
									r = Func_RequestDeviceEject(EjectDevInst,&VetoType,VetoName,sizeof VetoName,0);
									if (r == CR_SUCCESS)
										printf("ok.\n");
									else
									{
										printf("FAILED (%u,%u)\n",r,VetoType);
										if (r == CR_REMOVE_VETOED)
										{
											Result = deec_ErrorEjectVetoed;
											*NativeErrorCode = VetoType;
										}
										else
										{
											Result = deec_ErrorDeviceEject;
											*NativeErrorCode = r;
										}
									}
								}
							}
						}
					}
					delete [] DeviceID;
				}
			}
		}

		if (Result == deec_Ok)
		{
			DEVINST Child;
			r = Func_GetChild(&Child,ThisDevInst,0);
			if (r == CR_NO_SUCH_DEVNODE)
			{
			}
			else if (r != CR_SUCCESS)
			{
				Result = deec_ErrorGettingChild;
				*NativeErrorCode = r;
			}
			else
			{
				TEnumDeviceInfo EnumDeviceInfo;
				EnumDeviceInfo.Parent        = ParentEnumDeviceInfo;
				EnumDeviceInfo.DevInst       = ThisDevInst;
				EnumDeviceInfo.Status        = Status;
				EnumDeviceInfo.ProblemNumber = ProblemNumber;
				Result = EnumDevices(&EnumDeviceInfo,EjectDeviceFound,EjectDevSpec,EjectDevSpecIsName,Child,Indent + 1,NativeErrorCode);
			}
			if (Result == deec_Ok)
			{
				DEVINST Sibling;
				r = Func_GetSibling(&Sibling,ThisDevInst,0);
				if (r == CR_NO_SUCH_DEVNODE)
					break;
				if (r != CR_SUCCESS)
				{
					Result = deec_ErrorGettingSibling;
					*NativeErrorCode = r;
				}
				ThisDevInst = Sibling;
			}
		}
	}
	return Result;
}

TDeviceEject::TDeviceEjectErrorCode TDeviceEject::Eject(PCHAR EjectDevSpec,BOOL EjectDevSpecIsName,PDWORD NativeErrorCode,PDWORD EjectedCount)
{
	TDeviceEjectErrorCode Result = deec_Ok;
	*NativeErrorCode = 0;
	if (EjectedCount)
		*EjectedCount = 0;
	if (!Func_LocateDevNode || !Func_GetChild || !Func_GetSibling || !Func_GetDevNodeStatus || !Func_GetDeviceIDSize || !Func_GetDeviceID || !Func_RequestDeviceEject)
		Result = deec_LibraryFunctionMissing;
	else
	{
		DEVINST RootDevInst;
		CONFIGRET r = Func_LocateDevNode(&RootDevInst,NULL,0/*CM_LOCATE_DEVNODE_NORMAL*/);
		if (r != CR_SUCCESS)
		{
			Result = deec_ErrorLocatingDevNode;
			*NativeErrorCode = r;
		}
		else
		{
			DWORD EjectDeviceFound = 0;
			Result = EnumDevices(NULL,&EjectDeviceFound,EjectDevSpec,EjectDevSpecIsName,RootDevInst,0,NativeErrorCode);
			if ((Result == deec_Ok) && EjectDevSpec[0] && !EjectDeviceFound)
				Result = deec_DeviceNotFound;
			if (EjectedCount)
				*EjectedCount = EjectDeviceFound;
		}
	}
	return Result;
}

int CDoFileCopy::Eject(char* EjectDrive)
{
	int iResult = 0;

	BOOL Verbose = FALSE;
	BOOL Debug   = FALSE;
	CHAR EjectDevSpec[1024];
	EjectDevSpec[0] = '\0';
	BOOL EjectDevSpecIsName = FALSE;
	if (!TDeviceEject::GetDriveDeviceId(EjectDrive,EjectDevSpec,sizeof EjectDevSpec))
	{
		//		ArgsOk = FALSE;
	}

	PDeviceEject DeviceEject = new TDeviceEject(Verbose,Debug);
	if (!DeviceEject)
		iResult = 11;
	else
	{
		if (DeviceEject->DeviceEjectErrorCode)
			iResult = 12;
		else
		{
			DWORD NativeErrorCode;
			DWORD EjectedCount;
			TDeviceEject::TDeviceEjectErrorCode ErrorCode = DeviceEject->Eject(EjectDevSpec,EjectDevSpecIsName,&NativeErrorCode,&EjectedCount);
			if (ErrorCode != TDeviceEject::deec_Ok)
				iResult = 13;
		}
		delete DeviceEject;
	}
	return iResult;
}

/////////////////////////////////////////////////////////////////////////////
// CDoFileCopy dialog

#define SHOW_ALLSUFFIXMATCHES			"SHOW_ALLSUFFIXMATCHES"   // a special value


UINT uiDoQueryCancelAutoPlay;													//DR00175

CDoFileCopy::CDoFileCopy(CWnd* parent)
: CDialog(CDoFileCopy::IDD, parent)
{
	//	TRACE("CDoFileCopy::CTOR\n");
	uiDoQueryCancelAutoPlay = RegisterWindowMessage(TEXT("QueryCancelAutoPlay"));		//DR00175
	m_dToClose = 0;
	//m_bShowAllSuffixMatches = false;
}

CDoFileCopy::~CDoFileCopy()
{
	//	TRACE("CDoFileCopy::DTOR\n");
	delete m_pImageList;
}

void CDoFileCopy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDoFileCopy)
	DDX_Control(pDX, IDC_FILE_LIST, m_pFileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDoFileCopy, CDialog)
	//{{AFX_MSG_MAP(CDoFileCopy)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_DEVICECHANGE()											 		//DR00175
	ON_BN_CLICKED(IDC_UNDO_BUTTON, OnUndoButton)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILE_LIST, OnItemchangedFileList)
	ON_BN_CLICKED(IDC_BEGINCOPY_BUTTON, OnBegincopyButton)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_FILE_LIST, OnItemchangingFileList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_FILE_LIST, OnColumnclickFileList)
	ON_BN_CLICKED(IDC_TOCLOSE_BUTTON, OnTocloseButton)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_REGISTERED_MESSAGE(uiDoQueryCancelAutoPlay, OnQueryCancelAutoPlay)	//DR00175
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoFileCopy message handlers
LRESULT CDoFileCopy::OnQueryCancelAutoPlay(WPARAM wParam,LPARAM lParam)		//DR00175
{																			//DR00175
	return TRUE;															//DR00175
}																			//DR00175
//DR00175
BOOL CDoFileCopy::OnDeviceChange(UINT nEventType, DWORD dwData)				//DR00175
{																			//DR00175
	if (nEventType == DBT_DEVICEARRIVAL)									//DR00175
	{																		//DR00175
		PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)dwData;			//DR00175
		char cDisk;															//DR00175
		unsigned int mCheck = 1;											//DR00175
		cDisk = 'A';														//DR00175
		while (!(pbv->dbcv_unitmask & mCheck) && (mCheck < 0x2000000) )		//DR00175
		{																	//DR00175
			cDisk++;														//DR00175
			mCheck = mCheck<<1;												//DR00175
		}																	//DR00175
		m_pDestination.Format("%c:\\",cDisk);								//DR00175
		SetDlgItemText(IDC_DESTINATION_STATIC,m_pDestination);				//DR00175
		//DR00175
		//save the new destination											//DR00175
		WritePrivateProfileString(CONFIGURATION,DEFAULTCOPYPATH,m_pDestination,g_szIniFile);	//DR00175
	}																		//DR00175
	/*																			//DR00175
	//at this point we don't really care if the media has been removed		//DR00175
	else if (nEventType == DBT_DEVICEREMOVECOMPLETE)						//DR00175
	{																		//DR00175
	PDEV_BROADCAST_VOLUME pbv = (PDEV_BROADCAST_VOLUME)dwData;			//DR00175
	char cDisk;															//DR00175
	unsigned int mCheck = 1;											//DR00175
	cDisk = 'A';														//DR00175
	while (!(pbv->dbcv_unitmask & mCheck)&& (mCheck < 0x2000000))		//DR00175
	{ 																	//DR00175
	cDisk++;														//DR00175
	mCheck = mCheck<<1;												//DR00175
	}																	//DR00175
	CString cMsg;														//DR00175
	}																		//DR00175
	*/																			//DR00175
	return TRUE;															//DR00175
}																			//DR00175

void CDoFileCopy::OnTimer(UINT nIDEvent) 
{
	//timer 1 is the 1/2 second timer set in parent
	if (nIDEvent == 1)
	{
		//if elapsed since user entered a new media greater then
		//autoclose -- something is wrong
		if (m_dToClose*500 >= g_iDlgCloseMillisec)
			OnCancel();
		else
		{
			char temp[32];
			int minutes = (g_iDlgCloseMillisec/1000-m_dToClose/2)/60;
			int seconds   = (g_iDlgCloseMillisec/1000-m_dToClose/2) % 60;
			sprintf(temp,"Ext. Auto Close: %d:%02d",minutes,seconds);
			GetDlgItem(IDC_TOCLOSE_BUTTON)->SetWindowText(temp);
		}

		//count time in this dialog so that we can terminate from inactivity
		if (!m_bWaitingForMedia) m_dToClose++;
		//set the icons color "Select File Sets..."
		//and toggle the yellow ones
		if (m_hSelectState == m_hIconYellow1)
		{
			m_hSelectState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_5))->SetIcon(m_hSelectState);
		}
		else if
			(m_hSelectState == m_hIconYellow2)
		{
			m_hSelectState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_5))->SetIcon(m_hSelectState);
		}

		//"Insert Next Media"
		if (m_hInsertState == m_hIconYellow1)
		{
			m_hInsertState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
		}
		else if
			(m_hInsertState == m_hIconYellow2)
		{
			m_hInsertState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
		}

		//"Remove Media"
		if (m_hRemoveState == m_hIconYellow1)
		{
			m_hRemoveState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
		}
		else if
			(m_hRemoveState == m_hIconYellow2)
		{
			m_hRemoveState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
		}

		//"Select "Archive & Exit..."
		if (m_hVerifyState == m_hIconYellow1)
		{
			m_hVerifyState = m_hIconYellow2;
			((CStatic*)GetDlgItem(IDC_ICON_8))->SetIcon(m_hVerifyState);
		}
		else if
			(m_hVerifyState == m_hIconYellow2)
		{
			m_hVerifyState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_8))->SetIcon(m_hVerifyState);
		}

		//automatically detect media state and change flags accordingly
		//new media state = 1 is waiting for disk to be inserted
		if (m_dNewMediaState == 1)
		{
			if (m_bFinishState==1)
			{
				m_bFinishState = 2;
				GetDlgItem(IDOK)->EnableWindow(true);
				m_hVerifyState = m_hIconYellow1;
				((CStatic*)GetDlgItem(IDC_ICON_8))->SetIcon(m_hVerifyState);
				if (m_bRemovable)
					SetDlgItemText(IDC_NEXT_STATIC,"Insert Media for\nNext Copy");
			}
			//see if there is a disk in the drive
			if (TestDrive())
			{
				if (m_bFinishState == 2)
				{
					m_bFinishState = 0;
					GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(true);
					SetDlgItemText(IDC_NEXT_STATIC,"Insert Next\nMedia");
				}
				m_dToClose = 0;
				m_dNewMediaState = (m_bDoingSwap)?2:0;
				m_hInsertState = m_hIconGreen;
				((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
				m_hRemoveState = m_hIconGreen;
				((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
			}
		}
		//new media state == 0 is normal state
		if (m_dNewMediaState == 0)
		{
			//watch for disk to be removed
			if (!TestDrive())
			{
				m_dToClose = 0;
				m_dNewMediaState = 1;
				m_hRemoveState = m_hIconGreen;
				((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
				if (m_bRemovable)
				{
					m_hInsertState = m_hIconYellow1;
					((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
				}
			} 
		}

	}
	CDialog::OnTimer(nIDEvent);
}

bool CDoFileCopy::TestDrive()
{
	//save the old error mode and catch all 
	UINT oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
	//check for read and write
	char drive[_MAX_DRIVE], dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(m_pDestination,drive,dir,fname,ext);
	bool good;
	if (strlen(drive) == 0)
		good = (_access(dir,2)==0);
	else
		good = (_access(drive,2)==0);
	//	bool good = (_access(m_pDestination,2) == 0);
	SetErrorMode(oldErrorMode);
	TRACE("%s\r\n",good?"GOOD":"BAD");
	return good;
}

void CDoFileCopy::GetYMDFromName(char* szName, bool bNotAShortFileName, int* piYear,int* piMonth,int* piDay)
{
	char szTemp[16];
	if (m_bShortName && !bNotAShortFileName)//handle short form of file name
	{
		*piYear = (szName[2]>'9')?(toupper(szName[2])-'A'+2000):szName[2]-'0'+1990;
		*piMonth= (szName[3]>'9')?(toupper(szName[3])-'A'+10  ):szName[3]-'0';
		*piDay  = (szName[4]>'9')?(toupper(szName[4])-'A'+10  ):szName[4]-'0';
	}
	else //handle long form of file name
		try { // JFL this a dicey crappy dangerous piece of code
			char szTemplate[_MAX_PATH];
			memset(szTemplate,NULL,_MAX_PATH);
			bool bInPercent = false;
			CString cTemp;
			cTemp = m_csNamePattern;
			//first build a char template of the file name
			//lower case for everything other than the stuff
			//we are looking for
			int j=0;
			for (int i=0,j=0; i<cTemp.GetLength(); i++)
			{
				char ch = cTemp[i];
				if (bInPercent)
				{
					//expand it to destination
					switch (ch) {
						case 'T'://two char type
							szTemplate[j++] = 'T';
							szTemplate[j++] = 'T';
							break;
						case 't'://two char type
							szTemplate[j++] = 't';
							szTemplate[j++] = 't';
							szTemplate[j++] = 't';
							szTemplate[j++] = 't';
							break;
						case 'I'://three char inst ID
							szTemplate[j++] = 'I';
							szTemplate[j++] = 'I';
							szTemplate[j++] = 'I';
							break;
						case 'i'://two char inst ID
							szTemplate[j++] = 'K';
							szTemplate[j++] = 'K';
							break;
						case 'y'://4 char year
							szTemplate[j++] = 'Y';
							szTemplate[j++] = 'Y';
							szTemplate[j++] = 'Y';
							szTemplate[j++] = 'Y';
							break;
						case 'm'://2 char month
							szTemplate[j++] = 'M';
							szTemplate[j++] = 'M';
							break;
						case 'd'://2 char day
							szTemplate[j++] = 'D';
							szTemplate[j++] = 'D';
							break;
						case 'H'://2 char hour
							szTemplate[j++] = 'H';
							szTemplate[j++] = 'H';
							break;
						case 'M'://2 char minute
							szTemplate[j++] = 'M';
							szTemplate[j++] = 'M';
							break;
						case 'S'://2 char second
							szTemplate[j++] = 'S';
							szTemplate[j++] = 'S';
							break;
						case '%'://one char %
							szTemplate[j++] = '%';
							break;
					}
					bInPercent = false;
				}
				else 
				{
					if (ch == '%')
						bInPercent = true;
					else
						szTemplate[j++] = tolower(ch);
				}
			}

			//find where the year is supposed to be in the template
			//and get the numeric value from the real name
			char* szAt;
			szAt = strstr(szTemplate,"YYYY");
			if (szAt)
			{
				strncpy(szTemp,&szName[szAt-szTemplate],4);
				szTemp[4] = NULL;
				sscanf(szTemp,"%d",piYear);
				if (*piYear < 1899 || *piYear > 2100)
					*piYear = 0;
			}
			else
				*piYear = 0;

			//find where the month is supposed to be in the template
			//and get the numeric value from the real name
			szAt = strstr(szTemplate,"MM");
			if (szAt)
			{
				strncpy(szTemp,&szName[szAt-szTemplate],2);
				szTemp[2] = NULL;
				sscanf(szTemp,"%d",piMonth);
				if (*piMonth < 1 || *piMonth > 12)
					*piMonth = 0;
			}
			else
				*piMonth = 0;

			//find where the day is supposed to be in the template
			//and get the numeric value from the real name
			szAt = strstr(szTemplate,"DD");
			if (szAt)
			{
				strncpy(szTemp,&szName[szAt-szTemplate],2);
				szTemp[2] = NULL;
				sscanf(szTemp,"%d",piDay);
				if (*piDay < 1 || *piDay > 31)
					*piDay = 0;
			}
			else
				*piDay = 0;
	}
	catch(  ... )
	{
		// could emit log here
	}
}

DWORD CDoFileCopy::GetMediaSize()
{
	//get the size of the current media
	DWORD SectorsPerCluster = 0;
	DWORD BytesPerSector = 0;
	DWORD NumberOfFreeClusters = 0;
	DWORD BytesPerCluster = 0;
	//don't know if GetDiskFreeSpace provides the correct values 
	//on NT with large drives (over 2.1Gbyte) -- it doesn't on '95
	//	char path_buffer[_MAX_PATH];   
	char drive[_MAX_DRIVE+2];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   
	char ext[_MAX_EXT];

	_splitpath(m_pDestination,drive,dir,fname,ext);
	strcat(drive,"\\");
	if (GetDiskFreeSpace(drive,&SectorsPerCluster,&BytesPerSector,
		&NumberOfFreeClusters,&m_dTotalNumberOfClusters))
	{
		//into the next to last cluster -- save a cluster for growth of dir
		NumberOfFreeClusters--;
		BytesPerCluster = BytesPerSector * SectorsPerCluster;
	}
	return BytesPerCluster;
}

BOOL CDoFileCopy::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//get form of file name
	char szTemp[_MAX_PATH];
	GetPrivateProfileString(CONFIGURATION,DOSHORTNAME,"Yes",szTemp,sizeof(szTemp),g_szIniFile);
	m_bShortName = (toupper(szTemp[0]) == 'Y');
	if (!m_bShortName)		
	{
		GetPrivateProfileString(CONFIGURATION,LONGNAMEFORM,"",szTemp,sizeof(szTemp),g_szIniFile);
		if (strlen(szTemp))
			m_csNamePattern = szTemp;
	}
	//GetPrivateProfileString(CONFIGURATION,SHOW_ALLSUFFIXMATCHES,"No",szTemp,sizeof(szTemp),g_szIniFile);
	//m_bShowAllSuffixMatches = (toupper(szTemp[0]) == 'Y');

	//timeindialog incremented in ontimer, used to cancel
	//dialog box for inactivity
	m_dToClose = 0;
	//flag the times through the filecopy -- after the
	//first we won't let the selections change
	m_dEntryCount = 0;
	//don't start by waiting for media
	m_bWaitingForMedia = false;
	//direction to sort
	m_bDirectionUp = true;
	//which column will we sort on
	m_dSortedCol = 1;
	//if doing sort we don't want to update the static fields
	m_bDoingSort = false;
	//don't want to use the clickfilelist during setup
	m_bDoneSetup = false;
	//used in timer to signal how to set the icons
	m_bDoingSwap = false;
	//set ip file name only on the first pass through the filelist
	m_bFirstPass = true;
	//only allow selection change until a copy has been made
	m_bSelectionChangable = true;
	//icons work differently when doing endgame
	m_bFinishState = 0;
	//used to control waiting for removal, waiting for new media
	m_dNewMediaState = 0;
	//get the icon handles
	//	m_hIconYellow1	= AfxGetApp()->LoadIcon(IDI_ICON_YELLOWR);
	//	m_hIconYellow2	= AfxGetApp()->LoadIcon(IDI_ICON_YELLOWR2);
	m_hIconYellow1	= AfxGetApp()->LoadIcon(IDI_ICON11);
	m_hIconYellow2	= AfxGetApp()->LoadIcon(IDI_ICON12);

	m_hIconRed		= AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconGreen	= AfxGetApp()->LoadIcon(IDI_ICON_GREENT);
	//set the icon states to the startup condition
	m_hSelectState	= m_hIconYellow1;
	m_hInsertState	= m_hIconRed;
	m_hRemoveState	= m_hIconRed;
	m_hVerifyState	= m_hIconRed;
	//and set their icons
	((CStatic*)GetDlgItem(IDC_ICON_5))->SetIcon(m_hSelectState);
	((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
	((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
	((CStatic*)GetDlgItem(IDC_ICON_8))->SetIcon(m_hVerifyState);

	//don't allow ok until after at least one copy
	//also will do the "file move" 
	GetDlgItem(IDOK)->EnableWindow(false);

	//set the static destination
	SetDlgItemText(IDC_DESTINATION_STATIC,m_pDestination);

	//half second timer for display and watching media removal/replacement
	SetTimer(1,500,NULL);

	DWORD BytesPerCluster = GetMediaSize();

	//get the image list
	m_pImageList = new CImageList();
	m_pImageList->Create( IDB_BMP_ALL, 18, 1, COLOR_GRAY);

	//tell filelist to use this image list
	m_pFileList.SetImageList(m_pImageList,LVSIL_SMALL);//SMALL//NORMAL
	//create the first column header
	m_pFileList.InsertColumn(1, "Date (Y\\M\\D)", LVCFMT_LEFT, 110, -1 );
	//create the second column header
	m_pFileList.InsertColumn(2, "Old Data",LVCFMT_LEFT,60,1);
	//set the first column width
	m_pFileList.SetColumnWidth(0,m_pFileList.GetStringWidth("Date (Y\\M\\D)      "));

	m_pFileList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,0,(LPARAM)
		LVS_EX_FULLROWSELECT);

	char *pcLocation;
	CString csPath, csPathArchive, csPathArchiveName;
	CString csPathName;
	CString csPathNameTemp;
	CString csName;
	CString csCurrent;
	char buff[_MAX_PATH];
	long hFile, result;
	char Yes[] = "Yes";
	char No[] = "No";
	struct _finddata_t fileinfo;
	LV_FINDINFO pFindInfo;
	int TotalClusters = 0;
	int NumberOfColumns = 2;

	BSTR bstrSaveLoc, bstrFileExtensions;
	char cSaveLoc[_MAX_PATH];
	BSTR bstrID;
	char cID[_MAX_PATH];

	char *pFileExtensionList = NULL;
	int  iNumberOfFileExtensions;

	//for each instrument
	int nItem = 0;
	for (int i = 0; i < g_iInstCount; i++)
	{
		//if there is an instrument there (should be)
		if (!g_pIISO[i])
			continue;

		//get the location it is saving its files to
		g_pIISO[i]->get_LocationAndExtensionsForDataFiles(
			&bstrSaveLoc,
			&bstrFileExtensions);

		Convert_BSTR_to_Character_String(bstrSaveLoc,cSaveLoc,sizeof(cSaveLoc));

		if (strlen(cSaveLoc) == 0)
			continue;

		try
		{
			pcLocation = cSaveLoc;

			if (pFileExtensionList != NULL)
			{
				free (pFileExtensionList);
				pFileExtensionList = NULL;
			}

			UnpackFileExtensionsList(
				bstrFileExtensions,
				&pFileExtensionList,
				&iNumberOfFileExtensions);

			//insert a column for this instrument with the instrument ID
			g_pIISO[i]->get_ID(&bstrID);
			Convert_BSTR_to_Character_String(bstrID,cID,sizeof(cID));

			m_pFileList.InsertColumn(++NumberOfColumns,cID,LVCFMT_LEFT,30,-1);
			//build the full path to the files and the archive files	
			csPath = CString(pcLocation);
			if (csPath.Right(1) != '\\') csPath += '\\';
			csPathArchive = csPath + "archive\\";
			csPathName = csPath;
			//				csPath += CString(cID);
			//				csPath += "???a00.*";
			csPath += "*.*";
			//check if one file DOES exist in the save pcLocation
			if ((hFile = _findfirst(LPCTSTR(csPath),&fileinfo))!=-1)
			{
				//find each file and add the count to the filelist
				result = 0;
				while (result == 0)
				{
					if (!(fileinfo.attrib & _A_SUBDIR) && 
						!(fileinfo.attrib & _A_SYSTEM) &&
						!(fileinfo.attrib & _A_HIDDEN)
						)
					{
						//build file name prepending a blank
						//iff a one character id is used
						if (strlen(fileinfo.name)>11)
							buff[0] = NULL;
						else
							strcpy(buff," ");
						csPathNameTemp = csPathName + fileinfo.name;
						csPathArchiveName = csPathArchive + fileinfo.name;
						//include means to include this file in the list
						bool include = true;
						//addto means this file has been added to 
						//e.g. a file by the same name exists in archive
						bool addedto = false;
						//get the data on the file
						struct _WIN32_FILE_ATTRIBUTE_DATA FileInfoArchive; 
						//if file exists in archive

						if (GetFileAttributesEx(csPathArchiveName,GetFileExInfoStandard ,&FileInfoArchive))
						{
							//file exists in archive subdirectory
							COleDateTime archive(FileInfoArchive.ftLastWriteTime);
							COleDateTime normal(fileinfo.time_write);
							//if different write times then we need to append
							//include means only that we need to make an entry in the file box
							//addedto means that we need to set the icon to red/green and the new data column 
							char *p = NULL;
							char *pAt = pFileExtensionList;

							bool bHaveExtensionMatch = false;
							int length;

							char szDrive[_MAX_DRIVE+2];
							char szDir[_MAX_DIR];
							char szFName[_MAX_FNAME];
							char szExt[_MAX_EXT];
							_splitpath(fileinfo.name,szDrive,szDir,szFName,szExt);
							char szExt2[8];

							//added to make case insensitive DGP 11 Jan 2008
							unsigned int ui;
							for (ui = 0; ui < strlen(szExt); ui++)
								szExt[ui] = toupper(szExt[ui]);
							szExt[ui] = NULL;

							for (int index = 0; index < iNumberOfFileExtensions; index++)
							{	
								//added to make case insensitive  DGP 11 Jan 2008
								for (ui = 0; ui < strlen(pAt); ui++)
									szExt2[ui] = toupper(pAt[ui]);
								szExt2[ui] = NULL;
								if(strcmp(szExt2,&szExt[1])==0)
								{
									bHaveExtensionMatch = true;
									break;//out of the for loop
								}
								length = strlen(pAt);
								pAt = pAt + length + 1;
							}

							if ((archive != normal) && bHaveExtensionMatch)
							{
								addedto	= true;
							}
							else
							{
								include = false;
							}
						}

						char *p = NULL;
						char *pAt = pFileExtensionList;
						bool bHaveExtensionMatch = false;
						int length;

						char szDrive[_MAX_DRIVE];
						char szDir[_MAX_DIR];
						char szFName[_MAX_FNAME];
						char szExt[_MAX_EXT];
						_splitpath(fileinfo.name,szDrive,szDir,szFName,szExt);
						char szExt2[8];
						bool bNotAShortFileName = strlen(szFName)>8;  // JFL: forces selection pattern to be "%t%i_%y %m %d" in all cases, must be a bug, dunno how it got here!

						//added to make case insensitive DGP 11 Jan 2008
						unsigned int ui;
						for (ui = 0; ui < strlen(szExt); ui++)
							szExt[ui] = toupper(szExt[ui]);
						szExt[ui] = NULL;

						for (int index = 0; index < iNumberOfFileExtensions; index++)
						{								
							//added to make case insensitive DGP 11 Jan 2008
							for (ui = 0; ui < strlen(pAt); ui++)
								szExt2[ui] = toupper(pAt[ui]);
							szExt2[ui] = NULL;
							if(strcmp(szExt2,&szExt[1])==0)//modify to be case insensitive (JFL: but why not stricmp used?)
							{
								bHaveExtensionMatch = true;
								break;
							}
							length = strlen(pAt);
							pAt = pAt + length + 1;
						}


						if  ((include) && bHaveExtensionMatch)	
							//if  ((include) && (strstr(fileinfo.name,pFileExtensionList)))
						{
							//going to include it so
							//build the date to place in column 1
							strcat(buff,fileinfo.name);
							strtok(buff,".");
							if (buff[0] == '.') buff[0] = NULL;
							if (strlen(buff)<=0)
								continue;

							int Year = 0, Month = 0, Day = 0;
							GetYMDFromName(fileinfo.name, bNotAShortFileName, &Year, &Month, &Day);

							if (Year == 0 || Month == 0 || Day == 0) // && !m_bShowAllSuffixMatches)
							{
								// skipping this file because file name format fails to parse to a usable date
							}
							else
							{
								csName.Format("%04d\\%02d\\%02d",Year, Month, Day);

								pFindInfo.flags = LVFI_STRING;
								pFindInfo.psz = LPCTSTR(csName);
								int inewitem;
								LV_ITEM lvitem;

								//add the number of clusters -- needed to 
								//figure the number of media showned in the cstatic
								TotalClusters += ((fileinfo.size + (addedto?FileInfoArchive.nFileSizeLow:0))/BytesPerCluster + 1);
								//see if we already have an entry in the list
								if ((inewitem = m_pFileList.FindItem(&pFindInfo))<0)
								{
									//new item
									//so create it
									lvitem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
									lvitem.iItem = 0;
									lvitem.iSubItem = 0;
									lvitem.pszText = (char *)LPCTSTR(csName); 
									lvitem.iImage = addedto?8:1;
									lvitem.lParam  = ((fileinfo.size + (addedto?FileInfoArchive.nFileSizeLow:0))/BytesPerCluster + 1);
									inewitem = m_pFileList.InsertItem(&lvitem);
									//create the second column entry
									lvitem.mask = LVIF_TEXT;
									lvitem.iItem = inewitem;
									lvitem.iSubItem = 1;
									lvitem.iImage = 0;
									lvitem.pszText = (addedto)?Yes:No;
									m_pFileList.SetItem(&lvitem);
									//modify the nth column to show the number of files
									lvitem.mask = LVIF_TEXT;
									lvitem.iItem = inewitem;
									lvitem.iSubItem = NumberOfColumns-1;
									lvitem.pszText = "1/1";
									m_pFileList.SetItem(&lvitem);

									nItem++;
								} 
								else
								{
									//existing item may need changed
									lvitem.mask = LVIF_PARAM;
									lvitem.iItem = inewitem;
									lvitem.iSubItem = 0;
									m_pFileList.GetItem(&lvitem);
									csCurrent = m_pFileList.GetItemText(inewitem,NumberOfColumns-1);
									//modify the instrument column to show the # of files
									int iCurrent = atoi(LPCTSTR(csCurrent));
									iCurrent++; 
									csCurrent.Format("%d/%d",iCurrent,iCurrent);
									//update the number of clusters and the icon
									if (addedto)
										lvitem.mask = LVIF_IMAGE | LVIF_PARAM;
									else
										lvitem.mask = LVIF_PARAM;
									lvitem.iItem = inewitem;
									lvitem.iSubItem = 0;
									lvitem.iImage = 8;
									lvitem.lParam += ((fileinfo.size + ((addedto)?FileInfoArchive.nFileSizeLow:0))/BytesPerCluster + 1);
									m_pFileList.SetItem(&lvitem);

									//change the string in the nth column
									lvitem.mask = LVIF_TEXT;
									lvitem.iItem = inewitem;
									lvitem.iSubItem = NumberOfColumns-1;
									lvitem.iImage = 0;
									lvitem.lParam = iCurrent;
									lvitem.pszText = (char*)LPCTSTR(csCurrent);
									m_pFileList.SetItem(&lvitem);

									//if added to then change the second column
									if (addedto)
									{
										lvitem.mask = LVIF_TEXT;
										lvitem.iItem = inewitem;
										lvitem.iSubItem = 1;
										lvitem.pszText = Yes;
										m_pFileList.SetItem(&lvitem);
									}
								}
							}
						}
					}
					result = _findnext(hFile, &fileinfo);
				}
				_findclose(hFile);
			}	
			if (pFileExtensionList != NULL)
			{
				free (pFileExtensionList);
				pFileExtensionList = NULL;
			}
		}
		catch(  ... )
		{
			// could emit log here
		}
	}

	//set the cstatic to show how many media needed
	CString form;
	CString Result;
	GetDlgItemText(IDC_TOTALMEDIA_STATIC,form);
	int TotalMedia = TotalClusters/m_dTotalNumberOfClusters + 1;
	Result.Format(form,TotalMedia);
	SetDlgItemText(IDC_TOTALMEDIA_STATIC,Result);

	//show how many file sets total and selected
	SetDlgItemInt(IDC_TOTAL_STATIC,nItem);
	SetDlgItemInt(IDC_SELECTED_STATIC,nItem);
	m_dTotal	= nItem;
	m_dSelected = nItem;
	m_bDoneSetup = true;

	//set the last inspection date
	SetDlgItemText(IDC_PREVIOUS_STATIC,m_pPrevious);

	//flush all NT io buffers (we are going to be reading/moving files)
	_flushall();

	if (m_dTotal <=0)
		GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(false);
	else
		GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CDoFileCopy::DoModal(CString Destination,CString Previous, CString DateTime,CString InspectorName,CString InspectorID,CString Inspection) 
{	// TODO: Add your specialized code here and/or call the base class
	//if removable drive then we need to know that so that 
	//we can ask the user to remove the drive -- after all
	//we don't want to ask the user to remove an internal 
	//non-removable hard drive!
	DWORD Type = GetDriveType(Destination);
	if ((Type == DRIVE_REMOVABLE) ||	//all removables
		(Type == DRIVE_CDROM) ||		//don't know how cd-rws are reported
		(Type == DRIVE_RAMDISK) ||		//pcmcia card may be considered a ramdisk
		(Type == DRIVE_UNKNOWN) )		//who knows?
		m_bRemovable = true;
	else
		m_bRemovable = false;
	if (Destination.Right(1) != '\\') Destination += "\\";
	m_pDestination	= Destination;	
	m_pPrevious		= Previous;
	m_pDateTime		= DateTime;
	m_pDestination	= Destination;
	m_pDateTime		= DateTime;
	m_pInspectorName = InspectorName;
	m_pInspectorID	= InspectorID;
	m_pInspection	= Inspection;
	return CDialog::DoModal();
}

BOOL CDoFileCopy::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	((CListCtrl*)GetDlgItem(IDC_FILE_LIST))->SetImageList(NULL,LVSIL_NORMAL);

	return CDialog::DestroyWindow();
}

HBRUSH CDoFileCopy::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO: Change any attributes of the DC here
	UINT index = pWnd->GetDlgCtrlID();
	if ((index == IDC_TOTAL_STATIC) ||
		(index == IDC_SELECTED_STATIC) ||
		(index == IDC_CAUTION_STATIC))
	{
		if (m_dTotal != m_dSelected)

		{
			pDC->SetBkColor(COLOR_RED);
			pDC->SetTextColor(COLOR_WHITE);
		} 
		else
		{
			pDC->SetTextColor(COLOR_BLACK);
		}
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);

	}
	else if (index==IDC_TOTALMEDIA_STATIC)
		pDC->SelectStockObject(SYSTEM_FIXED_FONT);
	//	else
	//		pDC->SetTextColor(COLOR_DEFAULT_TEXT);

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDoFileCopy::OnClickFileList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_dToClose = 0;
	int selectcount = m_pFileList.GetSelectedCount();
	if (selectcount)
	{
		m_dSelected = m_dTotal-selectcount;
		SetDlgItemInt(IDC_SELECTED_STATIC,m_dSelected);
		if (m_dSelected <=0)
			GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(false);
		else
			GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(true);
		SetDlgItemText(IDC_CAUTION_STATIC,
			">> CAUTION <<\n One or more \n    files    \nEXCLUDED from\n    copy!    ");
	} 
	else
	{
		m_dSelected = m_dTotal;
		SetDlgItemInt(IDC_SELECTED_STATIC,m_dSelected);
		GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(true);
		SetDlgItemText(IDC_CAUTION_STATIC,"All file sets currently selected for copy.");
	}

	DWORD TotalClusters = 0;
	LV_ITEM lvitem;

	//for each "non" selected dates
	if (m_bDoneSetup) 
	{
		for (int item = 0; item < m_dTotal; item++)
		{
			//if item is selected
			lvitem.mask = LVIF_PARAM | LVIF_STATE;
			lvitem.stateMask = LVIS_SELECTED;
			lvitem.iItem = item;
			lvitem.iSubItem = 0;
			m_pFileList.GetItem(&lvitem);
			if (~lvitem.state & LVIS_SELECTED)
			{
				TotalClusters += lvitem.lParam;
			}
		}
		int TotalMedia = TotalClusters / m_dTotalNumberOfClusters + 1;
		if (((TotalMedia-1) * m_dTotalNumberOfClusters) == TotalClusters)
			TotalMedia = 0;
		CString Result;
		Result.Format("Approximately %d empty media needed.",TotalMedia);
		SetDlgItemText(IDC_TOTALMEDIA_STATIC,Result);
	}
	*pResult = 0;
}

void CDoFileCopy::OnUndoButton() 
{
	// TODO: Add your control notification handler code here
	m_dToClose = 0;
	for (int i = 0; i < m_dTotal; i++)
		m_pFileList.SetItemState(i, NULL, LVIS_SELECTED);
	m_dSelected = m_dTotal;
	SetDlgItemInt(IDC_SELECTED_STATIC,m_dSelected);
	if (m_dSelected <=0)
		GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(false);
	else
		GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(true);
	SetDlgItemText(IDC_CAUTION_STATIC,"All file sets currently selected for copy.");
}


void CDoFileCopy::OnColumnclickFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	m_dToClose = 0;
	m_bDirectionUp = !m_bDirectionUp;
	m_dSortedCol = phdn->iButton;
	m_bDoingSort = true;
	SortTextItems( m_dSortedCol, m_bDirectionUp );
	m_bDoingSort = false;
	*pResult = 0;
}


BOOL CDoFileCopy::SortTextItems( int nCol, BOOL bAscending, 
								int low /*= 0*/, int high /*= -1*/ )
{
	if( nCol >= ((CHeaderCtrl*)m_pFileList.GetDlgItem(0))->GetItemCount() )
		return FALSE;
	if( high == -1 ) high = m_pFileList.GetItemCount() - 1;


	int lo = low;
	int hi = high;
	CString midItem;

	if( hi <= lo ) return FALSE;

	midItem = m_pFileList.GetItemText( (lo+hi)/2, nCol );

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && ( m_pFileList.GetItemText(lo, nCol) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( m_pFileList.GetItemText(lo, nCol) > midItem ) )
				++lo;

		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && ( m_pFileList.GetItemText(hi, nCol) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( m_pFileList.GetItemText(hi, nCol) < midItem ) )
				--hi;

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( m_pFileList.GetItemText(lo, nCol) != m_pFileList.GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = 
					((CHeaderCtrl*)m_pFileList.GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = m_pFileList.GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
					LVIS_FOCUSED |  LVIS_SELECTED | 
					LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				m_pFileList.GetItem( &lvitemlo );
				m_pFileList.GetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					m_pFileList.SetItemText(lo, i, m_pFileList.GetItemText(hi, i));

				lvitemhi.iItem = lo;
				m_pFileList.SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					m_pFileList.SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				m_pFileList.SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

	return TRUE;
}



void CDoFileCopy::OnItemchangedFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (!m_bDoingSort) 
		OnClickFileList(pNMHDR,pResult) ;
	*pResult = 0;
}

void CDoFileCopy::PrintINISection(char *szSection, FILE*outfile)
{
	fprintf(outfile,"[%s]\n",szSection);
	char buff[32768];
	if (GetPrivateProfileSection(szSection,buff,
		sizeof(buff), g_szIniFile)>0)
	{
		char *at = buff;
		while (strlen(at) > 0)
		{
			fprintf(outfile,"  %s\n",at);
			at = at + strlen(at)+1;
		}
	}
	fprintf(outfile,"\n");
}

void CDoFileCopy::OnBegincopyButton() 
{
	//get the list in chronological order (column 1, ascending)
	LV_ITEM lvitem;
	CString Path(m_pDestination);
	//	CString PathInstMnx;
	CString PathInstSafe;
	CString Temp;
	int i;

	m_hSelectState = m_hIconGreen;
	((CStatic*)GetDlgItem(IDC_ICON_5))->SetIcon(m_hSelectState);

	GetDlgItem(IDC_BEGINCOPY_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_UNDO_BUTTON)->EnableWindow(false);
	//	GetDlgItem(IDC_ADVANCED_BUTTON)->EnableWindow(false);

	m_hVerifyState = m_hIconRed;
	((CStatic*)GetDlgItem(IDC_ICON_8))->SetIcon(m_hVerifyState);

	GetDlgItem(IDOK)->EnableWindow(false);
	m_bSelectionChangable = false;

	COleDateTime Today = COleDateTime::GetCurrentTime();
	CString Date;
	char Previous[64];
	GetPrivateProfileString(CONFIGURATION,IPDATE,"NONE",Previous,sizeof(Previous),g_szIniFile);
	//	if (g_bInIAEA)
	//		Date = Today.Format("%b%Y%d");
	//	else
	Date = Today.Format("%Y_%b_%d_%H%M");

	if (m_bFirstPass)
	{
		char path_buffer[_MAX_PATH];   
		char drive[_MAX_DRIVE];   
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];   
		char ext[_MAX_EXT];
		_splitpath( g_szIniFile, drive, dir, fname, ext );		

		m_pIPName = g_sFacilityName;
		CleanUpName(&m_pIPName);
		m_pIPName += "_";
		m_pIPName += Date;

		m_csDLGName = m_pIPName;

		_makepath( path_buffer, drive, dir, m_pIPName, ".ip" );
		m_pIPName = path_buffer;
		m_pIPName += '\0';
		m_pIPFile = fopen(m_pIPName,"w");
		_makepath( path_buffer, drive, dir, m_csDLGName, ".dlg");
		m_csDLGName = path_buffer;
		m_csDLGName += '\0';
		m_pDLGFile = fopen(m_csDLGName,"w");

		fprintf(m_pIPFile,"[GENERAL]\n");
		fprintf(m_pIPFile,"FACILITY=%s\n",g_sFacilityName);
		fprintf(m_pIPFile,"PREVIOUS=%s\n",Previous);
		fprintf(m_pIPFile,"DATE_TIME=%s\n",m_pDateTime.GetString()); // GJK
		fprintf(m_pIPFile,"INSPECTOR_NAME=%s\n",m_pInspectorName.GetString()); // GJK
		fprintf(m_pIPFile,"INSPECTOR_ID=%s\n",m_pInspectorID.GetString()); // GJK
		fprintf(m_pIPFile,"INSPECTION_NUMBER=%s\n",m_pInspection.GetString()); // GJK
		fprintf(m_pIPFile,"MIC_VERSION=%s\n",g_csVersion.GetString()); // GJK

		BSTR bstrID, bstrName, bstrDiscoveryType;
		char cID[_MAX_PATH], cName[_MAX_PATH];

		char cdiscovery_type[64];

		for (int i = 0, j=1; i < g_iInstCount; i++)
		{
			memset(cdiscovery_type,0,sizeof(cdiscovery_type));

			if ((g_eISOCategory[i] == STANDARD_ISO) ||
				(g_eISOCategory[i] == UPS_ISO))
			{
				g_pIISO[i]->get_ISOTypeString(&bstrDiscoveryType);
				Convert_BSTR_to_Character_String(
					bstrDiscoveryType,
					cdiscovery_type,
					sizeof(cdiscovery_type));
			}
			if (0 != strlen(cdiscovery_type))
			{
				g_pIISO[i]->get_ID(&bstrID);
				Convert_BSTR_to_Character_String(bstrID,cID,sizeof(cID));

				g_pIISO[i]->get_Name(&bstrName);
				Convert_BSTR_to_Character_String(bstrName,cName,sizeof(cName));

				fprintf(m_pIPFile,"INSTRUMENT%d=%s,%s,%s\n",j,cdiscovery_type,cID,cName);
			}
			j++;

		}  //end for

		fprintf(m_pIPFile,"\n");

		fprintf(m_pIPFile,"[FILES]\n");
		m_dEntryCount = 1;
	}
	else
	{
		CString temp;
		CString Current;
		int Curr, Total;
		//step through all date entries and reset the number of files for each instrument
		for (int item = 0; item < m_dTotal; item++)
		{
			//for each instrument
			for (int inst = 0; inst < g_iInstCount; inst++)
			{
				Current = m_pFileList.GetItemText(item,inst+2);
				//try {
				sscanf(Current,"%d/%d",&Curr,&Total);
				//	}
				//catch(...)
				//	{
				//		AfxMessageBox("Error at 4",MB_OK);
				//		AfxMessageBox(Current,MB_OK);
				//	}
				if ((Total > 0)&&(!Current.IsEmpty()))
				{
					temp.Format("%d/%d",Total,Total);
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = item;
					lvitem.iSubItem = 2 + inst;
					lvitem.pszText = (char*)LPCTSTR(temp);
					m_pFileList.SetItem(&lvitem);
				}
			}

		}

	}

	//get drive information
	char szDrive[8];
	strcpy(szDrive,"A:\\");
	for (i = 0; i < 26; i++)
	{
		szDrive[0] = (char)('A'+i);
		UINT uiType = GetDriveType(szDrive);
		if (uiType != DRIVE_NO_ROOT_DIR)
		{
			ULONG64  lFreeBytesAvailable;
			ULONG64  lTotalNumberOfBytes;
			ULONG64  lTotalNumberOfFreeBytes;
			if (GetDiskFreeSpaceEx(szDrive,(union _ULARGE_INTEGER *)&lFreeBytesAvailable,(union _ULARGE_INTEGER *)&lTotalNumberOfBytes,(union _ULARGE_INTEGER *)&lTotalNumberOfFreeBytes))
				fprintf(m_pDLGFile,"%s, "
				"Total Number Of Free Bytes: %12I64u, "
				"Total Number Of Bytes: %12I64u\n",
				szDrive,
				lTotalNumberOfFreeBytes,
				lTotalNumberOfBytes);
			else
			{
				fprintf(m_pDLGFile,"%s, Access Drive Failed ",szDrive);
				if ((uiType == DRIVE_REMOVABLE) || (uiType == DRIVE_CDROM))
					fprintf(m_pDLGFile,"Possibly no disk in removable drive.");
				fprintf(m_pDLGFile,"\n");
			}
		}
	}
	fprintf(m_pDLGFile,"\n");

	//get the MIC configuration
	fprintf(m_pDLGFile,"Multi-Instrument Collect Configuration (from MIC.INI file)\n");

	PrintINISection(CONFIGURATION,m_pDLGFile);
	PrintINISection(USERS,m_pDLGFile);
	PrintINISection(SUPERUSERS,m_pDLGFile);
	PrintINISection(NETWORK,m_pDLGFile);
	PrintINISection(COMMUNICATIONS,m_pDLGFile);
	PrintINISection(INSTRUMENTS,m_pDLGFile);
	fprintf(m_pDLGFile,"\n");

	fclose(m_pDLGFile);  //<<QA>> added for MIC_GM

	//for each instrument
	bool bContinue = true;
	int Inst = 0;

	BSTR bstrSaveLoc, bstrFileExtensions, bstrID, bstrName, bstrIssuePrintFilename;
	char cSaveLoc[_MAX_PATH], cID[_MAX_PATH];
	CString csName;

	char *pFileExtensionList = NULL;
	int  iNumberOfFileExtensions;

	Convert_CString_to_BSTR(m_csDLGName,bstrIssuePrintFilename);

	for (i = 0; (i < g_iInstCount) && bContinue; i++)
	{
		m_dToClose = 0;

		if (g_pIISO[i])
		{
			//get the location it is saving its files to
			g_pIISO[i]->get_LocationAndExtensionsForDataFiles(
				&bstrSaveLoc,
				&bstrFileExtensions);

			Convert_BSTR_to_Character_String(bstrSaveLoc,cSaveLoc,sizeof(cSaveLoc));

			if ((strlen(cSaveLoc) != 0) && (g_eISOCategory[i] != WATCH_ISO))

				//if (((Location = g_pIISO[i]->GetSaveLoc())!=NULL) &&
				//	(g_dInstType[i] != WATCH) )
			{

				if (pFileExtensionList != NULL)
				{
					free (pFileExtensionList);
					pFileExtensionList = NULL;
				}

				UnpackFileExtensionsList(
					bstrFileExtensions,
					&pFileExtensionList,
					&iNumberOfFileExtensions);

				g_pIISO[i]->AppendGUIValues(bstrIssuePrintFilename);

				//g_pIISO[i]->IssuePrint(m_pDLGFile);

				//if iaea build iaea destination pfm path(s)
				//				if (g_bInIAEA)
				//				{
				g_pIISO[i]->get_Name(&bstrName);
				Convert_BSTR_to_CString(bstrName,csName);

				PathInstSafe = Path;
				Temp = g_sFacilityName;
				Temp += "_";
				Temp += csName;//g_pIISO[i]->GetName();
				CleanUpName(&Temp);
				PathInstSafe += Temp;
				PathInstSafe += "_";
				//					PathInstSafe = PathInstMnx;
				//					PathInstMnx += "MN_";
				//					PathInstSafe+= "SG_";
				//					PathInstMnx += Date;
				PathInstSafe+= Date;
				//				}
				//				else
				//				//else build "normal" destination pfm path(s)
				//				{
				//					PathInstMnx = Path;
				//					Temp = g_sFacilityName;
				//					Temp += "_";
				//					CleanUpName(&Temp);
				//					PathInstMnx += Date;
				//					PathInstSafe = PathInstMnx;
				//				}

				g_pIISO[i]->get_ID(&bstrID);
				Convert_BSTR_to_Character_String(bstrID,cID,sizeof(cID));

				char * Id = cID;
				char *suffix = pFileExtensionList;
				int suffix_length;

				for (int index = 0; index < iNumberOfFileExtensions; index++)
				{
					suffix_length = strlen(suffix);
					bContinue = DoCopy(suffix,&PathInstSafe,cSaveLoc,Id,Inst);
					if (!bContinue)
						break;
					suffix = suffix + suffix_length + 1;
				}

				Inst++;
			}
			else
			{
				//if (g_dInstType[i] == APC_UPS)
				//	((CAPCInst*)g_pIISO[i])->IssuePrint(m_pDLGFile);
				//else if (g_dInstType[i] == AQUILA_UPS)
				//	((CAQUILAInst*)g_pIISO[i])->IssuePrint(m_pDLGFile);

				if (g_eISOCategory[i] == UPS_ISO)
					g_pIISO[i]->AppendGUIValues(bstrIssuePrintFilename);
			}
		}
	}

	if (pFileExtensionList != NULL)
	{
		free (pFileExtensionList);
		pFileExtensionList = NULL;
	}

	fprintf(m_pIPFile,"%d=END\n",m_dEntryCount++);
	fclose (m_pIPFile);
	fclose (m_pDLGFile);
	if (bContinue)
	{
		DoCopyAFile(m_pIPName);
		DoCopyAFile(m_csDLGName);
	}
	else
	{
		DeleteFile(m_pIPName);
		DeleteFile(m_csDLGName);
	}

	if (bContinue)
	{
		WritePrivateProfileString(CONFIGURATION,IPDATE,m_pDateTime,g_szIniFile);
		m_bFinishState = 1;
		if (m_bRemovable)
		{
			//ask the user to remove the media
			//make sure it is ok first
			_flushall(); //COMMODE.OBJ must be linked in also	//DR00175
			char szDrive[4];
			strncpy(szDrive,m_pDestination,2);
			szDrive[2] = NULL;
			Eject(szDrive);
			m_dNewMediaState = 0;
			m_hRemoveState = m_hIconYellow1;
			((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
		}
		else
			//continue as though media has been removed
			m_dNewMediaState = 1;
		//allow another copy
		SetDlgItemText(IDC_BEGINCOPY_BUTTON,"Begin Another Copy");
		//but don't allow the file selection to change
		m_bFirstPass = false;
	} 
	else
	{
		OnCancel();
	}
}

bool CDoFileCopy::FileMkDir(CString FullPath)
{
	if (_mkdir(FullPath)==-1)
	{
		if ((errno == EACCES)||(errno == EEXIST)) return false;
		char path_buffer[_MAX_PATH];   
		char drive[_MAX_DRIVE];   
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];   
		char ext[_MAX_EXT];
		_splitpath( FullPath, drive, dir, fname, ext ); 
		_makepath(path_buffer,drive,dir,NULL,NULL);
		CString PartPath(path_buffer);
		return FileMkDir(PartPath);
	}
	return true;
}


//this function should go into the parent class
void CDoFileCopy::BuildLongFileName(CString &cResult,
									CString cFormat,CString cPath,CString cType,
									CString cID,
									int iYear,	int iMonth,	int iDay,
									//	int iHour,	int iMinute,int iSecond,
									CString cSuffix
									)
{
	cResult.Empty();
	if (cPath.GetLength())
	{
		cResult = cPath;
		if (cResult.Right(1) != '\\')
			cResult += '\\';
	}
	CString cTemp;
	bool bHaveEscape = false;
	for (int iSourceIndex = 0; iSourceIndex < cFormat.GetLength(); iSourceIndex++)
	{
		if ((cFormat[iSourceIndex]=='%') && (!bHaveEscape))
			bHaveEscape = true;
		else if (bHaveEscape)
		{
			bHaveEscape = false;
			switch (cFormat[iSourceIndex]) {
case 'T':
	cResult += cType;
	break;
case 'I':
	{
		int iLength = cID.GetLength();
		while (iLength < 3)
		{
			cResult += '0';
			iLength++;
		}
		cResult += cID;
	}
	break;
case 'i':
	{
		int iLength = cID.GetLength();
		while (iLength < 2)
		{
			cResult += '0';
			iLength++;
		}
		cResult += cID;
	}
	break;
case 'y':
	cTemp.Format("%04d",iYear);
	cResult += cTemp;
	break;
case 'm':
	cTemp.Format("%02d",iMonth);
	cResult += cTemp;
	break;
case 'd':
	cTemp.Format("%02d",iDay);
	cResult += cTemp;
	break;
case 'H':
	//				cTemp.Format("%02d",iHour);
	//				cResult += cTemp;
	cResult += "??";
	break;
case 'M':
	//				cTemp.Format("%02d",iMinute);
	//				cResult += cTemp;
	cResult += "??";
	break;
case 'S':
	//				cTemp.Format("%02d",iSecond);
	//				cResult += cTemp;
	cResult += "??";
	break;
default:
	cResult += cFormat[iSourceIndex];
			}
		}
		else
			cResult += cFormat[iSourceIndex];
	}
	if (cSuffix.GetLength())
	{
		if ((cResult.Right(1) != '.') && (cSuffix.Left(1) != '.'))
			cResult += '.';
		cResult += cSuffix;
	}
}

bool CDoFileCopy::DoCopy(char* Suffix, CString* FullPath,char* SourceDir,char*Id,int Inst)
{
	//get the list in chronological order (column 1, ascending)
	SortTextItems( 0, TRUE);
	//get a pointer to list
	LV_ITEM lvitem;
	CString OutFilePathNameExt;
	CString InFilePathNameExt;
	CString InFileArchivePathNameExt;
	CString Current;
	CString OutDate;

	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	DWORD BytesPerCluster;
	//	DWORD BytesWritten = 0;

	CHideDlg WaitForDisk(this);

	SHFILEOPSTRUCT FileOp;

	char Date[4];
	bool WroteArchive;

	CString NewDestination;
	char ddrive[_MAX_DRIVE+1];   
	char ddir[_MAX_DIR];
	char dfname[_MAX_FNAME];   
	char dext[_MAX_EXT];
	_splitpath( m_pDestination, ddrive, ddir, dfname, dext );		
	if (strlen(ddrive)==0)
		NewDestination = ddir;
	else
	{
		strcat(ddrive,"\\");
		NewDestination = ddrive;
	}

	FileMkDir(*FullPath);
	//for each date in list box
	for (int EachDate = 0; EachDate < m_dTotal; EachDate++)
	{
		//if "non" selected then do the file transfers
		//need all of the information
		lvitem.mask = LVIF_PARAM | LVIF_STATE | LVIF_IMAGE;
		//including only the selected state
		lvitem.stateMask = LVIS_SELECTED;
		//control's nth entry
		lvitem.iItem = EachDate;
		//don't care about subitems
		lvitem.iSubItem = 0;
		//get the item from the list
		m_pFileList.GetItem(&lvitem);

		int Curr, All;
		Current = m_pFileList.GetItemText(EachDate,Inst+2);
		if (Current.GetLength() == 0)
			Curr = All = 0;
		else
		{
			//try {
			sscanf(Current,"%d/%d",&Curr,&All);
			//	}
			//catch(...)
			//	{
			//		AfxMessageBox("Error at 5",MB_OK);
			//		AfxMessageBox(Current,MB_OK);
			//	}
		}

		WroteArchive = false;
		//if not selected state
		if ((~lvitem.state & LVIS_SELECTED) && (All > 0))
		{
			//save current list icon
			//set list icon to "working"
			//build partial file name date
			char Source[32];
			int Year=0, Month=0, Day=0;
			m_pFileList.GetItemText(EachDate,0,Source,sizeof(Source));

			//try {
			sscanf(Source,"%4d\\%2d\\%2d",&Year,&Month,&Day);
			//	}
			//catch(...)
			//	{
			//		AfxMessageBox("Error at 6",MB_OK);
			//		AfxMessageBox(Source,MB_OK);
			//	}
			if (m_bShortName)
			{
				sprintf(Date,"%c%c%c",
					((Year-1990)<10)?'0'+ Year-1990 :'A' + Year-2000,
					(Month     <10)?'0'+ Month     :'A' + Month-10,
					(Day       <10)?'0'+ Day       :'A' + Day  -10 );
				CString temp;

				InFilePathNameExt = SourceDir;
				InFilePathNameExt += "\\";
				InFileArchivePathNameExt = InFilePathNameExt;
				InFileArchivePathNameExt += "archive\\";
				temp = Id;
				temp += Date;
				//				temp += "a00.";
				temp += "???.";
				temp += Suffix;
				InFilePathNameExt += temp;
				InFilePathNameExt += '\0';
				InFileArchivePathNameExt += temp;
				InFileArchivePathNameExt += '\0';

				OutFilePathNameExt = *FullPath;
				OutFilePathNameExt += "\\";
				OutFilePathNameExt += temp;
				OutFilePathNameExt += '\0';
			}
			else
			{
				//build the InFilePathNameExt
				BuildLongFileName(InFilePathNameExt,
					m_csNamePattern,SourceDir+CString("\\"),"??",
					Id,
					Year,	Month,	Day,
					//					0,0,0,
					Suffix);
			}

			DWORD ClustersNeeded;
			struct _finddata_t fileinfo;
			long hFileFind; 
			//get the data on the file if it exists
			if ((hFileFind = _findfirst(InFilePathNameExt,&fileinfo))!=-1)
			{
				int iNotDone = 0;
				while (iNotDone == 0)
				{

					//re-build the InFilePathNameExt
					InFilePathNameExt = SourceDir;
					InFilePathNameExt += "\\";
					InFilePathNameExt += fileinfo.name;

					//build the InFileArchivePathNameExt
					InFileArchivePathNameExt = SourceDir;
					InFileArchivePathNameExt += CString("\\archive\\");
					InFileArchivePathNameExt += fileinfo.name;

					//build the OutFilePathNameExt
					OutFilePathNameExt = *FullPath;
					OutFilePathNameExt += "\\";
					OutFilePathNameExt += fileinfo.name;

					_splitpath(LPCTSTR(OutFilePathNameExt),ddrive,ddir,dfname,dext);
					//					_splitpath(LPCTSTR(NewDestination),ddrive,ddir,dfname,dext);
					strcat(ddrive,"\\");
					GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
						//				GetDiskFreeSpace(LPCTSTR(NewDestination),&SectorsPerCluster,&BytesPerSector,
						&NumberOfFreeClusters,&TotalNumberOfClusters);
					NumberOfFreeClusters--;
					BytesPerCluster = BytesPerSector * SectorsPerCluster;

					//					_findclose(hFileFind);
					lvitem.mask = LVIF_TEXT;
					lvitem.iItem = EachDate;
					lvitem.iSubItem = Inst + 2;
					lvitem.iImage = 0;
					Current.Format("%d/%d",--Curr,All);
					lvitem.pszText = (char*)LPCTSTR(Current);
					m_pFileList.SetItem(&lvitem);

					m_pFileList.EnsureVisible(EachDate,true);
					m_pFileList.UpdateWindow();

					struct _WIN32_FILE_ATTRIBUTE_DATA FileInfoArchive; 
					//if file exists in archive
					if (GetFileAttributesEx(InFileArchivePathNameExt,GetFileExInfoStandard ,&FileInfoArchive))
					{
						//file exists in archive subdirectory
						COleDateTime archive(FileInfoArchive.ftLastWriteTime);
						COleDateTime normal(fileinfo.time_write);
						if (archive < normal) 
						{
							WroteArchive = true;
							//if size of file > will fit on disk then 
							ClustersNeeded = FileInfoArchive.nFileSizeLow/BytesPerCluster +1;
							if (ClustersNeeded > TotalNumberOfClusters)
							{
								//	tell user he has a problem
								CString f("File ");
								f += InFileArchivePathNameExt;
								f += "\nWon't fit on this type of media!\nCompress and copy by hand!";
								m_bWaitingForMedia = true;
								MessageBox(f,"MIC: FileCopy",MB_OK | MB_ICONEXCLAMATION);
								m_dToClose = 0;
								m_bWaitingForMedia = false;
							}
							else
							{
								bool DidMediaChange = false;
								//	if file will fit on remaining disk space then
								m_bDoingSwap = true;
								while (ClustersNeeded > NumberOfFreeClusters)
								{
									DidMediaChange = true;
									m_dNewMediaState = 0;
									m_hRemoveState = m_hIconYellow1;
									((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
									m_hInsertState = m_hIconRed;
									((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
									m_bWaitingForMedia = true;
									if (WaitForDisk.DoModal() != IDOK)
									{
										m_bWaitingForMedia = false;
										m_dNewMediaState = 0;
										m_hRemoveState = m_hIconRed;
										m_hInsertState = m_hIconRed;
										((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
										((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
										return false;
									}
									else
									{
										m_bWaitingForMedia = false;
										_splitpath(LPCTSTR(NewDestination),ddrive,ddir,dfname,dext);
										strcat(ddrive,"\\");
										GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
											//									GetDiskFreeSpace(LPCTSTR(NewDestination),&SectorsPerCluster,&BytesPerSector,
											&NumberOfFreeClusters,&TotalNumberOfClusters);
										NumberOfFreeClusters--;
										BytesPerCluster = BytesPerSector * SectorsPerCluster;
										ClustersNeeded = FileInfoArchive.nFileSizeLow/BytesPerCluster + 1;
									}
								}
								m_bDoingSwap = false;
								if (DidMediaChange)
									FileMkDir(*FullPath);

								FileOp.hwnd = m_hWnd;
								FileOp.wFunc = FO_COPY;
								FileOp.pFrom = InFileArchivePathNameExt;
								FileOp.pTo = OutFilePathNameExt;
								FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;
								FileOp.fAnyOperationsAborted = false;
								FileOp.hNameMappings = NULL;
								FileOp.lpszProgressTitle = OutFilePathNameExt;
								SHFileOperation(&FileOp);
								if (FileOp.fAnyOperationsAborted)
									return false;

								fprintf(m_pIPFile,"%d=%s=COPYARCHIVEFILE\n",m_dEntryCount++,InFilePathNameExt.GetString()); // gjk
							}
						}
						_splitpath(LPCTSTR(NewDestination),ddrive,ddir,dfname,dext);
						strcat(ddrive,"\\");
						GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
							//					GetDiskFreeSpace(LPCTSTR(NewDestination),&SectorsPerCluster,&BytesPerSector,
							&NumberOfFreeClusters,&TotalNumberOfClusters);
						NumberOfFreeClusters--;
						BytesPerCluster = BytesPerSector * SectorsPerCluster;
					}

					//how much disk space do we need
					if (BytesPerCluster == 0) BytesPerCluster = 512;
					ClustersNeeded = fileinfo.size/BytesPerCluster + 1;
					//we will need to know if media changed so we can create the dir
					bool DidMediaChange = false;
					//repeat until user aborts or a large enough media is installed
					m_bDoingSwap = true;
					bool FileTooBig = false;				
					while ((ClustersNeeded > NumberOfFreeClusters) && (!FileTooBig))
					{
						//media must change now
						DidMediaChange = true;
						//set flag for timer to start changing icons
						m_dNewMediaState = 0;
						//show the user that we want the media removed
						m_hRemoveState = m_hIconYellow1;
						((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
						//and not to insert new media yet
						m_hInsertState = m_hIconRed;
						((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
						//wait for new media to be inserted and see if user wants to abort
						m_bWaitingForMedia = true;
						if (WaitForDisk.DoModal() != IDOK)
						{
							//going to abort
							//reset the icons to normal state
							m_bWaitingForMedia = false;
							m_dNewMediaState = 0;
							m_hRemoveState = m_hIconRed;
							m_hInsertState = m_hIconRed;
							((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
							((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
							return false;
						}
						else
						{
							m_bWaitingForMedia = false;
							//user clicked on ok OR dialog box saw a new diskette installed
							//figure the size and free of this media
							_splitpath(LPCTSTR(NewDestination),ddrive,ddir,dfname,dext);
							strcat(ddrive,"\\");
							GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
								//						GetDiskFreeSpace(LPCTSTR(NewDestination),&SectorsPerCluster,&BytesPerSector,
								&NumberOfFreeClusters,&TotalNumberOfClusters);
							NumberOfFreeClusters--;
							BytesPerCluster = BytesPerSector * SectorsPerCluster;
							//calculate the number of clusters needed for this type of media
							//recognize that some drives allow different size media
							ClustersNeeded = FileInfoArchive.nFileSizeLow/BytesPerCluster + 1;
							if (TotalNumberOfClusters < ClustersNeeded) FileTooBig = true;
						}
					}
					m_bDoingSwap = false;
					//bump the a00. so we and the user won't have a conflict
					if (FileTooBig)
					{
						fprintf(m_pIPFile,"%d=%s=FAILCOPY\n",m_dEntryCount++,InFilePathNameExt.GetString()); //gjk
						CString f("File ");
						f += InFileArchivePathNameExt;
						f += "\nWon't fit on this type of media!\nCompress and copy by hand!";
						m_bWaitingForMedia = true;
						MessageBox(f,"MIC: FileCopy",MB_OK | MB_ICONEXCLAMATION);
						m_dToClose = 0;
						m_bWaitingForMedia = false;

					}
					else
					{
						if (WroteArchive)
							OutFilePathNameExt.SetAt(OutFilePathNameExt.GetLength()-6,'1');
						//create the directory if we had to change media
						if (DidMediaChange)
							FileMkDir(*FullPath);
						//copy the file to the new media
						char InBuff[_MAX_PATH];
						char OutBuff[_MAX_PATH];
						memset(InBuff,0x00,sizeof(InBuff));
						memset(OutBuff,0x00,sizeof(OutBuff));
						strcpy(InBuff,InFilePathNameExt);
						strcpy(OutBuff,OutFilePathNameExt);
						FileOp.hwnd = m_hWnd;
						FileOp.wFunc = FO_COPY;
						FileOp.pFrom = InBuff;
						FileOp.pTo = OutBuff;
						FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;
						FileOp.fAnyOperationsAborted = false;
						FileOp.hNameMappings = NULL;
						FileOp.lpszProgressTitle = OutFilePathNameExt;
						SHFileOperation(&FileOp);
						if (FileOp.fAnyOperationsAborted)
							return false;
						fprintf(m_pIPFile,"%d=%s=COPYFILE\n",m_dEntryCount++,InFilePathNameExt.GetString()); // gjk
					}
					iNotDone = _findnext(hFileFind,&fileinfo);
				}
				_findclose(hFileFind);

			}//find first
		}//if selected
	}//for each date
	return true;
}

bool CDoFileCopy::DoCopyAFile(CString FullFileName)
{
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	DWORD BytesPerCluster;

	CHideDlg WaitForDisk(this);

	SHFILEOPSTRUCT FileOp;

	struct _WIN32_FILE_ATTRIBUTE_DATA FileInfo; 
	//if file exists in archive
	GetFileAttributesEx(FullFileName,GetFileExInfoStandard ,&FileInfo);

	char ddrive[_MAX_DRIVE+1];   
	char ddir[_MAX_DIR];
	char dfname[_MAX_FNAME];   
	char dext[_MAX_EXT];
	_splitpath( m_pDestination, ddrive, ddir, dfname, dext );		
	if (strlen(ddrive)==0)
	{
		GetDiskFreeSpace(NULL,&SectorsPerCluster,&BytesPerSector,
			&NumberOfFreeClusters,&TotalNumberOfClusters);
	}
	else
	{
		strcat(ddrive,"\\");
		GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
			&NumberOfFreeClusters,&TotalNumberOfClusters);
	}

	NumberOfFreeClusters--;
	BytesPerCluster = BytesPerSector * SectorsPerCluster;

	//how much disk space do we need
	if (BytesPerCluster == 0 ) BytesPerCluster = 1;
	DWORD ClustersNeeded = FileInfo.nFileSizeLow/BytesPerCluster + 1;
	//we will need to know if media changed so we can create the dir
	bool DidMediaChange = false;
	//repeat until user aborts or a large enough media is installed
	m_bDoingSwap = true;
	while (ClustersNeeded > NumberOfFreeClusters)
	{
		//media must change now
		DidMediaChange = true;
		//set flag for timer to start changing icons
		m_dNewMediaState = 0;
		//show the user that we want the media removed
		m_hRemoveState = m_hIconYellow1;
		((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
		//and not to insert new media yet
		m_hInsertState = m_hIconRed;
		((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
		//wait for new media to be inserted and see if user wants to abort
		m_bWaitingForMedia = true;
		if (WaitForDisk.DoModal() != IDOK)
		{
			m_bWaitingForMedia = false;
			//going to abort
			//reset the icons to normal state
			m_dNewMediaState = 0;
			m_hRemoveState = m_hIconRed;
			m_hInsertState = m_hIconRed;
			((CStatic*)GetDlgItem(IDC_ICON_7))->SetIcon(m_hRemoveState);
			((CStatic*)GetDlgItem(IDC_ICON_6))->SetIcon(m_hInsertState);
			return false;
		}
		else
		{
			m_bWaitingForMedia = false;
			//user clicked on ok OR dialog box saw a new diskette installed
			//figure the size and free of this media
			_splitpath( m_pDestination, ddrive, ddir, dfname, dext );		
			if (strlen(ddrive)==0)
			{
				GetDiskFreeSpace(NULL,&SectorsPerCluster,&BytesPerSector,
					&NumberOfFreeClusters,&TotalNumberOfClusters);
			}
			else
			{
				strcat(ddrive,"\\");
				GetDiskFreeSpace(ddrive,&SectorsPerCluster,&BytesPerSector,
					&NumberOfFreeClusters,&TotalNumberOfClusters);
			}
			NumberOfFreeClusters--;
			BytesPerCluster = BytesPerSector * SectorsPerCluster;
			//calculate the number of clusters needed for this type of media
			//recognize that some drives allow different size media
			ClustersNeeded = FileInfo.nFileSizeLow/BytesPerCluster + 1;
		}
	}
	m_bDoingSwap = false;

	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];   
	char ext[_MAX_EXT];
	_splitpath( FullFileName, drive, dir, fname, ext );		

	CString to(m_pDestination);
	if (to.Right(1) != "\\")
		to += "\\";
	to += fname;
	to += ext;
	to += '\0';

	FileOp.hwnd = m_hWnd;
	FileOp.wFunc = FO_COPY;
	FileOp.pFrom = FullFileName;
	FileOp.pTo = to;
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_SIMPLEPROGRESS;
	FileOp.fAnyOperationsAborted = false;
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = to;
	SHFileOperation(&FileOp);
	if (FileOp.fAnyOperationsAborted)
		return false;
	return true;
}

void CDoFileCopy::CleanUpName(CString* Name)
{
	int Result;
	while ((Result = Name->Find('\\'))!=-1)
		Name->SetAt(Result,'-');
	while ((Result = Name->Find(':'))!=-1)
		Name->SetAt(Result,'-');
	while ((Result = Name->Find('/'))!=-1)
		Name->SetAt(Result,'-');
	while ((Result = Name->Find('.'))!=-1)
		Name->SetAt(Result,'-');
	while ((Result = Name->Find(' '))!=-1)
		Name->SetAt(Result,'-');
}

void CDoFileCopy::OnOK() 
{
	// TODO: Add extra validation here
	DoMove();
	CDialog::OnOK();
}

void CDoFileCopy::OnItemchangingFileList(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if ((m_bSelectionChangable) || (m_bDoingSort))
		*pResult = 0;
	else
		*pResult = -1;
}

void CDoFileCopy::DoMove()
{
	char Key[8];
	int ikey = 0;
	char inFilePathNameExt[_MAX_PATH];
	char outFilePathNameExt[_MAX_PATH];
	char dumpFilePathNameExt[_MAX_PATH];
	CString ArchiveDir;

	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];   
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char Dir[_MAX_DIR];

	bool Done = false;
	while (!Done)
	{
		ikey++;
		_itoa(ikey,Key,10);
		GetPrivateProfileString("FILES",Key,"END",
			inFilePathNameExt,sizeof(inFilePathNameExt),m_pIPName);

		if (strcmp(inFilePathNameExt,"END")==0)
			Done = true;
		else
		{
			//check type of entr
			if ( (strstr(inFilePathNameExt,"=INFILE")!=0) | 
				(strstr(inFilePathNameExt,"=COPYFILE")!=0)	)
			{
				//build filename with "archive"
				strtok(inFilePathNameExt,"=");
				_splitpath(inFilePathNameExt,drive,dir,fname,ext);
				strcpy(Dir,dir);
				strcat(dir,"archive\\");
				ArchiveDir = drive;
				ArchiveDir += dir;
				if (_access(ArchiveDir,0)!=0)
					FileMkDir(ArchiveDir);
				_makepath(outFilePathNameExt,drive,dir,fname,ext);
				//move file
				MoveFileEx(inFilePathNameExt,outFilePathNameExt,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
				//do dump file of same name
				_makepath(dumpFilePathNameExt,drive,Dir,fname,DMP_SUFFIX);
				_makepath(outFilePathNameExt,drive,dir,fname,DMP_SUFFIX);
				MoveFileEx(dumpFilePathNameExt,outFilePathNameExt,
					MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH );
			}
		}
	}
}

void CDoFileCopy::OnCancel() 
{
	// TODO: Add extra cleanup here
	_fcloseall();
	CDialog::OnCancel();
}

void CDoFileCopy::OnTocloseButton() 
{
	//add 5 minutes (being bumped each 1/2 second)
	m_dToClose -= (BUMP_EXTEND * 2);
	if ((g_iDlgCloseMillisec/1000 - m_dToClose/2) > MAX_EXTEND) 
		m_dToClose = g_iDlgCloseMillisec/500-(MAX_EXTEND * 2);
}

void CDoFileCopy::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	m_dToClose = 0;
	CWnd* pWnd;
	if ((pWnd = ChildWindowFromPoint(point,CWP_ALL))!=NULL)
	{
		UINT index = pWnd->GetDlgCtrlID();
		CString cstitle("Action Indicator Icon");
		CString csmessage;
		csmessage.Empty();
		//CTimeMsgDlg MsgBox(this);
		KillTimer(1);
		switch(index)
		{
		case IDC_ICON_1:
		case IDC_ICON_2:
		case IDC_ICON_3:
		case IDC_ICON_4:
			csmessage = CString("This icon is not active on this screen");
			break;
		case IDC_ICON_5:
			csmessage = CString("This icon informs you when it is time to select files and click on begin copy");
			break;
		case IDC_ICON_7:
			csmessage = CString("This icon informs you when it is time to remove the destination media.");
			break;
		case IDC_ICON_6:
			csmessage = CString("This icon informs you when it is time to insert media.");
			break;
		case IDC_ICON_8:
			csmessage = CString("This icon informs you when the copy has completed and you may now:\nExit and Archive,\nExit with no Archive, or\nBegin Another Copy");
			break;
		}

		if (!csmessage.IsEmpty())
		{
			new CTimedMessageDialog(cstitle,csmessage,g_iDlgCloseMillisec/1000,this);
		}

		SetTimer(1,500,NULL);  //<<QA>> is this still needed?
	}
	CDialog::OnLButtonDown(nFlags, point);
}


void CDoFileCopy::UnpackFileExtensionsList(
	BSTR& bstrFileExtensions,
	char **pFileExtensionList,
	int  *iNumberOfFileExtensions)
{
	UINT length = 0;

	*pFileExtensionList = NULL;
	*iNumberOfFileExtensions = 0;

	if (bstrFileExtensions != NULL)
		length = SysStringLen(bstrFileExtensions);

	if (length != 0)
	{
		char *pch = (char *)malloc(length + 1);

		Convert_BSTR_to_Character_String(
			bstrFileExtensions,
			pch,
			length + 1);

		int comma_count = 0;
		char *pAt = pch;
		char *pcomma_location;

		do
		{
			pcomma_location = strchr(pAt,',');

			if (pcomma_location != NULL)
			{
				*pcomma_location = 0;
				pAt = pcomma_location + 1;
				comma_count++;
			}
		}
		while (pcomma_location != NULL);

		*pFileExtensionList = pch;
		*iNumberOfFileExtensions = comma_count + 1;
	}
}