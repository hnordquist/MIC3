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
//ccbutton.cpp

#include <stdafx.h> // changed "" to <> gjk
#include "ccbutton.h"
#include "ccbutton_messages.h"
#include "ini_definitions.h"
#include "colors.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

//constructor

CColorButton::CColorButton(bool ParticipateInNet, char *inifile)
	:CButton()
{
	m_LastSendDateTime = 0.0;
	if (inifile)
		strncpy(m_szIniFile, inifile, sizeof(m_szIniFile));
	else
		m_szIniFile[0] = NULL;

	m_bParticipateInNetwork = ParticipateInNet;

	FinishConstruction();
}

void CColorButton::FinishConstruction()
{
	//set default face and text
	face_color = GetSysColor(COLOR_BTNFACE);
	text_color = GetSysColor(COLOR_BTNTEXT);
	flag_color = COLOR_RED;
	flag_size  = 32;
	flag_bell  = false;
	flag_time  = false;
	flag_flag  = false;
	flag_othr  = false;
	flag_disk  = false;
	flag_square = false;
	m_bFlash   = false;

	if (m_bParticipateInNetwork)
		m_dNetIndex = m_dNetCount++;

	pflag_font = NULL;

	flag = 'O';//flag character in wingdings
	bell = '%';//bell character in wingdings
	time = '»';//clock character in wingdings
	othr = 'L';//other character in wingdings(frown face)
	disk = '<';//disk character in wingdings
	square = 'q';//Square box in wingdings  //MIC_CB_028 to MIC 1.9.0.7
	space = ' ';

	char szTemp[16];
	szTemp[0] = NULL;
	GetPrivateProfileString("CONFIGURATION","BUTTONFLAGS","",szTemp,sizeof(szTemp)-1,m_szIniFile);


	// Modified below to bring the MIC_CB_028 baseline code
	// up with the MIC 1.9.0.7 baseline code.
	// PJM 01/19/05
	//
	//if old style
	if (strlen(szTemp)==5)
	{
		flag = szTemp[0];
		bell = szTemp[1];
		time = szTemp[2];
		othr = szTemp[3];
		disk = szTemp[4];
		szTemp[5] = square = '´';
		szTemp[6] = NULL;
		WritePrivateProfileString(CONFIGURATION,BUTTONFLAGS,szTemp,m_szIniFile);
	}
	//if new including ?
	else if (strlen(szTemp)==6)
	{
		flag = szTemp[0];
		bell = szTemp[1];
		time = szTemp[2];
		othr = szTemp[3];
		disk = szTemp[4];
		square = szTemp[5];
	}
	//otherwise 
	else 
	{
		WritePrivateProfileString(CONFIGURATION,BUTTONFLAGS,"O%»L<´",m_szIniFile);
	}
	//
	// end PJM 01/19/05

	numeric = 0;
	m_iBase = 10;

	square = 'q';  //<<QA>>MIC_GM 5/2/2005

	//SCR 118
	CVersionInfo VersionInfo;
	m_csVersion = VersionInfo.FileVersion();
	m_csVersion.Remove(' ');
	m_csVersion.Replace(',','.');
	CString Name = VersionInfo.InternalName();
	m_csNameVersionDate = Name + "\t" + m_csVersion + "\t" + __DATE__;

}

