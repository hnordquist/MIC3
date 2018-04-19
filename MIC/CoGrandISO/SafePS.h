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
// SafePS.h

#ifndef SAFEPS_H
#define SAFEPS_H

// SafePS.h : header file

#include "ccbutton.h"
#include "JulianT.h"
#include "DataWatchDlg.h"

#define NUMBEROFACQUIRERECS		32
#define ACCUMULATOR_SIZE		2048
#define BINARYACCUMULATOR_SIZE	2048
#define EOL1					0x0a
#define EOL2					0x0d

#define STATUS_RECORD			(0x12)
#define OFFSET_RECORD			(0x13)
#define ID_RECORD				(0x14)
#define ACQUIRE_RECORD			(0x15)
#define ACQUIRE_RECORD_CHAR		"15"
#define END_DUMP_RECORD			(0x16)
#define END_DUMP2_RECORD		(0x61)
#define END_DUMP3_RECORD		(0x62)
#define DUMP_OK_RECORD			(0x17)
#define BATTERY_RECORD			(0x18)
#define TRIP_NEU_STAT_RECORD	(0x19)
#define DUAL_ION_STAT_RECORD	(0x1a)
#define GEN_STATUS_RECORD		(0x1b)
#define MONITOR_A_RECORD		(0x1c)
#define TRIP_NEU2_RECORD		(0x1d)
#define MONITOR_ID2_RECORD		(0x1e)
#define ANALYZE_OK_RECORD		(0x1f)
#define INFORMATION_RECORD		(0x30)
#define MONITOR_B_RECORD		(0x31)
//these don't actually correspond to the first chars of a message
#define UNKNOWN_REC				(0xf8)
#define ECHO_6_REC				(0xf9)
#define ECHO_5_REC				(0xfb)
#define FORCEDATETIME			(0xfa)
#define ECHO_4_REC				(0xfc)
#define ECHO_RECORD				(0xfd)
#define ECHO_2_REC				(0xfe)
#define ECHO_3_REC				(0xff)

//version 4.0? and above of mini-grand monitor
#define MONITOR_ID3_RECORD		(0x4e)
#define DUMP_OK3_RECORD			(0x4f)
#define GENERAL_CONFIG_RECORD	(0x41)
#define INSTRUMENT_STATUS_RECORD (0x42)
#define INSTRUMENT_INFO_RECORD	(0x43)
#define DUAL_CURRENTMODE_CONFIG_RECORD (0x44)
#define TRIGGER_CONFIG_RECORD	(0x45)
#define CHANNEL0_CONFIG_RECORD	(0x46)
#define CHANNEL1_CONFIG_RECORD	(0x47)
#define CHANNEL2_CONFIG_RECORD	(0x48)
#define CHANNEL3_CONFIG_RECORD	(0x49)
#define CHANNEL4_CONFIG_RECORD	(0x4a)
#define CHANNEL5_CONFIG_RECORD	(0x4b)
#define CHANNEL6_CONFIG_RECORD	(0x4c)
#define ANALYZE_OK3_RECORD		(0x4d)
#define CHANNELN_CONFIG_RECORD	CHANNEL0_CONFIG_RECORD

#define DUMP_OK3_RECORD_LENGTH			16
#define MONITOR_ID3_RECORD_LENGTH		32
#define GENERAL_CONFIG_RECORD_LENGTH	84
#define INSTRUMENT_STATUS_RECORD_LENGTH 55
#define INSTRUMENT_INFO_RECORD_LENGTH	54
#define DUAL_CURRENTMODE_CONFIG_RECORD_LENGTH 45
#define TRIGGER_CONFIG_RECORD_LENGTH	84
#define CHANNELN_CONFIG_RECORD_LENGTH	56
#define ANALYZE_OK3_RECORD_LENGTH		16

