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
//DSPEC50Strings.h

#ifndef DSPEC50STRINGS_H
#define DSPEC50STRINGS_H

/////////////////////////////////////////////////////////////////////////////
// Global strings for the DSPEC50 instrument - general use


#define ACTIVE				"ACTIVE"
#define ANTICOINCIDENT		"ANTICOINCIDENT"
#define COINCIDENT			"COINCIDENT"
#define DATE				"DATE"
#define DATESTART			"DATESTART"
#define DEVICE				"DEVICE"
#define DISABLE				"DISABLE"
#define DSPEC50_DEFAULT		"DSPEC50 Default"
#define ENABLE				"ENABLE"
#define ERROR_STRING		"ERROR"
#define GAUSS				"GAUSS"
#define HV_FAIL				"HV_FAIL"
#define HVVOLTH				"HVVOLTH"
#define HVVOLTL				"HVVOLTL"
#define INACTIVE			"INACTIVE"
#define LIVE				"LIVE"
#define LIVEPRESET			"LIVEPRESET"
#define LONG_STRING			"LONG"
#define LONGDWELL_SEC		"LONGDWELL_SEC"
#define MAXPAUSE			"MAXPAUSE"
#define MAXSTATUS			"MAXSTATUS"
#define NEGATIVE			"NEGATIVE"
#define NETWORKADDRESS		"NETWORKADDRESS"
#define NETWORKID			"NETWORKID"
#define NODE				"NODE"
#define NORMAL				"NORMAL"
#define OFF					"OFF"
#define ORTEC				"ORTEC"
#define OVERLOAD			"OVERLOAD"
#define POINT				"POINT"
#define POSITIVE			"POSITIVE"
#define QUIET				"QUIET"
#define REALTIMEDWELL_SEC	"REALTIMEDWELL_SEC"
#define RESISTIVE_FEEDBACK	"RESISTIVE_FEEDBACK"
#define SHORT_STRING		"SHORT"
#define SHORTDWELL_SEC		"SHORTDWELL_SEC"
#define SHOW_SPECTRA		"SHOW_SPECTRA"
#define SHUTACTUAL			"SHUTACTUAL"
#define SLOW_RESET			"SLOW_RESET"
#define TRANSISTOR_RESET	"TRANSISTOR_RESET"
#define THRESHOLD			"THRESHOLD"
#define TIME				"TIME"
#define TIMEOUT				"TIMEOUT"
#define TIMESTART			"TIMESTART"
#define TRUE_STRING			"TRUE"
#define TTL					"TTL"
#define VERSION				"VERSION"
/////////////////////////////////////////////////////////////////////////////
// Global strings for the DSPEC50 commands
//

#define CLEAR						"CLEAR"
#define CLEAR_COUNTERS				"CLEAR_COUNTERS"
#define DISABLE_BLRE_AUTOMATIC		"DISABLE_BLRE_AUTOMATIC"
#define DISABLE_GAIN_STABILIZATION	"DISABLE_GAIN_STABILIZATION"
#define DISABLE_HV					"DISABLE_HV"
#define DISABLE_PZ_AUTOMATIC		"DISABLE_PZ_AUTOMATIC"
#define DISABLE_ZDT					"DISABLE_ZDT"
#define DISABLE_ZERO_STABILIZATION	"DISABLE_ZERO_STABILIZATION"
#define ENABLE_BLRE_AUTOMATIC		"ENABLE_BLRE_AUTOMATIC"
#define ENABLE_GAIN_STABILIZATION	"ENABLE_GAIN_STABILIZATION"
#define ENABLE_HV					"ENABLE_HV"
#define ENABLE_PZ_AUTOMATIC			"ENABLE_PZ_AUTOMATIC"
#define ENABLE_ZDT					"ENABLE_ZDT"
#define ENABLE_ZERO_STABILIZATION	"ENABLE_ZERO_STABILIZATION"

