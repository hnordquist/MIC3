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
#ifndef HHMRSAFEPS_H
#define HHMRSAFEPS_H

// HHMRSafePS.h : header file

#include "ccbutton.h"
#include "JulianT.h"

#define HHMRACQUIRERECCCOUNT			32
#define ACCUMSIZE					2048
#define EOL1						0x0a
#define EOL2						0x0d

#define RESETUP						(0xfa)

#define HHMR_READ_STATUS_LENGTH      1
#define HHMR_READ_AUX_SCALERS_LENGTH 10
#define HHMR_READ_HV_SP_LENGTH       2
#define HHMR_READ_HV_LENGTH          2
#define HHMR_READ_COUNTTIME_LENGTH   3
#define HHMR_READ_T_TA_A_LENGTH      21
#define HHMR_READ_PREDELAY_LENGTH    2
#define HHMR_READ_GATEWIDTH_LENGTH   2

#define HHMR_READ_STATUS			25001
#define HHMR_READ_AUX_SCALERS	25002
#define HHMR_READ_HV_SP          25003
#define HHMR_READ_HV             25004
#define HHMR_READ_COUNTTIME      25005
#define HHMR_READ_T_TA_A         25006
#define HHMR_READ_PREDELAY       25007
#define HHMR_READ_GATEWIDTH      25008
#define HHMR_UNKNOWN_RECORD      25099



//ID2 Status Byte One
#define ID21_ACPOWER		0x0080
#define ID21_BATTERY		0x0040
//#define ID21_BBMERROR		0x0008

#pragma pack(2)
struct HHMRAcquireRecord
{
	unsigned int	JulianSeconds;	//4 bytes
	unsigned short	StatusByte;		//2 bytes
	double			TotalsCount;	//8 bytes
	double			Totals2Count;	//8 bytes
	double			Totals3Count;	//8 bytes
	double			RealAccCount;	//8 bytes
	double			AccidenCount;	//8 bytes
	double			ElapsedTime;	//8 bytes 1/10 sec increments
};

#pragma pack(1)
struct HHMRAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
//	char	unused2[47];
// or replace unused2 with...
	char	Year4[4];
	char	unused2[43];
};
#pragma pack()

class CHHMRInst;
/////////////////////////////////////////////////////////////////////////////
// CHHMRPropertySheet

class CHHMRPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CHHMRPropertySheet)

// Construction
public:
	CHHMRPropertySheet(
		UINT nIDCaption, 
		CHHMRInst* pParentWnd,
		char *inifile); 

	CHHMRPropertySheet(
		LPCTSTR pszCaption, 
		CHHMRInst* pParentWnd,
		char *inifile); 

	void FinishConstruction();

	virtual ~CHHMRPropertySheet();

// Attributes
public:

// Operations
public:
	COLORREF SetButtonColor(COLORREF Color);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL);
	bool WriteToHHMRFile(CJulianTime* Time, HHMRAcquireRecord* AcquireRec);
	void SetTabColor(int Tab, COLORREF Color);

	void SetLimits(
		double lRealThresh,
		double lTotalThresh,
		double lTotal1Thresh,
		double lTotalLowThresh,
		double lSigmaThresh,
		int	dMaxCompress,
		int	dCountTimeUnit,
//		int dCountTimeExp,
		int	dHighV,
		int	dGateWidth,
		int	dPreDelay,
		int	dHighVoltH, int dHighVoltL,
//		char cCameraRealorTotals[4],
//		bool bCameraDo,
		char * pId);

	void IssuePrint(FILE* outfile);

	CString *GetVersionString();
	//void SetCamera();

	virtual void SetToClose(int set);

	void	UpdateSummary(char *);
	void    RestartAutoCloseTimer(){m_dToClose = 0;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHHMRPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	COleDateTime	m_LastStart;

	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CHHMRInst		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentHHMRFileName[_MAX_PATH];
	char			m_szIniFile[_MAX_PATH];

	char			m_cCameraRealorTotals[4];
	bool			m_bCameraDo;

	int				m_dAt;
	int				m_dAcquireRecordCount;
	int				m_dMessageRecordCount;

	struct HHMRAcquireRecord HHMRAcquireRec;
	CJulianTime		AcquireTime; 

	void PrepForReceipt(int iTypeOfMessageExpected, bool bClearAccumulator);

protected:
	int General_Tab;
	int Data_Tab;
	int UserProg_Tab;

	void FinishInstance();

	void StatusRec(BYTE *pAccumulator);
	void AuxRec(BYTE *pAccumulator);
	void HVSetPoint(BYTE *pAccumulator);
	void HVValue(BYTE *pAccumulator);
	void CountTime(BYTE *pAccumulator);
	void T_TA_AValues(BYTE *pAccumulator);
	void PreDelay(BYTE *pAccumulator);
	void GateWidth(BYTE *pAccumulator);

/*
	void StatusRec(char *);
	void StatusRecErr(char *);
	void HighVRec(char *);
	void VersionRec(char *);
	void SetupRec(char *);
	void AuxRec(char *);
	void ReadRec(char *);
*/
	void MakeMyPath(const char*);
	HHMRAcquireRecord SumAcquireRec;

	int m_dCompressCount;
	bool m_bOverThresh;
	bool m_bRegisteredZero;
	bool m_bZeroTransitionUp;
	bool m_bZeroTransitionDown;
	int m_dBackgroundCount;

	bool m_bHVSetPointStatusFail;
	bool m_bCountTimeStatusFail;
	bool m_bPreDelayStatusFail;
	bool m_bGateWidthStatusFail;

	double m_lSigmaThresh;
	double m_lRealThresh;
	double m_lTotalThresh;
	double m_lTotal1Thresh;
	double m_lTotalLThresh;
	int	m_dMaxCompress;
	int	m_dCountTimeUnit;
//	int m_dCountTimeExp;
	int	m_dHighV;
	int	m_dGateWidth;
	int	m_dPreDelay;

	int m_iBytesToWaitFor;
	int m_iTypeOfMessageExpected;

	double m_fLastAux1;
	double m_fLastAux2;
	double m_fInquireInt;

	int   m_dLowHV;
	int	  m_dCurHV;
	int	  m_dHighHV;
	bool  m_bFirstHV;

	int	  m_dHighVoltH;
	int	  m_dHighVoltL;
	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
//	int   m_dCurrentDay;
//	int	  m_dCurrentHour;
//	int   m_dCurrentMinute;

//d	bool  m_bStartOfFile;

	bool  m_bHighVoltOk;
	bool  m_bNewFileName;
	bool  m_bDataFiltered;
//	char  m_szPreviousGTime[16]; 
//	int	  m_dPrevHHMRHour;
	COleDateTime m_PreviousSetup;
	unsigned char  m_pAccumulator[ACCUMSIZE];
	char  m_pCaption[256];

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

	//{{AFX_MSG(CHHMRPropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg LRESULT OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg LRESULT OnExtendClose(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // HHMRSAFEPS_H
