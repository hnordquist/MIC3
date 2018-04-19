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
// colors.h
 
#ifndef COLORS_H
#define COLORS_H

#define COLOR_RED				RGB(255,0,0)
#define COLOR_GREEN				RGB(0,255,40)
#define COLOR_DARKGREEN			RGB(0,128,20)
#define COLOR_BLUE				RGB(0,0,255)
#define COLOR_DARKBLUE			RGB(0,0,128)

#define COLOR_GRAY				RGB(192,192,192)
#define COLOR_YELLOW			RGB(255,255,0)
#define COLOR_CYAN				RGB(0,255,255)
#define COLOR_MAGENTA			RGB(255,0,255)
#define COLOR_BLACK				RGB(0,0,0)
#define COLOR_WHITE				RGB(255,255,255)

#define COLOR_YELLOWRED			RGB(255,255,1)
#define COLOR_LIGHT_GREEN       RGB(170,240,170) //pjm 01/26/05
#define COLOR_LIGHT_BLUE_GREEN  RGB(160,210,210) //pjm 01/26/05


#define COLOR_PAUSE				COLOR_GRAY
#define COLOR_WAIT				COLOR_YELLOW
#define COLOR_OK				COLOR_GREEN
#define COLOR_ERROR				COLOR_RED
#define COLOR_DEFAULT_TEXT		COLOR_BLACK

#endif COLORS_H