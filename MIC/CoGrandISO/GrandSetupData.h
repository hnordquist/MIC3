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
// GrandSetupData.h

#if !defined(AFX_GRANDSETUPDATA_H__A9F59982_0924_483C_984C_5B9D428ECF3A__INCLUDED_)
#define AFX_GRANDSETUPDATA_H__A9F59982_0924_483C_984C_5B9D428ECF3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrandSetupData.h : header file
//

#define SET_BASE					1000
#define	SET_ACQ_MODE12				(SET_BASE)
#define	SET_ACQ_MODE1B				(SET_BASE+1)
#define	SET_1B_ROM_CHECKSUM			(SET_BASE+2)
#define	SET_LLD_A					(SET_BASE+3)
#define	SET_LLD_B					(SET_BASE+4)
#define	SET_ULD_A					(SET_BASE+6)
#define	SET_ULD_B					(SET_BASE+7)
#define	SET_UNIT_ID					(SET_BASE+8)
#define	SET_BOARD1_ID				(SET_BASE+9)
#define	SET_BOARD2_ID				(SET_BASE+10)
#define	SET_USER_ID					(SET_BASE+11)
#define	SET_GAMMA1_MODE				(SET_BASE+12)
#define	SET_GAMMA2_MODE				(SET_BASE+13)
#define	SET_MAX_GAIN0				(SET_BASE+14)
#define	SET_MAX_GAIN1				(SET_BASE+15)
#define	SET_DISCONNECT_MODE			(SET_BASE+16)
#define	SET_GRAND_USERVER			(SET_BASE+17)
#define	SET_VERSION					(SET_BASE+18)
#define	SET_ACQUIRE_TIME			(SET_BASE+19)
#define	SET_BUFFER_TOTAL			(SET_BASE+20)
#define	SET_BUFFER_SAVE				(SET_BASE+21)
#define	SET_NEUTRON_PULSEATHRESH	(SET_BASE+22)
#define	SET_NEUTRON_PULSEBTHRESH	(SET_BASE+23)
#define	SET_NEUTRON_PULSECTHRESH	(SET_BASE+24)
#define	SET_PULSE_CONF				(SET_BASE+25)
#define	SET_GAMMA_1THRESH			(SET_BASE+26)
#define	SET_GAMMA_2THRESH			(SET_BASE+27)
#define	SET_NOFILTERENTERMII_RUNS	(SET_BASE+28)
#define	SET_NOFILTERENTERMII_SIGMA	(SET_BASE+29)
#define	SET_ENTERMII_RUNS			(SET_BASE+30)
#define	SET_ENTERMII_SIGMA			(SET_BASE+31)
#define	SET_MII_EXIT				(SET_BASE+32)
#define	SET_LOCAL_BACKGROUND_FIRST	(SET_BASE+33)
#define	SET_LOCAL_BACKGROUND_LAST	(SET_BASE+34)
#define	SET_ACQ_MODE1C				(SET_BASE+35)
#define	SET_1C_ROM_CHECKSUM			(SET_BASE+36)
#define	SET_FILTER_METHOD			(SET_BASE+37)
#define	SET_TRIGGER_MODE			(SET_BASE+38)
#define	SET_TAKE_OFFSET				(SET_BASE+39)
#define	SET_TAKEOFFSETMAX			(SET_BASE+40)
#define	SET_LOG_BATTERY				(SET_BASE+41)
#define	SET_FILTERING_ENABLED		(SET_BASE+42)
#define	SET_NEUTRON_A_LOWERLIMIT	(SET_BASE+43)
#define	SET_NEUTRON_B_LOWERLIMIT	(SET_BASE+44)
#define	SET_NEUTRON_C_LOWERLIMIT	(SET_BASE+45)
#define	SET_GAMMA_1_LOWERLIMIT		(SET_BASE+46)
#define	SET_GAMMA_2_LOWERLIMIT		(SET_BASE+47)
#define	SET_SYNC_ENABLED			(SET_BASE+48)
#define	SET_SYNC_TIME				(SET_BASE+49)
#define	SET_SYNC_LOWERLIMIT			(SET_BASE+50)
#define	SET_SYNC_UPPERLIMIT			(SET_BASE+51)
#define SET_ANALOG_GAIN				(SET_BASE+52)
#define SET_TOP						(SET_BASE+52)

