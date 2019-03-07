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
// TestDlg.cpp

#include "stdafx.h"
#include "resource.h"
#include "EditSend.h"
#include "TestDlg.h"
#include "TestInst.h"
#include "CSO_Definitions.h"
#include "WatchAboutDlg.h"
#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	INVALIDATETIMER 2500

//registered message values
static UINT uIDC_RECEIVE;
static UINT uIDC_RECEIVE_OOB;

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

//IMPLEMENT_DYNAMIC(CTestDlg, CDialog)

CTestDlg::CTestDlg(CWnd* pParent, ICSO* pComm, int Node)
	: CDialog(CTestDlg::IDD, pParent)
{
//	TRACE("CTestDlg::CTOR\n");

	m_pComm = pComm;
	m_dNode = Node;
	m_bNl = false;
	m_bIn33 = false;
	m_dLength = 0;
	m_cLastChar = NULL;
	m_bDoingTransfer = false;
	m_bInitialized = false;
	m_iLastCommand = 0;					//dgp 3 Nov 2006
	m_pParent = (CTestInst *)pParent;

	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTestDlg::~CTestDlg()
{
//	TRACE("CTestDlg::DTOR\n");
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_TEST_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_SPIN_DELAY, m_EditSpin);
	DDX_Control(pDX, IDC_EDIT_DELAY, m_EditDelay);
	DDX_Control(pDX, IDC_TEST_SEND, m_pSend);
	DDX_Control(pDX, IDC_TEST_RECEIVE, m_pReceive);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_BN_CLICKED(IDC_TEST_CLOSE_BUTTON, OnTestCloseButton)
	ON_BN_CLICKED(IDC_TEST_PRINT_BUTTON, OnTestPrintButton)
	ON_BN_CLICKED(IDC_TEST_CLEAR_BUTTON, OnTestClearButton)
	ON_WM_SYSCOMMAND()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_TEST_SENDFILE_BUTTON, OnTestSendfileButton)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_DELAY, OnChangeEditDelay)
	ON_BN_CLICKED(IDC_RADIO_CR, OnRadioMost)
	ON_BN_CLICKED(IDC_RADIO_LF, OnRadioMost)
	ON_BN_CLICKED(IDC_RADIO_CRLF, OnRadioMost)
	ON_BN_CLICKED(IDC_RADIO_LFCR, OnRadioMost)
	ON_BN_CLICKED(IDC_RADIO_NOTHING, OnRadioMost)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_MESSAGE(IDC_SPECIALKEY, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnSpecialKey)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceive)
	ON_REGISTERED_MESSAGE(uIDC_RECEIVE_OOB, (LRESULT(AFX_MSG_CALL CWnd::*)(WPARAM, LPARAM))OnReceiveOOB)
END_MESSAGE_MAP()
 
/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

void CTestDlg::OnReceiveOOB(WPARAM WParam, LPARAM LParam)
{
//	TRACE("CTestDlg::OnReceiveOOB()\n");

	//make tbuff point to LParam
	char* tbuff = (char*)LParam;
	char cbuff2[8192];

	try {
		if ((int)WParam != -1)
		{
			strncpy(cbuff2,tbuff,min(sizeof(cbuff2)-1,(int)WParam));
			cbuff2[min(sizeof(cbuff2-1),(int)WParam)] = NULL;
			m_dLength += min(sizeof(cbuff2)-1,(int)WParam);
		}
		else
		{
			strncpy(cbuff2,tbuff,min(sizeof(cbuff2)-1,strlen(tbuff)));
			cbuff2[min(sizeof(cbuff2)-1,strlen(tbuff))] = NULL;
			m_dLength += min(sizeof(cbuff2)-1,strlen(tbuff));
		}
	}
	catch (...)
	{
		strcpy(cbuff2,"ERROR HANDLING OOB STRING\r\n");
		m_dLength += strlen(cbuff2);
	}

	//create an "exchange" buffer to receive the data
	//increment the count of number of char in the cedit
	//m_dLength += WParam;

	//select the end of the cedit (insertion point)
	int start, end;
	m_pReceive.GetSel(start,end);
	m_pReceive.SetSel(-2,-1,true);
	//append the new data
	m_pReceive.ReplaceSel((LPCSTR)cbuff2,false);
	if (end - start) 
		m_pReceive.SetSel(start,end);
	if (m_dLength > MAX_CEDIT_HIGH)
	{
		m_pReceive.SetSel(0,MAX_CEDIT_DECR,true);
		m_pReceive.ReplaceSel("",false);
		if (start > MAX_CEDIT_DECR)
		{
			start -= MAX_CEDIT_DECR;
			end   -= MAX_CEDIT_DECR;
			m_pReceive.SetSel(start,end);
		}
		m_dLength -= MAX_CEDIT_DECR;
	}
	CoTaskMemFree((void *)LParam);
}

