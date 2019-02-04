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
// GamNew.cpp

// This was derived from Tom Kelly's gam library.  We skinnied it up for MIC.
// pjm 13-feb-2006

#include "stdafx.h"
#include  <stdio.h>
#include  <fstream>
#include  <string.h>
#include  <stdlib.h>
#include  <time.h>
#include  <ctype.h>
#include "gamnew.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
using namespace std;
static long  lError;

/*++ constructor */
CGamLib::CGamLib() {
	lError = 0L;
}

/*++ check consistency of version */
bool CGamLib::CheckVersion(char *version) {
	if ( strcmp(version, "V4.1")==0 ) {
		return true;
	}
	else {
		return false;
	}
}

/*++ retrieve error */
long CGamLib::Error() {
	return lError;
}

/*++ set error status */
void CGamLib::SetError(long error) {
	lError = error;
}

/*++ retrieve error mesage */
#define  MAX_GAM_MESSAGES   20	
static const char *Message[MAX_GAM_MESSAGES]= {
    "Successful completion",
    "Database error",
    "Database has not been opened",
    "Failure to allocate dynamic memory",
    "Database is not available",
    "Transaction active",
    "Cannot open file",
    "File not found",
    "Cannot find file",
    "Error while reading file",
    "Invalid input",
    "Interpolation failure",
    "Cannot create file",
    "Error while writing file",
    "Error in least squares calculations",
    "Error in calculating a response function",
    "No significant tail was found",
    "Questionable background",
    "Isotopic fractions are invalid",
    "This peak is not linked to a region",
};
static const char *NoMessage = "Unknown Error";
const char * CGamLib::GamErrMsg() {
	long  index;
	
	index = lError;
	if ( index >= 0  &&  index < MAX_GAM_MESSAGES ) {
		return Message[index];
	}
	else {
		return NoMessage;
	}
}

// global variables
static int    iDaysInMonth[12]={31,29,31,30,31,30,31,31,30,31,30,31};
static char   sDfltMonths[37]={"JanFebMarAprMayJunJulAugSepOctNovDec"};


/*++ constructor */
CDatTim::CDatTim() {
	this->SetDateTime();
	iTimeZone = 0;
	memset(szDateTime, 0, 24);
	memset(szTemplate, 0, 24);
}

/*++ destructor */
CDatTim::~CDatTim() {
}

/*++ overload of assignment operator */
CDatTim & CDatTim::operator = (CDatTim Temp) {
	this->iYear = Temp.iYear;
	this->iMonth = Temp.iMonth;
	this->iDay = Temp.iDay;
	this->iHour = Temp.iHour;
	this->iMinute = Temp.iMinute;
	this->iSecond = Temp.iSecond;
	this->iMilliSecond = Temp.iMilliSecond;
	this->iTimeZone = Temp.iTimeZone;
	memcpy(this->szDateTime, Temp.szDateTime, 24);	
	memcpy(this->szTemplate, Temp.szTemplate, 24);	
	return *this;
}

/*++ retrieve each field */
int CDatTim::Year() {
	return iYear;
}
int CDatTim::Mon() {
	return iMonth;
}
int CDatTim::Day() {
	return iDay;
}
int CDatTim::Hour() {
	return iHour;
}
int CDatTim::Min() {
	return iMinute;
}
int CDatTim::Sec() {
	return iSecond;
}
int CDatTim::MSec() {
	return iMilliSecond;
}
int CDatTim::TZ() {
	return iTimeZone;
}

/*++ check for a valid date */
bool CDatTim::CheckDate(char *string, char *tmpl, char *months, int *index, int *year, int *month, int *day) {
	int     digit;
	int     i=0;     // position in string
	int     j=0;     // num digits in year
	int     k=0;     // position in month string
	bool    ok=true;
	int     pos;
	char    temp[4];
	int     year_digits=0;
    
    *year = 0;
    *month = 0;
    *day = 0;
	pos = 0;  
	temp[0] = '\0';
	temp[1] = '\0';
	temp[2] = '\0';
	temp[3] = '\0';
	while ( ok && string[i]!='\0' && tmpl[i]!='\0' ) {
		if ( tmpl[i]=='d' ) {
			if ( pos==0 ) {
				pos = i;
			}
			if ( isdigit(string[i]) ) {
				digit = (int)(string[i] - '0');
				*day = 10 * (*day) + digit;
				if ( *day>31 ) {
					ok = false;
				}
 			}
			else {
				ok = false;
			}
		}
		else if ( tmpl[i]=='m' ) {
			if ( pos==0 ) {
				pos = i;
			}
			if ( isdigit(string[i]) ) {
				digit = (int)(string[i] - '0');
				*month = 10 * (*month) + digit;
				if ( *month>12 ) {
					ok = false;
				}
 			}
			else {
				ok = false;
			}
		}
		else if ( tmpl[i]=='M' ) {
			if ( pos==0 ) {
				pos = i;
			}
			if ( k<3 ) {
				temp[k] = string[i];
				k++;
				if ( k==3 ) {
					*month = 1;
					while ( *month<13 && strncmp(temp,&months[3*(*month-1)],3)!=0 ) {
						(*month)++;
					}
					if ( *month>12 ) {
						ok = false;
					}
				}
			}
			else {
				ok = false;
			}
		}
		else if ( tmpl[i]=='y' ) {
			if ( pos==0 ) {
				pos = i;
			}
			if ( isdigit(string[i]) ) {
				year_digits++;
				digit = (int)(string[i] - '0');
				*year = 10 * (*year) + digit;
			}
			else {
				ok = false;
			}
		}
		else {
			if ( string[i]!=tmpl[i] ) {
				ok = false;
				pos = i;
			}
			else {
				pos = 0;
			}
		}
		i++;
	}
	if ( year_digits<3 ) {
		if ( *year<52 ) {
			*year += 2000;
		}
		else {
			*year += 1900;
		}
	}
	if ( k>0  &&  k<3 ) {
		ok = false;
		pos = 0;
	}
	else if ( tmpl[i]<=' ' && *month>0 && *month<13 && *day>iDaysInMonth[*month-1] ) {
		ok = false;
		pos = 0;
	}
	// make sure Feb 29 occurs only in a leap year
	else if ( *month==2  &&  *day==29  &&  !LeapYear(*year) ) {
		ok = false;
		pos = 0;
	}
	else {
		pos = i;
	}
	*index = pos;
	return ok;
}

