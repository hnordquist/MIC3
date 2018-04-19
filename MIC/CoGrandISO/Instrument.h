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
// Instrument.h

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "COM_Utilities.h"
#include "GenlStat.h"
#include "Genl4Stat.h"
#include "DataStat.h"
#include "UserProg.h"
#include "User4Prog.h"
#include "CDData.h"
#include "ModParam.h"
#include "GrandLegend.h"
#include "Summary.h"
#include "SetupData.h"
#include "GrandSetupData.h"
#include "ISO.h"   //base class
#include "../Interfaces/ICSO.h"  //the communications component interface

#define GRAND_DISCOVERY_TYPE "GRAND"

/////////////////////////////////////////////////////////////////////////////
// CInstrument window

class CInstrument : public CISO
{
// Construction
public:
	CInstrument(CWnd* Parent, char *inifile);
	CInstrument();
	virtual ~CInstrument();
	void FinishConstruction();

// Attributes
public:
	bool CollectParametersForNewISO(CString& csNameOut);
	bool CreateInstrument();
	void ShowInstrument(bool);
	void PauseInstrument(bool);
	bool UpdateParams(bool);
	void UpdateStrings();
	void DriveToZero(bool bDriveToZero, bool bInPwrFail = false);
	void FlagComm(int val, bool redraw);
	void FlagOthr(int val, bool redraw);
	void FlagTime(int val, bool redraw);
	void IssuePrint(FILE* outfile);
	void Reset();
	void SetLocalRegisteredMessageValues();
	void ReConfig();

#ifdef ALLOWTRIGGERHOLDOFF
	void AddExclusion(LPARAM lParam);
	void DelExclusion();
#endif

	CSummary    *m_pSummaryPage;
	CGenlStat	*m_pGeneralStatPage;
	CGenl4Stat	*m_pGeneral4StatPage;
	CDataStat	*m_pDataStatus;
	CUserProg	*m_pUserProgStatus;
	CUser4Prog	*m_pUser4ProgStatus;
	CCDData		*m_pCDData;
	CModParam	*m_pModParams;
	CSetupData	*m_pSetupData;
	CSetupData	*m_pSetup4Data;
	CLegend		*m_pLegend;

	CSafePropertySheet	*m_pPropertySheet;

	typedef enum PAGETYPE_TAG
	{
		SUMMARYPAGE,
		GENERALPAGE,
		GENERAL4PAGE,
		USERPAGE,
		USER4PAGE,
		DATAPAGE,
		DATA4PAGE
	} PAGETYPE;

	COleDateTime m_pToday;
	char		m_pFullNameSpaces[128];
	bool		m_bIsPaused; 
	bool		m_bLogMII;
	bool		m_bLogFilter;
	bool		m_bInDumpLast;
	int			m_dSave_State;
	int			m_dSlowReset;
	int			m_dNumberOfAcquires;
	int			m_dBreakSentCount;
	int			m_dTimeOutCount;
	int			m_dInvalidTimeCount;

	int			m_dCurrent_State;
	CWnd*		m_pParent;
	unsigned int m_dPrevBIDTime;
	int			m_dStatusInt;
	int			m_dCycleInt;
	int			m_dMaxBBM;
	bool		m_bDoBinaryBBM;
	int			m_dTick_Count;
	int			m_dTickToInquire;
	int			m_dInquireInt;
	int			m_dTickToStatus;
	int			m_dCommFail;
	int			m_dTimeOut;
	int			m_IsMiniGrand;
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
	int			m_dPreviousDay;
	char		m_szAutoTimeDailyAt[16];
	unsigned	m_uiSecInDayAutoTime;
	bool		m_bAutoTimeDaily;
	bool		m_bAutoTimeDailyTrigger;
//	bool		m_bSecInDayTrigger;

	COleDateTime	m_pNow;

