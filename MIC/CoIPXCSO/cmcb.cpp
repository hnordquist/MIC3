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

/* cmbc.cpp   code for implementing the ORTEC MCB class */

#include "stdafx.h"
// Windows includes
#include <windows.h>
// C run time library includes
#include <stdio.h>
#include <string.h>
// ORTEC's includes
#include "mcbcio32/mcbcio32.h"
// User's includes
//#include "cdattim.h"
#include "cmcb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*SO CMcb */
/*-> cmcb.h */

// global variables
static const char * gaszCMcbErrorMessages[]={
	/* CMBC_OK               */ "success.",
	/* CMCB_VERSION_MISMATCH */ "version mismatch.",
	/* CMCB_NOT_OPEN         */ "Open has not been called.",
	/* CMCB_ALREADY_OPEN     */ "Open has already been called.",
	/* CMCB_INIT_FAILED      */ "Error in initialization.",
	/* CMCB_BUSY             */ "MCB is currently acquiring data.",
	/* CMCB_NOT_BUSY         */ "MCB is not currently acquiring data.",
	/* CMCB_STILL_BUSY       */ "MCB is still busy after a STOP command.",
	/* CMCB_UMCBI_LIST       */ "cannot access the master list.",
	/* CMCB_RSP_GARBLE       */ "response from MCB was garbled.",
	/* CMCB_UMCBI_ERROR      */ "UMCBI error encountered.",
	/* CMCB_IDETIDX_TOO_BIG  */ "MCB index is out of range.",
	/* CMCB_UMCBI_OPEN       */ "cannot open a connection to the MCB.",
	/* CMCB_NO_PRESET        */ "error in retrieving preset information.",
	/* CMCB_PRESET_REACHED   */ "on START, preset time already exceeded.",
	/* CMCB_DUMMY_FILE_IO    */ "I/O error with dummy file.",
	/* CMCB_SPW_ERR          */ "error return from SpwXxx routine.",
	/* CMCB_BAD_INPUT        */ "invalid input for a function.",
	/* CMCB_BAD_INDEX        */ "invalid index into detector list.",
	/* CMCB_BAD_HANDLE       */ "invalid detector handle.",
	/* CMCB_ILLEGAL          */ "illegal status code"
};

/*++ constructor function */
CMcb::CMcb(char *DetName, DWORD DetId, CSpctrm *pSpecInfo) {
	int    iRetVal;

	ClearError();
	memset(szCommand, 0, CMCB_LEN_COMMAND+1);
	lDetId = 0L;
	iLength = 0;
	bLogErrors = false;
	szLogFile = (char *)0;
	memset(szReply, 0, CMCB_LEN_REPLY+1);
	lSimLiveTime = 0L;
	lSimTrueTime = 0L;
	bSpecInfo = false;
	pxSpecInfo = (CSpctrm *)0;
	pSpec = (CChnSpc *)0;
	bValid = false;

	// check input arguments
	if ( DetName==(char *)0 ) {
		return;
	}

	// connect to MCBCIO.DLL
	//for some reason this throws an exception
	iRetVal = MIOStartup();
	if ( iRetVal==0 ) {
		iError = MIOGetLastError(&iErrorMacro, &iErrorMicro);
		MIOCleanup();
		iStatus = CMCB_INIT_FAIL;
	}
	else {
		bValid = true;
		SetDetName(DetName, "Ortec");
		lDetId = DetId;
		if ( pSpecInfo==(CSpctrm *)0 ) {
			bSpecInfo = false;
		}
		else {
			bSpecInfo = true;
			pxSpecInfo = pSpecInfo;
			pSpec = new CChnSpc(pSpecInfo->MaxChans(), pSpecInfo->pSpectrum());
		}
	}
}

/*++ destructor function */
CMcb::~CMcb() {
	if ( bValid ) {
		if ( bOpen ) {
			Close();
		}
		// disconnect from MCBCIO.DLL
		MIOCleanup();
		// release dynamic memory used
		if ( bSpecInfo ) {
			delete pSpec;
		}
	}
}

