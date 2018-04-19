// CoHHMRISO.cpp : Implementation of DLL Exports.

// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f CoHHMRISOps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "CoHHMRISO.h"

#include "CoHHMRISO_i.c"
#include "HHMRISO.h"
#include "RegisterByCategory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_HHMRISO, CHHMRISO)
END_OBJECT_MAP()

class CCoHHMRISOApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoHHMRISOApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCoHHMRISOApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CCoHHMRISOApp, CWinApp)
	//{{AFX_MSG_MAP(CCoHHMRISOApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CCoHHMRISOApp theApp;

BOOL CCoHHMRISOApp::InitInstance()
{
//	TRACE("CCoHHMRISOApp::InitInstance()\n");
    _Module.Init(ObjectMap, m_hInstance, &LIBID_COHHMRISOLib);
    return CWinApp::InitInstance();
}

int CCoHHMRISOApp::ExitInstance()
{
//	TRACE("CCoHHMRISOApp::ExitInstance()\n");
    _Module.Term();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
	HRESULT hr = _Module.RegisterServer(TRUE);

	if (SUCCEEDED(hr))
		hr = RegisterThisComponentWithItsCategory(MIC_ISO,CLSID_HHMRISO);

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	UnRegisterThisComponentFromItsCategory(MIC_ISO,CLSID_HHMRISO);

	return _Module.UnregisterServer(TRUE);
}


