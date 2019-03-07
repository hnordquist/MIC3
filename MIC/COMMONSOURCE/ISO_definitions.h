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
// ISO_definitions.h

#ifndef ISO_DEFINITIONS_H
#define ISO_DEFINITIONS_H

#define RFIDDEMO
//#undef RFIDDEMO


#define SLOWRESET       900000

#define CHECKSUM_GOOD   1
#define CHECKSUM_BAD    0

#define TYPE_GID2		1
#define TYPE_INST		2
#define TYPE_COMP		3
#define TYPE_TIME		4
#define	TYPE_INVTIME	5
#define TYPE_START		6
#define TYPE_DELETE		7
#define TYPE_DUMP		8
#define TYPE_ABNORMAL	9
#define TYPE_NEWDAY		10

#define TO_CEV			0x00000001
#define TO_PFM			0x00000002
#define TO_DMP			0x00000004

#define BOMB_CHAR      'M'
#define SKULL_CHAR     'N'
#define FLAG_CHAR      'O'
#define CHICK_CHAR     'q'  //MIC_CB_028 to MIC 1.907 PJM 1/19/05

#define OOBFLAG			0x00010000

#define TIMER_PAUSE				1
#define TIMER_TICK				2
//#define TIMER_UPDATE			4
#define TIMER_CAMERA_START		5
#define TIMER_CAMERA_END		6
#define TIMER_EXCLUSION			7
#define TIMER_ANALYSIS_DWELL	8
#define TIMER_REALTIME_DWELL	9
#define TIMER_AUTOCLOSE			10

#define COLLECTINGBEGIN			"Begin\nCollecting"
#define COLLECTINGPAUSE			"Pause\nCollecting"

#define BUMP_EXTEND	300
#define MAX_EXTEND 7200

#define CEV_SUFFIX		"CEV"
#define PFM_SUFFIX		"PFM"
#define DMP_SUFFIX		"DMP"
#define BID_SUFFIX		"BID"
#define ISR_SUFFIX		"ISR"
#define JSR_SUFFIX		"JSR"
#define MCA_SUFFIX		"MCA"
#define BNY_SUFFIX		"BNY"
#define GPS_SUFFIX		"GPS"
#define VCS_SUFFIX		"VCS"
#define EVT_SUFFIX		"EVT"
#define CHN_SUFFIX		"CHN"
#define MGD_SUFFIX		"MGD" //microGrand
#define SOH_SUFFIX		"SOH"
#define PSR_SUFFIX		"PSR"
#define HMR_SUFFIX		"HMR"
#define ESS_SUFFIX		"ESS"
#ifdef RFIDDEMO
#define RFI_SUFFIX		"RFI"
#define RAD_SUFFIX		"RAD"
#endif
#define ID_CLOSE_BUTTON                 9000
#define ID_PAUSE_BUTTON                 9001
#define ID_PRINT_BUTTON                 9002
#define ID_ACKFL_BUTTON                 9003
#define ID_TOCLOSE                      9004

//<<QA>> put this somewhere else eventually.
#define IDC_EXTENDCLOSE		35999    //MIC_CB_028 to MIC 1.907 PJM 1/19/05


#endif //ISO_DEFINITIONS_H