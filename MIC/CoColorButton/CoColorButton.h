

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Mar 07 15:14:46 2019
 */
/* Compiler settings for CoColorButton.idl:
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

#ifndef __CoColorButton_h__
#define __CoColorButton_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICB_FWD_DEFINED__
#define __ICB_FWD_DEFINED__
typedef interface ICB ICB;
#endif 	/* __ICB_FWD_DEFINED__ */


#ifndef __ColorButton_FWD_DEFINED__
#define __ColorButton_FWD_DEFINED__

#ifdef __cplusplus
typedef class ColorButton ColorButton;
#else
typedef struct ColorButton ColorButton;
#endif /* __cplusplus */

#endif 	/* __ColorButton_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


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
            /* [out] */ ULONG *pRGBvalue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Numeric( 
            /* [out] */ ULONG *pNumeric) = 0;
        
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
            /* [out] */ BSTR *pbstrVersionString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICB * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICB * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICB * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICB * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICB * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICB * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_FaceColor )( 
            ICB * This,
            /* [out] */ ULONG *pRGBvalue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_Numeric )( 
            ICB * This,
            /* [out] */ ULONG *pNumeric);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_DisplayNumericString )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb,
            /* [in] */ ULONG uRadix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_FaceColor )( 
            ICB * This,
            /* [in] */ ULONG RGBcolor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_Flash )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_Numeric )( 
            ICB * This,
            /* [in] */ ULONG newValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_PreNumericString )( 
            ICB * This,
            /* [in] */ BSTR string);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDiskIcon_Floppy )( 
            ICB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetDiskIcon_Hourglass )( 
            ICB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetErrorIcon_Bomb )( 
            ICB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetErrorIcon_Flag )( 
            ICB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetErrorIcon_Skull )( 
            ICB * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMICObjectHandle )( 
            ICB * This,
            /* [in] */ ULONG handle);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Bell )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Clock )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Disk )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Error )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Frown )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowIcon_Square )( 
            ICB * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_VersionString )( 
            ICB * This,
            /* [out] */ BSTR *pbstrVersionString);
        
        END_INTERFACE
    } ICBVtbl;

    interface ICB
    {
        CONST_VTBL struct ICBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICB_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICB_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICB_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICB_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICB_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICB_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICB_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICB_get_FaceColor(This,pRGBvalue)	\
    ( (This)->lpVtbl -> get_FaceColor(This,pRGBvalue) ) 

#define ICB_get_Numeric(This,pNumeric)	\
    ( (This)->lpVtbl -> get_Numeric(This,pNumeric) ) 

#define ICB_put_DisplayNumericString(This,vb,uRadix)	\
    ( (This)->lpVtbl -> put_DisplayNumericString(This,vb,uRadix) ) 

#define ICB_put_FaceColor(This,RGBcolor)	\
    ( (This)->lpVtbl -> put_FaceColor(This,RGBcolor) ) 

#define ICB_put_Flash(This,vb)	\
    ( (This)->lpVtbl -> put_Flash(This,vb) ) 

#define ICB_put_Numeric(This,newValue)	\
    ( (This)->lpVtbl -> put_Numeric(This,newValue) ) 

#define ICB_put_PreNumericString(This,string)	\
    ( (This)->lpVtbl -> put_PreNumericString(This,string) ) 

#define ICB_SetDiskIcon_Floppy(This)	\
    ( (This)->lpVtbl -> SetDiskIcon_Floppy(This) ) 

#define ICB_SetDiskIcon_Hourglass(This)	\
    ( (This)->lpVtbl -> SetDiskIcon_Hourglass(This) ) 

#define ICB_SetErrorIcon_Bomb(This)	\
    ( (This)->lpVtbl -> SetErrorIcon_Bomb(This) ) 

#define ICB_SetErrorIcon_Flag(This)	\
    ( (This)->lpVtbl -> SetErrorIcon_Flag(This) ) 

#define ICB_SetErrorIcon_Skull(This)	\
    ( (This)->lpVtbl -> SetErrorIcon_Skull(This) ) 

#define ICB_SetMICObjectHandle(This,handle)	\
    ( (This)->lpVtbl -> SetMICObjectHandle(This,handle) ) 

#define ICB_ShowIcon_Bell(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Bell(This,vb) ) 

#define ICB_ShowIcon_Clock(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Clock(This,vb) ) 

#define ICB_ShowIcon_Disk(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Disk(This,vb) ) 

#define ICB_ShowIcon_Error(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Error(This,vb) ) 

#define ICB_ShowIcon_Frown(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Frown(This,vb) ) 

#define ICB_ShowIcon_Square(This,vb)	\
    ( (This)->lpVtbl -> ShowIcon_Square(This,vb) ) 

#define ICB_get_VersionString(This,pbstrVersionString)	\
    ( (This)->lpVtbl -> get_VersionString(This,pbstrVersionString) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICB_INTERFACE_DEFINED__ */



#ifndef __COCOLORBUTTONLib_LIBRARY_DEFINED__
#define __COCOLORBUTTONLib_LIBRARY_DEFINED__

/* library COCOLORBUTTONLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_COCOLORBUTTONLib;

EXTERN_C const CLSID CLSID_ColorButton;

#ifdef __cplusplus

class DECLSPEC_UUID("2B1A162B-1FBC-49EC-A9C7-5DE13A2E1061")
ColorButton;
#endif
#endif /* __COCOLORBUTTONLib_LIBRARY_DEFINED__ */

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