/*++ check for a valid time */
bool CDatTim::CheckTime(char *string, int *hour, int *minute, int *second) {
	int     digit;
	int     pos=0;
	int     value;

	while ( string[pos]==' ' ) {
		pos++;
	}
	value = 0;
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	*hour = value;
	if ( *hour>23 ) {
		goto time_error;
	}
	if ( string[pos++]!=':' ) {
		goto time_error;
	}
	value = 0;
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	*minute = value;
	if ( *minute>59 ) {
		goto time_error;
	}
	if ( string[pos++]!=':' ) {
		goto time_error;
	}
	value = 0;
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	if ( isdigit(string[pos]) ) {
		digit = (int)(string[pos++] - '0');
		value = 10 * value + digit;
	}
	else {
		goto time_error;
	}
	*second = value;
	if ( *second>59 ) {
		goto time_error;
	}
	return true;
time_error:
	return false;
}

/*++ calculate elapsed time from start of 1900 in days */
double CDatTim::Days() {
    double dDays;
    int     n;

    dDays = 0.;
    if ( iYear<1900 ) {
    	return dDays;
    }
    n = 1900;
    while ( n<iYear ) {
        dDays += 365.;
        if ( LeapYear(n) ) {
            dDays += 1.;
		}
        n++;
	}
    n = 0;
    while ( n<iMonth-1 ) {
        dDays += (double)(iDaysInMonth[n]);
        n++;
	}
    if ( !LeapYear(iYear) && n>1 ) {
        dDays -= 1.;
	}
    dDays += (double)(iDay - 1);
    dDays += (double)iHour / 24.;
    dDays += (double)iMinute / 1440.;
    dDays += (double)iSecond / 86400.;
	return dDays;
}

/*++ compute the difference between now and then in seconds */
double CDatTim::DeltaSeconds(CDatTim *Then) {
	int      year;
	double   temp1;
	double   temp2;

	year = Then->Year();
	if ( year>iYear ) {
		year = iYear;
	}
	temp1 = this->Seconds(year);
	temp2 = Then->Seconds(year);
	return temp1-temp2;
}

/*++ get date string */
const char * CDatTim::GetDate(bool current) {
	const char *string;
	char        tmpl[24];
	
	strcpy(tmpl, szTemplate);
	string = GetDate(tmpl, sDfltMonths, current);
	return string;
}
const char * CDatTim::GetDate(char *tmpl, bool current) {
	const char *string;
	
	string = GetDate(tmpl, sDfltMonths, current);
	return string;
}
const char * CDatTim::GetDate(char *tmpl, char *months, bool current) {
	int     day;
	char    day_string[2];
	int     i;
	int     j;
	int     k1 = 0;
	int     k2 = 0;
	int     k3 = 0;
	int     k4 = 0;
	int     month;
	char    month_name[4];
	char    month_string[2];
	int     year;
	char    year_string[4];
	
	if ( current ) {
		SetDateTime();
	}
	day = iDay;
	for (i=1; i<3; i++) {
		j = day % 10;
		day_string[2-i] = (char)('0' + j);
		day /= 10;
	}
	month = iMonth;
	for (i=1; i<3; i++) {
		j = month % 10;
		month_string[2-i] = (char)('0' + j);
		month /= 10;
	}
	year = iYear;
	for (i=1; i<5; i++) {
		j = year % 10;
		year_string[4-i] = (char)('0' + j);
		year /= 10;
	}
	i = 0;
	j = 0;
	while ( i<24   &&   tmpl[i]>' ' ) {
		if ( tmpl[i]=='y' )  j++;
		i++;
	}
	if ( j>4 ) {
		j = 4;
	}
	j = 4 - j;
	if ( strlen(months)>=36 ) {
		strncpy(month_name, &months[3*(iMonth-1)], 3);
		month_name[3] = '\0';
	}
	else {
		memset(month_name,0,4);
	}
	i = 0;
	while ( i<24   &&   tmpl[i]>' ' ) {
		if ( tmpl[i]=='d' && k1<2 ) {
			szDateTime[i] = day_string[k1];
			k1++;
		}
		else if ( tmpl[i]=='m'   &&   k2<2 ) {
			szDateTime[i] = month_string[k2];
			k2++;
		}
		else if ( tmpl[i]=='M'   &&   k3<3 ) {
			szDateTime[i] = month_name[k3];
			k3++;
		}
		else if ( tmpl[i]=='y'   &&   k4<4 ) {
			szDateTime[i] = year_string[j+k4];
			k4++;
		}
		else {
			szDateTime[i] = tmpl[i];
		}
		i++;
	}
	szDateTime[i] = '\0';
	return szDateTime;	
}

/*++ convert date to specified format */
const char * CDatTim::GetDate(int year, int month, int day, char *tmpl) {
	const char *string;
	
	SetDateTime(year, month, day);
	string = GetDate(tmpl, sDfltMonths, false);
	return string;
}
const char * CDatTim::GetDate(int year, int month, int day, char *tmpl, char *months) {
	const char *string;
	
	SetDateTime(year, month, day);
	string = GetDate(tmpl, months, false);
	return string;
}

/*++ get date and time string */
const char * CDatTim::GetDateTime(bool current) {
	const char *string;
	char        tmpl[24];
	
	strcpy(tmpl, szTemplate);
	string = GetDateTime(tmpl, current);
	return string;
}
const char * CDatTim::GetDateTime(char *tmpl, bool current) {
	const char *string;

	string = GetDateTime(tmpl, sDfltMonths, current);
	return string;
}
const char * CDatTim::GetDateTime(char *tmpl, char *months, bool current) {
	int     i;
	int     ones;
	int     tens;
	
	GetDate(tmpl, months, current);
	
	i = 0;
	while ( i<23  &&  szDateTime[i]!='\0' ) {
		i++;
	}
	szDateTime[i++] = ' ';
	
	tens = iHour / 10;
	ones = iHour % 10;
	if ( i<23 ) {
		szDateTime[i++] = (char)('0' + tens);
	}
	if ( i<23 ) {
		szDateTime[i++] = (char)('0' + ones);
	}
	if ( i<23 ) {
		szDateTime[i++] = ':';
	}
	
	tens = iMinute / 10;
	ones = iMinute % 10;
	if ( i<23 ) {
		szDateTime[i++] = (char)('0' + tens);
	}
		if ( i<23 ) {
		szDateTime[i++] = (char)('0' + ones);
	}
		if ( i<23 ) {
		szDateTime[i++] = ':';
	}
	
	tens = iSecond / 10;
	ones = iSecond % 10;
	if ( i<23 ) {
		szDateTime[i++] = (char)('0' + tens);
	}
	if ( i<23 ) {
		szDateTime[i++] = (char)('0' + ones);
	}
	
	szDateTime[i] = '\0';
	return szDateTime;
}

