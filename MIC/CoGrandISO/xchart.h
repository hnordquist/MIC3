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
///////////////////////////////////////////////////////////////////////
//chart.h, base written by Juan Carlos Sanchez 
//repaired by David Pelowitz
///////////////////////////////////////////////////////////////////////

#if !defined(GRAF_H_10101010)
#define GRAF_H_10101010

#include "resource.h"
#include "JulianT.h"

#define CTLOFFSET 0
#define SCROLL_VALUE 1000.0 

//#define WM_NEWHORZLIMITS (CTLOFFSET + WM_USER + 700)
//#define WM_NEWVERTLIMITS (CTLOFFSET + WM_USER + 800)

#define GS_NOTIFY	0x00000001L
#define LS_HORZAUTO 0x00010000
#define LS_POINT	0x00020000

#define TIMER_TIME		2000
#define TIMER_EVENT		9876

#ifdef _AFXDLL

#define BEGIN_MESSAGE_MAP_TEMPLATES(thetemplate, theClass, baseClass) \
	thetemplate const AFX_MSGMAP* PASCAL theClass::_GetBaseMessageMap() \
		{ return &baseClass::messageMap; } \
	thetemplate const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	thetemplate AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
	{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
	thetemplate AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

#else

#define BEGIN_MESSAGE_MAP_TEMPLATES(thetemplate, theClass, baseClass) \
	thetemplate const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return &theClass::messageMap; } \
	thetemplate AFX_COMDAT AFX_DATADEF const AFX_MSGMAP theClass::messageMap = \
	{ &baseClass::messageMap, &theClass::_messageEntries[0] }; \
	thetemplate AFX_COMDAT const AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
	{ \

#endif //_AFXDLL

#pragma warning (disable: 4786 4788)

#include <afxwin.h>
#include <math.h>
#include <map>
#include <set>

using namespace std;

template<class T >
class CScrollChart: public CWnd
{

protected:
	
typedef map<T, T> MAPDATA;

	typedef struct  {
		MAPDATA m_mapData;
		COLORREF crColor;
		int nStyle;
		int nWidth;
		BOOL bIsVisible;
		BOOL bIsAuto;
		BOOL bPoints;
		T nAutoHorzInc;
	} stuctLine;

	typedef map<UINT, stuctLine> MAPLINE;

	T m_MaxValue;
	T m_MinValue;
	T m_pointIni;
	T m_MaxHzVal;
	T m_MinHzVal;
	T m_MinHVal;
	T m_nHzRange;

	MAPLINE m_mapLines;
	COLORREF m_ColorBk;
	COLORREF m_ColorBkLocked;
	COLORREF m_BkColor;
	BOOL m_bGrid;
	BOOL m_bNotify;
//	BOOL m_bLocked;
	UINT m_nHorzDiv;
	UINT m_nVertDiv;
	UINT m_nMaxPoints;	
	UINT m_uH;
	UINT m_uV;
	UINT m_uFP;
	UINT m_uLP;
	set<UINT> m_setLinesID;

public:
	CScrollChart(): CWnd(),  m_pointIni(0,0),   m_nHorzDiv(60), m_nVertDiv(14), m_nMaxPoints(1440),
		m_mapLines(), m_setLinesID(), m_MaxHzVal(3600), m_nHzRange(3600), m_MinHzVal(0)  {}

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, UINT uH, UINT uV, UINT uFP, UINT uLP);

	BOOL AddLine(UINT nID, COLORREF crColor, DWORD nStyle = PS_SOLID, 
					int nWidth = 1,BOOL bVisible = TRUE, int nAutoInc = 10);

	BOOL AddData(UINT nID, T nX, T nY, BOOL bDraw = TRUE);
	BOOL AddData(UINT nID, T nY, BOOL bDraw = TRUE);
	COLORREF SetBkColor(COLORREF cr );
	COLORREF GetBkColor() const;
	COLORREF SetBkColorLocked(COLORREF cr );
	COLORREF GetBkColorLocked() const;
	COLORREF SetLineColor(UINT nID, COLORREF cr );
	COLORREF GetLineColor(UINT nID);
	void ShowLine(UINT nID, BOOL value = TRUE, BOOL bDraw = TRUE); 
	void ShowPoints(UINT nID, BOOL value = TRUE, BOOL bDraw = TRUE); 
	void SetGrid(BOOL value = TRUE);
	T SetHorzRange(T nRange);
	T GetHorzRange() {return m_nHzRange;}
	T SetAutoinc(UINT nID, T nInc);
	UINT SetHorzDiv(UINT nDiv, BOOL bDraw = TRUE);
	UINT GetHorzDiv() const {return m_nHorzDiv;}
	UINT SetVertDiv(UINT nDiv, BOOL bDraw = TRUE);
	UINT GetVertDiv() const {return m_nVertDiv;}
	UINT SetMaxPoints(UINT nValue);
	UINT GetMaxPoints() const { return m_nMaxPoints;}
	void GetVertMinMax(T* MinValue, T*MaxValue) {*MinValue = m_MinValue; *MaxValue = m_MaxValue;}
	int SetLineStyle(UINT nID, int nStile, BOOL bDraw = TRUE);
	int GetLineStyle(UINT nID);
	int SetLineWidth(UINT nID, int nWidth, BOOL bDraw = TRUE);
	int GetLineWidth(UINT nID);
	void SetMinValue(UINT nID, T MinValue);
	void SetBaseTime(UINT uBaseTime){m_uBaseTime = uBaseTime;}
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	void CreateSafeTooltipRect(int x, int y, LPCTSTR lpszText);
	
protected:
	afx_msg void OnPaint();
	unsigned m_uBaseTime;
	HCURSOR m_hCursor;
	CRect	tooltipRect;
	CRect	m_rcClient;
	CPoint LPtoDP(T X, T Y);
	CPoint LPtoDP(pair<T, T> pnt) {return LPtoDP(pnt.first, pnt.second);}
	T GetY(pair<T, T> pnt) {return pnt.second;} 
	T GetX(pair<T, T> pnt) {return pnt.first;}
	BOOL bIsFirstPoint();	
	void UpdateScrollInfo();
	void SendNewLimits();
	void Normalize();
	BOOL ErasePoints();
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP_TEMPLATES(template<class T>, CScrollChart<T> , CWnd)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

template<class T> 
void CScrollChart<T>::OnPaint( )
{
	
	CRect RectClient;
	CPen *penLine;
	CPen *penLineHour;
	CPen *oldPen;

	CPaintDC lop(this);
	GetClientRect(RectClient);
	CDC PaintDc;
	CBitmap bitm;
	PaintDc.CreateCompatibleDC(&lop);

	CBitmap* pOldBitmap;
	bitm.CreateCompatibleBitmap(&lop, RectClient.Width(), RectClient.Height());
	pOldBitmap = PaintDc.SelectObject (&bitm);

	PaintDc.SetBkMode(OPAQUE);
	PaintDc.SetBkColor(m_BkColor);
	PaintDc.SetViewportOrg(0, 0);

	CBrush bursh(m_BkColor);
	PaintDc.FillRect(RectClient, &bursh);	

	//set vertical scale 
	T tMax;
	int izzy = 1;
	if (!m_mapLines.empty())
	{
		MAPLINE::const_iterator iterLine = m_mapLines.begin();
		while ((iterLine) != m_mapLines.end())
		{
			if (!(*iterLine).second.m_mapData.empty())
			{
				if ((*iterLine).second.bIsVisible)
				{
					MAPDATA::const_iterator iter  = (*iterLine).second.m_mapData.lower_bound(m_pointIni);
					if (iter != (*iterLine).second.m_mapData.begin()) --iter;
					
					tMax = GetY(*iter);

					iter++;
					while ( (iter != (*iterLine).second.m_mapData.upper_bound(m_nHzRange + m_pointIni))
							&& (iter != (*iterLine).second.m_mapData.end()) )
					{
						tMax = max(GetY(*iter),tMax);
						++iter;
					}
				}
			}
			iterLine++;
		}

		double dx = log10(tMax);	//log of tmax
		double dy = dx - int(dx);	//dy is the "right of the decimal"
		int iz = int(pow(10,dy));	//iz is now the real value with no exponent
		int iy = int(dx);			//iy is the exponent (base 10)

		//scale the real value to the next higher of 1,2,and 5
		if (iz < 1) iz = 1;			
		else if (iz < 2) iz = 2;
		else if (iz < 5) iz = 5;
		else iz = 10;
		
		izzy = iz;
		//iz should now be the real part but not scaled
		iz = iz * pow(10,iy);
		
		if (iz == 0)
			iz = 1;

		if (iz != m_MaxValue)
		{
			m_MaxValue = iz;
			SendNewLimits();
		}

	} // not empty

    //do the grid
	if (m_bGrid)
	{

		penLine = new CPen(PS_SOLID, 1, RGB(0, 128, 0));
		penLineHour = new CPen(PS_SOLID, 3, RGB(0,128,0));

		oldPen = PaintDc.SelectObject(penLine);
		if (m_nVertDiv > 0)//horizontal lines
		{
			int iHeight = RectClient.Height();
			float deltaN = (float)iHeight / 10;
			for (int i = 1; i <= 10; i++)
			{
				int n = i * deltaN;
				PaintDc.MoveTo(0, n);
				PaintDc.LineTo(RectClient.Width(), n);
			}
		}

		if (m_nHorzDiv > 0)//vertical lines
		{
			for (float n = m_pointIni + ((m_nHzRange / m_nHorzDiv) - fmod(m_pointIni,m_nHzRange));
					n < m_pointIni + m_nHzRange; 
					n += (m_nHzRange / m_nHorzDiv))
			{
				//draw the "time" markers
				if ((int(n) % 3600) == 0)
				{
					PaintDc.SelectObject(penLineHour);
					int nInipoint=LPtoDP(int(n), 0).x;
					PaintDc.MoveTo(nInipoint, 0);
					PaintDc.LineTo(nInipoint, RectClient.Height()*.75);
					PaintDc.SelectObject(penLine);
				}
				else if ((int(n) % 1800) == 0) 
				{
					if (m_nHzRange < 14400*16)
					{
						PaintDc.SelectObject(penLineHour);
						int nInipoint=LPtoDP(int(n), 0).x;
						PaintDc.MoveTo(nInipoint, 0);
						PaintDc.LineTo(nInipoint, RectClient.Height()*.55);
						PaintDc.SelectObject(penLine);
					}
				}
				else if ((int(n) % 600) == 0)
				{
					if (m_nHzRange < 14400*8)
					{
						PaintDc.SelectObject(penLineHour);
						int nInipoint=LPtoDP(int(n), 0).x;
						PaintDc.MoveTo(nInipoint, 0);
						PaintDc.LineTo(nInipoint, RectClient.Height()*.35);
						PaintDc.SelectObject(penLine);
					}
				}
				else if ((int(n) % 300) == 0)
				{
					if (m_nHzRange < 14400*4)
					{
						PaintDc.SelectObject(penLineHour);
						int nInipoint=LPtoDP(int(n), 0).x;
						PaintDc.MoveTo(nInipoint, 0);
						PaintDc.LineTo(nInipoint, RectClient.Height()*.15);
						PaintDc.SelectObject(penLine);
					}
				}
				
				//draw the virtical grid lines
				if (m_nHzRange < 14400)
				{
					int nInipoint=LPtoDP(int(n), 0).x;
					PaintDc.MoveTo(nInipoint, 0);
					PaintDc.LineTo(nInipoint, RectClient.Height());
				}
			}		
		}
		PaintDc.SelectObject(oldPen);
		delete(penLine);
		delete(penLineHour);

	}  //Grid

	//do the data
	if (!m_mapLines.empty())
	{
		MAPLINE::const_iterator iterLine = m_mapLines.begin();
		T tFPoint1 = NULL;
		T tFPoint2 = NULL;
		T tLPoint1 = NULL;
		T tLPoint2 = NULL;
		while ((iterLine) != m_mapLines.end())
		{

			if (!(*iterLine).second.m_mapData.empty())
			{

				if ((*iterLine).second.bIsVisible)
				{
					CPoint pointDraw;
					CPoint pointOld;
					CSize sizePoint( 2+((*iterLine).second.nWidth),
									2+((*iterLine).second.nWidth) );
					CBrush brushPoint;
					brushPoint.CreateSolidBrush((*iterLine).second.crColor);
					penLine = new CPen((*iterLine).second.nStyle, 
									(*iterLine).second.nWidth, 
									(*iterLine).second.crColor);
					oldPen = PaintDc.SelectObject(penLine);
					CBrush* oldBrush = PaintDc.SelectObject(&brushPoint);
					MAPDATA::const_iterator iter = (*iterLine).second.m_mapData.lower_bound(m_pointIni);
					if (iter != (*iterLine).second.m_mapData.begin())
					{
						--iter;
					} 
					else
					{
						if (iterLine == m_mapLines.begin())
						{
							pair<T, T> pnt = *iter;	
							tLPoint1 = pnt.first;
							tLPoint2 = pnt.second;
						}
					}

					pointDraw = LPtoDP((*iter));
					if (pointDraw.y < 0) 
						pointDraw.y = 1;
					PaintDc.MoveTo(pointDraw);
					pointOld = pointDraw;
					iter++;
					while ( (iter != (*iterLine).second.m_mapData.upper_bound(m_nHzRange + m_pointIni))
							&& (iter != (*iterLine).second.m_mapData.end()) )
					{
						if (iterLine == m_mapLines.begin())
						{
							pair<T, T> pnt = *iter;	
							tFPoint1 = pnt.first;
							tFPoint2 = pnt.second;
							if (tLPoint1 == NULL)
							{
								tLPoint1 = pnt.first;
								tLPoint2 = pnt.second;
							}
						}
						pointDraw = LPtoDP((*iter));
						if (pointDraw.y < 0)
							pointDraw.y = 1;
						PaintDc.LineTo(pointDraw);
						if ((*iterLine).second.bPoints) 
							PaintDc.Ellipse(CRect( pointOld - sizePoint,
													pointOld + sizePoint) );
						pointOld = pointDraw;
						++iter;
					}
					if ((*iterLine).second.bPoints) 
						PaintDc.Ellipse(CRect( pointOld - sizePoint,
											pointOld + sizePoint) );
					if (iter != (*iterLine).second.m_mapData.end()) 
						{
							pointOld=LPtoDP(*iter);
							PaintDc.LineTo(pointOld);
							if ((*iterLine).second.bPoints) 
								PaintDc.Ellipse( CRect( pointOld - sizePoint,
												pointOld + sizePoint) );
						}
					PaintDc.SelectObject(oldPen);
					PaintDc.SelectObject(oldBrush);
					delete(penLine);
				}
			}
			iterLine++;
		}
		//post message with last point
		GetParent()->PostMessage (m_uFP, 
			(T) tLPoint1,
			(T) tLPoint2);
		GetParent()->PostMessage (m_uLP,
			(T) tFPoint1,
			(T) tFPoint2);
		
	} // not empty
	lop.BitBlt(0,0,RectClient.Width(), RectClient.Height(), &PaintDc, 0, 0, SRCCOPY);
	PaintDc.SelectObject(pOldBitmap);
}

template<class T> 
BOOL CScrollChart<T>::AddData(UINT nID, T nY, BOOL bDraw)
{
	if ( (m_setLinesID.find(nID) == m_setLinesID.end()) || (!(m_mapLines[nID].bIsAuto)) )
		return FALSE;
	if (m_mapLines[nID].m_mapData.empty())
		return AddData(nID, 0, nY, bDraw);
	return AddData(nID, 
		(*(--m_mapLines[nID].m_mapData.end())).first + m_mapLines[nID].nAutoHorzInc,
		nY, bDraw);
}
	
template<class T> 
BOOL CScrollChart<T>::AddData(UINT nID, T nX, T nY, BOOL bDraw)
{
	double dx = log10(nY);
	double dy = dx - int(dx);
	int iz = int(pow(10,dy)+0.5);
	int iy = int(dx);

	if (iz < 1) iz = 1;
	else if (iz < 2) iz = 2;
	else if (iz < 5) iz = 5;
	else iz = 10;

	iz = iz * pow(10,iy);

	if (m_setLinesID.find(nID) == m_setLinesID.end())
		return FALSE;

	BOOL bScrollModi = FALSE;
	if (bIsFirstPoint())
	{
		m_MaxValue = iz;
		if (m_MaxValue == 0)
			m_MaxValue = 1;
		m_MinValue = 0;

		SendNewLimits();
	} 
	else 
	{
		if (!m_mapLines[nID].m_mapData.empty())
		{
			if ( ((*(--(m_mapLines[nID].m_mapData.end()))).first >= m_pointIni) &&
				 ((*(--(m_mapLines[nID].m_mapData.end()))).first <= m_pointIni + m_nHzRange) &&
//				 (nX > m_pointIni + m_nHzRange * .99) )
	(nX > m_pointIni + m_nHzRange) )
			{
			//Move init
//				m_pointIni = nX - (m_nHzRange * .99);
	m_pointIni = nX - m_nHzRange;
				bScrollModi = TRUE;
			}
		}
	
		if ( (!bScrollModi) && (nX > m_MaxHzVal) && (nX > m_nHzRange) )
		{
			//right-out
//			m_MaxHzVal = nX + m_nHzRange * .01;
	m_MaxHzVal = nX;// + m_nHzRange;
			bScrollModi = TRUE;
		}

		if ( (!bScrollModi) && (nX < m_MinHzVal) )
		{
			//left
			m_MinHzVal = nX;
			bScrollModi = TRUE; 
		}
/*
		if (nY > m_MaxValue)
		{
			m_MaxValue = iz;//nY;
			SendNewLimits();
		}
		if (nY < m_MinValue) 
		{
			m_MinValue = iz;//nY;
			SendNewLimits();
		}
*/
	} //else fisrt point	

	m_mapLines[nID].m_mapData[nX] = nY;

	if (ErasePoints()) bScrollModi = TRUE;

	if (bScrollModi) Normalize();

	if (bDraw) Invalidate();

	if (bScrollModi) UpdateScrollInfo();

	return TRUE;
}

template<class T>
void CScrollChart<T>::SetMinValue(UINT nID, T MinValue)
{
	if (m_setLinesID.find(nID)==NULL) return;
	//m_mapLines[nID].m_MinHzVal = MinValue;
	m_MinHVal = MinValue;
//	Invalidate();
}

template<class T> 
BOOL CScrollChart<T>::AddLine(UINT nID, COLORREF crColor, DWORD  nStyle, int nWidth, BOOL bVisible, int nAutoInc)
{
	if (!(m_setLinesID.insert(nID).second)) return FALSE;
	if (!((0x0000FFFF) & nStyle)) nStyle |= PS_SOLID;
	stuctLine structLineTmp;
	structLineTmp.bIsAuto = ((LS_HORZAUTO & nStyle)? TRUE : FALSE);
	structLineTmp.bPoints = ((LS_POINT & nStyle)? TRUE : FALSE);
	structLineTmp.nAutoHorzInc = nAutoInc;
	structLineTmp.crColor = crColor;
	structLineTmp.nStyle = ((0x0000FFFF) & nStyle);
	structLineTmp.nWidth = nWidth;
	structLineTmp.bIsVisible = bVisible;
	m_mapLines[nID] = structLineTmp;
	return TRUE;
}

template<class T> 
BOOL CScrollChart<T>::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, UINT uH, UINT uV, UINT uFP, UINT uLP)
{
	dwStyle |= WS_CHILD;
	dwStyle |= WS_HSCROLL;
	if (dwStyle & GS_NOTIFY) m_bNotify=TRUE; else m_bNotify=FALSE;
	dwStyle &= ~(GS_NOTIFY);
	BOOL nresult = CWnd::Create(NULL, NULL,dwStyle, rect, pParentWnd, nID, NULL);
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND);
	m_ColorBk = RGB(0,0,0);
	m_ColorBkLocked = RGB(255,255,255);
	m_BkColor = m_ColorBk;

	m_bGrid = TRUE;
	m_MinValue = 0;	
	//m_MaxValue = m_nHzRange;
	m_MaxHzVal = m_nHzRange;
	m_MaxValue = 0;
	m_nHorzDiv = 60;
	m_nVertDiv = 10;
	m_uBaseTime = 0;
	UpdateScrollInfo();
	m_uH = uH;
	m_uV = uV;
	m_uFP = uFP;
	m_uLP = uLP;
