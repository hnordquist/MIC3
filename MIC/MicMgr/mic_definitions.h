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


#ifndef MIC_DEFINITIONS_H
#define MIC_DEFINITIONS_H

// MIC_definitions.h

// For MICGM, we want this file to contain NO definitions needed by the components.

// Have changed some of the names to include "MIC_" at the front end to ensure there
// is no place in the components' code where these definitions are used.

//<<QA>>#include "ALLOWTRIGGERHOLDOFF.h"  add this back in

/////////////////////////////////////////////////////////////////////////////
// Defines
//nameshort is used for INI file name
//namelong  is used for window titles
#define MIC_NAMELONG				"Multi-Instrument Collect"
#define MIC_NAMEMEDIUM				"Multi-Inst. Collect"
#define MIC_NAMESHORT				"MIC"

#define INVALIDCHARS			"^&+|<>,=[]?*\\/;\""
#define INVALIDCHARSSUB			"^&+|<>,=[]?*\""
#define INVALIDFILENAMECHARS	"|<>*?:\\"

//#define MIC_MAX_EXTEND				7200
//#define MIC_BUMP_EXTEND				300

//number of characters to make password
#define MIC_PASSWORDLEN				20

//
// MIC default values for ini entries
// These are "tuneable".
//

#define BUTTONSWIDE_DEFAULT				5
#define BUTTONWIDTH_DEFAULT				87
#define BUTTONHEIGHT_DEFAULT			70
#define BUTTONFLAGSIZE_DEFAULT			12
#define DAYSBEFOREDELFILES_DEFAULT		90
#define DEFAULTCOPYPATH_DEFAULT			"A:"
#define DIALOGCLOSE_DEFAULT				30
#define DOINGNETWORK_DEFAULT			0
#define BEATNIK_DEFAULT					0
#define HIDEWINDOWS_DEFAULT				0
#define LOGARCHIVEACT_DEFAULT			false
#define MINUTESTOCHECKARCHIVE_DEFAULT	60
#define LONGNAMEFORM_DEFAULT			"AA%T%I_%y%m%d_%H%M%S"

#endif //MIC_DEFINITIONS_H