/*++ test for a leap year */
bool CDatTim::LeapYear(int year) {
	if ( year%4!=0 ) {
		return false;
	}
	else if ( year%100==0   &&  year%400!=0 ) {
		return false;
	}
	else {
		return true;
	}
}

/*++ number of days in month */
int CDatTim::NumDays(int month, int year) {
	int   iDays;

	if ( month<1 ) {
		month = 1;
	}
	if ( month>12 ) {
		month = 12;
	}
	iDays = iDaysInMonth[month-1];
	if ( month==2 && !LeapYear(year) ) {
		iDays--;
	}
	return iDays;
}

/*++ retrieve information from a condensed time stamp structure */
void CDatTim::ReadTimeStamp(PTIME_STAMP pTimeStamp) {
	iYear = (int)pTimeStamp->Year;
	iMonth = (int)pTimeStamp->Month;
	iDay = (int)pTimeStamp->Day;
	iHour = (int)pTimeStamp->Hour;
	iMinute = (int)pTimeStamp->Minute;
	iSecond = (int)pTimeStamp->Second;
	iMilliSecond = (pTimeStamp->Fraction) * 4;
}

/*++ calculate elapsed time from a given year in seconds */
double CDatTim::Seconds(int year) {
	int      n;
    double   temp;

    if ( year<1970 ) {
		return 0.0;
	}
    	
    temp = 0.0;
    n = year;
    while ( n<iYear ) {
        temp += 365.;
        if ( LeapYear(n) ) {
            temp += 1.;
		}
        n++;
	}

    n = 0;
    while ( n<iMonth-1 ) {
        temp += (double)(iDaysInMonth[n]);
        n++;
	}
    if ( !LeapYear(iYear) && n>1 ) {
        temp -= 1.;
    }

    temp += (double)(iDay - 1);
    temp  = temp * 24. + (double)iHour;
    temp  = temp * 60. + (double)iMinute;
    temp  = temp * 60. + (double)iSecond;
	temp += (double)iMilliSecond / 1000.;
	return temp;
}

/*++ calculate elapsed time from start of 1970 in seconds */
unsigned long CDatTim::Seconds() {
	unsigned long   seconds;

	seconds = (unsigned long)(this->Seconds(1970)+0.5);
	return seconds;
}
unsigned long CDatTim::SecondsGMT() {
	unsigned long   seconds;

	seconds = this->Seconds();
	seconds += this->TZ() * 3600L;
	return seconds;
}

/*++ set the date */
bool CDatTim::SetDate(int year, int month, int day) {
	// test for valid input
	if ( month<1 || month>12 ) {
		return false;
	}
	if ( day<1 || day>iDaysInMonth[month-1] ) {
		return false;
	}
	// set the fields according to the input
	iYear = year;
	iMonth = month;
	iDay = day;
	return true;
}

/*++ set date from a string */
bool CDatTim::SetDate(char *string) {
	bool     ok;
	char     tmpl[24];
	
	strcpy(tmpl, szTemplate);
	ok = SetDate(string, tmpl);
	return ok;
}
bool CDatTim::SetDate(char *string, char *tmpl) {
	int     day;
	int     index;
	int     month;
	bool    ok;
	int     year;
	
	ok = CheckDate(string, tmpl, sDfltMonths, &index, &year, &month, &day);
	if ( ok ) {
		ok = SetDate(year, month, day);
	}
	return ok;
}
bool CDatTim::SetDate(char *string, char *tmpl, char *months) {
	int     day;
	int     index;
	int     month;
	bool    ok;
	int     year;
	
	ok = CheckDate(string, tmpl, months, &index, &year, &month, &day);
	if ( ok ) {
		ok = SetDate(year, month, day);
	}
	return ok;
}

/*++ set the date and time */
bool CDatTim::SetDateTime(int year, int month, int day, int hour, int min, int sec, int msec) {
	// test for valid input
	if ( month<1 || month>12 ) {
		return false;
	}
	if ( day<1 || day>iDaysInMonth[month-1] ) {
		return false;
	}
	if ( hour<0 || hour>23 ) {
		return false;
	}
	if ( min<0 || min>59 ) {
		return false;
	}
	if ( sec<0 || sec>59 ) {
		return false;
	}
	if ( msec<0 || msec>999 ) {
		return false;
	}
	// set the fields according to the input
	iYear = year;
	iMonth = month;
	iDay = day;
	iHour = hour;
	iMinute = min;
	iSecond = sec;
	iMilliSecond = msec;
	return true;
	}

/*++ set the date and time */
void CDatTim::SetDateTime(unsigned long seconds) {
    struct tm  *then;
    time_t      time=seconds;

    then = localtime(&time);  /* convert to local time data structure */
    if ( then!=(struct tm *)0 ) {
	    SetDateTime((then->tm_year)+1900, then->tm_mon+1, then->tm_mday, then->tm_hour, then->tm_min, then->tm_sec );
	}
}

/*++ set the date and time to the current date and time */
void CDatTim::SetDateTime() {
	bool        ok=false;
    time_t      ltime;
    struct tm  *now;
    int         zone;
    
    time(&ltime);                // get current time as seconds since 1970
    now = localtime(&ltime);     // convert to local time data structure
	if ( now!=(struct tm *)0 ) {
	    zone = (int)(_timezone/3600L);    
	    ok = SetDateTime(now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, 0);
		if ( ok ) {
			SetZone(zone);
		}
	}
}