//	GetClientRect(m_rcClient);
	return nresult;
}

template<class T> 
void CScrollChart<T>::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	if ((void*)pScrollBar != (void*)-1)
	{
		GetParent()->PostMessage (m_uH+2,	nSBCode, nPos);
	}

	float nIntPos;
	switch (nSBCode) {

	case SB_LINELEFT:
		if (m_pointIni < 1) return;
		m_pointIni -= (m_nHzRange * .01);
		break;

	case SB_LINERIGHT:
		m_pointIni += (m_nHzRange * .01);
		break;

	case SB_PAGEUP:
		m_pointIni -= m_nHzRange / 2;
		if ( m_pointIni < 0 ) m_pointIni = 0;
		break;

	case SB_PAGERIGHT:
		m_pointIni += m_nHzRange / 2;
		break;

	case SB_THUMBTRACK:
		nIntPos=( (int)nPos / (SCROLL_VALUE) );
		m_pointIni = ( (m_MaxHzVal - m_MinHzVal ) * nIntPos) + m_MinHzVal;
		break;

	default:
		return;
	}
	if (m_pointIni > m_MaxHzVal - m_nHzRange)
	{
		m_pointIni = m_MaxHzVal - m_nHzRange;
		m_BkColor = m_ColorBk;
	}
	else
	{
		m_BkColor = m_ColorBkLocked;
	}
	if ( m_pointIni < m_MinHzVal ) m_pointIni = m_MinHzVal;

	Invalidate();
	UpdateScrollInfo();
}

