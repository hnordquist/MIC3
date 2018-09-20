#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************/
/* This file is for version 4.00 (and later) of the Unified MCB Interface */
/* (mcbcio32.lib, mcbcio32.dll, etc.)                                     */
/**************************************************************************/
#ifndef _MCBCIO32_TYPESDEFINED
#pragma pack(2)				/* use 2 byte alignment in structs */

/*************************************************************************/
/* Define buffer sizes needed to hold certain response records from MCBs */
/*************************************************************************/
#define MCBSTATUSSIZE (36)		/* Bytes in status rec including NULL */
#define MCBTYPESIZE (9)			/* Bytes in ID string incl NULL */
#define MCBTIMESIZE (9)			/* Bytes in time string incl NULL */
#define MCBDATESIZE (9)			/* Bytes in date string incl NULL */

/*******************************/
/* Define MCBCIO Buffer sizes. */
/*******************************/
#define MIOHOSTMAX (64)			/* network Host name including NULL */
#define MIOAUTHMAX (32)			/* Authorization str including NULL */
#define MIOPASSMAX (32)			/* Password string including NULL */
#define MIOOWNERMAX (64)		/* Det Owner Name str including NULL */
#define MIODETDESCMAX (128)		/* Det Description str including NULL */
#define MIODETNAMEMAX (128)		/* Det Name from MIOGetConfigName() */
#define MIOAPPDATANAMEMAX (32)		/* App Data Name string including NULL */
#define MIOAPPDATAMAX (128)		/* App Data string including NULL */
#define MIOCONFIGMCBSMAX (16)		/* max MCBs to report on GETHARDCONFIG msg */
#define MIOSECURITYMAX (32)		/* Length of Server Security String */
#define MIOMAXSERVERDATA (10)		/* number of pointers in LPSERVERDATA */
#define MIOMAXLANAS (6)                 /* max number of NetBIOS lan adapters concurrently supported */
#define MIOKEYMAX (48)                  /* Key string including NULL */

/****************************************************/
/* Define error codes returned by MIOGetLastError() */
/****************************************************/
#define MIOENONE (0)		/* No error (maybe an MCB warning) */
#define MIOEINVALID (1)		/* Det handle or other parameter is invalid */
#define MIOEMCB (2)		/* MCB reported error (see nMacroErr & nMicroErr) */
#define MIOEIO (3)		/* Disk, Network or MCB I/O error (see nMacroErr) */
#define   MIOEMACCLOSED (-1)	/*   Detector Comm Broken, Call MIOCloseDetector() */
#define   MIOEMACTIMEOUT (-2)	/*   Detector communication timeout -- Try Again */
#define   MIOEMACCOMM (-3)	/*   Detector communication error -- Try Again */
#define   MIOEMACTOOMANY (-4)	/*   Too Many open detectors -- close detector then try again */
#define   MIOEMACOTHER (-5)     /*   Disk, OS or any other error */
#define MIOEMEM (4)		/* Memory allocation error */
#define MIOENOTAUTH (5)		/* Authorization or Password failure */
#define MIOEBLOCKING (6)	/* MCBCIO call already in progress    */
				/* (obsolete -- not used by MCBCIO32) */
#define MIOEINTR (7)		/* MCBCIO call interrupted            */
				/* (obsolete -- not used by MCBCIO32) */
#define MIOENOCONTEXT (8)       /* MCBCIO call before MIOStartup() or...*/
                                /*...after MIOCleanup().                */

/**********************************************************/
/* Define authorization codes returned by MIOGetAccess(). */
/**********************************************************/
#define MIOAUTHNONE (0)
#define MIOAUTHREAD (1)
#define MIOAUTHWRITE (2)
#define MIOAUTHFULL (3)