void CTestDlg::OnReceive(WPARAM WParam, LPARAM LParam)
{
	//TRACE("CTestDlg::OnReceive()\n");

	char hex[] = "0123456789ABCDEF";
	//make tbuff point to LParam
	unsigned char* tbuff = (unsigned char*)LParam;
	//create an "exchange" buffer to receive the data
	char  xbuff[8192];//can be up to 4 times the size 
	//j is index into xbuff
	int j = 0;

//#define _TEST
#ifdef _TEST
	//<<QA>>Insert the time
	CTime time = CTime::GetCurrentTime();
	CString cs = time.Format("%H:%M:%S");
	const char *pch = LPCTSTR(cs);
	int len = cs.GetLength();	
	strncpy (xbuff + j, pch, len);
	j += len;
	xbuff[j++] = '\t';
#endif

	try {
		//catch that pesky binary message
		if ((tbuff[0] == '3' && tbuff[1] == '3' && m_cLastChar == '\n') || m_bIn33)
		{
			//at the start of the binary message
			if (!m_bIn33)
			{
				m_dIn33Count = 0;
				m_dPostBEEFCount = 0;
			}
			m_bIn33 = true;
			//buffer to build ASCII representation into
			char xBuffA[8192];
			//index into xBuffA
			int indexA;
			indexA = 0;
			//count of characters -- used to insert cr/lf
			int iCharCount = 0;
			//step through all of the currently received text
			for (unsigned int i = 0; i < WParam; i++)
			{
				m_dIn33Count++;
				if (m_dPostBEEFCount)
					m_dPostBEEFCount++;
				xBuffA[indexA++] = hex[(tbuff[i] & 0xf0)>>4];
				xBuffA[indexA++] = hex[tbuff[i] & 0x0f];
				xBuffA[indexA++] = ' ';
				//is it time to break up a line (add cr/lf)
				iCharCount += 3;
				if (iCharCount > 75)
				{
					xBuffA[indexA++] = '\r';
					xBuffA[indexA++] = '\n';
					iCharCount = 0;
				}
				
				//need to watch for 0xBE 0xEF and the two checksum bytes
				if (m_dPostBEEFCount >= 3)
				{
					m_bIn33 = false;
					m_dPostBEEFCount = 0;
				}

				if (tbuff[i] == 0xef)
					if ((unsigned char)m_cLastChar == (unsigned char)0xbe)
						m_dPostBEEFCount++;
				m_cLastChar = tbuff[i];
			}
			//this looks real awkward, but we could have received a "non" binary message
			//and then a binary message -- with bob's current protocol this never happens
			xBuffA[indexA] = NULL;
			strcpy(xbuff,"33 =>");
			strcat(xbuff,xBuffA);
			strcat(xbuff,"\r\n");
			j = strlen(xbuff);
		}
		else
		{
			bool bBin = false;
			int  iBinCnt = 0;
			//convert to displayable in CEdit
			for (unsigned int i = 0; i < WParam; i++)
			{
				if ( ( (tbuff[i]&0x7f) == 0x0d) || ((tbuff[i]&0x7f) == 0x0a))
				{
					if (!m_bNl)
					{
	//					xbuff[j++] = 0x0d;
						xbuff[j++] = 0x0d;
						xbuff[j++] = 0x0a;
						m_bNl = true;
					}
				}
				else 
				{
					// changed by pjm for dp 3 Aug 2005
					//changed at Bob Parker's request 
					//if (isprint(tbuff[i]&0x7f))
					//          xbuff[j++] = (char)(tbuff[i] & 0x7f);
					if (!iscntrl(tbuff[i]) && (tbuff[i] < 0x7f))
					{
						if (j < 8190)
						{
							xbuff[j++] = (char)(tbuff[i]);
							iBinCnt++;
						}
					}
					else
					{
						bBin = true;
						if (j < 8187)
						{
							xbuff[j++] = '{';
							xbuff[j++] = hex[(tbuff[i] & 0xf0)>>4];
							xbuff[j++] = hex[tbuff[i] & 0x0f];
							xbuff[j++] = '}';
							iBinCnt++;
							if (iBinCnt >= 16)
							{
								iBinCnt = 0;
								xbuff[j++] = '\r';
								xbuff[j++] = '\n';
								bBin = false;
							}
						}
					}
					m_bNl = false;
				}
				m_cLastChar = tbuff[i];
			}
			if (bBin && (m_cLastChar != '\n'))
			{
				xbuff[j++] = '\r';
				xbuff[j++] = '\n';
			}
		}
	}
	catch(...)
	{
		strcpy(xbuff,"ERROR HANDLING MESSAGE\r\n");
		j = strlen(xbuff);
	}

	//terminate the character buffer 
	xbuff[j] = NULL;
	//increment the count of number of char in the cedit
	m_dLength += j;

	//select the end of the cedit (insertion point)
	int start, end;
	m_pReceive.GetSel(start,end);
	m_pReceive.SetSel(-2,-1,true);
	//append the new data
	m_pReceive.ReplaceSel(xbuff,false);
	if (end - start) 
		m_pReceive.SetSel(start,end);
	if (m_dLength > MAX_CEDIT_HIGH)
	{
		m_pReceive.SetSel(0,MAX_CEDIT_DECR,true);
		m_pReceive.ReplaceSel("",false);
		if (start > MAX_CEDIT_DECR)
		{
			start -= MAX_CEDIT_DECR;
			end   -= MAX_CEDIT_DECR;
			m_pReceive.SetSel(start,end);
		}
		m_dLength -= MAX_CEDIT_DECR;
	}
	CoTaskMemFree((void *)LParam);
}

