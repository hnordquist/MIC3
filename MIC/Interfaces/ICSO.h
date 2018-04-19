/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:13:17 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\Interfaces\ICSO.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __ICSO_h__
#define __ICSO_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICSO_FWD_DEFINED__
#define __ICSO_FWD_DEFINED__
typedef interface ICSO ICSO;
#endif 	/* __ICSO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

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
    }	ISO_FUNCTIONAL_CATEGORY;


EXTERN_C const IID IID_ICSO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B7180771-895F-417E-8239-353EE09F7009")
    ICSO : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CollectParametersForNewCSO( 
            /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewCSO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableHeartbeat( 
            /* [in] */ VARIANT_BOOL vbCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_CSOTypeString( 
            /* [out] */ BSTR __RPC_FAR *pbstrTypeString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [out] */ BSTR __RPC_FAR *pbstrErrorString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_VersionString( 
            /* [out] */ BSTR __RPC_FAR *pbstrVersionString) = 0;
        
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
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendHeartbeat( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SendToWatch( 
            /* [in] */ BSTR bstrDisplayString,
            /* [in] */ ULONG uNode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetUpPort( 
            /* [in] */ BSTR bstrPortName,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShutdownCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartCSO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Transmit( 
            /* [in] */ ULONG uLength,
            /* [in] */ BLOB blobData,
            /* [in] */ ULONG uNode,
            /* [in] */ VARIANT_BOOL vbQuiet,
            /* [out] */ ULONG __RPC_FAR *puBytesWritten) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnRegisterISO( 
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICSOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICSO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICSO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICSO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICSO __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICSO __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICSO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICSO __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CollectParametersForNewCSO )( 
            ICSO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewCSO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableHeartbeat )( 
            ICSO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vbCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CSOTypeString )( 
            ICSO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrTypeString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastError )( 
            ICSO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrErrorString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ICSO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VersionString )( 
            ICSO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrVersionString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DialogAutoCloseTime )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG uSeconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_INIPathname )( 
            ICSO __RPC_FAR * This,
            /* [in] */ BSTR bstrPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            ICSO __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowsRegisteredMessageStrings )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ThresholdValue )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG uThresholdValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterISO )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG uReceiverHandle,
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory,
            /* [in] */ VARIANT_BOOL vbOOB,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetCSO )( 
            ICSO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendHeartbeat )( 
            ICSO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendToWatch )( 
            ICSO __RPC_FAR * This,
            /* [in] */ BSTR bstrDisplayString,
            /* [in] */ ULONG uNode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetUpPort )( 
            ICSO __RPC_FAR * This,
            /* [in] */ BSTR bstrPortName,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownCSO )( 
            ICSO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartCSO )( 
            ICSO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Transmit )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG uLength,
            /* [in] */ BLOB blobData,
            /* [in] */ ULONG uNode,
            /* [in] */ VARIANT_BOOL vbQuiet,
            /* [out] */ ULONG __RPC_FAR *puBytesWritten);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnRegisterISO )( 
            ICSO __RPC_FAR * This,
            /* [in] */ ULONG uNode,
            /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory);
        
        END_INTERFACE
    } ICSOVtbl;

    interface ICSO
    {
        CONST_VTBL struct ICSOVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICSO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICSO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICSO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICSO_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICSO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICSO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICSO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICSO_CollectParametersForNewCSO(This,pbstrNameOut,pvbHaveNewCSO)	\
    (This)->lpVtbl -> CollectParametersForNewCSO(This,pbstrNameOut,pvbHaveNewCSO)

#define ICSO_EnableHeartbeat(This,vbCommand)	\
    (This)->lpVtbl -> EnableHeartbeat(This,vbCommand)

#define ICSO_get_CSOTypeString(This,pbstrTypeString)	\
    (This)->lpVtbl -> get_CSOTypeString(This,pbstrTypeString)

#define ICSO_get_LastError(This,pbstrErrorString)	\
    (This)->lpVtbl -> get_LastError(This,pbstrErrorString)

#define ICSO_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define ICSO_get_VersionString(This,pbstrVersionString)	\
    (This)->lpVtbl -> get_VersionString(This,pbstrVersionString)

#define ICSO_put_DialogAutoCloseTime(This,uSeconds)	\
    (This)->lpVtbl -> put_DialogAutoCloseTime(This,uSeconds)

#define ICSO_put_INIPathname(This,bstrPathname)	\
    (This)->lpVtbl -> put_INIPathname(This,bstrPathname)

#define ICSO_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)

#define ICSO_put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)	\
    (This)->lpVtbl -> put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)