/*************************************************/
/* Define feature codes used with MIOIsFeature() */
/*************************************************/
#define MIOFEAT_CONVGAIN (0)            /* software settable conversion gain */
#define MIOFEAT_COARSEGAIN (1)          /* software settable coarse gain */
#define MIOFEAT_FINEGAIN (2)            /* software settable fine gain */
#define MIOFEAT_GAINSTAB (3)            /* gain stabilizer */
#define MIOFEAT_ZEROSTAB (4)            /* zero stabilizer */
#define MIOFEAT_PHAMODE (5)             /* PHA mode functions available */
#define MIOFEAT_MCSMODE (6)             /* MCS mode functions available */
#define MIOFEAT_LISTMODE (7)            /* list mode functions available */
#define MIOFEAT_SAMPMODE (8)            /* sample mode functions available */
#define MIOFEAT_DIGITALOFF (9)          /* digital Offset (SET/SHOW_OFFSET) */
#define MIOFEAT_FINEOFF (10)            /* software settable fine Offset (SET/SHOW_OFFS_FINE) */
#define MIOFEAT_HVPOWER (11)            /* HV power supply (SHOW_HV, ENA/DIS_HV) */
#define MIOFEAT_HVENHANCED (12)         /* Enhanced HV (SET_HV, SET/SHOW_HV_POL, SHOW_HV_ACT) */
#define MIOFEAT_HVRANGE (13)            /* software settable HV range (ENA_NAI, DIS_NAI) */
#define MIOFEAT_AUTOPZ (14)             /* auto PZ (START_PZ_AUTO) */
#define MIOFEAT_MANPZ (15)              /* software settable manual PZ (SET/SHOW_PZ) */
#define MIOFEAT_CLOCK (16)              /* internal clock (SHOW_DATE/TIME, SHOW_DATE/TIME_START) */
#define MIOFEAT_SAMPCHANGER (17)        /* Sample Changer support (SET/SHOW_OUTPUT, SHOW_INPUT) */
#define MIOFEAT_FIELDMODE (18)          /* one-button acq (ENA/DIS/SHOW_TRIG_SPEC, MOVE) */
#define MIOFEAT_NOMADIC (19)            /* nomadic (likely to move between opens) */
#define MIOFEAT_APPDATA (20)            /* local app data (SET_DATA_APP, SHOW_DATA_APP) */
#define MIOFEAT_SERIALNUM (21)          /* software retrievable serial number (SHOW_SNUM) */
#define MIOFEAT_POWERMAN (22)           /* power management commands (CONS, ON, OFF, etc.) */
#define MIOFEAT_BATTERYSTAT (23)        /* battery status support (SH_STAT_BATT) */
#define MIOFEAT_AMPPOLARITY (24)        /* software settable AMP polarity (SET/SHOW_GAIN_POLAR) */
#define MIOFEAT_OPTIMIZE (25)           /* support for flattop optimization (ENA/DIS_OPTI) */
#define MIOFEAT_STOPPZ (26)             /* stoppable AutoPZ (STOP_PZ_AUTO cmd) */
#define MIOFEAT_NETWORK (27)            /* network support (SET/SHOW_NET_ADDR) */
#define MIOFEAT_MULTIDROP (28)          /* multi-drop serial support (i.e. uNomad) */
#define MIOFEAT_DPMADDR (29)            /* software settable DPM address (SET_DPM_ADDR) */
#define MIOFEAT_MULTIDEV (30)           /* multiple devices (i.e. 919) */
#define MIOFEAT_GATEMODE (31)           /* software settable ADC gate mode (SET_GATE...) */
#define MIOFEAT_DOWNLOAD (32)           /* downloadable firmware */
#define MIOFEAT_THAMODE (33)            /* time histogram functions available (i.e. 9308) */
#define MIOFEAT_LLD (34)                /* software settable Lower level disc (SET_LLD) */
#define MIOFEAT_ULD (35)                /* software settable Upper level disc (SET_ULD) */
#define MIOFEAT_SCAINPUT (36)           /* MCS mode SCA input available */
#define MIOFEAT_TTLINPUT (37)           /* MCS mode positive TTL input available */
#define MIOFEAT_NEGNIMINPUT (38)        /* MCS mode fast negative NIM input available */
#define MIOFEAT_DISCINPUT (39)          /* MCS mode discriminator input available */
#define MIOFEAT_DISCEDGE (40)           /* software switchable discriminator edge */
#define MIOFEAT_DISCLEVEL (41)          /* software programmable discriminator level */
#define MIOFEAT_SCAPROG (42)            /* software programmable SCA upper and lower threasholds */
#define MIOFEAT_INPUTSELECT (43)        /* software selectable MCS mode input sources */
#define MIOFEAT_STATPRESET (44)         /* statistical preset (SET/SHOW_UNCERT_PRES) */
#define MIOFEAT_VARIFEAT (45)           /* features vary by input (SHOW_FEAT depends on device/segment) */
#define MIOFEAT_SHUTDOWN (46)           /* software settable HV shutdown mode (SET/SHOW/VERI_SHUTDOWN) */
#define MIOFEAT_SHAPECONST (47)         /* software settable shaping time constants (SET_SHAP) */
#define MIOFEAT_EXPLORESHAPE (48)       /* explorable shaping time constants (SHOW_CONFIG_SHAP) */
#define MIOFEAT_ADVANCEDSHAPE (49)      /* advanced shaping time (SET_SHAP_RISE, SET_SHAPE_FLAT, etc.) */
#define MIOFEAT_BLR (50)                /* software settable BLR (ENA/DIS/SHO_BLR_AUTO SET/SHO/VERI_BLR) */
#define MIOFEAT_SHOWSTAT (51)           /* SHOW_STATUS command supported (returns $M record) */
#define MIOFEAT_OVERPRESET (52)         /* overflow preset (ENA/DIS/SHO_OVER_PRES) */
#define MIOFEAT_CLICKER (53)            /* software enabled audio clicker (ENA/DIS_CLICK) */
#define MIOFEAT_THERMISTOR (54)         /* software readable thermistor (SHOW_THERM) */
#define MIOFEAT_FLOATFINE (55)          /* Fine Gain is float number (SET/SHO/VERI/LIST_GAIN_FINE) */
#define MIOFEAT_PUR (56)                /* software enabled Pile-up Rej. (ENA/DIS/SHO_PUR, SET_WIDT_REJ) */
#define MIOFEAT_ALPHAHV (57)            /* Alpha style HV power (SHOW_HV_CURRENT) */
#define MIOFEAT_VACUUM (58)             /* software readable vacuum (SHOW_VACUUM) */
#define MIOFEAT_ACQALARM (59)           /* acquisition alarms (ENA/DIS_ALARM) */
#define MIOFEAT_TRIGGER (60)            /* hardware acquisition trigger (ENA/DIS_TRIG) */
#define MIOFEAT_ORDINALSHAP (61)        /* ordinal shapping times (SET_SHAP 0, SET_SHAP 1, ...) */
#define MIOFEAT_LISTGAINS (62)          /* querey gain ranges (LIST/VERI_GAIN_FINE, ..._COAR, ..._CONV) */
#define MIOFEAT_ROUTINPUT (63)          /* routable inputs (SET/SHOW_INPUT_ROUTE) */
#define MIOFEAT_EXTDWELL (64)           /* external dwell support (ENA/DIS_DWELL_EXT) */
#define MIOFEAT_SUMREPLACE (65)         /* selectable SUM or REPLACE MCS modes (ENA/DIS_SUM) */
#define MIOFEAT_EXTSTART (66)           /* external Start support (ENA/DIS/SHO_START_EXT) */
#define MIOFEAT_LISTMCS (67)            /* explorable MCS (LIST_SOURCE, LIST_LLSCA & LIST_ULSCA) */
#define MIOFEAT_MDAPRESET (68)          /* Device support the MDA preset (DSPEC & 92xII) */
#define MIOFEAT_EXTENDED (127)          /* extended feature mask available (SH_FEAT_EXT) */

