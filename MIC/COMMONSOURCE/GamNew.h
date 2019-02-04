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
// GamNew.h

#ifndef GAMNEW_H
#define GAMNEW_H

#include  <fstream>
using namespace std;
// be sure to pack data structures on byte boundaries
#pragma pack(1)

#define  GAM_VERSION     "MIC 2.0"

// useful time constants
#define GAM_SECONDS_PER_DAY           86400.
#define GAM_DAYS_PER_YEAR             365.2422
#define GAM_DOS_TIME_ZERO_DAYS        6575.
#define GAM_SECONDS_1900_TO_1970      2208988800L

//  Library status codes 
#define GAM_SUCCESS  0
#define GAM_DB_ERROR  1
#define GAM_DB_NOT_OPEN  2
#define GAM_NO_MEMORY  3
#define GAM_DB_UNAVAILABLE  4
#define GAM_TR_ACTIVE  5
#define GAM_OPEN_ERROR  6
#define GAM_NOT_FOUND  7
#define GAM_FIND_ERROR  8
#define GAM_READ_ERROR  9
#define GAM_INPUT_ERROR  10
#define GAM_BAD_INTERP  11
#define GAM_CREATE_ERROR  12
#define GAM_WRITE_ERROR  13
#define GAM_LSQ_FAILURE  14
#define GAM_RF_ERROR  15
#define GAM_NO_TAIL  16
#define GAM_BAD_BKG  17
#define GAM_BAD_FRACTIONS 18
#define GAM_NO_REGION 19

class CGamLib
{
public:
	// constructor
	CGamLib();
	
	// destructor
	~CGamLib()  {;}

	bool         CheckVersion(char *);  // check consistency of version
	long         Error();  // get error status	
	const char * GamErrMsg();  // get error message
	void         SetError(long status=GAM_SUCCESS);  // set error status
};

// Symbolic definition of the storage formats
#define  GAM_DF_DEFAULT       0
#define  GAM_DF_RT11          1
#define  GAM_DF_VAX           2
#define  GAM_DF_N1_STD        3
#define  GAM_DF_CANB_S100     4
#define  GAM_DF_ORTEC_CHN     5
#define  GAM_DF_INEL_PCGAP    6
#define  GAM_DF_ASCII         7
#define  GAM_DF_ORTEC_SPC     8
#define  GAM_DF_CANB_CAM      9
#define  GAM_DF_CANB_G2K      10
#define  GAM_DF_IAEA_MCRS     11
#define  GAM_DF_IAEA_MMCA     12
#define  GAM_DF_SBS           13

#define  GAM_DF_TYPE_0        "NIS-5 minimum"
#define  GAM_DF_TYPE_1        "old RT-11"
#define  GAM_DF_TYPE_2        "old VAX/VMS"
#define  GAM_DF_TYPE_3        "NIS-5 standard"
#define  GAM_DF_TYPE_4        "Canberra S100"
#define  GAM_DF_TYPE_5        "Ortec 'chn'"
#define  GAM_DF_TYPE_6        "INEL pcgap"
#define  GAM_DF_TYPE_7        "ASCII"
#define  GAM_DF_TYPE_8        "Ortec 'spc'"
#define  GAM_DF_TYPE_9        "Canberra CAM"
#define  GAM_DF_TYPE_10       "Genie 2000"
#define  GAM_DF_TYPE_11       "IAEA MCRS"
#define  GAM_DF_TYPE_12       "IAEA MMCA"
#define  GAM_DF_TYPE_13       "GreenStar"

// definition of the S100 header
#define GAM_S100_SPEC_ID_LEN          26
#define GAM_S100_HEADER_LEN           128

struct  GAM_ST {
	unsigned long  sec; /* time in sec since 00:00:00 GMT 1-JAN-70 */
	short   msec;       /* milli-seconds */
	short   timezone;
	short   dstflag;
};

struct  GAM_ET {
	long    live_time;  /* in hundredths of a second */
	long    true_time;  /* in hundredths of a second */
	long    sweeps;     /* Elapsed MCSR sweeps */
	double  comp;       /* Elapsed Computationsal Preset */
};

struct  GAM_CA {
	float   second_order_term;
	float   first_order_term;
	float   constant_term;
	char    unit_name[5];
	char    unit_type;
	char    format;
	char    order;    /* 1-first order; 2-second order */
};


struct  GAM_S100_HEADER {
	short          readout_type;    /* generally 2; i.e. spectrum data */
	short          mca_number;
	short          readout_region;    /* 0 - full, 1 - 1st half, etc. */
	long           tag_number;
	char           spec_id[GAM_S100_SPEC_ID_LEN];
	short          acquire_mode;    /* generally 0x180; i.e. PHA+ */
	struct GAM_ST  start_time;
	struct GAM_ET  elapsed_time;
	struct GAM_CA  calibration;
	short          filler[19];
	short          num_channels;
};