/*++ set the date */
bool CDatTim::SetDateTime(char *string, char *tmpl, char *months) {
	int     day;
	int     hour;
	int     i;
	int     j;
	int     index;
	int     length;
	int     min;
	int     month;
	bool    ok;
	int     sec;
	int     year;

	length = strlen(string);
	i = 0;
	while ( i<length  &&  string[i]==' ' ) {
		i++;
	}
	j = i;
	while ( j<length  &&  string[j]!=' ' ) {
		j++;
	}
	if ( j<length ) {
		string[j++] = '\0';
	}

	ok = CheckDate(&string[i], tmpl, months, &index, &year, &month, &day);
	if ( ok ) {
		ok = CheckTime(&string[j], &hour, &min, &sec);
	}
	if ( ok ) {
		SetDate(year, month, day);
		SetTime(hour, min, sec);
	}
	return ok;
}

/*++ change the template */
bool CDatTim::SetTemplate(char *tmpl) {
	int    i=0;
	
	while( i<23  &&  tmpl[i]>=' ' ) {
		szTemplate[i] = tmpl[i];
		i++;
	}
	szTemplate[i] = '\0';
	return true;
}

/*++ set the time */
bool CDatTim::SetTime(int hour, int min, int sec, int msec) {
	// test for valid input
	if ( hour<0 || hour>23 ) {
		return false;
	}
	if ( min<0 || min>59 ) {
		return false;
	}
	if ( sec<0 || sec>59 ) {
		return false;
	}
	if ( msec<0 || msec>999 ) {
		return false;
	}
	// set the fields according to the input
	iHour = hour;
	iMinute = min;
	iSecond = sec;
	iMilliSecond = msec;
	return true;
}

/*++ set the time zone */
bool CDatTim::SetZone(int zone) {
	// test for valid input
	if ( zone<0 || zone>12 ) {
		return false;
	}
	// set the fields according to the input
	iTimeZone = zone;
	return true;
}

/*++ check for valid date */
bool CDatTim::ValidDate(char *string) {
	int   index;
	bool  ok;
	
	ok = ValidDate(string, &index);
	return ok;
}
bool CDatTim::ValidDate(char *string, int *Index) {
	bool  ok;
	
	ok = ValidDate(string, szTemplate, Index);
	return ok;
}
bool CDatTim::ValidDate(char *string, char *tmpl) {
	int   index;
	bool  ok;
	
	ok = ValidDate(string, tmpl, &index);
	return ok;
}
bool CDatTim::ValidDate(char *string, char *tmpl, char *months) {
	int   index;
	bool  ok;
	
	ok = ValidDate(string, tmpl, months, &index);
	return ok;
}
bool CDatTim::ValidDate(char *string, char *tmpl, int *index) {
	int    day;
	int    month;
	bool   ok;
	int    year;
	
	ok = CheckDate(string, tmpl, sDfltMonths, index, &year, &month, &day);
	return ok;
}
bool CDatTim::ValidDate(char *string, char *tmpl, char *months, int *index) {
	int    day;
	int    month;
	bool   ok;
	int    year;
	
	ok = CheckDate(string, tmpl, months, index, &year, &month, &day);
	return ok;
}

/*++ condense information into a time stamp structure */
void CDatTim::WriteTimeStamp(PTIME_STAMP pTimeStamp) {
	pTimeStamp->Year = (short)iYear;
	pTimeStamp->Month = (unsigned char)iMonth;
	pTimeStamp->Day = (unsigned char)iDay;
	pTimeStamp->Hour = (unsigned char)iHour;
	pTimeStamp->Minute = (unsigned char)iMinute;
	pTimeStamp->Second = (unsigned char)iSecond;
	pTimeStamp->Fraction = (unsigned char)(iMilliSecond/4);
}

// global data structures
static union  DATUM
    {
    long           long_field;
    float          real_field;
    unsigned char  byte_field[4];
    short          int_field[2];
    } gDatum;
static union  FLAG_DATA
    {
    long    long_field;
    char    byte_field[4];
    } gFlagData;

// prototypes for static functions
bool  pvt_check_flag(long, int);

/*++ constructor */
CSpctrm::CSpctrm(long MaxChans, long *spectrum, long StartChan, long LiveTime, long TrueTime) {
    // set the current day and time
    xDateTime.SetDateTime();
    // set the live time and true time to zero
    lLiveTime = (LiveTime>0L) ? LiveTime : 0L;
    lTrueTime = (TrueTime>0L) ? TrueTime : 0L;
    // set the parameters describing the spectrum size
    lMaxChans = (lMaxChans>0L) ? lMaxChans : 0L;
    lMaxChans = (MaxChans<CS_MAX_CHANNELS) ? MaxChans : CS_MAX_CHANNELS;
    lStartChan = (StartChan>0L) ? StartChan : 0L;
    lNumChans = 0L;
    // set the pointer to the array of spectral counts
    palSpectrum = spectrum;
    // initialize other fields
    memset(sBuffer, 0, CS_BUFFER_DIM);
    memset(szFileName, 0, CS_FILENAME_LEN+1);
}

/*++ destructor */
CSpctrm::~CSpctrm() {
    lMaxChans = 0L;
    lStartChan = 0L;
    lNumChans = 0L;
    palSpectrum = (long *)0;
}

/*++ retrieve date-time info */
int CSpctrm::Year() {
	return xDateTime.Year();
}
int CSpctrm::Mon() {
	return xDateTime.Mon();
}
int CSpctrm::Day() {
	return xDateTime.Day();
}
int CSpctrm::Hour() {
	return xDateTime.Hour();
}
int CSpctrm::Min() {
	return xDateTime.Min();
}
int CSpctrm::Sec() {
	return xDateTime.Sec();
}
int CSpctrm::MSec() {
	return xDateTime.MSec();
}
int CSpctrm::TZ() {
	return xDateTime.TZ();
}
int CSpctrm::NumDays(int month, int year) {
	return xDateTime.NumDays(month, year);
}
double CSpctrm::Days() {
	return xDateTime.Days();
}

/*++ retrieve field values */
CDatTim * CSpctrm::DateTime() {
	return &xDateTime;
}
long CSpctrm::LiveTime() {
	return lLiveTime;
}

long CSpctrm::TrueTime() {
	return lTrueTime;
}
long  CSpctrm::MaxChans() {
	return lMaxChans;
}
long  CSpctrm::StartChan() {
	return lStartChan;
}
long  CSpctrm::NumChans() {
	return lNumChans;
}
long * CSpctrm::pSpectrum() {
	return palSpectrum;
}

