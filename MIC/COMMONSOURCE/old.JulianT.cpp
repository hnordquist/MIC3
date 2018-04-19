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
// JulianT.cpp
 
#include "stdafx.h"
#include "JulianT.h"

CJulianTime::CJulianTime()
{
}

void
CJulianTime::Init(unsigned Input)
{
	m_dTotalSeconds = Input;
	m_dDouble = (double)m_dTotalSeconds;
	itoa(Input,m_pCharacter,10);
	FinishInit();
}

void
CJulianTime::Init(char*Input)
{
	m_dTotalSeconds = (unsigned)atoi(Input);
	strcpy(m_pCharacter,Input);
	sscanf(Input,"%Lf",&m_dDouble);
	FinishInit();
}

void
CJulianTime::FinishInit()
{
/*
	COleDateTime cBaseTime(1952,1,1,0,0,0);
	COleDateTime cThisTime;
	if (m_dTotalSeconds > INT_MAX)
	{
		COleDateTimeSpan cOffset(0,0,0,m_dTotalSeconds - INT_MAX);
		cThisTime = cBaseTime + cOffset;
		cOffset.SetDateTimeSpan(0,0,0,INT_MAX);
		cThisTime += cOffset;
	}
	else
	{
		COleDateTimeSpan cOffset(0,0,0,m_dTotalSeconds);
		cThisTime = cBaseTime + cOffset;
	}
	
	m_bIsValid = (cThisTime.GetStatus()==COleDateTime::valid);

	if (!m_bIsValid)
		cThisTime = cBaseTime;

	m_Year = cThisTime.GetYear();
	m_Month = cThisTime.GetMonth();
	m_Day = cThisTime.GetDay();
	m_Hour = cThisTime.GetHour();
	m_Minute = cThisTime.GetMinute();
	m_Second = cThisTime.GetSecond();
	m_MSecond = 0;

	sprintf(m_pDate,"%s",cThisTime.Format("%Y.%m.%d"));
	sprintf(m_pTime,"%s",cThisTime.Format("%H:%M:%S"));
*/	
	int year_num;
	int month_num;
	int day_num;
	int iyrprm;
	int mnthprm;
	m_bIsValid = true;
	double double_julian;
	long   long_julian;
	double time_frac;
//	strcpy(m_pCharacter,Input);
//	sscanf(Input,"%Lf",&m_dDouble);
//	sscanf(Input,"%I64f",m_I64);
	double_julian = m_dDouble/SECONDS_PER_YEAR;
	double_julian = double_julian - BASE_OFFSET_DAYS;
	long_julian = (long) double_julian;
	// add .5 milliseconds for roundoff 
	time_frac = double_julian - long_julian + HALF_MILLISECOND;
	
	iyrprm = (int)((long_julian - 122.1) / 365.25);
	mnthprm = (int)((long_julian - (long)(365.25 * iyrprm)) / 30.6001);
	day_num = long_julian - (int)(365.25 * iyrprm);
	day_num = day_num - (int)(30.6001 * mnthprm);
	
	if (mnthprm < 14) month_num = mnthprm - 1;
	else month_num = mnthprm - 13;
	
	if (month_num > 2) year_num = BASE_YEAR + iyrprm;
	else year_num = BASE_YEAR + iyrprm + 1;

	if (year_num > 9999) 
	{
		year_num = 9999;
		m_bIsValid = false;
	}

//	if (year_num < 1982)
//	{
//		year_num = 1982;
//		m_bIsValid = false;
//	}

	m_Year = year_num;
	m_Month = month_num;
	m_Day = day_num;

	m_Hour = (int) (time_frac * 24.0);

	time_frac = time_frac - (double)(m_Hour) / 24.0;
	m_Minute = (int) (time_frac * 60.0 * 24.0);

	time_frac = time_frac - (double)(m_Minute) / 24.0 / 60.0;
	m_Second = (int) (time_frac * 60.0 * 60.0 * 24.0);

	time_frac = time_frac - (double)(m_Second) / 24.0 / 60.0 / 60.0;
	m_MSecond = (int) (time_frac * 60.0 * 60.0 * 24.0 * 1000.0);

	if (m_MSecond >= 1000)
	{
	    if ((m_Hour == 23) && (m_Minute == 59) &&
						(m_Second == 59))
	    {
		m_MSecond = 999;
	    }
	    else
	    {
	        m_Second += 1;
	        m_MSecond -= 1000;

	        if (m_Second >= 60)
	        {
				m_Minute += 1;
				m_Second -= 60;

				if (m_Minute >= 60)
				{
					m_Hour += 1;
					m_Minute -= 60;
				}
			}
	    }
	}
	sprintf(m_pDate,"%04i.%02i.%02i",m_Year,m_Month,m_Day);
	sprintf(m_pTime,"%02i:%02i:%02i",m_Hour,m_Minute,m_Second);
}

//MIC_CB_028 to MIC 1.907 PJM 1/20/05
CJulianTime::CJulianTime(CString Input)
{
	Init((char*)LPCSTR(Input));
}

CJulianTime::CJulianTime(char* Input)
{
	Init(Input);
}

CJulianTime::~CJulianTime()
{
}

unsigned int
CJulianTime::TotalSeconds()
{
	return m_dTotalSeconds;
}

int
CJulianTime::Year()
{
	if (m_Year < 1982)
		return 1982;
	else
		return m_Year;
}

int
CJulianTime::Month()
{
	if ((m_Month < 1) || (m_Month > 12))
		return 0;
	else
		return m_Month;
}

int
CJulianTime::Day()
{
	if ((m_Day < 1) || (m_Day > 31))
		return 0;
	else
		return m_Day;
}

int
CJulianTime::Hour()
{
	return m_Hour;
}

int
CJulianTime::Minute()
{
	return m_Minute;
}

int 
CJulianTime::Second()
{
	return m_Second;
}

char *
CJulianTime::Date()
{
	return m_pDate;
}

char *
CJulianTime::Time()
{
	return m_pTime;
}

bool
CJulianTime::IsValid()
{
	return m_bIsValid;
}