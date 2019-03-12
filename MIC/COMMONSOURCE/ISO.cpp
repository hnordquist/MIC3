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
//ISO.cpp

#include "stdafx.h"
#include "ISO.h"
#include "INI_definitions.h"
#include "ISO_Definitions.h"
#include "version.h"
#include  <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CISO::CISO(char *inifile)
{
//	TRACE("CISO::CTR #1\n");
	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));
	InitializeVariables();
}

CISO::CISO()
{
//	TRACE("CISO::CTR #2\n");
	m_szIniFile[0] = 0;
	InitializeVariables();
}

void CISO::InitializeVariables()
{
	m_pValidationEvent  = NULL;
	m_pButton           = NULL;
	m_pComm             = NULL;

	m_hSpecialUPSCommunicationsHandle = 0;
	m_hValidationHandle = 0;
	m_hButtonKeeperHandle = 0;

	m_pID[0]            = 0;
	m_pName[0]          = 0;
	m_pType[0]          = 0;
	m_szUtilFile[0]     = 0;
	m_pFullName[0]      = 0;
	m_pPort[0]          = 0;

	m_csLastError        = "None";
	m_eCategory         = UNKNOWN_ISO;
	m_csLongNameFormatString.Empty();

	m_bGoodRegister     = false;
	m_bBlockWrite       = false;
	m_bInStartUp        = true;
	m_bInShutDown       = false;
	m_bPause            = true;
	m_bNormalShutdown   = true;
	m_bUseShortFilename = true;

	m_iCurrentBBM           = 0;
	m_iDlgCloseMilliseconds = 999000;
	m_iMaxRepetition        = 9999;
	m_iNode                 = -1;
	m_iFlagTime             = -1;
	m_iFlagOthr             = -1;
	m_iFlagComm             = -1;
	m_iPauseMax				= 30000;

	m_szFileExtensionsList[0] = 0;
	m_szSaveLoc[0] = 0;

	//SCR 118
	CVersionInfo VersionInfo;
	m_csVersion = VersionInfo.FileVersion();
	m_csVersion.Remove(' ');
	m_csVersion.Replace(',','.');
	CString Name = VersionInfo.InternalName();
	m_csNameVersionDate = Name + " " + m_csVersion + " " + __DATE__;
}

CISO::~CISO()
{
//	TRACE("CISO::DTOR(%s) - NOOP\n", m_pName);
	//TRACE("%s\n",m_szFileExtensionsList);
}


void CISO::BlockWrite(bool blockwrite)
{
	m_bBlockWrite = blockwrite;
	m_pButton->put_Flash(blockwrite ? VARIANT_TRUE : VARIANT_FALSE);
	m_pButton->ShowIcon_Disk(blockwrite ? VARIANT_TRUE : VARIANT_FALSE);
}

void CISO::PostToWatch(char *pMessage)
{
	if ((NULL != pMessage) && (NULL != m_pComm))
	{
		BSTR bstr_out;
		Convert_Character_String_to_BSTR(pMessage, bstr_out);
		m_pComm->SendToWatch(bstr_out, ULONG(m_iNode));
	}
}

void CISO::PutINIPathname(char *inifile)
{
	strncpy(m_szIniFile,inifile,sizeof(m_szIniFile));
}
void CISO::PutUTILPathname(char *utilfile)
{
	strncpy(m_szUtilFile,utilfile,sizeof(m_szUtilFile));
}
void CISO::PutName(char *name)
{
	strncpy(m_pName,name,sizeof(m_pName));
}

int CISO::COM_Transmit(int ilength,char *output,int Node, bool Quiet)
{
	//TRACE("CISO::COM_Transmit(ilength = %d, output = %s)\n", ilength, output);

	BLOB blob;
	blob.cbSize = 1 + strlen(output);
	blob.pBlobData = (unsigned char *)output;

	ULONG byteswritten;
	VARIANT_BOOL vbquiet = Quiet ? VARIANT_TRUE : VARIANT_FALSE;
	HRESULT hr = m_pComm->Transmit((ULONG)ilength, blob, (ULONG)Node, vbquiet, &byteswritten);	
	return (int)byteswritten;
}