CColorButton::~CColorButton()
{
	//TRACE("CColorButton::DTOR()\n");
	delete pflag_font;
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DISPLAY_NUMERIC_STRING,	OnDisplayNumericString)
	ON_MESSAGE(WM_GET_FACE_COLOR,			OnGetFaceColor)
	ON_MESSAGE(WM_GET_NUMERIC,				OnGetNumeric)
	ON_MESSAGE(WM_GET_VERSION,				OnGetVersion)
	ON_MESSAGE(WM_PUT_FACE_COLOR,			OnPutFaceColor)
	ON_MESSAGE(WM_PUT_FLASH,				OnPutFlash)
	ON_MESSAGE(WM_PUT_NUMERIC,				OnPutNumeric)
	ON_MESSAGE(WM_PUT_NUMERIC_STRING,      OnPreNumericString)
	ON_MESSAGE(WM_SHOW_ICON_BELL,          OnShowIconBell)
	ON_MESSAGE(WM_SHOW_ICON_CLOCK,         OnShowIconClock)
	ON_MESSAGE(WM_SHOW_ICON_FROWN,         OnShowIconFrown)
	ON_MESSAGE(WM_SET_ERROR_ICON_BOMB,     OnSetErrorIconBomb)
	ON_MESSAGE(WM_SET_ERROR_ICON_FLAG,     OnSetErrorIconFlag)
	ON_MESSAGE(WM_SET_ERROR_ICON_SKULL,    OnSetErrorIconSkull)
	ON_MESSAGE(WM_SHOW_ICON_ERROR,			OnShowIconError)
	ON_MESSAGE(WM_SET_DISK_ICON_FLOPPY,		OnSetDiskIconFloppy)
	ON_MESSAGE(WM_SET_DISK_ICON_HOURGLASS,	OnSetDiskIconHourglass)
	ON_MESSAGE(WM_SHOW_ICON_DISK,			OnShowIconDisk)
	ON_MESSAGE(WM_SHOW_ICON_SQUARE,			OnShowIconSquare)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//the "real" work of displaying the owner draw button
