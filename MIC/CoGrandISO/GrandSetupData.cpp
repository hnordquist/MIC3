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
// GrandSetupData.cpp

#include "stdafx.h"
#include "GrandSetupData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrandSetupData

CGrandSetupData::CGrandSetupData()
{
	m_uItterator = 0;
	m_uNewItterator = 0;
	static LPTSTR szRow[] =	{
		"-Acquisition Mode (12)","SET_ACQ_MODE12","1F","",
		"-Acquisition Mode (1b)","SET_ACQ_MODE1B","2","",
		"-ROM Checksum (1b)","SET_1B_ROM_CHECKSUM","0000","Read Only",
		"-GRAND3 Low Level Disc. A","SET_LLD_A","  0","",
		"-GRAND3 Low Level Disc. B","SET_LLD_B","  0","",
		"-GRAND3 Upper Level Disc. A","SET_ULD_A","  0","",
		"-GRAND3 Upper Level Disc. B","SET_ULD_B","  0","",
		"-Unit Identifier","SET_UNIT_ID","11","Read Only",
		"-Board 1 Identifier","SET_BOARD1_ID","07","Read Only",
		"-Board 2 Identifier","SET_BOARD2_ID","06","Read Only",
		"-User Identifier","SET_USER_ID","7777777","b8 [xxxxxxx]",
		"-Gamma 1 Mode","SET_GAMMA1_MODE","A","83 [m [g]] set/display gain mode (0=auto/1=manual) and max value",
		"-Gamma 2 Mode","SET_GAMMA2_MODE","A","83 [m [g]] set/display gain mode (0=auto/1=manual) and max value",
		"-Max Gain ch 0","SET_MAX_GAIN0","4194304","83 [m [g]] set/display gain mode (0=auto/1=manual) and max value",
		"-Max Gain ch 1","SET_MAX_GAIN1","4194304","83 [m [g]] set/display gain mode (0=auto/1=manual) and max value",
		"-Disconnect Mode","SET_DISCONNECT_MODE","I","81 [m] set/display offset mode (0=internal/1=remote",
		"-Program Version Number (1c)","SET_GRAND_USERVER","03.76","Read Only",
		"-Program Version Number (1b)","SET_VERSION","03.76","Read Only",
		"-Acquire Time","SET_ACQUIRE_TIME","    1","01 ttttt set count time (1-65535 sec)",
		"-Buffer Total","SET_BUFFER_TOTAL","70","b6[xx [yy]] when not filtering save first xx of last yy points",
		"-Buffer Save","SET_BUFFER_SAVE","50","b6[xx [yy]] when not filtering save first xx of last yy points",
		"-Neutron Pulse A Threshold","SET_NEUTRON_PULSEATHRESH","   100.0","",
		"-Neutron Pulse B Threshold","SET_NEUTRON_PULSEBTHRESH","   100.0","",
		"-Neutron Pulse C Threshold","SET_NEUTRON_PULSECTHRESH","   100.0","",
		"-Set Pulse Configuration","SET_PULSE_CONF","EXTAEXTBEXTC","",
		"-Gamma 1 Threshold","SET_GAMMA_1THRESH","     100.0","",
		"-Gamma 2 Threshold","SET_GAMMA_2THRESH","     100.0","",
		"-No Filter Enter MII Runs","SET_NOFILTERENTERMII_RUNS"," 3","b2 [xx[ y.yy]] do not filter if xx runs are y.yy sigma > local bkgd",
		"-No Filter Enter MII Sigma","SET_NOFILTERENTERMII_SIGMA","3.0","b2 [xx[ y.yy]] do not filter if xx runs are y.yy sigma > local bkgd",
		"-Enter MII Runs","SET_ENTERMII_RUNS"," 3","b4 [xx[ y.yy]] enter mii if xx runs > y.yy*thresh or exceed sigma test",
		"-Enter MII Sigma","SET_ENTERMII_SIGMA","1.5","b4 [xx[ y.yy]] enter mii if xx runs > y.yy*thresh or exceed sigma test",
		"-MII Exit","SET_MII_EXIT","10","",
		"-Local Background First","SET_LOCAL_BACKGROUND_FIRST","25","b7 [xx[ yy]] for local bkgd use first xx of last yy runs",
		"-Local Background Last","SET_LOCAL_BACKGROUND_LAST","30","b7 [xx[ yy]] for local bkgd use first xx of last yy runs",
		"-Acquisition Mode (1c)","SET_ACQ_MODE1C","1f","",
		"-Rom Checksum (1c)","SET_1C_ROM_CHECKSUM","0000","Read Only",
		"-Filter Method","SET_FILTER_METHOD","0","",
		"-Trigger Mode","SET_TRIGGER_MODE","1","",
		"-Take Offset","SET_TAKE_OFFSET"," 24","82 [t] set/display offset interval",
		"-Take Offset Max","SET_TAKEOFFSETMAX"," 48","83 [m[ g]] set/display gain mode (0=auto/1=manual) and max value",
		"-Log Battery","SET_LOG_BATTERY"," 1","b0 [ddddd] set battery check interval (minutes)",
		"-Filtering Enabled","SET_FILTERING_ENABLED","0","b1 [x[ s]] set filtering on/off (1/0), save avg/first points (1/0)",
		"-Neutron A Lower Limit","SET_NEUTRON_A_LOWERLIMIT","0.200","",
		"-Neutron B Lower Limit","SET_NEUTRON_B_LOWERLIMIT","0.200","",
		"-Neutron C Lower Limit","SET_NEUTRON_C_LOWERLIMIT","0.200","",
		"-Gamma 1 Lower Limit","SET_GAMMA_1_LOWERLIMIT","  0.020","",
		"-Gamma 2 Lower Limit","SET_GAMMA_2_LOWERLIMIT","  0.020","",
		"-Sync Enabled","SET_SYNC_ENABLED","1","c5 [h[ d]] do sync state: hourly and daily, 1 = enable",
		"-Sync Time","SET_SYNC_TIME","04:30:30","c2 [mmss] and c3 [hhmmss]",
		"-Sync Lower Limit","SET_SYNC_LOWERLIMIT"," 2","c4 [l[ u]] do sync tolerance-lower(s) upper(m)",
		"-Sync Upper Limit","SET_SYNC_UPPERLIMIT","5","c4 [l[ u]] do sync tolerance-lower(s) upper(m)",
		"-Set Analog Gain","SET_ANALOG_GAIN","","",
		NULL
	};
	lpszRow = szRow;

	static LPTSTR szNewRow[] =	{
		"General Config Record (41)","SET4_GCR_TIME","","Read Only",
		"-Count Time","SET4_COUNT_TIME","","",
		"-Configured State","SET4_CONF_STATE","","",
		"-Bias Type","SET4_BIAS_TYPE","","",
		"-Bias Set Point","SET4_BIAS_SETPOINT","","",
		"-Baud Rate","SET4_BAUD_RATE","","",
		"-Frame","SET4_FRAME","","",
		"-Mode Flags 1","SET4_MODE_FLAGS1","","",
		"-Mode Flags 2","SET4_MODE_FLAGS2","","",
		"-Battery Rec. Int.","SET4_BATT_INTER","","",
		"-Filter Control","SET4_FILTER_CONT","","",
		"-Imm. Buff. Size","SET4_IMMBUFF_SZ","","",
		"-Imm. Buff. Save Size","SET4_IMMBUFFSAVE_SZ","","",
		"-Local Background Size","SET4_LOCBG_SZ","","",
		"-Local Background End","SET4_LOCBG_END","","",
		"-Status Rec Intvl.","SET4_STATUS_INT","","",
		"-Insp. ID","SET4_INSP_ID","","",
		"-Unit ID","SET4_UNIT_ID","","",
		"-Time Sync State","SET4_TIMESYNC_STATE","","",
		"-Hourly Sync Time","SET4_TIMESYNC_HOUR","","",
		"-Daily Sync Time","SET4_TIMESYNC_DAILY","","",
		"-Sync. Low Tol.","SET4_TIMESYNC_LOWTOL","","",
		"-Sync. Up Tol.","SET4_TIMESYNC_UPPTOL","","",
		"-Firmware Vers.","SET4_FIRMWARE_VERSION","","Read Only",
		"-Firmware Cksm.","SET4_FIRMWARE_CHKSUM","","Read Only",

		"Instrument Status Record (42)","SET4_ISR_TIME","","Read Only",
		"-Battery Voltage Reading","SET4_BAT_VOLT","","Read Only",
		"-+5 Volt Reading","SET4_P5READ_VOLT","","Read Only",
		"-+12 Volt Reading","SET4_P12READ_VOLT","","Read Only",
		"--12 Volt Reading","SET4_M12READ_VOLT","","Read Only",
		"-AC Power Status","SET4_ACPOWER_STAT","","Read Only",
		"-Bias Voltage Reading","SET4_BIASREAD_VOLT","","Read Only",
		"-ION Chamber V. Reading","SET4_IONREAD_VOLT","","Read Only",
		"-On Board Temp","SET4_ONBOARDREAD_TEMP","","Read Only",
		"-Off Board Temp","SET4_OFFBOARDREAD_TEMP","","Read Only",

		"Instrument Info Record (43)","SET4_IIR_TIME","","Read Only",
		"-MPB ID","SET4_MPB_ID","","Read Only",
		"-MPB Xilinx Ver","SET4_MPBXILINX_VER","","Read Only",
		"-DCM Ser. Num","SET4_DCMSER_NUM","","Read Only",
		"-DCM Board Type","SET4_DCMBRD_TYPE","","Read Only",
		"-DCM Xilinx Ver","SET4_DCMXILINX_VER","","Read Only",
		"-DCM Actel Ver","SET4_DCMACTEL_VER","","Read Only",
		"-DCM Presence","SET4_DCMPRESENCE","","Read Only",
		"-TPC Ser. Num","SET4_TPCSER_NUM","","Read Only",
		"-TPC Board Type","SET4_TPCBRD_TYPE","","Read Only",
		"-TPC Xilinx Ver","SET4_TPCXILINX_VER","","Read Only",
		"-TPC Presence","SET4_TPCPRESENCE","","Read Only",
		"-PS Serial Num","SET4_PSSER_NUM","","Read Only",
		"-PS Board Ver","SET4_PSBRD_VER","","Read Only",
		"-PS Board Type","SET4_PSBRD_TYPE","","Read Only",
		"-PS Xilinx Ver","SET4_PSXILINX_VER","","Read Only",

		"Dual Current Mode Config Record (44)","SET4_DCMCR_TIME","","Read Only",
		"-Offset Mode","SET4_OFFSET_MODE","","",
		"-Nominal Offset Interval","SET4_NOMOS_INTERVAL","","",
		"-Maximum Offset Interval","SET4_MAXOS_INTERVAL","","",
		"-Gamma Ch 0 Gain Mode","SET4_G0_GAINMODE","","",
		"-Gamma Ch 0 Max or Fixed Gain Index","SET4_G0_MAXFIX","","",
		"-Gamma Ch 0 Present Gain Index","SET4_G0_PRES","","",
		"-Gamma Ch 1 Gain Mode","SET4_G1_GAINMODE","","",
		"-Gamma Ch 1 Max or Fixed Gain Index","SET4_G1_MAXFIX","","",
		"-Gamma Ch 1 Present Gain Index","SET4_G1_PRES","","",
		"-Ion Chamber HV Set Point","SET4_ICHV_SETPOINT","","",
		"-Ion Chamber HV Config State","SET4_ICHV_CONFIG","","",
		"-Gamma Uncertainty multiplier","SET4_GUNCERT_MULT","","",

		"Trigger Configuration Record (45)","SET4_TCR_TIME","","Read Only",
		"-Trigger Pin 4 Logic","SET4_TRIGGER4_LOGIC","","",
		"-Trigger Pin 4 Ch/Type","SET4_TRIGGER4_CHTYPE","","",
		"-Trigger Pin 5 Logic","SET4_TRIGGER5_LOGIC","","",
		"-Trigger Pin 5 Ch/Type","SET4_TRIGGER5_CHTYPE","","",
		"-Trigger Pin 6 Logic","SET4_TRIGGER6_LOGIC","","",
		"-Trigger Pin 6 Ch/Type","SET4_TRIGGER6_CHTYPE","","",
		"-Trigger Pin 7 Logic","SET4_TRIGGER7_LOGIC","","",
		"-Trigger Pin 7 Ch/Type","SET4_TRIGGER7_CHTYPE","","",

		"Channel 0 Configuration Record (46)","SET4_CH0_TIME","","Read Only",
		"-Used for Analysis","SET4_CH0_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH0_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH0_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH0_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH0_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH0_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH0_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH0_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH0_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH0_FILTERLIM","","",
		"-Hysteresis","SET4_CH0_HYSTER","","",

		"Channel 1 Configuration Record (47)","SET4_CH1_TIME","","Read Only",
		"-Used for Analysis","SET4_CH1_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH1_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH1_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH1_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH1_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH1_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH1_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH1_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH1_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH1_FILTERLIM","","",
		"-Hysteresis","SET4_CH1_HYSTER","","",

		"Channel 2 Configuration Record (48)","SET4_CH2_TIME","","Read Only",
		"-Used for Analysis","SET4_CH2_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH2_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH2_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH2_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH2_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH2_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH2_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH2_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH2_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH2_FILTERLIM","","",
		"-Hysteresis","SET4_CH2_HYSTER","","",

		"Channel 3 Configuration Record (49)","SET4_CH3_TIME","","Read Only",
		"-Used for Analysis","SET4_CH3_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH3_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH3_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH3_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH3_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH3_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH3_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH3_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH3_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH3_FILTERLIM","","",
		"-Hysteresis","SET4_CH3_HYSTER","","",

		"Channel 4 Configuration Record (4a)","SET4_CH4_TIME","","Read Only",
		"-Used for Analysis","SET4_CH4_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH4_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH4_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH4_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH4_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH4_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH4_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH4_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH4_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH4_FILTERLIM","","",
		"-Hysteresis","SET4_CH4_HYSTER","","",

		"Channel 5 Configuration Record (4b)","SET4_CH5_TIME","","Read Only",
		"-Used for Analysis","SET4_CH5_USED","","",
		"-Threshold 1 Type and Direction","SET4_CH5_T1TYPEDIR","","",
		"-Threshold 1 value","SET4_CH5_T1VALUE","","",
		"-Threshold 1 entry\\exit values","SET4_CH5_T1EEV","","",
		"-Threshold 2 Type and Direction","SET4_CH5_T2TYPEDIR","","",
		"-Threshold 2 value","SET4_CH5_T2VALUE","","",
		"-Threshold 2 entry\\exit values","SET4_CH5_T2EEV","","",
		"-Changing Sig Multiplier","SET4_CH5_CHANGMULT","","",
		"-Changing Sig Entry Count","SET4_CH5_CHANGENTCNT","","",
		"-Filter Limit","SET4_CH5_FILTERLIM","","",
		"-Hysteresis","SET4_CH5_HYSTER","","",

//		"Channel 6 Configuration Record (4c)","SET4_CH6_TIME","","Read Only",
//		"-Used for Analysis","SET4_CH6_USED","","",
//		"-Threshold 1 Type and Direction","SET4_CH6_T1TYPEDIR","","",
//		"-Threshold 1 value","SET4_CH6_T1VALUE","","",
//		"-Threshold 1 entry\exit values","SET4_CH6_T1EEV","","",
//		"-Threshold 2 Type and Direction","SET4_CH6_T2TYPEDIR","","",
//		"-Threshold 2 value","SET4_CH6_T2VALUE","","",
//		"-Threshold 2 entry\exit values","SET4_CH6_T2EEV","","",
//		"-Changing Sig Multiplier","SET4_CH6_CHANGMULT","","",
//		"-Changing Sig Entry Count","SET4_CH6_CHANGENTCNT","","",
//		"-Filter Limit","SET4_CH6_FILTERLIM","","",
//		"-Hysteresis","SET4_CH6_HYSTER","","",

		NULL
	};
	lpszNewRow = szNewRow;
}

CGrandSetupData::~CGrandSetupData()
{
}


char *
CGrandSetupData::GetNext()
{
	if (lpszRow[m_uItterator])
		return lpszRow[m_uItterator++];
	else
		return NULL;
}

char *
CGrandSetupData::GetNewNext()
{
	if (lpszNewRow[m_uNewItterator])
		return lpszNewRow[m_uNewItterator++];
	else
		return NULL;
}

BEGIN_MESSAGE_MAP(CGrandSetupData, CStatic)
	//{{AFX_MSG_MAP(CGrandSetupData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrandSetupData message handlers