#define SET4_BASE					2000
#define SET4_GCR_TIME				SET4_BASE					
#define	SET4_COUNT_TIME				(SET4_BASE+1)			
#define	SET4_CONF_STATE				(SET4_BASE+2)
#define	SET4_BIAS_TYPE				(SET4_BASE+3)
#define	SET4_BIAS_SETPOINT			(SET4_BASE+4)
#define	SET4_BAUD_RATE				(SET4_BASE+5)
#define	SET4_FRAME					(SET4_BASE+6)
#define	SET4_MODE_FLAGS1			(SET4_BASE+7)
#define	SET4_MODE_FLAGS2			(SET4_BASE+8)
#define	SET4_BATT_INTER				(SET4_BASE+9)
#define	SET4_FILTER_CONT			(SET4_BASE+10)
#define	SET4_IMMBUFF_SZ				(SET4_BASE+11)
#define	SET4_IMMBUFFSAVE_SZ			(SET4_BASE+12)
#define	SET4_LOCBG_SZ				(SET4_BASE+13)
#define	SET4_LOCBG_END				(SET4_BASE+14)
#define	SET4_STATUS_INT				(SET4_BASE+15)
#define	SET4_INSP_ID				(SET4_BASE+16)
#define	SET4_UNIT_ID				(SET4_BASE+17)
#define	SET4_TIMESYNC_STATE			(SET4_BASE+18)
#define	SET4_TIMESYNC_HOUR			(SET4_BASE+19)
#define	SET4_TIMESYNC_DAILY			(SET4_BASE+20)
#define	SET4_TIMESYNC_LOWTOL		(SET4_BASE+21)
#define	SET4_TIMESYNC_UPPTOL		(SET4_BASE+22)
#define	SET4_FIRMWARE_VERSION		(SET4_BASE+23)
#define	SET4_FIRMWARE_CHKSUM		(SET4_BASE+24)
#define	SET4_ISR_TIME				(SET4_BASE+25)
#define	SET4_BAT_VOLT				(SET4_BASE+26)
#define	SET4_P5READ_VOLT			(SET4_BASE+27)
#define	SET4_P12READ_VOLT			(SET4_BASE+28)
#define	SET4_M12READ_VOLT			(SET4_BASE+29)
#define	SET4_ACPOWER_STAT			(SET4_BASE+30)
#define	SET4_BIASREAD_VOLT			(SET4_BASE+31)
#define	SET4_IONREAD_VOLT			(SET4_BASE+32)
#define	SET4_ONBOARDREAD_TEMP		(SET4_BASE+33)
#define	SET4_OFFBOARDREAD_TEMP		(SET4_BASE+34)
#define	SET4_IIR_TIME				(SET4_BASE+35)
#define	SET4_MPB_ID					(SET4_BASE+36)
#define	SET4_MPBXILINX_VER			(SET4_BASE+37)
#define	SET4_DCMSER_NUM				(SET4_BASE+38)
#define	SET4_DCMBRD_TYPE			(SET4_BASE+39)
#define	SET4_DCMXILINX_VER			(SET4_BASE+40)
#define	SET4_DCMACTEL_VER			(SET4_BASE+41)
#define	SET4_DCMPRESENCE			(SET4_BASE+42)
#define	SET4_TPCSER_NUM				(SET4_BASE+43)
#define	SET4_TPCBRD_TYPE			(SET4_BASE+44)
#define	SET4_TPCXILINX_VER			(SET4_BASE+45)
#define	SET4_TPCPRESENCE			(SET4_BASE+46)
#define	SET4_PSSER_NUM				(SET4_BASE+47)
#define	SET4_PSBRD_VER				(SET4_BASE+48)
#define	SET4_PSBRD_TYPE				(SET4_BASE+49)
#define SET4_PSXILINX_VER			(SET4_BASE+50)
#define SET4_DCMCR_TIME				(SET4_BASE+51)
#define	SET4_OFFSET_MODE			(SET4_BASE+52)
#define	SET4_NOMOS_INTERVAL			(SET4_BASE+53)
#define	SET4_MAXOS_INTERVAL			(SET4_BASE+54)
#define	SET4_G0_GAINMODE			(SET4_BASE+55)
#define	SET4_G0_MAXFIX				(SET4_BASE+56)
#define	SET4_G0_PRES				(SET4_BASE+57)
#define	SET4_G1_GAINMODE			(SET4_BASE+58)
#define	SET4_G1_MAXFIX				(SET4_BASE+59)
#define	SET4_G1_PRES				(SET4_BASE+60)
#define	SET4_ICHV_SETPOINT			(SET4_BASE+61)
#define	SET4_ICHV_CONFIG			(SET4_BASE+62)
#define	SET4_GUNCERT_MULT			(SET4_BASE+63)
#define	SET4_TCR_TIME				(SET4_BASE+64)
#define	SET4_TRIGGER4_LOGIC			(SET4_BASE+65)
#define	SET4_TRIGGER4_CHTYPE		(SET4_BASE+66)
#define	SET4_TRIGGER5_LOGIC			(SET4_BASE+67)
#define	SET4_TRIGGER5_CHTYPE		(SET4_BASE+68)
#define	SET4_TRIGGER6_LOGIC			(SET4_BASE+69)
#define	SET4_TRIGGER6_CHTYPE		(SET4_BASE+70)
#define	SET4_TRIGGER7_LOGIC			(SET4_BASE+71)
#define	SET4_TRIGGER7_CHTYPE		(SET4_BASE+72)
#define	SET4_CH0_TIME				(SET4_BASE+73)
#define	SET4_CH0_USED				(SET4_BASE+74)
#define	SET4_CH0_T1TYPEDIR			(SET4_BASE+75)
#define	SET4_CH0_T1VALUE			(SET4_BASE+76)
#define	SET4_CH0_T1EEV				(SET4_BASE+77)
#define	SET4_CH0_T2TYPEDIR			(SET4_BASE+78)
#define	SET4_CH0_T2VALUE			(SET4_BASE+79)
#define	SET4_CH0_T2EEV				(SET4_BASE+80)
#define	SET4_CH0_CHANGMULT			(SET4_BASE+81)
#define	SET4_CH0_CHANGENTCNT		(SET4_BASE+82)
#define	SET4_CH0_FILTERLIM			(SET4_BASE+83)
#define	SET4_CH0_HYSTER				(SET4_BASE+84)
#define	SET4_CH1_TIME				(SET4_BASE+85)
#define	SET4_CH1_USED				(SET4_BASE+86)
#define	SET4_CH1_T1TYPEDIR			(SET4_BASE+87)
#define	SET4_CH1_T1VALUE			(SET4_BASE+88)
#define	SET4_CH1_T1EEV				(SET4_BASE+89)
#define	SET4_CH1_T2TYPEDIR			(SET4_BASE+90)
#define	SET4_CH1_T2VALUE			(SET4_BASE+91)
#define	SET4_CH1_T2EEV				(SET4_BASE+92)
#define	SET4_CH1_CHANGMULT			(SET4_BASE+93)
#define	SET4_CH1_CHANGENTCNT		(SET4_BASE+94)
#define	SET4_CH1_FILTERLIM			(SET4_BASE+95)
#define	SET4_CH1_HYSTER				(SET4_BASE+96)
#define	SET4_CH2_TIME				(SET4_BASE+97)
#define	SET4_CH2_USED				(SET4_BASE+98)
#define SET4_CH2_T1TYPEDIR			(SET4_BASE+99)
#define SET4_CH2_T1VALUE			(SET4_BASE+100)
#define	SET4_CH2_T1EEV				(SET4_BASE+101)
#define	SET4_CH2_T2TYPEDIR			(SET4_BASE+102)
#define	SET4_CH2_T2VALUE			(SET4_BASE+103)
#define	SET4_CH2_T2EEV				(SET4_BASE+104)
#define	SET4_CH2_CHANGMULT			(SET4_BASE+105)
#define	SET4_CH2_CHANGENTCNT		(SET4_BASE+106)
#define	SET4_CH2_FILTERLIM			(SET4_BASE+107)
#define	SET4_CH2_HYSTER				(SET4_BASE+108)
#define	SET4_CH3_TIME				(SET4_BASE+109)
#define	SET4_CH3_USED				(SET4_BASE+110)
#define	SET4_CH3_T1TYPEDIR			(SET4_BASE+111)
#define	SET4_CH3_T1VALUE			(SET4_BASE+112)
#define	SET4_CH3_T1EEV				(SET4_BASE+113)
#define	SET4_CH3_T2TYPEDIR			(SET4_BASE+114)
#define	SET4_CH3_T2VALUE			(SET4_BASE+115)
#define	SET4_CH3_T2EEV				(SET4_BASE+116)
#define	SET4_CH3_CHANGMULT			(SET4_BASE+117)
#define	SET4_CH3_CHANGENTCNT		(SET4_BASE+118)
#define	SET4_CH3_FILTERLIM			(SET4_BASE+119)
#define	SET4_CH3_HYSTER				(SET4_BASE+120)
#define	SET4_CH4_TIME				(SET4_BASE+121)
#define	SET4_CH4_USED				(SET4_BASE+122)
#define	SET4_CH4_T1TYPEDIR			(SET4_BASE+123)
#define	SET4_CH4_T1VALUE			(SET4_BASE+124)
#define	SET4_CH4_T1EEV				(SET4_BASE+125)
#define	SET4_CH4_T2TYPEDIR			(SET4_BASE+126)
#define	SET4_CH4_T2VALUE			(SET4_BASE+127)
#define	SET4_CH4_T2EEV				(SET4_BASE+128)
#define	SET4_CH4_CHANGMULT			(SET4_BASE+129)
#define	SET4_CH4_CHANGENTCNT		(SET4_BASE+130)
#define	SET4_CH4_FILTERLIM			(SET4_BASE+131)
#define	SET4_CH4_HYSTER				(SET4_BASE+132)
#define	SET4_CH5_TIME				(SET4_BASE+133)
#define	SET4_CH5_USED				(SET4_BASE+134)
#define	SET4_CH5_T1TYPEDIR			(SET4_BASE+135)
#define	SET4_CH5_T1VALUE			(SET4_BASE+136)
#define	SET4_CH5_T1EEV				(SET4_BASE+137)
#define	SET4_CH5_T2TYPEDIR			(SET4_BASE+138)
#define	SET4_CH5_T2VALUE			(SET4_BASE+139)
#define	SET4_CH5_T2EEV				(SET4_BASE+140)
#define	SET4_CH5_CHANGMULT			(SET4_BASE+141)
#define	SET4_CH5_CHANGENTCNT		(SET4_BASE+142)
#define	SET4_CH5_FILTERLIM			(SET4_BASE+143)
#define	SET4_CH5_HYSTER				(SET4_BASE+144)
//#define	SET4_CH6_TIME				(SET4_BASE+145)
//#define	SET4_CH6_USED				(SET4_BASE+146)
//#define	SET4_CH6_T1TYPEDIR			(SET4_BASE+147)
//#define SET4_CH6_T1VALUE			(SET4_BASE+148)
//#define	SET4_CH6_T1EEV				(SET4_BASE+149)
//#define	SET4_CH6_T2TYPEDIR			(SET4_BASE+150)
//#define	SET4_CH6_T2VALUE			(SET4_BASE+151)
//#define	SET4_CH6_T2EEV				(SET4_BASE+152)
//#define	SET4_CH6_CHANGMULT			(SET4_BASE+153)
//#define	SET4_CH6_CHANGENTCNT		(SET4_BASE+154)
//#define	SET4_CH6_FILTERLIM			(SET4_BASE+155)
//#define	SET4_CH6_HYSTER				(SET4_BASE+156)
#define SET4_TOP					SET4_CH5_HYSTER


