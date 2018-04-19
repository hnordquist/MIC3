/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:13:17 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\Interfaces\ICB.idl:
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

#ifndef __ICB_h__
#define __ICB_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICB_FWD_DEFINED__
#define __ICB_FWD_DEFINED__
typedef interface ICB ICB;
#endif 	/* __ICB_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICB_INTERFACE_DEFINED__
#define __ICB_INTERFACE_DEFINED__

/* interface ICB */
/* [uuid][object] */ 


EXTERN_C const IID IID_ICB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE6E7E5C-77FA-433F-AE7E-8684CBA460F2")
    ICB : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_FaceColor( 
            /* [out] */ ULONG __RPC_FAR *pRGBvalue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Numeric( 
            /* [out] */ ULONG __RPC_FAR *pNumeric) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_DisplayNumericString( 
            /* [in] */ VARIANT_BOOL vb,
            /* [in] */ ULONG uRadix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_FaceColor( 
            /* [in] */ ULONG RGBcolor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_Flash( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_Numeric( 
            /* [in] */ ULONG newValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_PreNumericString( 
            /* [in] */ BSTR string) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDiskIcon_Floppy( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDiskIcon_Hourglass( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetErrorIcon_Bomb( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetErrorIcon_Flag( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetErrorIcon_Skull( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMICObjectHandle( 
            /* [in] */ ULONG handle) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Bell( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Clock( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Disk( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Error( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Frown( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowIcon_Square( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_VersionString( 
            /* [out] */ BSTR __RPC_FAR *pbstrVersionString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICB __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICB __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICB __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICB __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICB __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICB __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICB __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FaceColor )( 
            ICB __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pRGBvalue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Numeric )( 
            ICB __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pNumeric);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DisplayNumericString )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb,
            /* [in] */ ULONG uRadix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FaceColor )( 
            ICB __RPC_FAR * This,
            /* [in] */ ULONG RGBcolor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Flash )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Numeric )( 
            ICB __RPC_FAR * This,
            /* [in] */ ULONG newValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PreNumericString )( 
            ICB __RPC_FAR * This,
            /* [in] */ BSTR string);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDiskIcon_Floppy )( 
            ICB __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDiskIcon_Hourglass )( 
            ICB __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetErrorIcon_Bomb )( 
            ICB __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetErrorIcon_Flag )( 
            ICB __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetErrorIcon_Skull )( 
            ICB __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMICObjectHandle )( 
            ICB __RPC_FAR * This,
            /* [in] */ ULONG handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Bell )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Clock )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Disk )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Error )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Frown )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowIcon_Square )( 
            ICB __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VersionString )( 
            ICB __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrVersionString);
        
        END_INTERFACE
    } ICBVtbl;

    interface ICB
    {
        CONST_VTBL struct ICBVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICB_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICB_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICB_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICB_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICB_get_FaceColor(This,pRGBvalue)	\
    (This)->lpVtbl -> get_FaceColor(This,pRGBvalue)

#define ICB_get_Numeric(This,pNumeric)	\
    (This)->lpVtbl -> get_Numeric(This,pNumeric)

#define ICB_put_DisplayNumericString(This,vb,uRadix)	\
    (This)->lpVtbl -> put_DisplayNumericString(This,vb,uRadix)

#define ICB_put_FaceColor(This,RGBcolor)	\
    (This)->lpVtbl -> put_FaceColor(This,RGBcolor)

#define ICB_put_Flash(This,vb)	\
    (This)->lpVtbl -> put_Flash(This,vb)

#define ICB_put_Numeric(This,newValue)	\
    (This)->lpVtbl -> put_Numeric(This,newValue)

#define ICB_put_PreNumericString(This,string)	\
    (This)->lpVtbl -> put_PreNumericString(This,string)

#define ICB_SetDiskIcon_Floppy(This)	\
    (This)->lpVtbl -> SetDiskIcon_Floppy(This)

#define ICB_SetDiskIcon_Hourglass(This)	\
    (This)->lpVtbl -> SetDiskIcon_Hourglass(This)

#define ICB_SetErrorIcon_Bomb(This)	\
    (This)->lpVtbl -> SetErrorIcon_Bomb(This)

#define ICB_SetErrorIcon_Flag(This)	\
    (This)->lpVtbl -> SetErrorIcon_Flag(This)

#define ICB_SetErrorIcon_Skull(This)	\
    (This)->lpVtbl -> SetErrorIcon_Skull(This)

#define ICB_SetMICObjectHandle(This,handle)	\
    (This)->lpVtbl -> SetMICObjectHandle(This,handle)

#define ICB_ShowIcon_Bell(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Bell(This,vb)

#define ICB_ShowIcon_Clock(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Clock(This,vb)

#define ICB_ShowIcon_Disk(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Disk(This,vb)

#define ICB_ShowIcon_Error(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Error(This,vb)

#define ICB_ShowIcon_Frown(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Frown(This,vb)

#define ICB_ShowIcon_Square(This,vb)	\
    (This)->lpVtbl -> ShowIcon_Square(This,vb)

#define ICB_get_VersionString(This,pbstrVersionString)	\
    (This)->lpVtbl -> get_VersionString(This,pbstrVersionString)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_get_FaceColor_Proxy( 
    ICB __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pRGBvalue);


void __RPC_STUB ICB_get_FaceColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_get_Numeric_Proxy( 
    ICB __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pNumeric);


void __RPC_STUB ICB_get_Numeric_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_put_DisplayNumericString_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb,
    /* [in] */ ULONG uRadix);


void __RPC_STUB ICB_put_DisplayNumericString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_put_FaceColor_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ ULONG RGBcolor);


void __RPC_STUB ICB_put_FaceColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_put_Flash_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_put_Flash_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_put_Numeric_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ ULONG newValue);


void __RPC_STUB ICB_put_Numeric_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_put_PreNumericString_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ BSTR string);


void __RPC_STUB ICB_put_PreNumericString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetDiskIcon_Floppy_Proxy( 
    ICB __RPC_FAR * This);


void __RPC_STUB ICB_SetDiskIcon_Floppy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetDiskIcon_Hourglass_Proxy( 
    ICB __RPC_FAR * This);


void __RPC_STUB ICB_SetDiskIcon_Hourglass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetErrorIcon_Bomb_Proxy( 
    ICB __RPC_FAR * This);


void __RPC_STUB ICB_SetErrorIcon_Bomb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetErrorIcon_Flag_Proxy( 
    ICB __RPC_FAR * This);


void __RPC_STUB ICB_SetErrorIcon_Flag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetErrorIcon_Skull_Proxy( 
    ICB __RPC_FAR * This);


void __RPC_STUB ICB_SetErrorIcon_Skull_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_SetMICObjectHandle_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ ULONG handle);


void __RPC_STUB ICB_SetMICObjectHandle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Bell_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Bell_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Clock_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Clock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Disk_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Disk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Error_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Error_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Frown_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Frown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_ShowIcon_Square_Proxy( 
    ICB __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB ICB_ShowIcon_Square_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICB_get_VersionString_Proxy( 
    ICB __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrVersionString);


void __RPC_STUB ICB_get_VersionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICB_INTERFACE_DEFINED__ */


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
