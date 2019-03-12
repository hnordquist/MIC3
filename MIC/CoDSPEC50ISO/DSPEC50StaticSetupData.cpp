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
// DSPEC50StaticSetupData.cpp

// pjm 8-Mar-2006: put in fix for DR 265: GAIN FINE setting cannot be modified.

#include "stdafx.h"
#include "resource.h"
#include "DSPEC50StaticSetupData.h"
#include "DSPEC50Strings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50StaticSetupData

CDSPEC50StaticSetupData::CDSPEC50StaticSetupData()
{
//	TRACE("CDSPEC50StaticSetupData::CTR()\n");

	m_uItterator = 0;
	static LPTSTR szRow[] =	{

	//Description								INI name						??	DSPEC50 Commands																		Range of values & decimal places

	"Stabilizer: Gain Peak Center Channel",		SET_GAIN_CHANNEL,				"",	"SHOW_GAIN_CHANNEL, SET_GAIN_CHANNEL",												"0 16384 0 16384 -1",
	"Stabilizer: Gain Peak Width (channels)",	SET_GAIN_WIDTH,				"",	"SHOW_GAIN_WIDTH, SET_GAIN_WIDTH chans",											"3 65536 3 65536 -1",
	"Stabilizer: Enable/Disable",				SET_GAIN_STABILIZATION_ENABLE,"",	"SHOW_GAIN_STABILIZATION, ENABLE_GAIN_STABILIZATION, DISABLE_GAIN_STABILIZATION",	"ENABLE,DISABLE",
	"Stabilizer: Mode",							SET_MODE_STABILIZATION,		"", "SHOW_MODE_STABILIZATION, SET_MODE_POINT, SET_MODE_GAUSS",							"POINT,GAUSS",
	"Gain: Coarse",								SET_GAIN_COARSE,				"",	"SHOW_GAIN_COARSE, SET_GAIN_COARSE nnn",											"1,2,5,10,20,50,100",
	//"Gain: Fine",								SET_GAIN_FINE,				"",	"SHOW_GAIN_FINE, SET_FINE_GAIN n.nnnn",												"0.333 1.0 65536 196607 3",
	"Gain: Fine",								SET_GAIN_FINE,				"",	"SHOW_GAIN_FINE, SET_FINE_GAIN n.nnnn",												"0.333 1.0 65536 196607 4", //Fix for DR 265
	"Gain: Conversion (number of chans)",		SET_GAIN_CONVERSION,			"",	"SHOW_GAIN_CONVERSION, SET_GAIN_CONVERSION nnnnn",									"512,1024,2048,4096,8192,16384",
	"Gain: Polarity",							SET_GAIN_POLARITY,			"",	"SHOW_GAIN_POLARITY, SET_GAIN_POSITIVE, SET_GAIN_NEGATIVE",							"POSITIVE,NEGATIVE",
	"HV: Voltage",								SET_HV,						"",	"SHOW_HV, SET_HV nnnn",																"0 5000 0 5000 -1",
	"HV: Enable",								SET_HV_ENABLE,				"",	"SHOW_HV, ENABLE_HV, DISABLE_HV",													"ENABLE,DISABLE",
	"HV: Polarity",								SET_HV_POLARITY,				"",	"SHOW_HV, SET_HV_POSITIVE, SET_HV_NEGATIVE",										"POSITIVE,NEGATIVE",
	"Shape: Cusp Factor",						SET_SHAP_CUSP,				"",	"SHOW_SHAP_CUSP, SET_SHAP_CUSP n.n",												"0.4 1.0 4 10 1",
	"Shape: Flattop Width",						SET_SHAP_FLAT,				"",	"SHOW_SHAP_FLAT, SET_SHAP_FLAT n.n",												"0.3 2.4 3 24 1",
	"Shape: Flattop Correction",				SET_CORRECTION_FLAT,			"", "SHOW_CORRECTION_FLAT, SET_CORRECTION_FLAT -n.nnnnnnn",								"-1.0 .992 0 255 5",
	"Shape: Risetime",							SET_SHAP_RISE,				"",	"SHOW_SHAP_RISE, SET_SHAP_RISE nn.n",												"0.2 23.0 1 115 1",
	"Shutdown Type",							SET_SHUTDOWN,					"",	"SHOW_SHUTDOWN, SET_SHUTDOWN_ORTEC, SET_SHUTDOWN_TTL",								"ORTEC,TTL",
	"Pole Zero: PZ Automatic",					SET_PZ_AUTOMATIC_ENABLE,		"", "SHOW_PZ_AUTOMATIC, ENABLE_PZ_AUTOMATIC, DISABLE_PZ_AUTOMATIC",						"ENABLE,DISABLE",
	"Pole Zero: Value",							SET_PZ,						"", "SHOW_PZ, SET_PZ nnnn",																"0 4095 0 4095 -1",
	"Discriminator: Lower Level",				SET_LLD,						"", "SHOW_LLD, SET_LLD nnnnn",															"0 16383 0 16383 -1",
	"Discriminator: Upper Level",				SET_ULD,						"", "SHOW_ULD, SET_ULD nnnnn",															"0 16383 0 16383 -1",
	"Automatic Baseline",						SET_BLRE_AUTOMATIC_ENABLE,	"", "SHOW_BLRE_AUTOMATIC, ENABLE_BLRE_AUTOMATIC, DISABLE_BLRE_AUTOMATIC",				"ENABLE,DISABLE",
	"Zero Dead Time",							SET_ZDT_ENABLE,				"", "SHOW_ZDT, ENABLE_ZDT, DISABLE_ZDT",												"ENABLE,DISABLE",
	"ADC Gate Input Signal",					SET_GATE,						"",	"SHOW_GATE, SET_GATE_ANTICOINCIDENT, SET_GATE_COINCIDENT, SET_GATE_OFF",			"ANTICOINCIDENT,COINCIDENT,OFF",

		NULL
	};
	lpszRow = szRow;

}

CDSPEC50StaticSetupData::~CDSPEC50StaticSetupData()
{
//	TRACE("CDSPEC50StaticSetupData::DTR\n");
}

char *
CDSPEC50StaticSetupData::GetNext()
{
	if (lpszRow[m_uItterator])
		return lpszRow[m_uItterator++];
	else
		return NULL;
}

BEGIN_MESSAGE_MAP(CDSPEC50StaticSetupData, CStatic)
	//{{AFX_MSG_MAP(CDSPEC50StaticSetupData)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSPEC50StaticSetupData message handlers