//other records lengths
#define STATUS_RECORD_LENGTH		80
#define OFFSET_RECORD_LENGTH		114
#define ID_RECORD_LENGTH			39
#define ACQUIRE_RECORD_LENGTH		79
#define ACQUIRE_RECORD_LENGTH_BIN	37
#define END_DUMP_RECORD_LENGTH		6
#define END_DUMP2_RECORD_LENGTH		7
#define DUMP_OK_RECORD_LENGTH		12
#define BATTERY_RECORD_LENGTH		20
#define TRIP_NEU_STAT_RECORD_LENGTH	53
#define DUAL_ION_STAT_RECORD_LENGTH	55
#define GEN_STATUS_RECORD_LENGTH	66
#define MONITOR_A_RECORD_LENGTH		107
#define TRIP_NEU2_RECORD_LENGTH		76
#define MONITOR_ID2_RECORD_LENGTH	28
#define ANALYZE_OK_RECORD_LENGTH	12
#define INFORMATION_RECORD_LENGTH	34
#define MONITOR_B_RECORD_LENGTH		73

//status byte 1
#define ACPOWER				0x0080
#define BATTERY				0x0040
#define TAMPER				0x0020
#define ACPOWERCH			0x0020
#define RMSTAMPER			0x0010
#define BBMERROR			0x0008
#define CWSTART				0x0004
//#define UNUSED			0x0002
#define INMII				0x0001

#define FAIL				"Fail"
#define OK					"Ok"
#define LOW					"Low"
#define ILOW				"Instrument Indicates LOW"
#define YES					"Yes"
#define NO					"No"
#define INMII_TXT			"In MII"
#define NOTINMII_TXT		"Not in MII"
#define START_COLD			"Cold Start"
#define START_NORMAL		"Normal"

//status byte 2
#define DATAFILTER_FLAG		0x0080
#define BBMISFULLEDER_FLAG	0x0040
#define BBMNUMOFBYTES_FLAG	0x0020
#define BBMWRITEERROR_FLAG	0x0010

#define DATAFILTER_YES		"Filtering"
#define DATAFILTER_NO		"Not filtering"
#define	BBMWRITEERROR		"Could not write value to BBM"
#define BBMNUMOFBYTES		"Number of bytes for opcode wrong"
#define BBMISFULLEDER		"BBM is filled"
#define BBMINVALIDOPC		"Invalid opcode read from BBM"

#define REDFLAG				"*"
enum MONVER {UNKNOWN,PRE4,POST4};

//pack structure at the word level
#pragma pack(2)
struct AcquireRecord
{
	unsigned int JulianSeconds;	//4 bytes
	short	StatusByte;		//2 bytes
	float	NeutronChA;		//4 bytes
	float	NeutronChB;		//4 bytes
	float	NeutronChC;		//4 bytes
	float	GammaCh1;		//4 bytes
	float	GammaCh1Sigma;	//4 bytes
	float	GammaCh2;		//4 bytes
	float	GammaCh2Sigma;	//4 bytes
	short	CountTime;		//2 bytes
};
//pack structure at the byte level
#pragma pack(1)
struct AcquireHeader
{
	char	SizeOfHeader[4];
	char	unused1[5];
	char	Version[5];
	char	StationId[3];
	char	Year[3];
	char	Month[3];
	char	Day[3];
	char	unused2[47];
	//or instead of unused2
	//char FullYear[4];
	//char unused2[43];
};
#pragma pack()

class CInstrument;
/////////////////////////////////////////////////////////////////////////////
// CSafePropertySheet
// property sheet for the mini-GRAND and GRAND3 instruments
class CSafePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSafePropertySheet)

friend class CInstrument;