	unsigned int	m_uiLastStatusRecTime;					//DR00406
	unsigned int	m_uiLastOffsetRecTime;					//DR00406
	unsigned int	m_uiLastBatteryRecTime;					//DR00406
	unsigned int	m_uiLastTripNSRecTime;					//DR00406
	unsigned int	m_uiLastTripN2RecTime;					//DR00406
	unsigned int	m_uiLastInformRecTime;					//DR00406
	unsigned int	m_uiLastDualIonRecTime;					//DR00406
	unsigned int	m_uiLastGenStatRecTime;					//DR00406
	unsigned int	m_uiLastMon_A_RecTime;					//DR00406
	unsigned int	m_uiLastMon_B_RecTime;					//DR00406
	unsigned int	m_uiLastGeneralConfigRecordTime;		//DR00406
	unsigned int	m_uiLastInstrumentStatusRecordTime;		//DR00406
	unsigned int	m_uiLastInstrumentInfoRecordTime;		//DR00406
	unsigned int	m_uiLastDualCurrentModeConfigRecordTime;//DR00406
	unsigned int	m_uiLastTriggerConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel0ConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel1ConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel2ConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel3ConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel4ConfigRecordTime;		//DR00406
	unsigned int	m_uiLastChannel5ConfigRecordTime;		//DR00406


#ifdef ALLOWTRIGGERHOLDOFF
	void ReadExclusion();
	void WriteExclusion();
	typedef struct EXCLUSIONRECTAG{
		unsigned int uiStartTime;
		unsigned int uiEndTime;
	} EXCLUSIONREC;
	CList< EXCLUSIONREC, EXCLUSIONREC > m_ExclusionList;
#endif


#pragma pack(1)
	struct SBinaryStatusRecOld {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		UCHAR acq_mode;			//1
		short g_gain1;			//2
		short g_gain2;			//2
		float n_gain;			//4
		UCHAR lld_a;			//1
		UCHAR uld_a;			//1
		UCHAR lld_b;			//1
		UCHAR uld_b;			//1
		float fission_bias;		//4
		float ion_bias;			//4
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} m_sBinaryStatusRecOld;			//44 total

	struct SBinaryStatusRec {
		UCHAR length;			//1
		UCHAR type;				//1
		unsigned int julian;	//4
		UCHAR status;			//1
		UCHAR acq_mode;			//1
		unsigned int g_gain1;	//4
		unsigned int g_gain2;	//4
		float n_gain;			//4
		UCHAR lld_a;			//1
		UCHAR uld_a;			//1
		UCHAR lld_b;			//1
		UCHAR uld_b;			//1
		float fission_bias;		//4
		float ion_bias;			//4
		float battery;			//4
		float plus5;			//4
		float plus12;			//4
		float minus12;			//4
	} m_sBinaryStatusRec;			//48 total
#pragma pack()
	CString m_cStatusRec;
	CString m_cOffsetRec;
	CString m_cIDRec;
	CString m_cAcquireRec[4];
	CString m_cBatteryRec;
	CString m_cTripNSRec;
	CString m_cDualIonRec;
	CString m_cGenStatRec;
	CString m_cMon_A_Rec;
	CString m_cTripN2Rec;
	CString m_cID2_Record;
	CString m_cID3_Record;
	CString m_cAnalyzeOkRec;
	CString m_cAnalyzeOk3Rec;
	CString m_cInformRec;
	CString m_cMon_B_Rec;
	CString m_cGeneralConfigRecord;
	CString m_cInstrumentStatusRecord;
	CString m_cInstrumentInfoRecord;
	CString m_cDualCurrentModeConfigRecord;
	CString m_cTriggerConfigRecord;
	CString m_cChannel0ConfigRecord;
	CString m_cChannel1ConfigRecord;
	CString m_cChannel2ConfigRecord;
	CString m_cChannel3ConfigRecord;
	CString m_cChannel4ConfigRecord;
	CString m_cChannel5ConfigRecord;
	CString m_cChannel6ConfigRecord;
	CString m_cSummaryRecent[6];
	CString m_cGENERAL_CHECKSUM_text;
	CString m_cSetParamMatchText;

