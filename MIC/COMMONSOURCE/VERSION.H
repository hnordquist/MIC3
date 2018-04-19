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
// version.h
 
#ifndef VERSION_H
#define VERSION_H

///////////////////////////////////////////////////////////////////////////
//
//  Class CVersionInfo
//
//
//  Handles accessing the VERSIONINFO resource, which is particularly poorly
//  documented by Microsoft, but does define it's items as follows:
//
/* ----- start excerpt ------------
     
====================+===============================================================
Comments            |Specifies additional information that should be displayed for
                    |diagnostic purposes.
====================+===============================================================
CompanyName         |Specifies the company that produced the file-for example,
                    |"Microsoft Corporation" or "Standard Microsystems 
                    |Corporation, Inc.". This string is required.
====================+===============================================================
FileDescription     |Specifies a file description to be presented to users. This
                    |string may be displayed in a list box when the user is 
                    |choosing files to install-for example, "Keyboard Driver for
                    |AT-Style Keyboards" or "Microsoft Word for Windows". This
                    |string is required.
====================+===============================================================
FileVersion         |Specifies the version number of the file-for example, "3.10"
                    |or "5.00.RC2". This string is required.
====================+===============================================================
InternalName        |Specifies the internal name of the file, if one exists-for
                    |example, a module name if the file is a dynamic-link library.
                    |If the file has no internal name, this string should be the
                    |original filename, without extension. This string is 
                    |required.
====================+===============================================================
LegalCopyright      |Specifies all copyright notices that apply to the file. This
                    |should include the full text of all notices, legal symbols,
                    |copyright dates, and so on-for example, "Copyright Microsoft
                    |Corp. 1990,1991". This string is optional.
====================+===============================================================
LegalTrademarks     |Specifies all trademarks and registered trademarks that apply
                    |to the file. This should include the full text of all 
                    |notices, legal symbols, trademark numbers, and so on-for
                    |example, "Windows(TM) is a trademark of Microsoft 
                    |Corporation". This string is optional.
====================+===============================================================
OriginalFilename    |Specifies the original name of the file, not including a
                    |path. This information enables an application to determine
                    |whether a file has been renamed by a user. The format of the
                    |name depends on the file system for which the file was 
                    |created. This string is required.
====================+===============================================================
PrivateBuild        |Specifies information about a private version of the file-for
                    |example, "Built by TESTER1 on \TESTBED". This string should
                    |be present only if the VS_FF_PRIVATEBUILD flag is set in the
                    |dwFileFlags member of the VS_FIXEDFILEINFO structure of the
                    |root block.
====================+===============================================================
ProductName         |Specifies the name of the product with which the file is
                    |distributed-for example, "Microsoft Windows". This string is
                    |required.
====================+===============================================================
ProductVersion      |Specifies the version of the product with which the file is
                    |distributed-for example, "3.10" or "5.00.RC2". This string is
                    |required.
====================+===============================================================
SpecialBuild        |Specifies how this version of the file differs from the
                    |standard version-for example, "Private build for TESTER1
                    |solving mouse problems on M250 and M250E computers". This
                    |string should be present only if the VS_FF_SPECIALBUILD flag
                    |is set in the dwFileFlags member of the VS_FIXEDFILEINFO
                    |structure in the root block.
====================+===============================================================
     
-------- end excerpt   --- */
     
//  CVersionInfo objects have a member function for each of the above elements
//  returned as CString objects.
//
//  Values are only retrieved from the VERSIONINFO structure; they cannot be
//  written to.
//
//  If new elements are added to VERSIONINFO structure in the future, they can
//  be retieved using the GetValue() member function. 
//
     
class CVersionInfo
{
public:
    CVersionInfo(); 
    ~CVersionInfo();
     
    CString Comments()        {return GetValue("Comments");} 
    CString CompanyName()     {return GetValue("CompanyName");} 
    CString FileDescription() {return GetValue("FileDescription");} 
    CString FileVersion()     {return GetValue("FileVersion");} 
    CString InternalName()    {return GetValue("InternalName");}
    CString OriginalFilename(){return GetValue("OriginalFilename");} 
    CString ProductName()     {return GetValue("ProductName");} 
    CString ProductVersion()  {return GetValue("ProductVersion");} 
    CString SpecialBuild()    {return GetValue("SpecialBuild");} 
    CString LegalCopyright()  {return GetValue("LegalCopyright");} 
    CString LegalTrademarks() {return GetValue("LegalTrademarks");} 
    CString PrivateBuild()    {return GetValue("PrivateBuild");}
	CString FilePath()		  {return CString(m_name);}
     
    CString GetValue(CString const sSubBlock);
    char    m_name[_MAX_PATH];

     
private:
    CString     m_sSubBlockHead;
    void *      m_VerData;
     
};

#endif VERSION_H