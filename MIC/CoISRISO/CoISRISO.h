

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 20:14:07 2038
 */
/* Compiler settings for CoISRISO.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CoISRISO_h__
#define __CoISRISO_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IISO_FWD_DEFINED__
#define __IISO_FWD_DEFINED__
typedef interface IISO IISO;

#endif 	/* __IISO_FWD_DEFINED__ */


#ifndef __ISRISO_FWD_DEFINED__
#define __ISRISO_FWD_DEFINED__

#ifdef __cplusplus
typedef class ISRISO ISRISO;
#else
typedef struct ISRISO ISRISO;
#endif /* __cplusplus */

#endif 	/* __ISRISO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IISO_INTERFACE_DEFINED__
#define __IISO_INTERFACE_DEFINED__

/* interface IISO */
/* [uuid][object] */ 


EXTERN_C const IID IID_IISO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1AD5FF6-B607-4454-9E7A-035235D49591")
    IISO : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AllowWriteToDisk( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AppendGUIValues( 
            /* [in] */ BSTR bstrFilename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CollectParametersForNewISO( 
            /* [out] */ BSTR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL *pvbHaveNewISO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateISO( 
            /* [out] */ VARIANT_BOOL *pvbSuccess) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DriveBBMToZero( 
            /* [in] */ VARIANT_BOOL vbDriveToZero,
            /* [in] */ VARIANT_BOOL vbInPowerFail) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EstablishButtonKeeperCommunicationsPath( 
            /* [in] */ ULONG ulButtonKeeperHandle,
            /* [in] */ ULONG ulCheckButtonRegisteredMessageValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EstablishSpecialUPSCommunicationsPath( 
            /* [in] */ ULONG ulSendToHandle,
            /* [in] */ ULONG ulQueryAllPausedRegisteredMessageValue,
            /* [in] */ ULONG ulDriveAllToZeroRegisteredMessageValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EstablishValidationCommunicationsPath( 
            /* [in] */ ULONG ulValidationHandle,
            /* [in] */ ULONG ulValidationRegisteredMessageValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_CurrentBBM( 
            /* [out] */ ULONG *pCurrentBBM) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [out] */ BSTR *pbstrID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_InShutDown( 
            /* [out] */ VARIANT_BOOL *pvbInShutDown) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_InStartUp( 
            /* [out] */ VARIANT_BOOL *pvbInStartUp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ISOCategory( 
            /* [out] */ ULONG *pCategory) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ISOTypeString( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_IsPaused( 
            /* [out] */ VARIANT_BOOL *pvbIsPaused) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [out] */ BSTR *pbstrFullName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [out] */ BSTR *pbstrErrorString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LocationAndExtensionsForDataFiles( 
            /* [out] */ BSTR *pbstrLocation,
            /* [out] */ BSTR *pbstrExtensions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_VersionString( 
            /* [out] */ BSTR *pbstrVersion) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PauseInstrument( 
            /* [in] */ VARIANT_BOOL vbPauseCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_DialogAutoCloseTime( 
            /* [in] */ ULONG seconds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ICBPointer( 
            /* [in] */ IUnknown *pICB) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ICSOPointer( 
            /* [in] */ IUnknown *pICSO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_INIPathname( 
            /* [in] */ BSTR bstrINIPathname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_LongNameFormatString( 
            /* [in] */ BSTR bstrFormatString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_MaximumRetryRepetitions( 
            /* [in] */ ULONG uNumberRepetitions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_NormalShutdown( 
            /* [in] */ VARIANT_BOOL vbNormalShutdown) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_UTILPathname( 
            /* [in] */ BSTR bstrUTILPathname) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_WindowsRegisteredMessageStrings( 
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetISO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowISOGUI( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShutdownISO( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UPSGoDumb( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UseShortFilename( 
            /* [in] */ VARIANT_BOOL vb) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IISOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IISO * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IISO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IISO * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IISO * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IISO * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IISO * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IISO * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AllowWriteToDisk )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AppendGUIValues )( 
            IISO * This,
            /* [in] */ BSTR bstrFilename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CollectParametersForNewISO )( 
            IISO * This,
            /* [out] */ BSTR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL *pvbHaveNewISO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateISO )( 
            IISO * This,
            /* [out] */ VARIANT_BOOL *pvbSuccess);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DriveBBMToZero )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vbDriveToZero,
            /* [in] */ VARIANT_BOOL vbInPowerFail);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EstablishButtonKeeperCommunicationsPath )( 
            IISO * This,
            /* [in] */ ULONG ulButtonKeeperHandle,
            /* [in] */ ULONG ulCheckButtonRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EstablishSpecialUPSCommunicationsPath )( 
            IISO * This,
            /* [in] */ ULONG ulSendToHandle,
            /* [in] */ ULONG ulQueryAllPausedRegisteredMessageValue,
            /* [in] */ ULONG ulDriveAllToZeroRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EstablishValidationCommunicationsPath )( 
            IISO * This,
            /* [in] */ ULONG ulValidationHandle,
            /* [in] */ ULONG ulValidationRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentBBM )( 
            IISO * This,
            /* [out] */ ULONG *pCurrentBBM);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IISO * This,
            /* [out] */ BSTR *pbstrID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_InShutDown )( 
            IISO * This,
            /* [out] */ VARIANT_BOOL *pvbInShutDown);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_InStartUp )( 
            IISO * This,
            /* [out] */ VARIANT_BOOL *pvbInStartUp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_ISOCategory )( 
            IISO * This,
            /* [out] */ ULONG *pCategory);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_ISOTypeString )( 
            IISO * This,
            /* [out] */ BSTR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_IsPaused )( 
            IISO * This,
            /* [out] */ VARIANT_BOOL *pvbIsPaused);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullName )( 
            IISO * This,
            /* [out] */ BSTR *pbstrFullName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_LastError )( 
            IISO * This,
            /* [out] */ BSTR *pbstrErrorString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationAndExtensionsForDataFiles )( 
            IISO * This,
            /* [out] */ BSTR *pbstrLocation,
            /* [out] */ BSTR *pbstrExtensions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IISO * This,
            /* [out] */ BSTR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *get_VersionString )( 
            IISO * This,
            /* [out] */ BSTR *pbstrVersion);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PauseInstrument )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vbPauseCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_DialogAutoCloseTime )( 
            IISO * This,
            /* [in] */ ULONG seconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_ICBPointer )( 
            IISO * This,
            /* [in] */ IUnknown *pICB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_ICSOPointer )( 
            IISO * This,
            /* [in] */ IUnknown *pICSO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_INIPathname )( 
            IISO * This,
            /* [in] */ BSTR bstrINIPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_LongNameFormatString )( 
            IISO * This,
            /* [in] */ BSTR bstrFormatString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_MaximumRetryRepetitions )( 
            IISO * This,
            /* [in] */ ULONG uNumberRepetitions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            IISO * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_NormalShutdown )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vbNormalShutdown);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_UTILPathname )( 
            IISO * This,
            /* [in] */ BSTR bstrUTILPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *put_WindowsRegisteredMessageStrings )( 
            IISO * This,
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetISO )( 
            IISO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowISOGUI )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShutdownISO )( 
            IISO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UPSGoDumb )( 
            IISO * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UseShortFilename )( 
            IISO * This,
            /* [in] */ VARIANT_BOOL vb);
        
        END_INTERFACE
    } IISOVtbl;

    interface IISO
    {
        CONST_VTBL struct IISOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IISO_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IISO_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IISO_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IISO_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IISO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IISO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IISO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IISO_AllowWriteToDisk(This,vb)	\
    ( (This)->lpVtbl -> AllowWriteToDisk(This,vb) ) 

#define IISO_AppendGUIValues(This,bstrFilename)	\
    ( (This)->lpVtbl -> AppendGUIValues(This,bstrFilename) ) 

#define IISO_CollectParametersForNewISO(This,pbstrNameOut,pvbHaveNewISO)	\
    ( (This)->lpVtbl -> CollectParametersForNewISO(This,pbstrNameOut,pvbHaveNewISO) ) 

#define IISO_CreateISO(This,pvbSuccess)	\
    ( (This)->lpVtbl -> CreateISO(This,pvbSuccess) ) 

#define IISO_DriveBBMToZero(This,vbDriveToZero,vbInPowerFail)	\
    ( (This)->lpVtbl -> DriveBBMToZero(This,vbDriveToZero,vbInPowerFail) ) 

#define IISO_EstablishButtonKeeperCommunicationsPath(This,ulButtonKeeperHandle,ulCheckButtonRegisteredMessageValue)	\
    ( (This)->lpVtbl -> EstablishButtonKeeperCommunicationsPath(This,ulButtonKeeperHandle,ulCheckButtonRegisteredMessageValue) ) 

#define IISO_EstablishSpecialUPSCommunicationsPath(This,ulSendToHandle,ulQueryAllPausedRegisteredMessageValue,ulDriveAllToZeroRegisteredMessageValue)	\
    ( (This)->lpVtbl -> EstablishSpecialUPSCommunicationsPath(This,ulSendToHandle,ulQueryAllPausedRegisteredMessageValue,ulDriveAllToZeroRegisteredMessageValue) ) 

#define IISO_EstablishValidationCommunicationsPath(This,ulValidationHandle,ulValidationRegisteredMessageValue)	\
    ( (This)->lpVtbl -> EstablishValidationCommunicationsPath(This,ulValidationHandle,ulValidationRegisteredMessageValue) ) 

#define IISO_get_CurrentBBM(This,pCurrentBBM)	\
    ( (This)->lpVtbl -> get_CurrentBBM(This,pCurrentBBM) ) 

#define IISO_get_ID(This,pbstrID)	\
    ( (This)->lpVtbl -> get_ID(This,pbstrID) ) 

#define IISO_get_InShutDown(This,pvbInShutDown)	\
    ( (This)->lpVtbl -> get_InShutDown(This,pvbInShutDown) ) 

#define IISO_get_InStartUp(This,pvbInStartUp)	\
    ( (This)->lpVtbl -> get_InStartUp(This,pvbInStartUp) ) 

#define IISO_get_ISOCategory(This,pCategory)	\
    ( (This)->lpVtbl -> get_ISOCategory(This,pCategory) ) 

#define IISO_get_ISOTypeString(This,pbstrName)	\
    ( (This)->lpVtbl -> get_ISOTypeString(This,pbstrName) ) 

#define IISO_get_IsPaused(This,pvbIsPaused)	\
    ( (This)->lpVtbl -> get_IsPaused(This,pvbIsPaused) ) 

#define IISO_get_FullName(This,pbstrFullName)	\
    ( (This)->lpVtbl -> get_FullName(This,pbstrFullName) ) 

#define IISO_get_LastError(This,pbstrErrorString)	\
    ( (This)->lpVtbl -> get_LastError(This,pbstrErrorString) ) 

#define IISO_get_LocationAndExtensionsForDataFiles(This,pbstrLocation,pbstrExtensions)	\
    ( (This)->lpVtbl -> get_LocationAndExtensionsForDataFiles(This,pbstrLocation,pbstrExtensions) ) 

#define IISO_get_Name(This,pbstrName)	\
    ( (This)->lpVtbl -> get_Name(This,pbstrName) ) 

#define IISO_get_VersionString(This,pbstrVersion)	\
    ( (This)->lpVtbl -> get_VersionString(This,pbstrVersion) ) 

#define IISO_PauseInstrument(This,vbPauseCommand)	\
    ( (This)->lpVtbl -> PauseInstrument(This,vbPauseCommand) ) 

#define IISO_put_DialogAutoCloseTime(This,seconds)	\
    ( (This)->lpVtbl -> put_DialogAutoCloseTime(This,seconds) ) 

#define IISO_put_ICBPointer(This,pICB)	\
    ( (This)->lpVtbl -> put_ICBPointer(This,pICB) ) 

#define IISO_put_ICSOPointer(This,pICSO)	\
    ( (This)->lpVtbl -> put_ICSOPointer(This,pICSO) ) 

#define IISO_put_INIPathname(This,bstrINIPathname)	\
    ( (This)->lpVtbl -> put_INIPathname(This,bstrINIPathname) ) 

#define IISO_put_LongNameFormatString(This,bstrFormatString)	\
    ( (This)->lpVtbl -> put_LongNameFormatString(This,bstrFormatString) ) 

#define IISO_put_MaximumRetryRepetitions(This,uNumberRepetitions)	\
    ( (This)->lpVtbl -> put_MaximumRetryRepetitions(This,uNumberRepetitions) ) 

#define IISO_put_Name(This,bstrName)	\
    ( (This)->lpVtbl -> put_Name(This,bstrName) ) 

#define IISO_put_NormalShutdown(This,vbNormalShutdown)	\
    ( (This)->lpVtbl -> put_NormalShutdown(This,vbNormalShutdown) ) 

#define IISO_put_UTILPathname(This,bstrUTILPathname)	\
    ( (This)->lpVtbl -> put_UTILPathname(This,bstrUTILPathname) ) 

#define IISO_put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)	\
    ( (This)->lpVtbl -> put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET) ) 

#define IISO_ResetISO(This)	\
    ( (This)->lpVtbl -> ResetISO(This) ) 

#define IISO_ShowISOGUI(This,vb)	\
    ( (This)->lpVtbl -> ShowISOGUI(This,vb) ) 

#define IISO_ShutdownISO(This)	\
    ( (This)->lpVtbl -> ShutdownISO(This) ) 

#define IISO_UPSGoDumb(This)	\
    ( (This)->lpVtbl -> UPSGoDumb(This) ) 

#define IISO_UseShortFilename(This,vb)	\
    ( (This)->lpVtbl -> UseShortFilename(This,vb) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IISO_INTERFACE_DEFINED__ */



#ifndef __COISRISOLib_LIBRARY_DEFINED__
#define __COISRISOLib_LIBRARY_DEFINED__

/* library COISRISOLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_COISRISOLib;

EXTERN_C const CLSID CLSID_ISRISO;

#ifdef __cplusplus

class DECLSPEC_UUID("86D16BA1-E8DD-41EC-854B-1B032281BDA0")
ISRISO;
#endif
#endif /* __COISRISOLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  BSTR_UserSize64(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal64(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree64(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