	COLORREF SummaryStaticColors[IDC_SUMMARY_LAST-IDC_SUMMARY_FIRST+1];
	COLORREF GeneralStaticColors[IDC_GENERAL_LAST-IDC_GENERAL_FIRST+1];
	COLORREF General4StaticColors[IDC_GENERAL4_LAST-IDC_GENERAL4_FIRST+1];
	COLORREF UserStaticColors[IDC_USERPROG_LAST-IDC_USERPROG_FIRST+1];
	COLORREF User4StaticColors[IDC_USER4PROG_LAST-IDC_USER4PROG_FIRST+1];
	COLORREF DataStaticColors[IDC_DATA_LAST-IDC_DATA_FIRST+1];
	COLORREF SetupStaticColors[SET_TOP-SET_BASE+1];
	COLORREF Setup4StaticColors[SET4_TOP-SET4_BASE+1];

	CString SummaryStrings[IDC_SUMMARY_LAST-IDC_SUMMARY_FIRST+1];
	CString GeneralStrings[IDC_GENERAL_LAST-IDC_GENERAL_FIRST+1];
	CString General4Strings[IDC_GENERAL4_LAST-IDC_GENERAL4_FIRST+1];
	CString UserStrings[IDC_USERPROG_LAST-IDC_USERPROG_FIRST+1];
	CString User4Strings[IDC_USER4PROG_LAST-IDC_USER4PROG_FIRST+1];
	CString DataStrings[IDC_DATA_LAST-IDC_DATA_FIRST+1];
	CString SetupStrings[SET_TOP-SET_BASE+1];
	CString Setup4Strings[SET4_TOP-SET4_BASE+1];
	CString SetupINIStrings[SET_TOP-SET_BASE+1];
	CString Setup4INIStrings[SET4_TOP-SET4_BASE+1];

	int m_iMessageLimiter[512];//look for the end one, see WriteToFileEx
	bool WriteToFileEx(unsigned uiIndex, int ToWhom, int Type, CJulianTime* Time, char* Msg, bool bStale = false);		//DR00406
	//void ReConfig();see above
	
	void SummaryPageIssuePrint(FILE *outfile);
	void GeneralPageIssuePrint(FILE *outfile);
	void General4PageIssuePrint(FILE *outfile);
	void DataPageIssuePrint(FILE *outfile);
	void UserPageIssuePrint(FILE *outfile);
	void User4PageIssuePrint(FILE *outfile);
	void SetupPageIssuePrint(FILE *outfile);
	void Setup4PageIssuePrint(FILE *outfile);

	int	m_iGeneral_Tab;
	int	m_iGeneral4_Tab;
	int m_iData_Tab;
	int m_iUserProg_Tab;
	int m_iUser4Prog_Tab;
	int m_iInstSetup_Tab;
	int m_iInst4Setup_Tab;

	char GetFlags(PAGETYPE ePageType, int nId);
	void SetString(PAGETYPE ePageType, UINT uiID, CString cString);
	CString GetString(PAGETYPE ePageType, UINT uiID);
	void GetStrings(PAGETYPE ePageType, UINT uiDlgItem, CString& cString, char* cFlag);
	void UpdateDisplay();

	char			m_szCurrentFileName[_MAX_PATH];
	char			m_szCurrentBIDFileName[_MAX_PATH];

	int				m_dBinaryAt;
	int				m_dAt;
	int				m_dAcquireRecordCount;
	struct			AcquireRecord AcquireRec[NUMBEROFACQUIRERECS];
	CJulianTime		AcquireTime[NUMBEROFACQUIRERECS];
	short			AcquireCS[NUMBEROFACQUIRERECS];
	short			AcquireAUTH[NUMBEROFACQUIRERECS];
	bool			AcquireAUTHIgnore[NUMBEROFACQUIRERECS];
//	virtual void	ForceDateTime(void);
//	virtual void	UpdateSummary(char *);
	int				m_dPreviousBIDTime;
	enum MONVER		m_eMonitorVers;
	void AdjustTabsTo(enum MONVER eMonitorVers);
	void AckButton();
	void SetLimits(
				float batvolth, float batvoltl,
				float sup_p15h, float sup_p15l,
				float sup_m15h, float sup_m15l,
				float sup_p05h, float sup_p05l,
				int   time,		char * pId);

	bool m_bFirst15Rec;

	bool CurrentSetParam(UINT uiIniName, char*szIniName,char*szValue,bool bMatch);
	bool CurrentSetParam(UINT uiIniName, CString cIniName,CString cValue, bool bMatch);

