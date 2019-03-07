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
// DSPECSetEdit.cpp

#include "stdafx.h"
#include "resource.h"
#include "DSPECSetEdit.h"
#include "DSPECStrings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDSPECSetEdit::CDSPECSetEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDSPECSetEdit::IDD, pParent)
{
	m_uLength = 0;
	//{{AFX_DATA_INIT(CDSPECSetEdit)
	m_cDesired = _T("");
	m_cDiscription = _T("");
	m_cReported = _T("");
	m_cIniName = _T("");
	m_cCommand = _T("");
	m_cAllowedValues = _T("");
	//}}AFX_DATA_INIT
}

void CDSPECSetEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSPECSetEdit)
	DDX_Text(pDX, IDC_DSPEC_STATIC_DESCRIPTION, m_cDiscription);
	DDV_MaxChars(pDX, m_cDiscription, 128);
	DDX_Text(pDX, IDC_DSPEC_STATIC_REPORTED, m_cReported);
	DDV_MaxChars(pDX, m_cReported, 128);
	DDX_Text(pDX, IDC_DSPEC_STATIC_ININAME, m_cIniName);
	DDX_Text(pDX, IDC_DSPEC_STATIC_COMMAND, m_cCommand);
	DDX_Text(pDX, IDC_DSPEC_STATIC_ALLOWED_VALUES, m_cAllowedValues);
	DDV_MaxChars(pDX, m_cAllowedValues, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSPECSetEdit, CDialog)
	//{{AFX_MSG_MAP(CDSPECSetEdit)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(UDN_DELTAPOS, IDC_DSPEC_BUDDYSPIN, OnDeltaposDSPECBuddySpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetEdit message handlers

