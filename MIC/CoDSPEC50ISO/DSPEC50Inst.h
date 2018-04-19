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
//DSPEC50Inst.h

#ifndef DSPEC50INST_H
#define DSPEC50INST_H

#include "COM_Utilities.h"
#include "DSPEC50DataStat.h"
#include "DSPEC50ModParam.h"
#include "DSPEC50SafePS.h"
#include "DSPEC50Legend.h"
#include "DSPEC50Summary.h"
#include "DSPEC50ProppageSetupData.h"
#include "ORTECSpectrum.h"
#include "DSPEC50Strings.h"

#include "ISO.h"                 //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#include <commctrl.h>

#define DSPEC50_DISCOVERY_TYPE "DSPEC50"

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50Inst window

class CDSPEC50Inst : public CISO
{
	//MIC_CB_028 to MIC 1.907 PJM 1/19/05 - added these friend classes
	friend class CDSPEC50SafePropertySheet;
	friend class CDSPEC50DataStat;
	friend class CDSPEC50Modparam;

// Construction
public:
	CDSPEC50Inst();
	CDSPEC50Inst(CWnd* Parent, char *inifile);

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
	bool UnRegInstrument();
private:
	void AdjustShutdownAndStartupBooleans();
	bool DwellSettingIsShort();
	bool DwellSettingIsLong();
	void GetListsFromInstrument();
	void StartNewShortDwell(bool resetmemory);
	void TransmitAndUpdateSummary(char *command);	
	void AcknowledgeInputFromCSO();
	void Query_CSO();
	void RecoverFromFailure();
	void SendSetupCommandsToDSPEC50();  //MIC 1.907a
	void KillAllTimers();

	// Grouped commands for UpdateParams() processing
	void SendShapeCommandsToDSPEC50(bool resetting);
	void SendStandardGainCommandsToDSPEC50(bool resetting);
	void SendStabilizationCommandsToDSPEC50(bool resetting);
	void SendHVCommandsToDSPEC50(bool resetting);
	void SendPoleZeroCommandsToDSPEC50(bool resetting);
	void SendOtherCommandsToDSPEC50(bool resetting);

	bool m_bIsPaused;
	bool m_bRealtimeWatcherDataQuiet;
	bool m_bCommsFailure;
	bool m_bHVVoltageLevelFailure;
	bool m_bHVShutdownFailure;

	COleDateTime m_cLatestReportedColumnUpdate;

	CDSPEC50Summary			*m_pSummaryPage;
	CDSPEC50DataStat			*m_pDataStatus;
	CDSPEC50ModParam			*m_pModParams;
	CDSPEC50ProppageSetupData	*m_pSetupData;
	CLegend					*m_pLegend;
	CDSPEC50SafePropertySheet	*m_pPropertySheet;

	COleDateTime			m_pNow;
	DSPEC50_PARAMETERS		m_SentParameters;
	char					m_cCurrent_Tag;
	CWnd					*m_pParent;

	DWELL_TYPE				m_eDwellType;

	int	 m_iCycleInt_ms;			//for TIMER_TICK
	int	 m_iHVFail;
	int	 m_iLongdwell_sec;
//	int	 m_iPauseMax_ms;			//for TIMER_PAUSE	//dgp 18 Oct 2006
	int	 m_iPreviousDay;
	int	 m_iRealtimedwell_sec;
	int	 m_iSecToGo;             //Until End Of Dwell
	int	 m_iShortdwell_sec;

	int	 m_iSlowReset_sec;
	int	 m_iSlowResetCount_sec;

	int	 m_iShortTermTimeouts;
	int	 m_iShortTermTimeoutRunningCounter;

	int	 m_iStatusInt_sec;
	int	 m_iTickToStatus;

	ULONG m_ulThreshold;

	int	 m_iCommFail;  //timeouts allowed before hard fail
	int	 m_iTimeOut;   //length of a timeout (seconds)

	int m_iSecondsElapsedSinceReceivingData;

	int	 m_ihvvolth;
	int	 m_ihvvoltl;

///////////////////////////////////////////////////
//
//              STATE DEFINITIONS
//
///////////////////////////////////////////////////
enum DSPEC50_STATE
{
	INITIALIZING_1000                        = 1000,
	FIRST_NON_STARTUP_STATE_1100             = 1100,

	WAITING_WITHIN_SHORT_DWELL_INTERVAL_2000 = 2000,
	EXPECTING_SHORT_DWELL_SPECTRUM_DATA_2010 = 2010,

	WAITING_WITHIN_LONG_DWELL_INTERVAL_3000  = 3000,
	EXPECTING_LONG_DWELL_SPECTRUM_DATA_3010  = 3010,

	EXPECTING_MANUAL_WRITE_SPECTRUM_DATA_4000 = 4000,

	SHORT_TERM_HARD_FAIL_9000          = 9000,	//9000 and above are "in shutdown"
	LONG_TERM_HARD_FAIL_9500           = 9500,	// Doing Slow reset
	LOCKED_UP_9900                     = 9900
};

	DSPEC50_STATE	m_dCurrent_State;
	DSPEC50_STATE m_dSave_State;
	void SetStateAndButtonColor(DSPEC50_STATE ds);
/////////////////////////////////////////
/////////////////////////////////////////

#ifdef ALLOWTRIGGERHOLDOFF
//	void ReadExclusion();
//	void WriteExclusion();
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
	//{{AFX_VIRTUAL(CDSPEC50Inst)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDSPEC50Inst();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDSPEC50Inst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRequestSettingsUpdates(WPARAM, LPARAM);
	afx_msg void OnUpdateParams(WPARAM, LPARAM);
	afx_msg void OnDesiredViceCurrentMismatch(WPARAM,LPARAM);
	afx_msg void OnClearAndRestartSpectrum(WPARAM,LPARAM);
	afx_msg void OnManualWriteToCHNFile(WPARAM,LPARAM);
	afx_msg void OnHVVoltageLevelFail(WPARAM,LPARAM);
	afx_msg void OnHVVoltageLevelRecover(WPARAM,LPARAM);
	afx_msg void OnHVShutdown(WPARAM,LPARAM);
	afx_msg void OnHVShutdownRecover(WPARAM,LPARAM);
	afx_msg void OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // DSPEC50INST_H
