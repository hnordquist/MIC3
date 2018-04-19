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
/* cmcb.h   definition of the ORTEC MCB class */

#ifndef CMCB_H
#define CMCB_H

//#include  "cchnspc.h"
#include "gamnew.h"
#include  "cmca.h"

#define CMCB_MAX_TRIES          10
#define CMCB_LEN_COMMAND        79
#define CMCB_LEN_REPLY         127
#define CMCB_TICKS_PER_SECOND   50

enum CMCB_STATUS {
	CMCB_OK,                // Success.
	CMCB_VERSION_MISMATCH,	// Orw.lib and Ors.h version mismatch.
	CMCB_NOT_OPEN,          // OrwOpen hasn't been called.
	CMCB_ALREADY_OPEN,      // OrwOpen has already been called.
	CMCB_INIT_FAIL,         // error calling MIOStartup.
	CMCB_BUSY,              // the detector is currently acquiring data.
	CMCB_NOT_BUSY,          // the detector isn't currently acquiring data.
	CMCB_STILL_BUSY,        // the detector is still busy after STOP.
	CMCB_UMCBI_LIST,        // couldn't access the master list.
	CMCB_RSP_GARBLE,        // response from detector was garbled.
	CMCB_UMCBI_ERROR,       // error encountered; details in pCtx.
	CMCB_IDETIDX_TOO_BIG,   // iDetIndex is outside range of master list.
	CMCB_UMCBI_OPEN,        // MIOXxx error trying to open connection.
	CMCB_NO_PRESET,         // error in retrieveing preset.
	CMCB_PRESET_REACHED,    // on START, preset time already exceeded.
	CMCB_DUMMY_FILE_IO,     // I/O error with dummy file.
	CMCB_SPW_ERR,           // error return from SpwXxx routine.
	CMCB_BAD_INPUT,         // a function input check failed.
	CMCB_BAD_INDEX,         // invalid index into detector list.
	CMCB_BAD_HANDLE,        // invalid detector handle.
	CMCB_ILLEGAL
};

class CMcb : public CMca
{
public:
	// constructor
	CMcb(char * DetName, DWORD DetId, CSpctrm *pSpecInfo);
	
	// destructor
	~CMcb();

	virtual bool       CheckStatus(void);
	virtual bool       Close(void);
	virtual DWORD      DetId(void);
	virtual bool       GetDetectorInfo(int Index, DWORD *Id, int StrMax,
					char *Desc, int *DetMax);
	virtual bool       GetSpectrum(void);
	virtual bool       IoCtl(void *Input=(void *)0);
	virtual bool       IsValid(void);
	virtual bool       Open(bool Simulate=false, char *File=(char *)0);
	virtual bool       Resume(void);
	virtual bool       SaveSpectrum(char *File, void *Input=(void *)0);
	virtual void       SetDetId(DWORD Id);
	virtual CChnSpc * pSpctrm(void);
	virtual bool       StartAcquire(void *Input=(void *)0);
	virtual bool       StopAcquire(void);
	virtual bool       Suspend(void);

protected:
//private: // <<QA>> So that CMICMcb can derive from CMcb  (PJM 2/19/2003)
	char      szCommand[CMCB_LEN_COMMAND+1];
	DWORD      lDetId;
	HDET       hDet;
	int        iError;
	int        iErrorMacro;
	int        iErrorMicro;
	int        iLength;
	bool       bLogErrors;
	char *    szLogFile;
	char      szReply[CMCB_LEN_REPLY+1];
	long       lSimLiveTime;
	long       lSimTrueTime;
	bool       bSpecInfo;
	CSpctrm * pxSpecInfo;
	CChnSpc *  pSpec;
	int        iStatus;
	bool       bValid;

	virtual void  ClearError(void);
	virtual bool  GetInt(char *Cmd, int *Value);
	virtual bool  GetPreset(bool *Live, long *Time);
	virtual bool  GetString(char *Cmd, int Max, char *Buffer);
	virtual bool  GetTriple(char *Cmd, int *Value1, int *Value2, int *Value3);
	virtual bool  GetULong(char *Cmd, ULONG *Value);
	virtual void  LogEvent(bool, char *);
	virtual bool  Query(char *Cmd);
	virtual bool  SendCommand(char *Cmd);
	virtual bool  SetDateTime(void);
};

#endif