void CTestDlg::OnOK()
{
//	TRACE("CTestDlg::OnOK()\n");

	char buff[256];
	char buffout[256];
	char bufftemp[64];
	m_pSend.GetWindowText(buff,sizeof(buff)-1);
	m_pSend.SetWindowText("");

	//push the stack and add the latest
	for (unsigned i = 1; i < 10; i++)
		m_cLastCommands[10-i] = m_cLastCommands[9-i];
	m_cLastCommands[0] = buff;
	m_iLastCommand = 0;

	int j = 0;
	int state = 0;
	int bufftempindex = 0;
	short int byte = 0;
	char *token;
	int NumberOfBytes;

	__int64 Value = 0;

	for (unsigned int i = 0; i < strlen(buff); i++)
	{
		if (buff[i] == '{') 
			state = 1;
		switch (state) {
		case 0: 
//			buffout[j++] = toupper(buff[i]);
			buffout[j++] = buff[i];
			break;
		case 1:
			state = 2;
			bufftempindex = 0;
			break;
		case 2:
			if (buff[i] == '}')
			{
				if (strchr(bufftemp,':'))
				{
					token = strtok( bufftemp, ":");   
					if (token != NULL)
					{
						NumberOfBytes = atoi(token);
						if (NumberOfBytes > sizeof(Value)) NumberOfBytes = sizeof(Value);
 						if ((token = strtok( NULL, ":" ))!=NULL)
						{
							Value = _atoi64(token);
							for (;NumberOfBytes>0;NumberOfBytes--)
								buffout[j++] = ((char*)&Value)[NumberOfBytes-1];
						}
					}					
				}
				else
				{
					//convert temp string to byte
					bufftemp[bufftempindex] = NULL;
//try{
					sscanf(bufftemp,"%2hx",&byte);
//}
//catch(...)
//{
//	AfxMessageBox("Error at 20",MB_OK);
//	AfxMessageBox(bufftemp,MB_OK);
//}
					//append result to buffout
					buffout[j++] = (char)byte;
				}
				bufftempindex = 0;
				state = 0;
			}
			else
				bufftemp[bufftempindex++] = buff[i];
			break;
		}
	}
	if (((CButton*)GetDlgItem(IDC_RADIO_CR))->GetCheck()==1)
		buffout[j++] = '\r';
	else if (((CButton*)GetDlgItem(IDC_RADIO_LF))->GetCheck()==1)
		buffout[j++] = '\n';
	else if (((CButton*)GetDlgItem(IDC_RADIO_CRLF))->GetCheck()==1) {
		buffout[j++] = '\r';
		buffout[j++] = '\n';
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_LFCR))->GetCheck()==1) {
		buffout[j++] = '\n';
		buffout[j++] = '\r';
	}
	buffout[j] = NULL;
	DLG_COM_Transmit(j,buffout,m_dNode);//string flag, string, node#
}