template<class T> 
COLORREF CScrollChart<T>::SetBkColor(COLORREF cr )
{
	COLORREF crOld = m_ColorBk;
	m_ColorBk = cr;
	Invalidate();
	return crOld;
}
template<class T> 
COLORREF CScrollChart<T>::SetBkColorLocked(COLORREF cr )
{
	COLORREF crOld = m_ColorBkLocked;
	m_ColorBkLocked = cr;
	Invalidate();
	return crOld;
}

template<class T> 
COLORREF CScrollChart<T>::GetBkColor() const
{
	return m_ColorBk;
}
template<class T> 
COLORREF CScrollChart<T>::GetBkColorLocked() const
{
	return m_ColorBkLocked;
}

template<class T> 
COLORREF CScrollChart<T>::SetLineColor(UINT nID, COLORREF cr )
{
	COLORREF crOld;
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return NULL;
	crOld = m_mapLines[nID].crColor;
	m_mapLines[nID].crColor = cr;
	Invalidate();
	return crOld;
}

template<class T> 
COLORREF CScrollChart<T>::GetLineColor(UINT nID) 
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return NULL;
	return m_mapLines[nID].crColor;
}

template<class T> 
int CScrollChart<T>::SetLineStyle(UINT nID, int nStyle,BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	int nResult = m_mapLines[nID].nStyle;
	m_mapLines[nID].nStyle = nStyle;
	if (bDraw) Invalidate();
	return nResult;
}

