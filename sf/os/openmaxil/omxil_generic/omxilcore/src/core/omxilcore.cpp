// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
#include <ecom/ecom.h>
#include <string.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <uri8.h>
#include <e32des8.h>

#include "log.h"
#include "omxilcore.h"
#include "omxiluids.hrh"
#include <openmax/il/core/omxilloaderif.h>
#include <openmax/il/core/omxilloaderif.hrh>
#include <openmax/il/core/omxilcontentpipeif.h>
#include <openmax/il/core/omxilcontentpipeif.hrh>

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
#define ENABLE_OMXIL_TRACING
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL
#define ENABLE_OMXIL_TRACING
#endif


#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
#include "..\..\traces\OstTraceDefinitions.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "OmxilcoreTraces.h"
#endif

#endif

#ifdef ENABLE_OMXIL_TRACING
#include "OmxilComponentAPIPerformanceTrace.h"
#endif



#ifdef _DEBUG
// This used only in debug build, for now...
_LIT(KOmxILCorePanic, "OmxILCore Panic");
#endif

const TInt KLoadersGranularity = 1;
const TInt KContentPipesGranularity = 1;

const TUint8 COmxILCore::iSpecVersionMajor;
const TUint8 COmxILCore::iSpecVersionMinor;
const TUint8 COmxILCore::iSpecVersionRevision;
const TUint8 COmxILCore::iSpecVersionStep;


COmxILCore::COmxILCore()
	:
	iLoaders(KLoadersGranularity),
	iContentPipes(KContentPipesGranularity)
	{
    DEBUG_PRINTF(_L8("COmxILCore::COmxILCore"));
	}

COmxILCore::~COmxILCore()
	{
    DEBUG_PRINTF(_L8("COmxILCore::~COmxILCore"));
	}

OMX_ERRORTYPE COmxILCore::ListLoaders()
	{
    DEBUG_PRINTF(_L8("COmxILCore::ListLoaders"));

	RImplInfoPtrArray ecomArray;
	OMX_ERRORTYPE omxError = OMX_ErrorNone;
	TInt error = KErrNone;
	TRAP(error, REComSession::ListImplementationsL(
			  TUid::Uid(KUidOmxILLoaderInterface),ecomArray));

	if(KErrNone == error)
		{
		const TInt ecomCount = ecomArray.Count();
		const TInt loadersCount = iLoaders.Count();

		// We only consider a situation where loader plugins are added to the
		// system...
		if(ecomCount > loadersCount)
			{
			TOmxLoaderInfo loader;
			CImplementationInformation* info;
			TInt indexLoaders;
			for(TInt index=0; index<ecomCount; ++index)
				{
				info = ecomArray[index];
				for( indexLoaders=0;
					 (indexLoaders<loadersCount)
						 &&(info->ImplementationUid()!=
							iLoaders[indexLoaders].iUid);
					 ++indexLoaders )
					{}

				if(indexLoaders>=loadersCount)
					{
					loader.iUid = info->ImplementationUid();
					loader.iLoader = NULL;

					// We load loader plugins here, instead of "on-demand"...
					omxError = LoadAndInitLoader(loader.iUid,
												 loader.iLoader);
					if (OMX_ErrorNone == omxError)
						{
						error = iLoaders.Append(loader);
						if(KErrNone != error)
							{
							__ASSERT_DEBUG(loader.iLoader != NULL,
										   User::Panic(KOmxILCorePanic, 1));
							OMX_LOADERTYPE* omx_loader =
								reinterpret_cast<OMX_LOADERTYPE*>(loader.iLoader->Handle());
							__ASSERT_DEBUG(omx_loader != NULL,
										   User::Panic(KOmxILCorePanic, 1));
							omx_loader->UnInitializeComponentLoader(omx_loader);
							delete loader.iLoader;
							break;
							}
						}
					else
						{
						DEBUG_PRINTF3(_L8("COmxILCore::ListLoaders : Loader [%d] returned OMX Error [%d]"), index, omxError);
						// Check whether there's any point on continuing with
						// the next loader...
						if (OMX_ErrorInsufficientResources == omxError)
							{
							error=KErrNoMemory;
							break;
							}
						else
							{
							DEBUG_PRINTF3(_L8("COmxILCore::ListLoaders : Loader [%d] returned OMX Error [%d]  Ignoring error code as it's no considered critical!"), index, omxError);
							omxError=OMX_ErrorNone; //ignore any non-critical errors
							}
						}
					}
				}
			}

		}

	if (KErrNone != error)
		{
		if (KErrNoMemory == error)
			{
			omxError = OMX_ErrorInsufficientResources;
			}
		else
			{
			// We should return an OMX error that at least is allowed by the
			// spec during OMX_Init()
			omxError = OMX_ErrorTimeout;
			}
		}

	ecomArray.ResetAndDestroy();
	return omxError;
	}

TBool VerifyComponentName(const OMX_STRING aComponentName)
	{
	const TInt KComponentNamePrefixLen = 4;
	return strncmp(aComponentName,"OMX.", KComponentNamePrefixLen) == 0;
	}