#define LIST_CORRECTION_FLAT		"LIST_CORRECTION_FLAT"
#define LIST_GAIN_COARSE			"LIST_GAIN_COARSE"
#define LIST_GAIN_CONV				"LIST_GAIN_CONV"
#define LIST_GAIN_FINE				"LIST_GAIN_FINE"
#define LIST_HV						"LIST_HV"
#define LIST_SHAP_CUSP				"LIST_SHAP_CUSP"
#define LIST_SHAP_FLAT				"LIST_SHAP_FLAT"
#define LIST_SHAP_RISE				"LIST_SHAP_RISE"
#define LIST_SHUTDOWN				"LIST_SHUTDOWN"

#define SET_BLRE_AUTOMATIC_ENABLE	"SET_BLRE_AUTOMATIC_ENABLE"
#define SET_CORRECTION_FLAT			"SET_CORRECTION_FLAT"
#define SET_GAIN_ADJUSTMENT			"SET_GAIN_ADJUSTMENT"
#define SET_GAIN_CHANNEL			"SET_GAIN_CHANNEL"
#define SET_GAIN_COARSE				"SET_GAIN_COARSE"
#define SET_GAIN_CONVERSION			"SET_GAIN_CONVERSION"
#define SET_GAIN_FINE				"SET_GAIN_FINE"
#define SET_GAIN_NEGATIVE			"SET_GAIN_NEGATIVE"
#define SET_GAIN_POSITIVE			"SET_GAIN_POSITIVE"
#define SET_GAIN_POLARITY			"SET_GAIN_POLARITY"
#define SET_GAIN_PRESET				"SET_GAIN_PRESET"
#define SET_GAIN_STABILIZATION_ENABLE "SET_GAIN_STABILIZATION_ENABLE"
#define SET_GAIN_WIDTH				"SET_GAIN_WIDTH"
#define SET_GATE					"SET_GATE"
#define SET_GATE_ANTICOINCIDENT		"SET_GATE_ANTICOINCIDENT"
#define SET_GATE_COINCIDENT			"SET_GATE_COINCIDENT"
#define SET_GATE_OFF				"SET_GATE_OFF"
#define SET_HV						"SET_HV"
#define SET_HV_ENABLE				"SET_HV_ENABLE"
#define SET_HV_NEGATIVE				"SET_HV_NEGATIVE"
#define SET_HV_POLARITY				"SET_HV_POLARITY"
#define SET_HV_POSITIVE				"SET_HV_POSITIVE"
#define SET_LIVE					"SET_LIVE"
#define SET_LIVE_PRESET				"SET_LIVE_PRESET"
#define SET_LLD						"SET_LLD"
#define SET_MODE_POINT				"SET_MODE_POINT"
#define SET_MODE_STABILIZATION		"SET_MODE_STABILIZATION"
#define SET_MODE_GAUSS				"SET_MODE_GAUSS"
#define SET_PZ						"SET_PZ"
#define SET_PZ_AUTOMATIC_ENABLE		"SET_PZ_AUTOMATIC_ENABLE"
#define SET_SHAP_CUSP				"SET_SHAP_CUSP"
#define SET_SHAP_FLAT				"SET_SHAP_FLAT"
#define SET_SHAP_RISE				"SET_SHAP_RISE"
#define SET_SHUTDOWN				"SET_SHUTDOWN"
#define SET_SHUTDOWN_ORTEC			"SET_SHUTDOWN_ORTEC"
#define SET_SHUTDOWN_TTL			"SET_SHUTDOWN_TTL"
#define SET_ULD						"SET_ULD"
#define SET_ZDT_ENABLE				"SET_ZDT_ENABLE"
#define SET_ZERO_ADJUSTMENT			"SET_ZERO_ADJUSTMENT"
#define SET_ZERO_CHANNEL			"SET_ZERO_CHANNEL"
#define SET_ZERO_PRESET				"SET_ZERO_PRESET"
#define SET_ZERO_WIDTH				"SET_ZERO_WIDTH"