template<class T> 
int CScrollChart<T>::GetLineStyle(UINT nID)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	return m_mapLines[nID].nStyle;
}

template<class T> 
int CScrollChart<T>::SetLineWidth(UINT nID, int nWidth,BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	int nResult = m_mapLines[nID].nWidth;
	m_mapLines[nID].nWidth = nWidth;
	if (bDraw) Invalidate();
	return nResult;
}

template<class T> 
int CScrollChart<T>::GetLineWidth(UINT nID)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	return m_mapLines[nID].nWidth;
}

template<class T> 
T CScrollChart<T>::SetAutoinc(UINT nID,T nInc)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return 0;
	T nTmp = m_mapLines[nID].nAutoHorzInc;
	m_mapLines[nID].nAutoHorzInc = nInc;
	return nTmp;

}

template<class T> 
void CScrollChart<T>::ShowLine(UINT nID, BOOL value, BOOL bDraw)
{
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return;
	m_mapLines[nID].bIsVisible = value;
	if (bDraw) Invalidate();
}

template<class T> 
void CScrollChart<T>::ShowPoints(UINT nID, BOOL value, BOOL bDraw)
{	
	if (m_setLinesID.find(nID) == m_setLinesID.end()) return;
	m_mapLines[nID].bPoints = value;
	if (bDraw) Invalidate();
}

