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
// DirDialog.cpp
// 
// User: Moore        Date: 12/01/05   Time: 6:05p
// Updated in $/MIC 2.000/CommonSource
// Changed DIRDLG.cpp to keep error message from coming up when
// running from an MPSS startup on a Windows XP operating system.
// This was a problem Kelly Michel and Peggy Moore observed in Feb 2004
// in Vienna during the Baseline 1 training.  The students had installed
// MIC 1.907 on Windows XP laptops and kept getting an error message
// whenever the Browse buttons were pressed. Thereafter, however, the
// function was successful in presenting the correct dialog.  That is, the
// problem was a cosmetic one, but very ugly.  The error message did not
// happen when running with Windows 2000.
// 
 
///////////////////////////////////////////////////////////////////////////
// DirDialog.cpp: implementation of the CDirDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDialog.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirDialog::CDirDialog(CWnd *pParent)
{
	//save the parent
	m_pParent = pParent;
	m_strInitDir.Empty();
}

CDirDialog::~CDirDialog()
{
}

//do the browse window
int CDirDialog::DoBrowse()
{
  LPMALLOC pMalloc;
  if (SHGetMalloc (&pMalloc)!= NOERROR)
  {
      return 0;
  }

  BROWSEINFO bInfo;
  LPITEMIDLIST pidl;
  ZeroMemory ( (PVOID) &bInfo,sizeof (BROWSEINFO));

  //
  //Deleted the following line to keep error message from coming up when
  //running from an MPSS startup on a Windows XP operating system.
  //This was a problem Kelly Michel and Peggy Moore observed in February 2004
  //in Vienna during the Baseline 1 training.  The students had installed
  //MIC 1.907 on Windows XP laptops and kept getting an error message whenever
  //the Browse buttons were pressed. Thereafter, however, the function was
  //successful in presenting the correct dialog.  That is, the problem was
  //a cosmetic one, but very ugly.  The error message did not happen when
  //running with Windows 2000.
  //
  //The fix makes it so the desktop is evoked not via a NULL in the BROWSEINFO
  //structure, but by using an empty string to determine the location of the
  //root folder.
  //
  //PJM 12/1/2005
  //
  //if (!m_strInitDir.IsEmpty ())
  {
    OLECHAR       olePath[MAX_PATH];
    ULONG         chEaten;
    ULONG         dwAttributes;
    HRESULT       hr;
    LPSHELLFOLDER pDesktopFolder;
    // 
    // Get a pointer to the Desktop's IShellFolder interface. 
    //
    if (SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))
    {
      //
      // IShellFolder::ParseDisplayName requires the file name be in Unicode.
      //
      MultiByteToWideChar(CP_ACP, 
		  MB_PRECOMPOSED, m_strInitDir.GetBuffer(MAX_PATH), -1,
                          olePath, MAX_PATH);

      m_strInitDir.ReleaseBuffer (-1);
      //
      // Convert the path to an ITEMIDLIST.
      //
      hr = pDesktopFolder->ParseDisplayName(NULL,
			NULL,olePath,&chEaten,&pidl,&dwAttributes);

      if (FAILED(hr))
      {
        pMalloc ->Free (pidl);
        pMalloc ->Release ();
        return 0;
      }

      bInfo.pidlRoot = pidl;

    }
  }
  bInfo.hwndOwner = m_pParent->m_hWnd;
  bInfo.pszDisplayName = m_strPath.GetBuffer (MAX_PATH);
  bInfo.lpszTitle = (m_strTitle.IsEmpty()) ? "Open":m_strTitle;
  bInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;

  if ((pidl = ::SHBrowseForFolder(&bInfo)) == NULL)
  {
	  return 0;
  }
  m_strPath.ReleaseBuffer();
  m_iImageIndex = bInfo.iImage;

  if (::SHGetPathFromIDList(pidl,m_strPath.GetBuffer(MAX_PATH)) == FALSE)
  {
    pMalloc ->Free(pidl);
    pMalloc ->Release();
    return 0;
  }

  m_strPath.ReleaseBuffer();

  pMalloc ->Free(pidl);
  pMalloc ->Release();

  return 1;
}