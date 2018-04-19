/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Nov 27 12:13:17 2007
 */
/* Compiler settings for C:\Program Files\Microsoft Visual Studio\MyProjects\MIC 2004\Interfaces\IISO.idl:
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

#ifndef __IISO_h__
#define __IISO_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IISO_FWD_DEFINED__
#define __IISO_FWD_DEFINED__
typedef interface IISO IISO;
#endif 	/* __IISO_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

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
            /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewISO) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateISO( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbSuccess) = 0;
        
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
            /* [out] */ ULONG __RPC_FAR *pCurrentBBM) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [out] */ BSTR __RPC_FAR *pbstrID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_InShutDown( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInShutDown) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_InStartUp( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInStartUp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ISOCategory( 
            /* [out] */ ULONG __RPC_FAR *pCategory) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ISOTypeString( 
            /* [out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_IsPaused( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbIsPaused) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [out] */ BSTR __RPC_FAR *pbstrFullName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LastError( 
            /* [out] */ BSTR __RPC_FAR *pbstrErrorString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_LocationAndExtensionsForDataFiles( 
            /* [out] */ BSTR __RPC_FAR *pbstrLocation,
            /* [out] */ BSTR __RPC_FAR *pbstrExtensions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE get_VersionString( 
            /* [out] */ BSTR __RPC_FAR *pbstrVersion) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PauseInstrument( 
            /* [in] */ VARIANT_BOOL vbPauseCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_DialogAutoCloseTime( 
            /* [in] */ ULONG seconds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ICBPointer( 
            /* [in] */ IUnknown __RPC_FAR *pICB) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ICSOPointer( 
            /* [in] */ IUnknown __RPC_FAR *pICSO) = 0;
        
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
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IISO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IISO __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IISO __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IISO __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IISO __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IISO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IISO __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AllowWriteToDisk )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AppendGUIValues )( 
            IISO __RPC_FAR * This,
            /* [in] */ BSTR bstrFilename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CollectParametersForNewISO )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewISO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateISO )( 
            IISO __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbSuccess);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DriveBBMToZero )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vbDriveToZero,
            /* [in] */ VARIANT_BOOL vbInPowerFail);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EstablishButtonKeeperCommunicationsPath )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG ulButtonKeeperHandle,
            /* [in] */ ULONG ulCheckButtonRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EstablishSpecialUPSCommunicationsPath )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG ulSendToHandle,
            /* [in] */ ULONG ulQueryAllPausedRegisteredMessageValue,
            /* [in] */ ULONG ulDriveAllToZeroRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EstablishValidationCommunicationsPath )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG ulValidationHandle,
            /* [in] */ ULONG ulValidationRegisteredMessageValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentBBM )( 
            IISO __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pCurrentBBM);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ID )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InShutDown )( 
            IISO __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInShutDown);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InStartUp )( 
            IISO __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInStartUp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ISOCategory )( 
            IISO __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pCategory);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ISOTypeString )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsPaused )( 
            IISO __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pvbIsPaused);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrFullName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastError )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrErrorString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LocationAndExtensionsForDataFiles )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrLocation,
            /* [out] */ BSTR __RPC_FAR *pbstrExtensions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VersionString )( 
            IISO __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pbstrVersion);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PauseInstrument )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vbPauseCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DialogAutoCloseTime )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG seconds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ICBPointer )( 
            IISO __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pICB);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ICSOPointer )( 
            IISO __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pICSO);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_INIPathname )( 
            IISO __RPC_FAR * This,
            /* [in] */ BSTR bstrINIPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LongNameFormatString )( 
            IISO __RPC_FAR * This,
            /* [in] */ BSTR bstrFormatString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaximumRetryRepetitions )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG uNumberRepetitions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IISO __RPC_FAR * This,
            /* [in] */ BSTR bstrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NormalShutdown )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vbNormalShutdown);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UTILPathname )( 
            IISO __RPC_FAR * This,
            /* [in] */ BSTR bstrUTILPathname);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowsRegisteredMessageStrings )( 
            IISO __RPC_FAR * This,
            /* [in] */ ULONG ulRECEIVE,
            /* [in] */ ULONG ulRECEIVE_OOB,
            /* [in] */ ULONG ulSTART,
            /* [in] */ ULONG ulSTOP,
            /* [in] */ ULONG ulRESET);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetISO )( 
            IISO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowISOGUI )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownISO )( 
            IISO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UPSGoDumb )( 
            IISO __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UseShortFilename )( 
            IISO __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL vb);
        
        END_INTERFACE
    } IISOVtbl;

    interface IISO
    {
        CONST_VTBL struct IISOVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IISO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IISO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IISO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IISO_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IISO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IISO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IISO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IISO_AllowWriteToDisk(This,vb)	\
    (This)->lpVtbl -> AllowWriteToDisk(This,vb)

#define IISO_AppendGUIValues(This,bstrFilename)	\
    (This)->lpVtbl -> AppendGUIValues(This,bstrFilename)

#define IISO_CollectParametersForNewISO(This,pbstrNameOut,pvbHaveNewISO)	\
    (This)->lpVtbl -> CollectParametersForNewISO(This,pbstrNameOut,pvbHaveNewISO)

#define IISO_CreateISO(This,pvbSuccess)	\
    (This)->lpVtbl -> CreateISO(This,pvbSuccess)

#define IISO_DriveBBMToZero(This,vbDriveToZero,vbInPowerFail)	\
    (This)->lpVtbl -> DriveBBMToZero(This,vbDriveToZero,vbInPowerFail)

#define IISO_EstablishButtonKeeperCommunicationsPath(This,ulButtonKeeperHandle,ulCheckButtonRegisteredMessageValue)	\
    (This)->lpVtbl -> EstablishButtonKeeperCommunicationsPath(This,ulButtonKeeperHandle,ulCheckButtonRegisteredMessageValue)

#define IISO_EstablishSpecialUPSCommunicationsPath(This,ulSendToHandle,ulQueryAllPausedRegisteredMessageValue,ulDriveAllToZeroRegisteredMessageValue)	\
    (This)->lpVtbl -> EstablishSpecialUPSCommunicationsPath(This,ulSendToHandle,ulQueryAllPausedRegisteredMessageValue,ulDriveAllToZeroRegisteredMessageValue)

#define IISO_EstablishValidationCommunicationsPath(This,ulValidationHandle,ulValidationRegisteredMessageValue)	\
    (This)->lpVtbl -> EstablishValidationCommunicationsPath(This,ulValidationHandle,ulValidationRegisteredMessageValue)

#define IISO_get_CurrentBBM(This,pCurrentBBM)	\
    (This)->lpVtbl -> get_CurrentBBM(This,pCurrentBBM)

#define IISO_get_ID(This,pbstrID)	\
    (This)->lpVtbl -> get_ID(This,pbstrID)

#define IISO_get_InShutDown(This,pvbInShutDown)	\
    (This)->lpVtbl -> get_InShutDown(This,pvbInShutDown)

#define IISO_get_InStartUp(This,pvbInStartUp)	\
    (This)->lpVtbl -> get_InStartUp(This,pvbInStartUp)

#define IISO_get_ISOCategory(This,pCategory)	\
    (This)->lpVtbl -> get_ISOCategory(This,pCategory)

#define IISO_get_ISOTypeString(This,pbstrName)	\
    (This)->lpVtbl -> get_ISOTypeString(This,pbstrName)

#define IISO_get_IsPaused(This,pvbIsPaused)	\
    (This)->lpVtbl -> get_IsPaused(This,pvbIsPaused)

#define IISO_get_FullName(This,pbstrFullName)	\
    (This)->lpVtbl -> get_FullName(This,pbstrFullName)

#define IISO_get_LastError(This,pbstrErrorString)	\
    (This)->lpVtbl -> get_LastError(This,pbstrErrorString)

#define IISO_get_LocationAndExtensionsForDataFiles(This,pbstrLocation,pbstrExtensions)	\
    (This)->lpVtbl -> get_LocationAndExtensionsForDataFiles(This,pbstrLocation,pbstrExtensions)

#define IISO_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IISO_get_VersionString(This,pbstrVersion)	\
    (This)->lpVtbl -> get_VersionString(This,pbstrVersion)

#define IISO_PauseInstrument(This,vbPauseCommand)	\
    (This)->lpVtbl -> PauseInstrument(This,vbPauseCommand)

#define IISO_put_DialogAutoCloseTime(This,seconds)	\
    (This)->lpVtbl -> put_DialogAutoCloseTime(This,seconds)

#define IISO_put_ICBPointer(This,pICB)	\
    (This)->lpVtbl -> put_ICBPointer(This,pICB)

#define IISO_put_ICSOPointer(This,pICSO)	\
    (This)->lpVtbl -> put_ICSOPointer(This,pICSO)

#define IISO_put_INIPathname(This,bstrINIPathname)	\
    (This)->lpVtbl -> put_INIPathname(This,bstrINIPathname)

#define IISO_put_LongNameFormatString(This,bstrFormatString)	\
    (This)->lpVtbl -> put_LongNameFormatString(This,bstrFormatString)

#define IISO_put_MaximumRetryRepetitions(This,uNumberRepetitions)	\
    (This)->lpVtbl -> put_MaximumRetryRepetitions(This,uNumberRepetitions)

#define IISO_put_Name(This,bstrName)	\
    (This)->lpVtbl -> put_Name(This,bstrName)

#define IISO_put_NormalShutdown(This,vbNormalShutdown)	\
    (This)->lpVtbl -> put_NormalShutdown(This,vbNormalShutdown)

#define IISO_put_UTILPathname(This,bstrUTILPathname)	\
    (This)->lpVtbl -> put_UTILPathname(This,bstrUTILPathname)

#define IISO_put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)	\
    (This)->lpVtbl -> put_WindowsRegisteredMessageStrings(This,ulRECEIVE,ulRECEIVE_OOB,ulSTART,ulSTOP,ulRESET)

#define IISO_ResetISO(This)	\
    (This)->lpVtbl -> ResetISO(This)

#define IISO_ShowISOGUI(This,vb)	\
    (This)->lpVtbl -> ShowISOGUI(This,vb)

#define IISO_ShutdownISO(This)	\
    (This)->lpVtbl -> ShutdownISO(This)

#define IISO_UPSGoDumb(This)	\
    (This)->lpVtbl -> UPSGoDumb(This)

#define IISO_UseShortFilename(This,vb)	\
    (This)->lpVtbl -> UseShortFilename(This,vb)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_AllowWriteToDisk_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB IISO_AllowWriteToDisk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_AppendGUIValues_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ BSTR bstrFilename);


