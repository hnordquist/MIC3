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
// FileMgr.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "FileMgr.h"
#include "CopyFiles.h"
#include "MicMgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT uIDC_COM_STOP;

/////////////////////////////////////////////////////////////////////////////
// CFileMgr

IMPLEMENT_DYNCREATE(CFileMgr, CWinThread)

CFileMgr::CFileMgr()
{
//	TRACE("CFileMgr::CTOR\n");
	m_bDoingCopyFile = false;
	m_bDoingAgeFile = false;
	m_dTimeInCopy = 0;
	m_dTimeFromAge = 0;
	m_pMicMgrDlgParent = NULL;

	uIDC_COM_STOP = 0;
}

CFileMgr::~CFileMgr()
{
//	TRACE("CFileMgr::DTOR\n");
}

void CFileMgr::SetParentPointer(CMicMgrDlg *p)
{
//	TRACE("CFileMgr::SetParentPointer\n");
	m_pMicMgrDlgParent = p;
}
void CFileMgr::SetIDC_COM_STOPRegisteredMessageValue(UINT value)
{
	uIDC_COM_STOP = value;
}

BOOL CFileMgr::InitInstance()
{
//	TRACE("CFileMgr::InitInstance\n");

	//uIDC_COM_STOP = m_pMicMgrDlgParent->m_uIDC_COM_STOP;

	return TRUE;
}

int CFileMgr::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

void CFileMgr::Insert(FileRec* NewRec, FileRec* At)
{
	if (Head == NULL)
	{
		Head = NewRec;
	}
	else if (CompareFileTime(&(NewRec)->Date,&At->Date) != 1)
	{
		if (At->Older == NULL)
			At->Older = NewRec;
		else
			Insert(NewRec, At->Older);
	}
	else
	{
		if (At->Newer == NULL)
			At->Newer = NewRec;
		else
			Insert(NewRec, At->Newer);
	}
}

void CFileMgr::CleanUp(FileRec* At)
{
	if (At == NULL) return;
	if (At->Older)
		CleanUp(At->Older);
	if (At->Newer)
		CleanUp(At->Newer);
	delete [] At->PathName;
	delete At;
}

__int64	 CFileMgr::VisitSize(FileRec* At, __int64 BytesToDelete, FILE* OutFile)
{
//	if (OutFile)
//		fprintf(OutFile,"Enter VisitSize, BytesToDelete:%d\n",BytesToDelete);
	if (At == NULL) return BytesToDelete;
	if (BytesToDelete <= 0) return BytesToDelete;
	__int64 Result = BytesToDelete;
	if (At->Older)
		Result = VisitSize(At->Older,BytesToDelete, OutFile);
	if ((Result > 0) && (At->Deleted==false))
	{
		At->Deleted = true;
		if (remove(At->PathName)==0)
		{
			Result -= At->Size;
			if (OutFile)
				fprintf(OutFile,"Delete do to HD free bytes: %s %d\n",At->PathName, (int)Result); // added typecast gjk
		}
	}
	if ((Result > 0) && (At->Newer))
		Result = VisitSize(At->Newer,Result,OutFile);
	return Result;
}

void CFileMgr::VisitAge(FileRec* At, COleDateTime TodayMx, FILE* OutFile)
{
	if (At == NULL) return;
	if (At->Older)
		VisitAge(At->Older, TodayMx, OutFile);
	COleDateTime FilesTime(At->Date);
	if ((FilesTime < TodayMx)&&(At->Deleted == false))
	{
		At->Deleted = true;
		remove(At->PathName);
		if (OutFile)
			fprintf(OutFile,"Delete do to File Age: %s\n",At->PathName);
	}
	if (At->Newer)
		VisitAge(At->Newer, TodayMx, OutFile);
}

FileRec* CFileMgr::CreateRec(char* Path,WIN32_FIND_DATA FindFileData)
{
	FileRec* temp = new FileRec;
	memcpy(&temp->Date,&FindFileData.ftLastWriteTime,sizeof(FILETIME));
	temp->Size = FindFileData.nFileSizeLow; //these aren't big files
	temp->PathName = (char*)malloc(strlen(FindFileData.cFileName)+strlen(Path)+1);
	strcpy(temp->PathName,Path);
	strcat(temp->PathName,FindFileData.cFileName);
	temp->Older = NULL;
	temp->Newer = NULL;
	temp->Deleted = false;
	return temp;
}

void CFileMgr::DoArchiveCheck()
{
	//force it to be time to check
	m_dTimeFromAge = g_iMinutesToCheckArchive * 2;
	//send message to this thread
	//the "call" was from base thread 
	::PostThreadMessage(m_nThreadID,IDC_TIME_CHECK,0,0);
}

