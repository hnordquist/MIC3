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
// ini_definitions.h

#ifndef INI_DEFINITIONS_H
#define INI_DEFINITIONS_H

#define AUTORESTARTCOUNT	"AUTORESTARTCOUNT"
#define BAUDRATE			"BaudRate"
#define BEATNIK				"BEAT"
#define BUTTONWIDTH			"ButtonWidth"
#define BUTTONFLAGS			"BUTTONFLAGS"
#define BUTTONFLAGSIZE		"ButtonFlagSize"
#define BUTTONHEIGHT		"ButtonHeight"
#define BUTTONSWIDE			"ButtonsWide"  //number of columns of buttons
#define COMMFAIL			"COMMFAIL"
#define COMMUNICATIONS		"Communications"
#define CONFIGURATION		"Configuration"
#define LAST_VALIDATION		"Last_Validation"
#define DATABITS			"DataBits"
#define DAYSBEFOREDELFILES	"DaysBeforeDelFiles"
#define DEFAULTCOPYPATH		"DefaultCopyPath"
#define DIALOGCLOSE			"DialogClose"
#define DOINGNETWORK		"ACTIVE"       //<<QA>> maybe we should rename for clarity
#define DO_DUMP				"DO_DUMP"
#define DOSHORTNAME			"DOSHORTNAME"
#define FACILITYNAME		"FacilityName"
#define FASTSTARTUP			"FastStartup"
#define FILE_ID				"FILE_ID"
#define FLAGCOMM			"FLAGCOMM"
#define FLAGTIME			"FLAGTIME"
#define FLAGOTHR			"FLAGOTHR"
#define HIDEWATCH			"HideWatch"
#define HIDEWINDOWS			"HideWindows"
#define INSTRUMENTS			"Instruments"
#define IPDATE				"IPDate"
#define LAST_X				"LAST_X"
#define LAST_Y				"LAST_Y"
#define LOG_MII				"LOG_MII"
#define LOGARCHIVEACT		"LogArchiveAct"
#define LONGNAMEFORM		"LONGNAMEFORM"
#define MAXPAUSE			"MAXPAUSE"
#define MAXPERCENTDISKUSE	"MaxPercentDiskUse"
#define MAX_REP					"MAX_REP"
#define MINUTESTOCHECKARCHIVE	"MinutesToCheckArchive"
#define MODE					"Mode"
#define NETWORK					"NETWORK"
#define NODE					"NODE"
#define NORMALSHUTDOWN			"NormalShutdown"
#define PARITY					"Parity"
#define PORT					"Port"
#define SAVE_LOC				"SAVE_LOC"
#define SLOW_RESET				"SLOW_RESET"
#define SLOWSTART				"SlowStart"
#define SOH_APPEND				"SOH_APPEND"
#define SOH_BAD					"SOH_BAD"
#define SOH_BADTOGOOD			"SOH_BADTOGOOD"
#define SOH_GOOD				"SOH_GOOD"
#define SOH_LIMITED_EXT			"SOH_LIMITED_EXT"
#define SOH_PATH				"SOH_PATH"
#define SOH_PATH_EXT			"SOH_PATH_EXT"
#define SOH_TIME_EXT			"SOH_TIME_EXT"
#define SOH_TIME_EXT_LAST		"SOH_TIME_EXT_LAST"
#define SOH_DAILY				"SOH_DAILY"
#define SOH_DAILYAT				"SOH_DAILYAT"
#define STOPBITS				"StopBits"
#define SUPERUSERS				"SuperUsers"
#define USERS					"Users"

#endif //INI_DEFINITIONS.H