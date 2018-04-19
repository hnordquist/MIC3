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
#ifndef ISRSAFEPS_H
#define ISRSAFEPS_H

// ISRSafePS.h : header file
#include "ccbutton.h"
#include "JulianT.h"

#define NUMBEROFACQUIRERECS			32
#define ACCUMULATOR_SIZE			2048
#define EOL1						0x0a
#define EOL2						0x0d

#define ISRSTATUS_RECORD			(0x21)
#define ISRACQUIRE_RECORD			(0x20)
#define ISREND_DUMP_RECORD			(0x16)
#define ISREND_DUMP2_RECORD			(0x61)
#define ISREND_DUMP3_RECORD			(0x62)
#define ISRBUSY_RECORD				(0x24)
#define ISRDUMP_OK_RECORD			(0x17)
#define ISRBATTERY_RECORD			(0x18)
#define ISRDUMP_OK2_RECORD			(0x19)
#define ISRMON_STATUS_RECORD		(0x22)
#define ISRTRIG_SIG_RECORD			(0x23)
#define ISRMONITOR_ID2_RECORD		(0x1e)
#define ISRANALYZE_OK_RECORD		(0x1f)
#define ISRINFORMATION_RECORD		(0x30)
//these don't actually correspond to the first chars of a message
#define UNKNOWN_REC					(0xf8)
#define UNKNOWN_REC_CHAR			"f8"
#define FORCEDATETIME				(0xfa)
#define ISR_ISAMSR					(0xfc)  //to see if it is used
#define ISR_Q0_RECORD				(0xfd)
#define ISR_Q1_RECORD				(0xfe)
#define ISR_Q2_RECORD				(0xff)

#define ISRSTATUS_RECORD_LENGTH			78
#define ISRACQUIRE_RECORD_LENGTH		88
#define ISREND_DUMP_RECORD_LENGTH		6
#define ISREND_DUMP2_RECORD_LENGTH		7
#define ISREND_DUMP3_RECORD_LENGTH		8
#define ISRBUSY_RECORD_LENGTH			7
#define ISRDUMP_OK_RECORD_LENGTH		12
#define ISRDUMP_OK2_RECORD_LENGTH		14
#define ISRBATTERY_RECORD_LENGTH		20
//#define ISRMON_STATUS_RECORD_LENGTH		131
//#define ISRMON_STATUS_RECORD_LENGTH		163
#define ISRMON_STATUS_RECORD_LENGTH		198
#define ISRTRIG_SIG_RECORD_LENGTH		19
#define ISRMONITOR_ID2_RECORD_LENGTH	28
#define ISRANALYZE_OK_RECORD_LENGTH		12
#define ISRINFORMATION_RECORD_LENGTH	34

//ID2 Status Byte One
#define ID21_ACPOWER		0x0080
#define ID21_BATTERY		0x0040
#define ID21_BBMERROR		0x0008

//ID2 Status Byte Two
#define ID22_FILTER			0x0080

#define ID22_BBMFILLED		0x0040
#define ID22_BBMFILLED_TXT	"BBM is filled"
#define ID22_BBMNUMBBAD		0x0020
#define ID22_BBMNUMBBAD_TXT	"BBM contains incorrect number of bytes for Opcode"
#define ID22_BBMWRITEERR	0x0010
#define ID22_BBMWRITEERR_TXT "Could not write value to BBM"
#define ID22_BBMINVOPCOD	0x0000
#define ID22_BBMINVOPCOD_TXT "Invalid Opcode read from BBM"

#define ID22_LONOK			0x0008

//ACQUIRE SR & STATUS SR Status Byte
#define AS_ACPOWER		0x0080
#define AS_BATTERY		0x0040
#define AS_ACLOSS		0x0020

#pragma pack(2)
struct ISRAcquireRecord
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
struct ISRAcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
// or replace unused2 with...
//	char	Year4[4];
//	char	unused2[43];
};
#pragma pack()

class CISRInst;
/////////////////////////////////////////////////////////////////////////////
// CISRPropertySheet

class CISRPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CISRPropertySheet)

// Construction
public:
	CISRPropertySheet(
		UINT nIDCaption, 
		CISRInst* pParentWnd);

	CISRPropertySheet(
		LPCTSTR pszCaption, 
		CISRInst* pParentWnd);

	void FinishConstruction();

	virtual ~CISRPropertySheet();