// Library status codes
#define GAM_SUCCESS  0
#define GAM_DB_ERROR  1
#define GAM_DB_NOT_OPEN  2
#define GAM_NO_MEMORY  3
#define GAM_DB_UNAVAILABLE  4
#define GAM_TR_ACTIVE  5
#define GAM_OPEN_ERROR  6
#define GAM_NOT_FOUND  7
#define GAM_FIND_ERROR  8
#define GAM_READ_ERROR  9
#define GAM_INPUT_ERROR  10
#define GAM_BAD_INTERP  11
#define GAM_CREATE_ERROR  12
#define GAM_WRITE_ERROR  13
#define GAM_LSQ_FAILURE  14
#define GAM_RF_ERROR  15
#define GAM_NO_TAIL  16
#define GAM_BAD_BKG  17

#pragma pack()

typedef struct _TIME_STAMP 
{
	unsigned short     Year;
	unsigned char      Month;
	unsigned char      Day;
	unsigned char      Hour;
	unsigned char      Minute;
	unsigned char      Second;
	unsigned char      Fraction;
} TIME_STAMP, *PTIME_STAMP;

class CDatTim
{
friend class CSpctrm;
public:
	// constructor
	CDatTim();
	
	// destructor
	~CDatTim();

// MEMBER FUNCTIONS

	int           Day();  // get day of month
	double        Days();  // elapsed time from start of 1970 in days
	double        DeltaSeconds(CDatTim *Then);  // calculate dela time in seconds
	const char *  GetDate(bool current=true);
	const char *  GetDate(char *tmpl, bool current=true);
	const char *  GetDate(char *tmpl, char *months, bool current=true);
	const char *  GetDate(int year, int month, int day, char *tmpl);
	const char *  GetDate(int year, int month, int day, char *tmpl, char *months);
	const char *  GetDateTime(bool current=true);
	const char *  GetDateTime(char *tmpl, bool current=true);
	const char *  GetDateTime(char *tmpl, char *months, bool current=true);
	int           Hour();  // get hours since midnight
	int           Min();  // get minutes after the hour
	int           Mon();  // get month in year
	int           MSec();  // get milliseconds after the second
	int           NumDays(int month, int year);  // get number of days in month
	void          ReadTimeStamp(PTIME_STAMP pTimeStamp);
	int           Sec();  // get seconds after the minute
	unsigned long Seconds(void);  // elapsed time from start of 1970 in seconds
	unsigned long SecondsGMT(void);  // elapsed time from start of 1970 in seconds
	bool          SetDate(char *string);	
	bool          SetDate(char *string, char *tmpl);	
	bool          SetDate(char *string, char *tmpl, char *months);	
	bool          SetDate(int year, int month, int day);
	void          SetDateTime(void);  // set current date and time
	bool          SetDateTime(int year, int month, int day, int hour=0,
					int min=0, int sec=0, int msec=0);
	void          SetDateTime(unsigned long seconds);
	bool          SetDateTime(char *string, char *tmpl, char *months);	
	bool          SetTemplate(char *tmpl);
	bool          SetTime(int hour, int min, int sec=0, int msec=0);
	bool          SetZone(int zone);
	int           TZ();  // get time zone correction
	bool          ValidDate(char *string);
	bool          ValidDate(char *string, int *index);
	bool          ValidDate(char *string, char *tmpl);
	bool          ValidDate(char *string, char *tmpl, char *months);
	bool          ValidDate(char *string, char *tmpl, int *index);
	bool          ValidDate(char *string, char *tmpl, char *months, int *index);
	void          WriteTimeStamp(PTIME_STAMP pTimeStamp);
	int           Year();  // get year
	
// OPERATOR OVERLOADS	
	CDatTim & operator = (CDatTim);

private:	
	char szDateTime[24];// buffer for constructing date and time string
	int   iDay;         // day of the month,  1...n where n = # days in month
	int   iHour;        // hours since midnight,  0...23
	int   iMilliSecond; // milliseconds after the second,  0...999
	int   iMinute;      // minutes after the hour,  0...59
	int   iMonth;       // month of the year,  1...12
	int   iSecond;      // seconds after the minute,  0...59
	char szTemplate[24];// buffer for storing date template
	int   iTimeZone;    // Greenwich time minus local time in hours	
	int   iYear;        // current year
		
	bool   CheckDate(char *string, char *tmpl, char *months, int *index, int *year,
					int *month, int *day);
	bool   CheckTime(char *string, int *hour, int *minute, int *second);
	bool   LeapYear(int year);
	double Seconds(int year);
};

// symbolic constants
#define  CS_MAX_CHANNELS    16384
#define  CS_FILENAME_LEN    127
#define  CS_BUFFER_DIM      128

class CSpctrm : public CGamLib
{
public:
	// constructor
	CSpctrm(long max, long *spctrm, long StartChan=0L,
		long LiveTime=0L, long TrueTime=0L);
	
	// destructor
	virtual ~CSpctrm();
	