#define SHOW_ACTIVE					"SHOW_ACTIVE"
#define SHOW_BLRE_AUTOMATIC			"SHOW_BLRE_AUTOMATIC"
#define SHOW_CORRECTION_FLAT		"SHOW_CORRECTION_FLAT"
#define SHOW_DATE					"SHOW_DATE"
#define SHOW_DATE_START				"SHOW_DATE_START"
#define SHOW_DEVICE					"SHOW_DEVICE"
#define SHOW_GAIN_ADJUSTMENT		"SHOW_GAIN_ADJUSTMENT"
#define SHOW_GAIN_CHANNEL			"SHOW_GAIN_CHANNEL"
#define SHOW_GAIN_COARSE			"SHOW_GAIN_COARSE"
#define SHOW_GAIN_CONVERSION		"SHOW_GAIN_CONVERSION"
#define SHOW_GAIN_FINE				"SHOW_GAIN_FINE"
#define SHOW_GAIN_POLARITY			"SHOW_GAIN_POLARITY"
#define SHOW_GAIN_PRESET			"SHOW_GAIN_PRESET"
#define SHOW_GAIN_STABILIZATION		"SHOW_GAIN_STABILIZATION"
#define SHOW_GAIN_WIDTH				"SHOW_GAIN_WIDTH"
#define SHOW_GATE					"SHOW_GATE"
#define SHOW_HV						"SHOW_HV"
#define SHOW_HV_ACTUAL				"SHOW_HV_ACTUAL"
#define SHOW_HV_POLARITY			"SHOW_HV_POLARITY"
#define SHOW_LIVE					"SHOW_LIVE"
#define SHOW_LIVE_PRESET			"SHOW_LIVE_PRESET"
#define SHOW_LLD					"SHOW_LLD"
#define SHOW_MODE_STABILIZATION		"SHOW_MODE_STABILIZATION"
#define SHOW_NETWORK_ADDRESS		"SHOW_NETWORK_ADDRESS"
#define SHOW_NETWORK_ID				"SHOW_NETWORK_ID"
#define SHOW_PZ						"SHOW_PZ"
#define SHOW_PZ_AUTOMATIC			"SHOW_PZ_AUTOMATIC"
#define SHOW_SHAP_CUSP				"SHOW_SHAP_CUSP"
#define SHOW_SHAP_FLAT				"SHOW_SHAP_FLAT"
#define SHOW_SHAP_RISE				"SHOW_SHAP_RISE"
#define SHOW_SHUT_ACTUAL			"SHOW_SHUT_ACTUAL"
#define SHOW_SHUTDOWN				"SHOW_SHUTDOWN"
#define SHOW_TIME					"SHOW_TIME"
#define SHOW_TIME_START				"SHOW_TIME_START"
#define SHOW_TRUE					"SHOW_TRUE"
#define SHOW_ULD					"SHOW_ULD"
#define SHOW_VERSION				"SHOW_VERSION"
#define SHOW_ZDT					"SHOW_ZDT"
#define SHOW_ZERO_ADJUSTMENT		"SHOW_ZERO_ADJUSTMENT"
#define SHOW_ZERO_CHANNEL			"SHOW_ZERO_CHANNEL"
#define SHOW_ZERO_STABILIZATION		"SHOW_ZERO_STABILIZATION"
#define SHOW_ZERO_WIDTH				"SHOW_ZERO_WIDTH"

#define START						"START"
#define STOP						"STOP"

/////////////////////////////////////////////////////////////////////////////
// Global strings for the DSPEC50 responses
//