/*++ reset the error flags */
void CMcb::ClearError() {
	iStatus = CMCB_OK;
	iError = MIOENONE;
	iErrorMacro = 0;
	iErrorMicro = 0;
}

/*++ was the constructor successful? */
bool CMcb::IsValid() {
	return bValid;
}

/*++ set the detector id */
void CMcb::SetDetId(DWORD Id) {
	lDetId = Id;
}

/*++ retrieve the detector id */
DWORD CMcb::DetId() {
	return lDetId;
}

/*++ retrieve pointer to spectral information */
CChnSpc * CMcb::pSpctrm() {
	return pSpec;
}

/*++ get detector information */
bool CMcb::GetDetectorInfo(int Index, DWORD *DetId, int StrMax, char *Desc, int *DetMax) {
	bool       bOk=true;
	int        iRetVal;
	int        iStale;

	if ( !bValid ) {
		return false;
	}
	iRetVal = MIOGetConfigMax("", DetMax);
	if ( !iRetVal ) {
		return false;
	}
	if ( Index<=0 ) {
		return true;
	}
	if ( Index<=*DetMax ) {
		iRetVal = MIOGetConfigName(Index, "", StrMax, Desc, DetId, &iStale);
	}
	return bOk;
}

//*************************************************************************		

/*++  check Status of MCA */
bool CMcb::CheckStatus() {
	int      bActive;
	bool     bOk=true;
	bool     bLive;
	long     lTime;
	ULONG    uTimeL;
	ULONG    uTimeT;

	if ( !bOpen ) {
		return false;
	}
	if ( bBusy ) {
		if ( bSimulate ) {
			CDatTim   DT;
			double   dFactor=1.0;
			if ( lCountTime>500 ) {
				dFactor = 100;
			}
			dElapsedTime = dSaveTime + DT.DeltaSeconds(&xStartTime)*dFactor;
			bBusy = (dElapsedTime<(double)lCountTime);
		}
		else if ( !GetInt("SHOW_ACTIVE", &bActive) ) {
			bOk = false;
		}
		else if ( bActive==0 ) {
			bBusy = false;
		}
		else if ( !GetPreset(&bLive, &lTime) ) {
			bOk = false;
		}
		else if ( !GetULong("SHOW_LIVE", &uTimeL) ) {
			bOk = false;
		}
		else if ( !GetULong("SHOW_TRUE", &uTimeT) ) {
			bOk = false;
		}
		else if ( bLive!=0 ) {
			dElapsedTime = ((double)uTimeL)/CMCB_TICKS_PER_SECOND;
		}
		else {
			dElapsedTime = ((double)uTimeT)/CMCB_TICKS_PER_SECOND;
		}
		if ( bOk  &&  !bBusy ) {
			bDone = (iCycleNum>=iNumCycles);
		}
	}
    return bOk;
}

/*++ close the MCA */
bool CMcb::Close() {
	if ( !bOpen ) {
		return true;
	}
	if ( !bSimulate ) {
		MIOCloseDetector(hDet);
		LogEvent(false, "Close");
	}
	bDone = true;
	bOpen = false;
    return true;
}