/////////////////////////////////////////////////////////////////////////////
// CGrandSetupData window

class CGrandSetupData : public CStatic
{
// Construction
public:
	CGrandSetupData();

// Attributes
public:
	
// Operations
public:
	void ReSetItterator(){m_uItterator = 0;};
	void ReSetNewItterator(){m_uNewItterator = 0;};
	void SetItteratorTo(UINT i)
	{
		m_uItterator = i;
		if (m_uItterator > SET_TOP)
			m_uItterator = 0;
	};
	void SetNewItteratorTo(UINT i)
	{
		m_uNewItterator = i;
		if (m_uNewItterator > SET4_TOP)
			m_uNewItterator = 0;
	};

	void SetItteratorLineTo(UINT i)
	{
		m_uItterator = i*4;
		if (m_uItterator > (SET_TOP-SET_BASE)*4)
			m_uItterator = 0;
	};
	void SetNewItteratorLineTo(UINT i)
	{
		m_uNewItterator = i*4;
		if (m_uNewItterator > (SET4_TOP-SET_BASE)*4)
			m_uNewItterator = 0;
	};


	char *GetNext();
	char *GetNewNext();
	LPTSTR *lpszRow;
	LPTSTR *lpszNewRow;
	UINT m_uItterator;
	UINT m_uNewItterator;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrandSetupData)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGrandSetupData();

	// Generated message map functions
protected:
	int m_iRowCount;
	int m_iNewRowCount;
	//{{AFX_MSG(CGrandSetupData)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRANDSETUPDATA_H__A9F59982_0924_483C_984C_5B9D428ECF3A__INCLUDED_)
