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
// MicMgrDlg.cpp

// DR00169 - Buttons can be crowded and unusable when adding a new instrument.
// DR00169 - Fixed for version 2.001 pjm 13-feb-2006

// DR00200 - Error messages are needed when installation is not as expected.
// DR00200 - Fixed for version 2.001 pjm 13-feb-2006
#include <objbase.h>
#include "stdafx.h"
#include "MicMgr.h"
#include "MicMgrDlg.h"
#include "mic_definitions.h"
#include "ini_definitions.h"
#include "globals.h"
#include "validate.h"
#include "resource.h"
#include "TimedMessageDialog.h"
#include "colors.h"
#include "access.h"
#include "AboutDlg.h"
#include "config.h"
#include "ports.h"
#include "reorganize.h"
#include "version.h"
#include "timedintegerdlg.h"
#include "timeddlg.h"

#include <io.h>
#include <process.h>

#include "../Interfaces/ICSO_i.c"
#include "../Interfaces/IISO_i.c"
#include "../CoColorButton/CoColorButton_i.c"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//local registered message values
static  UINT uWM_ISO_PAUSE_STATUS_CHANGE;
static  UINT uWM_VALIDATE;
static  UINT uWM_QUERY_ALL_PAUSED;
static  UINT uWM_DRIVE_TO_ZERO_COMMAND;

//<<QA>> these look risky...
#define SOH_CMD				(WM_APP + 55)
#define SOH_CMD_MENU		(SOH_CMD + 1)
#define MICXFER_CMD			(WM_APP + 60)

#define TIMER_FILE_HEARTBEAT_SOH	3
#define TIMER_AUTO_SOH				8
#define TIMER_SLOW_START			885533
#define TIMER_PAUSE_ALL1			9876
#define TIMER_PAUSE_ALL2			9877
//dgp - fix coding errors
#define TIMER_PAUSE_ALL3			9878
#define TIMER_PAUSE_ALL_TIME		4
#define TIMER_STALE_STATUS_HEARTBEAT		4093


#include "InitialStatusDataMap.h"
static CCOMStatusMap BadBeginningStatusMap;

/////////////////////////////////////////////////////////////////////////////
// CMicMgrDlg dialog

CMicMgrDlg::CMicMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMicMgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMicMgrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Initialize global values
	g_bHideWindows = false;
	g_iDlgCloseMillisec = 60000;
	g_iInstCount = 0;
	g_iButtonFlagSize = 12;
	g_iButtonWidth = 175;
	g_iButtonHeight = 70;
	g_pVal_Name[0] = NULL;
	g_szIniFile[0] = NULL;

	g_bUseShortName = true; //use short form (8.3) file name convention
	g_csLongName.Empty();

	CVersionInfo VersionInfo;
	g_csVersion = VersionInfo.FileVersion();
	g_csVersion.Replace(", ",".");
#ifdef ALLOWTRIGGERHOLDOFF
	g_csVersion += "x";
#endif
	CString csName = VersionInfo.InternalName();
	g_csNameVersionDate = csName + " " + g_csVersion + " Dec. 1, 2013";

	// Initialize member variables
	m_bRestart = false;
	m_bAtLeastOneISOIsCollectingData = false;
	m_bValidShutdown = false;
	m_iLastX = 0;
	m_iLastY = 0;

	m_iStateOfHealth = 2;			//init at unknown state
									//-1 = bad, 0 = was bad now good, 1 = good
	m_bSOHNewDay = false;			//need to do a "new day" soh file
	m_iSOHDay	= -1;

	m_uiHours = 0;

	InitializeComponentRelatedMemberVariables();

	SetUpRegisteredMessageValues();

	// DR00200
	CString cs("This usually indicates an essential library file (*.dll)\n");
	cs += "cannot be located.\n\n";
	cs += "Re-registering the DLL's -- found in the MIC install directory --\n";
	cs += "may fix the problem.\n\n"; 
	cs += "1. Stop the MPSS service and shut down MIC.\n\n";
	cs += "2. Execute the Register_MIC_Components.bat file\n";
	cs += "in the MIC install directory.\n\n";
	cs += "3. Restart MPSS (and MIC).\n";
	m_csRegistrationAnomalyMessage = cs;

	m_uiDoingNetworkToThatBrandNewBeat = 0;
	m_BeatSeconds.SetDateTimeSpan(0,0,0,0);


}

CMicMgrDlg::~CMicMgrDlg()
{
	if (g_pCoColorButtonFactory != NULL)
		g_pCoColorButtonFactory->Release();

	BadBeginningStatusMap.ClearMap();
	BadBeginningStatusMap.RemoveAll();
}

void CMicMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMicMgrDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMicMgrDlg, CDialog)
	//{{AFX_MSG_MAP(CMicMgrDlg)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
	ON_WM_ENDSESSION()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_QUERYENDSESSION()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()

	ON_MESSAGE(IDC_COPY_DONE,OnCopyDone)
	ON_MESSAGE(IDC_CREATE_COMM,OnAddCommMsg)
	ON_MESSAGE(IDC_CREATE_INST,OnAddInstMsg)
	ON_MESSAGE(IDC_DELETE_COMM,OnDelCommMsg)
	ON_MESSAGE(IDC_DELETE_INST,OnDelInstMsg)
	ON_MESSAGE(IDC_FORCE_PAUSE,OnForcePause)

	ON_MESSAGE(MICXFER_CMD,OnMICXferCmd)  //<<QA>> what does this do?
	ON_MESSAGE(SOH_CMD,OnSOHCmd)

	ON_BN_CLICKED(IDC_COPYDATA_BUTTON, OnCopydataButton)
	ON_BN_CLICKED(IDC_STARTDATA_BUTTON, OnStartdataButton)
	ON_BN_CLICKED(IDM_ARCH_ALT,OnArchiveCheck)

	ON_CONTROL_RANGE(BN_CLICKED,IDC_INSTRUMENT_BUTTON, IDC_INSTRUMENT_BUTTON+MAX_INSTRUMENTS, OnInstrumentButton )
	ON_CONTROL_RANGE(BN_DOUBLECLICKED ,IDC_INSTRUMENT_BUTTON, IDC_INSTRUMENT_BUTTON+MAX_INSTRUMENTS, OnInstrumentButtonDbl )

	ON_REGISTERED_MESSAGE(uWM_ISO_PAUSE_STATUS_CHANGE, OnISOPauseStatusChange)
	ON_REGISTERED_MESSAGE(uWM_VALIDATE, OnValidationRequest)
	ON_REGISTERED_MESSAGE(uWM_DRIVE_TO_ZERO_COMMAND, OnDriveAllToZero)
	ON_REGISTERED_MESSAGE(uWM_QUERY_ALL_PAUSED, OnQueryAllPaused)
	//}}AFX_MSG_MAP
#ifdef ALLOWTRIGGERHOLDOFF
	ON_MESSAGE(TRIGGER_CMD,OnTrigger)
	ON_MESSAGE(TRIGGER_CLR,OnTriggerClr)
#endif

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
//
//                        AddColorButton
//
//////////////////////////////////////////////////////////////////////////////
bool CMicMgrDlg::AddColorButton(bool bParticipateInNet, char *name, ICB **ppICB, CColorButton **ppbutton)
{
	//The ccbutton object belongs to MICDlg 
	//and only the ICB interface to it gets passed to the instrument.

//	TRACE("CMicMgrDlg::AddColorButton()");
//	TRACE("	ID = %d\n",IDC_INSTRUMENT_BUTTON + g_iInstCount);

	ICB *pICB = NULL;
	CColorButton *pbutton = new CColorButton(bParticipateInNet, g_szIniFile);

	if (pbutton == NULL)
	{
		*ppICB = NULL;
		*ppbutton = NULL;
		return false;
	}

	// Create instance of ICB interface object and get its ICB pointer
	HRESULT hr = g_pCoColorButtonFactory->CreateInstance(NULL, IID_ICB, (void **)&pICB);

	if (FAILED(hr))
	{
		delete pbutton;
		*ppICB = NULL;
		*ppbutton = NULL;
		return false;
	}

	// Initialize the new CColorButton object that belongs to MicMgr.
	// The methods directly on the CColorColor object are private to MicMgr.
	pbutton->SetINIPathname(g_szIniFile, sizeof(g_szIniFile));
	pbutton->SetDoingNetwork(g_bDoingNetwork && bParticipateInNet);

	ULONG id = IDC_INSTRUMENT_BUTTON + g_iInstCount;
	DWORD dwstyle = 
		WS_CHILD | 
		WS_TABSTOP | 
		WS_VISIBLE | 
		BS_PUSHBUTTON | 
		BS_MULTILINE | 
		//BS_TOP  | 
		BS_CENTER |
		BS_OWNERDRAW |
		//BS_LEFT |
		0;
			// from MIC 1.9.0.7
			//button->Create(Name, 
			//	WS_CHILD | WS_TABSTOP |
			//	BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER |
			//	BS_OWNERDRAW,
			//	CRect(0,0,g_dButtonWidth,g_dButtonHeight),//BUTTON_HEIGHT),
			//	m_pParent, 
			//	IDC_INSTRUMENT_BUTTON+g_dCount);

	BOOL success_in_creating_ccbutton_object = 
		pbutton->Create(
		name,
		dwstyle,
		CRect(0,0,g_iButtonWidth,g_iButtonHeight),
		this,
		id);

	if (!success_in_creating_ccbutton_object)
	{
		delete pbutton;
		*ppICB = NULL;
		*ppbutton = NULL;
		return false;
	}

	//This is the window handle of the ccbutton object that 
	//belongs to MICDlg.  The ICB COM interface
	//is implemented using this handle to send messages.
	pICB->SetMICObjectHandle(ULONG(pbutton->m_hWnd));

	*ppICB = pICB;
	*ppbutton = pbutton;

	return true;
}

//////////////////////////////////////////////////////////////////
//
//                        AddComm
//
///////////////////////////////////////////////////////////////////////
void CMicMgrDlg::AddComm(char *name, bool bNewEntry)
{
//	TRACE("CMicMgrDlg::AddComm(%s)\n",name);

	CString csTitle = CString(MIC_NAMELONG)+CString(": ")+CString(name);

	//////////////////////////////////////////////////////////////////////////
	// Don't exceed the number of communications objects allowed.
	//////////////////////////////////////////////////////////////////////////
	if (g_iCommCount >= MAX_COMM)
	{
		CString csTitle = CString(MIC_NAMESHORT)+CString(": ")+CString(name);
		CString cs;
		cs.Format("Cannot create %s communications object.  Cannot exceed %d comm objects.", 
			name, MAX_INSTRUMENTS);

		new CTimedMessageDialog(csTitle,cs,g_iDlgCloseMillisec/1000,this);
		return;
	}
	//////////////////////////////////////////////////////////////////////////
	// Set up local variables
	//////////////////////////////////////////////////////////////////////////
	ICSO * picso = NULL;
	VARIANT_BOOL vbgoodport = VARIANT_FALSE;
	IClassFactory *pfactory = NULL;

	bool bSoFarSoGood = true;
	bool bAlreadyDisplayedErrorMessageBox = false; // DR00200

	//////////////////////////////////////////////////////////////////////////
	// Get IClassFactory pointer.
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		char clsid_buf[128];
		char type_buf[128];
		CLSID clsid;
		WCHAR wide_clsid_buf[128];

		//get the CLSID string from the INI file.
		GetPrivateProfileString(name,"CLSID","ERROR", clsid_buf,sizeof(clsid_buf), g_szIniFile);

		// This section of code will modify MIC.INI files that were made with MIC versions
		// prior to MIC 2.0.  Each of the instantiated communications objects needs the
		// CLSID of its COM componenet (DLL) in the MIC.INI file.  This section will insert that
		// information into the file if it is not already there.

		if (0 == strcmp(clsid_buf, "ERROR"))  //succeeds if there is no CLSID entry (pre-2.0)
		{
			//get the type
			GetPrivateProfileString(COMMUNICATIONS,name,"ERROR", type_buf, sizeof(type_buf), g_szIniFile);

			if (0 != strcmp(type_buf, "ERROR")) //succeeds if we have the type.  Else bail.
			{
				//Get the true CLSID from the CLSIDLIST.INI file
				GetPrivateProfileString("CLSIDLIST",type_buf,"ERROR",clsid_buf,sizeof(clsid_buf), g_szClsidIniFile);

				if (0 != strcmp(clsid_buf, "ERROR"))  //succeeds if we found a CLSID for this type
				{
					WritePrivateProfileString(name,"CLSID",clsid_buf,g_szIniFile);
				}
			}
		}
		// this ends the section that inserts CLSID's into the MIC.INI file if it is
		// not there.

		if (0 != strcmp(clsid_buf, "ERROR"))
		{
			//change the CLSID string to the actual clsid.
			::mbstowcs(wide_clsid_buf, clsid_buf, 128);
			::CLSIDFromString(wide_clsid_buf,&clsid);

			//get the class factory
			CoGetClassObject(clsid,       //CLSID associated with the class object
					CLSCTX_INPROC_SERVER, //In-process server
					NULL,                 //Instantiate class object on this machine.
					IID_IClassFactory,    //Interface we are looking for
					(LPVOID *)&pfactory);   //pointer to the interface

			// DR00200
			if (pfactory == NULL)
			{
				CString cs;
				cs.Format("CoGetClassObject fails for communications component\n \"%s\"\n",name);
				cs += "as defined in the MIC.INI file\n\n";
				cs += m_csRegistrationAnomalyMessage;

				new CTimedMessageDialog(
					CString("MIC Setup Anomaly"),
					cs,
					g_iDlgCloseMillisec/1000);  //seconds

				bAlreadyDisplayedErrorMessageBox = true; // DR00200
			}
		}
		bSoFarSoGood = (NULL != pfactory);
	}
	//////////////////////////////////////////////////////////////////////////
	// Create the class object that implements the ICSO interface
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		HRESULT hr = pfactory->CreateInstance(NULL, IID_ICSO, (void **)&picso);

		if (!SUCCEEDED(hr) || (picso == NULL))
			bSoFarSoGood = false;
	}
	//////////////////////////////////////////////////////////////////////////
	//initialize variables for the new communications component
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		BSTR bstr;

		Convert_Character_String_to_BSTR(name,bstr);
		picso->put_Name(bstr);

		Convert_Character_String_to_BSTR(g_szIniFile,bstr);
		picso->put_INIPathname(bstr);

		picso->put_WindowsRegisteredMessageStrings(
				(ULONG)m_uIDC_RECEIVE,
				(ULONG)m_uIDC_RECEIVE_OOB,
				(ULONG)m_uIDC_COM_START,
				(ULONG)m_uIDC_COM_STOP,
				(ULONG)m_uIDC_COM_RESET);

		Convert_Character_String_to_BSTR(name,bstr);
		picso->SetUpPort(bstr, &vbgoodport);

		//bSoFarSoGood = (VARIANT_TRUE == vbgoodport);
		if (!vbgoodport && !bNewEntry)  // if port setup failed and this is not a new entry, then save status for this Comm 
		{
			BSTR bstrVersionInfo = NULL, bstrErr = NULL;
			picso->get_LastError(&bstrErr);
			picso->get_VersionString(&bstrVersionInfo);
			BadBeginningStatusMap.GetNewCSO(name, bstrVersionInfo, bstrErr);
			if (bstrVersionInfo) ::SysFreeString(bstrVersionInfo);
			if (bstrErr) ::SysFreeString(bstrErr);
		}

	}

	//////////////////////////////////////////////////////////////////////////
	// Record the communication object's globals only if 
	// ALL phases of adding it passed
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		//setup went ok so add the CSO to the comm array and start it up
		g_pICSO[g_iCommCount] = picso;
		g_pICSO[g_iCommCount]->StartCSO();

		g_iCommCount++;
	}
	else
	{
		//new CSO failed port set up so throw it away
		if (picso != NULL)
		{
			picso->ShutdownCSO();
			picso->Release();
			picso = NULL;
		}
		m_bValidShutdown = true;  //<<QA>> what does this do?

		if (bNewEntry)
		{
			// If new entry, remove failed communications object 
			// data from the INI file.  If old stuff, leave it in the INI file.
			WritePrivateProfileString(COMMUNICATIONS,name,NULL,g_szIniFile);
			WritePrivateProfileSection(name,NULL,g_szIniFile);
		}

		//tell the user with a timeout dialog box
		// DR00200
		if ((vbgoodport == VARIANT_FALSE)  && !bAlreadyDisplayedErrorMessageBox)
		{
			CString csComm_port_fail_message( 
			"Setup Port Failed! There may be an error in the MIC.INI file or "\
			"MIC is already running as a service and is controlling the port requested. "\
			"Correct the error manually and RESTART MIC NOW!");

			new CTimedMessageDialog(
				csTitle,
				csComm_port_fail_message,
				g_iDlgCloseMillisec/1000,
				this);
		}
	}

	//Added 16 March 2005 pjm
	if (pfactory != NULL)
		pfactory->Release();
}