/*++ retrieve a channel count */
long CSpctrm::Spectrum(long index) {
	long  i=index;
	long  j=lStartChan+lNumChans-1;
	
	i = (i>lStartChan) ? i : lStartChan;
	i = (i<j) ? i : j;
	return palSpectrum[i];
}

/*++ copy the date and time */
void CSpctrm::XferDateTime(CSpctrm *source) {
	xDateTime = *(source->DateTime());
}

/*++ set date and time to current values */
void CSpctrm::ResetDateTime() {
    xDateTime.SetDateTime();
}

/*++ set live and ture times */
void CSpctrm::SetLiveTime(long time) {
	lLiveTime = (time>0L) ? time : 0L;
}
void CSpctrm::SetTrueTime(long time) {
	lTrueTime = (time>0L) ? time : 0L;
}

/*++ change channel information */
void CSpctrm::SetMaxChans(long nchans) {
	long  nch;
	
    nch = (nchans>0L) ? nchans : 0L;
    nch = (nch<CS_MAX_CHANNELS) ? nch : CS_MAX_CHANNELS;
	lMaxChans = nch;
	lNumChans = (lNumChans>lMaxChans) ? lMaxChans : lNumChans;
}
void CSpctrm::SetNumChans(long nchans) {
	long  nch;
	
    nch = (nchans>0L) ? nchans : 0L;
    nch = (nch<CS_MAX_CHANNELS) ? nch : CS_MAX_CHANNELS;
	nch = (nch<lMaxChans) ? nch : lMaxChans;
	lNumChans = nch;
}
void CSpctrm::SetStartChan(long chan) {
	long  nch;
	
    nch = (chan>0L) ? chan : 0L;
    nch = (nch<CS_MAX_CHANNELS) ? nch : CS_MAX_CHANNELS;
	lStartChan = chan;
}

/*++ store file name */
void CSpctrm::StoreFileName(char *name) {
    strncpy(szFileName, name, CS_FILENAME_LEN);
    szFileName[CS_FILENAME_LEN] = '\0';
}

/*++ read trailer information */
bool CSpctrm::ReadExtraInfo(ifstream *InputFile, char *Buffer, int ByteCount) {
    char    ch;
    bool    more=true;
    int     n;
    bool    ok=true;

    if ( Buffer!=NULL && ByteCount>4 ) {
        gDatum.long_field = 0L;
        while ( !InputFile->bad() && !InputFile->eof() && gDatum.long_field!=-1 && gDatum.long_field!=-2 ) {
            InputFile->read((char *)&gDatum, 4);
		}
        if ( InputFile->bad() ) {
            ok = false;
		}
        else if ( InputFile->eof() ) {
            more = false;
		}

        n = 0;
        Buffer[n++] = gDatum.byte_field[0];
        Buffer[n++] = gDatum.byte_field[1];
        Buffer[n++] = gDatum.byte_field[2];
        Buffer[n++] = gDatum.byte_field[3];
        while ( n<ByteCount && ok && more ) {
            InputFile->read(&ch, 1);
            if ( InputFile->eof() ) {
                more = false;
			}
            else if ( InputFile->bad() ) {
                ok = false;
			}
            else {
                Buffer[n++] = ch;
			}
		}
	}
    return ok;
}

/*++ read spectral data in floating-point format */
bool CSpctrm::ReadFloatCounts(ifstream *InputFile, int RecordLength) {
    int     i;
    long    maxchan;
    bool    more_data = true;
    long    n;
    long    num;
    bool    ok;
    int     status = GAM_SUCCESS;
    char    tmp;

    maxchan = lStartChan + lMaxChans;
    
    ok = (!InputFile->bad() && !InputFile->eof());
    i = 0;
    n = lStartChan;
    num = 0;
    while ( n<maxchan && ok && more_data ) {
        InputFile->read( (char *)&gDatum, 4 );
        if ( InputFile->bad() ) {
            ok = false;
		}
        else if ( gDatum.long_field==-1L ) {
            more_data = false;
            InputFile->seekg( -4L, ios::cur );
		}
        else if ( gDatum.long_field==0L ) {
            palSpectrum[n] = 0L;
            n++;
            num++;
		}
        else {
            tmp                  = gDatum.byte_field[0];
            gDatum.byte_field[0] = gDatum.byte_field[2];
            gDatum.byte_field[2] = tmp;
            tmp                  = gDatum.byte_field[1];
            gDatum.byte_field[1] = gDatum.byte_field[3];
            gDatum.byte_field[3] = tmp - 0x01;
            if ( gDatum.real_field<0. ) {
                ok = false;
			}
            else {
                palSpectrum[n] = (long)(gDatum.real_field + 0.5);
			}
            n++;
            num++;
		}
        i++;
        if ( RecordLength>0 && i==RecordLength ) {
            InputFile->read((char *)&gDatum, 2);
            if ( InputFile->bad() ) {
				ok = false;
			}
            i = 0;
		}
	}
    lNumChans = num;
    return ok;
}

/*++ read spectral data in integer format */
bool CSpctrm::ReadLongCounts(ifstream *InputFile, long flag1, long flag2, int bytes) {
    long    maxchan;
    bool    more_data = true;
    long    n;
    long    num;
    bool    ok = true;

    maxchan = lStartChan + lMaxChans;

    n = lStartChan;
    num = 0;
    while ( n<maxchan && ok && more_data ) {
        InputFile->read((char *)&(gDatum), 4);
        if ( InputFile->bad() ) {
            ok = false;
		}
        else if ( InputFile->eof() ) {
            more_data = false;
		}
        else if ( pvt_check_flag(flag1, bytes) ) {
            more_data = false;
            InputFile->seekg(-4L, ios::cur);
		}
        else if ( pvt_check_flag(flag2, bytes) ) {
            more_data = false;
            InputFile->seekg(-4L, ios::cur);
		}
        else if ( gDatum.long_field < 0L ) {
            ok = false;
		}
        else {
            palSpectrum[n] = gDatum.long_field;
            n++;
            num++;
		}
	}
    lNumChans = num;
    return ok;
}

