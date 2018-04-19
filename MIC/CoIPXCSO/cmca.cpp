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


/* cmca.cpp   code for implementing the MCA class */

#include "stdafx.h"
#include  <string.h>
#include  "cmca.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*SO CMca */
/*-> cmca.h */

static  char *szNullMessage="";
static  char *szInvalidMca="Invalid MCA";


/*++ constructor function */
CMca::CMca() {
	bBusy = false;
	bClear = false;
	iCycleNum = 1;
	szDetLogo = (char *)0;
	szDetName = (char *)0;
	bDone = true;
	dElapsedTime = 0.0;
	szErrorMessage = (char *)0;
	bLiveTime = true;
	bLogErrors = false;
	szLogFile = (char *)0;
	iNumCycles = 1;
	bOpen = false;
	dSaveTime = 0.0;
	bSimulate = false;
	szSrcFile = (char *)0;
	bSuspended = false;
}

/*++ destructor function */
CMca::~CMca() {
	if ( szLogFile!=(char *)0 ) {
		delete [] szLogFile;
	}
	if ( szSrcFile!=(char *)0 ) {
		delete [] szSrcFile;
	}
	if ( szDetLogo!=(char *)0 ) {
		delete [] szDetLogo;
	}
	if ( szDetName!=(char *)0 ) {
		delete [] szDetName;
	}
}

/*++ retrieve field values */
long CMca::CountTime() {
	return lCountTime;
}
int CMca::CycleNum() {
	return iCycleNum;
}
const char * CMca::DetName() {
	return szDetLogo;
}
long CMca::ElapsedTime() {
	return (long)(dElapsedTime+0.5);
}
const char * CMca::ErrorMessage() {
	if ( szErrorMessage==(char *)0 ) {
		return szNullMessage;
	}
	else {
		return szErrorMessage;
	}
}
bool CMca::IsBusy() {
	return bBusy;
}
bool CMca::IsDone() {
	return bDone;
}
bool CMca::IsOpen() {
	return bOpen;
}
bool CMca::IsSuspended() {
	return bSuspended;
}
int CMca::NumCycles() {
	return iNumCycles; 
}

/*++ change the log errors flag */
void CMca::LogErrors(bool Flag, char *File) {
	if ( Flag  &&  File!=(char *)0 ) {
		int  size=strlen(File);
		if ( szLogFile!=(char *)0 ) {
			delete [] szLogFile;
		}
		szLogFile = new char [size+1];
		strncpy(szLogFile, File, size);
		szLogFile[size] = '\0';
		bLogErrors = true;
	}
}

/*++ change the clear data flag */
void CMca::SetClearFlag(bool Flag) {
	bClear = Flag;
}

/*++ change the preset count time */
void CMca::SetCountTime(long Sec, bool LiveTime) {
	lCountTime = Sec;
	bLiveTime = LiveTime;
}

/*++ change the current cycle number */
void CMca::SetCycleNum(int Num) {
	iCycleNum = Num; 
}

/*++ set the detector name */
void CMca::SetDetName(char *Name, char *Mfg) {
	if ( Name!=(char *)0 ) {
		int   size1=strlen(Name);
		int   size2=strlen(Mfg);
		if ( size1>119 ) {
			size1 = 119;
		}
		if ( size2>8 ) {
			size2 = 8;
		}
		if ( szDetLogo!=(char *)0 ) {
			delete [] szDetLogo;
		}
		szDetLogo = new char [size1+size2+3];
		strncpy(&szDetLogo[0], Mfg, size2);
		szDetLogo[size2] = '(';
		strncpy(&szDetLogo[size2+1], Name, size1);
		szDetLogo[size1+size2+1] = ')';
		szDetLogo[size1+size2+2] = '\0';
		if ( szDetName!=(char *)0 ) {
			delete [] szDetName;
		}
		szDetName = new char [size1+1];
		strncpy(&szDetName[0], Name, size1);
		szDetName[size1] = '\0';
	}
}

/*++ change the live time flag */
void CMca::SetLiveTimeFlag(bool Flag) {
	bLiveTime = Flag;
}

/*++ change the number of cycles */
void CMca::SetNumCycles(int Num) {
	iNumCycles = Num; 
}

/*++ change the name of the source file for simulation */
void  CMca::SetSourceFile(char *File) {
	if ( szSrcFile!=(char *)0 ) {
		delete [] szSrcFile;
	}
	if ( File!=(char *)0 ) {
		int  size=strlen(File);
		szSrcFile = new char [size+1];
		strncpy(szSrcFile, File, size);
		szSrcFile[size] = '\0';
	}
	else {
		szSrcFile = new char [1];
		szSrcFile[0] = '\0';
	}
}

/*++ set the simulate flag */
void CMca::Simulate(bool Flag) {
	bSimulate = Flag;
}
/*EO CMca */