EXPORT_C OMX_ERRORTYPE COmxILCore::LoadComponent(
	const OMX_STRING aComponentName,
	OMX_HANDLETYPE* aHandle,
	OMX_PTR aAppData,
	OMX_CALLBACKTYPE* aCallBacks)
	{
	OMX_CALLBACKTYPE* pLocCallBacks = aCallBacks;
	OMX_ERRORTYPE error = OMX_ErrorNone;
	DEBUG_PRINTF(_L8("COmxILCore::LoadComponent"));	

#ifdef ENABLE_OMXIL_TRACING
	OMX_HANDLETYPE hClientHandle ;
	TBool IsPrepareTraceStructuresLFailure=EFalse;
	COMPONENT_REF* pCompRefToClient; 
	COMPONENT_REF* pCompRefToTrace;
#endif
 
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _LoadComponent1, "OMX_GetHandle >" );
#endif 
	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetHandle)); 
#endif
    
    
    if(error ==  OMX_ErrorNone)
        {
        if(!aHandle || !aComponentName)
            {
            error = OMX_ErrorBadParameter;
            }
        else
            {
            if( !VerifyComponentName(aComponentName))
                {
                error = OMX_ErrorInvalidComponentName;
                }
            else
                {
#ifdef ENABLE_OMXIL_TRACING 
                TRAPD(Err, OMXILAPITraceWrapper::PrepareTraceStructuresL(hClientHandle, pLocCallBacks, pCompRefToClient, pCompRefToTrace) );
                if(Err != KErrNone)
                    {
                    IsPrepareTraceStructuresLFailure = ETrue;
                    error = OMX_ErrorInsufficientResources;
                    }
#endif
                }
            }
        }

    if(error ==  OMX_ErrorNone)
        {
        error = OMX_ErrorComponentNotFound;
        OMX_LOADERTYPE* loader;
    
        const TInt count = iLoaders.Count();
        for( TInt index=0; index<count; ++index )
            {
            loader = reinterpret_cast<OMX_LOADERTYPE*>(
                iLoaders[index].iLoader->Handle());
            __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
            error = loader->LoadComponent(loader, aHandle,
                                          aComponentName,
                                          aAppData, pLocCallBacks);
            // if loading the component succeeded we can return else we disregard
            // the error on this loader and search for the component with the next
            // loader.  If this is the last loader the error will be returned at
            // the bottom of the function. We return also if there are no resoruces
            // available...
            if( OMX_ErrorNone == error || OMX_ErrorInsufficientResources == error)
                {
                break;
                }
            }
        }
#ifdef   ENABLE_OMXIL_TRACING
        if(error == OMX_ErrorNone)
            {  
            OMXILAPITraceWrapper::PatchStructures(hClientHandle, aCallBacks, *aHandle, pLocCallBacks, pCompRefToClient, pCompRefToTrace);
            *aHandle = hClientHandle;
            }
        else
            {
            if(!IsPrepareTraceStructuresLFailure)
                {
                // this means the component was not loaded, do cleanup
                OMXILAPITraceWrapper::UndoPrepareTraceStructures(hClientHandle, pLocCallBacks, pCompRefToClient, pCompRefToTrace);
                }
            }
#endif       
        
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<ADDITIONAL_INFO_SIZE> HandleInfomation;
    
    HandleInfomation.Format(Ost_OMXIL_Performance::K_HandleInfo, (aHandle)?(*aHandle):0, (!aHandle || !aComponentName)?"" :aComponentName);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetHandle), error ,&HandleInfomation); 
#endif

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    TPtrC8 ComponentInfo(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(aComponentName)));
    OstTraceExt5( TRACE_API, _LoadComponent2, "OMX_GetHandle < ReturnVal=%{OMX_ERRORTYPE} Component=%p aComponentName=%s pAppData=%p aCallBacks=%p", 
            (TUint)error, *aHandle, ComponentInfo, aAppData, aCallBacks  );

#endif    
    
	return error;
	}

OMX_ERRORTYPE COmxILCore::LoadAndInitLoader(
	const TUid& aUid, COmxComponentLoaderIf*& aLoader)
	{
    DEBUG_PRINTF(_L8("COmxILCore::LoadAndInitLoader"));

	COmxComponentLoaderIf* loader = NULL;
	TRAPD(err, loader = COmxComponentLoaderIf::CreateImplementationL(aUid));
	if(err != KErrNone)
		{
		if (err == KErrNoMemory)
			{
			return OMX_ErrorInsufficientResources;
			}
		return OMX_ErrorInvalidComponent;
		}

	OMX_HANDLETYPE handle = loader->Handle();
	if (!handle)
		{
		delete loader;
		return OMX_ErrorUndefined;
		}

	OMX_LOADERTYPE* omx_loader = reinterpret_cast<OMX_LOADERTYPE*>(handle);
	__ASSERT_DEBUG(omx_loader != NULL, User::Panic(KOmxILCorePanic, 1));
	OMX_ERRORTYPE error = omx_loader->InitializeComponentLoader(omx_loader);
	if(error != OMX_ErrorNone)
		{
		// We should call UnInitializeComponentLoader before deletion, just in
		// case...returned error is irrelevant now...
		omx_loader->UnInitializeComponentLoader(omx_loader);
		delete loader;
		return error;
		}

	aLoader = loader;
	return error;
	}