	void BuildFileName(CString &cResult,CString cFormat,CString cPath,
		CString cType,CString cID,int iYear,int iMonth,int iDay,
		int iHour,int iMinute,int iSecond,
		CString cSuffix);

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
	void SetColor(PAGETYPE ePageType,UINT uiDlgItem,COLORREF rgbColor);

	//version 4.10? and above mini-grand
	void ID3_Record(char *);
	void DumpOk3_Record(char *);
	void GeneralConfigRecord(char *Record);
	void InstrumentStatusRecord(char *Record);
	void InstrumentInfoRecord(char *Record);
	void DualCurrentModeConfigRecord(char *Record);
	void TriggerConfigRecord(char *Record);
	void ChannelNConfigRecord(char *Record, short message_type);
	void AnalyzeOk3Record(char *);

	void Reverse(BYTE*,int);
	int AppendCheckSum(char*);
	char* TrunkExp(char* szString, int iDigits);

	int BinaryStatusRec(BYTE *);
	int BinaryStatusRecOld(BYTE *);
	int BinaryOffsetRec(BYTE *);
////	int BinaryID_Record(BYTE *);
	int BinaryAcquirRec(BYTE *);
////	int BinaryDumpEnd(BYTE *);
////	int BinaryDumpEnd2(BYTE *);
////	int BinaryDumpOk(BYTE *);
	int BinaryBatteryRec(BYTE *);
////	int BinaryTripNSRec(BYTE *);
	int BinaryDualIonRec(BYTE *);
	int BinaryDualIonRecOld(BYTE *);
	int BinaryGenStatRec(BYTE *);
	int BinaryGenStatRecOld(BYTE *);
	int BinaryMon_A_Rec(BYTE *);
	int BinaryMon_A_RecOld(BYTE *);
////	int BinaryTripN2Rec(BYTE *);
	int BinaryID2_Record(BYTE *);
	int BinaryAnalyzeOk(BYTE *);
	int BinaryInformRec(BYTE *);
	int BinaryMon_B_Rec(BYTE *);

	//version 4.0? and above mini-grand
	int BinaryID3_Record(BYTE *);
	int BinaryGeneralConfigRecord(BYTE *);
	int BinaryInstrumentStatusRecord(BYTE *);
	int BinaryInstrumentInfoRecord(BYTE *);
	int BinaryDualCurrentModeConfigRecord(BYTE *);
	int BinaryTriggerConfigRecord(BYTE *);
	int BinaryChannelNConfigRecord(BYTE *, short);

	int m_dBinaryChecksum;
	int m_dBinaryState;
	int m_dBinaryAT;
	unsigned short m_usLength;
	unsigned short m_usMessageType;
	int m_dRecLengthCount;
	int m_dGivenChecksum;
	int m_dChecksum;

	void Echo_Rec(char *);
	void Echo_2_Rec(char *);
	void Echo_3_Rec(char *);
	void Echo_4_Rec(char *);
	void Echo_5_Rec(char *);
	void Echo_6_Rec(char *);
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

	bool  m_bStartOfFile;

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
//	char  m_pCaption[256];
	CString m_cCommStatus;
	CString m_cCheckSumMsg;
	bool  m_bCouldNotOpenBinaryFile;

	bool  m_bFCHVError;
	bool  m_bICHVError;
	int	  m_iFCHVError;
	int	  m_iICHVError;
	bool  m_bDoOneDumpStat;	//SCR00074

	void IssuePrint2(FILE* outfile);

	void WriteToFile(int ToWhom, int Type, CJulianTime* Time = NULL, char* Message = NULL, bool bStale = false);		//DR00406
	bool WriteToBIDFile(CJulianTime* Time, AcquireRecord* AcquireRec, 
		short AcquireCS, short AcquireAUTH, bool bIgnoreAuth);
	virtual void	UpdateSummary(char *);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrument)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CInstrument)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg void OnReceiveOOB(WPARAM WParam, LPARAM LParam);
	afx_msg void OnReceive(WPARAM WParam,LPARAM LParam);
	afx_msg void OnReceiveData(WPARAM WParam,LPARAM LParam);
	DECLARE_MESSAGE_MAP()
};

#endif // INSTRUMENT_H