/*++ check for a specifi flag amongs the input */
static  bool  pvt_check_flag(long flag, int num) {
    int     n;
    bool    ok;

    if ( flag!=0L ) {
        ok = true;
        gFlagData.long_field = flag;
        for (n=0; n<num; n++) {
            ok = ok && (gFlagData.byte_field[n] == gDatum.byte_field[n]);
		}
	}
    else {
        ok = false;
	}
    return ok;
}

/*++ write trailer information */
bool CSpctrm::WriteExtraInfo(ofstream *OutputFile, char *Buffer, int ByteCount) {
    int    n;
    bool   ok = true;
    
    if ( Buffer!=NULL && ByteCount>4 ) {
    	memcpy((char *)&gDatum, Buffer, 4);
    	if ( gDatum.long_field!=-1L  &&  gDatum.long_field!=-2L ) {
        	Buffer[0] = -1;
        	Buffer[1] = -1;
        	Buffer[2] = -1;
        	Buffer[3] = -1;
		}
        OutputFile->write((char *)&Buffer[0], 4);
        if ( OutputFile->bad() ) {
			ok = false;
		}
        n = 4;
        while ( n<ByteCount && ok ) {
            OutputFile->write((char *)&Buffer[n], 1);
            if ( OutputFile->bad() ) {
				ok = false;
			}
            n++;
		}
	}
    return ok;
}

/*++ write spectral in floating-point format */
bool CSpctrm::WriteFloatCounts(ofstream *OutputFile, int RecordLength) {
    int     i;
    long    maxchan;
    long    n;
    bool    ok;
    char    tmp;

    maxchan = lStartChan + lNumChans;
    ok = !OutputFile->bad();
    i = 0;
    n = lStartChan;
    while ( n<maxchan && ok ) {
        gDatum.real_field = (float)palSpectrum[n++];
        if ( gDatum.long_field!=0L ) {
            tmp                  = gDatum.byte_field[0];
            gDatum.byte_field[0] = gDatum.byte_field[2];
            gDatum.byte_field[2] = tmp;
            tmp                  = gDatum.byte_field[3];
            gDatum.byte_field[3] = gDatum.byte_field[1];
            gDatum.byte_field[1] = tmp + 0x01;
		}
        OutputFile->write( (char *)&gDatum, 4 );
        if ( OutputFile->bad() ) {
            ok = false;
		}
        else {
            i++;
            if ( RecordLength>0 && i==RecordLength ) {
                OutputFile->write( (char *)&gDatum, 2 );
                if ( OutputFile->bad() ) {
					ok = false;
				}
                i = 0;
			}
		}
	}
    return ok;
}

/*++ write spectral data in integer format */
bool CSpctrm::WriteLongCounts(ofstream *OutputFile, int BlockSize) {
	long  *plBuffer;
	int      i;
    long     maxchan;
    long     n;
    long     zero=0L;
    
    if ( BlockSize<1 ) {
    	SetError(GAM_INPUT_ERROR);
    	return false;
	}
    
    plBuffer = new long [BlockSize+1];
    if ( plBuffer==(long *)0 ) {
    	SetError(GAM_NO_MEMORY);
    	return false;
	}
    
    maxchan = lStartChan + lNumChans;
	i = 1;
    n = lStartChan;
    while ( n<maxchan && !OutputFile->bad() ) {
        plBuffer[i++] = palSpectrum[n++];
        if ( i>BlockSize ) {
        	OutputFile->write( (char *)&(plBuffer[1]), sizeof(long)*BlockSize );
        	i = 1;
		}
	}
	if ( i<BlockSize  &&  !OutputFile->bad() ) {
		while ( i<BlockSize ) {
			plBuffer[i++] = zero;
		}
        OutputFile->write( (char *)&(plBuffer[1]), sizeof(long)*BlockSize );
	}
    delete [] plBuffer;
    if ( OutputFile->bad() ) {
    	return false;
	}
    else {
		return true;
	}
}

/*++ read header information in the S100 format */
bool CSpctrm::ReadS100Header(ifstream *InputFile, GAM_S100_HEADER *Header) {
    unsigned long  ulSeconds;
    
    InputFile->read((char *)Header, GAM_S100_HEADER_LEN);
    if ( InputFile->bad()  ||  Header->readout_type!=2 ) {
        return false;
	}
    
    ulSeconds = Header->start_time.sec;
	xDateTime.SetDateTime(ulSeconds);

    lLiveTime = Header->elapsed_time.live_time / 100L;

    lTrueTime = Header->elapsed_time.true_time / 100L;
	
	return true;
}

/*++ write header information in the S100 format */
bool CSpctrm::WriteS100Header(ofstream *OutputFile, GAM_S100_HEADER *Header) {

    Header->readout_type = 2;
    Header->mca_number = 1;
    Header->readout_region = 0;
    Header->tag_number = 0;
    memset( Header->spec_id, 0, GAM_S100_SPEC_ID_LEN );
    Header->acquire_mode = 0x180;
    
    Header->start_time.sec = xDateTime.SecondsGMT();
    Header->start_time.msec = 0;
    Header->start_time.timezone = xDateTime.TZ();
    Header->start_time.dstflag = 0;

    Header->elapsed_time.live_time = 100L * lLiveTime;
    Header->elapsed_time.true_time = 100L * lTrueTime;
    Header->elapsed_time.sweeps = 0L;
    Header->elapsed_time.comp = 0.;

    Header->calibration.second_order_term = 0.0F;
    Header->calibration.first_order_term = 1.0F;
    Header->calibration.constant_term = 0.0F;
    memset( Header->calibration.unit_name, 0, 5 );
    Header->calibration.unit_type = 0;       
    Header->calibration.format = 1;        
    Header->calibration.order = 1;

    memset( Header->filler, 0, 19 );
    Header->num_channels = (int)lMaxChans;

    OutputFile->write((char *)Header, GAM_S100_HEADER_LEN);
    if ( OutputFile->bad() ) {
        return false;
	}
    else {
        return true;
	}
}

/*++ change the template */
bool CSpctrm::SetTemplate(char *tmpl) {
	this->xDateTime.SetTemplate(tmpl);
	return true;
}

/*++ get date of the spectrum */
const char * CSpctrm::GetDate() {
	const char *string;
	
	string = xDateTime.GetDate(false);
	return string;
}
const char * CSpctrm::GetDate(char *tmpl) {
	const char *string;
	
	string = xDateTime.GetDate(tmpl, false);
	return string;
}

