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
#if !defined(AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_)
#define AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataWatchDlg.h : header file
//
#include "chart.h"
#include "JulianT.h"
#include "resource.h"

class CSafePropertySheet;
/////////////////////////////////////////////////////////////////////////////
// CDataWatchDlg dialog

class CDataWatchDlg : public CDialog
{
// Construction
friend class CSafePropertySheet;

public:
	CDataWatchDlg(CWnd* pParent = NULL);   // standard constructor
	~CDataWatchDlg();
	void SetDlgTitle(CString cTitle, CSafePropertySheet* pParent);
	void SetGraphTitles(CString cs1, CString cs2, CString cs3, CString cs4, CString cs5);
	void SetData(unsigned int uiTime, float f1, float f2, float f3, float f4, float f5, bool bDraw);

// Dialog Data
	//{{AFX_DATA(CDataWatchDlg)
	enum { IDD = IDD_DATAWATCH_DIALOG };
	CButton	m_cButtonExpand;
	CButton	m_cButtonContract;
	CButton	m_cLocked;
	CStatic	m_cStaticStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataWatchDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_cTitle;
	CString m_cs1, m_cs2, m_cs3, m_cs4, m_cs5;
	HICON m_hIcon;
	CSafePropertySheet* m_pParent;
	void ResizeChildren();
	bool m_bConfig;
	UINT m_uiLastTime;
	UINT m_uiLineIndex;
	bool m_bNewLine;
	CRect m_WindowRect;


	UINT m_uBaseTime;
	UINT m_uiCurrentHour;
	CFont m_VertFont;
//	ITEM_COLOR m_cRGBbk;
//	ITEM_COLOR m_cRGBfg;
	UINT m_uiMultiplier;
	COleDateTime m_cLastReadTime;

	CString cLabel1;
	CString cLabel2;
	CString cLabel3;
	CString cLabel4;
	CString cLabel5;

	CScrollChart m_wnd1Chart; // was < float > gjk
	CScrollChart m_wnd2Chart;
	CScrollChart m_wnd3Chart;
	CScrollChart m_wnd4Chart;
	CScrollChart m_wnd5Chart;

	int m_iChart1x;
	int m_iChart2x;
	int m_iChart3x;
	int m_iChart4x;
	int m_iChart5x;

//	void SetStatusText();
	void OnNewV1Limits(WPARAM wParam, LPARAM lParam);
	void OnNewH1Limits(WPARAM wParam, LPARAM lParam);

	void OnNewV2Limits(WPARAM wParam, LPARAM lParam);
	void OnNewH2Limits(WPARAM wParam, LPARAM lParam);

	void OnNewV3Limits(WPARAM wParam, LPARAM lParam);
	void OnNewH3Limits(WPARAM wParam, LPARAM lParam);

	void OnNewV4Limits(WPARAM wParam, LPARAM lParam);
	void OnNewH4Limits(WPARAM wParam, LPARAM lParam);

	void OnNewV5Limits(WPARAM wParam, LPARAM lParam);
	void OnNewH5Limits(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CDataWatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnScrollH1Msg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnScrollH2Msg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnScrollH3Msg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnScrollH4Msg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnScrollH5Msg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData1FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData2FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData3FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData4FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData5FPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData1LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData2LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData3LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData4LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnData5LPoint(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnCheckLocked();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonContract();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAWATCHDLG_H__E0C56560_F034_4831_925D_37911F828151__INCLUDED_)