//////////////////////////////////////////////////////////////////////////////
//
//                           AddInstrument
//
//////////////////////////////////////////////////////////////////////////////
//function to add an instrument by "name" from INI file
void CMicMgrDlg::AddInstrument(char *name, bool bNewEntry)
{
//	TRACE("CMicMgrDlg::AddInstrument()\n");

	//////////////////////////////////////////////////////////////////////////
	// Don't exceed the number of instruments allowed.
	//////////////////////////////////////////////////////////////////////////
	if (g_iInstCount >= MAX_INSTRUMENTS)
	{
		new CTimedMessageDialog(
			"MIC::AddInstrument()",
			"Cannot create any more instruments.",
			g_iDlgCloseMillisec/1000,this);

		return;
	}
	//////////////////////////////////////////////////////////////////////////
	// Set up local variables - some with default values
	//////////////////////////////////////////////////////////////////////////
	ISO_FUNCTIONAL_CATEGORY inst_cat = UNKNOWN_ISO;

	IClassFactory *pFactory = NULL;
	IISO          *piiso    = NULL;
	ICB           *picb     = NULL;
	CColorButton  *pbutton  = NULL;
	ICSO          *picso    = NULL;

	CString csTheProblem("");

	bool bSoFarSoGood = true;
	bool bIsHiddenWatchISO = false;

	//////////////////////////////////////////////////////////////////////////
	// Get IClassFactory pointer.
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		bSoFarSoGood = GetInstrumentFactory(name,&pFactory);

		//bSoFarSoGood = false;  //debug

		if (!bSoFarSoGood)
		{
			csTheProblem.Format(
			"Could not get class factory for \"%s\" instrument\nas defined in the MIC.INI file.\n\n", name);

			// DR00200
			csTheProblem += m_csRegistrationAnomalyMessage;
		}

	}
	//////////////////////////////////////////////////////////////////////////
	// Get port associated with this instrument's name out of the INI file
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		// First of all insure we can create this instrument at this port.
		// Find port and the CSO which supports this Instrument e.g. "COM1".

		char portbuff[32],thisport[32];
		BSTR bstr;
		bool bport_match_found = false;

		GetPrivateProfileString(name,PORT,"",portbuff,sizeof(portbuff)-1,g_szIniFile);

		for (int i = 0; i < g_iCommCount; i++)
		{
			if (NULL != g_pICSO[i])
			{
				g_pICSO[i]->get_Name(&bstr);
				Convert_BSTR_to_Character_String(bstr,thisport, sizeof(thisport));

				if (0 == strcmp(thisport,portbuff))
				{
					bport_match_found = true;
					picso = g_pICSO[i];
					break;
				}
			}
		}

		if (!bport_match_found)
		{
			bSoFarSoGood = false;
			//csTheProblem = "Could not find matching port.";
			csTheProblem.Format(
				"Could not find matching port for %s.\nCheck the MIC.INI file.",name);
		}
	}
	///////////////////////////////////////////////////////////////////////
	// Create the color button object
	///////////////////////////////////////////////////////////////////////
//	if (bSoFarSoGood)
//	{
//		bSoFarSoGood = AddColorButton(true,name,&picb,&pbutton);
//
//		if (!bSoFarSoGood)
//		{
//			csTheProblem = 
//				"Could not add color button.\nCheck registration of CoColorButton DLL.";
//		}
//	}

	//////////////////////////////////////////////////////////////////////////
	// Create the class object that implements the IISO interface
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		HRESULT hr = pFactory->CreateInstance(NULL, IID_IISO, (void **)&piiso);

		if (!SUCCEEDED(hr) || (piiso == NULL))
		{
			bSoFarSoGood = false;
			//csTheProblem = 
			//	"Could not create instance of the %s instrument.\nCheck registration of the Instrument's DLL.";
			csTheProblem.Format(
				"Could not create the %s instrument.\nCheck registration of the Instrument's DLL.",name);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// Ensure we aren't putting a second Watcher instrument on this port&node
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		//get the category of the new ISO: standard, watch, or ups
		ULONG ucat;
		piiso->get_ISOCategory(&ucat);
		inst_cat = (ISO_FUNCTIONAL_CATEGORY)ucat;

		///////////////////////////////////////////////////////////////////////
		// Create the color button object
		///////////////////////////////////////////////////////////////////////
		bSoFarSoGood = AddColorButton((inst_cat==WATCH_ISO)?false:true,name,&picb,&pbutton);

		if (!bSoFarSoGood)
		{
			csTheProblem = 
				"Could not add color button.\nCheck registration of CoColorButton DLL.";
		}

		if (inst_cat == WATCH_ISO)
		{

			char buff[8];
			GetPrivateProfileString(CONFIGURATION,HIDEWATCH,"No",buff,sizeof(buff),g_szIniFile);
			buff[0] = (char)toupper(buff[0]);
			if (buff[0] == 'Y')
			{
				bSoFarSoGood = false;  //do not instantiate hidden watch windows
				bIsHiddenWatchISO = true;  //there is some special processing
											//below for a hidden watch window.
			}

			//also .....
			//<<QA>> figure this out...
			//<<QA>> not supposed to have two watch windows on the
			//<<QA>> same port/node combo.
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//initialize variables for the new instrument component and start it
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		BSTR bstr;
		VARIANT_BOOL vbool;

		//communications component pointer
		piiso->put_ICSOPointer(picso);
		//Color Button pointer
		piiso->put_ICBPointer(picb);
		//name
		Convert_Character_String_to_BSTR(name,bstr);
		piiso->put_Name(bstr);
		//INI file path and name
		Convert_Character_String_to_BSTR(g_szIniFile,bstr);
		piiso->put_INIPathname(bstr);
		//Auto Close time
		piiso->put_DialogAutoCloseTime((ULONG)(g_iDlgCloseMillisec/1000));

		//Max repetitions
		piiso->put_MaximumRetryRepetitions((ULONG)g_iMaxRepetition);

		//normal shutdown  //<<QA>> review this for the ISO interface
		piiso->put_NormalShutdown(m_bNormalShutdown ? VARIANT_TRUE:VARIANT_FALSE);//un-commented dgp

		//utility path and name
		Convert_Character_String_to_BSTR(g_szUtilPath,bstr);
		piiso->put_UTILPathname(bstr);

		//tell the ISO about using long or short filenames
		Convert_CString_to_BSTR(g_csLongName,bstr);
		piiso->put_LongNameFormatString(bstr);
		vbool = g_bUseShortName?VARIANT_TRUE:VARIANT_FALSE;
		piiso->UseShortFilename(vbool);

		//communications information back to MicMgrDlg for button changes
		piiso->EstablishButtonKeeperCommunicationsPath(
			(ULONG)m_hWnd,uWM_ISO_PAUSE_STATUS_CHANGE);

		//communications back to MicMgrDlg for validating changes
		piiso->EstablishValidationCommunicationsPath((ULONG)m_hWnd,uWM_VALIDATE);

		//communications back to MicMgrDlg for special UPS control
		if (inst_cat == UPS_ISO)
		{
			piiso->EstablishSpecialUPSCommunicationsPath(
				(ULONG)m_hWnd,uWM_QUERY_ALL_PAUSED,uWM_DRIVE_TO_ZERO_COMMAND);
		}
		//establish the registered message values in the iiso
		piiso->put_WindowsRegisteredMessageStrings(
				(ULONG)m_uIDC_RECEIVE,
				(ULONG)m_uIDC_RECEIVE_OOB,
				(ULONG)m_uIDC_COM_START,
				(ULONG)m_uIDC_COM_STOP,
				(ULONG)m_uIDC_COM_RESET);

		//create the ISO
		VARIANT_BOOL vb;
		piiso->CreateISO(&vb);

		bSoFarSoGood = (vb == VARIANT_TRUE);

		if (!bSoFarSoGood)
			csTheProblem.Format("Function call to CreateISO() for %s failed.",name);
	}
	//////////////////////////////////////////////////////////////////////////
	// Record the instrument globals only if ALL phases of adding it passed
	//////////////////////////////////////////////////////////////////////////
	if (bSoFarSoGood)
	{
		// Set up the client's global values to track this ISO/CB/CSO set
		g_pIISO[g_iInstCount]        = piiso;
		g_pICB[g_iInstCount]         = picb;
		g_pColorButton[g_iInstCount] = pbutton;
		g_eISOCategory[g_iInstCount] = inst_cat;

		g_iInstCount++;
		ReorgDialogLayout();
	} 
	else
	{
		// Add instrument failed
		// or it was a "hidden" watch window that is not to be
		// instantiated.

		//Clean up the mess and tell the operator we have a problem
		//AddInstrumentFailed(name, piiso, picb, pbutton);

		//Do not process as an error if it is a hidden Watch window.
		if (!bIsHiddenWatchISO)
		{
			if (bNewEntry)
			{
				// If new entry, remove failed instrument 
				// data from the INI file.  If old stuff, leave it in the INI file.
				WritePrivateProfileString(INSTRUMENTS,name,NULL,g_szIniFile);
				WritePrivateProfileSection(name,NULL,g_szIniFile);
			}

			CString csTitle = 
				CString(MIC_NAMESHORT)+
				CString(": Add ")+ name +
				CString(" Instrument");
			
			CString csText = csTheProblem;

			new CTimedMessageDialog(csTitle,csText,g_iDlgCloseMillisec/1000, NULL);

			BSTR bstrVersionInfo = NULL, bstrErr = NULL;
			if (piiso)
			{
				piiso->get_LastError(&bstrErr);
				piiso->get_VersionString(&bstrVersionInfo);
				BadBeginningStatusMap.GetNewISO(name, bstrVersionInfo, bstrErr);
				if (bstrVersionInfo) ::SysFreeString(bstrVersionInfo);
				if (bstrErr) ::SysFreeString(bstrErr);
			}
			else
			{
				//BSTR bs = csTitle.AllocSysString();
				BadBeginningStatusMap.GetNewISO(name, NULL, NULL);
				//if (bs) ::SysFreeString(bs);
			}

		}
		if (piiso != NULL)
			piiso->Release();

		if (picb != NULL)
			picb->Release();

		if (pbutton != NULL)
			delete pbutton;
	}

	if (pFactory)
		pFactory->Release();
}