// Construction
public:
	CSafePropertySheet(
		UINT nIDCaption, 
		CInstrument* pParentWnd, 
		char *inifile);

	CSafePropertySheet(
		LPCTSTR pszCaption, 
		CInstrument* pParentWnd, 
		char *inifile);

	CSafePropertySheet(); 

	static int ms_iGRANDPropertySheetCount;

	COLORREF SetButtonColor(COLORREF Color);
	void SetTabColor(int Tab, COLORREF Color);
	void SetLimits(
				float batvolth, float batvoltl,
				float sup_p15h, float sup_p15l,
				float sup_m15h, float sup_m15l,
				float sup_p05h, float sup_p05l,
				int   time,		char * pId);
	void SetHLVoltageDisplay(
				float batvolth, float batvoltl, bool bFirstBV,
				float sup_p15h, float sup_p15l, bool bFirstP15,
				float sup_m15h, float sup_m15l,	bool bFirstM15,
				float sup_p05h, float sup_p05l,	bool bFirst5);

	void UpdateTabs();

	CString *GetVersionString();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSafePropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSafePropertySheet();
	CColorButton	*m_pCloseButton;
	CButton			*m_pPauseButton;
	CButton			*m_pPrintButton;
	CButton			*m_pAckFlagsButton;
	CButton			*m_pToClose;
	int				m_dToClose;
	CInstrument		*m_pParent;
	CImageList		*m_pImageList;

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentBIDFileName[_MAX_PATH];
	char			m_szIniFile[_MAX_PATH];
	bool			m_bUpdatingFromFile;

	int				m_dBinaryAt;
	int				m_dAt;
	virtual void	SetToClose(int set);
	virtual void	ForceDateTime(void);
	virtual void	UpdateSummary(char *);
	void RestartAutoCloseTimer(){m_dToClose = 0;}
	void DisplayTodayData();
	CDataWatchDlg*	m_pDataWatchDlg;
	bool			m_bDataWatchDlgOpen;
	int				m_dPreviousBIDTime;
	enum MONVER		m_eMonitorVers;
	void AdjustTabsTo(enum MONVER eMonitorVers);

	// Generated message map functions
protected:
	int m_iGeneral_Tab;
	int m_iGeneral4_Tab;
	int m_iData_Tab;
	int m_iUserProg_Tab;
	int m_iUser4Prog_Tab;
	int m_iInstSetup_Tab;
	int m_iInst4Setup_Tab;
	bool m_bFirst15Rec;

	void FinishConstruction();
	bool CurrentSetParam(char*szIniName,char*szValue,bool bMatch,bool bTextOnly=true);
	bool CurrentSetParam(CString cIniName,CString cValue, bool bMatch, bool bTextOnly=true);

	//void BuildFileName(CString &cResult,CString cFormat,CString cPath,
	//	CString cType,CString cID,int iYear,int iMonth,int iDay,
	//	int iHour,int iMinute,int iSecond,
	//	CString cSuffix);

	void StatusRec(char *);
	void OffsetRec(char *);
	void ID_Record(char *);
	void AcquirRec(char *, bool bIgnoreAuth=false);
	void DumpEnd(char *);
	void DumpEnd2(char *);
	void DumpOk(char *);
	void BatteryRec(char *);
	void TripNSRec(char *);
	void DualIonRec(char *, bool bBinarySource = false);
	void GenStatRec(char *, bool bBinarySource = false);
	void Mon_A_Rec(char *,  bool bBinarySource = false);
	void TripN2Rec(char *);
	void ID2_Record(char *);
	void AnalyzeOk(char *);
	void InformRec(char *);
	void Mon_B_Rec(char *);

	//version 4.0? and above mini-grand
	void ID3_Record(char *);
	void AnalyzeOk3Record(char *);
	void DumpOk3_Record(char *);
	void GeneralConfigRecord(char *Record);
	void InstrumentStatusRecord(char *Record);
	void InstrumentInfoRecord(char *Record);
	void DualCurrentModeConfigRecord(char *Record);
	void TriggerConfigRecord(char *Record);
	void ChannelNConfigRecord(char *Record, short message_type);

	void Reverse(BYTE*,int);
	int AppendCheckSum(char*);
	char* TrunkExp(char* szString, int iDigits);