//this function is called when the button is redrawn
void CColorButton::DrawItem( LPDRAWITEMSTRUCT lpDI ) 
{
	//TRACE("CColorButton::DrawItem(CtlID = %d)\n",lpDI->CtlID);

	CRect r1(lpDI->rcItem);//rectangle for up/down effect
	CRect r2(lpDI->rcItem);//rectangle for fill color
	CRect r3(lpDI->rcItem);//rectangle for focus 
	CRect r4(lpDI->rcItem);//rectangle for flags
	CRect r5(0,0,0,0);     //temp working rectangle 
	CRect r6(0,0,0,0);     //temp working rectangle 
	r1.InflateRect(-1,-1); //rectangle for up/down effect
	r2.InflateRect(-2,-2); //rectangle for fill color
	r3.InflateRect(-5,-5); //rectangle for focus 
	r4.InflateRect(-3,-3); //rectangle for flags
	char text[255];

	//Draw whole button
	if ((lpDI->itemAction & ODA_DRAWENTIRE) ||
		(lpDI->itemAction & ODA_SELECT))
	{
		//get a dc from handle hDC
		CDC dc;
		dc.m_hDC = lpDI->hDC;

		//save the current state
		int saveddc = dc.SaveDC();

		//create the text 
		CButton::GetWindowText(text,sizeof(text)-1);

		if (m_iBase > 0)
		{
			char tempstr[17];
			_itoa(numeric,tempstr,m_iBase);
			strcat(text,"\n");
			if (m_szPreNum[strlen(m_szPreNum)-1]=='%')
			{
				strncat(text,m_szPreNum,strlen(m_szPreNum)-1);
				strcat(text,tempstr);
				strcat(text,"%");
			}
			else
			{
				strcat(text,m_szPreNum);
				strcat(text,tempstr);
			}
		}
		// end PJM 01/19/05

		//force black but could enhance to settable color
		dc.SetTextColor(text_color);
		//make sure the background shows through the text
		dc.SetBkMode(TRANSPARENT);

		//create and select a solid brush at the current color
		CBrush sb;
		if (m_bFlashState && m_bFlash)
			sb.CreateSolidBrush(COLOR_WHITE);
		else
			sb.CreateSolidBrush(face_color);
		dc.SelectObject(&sb);

		//draw an outline in black
		if (lpDI->itemState & ODS_FOCUS)
			dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_3DDKSHADOW),::GetSysColor(COLOR_3DDKSHADOW));
		else
			dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_BTNHILIGHT),::GetSysColor(COLOR_3DDKSHADOW));

		//draw the raised or lowered edge of button
		if (lpDI->itemState & ODS_SELECTED)
		{
		  dc.Draw3dRect(r1,::GetSysColor(COLOR_BTNSHADOW),
		                   ::GetSysColor(COLOR_BTNHILIGHT));
		}
		else
		{
		  dc.Draw3dRect(r1,::GetSysColor(COLOR_BTNHILIGHT), 
		                   ::GetSysColor(COLOR_BTNSHADOW));
		}

		//fill with the desired brush/color
		dc.FillRect(r2,&sb);

		//calculate the bounding box of the text
		UINT uFormat;

		if (m_iBase >= 0)
			uFormat = DT_CENTER|DT_WORDBREAK;
		else
			uFormat = DT_CENTER|DT_WORDBREAK|DT_SINGLELINE |DT_VCENTER;

		dc.DrawText(text, strlen(text), &r6, DT_CALCRECT);
		//dc.DrawText(text, strlen(text), &r6, DT_CALCRECT | uFormat);

		//center/top the box in the button
		//CRect r5((r2.right-r6.right)/2,
		//	3,
		//	(r2.right-r6.right)/2+r6.right,
		//	3+r6.bottom);
		if (m_iBase == -1)
		{
			r5 = r4;
		}
		else
		{
			r5.left  = (r2.right-r6.right)/2;
			r5.top   = 3;
			r5.right = (r2.right-r6.right)/2+r6.right;
			r5.bottom = 3+r6.bottom;
		}

		//offset the position of the text box if selected
		if (lpDI->itemState & ODS_SELECTED) 
		{
			r5.right += 2;
			r5.bottom += 2;
			r5.top += 2;
			r5.left += 2;
		}

		//draw the button's text
		dc.DrawText(text, strlen(text), &r5, uFormat);

		if (lpDI->itemState & ODS_DISABLED) 
		{
			r4 = r5;
			r4.InflateRect(-2,-2,2,2);
			dc.SetTextColor(COLOR_WHITE);
			dc.DrawText(text,strlen(text),&r4,uFormat);
			r4.InflateRect(1,1,-1,-1);
			dc.SetTextColor(face_color);
			dc.DrawText(text,strlen(text),&r4,uFormat);
		}

		//draw the button's special flags
		//concat all the selected flags
		if (flag_flag || flag_bell || flag_time || flag_othr || flag_disk || flag_square) 
		{
			int j = 0;
			if (flag_flag)
			{
				text[j]=flag;
				j++;
			}
			if (flag_bell)
			{
				text[j]=bell;
				j++;
			}
			if (flag_time)
			{
				text[j]=time;
				j++;
			}
			if (flag_disk)
			{
				text[j]=disk;
				j++;
			}
			if (flag_othr)
			{
				text[j]=othr;
				j++;
			}
			if (flag_square)
			{
				text[j] = square;
				j++;
			}
			text[j] = NULL;

			dc.SelectObject(pflag_font);
			if (face_color==flag_color)
				dc.SetTextColor(COLOR_BLACK);
			else
				dc.SetTextColor(flag_color);

			dc.DrawText(text,j,
				CRect(r4.left,r4.bottom-flag_size,
				r4.right,r4.bottom),
				DT_CENTER);

		}

		//restore all of the dc's condition
		dc.RestoreDC(saveddc);
		if (lpDI->itemState & ODS_FOCUS)
			DrawFocusRect(lpDI->hDC,r3);

		//make sure our dc never gets used
		dc.m_hDC = NULL;
		return;
	}

	//Focus changing
	if (lpDI->itemAction & ODA_FOCUS) 
	{
		//only need to add or drop the focus rectangle
		//if it is already displayed redrawing it will
		//remove it.
		DrawFocusRect(lpDI->hDC, r3);
		CDC dc;
		dc.m_hDC = lpDI->hDC;
		dc.Draw3dRect(&lpDI->rcItem,::GetSysColor(COLOR_3DDKSHADOW),::GetSysColor(COLOR_3DDKSHADOW));

	}
}

void CColorButton::GetStrings(CString* cFirst, CString *cSecond)
{
	char tempstr[32];

	GetWindowText(*cFirst);

	//<<QA>> need to verify this logic.
	if (m_iBase)
		cSecond->Format("%s%s",m_szPreNum,_itoa(numeric,tempstr,m_iBase));
	else
		cSecond->Format("%s",m_szPreNum);
}


//set the "flag" icon 
//used to indicate comm error occured
void CColorButton::SetFlag(bool set, bool redraw)
{
	flag_flag = set;
	if (redraw) Redraw(false);
}