// Attributes
public:

// Operations
public:

	COLORREF SetButtonColor(COLORREF Color);
	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL, bool bStale = false);
	bool WriteToISRFile(CJulianTime* Time, ISRAcquireRecord* AcquireRec);
	void SetTabColor(int Tab, COLORREF Color);
	void SetLimits(
		float batvolth, float batvoltl,
		float sup_p15h, float sup_p15l,
		float sup_m15h, float sup_m15l,
		float sup_p05h, float sup_p05l,
		int   dHighVoltH, int dHighVoltL,
		int   time,		char * pId);

	void IssuePrint(FILE* outfile);

	CString *GetVersionString();

	virtual void	SetToClose(int set);
	virtual void	ForceDateTime(void);
	void	UpdateSummary(char *);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISRPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CISRInst		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentISRFileName[_MAX_PATH];

	int				m_dAt;
	int				m_dAcquireRecordCount;
	int				m_dMessageRecordCount;

	struct ISRAcquireRecord ISRAcquireRec[NUMBEROFACQUIRERECS];
	CJulianTime		AcquireTime[NUMBEROFACQUIRERECS]; 

	// Generated message map functions
protected:
	int General_Tab;
	int Data_Tab;
	int UserProg_Tab;

	void ResponseRec(char *);
	void StatusRec(char *);
	void AcquirRec(char *);
	void DumpEnd(char *);
	void DumpEnd2(char *);
	void DumpEnd3(char *);
	void DumpEnd4(char *);
	void DumpOk(char *);
	void DumpOk2(char *);
	void BusyRec(char *);
	void BatteryRec(char *);
	void MonStatRec(char *);
	void TrigSigRec(char *);
	void ID2_Record(char *);
	void AnalyzeOk(char *);
	void InformRec(char *);
	void Echo_0_Rec(char *);
	void Echo_1_Rec(char *);
	void Echo_2_Rec(char *);
	int Checksum(char *);
	bool LengthOk(int length, char* Record);
	char *StripLeadingBlanks(char *);
	void SetChecksumDisplay();

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

	unsigned int m_uiStatusRecTime;		//DR00406
	unsigned int m_uiMonStatRecTime;		//DR00406

	float m_fBatVoltH; 
	float m_fBatVoltL;
	float m_fLowBV;
	float m_fCurBV;
	float m_fHighBV;
	bool  m_bFirstBV;
	
	float m_fLowP15;
	float m_fCurP15;
	float m_fHighP15;
	bool  m_bFirstP15;

	float m_fLowM15;
	float m_fCurM15;
	float m_fHighM15;
	bool  m_bFirstM15;

	float m_fLowP5;
	float m_fCurP5;
	float m_fHighP5;
	bool  m_bFirstP5;

	float m_fSup_P15H; 
	float m_fSup_P15L;
	float m_fSup_M15H; 
	float m_fSup_M15L;
	float m_fSup_P05H; 
	float m_fSup_P05L;
	long  m_dTime_Err;

	int   m_dLowHV;
	int	  m_dCurHV;
	int	  m_dHighHV;
	bool  m_bFirstHV;

	int	  m_dHighVoltH;
	int	  m_dHighVoltL;
	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
	int   m_dCurrentDay;

	bool  m_bStartOfFile;

	bool  m_bHighVoltOk;
	bool  m_bBatteryOk;
	bool  m_bBatteryTolOk;
	bool  m_bP5VoltOk;
	bool  m_bP15VoltOk;
	bool  m_bM15VoltOk;
	bool  m_bACPowerOk;
	bool  m_bACPowerLoss;
	bool  m_bTimeError;
	bool  m_bBBMOk;
	bool  m_bLonOk;
	bool  m_bNewFileName;
	bool  m_bDataFiltered;
	char  m_szPreviousGTime[16]; 
	int	  m_dPrevISRHour;

	char  m_pAccumulator[ACCUMULATOR_SIZE];
	char  m_pCaption[256];
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	//{{AFX_MSG(CISRPropertySheet)
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
	afx_msg void OnReceive(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg void OnExtendClose(WPARAM,LPARAM);  //MIC_CB_028 to MIC 1.907 PJM 1/26/05
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //ISRSAFEPS_H