void COmxILCore::DeinitCore()
	{
    DEBUG_PRINTF(_L8("COmxILCore::DeinitCore"));

	TInt index;
	COmxComponentLoaderIf* loader;

	const TInt KLoaderCount = iLoaders.Count();
	for( index = 0; index<KLoaderCount; ++index )
		{
		loader = iLoaders[index].iLoader;
		// Check this in case the loader has not been initialized yet...
		if(loader)
			{
			OMX_HANDLETYPE handle = loader->Handle();
			OMX_LOADERTYPE* omx_loader =
				reinterpret_cast<OMX_LOADERTYPE*>(handle);
			__ASSERT_DEBUG(omx_loader != NULL,
						   User::Panic(KOmxILCorePanic, 1));
			OMX_ERRORTYPE error =
				omx_loader->UnInitializeComponentLoader(omx_loader);
			delete loader;
			}
		}
	iLoaders.Close();
	iContentPipes.ResetAndDestroy();
	}


EXPORT_C OMX_ERRORTYPE COmxILCore::ComponentNameEnum(
	OMX_STRING aComponentName,
	OMX_U32 aNameLength,
	const OMX_U32 aIndex)
	{
	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _ComponentNameEnum1, "OMX_ComponentNameEnum >" );
#endif 
	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_ComponentNameEnum));
