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
// C Julian 
#include "stdafx.h"
#include "JulianT.h"
#include <afxdisp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CJulianTime::CJulianTime()
{
}

void
CJulianTime::Init(double Input)
{
	m_dDouble = Input;
	FinishInit();
}

void
CJulianTime::Init(unsigned Input)
{
	m_dDouble = (double)Input;
	FinishInit();
}

void
CJulianTime::Init(char*Input)
{
	sscanf(Input,"%lf",&m_dDouble);
	FinishInit();
}

void
CJulianTime::FinishInit()
{
	COleDateTimeSpan cSpan(m_dDouble/SECONDS_PER_YEAR);
	COleDateTime cTime(BASE_YEAR,BASE_MONTH,BASE_DAY,
		BASE_HOUR,BASE_MINUTE,BASE_SECOND);
	cTime = cTime + cSpan;
	cTime -= HALF_MILLISECOND;

	m_bIsValid = cTime.GetStatus() == COleDateTime::valid;
	sprintf(m_pDate,"%04i.%02i.%02i",
		cTime.GetYear(),cTime.GetMonth(),cTime.GetDay());
	sprintf(m_pTime,"%02i:%02i:%02i",
		cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond());
	m_Year = cTime.GetYear();
	m_Month = cTime.GetMonth();
	m_Day = cTime.GetDay();
	m_Hour = cTime.GetHour();
	m_Minute = cTime.GetMinute();
	m_Second = cTime.GetSecond();
}

CJulianTime::CJulianTime(unsigned int uiTime)
{
	m_dDouble = (double)uiTime;
	FinishInit();
}

CJulianTime::CJulianTime(double dTime)
{
	m_dDouble = dTime;
	FinishInit();
}

CJulianTime::CJulianTime(char* Input)
{
	Init(Input);
}

//MIC_CB_028 to MIC 1.907 PJM 1/20/05
CJulianTime::CJulianTime(CString csInput)
{
	Init((char*)LPCSTR(csInput));
}

CJulianTime::~CJulianTime()
{
}

unsigned int
CJulianTime::TotalSeconds()
{
	//return (unsigned int)(m_dDouble * SECONDS_PER_YEAR);
	return (unsigned int)(m_dDouble );  //DP fix 3-Aug-2006
}

int
CJulianTime::Year()
{
	return m_Year;
}

int
CJulianTime::Month()
{
	return m_Month;
}

int
CJulianTime::Day()
{
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