template<class T> 
UINT CScrollChart<T>::SetMaxPoints(UINT nValue)
{
	UINT nTemp = m_nMaxPoints;
	m_nMaxPoints = nValue;
	if (nTemp != nValue) 
	{
		
		ErasePoints();
		Invalidate();
	}
	return nTemp;
}

template<class T> 
void CScrollChart<T>::SetGrid(BOOL value)
{
	m_bGrid = value;
	Invalidate();
}

template<class T> 
UINT CScrollChart<T>::SetHorzDiv(UINT nDiv, BOOL bDraw)
{
	UINT nTmp = m_nHorzDiv;
	m_nHorzDiv = nDiv;
	if (bDraw) Invalidate();
	return nTmp;
}

template<class T> 
UINT CScrollChart<T>::SetVertDiv(UINT nDiv, BOOL bDraw)
{
	UINT nTmp = m_nVertDiv;
	m_nVertDiv = nDiv;
	if (bDraw) Invalidate();
	return nTmp;
}

template<class T> 
CPoint CScrollChart<T>::LPtoDP(T X,T Y)
{
	int nY;
	int nX;
	CRect rectClient;
	GetClientRect(rectClient);
//	nY = ((rectClient.Height() * Y * .95) / m_MaxValue);
	nY = ((rectClient.Height() * Y) / m_MaxValue) + 1.0;
	nX = ((rectClient.Width() * (X - m_pointIni)) / m_nHzRange);
	return CPoint(nX, rectClient.Height() - nY);
}

