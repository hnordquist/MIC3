

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Mar 07 15:14:46 2019
 */
/* Compiler settings for CoDirectSerialCSO.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CoDirectSerialCSO_h__
#define __CoDirectSerialCSO_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICSO_FWD_DEFINED__
#define __ICSO_FWD_DEFINED__
typedef interface ICSO ICSO;
#endif 	/* __ICSO_FWD_DEFINED__ */


#ifndef __DirectSerialCSO_FWD_DEFINED__
#define __DirectSerialCSO_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirectSerialCSO DirectSerialCSO;
#else
typedef struct DirectSerialCSO DirectSerialCSO;
#endif /* __cplusplus */

#endif 	/* __DirectSerialCSO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICSO_INTERFACE_DEFINED__
#define __ICSO_INTERFACE_DEFINED__

/* interface ICSO */
/* [uuid][object] */ 

typedef /* [v1_enum] */ 
enum tagISO_FUNCTIONAL_CATEGORY
    {	STANDARD_ISO	= 0,
	WATCH_ISO	= 1,
	UPS_ISO	= 2,
	UNKNOWN_ISO	= 99
    } 	ISO_FUNCTIONAL_CATEGORY;


EXTERN_C const IID IID_ICSO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B7180771-895F-417E-8239-353EE09F7009")
    ICSO : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CollectParametersForNewCSO( 
            /* [out] */ BSTR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL *pvbHaveNewCSO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableHeartbeat( 
            /* [in] */ VARIANT_BOOL vbCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_CSOTypeString( 
            /* [out] */ BSTR *pbstrTypeString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [out] */ BSTR *pbstrErrorString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_VersionString( 
            /* [out] */ BSTR *pbstrVersionString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_DialogAutoCloseTime( 
            /* [in] */ ULONG uSeconds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_INIPathname( 
            /* [in] */ BSTR bstrPathname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_WindowsRegisteredMessageStrings( 
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ThresholdValue( 
            /* [in] */ ULONG uThresholdValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterISO( 
            /* [in] */ ULONG uReceiverHandle,
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory,
            /* [in] */ VARIANT_BOOL vbOOB,
            /* [out] */ VARIANT_BOOL *pvbResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendHeartbeat( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendToWatch( 
            /* [in] */ BSTR bstrDisplayString,
            /* [in] */ ULONG uNode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUpPort( 
            /* [in] */ BSTR bstrPortName,
            /* [out] */ VARIANT_BOOL *pvbResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShutdownCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Transmit( 
            /* [in] */ ULONG uLength,
            /* [in] */ BLOB blobData,
            /* [in] */ ULONG uNode,
            /* [in] */ VARIANT_BOOL vbQuiet,
            /* [out] */ ULONG *puBytesWritten) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnRegisterISO( 
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICSO * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICSO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICSO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICSO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICSO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICSO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICSO * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CollectParametersForNewCSO )( 
            ICSO * This,
            /* [out] */ BSTR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL *pvbHaveNewCSO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableHeartbeat )( 
            ICSO * This,
            /* [in] */ VARIANT_BOOL vbCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_CSOTypeString )( 
            ICSO * This,
            /* [out] */ BSTR *pbstrTypeString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            ICSO * This,
            /* [out] */ BSTR *pbstrErrorString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            ICSO * This,
            /* [out] */ BSTR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_VersionString )( 
            ICSO * This,
            /* [out] */ BSTR *pbstrVersionString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_DialogAutoCloseTime )( 
            ICSO * This,
            /* [in] */ ULONG uSeconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_INIPathname )( 
            ICSO * This,
            /* [in] */ BSTR bstrPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            ICSO * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowsRegisteredMessageStrings )( 
            ICSO * This,
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_ThresholdValue )( 
            ICSO * This,
            /* [in] */ ULONG uThresholdValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterISO )( 
            ICSO * This,
            /* [in] */ ULONG uReceiverHandle,
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory,
            /* [in] */ VARIANT_BOOL vbOOB,
            /* [out] */ VARIANT_BOOL *pvbResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetCSO )( 
            ICSO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SendHeartbeat )( 
            ICSO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SendToWatch )( 
            ICSO * This,
            /* [in] */ BSTR bstrDisplayString,
            /* [in] */ ULONG uNode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetUpPort )( 
            ICSO * This,
            /* [in] */ BSTR bstrPortName,
            /* [out] */ VARIANT_BOOL *pvbResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShutdownCSO )( 
            ICSO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartCSO )( 
            ICSO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Transmit )( 
            ICSO * This,
            /* [in] */ ULONG uLength,
            /* [in] */ BLOB blobData,
            /* [in] */ ULONG uNode,
            /* [in] */ VARIANT_BOOL vbQuiet,
            /* [out] */ ULONG *puBytesWritten);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnRegisterISO )( 
            ICSO * This,
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory);
        
        END_INTERFACE
    } ICSOVtbl;

    interface ICSO
    {
        CONST_VTBL struct ICSOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSO_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICSO_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICSO_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICSO_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICSO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICSO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICSO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICSO_CollectParametersForNewCSO(This,pbstrNameOut,pvbHaveNewCSO)	\
    ( (This)->lpVtbl -> CollectParametersForNewCSO(This,pbstrNameOut,pvbHaveNewCSO) ) 

#define ICSO_EnableHeartbeat(This,vbCommand)	\
    ( (This)->lpVtbl -> EnableHeartbeat(This,vbCommand) ) 

#define ICSO_get_CSOTypeString(This,pbstrTypeString)	\
    ( (This)->lpVtbl -> get_CSOTypeString(This,pbstrTypeString) ) 

#define ICSO_get_LastError(This,pbstrErrorString)	\
    ( (This)->lpVtbl -> get_LastError(This,pbstrErrorString) ) 

#define ICSO_get_Name(This,pbstrName)	\
    ( (This)->lpVtbl -> get_Name(This,pbstrName) ) 

#define ICSO_get_VersionString(This,pbstrVersionString)	\
    ( (This)->lpVtbl -> get_VersionString(This,pbstrVersionString) ) 

#define ICSO_put_DialogAutoCloseTime(This,uSeconds)	\
    ( (This)->lpVtbl -> put_DialogAutoCloseTime(This,uSeconds) ) 

#define ICSO_put_INIPathname(This,bstrPathname)	\
    ( (This)->lpVtbl -> put_INIPathname(This,bstrPathname) ) 

#define ICSO_put_Name(This,bstrName)	\
    ( (This)->lpVtbl -> put_Name(This,bstrName) ) 

#define ICSO_put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)	\
    ( (This)->lpVtbl -> put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET) ) 

#define ICSO_put_ThresholdValue(This,uThresholdValue)	\
    ( (This)->lpVtbl -> put_ThresholdValue(This,uThresholdValue) ) 

#define ICSO_RegisterISO(This,uReceiverHandle,uNode,eCategory,vbOOB,pvbResult)	\
    ( (This)->lpVtbl -> RegisterISO(This,uReceiverHandle,uNode,eCategory,vbOOB,pvbResult) ) 

#define ICSO_ResetCSO(This)	\
    ( (This)->lpVtbl -> ResetCSO(This) ) 

#define ICSO_SendHeartbeat(This)	\
    ( (This)->lpVtbl -> SendHeartbeat(This) ) 

#define ICSO_SendToWatch(This,bstrDisplayString,uNode)	\
    ( (This)->lpVtbl -> SendToWatch(This,bstrDisplayString,uNode) ) 

#define ICSO_SetUpPort(This,bstrPortName,pvbResult)	\
    ( (This)->lpVtbl -> SetUpPort(This,bstrPortName,pvbResult) ) 

#define ICSO_ShutdownCSO(This)	\
    ( (This)->lpVtbl -> ShutdownCSO(This) ) 

#define ICSO_StartCSO(This)	\
    ( (This)->lpVtbl -> StartCSO(This) ) 

#define ICSO_Transmit(This,uLength,blobData,uNode,vbQuiet,puBytesWritten)	\
    ( (This)->lpVtbl -> Transmit(This,uLength,blobData,uNode,vbQuiet,puBytesWritten) ) 

#define ICSO_UnRegisterISO(This,uNode,eCategory)	\
    ( (This)->lpVtbl -> UnRegisterISO(This,uNode,eCategory) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICSO_INTERFACE_DEFINED__ */



#ifndef __CODIRECTSERIALCSOLib_LIBRARY_DEFINED__
#define __CODIRECTSERIALCSOLib_LIBRARY_DEFINED__

/* library CODIRECTSERIALCSOLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CODIRECTSERIALCSOLib;

EXTERN_C const CLSID CLSID_DirectSerialCSO;

#ifdef __cplusplus

class DECLSPEC_UUID("7F36351B-8AAD-4799-AC44-1F23FD8505C2")
DirectSerialCSO;
#endif
#endif /* __CODIRECTSERIALCSOLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


