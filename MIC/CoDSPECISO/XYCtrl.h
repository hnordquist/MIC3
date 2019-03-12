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

#ifndef XYCTRL_H
#define XYCTRL_H

// XYCtrl.h

class CXYCtrl : public CWnd
{
// Construction
public:
	CXYCtrl(UINT uMaxChannels = 16384);

// Attributes
public:
	COLORREF m_colorDefault;
	COLORREF m_colorLine;
	COLORREF m_crPixelColor;

	UINT m_uNumChannels, m_uMaxCountPerChannel;
	bool m_bRecalculateConstants;

	double m_d_x_axis_length_in_pixels;
	double m_d_y_axis_length_in_pixels;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXYCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTitle(const CString& str);
	void SetLeftLabel(const CString &str);
	void SetBottomLabel(const CString &str);
	void SetNumChannels(UINT u);
	CString m_strTitle, m_strLeftLabel, m_strBottomLabel;
	void GetItemColor(int i, COLORREF& color);
	void Reset();
	//void CalculatePoint(UINT ux, UINT uy, CPoint& pt);
	void CalculatePoint(CPoint& pt_in, CPoint& pt_out);
	BOOL AddPoint(UINT uX, UINT uY);

	void DrawThresholdLine(COLORREF cThresholdLineColor, UINT uValue);
	int m_iThresholdLineOffsetFromTopInPixels;
	COLORREF m_rgbThresholdLineColor;

#define MAX_NUM_GRID_LINES (20)
	void DrawHorizontalGridLines(COLORREF color);
	COLORREF m_rgbHorizonatlGridLinesColor;
	int m_iHorizontalGridLinesOffsetFromTopInPixels[MAX_NUM_GRID_LINES];

#define MAX_POINTS 16384
	CPoint m_point[MAX_POINTS];
	int m_index;

	double m_dxmax, m_dymax;

	void RecalcRect();
	CRect m_ClientRect;
	CRect m_PlotRect;

	static BOOL RegisterWndClass(HINSTANCE hInstance);
	virtual ~CXYCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXYCtrl)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // XYCTRL_H
