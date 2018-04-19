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
// ISRInst.h 
#ifndef ISRINST_H
#define ISRINST_H

#include "COM_Utilities.h"
#include "ISRSafePS.h"
#include "ISRGenlStat.h"
#include "ISRDataStat.h"
#include "ISRUserProg.h"
#include "ISRCDData.h"
#include "ISRModParam.h"
#include "ISRLegend.h"
#include "ISRSummary.h"

#include "ISO.h"   //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define BUSY		  2
#define NOTBUSY		  0

#define ISR_DISCOVERY_TYPE "ISR"

/////////////////////////////////////////////////////////////////////////////
// CISRInst window

class CISRInst : public CISO
{
// Construction
public:
	CISRInst(CWnd* Parent, char *inifile);
	CISRInst();
	virtual ~CISRInst();
	void FinishConstruction();

// Attributes
public:
	bool CollectParametersForNewISO(CString& csNameOut);
	bool CreateInstrument(void);
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void UpdateStrings();
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();

#ifdef ALLOWTRIGGERHOLDOFF
	void AddExclusion(LPARAM lParam);
	void DelExclusion();
#endif

	CISRSummary		*m_pSummaryPage;
	CISRGenlStat	*m_pGeneralStatPage;
	CISRDataStat	*m_pDataStatus;
	CISRUserProg	*m_pUserProgStatus;
	CISRCDData		*m_pCDData;
	CISRModParam	*m_pModParams;
	CLegend			*m_pLegend;
	CISRPropertySheet	*m_pPropertySheet;

	COleDateTime	m_pToday;
	bool		m_bIsPaused;//caution on using this var
	bool		m_bLogMII;
	bool		m_bLogFilter;
	bool		m_bInDumpBBM;

	int			m_dSave_State;
	COLORREF	m_crSave_Color;
//	int			m_dCurrent_BBM;		//dgp 18 Oct 2006
	int			m_dSlowReset;
	int			m_dNumberOfAcquires;
	int			m_dNumberOfMessages;
	int			m_dNumberOfFiles;
	int			m_dBreakSentCount;
	int			m_dTimeOutCount;
	int			m_dInvalidTimeCount;

	char		m_szAutoTimeDailyAt[16];
	unsigned	m_uiSecInDayAutoTime;
	bool		m_bAutoTimeDaily;
	bool		m_bAutoTimeDailyTrigger;
	bool		m_bDoOneDumpStat;

private:
	int			m_dCurrent_State;
	CWnd*		m_pParent;
	unsigned int   m_dPrevISRTime;
	int			m_dStatusInt;
	int			m_dCycleInt;
	int			m_dMaxBBM;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	int			m_dTickToStatus;
	int			m_dCommFail;
	int			m_dTimeOut;
	bool		m_bIsAMSR;
	int			m_dHighVoltH;
	int			m_dHighVoltL;
	float		m_fbatvolth;
	float		m_fbatvoltl;
	float		m_fsup_p15h;
	float		m_fsup_p15l;
	float		m_fsup_m15h;
	float		m_fsup_m15l;
	float		m_fsup_p05h;
	float		m_fsup_p05l;
	int			m_dtime_err;
	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bdoingBBM;
	bool		m_bdumpOK;
	bool		m_bChecksumFail;
	int			m_dChecksumCount;
	bool		m_bInHardFail;
	int			m_dHardFailCount;
	int			m_iLastRecordWritten;
	bool		m_bInitInitTimeOut;
	bool		m_bDriveToZero;
	bool		m_bDoAnalyzeAfterDT0;
	bool		m_bbbmVal;
	bool		m_bAllowAutoTime;
	bool		m_bDoingAutoTimeSet;
	int			m_dLastCommand;
	int			m_dPreviousDay;
	int			m_dBusyCount;
	COleDateTime m_pNow;
#ifdef ALLOWTRIGGERHOLDOFF
	void ReadExclusion();
	void WriteExclusion();
	typedef struct EXCLUSIONRECTAG{
		unsigned int uiStartTime;
		unsigned int uiEndTime;
	} EXCLUSIONREC;
	CList< EXCLUSIONREC, EXCLUSIONREC > m_ExclusionList;
#endif
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISRInst)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CISRInst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif  //ISRINST_H