BOOL CDSPECSetEdit::OnInitDialog() 
{
	int pAccel [] =
	{
		0,1,
		2,10,
		4,100,
		6,1000,
		8,10000,
	};


	CDialog::OnInitDialog();
	
	m_cComboBox.SubclassDlgItem(IDC_DSPEC_COMBOBOX, this);
	m_cEdit.SubclassDlgItem(IDC_DSPEC_EDIT,this);
	m_cSpinButton.SubclassDlgItem(IDC_DSPEC_BUDDYSPIN,this);

	m_cSpinButton.SetRange(0,32000);
	m_cSpinButton.SetPos(0);
	m_cSpinButton.SetAccel(5, (struct _UDACCEL *)pAccel );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDSPECSetEdit::OnShowWindow( BOOL bShow, UINT nStatus )
{
 	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
	{
		GetDlgItem(IDC_DSPEC_STATIC_DESIRED)->SetWindowText(m_cDesired);
		// look at the allowed values to determine if the choices are from
		// a list (use combo box) or from a range of numbers (use edit/buddyspin)

		int comma_location = m_cAllowedValues.Find(',',0);

		if (comma_location != -1)  //is a comma-delimited string of discrete values
		{
			// set up combo box with the discrete list of selections
			m_bUsingComboBox = true;
			m_cEdit.ShowWindow(SW_HIDE);
			m_cSpinButton.ShowWindow(SW_HIDE);
			PopulateComboBox();
		}
		else
		{
			// set up the edit/buddyspin pair to collect a numerical value			
			m_bUsingComboBox = false;
			m_cComboBox.ShowWindow(SW_HIDE);
			InitializeEditSpinBuddyPair();
		}
	}
}

void CDSPECSetEdit::PopulateComboBox()
{
	char *allowed_values = (char *)LPCTSTR(m_cAllowedValues);
	char *desired = (char *)LPCTSTR(m_cDesired);
	char substring[64];

	int  iselection = 0;
	char *at = allowed_values;
	bool match_found = false;
	bool last_one = false;

	m_cComboBox.ResetContent();

	while (!last_one)
	{
		strcpy(substring,at);
		strtok(substring,",");	 //chop off one element
		m_cComboBox.AddString(substring);

		if (strlen(substring) == strlen(at))
			last_one = true;

		if (!match_found)
		{
			if ((strcmp(substring,desired)) != 0)
				iselection++;
			else
				match_found = true;
		}
		at = at + strlen(substring) + 1;
	}
	m_cComboBox.SetCurSel(iselection);
}

void CDSPECSetEdit::InitializeEditSpinBuddyPair()
{
	// calculate the parameters for the spin control
	// format is:  min max steplow stephigh

	int decimal_location = m_cAllowedValues.Find('.',0);
	if (decimal_location == -1)
		m_bIntegerOnly = true;
	else
		m_bIntegerOnly = false;

	char *at = (char *)LPCTSTR(m_cAllowedValues);
	char substring[64];
	char fformat[] = "%f";
	char uformat[] = "%u";
	char iformat[] = "%d";
	unsigned ulowstep, uhighstep, unumsteps;
	float ftemp;

	 //first element is min value
	strcpy(substring,at);
	strtok(substring," ");
	sscanf(substring,fformat,&ftemp);
	m_dSpinMin = (double)ftemp;
	at = at + strlen(substring) + 1;

	//second element is max value
	strcpy(substring,at);
	strtok(substring," ");
	sscanf(substring,fformat,&ftemp);
	m_dSpinMax = (double)ftemp;
	at = at + strlen(substring) + 1;

	//third element is low value used in calculating number of steps
	strcpy(substring,at);
	strtok(substring," ");
	sscanf(substring,uformat,&ulowstep);
	at = at + strlen(substring) + 1;

	//fourth element is high value used in calculating number of steps
	strcpy(substring,at);
	strtok(substring," ");
	sscanf(substring,uformat,&uhighstep);
	at = at + strlen(substring) + 1;

	//fifth element is the number of places to the right of the decimal
	strcpy(substring,at);
	strtok(substring," ");
	sscanf(substring,iformat,&m_iRightOfDecimal);

	unumsteps = uhighstep - ulowstep;  //<<QA>> inconsistent in DSPEC manual

	if (unumsteps != 0)
		m_dSpinStepSize = (m_dSpinMax - m_dSpinMin)/(double)unumsteps;
	else
		m_dSpinStepSize = 0.0;

	// The gainfine sledgehammer:
	if (m_cIniName == CString(SET_GAIN_FINE))
		m_dSpinStepSize = 0.0001;  //MIC_CB_028 to MIC 1.907 PJM 1/19/05

	// The correction flat sledgehammer:
	if (m_cIniName == CString(SET_CORRECTION_FLAT))
		m_dSpinStepSize = 0.00001;

	// put the "coming-in" value in the edit box
	m_cEdit.SetWindowText(m_cDesired);
}
void CDSPECSetEdit::OnOK() 
{
//	TRACE("CDSPECSetEdit::OnOK()\n");

	// the parent looks in m_cDesired for the new value.  So we
	// need to put it there.

	if (m_bUsingComboBox)
		m_cComboBox.GetWindowText(m_cDesired);
	else
		m_cEdit.GetWindowText(m_cDesired);

	CDialog::OnOK();
}

void CDSPECSetEdit::OnCancel() 
{
//	TRACE("CDSPECSetEdit::OnCancel()\n");
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

void CDSPECSetEdit::OnDeltaposDSPECBuddySpin(NMHDR* pNMHDR,LRESULT* pResult)
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	char text[16], cleantext[16];
	
	m_cEdit.GetWindowText(text,sizeof(text)-1);

	double dNew = atof(text) + (m_dSpinStepSize *(double)(pNMUpDown->iDelta));

	if (dNew > m_dSpinMax)
		dNew = m_dSpinMax; 

	if (dNew < m_dSpinMin) 
		dNew = m_dSpinMin;

	if (m_bIntegerOnly)  //round it
	{
		if (dNew >= 0.0)
			dNew += 0.5;
		else
			dNew -= 0.5;
		int iNew = (int)dNew;

		sprintf(text,"%d",iNew);
	}
	else
	{
		sprintf(text,"%f",dNew);
	}

	CleanUpString(cleantext, text, sizeof(cleantext), m_iRightOfDecimal);
	m_cEdit.SetWindowText(cleantext);
	*pResult = 0;
}

//////////////////////////////////////////////////////////////////////////////
//
//                          CleanUpString
//
///////////////////////////////////////////////////////////////////////////////
void CDSPECSetEdit::CleanUpString(char *destination, 
									char *source, 
									int destination_length,
									int digits_after_decimal)
{
	size_t source_length = strlen(source);

	char *firstchar    = source;
	char *decimalpoint = strchr(source,'.');
	char *nullpos = source + source_length;  //terminating null

	char *minussign = strchr(source,'-');
	if (minussign == firstchar)
		firstchar++;

	bool bleadingzero = false;

	memset(destination,0,destination_length);
	int allowed_chars = destination_length - 1;

	//does the source string have a decimal place?
	if (decimalpoint != NULL)
	{
		for (; firstchar < decimalpoint; firstchar++)
		{
			if (*firstchar != '0')
				break;
			else
				bleadingzero = true;
		}
		if (bleadingzero && (firstchar == decimalpoint))
			firstchar--;
	}
	else
	{
		//no decimal point - assume integer
		for (; firstchar < nullpos; firstchar++)
		{
			if (*firstchar != '0')
				break;
			else
				bleadingzero = true;
		}
		if (bleadingzero && (firstchar == nullpos))
			firstchar--;
	}

	if (minussign != NULL)
	{
		*destination++ = '-';
		destination_length--;
	}

	size_t numchars = strlen(firstchar);
	if (numchars >= (size_t)destination_length)
		numchars = destination_length - 1;

	// clean up any trailing zeroes
	if (decimalpoint)  //don't do this for integers!
	{
		char *lastchar= strchr(firstchar,0) - 1;

		for (;;)
		{
			if ((lastchar != decimalpoint) && (*lastchar == '0'))
				*lastchar-- = 0;
			else
				break;
		}
	}

	if ((digits_after_decimal > 0) && (digits_after_decimal < 10) )  //allow 1 - 9
	{
			if (!decimalpoint)
			{
				strncat(firstchar, ".", 1);  //append the dp
				numchars++;
				for (int i = 0; i < digits_after_decimal; i++)
				{
					if (numchars >= (size_t)allowed_chars)
						break;
					strncat(firstchar, "0", 1);
					numchars++;
				}
			}
			else
			{
				float fnum;
				int i,total;
				unsigned int unum;

				sscanf(firstchar,"%f",&fnum);

				float faddfactor = 0.5;
				float fmulfactor = 1.0;

				for (i = 0; i < digits_after_decimal; i++)
				{
					faddfactor /= 10.0;
					fmulfactor *= 10.0;
				}

				fnum += faddfactor;
				fnum *= fmulfactor;

				unum = (unsigned int)(fnum);  //truncate
				fnum = (float)unum;
				fnum /= fmulfactor;

				int digits_to_the_left = 0;
				float fleft = 1.0;
				
				for (;;)
				{
					if (fnum < fleft)
					{
						break;
					}
					else
					{
						fleft *= 10.0;
						digits_to_the_left++;

						if (digits_to_the_left >= 5)  //need some limit here
							break;
					}
				}

				if (digits_to_the_left == 0)  //leading zero
					digits_to_the_left = 1;

				total = digits_to_the_left + 1 + digits_after_decimal;

				char fmt[16], temp[16];;
				memset(fmt,0,16);

				fmt[0] = '%';							//"%"
				sprintf(temp,"%d",total);
				strncat(fmt,temp,3);					//"%7"
				strncat(fmt,".",1);						//"%7."
				sprintf(temp,"%d",digits_after_decimal);
				strncat(fmt,temp,3);					//"%7.5"
				strncat(fmt,"f",1);						//"%7.5f"

				sprintf(firstchar,fmt,fnum);
			}
	}
	memcpy(destination, firstchar, numchars);
}
