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
// file XYCtrl.cpp

#include "stdafx.h"
#include "XYCtrl.h"
#include "colors.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XYCtrl

CXYCtrl::CXYCtrl(UINT uNumChannels)
{
	//TRACE("CXYCtrl::CTR\n");

	CXYCtrl::RegisterWndClass(AfxGetInstanceHandle());

	m_colorLine = RGB(0,0,0);
	m_colorDefault = RGB(0,0,255);

	m_bRecalculateConstants = true;
	
	m_strTitle       = _T("Spectrum Data");
	m_strLeftLabel   = _T("HITS");
	m_strBottomLabel = _T("CHANNELS");
	m_crPixelColor   = COLOR_CYAN;//COLOR_RED;
	m_uNumChannels   = uNumChannels;
	m_uMaxCountPerChannel = 0xFFFFFFFF;  //DSPEC channels are 32-bits each

	m_dxmax = (double)m_uNumChannels;
	m_dymax = log10((double)m_uMaxCountPerChannel);

	m_iThresholdLineOffsetFromTopInPixels = -1;

	int i;

	for (i = 0; i < MAX_NUM_GRID_LINES; i++)
		m_iHorizontalGridLinesOffsetFromTopInPixels[i] = -1;

	for (i = 0; i < MAX_POINTS; i++)
	{
		m_point[i].x = 0;
		m_point[i].y = 0;
	}

	m_index = 0;
}

CXYCtrl::~CXYCtrl()
{
//	TRACE("CXYCtrl::DTR\n");
	Reset();
}


BEGIN_MESSAGE_MAP(CXYCtrl, CWnd)
	//{{AFX_MSG_MAP(CXYCtrl)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	ON_WM_LBUTTONUP()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_MOUSEMOVE()

/////////////////////////////////////////////////////////////////////////////
// XYCtrl message handlers

BOOL CXYCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	//TRACE("CXYCtrl::RegisterWndClass\n");
	WNDCLASS wc;
	wc.lpszClassName = "XY_CTRL"; // matches class name in client
	wc.hInstance = hInstance;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hCursor = ::LoadCursor(NULL, IDC_CROSS);
	wc.hIcon = 0;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	wc.style = CS_BYTEALIGNWINDOW|CS_GLOBALCLASS; // To be modified
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	return (::RegisterClass(&wc) != 0);
}

void CXYCtrl::OnSize(UINT nType, int cx, int cy) 
{
	//TRACE("CXYCtrl::OnSize\n");
	CWnd::OnSize(nType, cx, cy);
	RecalcRect();
}

void CXYCtrl::RecalcRect()
{
	//TRACE("CXYCtrl::RecalcRect\n");

	CClientDC dc(this);
	CSize sz(0,0);

	GetClientRect(&m_ClientRect);

	if (!m_strTitle.IsEmpty())
		sz = dc.GetTextExtent(m_strTitle);
	//
	// determine the plot area rectangle dimensions
	//
	m_PlotRect = m_ClientRect;	       //Start out whole thing
	m_PlotRect.top += sz.cy;           //reduce by height of title

	sz = dc.GetTextExtent(m_strLeftLabel);//"HITS");        //reduce by width of left label
	m_PlotRect.DeflateRect(sz.cx,sz.cy);  //Same height as "CHANNELS"


	m_d_x_axis_length_in_pixels = (double)m_PlotRect.Width();
	m_d_y_axis_length_in_pixels = (double)m_PlotRect.Height();
}


BOOL CXYCtrl::AddPoint(UINT uX, UINT uY)
{
	//TRACE("CXYCtrl::AddPoint\n");

	if (m_index >= MAX_POINTS)
		return FALSE;

	m_point[m_index].x = (int)uX;
	m_point[m_index].y = (int)uY;
	m_index++;

	return TRUE;
}