void CTestDlg::OnTestCloseButton() 
{
//	TRACE("CTestDlg::OnTestCloseButton()\n");
	ShowWindow(SW_HIDE);
}

void CTestDlg::OnTestPrintButton() 
{
//	TRACE("CTestDlg::OnTestPrintButton()\n");

	int First,Last;
	m_pReceive.GetSel(First, Last);
	CString text;
	m_pReceive.GetWindowText(text);
	if (Last > First)
		text = text.Mid(First,First+Last);
	CFileDialog GetFileName(FALSE,"txt",NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.txt|*.txt||");
	if (GetFileName.DoModal() == IDOK)
	{
		CString FileName = GetFileName.GetPathName();
		FILE * OutFile;
		if ((OutFile = fopen(FileName,"wt"))!=NULL)
		{
			for (int i = 0; i < text.GetLength(); i++)
				if (text[i] != '\r')
					fputc(text[i],OutFile);
			fclose(OutFile);
		}
		else
			MessageBox("Open File Failed.  Aborting!","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}
}

BOOL CTestDlg::OnInitDialog() 
{
//	TRACE("CTestDlg::OnInitDialog()\n");

	HWND h = this->m_hWnd;

	CDialog::OnInitDialog();

	uIDC_RECEIVE     = m_pParent->m_uIDC_RECEIVE; 
	uIDC_RECEIVE_OOB = m_pParent->m_uIDC_RECEIVE_OOB;

	m_pSend.SetFocus();
	m_pReceive.SetLimitText(MAX_CEDIT_HIGH+1024);
	m_bDoingTransfer = false;

	if (EditFont.CreateFont(-12,0,0,0,FW_SEMIBOLD,0,0,0,0,0,0,0,0,"SYSTEM"))
	{
		m_pReceive.SetFont(&EditFont,false);
		m_pSend.SetFont(&EditFont,false);
	}

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->EnableMenuItem(SC_MAXIMIZE,MF_GRAYED |MF_BYCOMMAND);
		pSysMenu->EnableMenuItem(SC_SIZE,MF_GRAYED |MF_BYCOMMAND);
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, "About Watch...");
	}

	SetDlgItemInt(IDC_EDIT_DELAY,20);
	m_EditSpin.SetRange(1,100);

	((CButton*)GetDlgItem(IDC_RADIO_CR))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_LF))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_CRLF))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_LFCR))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_RADIO_NOTHING))->SetCheck(0);

	GetClientRect(&m_OldRect);
	
	CRect initRect;
	GetClientRect(initRect);
	initRect.left = initRect.right - GetSystemMetrics(SM_CXHSCROLL);
	initRect.top  = initRect.bottom - GetSystemMetrics(SM_CYVSCROLL);
	m_Grip.Create(WS_CHILD | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEGRIP |
		WS_VISIBLE,initRect, this, AFX_IDW_SIZE_BOX);

	m_bInitialized = true;
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTestDlg::OnTestClearButton() 
{
//	TRACE("CTestDlg::OnTestClearButton()\n");

	m_pReceive.SetSel(0,-1,true);
	m_pReceive.ReplaceSel("",false);
	m_dLength = 0;
	m_pSend.SetFocus();
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
	//watch for alt-f4 to try to close us -- don't let it
	if (nID == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
	}
	else if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CWatchAboutDlg dlgAbout(
			m_pParent->m_iDlgCloseMilliseconds,
			//&m_pParent->m_csVersion);
			&m_pParent->m_csNameVersionDate);

		dlgAbout.DoModal();
	}
	else
	{
  		CDialog::OnSysCommand(nID, lParam);
	}
}