template<class T> 
T CScrollChart<T>::SetHorzRange(T nRange)
{
	T nTmp = m_nHzRange;
	m_nHzRange = nRange;
	if (m_MaxHzVal < m_nHzRange) m_MaxHzVal = m_nHzRange * 1.1;
	//Normalize();
	UpdateScrollInfo();
	return nTmp;
}

template<class T> 
BOOL CScrollChart<T>::bIsFirstPoint()
{
	MAPLINE::const_iterator iterLine = m_mapLines.begin();
	while ( (iterLine) != m_mapLines.end() )
	{
		if ( !(*iterLine).second.m_mapData.empty() ) 
			return FALSE;
		++iterLine;
	}
	return TRUE;
}

template<class T> 
void CScrollChart<T>::UpdateScrollInfo()
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nPos = SCROLL_VALUE * ( (m_pointIni - m_MinHzVal) / ( m_MaxHzVal - m_MinHzVal ) );
	si.nMin = 0;
	si.nMax = SCROLL_VALUE;
	si.nPage = SCROLL_VALUE * ( m_nHzRange / ( m_MaxHzVal - m_MinHzVal ) );	
	SetScrollInfo(SB_HORZ, &si, TRUE);
	SendNewLimits();
}

template<class T> 
BOOL CScrollChart<T>::ErasePoints()
{
	if (!m_nMaxPoints) return FALSE;
	BOOL bResult = FALSE;
	MAPLINE::iterator iterLine = m_mapLines.begin();
	while ((iterLine) != m_mapLines.end())
	{
		while ( ( (*iterLine).second.m_mapData.size() ) > m_nMaxPoints ) 
		{
			(*iterLine).second.m_mapData.erase((*iterLine).second.m_mapData.begin() );
			bResult = TRUE;
		}		
		++iterLine;
	}

	if (bResult)
	{
		BOOL bFirst=FALSE;	
		iterLine = m_mapLines.begin();
		while ( (!bFirst) && ((iterLine) != m_mapLines.end()) )
		{
			if ( !((*iterLine).second.m_mapData.empty()) )  
			{
				m_MinHzVal = ( (*((*iterLine).second.m_mapData.begin()) ).first);
				bFirst = TRUE;
			}
			++iterLine;
		}
		
		while ((iterLine) != m_mapLines.end())
		{
			if ( !((*iterLine).second.m_mapData.empty()) )  
			{
				if  ( m_MinHzVal > ( (*((*iterLine).second.m_mapData.begin() )).first) ) 	
					m_MinHzVal = ( (*((*iterLine).second.m_mapData.begin() )).first);
			}
			++iterLine;
		}
	}
	return bResult;
}