/*************************************/
/* Define codes used with MIODebug() */
/*************************************/
#define MIODEBUGNONE (0)
#define MIODEBUGNORMAL (1)
#define MIODEBUGDETAIL (2)
#define MIODEBUGALL (3)
#define MIODEBUGSTARTTIMER (-1)
#define MIODEBUGSTOPLOCAL (-2)
#define MIODEBUGSTOPCLIENT (-3)
#define MIODEBUGTRACKTASK (-4)
#define MIODEBUGSTARTTRACE (-5)
#define MIODEBUGSTOPTRACE (-6)
#define MIODEBUGNETDUMP (-7)
#define MIODEBUGGETLEVEL (-8)
#define MIODEBUGGETTRACE (-9)

/***********************************************************************/
/* Define MCBCIO data types.  Note that detector handles can be shared */
/* by threads within a process but must not be shared among processes. */
/***********************************************************************/
typedef void *HDET;		        /* Detector handle */

typedef struct tagMIOHARDCONF {
    int nNumDevSegs[MIOCONFIGMCBSMAX];	/* number of DevSegs for each MCB */
} MIOHARDCONF, *PMIOHARDCONF, *LPMIOHARDCONF;

typedef struct tagMIOSERVERENTRY {
    char szAddr[MIOHOSTMAX];            /* host address string */
    char szName[MIOHOSTMAX];            /* host name string */
    int nLana;                          /* lana number for host connection */
    BOOL bLocal;                        /* TRUE if this entry is for Local MCBs */
    BOOL bFound;                        /* TRUE if this entry is valid */
    BOOL bFailed;                       /* TRUE if Host responded but not reachable */
    int nFailedError;                   /* error code related to failure (-1=open failed, */
                                        /* 0=bad server response, >0 is NetBIOS error code) */
} MIOSERVERENTRY;
typedef MIOSERVERENTRY *PMIOSERVERENTRY, *LPMIOSERVERENTRY;