/*++ get date and time of the spectrum */
const char * CSpctrm::GetDateTime() {
	const char *string;
	
	string = xDateTime.GetDateTime(false);
	return string;
}
const char * CSpctrm::GetDateTime(char *tmpl) {
	const char *string;
	
	string = xDateTime.GetDateTime(tmpl, false);
	return string;
}
const char * CSpctrm::GetDateTime(char *tmpl, char *months) {
	const char *string;
	
	string = xDateTime.GetDateTime(tmpl, months, false);
	return string;
}

/*++ convert storage format from string to int */
int CSpctrm::StorageFormat(char *format) {
	if ( strcmp(format, GAM_DF_TYPE_1)==0 ) {
		return GAM_DF_RT11;
	}
	else if ( strcmp(format, GAM_DF_TYPE_2)==0 ) {
		return GAM_DF_VAX;
	}
	else if ( strcmp(format, GAM_DF_TYPE_3)==0 ) {
		return GAM_DF_N1_STD;
	}
	else if ( strcmp(format, GAM_DF_TYPE_4)==0 ) {
		return GAM_DF_CANB_S100;
	}
	else if ( strcmp(format, GAM_DF_TYPE_5)==0 ) {
		return GAM_DF_ORTEC_CHN;
	}
	else if ( strcmp(format, GAM_DF_TYPE_6)==0 ) {
		return GAM_DF_INEL_PCGAP;
	}
	else if ( strcmp(format, GAM_DF_TYPE_7)==0 ) {
		return GAM_DF_ASCII;
	}
	else if ( strcmp(format, GAM_DF_TYPE_8)==0 ) {
		return GAM_DF_ORTEC_SPC;
	}
	else if ( strcmp(format, GAM_DF_TYPE_9)==0 ) {
		return GAM_DF_CANB_CAM;
	}
	else if ( strcmp(format, GAM_DF_TYPE_10)==0 ) {
		return GAM_DF_CANB_G2K;
	}
	else if ( strcmp(format, GAM_DF_TYPE_11)==0 ) {
		return GAM_DF_IAEA_MCRS;
	}
	else if ( strcmp(format, GAM_DF_TYPE_12)==0 ) {
		return GAM_DF_IAEA_MMCA;
	}
	else if ( strcmp(format, GAM_DF_TYPE_13)==0 ) {
		return GAM_DF_SBS;
	}
	else {
		return GAM_DF_DEFAULT;
	}
}

/*++ convert storate format from int to string */
const char * CSpctrm::StorageFormat(int format) {
	switch (format) {
		case GAM_DF_RT11:
			return GAM_DF_TYPE_1;
		case GAM_DF_VAX:
			return GAM_DF_TYPE_2;
		case GAM_DF_N1_STD:
			return GAM_DF_TYPE_3;
		case GAM_DF_CANB_S100:
			return GAM_DF_TYPE_4;
		case GAM_DF_ORTEC_CHN:
			return GAM_DF_TYPE_5;
		case GAM_DF_INEL_PCGAP:
			return GAM_DF_TYPE_6;
		case GAM_DF_ASCII:
			return GAM_DF_TYPE_7;
		case GAM_DF_ORTEC_SPC:
			return GAM_DF_TYPE_8;
		case GAM_DF_CANB_CAM:
			return GAM_DF_TYPE_9;
		case GAM_DF_CANB_G2K:
			return GAM_DF_TYPE_10;
		case GAM_DF_IAEA_MCRS:
			return GAM_DF_TYPE_11;
		case GAM_DF_IAEA_MMCA:
			return GAM_DF_TYPE_12;
		case GAM_DF_SBS:
			return GAM_DF_TYPE_13;
		default:
			return GAM_DF_TYPE_0;
	}
}

// global variables
static  char   *gszMonths = "JanFebMarAprMayJunJulAugSepOctNovDec";

/*++ constructor */
CChnSpc::CChnSpc(
	long MaxChans, 
	long *spectrum, 
	long StartChan, 
	long LiveTime, 
	long TrueTime)
	: CSpctrm(MaxChans, spectrum, StartChan, LiveTime, TrueTime) 
{
	memset((void *)&xHeader, 0, sizeof(struct GAM_CHN_HEADER));
	memset((void *)&xTrailer, 0, sizeof(struct GAM_CHN_TRAILER));
}

/*++ destructor */
CChnSpc::~CChnSpc() 
{
}

/*++ read */
bool CChnSpc::Read(char *file, char *extra, int bytes) {
	ifstream  InputFile(file, ios::in|ios::binary);//nocreate didn't exist using ios::in instead
    
    // clear the error flag
    SetError();
	// check the input	
    if ( palSpectrum==(long *)0 ) {
        SetError(GAM_INPUT_ERROR);
        return false;
	}
    // can the input file be opened?
    if ( InputFile.fail() ) {
        SetError(GAM_OPEN_ERROR);
        return false;
	}
    // store the name of the input file
    StoreFileName(file);
	// retrieve header information        
    if ( !ReadChnHeader(&InputFile) ) {
        SetError(GAM_READ_ERROR);
        return false;
	}
    // retrieve spectral counts
    if ( !ReadLongCounts(&InputFile, -101L, -102L, 4) ) {
        SetError(GAM_READ_ERROR);
        return false;
	}
	// retrieve trailer information
    if ( !ReadChnTrailer(&InputFile) ) {
        SetError(GAM_READ_ERROR);
        return false;
	}
	// retrieve extra information
    if ( !ReadExtraInfo(&InputFile, extra, bytes) ) {
        SetError(GAM_READ_ERROR);
        return false;
	}
	return true;
}
	
/*++ write */
bool CChnSpc::Write(char *file, char *extra, int bytes) {
	ofstream   OutputFile(file, ios::binary);
	
    // clear the error flag
    SetError();
	// can the output file be created?
    if ( OutputFile.fail() ) {
        SetError(GAM_CREATE_ERROR);
        return false;
	}
    // write header information
    if ( !WriteChnHeader(&OutputFile) ) {
        SetError(GAM_WRITE_ERROR);
        return false;
	}
    // write the spectral data
	if ( !WriteLongCounts(&OutputFile, 1) ) {
		SetError(GAM_WRITE_ERROR);
		return false;
	}
	// write trailer information
    if ( !WriteChnTrailer(&OutputFile) ) {
        SetError(GAM_WRITE_ERROR);
        return false;
	}
	// write extra information
    if ( !WriteExtraInfo(&OutputFile, extra, bytes) ) {
        SetError(GAM_WRITE_ERROR);
        return false;
	}
	return true;
}