template<class T> 
void CScrollChart<T>::SendNewLimits()
{
	if(!m_bNotify) return;

	GetParent()->PostMessage (m_uH,
		(T) m_pointIni, 
		(T) m_pointIni + m_nHzRange );
	
	GetParent()->PostMessage (m_uV, 
		(T) m_MinValue, 
		(T) m_MaxValue);
}

template<class T> 
void CScrollChart<T>::Normalize()
{
		if ( m_pointIni < m_MinHzVal ) m_pointIni = m_MinHzVal;
		if ( m_MaxHzVal < m_pointIni + m_nHzRange) m_MaxHzVal = m_pointIni + m_nHzRange;
}

template<class T> 
void CScrollChart<T>::OnLButtonDown( UINT nFlags, CPoint point )
{
	SetFocus();
//	SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	SetCursor(m_hCursor);
	T x;
	T y;

	CPoint pointDraw;
	CPoint pointData;
	if (!m_mapLines.empty())
	{
		MAPLINE::const_iterator iterLine = m_mapLines.begin();
		bool bDone = false;
		while ((iterLine) != m_mapLines.end() && !bDone)
		{
			if (!(*iterLine).second.m_mapData.empty())
			{
				if ((*iterLine).second.bIsVisible)
				{
					MAPDATA::const_iterator iter = (*iterLine).second.m_mapData.lower_bound(m_pointIni);
					if (iter != (*iterLine).second.m_mapData.begin()) --iter;
					unsigned uDiff = UINT_MAX;
//					while ( ((iter != (*iterLine).second.m_mapData.upper_bound(m_nHzRange + m_pointIni))
//							&& (iter != (*iterLine).second.m_mapData.end()) && !bDone) )
					while (!bDone)
					{
						pointData = LPtoDP((*iter));
//						unsigned uDiffTemp = (point.x - pointData.x)*(point.x - pointData.x) + (point.y - pointData.y)*(point.y - pointData.y) ;
						unsigned uDiffTemp = abs(point.x - pointData.x);
						if (
							(uDiffTemp <= uDiff)
							)
						{

							uDiff = uDiffTemp;
						}
						else
						{
							iter--;
							pointData = LPtoDP((*iter));
							x = GetX(*iter);
							y = GetY(*iter);

							bDone = true;
							CJulianTime cJTime(x + m_uBaseTime);

							SetCapture();

							CString strNumber;
							strNumber.Format("%s %f", cJTime.Time(), y);
							
							CClientDC dc(this);
							int iSaved = dc.SaveDC();
							CPen pen;

							pen.CreatePen(PS_SOLID,3,RGB(0,128,255));
							dc.SelectObject(pen);

							dc.MoveTo(point);
							pointData.Offset(-1,0);
							dc.LineTo(pointData);
							dc.RestoreDC(iSaved);

							CreateSafeTooltipRect(point.x, point.y, strNumber);
							
							CBrush brush;
							brush.CreateSolidBrush(GetSysColor(COLOR_INFOBK));

							dc.Rectangle(tooltipRect);
							
							tooltipRect.left++;
							tooltipRect.top++;
							tooltipRect.bottom--;
							tooltipRect.right--;
							
							dc.FillRect(tooltipRect, &brush);
							
							CFont font;
							LOGFONT logFont;
							strcpy(logFont.lfFaceName, "Verdana");
							logFont.lfHeight = -MulDiv(12, GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);
							logFont.lfWidth = 0;
							logFont.lfEscapement = 0;
							logFont.lfItalic = FALSE;
							logFont.lfStrikeOut = FALSE;
							logFont.lfUnderline = FALSE;
							logFont.lfWeight = FW_BOLD;
							
							font.CreateFontIndirect(&logFont);
							
							dc.SelectObject(font);
							
							dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
							dc.SetBkColor(GetSysColor(COLOR_INFOBK));
							dc.DrawText(strNumber, tooltipRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
							
							tooltipRect.left--;
							tooltipRect.top--;
							tooltipRect.bottom++;
							tooltipRect.right++;

							tooltipRect.left = min(tooltipRect.left, pointData.x)-3;
							tooltipRect.right = max(tooltipRect.right, pointData.x)+3;
							tooltipRect.top = min(tooltipRect.top, pointData.y)-3;
							tooltipRect.bottom = max(tooltipRect.bottom,pointData.y)+3;

//							SetTimer(TIMER_EVENT,TIMER_TIME,NULL);
						}
						++iter;
//						if (iter == (*iterLine).second.m_mapData.end())
//							bDone = true;
					}
				}
			}
			else
				bDone = true;

			iterLine++;
//			if ((iterLine) == m_mapLines.end())
//				bDone = true;
		}
		
	} // not empty
//	CWnd::OnLButtonDown(nFlags, point);
}

template<class T> 
void CScrollChart<T>::OnLButtonUp( UINT nFlags, CPoint point )
{
	InvalidateRect(&tooltipRect, FALSE);
	ReleaseCapture();
	SetCursor(m_hCursor);
}

template<class T> 
void CScrollChart<T>::OnRButtonDown( UINT nFlags, CPoint point )
{
	SetFocus();
	SetCursor(m_hCursor);
}

template<class T> 
void CScrollChart<T>::OnRButtonUp( UINT nFlags, CPoint point )
{
	SetFocus();
	SetCursor(m_hCursor);
}

template<class T> 
void CScrollChart<T>::OnTimer( UINT nIDEvent )
{
	if (nIDEvent == TIMER_EVENT)
	{
		InvalidateRect(&tooltipRect, FALSE);
		ReleaseCapture();
	}
}


template<class T> 
void CScrollChart<T>::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_HAND)));
	SetCursor(m_hCursor);
	CWnd::OnMouseMove(nFlags, point);
}

