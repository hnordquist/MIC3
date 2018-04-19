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
// ISO.h
 
#ifndef ISO_H
#define ISO_H

//MIC_CB_028 to MIC 1.907 PJM 1/20/05
#include "ALLOWTRIGGERHOLDOFF.h"

//Component interfaces 
#include "../Interfaces/ICB.h"
#include "../Interfaces/ICSO.h"

#include <Afxmt.h>
#include "COM_Utilities.h"

class CISO : public CWnd
{
// Construction
public:
	CISO(char *inifile);
	CISO();
	virtual ~CISO();

// Attributes
public:

	//
	// Pure virtual
	//
	virtual bool CreateInstrument(void) = 0;
	virtual void SetLocalRegisteredMessageValues() = 0;
	virtual void ShowInstrument(bool) = 0;
	virtual void PauseInstrument(bool) = 0;
	virtual bool UpdateParams(bool) = 0;
	virtual void UpdateStrings() = 0;
	virtual void DriveToZero(bool bDriveToZero, bool bInPwrFail = false) = 0;
	virtual void IssuePrint(FILE* outfile) = 0;
	virtual void Reset() = 0;

	virtual bool CollectParametersForNewISO(CString& csNameOut) = 0;

	//
	// Non-pure virtual - have implementations in ISO.cpp
	//
	virtual int COM_Transmit(
		int Length, 
		char* Output, 
		int node, 
		bool Quiet = false);

	virtual void BlockWrite(bool blockwrite);

	virtual bool CreateInstrument(char *, ICB *);
	virtual bool CreateInstrument(char *, char*, int, int, ICB *);

	virtual void EstablishButtonKeeperCommunicationsPath(
		HWND ButtonKeeperHandle, 
		UINT uCheckButtonRegisteredMessageValue);

	virtual void EstablishValidationCommunicationsPath(
		HWND hValidationHandle, 
		UINT uValidationRegisteredMessageValue);

	virtual void EstablishVersionString();  //SCR 118 - eventually delete this

	virtual void EstablishUPSCommunicationsPath(
		HWND hRecipientHandle, 
		UINT uWM_QUERY_ALL_PAUSED,
		UINT uWM_DRIVE_TO_ZERO_COMMAND);

	virtual void SetLegendText(CWnd *pLegend);

	virtual void SetWindowsRegisteredMessageStrings(
		ULONG ulRECEIVE, 
		ULONG ulRECEIVE_OOB,
		ULONG ulIDC_COM_START,
		ULONG ulIDC_COM_STOP,
		ULONG ulIDC_COM_RESET);

	virtual void     FlagTime(int newvalue,bool redraw);
	virtual void     FlagComm(int newvalue,bool redraw);
	virtual void     FlagOthr(int newvalue,bool redraw);
	virtual void     CreateFullNameString();
	virtual int      GetCurrentBBM();
	virtual char     *GetFullName();
	virtual CString  *GetLastError();
	virtual char     *GetName();
	virtual char     *GetType();
	virtual char     *GetID();
	virtual char     *GetSaveLoc();
	virtual char     *GetFileExtensionsList();
	virtual bool     InShutDown();
	virtual bool     InStartUp();
	virtual bool     IsPaused();
	virtual void	 MakeMyPath(const char *path);
	virtual void     PostToWatch(char *pMessage);
	virtual void     PutICBPointer(ICB *picb);
	virtual void     PutICSOPointer(ICSO *picso);
	virtual void     PutINIPathname(char *inifile);
	virtual void     PutUTILPathname(char *utilfile);
	virtual void     PutName(char *name);
	virtual void     PutDialogAutoCloseTime(int milliseconds);
	virtual void     PutMaxRepetition(int maxreps);
	virtual void     PutNormalShutdown(bool b);
	virtual void     PutUseShortFilename(bool b);
	virtual void     PutLongNameFormatString(CString cs);
	virtual bool     TogglePause();
	virtual bool     UnRegInstrument();
	virtual bool     Validate(bool superuser = false);
	virtual void     GoDumb();
	virtual void     Reconfig();  //MIC_CB_028 to MIC 1.907 PJM 1/20/05

	virtual ISO_FUNCTIONAL_CATEGORY GetISOCategory();

#ifdef ALLOWTRIGGERHOLDOFF
	virtual void AddExclusion(LPARAM lParam);
	virtual void DelExclusion();
#endif


	//
	// ISO Base class variables
	//
public:
	HWND         m_hValidationHandle;
	CEvent       *m_pValidationEvent;
	HWND         m_hButtonKeeperHandle;
	HWND         m_hSpecialUPSCommunicationsHandle;

	ICB          *m_pButton;
	ICSO         *m_pComm;
	bool         m_bBlockWrite;
	CString		m_csLastError;

	CString		m_csVersion;
	CString		m_csNameVersionDate;

	char		m_szFileExtensionsList[512]; //DR 353
	char         m_szIniFile[_MAX_PATH];
	char         m_szUtilFile[_MAX_PATH];
	char         m_szSaveLoc[MAX_PATH];
	char         m_pID[3];
	char         m_pName[32];
	char         m_pType[32];
	int          m_iCurrentBBM;
	char         m_pFullName[80];
	char         m_pPort[32];
	bool         m_bInStartUp;
	bool         m_bInShutDown;
	bool         m_bPause;
	int          m_iPauseMax;
	bool         m_bGoodRegister;
	int          m_iDlgCloseMilliseconds;
	int          m_iMaxRepetition;
	bool         m_bNormalShutdown;
	bool         m_bUseShortFilename;
	CString      m_csLongNameFormatString;
	int          m_iNode;
	bool         m_bDoDump;

	int			m_iFlagTime;
	int			m_iFlagOthr;
	int			m_iFlagComm;

	ISO_FUNCTIONAL_CATEGORY m_eCategory;

    //registered messages
	UINT    m_uWM_VALIDATE; 
	UINT    m_uWM_CHECKDATABUTTON;

	UINT    m_uIDC_RECEIVE;
	UINT    m_uIDC_RECEIVE_OOB;
	UINT    m_uIDC_COM_START;
	UINT    m_uIDC_COM_STOP;
	UINT    m_uIDC_COM_RESET;

	UINT    m_uWM_QUERY_ALL_PAUSED;
	UINT    m_uWM_DRIVE_TO_ZERO_COMMAND;

private:
	void InitializeVariables();
};

#endif //ISO_H