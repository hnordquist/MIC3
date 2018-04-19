// MyTreeCtrl.cpp : implementation file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "mtreectl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SCROLL_BORDER 10
#define SCROLL_SPEED_ZONE_WIDTH 20


/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl

CMyTreeCtrl::CMyTreeCtrl()
{
	m_bDragging = FALSE;
	m_pimagelist = NULL;
	m_bItemChanged = FALSE;
	m_timerticks = 0;
}

CMyTreeCtrl::~CMyTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMyTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBeginDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl message handlers
void CMyTreeCtrl::OnDestroy()
{
	CImageList  *pimagelist;

	pimagelist = GetImageList(TVSIL_NORMAL);
	pimagelist->DeleteImageList();
	delete pimagelist;
}

void CMyTreeCtrl::SetNewStyle(long lStyleMask, BOOL bSetBits)
{
	long        lStyleOld;

	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld &= ~lStyleMask;
	if (bSetBits)
		lStyleOld |= lStyleMask;

	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void CMyTreeCtrl::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	TV_DISPINFO     *ptvinfo;

	ptvinfo = (TV_DISPINFO *)pnmhdr;
	if (ptvinfo->item.pszText != NULL)
	{
		ptvinfo->item.mask = TVIF_TEXT;
		SetItem(&ptvinfo->item);
	}
	*pLResult = TRUE;
}

void CMyTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM           hitem;//,hitemlast;
	UINT                flags;

	if (m_bDragging)
	{
		ASSERT(m_pimagelist != NULL);
		m_pimagelist->DragMove(point);
		if ((hitem = HitTest(point, &flags)) != NULL)
		{
			m_pimagelist->DragLeave(this);
			SelectDropTarget(hitem);

			m_hitemDrop = hitem;
			m_pimagelist->DragEnter(this, point);
		}
//		else
//		{
//			if (flags & TVHT_ABOVE)
//			{
//				PostMessage(WM_KEYDOWN,VK_UP,NULL);
//				PostMessage(WM_KEYUP,VK_UP,NULL);
//
//			}
//			if (flags & TVHT_BELOW)
//			{
//				PostMessage(WM_KEYDOWN,VK_DOWN,NULL);
//				PostMessage(WM_KEYUP,VK_DOWN,NULL);
//			}
//		}
/*
			int iLLimit;
			int iULimit;
			SCROLLINFO sScrollInfo;
			GetScrollInfo( SB_VERT, &sScrollInfo);
			GetScrollRange(SB_VERT,&iLLimit,&iULimit);

			if (flags & TVHT_ABOVE)
			{
				int iAt = GetScrollPos(SB_VERT);
				if (iAt > iLLimit )
				{
					SetScrollPos( SB_VERT, iAt-sScrollInfo.nPage, TRUE );
					ScrollWindow(0,sScrollInfo.nPage);
					UpdateWindow();
				}
			}

			if (flags & TVHT_BELOW)
			{
				int iAt = GetScrollPos(SB_VERT);
				if (iAt < iULimit)
				{
					SetScrollPos(SB_VERT, iAt+sScrollInfo.nPage, TRUE);
					ScrollWindow(0,-sScrollInfo.nPage);
					UpdateWindow();
				}
			}
		}
*/
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

BOOL CMyTreeCtrl::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)
			break;
	}
	while ((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}


BOOL CMyTreeCtrl::TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
	TV_INSERTSTRUCT     tvstruct;
	TCHAR               sztBuffer[50];
	HTREEITEM           hNewItem, hFirstChild;

		// avoid an infinite recursion situation
	tvstruct.item.hItem = hitemDrag;
	tvstruct.item.cchTextMax = 49;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	GetItem(&tvstruct.item);  // get information of the dragged element

	tvstruct.hParent = hitemDrop;
	tvstruct.hInsertAfter = TVI_SORT;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);

	while ((hFirstChild = GetChildItem(hitemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem);  // recursively transfer all the items
		DeleteItem(hFirstChild);        // delete the first child and all its children
	}
	return TRUE;
}

void CMyTreeCtrl::OnButtonUp()
{
	if (m_bDragging)
	{
		ASSERT(m_pimagelist != NULL);
		m_pimagelist->DragLeave(this);
		m_pimagelist->EndDrag();
		delete m_pimagelist;
		m_pimagelist = NULL;
		if (m_hitemDrag != m_hitemDrop && 
			!IsChildNodeOf(m_hitemDrop, m_hitemDrag) &&
			GetParentItem(m_hitemDrag) != m_hitemDrop &&
			GetParentItem(m_hitemDrag) != NULL &&
			GetParentItem(m_hitemDrop) == NULL )
		{
			m_hitemDragParent = GetParentItem(m_hitemDrag);
			GetParent()->SendMessage(TV_PREDROP,(unsigned int)m_hitemDrag,(long)m_hitemDrop);
			TransferItem(m_hitemDrag, m_hitemDrop);
			DeleteItem(m_hitemDrag);
			GetParent()->PostMessage(TV_POSTDROP,(unsigned int)m_hitemDragParent,NULL);
			m_bItemChanged = TRUE;
		}
		else
			MessageBeep(0);

		ReleaseCapture();
		m_bDragging = FALSE;
		SelectDropTarget(NULL);
		KillTimer(1000);
	}
}

void CMyTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnButtonUp();
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CMyTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnButtonUp();
	CTreeCtrl::OnRButtonUp(nFlags, point);
}

void CMyTreeCtrl::OnBeginDrag(LPNMHDR /*pnmhdr*/, LRESULT * /*pLResult*/)
{
	CPoint      ptAction;
	UINT        nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	ASSERT(!m_bDragging);
	m_bDragging = TRUE;
	m_hitemDrag = HitTest(ptAction, &nFlags);
	m_hitemDrop = NULL;

	ASSERT(m_pimagelist == NULL);
	m_pimagelist = CreateDragImage(m_hitemDrag);  // get the image list for dragging
	m_pimagelist->DragShowNolock(TRUE);
	m_pimagelist->SetDragCursorImage(1, CPoint(0, 0));
	m_pimagelist->BeginDrag(0, CPoint(0,0));
	m_pimagelist->DragMove(ptAction);
	m_pimagelist->DragEnter(this, ptAction);
	SetCapture();

	//set the timer to watch for scroll up and down
	SetTimer(1000,75,NULL);
}

void CMyTreeCtrl::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1000)
	{

	// Doesn't matter that we didn't initialize m_timerticks
		m_timerticks++;

		POINT pt;
		GetCursorPos( &pt );
		CRect rect;
		GetClientRect( &rect );
		ClientToScreen( &rect );

		// NOTE: Screen coordinate is being used because the call
		// to DragEnter had used the Desktop window.
		//CImageList::DragMove(pt);

		HTREEITEM hitem = GetFirstVisibleItem();

  		int iMaxV = GetScrollLimit(SB_VERT);
 		int iPosV = GetScrollPos(SB_VERT);
		// The cursor must not only be SOMEWHERE above/beneath the tree control
		// BUT RIGHT above or beneath it 
		// i.e. the x-coordinates must be those of the control (+/- SCROLL_BORDER)
		if ( pt.x < rect.left - SCROLL_BORDER ) 
		  ; // Too much to the left
		else if ( pt.x > rect.right + SCROLL_BORDER ) 
		  ; // Too much to the right
		else if( (pt.y < rect.top + SCROLL_BORDER) && iPosV )
		{
			// We need to scroll up
			// Scroll slowly if cursor near the treeview control
			int slowscroll = 6 - (rect.top + SCROLL_BORDER - pt.y) / SCROLL_SPEED_ZONE_WIDTH;
			if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
			{
				CImageList::DragShowNolock(FALSE);
				SendMessage( WM_VSCROLL, SB_LINEUP);
				SelectDropTarget(hitem);
				m_hitemDrop = hitem;
				CImageList::DragShowNolock(TRUE);
			}
		}
		else if( (pt.y > rect.bottom - SCROLL_BORDER) && (iPosV!=iMaxV) )
		{
			// We need to scroll down
			// Scroll slowly if cursor near the treeview control
			int slowscroll = 6 - (pt.y - rect.bottom + SCROLL_BORDER ) / SCROLL_SPEED_ZONE_WIDTH;
			if( 0 == ( m_timerticks % (slowscroll > 0? slowscroll : 1) ) )
			{
				CImageList::DragShowNolock(FALSE);
				SendMessage( WM_VSCROLL, SB_LINEDOWN);
				int nCount = GetVisibleCount();
				for ( int i=0; i<nCount-1; ++i )
					hitem = GetNextVisibleItem(hitem);
				if( hitem )
					SelectDropTarget(hitem);
				m_hitemDrop = hitem;
				CImageList::DragShowNolock(TRUE);
			}
		}

		// The cursor must be in a small zone IN the treecontrol at the left/right
		int iPosH = GetScrollPos(SB_HORZ);
		int iMaxH = GetScrollLimit(SB_HORZ);

		if ( !rect.PtInRect(pt) ) return; // not in TreeCtrl
		else if ( (pt.x < rect.left + SCROLL_BORDER) && (iPosH != 0) )
		{
			// We need to scroll to the left
			CImageList::DragShowNolock(FALSE);
			SendMessage(WM_HSCROLL, SB_LINELEFT);
			CImageList::DragShowNolock(TRUE);
		}
		else if ( (pt.x > rect.right - SCROLL_BORDER) && (iPosH != iMaxH) )
		{
			// We need to scroll to the right
			CImageList::DragShowNolock(FALSE);
			SendMessage(WM_HSCROLL, SB_LINERIGHT);
			CImageList::DragShowNolock(TRUE);
		}

	}
//	CDialog::OnTimer(nIDEvent);
}