/*
	int BinaryStatusRec(BYTE *);
	int BinaryStatusRecOld(BYTE *);
	int BinaryOffsetRec(BYTE *);
	int BinaryAcquirRec(BYTE *);
	int BinaryBatteryRec(BYTE *);
	int BinaryDualIonRec(BYTE *);
	int BinaryDualIonRecOld(BYTE *);
	int BinaryGenStatRec(BYTE *);
	int BinaryGenStatRecOld(BYTE *);
	int BinaryMon_A_Rec(BYTE *);
	int BinaryMon_A_RecOld(BYTE *);
	int BinaryID2_Record(BYTE *);
	int BinaryAnalyzeOk(BYTE *);
	int BinaryInformRec(BYTE *);
	int BinaryMon_B_Rec(BYTE *);
*/
	//version 4.0? and above mini-grand

	int m_dBinaryChecksum;
	int m_dBinaryState;
	int m_dBinaryAT;
	unsigned short m_usLength;
	unsigned short m_usMessageType;
	int m_dRecLengthCount;
	int m_dGivenChecksum;
	int m_dChecksum;

	int  Checksum(char *);
	bool LengthOk(char* Record, int length);
	void SetChecksumDisplay();

	float m_fBatVoltH;			//battery voltage High
	float m_fBatVoltL;			//battery voltage Low
	float m_fSup_P15H;			//+15 volt supply High
	float m_fSup_P15L;			//+15 volt supply Low
	float m_fSup_M15H;			//-15 volt supply High
	float m_fSup_M15L;			//-15 volt supply Low
	float m_fSup_P05H;			//  5 volt supply High
	float m_fSup_P05L;			//  5 volt supply Low
	long  m_dTime_Err;			//Difference between MIC and Inst 

	float m_fLowBV;
	float m_fCurBV;
	float m_fHighBV;

	float m_fFBLow;
	float m_fFBCur;
	float m_fFBHigh;
	bool  m_bFBFirst;

	float m_fLowP15;
	float m_fCurP15;
	float m_fHighP15;
	float m_fLowM15;
	float m_fCurM15;
	float m_fHighM15;
	float m_fLowP5;
	float m_fCurP5;
	float m_fHighP5;
	bool  m_bFirstBV;
	bool  m_bFirstP15;
	bool  m_bFirstM15;
	bool  m_bFirstP5;

	float m_fTNSRHigh;
	float m_fTNSRLow;
	int   m_dTNSRFirst;

	float m_fDIRHigh;
	float m_fDIRLow;
	int   m_dDIRFirst;

	char  m_szCurrentID[3];
	int	  m_dCurrentYear;
	int   m_dCurrentMonth;
	int   m_dCurrentDay;

//	bool  m_bStartOfFile;

	bool  m_bBatteryOk;
	bool  m_bBatteryTolOk;
	bool  m_bP5VoltOk;
	bool  m_bP15VoltOk;
	bool  m_bM15VoltOk;
	bool  m_bACPowerOk;
	bool  m_bACPowerLoss;
	bool  m_bTimeError;
	bool  m_bBBMOk;
	bool  m_bCountRateTamperOk;
	bool  m_bRMSSigmaTamperOk;
	int   m_dCWStart;
	bool  m_bNewFileName;
	bool  m_bInMII;
	bool  m_bDataFiltered;
	char  m_szPreviousGTime[16];
	char  m_pAccumulator[ACCUMULATOR_SIZE];
	BYTE  m_pBinaryAccumulator[BINARYACCUMULATOR_SIZE];
	char  m_pCaption[256];
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	bool  m_bFCHVError;
	bool  m_bICHVError;
	int	  m_iFCHVError;
	int	  m_iICHVError;

	//to support binary records

	//{{AFX_MSG(CSafePropertySheet)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPauseButton();
	afx_msg void OnAckButton();
	afx_msg void OnPrintButton();
	afx_msg void OnToCloseButton();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	afx_msg void OnExtendClose(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // SAFEPS_H