BOOL CTestDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	//catch escape and don't do anything with it 
	//escape will close the dialog box
	if ((nID == 2) && (nCode == -1)) return FALSE;
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CTestDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
//	TRACE("CTestDlg::OnShowWindow()\n");
	CDialog::OnShowWindow(bShow, nStatus);
	m_pSend.SetFocus();
}

void CTestDlg::OnTestSendfileButton() 
{
//	TRACE("CTestDlg::OnTestSendfileButton()\n");
	if (m_bDoingTransfer)
	{
		m_bDoingTransfer = false;
		SetDlgItemText(IDC_TEST_SENDFILE_BUTTON,"Send File...");
		KillTimer(100);//stop reading
		KillTimer(200);//stop updating progress
		((CProgressCtrl *)GetDlgItem(IDC_TEST_PROGRESS))->SetPos(0);
		fclose(m_Finfile);
	}
	else
	{
		CFileDialog FileDialog(TRUE,"txt","",
			OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST, 
			"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||",this);
	
		if (FileDialog.DoModal()!=IDCANCEL)
		{
			int hfile;
			CString filename = FileDialog.GetPathName();
			if ((hfile = _open(filename,_O_RDONLY)) != -1)
			{
				if ((m_dFileSize = _filelength(hfile))!=-1)
				{
					if (( m_Finfile = _fdopen(hfile,"r")) != NULL)
					{
						int delay = GetDlgItemInt(IDC_EDIT_DELAY)*100;
						if (delay < 100) delay = 100;
						m_bDoingTransfer = true;
						m_dTotalBytesRead = 0;
						m_dBytesRemainingToSend = 0;
						m_dBytesSent = 0;
						SetTimer(100,delay/2,NULL);
						SetTimer(200,250,NULL);// 1/4 of a second
						SetDlgItemText(IDC_TEST_SENDFILE_BUTTON,"Abort Send");
						m_Progress.SetRange(0,(short)m_dFileSize);
						m_Progress.SetStep(m_dFileSize/100);
						m_Progress.SetPos(0);
					}
				}
			}
		}
	}
}