typedef MIOSERVERENTRY MIOSERVERDATA[MIOMAXSERVERDATA];
typedef MIOSERVERDATA *PMIOSERVERDATA, *LPMIOSERVERDATA;

typedef struct tagMIOCONFIGITEM {
    int nLength;                        /* set to sizeof(MIOCONFIGITEM) */
    DWORD dwID;                         /* det ID number as shown in pick list */
    int nMCB;                           /* MCB number on network host */
    int nDevSeg;                        /* MCB Device/Segment number */
    int nLana;                          /* lana number for host connection */
    char szName[MIODETNAMEMAX];         /* det name as shown in pick list */
    char szHost[MIOHOSTMAX];            /* Network host computer name */
} MIOCONFIGITEM;
typedef MIOCONFIGITEM *PMIOCONFIGITEM, *LPMIOCONFIGITEM;
#define MIOCONFIGITEMNULL {0,0,0,0,0,"",""}     /* used as initializer */

typedef struct tagMIOTM {		/* identical to tm struct from time.h */
    int tm_sec;				/* seconds after the minute - [0,59] */
    int tm_min;				/* minutes after the hour - [0,59] */
    int tm_hour;			/* hours since midnight - [0,23] */
    int tm_mday;			/* day of the month - [1,31] */
    int tm_mon;				/* months since January - [0,11] */
    int tm_year;			/* years since 1900 */
    int tm_wday;			/* days since Sunday - [0,6] */
    int tm_yday;			/* days since January 1 - [0,365] */
    int tm_isdst;			/* daylight savings time flag */
} MIOTM, *PMIOTM, *LPMIOTM;

#define MIOMAXCHANS ((WORD)(-1))
#define MIOLASTDET ((int)(-1))

#pragma pack()				/* resume default alignment for structs */
#define _MCBCIO32_TYPESDEFINED
#endif /* #ifndef _MCBCIO32_TYPESDEFINED */

#ifndef _MCBCIO32_EXPORTSDEFINED
#ifndef _MCBCIO32_NOEXPORTS