void CXYCtrl::CalculatePoint(CPoint& pt_in, 
							 //UINT ux, UINT uy, 
							 CPoint& pt_out)
{
	//TRACE("CXYCtrl::CalculatePoint\n");

	//if (uy == 0)
	if (pt_in.y == 0)
	{
		pt_out.x = m_PlotRect.left;
		pt_out.y = m_PlotRect.bottom;
	}
	else
	{
		if (pt_in.x > (int)m_uNumChannels)
			pt_in.x = (int)m_uNumChannels;

		double dxmax = (double)m_uNumChannels;
		double dxinput = (double)pt_in.x;

		double dymax = log10((double)m_uMaxCountPerChannel);
		double dyinput = log10((double)pt_in.y);

		double dxpixels = (dxinput/dxmax) * m_d_x_axis_length_in_pixels;
		double dypixels = (dyinput/dymax) * m_d_y_axis_length_in_pixels;

		pt_out.x = m_PlotRect.left + (int)(dxpixels + 0.9);
		pt_out.y = m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));
	}
}


void CXYCtrl::Reset()
{
	//TRACE("CXYCtrl::Reset());

	for (UINT u = 0; u < m_uNumChannels; u++)
	{
		m_point[u].x = 0;
		m_point[u].y = 0;
	}

	m_index = 0;

	if (::IsWindow(m_hWnd))  //fails when DTR calls Reset()
		InvalidateRect(NULL, FALSE);
}

void CXYCtrl::SetTitle(const CString & str)
{
	//TRACE("CXYCtrl::SetTitle() = %s\n", str);
	m_strTitle = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CXYCtrl::SetLeftLabel(const CString &str)
{
	//TRACE("CXYCtrl::SetLeftLabel() = %s\n", str);
	m_strLeftLabel = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CXYCtrl::SetBottomLabel(const CString &str)
{
	//TRACE("CXYCtrl::SetBottomLabel() = %s\n", str);
	m_strBottomLabel = str;
	RecalcRect();
	InvalidateRect(NULL, FALSE);
}

void CXYCtrl::DrawThresholdLine(COLORREF cThresholdLineColor, UINT uValue)
{
	m_rgbThresholdLineColor = cThresholdLineColor;

	if (uValue == 0)
		uValue = 1;

	//double m_dymax = log10((double)m_uMaxCountPerChannel);
	double dyinput = log10((double)uValue);

	double dypixels = (dyinput/m_dymax) * m_d_y_axis_length_in_pixels;

	m_iThresholdLineOffsetFromTopInPixels = 
		m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));
}

void CXYCtrl::DrawHorizontalGridLines(COLORREF color)
{
	m_rgbHorizonatlGridLinesColor = color;
	double dyvalue, dypixels;
	double dValue = 10.;
	double dMaxCountPerChannel = (double)m_uMaxCountPerChannel;
	//double dymax = log10(dMaxCountPerChannel);

	for (int j = 0; j < MAX_NUM_GRID_LINES; j++)
	{
		dyvalue = log10(dValue);
		dypixels = (dyvalue/m_dymax) * m_d_y_axis_length_in_pixels;

		m_iHorizontalGridLinesOffsetFromTopInPixels[j] = 
			m_PlotRect.top + (int)((m_d_y_axis_length_in_pixels - dypixels));

		dValue *= 10.;
		if (dValue > dMaxCountPerChannel)
			break;
	}	
}
void CXYCtrl::SetNumChannels(UINT u)
{
//	TRACE("CXYCtrl::SetNumChannels(u = %u)\n", u);

	if (m_uNumChannels != u)
		Reset();

	m_uNumChannels = u;

	m_dxmax = (double)m_uNumChannels;
}