void CTestDlg::OnTimer(UINT nIDEvent) 
{
//	TRACE("CTestDlg::OnTimer(%u)\n",nIDEvent);
	if (nIDEvent == 100)
	{
		try
		{
			if (feof(m_Finfile))
			{
				KillTimer(100);
				m_bDoingTransfer = false;
	//			SetDlgItemText(IDC_TEST_SENDFILE_BUTTON,"Send File...");
	//			((CProgressCtrl *)GetDlgItem(IDC_TEST_PROGRESS))->SetPos(0);
				fclose(m_Finfile);
			}
			else
			{
				m_cTempBuf[0] = NULL;
				if (fgets( m_cTempBuf, sizeof(m_cTempBuf), m_Finfile ) != NULL )
				{
					m_dTotalBytesRead += strlen(m_cTempBuf);
					m_dBytesRemainingToSend += strlen(m_cTempBuf);

					if ((m_cTempBuf[0] != ';') && (m_cTempBuf[0] != NULL))
					{
						char *at;
						if ((at = strchr(m_cTempBuf,'\n'))!=NULL) at[0] = NULL;

						if (((CButton*)GetDlgItem(IDC_RADIO_CR))->GetCheck()==1)
							strcat(m_cTempBuf,"\r");
						else if (((CButton*)GetDlgItem(IDC_RADIO_LF))->GetCheck()==1)
							strcat(m_cTempBuf,"\n");
						else if (((CButton*)GetDlgItem(IDC_RADIO_CRLF))->GetCheck()==1) 
							strcat(m_cTempBuf,"\r\n");
						else if (((CButton*)GetDlgItem(IDC_RADIO_LFCR))->GetCheck()==1) 
							strcat(m_cTempBuf,"\n\r");
						
						DLG_COM_Transmit(ISZSTRING,m_cTempBuf,m_dNode,true);
					} 
					else
					{
						if ((m_cTempBuf[1] == ';') && (m_cTempBuf[0] != NULL))
						{
							char* temp;
							int length = strlen(m_cTempBuf);
							temp = (char*)CoTaskMemAlloc(length);
							memcpy(temp,m_cTempBuf,length);
							PostMessage(uIDC_RECEIVE,length,(LPARAM)temp);
						}
					}
				}
			}
		}
		catch(...)
		{
			m_bDoingTransfer = false;
			SetDlgItemText(IDC_TEST_SENDFILE_BUTTON,"Send File...");
			KillTimer(100);
			KillTimer(200);
			m_Progress.SetPos(0);
	//		fclose(m_Finfile);
		}
	}
	if (nIDEvent == 200)
	{
		if (m_dBytesRemainingToSend > 0)
		{
			//1000 mSec at 9600 baud == 960 char iff None,8,1
			//500  mSec at 9600 baud == 480 char iff None,8,1
			//250  mSec at 9600 baud == 240 char iff None,8,1
			//this is a hack at best 'cause at any one time we 
			//really don't know how much the serial port has 
			//actually sent from this stream
			m_dBytesRemainingToSend -= 240;
			m_dBytesSent += 240;
			m_Progress.SetPos(m_dBytesSent);
		}
		else
		{
			if (!m_bDoingTransfer)
			{
				KillTimer(200);
				SetDlgItemText(IDC_TEST_SENDFILE_BUTTON,"Send File...");
				((CProgressCtrl *)GetDlgItem(IDC_TEST_PROGRESS))->SetPos(0);
			}
		}
	}
	if (nIDEvent == INVALIDATETIMER)
	{
		Invalidate(TRUE);
		KillTimer(INVALIDATETIMER);
	}
	CDialog::OnTimer(nIDEvent);
}

void CTestDlg::OnChangeEditDelay() 
{
	int delay = GetDlgItemInt(IDC_EDIT_DELAY)*100;
	if (delay < 100) 
	{
		delay = 100;
		SetDlgItemInt(IDC_EDIT_DELAY,1);
	}
	if (m_bDoingTransfer)
	{
		KillTimer(100);
		SetTimer(100,delay,NULL);
	}
}
void CTestDlg::OnRadioMost()
{
//	TRACE("CTestDlg::OnRadioMost()\n");
	GetDlgItem(IDC_TEST_SEND)->SetFocus();
}

BOOL CTestDlg::DestroyWindow() 
{
//	TRACE("CTestDlg::DestroyWindow()\n");
	// TODO: Add your specialized code here and/or call the base class
	//return CDialog::DestroyWindow();
	return ::DestroyWindow(m_hWnd);
	//return -1;
}

