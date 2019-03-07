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
//MicMgrDlg.h 

// User: Moore        Date: 7/21/05    Time: 1:23p
// Updated in $/MIC 2.0/MicMgr
// Added restart after ReOrganization.

// User: Moore        Date: 6/15/05    Time: 3:00p
// Put in Halbig's default values. - SCR MIC 0008
// 
// User: Moore        Date: 6/14/05    Time: 3:53p
// FIXED 6/14/2005 MICDlg: Modified trigger of write extended 
// SOH to be based on time of previous and cycle time.  
// Previously it was a timer that was started when MIC was
//  started.  MIC now checks every minute to see if 
// it is time to print another extended SOH file.
// 
// User: Moore        Date: 6/10/05    Time: 6:27p
// PreTranslateMessage is required in CMicMgrDlg because of an
// MFC message processing error in the release build.
// 

#ifndef MICMGRDLG_H
#define MICMGRDLG_H

// MicMgrDlg.h : header file
//
#include "stdafx.h"
#include "../Interfaces/ICB.h"
#include "ccbutton.h"
#include "COM_Utilities.h"
#include "resource.h"
#include "FileMgr.h"


#ifdef ALLOWTRIGGERHOLDOFF
//#define BMPBACKGRND
#define TRIGGER_ON			1
#define TRIGGER_OFF			0
#define TRIGGER_TRIP		2
#define TRIGGER_CMD			(WM_APP + 12)
#define TRIGGER_CLR			(TRIGGER_CMD+1)
#define TRIGGER_EXCLUDE		(WM_USER + 42)
#endif


/////////////////////////////////////////////////////////////////////////////
// CMicMgrDlg dialog

class CMicMgrDlg : public CDialog
{
// Construction
public:
	CMicMgrDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CMicMgrDlg();

	// Windows registered message values.
	// These are universal values for communication and 
	// instrument component messages.
	UINT m_uIDC_RECEIVE;
	UINT m_uIDC_RECEIVE_OOB;
	UINT m_uIDC_COM_START;
	UINT m_uIDC_COM_STOP;
	UINT m_uIDC_COM_RESET;

	//
	// Member variables (in alphabetical order)
	//
	bool	m_bAtLeastOneISOIsCollectingData;
	bool	m_bNormalShutdown;		//flag normal shutdown
	bool	m_bSOHNewDay;			//day change flag
	bool	m_bValidShutdown;
	bool	m_bRestart;
	int		m_iLastX;
	int		m_iLastY;
	int		m_iSlowStartIndex;
	int		m_iSlowStartIntervalInSeconds;
	int     m_iSOHDay;				//current day
	int     m_iStateOfHealth;		//current state of health -1,0,1=uninit,bad,good
	int		m_iTop;
	CFileMgr* m_pFileMgr;			//file manager
	CRect	m_sInitialRect;			//physical size of main dialog box
	UINT	m_uiHours;				//hours between SOH_TIME_EXT ??
	bool	m_bDoOnce; 
	char	m_szDailyAt[32];
	COleDateTimeSpan	m_BeatSeconds;
	UINT    m_uiDoingNetworkToThatBrandNewBeat;	  //interval to send network heartbeat update to listeners (like tracker),  > 0, in seconds 

	//
	// functions (in alphabetical order)
	//
	bool AddColorButton(bool bParticipateInNet, char *name, ICB **pInterface_for_ISO, CColorButton **pbutton_for_MICDlg);
	void AddComm(char *name, bool bNewEntry);
	void AddInstrument(char *name, bool bNewEntry);
	void DoSOHCmd(char* szFileName);
	bool GetInstrumentFactory(char *name,IClassFactory **factory);
	void InitializeComponentRelatedMemberVariables();
	void NetworkHeartbeat();
	void PrintButtonStatus(
		FILE* outfile, 
		CString cNameText, 
		CString cSecondText, 
		char* szType, 
		COLORREF cColor,
		int iState);

	void PrintINISection(char *szSection, FILE*outfile);
	void ReorgBySuperuser();
	void ReorgDialogLayout();
	void SetNetworkHeartbeatTimer(bool enable);
	void SetUpRegisteredMessageValues();
	void ShowStartupErrors();
	void ShutDown();
	void ShutDownFinal();
	void ShutDownWork();
	void StartData(bool bStart);
	void OnDT0Pause();
	CString** EnumerateSerialPorts(); // an array of CString*

#ifdef BMPBACKGRND
	CBitmap m_bmapBrush;
	CBrush m_brush;
#endif


// Dialog Data
	//{{AFX_DATA(CMicMgrDlg)
	enum { IDD = IDD_MIC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMicMgrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	HICON m_hIcon32;			//our icon 32x32
	HICON m_hIcon16;			//our icon 16x16
	HICON m_hIcon32b;
	HICON m_hIcon16b;

	// Generated message map functions
	//{{AFX_MSG(CMicMgrDlg)
	afx_msg void OnAccessControlMenuItem();
	afx_msg LRESULT OnAddCommMsg(WPARAM, LPARAM);
	afx_msg LRESULT OnAddInstMsg(WPARAM, LPARAM);
	afx_msg void OnArchiveCheck();
	afx_msg void OnClose();
	afx_msg void OnConfigurationMenuItem();
	afx_msg void OnCopydataButton();
	afx_msg void OnEnumPorts();
	afx_msg LRESULT OnCopyDone(WPARAM,LPARAM);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnDelCommMsg(WPARAM, LPARAM);
	afx_msg LRESULT OnDelInstMsg(WPARAM, LPARAM);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg LRESULT OnForcePause(WPARAM, LPARAM);
	afx_msg void OnInstrumentButton(UINT nID);
	afx_msg void OnInstrumentButtonDbl(UINT nID);
	afx_msg LRESULT OnMICXferCmd(WPARAM wParam, LPARAM lParam);//<<QA>> candidate for deletion
	afx_msg void OnMove(int x, int y);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnQueryEndSession();
	afx_msg LRESULT OnSOHCmd(WPARAM wParam, LPARAM lParam);
	afx_msg void OnStartdataButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg LRESULT OnISOPauseStatusChange(WPARAM, LPARAM);
	afx_msg LRESULT OnDriveAllToZero(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQueryAllPaused(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnValidationRequest(WPARAM wsuperuser, LPARAM lParam);
	//}}AFX_MSG
#ifdef ALLOWTRIGGERHOLDOFF
	afx_msg void OnTrigger(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTriggerClr(WPARAM,LPARAM);
#endif
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MICMGRDLG_H__AA6F9DB8_3936_4BC8_8F80_51A0FF710FF0__INCLUDED_)