void CColorButton::SetPreNum(char *txt, bool redraw) 
{
	//TRACE("CColorButton::SetPreNum(%s)\n",txt);

	if (strlen(txt) < sizeof(m_szPreNum))
		strcpy(m_szPreNum,txt);

	if (redraw) 
		Redraw(false);
}

bool CColorButton::IsDoingNetwork()
{
	return (m_bDoingNetwork && m_bParticipateInNetwork);
}

void CColorButton::SendStateToNetworkCoConspirators()
{
	//send packet
	typedef struct senddata 
	{
		int BBM;
		int Index;
		int	TotalNetButton;
		int Base;
		COLORREF FaceColor;
		bool Flash;
		bool Flag;
		bool Bell;
		bool Time;
		bool Other;
		bool Disk;
		char FlagC;
		char BellC;
		char TimeC;
		char OtherC;
		char DiskC;
		char Title[32];
		char PreNum[30];
		char SquareC;
		bool Square;
	} SendDataType;

	if (IsDoingNetwork())
	{
		SendDataType SendData;
		memset(&SendData,NULL,sizeof(SendData));

		SendData.BBM = numeric;
		SendData.Index = m_dNetIndex;
		SendData.TotalNetButton = m_dNetCount;
		SendData.Base = m_iBase;
		SendData.FaceColor = face_color;
		SendData.Flash = m_bFlash;
		SendData.Flag = flag_flag;
		SendData.Bell = flag_bell;
		SendData.Time = flag_time;
		SendData.Other= flag_othr;
		SendData.Disk = flag_disk;
		SendData.Square = flag_square;
		SendData.FlagC = flag;
		SendData.BellC = bell;
		SendData.TimeC = time;
		SendData.OtherC= othr;
		SendData.DiskC = disk;
		SendData.SquareC = square;

		// Modified below to bring the MIC_CB_028 baseline code
		// up with the MIC 1.9.0.7 baseline code.
		// PJM 01/19/05

		//limit the number of characters we try
		//to place in SendData.Title to 30 or less!
		CButton::GetWindowText(SendData.Title,sizeof(SendData.Title)-1);
		if (m_iBase > 0)
			strncpy(SendData.PreNum,m_szPreNum,sizeof(SendData.PreNum)-1);
		// end PJM 01/19/05 

		s_sAddr_Rem.sin_family = PF_INET;
		s_sAddr_Rem.sin_port = (unsigned short)(htons((unsigned short)s_dDefPort));
		char temp1[4];
		char temp2[32];
		int i = 1;
		_itoa(i,temp1,10);

		// We should never get here with a NULL filename.   But since it is possible, should
		// not allow blowup
		if (0 != strlen(m_szIniFile))
			GetPrivateProfileString(NETWORK,temp1,"",temp2,sizeof(temp2),m_szIniFile);
		else
			temp2[0] = NULL;

		while (strlen(temp2) > 0)
		{
			s_sAddr_Rem.sin_addr.s_addr = inet_addr(temp2);
			sendto(s_sd_dg,(char*)&SendData,sizeof(SendData),0,
				(struct sockaddr FAR *) &s_sAddr_Rem,sizeof(s_sAddr_Rem));
			i++;
			_itoa(i,temp1,10);

			// We should never get here with a NULL filename.   But since it is possible, should
			// not allow blowup
			if (0 != strlen(m_szIniFile))
				GetPrivateProfileString("NETWORK",temp1,"",temp2,sizeof(temp2),m_szIniFile);
			else
				temp2[0] = NULL;
		}
		m_LastSendDateTime = COleDateTime::GetCurrentTime();
	}
}

void CColorButton::Redraw(BOOL redraw)
{

	SendStateToNetworkCoConspirators();
	//invalidate the rectangle
	if (IsWindow(m_hWnd))
		Invalidate(redraw);
}

//set the "disk" icon 
//used to indicate cannot write to disk
void CColorButton::SetDisk(bool set, bool redraw)
{
	flag_disk = set;

	if (redraw) 
		Redraw(false);
}

//set the "bell" icon
//used to indicate currently in MII
void
CColorButton::SetBell(bool set, bool redraw) {
	flag_bell = set;
	if (redraw) Redraw(false);
}

//set the "clock" icon
//used to indicate difference between MIC and instrument 
//is larger than acceptable
void
CColorButton::SetTime(bool set, bool redraw) {
	flag_time = set;
	if (redraw) Redraw(false);
}