void CFileMgr::DoShutdown(WPARAM,LPARAM)
{
//	TRACE("CFileMgr::DoShutdown\n");
//	ExitThread(0);
	AfxEndThread(0);
}

void CFileMgr::DoTimeCheck(WPARAM, LPARAM)
{
	char cSaveLoc[_MAX_PATH];
	BSTR bstrSaveLoc, bstrFileExtensions;
	VARIANT_BOOL vbTrueFalse;

	//message is sent from timer in CMicDlg 
	//and is set in CMicDlg.OnInitDialog as 30000 (30 seconds)
	//if it has been an hour (120) then attempt to age files 
	if ((!m_bDoingCopyFile) && 
		(!m_bDoingAgeFile) && 
		(m_dTimeFromAge >= (g_iMinutesToCheckArchive*2)))
	{
		m_dTimeFromAge = 0;

		FILE *outfile;

		if (g_blogarchiveact)
		{
			outfile = fopen("AGE.LOG","a+");
			if (outfile)
			{
				COleDateTime TodayMx = COleDateTime::GetCurrentTime();
				fprintf(outfile,"begin at: %s\n",TodayMx.Format("%B %d, %Y: %H:%M:%S").GetString());
			}
		}
		else
			outfile = NULL;

		m_bDoingAgeFile = true;
		m_dTimeInCopy = 0;
		Head = NULL;

		WIN32_FIND_DATA FindFileData;
		FileRec* NewRec;

		//count the number of non "test" instruments
		int NonTestInstCount = 0;
		int NonTestInstFound = 0;
		for (int inst = 0; inst < g_iInstCount; inst++)
		{
			//if ((g_dInstType[inst] != WATCH) && 
			//	(g_dInstType[inst] != 0) )
			if ((g_eISOCategory[inst] != WATCH_ISO) && 
				(g_eISOCategory[inst] != 0) &&
				(g_eISOCategory[inst] != UNKNOWN_ISO))
				NonTestInstCount++;
		}

		//for each drive and while total instruments done < total instruments != test
		char cDrivePath[_MAX_PATH];
		char cDriveAndPath[_MAX_PATH];
		char cDriveName[4];
		ULARGE_INTEGER BytesAvailableToCaller;
		ULARGE_INTEGER TotalNumberOfBytes;
		_int64 i64TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;
		_int64 i64TotalNumberOfFreeBytes;

		for (int nDrive = 0; (nDrive < 25) && (NonTestInstFound < NonTestInstCount); nDrive++)
		{
			cDriveName[0] = (char)('A'+nDrive);
			cDriveName[1] = ':';
			cDriveName[2] = '\\';
			cDriveName[3] = NULL;

			//if nonzero then function worked
			if (GetDiskFreeSpaceEx(cDriveName, 
								   &BytesAvailableToCaller,
								   &TotalNumberOfBytes, 
								   &TotalNumberOfFreeBytes))
			{

//				if (g_blogarchiveact)
//				{
//					if (outfile)
//					{
//						fprintf(outfile,"%s Bytes Available to Caller:%I64d Total Number Of Bytes:%I64d Total Number Of Free Bytes:%I64d\n",
//							DriveName,BytesAvailableToCaller,TotalNumberOfBytes,TotalNumberOfFreeBytes);
//					}
//				}
				bool HaveAtLeastOneInst = false;
				//for each instrument
				for (int inst = 0; inst < g_iInstCount; inst++)
				{
					//if not a test instrument
					if ((g_eISOCategory[inst] != WATCH_ISO) && 
						(g_eISOCategory[inst] != 0) &&
						(g_eISOCategory[inst] != UNKNOWN_ISO))
					{
						g_pIISO[inst]->get_LocationAndExtensionsForDataFiles(
							&bstrSaveLoc, 
							&bstrFileExtensions);

						Convert_BSTR_to_Character_String(
							bstrSaveLoc,
							cSaveLoc,
							sizeof(cSaveLoc));

						//strcpy(DriveAndPath,g_pIISO[inst]->GetSaveLoc());
						strcat(cDriveAndPath,"\\archive\\");
						//if drive == save path's drive
						if (toupper(cSaveLoc[0]) == toupper(cDriveName[0]))
						{
							HaveAtLeastOneInst = true;
							NonTestInstFound++;
							HANDLE hFile;

							g_pIISO[inst]->get_LocationAndExtensionsForDataFiles(
								&bstrSaveLoc,
								&bstrFileExtensions);

							Convert_BSTR_to_Character_String(
								bstrSaveLoc,
								cDrivePath,
								sizeof(cDrivePath));
							//strcpy(DrivePath,g_pInstruments[inst]->GetSaveLoc());

							strcat(cDrivePath,"\\archive\\*.*");
							//get all filenames and place them in the btree
							if ((hFile = FindFirstFile(cDrivePath,&FindFileData)) != INVALID_HANDLE_VALUE)
							{
								if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
								{
									NewRec = CreateRec(cDriveAndPath,FindFileData);
									Insert(NewRec, Head);
								}
								while (FindNextFile(hFile,&FindFileData))
								{
									if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
									{
										NewRec = CreateRec(cDriveAndPath,FindFileData);
										Insert(NewRec, Head);
									}
								}
								FindClose(hFile);
							}
						}
					}
				}

				//don't bother if no instruments on this drive
				if (HaveAtLeastOneInst)
				{
					//visit by date deleting files older then desired
					//get todays date
					COleDateTime TodayMx = COleDateTime::GetCurrentTime();
					//figure what the date was g_dKeepData days ago
					TodayMx = TodayMx - COleDateTimeSpan(g_iDaysUntilDeleteFiles,0,0,0);//d,h,m,s//getprivateprofileint
					//delete anything older
					VisitAge(Head,TodayMx,outfile);
	
					//  visit by volume size required to be empty
					//desired-actual  = # bytes needing to be deleted
					i64TotalNumberOfBytes = TotalNumberOfBytes.QuadPart;
					i64TotalNumberOfFreeBytes = TotalNumberOfFreeBytes.QuadPart;
//					_int64 result = VisitSize(Head,(i64TotalNumberOfBytes*(100L-g_dMaxPercent)/100L)-i64TotalNumberOfFreeBytes,outfile);
					_int64 result = VisitSize(Head,(_int64)((i64TotalNumberOfBytes*(100.0-g_fMaxPercentDiskUse)/100.0)-i64TotalNumberOfFreeBytes),outfile);
					{
						//result > 0 then Could Not Delete Enough -- so stop writing to disk
						//   or turn back on writing to disk 
						//for each instrument

						BSTR bstrSaveLoc, bstrFileExtensions;
						int inst;
						for (inst = 0; inst < g_iInstCount; inst++)
						{
							//if not a test instrumnet
							if ((g_eISOCategory[inst] != WATCH_ISO) && 
								(g_eISOCategory[inst] != 0) &&
								(g_eISOCategory[inst] != UNKNOWN_ISO))
							{
								g_pIISO[inst]->get_LocationAndExtensionsForDataFiles(
									&bstrSaveLoc,
									&bstrFileExtensions);

								Convert_BSTR_to_Character_String(
									bstrSaveLoc,
									cSaveLoc,
									sizeof(cSaveLoc));
								
								//if drive == save path's drive
								if (toupper(cSaveLoc[0]) == toupper(cDriveName[0]))
								{
									//allow or disallow writing to disk
									//disallow only if we could not delete enough
									//disk space to bring us at or above the desired
									//amount of free space
									if (result > 0)
										vbTrueFalse = VARIANT_TRUE;
									else
										vbTrueFalse = VARIANT_FALSE;

									g_pIISO[inst]->AllowWriteToDisk(vbTrueFalse);
								}
							}
						}

					}
					//  clean up btree
					CleanUp(Head);
				}
				Head = NULL;
			}
		}
		m_bDoingAgeFile = false;
		if (g_blogarchiveact)
		{
			if (outfile)
			{
				fprintf(outfile,"done\n");
				fclose(outfile);
			}
		}
	}
	m_dTimeFromAge++;
	m_dTimeInCopy++;
}


void CFileMgr::DoCopyData(WPARAM, LPARAM)
{
//	TRACE("CFileMgr::DoCopyData\n");

	m_bDoingCopyFile = true;

	CCopyFiles *p = new CCopyFiles();
	//p->SetIniFileName(m_szIniFile);
	//p->SetMicMgrPointer(m_pMicMgrDlgParent)
	p->DoModal(this);
	delete p;

	//CCopyFiles copyfiles(this);
	//copyfiles.DoModal(this);

	GetMainWnd()->PostMessage(IDC_COPY_DONE,0,0);
	m_bDoingCopyFile = false;
}


BEGIN_MESSAGE_MAP(CFileMgr, CWinThread)
	//{{AFX_MSG_MAP(CFileMgr)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(IDC_TIME_CHECK,DoTimeCheck)
	ON_THREAD_MESSAGE(IDC_COPY_DATA,DoCopyData)
	ON_REGISTERED_THREAD_MESSAGE(uIDC_COM_STOP,DoShutdown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileMgr message handlers