/*++ retrieve spectral data */
bool CMcb::GetSpectrum() {
	ULONG   *puBuffer;
	ULONG     lDataMask;
	long      lIndex;
	WORD      iNch;
	WORD      iNum;
	long      lNumChans;
	bool      bOk;
	double    dRatio;
	ULONG     lRoiMask;
	long      *Spectrum;
	double    dTime;
	ULONG     uTimeL;
	ULONG     uTimeT;

	if ( !bOpen || !bSpecInfo ) {
		return false;
	}
	Spectrum = pSpec->pSpectrum();
	lNumChans = pSpec->NumChans();
	iNch = lNumChans & 0xFFFF;
	if ( bSimulate ) {
		dRatio = (double)lSimTrueTime / (double)lSimLiveTime;
		if ( bLiveTime ) {
			pSpec->SetLiveTime((long)(dElapsedTime+0.5));
			pSpec->SetTrueTime((long)(dElapsedTime*dRatio+0.5));
			pxSpecInfo->SetLiveTime((long)(dElapsedTime+0.5));
			pxSpecInfo->SetTrueTime((long)(dElapsedTime*dRatio+0.5));
		}
		else {
			pSpec->SetLiveTime((long)(dElapsedTime/dRatio+0.5));
			pSpec->SetTrueTime((long)(dElapsedTime+0.5));
			pxSpecInfo->SetLiveTime((long)(dElapsedTime/dRatio+0.5));
			pxSpecInfo->SetTrueTime((long)(dElapsedTime+0.5));
		}
		dRatio = dElapsedTime / (double)lCountTime;
		if ( dRatio<0.0 ) {
			dRatio = 0.0;
		}
		if ( dRatio>1.0 ) {
			dRatio = 1.0;
		}
		bOk = (pSpec->Read(szSrcFile)!=0);
		for (lIndex=0; lIndex<lNumChans; lIndex++) {
			Spectrum[lIndex] = (long)(dRatio * Spectrum[lIndex]);
		}
	}
	else {
		// retrieve the live and true times
		if ( !GetULong("SHOW_LIVE", &uTimeL) ) {
			bOk = false;
		}
		else if ( !GetULong("SHOW_TRUE", &uTimeT) ) {
			bOk = false;
		}
		else {
			dTime = ((double)uTimeL)/CMCB_TICKS_PER_SECOND;
			pSpec->SetLiveTime((long)(dTime+0.5));
			pxSpecInfo->SetLiveTime((long)(dTime+0.5));
			dTime = ((double)uTimeT)/CMCB_TICKS_PER_SECOND;
			pSpec->SetTrueTime((long)(dTime+0.5));
			pxSpecInfo->SetTrueTime((long)(dTime+0.5));
			// retrieve the spectral data
			puBuffer = MIOGetData(hDet, 0, iNch, (ULONG *)Spectrum, &iNum, &lDataMask, &lRoiMask, "");
			if ( puBuffer==NULL ) {
				bOk = false;
				iError = MIOGetLastError(&iErrorMacro, &iErrorMicro);
				iStatus = CMCB_UMCBI_ERROR;
			}
			else {
				for (lIndex=0; lIndex<lNumChans; lIndex++) {
					Spectrum[lIndex] &= lDataMask;
				}
			}
		}
	}
	return bOk;
}

/*++ special control functions */
bool CMcb::IoCtl(void *Input) {
	szErrorMessage = "Not Implemented.";
	return false;
}

/*++ open the MCA */
bool CMcb::Open(bool Simulate, char *File) {

	if ( !bValid  ||  !bSpecInfo ) {
		return false;
	}
	if ( bOpen ) {
		return true;
	}

	ClearError();
	bSimulate = Simulate;
	if ( bSimulate ) {
		SetSourceFile(File);
		bOpen = (pSpec->Read(szSrcFile)!=0);
		if ( bOpen ) {
			lSimLiveTime = pSpec->LiveTime();
			lSimTrueTime = pSpec->TrueTime();
		}
	}
	else {
		int   iNumCh;
		LogEvent(false, "Open");
		hDet = MIOOpenDetID(lDetId, "");
		if ( hDet==NULL ) {
			bOpen = false;
		}
		else if ( !GetInt("SHOW_GAIN_CONVERSION", &iNumCh) ) {
			bOpen = false;
		}
		else if ( !SendCommand("STOP") ) {
			bOpen = false;
		}
		else if ( !SendCommand("CLEAR") ) {
			bOpen = false;
		}
		else {
			bOpen = true;
			if ( iNumCh<0 ) {
				iNumCh = 0;
			}
			if ( iNumCh>pxSpecInfo->MaxChans() ) {
				iNumCh = pxSpecInfo->MaxChans();
			}
			pSpec->SetNumChans((UINT)iNumCh);
			pxSpecInfo->SetNumChans((UINT)iNumCh);
		}
	}
	bDone = false;
    return bOpen;
}

