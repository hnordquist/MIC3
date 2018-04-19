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
// globals.cpp

//DR00200 - Error messages are needed when installation is not as expected.
//DR00200 - Added global registration anomoly message pjm 13-feb-2006 

#include "stdafx.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Data

CString	g_csVersion;
CString g_csNameVersionDate; //SCR 118

char    g_szIniFile[_MAX_PATH];               //configuration file
char	g_szClsidIniFile[_MAX_PATH];		  //CLSID file for DLL's
int     g_iInstCount;                         //how many instruments
int     g_iCommCount;                         //how many comm objects
int     g_iDlgCloseMillisec;                  //autodialog close at 
char    g_szUtilPath[_MAX_PATH];
bool    g_bHideWindows;                       //minimize the number of windows on screen
bool    g_blogarchiveact;                     //logarchive actions
bool    g_bNoPauseRestart;                    //fast restart -- instruments not paused
bool    g_bUseShortName;                      //use short form (8.3) file name convention
CString g_csLongName;                          //form of the long name
int     g_iButtonColumns;                       //number of button columns to instantiate
int     g_iButtonWidth;                       //width of an instrument button
int     g_iButtonHeight;                      //height of an instrument button
int     g_iButtonFlagSize;                    //height of an instrument button's flag
float   g_fMaxPercentDiskUse;                        //disk usage percentage
int     g_iMinutesToCheckArchive;
int     g_iDaysUntilDeleteFiles;                          //days until del files
char    g_sFacilityName[64];                  //facility name
bool    g_bDoingNetwork;                      //transmit button data to network addresses
int     g_iMaxRepetition;                     //maximum repetition of selected commands in the pfm file
bool    g_bInIAEA;
char    g_pVal_Name[64];
COleDateTime g_cLastGoodValidate;

CColorButton*             g_pColorButton[MAX_INSTRUMENTS];

IClassFactory            *g_pCoColorButtonFactory;
ISO_FUNCTIONAL_CATEGORY   g_eISOCategory[MAX_INSTRUMENTS];
IISO*                     g_pIISO[MAX_INSTRUMENTS];
ICB*                      g_pICB[MAX_INSTRUMENTS];
ICSO*                     g_pICSO[MAX_COMM];

CString m_csRegistrationAnomalyMessage; // DR00200
CString g_csColorButtonVersionString;