//////////////////////////////////////////////////////////////////
//
//                        DoSOHCmd
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::DoSOHCmd(char* szFileName)
{
//	TRACE("CMicMgrDlg::DoSOHCmd()\n");
	FILE *outfile;
	char szTemp[32];
	GetPrivateProfileString(
		CONFIGURATION,
		SOH_LIMITED_EXT,"No",szTemp,sizeof(szTemp),g_szIniFile);

	bool bLimited = (toupper(szTemp[0]) == 'Y');
	GetPrivateProfileString(CONFIGURATION,SOH_APPEND,"No",szTemp,sizeof(szTemp),g_szIniFile);

	bool bAppend = (toupper(szTemp[0]) == 'Y');
	if (strlen(szFileName))
	{
		if (bAppend)
			outfile = fopen(szFileName,"at");
		else
			outfile = fopen(szFileName,"wt");
	}
	else
		outfile = NULL;
	if (outfile)
	{
		COLORREF color = COLOR_BLACK;
		int		iState = 0;
		CString cString;
		CString cNameText;
		CString cSecondText;
		CString cInstType;
		GetWindowText(cString);
		COleDateTime cCurrentDT = COleDateTime::GetCurrentTime();
		fprintf(outfile,"%s\n%s\n\n",cString.GetString(), cCurrentDT.Format("%Y.%m.%d %H:%M:%S").GetString());  //gjk
		//get summary of disk drives
		fprintf(outfile,"Drive Status:\n");
		char szDrive[8];
		strcpy(szDrive,"A:\\");
		for (int i = 0; i < 26; i++)
		{
			szDrive[0] = (char)('A'+i);
			UINT uiType = GetDriveType(szDrive);
			if (uiType != DRIVE_NO_ROOT_DIR)
			{
				ULONG64  lFreeBytesAvailable;
				ULONG64  lTotalNumberOfBytes;
				ULONG64  lTotalNumberOfFreeBytes;
				if (GetDiskFreeSpaceEx(szDrive,(union _ULARGE_INTEGER *)&lFreeBytesAvailable,(union _ULARGE_INTEGER *)&lTotalNumberOfBytes,(union _ULARGE_INTEGER *)&lTotalNumberOfFreeBytes))
					fprintf(outfile,"%s, "
						"Total Number Of Free Bytes: %12I64u, "
						"Total Number Of Bytes: %12I64u\n",
						szDrive,
						lTotalNumberOfFreeBytes,
						lTotalNumberOfBytes);
				else
				{
					fprintf(outfile,"%s, Access Drive Failed ",szDrive);
					if ((uiType == DRIVE_REMOVABLE) || (uiType == DRIVE_CDROM))
						fprintf(outfile,"Possibly no disk in removable drive.");
					fprintf(outfile,"\n");
				}
			}
		}
		fprintf(outfile,"\n");

		//get summary of button information
		fprintf(outfile,"MIC Main Dialog Status:\n");
		PrintButtonStatus(outfile,"","","",NULL,NULL);
		char szType[64];
		CColorButton *pbutton;

		for (int instrument = 0; instrument < g_iInstCount; instrument ++) 
		{
			//piso = g_pIISO[instrument];
			pbutton = g_pColorButton[instrument];

			try
			{
				pbutton->GetStrings(&cNameText,&cSecondText);

				//if (g_dInstType[instrument] == WATCH)
				if (g_eISOCategory[instrument] == WATCH_ISO)
				{
					color = COLOR_WHITE;
					iState = 0;
					cSecondText = "";
				}
				else
				{
					color = pbutton->GetFaceColor();  //color is already a ULONG
					iState = (int)pbutton->GetState();
				}
			}
			catch(...)
			{
			}

			GetPrivateProfileString(INSTRUMENTS,cNameText,"UNKNOWN",szType,sizeof(szType),g_szIniFile);

			PrintButtonStatus(outfile, cNameText, cSecondText, szType, color, iState);
		}  //end for

		//insert a blank line
		fprintf(outfile,"\n");

		bool bfileclosed = false;

		if (!bLimited)
		{
			fprintf(outfile,"MIC.INI Non-Instrument Specific Sections:\n");	
			PrintINISection(CONFIGURATION,outfile);
			PrintINISection(USERS,outfile);
			PrintINISection(SUPERUSERS,outfile);
			PrintINISection(NETWORK,outfile);
			PrintINISection(COMMUNICATIONS,outfile);
			PrintINISection(INSTRUMENTS,outfile);
			fprintf(outfile,"\n");

			fclose(outfile);
			bfileclosed = true;

			BSTR bstrFileName;
			Convert_Character_String_to_BSTR(szFileName,bstrFileName);
			
			//get all the instrument information
			for ( int instrument = 0; instrument < g_iInstCount; instrument ++) 
			{
				g_pIISO[instrument]->AppendGUIValues(bstrFileName);

				outfile = fopen(szFileName,"at");
				fprintf(outfile,"\n");
				fclose(outfile);
			}
		}
		if (bfileclosed)
			outfile = fopen(szFileName,"at");

		fprintf(outfile,"%s\n%s\nEND OF FILE\n",cString.GetString(), cCurrentDT.Format("%Y.%m.%d %H:%M:%S").GetString()); // gjk
		fclose(outfile);
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//               GetInstrumentFactory
//
//////////////////////////////////////////////////////////////////////////////
bool CMicMgrDlg::GetInstrumentFactory(
	char *name,
	IClassFactory **factory)
{
	bool success = true;

	char clsid_buf[128];
	char type_buf[128];

	//get the CLSID string from the INI file.
	GetPrivateProfileString(name,"CLSID","ERROR", clsid_buf,sizeof(clsid_buf), g_szIniFile);

		// This section of code will modify MIC.INI files that were made with MIC versions
		// prior to MIC 2.0.  Each of the instantiated communications objects needs the
		// CLSID of its COM componenet (DLL) in the MIC.INI file.  This section will insert that
		// information into the file if it is not already there.

		if (0 == strcmp(clsid_buf, "ERROR"))  //succeeds if there is no CLSID entry (pre-2.0)
		{
			//get the type
			GetPrivateProfileString(INSTRUMENTS,name,"ERROR", type_buf, sizeof(type_buf), g_szIniFile);

			if (0 != strcmp(type_buf, "ERROR")) //succeeds if we have the type.  Else bail.
			{
				//Get the true CLSID from the CLSIDLIST.INI file
				GetPrivateProfileString("CLSIDLIST",type_buf,"ERROR",clsid_buf,sizeof(clsid_buf), g_szClsidIniFile);

				if (0 != strcmp(clsid_buf, "ERROR"))  //succeeds if we found a CLSID for this type
				{
					WritePrivateProfileString(name,"CLSID",clsid_buf,g_szIniFile);
				}
			}
		}
		// this ends the section that inserts CLSID's into the MIC.INI file if it is
		// not there.

	if (0 != strcmp(clsid_buf, "ERROR"))
	{
		//change the CLSID string to the actual clsid.
		CLSID clsid;
		WCHAR wide_clsid_buf[128];
		::mbstowcs(wide_clsid_buf, clsid_buf, 128);

		::CLSIDFromString(wide_clsid_buf,&clsid);
		//get the class factory
		CoGetClassObject(clsid, //CLSID associated with the class object
					CLSCTX_INPROC_SERVER, //In-process server
					NULL,                 //Instantiate class object on this machine.
					IID_IClassFactory,    //Interface we are looking for
					(LPVOID *)factory);//&g_pClassFactory[i]); //pointer to the interface

		if (NULL == *factory)
			success = false;
	}
	else
	{
		success = false;
	}
	return success;
}
//////////////////////////////////////////////////////////////////
//
//        InitializeComponentRelatedMemberVariables
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::InitializeComponentRelatedMemberVariables()
{
	int i;
	for (i = 0; i < MAX_INSTRUMENTS; i++)
	{
		g_pIISO[i] = NULL;
		g_pICB[i] = NULL;
		g_pColorButton[i] = NULL;
		g_eISOCategory[i] = UNKNOWN_ISO;
	}
	for (i = 0; i < MAX_COMM; i++)
	{
		g_pICSO[i] = NULL;
	}

	g_pCoColorButtonFactory = NULL;
}
//////////////////////////////////////////////////////////////////
//
//                   OnAccessControlMenuItem
//
//////////////////////////////////////////////////////////////////
//do access control dialog box
//to add, edit, and remove users/password pairs
void CMicMgrDlg::OnAccessControlMenuItem() 
{
	if (g_bHideWindows)
		ShowWindow(SW_HIDE);
	CAccess Access(this);
	Access.DoModal();
	ShowWindow(SW_SHOW);

	// set the menu text to do the right thing
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (Access.CountUsers()==0)
		pSysMenu->EnableMenuItem(IDM_CONFIG_ALT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	else
		pSysMenu->EnableMenuItem(IDM_CONFIG_ALT, MF_BYCOMMAND | MF_ENABLED );
}
//////////////////////////////////////////////////////////////////
//
//                        OnAddCommMsg
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnAddCommMsg(WPARAM WParam, LPARAM LParam)
{
//	TRACE("CMicMgrDlg::OnAddCommMsg()\n");

	char* Name = (char*)LParam;
	int charcount = (int)WParam;

	//This is a newly entered communications object because it
	//is being instantiated in response to a message from CNewComm.
	//If the object data is malformed, this flag tells AddComm() to 
	//remove its information from the MIC.INI file.

	if ((Name != NULL) && (charcount > 0))
		AddComm(Name, true); //true indicates new entry

	CoTaskMemFree((void *)LParam);
	return 0;
}
//////////////////////////////////////////////////////////////////
//
//                        OnAddInstMsg
//
//////////////////////////////////////////////////////////////////
//received an "add instrument object" message
LRESULT CMicMgrDlg::OnAddInstMsg(WPARAM WParam, LPARAM LParam)
{
//	TRACE("CMicMgrDlg::OnAddInstMsg()\n");

	char* Name = (char*)LParam;
	int charcount = (int)WParam;

	if ((Name != NULL) && (charcount > 0)) 
		AddInstrument(Name, true); //true indicates a new instrument

	CoTaskMemFree((void *)LParam);
	return 0;
}

//////////////////////////////////////////////////////////////////
//
//					  OnDT0Pause
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnDT0Pause()							//SCR00361
{														//SCR00361
	//drive to zero and pause but not in power fail		//SCR00361
	OnDriveAllToZero(1,0);								//SCR00361
	//look to see if all are paused every 10 seconds	//SCR00361
	KillTimer(TIMER_PAUSE_ALL1);						//SCR00361
	KillTimer(TIMER_PAUSE_ALL2);						//SCR00361
	SetTimer(TIMER_PAUSE_ALL1,10000,NULL);				//SCR00361
	KillTimer(TIMER_STALE_STATUS_HEARTBEAT);					

}														//SCR00361
//////////////////////////////////////////////////////////////////
//
//                    OnForcePause
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnForcePause(WPARAM wParam, LPARAM lParam) 
{
	StartData(false);
	return 0;
}
//////////////////////////////////////////////////////////////////
// 
//                       OnInitDialog
//
//////////////////////////////////////////////////////////////////
BOOL CMicMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buff[32768];

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))							//there is no real failure
	{										//possible here
		{																//dgp - fix coding errors
			CTimedDlg cTimedDlg("MIC Error",							//dgp - fix coding errors
				"Could not initialize the COM runtime.",10,this);		//dgp - fix coding errors
			cTimedDlg.DoModal();										//dgp - fix coding errors
		}//block to force dlg out of scope before exit is called		//dgp - fix coding errors
		exit (0);														//dgp - fix coding errors
	}

	//Get the factory (aka class object) for the color button class
	CoGetClassObject(
		CLSID_ColorButton,    //CLSID associated with the class object
		CLSCTX_INPROC_SERVER, //In-process server
		NULL,                 //Instantiate class object on this machine.
		IID_IClassFactory,    //Interface we are looking for
		(LPVOID *)&g_pCoColorButtonFactory); //pointer to the interface

	if (g_pCoColorButtonFactory == NULL)
	{
		{																//dgp - fix coding errors
			CTimedDlg cTimedDlg("MIC Error",							//dgp - fix coding errors
				"Could not set up color button class factory.\n"		//dgp - fix coding errors
				"Reinstall MIC or manually register CoColor",10,this);	//dgp - fix coding errors
			cTimedDlg.DoModal();										//dgp - fix coding errors
		}//block to force dlg out of scope before exit is called		//dgp - fix coding errors
		exit (0);														//dgp - fix coding errors
	}

	//Get the CoColorButton version string for the About box.
	//this requires the colorbutton interface to be changed to
	//include this function
	ICB *picb;
	BSTR bstr;
	//CString g_csColorButtonVersionString;

	hr = g_pCoColorButtonFactory->CreateInstance(NULL, IID_ICB, (void **)&picb);

	if (hr == S_OK && picb != NULL)
	{
		picb->get_VersionString(&bstr);
		Convert_BSTR_to_CString(bstr,g_csColorButtonVersionString);
		picb->Release();
	}

//fill the background of the main dialog box
#ifdef BMPBACKGRND
//	VERIFY(m_bmapBrush.LoadBitmap(IDB_BMAPBRUSH));
#ifdef ALLOWTRIGGERHOLDOFF
	VERIFY(m_bmapBrush.LoadBitmap(IDB_BITMAP8));  
#else
	VERIFY(m_bmapBrush.LoadBitmap(IDB_BITMAP7));
#endif
	CBitmap* pDlgBmp = &m_bmapBrush;
	m_brush.CreatePatternBrush(pDlgBmp);
#endif

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED |MF_BYCOMMAND);
		pSysMenu->EnableMenuItem(SC_SIZE,MF_GRAYED |MF_BYCOMMAND);

		pSysMenu->AppendMenu(MF_STRING, IDM_CONFIG_ALT,"Configuration...");
		pSysMenu->AppendMenu(MF_STRING, IDM_ACCESS_ALT,"Access Control...");
		pSysMenu->AppendMenu(MF_STRING, IDM_ARCH_ALT,  "Do File Aging Now");
		pSysMenu->AppendMenu(MF_STRING, IDM_REORG_ALT, "Re-Organize...");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, SOH_CMD_MENU,  "Write SOH File");
		pSysMenu->AppendMenu(MF_STRING, IDM_DT0PAUSE,  "Get BBM && Pause");
		pSysMenu->AppendMenu(MF_STRING, IDM_COPY_ALT,  "Copy Data Files...");
		pSysMenu->AppendMenu(MF_STRING, IDM_DATA_ALT,  "Start All");
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_CEREAL_BOWL,  "Serial Port List...");

		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
		pSysMenu->AppendMenu(MF_STRING, IDM_EXIT_ALT,  "Exit");
	}

	m_hIcon32 = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon16 = AfxGetApp()->LoadIcon(IDI_ICON1);
	//<<QA>> why are these the same?? Because they weren't going to be the same.
	m_hIcon32b = AfxGetApp()->LoadIcon(IDI_ICON13);
	m_hIcon16b = AfxGetApp()->LoadIcon(IDI_ICON13);

	// Set the icon for this dialog.
	SetIcon(m_hIcon32, TRUE);		// Set big icon
	SetIcon(m_hIcon16, FALSE);		// Set small icon

	//build the ini file name and the util pathname
	//get full path and name for this instance of application
	GetModuleFileName(AfxGetInstanceHandle(),g_szIniFile,sizeof(g_szIniFile));
	char *c = strrchr(g_szIniFile,'\\');//find the last "\"
	*c = '\0';						//make it null
	strcat(g_szIniFile,"\\");
	strcat(g_szClsidIniFile,g_szIniFile);
	strcat(g_szClsidIniFile,"CLSIDLIST.INI");
	strcpy(g_szUtilPath,g_szIniFile);
	strcat(g_szUtilPath,"MsgUtil.exe");
	strcat(g_szIniFile,MIC_NAMESHORT);
	strcat(g_szIniFile,".INI");

	//if the ini file isn't there create a "default" one
	if (_access(g_szIniFile,0)!=0)
	{
		FILE* temp;
		if ((temp = fopen(g_szIniFile,"wt"))!=NULL)
		{
			fprintf(temp,"[CONFIGURATION]\n");
			fprintf(temp,"BUTTONSWIDE=1\n");
			fprintf(temp,"FACILITYNAME=New\n");
			fprintf(temp,"DEFAULTCOPYPATH=A:\\\n");
			fprintf(temp,"MINUTESTOCHECKARCHIVE=60\n");
			fprintf(temp,"DAYSBEFOREDELFILES=90\n");
			fprintf(temp,"MAXPERCENTDISKUSE=99.99\n");
			fprintf(temp,"HIDEWINDOWS=0\n");
			fprintf(temp,"LOGARCHIVEACT=0\n");
			fprintf(temp,"DIALOGCLOSE=999\n");
			fprintf(temp,"PAUSE_ALL_TIME=4\n");
			fprintf(temp,"MODE=IAEA\n");

			//These 2 lines here so that MIC 2.0.0.0 MIC.INI files are
			//backward compatible with the monolithic MIC
			fprintf(temp,"COMMUNICATIONS=ILON,IPX,SERIAL,LOCNUT\n");
			fprintf(temp,"INSTRUMENTS=APC_UPS,AQUILA_UPS,DSPEC,EVENT,GRAND,ISR,JSR,MCA,WATCH,HHMR,EOSS\n");
			fprintf(temp,"BUTTONWIDTH=175\n");
			fprintf(temp,"BUTTONFLAGSIZE=36\n");
			fprintf(temp,"BUTTONHEIGHT=70\n");
			fprintf(temp,"FASTSTARTUP=1\n");
			fprintf(temp,"NORMALSHUTDOWN=1\n");
			fprintf(temp,"IPDate=None\n");
			fprintf(temp,"SOH_PATH=C:\\MICSOH.TXT\n");
			fprintf(temp,"SOH_GOOD=GOOD\n");
			fprintf(temp,"SOH_BAD=BAD\n");
			fprintf(temp,"SOH_BADTOGOOD=GOOD NOW\n");
			fprintf(temp,"SOH_PATH_EXT=C:\\MIC_SOH_EXT.txt\n");
			fprintf(temp,"SOH_TIME_EXT=0\n");
			fprintf(temp,"SOH_DAILY=No\n");
			fprintf(temp,"SOH_DAILYAT=01:00:00\n");
			fprintf(temp,"MAX_REP=10\n");  //was 9999
			fprintf(temp,"VERSION=\n");
			fprintf(temp,"DOSHORTNAME=Yes\n");
			fprintf(temp,"LONGNAMEFORM=AA%%T%%I_%%y%%m%%d_%%H%%M%%S\n");
			fprintf(temp,"LAST_X=0\n");
			fprintf(temp,"LAST_Y=0\n");
			fprintf(temp,"AUTORESTARTCOUNT=0\n");
			fprintf(temp,"SLOWSTART=0\n\n");

			fprintf(temp,"[USERS]\n\n");
			
			fprintf(temp,"[NETWORK]\n");
			fprintf(temp,"ACTIVE=0\n");
			fprintf(temp,"PORT=1028\n");
			fprintf(temp,"BEAT=0\n");
			fprintf(temp,";1=0.0.0.0\n");
			fprintf(temp,";2=0.0.0.0\n\n");
			
			fprintf(temp,"[Communications]\n\n");
			
			fprintf(temp,"[Instruments]\n\n");
			fclose(temp);
		}
	}

	//get prior position of main dlg box and move there
	GetWindowRect(&m_sInitialRect);
	m_iLastX = GetPrivateProfileInt(CONFIGURATION,LAST_X,0,g_szIniFile);
	m_iLastY = GetPrivateProfileInt(CONFIGURATION,LAST_Y,0,g_szIniFile);

	m_sInitialRect.left = min(m_iLastX,::GetSystemMetrics(SM_CXSCREEN)-GetSystemMetrics(SM_CXICON));
	m_sInitialRect.top  = min(m_iLastY,::GetSystemMetrics(SM_CYSCREEN)-GetSystemMetrics(SM_CYICON));
	MoveWindow(&m_sInitialRect);

	//get the maximum number of repetitions for an entry in pfm
	g_iMaxRepetition = GetPrivateProfileInt(CONFIGURATION,MAX_REP,0,g_szIniFile);
	if (g_iMaxRepetition < 2)
	{
		g_iMaxRepetition = 10;  //<<QA>> this was 9999
		WritePrivateProfileString(CONFIGURATION,MAX_REP,"10",g_szIniFile);
	}

	//get the last one to validate
	GetPrivateProfileString(CONFIGURATION,LAST_VALIDATION,"",g_pVal_Name,sizeof(g_pVal_Name),g_szIniFile);//DR00115

	//were we shut down normally on the previous shutdown
	if (GetPrivateProfileInt(CONFIGURATION,NORMALSHUTDOWN,0,g_szIniFile)==0)
		m_bNormalShutdown = false;
	else
		m_bNormalShutdown = true;

	//slowly sequentially start the instruments so they won't all try
	//to hit the ilon all near the same time
	if (GetPrivateProfileInt(CONFIGURATION,FASTSTARTUP,1,g_szIniFile)==1)
		g_bNoPauseRestart = true;
	else
		g_bNoPauseRestart = false;

	//Get number of seconds between starting up ISO's.
	m_iSlowStartIntervalInSeconds = 
		GetPrivateProfileInt(CONFIGURATION,SLOWSTART,0,g_szIniFile);

	//show normal shutdown
	WritePrivateProfileString(CONFIGURATION,NORMALSHUTDOWN,"0",g_szIniFile);

	//get the width and height of each button and the flagsize; adjust flagsize
	g_iButtonWidth = 
		GetPrivateProfileInt(CONFIGURATION,BUTTONWIDTH,BUTTONWIDTH_DEFAULT,g_szIniFile);
	g_iButtonHeight= 
		GetPrivateProfileInt(CONFIGURATION,BUTTONHEIGHT,BUTTONHEIGHT_DEFAULT,g_szIniFile);
	g_iButtonFlagSize = 
		GetPrivateProfileInt(CONFIGURATION,BUTTONFLAGSIZE,BUTTONFLAGSIZE_DEFAULT,g_szIniFile);

	if (g_iButtonFlagSize < BUTTONFLAGSIZE_DEFAULT) g_iButtonFlagSize = BUTTONFLAGSIZE_DEFAULT;

	//Get the facility name
	GetPrivateProfileString(CONFIGURATION,FACILITYNAME,"Unknown",buff,sizeof(buff),g_szIniFile);
	strcpy(g_sFacilityName,buff);

	//Set the text in the title bar of the main MIC dialog
	SetWindowText(
		CString(g_sFacilityName) +
		CString(" - ") +
		CString(MIC_NAMESHORT) + 
		CString(" - ") + 
		g_csVersion);

	//get hidewindows flag
	g_bHideWindows = 
		GetPrivateProfileInt(CONFIGURATION,HIDEWINDOWS,HIDEWINDOWS_DEFAULT,g_szIniFile)!=0;

	//get the number of rows of ISO buttons to be presented on the main dialog
	g_iButtonColumns = 
		GetPrivateProfileInt(CONFIGURATION,BUTTONSWIDE,BUTTONSWIDE_DEFAULT,g_szIniFile);
	if (g_iButtonColumns < 1) 
		g_iButtonColumns = BUTTONSWIDE_DEFAULT;

	//Set the time and date started in the main dialog.
	CString cTemp(" Started:");
	COleDateTime cNow = COleDateTime::GetCurrentTime();
	cTemp += cNow.Format("%Y %b %d %H:%M:%S ");
	GetDlgItem(IDC_ALL_STATIC)->SetWindowText(cTemp);

	//if minimal windows then hide the control buttons and set the 
	//top of instrument buttons up high
	CRect rect;
	GetDlgItem(IDC_ALL_STATIC)->GetWindowRect(rect);
	ScreenToClient(&rect);

	if (g_iButtonWidth == 0) 
		g_iButtonWidth = rect.right - rect.left + 8;
	if (g_iButtonHeight== 0) 
		g_iButtonHeight= rect.bottom- rect.top;

	m_iTop = rect.bottom;

	if (g_bHideWindows) 
	{
		GetDlgItem(IDC_STARTDATA_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COPYDATA_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALL_STATIC)->ShowWindow(SW_HIDE);
		m_iTop = rect.top;
	}

	//get logarchiveact flag  <<QA>> what is this??
	g_blogarchiveact = GetPrivateProfileInt(
		CONFIGURATION,LOGARCHIVEACT,LOGARCHIVEACT_DEFAULT,g_szIniFile)!=0;

	//save the version to INI file
	LPCTSTR ptr = (LPCTSTR)g_csVersion;
	WritePrivateProfileString(CONFIGURATION,"VERSION",ptr,g_szIniFile);

	//get/save the State-Of-Health path\filename and messages
	GetPrivateProfileString(CONFIGURATION,SOH_PATH,"",buff,sizeof(buff),g_szIniFile);
	if (strlen(buff)==0)
		WritePrivateProfileString(CONFIGURATION,SOH_PATH,"C:\\MIC_SOH.TXT",g_szIniFile);
	GetPrivateProfileString(CONFIGURATION,SOH_GOOD,"",buff,sizeof(buff),g_szIniFile);
	if (strlen(buff)==0)
		WritePrivateProfileString(CONFIGURATION,SOH_GOOD,"GOOD",g_szIniFile);
	GetPrivateProfileString(CONFIGURATION,SOH_BAD,"",buff,sizeof(buff),g_szIniFile);
	if (strlen(buff)==0)
		WritePrivateProfileString(CONFIGURATION,SOH_BAD,"BAD",g_szIniFile);
	GetPrivateProfileString(CONFIGURATION,SOH_BADTOGOOD,"",buff,sizeof(buff),g_szIniFile);
	if (strlen(buff)==0)
		WritePrivateProfileString(CONFIGURATION,SOH_BADTOGOOD,"GOOD NOW",g_szIniFile);

	//get the maximum percent of disk usage
	GetPrivateProfileString(CONFIGURATION,MAXPERCENTDISKUSE,"99.99",buff,sizeof(buff),g_szIniFile);
	g_fMaxPercentDiskUse = (float)atof(buff);
	if (g_fMaxPercentDiskUse > (float)99.99)
		g_fMaxPercentDiskUse = (float)99.99;

	//get the number of minutes between automaticly aging files
	//in the archive directories
	g_iMinutesToCheckArchive = GetPrivateProfileInt(
		CONFIGURATION,
		MINUTESTOCHECKARCHIVE,
		MINUTESTOCHECKARCHIVE_DEFAULT,
		g_szIniFile);

	if (g_iMinutesToCheckArchive < 1) 
		g_iMinutesToCheckArchive = MINUTESTOCHECKARCHIVE_DEFAULT;

	//get the time to automatically close dialog boxes 
	int i = 
		GetPrivateProfileInt(CONFIGURATION,DIALOGCLOSE,DIALOGCLOSE_DEFAULT,g_szIniFile);
	if (i < 10) 
		i = DIALOGCLOSE_DEFAULT;
	g_iDlgCloseMillisec = i * 1000;

	//get the quantity of days before deleting files
	g_iDaysUntilDeleteFiles = GetPrivateProfileInt(
		CONFIGURATION,
		DAYSBEFOREDELFILES,
		DAYSBEFOREDELFILES_DEFAULT,
		g_szIniFile);

	if (g_iDaysUntilDeleteFiles < 1) 
		g_iDaysUntilDeleteFiles = DAYSBEFOREDELFILES_DEFAULT;

	//get the flag for short or long file names
	GetPrivateProfileString(
		CONFIGURATION,DOSHORTNAME,"Yes",buff,sizeof(buff),g_szIniFile);
	g_bUseShortName = (toupper(buff[0])=='Y')?true:false;

	//get the form for long file names
	GetPrivateProfileString(
		CONFIGURATION,LONGNAMEFORM,"AA%T%I_%y%m%d_%H%M%S",buff,sizeof(buff),g_szIniFile);
	g_csLongName = buff;

	//get the doing network notification flag
	g_bDoingNetwork = GetPrivateProfileInt(
		NETWORK,DOINGNETWORK,DOINGNETWORK_DEFAULT,g_szIniFile)==1;

	m_uiDoingNetworkToThatBrandNewBeat = GetPrivateProfileInt(NETWORK,BEATNIK,BEATNIK_DEFAULT,g_szIniFile);
	if ((m_uiDoingNetworkToThatBrandNewBeat > 0 && m_uiDoingNetworkToThatBrandNewBeat < 22)) 		// don't let it be less than 22 seconds or more than 168 hours , just because
		m_uiDoingNetworkToThatBrandNewBeat = 22;
	else if (m_uiDoingNetworkToThatBrandNewBeat > 604800000)
		m_uiDoingNetworkToThatBrandNewBeat = 604800000;
	m_BeatSeconds.SetDateTimeSpan(0,0,0,m_uiDoingNetworkToThatBrandNewBeat);

	//Instantiate the CSO's from the MIC.INI file
	int iRequested = 0;
	if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff), g_szIniFile)>0)
	{
		char *at = buff;
		while (strlen(at) > 0)
		{
			CString At(at);
			At.TrimRight();
			char name[32];
			strcpy(name,At.SpanExcluding((LPCTSTR)"="));
			if ((strlen(name)>0) && (name[0] != ';'))
			{
				AddComm(name, false);  //false says this is not a new entry
				iRequested++;
			}
			at = at + strlen(at)+1;
		}
	}

	//if we couldn't get at least one comm running
	int iAutoRestartCount = 
		GetPrivateProfileInt(CONFIGURATION,AUTORESTARTCOUNT,0,g_szIniFile);

	if (m_bValidShutdown && (g_iCommCount < iRequested))
	{
		if (iAutoRestartCount < 1)
		{
			WritePrivateProfileString(CONFIGURATION,AUTORESTARTCOUNT,"1",g_szIniFile);
			PostMessage(WM_QUIT,0,0);
			return FALSE;
		}
	}

	WritePrivateProfileString(CONFIGURATION,AUTORESTARTCOUNT,"0",g_szIniFile);

	m_bValidShutdown = false;
	

	//Instantiate the ISO's from the MIC.INI file
	if (GetPrivateProfileSection(INSTRUMENTS,buff, sizeof(buff), g_szIniFile) > 0)
	{
		//we must have at least one communications object or we wouldn't get here	
		char *at = buff;

		//step through list of instruments using a pointer into buff
		while ((strlen(at) > 0) && (g_iInstCount < MAX_INSTRUMENTS) ) 
		{
			//getting instrument names from INI file so strip off extra stuff
			CString At(at);
			At.TrimRight();
			char temp[32];
			strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
			if ((strlen(temp)>0) && (temp[0] != ';'))
			{
				AddInstrument(temp, false);  //false means not a new entry
			}
   			at = at + strlen(at)+1;
		}
		//if none of the instruments are valid then shutdown -- something is wrong 
		//with us so let the "other" mic computer do its thing
  		if (m_bValidShutdown && (g_iInstCount == 0)) 
		{
			PostMessage(WM_QUIT,0,0);
			return FALSE;
		}

		m_bValidShutdown = false;

		//reorganize the display 
		//ReorgDialogLayout();
	}

	//start up the file manager thread
	m_pFileMgr = (CFileMgr*)AfxBeginThread(RUNTIME_CLASS( CFileMgr ));
	if (m_pFileMgr != NULL)
	{
		m_pFileMgr->SetParentPointer(this);
		m_pFileMgr->SetIDC_COM_STOPRegisteredMessageValue(m_uIDC_COM_STOP);
	}

	//set multi-purpose 30-second timer
	SetTimer(TIMER_FILE_HEARTBEAT_SOH,30000,NULL);

	//make sure that nothing is paused if no pause on restart is true
	//or if we had a abnormal shutdown
	if (!m_bNormalShutdown || g_bNoPauseRestart)
		StartData(true);

	ShowWindow(SW_SHOW);
	//required 'cause some systems seem to miss the reorg above
	ReorgDialogLayout();  //<<QA>> check to see if this is needed above where I commented it out.

	//Disable the config function if there is no user set up
	if (pSysMenu != NULL)
	{
		CAccess Access(this);
		// set the menu text to do the right thing
		if (Access.CountUsers()==0)
			pSysMenu->EnableMenuItem(IDM_CONFIG_ALT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		else
			pSysMenu->EnableMenuItem(IDM_CONFIG_ALT, MF_BYCOMMAND | MF_ENABLED );
	}

	//how often does the user want to do an extended soh
	m_uiHours = GetPrivateProfileInt(CONFIGURATION,SOH_TIME_EXT,0,g_szIniFile);

	char szTemp[32];																		//scr00079
	//once per day yea or nea																//scr00079
	GetPrivateProfileString(CONFIGURATION,SOH_DAILY,"No",szTemp,sizeof(szTemp),g_szIniFile);//scr00079
	m_bDoOnce = (toupper(szTemp[0]) == 'Y');												//scr00079

	//what time is set in the ini for extended soh												//scr00079	
	GetPrivateProfileString(CONFIGURATION,SOH_DAILYAT,"1:00:00",m_szDailyAt,sizeof(m_szDailyAt),g_szIniFile);	//scr00079

	if ((m_uiHours > 0) || m_bDoOnce)															//scr00079
		SetTimer(TIMER_AUTO_SOH,60000,NULL);//dgp 16 dec 2004 1.9.0.8//now checks every minute

	if (m_iSlowStartIntervalInSeconds > 0)
	{
		m_iSlowStartIndex = 0;
		SetTimer(TIMER_SLOW_START, m_iSlowStartIntervalInSeconds * 1000 ,NULL);
	}

	if (m_iSlowStartIntervalInSeconds > 0)
	{
		m_iSlowStartIndex = 0;
		SetTimer(TIMER_SLOW_START, m_iSlowStartIntervalInSeconds * 1000 ,NULL);
	}

	SetNetworkHeartbeatTimer(true); // starts the timer only if the celestial objects are all aligned

	if (BadBeginningStatusMap.GetCount() > 0)
	{
		ShowStartupErrors();
	}

	Invalidate();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMicMgrDlg::ShowStartupErrors()
{
	CString CSO, ISO, combo;
	BadBeginningStatusMap.PrepareNoticeStrings(CSO, ISO);

	combo = "Configuration Errors Found";

	new CTimedMessageDialog(
				combo,
				CSO + ISO,
				g_iDlgCloseMillisec/1000,
				this);

}
//////////////////////////////////////////////////////////////////
//
//            OnQueryEndSession
//
//////////////////////////////////////////////////////////////////
BOOL CMicMgrDlg::OnQueryEndSession()
{
	//<<QA>> this apparently never gets called.  Where else do we tell
	//the UPS to go dumb?  Sent email to DP on April 4, 2005.
	//make sure we are not trying to talk to the instruments
	StartData(false);

	for (int i = 0; i < g_iInstCount;i++)
	{
		if (g_pIISO[i])
				g_pIISO[i]->UPSGoDumb();
	}

	ShutDownWork();
	//considered putting this in to be sure the instruments
	//have finished sending us stuff
	//tell NT it is ok to shut down

	return TRUE;
}
/////////////////////////////////////////////////////////////////
//
//                      OnSOHCmd
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnSOHCmd(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
//	TRACE("CMicMgrDlg::OnSOHCmd\n");

	char szFileName[MAX_PATH];
	char szFileNameTemp[MAX_PATH];
	strcpy(szFileName,"C:\\MIC_SOH_EXT.TXT");
	szFileNameTemp[0] = NULL;

	GetPrivateProfileString(
		CONFIGURATION,
		SOH_PATH_EXT,"",szFileNameTemp,sizeof(szFileNameTemp),g_szIniFile);

	if (strlen(szFileNameTemp)==0)
		WritePrivateProfileString(CONFIGURATION,SOH_PATH_EXT,szFileName,g_szIniFile);
	else
		strcpy(szFileName,szFileNameTemp);

	if (strchr(szFileName,'%'))
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		strcpy(szFileName,now.Format(szFileName));
	}
	DoSOHCmd(szFileName);
	return 0;
}
/////////////////////////////////////////////////////////////////
//
//                      OnStartdataButton
//
//////////////////////////////////////////////////////////////////
//toggle pause 
void CMicMgrDlg::OnStartdataButton() 
{
	//if at least one ISO is Collecting Data, then the
	//button label going in is "Pause All"
	StartData(!m_bAtLeastOneISOIsCollectingData);
}
//////////////////////////////////////////////////////////////////
//
//            OnSysCommand
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == IDM_CONFIG_ALT)
	{
		OnConfigurationMenuItem();
	} 
	else if (nID == IDM_ACCESS_ALT)
	{
		OnAccessControlMenuItem();
	} 
	else if (nID == IDM_EXIT_ALT)
	{
		ShutDown();
	}
	else if (nID == IDM_COPY_ALT)
	{
		OnCopydataButton();
	} 
	else if (nID == IDM_CEREAL_BOWL)
	{
		OnEnumPorts();
	}
	else if (nID == IDM_DATA_ALT)
	{
		OnStartdataButton();
	} 
	else if (nID == IDM_ARCH_ALT)
	{
		OnArchiveCheck();
	} 
	else if (nID == IDM_REORG_ALT) 
	{
		ReorgBySuperuser();
	}
	else if (nID == SOH_CMD)	//SCR00361
	{							//SCR00361
		OnSOHCmd(NULL,NULL);	//SCR00361
	}							//SCR00361
	else if (nID == IDM_DT0PAUSE)
	{
		OnDT0Pause();
	}
	else if (nID == SOH_CMD_MENU) 
	{
		char szTemp[MAX_PATH];
		CTimedIntegerDlg cTimeDlg;
		cTimeDlg.m_iTimeCount = g_iDlgCloseMillisec/1000;
		cTimeDlg.m_cMsg0 = "Extended State of Health";
		cTimeDlg.m_cMsg1 = "MIC can generate the extended state of health files automatically."
			"  Set the time to the desired number of hours between each generation."
			"  Set to 0 for never."
			"  File name may contain %Y_%m_%d %H,%M,%S"
		    "  Check \"Do once per day\" and set \"0\" above to force a specific time.";
		cTimeDlg.m_cMsg2 = "Generate extended SOH files every (hours):";											//scr00079
		cTimeDlg.m_cMsg3 = "Location and File Name: (invalid chars: ";
		cTimeDlg.m_cMsg3.Append(INVALIDFILENAMECHARS);
		cTimeDlg.m_cMsg3.Append(")\n (Example: C:\\SOH_%Y_%m_%d_%H_%M_%S.txt)");
		m_uiHours = GetPrivateProfileInt(CONFIGURATION,SOH_TIME_EXT,0,g_szIniFile);

		GetPrivateProfileString(CONFIGURATION,SOH_LIMITED_EXT,"No",szTemp,sizeof(szTemp),g_szIniFile);
		cTimeDlg.m_bLimited = (toupper(szTemp[0]) == 'Y');

		GetPrivateProfileString(CONFIGURATION,SOH_APPEND,"No",szTemp,sizeof(szTemp),g_szIniFile);
		cTimeDlg.m_bAppend = (toupper(szTemp[0]) == 'Y');

		GetPrivateProfileString(CONFIGURATION,SOH_PATH_EXT,"",szTemp,sizeof(szTemp),g_szIniFile);
		cTimeDlg.m_cEditName = szTemp;

		GetPrivateProfileString(CONFIGURATION,SOH_DAILY,"No",szTemp,sizeof(szTemp),g_szIniFile);					//scr00079
		cTimeDlg.m_bDoOnce = m_bDoOnce = (toupper(szTemp[0]) == 'Y');												//scr00079
		
		COleDateTime cNow = COleDateTime::GetCurrentTime();															//scr00079
		GetPrivateProfileString(CONFIGURATION,SOH_DAILYAT,"01:00:00",m_szDailyAt,sizeof(m_szDailyAt),g_szIniFile);	//scr00079
		if (strlen(m_szDailyAt) == 0)																				//scr00079
			strcpy(m_szDailyAt,"01:00:00");																			//scr00079
		int iH,iM,iS;																								//scr00079
//try{
		sscanf(m_szDailyAt,"%d:%d:%d",&iH,&iM,&iS);																	//scr00079
//}
//catch(...)
//{
//	AfxMessageBox("Error at 8",MB_OK);
//	AfxMessageBox(cTimeDlg.m_cEditCtrl,MB_OK);
//}
		CTime cTime = CTime(cNow.GetYear(),cNow.GetMonth(),cNow.GetDay(),iH,iM,iS);;								//scr00079
		cTimeDlg.m_cTime = cTime;																					//scr00079
				
		cTimeDlg.m_iPos = m_uiHours;
 		if (cTimeDlg.DoModal()==IDOK)
		{
			WritePrivateProfileString(CONFIGURATION,SOH_TIME_EXT,cTimeDlg.m_cEditCtrl,g_szIniFile);
			WritePrivateProfileString(CONFIGURATION,SOH_PATH_EXT,cTimeDlg.m_cEditName,g_szIniFile);
			WritePrivateProfileString(CONFIGURATION,SOH_LIMITED_EXT,(cTimeDlg.m_bLimited?"Yes":"No"),g_szIniFile);
			WritePrivateProfileString(CONFIGURATION,SOH_APPEND,(cTimeDlg.m_bAppend?"Yes":"No"),g_szIniFile);
			WritePrivateProfileString(CONFIGURATION,SOH_DAILY,(cTimeDlg.m_bDoOnce?"Yes":"No"),g_szIniFile);			//scr00079
			strcpy(m_szDailyAt,cTimeDlg.m_cTime.Format("%H:%M:%S"));												//scr00079
			WritePrivateProfileString(CONFIGURATION,SOH_DAILYAT,m_szDailyAt,g_szIniFile);							//scr00079
//try{
			sscanf(cTimeDlg.m_cEditCtrl,"%d",&m_uiHours);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 9",MB_OK);
//	AfxMessageBox(cTimeDlg.m_cEditCtrl,MB_OK);
//}

			if (m_uiHours > 0)
			{
				COleDateTime cNow = COleDateTime::GetCurrentTime();													//scr00079
				WritePrivateProfileString(CONFIGURATION,SOH_TIME_EXT_LAST,											//scr00079
					cNow.Format("%m/%d/%Y %H:00:00"),g_szIniFile);													//scr00079
			}																										//scr00079
			//if (cTimeDlg.m_bDoOnce)																					//scr00079
			//{																										//scr00079
			//	SetTimer(TIMER_AUTO_SOH,60000,NULL);																//scr00079
			//}																										//scr00079
			if ((m_uiHours <= 0) && !cTimeDlg.m_bDoOnce)															//scr00079
			{
				KillTimer(TIMER_AUTO_SOH);
				m_uiHours = 0;
			}
			else
				SetTimer(TIMER_AUTO_SOH,60000,NULL);

			OnSOHCmd(NULL,NULL);
		}
	}
	else
	{
		if (nID == SC_CLOSE)
		{
			StartData(false);
		}
		//comment these lines out to dissallow left mouse click on icon
		if ((nID != 0xf100) && (lParam != 0xf100))
		{
			CDialog::OnSysCommand(nID, lParam);
		}
	}
}

