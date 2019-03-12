/*
This work was supported by the United States Member State Support Program to IAEA Safeguards;
the U.S. Department of Energy, Office of Nonproliferation and National Security, International
Safeguards Division; and the U.S. Department of Energy, Office of Safeguards and Security.

LA-CC-14-089. This software was exported from the United States in accordance with the Export
Administration Regulations. Diversion contrary to U.S. law prohibited.

© 2019 Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL),
which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration.

All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration.
The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce,
prepare derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.

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
// HHMRInst.h

#ifndef HHMRINST_H
#define HHMRINST_H

//CHHMRInstrument Class Header File

#include "COM_Utilities.h"
#include "HHMRSafePS.h"
#include "HHMRSummary.h"
#include "HHMRDataStat.h"
#include "HHMRModParam.h"
#include "HHMRLegend.h"

//#include "ParallelPort.h"

#include "ISO.h"                 //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define HHMR_DISCOVERY_TYPE "HHMR"

/////////////////////////////////////////////////////////////////////////////
// CInstrument window

class CHHMRInst : public CISO
{
friend class CHHMRModParam;
friend class CHHMRPropertySheet;
//friend class CHHMRConfig2;

// Construction
public:
	CHHMRInst(CWnd* Parent, char *inifile);
	CHHMRInst();
	virtual ~CHHMRInst();
	void FinishConstruction();

// Attributes
public:
	bool  CollectParametersForNewISO(CString& csNameOut);
	bool  CreateInstrument(void);
	void  ShowInstrument(bool);
	void  PauseInstrument(bool);
	bool  UpdateParams(bool);
	void  UpdateStrings();
	void  DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	int	  GetCurrentBBM(){return 0;}
	void  IssuePrint(FILE* outfile);
	void  Reset();
	void SetLocalRegisteredMessageValues();
	void FlagTime(int,bool){;}  //there is no FlagTime for the HHMR

	//bool SetupParallelPort(bool reseting);

	CHHMRSummary		*m_pSummaryPage;
	CHHMRDataStat		*m_pDataStatus;
	CHHMRModParam		*m_pModParams;
	//CHHMRConfig2		*m_pCamera;
	CLegend				*m_pLegend;
	CHHMRPropertySheet	*m_pPropertySheet;

	bool		m_bIsPaused;//caution on using this var
	bool		m_bLogFilter;
	bool		m_bInDumpLast;
	int			m_dSlowReset;

private:

	int			m_dCurrent_State;
	int			CalculateDiff(SYSTEMTIME *Time1, SYSTEMTIME *Time2);
	bool		m_bCameraBlocked;

	CWnd*		m_pParent;
	unsigned int   m_dPrevHHMRTime;
	int			m_dCycleInt;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	int			m_dInquireIntAdjust;
	int			m_dTransmitXCount;
	int			m_dCommFail;
	int			m_dTimeOut;
	int			m_dHighVoltH;
	int			m_dHighVoltL;

	double		m_lSigmaThresh;
	double		m_lRealThresh;
	double		m_lTotalThresh;
	double		m_lTotal1Thresh;
	double		m_lTotalLThresh;
	int			m_dMaxCompress;

	SYSTEMTIME  m_Then;

	char		m_cCameraRealorTotals[4];
	COleDateTimeSpan m_pCameraHoldOff;
	COleDateTimeSpan m_pCameraSuppress;
	COleDateTime m_pSuppressUntil;
	SYSTEMTIME	m_sStartTime;

	bool		m_bCameraHoldingOff;

	int			m_dCountTimeUnit;
//	int			m_dCountTimeExp;
	int			m_dHighV;
	int			m_dGateWidth;
	int			m_dPreDelay;

	int			m_dRetransmit;
	int			m_dLongBreak;
	bool		m_bInHardFail;
	bool		m_bDriveToZero;

//	ParallelPort	m_cParallelPort;
//	int				m_dParallelPortPin;
//	int			m_dInquireIntOffset;
	int			m_iCycleCount;
	COleDateTime m_cStart;
	COleDateTime m_cFinish;
	COleDateTimeSpan m_cSpan;
	DWORD		m_dSpan;
	DWORD		m_dFinish;
	DWORD		m_dStart;
// Operations
public:
	//void	SetCamera(bool);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHMRInst)
	//}}AFX_VIRTUAL

// Implementation
public:


	// Generated message map functions
protected:
	//{{AFX_MSG(CHHMRInst)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnReceive(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif HHMRINST_H