#endif
    
    DEBUG_PRINTF(_L8("COmxILCore::ComponentNameEnum"));
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
	if( aNameLength == 0 || aComponentName == NULL )
		{
		error= OMX_ErrorBadParameter;
		}
	else
	    {
	    const TInt KLoadersCount = iLoaders.Count();
	    if (KLoadersCount == 0)
	        {
	        // There are no loaders in the system...
	        DEBUG_PRINTF2(_L8("COmxILCore::ComponentNameEnum (index = %d) : No loaders present..."), aIndex);
	        error = OMX_ErrorNoMore;
	        }
	    else
	        {
             const TInt KIndexForPopulateList = 0;
            // if aIndex is 0 (i.e. KIndexForPopulateList) the redescovery of
            // components is triggered in the client library
            if( aIndex == KIndexForPopulateList )
                {
                TBool found=EFalse;
                char tempComponentName[OMX_MAX_STRINGNAME_SIZE];
            
                // each loader should rediscover its components
                for( TInt index=0; index < KLoadersCount; ++index )
                    {
                    OMX_LOADERTYPE* loader = reinterpret_cast<OMX_LOADERTYPE*>(
                        iLoaders[index].iLoader->Handle());
                    __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
        
                    // Index 0 means two things according to the standard: core (through its loader(s)) has to discover components (populate component list) 
                    //  and also the component name at 0 index has to be returned 
                    // As there are multiple loaders present and there is no information which loader has any components
                    // we must go through all loaders inputting 0 index and see which loaders returns anything at index 0 first
                    // Once we've found the 0 index we can just ignore any component names returned from subsequent calls to loaders  
                    error = loader->ComponentNameEnum(loader,
                                                      tempComponentName,
                                                      aNameLength,aIndex);
                    if(OMX_ErrorNone==error)
                        {
                        if(!found)//component hasn't been found yet
                            {
                            strncpy(aComponentName, tempComponentName, aNameLength - 1);
                            found=ETrue; //now it's found
                            }
                        }
                    else
                        {
                        // We must continue with the next loader if the current loader
                        // has no components... or if there is some other error
                        // condition in the current loader...
                        if( OMX_ErrorNoMore == error )
                            {
                            DEBUG_PRINTF2(_L8("COmxILCore::ComponentNameEnum (index = 0) : Loader [%d] has 0 components..."), index);
                            }
                        else
                            {
                            // In case the loader returned with some other error condition,
                            // we log the situation...
                            DEBUG_PRINTF3(_L8("COmxILCore::ComponentNameEnum (index = 0) : Loader [%d] returned OMX error [%d].. ignoring loader..."), index, error);
                            // Check whether there's any point on continuing with the
                            // next loader...
                            if (OMX_ErrorInsufficientResources == error)
                                {
                                error = OMX_ErrorInsufficientResources;
                                break;
                                }
                            }
                        }
                    }
                error= (found ?  OMX_ErrorNone : OMX_ErrorNoMore);
                }
            else
                {
                RArray<TInt> componentCountPerLoader;
                TInt err=KErrNone;
        
                //init componentCountPerLoader array with -1; this array tells how many components a particular loader has  
                for(TInt loaderIndex=0; loaderIndex < KLoadersCount; ++loaderIndex)
                    {
                    //first check if loader is available
                    if( iLoaders[loaderIndex].iLoader == NULL )
                        {
                        // We are considering here that ComponentNameEnum with index =
                        // 0 must have been called previously... the spec is not
                        // completely clear here...We return ErrorNotReady although
                        // this is not a legal return code for this IL Core function...
                        componentCountPerLoader.Close();
                        error = OMX_ErrorNotReady;
                        break;
                        }
                    err=componentCountPerLoader.Append(-1);
                    switch(err)
                        {
                        case KErrNone:
                            //do nothing
                            error = OMX_ErrorNone;
                            break;   
                        case KErrNoMemory:
                            componentCountPerLoader.Close();
                            error = OMX_ErrorInsufficientResources;
                            break;
                        default:
                            componentCountPerLoader.Close();
                            error = OMX_ErrorUndefined;
                        }
                        if(error != OMX_ErrorNone)
                            {
                            break;
                            }   
                    }
                
                if(error == OMX_ErrorNone)
                    {    
                     for(TInt loaderIndex=0; loaderIndex < KLoadersCount; ++loaderIndex)
                        {
                        //fist acquire the actual loader
                        OMX_LOADERTYPE* loader = reinterpret_cast<OMX_LOADERTYPE*>(iLoaders[loaderIndex].iLoader->Handle());
                        __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
            
                        //then count how many components the previous loaders have in total
                        //obviously for the first loader this loop won't do anything (ie: for loaderIndex==0)
                        //for the second loader it will count the first loader's component count, etc....
                        TInt totalComponentCount(0);
                        for(TInt l=0; l<loaderIndex; ++l)
                            {
                            totalComponentCount+=componentCountPerLoader[l];
                            }
            
                        //then calculate the relative index, which is the index in the actual loader if the component
                        //was not found in previous loaders
                        //eg: if the query index is 8, and the first loader has 6 components, the second has 1 component, then
                        //the relative index is 8-(6+1)=1, that's we are querying the component at index 1 (which is in fact the second one as the indexing is zero based) of the 3rd loader
                        TInt relativeIndex = aIndex-totalComponentCount;
                        error = loader->ComponentNameEnum(loader,
                                                          aComponentName,
                                                          aNameLength, relativeIndex);
                        TBool IsReturn = ETrue;
                        switch(error)
                            {
                            case OMX_ErrorNone:
                                //this loader has component at this index (ie: at relativeIndex)
                                componentCountPerLoader.Close(); //free array before return
                                break;
                            
                            case OMX_ErrorNoMore:
                                if((KLoadersCount-1)==loaderIndex) //if this is the last loader then the component at this index index obviously does not exist
                                    {
                                    componentCountPerLoader.Close(); //free array before return
                                    break;
                                    }
                                //this loader has not this many components, so find out how many it actually has
                                //and then update the correponding item in the componentCountPerLoader array
                                if(0==relativeIndex)//if this loader has no component at index 0, then it has no components at all
                                    {
                                    componentCountPerLoader[loaderIndex]=0;
                                    IsReturn = EFalse;
                                    break;
                                    }
                                    
                                for(TInt m=relativeIndex-1; m>=0;--m) //try to figure the first available index (counting down)
                                    {
                                    OMX_ERRORTYPE tempError = loader->ComponentNameEnum(loader,
                                                                      aComponentName,
                                                                      aNameLength, m);
                                    if(OMX_ErrorNone==tempError)
                                        {
                                        //this is the first index (counting down) where this loader has component
                                        componentCountPerLoader[loaderIndex]=m+1; //eg: index==4, then component count is 5 (because of zero based indexing)
                                        IsReturn = EFalse; 
                                        break;
                                        }
                                    else if(OMX_ErrorInsufficientResources==tempError)
                                        {
                                        //critical error; free array before return
                                        componentCountPerLoader.Close();
                                        error = OMX_ErrorInsufficientResources;
                                        break;
                                        }
                                    //ignore any non-critical error (ie: other than OMX_ErrorInsufficientResources) and go one index down
                                    }
            
                                //if after the last loop (ie: m==0) the count was still not set for some reasons, then set to zero
                                if(!IsReturn  && (-1==componentCountPerLoader[loaderIndex] )) 
                                    {
                                    componentCountPerLoader[loaderIndex]=0;
                                    }
                                break;
                            case OMX_ErrorInsufficientResources:
                                componentCountPerLoader.Close(); //free array before return
                                break;
                            default:
                                //some unknown error happened to the loader therefore we can't count how many component it has, assume zero
                                DEBUG_PRINTF4(_L8("COmxILCore::ComponentNameEnum (index = %d) : Loader [%d] returned OMX error [%d].. ignoring loader (ie: assuming ZERO components)..."), aIndex, loaderIndex, error);
                                componentCountPerLoader[loaderIndex]=0;
                                IsReturn = EFalse;
                                break; 
                            }
                            
                        if(IsReturn)
                            {
                            break;
                            }
                        
                        }//end of for
                
                } // error == OMX_ErrorNone
                componentCountPerLoader.Close(); //free array before return
                } // else
	        }
	    }

	if (OMX_ErrorInsufficientResources == error)
		{
		error = OMX_ErrorUndefined;
		}
	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<1> Infomation;
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_ComponentNameEnum),error ,&Infomation); 
#endif
    

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    TPtrC8 componentNamePtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(aComponentName)));
    OstTraceExt4( TRACE_API, _ComponentNameEnum2, "OMX_ComponentNameEnum < ReturnVal=%{OMX_ERRORTYPE} aComponentName=%s aNameLength=%u aIndex=%u", 
            (TUint)error, componentNamePtr, (TUint)aNameLength, (TUint)aIndex  );
    
