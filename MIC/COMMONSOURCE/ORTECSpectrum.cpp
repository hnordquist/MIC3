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
// ORTECSpectrum.cpp

#include "stdafx.h"
#include "ORTECSpectrum.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CORTECSpectrum::CORTECSpectrum(
		long maxchannels,
		long StartChan, 
		long LiveTime, 
		long TrueTime):

	CChnSpc(maxchannels,(long *)m_rawdata,StartChan,LiveTime,TrueTime)
{
//	TRACE("CORTECSpectrum::CTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());

	m_channelsabovethreshold = 0;
	m_maximumvalue = 0;
	m_threshold = 0;
	m_nonzerochannels = 0;
	memset(m_rawdata,0, sizeof(m_rawdata));
}

CORTECSpectrum::~CORTECSpectrum()
{
//	TRACE("CORTECSpectrum::DTOR() pid:%u tid:0x%X\n", _getpid(),GetCurrentThreadId());
}

DWELL_TYPE CORTECSpectrum::DwellType(){return m_edwelltype;}
void CORTECSpectrum::SetDwellType(DWELL_TYPE value){m_edwelltype = value;}
UINT CORTECSpectrum::ChannelsAboveThreshold(){return m_channelsabovethreshold;}
UINT CORTECSpectrum::NonZeroChannels(){return m_nonzerochannels;}
UINT CORTECSpectrum::MaximumValue(){return m_maximumvalue;}
UINT *CORTECSpectrum::RawData(){return m_rawdata;}
UINT CORTECSpectrum::Threshold(){return m_threshold;}
void CORTECSpectrum::SetThreshold (UINT value){m_threshold = value;}

void CORTECSpectrum::EvaluateRawData()
{
	//TRACE("CORTECSpectrum::EvaluateRawData()\n");
	m_channelsabovethreshold = 0;
	m_maximumvalue = 0;
	m_nonzerochannels = 0;

	for (int i = 0; i < NumChans(); i++)
	{
		if (m_rawdata[i] > 0)
			m_nonzerochannels++;

		if (m_rawdata[i] > m_threshold)
			m_channelsabovethreshold++;

		if (m_rawdata[i] > m_maximumvalue)
			m_maximumvalue = m_rawdata[i];
	}
}