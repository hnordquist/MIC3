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
// NewSerialComm.cpp

#include "stdafx.h"
#include "INI_definitions.h"
#include "NewEthernetComm.h"
#include "cso_definitions.h"
#include "resource.h"
#include "TimedMessageDialog.h"
#include "colors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                 Constructors and Destructors
//
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CNewEthernetComm, CDialog)

CNewEthernetComm::CNewEthernetComm():CDialog(CNewEthernetComm::IDD)
{
//	TRACE("CNewEthernetComm::CTOR #1()\n");
}

CNewEthernetComm::CNewEthernetComm(char* inifile,int iDlgClose,char *name)
	: CDialog(CNewEthernetComm::IDD)
{
//	TRACE(_T("CNewEthernetComm::CTOR #2()\n"));

	strncpy(m_szIniFile, inifile, sizeof(m_szIniFile)-1);
	strncpy(m_pName, name, sizeof(m_pName));
	m_dDlgCloseMilliseconds = iDlgClose;

	m_dParity   = 3;
	m_dDataBits = 8;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
	m_szPort[0] = NULL;
	m_pCreateButton = NULL;

	//m_dCurSel_Serial = -1;

	m_csTitle = CString("Direct Serial CSO") + CString(" - ");
	m_csTitle += CString(": Create Comm");

	//{{AFX_DATA_INIT(CNewEthernetComm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
CNewEthernetComm::~CNewEthernetComm()
{
//	TRACE("CNewEthernetComm::DTOR()\n");
	delete m_pCreateButton;
}

BEGIN_MESSAGE_MAP(CNewEthernetComm, CDialog)
	//{{AFX_MSG_MAP(CNewEthernetComm)
	ON_BN_CLICKED(IDC_96E71_BUTTON, On96e71Button)
	ON_BN_CLICKED(IDC_96N81_BUTTON, On96n81Button)
	ON_BN_CLICKED(IDC_BAUD1200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_DATABITS16_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_PARITYEVEN_RADIO, OnParityRadio)
	ON_LBN_SELCHANGE(IDC_PORT_LIST, OnSelchangePortList)
	ON_BN_CLICKED(IDC_STOPBITS1_RADIO, OnStopbitsRadio)
	ON_BN_CLICKED(IDC_BAUD115200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD12800_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD14400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD1800_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD19200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD2400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD38400_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD4800_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD56000_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD57600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD7200_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_BAUD9600_RADIO, OnBaudRadio)
	ON_BN_CLICKED(IDC_DATABITS5_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS6_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS7_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_DATABITS8_RADIO, OnDatabitsRadio)
	ON_BN_CLICKED(IDC_PARITYMARK_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_PARITYNONE_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_PARITYODD_RADIO, OnParityRadio)
	ON_BN_CLICKED(IDC_STOPBITS15_RADIO, OnStopbitsRadio)
	ON_BN_CLICKED(IDC_STOPBITS2_RADIO, OnStopbitsRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CNewEthernetComm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewEthernetComm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CNewEthernetComm::On96e71Button() 
{
	// force even parity
	((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->SetCheck(0);
	
	//force 7 bits
	((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->SetCheck(0);

	//force 1 stop bit
	((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->SetCheck(0);

	//force 9600 baud
	((CButton*)GetDlgItem(IDC_BAUD600_RADIO))-> SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD12800_RADIO))->SetCheck(0);

	//set our member vars appropriately 
	m_dParity = 1;
	m_dDataBits = 7;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
}

void CNewEthernetComm::On96n81Button() 
{
	//force no parity
	((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->SetCheck(0);

	//force 8 data bits
	((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->SetCheck(0);

	//force 1 stop bits
	((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->SetCheck(0);

	//force 9600 baud
	((CButton*)GetDlgItem(IDC_BAUD600_RADIO))-> SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_BAUD12800_RADIO))->SetCheck(0);

	//set our member vars appropriately
	m_dParity = 3;
	m_dDataBits = 8;
	m_dStopBits = 1;
	m_dBaudRate = 9600;
}

void CNewEthernetComm::OnOK() 
{
//	TRACE("CNewEthernetComm::OnOK()\n");

	char buff2[32];
	CString csText;

	if (m_pName[0] == ';')
	{
		csText = CString("A \"Name\" for a communications object cannot start with \";\".");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//comm objects cannot have the same name
	GetPrivateProfileString(
		COMMUNICATIONS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("A Communications object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//because we are using INI sections to save the information
	//we cannot have the same name as an instrument section either
	GetPrivateProfileString(
		INSTRUMENTS,
		m_pName,"ERROR",buff2,sizeof(buff2)-1,m_szIniFile);

	if (strcmp(buff2,"ERROR")!=0)
	{
		csText = CString("An Instrument object by this\nname already exists!\n"\
				"Select a different name.");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	//get the port from the port list
	int port;
	if ((port=((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetCurSel())== LB_ERR)
	{
		csText = CString("A Communications PORT must be selected!  ");
		new CTimedMessageDialog(m_csTitle, csText,m_dDlgCloseMilliseconds/1000,this);
		return;
	}

	ShowWindow(SW_SHOW);  //<<QA>> is this needed?

	//save all to ini file
	//entry in the communications for this com object
	//WritePrivateProfileString(COMMUNICATIONS,m_pName,DIRECT_SERIAL_DISCOVERY_TYPE,m_szIniFile);
	//entrys in the "name" section for this com object
	//long break allowed
	WritePrivateProfileString(m_pName,LONGBREAK,"YES",m_szIniFile);
	//parity
	switch (m_dParity)
	{
		case 0:
			WritePrivateProfileString(m_pName,PARITY,"ODD",m_szIniFile);
			break;
		case 1:
			WritePrivateProfileString(m_pName,PARITY,"EVEN",m_szIniFile);
			break;
		case 2:
			WritePrivateProfileString(m_pName,PARITY,"MARK",m_szIniFile);
			break;
		default:
			WritePrivateProfileString(m_pName,PARITY,"NONE",m_szIniFile);
	}

	WritePrivateProfileString(m_pName,STOPBITS,_itoa(m_dStopBits,buff2,10),m_szIniFile);
	WritePrivateProfileString(m_pName,DATABITS,_itoa(m_dDataBits,buff2,10),m_szIniFile);
	WritePrivateProfileString(m_pName,BAUDRATE,_itoa(m_dBaudRate,buff2,10),m_szIniFile);
	((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetText(port,buff2);
	WritePrivateProfileString(m_pName,PORT,buff2,m_szIniFile);

	CDialog::OnOK();
}

void CNewEthernetComm::OnBaudRadio() 
{
	// get the currently selected baud rate
	if		(((CButton*)GetDlgItem(IDC_BAUD600_RADIO))->GetCheck()==1)
		m_dBaudRate = 600;
	else if (((CButton*)GetDlgItem(IDC_BAUD1200_RADIO))->GetCheck()==1)
		m_dBaudRate = 1200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD1800_RADIO))->GetCheck()==1)
		m_dBaudRate = 1800;
	else if	(((CButton*)GetDlgItem(IDC_BAUD2400_RADIO))->GetCheck()==1)
		m_dBaudRate = 2400;
	else if (((CButton*)GetDlgItem(IDC_BAUD4800_RADIO))->GetCheck()==1)
		m_dBaudRate = 4800;
	else if	(((CButton*)GetDlgItem(IDC_BAUD7200_RADIO))->GetCheck()==1)
		m_dBaudRate = 7200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD9600_RADIO))->GetCheck()==1)
		m_dBaudRate = 9600;
	else if	(((CButton*)GetDlgItem(IDC_BAUD14400_RADIO))->GetCheck()==1)
		m_dBaudRate = 14400;
	else if (((CButton*)GetDlgItem(IDC_BAUD19200_RADIO))->GetCheck()==1)
		m_dBaudRate = 19200;
	else if	(((CButton*)GetDlgItem(IDC_BAUD38400_RADIO))->GetCheck()==1)
		m_dBaudRate = 38400;
	else if (((CButton*)GetDlgItem(IDC_BAUD56000_RADIO))->GetCheck()==1)
		m_dBaudRate = 56000;
	else if	(((CButton*)GetDlgItem(IDC_BAUD57600_RADIO))->GetCheck()==1)
		m_dBaudRate = 57600;
	else if	(((CButton*)GetDlgItem(IDC_BAUD115200_RADIO))->GetCheck()==1)
		m_dBaudRate = 115200;
	else if (((CButton*)GetDlgItem(IDC_BAUD12800_RADIO))->GetCheck()==1)
		m_dBaudRate = 128000;
	else 
		m_dBaudRate = 9600;
}

BOOL CNewEthernetComm::OnInitDialog() 
{
//	TRACE(_T("CNewEthernetComm::OnInitDialog()\n"));

	CDialog::OnInitDialog();
	
	On96n81Button();
	//PopulateSerialPortList();

	m_bHavePort = false;

	CEdit *pedit = ((CEdit*)GetDlgItem(IDC_NAME_EDIT));;
	pedit->SetWindowText(m_pName);
	pedit->EnableWindow(false);

	m_pCreateButton = new CYellowButton("Create",IDOK,m_szIniFile,this);

	return TRUE;
}

/*void CNewEthernetComm::PopulateSerialPortList()
{
//	TRACE("CNewDirectSerialComm::PopulateSerialPortList()\n");

	CListBox *plistbox = (CListBox*)GetDlgItem(IDC_PORT_LIST);

	HKEY HKey;
	DWORD KeyIndex = 0;
	DWORD Type;

	//can have 256 comm items (very unlikely) in the form COMxxx
	char ValueName[2048];
	char Value[2048];
	DWORD ValueNameSize = 2047;
	DWORD ValueSize = 2047;
	//find out what com NT thinks it has 
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DEVICEMAP\\SERIALCOMM",
		0,KEY_READ,&HKey)==ERROR_SUCCESS)
	{
		while (RegEnumValue(HKey, KeyIndex,	
			ValueName, &ValueNameSize, NULL,
			&Type, (unsigned char*)&Value, &ValueSize)
			==ERROR_SUCCESS)
		{
			//and add these com objects iff there isn't a comm
			//object currently using it
			ValueSize = 2047;
			ValueNameSize = 2047;
			KeyIndex++;
			//if not currently used 
			bool used = false;
			//for each comm object check to see if it uses this port
			char buff[256];
			if (GetPrivateProfileSection(COMMUNICATIONS,buff,sizeof(buff), m_szIniFile)>0)
			{
				char *at = buff;				//char pointer to step through buff

		 		//step through list of communications using a pointer into buff
				while (strlen(at) > 0) 
				{
					//getting instrument names from INI file so strip off extra stuff
					CString At(at);
					At.TrimRight();
					char temp[32];
					char buff2[32];
					strcpy(temp,At.SpanExcluding((LPCTSTR)"="));
					if (strlen(temp)>0) 
					{
						GetPrivateProfileString(temp,"PORT","",buff2,sizeof(buff2),m_szIniFile);
						if (strcmp(buff2,Value)==0)
							used = true;
					}
   				at = at + strlen(at)+1;
				}
			}
			if (!used)
				plistbox->AddString(Value);
		}
		RegCloseKey(HKey);
	}

	// Highlight the previous selection if possible

	if (m_dCurSel_Serial != -1)
	{
		int success = plistbox->SetCurSel(m_dCurSel_Serial);

		if (success != LB_ERR)
		{
			m_bHavePort = true;
		}
		else
		{
			m_bHavePort = false;
			m_dCurSel_Serial = -1;  //was out of range
		}
	}
}*/

void CNewEthernetComm::OnDatabitsRadio() 
{
	//user clicked a data bits radio, so find out which is the 
	//current selection
	if		(((CButton*)GetDlgItem(IDC_DATABITS5_RADIO))->GetCheck()==1)
		m_dDataBits = 5;
	else if (((CButton*)GetDlgItem(IDC_DATABITS6_RADIO))->GetCheck()==1)
		m_dDataBits = 6;
	else if	(((CButton*)GetDlgItem(IDC_DATABITS7_RADIO))->GetCheck()==1)
		m_dDataBits = 7;
	else if	(((CButton*)GetDlgItem(IDC_DATABITS8_RADIO))->GetCheck()==1)
		m_dDataBits = 8;
	else	//(((CButton*)GetDlgItem(IDC_DATABITS16_RADIO))->GetCheck();
		m_dDataBits = 16;
}

void CNewEthernetComm::OnParityRadio() 
{
	// user clicked on one of the parity radio buttons 
	// so find out which is the current selection
	if		(((CButton*)GetDlgItem(IDC_PARITYODD_RADIO))->GetCheck()==1)
		m_dParity = 0;
	else if (((CButton*)GetDlgItem(IDC_PARITYEVEN_RADIO))->GetCheck()==1)
		m_dParity = 1;
	else if	(((CButton*)GetDlgItem(IDC_PARITYMARK_RADIO))->GetCheck()==1)
		m_dParity = 2;
	else //if (((CButton*)GetDlgItem(IDC_PARITYNONE_RADIO))->GetCheck()==1)
		m_dParity = 3;
}

void CNewEthernetComm::OnSelchangePortList() 
{
	// user clicked on one of the port selections in the list box
	// so find out which is the current selection
	if (((CListBox*)GetDlgItem(IDC_PORT_LIST))->GetCurSel()==LB_ERR)
	{
		m_bHavePort = false;
		m_pCreateButton->EnableWindow(FALSE);
	}
	else
	{
		m_bHavePort = true;
		m_pCreateButton->EnableWindow(TRUE);
	}
}

void CNewEthernetComm::OnStopbitsRadio() 
{
	// user clicked on one of the stop bit radio buttons
	// so find out which is the current selection
	if (((CButton*)GetDlgItem(IDC_STOPBITS1_RADIO))->GetCheck()==1)
		m_dStopBits = 1;
	else if (((CButton*)GetDlgItem(IDC_STOPBITS15_RADIO))->GetCheck()==1)
		m_dStopBits = 15;
	else //	(((CButton*)GetDlgItem(IDC_STOPBITS2_RADIO))->GetCheck()==1)
		m_dStopBits = 2;
}