/******************************/
/* Prototypes for UMCBI calls */
/******************************/
#ifndef _MCBCIO_IMPORT_
#define _MCBCIO_IMPORT_ __declspec(dllimport)
#endif
_MCBCIO_IMPORT_ BOOL WINAPI  MIOStartup(void);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCleanup(void);
_MCBCIO_IMPORT_ int WINAPI MIOGetDetMCB(HDET hDet);
_MCBCIO_IMPORT_ int WINAPI MIOGetDetDevSeg(HDET hDet);
_MCBCIO_IMPORT_ WORD WINAPI MIOGetDetLength(HDET hDet);
_MCBCIO_IMPORT_ LPSTR WINAPI MIOGetDetHost(HDET hDet);
_MCBCIO_IMPORT_ LPSTR WINAPI MIOGetDetHostEx(HDET hDet, LPSTR lpszHost, int nMaxHost);
#define MIOGetDetHost(hDet) MIOGetDetHostEx(hDet, NULL, 0)
_MCBCIO_IMPORT_ BOOL WINAPI MIOIsDetLocal(HDET hDet);
_MCBCIO_IMPORT_ LPSTR WINAPI MIOGetType(HDET hDet);
_MCBCIO_IMPORT_ LPSTR WINAPI MIOGetTypeEx(HDET hDet, LPSTR lpszType, int nMaxType);
#define MIOGetType(hDet) MIOGetTypeEx(hDet, NULL, 0)
_MCBCIO_IMPORT_ BOOL WINAPI MIOIsFeature(HDET hDet, int nFeature);
_MCBCIO_IMPORT_ int WINAPI MIODebug(int nLevel);
_MCBCIO_IMPORT_ int WINAPI MIOGetRevision(void);
_MCBCIO_IMPORT_ int WINAPI MIOGetLastError(LPINT lpnMacroErr, LPINT lpnMicroErr);
_MCBCIO_IMPORT_ BOOL WINAPI MIOIsDetector(HDET hDet);
_MCBCIO_IMPORT_ HDET WINAPI MIOOpenDetector(int nDet, LPSTR lpszListName, LPSTR lpszAuth);
_MCBCIO_IMPORT_ HDET WINAPI MIOOpenConfigItem(LPMIOCONFIGITEM lpConfigItem, LPSTR lpszAuth);
_MCBCIO_IMPORT_ HDET WINAPI MIOOpenDetID(DWORD dwID, LPSTR lpszAuth);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCloseDetector(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOComm(HDET hDet, LPSTR lpszCmd, LPSTR lpszAuth, LPSTR lpszPassword,
    int nMaxResp, LPSTR lpszResp, LPINT lpnRespLen);
_MCBCIO_IMPORT_ BOOL WINAPI MIOSetROI(HDET hDet, WORD wStartChan, WORD wNumChans,
    LPSTR lpszAuth, LPSTR lpszPass);
_MCBCIO_IMPORT_ BOOL WINAPI MIOClearROI(HDET hDet, WORD wStartChan, WORD wNumChans,
    LPSTR lpszAuth, LPSTR lpszPass);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCreateConfigList(LPSTR lpszListName);
_MCBCIO_IMPORT_ BOOL WINAPI MIOResetMasterList(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOAddMasterItemEx(HDET hDet, int nDet, LPMIOCONFIGITEM lpConfigItem);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCompressConfigList(LPSTR lpszListName);
_MCBCIO_IMPORT_ BOOL WINAPI MIOAddConfigItem(int nDet, LPSTR lpszListName, int nMasterDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetConfigItem(int nDet, LPSTR lpszListName,
    LPMIOCONFIGITEM lpConfigItem);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCreateConfigItem(HDET hDet, LPMIOCONFIGITEM lpConfigItem);
_MCBCIO_IMPORT_ BOOL WINAPI MIOCreateDetectorKey(HDET hDet, LPSTR lpszKey, int nKeyMax);
_MCBCIO_IMPORT_ BOOL WINAPI MIOAddConfigItemEx(int nDet, LPSTR lpszListName,
    LPMIOCONFIGITEM lpConfigItem);
_MCBCIO_IMPORT_ BOOL WINAPI MIODeleteConfigItem(int nDet, LPSTR lpszListName);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetConfigMax(LPSTR lpszListName, LPINT lpnDetMax);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetConfigName(int nDet, LPSTR lpszListName, int nNameMax,
    LPSTR lpszName, LPDWORD lpdwID, BOOL *lpbOutDated);
_MCBCIO_IMPORT_ LPDWORD WINAPI MIOGetData(HDET hDet, WORD wStartChan, WORD wNumChans,
    LPDWORD lpdwBuffer, LPWORD lpwRetChans, LPDWORD lpdwDataMask,
    LPDWORD lpdwROIMask, LPSTR lpszAuth);
_MCBCIO_IMPORT_ LPDWORD WINAPI MIOSetData(HDET hDet, WORD wStartChan, WORD wNumChans,
    LPDWORD lpdwBuffer, LPSTR lpszAuth);
_MCBCIO_IMPORT_ BOOL WINAPI MIOLockDetector(HDET hDet, LPSTR lpszAuth, LPSTR lpszPass,
    LPSTR lpszOwnerName);
_MCBCIO_IMPORT_ BOOL WINAPI MIOUnlockDetector(HDET hDet, LPSTR lpszAuth, LPSTR lpszPassword);
_MCBCIO_IMPORT_ long WINAPI MIOGetStartTime(HDET hDet, long *lpCurrentTime);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetAccess(HDET hDet, LPSTR lpszAuth, LPSTR lpszPass,
    LPSTR lpszOwnerName, int nMaxOwnerName, BOOL *lpbLocked, LPINT lpnAccess);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetDetectorInfo(HDET hDet, LPSTR lpszDesc,
    int nMaxDesc, BOOL *lpbDefaultDesc, DWORD *lpdwID,
    BOOL *lpbDefaultID);
_MCBCIO_IMPORT_ BOOL WINAPI MIOSetDetectorInfo(HDET hDet, LPSTR lpszAuth,
    LPSTR lpszPass, LPSTR lpszDesc, DWORD dwID);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetAppData(HDET hDet, LPSTR lpszDataName, LPSTR lpszDefault,
    LPSTR lpszDataString, int nMaxDataString);
_MCBCIO_IMPORT_ BOOL WINAPI MIOSetAppData(HDET hDet, LPSTR lpszDataName, LPSTR lpszDataString,
    LPSTR lpszAuth, LPSTR lpszPass);
_MCBCIO_IMPORT_ BOOL WINAPI MIOGetHardwareConf(HDET hDet, LPMIOHARDCONF lpHardConf);
_MCBCIO_IMPORT_ BOOL WINAPI MIOOverride(HDET hDet, int nMCB, int nDevSeg, LPSTR lpszAuth);
_MCBCIO_IMPORT_ BOOL WINAPI MIOIsActive(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOSignalDet(HDET hDet, LPSTR lpszMessage);
_MCBCIO_IMPORT_ BOOL WINAPI MIOEndSignalDet(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOBeQuiet(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOTalkToMe(HDET hDet);
_MCBCIO_IMPORT_ BOOL WINAPI MIOWhoAreYou(HDET hDet, LPSTR lpszHostName);
_MCBCIO_IMPORT_ BOOL WINAPI MIOBeginFindServers(BOOL bLocalOnly);
_MCBCIO_IMPORT_ LPMIOSERVERDATA WINAPI MIOFindServers(BOOL bNextLana);
_MCBCIO_IMPORT_ BOOL WINAPI MIORelinquishServerData(LPMIOSERVERDATA lpServerData);
_MCBCIO_IMPORT_ BOOL WINAPI MIOEndFindServers(void);
_MCBCIO_IMPORT_ BOOL WINAPI MIONetLoaded(void);
_MCBCIO_IMPORT_ void WINAPI MIOGetLocalHost(LPSTR lpszHost, int nMaxHost);
_MCBCIO_IMPORT_ long WINAPI MIOlocaltime(long lStartTime, LPMIOTM lptmStartTime);
#define _MCBCIO32_EXPORTSDEFINED
#endif /* #ifndef _MCBCIO32_NOEXPORTS */
#endif /* #ifndef _MCBCIO32_EXPORTSDEFINED */
#ifdef __cplusplus
}
#endif