void __RPC_STUB IISO_AppendGUIValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_CollectParametersForNewISO_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrNameOut,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbHaveNewISO);


void __RPC_STUB IISO_CollectParametersForNewISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_CreateISO_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbSuccess);


void __RPC_STUB IISO_CreateISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_DriveBBMToZero_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vbDriveToZero,
    /* [in] */ VARIANT_BOOL vbInPowerFail);


void __RPC_STUB IISO_DriveBBMToZero_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_EstablishButtonKeeperCommunicationsPath_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG ulButtonKeeperHandle,
    /* [in] */ ULONG ulCheckButtonRegisteredMessageValue);


void __RPC_STUB IISO_EstablishButtonKeeperCommunicationsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_EstablishSpecialUPSCommunicationsPath_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG ulSendToHandle,
    /* [in] */ ULONG ulQueryAllPausedRegisteredMessageValue,
    /* [in] */ ULONG ulDriveAllToZeroRegisteredMessageValue);


void __RPC_STUB IISO_EstablishSpecialUPSCommunicationsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_EstablishValidationCommunicationsPath_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG ulValidationHandle,
    /* [in] */ ULONG ulValidationRegisteredMessageValue);


void __RPC_STUB IISO_EstablishValidationCommunicationsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_CurrentBBM_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pCurrentBBM);


