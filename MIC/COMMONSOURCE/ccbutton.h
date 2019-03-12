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
// ccbutton.h

// User: Moore        Date: 6/23/05    Time: 2:52p
// Fixed button counter in ccbutton and MICMgr.

// User: Moore        Date: 5/18/05    Time: 3:13p
// Made CYellowButton font MS Sans Serif, 12 points.

// Clarified some of the "base" logic.
// Removed some dead code.
// Fixed a positioning problem.
// Changed "base" to "m_iBase".

// User: Moore        Date: 5/16/05    Time: 12:56p
// Parameterized color on CYellowButton so it doens't have to actually
// be yellow.

// User: Moore        Date: 4/29/05    Time: 3:41p
// Added get_Numeric

// User: Moore        Date: 4/29/05    Time: 10:14a
// Get rid of the term "glyph".  Add the square icon.
// Put message definitions out into ccbutton_messages.h.

#ifndef CCBUTTON_H
#define CCBUTTON_H

#include <winerror.h>
#include <winsock.h>
#include "colors.h"

static	WORD s_wVer;
static	WSADATA s_WSAData;
static	SOCKET s_sd_dg;
static	struct sockaddr_in s_sAddr_Loc;
static	struct sockaddr_in s_sAddr_Rem;
static	int s_dDefPort;
static  int s_iRc;
static int m_dNetCount;

#define TIMER_FLASH (1)
#define TIMER_NETWORK (2)

#define BELL_BIT	0x01
#define TIME_BIT	0x02
#define FLAG_BIT	0x04
#define OTHR_BIT	0x08
#define DISK_BIT	0x10
#define FLASH_BIT	0x20
#define SQUARE_BIT	0x40
#define DEAD_BIT	0x80

// wingding characters
#define CLOCK_CHAR		'»'
#define FACE_CHAR		'L'
#define DISK_CHAR		'<'
#define BELL_CHAR		'%'
#define BOMB_CHAR		'M'
#define SKULL_CHAR		'N'
#define FLAG_CHAR		'O'
#define HOURGLASS_CHAR	'6'
#define SQUARE_CHAR		'q'

class CColorButton: public CButton
{
public:
	CColorButton(bool ParticipateInNet = false, char *inifile = NULL);

	void FinishConstruction();

	BOOL Create( LPCTSTR lpszCaption, DWORD dwStyle, 
		         const RECT& rect, CWnd* pParentWnd, UINT nID );

	COLORREF SetFaceColor(UINT, bool redraw = true);
	COLORREF GetFaceColor();
	COLORREF SetTextColor(UINT, bool redraw = true);
	COLORREF SetFlagColor(UINT, bool redraw = true);
	int  GetNumeric();
	void GetStrings(CString* cFirst,CString* szSecond);
	void SetFlash(bool set);
	void SetNumeric(int, bool redraw = true);
	void SetFlagSize(UINT, bool redraw = true);
	void SetFlag(bool set, bool redraw = true);
	void SetBell(bool set, bool redraw = true);
	void SetTime(bool set, bool redraw = true);
	void SetOthr(bool set, bool redraw = true);
	void SetSquare(bool set, bool redraw = true);
	void SetDisk(bool set, bool redraw = true);
	void SetBase(int, bool redraw = true);
	void SetFlagChar(char ch, bool redraw = true);
	void SetBellChar(char ch, bool redraw = true);
	void SetTimeChar(char ch, bool redraw = true);
	void SetOthrChar(char ch, bool redraw = true);
	void SetDiskChar(char ch, bool redraw = true);
	void SetSquareChar(char ch, bool redraw = true);
	void SetPreNum(char *txt, bool redraw = true);
	int GetState();
	void ReCount(bool reset = false);
	void Redraw(BOOL redraw);
	void SendStateToNetworkCoConspirators();
	bool SetINIPathname(char *mbstr, int count);
	bool SetDoingNetwork(bool b);
	bool IsDoingNetwork();

	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	CString m_csNameVersionDate;

//private:

	char m_szIniFile[_MAX_PATH];

	COLORREF face_color;
	COLORREF text_color;
	COLORREF flag_color;
	UINT     flag_size;
	char	 m_szPreNum[32];

	bool     m_bDoingNetwork;
	bool     m_bParticipateInNetwork;

	CFont	*pflag_font;
	int      numeric;

	// m_iBase =  0 means one line of text, at top of button (e.g., Watch amd JSR)
	// m_iBase >  0 means 2 lines of text, at top of button (is default, Grand, APCUPS)
	// m_iBase >  0 also indicates the radix of the displayed number (e.g., base 2,8 10)
	// m_iBase = -1 means one line of text, centered vertically (yellow button)
	int      m_iBase;

	char	 flag;
	char	 bell;
	char	 time;
	char	 othr;
	char	 space;
	char	 disk;
	char	 square;
	bool	 m_bFlash;
	bool	 m_bFlashState;
	bool	 flag_flag;
	bool	 flag_bell;
	bool	 flag_time;
	bool	 flag_othr;
	bool	 flag_disk;
	bool	 flag_square;

	int      m_dNetIndex;
	CString  m_csVersion;

	COleDateTime m_LastSendDateTime;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	afx_msg void OnTimer(UINT nIDEvent);
	//afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);  //<<QA>> is this needed??
	//afx_msg void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	afx_msg LRESULT OnDisplayNumericString(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetVersion(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetFaceColor(WPARAM w, LPARAM l);
	afx_msg LRESULT OnGetNumeric(WPARAM w, LPARAM l);
	afx_msg LRESULT OnPutFaceColor(WPARAM w, LPARAM l);
	afx_msg LRESULT OnPutFlash(WPARAM w, LPARAM l);
	afx_msg LRESULT OnPutNumeric(WPARAM w, LPARAM l);
	afx_msg LRESULT OnPreNumericString(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconBell(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconClock(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconFrown(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconSquare(WPARAM w, LPARAM l);

	afx_msg LRESULT OnSetErrorIconBomb(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetErrorIconFlag(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetErrorIconSkull(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconError(WPARAM w, LPARAM l);

	afx_msg LRESULT OnSetDiskIconFloppy(WPARAM w, LPARAM l);
	afx_msg LRESULT OnSetDiskIconHourglass(WPARAM w, LPARAM l);
	afx_msg LRESULT OnShowIconDisk(WPARAM w, LPARAM l);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// YellowButton

class CYellowButton : public CColorButton
{
// Construction
public:
	CYellowButton(char *pcaption, 
		UINT id, 
		char *inifile, 
		CWnd *parent,
		UINT uRGBcolor = COLOR_YELLOW);

// Attributes
public:
	UINT m_uID;
	CWnd *m_pWnd;
	CString m_csCaption;
	UINT m_uRGBcolor;
	CFont m_Font;

// Operations
public:
	BOOL EnableWindow( BOOL bEnable = TRUE );
private:
	BOOL Create();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYellowButton)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CYellowButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CYellowButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
#endif  //CCBUTTON_H