/////////////////////////////////////////////////////////////////
//
//                      OnTimer
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == TIMER_STALE_STATUS_HEARTBEAT)
	{
		NetworkHeartbeat();
	}
	else if (nIDEvent == TIMER_PAUSE_ALL1)						//SCR00361
	{														//SCR00361
		if (OnQueryAllPaused(0,0) == 0)						//SCR00361
		{													//SCR00361
			KillTimer(TIMER_PAUSE_ALL1);					//SCR00361
			int iMinutes = GetPrivateProfileInt(CONFIGURATION,		//SCR00361
				"PAUSE_ALL_TIME",TIMER_PAUSE_ALL_TIME,g_szIniFile);	//SCR00361
			iMinutes *= 60000;								//SCR00361
			SetTimer(TIMER_PAUSE_ALL2,iMinutes,NULL);		//SCR00361
			OnISOPauseStatusChange(0,0);						//SCR00361
		}													//SCR00361
	}														//SCR00361
	else if (nIDEvent == TIMER_PAUSE_ALL2)					//SCR00361
	{														//SCR00361
		KillTimer(TIMER_PAUSE_ALL2);						//SCR00361
		OnDriveAllToZero(0,0);								//SCR00361
	}														//SCR00361
	else if (nIDEvent == TIMER_PAUSE_ALL3)
	{
		if (OnQueryAllPaused(NULL,NULL) == 0)
		{
			KillTimer(TIMER_PAUSE_ALL3);
			ShutDownFinal();
		}
	}
	else if (nIDEvent == TIMER_SLOW_START)					//SCR00361
	{
		VARIANT_BOOL vb_is_paused;

		if (m_iSlowStartIndex < g_iInstCount)
		{
			g_pIISO[m_iSlowStartIndex]->get_IsPaused(&vb_is_paused);

			if (vb_is_paused == VARIANT_TRUE)
				g_pIISO[m_iSlowStartIndex]->PauseInstrument(VARIANT_FALSE);

			m_iSlowStartIndex++;
		}
		else
		{
			KillTimer(TIMER_SLOW_START);
			m_iSlowStartIndex = 0;
		}
	}
	else if (nIDEvent == TIMER_AUTO_SOH)
	{
		//dgp 16 dec 2004
		//
		//this block is completely changed in 1.9.0.8
		//the previous just started a timer of length of time between extended SOH
		//this works iff mic is not power cycling or mic is not being brought down
		//daily during maintenance and the time was set to something larger than the
		//time between being brought down.  
		//
		//The new technique is to write the time of the last (M/D/YYYY H:0:0) to the 
		//ini file then once per minute read that time, add the offset to it and compare
		//to the current time.  Consequently, even if MIC is being brought down, as long
		//as it has been up for a minute a ext SOH file and the period between them has
		//been met it will be written

		char szTemp[64];						//much of this block changed
		bool bDoIt = false;
		COleDateTime cNow = COleDateTime::GetCurrentTime();
		COleDateTime cLastAt;																				//scr00079

		//do every m_uiHours
		m_uiHours = GetPrivateProfileInt(CONFIGURATION,SOH_TIME_EXT,0,g_szIniFile);

		//when was the last time SOH was written
		GetPrivateProfileString(CONFIGURATION,SOH_TIME_EXT_LAST,"",szTemp,sizeof(szTemp),g_szIniFile);		//scr00079
		cLastAt.ParseDateTime(szTemp);																		//scr00079
																											//scr00079
		//don't do it if someone has set the ini file to 0 hours											//scr00079
		if (m_uiHours)																						//scr00079
		{																									//scr00079
			if (cLastAt.GetStatus()==COleDateTime::valid)					  								//scr00079
			{																								//scr00079
				//add the offset to the last time to see what the next time should be						//scr00079
				COleDateTime cRequiredAt;																	//scr00079
				cRequiredAt = cLastAt + COleDateTimeSpan(0,m_uiHours,0,0);									//scr00079
				//if now is past the needed time then do the SOH											//scr00079
				if (cRequiredAt < cNow)																		//scr00079
					bDoIt = true;																			//scr00079
			}																								//scr00079
			else																							//scr00079
			{																								//scr00079
				//this is the first time through so do it now												//scr00079
				//cNeededAt = cNow;																			//scr00079
				bDoIt = true;																				//scr00079
			}																								//scr00079
		}																									//scr00079

		//are we configured to do once per day
		if (m_bDoOnce)																						//scr00079
		{
			COleDateTime cDailyAt;																			//scr00079
			//what time is set in the ini for extended soh													//scr00079
			GetPrivateProfileString(CONFIGURATION,SOH_DAILYAT,"1:00:00",m_szDailyAt,sizeof(m_szDailyAt),g_szIniFile);		//scr00079
																											//scr00079
			int iFields, iH, iM, iS;																		//scr00079
//try{
			iFields = sscanf(m_szDailyAt,"%d:%d:%d",&iH,&iM,&iS);											//scr00079
//}
//catch(...)
//{
//	AfxMessageBox("Error at 7",MB_OK);
//	AfxMessageBox(m_szDailyAt,MB_OK);
//}
			if (iFields == EOF)																				//scr00079
			{																								//scr00079
				iM = iS = 0;																				//scr00079
				iH = 1;																						//scr00079
				WritePrivateProfileString(CONFIGURATION,SOH_DAILYAT,"1:00:00",g_szIniFile);					//scr00079
			}																								//scr00079
			cDailyAt = COleDateTime(cNow.GetYear(),cNow.GetMonth(),cNow.GetDay(),iH,iM,iS);					//scr00079
			//if last < m_cDailyAt then do it																//scr00079
			if (cLastAt.GetStatus() == COleDateTime::valid)													//scr00079
			{																								//scr00079
				if ((cLastAt < cDailyAt) && (cNow >= cDailyAt)) //in this case cNeededAt					//scr00079
					bDoIt = true;																			//scr00079
			}
			else //catch the error state																	//scr00079
				bDoIt = true;																				//scr00079
		}																									//scr00079
																											//scr00079
		if (bDoIt)																							//scr00079
		{																									//scr00079
			//save the time we did it M/D/Y H:0:0															//scr00079
			WritePrivateProfileString(CONFIGURATION,SOH_TIME_EXT_LAST,										//scr00079
				cNow.Format("%m/%d/%Y %H:%M:%S"),g_szIniFile);												//scr00079
			//tell MIC to do it																				//scr00079
			PostMessage(SOH_CMD);																			//scr00079
		}																									//scr00079
																											//scr00079
		if ((m_uiHours <= 0) && !m_bDoOnce)																	//scr00079
			KillTimer(TIMER_AUTO_SOH);

	}
	else if (nIDEvent == TIMER_FILE_HEARTBEAT_SOH)
	{
		//tell the file manager to look at files  //<<QA>> 
		if (m_pFileMgr)
			m_pFileMgr->PostThreadMessage(IDC_TIME_CHECK,0,2);

		//step through all com objects
		//if com object is ILON
		//	tickle watchdogsend
		for (int i = 0; i < g_iCommCount; i++)
		{
			if (NULL != g_pICSO[i])
				g_pICSO[i]->SendHeartbeat();  //Was WatchDogSend();
		}

		//is there a change in state of health
		int StateOfHealth = 1;
		int temp = 0;
		int mask = (FLAG_BIT | OTHR_BIT | DISK_BIT | FLASH_BIT);

		CTime theTime = CTime::GetCurrentTime();
		if (theTime.GetDay() != m_iSOHDay)
		{
			m_iSOHDay = theTime.GetDay();
			m_bSOHNewDay = true;
		}
		else
			m_bSOHNewDay = false;

		for (int i = 0; i < g_iInstCount; i++)
		{
			ULONG ucolor;
			if (NULL != g_pICB[i])
				g_pICB[i]->get_FaceColor(&ucolor);

			if (ucolor == COLOR_RED)
			{
				StateOfHealth = -1;
			}
			else
			{
				if (NULL != g_pColorButton[i])
					temp = g_pColorButton[i]->GetState();

				if (temp & mask)
				{
					StateOfHealth = (StateOfHealth == -1) ? -1 : 0;
				}
			}
		}

		if ((StateOfHealth != m_iStateOfHealth) || m_bSOHNewDay)
		{
			char buff[_MAX_PATH];
			char buffgood[128];
			char buffbad[128];
			char buffbadnowgood[128];
			GetPrivateProfileString(CONFIGURATION,SOH_PATH,"",buff,sizeof(buff),g_szIniFile);
			GetPrivateProfileString(CONFIGURATION,SOH_GOOD,"GOOD",buffgood,sizeof(buffgood),g_szIniFile);
			GetPrivateProfileString(CONFIGURATION,SOH_BAD,"BAD",buffbad,sizeof(buffbad),g_szIniFile);
			GetPrivateProfileString(CONFIGURATION,SOH_BADTOGOOD,"GOOD NOW",buffbadnowgood,sizeof(buffbadnowgood),g_szIniFile);
			if (StateOfHealth == 0)
			{
				strcpy(buffbad,buffbadnowgood);
			}

			if (StateOfHealth==1)
			{
				//green ICONS
				SetIcon(m_hIcon32, TRUE);		// Set big icon
				SetIcon(m_hIcon16, FALSE);		// Set small icon
			}
			else
			{
				//RED ICONS
				SetIcon(m_hIcon32b, TRUE);		// Set big icon
				SetIcon(m_hIcon16b, FALSE);		// Set small icon
				//if we were minimized then restore the window
				if (IsIconic())
					ShowWindow(SW_RESTORE);
			}

			m_iStateOfHealth = StateOfHealth;
			if (strlen(buff))
			{
				FILE* sohfile = fopen(buff,"wt");
				if (sohfile)
				{
					fprintf(sohfile, (m_iStateOfHealth==1)?buffgood:buffbad);
					fclose(sohfile);
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////
//
//                     OnQueryDragIcon
//
//////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMicMgrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon32;
}
//////////////////////////////////////////////////////////////////
//
//                    OnArchiveCheck
//
//////////////////////////////////////////////////////////////////
//time to check archive files
void CMicMgrDlg::OnArchiveCheck()
{
	//call the function in the file manager to force an archive check
	if (m_pFileMgr) 
		m_pFileMgr->DoArchiveCheck();
}
//////////////////////////////////////////////////////////////////
//
//                     OnISOPauseStatusChange
//
//////////////////////////////////////////////////////////////////
//Look at each instrument and change the menu and button text 
//depending if all are paused or all are not paused.
LRESULT CMicMgrDlg::OnISOPauseStatusChange(WPARAM wParam, LPARAM lParam)
{
	//TRACE("CMicMgrDlg::OnISOPauseStatusChange()");  //Coupled with TRACE below
	VARIANT_BOOL vb;

	m_bAtLeastOneISOIsCollectingData = false;

	for (int i = 0; i < g_iInstCount; i++) 
	{
		if ((g_eISOCategory[i] == STANDARD_ISO) || (g_eISOCategory[i] == UPS_ISO))
		{
			g_pIISO[i]->get_IsPaused(&vb);

			if (vb == VARIANT_FALSE)
			{
				m_bAtLeastOneISOIsCollectingData = true;
			}
		}
	}

	CMenu* pSysMenu = GetSystemMenu(FALSE);

	UINT nFlags = MF_BYCOMMAND | MF_STRING;

	if (!m_bAtLeastOneISOIsCollectingData)
	{
		GetDlgItem(IDC_STARTDATA_BUTTON)->SetWindowText("Start All");
		pSysMenu->ModifyMenu( IDM_DATA_ALT, nFlags, IDM_DATA_ALT, "&Start All\tS");
	}
	else
	{
		GetDlgItem(IDC_STARTDATA_BUTTON)->SetWindowText("Pause All");
		pSysMenu->ModifyMenu( IDM_DATA_ALT, nFlags, IDM_DATA_ALT, "&Pause All\tP");
	}


//#ifdef _DEBUG
//	bool b = m_bAtLeastOneISOIsCollectingData;
//	TRACE(" m_bAtLeastOneISOIsCollectingData = %s\n",b?"true":"false");
//#endif
	return (LRESULT)0;
}
//////////////////////////////////////////////////////////////////
//
//                         OnClose
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnClose() 
{
//	TRACE("CMicMgrDlg::OnClose()\n");
	ShutDown();
}
//////////////////////////////////////////////////////////////////
//
//                         OnCmdMsg
//
//////////////////////////////////////////////////////////////////
BOOL CMicMgrDlg::OnCmdMsg(
	UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	//catch escape and don't do anything with it
	if ((nID == 2) && ((nCode == -1)||(nCode == 0)))
		return TRUE;
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

// enumerate available and in-use COM ports, use a direct port open operation, do not use registry enumeraiton
void CMicMgrDlg::OnEnumPorts()
{
	CString ** p = EnumerateSerialPorts();
	
	CComPortList x(this, p);	
	//bring up the dialog box
	x.DoModal();

	for (int myi = 0; myi < MAX_COMM; myi++)
	{
		if (p[myi])
		{
			delete p[myi];			
		}
	}
	if (p) delete [] p;	

}


//////////////////////////////////////////////////////////////////
//
//            OnConfigurationMenuItem
//
//////////////////////////////////////////////////////////////////
//configure menu item selected
void CMicMgrDlg::OnConfigurationMenuItem() 
{
	CConfig Config(this);	
	//hide dialog if we need to
	if (g_bHideWindows) 
	  ShowWindow(SW_HIDE);
	//bring up the config dialog box
	Config.DoModal();

	//<<QA>> if this logic is needed, it goes in config.cpp
	//if (Config.m_bApplied)
	//{
	//	//tell each of the inst a reconf occured
	//	for (int i = 0; i < g_iInstCount; i++)
	//	{
	//		g_pInstruments[i]->ReConfig();
	//	}
	//}

    //reorganize the display
	ReorgDialogLayout();
	//show dialog in case we hid it above
	ShowWindow(SW_SHOW);
}
//////////////////////////////////////////////////////////////////
//
//                   OnInstrumentButton
//                   OnInstrumentButtonDbl
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnInstrumentButton( UINT nID )
{
//	TRACE("CMicMgrDlg::OnInstrumentButton()\n");

	UINT index = nID - IDC_INSTRUMENT_BUTTON;  //<<QA>> debug
	int instnum = g_iInstCount;  //<<QA>> debug

	g_pIISO[nID - IDC_INSTRUMENT_BUTTON]->ShowISOGUI(VARIANT_TRUE);
}
void CMicMgrDlg::OnInstrumentButtonDbl(UINT nID)
{
	g_pIISO[nID - IDC_INSTRUMENT_BUTTON]->ResetISO();
}
//////////////////////////////////////////////////////////////////
//
//            OnMICXferCmd
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnMICXferCmd(WPARAM wParam, LPARAM lParam)
{
//	ASSERT(FALSE);  //<<QA>>
	/*
	for (int instrument = 0; instrument < g_iInstCount; instrument ++) 
	{
		try {
		if (g_dInstType[instrument] == APC_UPS)
			((CAPCInst*)g_pInstruments[instrument])->PostMessage(MICXFER_CMD,wParam,lParam);
		}
		catch(...)
		{
		}
	}
	*/
	return 0;
}
//////////////////////////////////////////////////////////////////
//
//                         OnMove
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	CRect current_rect;				// the main dialog box rectangle
	GetWindowRect(&current_rect);	// get the value
	m_sInitialRect.left = current_rect.left;
	m_sInitialRect.top  = current_rect.top;

	//update position of dlg box to ini file
	char temp[16];
	if (strlen(g_szIniFile))
	{
		WritePrivateProfileString(CONFIGURATION,
			LAST_X,
			_itoa(m_sInitialRect.left,temp,10),
			g_szIniFile);

		WritePrivateProfileString(CONFIGURATION,
			LAST_Y,
			_itoa(m_sInitialRect.top,temp,10),
			g_szIniFile);
	}
}
//////////////////////////////////////////////////////////////////
//
//                      OnOK
//
//////////////////////////////////////////////////////////////////
//received an ok (e.g. enter was pressed)
void CMicMgrDlg::OnOK()
{
	//catch the return and the space bar as 
	//button pressed events -- we don't want enter to mean shutdown
	int  index = GetFocus()->GetDlgCtrlID();
	if ((index >= IDC_INSTRUMENT_BUTTON)&&(index <(IDC_INSTRUMENT_BUTTON+g_iInstCount)))
		OnInstrumentButton(index);
	//the other buttons are caught automatically
}
//////////////////////////////////////////////////////////////////
//
//                      OnQueryAllPaused
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnQueryAllPaused(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
//	TRACE("CMicMgrDlg::OnQueryAllPaused()\n");

	int number_not_paused_yet = 0;  //assume all are paused or don't count

	VARIANT_BOOL vbInShutDown, vbInStartUp, vbIsPaused;

	for (int i = 0; i < g_iInstCount;i++)
	{
//		ASSERT(g_pIISO[i] != NULL); //if this isn't so, then we have coded incorrectly

		if (g_eISOCategory[i] == STANDARD_ISO)
		{
			g_pIISO[i]->get_InShutDown(&vbInShutDown);
			g_pIISO[i]->get_InStartUp(&vbInStartUp);
			g_pIISO[i]->get_IsPaused(&vbIsPaused);

			//not in startup or failure
			if ((vbInShutDown == VARIANT_FALSE) && (vbInStartUp == VARIANT_FALSE))
			{
				if (vbIsPaused == VARIANT_FALSE)
					number_not_paused_yet++;
			}
		}
	}
	return (LRESULT)number_not_paused_yet;
}
/////////////////////////////////////////////////////////////////
//
//                      OnTrigger
//                      OnTriggerClr
//
//////////////////////////////////////////////////////////////////
#ifdef ALLOWTRIGGERHOLDOFF
void CMicMgrDlg::OnTrigger(WPARAM wParam, LPARAM lParam)
{
	//accept upper or lower case in lparam
	//qualify it to the range
//	if ((lParam <= 0) || (lParam > 60))
//		lParam = -1;
	
	switch (wParam) 
	{
	case TRIGGER_TRIP:
	case TRIGGER_ON:
		{
			char szRelayTo[MAX_PATH];
			szRelayTo[0] = NULL;
			GetPrivateProfileString("EXCLUSIONS","RELAYTO","",szRelayTo,sizeof(szRelayTo),g_szIniFile);
			if (strlen(szRelayTo))
			{
				CWnd *cWnd = FindWindow(NULL,szRelayTo);
				if (cWnd)//#define TRIGGER_EXCLUDE		(WM_USER + 42)
					cWnd->PostMessage(TRIGGER_EXCLUDE,NULL,NULL);
			}
			//for (int i=0;i<g_iInstCount;i++) 
			//{
			//	if  (
			//		(g_dInstType[i] == ISR) ||
			//		(g_dInstType[i] == GRAND) ||
			//		(g_dInstType[i] == MCA) ||
			//		(g_dInstType[i] == DSPEC)
			//		)
			//		g_pInstruments[i]->AddExclusion(lParam);
			//}
			ULONG ulong;
			for (int i = 0; i < g_iInstCount; i++) 
			{
//				ASSERT(g_pIISO[i] != NULL);
				g_pIISO[i]->get_ISOCategory(&ulong);

				//<<QA>> put this back in.  Need to add "AddExclusion" to IISO
				//if (ulong == (ISO_FUNCTIONAL_CATEGORY)STANDARD_ISO)
				//	g_pIISO[i]->AddExclusion(lParam);
			}


		}
		break;

	case TRIGGER_OFF:
		break;
	}
}

void CMicMgrDlg::OnTriggerClr(WPARAM, LPARAM)
{
	//for (int i=0;i<g_iInstCount;i++) 
	//{
	//	if  (
	//		(g_dInstType[i] == ISR) ||
	//		(g_dInstType[i] == GRAND) ||
	//		(g_dInstType[i] == MCA) ||
	//		(g_dInstType[i] == DSPEC)
	//		)
	//		g_pInstruments[i]->DelExclusion();
	//}

	ULONG ulong;
	for (int i = 0; i < g_iInstCount; i++) 
	{
//		ASSERT(g_pIISO[i] != NULL);
		g_pIISO[i]->get_ISOCategory(&ulong);

		//<<QA>> put this back in.  Need to add "DelExclusion" to IISO
		//if (ulong == (ISO_FUNCTIONAL_CATEGORY)STANDARD_ISO)
		//	g_pIISO[i]->DelExclusion();
	}

}
#endif
//////////////////////////////////////////////////////////////////
//
//                   OnValidationRequest
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnValidationRequest(WPARAM wsuperuser, LPARAM)
{
	//TRACE("CMicMgrDlg::OnValidationRequest()\n");
	bool superuser = (wsuperuser != 0);
	CValidate cvalid(this,superuser);
	bool success = (IDOK == cvalid.DoModal());
	if (success)												//DR00115
	{															//DR00115
		char* szUserName = cvalid.GetUserName();				//DR00115
		WritePrivateProfileString(CONFIGURATION,LAST_VALIDATION,szUserName,g_szIniFile);//DR00115
	}															//DR00115
	return (success?1:0);
}
//////////////////////////////////////////////////////////////////
//
//               OnCopydataButton
//
//////////////////////////////////////////////////////////////////
//copy data button or menu item selected
void CMicMgrDlg::OnCopydataButton() 
{
//	TRACE("CMicMgrDlg::OnCopydataButton\n");

	//<<QA>> This is a handy place to put test code...

	if (m_pFileMgr)
	{
		//disable the copydata button
		GetDlgItem(IDC_COPYDATA_BUTTON)->EnableWindow(false);
		//disable the copydata menu item
		CMenu* pSysMenu = GetSystemMenu(FALSE);
		pSysMenu->EnableMenuItem(IDM_COPY_ALT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		//send the file manager a message to do its thing
		m_pFileMgr->PostThreadMessage(IDC_COPY_DATA,0,0);
	}
}
//////////////////////////////////////////////////////////////////
//
//               OnCopyDone
//
//////////////////////////////////////////////////////////////////
//received a copy files done message
LRESULT CMicMgrDlg::OnCopyDone(WPARAM w, LPARAM l)
{
//	TRACE("CMicMgrDlg::OnCopyDone\n");
	//set the pause all inst buttons and set text as needed		
	OnISOPauseStatusChange(w, l);
	//enable copy data button
	GetDlgItem(IDC_COPYDATA_BUTTON)->EnableWindow(true);
	//enable copy data menu item
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	pSysMenu->EnableMenuItem(IDM_COPY_ALT, MF_BYCOMMAND | MF_ENABLED);
	return 0;
}
//////////////////////////////////////////////////////////////////
//
//                    OnCtlColor
//
//////////////////////////////////////////////////////////////////
//place the background if desired
HBRUSH CMicMgrDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
#ifdef BMPBACKGRND
	//force all statics to transparent
	if (nCtlColor == CTLCOLOR_STATIC) 
		pDC->SetBkMode(TRANSPARENT);
	
	return (HBRUSH)m_brush;
#else
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
#endif
}
//////////////////////////////////////////////////////////////////
//
//                    OnDelCommMsg
//
//////////////////////////////////////////////////////////////////
//received a message to remove a com object 
LRESULT CMicMgrDlg::OnDelCommMsg(WPARAM /*WParam*/, LPARAM LParam)
{
//	TRACE("CMicMgrDlg::OnDelCommMsg()\n");

	char input_name[32], cso_name[32];
	strncpy(input_name,(char*)LParam,sizeof(input_name)); //make local copy of the name of the CSO
	CoTaskMemFree((void *)LParam);                        //release the storage passed to us

	//remove Name from the ini file
	WritePrivateProfileString(COMMUNICATIONS,input_name,NULL,g_szIniFile);
	WritePrivateProfileSection(input_name,NULL,g_szIniFile);

	BSTR bstr;
	HRESULT hr;

	for (int i = 0; (i < g_iCommCount); i++)
	{
		if (g_pICSO[i] != NULL)
		{
			g_pICSO[i]->get_Name(&bstr);
			Convert_BSTR_to_Character_String(bstr,cso_name,sizeof(cso_name));

			if ((strcmp(cso_name,input_name)) == 0) //we found it
			{
				hr = g_pICSO[i]->ShutdownCSO();
				hr = g_pICSO[i]->Release();
				g_pICSO[i] = NULL;

				//compact the g_pICSO array
				for (int j = i; j <= g_iCommCount - 1; j++)
					g_pICSO[j] = g_pICSO[j+1];

				g_iCommCount--; //decrement current number of objects
				break;
			}
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////
//
//                    OnDelInstMsg
//
//////////////////////////////////////////////////////////////////
//received a message to delete an instrument object 
LRESULT CMicMgrDlg::OnDelInstMsg(WPARAM /*WParam*/, LPARAM LParam)
{
//	TRACE("CMicMgrDlg::OnDelInstMsg()\n");

	char input_name[32], iso_name[32];
	strncpy(input_name,(char*)LParam,sizeof(input_name));
	CoTaskMemFree((void *)LParam);

	//remove Name from the ini file
	WritePrivateProfileString(INSTRUMENTS,input_name,NULL,g_szIniFile);
	WritePrivateProfileSection(input_name,NULL,g_szIniFile);

	BSTR bstr;
	HRESULT hr;
	bool bGotIt = false;
	int index = -1;

	//find the index of the ISO to be deleted
	for (int loopdex = 0; loopdex < g_iInstCount; loopdex++)
	{
		if (g_pIISO[loopdex] != NULL)
		{
			g_pIISO[loopdex]->get_Name(&bstr);
			Convert_BSTR_to_Character_String(bstr,iso_name,sizeof(iso_name));

			if ((strcmp(iso_name,input_name)) == 0) //we found it
			{
				bGotIt = true;
				index = loopdex;
				break;
			}
		}
	}

	if (bGotIt)
	{
		//reset the network participants
		((CColorButton*)GetDlgItem(IDC_INSTRUMENT_BUTTON))->ReCount(true);

		//destroy the ISO
		hr = g_pIISO[index]->PauseInstrument(VARIANT_TRUE);
		hr = g_pIISO[index]->ShutdownISO();
		hr = g_pIISO[index]->Release();
		//destroy its associated button interface and object
		hr = g_pICB[index]->Release();
		delete g_pColorButton[index];

		//compact the instrument-associated arrays
		int j = 0;
		for (int i = 0; i < g_iInstCount; i++)
		{
			if (i != index)
			{
				g_pIISO[j]        = g_pIISO[i];
				g_pICB[j]         = g_pICB[i];
				g_pColorButton[j] = g_pColorButton[i];
				g_eISOCategory[j] = g_eISOCategory[i];
				//g_dInstType[j]    = g_dInstType[i];
				((CButton*)GetDlgItem(IDC_INSTRUMENT_BUTTON+i))->SetDlgCtrlID(IDC_INSTRUMENT_BUTTON+j);
				((CColorButton*)GetDlgItem(IDC_INSTRUMENT_BUTTON+j))->ReCount();
				j++;
			}
		}
		g_pIISO[j] = NULL;
		g_pICB[j] = NULL;
		//g_dInstType[j] = NULL;
		g_eISOCategory[j] = UNKNOWN_ISO;
		g_pColorButton[j] = NULL;
		g_iInstCount--;

		ReorgDialogLayout();
	}
	return 0;
}
//////////////////////////////////////////////////////////////////
//
//                         OnDrawItem
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//TRACE("CMicMgrDlg::OnDrawItem()\n");

	int index = nIDCtl - IDC_INSTRUMENT_BUTTON;

	if (index >= 0 && index < g_iInstCount)
	{
		if (g_pColorButton[index] != NULL)
			g_pColorButton[index]->DrawItem(lpDrawItemStruct);
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
//////////////////////////////////////////////////////////////////
//
//                     OnDriveAllToZero
//
//////////////////////////////////////////////////////////////////
LRESULT CMicMgrDlg::OnDriveAllToZero(WPARAM wParam, LPARAM lParam)
{
	VARIANT_BOOL vbDriveToZero = wParam?VARIANT_TRUE:VARIANT_FALSE;
	VARIANT_BOOL vbInPowerFail = lParam?VARIANT_TRUE:VARIANT_FALSE;

	for (int i = 0; i < g_iInstCount; i++)
	{
		if (g_pIISO[i])
			g_pIISO[i]->DriveBBMToZero(vbDriveToZero,vbInPowerFail);
	}
	return 0;
}
/////////////////////////////////////////////////////////////////
//
//                      PrintButtonStatus
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::PrintButtonStatus(
	FILE* outfile, 
	CString cNameText, 
	CString cSecondText, 
	char *szType, 
	COLORREF cColor, 
	int iState)
{
	char szState[256];
	szState[0] = NULL;
	if (cNameText.GetLength()==0)
	{
		    strcat(szState," color ");
			strcat(szState," bell   ");
			strcat(szState," time   ");
			strcat(szState," flag   ");
			strcat(szState," bbm/pwr");
			strcat(szState," disk   ");
			strcat(szState," flash  ");
			strcat(szState," square ");
			fprintf(outfile,"%-41s: %s\n","TYPE and NAME of instrument",szState);
	}
	else
	{
		switch (cColor)
		{
		case COLOR_RED: 
			strcat(szState," RED*  ");
			break;
		case COLOR_GRAY:
			strcat(szState," GRAY  ");
			break;
		case COLOR_YELLOW:
			strcat(szState," YELLOW");
			break;
		case COLOR_GREEN:
			strcat(szState," GREEN ");
			break;
		case COLOR_WHITE:
			strcat(szState," WHITE ");
			break;
		case COLOR_CYAN:
			strcat(szState," CYAN  ");
			break;
		default:
			strcat(szState," OTHER ");
		}

		if (iState & BELL_BIT) 
			strcat(szState," BELL   ");
		else
			strcat(szState,"        ");
		if (iState & TIME_BIT)
			strcat(szState," TIME   ");
		else
			strcat(szState,"        ");
		if (iState & FLAG_BIT)
		{
			if (iState & DEAD_BIT)
				strcat(szState," DEAD   ");
			else
				strcat(szState," FLAG   ");
		}
		else
			strcat(szState,"        ");
		if (iState & OTHR_BIT)
			strcat(szState," BBM/PWR");
		else
			strcat(szState,"        ");
		if (iState & DISK_BIT)
			strcat(szState," DISK   ");
		else
			strcat(szState,"        ");

		if (iState & FLASH_BIT)
			strcat(szState," FLASH  ");
		else
			strcat(szState,"        ");

		if (iState & SQUARE_BIT)
			strcat(szState," SQUARE ");
		else
			strcat(szState,"        ");

		fprintf(outfile,"%-8s %32s: %s %s\n",szType, cNameText.GetString(),szState,cSecondText.GetString()); // gjk
	}
}
/////////////////////////////////////////////////////////////////
//
//                      PrintINISection
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::PrintINISection(char *szSection, FILE*outfile)
{
	fprintf(outfile,"[%s]\n",szSection);
	char buff[8192];
	if (GetPrivateProfileSection(szSection,buff,sizeof(buff), g_szIniFile) > 0)
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
/////////////////////////////////////////////////////////////////
//
//                      ReorgDialogLayout
//
//////////////////////////////////////////////////////////////////
//reorganize the instrument buttons in the dialog

// Numerous changes to calculate layout parameters related to DR00169.
// DR00169 - Buttons can be crowded and unusable....

void CMicMgrDlg::ReorgDialogLayout() 
{
	//TRACE("CMicMgrDlg::ReorgDialogLayout()\n");

	//for each existing button move it where it now belongs
	WINDOWPLACEMENT lpwndpl;	
	GetWindowPlacement( &lpwndpl );
	CRect Rect;
	int leftoffset;

	int iFrameX = ::GetSystemMetrics(SM_CXFRAME);	//dgp 18 Oct 2006
	int iFrameY = ::GetSystemMetrics(SM_CYFRAME);	//dgp 18 Oct 2006

	//hide the dialog box if required
	if (g_bHideWindows) 
	{
		GetDlgItem(IDC_STARTDATA_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COPYDATA_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALL_STATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALL_STATIC)->GetWindowRect(&Rect);
		leftoffset = Rect.left - lpwndpl.rcNormalPosition.left;
		m_iTop = iFrameY;//Rect.top;	//dgp 18 Oct 2006
	} 
	else 
	{
		GetDlgItem(IDC_STARTDATA_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_COPYDATA_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALL_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALL_STATIC)->GetWindowRect(&Rect);
		leftoffset = Rect.left - lpwndpl.rcNormalPosition.left;
		m_iTop = Rect.bottom - Rect.top + iFrameY;	//dgp 18 Oct 2006
	}

	//get the width of each button
	g_iButtonWidth = GetPrivateProfileInt(
		CONFIGURATION,BUTTONWIDTH,BUTTONWIDTH_DEFAULT,g_szIniFile);

	//get the height of each button
	g_iButtonHeight= GetPrivateProfileInt(
		CONFIGURATION,BUTTONHEIGHT,BUTTONHEIGHT_DEFAULT,g_szIniFile);

	//get the flag size of each button
	g_iButtonFlagSize = GetPrivateProfileInt(
		CONFIGURATION,BUTTONFLAGSIZE,BUTTONFLAGSIZE_DEFAULT,g_szIniFile);

	if (g_iButtonWidth == 0) g_iButtonWidth = Rect.right - Rect.left + iFrameX*2;
	if (g_iButtonHeight== 0) g_iButtonHeight= Rect.bottom- Rect.top;

	//figure where each button must go
	if (g_iInstCount>0)
	{
		CRect current_rect = m_sInitialRect;	
	  
		int row = 0;
		int column = 0;
		int maxcolumn = 0;
		int maxrow = 0;
		//step through list of instruments
		for (int instrument = 0; instrument < g_iInstCount; instrument ++, column++ ) 
		{
			//if at the first in a row make room for a row of buttons  
			if (g_iButtonColumns <= 0) g_iButtonColumns = 1;

			if ((instrument % g_iButtonColumns)==0) 
			{
				if (instrument != 0) row++;	//bump the row if we aren't on row 0
					column = 0;				//always start in column 0 
			}

			g_pColorButton[instrument]->MoveWindow(
					CRect(column*(g_iButtonWidth+1)+iFrameX*2,
					m_iTop+row*(g_iButtonHeight+1),
					g_iButtonWidth+column*(g_iButtonWidth+1)+iFrameX,
					m_iTop+g_iButtonHeight+row*(g_iButtonHeight+1)),false);

			g_pColorButton[instrument]->SetFlagSize((UINT)g_iButtonFlagSize);
			g_pColorButton[instrument]->ShowWindow(SW_SHOW);
			g_pColorButton[instrument]->Redraw(false);

			//increase width and height of dialog box if we need to
			maxcolumn = max(maxcolumn, column);
			maxrow    = max(maxrow,    row);
			if (g_bHideWindows) 
			{
				current_rect.right = (g_iButtonWidth+1) * (maxcolumn+1) + iFrameX*4;
			} 
			else 
			{ 
				if ((column+1)*(g_iButtonWidth+1)+10 > current_rect.Width()) 
					current_rect.right = ((g_iButtonWidth+1) * (maxcolumn+1) + iFrameX*4);
			}
			current_rect.bottom = (g_iButtonHeight+1)* (maxrow + 1) + m_iTop + iFrameX*4;

			//do all this so that the upper left corner of windows stays put
			lpwndpl.rcNormalPosition.bottom = lpwndpl.rcNormalPosition.top + current_rect.bottom + iFrameY*5;

			if (g_bHideWindows)
				lpwndpl.rcNormalPosition.right = lpwndpl.rcNormalPosition.left+ current_rect.right;
			else
			{
				lpwndpl.rcNormalPosition.right = lpwndpl.rcNormalPosition.left +
							(g_iButtonWidth + 1) * (maxcolumn + 1) + iFrameX*4;
			}

			//hide the dialog box if required
			if (!g_bHideWindows) 
			{
				CRect cRect;
				GetDlgItem(IDC_STARTDATA_BUTTON)->GetWindowRect(&cRect);
				ScreenToClient(&cRect);
				cRect.right = lpwndpl.rcNormalPosition.right -lpwndpl.rcNormalPosition.left - leftoffset*2;
				GetDlgItem(IDC_STARTDATA_BUTTON)->MoveWindow(cRect);

				GetDlgItem(IDC_COPYDATA_BUTTON)->GetWindowRect(&cRect);
				ScreenToClient(&cRect);
				cRect.right = lpwndpl.rcNormalPosition.right -lpwndpl.rcNormalPosition.left - leftoffset*2;
				GetDlgItem(IDC_COPYDATA_BUTTON)->MoveWindow(cRect);

				GetDlgItem(IDC_ALL_STATIC)->GetWindowRect(&cRect);
				ScreenToClient(&cRect);
				cRect.right = lpwndpl.rcNormalPosition.right -lpwndpl.rcNormalPosition.left - leftoffset;
				GetDlgItem(IDC_ALL_STATIC)->MoveWindow(cRect);
			}

			lpwndpl.showCmd = (g_bHideWindows)?SW_HIDE:SW_SHOW;
			SetWindowPlacement(&lpwndpl);

		}  //for each instrument

		Invalidate();

	} //if g_iInstCount > 0
	else 
	{
		// don't have any instruments to play with yet
		lpwndpl.rcNormalPosition.bottom = lpwndpl.rcNormalPosition.top+m_sInitialRect.bottom;
		lpwndpl.rcNormalPosition.right  = lpwndpl.rcNormalPosition.left+m_sInitialRect.right;
		lpwndpl.showCmd= (g_bHideWindows)?SW_HIDE:SW_SHOW;
		SetWindowPlacement(&lpwndpl);
	}
}
//////////////////////////////////////////////////////////////////
//
//                     ReorgBySuperuser
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::ReorgBySuperuser()
{
//	TRACE("CMicMgrDlg::ReorgBySuperuser\n");

	if (g_iInstCount)
	{
		bool bSuperUser = true;
		CValidate Valid(this,bSuperUser);

		//hide windows if 'minimal'
		if (g_bHideWindows)
			ShowWindow(SW_HIDE);

		//if ok then shutdown
		if (Valid.DoModal() == IDOK) 
		{
			char* szUserName = Valid.GetUserName();											//DR00115
			WritePrivateProfileString(CONFIGURATION,LAST_VALIDATION,szUserName,g_szIniFile);//DR00115
	
			CReOrganize dlgReOrg(this);
			if (dlgReOrg.DoModal() == IDOK)
			{
				ShowWindow(SW_SHOW);
				m_bRestart = true;
				ShutDown();
			}
		}
		ShowWindow(SW_SHOW);
	}
}
//////////////////////////////////////////////////////////////////
//
//                         SetUpRegisteredMessageValues
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::SetUpRegisteredMessageValues()
{
	//
	//Set up all of the registered message strings
	//
	//These static local values are received and processed here by MicMgr.
	//They are provided to the components via interface methods called
	//EstablishButtonKeeperCommunicationsPath(),
	//EstablishValidationCommunicationsPath(),
	// and EstablishSpecialUPSCommunicationsPath().

	uWM_ISO_PAUSE_STATUS_CHANGE  = 
		RegisterWindowMessage("WM_ISO_PAUSE_STATUS_CHANGE_MESSAGE_STRING");
	uWM_QUERY_ALL_PAUSED = 
		RegisterWindowMessage("WM_QUERY_ALL_PAUSED_MESSAGE_STRING");
	uWM_DRIVE_TO_ZERO_COMMAND = 
		RegisterWindowMessage("WM_DRIVE_TO_ZERO_MESSAGE_STRING");
	uWM_VALIDATE
		= RegisterWindowMessage("WM_VALIDATE_REGISTERED_MESSAGE_STRING");

	//These universal values for communication and instrument component messages.
	//These are used by the CSO's and ISO's to talk to each other.
	//They are provided to the components via interface methods called
	//put_WindowsRegisteredMessageStrings()
	m_uIDC_RECEIVE     = RegisterWindowMessage("IDC_RECEIVE");
	m_uIDC_RECEIVE_OOB = RegisterWindowMessage("IDC_RECEIVE_OOB");
	m_uIDC_COM_START   = RegisterWindowMessage("IDC_COM_START");
	m_uIDC_COM_STOP    = RegisterWindowMessage("IDC_COM_STOP");
	m_uIDC_COM_RESET   = RegisterWindowMessage("IDC_COM_RESET");
}
//////////////////////////////////////////////////////////////////
//
//                         ShutDown
//
//////////////////////////////////////////////////////////////////
//shut down all of MIC
void CMicMgrDlg::ShutDown()
{
//	TRACE("CMicMgrDlg::ShutDown\n");

	//pause data
	StartData(false);

	SetTimer(TIMER_PAUSE_ALL3,1000,NULL);												//dgp - fix coding errors
/*																						//dgp - fix coding errors
	//validate user via password														//dgp - fix coding errors
	CValidate Valid(this);																//dgp - fix coding errors
																						//dgp - fix coding errors
	//hide windows if 'minimal'															//dgp - fix coding errors
    if (g_bHideWindows)																	//dgp - fix coding errors
	    ShowWindow(SW_HIDE);															//dgp - fix coding errors
		//dgp - fix coding errors
	//if ok then shutdown																//dgp - fix coding errors
	if (Valid.DoModal() == IDOK) 														//dgp - fix coding errors
	{																					//dgp - fix coding errors
		char* szUserName = Valid.GetUserName(); 										//DR00115	//dgp - fix coding errors
		WritePrivateProfileString(CONFIGURATION,LAST_VALIDATION,szUserName,g_szIniFile);//DR00115	//dgp - fix coding errors
		ShutDownWork();																	//dgp - fix coding errors
	}																					//dgp - fix coding errors

	//show window 'case we hid it above and didn't quit									//dgp - fix coding errors
	ShowWindow(SW_SHOW);																//dgp - fix coding errors
*/
}

void CMicMgrDlg::ShutDownFinal()														//dgp - fix coding errors
{																						//dgp - fix coding errors
	//validate user via password														//dgp - fix coding errors
	CValidate Valid(this);																//dgp - fix coding errors

	//hide windows if 'minimal'															//dgp - fix coding errors
    if (g_bHideWindows)																	//dgp - fix coding errors
	    ShowWindow(SW_HIDE);															//dgp - fix coding errors

	//if ok then shutdown																//dgp - fix coding errors
	if (Valid.DoModal() == IDOK) 														//dgp - fix coding errors
	{																					//dgp - fix coding errors
		char* szUserName = Valid.GetUserName(); 										//DR00115	//dgp - fix coding errors
		WritePrivateProfileString(CONFIGURATION,LAST_VALIDATION,szUserName,g_szIniFile);//DR00115	//dgp - fix coding errors
		ShutDownWork();																	//dgp - fix coding errors
	}																					//dgp - fix coding errors

	//show window 'case we hid it above and didn't quit									//dgp - fix coding errors
	ShowWindow(SW_SHOW);																//dgp - fix coding errors
}																						//dgp - fix coding errors

//////////////////////////////////////////////////////////////////
//
//                         ShutDownWork
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::ShutDownWork()
{
//	TRACE("CMicMgrDlg::ShutDownWork()\n");

	m_bValidShutdown = true;
	HRESULT hr;

	//flag normal shutdown
	WritePrivateProfileString(CONFIGURATION,NORMALSHUTDOWN,"1",g_szIniFile);

	//destroy each ColorButton and ISO
	for (int j = 0; j < g_iInstCount; j++)
	{
		if (g_pICB[j])
		{
			hr = g_pICB[j]->Release();
			g_pICB[j] = NULL;
		}
		if (g_pColorButton[j])
		{
			delete g_pColorButton[j];
			g_pColorButton[j] = NULL;
		}
		if (g_pIISO[j])
		{
			g_pIISO[j]->ShutdownISO();
			g_pIISO[j]->Release();
			g_pIISO[j] = NULL;
		}
	}
	g_iInstCount = 0;

	//halt each comm object and remove it

	for (int i= 0; i < g_iCommCount; i++)
	{
		if (g_pICSO[i])
		{
			hr = g_pICSO[i]->ShutdownCSO();
			hr = g_pICSO[i]->Release();
			g_pICSO[i] = NULL;
		}
	}
	g_iCommCount = 0;

	//shutdown the file manager thread
	if (m_pFileMgr)
	{
		HANDLE hand = m_pFileMgr->m_hThread;
		m_pFileMgr->PostThreadMessage(m_uIDC_COM_STOP,0,0);
		::WaitForSingleObject(hand,1000);
		TerminateThread(hand,0);
	}

	if (m_bRestart)
	{
		char szTemp[MAX_PATH];
		GetModuleFileName(AfxGetInstanceHandle(),szTemp,sizeof(szTemp));
		_spawnl(_P_NOWAIT,szTemp,"MICMGR.EXE","WAIT",NULL);
	}

	PostQuitMessage(0);
}

//////////////////////////////////////////////////////////////////
//
//                            StartData
//
//////////////////////////////////////////////////////////////////
void CMicMgrDlg::StartData(bool start)
{
//	TRACE("CMicMgrDlg::StartData(%s)\n", start?"true":"false");

	if (start)							//SCR00361
	{									//SCR00361
		KillTimer(TIMER_PAUSE_ALL1);	//SCR00361
		KillTimer(TIMER_PAUSE_ALL2);	//SCR00361
	}									//SCR00361

	//For MIC_GM, adjust the button and the menu in
	//OnISOPauseStatusChange().

	// command to instruments to start or stop
	VARIANT_BOOL vb = !start ? VARIANT_TRUE:VARIANT_FALSE;

	m_iSlowStartIntervalInSeconds = 
		GetPrivateProfileInt(CONFIGURATION,SLOWSTART,0,g_szIniFile);

	m_iSlowStartIndex = 0;

	if ((m_iSlowStartIntervalInSeconds == 0) || !start)
	{
		KillTimer(TIMER_SLOW_START);

		for (int i = 0; i < g_iInstCount; i++) 
			g_pIISO[i]->PauseInstrument(vb);
	}
	else
	{
		m_iSlowStartIndex = 0;
		SetTimer(TIMER_SLOW_START,m_iSlowStartIntervalInSeconds*1000,NULL);
	}

	SetNetworkHeartbeatTimer(start);
}

BOOL CMicMgrDlg::PreTranslateMessage(MSG* pMsg) 
{
	//PreTranslateMessage is required here because MFC is BROKEN in this
	//area.  Found that the debug version of this class works fine.  However,
	//the release version crashes out within the MIO dll during the creation of the
	//CMcb base class object.  The MFC reference PRB is:
	//PRB: MFC Message Handler Not Called with PostThreadMessage() 
	//Q142415

	//uWM_ISO_PAUSE_STATUS_CHANGE, OnISOPauseStatusChange.

	//This same problem applied to the non-threaded version, too.
	//PJM 6/10/2005

	if (pMsg->message == uWM_ISO_PAUSE_STATUS_CHANGE)
	{
		//OutputDebugString("CMicMgrDlg::PreTranslateMessage(uWM_ISO_PAUSE_STATUS_CHANGE)\n");
		OnISOPauseStatusChange(0, 0);
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CMicMgrDlg::NetworkHeartbeat()
{
	if (m_uiDoingNetworkToThatBrandNewBeat <= 0)  // no heartbeat
		return;

	COleDateTime now = COleDateTime::GetCurrentTime();
	//step through list of instruments
	for (int instrument = 0; instrument < g_iInstCount; instrument++) 
	{
		if (!g_pColorButton[instrument])  // no button
			continue;
		if (!g_pColorButton[instrument]->IsDoingNetwork()) // not doing network broadcast
			continue;
		
		// get last update time
		// compute span
		// if span > BEAT value then prod it
		COleDateTimeSpan diff = now - g_pColorButton[instrument]->m_LastSendDateTime;
		if (diff > m_BeatSeconds) // number of seconds
		{
			OutputDebugString("Beating that drum! ! ! ! !");
			g_pColorButton[instrument]->SendStateToNetworkCoConspirators();
		}
	}

}

void CMicMgrDlg::SetNetworkHeartbeatTimer(bool bEnable)
{
	if (bEnable)
	{
		if (g_bDoingNetwork && (m_uiDoingNetworkToThatBrandNewBeat > 0))
		{
			UINT interval = 1000 * UINT(m_BeatSeconds.GetTotalSeconds());
			SetTimer(TIMER_STALE_STATUS_HEARTBEAT, interval, NULL);
		}
	}
	else
	{
		KillTimer(TIMER_STALE_STATUS_HEARTBEAT);
	}
}



CString** CMicMgrDlg::EnumerateSerialPorts()
{
	CString** res = new CString*[MAX_COMM];
	::ZeroMemory(res, sizeof(CString*) * MAX_COMM);  // null if not found

	for (int i = 0; i < MAX_COMM; i++)
	{
		TCHAR buffnoid[128];
		sprintf_s(buffnoid,128, "\\\\.\\COM%d", i);
		HANDLE        hCom = NULL;
		bool yup = false, active = false;
		try
		{
			if ((hCom = CreateFile ( buffnoid,
									GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) 
					!= INVALID_HANDLE_VALUE)
			{
				yup = true;
				if (hCom)
				{
					CloseHandle(hCom);
					hCom = NULL;
				}
			}
			else
			{
				DWORD err = ::GetLastError();
				if (ERROR_ACCESS_DENIED == err)
					active = yup = true;  // COM port is there, but in use
			}
			if (yup)
			{
				CString* p = new CString();
				res[i] = p;
				TCHAR noidbuff[20];
				sprintf_s(noidbuff, 20, (active ? "COM%d in use" : "COM%d"), i);
				res[i]->SetString(noidbuff);
			}
		}
		catch(...)
		{
		}
	}
	return res;

	// could also read the registry at HKEY_LOCAL_MACHINE HARDWARE\\DEVICEMAP\\SERIALCOMM" where the device name and API name are presented e.g. \device\serial0  COM1

}