#define DOLLARF0TTL		"$F0TTL"
#define DOLLARF1ORT		"$F1ORT"
#define DOLLARFANT		"$FANT"
#define DOLLARFCOI		"$FCOI"
#define DOLLARFOFF		"$FOFF"
#define DOLLARFGAUSS	"$FGAUSS"
#define DOLLARFNEG		"$FNEG"
#define DOLLARFPOINT	"$FPOINT"
#define DOLLARFPOS		"$FPOS"
#define DOLLARIF		"$IF"
#define DOLLARIT		"$IT"
#define GAIN_COAR		"GAIN_COAR"
#define GAIN_CONV		"GAIN_CONV"
#define GAIN_FINE		"GAIN_FINE"
#define HV				"HV"
#define PZ_DIS			"PZ_DIS"
#define PZ_ENA			"PZ_ENA"
#define SHAP_CUSP		"SHAP_CUSP"
#define SHAP_FLAT		"SHAP_FLAT"
#define SHAP_RISE		"SHAP_RISE"

/////////////////////////////////////////////////////////////////////////////
// Global strings for the DSPEC50 default values
//

//This should be the ONLY place these defaults are defined.
//All code that needs defaults should get them from here.
//One source - no confusion.

#define COMMFAIL_DEFAULT				"5"		//tries
#define DO_DUMP_DEFAULT					"0"		//false
#define FILEID_DEFAULT					"01"
#define FLAGCOMM_DEFAULT				"0"		//false
#define FLAGOTHR_DEFAULT				"0"		//false
#define FLAGTIME_DEFAULT				"0"		//false
#define HV_FAIL_DEFAULT					"10"		//volts
#define HVVOLTH_DEFAULT					"5000"
#define HVVOLTL_DEFAULT					"0"
#define LONGDWELL_DEFAULT				"300"	//seconds
#define MAXPAUSE_DEFAULT				"600"	//seconds
#define MAXSTATUS_DEFAULT				"0"		//minutes
#define QUIET_DEFAULT					"0"		//not quiet
#define REALTIMEDWELL_DEFAULT			"1"		//seconds
#define SAVELOC_DEFAULT					"C:\\DATA\\DSPEC5001"
#define SHORTDWELL_DEFAULT				"120"	//seconds
#define SHOW_SPECTRA_DEFAULT			"1"      //TRUE
#define SLOW_RESET_DEFAULT				"900"    //seconds (15 minutes)
#define THRESHOLD_DEFAULT				"200"	//counts
#define TIMEOUT_DEFAULT					"30"		//seconds

#define SET_BLRE_AUTOMATIC_ENABLE_DEFAULT		"ENABLE"
#define SET_CORRECTION_FLAT_DEFAULT				"-0.03906"
#define SET_GAIN_ADJUSTMENT_DEFAULT				"777"	//-32767 to 32767, undimensioned units
#define SET_GAIN_CHANNEL_DEFAULT				"1234"
#define SET_GAIN_COARSE_DEFAULT					"50"
#define SET_GAIN_CONVERSION_DEFAULT				"8192"
#define SET_GAIN_FINE_DEFAULT					"0.3450"  //MIC_CB_028 to MIC 1.907 PJM 1/19/05
#define SET_GAIN_POLARITY_DEFAULT				"NEGATIVE"
#define SET_GAIN_STABILIZATION_ENABLE_DEFAULT	"ENABLE"
#define SET_GAIN_WIDTH_DEFAULT					"42"
#define SET_GATE_DEFAULT						"OFF"
#define SET_HV_DEFAULT							"3000"
#define SET_HV_ENABLE_DEFAULT					"ENABLE"
#define SET_HV_POLARITY_DEFAULT					"NEGATIVE"
#define SET_LLD_DEFAULT							"50"
#define SET_MODE_STABILIZATION_DEFAULT			"POINT"
#define SET_PZ_DEFAULT							"2183"
#define SET_PZ_AUTOMATIC_ENABLE_DEFAULT			"ENABLE"
#define SET_SHAP_CUSP_DEFAULT					"1.0"
#define SET_SHAP_FLAT_DEFAULT					"1.0"
#define SET_SHAP_RISE_DEFAULT					"4.0"
#define SET_SHUTDOWN_DEFAULT					"ORTEC"
#define SET_ULD_DEFAULT							"8191"
#define SET_ZDT_ENABLE_DEFAULT					"DISABLE"

#endif // DSPEC50STRINGS_H