void __RPC_STUB IISO_get_CurrentBBM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_ID_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrID);


void __RPC_STUB IISO_get_ID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_InShutDown_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInShutDown);


void __RPC_STUB IISO_get_InShutDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_InStartUp_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbInStartUp);


void __RPC_STUB IISO_get_InStartUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_ISOCategory_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pCategory);


void __RPC_STUB IISO_get_ISOCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_ISOTypeString_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IISO_get_ISOTypeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_IsPaused_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pvbIsPaused);


void __RPC_STUB IISO_get_IsPaused_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_FullName_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrFullName);


void __RPC_STUB IISO_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_LastError_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrErrorString);


void __RPC_STUB IISO_get_LastError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_LocationAndExtensionsForDataFiles_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrLocation,
    /* [out] */ BSTR __RPC_FAR *pbstrExtensions);


void __RPC_STUB IISO_get_LocationAndExtensionsForDataFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_Name_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IISO_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_get_VersionString_Proxy( 
    IISO __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pbstrVersion);


void __RPC_STUB IISO_get_VersionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_PauseInstrument_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vbPauseCommand);


void __RPC_STUB IISO_PauseInstrument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_DialogAutoCloseTime_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG seconds);


void __RPC_STUB IISO_put_DialogAutoCloseTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_ICBPointer_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pICB);


void __RPC_STUB IISO_put_ICBPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_ICSOPointer_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pICSO);


void __RPC_STUB IISO_put_ICSOPointer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_INIPathname_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ BSTR bstrINIPathname);


void __RPC_STUB IISO_put_INIPathname_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_LongNameFormatString_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ BSTR bstrFormatString);


void __RPC_STUB IISO_put_LongNameFormatString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_MaximumRetryRepetitions_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG uNumberRepetitions);


void __RPC_STUB IISO_put_MaximumRetryRepetitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_Name_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ BSTR bstrName);


void __RPC_STUB IISO_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_NormalShutdown_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vbNormalShutdown);


void __RPC_STUB IISO_put_NormalShutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_UTILPathname_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ BSTR bstrUTILPathname);


void __RPC_STUB IISO_put_UTILPathname_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_put_WindowsRegisteredMessageStrings_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ ULONG ulRECEIVE,
    /* [in] */ ULONG ulRECEIVE_OOB,
    /* [in] */ ULONG ulSTART,
    /* [in] */ ULONG ulSTOP,
    /* [in] */ ULONG ulRESET);


void __RPC_STUB IISO_put_WindowsRegisteredMessageStrings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_ResetISO_Proxy( 
    IISO __RPC_FAR * This);


void __RPC_STUB IISO_ResetISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_ShowISOGUI_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB IISO_ShowISOGUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_ShutdownISO_Proxy( 
    IISO __RPC_FAR * This);


void __RPC_STUB IISO_ShutdownISO_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_UPSGoDumb_Proxy( 
    IISO __RPC_FAR * This);


void __RPC_STUB IISO_UPSGoDumb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IISO_UseShortFilename_Proxy( 
    IISO __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL vb);


void __RPC_STUB IISO_UseShortFilename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IISO_INTERFACE_DEFINED__ */


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