void CXYCtrl::OnPaint() 
{
	CPoint   cTextExtent, cpoint;
	CPaintDC dc(this);
	CDC      memoryDC;
	CBitmap  bitmap;
	CPen     pen, *oldpen;
	CBrush   defaultbrush, whitebrush;

	if (m_bRecalculateConstants)
	{
		RecalcRect();
		// <<QA>> could tuck some other 1-time calculations in here
		m_bRecalculateConstants = false;
	}

	//Create memory device context compatible with a painting device context
	memoryDC.CreateCompatibleDC(&dc);

	//Create bitmap that will correspond to the clientRect and select it into memory DC
	bitmap.CreateCompatibleBitmap( &dc, m_ClientRect.Width(), m_ClientRect.Height() );
	memoryDC.SelectObject(&bitmap);

	memoryDC.FillSolidRect(m_ClientRect, /*COLOR_GREEN*/GetSysColor(COLOR_3DFACE));

	memoryDC.SelectStockObject(NULL_PEN);

	defaultbrush.CreateSolidBrush(m_colorDefault);
	memoryDC.SelectObject(&defaultbrush);

	whitebrush.CreateSolidBrush(RGB(255,255,255));  //white
	memoryDC.SelectObject(&whitebrush);
	memoryDC.Rectangle(m_PlotRect);

	pen.CreatePen(PS_SOLID, 1, m_colorLine);
	memoryDC.SelectObject(&pen);
	memoryDC.SelectStockObject(NULL_BRUSH);
	memoryDC.Rectangle(m_PlotRect);

	//draw the lines of data
	CPen linepen;
	linepen.CreatePen(PS_SOLID, 1, COLOR_BLUE);//COLOR_RED);
	CPen *savepen = memoryDC.SelectObject(&linepen);

	for (UINT u = 0; u < m_uNumChannels; u++)
	{
		//CalculatePoint(m_point[i].x, m_point[i].y, cpoint);
		CalculatePoint(m_point[u], cpoint);

		cpoint.y -= 2;  //3;//5;
		memoryDC.MoveTo(cpoint.x, cpoint.y);
		memoryDC.LineTo(cpoint.x, m_PlotRect.bottom);
	}
	memoryDC.SelectObject(savepen);

	//Draw threshold line
	if (m_iThresholdLineOffsetFromTopInPixels != -1)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 3, m_rgbThresholdLineColor);
		CPen *oldpen = memoryDC.SelectObject(&pen);
		memoryDC.MoveTo(m_PlotRect.left,m_iThresholdLineOffsetFromTopInPixels);
		memoryDC.LineTo(m_PlotRect.right,m_iThresholdLineOffsetFromTopInPixels);
		memoryDC.SelectObject(oldpen);
	}

	//Draw Horizontal Grid Lines
	CPen apen;
	apen.CreatePen(PS_DASH, 1, m_rgbHorizonatlGridLinesColor);
	oldpen = memoryDC.SelectObject(&apen);
	for (int j = 0; j < MAX_NUM_GRID_LINES; j++)
	{
		if (m_iHorizontalGridLinesOffsetFromTopInPixels[j] == -1)
			break;

		memoryDC.MoveTo(m_PlotRect.left,m_iHorizontalGridLinesOffsetFromTopInPixels[j]);
		memoryDC.LineTo(m_PlotRect.right,m_iHorizontalGridLinesOffsetFromTopInPixels[j]);
	}
	memoryDC.SelectObject(oldpen);

	//Draw left and bottom labels
	CPoint centerPoint(m_PlotRect.CenterPoint());
	cTextExtent = memoryDC.GetTextExtent(m_strBottomLabel);                              //"CHANNEL");
	memoryDC.TextOut(centerPoint.x-cTextExtent.x/2,m_PlotRect.bottom,m_strBottomLabel);  //"CHANNEL");
	cTextExtent = memoryDC.GetTextExtent(m_strLeftLabel);                                //("HITS");
	memoryDC.TextOut(m_PlotRect.left-cTextExtent.x,centerPoint.y-cTextExtent.y/2,m_strLeftLabel);//"HITS");

	//Draw Title
	if (!m_strTitle.IsEmpty())
	{
		memoryDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		memoryDC.SetBkMode(TRANSPARENT);
		memoryDC.DrawText(m_strTitle,CRect(m_ClientRect.left,1,m_ClientRect.right,m_ClientRect.bottom),DT_CENTER);
	}
	dc.BitBlt( 0, 0, m_ClientRect.Width(), m_ClientRect.Height(), &memoryDC, 0, 0, SRCCOPY );
}