	CDatTim *    DateTime();  // get the date-time structure
	int          Day();  // get day of month from date-time structure
	double       Days();  // get elapsed time in days from start of 1970.
	const char * GetDate(void);  // get formatted date
	const char * GetDate(char *tmpl);  // get formatted date
	const char * GetDateTime(void);  // get formatted date and time
	const char * GetDateTime(char *tmpl);  // get formatted date and time
	const char * GetDateTime(char *tmpl, char *months);  // get formatted date and time
	int          Hour();  // get hour of day from date-time structure
	long         LiveTime();  // get the live time
	long         MaxChans();  // get the maximum # of channels
	int          Min();  // get minute of hour from date-time structure
	int          Mon();  // get month of year from date-time structure
	int          MSec();  // get milliseconds from date-time structure
	long         NumChans();  // get the actual # of channels
	int          NumDays(int month, int year);  // get number of days in month
	void         ResetDateTime();  // set current time in date-time structure
	int          Sec();  // get second of minute from date-time structure
	void         SetLiveTime(long);  // set the live time
	void         SetMaxChans(long nchans);  // change maximum # channels
	void         SetNumChans(long nchans);  // change actual # channels
	void         SetStartChan(long chan=0L);  // change starting channel
	bool         SetTemplate(char *tmpl);  // change date-time template
	void         SetTrueTime(long);  // set the true time
	long *      pSpectrum();  // get pointer to the spectral data
	long         Spectrum(long);  // get contents of a specific channel
	long         StartChan();  // get the starting channel
	int          StorageFormat(char *format);  // format string -> code
	const char * StorageFormat(int format);  // format code -> string
	void         StoreFileName(char *file);  // save file name
	long         TrueTime();  // get the true time
	int          TZ();  // get timezone correction from date-time structure
	void         XferDateTime(CSpctrm *);  // copy date-time info
	int          Year();  // get year from date-time structure
	
protected:
	char       sBuffer[CS_BUFFER_DIM];
	CDatTim    xDateTime;
	char      szFileName[CS_FILENAME_LEN+1];
	long       lLiveTime;
	long       lMaxChans;
	long       lNumChans;
	long *   palSpectrum;
	long       lStartChan;
	long       lTrueTime;
	
	bool  ReadExtraInfo(ifstream *stream, char *Buffer, int ByteCount);
	bool  ReadFloatCounts(ifstream *stream, int RecordLength);
	bool  ReadLongCounts(ifstream *stream, long Flag1, long Flag2, int Bytes);
	bool  ReadS100Header(ifstream *stream, GAM_S100_HEADER *Buffer);
	bool  WriteExtraInfo(ofstream *stream, char *Buffer, int ByteCount);
	bool  WriteFloatCounts(ofstream *stream, int RecordLength);
	bool  WriteLongCounts(ofstream *stream, int BlockSize);
	bool  WriteS100Header(ofstream *stream, GAM_S100_HEADER *Buffer);
};

// be sure to pack data structures on byte boundaries
#pragma pack(1)

#define  GAM_CHN_HEADER_LEN   32
#define  GAM_CHN_TRAILER_LEN  512

/*----------------------------------------------------------------------------
 *  Define a data structure for the ORTEC CHN header and trailer blocks.
 *--------------------------------------------------------------------------*/

struct  GAM_CHN_HEADER 
{
	short   flag;              /* must be -1 */
	short   mca_number;
	short   segment_number;
	char    seconds[2];        /* ASCII seconds of start time */
	long    real_time;         /* in increments of 20 ms */
	long    live_time;         /* in increments of 20 ms */
	char    start_date[8];     /* DDMMMYYn where n designates the century */
	char    start_time[4];     /* HHMM */
	short   channel_offset;
	short   num_channels;
};

struct  GAM_CHN_TRAILER 
{
	short   flag;              /* must be -101 or -102 */
	char    reserved1[2];
	double  energy_zero;       /* 0.0 for uncalibrated spectrum */
	double  energy_slope;      /* 1.0 for uncalibrated spectrum */
	char    reserved2[4];
	double  shape_zero;        /* 1.0 for uncalibrated spectrum */
	double  shape_slope;       /* 0.0 for uncalibrated spectrum */
	char    reserved3[232];
	char    det_desc_len;      /* length of detector description */
	char    det_desc[63];      /* detector description */
	char    sample_desc_len;   /* length of sample description */
	char    sample_desc[63];   /* sample description */
	char    reserved4[128];
};


class CChnSpc : public CSpctrm 
{
public:
	CChnSpc(long max, long *spectrum, long StartChan=0L,
		long LiveTime=0L, long TrueTime=0L);
	
	~CChnSpc();
	
	bool Read(char *file, char *buf=NULL, int num=0);
	
	bool Write(char *file, char *buf=NULL, int num=0);
	
private:
	struct GAM_CHN_HEADER  xHeader;
	struct GAM_CHN_TRAILER xTrailer;
	
	bool ReadChnHeader(ifstream *stream);
	bool ReadChnTrailer(ifstream *stream);
	bool WriteChnHeader(ofstream *stream);
	bool WriteChnTrailer(ofstream *stream);
};

#pragma pack()

class CChnSpcEx : public CChnSpc 
{
public:
	CChnSpcEx(long max, long *spectrum, long StartChan=0L, long LiveTime=0L, long TrueTime=0L);
	bool SetTime(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMSecond);
};

#endif GAMNEW_H