CString*  CISO::GetLastError()                     {return &m_csLastError;}
char*     CISO::GetID()                            {return m_pID;}
char*     CISO::GetSaveLoc()                       {return m_szSaveLoc;}
char*     CISO::GetFileExtensionsList()            {return m_szFileExtensionsList;}
void      CISO::PutICBPointer(ICB *picb)           {m_pButton = picb;}
void      CISO::PutICSOPointer(ICSO *picso)        {m_pComm = picso;}
char*     CISO::GetType()                          {return m_pType;}
char*     CISO::GetName()                          {return m_pName;}
int       CISO::GetCurrentBBM()                    {return m_iCurrentBBM;}
char*     CISO::GetFullName()                      {return m_pFullName;}
bool      CISO::InShutDown()                       {return m_bInShutDown;}
bool      CISO::InStartUp()                        {return m_bInStartUp;}
bool      CISO::IsPaused()                         {return m_bPause;}
void      CISO::PutDialogAutoCloseTime(int ms)     {m_iDlgCloseMilliseconds = ms;}
void      CISO::PutMaxRepetition(int maxreps)      {m_iMaxRepetition = maxreps;}
void      CISO::PutNormalShutdown(bool b)          {m_bNormalShutdown = b;}
void      CISO::PutUseShortFilename(bool b)        {m_bUseShortFilename = b;}
void      CISO::PutLongNameFormatString(CString cs){m_csLongNameFormatString = cs;}
void      CISO::GoDumb()                           {;}
void      CISO::Reconfig()                         {;}

ISO_FUNCTIONAL_CATEGORY CISO::GetISOCategory() {return m_eCategory;}

#ifdef ALLOWTRIGGERHOLDOFF
void		CISO::AddExclusion(LPARAM lParam){;}
void		CISO::DelExclusion(){;}
#endif

void CISO::FlagComm(int val, bool redraw)
{
	if (val != m_iFlagComm)
	{
		char buff[16];
		sprintf(buff,"%d",val);
		m_iFlagComm = val;
		WritePrivateProfileString(m_pName,FLAGCOMM,buff,m_szIniFile);
	}
	if (val == 1)
		m_pButton->ShowIcon_Error(VARIANT_TRUE);
	else
		m_pButton->ShowIcon_Error(VARIANT_FALSE);
	
}
void CISO::FlagOthr(int val, bool redraw)
{
	if (val != m_iFlagOthr)
	{
		char buff[16];
		sprintf(buff,"%d",val);
		m_iFlagOthr = val;
		WritePrivateProfileString(m_pName,FLAGOTHR,buff,m_szIniFile);
	}
	if (val == 1)
		m_pButton->ShowIcon_Frown(VARIANT_TRUE);
	else
		m_pButton->ShowIcon_Frown(VARIANT_FALSE);
}

void CISO::FlagTime(int val, bool redraw)
{
	if (val != m_iFlagTime)
	{
		char buff[16];
		sprintf(buff,"%d",val);
		m_iFlagTime = val;
		WritePrivateProfileString(m_pName,FLAGTIME,buff,m_szIniFile);
	}
	if (val == 1)
		m_pButton->ShowIcon_Clock(VARIANT_TRUE);
	else
		m_pButton->ShowIcon_Clock(VARIANT_FALSE);
}

bool CISO::TogglePause()
{
//	TRACE("CISO::TogglePause()\n");
	PauseInstrument(!m_bPause);  //flips the sense of m_bPause internally
	return m_bPause;
}
//////////////////////////////////////////////////////////////////
//
//                Validate
//
//////////////////////////////////////////////////////////////////
bool CISO::Validate(bool superuser)
{
//	TRACE("CISO::Validate()\n");

	//send message to the predefined recipient along with superuser bool

	//Note:  this is a SEND
	LRESULT lresult = ::SendMessage(
		m_hValidationHandle,     //MICDlg
		m_uWM_VALIDATE,          //registered message
		((WPARAM)superuser?1:0), //superuser boolean
		0);

	return (lresult != 0);
}