void CTestDlg::OnSize(UINT nType, int cx, int cy) 
{
//	TRACE("CTestDlg::OnSize()\n");

	CDialog::OnSize(nType, cx, cy);
	
	if (m_bInitialized && cx > 0 && cy > 130)
	{
		KillTimer(INVALIDATETIMER);
		int iY_Offset = cy - m_OldRect.bottom;
		int iX_Offset = cx - m_OldRect.right;

		CRect rect;	
		CWnd * item;
		
		item = GetDlgItem(IDC_TEST_RECEIVE);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top,
			rect.Width()+ iX_Offset,rect.Height() + iY_Offset,true);

		item = GetDlgItem(IDC_STATIC_ATEND);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset, 
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_RADIO_CR);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset, 
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_RADIO_LF);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_RADIO_CRLF);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_RADIO_LFCR);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_RADIO_NOTHING);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_SEND);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_SENDFILE_BUTTON);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_STATIC_DELAY);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_EDIT_DELAY);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_SPIN_DELAY);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_PROGRESS);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_CLOSE_BUTTON);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_CLEAR_BUTTON);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_TEST_PRINT_BUTTON);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		item = GetDlgItem(IDC_STATIC_MSG);
		item->GetWindowRect(&rect);
		ScreenToClient(&rect);
		item->MoveWindow(rect.left, rect.top + iY_Offset,
			rect.Width(),rect.Height(),true);

		CWnd *resizeWnd = GetDlgItem(AFX_IDW_SIZE_BOX);
		if (resizeWnd) 
		{
			CRect rect, rect2;
			resizeWnd->GetWindowRect(&rect);
			GetClientRect(rect2);
			rect2.left = rect2.right - rect.Width();
			rect2.top  = rect2.bottom - rect.Height();
			resizeWnd->MoveWindow(&rect2);
		}

		GetClientRect(&m_OldRect);
		SetTimer(INVALIDATETIMER,100,NULL);
	}
}

int CTestDlg::DLG_COM_Transmit(int ilength,char *output,int Node, bool Quiet)
{
//	TRACE("CTestDlg::DLG_COM_Transmit(ilength = %d, output = %s)\n", ilength, output);

	BLOB blob;
	blob.cbSize = 1 + strlen(output);
	blob.pBlobData = (unsigned char *)output;

	ULONG byteswritten;
	VARIANT_BOOL vbquiet = Quiet ? VARIANT_TRUE : VARIANT_FALSE;
	HRESULT hr = m_pComm->Transmit((ULONG)ilength, blob, (ULONG)Node, vbquiet, &byteswritten);	
	return (int)byteswritten;
}

BOOL CTestDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->hwnd == m_pSend.m_hWnd)
	{
		//key pressed and up arrow
		if (pMsg->message == 256 && pMsg->wParam == 38) //up
		{
			//set the window text
			m_pSend.SetWindowText(m_cLastCommands[m_iLastCommand]);
			//get ready for another up
			m_iLastCommand++;
			//loop within the number of saved commands
			if (m_iLastCommand > 9) m_iLastCommand = 0;
		}
		//key pressed an down arrow
		else if (pMsg->message == 256 && pMsg->wParam == 40) //down
		{
			//set the window text
			m_pSend.SetWindowText(m_cLastCommands[m_iLastCommand]);
			//get ready for the next
			m_iLastCommand--;
			//loop within the number of saved commands
			if (m_iLastCommand < 0) m_iLastCommand = 9;
		}
		//else if (pMsg->message == 256 && pMsg->wParam == 16) //enter
		//	m_iLastCommand = 0;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CTestDlg::OnSpecialKey(WPARAM wParam, LPARAM)
{
	switch (wParam) {
	case VK_UP:
		//set the window text
		m_pSend.SetWindowText(m_cLastCommands[m_iLastCommand]);
		//get ready for another up
		m_iLastCommand++;
		//loop within the number of saved commands
		if (m_iLastCommand >= COMMANDSTACKSIZE) m_iLastCommand = 0;
		break;
	case VK_DOWN:
		//set the window text
		m_pSend.SetWindowText(m_cLastCommands[m_iLastCommand]);
		//get ready for the next
		m_iLastCommand--;
		//loop within the number of saved commands
		if (m_iLastCommand < 0) m_iLastCommand = COMMANDSTACKSIZE-1;
		break;
	case VK_ESCAPE:
		m_pSend.SetWindowText("");
	}
}