//set the "frown face" icon
//used to indicate problem with memory or power
void CColorButton::SetOthr(bool set, bool redraw) 
{
	flag_othr = set;
	if (redraw) 
		Redraw(false);
}

void CColorButton::SetSquare(bool set, bool redraw) 
{
	flag_square = set;
	if (redraw) Redraw(false);
}

//set the ? character to something other
//than the current (default) setting
void CColorButton::SetSquareChar(char cSquare, bool redraw) 
{
	square = cSquare;
	if (redraw) Redraw(false);
}

//set the base of the numeric display
void CColorButton::SetBase(int ibase, bool redraw) 
{
	m_iBase = ibase;
	if (redraw) 
		Redraw(false);
}

//turn on or off flashing button
void CColorButton::SetFlash(bool set)
{
	if (set)
	{
		m_bFlash = true;
		m_bFlashState = true;
		SetTimer(TIMER_FLASH,500,NULL);
		Redraw(true);
	}
	else
	{
		KillTimer(TIMER_FLASH);
		m_bFlash = false;
		m_bFlashState = false;
		Redraw(true);
	}
}

int CColorButton::GetState()
{
	int ch = NULL;
	if (flag_bell) ch = ch | BELL_BIT;
	if (flag_time) ch = ch | TIME_BIT;
	if (flag_flag) ch = ch | FLAG_BIT;
	if (flag_othr) ch = ch | OTHR_BIT;
	if (flag_disk) ch = ch | DISK_BIT;
	if (m_bFlash)  ch = ch | FLASH_BIT;
	if (flag_square)ch = ch | SQUARE_BIT;
	if ((flag_flag) && ((flag == 'N')||(flag == 'M')))//"skull" or "bomb"
		ch = ch | DEAD_BIT;
	return ch;
}

void
CColorButton::ReCount(bool reset)
{
	//called during recounting of network
	//participating buttons
	if (reset) 
		m_dNetCount = 0;
	else if (m_bParticipateInNetwork)
		m_dNetIndex = m_dNetCount++;
}

//create and initialize
BOOL CColorButton::Create(LPCTSTR lpszCaption, 
		  DWORD dwStyle, 
		  const RECT& rect, 
		  CWnd* pParentWnd, 
		  UINT nID ) 
{
	//TRACE("CColorButton::Create()\n");

	//get wingdings font for special flags
	pflag_font = new CFont;
	pflag_font->CreateFont(flag_size,0,0,0,FW_BOLD,
		0,0,0,1,0,0,0,0,"Wingdings");

	if (m_dNetIndex == 0)
	{
		//getport
		s_dDefPort = GetPrivateProfileInt("NETWORK","PORT",1028,m_szIniFile);
		s_wVer = MAKEWORD(1,1);
		s_iRc = WSAStartup(s_wVer, &s_WSAData);
		if (s_iRc == 0)
		{
			if (LOBYTE(s_WSAData.wVersion) == 1 && 
				HIBYTE(s_WSAData.wVersion) == 1)
			{
				s_sd_dg = socket(PF_INET,SOCK_DGRAM,0);

				if (s_sd_dg != INVALID_SOCKET)
				{
					s_sAddr_Loc.sin_family=PF_INET;
					s_sAddr_Loc.sin_port=htons(0);
					s_sAddr_Loc.sin_addr.s_addr=htonl(INADDR_ANY);
					s_iRc = bind(s_sd_dg,(struct sockaddr far *) & s_sAddr_Loc,sizeof(s_sAddr_Loc));
				}
			}
			else
				s_iRc = SOCKET_ERROR;
				
		}
		else
			s_iRc = SOCKET_ERROR;
	}

	//create the button and force OWNER DRAW
	return CButton::Create(lpszCaption, dwStyle | BS_OWNERDRAW, 
		rect, pParentWnd, nID );
}

//set the text color and redraw the button
COLORREF CColorButton::SetTextColor(UINT color, bool redraw) 
{
	COLORREF temp = text_color;
	text_color = color;
	if (redraw) 
		Redraw(false);
	return temp;
}