//////////////////////////////////////////////////////////////////////////////
//
//                EstablishButtonKeeperCommunicationsPath
//
//////////////////////////////////////////////////////////////////////////////
void CISO::EstablishButtonKeeperCommunicationsPath(
		HWND hButtonKeeperHandle, UINT uCheckButtonRegisteredMessageValue)
{
	m_hButtonKeeperHandle = hButtonKeeperHandle;
	m_uWM_CHECKDATABUTTON = uCheckButtonRegisteredMessageValue;
}
//////////////////////////////////////////////////////////////////////////////
//
//                EstablishValidationCommunicationsPath
//
//////////////////////////////////////////////////////////////////////////////
void CISO::EstablishValidationCommunicationsPath(
		HWND hValidationHandle, 
		UINT uValidationRegisteredMessageValue)
{
	m_hValidationHandle = hValidationHandle;
	m_uWM_VALIDATE = uValidationRegisteredMessageValue;
}
//////////////////////////////////////////////////////////////////////////////
//
//                EstablishVersionString
//
//////////////////////////////////////////////////////////////////////////////
//SCR 118 - this function is no longer needed.  The version strings are
// "established" in the ISO constructor.
void CISO::EstablishVersionString()
{
	/*
	CVersionInfo VersionInfo;
	m_csVersion = VersionInfo.FileVersion();
	m_csVersion.Remove(' ');
	m_csVersion.Replace(',','.');
	//m_csVersion.Format(
	//	"%c.%c.%c.%c",
	//	m_csVersion[0],
	//	m_csVersion[3],
	//	m_csVersion[6],
	//	m_csVersion[9]);
#ifdef ALLOWTRIGGERHOLDOFF
	m_csVersion += "x";
#endif
	*/
}
//////////////////////////////////////////////////////////////////////////////
//
//                EstablishValidationCommunicationsPath
//
//////////////////////////////////////////////////////////////////////////////
void CISO::EstablishUPSCommunicationsPath(
		HWND hRecipientHandle, 
		UINT uWM_QUERY_ALL_PAUSED,
		UINT uWM_DRIVE_TO_ZERO_COMMAND)
{
	m_hSpecialUPSCommunicationsHandle = hRecipientHandle; //MICDlg is the recipient
	m_uWM_QUERY_ALL_PAUSED            = uWM_QUERY_ALL_PAUSED;
	m_uWM_DRIVE_TO_ZERO_COMMAND       = uWM_DRIVE_TO_ZERO_COMMAND;
}
////////////////////////////////////////////////
//
//					Create
//
////////////////////////////////////////////////
bool CISO::CreateInstrument(char *, ICB *){return false;}
bool CISO::CreateInstrument(char *, char*, int, int, ICB *){return false;}
////////////////////////////////////////////////
//
//				CreateFullNameString
//
////////////////////////////////////////////////
void CISO::CreateFullNameString()
{
	strcpy(m_pFullName,m_pName);
	strcat(m_pFullName,"  Port: ");
	strcat(m_pFullName,m_pPort);
	strcat(m_pFullName,"  Node: ");

	char temp[64];

	if (m_iNode == -1)
		strcat(m_pFullName,"N/A");
	else
		strcat(m_pFullName,_itoa(m_iNode,temp,10));
}

////////////////////////////////////////////////
//
//              SetLegendText
//
////////////////////////////////////////////////
void CISO::SetLegendText(CWnd *pLegend)
{
//	TRACE("CISO::SetLegendText()\n");
}

////////////////////////////////////////////////
//
//     SetWindowsRegisteredMessageStrings
//
////////////////////////////////////////////////
void CISO::SetWindowsRegisteredMessageStrings(
	ULONG ulRECEIVE, 
	ULONG ulRECEIVE_OOB,
	ULONG ulSTART,
	ULONG ulSTOP,
	ULONG ulRESET)
{
//	TRACE("CCSO::SetWindowsRegisteredMessageStrings()\n");

	m_uIDC_RECEIVE         = (UINT)ulRECEIVE;
	m_uIDC_RECEIVE_OOB     = (UINT)ulRECEIVE_OOB;
	m_uIDC_COM_START       = (UINT)ulSTART;
	m_uIDC_COM_STOP        = (UINT)ulSTOP;
	m_uIDC_COM_RESET       = (UINT)ulRESET;

	SetLocalRegisteredMessageValues();
}////////////////////////////////////////////////
//
//				UnRegInstrument
//
////////////////////////////////////////////////
bool CISO::UnRegInstrument()
{
//	TRACE("CISO::UnRegInstrument\n");
	//don't attempt to disconnect if it wasn't
	//registered in the first place
	if (m_pComm && m_bGoodRegister)
	{
		m_pComm->UnRegisterISO(m_iNode,m_eCategory);
		m_pComm->UnRegisterISO(m_iNode+OOBFLAG,m_eCategory);
	}
	return true;
}
////////////////////////////////////////////////
//
//				MakeMyPath
//
////////////////////////////////////////////////
void CISO::MakeMyPath(const char* path)
{
	//recursive routine to create all 
	//of the directories down the "path"
	//NOTE: existance of a "file" with the 
	//same name as a node in the
	//path will cause this routine to fail
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];   
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];		
	if (_access(path,0)==0) return;
	_splitpath(path,drive,dir,fname,ext);
	if (strlen(dir)==0) return;
	_makepath(path_buffer,drive,dir,NULL,NULL);
	if (path_buffer[strlen(path_buffer)-1] == '\\')
		path_buffer[strlen(path_buffer)-1] = NULL;
	MakeMyPath(path_buffer);
	//make sure directory exists
//	_mkdir(path_buffer);
	CreateDirectory(path_buffer,NULL);
}