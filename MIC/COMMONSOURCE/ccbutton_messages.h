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
// ccbutton_messages.h $
 
#ifndef CCBUTTON_MESSAGES_H
#define CCBUTTON_MESSAGES_H

//<<QA>> What is the likelihood of these being non-unique?

#define WM_DISPLAY_NUMERIC_STRING	(WM_USER + 1)
#define WM_GET_FACE_COLOR			(WM_USER + 2)
#define WM_PUT_FACE_COLOR			(WM_USER + 3)
#define WM_PUT_FLASH				(WM_USER + 4)
#define WM_PUT_NUMERIC				(WM_USER + 5)
#define WM_PUT_NUMERIC_STRING		(WM_USER + 6)
#define WM_SHOW_ICON_BELL			(WM_USER + 7)
#define WM_SHOW_ICON_CLOCK			(WM_USER + 8)
#define WM_SHOW_ICON_FROWN			(WM_USER + 9)
#define WM_SET_ERROR_ICON_BOMB		(WM_USER + 10)
#define WM_SET_ERROR_ICON_FLAG		(WM_USER + 11)
#define WM_SET_ERROR_ICON_SKULL		(WM_USER + 12)
#define WM_SHOW_ICON_ERROR			(WM_USER + 13)
#define WM_SET_DISK_ICON_FLOPPY		(WM_USER + 14)
#define WM_SET_DISK_ICON_HOURGLASS	(WM_USER + 15)
#define WM_SHOW_ICON_DISK			(WM_USER + 16)
#define WM_SHOW_ICON_SQUARE			(WM_USER + 17)
#define WM_GET_NUMERIC              (WM_USER + 18)
#define WM_GET_VERSION				(WM_USER + 19)

#endif //CCBUTTON_MESSAGES_H