#endif 

    return error;
	}


EXPORT_C OMX_ERRORTYPE COmxILCore::FreeHandle(OMX_HANDLETYPE aComponent) const
	{    
	DEBUG_PRINTF2(_L8("COmxILCore::FreeHandle : Component Handle [%X]"), aComponent);

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _FreeHandle1, "OMX_FreeHandle >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_FreeHandle)); 
#endif
    OMX_ERRORTYPE error = OMX_ErrorNone;
	if( aComponent == NULL )
		{
		error = OMX_ErrorBadParameter;
		}
	else
	    {
	    
#ifdef  ENABLE_OMXIL_TRACING
    // resolve the handle to be passed to the component
    OMX_HANDLETYPE ComponentTemp = OMXILAPITraceWrapper::ResolveCallsToComponent(aComponent);
    // delete the temporatry reference objects
    OMXILAPITraceWrapper::ComponentTracerDestroy(aComponent);
    aComponent = ComponentTemp;
#endif
	    OMX_LOADERTYPE* loader;
        error = OMX_ErrorComponentNotFound;
    
        const TInt KLoadersCount = iLoaders.Count();
        for(TInt index=0; index < KLoadersCount; ++index)
            {
            // Check this just in case the loader has not been initialized yet...
            if(iLoaders[index].iLoader != NULL)
                {
                loader = reinterpret_cast<OMX_LOADERTYPE*>(iLoaders[index].iLoader->Handle());
                __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
                error = loader->UnloadComponent(loader, aComponent);
                if( OMX_ErrorNone == error )
                    {
                    // Success case... otherwise, continue with next loader...
                    break;
                    }
    #ifdef _DEBUG
                if( OMX_ErrorComponentNotFound != error )
                    {
                    // Another error occured... log the situation...
                    DEBUG_PRINTF3(_L8("COmxILCore::FreeHandle : Loader [%d] returned OMX error [%d].. continuing with next loader..."), index, error);
                    }
    #endif
                }
            }
	    }


#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt2( TRACE_API, _FreeHandle2, "OMX_FreeHandle < ReturnVal=%x Component=%p", 
            (TUint)error, aComponent  );
#endif 

#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<ADDITIONAL_INFO_SIZE> FreeHandleInfo;
    FreeHandleInfo.Format(Ost_OMXIL_Performance::K_HandleOnlyInfo , aComponent);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_FreeHandle), error,&FreeHandleInfo); 
#endif
	
    
    return error;
	}