//set the face color and redraw the button
COLORREF CColorButton::SetFaceColor(UINT color, bool redraw) 
{
	COLORREF temp;
	try
	{
		temp = face_color;
		face_color = color;
		if (redraw) 
			Redraw(false);
	}
	catch(...)
	{}
	return temp;
}

//get the current face color
COLORREF CColorButton::GetFaceColor()
{
	return face_color;
}

//set the face color and redraw the button
COLORREF CColorButton::SetFlagColor(UINT color, bool redraw) 
{
	COLORREF temp = flag_color;
	flag_color = color;
	if (redraw) 
		Redraw(false);
	return temp;
}

//set the flag size
void CColorButton::SetFlagSize(UINT size, bool redraw) 
{
	flag_size = size;
	//get wingdings font for special flags
	if (pflag_font) 
	{
		delete pflag_font;
		pflag_font = new CFont;
		pflag_font->CreateFont(flag_size,0,0,0,FW_BOLD,
			0,0,0,1,0,0,0,0,"Wingdings");
	}
	if (redraw) 
		Redraw(false);
}

//set the numeric value
void CColorButton::SetNumeric(int number, bool redraw) 
{
	numeric = number;
	if (redraw) 
		Redraw(false);
}

//get the current numberic value
int CColorButton::GetNumeric()
{
	return numeric;
}

//set the "flag" character to something other
//than the current (default) setting
void CColorButton::SetFlagChar(char cflag, bool redraw)
{
	flag = cflag;
	if (redraw) 
		Redraw(false);
}

//set the "disk" character to someting other
//than the current (default) setting
void CColorButton::SetDiskChar(char cdisk, bool redraw)
{
	disk = cdisk;
	if (redraw) 
		Redraw(false);
}

//set the "bell" character to someting other
//than the current (default) setting
void CColorButton::SetBellChar(char cbell, bool redraw) 
{
	bell = cbell;
	if (redraw) 
		Redraw(false);
}

//set the "clock" character to someting other
//than the current (default) setting
void CColorButton::SetTimeChar(char ctime, bool redraw) 
{
	time = ctime;
	if (redraw) 
		Redraw(false);
}

//set the "frown face" character to someting other
//than the current (default) setting
void CColorButton::SetOthrChar(char cothr, bool redraw) 
{
	othr = cothr;
	if (redraw) 
		Redraw(false);
}