/*++ resume counting */
bool CMcb::Resume() {
	bool bSaveClear;

	if ( !bOpen ) {
		return false;
	}
	if ( !bBusy ) {
		bSaveClear = bClear;
		bClear = false;
		StartAcquire((void *)0);
		bClear = bSaveClear;
	}
	bSuspended = false;
	return true;
}

/*++ save the spectral data */
bool CMcb::SaveSpectrum(char *File, void *Input) {
	bool     bOk;

	if ( !bValid  ||  !bSpecInfo ) {
		return false;
	}
	if ( bSimulate ) {
		pSpec->ResetDateTime();
	}
	bOk = (pSpec->Write(File)!=0);
	return bOk;
}

/*++ start data acquisition on MCA */
bool CMcb::StartAcquire(void *Input){

	if ( !bOpen ) {
		return false;
	}
	if ( bSimulate ) {
		xStartTime.SetDateTime();
		if ( bClear ) {
			dSaveTime = 0.;
			dElapsedTime = 0.;
		}
		bBusy = true;
	}
	else {
		long  lTime = lCountTime * CMCB_TICKS_PER_SECOND;
		// prepare the command for presetting the time
		if ( bLiveTime ) {
			sprintf(szCommand, "SET_LIVE_PRESET %ld", lTime);
		}
		else {
			sprintf(szCommand, "SET_TRUE_PRESET %ld", lTime);
		}
		// clear the data and elapsed time if asked to
		if ( bClear  &&  !SendCommand("CLEAR") ) {
			bBusy = false;
		}
		// establish the preset time and mode
		else if ( !SendCommand("CLEAR_PRESET") ) {
			bBusy = false;
		}
		else if ( !SendCommand(szCommand) ) {
			bBusy = false;
		}
		// synchronize the date and time with the computer
		else if ( !SetDateTime() ) {
			bBusy = false;
		}
		// start the data acquisition
		else if ( !SendCommand("START\r") ) {
			bBusy = false;
		}
		else {
			SendCommand("ENABLE_OVERFLOW_PRESET");
			bBusy = true;
		}
	}
	return bBusy;
}

/*++ stop data acquisition on MCA */
bool CMcb::StopAcquire() {

	if ( !bOpen ) {
		return false;
	}
	if ( !bSimulate ) {
		SendCommand("STOP");
	}
	bBusy = false;
	return true;
}

/*++ suspend counting */
bool CMcb::Suspend() {
	if ( !bOpen ) {
		return false;
	}
	if ( bBusy ) {
		StopAcquire();
		dSaveTime = dElapsedTime;
	}
	bSuspended = true;
	return true;
}

//*************************************************************************		

/*++ retrieve an integer value from the MCB */
bool CMcb::GetInt(char *Cmd, int *Value) {
	if ( !Query(Cmd) ) {
		return false;
	}

	if ( szReply[1]!='C' ) {
		iStatus = CMCB_RSP_GARBLE;
		LogEvent(true, "GetInt");
		return false;
	}

	szReply[7] = '\0';  // cover up the checksum
	*Value = atoi(&szReply[2]);
	return true;
}

/*++ retrieve the preset time and mode */
bool CMcb::GetPreset(bool *Live, long *Time) {
	bool     bOk=true;
	ULONG    uTime;

	if ( !GetULong("SHOW_LIVE_PRESET", &uTime) ) {
		return false;
	}
	if ( uTime!=0 ) {
		*Live = true;
		*Time = uTime;
		return true;
	}
	if ( !GetULong("SHOW_TRUE_PRESET", &uTime) ) {
		return false;
	}
	if ( uTime!=0 ) {
		*Live = false;
		*Time = uTime;
		return true;
	}
	iStatus = CMCB_NO_PRESET;
	LogEvent(true, "GetPreset");
	return false;
}