EXPORT_C OMX_ERRORTYPE COmxILCore::SetupTunnel(OMX_HANDLETYPE aHandleOutput,
									  OMX_U32 aPortOutput,
									  OMX_HANDLETYPE aHandleInput,
									  OMX_U32 aPortInput)
	{
    DEBUG_PRINTF(_L8("COmxILCore::SetupTunnel"));

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _SetupTunnel1, "OMX_SetupTunnel >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_SetupTunnel));
#endif
    OMX_ERRORTYPE error = OMX_ErrorNone;
	if (!aHandleOutput && !aHandleInput )
		{
		error= OMX_ErrorBadParameter;
		}
	else
	    {
        OMX_COMPONENTTYPE *componentInput, *componentOutput;
        componentInput = reinterpret_cast<OMX_COMPONENTTYPE*>(aHandleInput);
        componentOutput = reinterpret_cast<OMX_COMPONENTTYPE*>(aHandleOutput);
    
        OMX_TUNNELSETUPTYPE tunnelSetup;
        OMX_PARAM_PORTDEFINITIONTYPE portDefinitionType;
        
        tunnelSetup.nTunnelFlags = 0;
        tunnelSetup.eSupplier = OMX_BufferSupplyUnspecified;
       
        if(componentOutput)
            {
            // Check that the output port is actually an output port
            portDefinitionType.nSize					= sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
            portDefinitionType.nVersion.s.nVersionMajor = COmxILCore::iSpecVersionMajor;
            portDefinitionType.nVersion.s.nVersionMinor = COmxILCore::iSpecVersionMinor;
            portDefinitionType.nVersion.s.nRevision     = COmxILCore::iSpecVersionRevision;
            portDefinitionType.nVersion.s.nStep         = COmxILCore::iSpecVersionStep;
            portDefinitionType.nPortIndex				= aPortOutput;
            if (OMX_ErrorNone != (error = componentOutput->GetParameter(
                                      componentOutput,
                                      OMX_IndexParamPortDefinition,
                                      &portDefinitionType)))
                {
                error = TOmxILCoreUtils::ConvertErrorGetParameterToSetupTunnel(error);
                }
            else
                {
                if (OMX_DirOutput != portDefinitionType.eDir)
                    {
                    error = OMX_ErrorBadParameter;
                    }
                else
                    {
                    error = componentOutput->ComponentTunnelRequest(componentOutput,
                                                            aPortOutput,
                                                            componentInput,
                                                            aPortInput,
                                                            &tunnelSetup);
                    
                    }
                }
    

            }
            
        	if(error == OMX_ErrorNone && componentInput)
        	    {
                // Check that the input port is actually an input port. Set again the
                // structure values just in case they were overwritten in the previous
                // GetParameter call...
                portDefinitionType.nSize					= sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
                portDefinitionType.nVersion.s.nVersionMajor = COmxILCore::iSpecVersionMajor;
                portDefinitionType.nVersion.s.nVersionMinor = COmxILCore::iSpecVersionMinor;
                portDefinitionType.nVersion.s.nRevision     = COmxILCore::iSpecVersionRevision;
                portDefinitionType.nVersion.s.nStep         = COmxILCore::iSpecVersionStep;
                portDefinitionType.nPortIndex				= aPortInput;
                if (OMX_ErrorNone != (error = componentInput->GetParameter(
                                          componentInput,
                                          OMX_IndexParamPortDefinition,
                                          &portDefinitionType)))
                    {
                    error = TOmxILCoreUtils::ConvertErrorGetParameterToSetupTunnel(error);
                    }
                else
                    {
                    if (OMX_DirInput != portDefinitionType.eDir)
                        {
                        error= OMX_ErrorBadParameter;
                        }
                    else
                        {
                        error = componentInput->ComponentTunnelRequest(componentInput,
                                                               aPortInput,
                                                               componentOutput,
                                                               aPortOutput,
                                                               &tunnelSetup);
                        
                        }
                    
                    if(OMX_ErrorNone != error)
                        {
                        if(componentOutput)
                            {
                            if( OMX_ErrorNone != componentOutput->ComponentTunnelRequest(
                                    componentOutput,
                                    aPortOutput,
                                    NULL,
                                    0,
                                    &tunnelSetup) )
                                {
                                error = OMX_ErrorUndefined;
                                }
                            }
                        }
                    }
              }
	    }

	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    TBuf8<ADDITIONAL_INFO_SIZE> TunnelInfomation;
    TunnelInfomation.Format(Ost_OMXIL_Performance::K_TunnelSetupInfo, aHandleOutput, aPortOutput, aHandleInput, aPortInput);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_SetupTunnel), error  ,&TunnelInfomation);
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTraceExt5( TRACE_API, _SetupTunnel2, "OMX_SetupTunnel < ReturnVal=%{OMX_ERRORTYPE} aHandleOutput=%p aPortOutput=%u aHandleInput=%p aPortInput=%u", 
            (TUint)error, aHandleOutput, (TUint) aPortOutput, aHandleInput, (TUint)aPortInput );
#endif     
    return error;
	}

EXPORT_C OMX_ERRORTYPE COmxILCore::GetContentPipe(
    OMX_HANDLETYPE *aPipe,const OMX_STRING aURI)
    {
    DEBUG_PRINTF(_L8("COmxILCore::GetContentPipe"));

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetContentPipe1, "OMX_GetContentPipe >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetContentPipe));
#endif
    
    OMX_ERRORTYPE omxError = OMX_ErrorNone;
    if( aURI == NULL )
        {
        omxError = OMX_ErrorBadParameter;
        }
    else
        {
        RImplInfoPtrArray ecomArray;
        TRAPD(error,REComSession::ListImplementationsL(
                  TUid::Uid(KUidOmxILContentPipeIf), ecomArray));
        if( error != KErrNone )
            {
            ecomArray.ResetAndDestroy();
            omxError= OMX_ErrorUndefined;
            }
        else
            {
            const TInt KEcomArrayCount = ecomArray.Count();
            if(!KEcomArrayCount)
                {
                ecomArray.Close();
                omxError= OMX_ErrorBadParameter;
                }
            else
                {
                HBufC8* convert = NULL;
                TPtr8 luri(reinterpret_cast<TUint8 *>(aURI), strlen(aURI), strlen(aURI));
            
                TRAPD(error, (convert = HBufC8::NewL(strlen(aURI))));
                if( error != KErrNone )
                    {
                    ecomArray.ResetAndDestroy();
                    omxError = OMX_ErrorUndefined;
                    }
                else
                    {
                    TUriParser8 parser;// Uri parser object
                    TInt result;
                    // Extract the scheme component from the parsed URI
                    const TDesC8& scheme = parser.Extract(EUriScheme);
                
                    *convert = luri;
                    result = parser.Parse(*convert);// parse the Uri descriptor
                    if( result != KErrNone )
                        {
                        ecomArray.ResetAndDestroy();
                        omxError = OMX_ErrorBadParameter;
                        }
                    else
                        {
                        TInt index;
                        CImplementationInformation* info;
                        COmxILContentPipeIf* cpipe = NULL;
                        omxError = OMX_ErrorBadParameter;
                        for(index=0; index < KEcomArrayCount; ++index)
                            {
                            info = ecomArray[index];
                            if(info->DataType().Find(scheme) != KErrNotFound )
                                {
                                //found
                                TRAPD(error, cpipe = COmxILContentPipeIf::CreateImplementationL(
                                         info->ImplementationUid()));
                                if(error != KErrNone)
                                    {
                                    omxError = OMX_ErrorUndefined;
                                    break;
                                    }
                                error = cpipe->GetHandle(aPipe);
                                if(error != KErrNone)
                                    {
                                    omxError = OMX_ErrorUndefined;
                                    break;
                                    }
                    
                                error = iContentPipes.Append(cpipe);
                                if(error != KErrNone)
                                    {
                                    omxError = OMX_ErrorUndefined;
                                    break;
                                    }
                    
                                omxError = OMX_ErrorNone;
                                break;
                                }
                            }

                            if (OMX_ErrorNone != omxError)
                                {
                                delete cpipe;
                                aPipe = NULL;
                                }
                        }
                    }
                    // 
                    delete convert;  
                }
            }
        ecomArray.ResetAndDestroy();
        }

#ifdef SYMBIAN_PERF_TRACE_OMX_IL    
    TBuf8<ADDITIONAL_INFO_SIZE> HandleInfomation;
    HandleInfomation.Format(Ost_OMXIL_Performance::K_HandleInfo, ( (aPipe)? *aPipe :0 ), aURI);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetContentPipe), omxError ,&HandleInfomation); 
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    TPtrC8 URIPtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(aURI)));
    OstTraceExt3( TRACE_API, _GetContentPipe2, "OMX_GetContentPipe < ReturnVal=%{OMX_ERRORTYPE} aPipe=%p, aURI=%s", 
            (TUint)omxError, (aPipe)? *aPipe :0 , URIPtr );
    