/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::OnTimer(UINT nIDEvent) 
{
	//#define TIMER_FLASH (1)
	//#define TIMER_NETWORK (2)

	if (nIDEvent == TIMER_NETWORK)
	{
		Redraw(false);//redraw and update network info
	}
	else if (nIDEvent == TIMER_FLASH)
	{

		//redraw the button at the flash rate
		m_bFlashState = !m_bFlashState;
		Invalidate(true);
	}

	CButton::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////////
//
//                      SetINIPathname()
//
/////////////////////////////////////////////////////////////////////////////////
bool CColorButton::SetINIPathname(char *mbstr, int count)
{
	//TRACE("CColorButton::SetINIPathname()\n");
	strncpy(m_szIniFile,mbstr,count);
	return true;
}
/////////////////////////////////////////////////////////////////////////////////
//
//                     SetDoingNetwork()
//
/////////////////////////////////////////////////////////////////////////////////
bool CColorButton::SetDoingNetwork(bool b)
{
	//TRACE("CColorButton::SetDoingNetwork()\n");
	m_bDoingNetwork = b;
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////
//
// Message handlers to process messages from the implementation of the ICB COM interface.
//
/////////////////////////////////////////////////////////////////////////////////////////
//
// The messages are sent (not posted) by the functions in file MICButton.cpp in the ColorButton
// project.  It is not necessary to define the message ID values dynamically since the
// COM interface implementation functions  use the ::SendMessage() function and not the
// ::PostMessage() function.
//
//////////////////////////////////////////////////////////////////////////////////////////
LRESULT CColorButton::OnDisplayNumericString(WPARAM vb, LPARAM radix)
{
	if (vb == VARIANT_TRUE)
		SetBase(int(radix),true);  // >0 means 2 lines of text, at top of button
	else
		SetBase(0,true);	       // 0 means one line of text, at top of button
	return 0;
}
LRESULT CColorButton::OnGetVersion(WPARAM w, LPARAM l)
{
	return (LONG)&m_csNameVersionDate;
}
LRESULT CColorButton::OnGetFaceColor(WPARAM, LPARAM)
{
	return GetFaceColor();
}
LRESULT CColorButton::OnGetNumeric(WPARAM, LPARAM)
{
	return GetNumeric();
}
LRESULT CColorButton::OnPutFaceColor(WPARAM w, LPARAM)
{
	SetFaceColor(UINT(w));
	return 0;
}
LRESULT CColorButton::OnPutFlash(WPARAM w, LPARAM)
{
	SetFlash(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnPutNumeric(WPARAM w, LPARAM)
{
	SetNumeric(int(w));
	return 0;
}
LRESULT CColorButton::OnPreNumericString(WPARAM w, LPARAM )
{
	SetPreNum((char *)w);
	return 0;
}
LRESULT CColorButton::OnShowIconBell(WPARAM w, LPARAM)
{
	SetBell(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnShowIconClock(WPARAM w, LPARAM )
{
	SetTime(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnShowIconFrown(WPARAM w, LPARAM)
{
	SetOthr(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnShowIconSquare(WPARAM w, LPARAM)
{
	SetSquare(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnSetErrorIconBomb(WPARAM, LPARAM)
{
	SetFlagChar(BOMB_CHAR);
	return 0;
}
LRESULT CColorButton::OnSetErrorIconFlag(WPARAM, LPARAM)
{
	SetFlagChar(FLAG_CHAR);
	return 0;
}
LRESULT CColorButton::OnSetErrorIconSkull(WPARAM, LPARAM)
{
	SetFlagChar(SKULL_CHAR);
	return 0;
}
LRESULT CColorButton::OnShowIconError(WPARAM w, LPARAM)
{
	SetFlag(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}
LRESULT CColorButton::OnSetDiskIconFloppy(WPARAM,LPARAM)
{
	SetDiskChar(DISK_CHAR);
	return 0;
}
LRESULT CColorButton::OnSetDiskIconHourglass(WPARAM, LPARAM)
{
	SetDiskChar(HOURGLASS_CHAR);
	return 0;
}
LRESULT CColorButton::OnShowIconDisk(WPARAM w, LPARAM)
{
	SetDisk(VARIANT_BOOL(w) == VARIANT_TRUE);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CYellowButton

CYellowButton::CYellowButton(
	char *pcaption,
	UINT id, 
	char *inifile, 
	CWnd *parent,
	UINT RGBcolor) //Defaults to COLOR_YELLOW.  Can be any color.
	: CColorButton(false, inifile)
{
	m_uID = id;
	m_pWnd = parent;
	m_csCaption = CString(pcaption);
	m_uRGBcolor = RGBcolor;
	Create();
}

CYellowButton::~CYellowButton()
{
}


BEGIN_MESSAGE_MAP(CYellowButton, CColorButton)
	//{{AFX_MSG_MAP(CYellowButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CYellowButton::Create()
{
	CRect cRect;
	m_pWnd->GetDlgItem(m_uID)->GetWindowRect(&cRect);
	m_pWnd->ScreenToClient(&cRect);
	m_pWnd->GetDlgItem(m_uID)->ShowWindow(SW_HIDE);

	unsigned int uState = 
		WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_DLGFRAME |
		BS_PUSHBUTTON | BS_MULTILINE | BS_CENTER;

	BOOL success = 
		CColorButton::Create( 
			m_csCaption,  
			uState,
			cRect,
			m_pWnd,
			m_uID);

	if (success)
	{
		SetBase(-1,false);
		SetFaceColor(COLOR_GRAY,false);
		EnableWindow(FALSE);

		m_Font.CreateFont(
			-12,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Microsoft Sans Serif");

		SetFont(&m_Font);
	}

	return success;
}

BOOL CYellowButton::EnableWindow(BOOL bEnable)
{
	BOOL returnval;

	if (bEnable)
	{
		returnval = CColorButton::EnableWindow(TRUE);
		CColorButton::SetFaceColor(m_uRGBcolor);
	}
	else
	{
		returnval = CColorButton::EnableWindow(FALSE);
		CColorButton::SetFaceColor(COLOR_GRAY);
	}

	return returnval;
}