#define ICSO_put_ThresholdValue(This,uThresholdValue)	\
    (This)->lpVtbl -> put_ThresholdValue(This,uThresholdValue)

#define ICSO_RegisterISO(This,uReceiverHandle,uNode,eCategory,vbOOB,pvbResult)	\
    (This)->lpVtbl -> RegisterISO(This,uReceiverHandle,uNode,eCategory,vbOOB,pvbResult)

#define ICSO_ResetCSO(This)	\
    (This)->lpVtbl -> ResetCSO(This)

#define ICSO_SendHeartbeat(This)	\
    (This)->lpVtbl -> SendHeartbeat(This)

#define ICSO_SendToWatch(This,bstrDisplayString,uNode)	\
    (This)->lpVtbl -> SendToWatch(This,bstrDisplayString,uNode)

#define ICSO_SetUpPort(This,bstrPortName,pvbResult)	\
    (This)->lpVtbl -> SetUpPort(This,bstrPortName,pvbResult)

#define ICSO_ShutdownCSO(This)	\
    (This)->lpVtbl -> ShutdownCSO(This)

#define ICSO_StartCSO(This)	\
    (This)->lpVtbl -> StartCSO(This)

#define ICSO_Transmit(This,uLength,blobData,uNode,vbQuiet,puBytesWritten)	\
    (This)->lpVtbl -> Transmit(This,uLength,blobData,uNode,vbQuiet,puBytesWritten)

#define ICSO_UnRegisterISO(This,uNode,eCategory)	\
    (This)->lpVtbl -> UnRegisterISO(This,uNode,eCategory)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_CollectParametersForNewCSO_Proxy( 
    ICSO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewCSO);


void __RPC_STUB ICSO_CollectParametersForNewCSO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_EnableHeartbeat_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vbCommand);


void __RPC_STUB ICSO_EnableHeartbeat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_get_CSOTypeString_Proxy( 
    ICSO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrTypeString);


void __RPC_STUB ICSO_get_CSOTypeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_get_LastError_Proxy( 
    ICSO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrErrorString);


void __RPC_STUB ICSO_get_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_get_Name_Proxy( 
    ICSO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB ICSO_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_get_VersionString_Proxy( 
    ICSO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrVersionString);


void __RPC_STUB ICSO_get_VersionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_put_DialogAutoCloseTime_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG uSeconds);


void __RPC_STUB ICSO_put_DialogAutoCloseTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_put_INIPathname_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ BSTR bstrPathname);


void __RPC_STUB ICSO_put_INIPathname_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_put_Name_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB ICSO_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_put_WindowsRegisteredMessageStrings_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG ulRECEIVE,
    /* [in] */ ULONG ulRECEIVE_OOB,
    /* [in] */ ULONG ulSTART,
    /* [in] */ ULONG ulSTOP,
    /* [in] */ ULONG ulRESET);


void __RPC_STUB ICSO_put_WindowsRegisteredMessageStrings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_put_ThresholdValue_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG uThresholdValue);


void __RPC_STUB ICSO_put_ThresholdValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_RegisterISO_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG uReceiverHandle,
    /* [in] */ ULONG uNode,
    /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory,
    /* [in] */ VARIANT_BOOL vbOOB,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult);


void __RPC_STUB ICSO_RegisterISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_ResetCSO_Proxy( 
    ICSO __RPC_FAR * This);


void __RPC_STUB ICSO_ResetCSO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_SendHeartbeat_Proxy( 
    ICSO __RPC_FAR * This);


void __RPC_STUB ICSO_SendHeartbeat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_SendToWatch_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ BSTR bstrDisplayString,
    /* [in] */ ULONG uNode);


void __RPC_STUB ICSO_SendToWatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_SetUpPort_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ BSTR bstrPortName,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbResult);


void __RPC_STUB ICSO_SetUpPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_ShutdownCSO_Proxy( 
    ICSO __RPC_FAR * This);


void __RPC_STUB ICSO_ShutdownCSO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_StartCSO_Proxy( 
    ICSO __RPC_FAR * This);


void __RPC_STUB ICSO_StartCSO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_Transmit_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG uLength,
    /* [in] */ BLOB blobData,
    /* [in] */ ULONG uNode,
    /* [in] */ VARIANT_BOOL vbQuiet,
    /* [out] */ ULONG __RPC_FAR *puBytesWritten);


void __RPC_STUB ICSO_Transmit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICSO_UnRegisterISO_Proxy( 
    ICSO __RPC_FAR * This,
    /* [in] */ ULONG uNode,
    /* [in] */ ISO_FUNCTIONAL_CATEGORY eCategory);


void __RPC_STUB ICSO_UnRegisterISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICSO_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