#endif     
    return omxError;
    }

EXPORT_C OMX_ERRORTYPE COmxILCore::GetComponentsOfRole (
	const OMX_STRING aRole, OMX_U32 *aNumComps, OMX_U8  **aCompNames)
	{
    DEBUG_PRINTF(_L8("COmxILCore::GetComponentsOfRole"));

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetComponentsOfRole1, "OMX_GetComponentsOfRole >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetComponentsOfRole));
#endif
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
	if (aNumComps == NULL)
		{
		error =  OMX_ErrorBadParameter;
		}
	else
	    {
        if (aCompNames != NULL && *aNumComps == 0)
            {
            error = OMX_ErrorBadParameter;
            }
        else
            {
            OMX_LOADERTYPE* loader;
            error = OMX_ErrorComponentNotFound;
            OMX_U32 numAllComps = 0;
            OMX_U32 maxNumComps = *aNumComps;
            OMX_U32 tempNumComps = maxNumComps;
            OMX_U8 **compNames = aCompNames;
        
            const TInt KLoadersCount = iLoaders.Count();
            for(TInt index=0; index < KLoadersCount; ++index)
                {
                loader = reinterpret_cast<OMX_LOADERTYPE*>(
                    iLoaders[index].iLoader->Handle());
                __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
                error = loader->GetComponentsOfRole(loader,
                                                    aRole,
                                                    &tempNumComps,
                                                    compNames);
                if(OMX_ErrorNone == error)
                    {
                    numAllComps += tempNumComps;
                    if(aCompNames != NULL && KLoadersCount > 1)
                        {
                        // Update tempNumComps in case there is a next iteration...
                        tempNumComps = maxNumComps - tempNumComps;
                        if (numAllComps >= maxNumComps)
                            {
                            if (numAllComps > maxNumComps)
                                {
                                DEBUG_PRINTF2(_L8("COmxILCore::GetComponentsOfRole : Loader [%d] returned a bad number of component roles... correcting"), index);
                                numAllComps = maxNumComps;
                                }
        
                            // The array of component names is full...
                            break;
                            }
        
                        compNames = &(aCompNames[numAllComps]);
                        if (*compNames == NULL)
                            {
                            *aNumComps = 0;
                            error = OMX_ErrorBadParameter;
                            break;
                            }
                        } //  if(aCompNames != NULL && KLoadersCount > 1)
                    } // if(OMX_ErrorNone == error)
                } // for(TInt index=0; index < KLoadersCount; ++index)
        
            // Update the number of components value to be returned to the client...
            *aNumComps = numAllComps;
            }
	    }
	if (error == OMX_ErrorInsufficientResources)
		{
		error =  OMX_ErrorUndefined;
		}

	
#ifdef SYMBIAN_PERF_TRACE_OMX_IL    
    TBuf8<ADDITIONAL_INFO_SIZE> Infomation;
    Infomation.Format(Ost_OMXIL_Performance::K_RoleInfo, aRole, (aNumComps)?*aNumComps:0);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetComponentsOfRole), error ,&Infomation); 
#endif
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1    
    TPtrC8 RolePtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(aRole)));
    OstTraceExt3( TRACE_API, _GetComponentsOfRole2, "OMX_GetComponentsOfRole < ReturnVal=%{OMX_ERRORTYPE} aRoleInfo=%s aNumComps=%u", 
            (TUint)error, RolePtr, (TUint)((aNumComps)?*aNumComps:0));

#endif     
    
    return error;
	}