template<class T> 
void CScrollChart<T>::CreateSafeTooltipRect(int x, int y, LPCTSTR lpszText)
{
	GetClientRect(m_rcClient);
	int nTextLength = strlen(lpszText);
	int nTextWidth = nTextLength;
	if(nTextWidth < 5)
		nTextWidth = 5;
	
//	if(x + 12 * nTextWidth <= m_rcClient.right)
	if (x < (m_rcClient.right + m_rcClient.left)/2)
		tooltipRect.right = x + 12 * nTextWidth;
	else
		tooltipRect.right = x - 12 * nTextWidth;


//	if(y + 25 <= m_rcClient.bottom)
	if (y < (m_rcClient.bottom + m_rcClient.top)/2)
		tooltipRect.bottom = y + 25;
	else
		tooltipRect.bottom = y - 25;

	tooltipRect.left = x;
	tooltipRect.top = y;

	if(tooltipRect.left > tooltipRect.right)
	{
		int nTemp = tooltipRect.left;
		tooltipRect.left = tooltipRect.right;
		tooltipRect.right = nTemp;
	}

	if(tooltipRect.top > tooltipRect.bottom)
	{
		int nTemp = tooltipRect.bottom;
		tooltipRect.bottom = tooltipRect.top;
		tooltipRect.top = nTemp;
	}
}



#endif 


  