/*++ ? */ 
bool CMcb::GetString(char *Cmd, int Max, char *Buffer) {
	if ( !Query(Cmd) ) {
		return false;
	}
	if ( szReply[1]!='F' ) {
		iStatus = CMCB_RSP_GARBLE;
		LogEvent(true, "GetString");
		return false;
	}
	else {
		int   iMax;
		iMax = (Max<iLength) ? Max : iLength;
		strncpy(Buffer, szReply, iMax);
		Buffer[iMax] = '\0';
	}
	return true;
}

/*++ ? */ 
bool CMcb::GetTriple(char *Cmd, int *Value1, int *Value2, int *Value3) {
	return true;
}

/*++ ? */ 
bool CMcb::GetULong(char *Cmd, ULONG *Value) {
	if ( !Query(Cmd) ) {
		return false;
	}
	if ( szReply[1]!='G' ) {
		iStatus = CMCB_RSP_GARBLE;
		LogEvent(true, "GetULong");
		return false;
	}
	szReply[12] = '\0';  // cover up the checksum
	*Value = atol(&szReply[2]);
	return true;
}

/*++ log given event */
void CMcb::LogEvent(bool Error, char *Desc) {
	CDatTim   DT;
	ULONG   ulError=0;
	FILE *    Fptr;

	if ( !bLogErrors ) {
		return;
	}
	Fptr = fopen(szLogFile, "a");
	if ( Fptr!=(FILE *)0 ) {
		if ( Error ) {
			fprintf(Fptr, "E %d.%02d.%02d %02d:%02d:%02d  (%s) %s  %d,%d,%d\n",DT.Year(), DT.Mon(), DT.Day(), DT.Hour(),
				DT.Min(), DT.Sec(), Desc, ErrorMessage(), iError, iErrorMacro, iErrorMicro);
		}
		else {
			fprintf(Fptr, "  %d.%02d.%02d %02d:%02d:%02d  %s\n", DT.Year(), DT.Mon(), DT.Day(), DT.Hour(), DT.Min(),
				DT.Sec(), Desc);
		}
		fclose(Fptr);
	}
}

/*++ ? */ 
bool CMcb::Query(char *Cmd) {
	if ( !SendCommand(Cmd) ) {
		return false;
	}
	if ( iLength<2 || szReply[0]!='$' || (szReply[iLength-1]!='\r'  &&  szReply[iLength-1]!='\n') ) {
		iStatus = CMCB_RSP_GARBLE;
		LogEvent(true, "Query");
		return false;
	}
	return true;
}

/*++  send command to MCB and receive the response */
bool CMcb::SendCommand(char *Cmd) {
	int     iNumTries=0;
	bool    bOk=false;
	bool    bRcvd=false;

	ClearError();
	if ( !MIOIsDetector(hDet) ) {
		iStatus = CMCB_BAD_HANDLE;
		iError = MIOGetLastError(&iErrorMacro, &iErrorMicro);
		LogEvent(true, "MIOIsDetector");
		return false;
	}
	while ( !bRcvd  &&  iNumTries<CMCB_MAX_TRIES ) {
		szReply[0] = '\0';
		if ( !MIOComm(hDet, Cmd, "", "", CMCB_LEN_REPLY, szReply, &iLength) ) {
			iError = MIOGetLastError(&iErrorMacro, &iErrorMicro);
			if ( iError!=MIOEIO || (iErrorMacro!=MIOEMACTIMEOUT  &&  iErrorMacro!=MIOEMACCOMM) ) {
				bRcvd = true;
			}
		}
		else {
			bRcvd = true;
			bOk = true;
		}
		iNumTries++;
	}
	if ( !bOk ) {
		LogEvent(true, Cmd);
	}
	return bOk;
}

/*++ ? */ 
bool CMcb::SetDateTime(void) {
	CDatTim   DT;

	sprintf(szCommand, "SET_DATE %d, %d, %d", DT.Day(), DT.Mon(), DT.Year()%100);
	if ( !SendCommand(szCommand) ) {
		return false;
	}

	sprintf(szCommand, "SET_TIME %d, %d, %d", DT.Hour(), DT.Min(), DT.Sec());
	if ( !SendCommand(szCommand) ) {
		return false;
	}
	return true;
}
/*EO CMcb */