EXPORT_C OMX_ERRORTYPE COmxILCore::GetRolesOfComponent (
	const OMX_STRING aCompName, OMX_U32 *aNumRoles, OMX_U8 **aRoles)
	{
    DEBUG_PRINTF(_L8("COmxILCore::GetRolesOfComponent"));
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1
    OstTrace0( TRACE_API, _GetRolesOfComponent1, "OMX_GetRolesOfComponent >" );
#endif 
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormatMin,
            Ost_OMXIL_Performance::EMeasurementStart, MAKESTRING(E_GetRolesOfComponent));
#endif
    
    OMX_ERRORTYPE error = OMX_ErrorNone;
    
	if (aNumRoles == NULL)
		{
		error = OMX_ErrorBadParameter;
		}
	else
	    {
	    if (aRoles != NULL && *aNumRoles == 0)
	       {
	       error = OMX_ErrorBadParameter;
	       }
	    else
	        {
	        if( !VerifyComponentName(aCompName))
                {
                error = OMX_ErrorInvalidComponentName;
                }
	        else
	            {
                 TInt numRolesCopy = *aNumRoles;
	             OMX_LOADERTYPE* loader;
	             error = OMX_ErrorComponentNotFound;
	             const TInt KLoadersCount = iLoaders.Count();
	             for(TInt index=0; index < KLoadersCount; ++index)
	                 {
	                 loader = reinterpret_cast<OMX_LOADERTYPE*>(
	                         iLoaders[index].iLoader->Handle());
	                 __ASSERT_DEBUG(loader != NULL, User::Panic(KOmxILCorePanic, 1));
	                 error = loader->GetRolesOfComponent(loader,
	                                                     aCompName,
	                                                     aNumRoles,
	                                                     aRoles);
	                 if(OMX_ErrorNone == error)
	                     {
	                     if ((aRoles != NULL) && (*aNumRoles > numRolesCopy))
	                         {
	                         // Just in case...
	                         DEBUG_PRINTF2(_L8("COmxILCore::GetRolesOfComponent : Loader [%d] returned a bad number of roles... correcting"), index);
	                         *aNumRoles =  numRolesCopy;
	                         }
	                     break;
	                     }
	         
	                 if((index+1 < KLoadersCount) && (aRoles != NULL))
	                     {
	                     // If we have other loaders after this, reset aNumRoles to the
	                     // value suplied by the client.
	                     *aNumRoles = numRolesCopy;
	                     }
	                 }
	            }
	        }
	    }
    
	// Make sure that even if loader does not conform to the set of errors required by the standard, we will
    if (KErrNone != error && (OMX_ErrorUndefined != error && OMX_ErrorInvalidComponentName != error && 
            OMX_ErrorInvalidComponent != error && OMX_ErrorBadParameter != error) )
        {
        error = OMX_ErrorUndefined;
        }
    
#ifdef SYMBIAN_PERF_TRACE_OMX_IL    
    TBuf8<ADDITIONAL_INFO_SIZE> Infomation;
    Infomation.Format(Ost_OMXIL_Performance::K_RoleInfo, aCompName, (aNumRoles)?*aNumRoles:0);
    OstPrintf(TTraceContext(KTracePerformanceTraceUID, EPerFormanceTraceClassification), Ost_OMXIL_Performance::K_OMX_PerformanceTraceFormat,
            Ost_OMXIL_Performance::EMeasurementEnd, MAKESTRING(E_GetRolesOfComponent), error ,&Infomation); 
#endif 

#ifdef SYMBIAN_PERF_TRACE_OMX_IL_OSTV1       
    TPtrC8 CompNamePtr(const_cast<const TUint8*>(reinterpret_cast<TUint8*>(aCompName)));
    OstTraceExt3( TRACE_API, _GetRolesOfComponent2, "OMX_GetRolesOfComponent < ReturnVal=%{OMX_ERRORTYPE} aCompName=%s aNumRoles=%u", 
            (TUint)error, CompNamePtr, (TUint)((aNumRoles)?*aNumRoles:0));
#endif 
    
    return error;
	}

/**
   Converts an error returned by GetParameter to a permitted error to be
   returned by OMX_SetupTunnel.

   @param aGetParameterError An error code returned by the GetParameter API

   @return An OMX_ERRORTYPE from the list of permitted errors in
   OMX_SetupTunnel
*/
OMX_ERRORTYPE TOmxILCoreUtils::ConvertErrorGetParameterToSetupTunnel(
	OMX_ERRORTYPE aGetParameterError)
	{
	switch(aGetParameterError)
		{
		// These are the common errors allowed in GetParameter and
		// OMX_SetupTunnel
	case OMX_ErrorInvalidComponent:
	case OMX_ErrorBadParameter:
	case OMX_ErrorInvalidState:
	case OMX_ErrorVersionMismatch:
	case OMX_ErrorTimeout:
	case OMX_ErrorBadPortIndex:
		return aGetParameterError;

		// These are the errors supported by GetParameter and not supported by
		// OMX_SetupTunnel
	case OMX_ErrorUndefined:
	case OMX_ErrorNoMore:
	case OMX_ErrorNotReady:
	case OMX_ErrorUnsupportedIndex:
	case OMX_ErrorSeperateTablesUsed:
	default:
		// Return something that is actually allowed...
		return OMX_ErrorBadParameter;
		};
	}