/*++ read header information */
bool CChnSpc::ReadChnHeader(ifstream *InputFile) {
    int    ch[3];
    int    day;
    bool   found = false;
    int    hour;
    int    i;
    int    j;
	long   maxchans;
    int    minute;
    int    mon;
    int    month[3];
    int    second;
    char   string[8];
    int    year;

	InputFile->read((char *)&xHeader, GAM_CHN_HEADER_LEN);
    if ( InputFile->eof() || InputFile->bad() || xHeader.flag!=-1 ) {
        return false;
	}
    string[0] = xHeader.start_date[5];
    string[1] = xHeader.start_date[6];
    string[2] = '\0';
    year = atoi( string );
/*2-Mar-1998  fix for year 2000 problem*/
	{
    int century=19;
	if ( xHeader.start_date[7]>='1'  &&  xHeader.start_date[7]<='9' ) {
		century += (int)(xHeader.start_date[7] - '0');
	}
	year += 100 * century;
	} 
/*2-Mar-1998  end fix*/
    month[0] = xHeader.start_date[2];
    month[0] = toupper( month[0] );
    month[1] = xHeader.start_date[3];
    month[1] = tolower( month[1] );
    month[2] = xHeader.start_date[4];
    month[2] = tolower( month[2] );
    i = 0;
    while ( i<12 && !found ) {
        j = 3 * i;
        ch[0] = (int)gszMonths[j];
        ch[1] = (int)gszMonths[j+1];
        ch[2] = (int)gszMonths[j+2];
        found = (bool)(ch[0]==month[0] && ch[1]==month[1] && ch[2]==month[2]);
        i++;
	}
    if ( found ) {
        mon = i;
	}
    else {
        mon = 0;
	}
	// get date and time
    string[0] = xHeader.start_date[0];
    string[1] = xHeader.start_date[1];
    string[2] = '\0';
    day = atoi( string );
    string[0] = xHeader.start_time[0];
    string[1] = xHeader.start_time[1];
    string[2] = '\0';
    hour = atoi( string );
    string[0] = xHeader.start_time[2];
    string[1] = xHeader.start_time[3];
    string[2] = '\0';
    minute = atoi( string );
    string[0] = xHeader.seconds[0];
    string[1] = xHeader.seconds[1];
    string[2] = '\0';
    second = atoi( string );
    xDateTime.SetDateTime(year, mon, day, hour, minute, second);
	// get live time and true time
	SetLiveTime(xHeader.live_time / 50L);
	SetTrueTime(xHeader.real_time / 50L);
	// get number of channels
	maxchans = (long)(xHeader.num_channels);
	maxchans = maxchans<MaxChans() ? maxchans : MaxChans();
	SetMaxChans(maxchans);
    return true;
}

/*++ read trailer information */
bool CChnSpc::ReadChnTrailer(ifstream *InputFile) 
{
    short  flag=0;

    while ( !InputFile->bad() && !InputFile->eof() && flag!=-101 && flag!=-102 ) {
        InputFile->read((char*)&flag, 2);
	}
    if ( InputFile->bad() || InputFile->eof() ) {
        return false;
	}
    xTrailer.flag = flag;
    InputFile->read((char *)&xTrailer.reserved1, GAM_CHN_TRAILER_LEN-2);
    if ( InputFile->bad() || InputFile->eof() ) 
	{
        return false;
	}
    return true;
}

/*++ write header information */
bool CChnSpc::WriteChnHeader(ofstream *OutputFile) 
{
    int   i;
    int   year;

    memset((char *)&xHeader, 0, GAM_CHN_HEADER_LEN);

    xHeader.flag = -1;
    xHeader.mca_number = 1;
    xHeader.segment_number = 1;

    sprintf(&(xHeader.seconds[0]), "%02d", xDateTime.Sec());

    xHeader.real_time = 50L * lTrueTime;
    xHeader.live_time = 50L * lLiveTime;

    sprintf(&(xHeader.start_date[0]), "%02d", xDateTime.Day());
    i = 3 * (xDateTime.Mon() - 1);
    strncpy(&(xHeader.start_date[2]), &gszMonths[i], 3);
/*2-Mar-1998  fix for year 2000 problem*/
	{
    int century;
    year = xDateTime.Year() - 1900;
    century = year / 100;
    sprintf(&(xHeader.start_date[5]), "%02d", year%100);
	sprintf(&(xHeader.start_date[7]), "%1d", century%10);
    }
/*2-Mar-1998  end fix*/

    sprintf(&(xHeader.start_time[0]), "%02d", xDateTime.Hour());
    sprintf(&(xHeader.start_time[2]), "%02d", xDateTime.Min());

    xHeader.num_channels = (int)lNumChans;
    xHeader.channel_offset = (int)lStartChan;

    OutputFile->write((char *)&xHeader, GAM_CHN_HEADER_LEN);
    if ( OutputFile->bad() ) 
	{
        return false;
	}
   	return true;
}

/*++ write trailer information */
bool CChnSpc::WriteChnTrailer(ofstream *OutputFile) 
{
    memset((char *)&xTrailer, 0, GAM_CHN_TRAILER_LEN);
    xTrailer.flag = -101;
    xTrailer.energy_zero = 0.0;
    xTrailer.energy_slope = 1.0;
    xTrailer.shape_zero = 1.0;
    xTrailer.shape_slope = 0.0;

    OutputFile->write((char *)&xTrailer, GAM_CHN_TRAILER_LEN);
    if ( OutputFile->bad() ) 
	{
    	return false;
	}
   	return true;
}

CChnSpcEx::CChnSpcEx(
	long max, long *spectrum, long StartChan,long LiveTime,long TrueTime)
	:CChnSpc(max,spectrum,StartChan,LiveTime,TrueTime){}


bool CChnSpcEx::SetTime(
	int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int iMSecond)
{
	return xDateTime.SetDateTime(iYear,iMonth,iDay,iHour,iMinute,iSecond,iMSecond);
}