/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "omxscripttest.h"
#include "paramconversion.h"
#include "statedes.h"

#include "omxilsymbianvideographicsinkextensions.h"
#include <openmax/il/shai/OMX_Symbian_IVCommonExt.h>
#include <graphics/surfaceconfiguration.h>
#include <hash.h>
#include "omxutil.h"
#include "nontunneledhandler.h"
#include "videobufferhandler.h"
#include "videobufferhandler_mpeg4.h" // for mpeg4
#include "videobuffersinkhandler.h"
#include "log.h"
#include "omx_xml_script.h"
#include "transition.h"
#include "parsemap.h"
#include <uri8.h>
#include "omxildroppedframeeventextension.h"
#include <c3gplibrary.h>
#include "baseprofilehandler.h"
#include "baseprofiletimestamping.h"
#include "windowmanager.h"
#ifdef OLD_ADPCM_EXTENSION
#include <openmax/il/extensions/omxilsymbianadpcmdecoderextensions.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// Temporary work-around for floating point issues on HREF 8500 ED hardware
// Using this macro, code that uses TReal will be #defined out if running on ED.
// TODO: Remove HREF_ED_WITHOUT_FLOATING_POINT and all of its conditional code
// once the floating point issue has been solved.
#if defined(NCP_COMMON_BRIDGE_FAMILY) && !defined(__WINSCW__)
#define HREF_ED_WITHOUT_FLOATING_POINT
#endif

DECL_PARSETYPE(OMX_EVENTTYPE);

_LIT8(KILTypeString, "LOGICAL");

static TInt StopScheduler(TAny* unused);

/**
 * Runs an OMX test specified in an XML file.
 * @param aScriptFilename path to XML file
 * @param aScriptSection section name in XML file
 */
void ROmxScriptTest::RunTestL(const TDesC& aScriptFilename, const TDesC& aScriptSection)
	{
	User::LeaveIfError(iEventHandler.Create());
	iStopSchedulerCallback = new(ELeave) CAsyncCallBack(StopScheduler, CActive::EPriorityHigh);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	INFO_PRINTF3(_L("Executing test steps in section %S of %S"), &aScriptSection, &aScriptFilename);
	
	TPtrC filename;
	COmxScriptParser* parser = COmxScriptParser::NewL(fs, aScriptFilename, *this);
	CleanupStack::PushL(parser);
	
	// this drives the methods from MOmxScriptIf
	TBool success = parser->RunScriptL(aScriptSection);
	if(success)
		{
		StopTest(EPass);
		}
	else
		{
		ERR_PRINTF1(_L("Script did not complete"));
		StopTest(EFail);
		}
	
	CleanupStack::PopAndDestroy(2, &fs);	// fs, parser
	
	User::LeaveIfError(Reason());
	}

/**
 * Constructs an object to parse and execute an OMX test case specified in
 * an XML file.
 * @param reference to logger interface to receive test messages
 */
ROmxScriptTest::ROmxScriptTest(MOmxScriptTestLogger& aLogger):
	iCoreLoaded(EFalse),
	iEventHandler(*this),
	iState(OMX_StateLoaded),
	iWindowManager(NULL),
	iVideoFitMode(EVideoFitRotateScaleAndCentre),
	iTimer(NULL),
	iWaitingForEvents(EFalse),
	iNonTunneledHandler(NULL),
	iBufferHandler(NULL),
	iBufferSinkHandler(NULL),
	iReason(KErrNone),
	iVerdict(EPending),
	iLogger(aLogger),
	iStopSchedulerCallback(NULL),
    iPortBufferHandler(NULL)
	{
	}

/**
 * Destroys all resources created by this object. 
 */
void ROmxScriptTest::Close()
	{
	// release memory and unload components
	// ~CComponentInfo unloads component in correct thread
	iComponents.ResetAndDestroy();
	
	if(iCoreLoaded)
		{
		INFO_PRINTF1(_L("OMX_Deinit()"));
		iCoreLoaded = EFalse;
		OMX_ERRORTYPE error = OMX_Deinit();
		if(error)
			{
			FailWithOmxError(_L("OMX_Deinit"), error);
			}
		}
	REComSession::FinalClose();
	iEventHandler.Close();
	delete iWindowManager;
	iWindowManager = NULL;
	iState = OMX_StateLoaded;
	
	if (iTimer)
		{
		iTimer->Cancel();
		delete iTimer;
		iTimer = NULL;
		}
	
	iExpectedEvents.Close();
	iIgnoredEvents.Close();
	
	if (iNonTunneledHandler)
		{
		delete iNonTunneledHandler;
		iNonTunneledHandler = NULL;
		}
	
   if(iPortBufferHandler)
       {
       delete iPortBufferHandler;
       iPortBufferHandler = NULL;
       }

   // Buffer component
	delete iBufferHandler;
	iBufferHandler = NULL;
	
	delete iBufferSinkHandler;
	iBufferSinkHandler = NULL;
	
	delete iStopSchedulerCallback;
	iStopSchedulerCallback = NULL;
	
	iTunnels.Close();
	}

// allows OMX components to be placed on the cleanup stack
//void ROmxScriptTest::CleanupOmxComponent(TAny* aPtr)
//	{
//	OMX_COMPONENTTYPE* component = static_cast<OMX_COMPONENTTYPE*>(aPtr);
//	OMX_ERRORTYPE error = OMX_FreeHandle(component);
//	// if this method is called, we are already in an error condition,
//	// but OMX_FreeHandle should still succeed. Perhaps a cleaner way of
//	// reporting any error is desired, but this is a static method so we
//	// lack the context to call the standard logging mechanism.
//	__ASSERT_DEBUG(!error, User::Invariant());
//	}

TBool ROmxScriptTest::MosLoadComponentL(const TDesC8& aComp, const TDesC8& aName, TBool aBaseProfile, const TDesC8* aBaseImpl, TBool aLoadInCoreServerThread)
	{
	if(!iCoreLoaded)
		{
		INFO_PRINTF1(_L("OMX_Init()"));
		OMX_ERRORTYPE error = OMX_Init();
		if(error)
			{
			CleanupStack::PopAndDestroy(2);
			FailWithOmxError(_L("OMX_Init()"), error);
			return EFalse;
			}
		iCoreLoaded = ETrue;
		}
	
	// convert from 8-bit descriptor for logging
	TBuf<64> nameCopy;
	nameCopy.Copy(aName);
	INFO_PRINTF2(_L("Loading component %S"), &nameCopy);

	CComponentInfo* componentInfo = CComponentInfo::NewL(*this);
	CleanupStack::PushL(componentInfo);
	componentInfo->iShortName = HBufC8::NewL(aComp.Length());
	*(componentInfo->iShortName) = aComp;
	// allow room for the '\0' used in call to OMX_GetHandle
	componentInfo->iComponentName = HBufC8::NewL(aName.Length() + 1);
	*(componentInfo->iComponentName) = aName;
	

	OMX_ERRORTYPE error = OMX_ErrorNone;
	if (aBaseProfile)
	    {
	    if (*aBaseImpl == _L8("base"))
	        {
	        componentInfo->iBaseHandler = new (ELeave) CBaseProfileHandler(*this, iEventHandler);
	        componentInfo->iComponent = componentInfo->iBaseHandler->LoadComponentL(*componentInfo->iShortName, *componentInfo->iComponentName);
	        
	        }
	    else if (*aBaseImpl == _L8("timestampcheck"))
            {
            componentInfo->iBaseHandler = new (ELeave) CBaseProfileTimestampHandling(*this, iEventHandler);
            componentInfo->iComponent = componentInfo->iBaseHandler->LoadComponentL(*componentInfo->iShortName, *componentInfo->iComponentName);
            
            }

	    }
	else
        {
        OMX_CALLBACKTYPE& callbacks = iEventHandler.CallbackStruct();
        // NOTE componentInfo.iShortName and componentInfo.iComponentName still on the cleanup stack
        
        if(aLoadInCoreServerThread)
            {
            INFO_PRINTF2(_L("Loading component %S in Core Server thread"), &nameCopy);
            if(componentInfo->iThreadRequest == NULL)
                {
                componentInfo->iThreadRequest = COmxThreadRequest::NewL();
                }
            error = componentInfo->iThreadRequest->GetHandle((TAny**) &(componentInfo->iComponent), (OMX_STRING) componentInfo->iComponentName->Des().PtrZ(), &iEventHandler, &callbacks);
            }
        else
            {
            INFO_PRINTF2(_L("Loading component %S in Client thread"), &nameCopy);
            error = OMX_GetHandle((TAny**) &(componentInfo->iComponent), (OMX_STRING) componentInfo->iComponentName->Des().PtrZ(), &iEventHandler, &callbacks);
            }
        }
	if(error || (componentInfo->iComponent == NULL))
		{
		CleanupStack::PopAndDestroy();	// delete component info
		FailWithOmxError(_L("OMX_GetHandle()"), error);
		return EFalse;
		}

	RDebug::Print(_L("OMX_GetHandle name=%S handle=0x%08X"), &nameCopy, componentInfo->iComponent);
	iEventHandler.AddComponentL(componentInfo->iComponent, nameCopy);
	iComponents.AppendL(componentInfo);
	CleanupStack::Pop(1, componentInfo);
	return ETrue;
	}

TBool ROmxScriptTest::MosSetupTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
                                     const TDesC8& aSinkComp, TInt aSinkPort, OMX_ERRORTYPE aExpectedError)
	{
	OMX_COMPONENTTYPE* source = NULL;
	OMX_COMPONENTTYPE* sink = NULL;
	if(aSourceComp!=KNullDesC8)
		{
		source = ComponentByName(aSourceComp);
		}
	
	if(aSinkComp!=KNullDesC8)
		{
		sink = ComponentByName(aSinkComp);
		}
	 
	//support for testing tunnel breakup
	if(source || sink)
		{
		OMX_ERRORTYPE error = OMX_SetupTunnel(source, aSourcePort, sink, aSinkPort);
		// TODO check tunnel reconfiguration
		if(error == OMX_ErrorNone)
			{
			RegisterTunnel(source, aSourcePort, sink, aSinkPort);
			}
		if(error != aExpectedError)
			{
			ERR_PRINTF3(_L("MosSetupTunnel OMX_SetupTunnel: Error 0x%X does not match expected error 0x%X"), error, aExpectedError);
			TBuf8<64> msg8;
			msg8.Format(_L8("source=%S:%d sink=%S:%d"), &aSourceComp, aSourcePort, &aSinkComp, aSinkPort);
			TBuf<64> msg;
			msg.Copy(msg8);
			INFO_PRINTF1(msg);
			return EFalse;
			}
		return ETrue;
		}
	else
		{
		// source or sink names not recognized
		return EFalse;
		}
	}

TBool ROmxScriptTest::MosSetupNonTunnel(const TDesC8& aSourceComp, TInt aSourcePort,
                                        const TDesC8& aSinkComp, TInt aSinkPort,
                                        OMX_BUFFERSUPPLIERTYPE aSupplier)
	{
	OMX_COMPONENTTYPE* source = ComponentByName(aSourceComp);
	OMX_COMPONENTTYPE* sink = ComponentByName(aSinkComp);
	if(source && sink)
		{
		OMX_PARAM_PORTDEFINITIONTYPE portDef;
		portDef.nSize = sizeof(portDef);
		portDef.nVersion = KOmxVersion;
		portDef.nPortIndex = aSourcePort;
		OMX_ERRORTYPE error = source->GetParameter(source, OMX_IndexParamPortDefinition, &portDef);
		if(error != OMX_ErrorNone)
			{
			FailWithOmxError(_L("GetParameter"), error);
			return EFalse;
			}

		TInt numBufs = portDef.nBufferCountActual;
		TInt bufSize = portDef.nBufferSize;

		portDef.nPortIndex = aSinkPort;
		error = sink->GetParameter(sink, OMX_IndexParamPortDefinition, &portDef);
		if(error != OMX_ErrorNone)
			{
			FailWithOmxError(_L("GetParameter"), error);
			return EFalse;
			}
		
		if (portDef.nBufferCountActual != numBufs)
			{
			ERR_PRINTF1(_L("Number of buffers does not match for non tunneled setup"));
			return EFalse;
			}

		if (portDef.nBufferSize > bufSize)
			{
			bufSize = portDef.nBufferSize;
			}

		if (!iNonTunneledHandler)
			{
			iNonTunneledHandler = new CNonTunneledHandler();

			if (!iNonTunneledHandler)
				{
				return EFalse;
				}
			}

		TRAPD(err, iNonTunneledHandler->AddLinkL(source, aSourcePort, sink, aSinkPort, aSupplier, numBufs, bufSize));
		if (err != KErrNone)
			return EFalse;

		return ETrue;
		}
	else
		{
		// source or sink names not recognized
		return EFalse;
		}
	}


TBool ROmxScriptTest::MosSetupBufferForPortL(const TDesC8& aComp,TInt aPortIndex, const TDesC& aFileName,TInt aHeaderLength,OMX_BUFFERSUPPLIERTYPE aSupplier)
    {
    TBuf<64> compConverted;
    compConverted.Copy(aComp);
    INFO_PRINTF3(_L("MosSetupBufferForPort comp %S port %d"), &compConverted, aPortIndex);
    OMX_COMPONENTTYPE* comp = ComponentByName(aComp);
    
    if(comp)
        {
        OMX_PARAM_PORTDEFINITIONTYPE portDef;
        portDef.nSize = sizeof(portDef);
        portDef.nVersion = KOmxVersion;
        portDef.nPortIndex = aPortIndex;
        OMX_ERRORTYPE error = comp->GetParameter(comp, OMX_IndexParamPortDefinition, &portDef);
        if(error != OMX_ErrorNone)
            {
            FailWithOmxError(_L("GetParameter"), error);
            return EFalse;
            }

        OMX_DIRTYPE portDirType = portDef.eDir;

        if(!iPortBufferHandler)
            {
            iPortBufferHandler = new (ELeave)CPortBufferHandler;
            }
        
        iPortBufferHandler->AddPortL(comp,aPortIndex,aFileName,aSupplier,portDirType,iLogger,aHeaderLength);
        }
    return ETrue;    
    }

TBool ROmxScriptTest::MosSetWindowL(const TDesC8& aComp)
	{
	if(iWindowManager)
		{
		delete iWindowManager;
		iWindowManager = NULL;
		}
	iWindowManager = CWindowManager::NewL();
	TSize screenSize = iWindowManager->ScreenSize();
	INFO_PRINTF3(_L("Screen size is %dx%d"), screenSize.iWidth, screenSize.iHeight);
	
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	iGraphicSink = component;
	
	OMX_ERRORTYPE error = OMX_GetExtensionIndex(component, const_cast<char*>(sOmxSymbianGfxSurfaceConfig), &iExtSurfaceConfigIndex);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetExtensionIndex(sOmxSymbianGfxSurfaceConfig)"), error);
		return EFalse;
		}
	
	//Temporary Hack: This is for Logical IL Video Tests: SurfaceId is supposed to be set in response to PortSettingsChanged event from 
	//MMHP. Currently, there is no support for it. Hence, we are temporarily setting here
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = 0;
	error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}
	error = component->SetParameter(component, OMX_IndexParamPortDefinition, &portDef);

	if(error==OMX_ErrorIncorrectStateOperation)
		{
		OMX_SYMBIAN_VIDEO_PARAM_SURFACECONFIGURATION surfaceConfigParam;
		surfaceConfigParam.nSize = sizeof(OMX_SYMBIAN_VIDEO_PARAM_SURFACECONFIGURATION);
		surfaceConfigParam.nVersion = KOmxVersion;
		surfaceConfigParam.nPortIndex = 0;
		
		// define must be removed when hack is removed - OMX_GetExtensionIndex() should be used but hack for when this
		// returns an error
        #define OMX_SymbianIndexParamVideoGFXSurfaceConfig 0x7F000011
		OMX_ERRORTYPE omxError = component->GetParameter(component, (OMX_INDEXTYPE) OMX_SymbianIndexParamVideoGFXSurfaceConfig, &surfaceConfigParam);
		if(omxError != OMX_ErrorNone)
		    {
		    FailWithOmxError(_L("GetParameter(OMX_SymbianIndexParamVideoGFXSurfaceConfig)"), omxError);
		    return EFalse;
		    }
		    
		TSurfaceConfiguration& surfaceConfig = *reinterpret_cast<TSurfaceConfiguration*>(surfaceConfigParam.pSurfaceConfig);
		                
		TRect rect(0,0,320,240);
		TInt err = surfaceConfig.SetExtent(rect);
		if (err == KErrNone)
		    {
		      err =iWindowManager->SetBackgroundSurface(surfaceConfig, ETrue);
		    }

		if(err)
		    {
		    FailWithOmxError(_L("SetParameter"), error);
		    return EFalse;
		    }
		}
	else if(error)
	    {
	    FailWithOmxError(_L("SetParameter"), error);
	    return EFalse;
	    }
	
	return ETrue;
	}

TBool ROmxScriptTest::MosSetDroppedFrameEvent(const TDesC8& aComp, const TDesC8& aAction)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	TBuf8<80> extension;

	if (aAction == _L8("invalidextension"))
		{
		extension.Append(_L8("OMX.NOKIA.INDEX.PARAM.INVALIDEXTENSION"));
		}
	else
		{
		extension.Append(_L8("OMX.NOKIA.INDEX.PARAM.DROPPEDFRAMEEVENT"));
		}

	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);

	if (aAction == _L8("invalidextension"))
		{
		if (error != OMX_ErrorUnsupportedIndex)
			{
			ERR_PRINTF2(_L("GetExtensionIndex with invalid extension incorrectly returned error 0x%X"), error);
			StopTest(EFail);
			return EFalse;
			}
		return ETrue;
		}

	if (error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetExtensionIndex"), error);
		return EFalse;
		}

	OMX_NOKIA_PARAM_DROPPEDFRAMEEVENT exten;
	exten.nSize = sizeof(exten);
	exten.nVersion = KOmxVersion;
	exten.nPortIndex = 0;

	if(aAction == _L8("enable"))
		{
		exten.bEnabled = OMX_TRUE;
		error = component->SetParameter(component, indexType, &exten);
		if(error)
			{
			FailWithOmxError(_L("SetParameter"), error);
			return EFalse;
			}
		return ETrue;
		}
	else 
		{
		exten.bEnabled = OMX_FALSE;
		error = component->SetParameter(component, indexType, &exten);
		if(error)
			{
			FailWithOmxError(_L("SetParameter"), error);
			return EFalse;
			}
		return ETrue;
		}
	}

TBool ROmxScriptTest::MosAllTransitionL(OMX_STATETYPE aState, OMX_ERRORTYPE aExpectedError, TTransitionOrder aOrder)
	{
	INFO_PRINTF2(_L("Transitioning all components to %S"), StateDes(aState));
	if(aOrder == ELoadOrder && aState == OMX_StateIdle && iState == OMX_StateLoaded)
		{
		WARN_PRINTF1(_L("Using component load order for loaded -> idle transition order"));
		}
	iEventHandler.AwaitTransition(aState, iState);
	RArray<TInt> order;
	CleanupClosePushL(order);
	order.ReserveL(iComponents.Count());
	if(aOrder == ELoadOrder)
		{
		for(TInt index = 0; index < iComponents.Count(); index++)
			{
			order.Append(index);
			}
		}
	else if(aOrder == EAutoOrder)
		{
		TRAPD(error, FindTransitionOrderL(iComponents, iTunnels, order));
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("Error determining transition order, error=%d"), error);
			User::Leave(error);
			}
		}
	
	if(aState == OMX_StateIdle && iState == OMX_StateExecuting)
		{
		// use reverse order for Idle->Executing transition
		TInt left = 0, right = order.Count() - 1;
		while(left < right)
			{
			TInt tmp = order[left];
			order[left] = order[right];
			order[right] = tmp;
			left++;
			right--;
			}
		}
	
	if(aOrder == EAutoOrder)
		{
		TBuf<128> orderDes;
		for(TInt index = 0, count = order.Count(); index < count; index++)
			{
			HBufC8* name8 = iComponents[order[index]]->iShortName;
			TBuf<16> name;
			name.Copy(*name8);
			if(index > 0)
				{
				orderDes.Append(_L(", "));
				}
			orderDes.Append(name);
			}
		INFO_PRINTF2(_L("Auto-detected transition order: %S"), &orderDes);
		}
	
	TBool commandError = EFalse;
	TBool ret = EFalse;
	for(TInt orderIndex = 0, count = order.Count(); orderIndex < count; orderIndex++)
		{
		TInt index = order[orderIndex];
		CComponentInfo* compInfo = iComponents[index];
		OMX_COMPONENTTYPE* component = compInfo->iComponent;
		OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandStateSet, aState, NULL);
		if(error)
			{
			TBuf<128> msg;
			msg.Format(_L("SendCommand(OMX_CommandStateSet, %S)"), StateDes(aState));
			if(error != aExpectedError)
				{
				FailWithOmxError(compInfo->iShortName->Des(), msg, error);
				ret = EFalse;
				}
			else //Need to stop the tests now be cause an error has occurred, even if it is expected.
				{
				StopTest(EPass);
				ret = ETrue;
				}
			commandError = ETrue;
			break;
			}

		if (!HandleNonTunneledBuffers(aState, component))
			{
			commandError = ETrue;
			ret = EFalse;
			break;
			}
	if(!HandlePortBufferHandler(aState, component))
		    {
		    return EFalse;
		    }
		}
	CleanupStack::PopAndDestroy(&order);
	if(commandError)
		{
		return ret;
		}
	
	// wait for the transitions to complete
	// use a nested loop of the active scheduler
	// AllComponentsTransitioned() calls EndWait() 
	BeginWait();
	return Reason() == KErrNone;
	}
	
TBool ROmxScriptTest::MosTransition(const TDesC8& aComp, OMX_STATETYPE aState, TBool aAsync)
	{
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF3(_L("Transitioning %S component to %S"), &compConverted, StateDes(aState));

	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	if (!aAsync)
		{
		iEventHandler.AwaitSingleTransition(component, aState, iState);
		}
	else
		{
		//Don't want to force a wait on this state transition so instead of using the event handler add the event to the expected list
		TExpectedEvent event;
		event.iComponent = component;
		event.iEvent = OMX_EventCmdComplete;
		event.iData1 = OMX_CommandStateSet;
		event.iData2 = aState;
		event.iEventData = NULL;
		TRAPD(err, iExpectedEvents.AppendL(event));
		if (err != KErrNone)
			return EFalse;
		}
		
	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandStateSet, aState, NULL);
	if(error)
		{
		FailWithOmxError(_L("SendCommand(OMX_CommandStateSet)"), error);
		return EFalse;
		}

	if (!HandleNonTunneledBuffers(aState, component))
		{
		return EFalse;
		}
	
	if(!HandlePortBufferHandler(aState, component))
         {
         return EFalse;
         }
		
	if (!aAsync)
		{
		BeginWait();
		}
	return Reason() == KErrNone;
	}	

TBool ROmxScriptTest::MosFailingTransition(const TDesC8& aComp, OMX_STATETYPE aState,OMX_ERRORTYPE aExpectedError)
	{
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF3(_L("Transitioning %S component to %S"), &compConverted, StateDes(aState));

	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandStateSet, aState, NULL);
	if(error == aExpectedError)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

TBool ROmxScriptTest::MosWaitEOS()
	{
	// AllComponentsEOS() calls EndWait()
	INFO_PRINTF1(_L("Waiting for End of Stream from all components"));
	iEventHandler.AwaitEOS();
	BeginWait();
	return ETrue;
	}

TBool ROmxScriptTest::MosWaitEOS(const TDesC8& aComp)
	{
	// AllComponentsEOS() calls EndWait()
	TBuf<16> comp16;
	comp16.Copy(aComp);
	INFO_PRINTF2(_L("Waiting for End of Stream from component %S"), &comp16);
	OMX_COMPONENTTYPE* compHandle = ComponentByName(aComp);
	if(compHandle == NULL)
		{
		return EFalse;
		}
	iEventHandler.AwaitEOS(compHandle);
	BeginWait();
	return ETrue;
	}

TBool ROmxScriptTest::MosWaitL(TTimeIntervalMicroSeconds32 aDelay)
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	else
		{
		iTimer = CVideoTestTimer::NewL(*this);		
		}	
	
	iTimer->Start(aDelay);
	BeginWait();
	return ETrue;		
	}

TBool ROmxScriptTest::MosExpectEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	TExpectedEvent event;	
	event.iComponent = component;
	event.iEvent = aEvent;
	event.iData1 = aData1;
	event.iData2 = aData2;
	event.iEventData = NULL;
	iExpectedEvents.AppendL(event);

	return ETrue;
	}

TBool ROmxScriptTest::MosWaitForAllEventsL()
	{
	if(iExpectedEvents.Count() > 0)
		{
		iWaitingForEvents = ETrue;
		BeginWait();
		iWaitingForEvents = EFalse;
		__ASSERT_ALWAYS(iExpectedEvents.Count() == 0, User::Invariant());
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosSetFilename(const TDesC8& aComp, const TDesC& aFilename)
	{
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF3(_L("Setting filename on component %S to %S"), &compConverted, &aFilename);
	
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_PARAM_CONTENTURITYPE* contentURI = NULL;
	TRAPD(err, contentURI = ParamConversion::FilenameAsContentUriStructV1_1L(aFilename));
	if(err==KErrBadName)//logical IL tests pass just the filename. so try adding the directory path
	    {
	    TBuf<64> fileName;
	    fileName.Copy(KPortDirectoryPath);
	    fileName.Append(aFilename);
	    err=KErrNone;
	    TRAP(err, contentURI = ParamConversion::FilenameAsContentUriStructV1_1L(fileName));
	    }
	if(err)
	  {
	  ERR_PRINTF2(_L("Error converting filename to OMX_CONTENTURI (%d)"), err);
	  return EFalse;
	  }
	OMX_ERRORTYPE error = component->SetParameter(component, OMX_IndexParamContentURI, contentURI);
	delete contentURI;
	if(error)
		{
		FailWithOmxError(_L("SetParameter(OMX_IndexParamContentURI)"), error);
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TBool ROmxScriptTest::MosSetFilename_bellagio(const TDesC8& aComp, char *filepath)
    {
    TBuf<64> compConverted;
        compConverted.Copy(aComp);
        
    //INFO_PRINTF3(_L("Setting filename on component %S to %S"), &compConverted, filepath);
        
        OMX_COMPONENTTYPE* component = ComponentByName(aComp);
        
        if(!component)
            {
            return EFalse;
            }
    OMX_INDEXTYPE indexType;
    OMX_ERRORTYPE omx_err;
       
     // Setting the input file name for File Source Component     
    omx_err = OMX_GetExtensionIndex(component, "OMX.Nokia.index.config.filename",&indexType);
    
    if(omx_err != OMX_ErrorNone)
   	{
   		return EFalse;
   	}

    OMX_ERRORTYPE error = OMX_SetConfig(component,indexType,filepath);
       
    if(error)
    {
      return EFalse;
    }
    else
    {
       return ETrue;
    }

    }

TBool ROmxScriptTest::MosSetBadFilename(const TDesC8& aComp)
	{
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF2(_L("Setting filename on component %S"), &compConverted);

	TText8 cstr[6] =  {':', 'e' ,'l' ,'l' ,'o','\0'};
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	OMX_PARAM_CONTENTURITYPE* contentURI = reinterpret_cast<OMX_PARAM_CONTENTURITYPE*>(new TUint8[15]);
	if (!contentURI)
		return EFalse;
		
	TPtr8 uriStructDes(contentURI->contentURI, sizeof(cstr));
	uriStructDes = cstr;

	OMX_ERRORTYPE error = component->SetParameter(component, OMX_IndexParamContentURI, contentURI);
	delete contentURI;
	
	return ETrue;

	}

TBool ROmxScriptTest::MosGetFilename(const TDesC8& aComp, TDesC& aFilename, OMX_ERRORTYPE aExpectedError)
	{
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF2(_L("Getting filename on component %S"), &compConverted);
	
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	OMX_PARAM_CONTENTURITYPE* contentURI = NULL;
	TRAPD(err, contentURI = ParamConversion::FilenameAsContentUriStructV1_1L(aFilename));
	if(err)
		{
		ERR_PRINTF2(_L("Error converting filename to OMX_CONTENTURI (%d)"), err);
		return EFalse;
		}
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamContentURI, contentURI);
	delete contentURI;
	if((error != aExpectedError) && (error != OMX_ErrorOverflow) && (error != OMX_ErrorUnsupportedSetting))
		{
        TBuf<64> errorDes;
        OMXUtil::ErrorDes(error, errorDes);
        TBuf<64> expectedErrorDes;
        OMXUtil::ErrorDes(aExpectedError, expectedErrorDes);
		ERR_PRINTF3(_L("GetParameter(OMX_IndexParamContentURI) returned %S but expected %S OMX_ErrorOverflow or OMX_ErrorUnsupportedSetting"), 
                    &errorDes, &expectedErrorDes);
		StopTest(EFail);
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}


void ROmxScriptTest::FormatHex(const TDesC8& bin, TDes& hex)
	{
	__ASSERT_DEBUG(hex.MaxLength() == bin.Length() * 2, User::Invariant());
	hex.SetLength(0);
	for(TInt index = 0, length = bin.Length(); index < length; index++)
		{
		TUint8 octet = bin[index];
		hex.AppendFormat(_L("%02X"), octet);
		}
	}

TBool ROmxScriptTest::MosSetBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aCount, OMX_ERRORTYPE aExpectedError)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nVersion = KOmxVersion;
	portDef.nSize = sizeof(portDef);
	portDef.nPortIndex = aPortIndex;
	
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter()"), error);
		return EFalse;
		}

	portDef.nBufferCountActual = aCount;
	
	INFO_PRINTF3(_L("MosSetBufferCount count %d; dir %d"), portDef.nBufferCountActual, portDef.eDir);

	error = component->SetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(aExpectedError != error)
		{
		ERR_PRINTF3(_L("MosSetBufferCount OMX_SetParameter() error 0x%X, expecting 0x%X"), error, aExpectedError);
		return EFalse;
		}
	return ETrue;
	}


TBool ROmxScriptTest::MosFilterAndCompareFilesL(
        const TDesC& aFileOne, const TDesC8& aFile1Filter1, const TDesC8& aFile1Filter2, const TDesC8& aFile1Filter3,
        const TDesC& aFileTwo, const TDesC8& aFile2Filter1, const TDesC8& aFile2Filter2, const TDesC8& aFile2Filter3)
    {
    TBuf8<16> aHashOne, aHashTwo;
    
    THashFilter f1filter1 = GetHashFilterByName(aFile1Filter1);
    THashFilter f1filter2 = GetHashFilterByName(aFile1Filter2);
    THashFilter f1filter3 = GetHashFilterByName(aFile1Filter3);

    THashFilter f2filter1 = GetHashFilterByName(aFile2Filter1);
    THashFilter f2filter2 = GetHashFilterByName(aFile2Filter2);
    THashFilter f2filter3 = GetHashFilterByName(aFile2Filter3);
    
    HashFileL(aFileOne, aHashOne, f1filter1, f1filter2, f1filter3 );
    HashFileL(aFileTwo, aHashTwo, f2filter1, f2filter2, f2filter3 );
    
    if (aHashOne == aHashTwo)
        {
        INFO_PRINTF1(_L("Filter and compare files = MATCH"));
        }
    else
        {
        INFO_PRINTF1(_L("Filter and compare files = FAIL"));
        }

    return aHashOne == aHashTwo;
    }

TBool ROmxScriptTest::MosCompareFilesL(const TDesC& aFileOne, const TDesC& aFileTwo)
    {
    TBuf8<16> aHashOne, aHashTwo;
    HashFileL(aFileOne, aHashOne);
    HashFileL(aFileTwo, aHashTwo);
    return aHashOne == aHashTwo;
    }


TBool ROmxScriptTest::MosBufferSupplierOverrideL(
    const TDesC8& aSourceComp, 
    TInt aSourcePort,
	const TDesC8& aSinkComp, TInt aSinkPort,
	OMX_BUFFERSUPPLIERTYPE aSupplier, 
	OMX_ERRORTYPE aExpectedSourceError,
	OMX_ERRORTYPE aExpectedSinkError
	)
	{
	OMX_COMPONENTTYPE* source = ComponentByName(aSourceComp);
	OMX_COMPONENTTYPE* sink = ComponentByName(aSinkComp);
	if(!source || !sink)
		{
		return EFalse;
		}
	
	OMX_PARAM_BUFFERSUPPLIERTYPE param;
	param.nSize = sizeof(param);
	param.nVersion = KOmxVersion;
	param.nPortIndex = aSourcePort;
	param.eBufferSupplier = aSupplier;
	OMX_ERRORTYPE error = source->SetParameter(source, OMX_IndexParamCompBufferSupplier, &param);
	if(error != aExpectedSourceError)
		{
		ERR_PRINTF3(_L("MosBufferSupplierOverrideL source->SetParameter error 0x%X, expected 0x%X"), error, aExpectedSourceError);
		return EFalse;
		}

	param.nPortIndex = aSinkPort;
	error = sink->SetParameter(sink, OMX_IndexParamCompBufferSupplier, &param);
	if(error != aExpectedSinkError)
		{
		ERR_PRINTF3(_L("MosBufferSupplierOverrideL sink->SetParameter error 0x%X, expected 0x%X"), error, aExpectedSinkError);
		return EFalse;
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosSetCameraOneShotL(const TDesC8& aComp, TInt aIsOneShot, OMX_ERRORTYPE aExpectedError)
    {
    //TODO DL
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_PARAM_SENSORMODETYPE sensorModeType;
    sensorModeType.nSize = sizeof(OMX_PARAM_SENSORMODETYPE);
    sensorModeType.nVersion = KOmxVersion;
    sensorModeType.nPortIndex = OMX_ALL;
    OMX_ERRORTYPE error = component->GetParameter(component,OMX_IndexParamCommonSensorMode,&sensorModeType);
    if(error)
        {
        FailWithOmxError(_L("GetParameter()"), error);
        return EFalse;
        }

    sensorModeType.bOneShot = aIsOneShot ? static_cast<OMX_BOOL>(ETrue) : static_cast<OMX_BOOL>(EFalse);
    error = component->SetParameter(component,OMX_IndexParamCommonSensorMode,&sensorModeType);

    if(error != aExpectedError)
        {
		ERR_PRINTF3(_L("MosSetCameraOneShotL SetParameter() error 0x%X, expected 0x%X"), error, aExpectedError);
        return EFalse;
        }

    return ETrue;
    }

TBool ROmxScriptTest::MosSetCameraCaptureL(const TDesC8& aComp, TInt aPortIndex, TInt aIsCapturing, OMX_ERRORTYPE aExpectedError)
    {
    //TODO DL
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }
    
    OMX_SYMBIAN_CONFIG_BOOLEANTYPE isCapturing;
    isCapturing.nSize = sizeof(OMX_SYMBIAN_CONFIG_BOOLEANTYPE);
    isCapturing.nVersion = KOmxVersion;
    isCapturing.nPortIndex = aPortIndex;
    OMX_INDEXTYPE index;
    isCapturing.bEnabled = aIsCapturing ? OMX_TRUE : OMX_FALSE; 
    	
   OMX_ERRORTYPE check = OMX_GetExtensionIndex(component,"OMX.Symbian.Index.Config.Common.ExtCapturing",&index);
   
   if(check != OMX_ErrorNone)
   	{
   		return EFalse;
   	}
   
    OMX_ERRORTYPE error = component->SetConfig(component,/*(OMX_INDEXTYPE)*/index,&isCapturing);

    if(error != aExpectedError)
        {
		ERR_PRINTF3(_L("MosSetCameraCaptureL SetConfig() error 0x%X, expected 0x%X"), error, aExpectedError);
        return EFalse;
        }

    return ETrue;
    }

TBool ROmxScriptTest::MosSetVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_COLOR_FORMATTYPE* aColorFormat, OMX_VIDEO_CODINGTYPE* aCodingType, TInt aStride, TReal aFps, OMX_ERRORTYPE aExpectedError)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	if(aWidth != -1)
		{
		portDef.format.video.nFrameWidth = aWidth;
		}
	if(aHeight != -1)
		{
		portDef.format.video.nFrameHeight = aHeight;
		}
	
	if (aColorFormat)
		{
		portDef.format.video.eColorFormat = *aColorFormat;
		}

	if (aCodingType)
		{
		portDef.format.video.eCompressionFormat = *aCodingType;
		}
		
	if(aStride != -1)
		{
		portDef.format.video.nStride = aStride;
		}
#ifndef HREF_ED_WITHOUT_FLOATING_POINT
	if(aFps != -1)
		{
		//TODO Fixup by removing below camera case, once camera team make appropiate changes to validateframerate function.
		//Currently the camera function only allows fps >=15 AND <=120 and it isnt in the Q16 format that is required.
		//Leave in the next two lines uncommented.
		//TUint32 fpsQ16 = (TUint32) (aFps * 65536 + 0.5);
		//portDef.format.video.xFramerate = fpsQ16;
		//Changed xml file from 10fps to 15fps due to camera constraint.Will need to be changed back also
		//BEGIN HACK CODE
		_LIT8(KCamera,"OMX.SYMBIAN.VIDEO.CAMERASOURCE");
		_LIT8(KLogicalILCamera,"OMX.SYMBIAN.LOGICAL.VIDEO.CAMERASOURCE");
		TBool cameraTest = EFalse;
		for(TInt index = 0, count = iComponents.Count(); index < count; index++)
			{
			const CComponentInfo* componentInfo = iComponents[index];
			if(componentInfo->iComponent == component)
				{
				if((*(componentInfo->iComponentName) == KCamera)||(*(componentInfo->iComponentName) == KLogicalILCamera))
					{
					portDef.format.video.xFramerate = aFps;
					cameraTest = ETrue;
					}
				break;
				}
			}
		if(cameraTest == EFalse)
			{
			TUint32 fpsQ16 = (TUint32) (aFps * 65536 + 0.5);
			portDef.format.video.xFramerate = fpsQ16;
			}
		}
	//END HACK CODE.
#else	
	portDef.format.video.xFramerate = 0;
#endif //HREF_ED_WITHOUT_FLOATING_POINT
	error = component->SetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error != aExpectedError)
		{
		ERR_PRINTF3(_L("MosSetVideoPortDefL SetParameter() error 0x%X, expected 0x%X"), error, aExpectedError);
		return EFalse;
		}
	return ETrue;
	}
	
TBool ROmxScriptTest::MosCheckStateL(const TDesC8& aComp, OMX_STATETYPE aState)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	OMX_STATETYPE state;
	OMX_ERRORTYPE error = component->GetState(component, &state);
	if(error)
		{
		FailWithOmxError(_L("GetState()"), error);
		return EFalse;
		}
	if(state != aState)
		{
		TBuf<64> compConverted;
		compConverted.Copy(aComp);
		ERR_PRINTF4(_L("Expected component %S to be in state %S, actually in %S"), &compConverted, StateDes(aState), StateDes(state));
		StopTest(EFail);
		return EFalse;
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosCheckVideoPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aWidth, TInt aHeight, OMX_VIDEO_CODINGTYPE aCoding, OMX_COLOR_FORMATTYPE aColorFormat)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	OMX_VIDEO_PORTDEFINITIONTYPE& vidDef = portDef.format.video;
	if( vidDef.nFrameWidth == aWidth &&
		vidDef.nFrameHeight == aHeight &&
		vidDef.eCompressionFormat == aCoding &&
		vidDef.eColorFormat == aColorFormat)
		{
		return ETrue;
		}
	else
		{
		ERR_PRINTF1(_L("video port definition did not match expected values"));
		StopTest(EFail);
		return EFalse;
		}
	}

TBool ROmxScriptTest::MosCheckMetaDataL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& aData)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	OMX_CONFIG_METADATAITEMTYPE* metadata = reinterpret_cast<OMX_CONFIG_METADATAITEMTYPE*>(new(ELeave) TUint8[102400]);
	CleanupStack::PushL(metadata);

	metadata->nSize = 102400;
	metadata->nVersion = KOmxVersion;
	metadata->eScopeMode = aScope;
	metadata->nScopeSpecifier = aPortIndex;
	metadata->nMetadataItemIndex = aAtomIndex;
	metadata->nKeySizeUsed = aAtomType.Length();
	
	TPtr8 keyDes(metadata->nKey, metadata->nKeySizeUsed);
	keyDes = aAtomType;
	
	metadata->nValueMaxSize = metadata->nSize - _FOFF(OMX_CONFIG_METADATAITEMTYPE, nValue);
		
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexConfigMetadataItem, metadata);
	if(error)
		{
		CleanupStack::PopAndDestroy(metadata);
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}

	TPtrC8 valueDes(metadata->nValue, metadata->nValueSizeUsed);	
	if(valueDes.Compare(aData) == 0)
		{
		CleanupStack::PopAndDestroy(metadata);
		return ETrue;
		}
	else
		{
		ERR_PRINTF1(_L("Did not read metedata"));
		CleanupStack::PopAndDestroy(metadata);
		StopTest(EFail);
		return EFalse;
		}
	}

TBool ROmxScriptTest::MosGetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& /*aData*/)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	OMX_CONFIG_METADATAITEMTYPE* metadata = reinterpret_cast<OMX_CONFIG_METADATAITEMTYPE*>(new(ELeave) TUint8[102400]);
	CleanupStack::PushL(metadata);

	metadata->nSize = 102400;
	metadata->nVersion = KOmxVersion;
	metadata->eScopeMode = aScope;
	metadata->nScopeSpecifier = aPortIndex;
	metadata->nMetadataItemIndex = aAtomIndex;
	metadata->nKeySizeUsed = aAtomType.Length();
	
	TPtr8 keyDes(metadata->nKey, metadata->nKeySizeUsed);
	keyDes = aAtomType;
	
	metadata->nValueMaxSize = metadata->nSize - _FOFF(OMX_CONFIG_METADATAITEMTYPE, nValue);

	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamAudioG723, metadata);
	INFO_PRINTF2(_L("MosGetParameterUnknownIndexTypeL: %d"), error);

	CleanupStack::PopAndDestroy(metadata);

	if(error)
		{
		// This is what we expected
		return ETrue;
		}

	ERR_PRINTF1(_L("MosGetParameterUnknownIndexTypeL unexpectedly received OMX_ErrorNone"));
	StopTest(EFail);
	return EFalse;	
	}
	
TBool ROmxScriptTest::MosSetParameterUnknownIndexTypeL(const TDesC8& aComp, TInt aPortIndex, OMX_METADATASCOPETYPE aScope, const TDesC8& aAtomType, TUint32 aAtomIndex, const TDesC8& /*aData*/)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	OMX_CONFIG_METADATAITEMTYPE* metadata = reinterpret_cast<OMX_CONFIG_METADATAITEMTYPE*>(new(ELeave) TUint8[102400]);
	CleanupStack::PushL(metadata);

	metadata->nSize = 102400;
	metadata->nVersion = KOmxVersion;
	metadata->eScopeMode = aScope;
	metadata->nScopeSpecifier = aPortIndex;
	metadata->nMetadataItemIndex = aAtomIndex;
	metadata->nKeySizeUsed = aAtomType.Length();
	
	TPtr8 keyDes(metadata->nKey, metadata->nKeySizeUsed);
	keyDes = aAtomType;
	
	metadata->nValueMaxSize = metadata->nSize - _FOFF(OMX_CONFIG_METADATAITEMTYPE, nValue);

	OMX_ERRORTYPE error = component->SetParameter(component, OMX_IndexParamAudioG723, metadata);

	CleanupStack::PopAndDestroy(metadata);

	if(error)
		{
		// This is what we expected
		return ETrue;
		}

	ERR_PRINTF1(_L("MosSetParameterUnknownIndexTypeL unexpectedly received OMX_ErrorNone"));
	StopTest(EFail);
	return EFalse;
	}
TBool ROmxScriptTest::MosDisablePort(const TDesC8& aComp, TInt aPortIndex)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandPortDisable, aPortIndex, NULL);
	if(error)
		{
		FailWithOmxError(aComp, _L("OMX_SendCommand(OMX_CommandPortDisable)"), error);
		return EFalse;
		}

	return ETrue;
	}

TBool ROmxScriptTest::MosEnablePort(const TDesC8& aComp, TInt aPortIndex)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandPortEnable, aPortIndex, NULL);
	if(error)
		{
		FailWithOmxError(aComp, _L("OMX_SendCommand(OMX_CommandPortEnable)"), error);
		return EFalse;
		}

	return ETrue;
	}

TBool ROmxScriptTest::MosIgnoreEventL(const TDesC8& aComp, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	TExpectedEvent event;	
	event.iComponent = component;
	event.iEvent = aEvent;
	event.iData1 = aData1;
	event.iData2 = aData2;
	event.iEventData = NULL;
	iIgnoredEvents.AppendL(event);

	return ETrue;
	}

TBool ROmxScriptTest::MosSetPcmAudioPortDefL(const TDesC8& aComp,
											 TInt aPortIndex,
											 TInt aNumChannels,
											 TInt aSamplingRate,
											 TInt aBitsperSample, 
											 OMX_NUMERICALDATATYPE aNumData,
											 OMX_ENDIANTYPE aEndian,
											 OMX_BOOL* aInterleaved,
											 const TDesC8* aEncoding )
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_AUDIO_PARAM_PCMMODETYPE pcmModeType;
	pcmModeType.nSize = sizeof(pcmModeType);
	pcmModeType.nVersion = KOmxVersion;
	pcmModeType.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamAudioPcm, &pcmModeType);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	if(aNumChannels != KErrNotFound)
		{
		pcmModeType.nChannels = aNumChannels;
		}
	if(aSamplingRate != KErrNotFound)
		{
		pcmModeType.nSamplingRate = aSamplingRate;
		}
	if(aBitsperSample != KErrNotFound)
		{
		pcmModeType.nBitPerSample = aBitsperSample;
		}
	if((aNumData != OMX_NumercialDataMax) && (aNumData != KErrNotFound))
 		{
 		pcmModeType.eNumData = aNumData;
 		}
 	if((aEndian != OMX_EndianMax) && (aEndian != KErrNotFound))
 		{
 		pcmModeType.eEndian = aEndian;
 		}
 	if(aInterleaved != NULL)
 		{
 		pcmModeType.bInterleaved = *aInterleaved;
 		}
    if (aEncoding)
        {
        if (*aEncoding==_L8("linear")) pcmModeType.ePCMMode = OMX_AUDIO_PCMModeLinear;
        else if (*aEncoding==_L8("alaw")) pcmModeType.ePCMMode = OMX_AUDIO_PCMModeALaw;
        else if (*aEncoding==_L8("mulaw")) pcmModeType.ePCMMode = OMX_AUDIO_PCMModeMULaw;
        else FailWithOmxError(_L("Invalid PCM encoding parameter"), OMX_ErrorBadParameter);
        }
	error = component->SetParameter(component, OMX_IndexParamAudioPcm, &pcmModeType);
	if(error)
		{
		FailWithOmxError(_L("SetParameter()"), error);
		return EFalse;
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosSetConfigAudioMuteL(const TDesC8& aComp,
                                             TInt aPortIndex,
                                             TBool aMute)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_AUDIO_CONFIG_MUTETYPE audioMuteType;
    audioMuteType.nSize = sizeof(audioMuteType);
    audioMuteType.nVersion = KOmxVersion;
    audioMuteType.nPortIndex = aPortIndex;
    OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigAudioMute, &audioMuteType);
    if(error)
        {
        FailWithOmxError(_L("GetConfig()"), error);
        return EFalse;
        }
    audioMuteType.bMute = (aMute ? OMX_TRUE : OMX_FALSE);

    error = component->SetConfig(component, OMX_IndexConfigAudioMute, &audioMuteType);
    if(error)
        {
        FailWithOmxError(_L("SetConfig()"), error);
        return EFalse;
        }
    return ETrue;
    }

TBool ROmxScriptTest::MosCheckConfigAudioMuteL(const TDesC8& aComp,
                                             TInt aPortIndex,
                                             TBool aMute)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        User::Leave(KErrGeneral);
        }

    OMX_AUDIO_CONFIG_MUTETYPE audioMuteType;
    audioMuteType.nSize = sizeof(audioMuteType);
    audioMuteType.nVersion = KOmxVersion;
    audioMuteType.nPortIndex = aPortIndex;
    OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigAudioMute, &audioMuteType);
    if(error)
        {
        FailWithOmxError(_L("GetConfig()"), error);
        return EFalse;
        }

    if(audioMuteType.bMute != aMute)
        {
        ERR_PRINTF1(_L("Mute not what expected."));
        StopTest(KErrGeneral, EFail);
        return EFalse;
        }
    return ETrue;
    }




TBool ROmxScriptTest::MosSetConfigAudioVolumeL(const TDesC8& aComp,
                                             TInt aPortIndex,
                                             TBool aLinear, 
                                             TInt aMinVolume, 
                                             TInt aMaxVolume, 
                                             TInt aVolume,
                                             OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_AUDIO_CONFIG_VOLUMETYPE audioVolumeType;
    audioVolumeType.nSize = sizeof(audioVolumeType);
    audioVolumeType.nVersion = KOmxVersion;
    audioVolumeType.nPortIndex = aPortIndex;
    OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigAudioVolume, &audioVolumeType);
    if(error)
        {
        FailWithOmxError(_L("GetConfig()"), error);
        return EFalse;
        }
    if(aMinVolume != -1)
        {
        audioVolumeType.sVolume.nMin = aMinVolume;
        }
    if(aMaxVolume != -1)
        {
        audioVolumeType.sVolume.nMax = aMaxVolume;
        }
    if(aVolume != -1)
        {
        audioVolumeType.sVolume.nValue = aVolume;
        }
    audioVolumeType.bLinear = (aLinear ? OMX_TRUE : OMX_FALSE);
        
    error = component->SetConfig(component, OMX_IndexConfigAudioVolume, &audioVolumeType);
    if(error != aExpectedError)
        {
        FailWithOmxError(_L("SetConfig()"), error);
        return EFalse;
        }
    return ETrue;
    }

TBool ROmxScriptTest::MosCheckConfigAudioVolumeL(const TDesC8& aComp,
                                             TInt aPortIndex,
                                             TBool aLinear, 
                                             TInt aMinVolume, 
                                             TInt aMaxVolume, 
                                             TInt aVolume)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        User::Leave(KErrGeneral);
        }

    OMX_AUDIO_CONFIG_VOLUMETYPE audioVolumeType;
    audioVolumeType.nSize = sizeof(audioVolumeType);
    audioVolumeType.nVersion = KOmxVersion;
    audioVolumeType.nPortIndex = aPortIndex;
    OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigAudioVolume, &audioVolumeType);
    if(error)
        {
        FailWithOmxError(_L("GetConfig()"), error);
        User::Leave(KErrGeneral);
        }
    if(aMinVolume != -1)
        {
        if (audioVolumeType.sVolume.nMin != aMinVolume)
            {
            ERR_PRINTF1(_L("Min volume not what expected."));
            StopTest(KErrGeneral, EFail);
            return EFalse;
            }
        }
    if(aMaxVolume != -1)
        {
        if (audioVolumeType.sVolume.nMax != aMaxVolume)
            {
            ERR_PRINTF1(_L("Max volume not what expected."));
            StopTest(KErrGeneral, EFail);
            return EFalse;
            }
        }
    if(aVolume != -1)
        {
        if (audioVolumeType.sVolume.nValue != aVolume)
            {
            ERR_PRINTF1(_L("Volume not what expected."));
            StopTest(KErrGeneral, EFail);
            return EFalse;
            }
        }
    if(audioVolumeType.bLinear != aLinear)
        {
        ERR_PRINTF1(_L("Linear not what expected."));
        StopTest(KErrGeneral, EFail);
        return EFalse;
        }
    return ETrue;
    }


TBool ROmxScriptTest::MosSetAacAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, TInt aNumChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAacTools, TInt aAacErTools, TInt aProfile, TInt aStreamFormat, TInt aChannelMode)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_AUDIO_PARAM_AACPROFILETYPE aacProfile;
	aacProfile.nSize = sizeof(aacProfile);
	aacProfile.nVersion = KOmxVersion;
	aacProfile.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamAudioAac, &aacProfile);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	
	if(aNumChannels != -1)
		{
		aacProfile.nChannels = aNumChannels;
		}
	if(aSamplingRate != -1)
		{
		aacProfile.nSampleRate = aSamplingRate;
		}
	if(aBitRate != -1)
		{
		aacProfile.nBitRate = aBitRate;
		}
	if(aAudioBandwidth != -1)
		{
		aacProfile.nAudioBandWidth = aAudioBandwidth;
		}
	if(aFrameLength != -1)
		{
		aacProfile.nFrameLength = aFrameLength;
		}
	if(aAacTools != -1)
		{
		aacProfile.nAACtools = aAacTools;
		}
	if(aAacErTools != -1)
		{
		aacProfile.nAACERtools = aAacErTools;
		}
	if(aProfile != -1)
		{
		aacProfile.eAACProfile = static_cast<OMX_AUDIO_AACPROFILETYPE>(aProfile);
		}
	if(aStreamFormat != -1)
		{
		aacProfile.eAACStreamFormat = static_cast<OMX_AUDIO_AACSTREAMFORMATTYPE>(aStreamFormat);
		}
	if(aChannelMode != -1)
		{
		aacProfile.eChannelMode = static_cast<OMX_AUDIO_CHANNELMODETYPE>(aChannelMode);
		}
	
	error = component->SetParameter(component, OMX_IndexParamAudioAac, &aacProfile);
	if(error)
		{
		FailWithOmxError(_L("SetParameter()"), error);
		return EFalse;
		}
	return ETrue;	
	}

TBool ROmxScriptTest::MosSetAudioPortDefL(const TDesC8& aComp, TInt aPortIndex, OMX_AUDIO_CODINGTYPE* aCodingType, OMX_ERRORTYPE aExpectedError)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	if(aCodingType)
		{
		portDef.format.audio.eEncoding = *aCodingType;
		}
	error = component->SetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error != aExpectedError)
		{
        TBuf<64> errorDes;
        OMXUtil::ErrorDes(error, errorDes);
        TBuf<64> expectedErrorDes;
        OMXUtil::ErrorDes(aExpectedError, expectedErrorDes);
		ERR_PRINTF4(_L("SetParameter(OMX_IndexParamPortDefinition) portDef.format.audio.eEncoding %d returned %S but expected %S OMX_ErrorOverflow or OMX_ErrorUnsupportedSetting"), 
                    portDef.format.audio.eEncoding, &errorDes, &expectedErrorDes);
		StopTest(EFail);
		return EFalse;
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosSetRefClockTypeL(const TDesC8& aComp, OMX_TIME_REFCLOCKTYPE aRefClockType)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE conf;
	conf.nVersion = KOmxVersion;
	conf.nSize = sizeof(conf);
	conf.eClock = aRefClockType;

	OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimeActiveRefClock, &conf);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimeActiveRefClock)"), error);
		return EFalse;
		}
	
	return ETrue;
	}

TBool ROmxScriptTest::MosSetClockTimeScale(const TDesC8& aComp, OMX_S32 aScale)
{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	INFO_PRINTF2(_L("MosSetClockTimeScale %d"), aScale);

	OMX_TIME_CONFIG_SCALETYPE conf;
	conf.nSize = sizeof(conf);
	conf.nVersion = KOmxVersion;
	conf.xScale = aScale;

	OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimeScale, &conf);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimeClockState)"), error);
		return EFalse;
		}
	
	return ETrue;
}	
	
TBool ROmxScriptTest::MosSetClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState, TInt aStartTime, TInt aOffset, TUint32 aWaitMask)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_TIME_CONFIG_CLOCKSTATETYPE conf;
	conf.nVersion = KOmxVersion;
	conf.nSize = sizeof(conf);
	conf.eState = aClockState;
	conf.nStartTime = aStartTime;
	conf.nOffset = aOffset;
	conf.nWaitMask = aWaitMask;

	OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimeClockState, &conf);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimeClockState)"), error);
		return EFalse;
		}
	
	return ETrue;
	}
	
// Buffer component handler
TBool ROmxScriptTest::InitialiseBufferHandlerL(const TDesC8& aComp, TDesC& aName, const TDesC8* aType)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	// Buffer component
	_LIT8(extensionBuf1, "OMX.SYMBIAN.INDEX.PARAM.BUFFERCHUNK");
	TBuf8<45> extension;
	extension.Copy(extensionBuf1); 
	
	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);
	if (error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetExtensionIndex"), error);
		return EFalse;
		}

	OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA chunkData;
	chunkData.nSize = sizeof(chunkData);
	chunkData.nVersion = KOmxVersion;
	chunkData.nPortIndex = 0;
	error = component->GetParameter(component, indexType, &chunkData);
	if (error != OMX_ErrorNone)
	   {
	   FailWithOmxError(_L("GetParameter"), error);
	   return EFalse;
	   }   
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = 0;
		
	error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}

	OMX_SYMBIAN_PARAM_BUFFER_SIZE bufferSize;
	bufferSize.nSize = sizeof(bufferSize);
	bufferSize.nVersion = KOmxVersion;
	bufferSize.nPortIndex = 0;

	error = component->GetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferSize), &bufferSize);

	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}

   	if ((aType != NULL) && (*aType == _L8("mpeg4")))
		{
		iBufferHandler = CVideoBufferHandlerMPEG4::NewL(aName, chunkData, bufferSize, portDef);
		}
	else
		{
		iBufferHandler = CVideoBufferHandler::NewL(aName, chunkData, bufferSize, portDef);
		}

    return ETrue;
	}
	
void ROmxScriptTest::StartBufferHandler()
	{
	if ( !iBufferHandler )
		{
		ERR_PRINTF1(_L("ROmxScriptTest::StartBufferHander() Exception: Buffer handler not created"));
		StopTest(EFail);	
		}
	
	iBufferHandler->Start();
	}
	
		
void ROmxScriptTest::SendInvalidBufferId(TInt aInvalidId)
	{
	if ( !iBufferHandler )
		{
		ERR_PRINTF1(_L("ROmxScriptTest::SendInvalidBufferId() Exception: Buffer handler not created"));
		StopTest(EFail);	
		}
	
	iBufferHandler->SetSendInvalidBufferId(aInvalidId);
	iBufferHandler->Start();
	}	
	
	
	

void ROmxScriptTest::StopBufferHandler()
	{
	if ( !iBufferHandler )
		{
		ERR_PRINTF1(_L("ROmxScriptTest::StopBufferHandler() Exception: Buffer handler not stopped"));
		StopTest(EFail);	
		}
		
	iBufferHandler->Cancel();
	}

// Buffer sink component handler
TBool ROmxScriptTest::InitialiseBufferSinkHandlerL(const TDesC8& aComp, TDesC& aName)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	// Buffer component
	_LIT8(extensionBuf1, "OMX.SYMBIAN.INDEX.PARAM.BUFFERCHUNK");
	TBuf8<45> extension;
	extension.Copy(extensionBuf1); 
	
	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);
	if (error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetExtensionIndex"), error);
		return EFalse;
		}

	OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA chunkData;
	chunkData.nSize = sizeof(chunkData);
	chunkData.nVersion = KOmxVersion;
	chunkData.nPortIndex = 0;
    error = component->GetParameter(component, indexType, &chunkData);
	if (error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nSize = sizeof(portDef);
	portDef.nVersion = KOmxVersion;
	portDef.nPortIndex = 0;
		
	error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}

	OMX_SYMBIAN_PARAM_BUFFER_SIZE bufferSize;
	bufferSize.nSize = sizeof(bufferSize);
	bufferSize.nVersion = KOmxVersion;
	bufferSize.nPortIndex = 0;

	error = component->GetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferSize), &bufferSize);

	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("GetParameter"), error);
		return EFalse;
		}

    iBufferSinkHandler = CVideoBufferSinkHandler::NewL(aName, chunkData, bufferSize, portDef);
    
    return ETrue;
	}	

void ROmxScriptTest::StartBufferSinkHandler()
	{
	if ( !iBufferSinkHandler )
		{
		ERR_PRINTF1(_L("ROmxScriptTest::StartBufferSinkHandler() Exception: Buffer sink handler not created"));
		StopTest(EFail);	
		}
	
	iBufferSinkHandler->Start();
	}

void ROmxScriptTest::StopBufferSinkHandler()
	{
	if ( !iBufferSinkHandler )
		{
		ERR_PRINTF1(_L("ROmxScriptTest::StopBufferSinkHandler() Exception: Buffer sink handler not stopped"));
		StopTest(EFail);	
		}
		
	iBufferSinkHandler->Cancel();
	}	
	
TBool ROmxScriptTest::MosNegativeSetDataChunk(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError, TInt /*aExpectedSystemError*/)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	// Buffer component
	_LIT8(extensionBuf1, "OMX.SYMBIAN.INDEX.PARAM.BUFFERCHUNK");
	    TBuf8<45> extension;
	    extension.Copy(extensionBuf1); 
	
	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);
	if (error != OMX_ErrorNone)
	   {
	   FailWithOmxError(_L("GetExtensionIndex"), error);
	   return EFalse;
	   }
	    
	OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA chunkData;
	chunkData.nSize = sizeof(chunkData);
	chunkData.nVersion = KOmxVersion;
	chunkData.nPortIndex = 0;
	error = component->GetParameter(component, indexType, &chunkData);
	    
	if (error != OMX_ErrorNone)
	   {
	   FailWithOmxError(_L("GetParameter"), error);
	   return EFalse;
	   }
	
	 if (NULL == chunkData.nChunk )
	    {
	    FailWithOmxError(_L("NULL TBufSrcComponentHandles"), OMX_ErrorInvalidState);
	    return EFalse;
	    }
	 
	 chunkData.nChunk = 666; 
	 error = component->SetParameter(component, indexType , &chunkData);
	
    if (aExpectedError != error)
        {
        FailWithOmxError(_L("SetParameter"), error);
		return EFalse;
        }  
	return ETrue;   
    }
    
TBool ROmxScriptTest::MosChangeFilledBufferLength(const TDesC8& aComp, TInt aValue)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	iBufferHandler->ChangeFilledBufferLength(aValue);
    
    return ETrue;    
    }
    
TBool ROmxScriptTest::MosSetOMX_SymbianIndexParamBufferMsgQueueData(const TDesC8& aComp, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
    OMX_SYMBIAN_PARAM_BUFFER_MSGQUEUE bufferMsgQueue;
    bufferMsgQueue.nSize = sizeof(bufferMsgQueue);
    bufferMsgQueue.nVersion = KOmxVersion;
    bufferMsgQueue.nPortIndex = 0;
		
	OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferMsgQueueData), &bufferMsgQueue);
	if(aExpectedError != error)
		{
		ERR_PRINTF3(_L("MosSetOMX_SymbianIndexParamBufferMsgQueueData SetParameter() error 0x%X, expected 0x%X"), error, aExpectedError);
		return EFalse;
		}
	return ETrue;
    }

TBool ROmxScriptTest::MosSetBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aData, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	OMX_SYMBIAN_PARAM_BUFFER_SIZE bufferSize;
    bufferSize.nSize = sizeof(bufferSize);
    bufferSize.nVersion = KOmxVersion;
    bufferSize.nPortIndex = aPortIndex;
    bufferSize.nBufferSize = aData;
                   
    OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferSize), &bufferSize);
	    if(aExpectedError != error)
	        {
	        ERR_PRINTF3(_L("MosSetBufferSize SetParameter() error 0x%X, expected 0x%X"), error, aExpectedError);
	        return EFalse;
	        }
	    return ETrue;
	    }
	

TBool ROmxScriptTest::MosGetAndCompareBufferCount(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nVersion = KOmxVersion;
	portDef.nSize = sizeof(portDef);
	portDef.nPortIndex = aPortIndex;
	
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter()"), error);
		return EFalse;
		}
    if (aDataToCompare != portDef.nBufferCountActual)
        {
        ERR_PRINTF3(_L("Compare buffer source count - expecting %d, actual %d"), aDataToCompare, portDef.nBufferCountActual);
		return EFalse;
        }
    
	return ETrue;
    }

TBool ROmxScriptTest::MosGetAndCompareBufferSize(const TDesC8& aComp, TInt aPortIndex, TInt aDataToCompare)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_SYMBIAN_PARAM_BUFFER_SIZE bufferSize;
	bufferSize.nSize = sizeof(bufferSize);
	bufferSize.nVersion = KOmxVersion;
	bufferSize.nPortIndex = aPortIndex;

	OMX_ERRORTYPE error = component->GetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferSize), &bufferSize);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter()"), error);
		return EFalse;
		}
    if (bufferSize.nBufferSize != aDataToCompare)
        {
        ERR_PRINTF3(_L("Compare buffer source size - expecting %d, actual %d"), aDataToCompare, bufferSize.nBufferSize);
		return EFalse;
        }
    
	return ETrue;
    }

TBool ROmxScriptTest::MosSetVideoEncQuantL(const TDesC8& aComp, TInt aPortIndex, TInt aQpb)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_VIDEO_PARAM_QUANTIZATIONTYPE videoQuantization;
	videoQuantization.nSize = sizeof(videoQuantization);
	videoQuantization.nVersion = KOmxVersion;
	videoQuantization.nPortIndex = aPortIndex;
	videoQuantization.nQpB = aQpb;

	OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_IndexParamVideoQuantization), &videoQuantization);
	if(error)
		{
		FailWithOmxError(_L("MosSetVideoEncQuantL SetParameter()"), error);
		return EFalse;
		}
	
	error = component->GetParameter(component, OMX_IndexParamVideoQuantization, &videoQuantization);
	if(error)
		{
		FailWithOmxError(_L("MosSetVideoEncQuantL GetParameter()"), error);
		return EFalse;
		}
	
	if (videoQuantization.nQpB != aQpb)
		{
		ERR_PRINTF3(_L("MosSetVideoEncQuantL Compare QPB failed - Got %d, Expected %d"), videoQuantization.nQpB, aQpb);
		return EFalse;
		}

	return ETrue;
	}

TBool ROmxScriptTest::MosSetVideoEncMotionVectL(const TDesC8& aComp, TInt aPortIndex, 
		TInt aAccuracy,	TInt aSxSearchrange, TInt aSySearchrange, OMX_ERRORTYPE aExpectedError)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_VIDEO_PARAM_MOTIONVECTORTYPE motionVect;
	motionVect.nSize = sizeof(motionVect);
	motionVect.nVersion = KOmxVersion;
	motionVect.nPortIndex = aPortIndex;
	motionVect.eAccuracy = (OMX_VIDEO_MOTIONVECTORTYPE)aAccuracy;
	motionVect.sXSearchRange = aSxSearchrange;
	motionVect.sYSearchRange = aSySearchrange;

	OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_IndexParamVideoMotionVector), &motionVect);
	if(aExpectedError != error)
		{
		ERR_PRINTF3(_L("MosSetVideoEncMotionVectL SetParameter() returned error 0x%X, expected error 0x%X"), error, aExpectedError);
		return EFalse;
		}
	
	if (aExpectedError)
		{
		return ETrue;
		}
	
	error = component->GetParameter(component, OMX_IndexParamVideoMotionVector, &motionVect);
	if(error)
		{
		FailWithOmxError(_L("MosSetVideoEncMotionVectL GetParameter()"), error);
		return EFalse;
		}
	
	if (motionVect.eAccuracy != aAccuracy || motionVect.sXSearchRange != aSxSearchrange ||
			motionVect.sYSearchRange != aSySearchrange)
		{
		ERR_PRINTF1(_L("MosSetVideoEncMotionVectL GetParameter() did not return expeccted value"));
		return EFalse;
		}

	return ETrue;
	}

TBool ROmxScriptTest::MosSetVideoEncMpeg4TypeL(const TDesC8& aComp, TInt aPortIndex, 
		OMX_VIDEO_MPEG4PROFILETYPE aMpeg4Profile, OMX_VIDEO_MPEG4LEVELTYPE aMpeg4Level, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_VIDEO_PARAM_MPEG4TYPE mpeg4;
	mpeg4.nSize = sizeof(mpeg4);
	mpeg4.nVersion = KOmxVersion;
	mpeg4.nPortIndex = aPortIndex;
	mpeg4.eProfile = aMpeg4Profile;
	mpeg4.eLevel = aMpeg4Level;
	
	OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_IndexParamVideoMpeg4), &mpeg4);
	if(aExpectedError != error)
		{
		ERR_PRINTF3(_L("MosSetVideoEncMpeg4TypeL SetParameter() returned error 0x%X, expected error 0x%X"), error, aExpectedError);
		return EFalse;
		}
	if (aExpectedError)
		{
		return ETrue;
		}
	
	error = component->GetParameter(component, OMX_IndexParamVideoMpeg4, &mpeg4);
	if(error)
		{
		FailWithOmxError(_L("MosSetVideoEncMpeg4TypeL GetParameter()"), error);
		return EFalse;
		}
	
	if (mpeg4.eProfile != aMpeg4Profile || mpeg4.eLevel != aMpeg4Level)
		{
		ERR_PRINTF1(_L("MosSetVideoEncMpeg4TypeL GetParameter() did not return expeccted value"));
		return EFalse;
		}
	return ETrue;
    }

TBool ROmxScriptTest::MosSetVideoEncBitRateL(const TDesC8& aComp, TInt aPortIndex, 
		OMX_VIDEO_CONTROLRATETYPE aControlRate, TInt aTargetBitrate, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_VIDEO_PARAM_BITRATETYPE bitRate;
	bitRate.nSize = sizeof(bitRate);
	bitRate.nVersion = KOmxVersion;
	bitRate.nPortIndex = aPortIndex;
	bitRate.eControlRate = aControlRate;
	bitRate.nTargetBitrate = aTargetBitrate;
	
	OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_IndexParamVideoBitrate), &bitRate);
	if(aExpectedError != error)
		{
		ERR_PRINTF3(_L("MosSetVideoEncBitRateL SetParameter() returned error 0x%X, expected error 0x%X"), error, aExpectedError);
		return EFalse;
		}
	if (aExpectedError)
		{
		return ETrue;
		}
	
	error = component->GetParameter(component, OMX_IndexParamVideoBitrate, &bitRate);
	if(error)
		{
		FailWithOmxError(_L("MosSetVideoEncBitRateL GetParameter()"), error);
		return EFalse;
		}
	
	if (bitRate.eControlRate != aControlRate || bitRate.nTargetBitrate != aTargetBitrate)
		{
		ERR_PRINTF1(_L("MosSetVideoEncBitRateL GetParameter() did not return expeccted value"));
		return EFalse;
		}
	return ETrue;
    }

void ROmxScriptTest::WriteInt32(TUint8* aPtr, TInt32 aData)
	{
	aPtr[0] = TUint8(aData>>24);
	aPtr[1] = TUint8(aData>>16);
	aPtr[2] = TUint8(aData>>8);
	aPtr[3] = TUint8(aData);
	}
	

TBool ROmxScriptTest::ShortNameMatchComparison(const TDesC8* aShortName, const CComponentInfo& aComponentInfo)
    {
    if (*aShortName == *aComponentInfo.iShortName)
        {
        return ETrue;
        }
    return EFalse;
    }

TBool ROmxScriptTest::ComponentLogicalIL(const TDesC8& aName)
    {
    for(TInt index = 0, count = iComponents.Count(); index < count; index++)
        {
        CComponentInfo* component = iComponents[index];
        if(*(component->iShortName) == aName)
            {
            if(component->iComponentName->FindC(KILTypeString()) == KErrNotFound)
                {
                // is not LIL
                return EFalse;
                }
            else
                {
                // is LIL
                return ETrue;
                }
            }
        }
    ERR_PRINTF1(_L("LogicalILComponent() - Component not found"));
    StopTest(EFail);
    
    return EFalse;
    }

TBool ROmxScriptTest::ComponentPhysicalIL(const TDesC8& aName)
    {
    for(TInt index = 0, count = iComponents.Count(); index < count; index++)
        {
        CComponentInfo* component = iComponents[index];
        if(*(component->iShortName) == aName)
            {
            if(component->iComponentName->FindC(KILTypeString) == KErrNotFound)
                {
                // is PIL
                return ETrue;
                }
            else
                {
                // is not PIL
                return EFalse;
                }
            }
        }
    ERR_PRINTF1(_L("PhysicalILComponent() - Component not found"));
    StopTest(EFail);
    
    return EFalse;
    }
    
OMX_COMPONENTTYPE* ROmxScriptTest::ComponentByName(const TDesC8& aName)
	{
	for(TInt index = 0, count = iComponents.Count(); index < count; index++)
		{
		CComponentInfo* component = iComponents[index];
		if(*(component->iShortName) == aName)
			{
			return component->iComponent;
			}
		}
	HBufC* name = HBufC::New(aName.Length());
	if(!name)
		{
		ERR_PRINTF1(_L("Component not found"));
		StopTest(EFail);
		}
	else
		{
		name->Des().Copy(aName);
		ERR_PRINTF2(_L("Component %S not found"), name);
		delete name;
		StopTest(EFail);
		}
	return NULL;
	}



void ROmxScriptTest::GetComponentName(const OMX_COMPONENTTYPE* aHandle, TDes& aName)
	{
	for(TInt index = 0, count = iComponents.Count(); index < count; index++)
		{
		const CComponentInfo* component = iComponents[index];
		if(component->iComponent == aHandle)
			{
			aName.Copy(*(component->iShortName));
			return;
			}
		}
	ERR_PRINTF1(_L("Invalid component handle"));
	StopTest(EFail);
	aName = _L("<invalid handle>");
	}

void ROmxScriptTest::FailWithOmxError(const TDesC& aOperation, OMX_ERRORTYPE aError)
	{	
	if(aError != OMX_ErrorNone)
		{
		TBuf<64> errorDes;
		OMXUtil::ErrorDes(aError, errorDes);
		ERR_PRINTF3(_L("%S returned %S"), &aOperation, &errorDes);
		StopTest(EFail);
		}
	}

void ROmxScriptTest::FailWithOmxError(const TDesC8& aComponent, const TDesC& aOperation, OMX_ERRORTYPE aError)
	{
	if(aError != OMX_ErrorNone)
		{
		TBuf<64> errorDes;
		TBuf<64> componentDes16;
		componentDes16.Copy(aComponent);
		OMXUtil::ErrorDes(aError, errorDes);
		ERR_PRINTF4(_L("%S->%S returned %S"), &componentDes16, &aOperation, &errorDes);
		StopTest(EFail);
		}
	}

void ROmxScriptTest::AllComponentsEOS()
	{
	EndWait();
	}

void ROmxScriptTest::AllComponentsTransitioned(OMX_STATETYPE aNewState,
			                                   OMX_STATETYPE aOldState)
	{
	__ASSERT_ALWAYS(aOldState == iState, User::Invariant());
	iState = aNewState;
	EndWait();
	}

void ROmxScriptTest::EventReceived(OMX_COMPONENTTYPE* aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aEventData)
	{

	if ((iMsgQueue.Handle()) && (aEvent == OMX_EventCmdComplete))
		{		
		iMsgQueue.Close();
		}
	
	
	// If the Graphic Sink has created a surface, assign it to the window
	if(aEvent == OMX_EventPortSettingsChanged)
		{
		if(aComponent == iGraphicSink)
		    {
			if (aData1 == iExtSurfaceConfigIndex ||
			    aData1 == OMX_IndexConfigCommonScale ||
			    aData1 == OMX_IndexConfigCommonOutputSize ||
			    aData1 == OMX_IndexConfigCommonInputCrop ||
			    aData1 == OMX_IndexConfigCommonOutputCrop ||
			    aData1 == OMX_IndexConfigCommonExclusionRect)
				{
				INFO_PRINTF1(_L("Surface configuration changed"));

				OMX_SYMBIAN_VIDEO_PARAM_SURFACECONFIGURATION surfaceConfigParam;
				surfaceConfigParam.nSize = sizeof(OMX_SYMBIAN_VIDEO_PARAM_SURFACECONFIGURATION);
				surfaceConfigParam.nVersion = KOmxVersion;
				surfaceConfigParam.nPortIndex = 0;
				
				OMX_ERRORTYPE omxError = aComponent->GetParameter(aComponent, iExtSurfaceConfigIndex, &surfaceConfigParam);
				if(omxError != OMX_ErrorNone)
					{
					FailWithOmxError(_L("GetParameter(OMX_SymbianIndexParamVideoGFXSurfaceConfig)"), omxError);
					return;
					}
				TSurfaceConfiguration& surfaceConfig = *reinterpret_cast<TSurfaceConfiguration*>(surfaceConfigParam.pSurfaceConfig);
				
				OMX_PARAM_PORTDEFINITIONTYPE portDef;
				portDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
				portDef.nVersion = KOmxVersion;
				portDef.nPortIndex = 0;
				omxError = aComponent->GetParameter(aComponent, OMX_IndexParamPortDefinition, &portDef);
				if(omxError != OMX_ErrorNone)
					{
					FailWithOmxError(_L("GetParameter(OMX_IndexParamPortDefinition)"), omxError);
					return;
					}
				TSize inputSize(portDef.format.video.nFrameWidth, portDef.format.video.nFrameHeight);
				INFO_PRINTF3(_L("Input size is %dx%d"), inputSize.iWidth, inputSize.iHeight);
				
				TSurfaceId surfaceId;
				surfaceConfig.GetSurfaceId(surfaceId);
				TInt err = iWindowManager->RegisterSurface(surfaceId);
				if(KErrNone != err)
					{
					ERR_PRINTF1(_L("RegisterSurface failed"));
					StopTest(err, EFail);
					}

				// scale, rotate and centre to fit
				// assumes square pixels
				
				TSize screenSize = iWindowManager->ScreenSize();
								
				TBool iInputLandscape = inputSize.iWidth > inputSize.iHeight;
				TBool iScreenLandscape = screenSize.iWidth > screenSize.iHeight;
				
				if (iVideoFitMode == EVideoFitRotateScaleAndCentre &&
					(iInputLandscape ^ iScreenLandscape))
					{
					surfaceConfig.SetOrientation(CFbsBitGc::EGraphicsOrientationRotated270);
					TInt tmp = inputSize.iWidth;
					inputSize.iWidth = inputSize.iHeight;
					inputSize.iHeight = tmp;
					}
				
				TRect rect;
				if( iVideoFitMode == EVideoFitScaleAndCentre ||
					iVideoFitMode == EVideoFitRotateScaleAndCentre)
					{
					if(!iScreenLandscape)
						{
						TInt displayHeight = screenSize.iWidth  * inputSize.iHeight / inputSize.iWidth;
						TInt top = screenSize.iHeight - displayHeight >> 1;
						TInt bottom = top + displayHeight;
						rect = TRect(0, top, screenSize.iWidth, bottom);
						}
					else
						{
						TInt displayWidth = screenSize.iHeight * inputSize.iWidth / inputSize.iHeight;
						TInt left = screenSize.iWidth - displayWidth >> 1;
						TInt right = left + displayWidth;
						rect = TRect(left, 0, right, screenSize.iHeight);
						}
					}
				else
					{
					TInt left = screenSize.iWidth - inputSize.iWidth >> 1;
					TInt top = screenSize.iHeight - inputSize.iHeight >> 1;
					rect = TRect(left, top, left + inputSize.iWidth, top + inputSize.iHeight);
					}
				
				surfaceConfig.SetExtent(rect);
				
				err = iWindowManager->SetBackgroundSurface(surfaceConfig, ETrue);
				if(KErrNone != err)
					{
					ERR_PRINTF2(_L("SetBackgroundSurface failed (%d)"), err);
					StopTest(err, EFail);
					return;
					}
				INFO_PRINTF1(_L("SetBackgroundSurface OK"));
				iWindowManager->Flush();
				return;
				}
			}
		}
	
	// ignore buffer flag events except EOS
	if((aEvent == OMX_EventBufferFlag && (aData2 & OMX_BUFFERFLAG_EOS) == 0) /*|| (aEvent == OMX_EventNokiaFirstFrameDisplayed)*/)
		{
		return;
		}
	
	TInt index = 0;
	TInt count = iExpectedEvents.Count();
	for(; index < count; index++)
		{
		const TExpectedEvent& event = iExpectedEvents[index];
		if( event.iComponent == aComponent &&
		    event.iEvent == aEvent &&
			event.iData1 == aData1 &&
			event.iData2 == aData2 &&
			event.iEventData == aEventData)
			{
			break;
			}
		}
	if(index == count)
		{
		// event was not expected, see if we can ignore this event
		TInt count1 = iIgnoredEvents.Count();
		TInt index1 = 0;
		for(; index1 < count1; index1++)
		    {
		    const TExpectedEvent& event = iIgnoredEvents[index1];
		    if( event.iComponent == aComponent &&
		              event.iEvent == aEvent &&
		              event.iData1 == aData1 &&
		              event.iData2 == aData2 &&
		              event.iEventData == aEventData)
		        {
		        return;
		        }
		    }
		 
		// event was not expected and we can't ignore the event, so fail the test		
		TBuf<64> name;
		GetComponentName(aComponent, name);
		char hexBuf[11];
		TBuf<32> eventWideChar;
		eventWideChar.Copy(TPtrC8((unsigned char*) format_OMX_EVENTTYPE(aEvent, hexBuf)));
		ERR_PRINTF2(_L("Unexpected event received from component %S"), &name);
		ERR_PRINTF5(_L("aEvent=%S aData1=0x%08X aData2=0x%08X, aEventData=0x%08X"), &eventWideChar, aData1, aData2, aEventData);
		StopTest(EFail);
		return;
		}
	iExpectedEvents.Remove(index);
	if(iWaitingForEvents && iExpectedEvents.Count() == 0)
		{
		EndWait();
		}
	}

void ROmxScriptTest::ComponentTransitioned(OMX_STATETYPE aNewState,
			                                     OMX_STATETYPE aOldState)
	{
	__ASSERT_ALWAYS(aOldState == iState, User::Invariant());
	iState = aNewState;
	EndWait();
	}

void ROmxScriptTest::BufferDone(OMX_COMPONENTTYPE* aComponent, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	if (iNonTunneledHandler)
		{
		iNonTunneledHandler->BufferDone(aComponent, aBufHdr, aSource);
		}
	
	if(iPortBufferHandler)
	    {
	    iPortBufferHandler->BufferDone(aComponent, aBufHdr, aSource);
	    }
		
	}

void ROmxScriptTest::TimerExpired()
	{
	EndWait();
	}

void ROmxScriptTest::HashFileL(const TDesC& aFileName, TDes8& aHash, void (*aFilter1)(RBuf8&), void (*aFilter2)(RBuf8&), void (*aFilter3)(RBuf8&))
    {
    const TInt KBufferSize = 1024;
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    RFile file;
    User::LeaveIfError(file.Open(fs, aFileName, EFileRead | EFileShareReadersOnly));
    CleanupClosePushL(file);
    
    RBuf8 buf;
    User::LeaveIfError(buf.Create(KBufferSize));
    CleanupClosePushL(buf);

    CMD5* md5 = CMD5::NewL();
    CleanupStack::PushL(md5);
    
    while(true)
        {
        User::LeaveIfError(file.Read(buf));
        if(buf.Length() == 0)
            {
            break;
            }
        
        if (aFilter1) aFilter1(buf);
        if (aFilter2) aFilter2(buf);
        if (aFilter3) aFilter3(buf);
        
        md5->Hash(buf);
        }
    
    aHash = md5->Final();
    
    CleanupStack::PopAndDestroy(4); // fs, file, buf, md5
    }



TBool ROmxScriptTest::HandleNonTunneledBuffers(OMX_STATETYPE aNewState, OMX_COMPONENTTYPE* aComp)
	{
	if (iNonTunneledHandler)
		{
		if (iState == OMX_StateLoaded && aNewState == OMX_StateIdle)
			{
			TRAPD(err, iNonTunneledHandler->AllocateBuffersL(aComp));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Non-tunneled AllocateBuffersL failed (%d)"), err);
				return EFalse;
				}
			}
		else if (iState == OMX_StateIdle && aNewState == OMX_StateExecuting)
			{
			TRAPD(err, iNonTunneledHandler->FillBuffers(aComp));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Non-tunneled FillBuffersL failed (%d)"), err);
				return EFalse;
				}		
			}
		else if (iState == OMX_StateExecuting && aNewState == OMX_StateIdle)
			{
			TRAPD(err, iNonTunneledHandler->HoldBuffers(aComp));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Non-tunneled HoldBuffersL failed (%d)"), err);
				return EFalse;
				}
			}
		else if (iState == OMX_StateIdle && aNewState == OMX_StateLoaded)
			{
			TRAPD(err, iNonTunneledHandler->FreeBuffers(aComp));
			if (err != KErrNone)
				{
				ERR_PRINTF2(_L("Non-tunneled FreeBuffersL failed (%d)"), err);
				return EFalse;
				}
			}
		}
	
	return ETrue;
	}


TBool ROmxScriptTest::HandlePortBufferHandler(OMX_STATETYPE aNewState, OMX_COMPONENTTYPE* aComp)
    {
    if(iPortBufferHandler)
        {
        if (iState == OMX_StateLoaded && aNewState == OMX_StateIdle)
            {
            TRAPD(err, iPortBufferHandler->AllocateBuffersL(aComp));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("PortBufferHandler AllocateBuffersL failed (%d)"), err);
                return EFalse;
                }
            }
        else if (iState == OMX_StateIdle && aNewState == OMX_StateExecuting)
            {

            }
        else if (iState == OMX_StateExecuting && aNewState == OMX_StateIdle)
            {
            }
        else if (iState == OMX_StateIdle && aNewState == OMX_StateLoaded)
            {
            TRAPD(err, iPortBufferHandler->FreeBuffers(aComp));
            if (err != KErrNone)
                {
                ERR_PRINTF2(_L("PortBufferHandler FreeBuffersL failed (%d)"), err);
                return EFalse;
                }
            }
        }
    return ETrue;
    }

TBool ROmxScriptTest::MosFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandFlush, aPortIndex, NULL);
	if(error != aExpectedError)
		{
		ERR_PRINTF3(_L("MosFlushBuffer SendCommand() error 0x%X, expected 0x%X"), error, aExpectedError);
		return EFalse;
		}

    return ETrue;
    }

/*
 * This is a test to force the buffer(s) to require being flushed.
 */
TBool ROmxScriptTest::MosForceBufferSourceFlushBufferL(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError)
	{
	// setup the msg queue
	TBuf<255> compWide;
	compWide.Copy(aComp);
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	_LIT8(extensionBuf1, "OMX.SYMBIAN.INDEX.PARAM.BUFFERCHUNK");
	TBuf8<45> extension;
	extension.Copy(extensionBuf1); 

	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);
	
	if (error != OMX_ErrorNone)
	    {
	    FailWithOmxError(_L("GetExtensionIndex"), error);
	    return EFalse;
	    }
                
	OMX_SYMBIAN_PARAM_3PLANE_CHUNK_DATA chunkData;

	chunkData.nSize = sizeof(chunkData);
	chunkData.nVersion = KOmxVersion;
	chunkData.nPortIndex = 0;
	error = component->GetParameter(component, indexType, &chunkData);
	
	if (error != OMX_ErrorNone)
	    {
	    FailWithOmxError(_L("GetParameter"), error);
	    return EFalse;
	    }
	     
	if (NULL == chunkData.nChunk )
	    {
	    FailWithOmxError(_L("NULL TBufSrcComponentHandles"), OMX_ErrorInvalidState);
	    return EFalse;
	    }

	OMX_SYMBIAN_PARAM_BUFFER_SIZE bufferSize;
	bufferSize.nSize = sizeof(bufferSize);
	bufferSize.nVersion = KOmxVersion;
	bufferSize.nPortIndex = 0;
	error = component->GetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamBufferSize), &bufferSize);

	if(error != OMX_ErrorNone)
	    {
	    FailWithOmxError(_L("GetParameter"), error);
	    return EFalse;
	    }

	RChunk msgsChunk;
	msgsChunk.SetHandle(chunkData.nChunk);
	CleanupClosePushL(msgsChunk);
	User::LeaveIfError(msgsChunk.Duplicate(RThread()));
	 
	iMsgQueue.SetHandle(chunkData.nFilledBufferQueue);
	User::LeaveIfError(iMsgQueue.Duplicate(RThread()));
	 
	// fill it up with 2 messages
	TInt numMessages = 2;   // change this to alter the number of buffers stacked up.
	TInt message = 0;
	TMMSharedChunkBufConfig bufConfig;
	bufConfig.iNumBuffers = numMessages;	
	bufConfig.iBufferSizeInBytes = bufferSize.nBufferSize;  //passed into ConstructL()
	TUint8* chunkBase = msgsChunk.Base(); 
	__ASSERT_ALWAYS(message < numMessages, User::Invariant());

	TUint8* bufBase1 = chunkBase + (message * (bufConfig.iBufferSizeInBytes));
	TFilledBufferHeaderV2 header1;
	header1.iOffset = bufBase1 - chunkBase;
	header1.iFilledLength = 3;
	header1.iTimeStamp = 0;
	header1.iFlags = ETrue ? OMX_BUFFERFLAG_STARTTIME : 0;  //is the first buffer.

	TInt sendError = iMsgQueue.Send(header1);

	if (sendError != KErrNone)
		{
		ERR_PRINTF2(_L("MosForceBufferSourceFlushBuffer MsgQueue Send() error 0x%X"), sendError);
		iMsgQueue.Close();
		CleanupStack::PopAndDestroy(); // msgsChunk
		return EFalse;
		}

	message++;
	TUint8* bufBase2 = chunkBase + (message * (bufConfig.iBufferSizeInBytes));
	TFilledBufferHeaderV2 header2;
	header2.iOffset = bufBase2 - chunkBase;
	header2.iFilledLength = 3;
	header2.iTimeStamp = 0;
	header2.iFlags = EFalse ? OMX_BUFFERFLAG_STARTTIME : 0;  //is NOT the first buffer.
	User::LeaveIfError(iMsgQueue.Send(header2));		

	// flush them while they're still outstanding
	error = component->SendCommand(component, OMX_CommandFlush, aPortIndex, NULL);
	if(error != aExpectedError)
		{
		ERR_PRINTF3(_L("MosForceBufferSourceFlushBuffer SendCommand() error 0x%X, expected 0x%X"), error, aExpectedError);
		iMsgQueue.Close();
		CleanupStack::PopAndDestroy(); // msgsChunk
		return EFalse;
		}
	
	CleanupStack::PopAndDestroy(); // msgsChunk
	return ETrue;	
	}



TBool ROmxScriptTest::MosForceBufferSinkFlushBuffer(const TDesC8& aComp, TInt aPortIndex, OMX_ERRORTYPE aExpectedError)
	{


	TBuf<255> compWide;
	compWide.Copy(aComp);
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		

	OMX_ERRORTYPE error = component->SendCommand(component, OMX_CommandFlush, aPortIndex, NULL);
	if(error != aExpectedError)
		{
		ERR_PRINTF3(_L("MosForceFlushBufferSinkBuffer error 0x%X, expected 0x%X"), error, aExpectedError);
		iMsgQueue.Close();
		return EFalse;
		}

	return ETrue;	
	}



TBool ROmxScriptTest::MosSetActiveStream(const TDesC8& aComp, TUint32 aPortIndex)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nVersion = KOmxVersion;
	portDef.nSize = sizeof(portDef);
	portDef.nPortIndex = aPortIndex;
	
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamPortDefinition, &portDef);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter()"), error);
		return EFalse;
		}
		
	error = component->SetParameter(component, OMX_IndexParamActiveStream, &portDef);
	if((error != OMX_ErrorNone) && (error != OMX_ErrorUnsupportedSetting))
		{
		FailWithOmxError(_L("SetParameter"), error);
		return EFalse;
		}
	return ETrue;
    }

TBool ROmxScriptTest::MosGetActiveStream(const TDesC8& aComp, TUint32 aPortIndex)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_PARAM_PORTDEFINITIONTYPE portDef;
	portDef.nVersion = KOmxVersion;
	portDef.nSize = sizeof(portDef);
	portDef.nPortIndex = aPortIndex;

	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamActiveStream, &portDef);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter()"), error);
		return EFalse;
		}
		
	return ETrue;
    }
    

TBool ROmxScriptTest::MosSetVideoFitModeL(TVideoFitMode aVideoFitMode)
	{
	iVideoFitMode = aVideoFitMode;
	return ETrue;
	}

// FIXME duplication with MosSetAacAudioPortDefL
TBool ROmxScriptTest::MosSetAACProfileL(const TDesC8& aComp, TInt aPortIndex, TInt aChannels, TInt aSamplingRate, TInt aBitRate, TInt aAudioBandwidth, TInt aFrameLength, TInt aAACTools, TInt aAACERTools, OMX_AUDIO_AACPROFILETYPE aProfile, OMX_AUDIO_AACSTREAMFORMATTYPE aStreamFormat, OMX_AUDIO_CHANNELMODETYPE aChannelMode)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
	
	OMX_AUDIO_PARAM_AACPROFILETYPE param;
	param.nSize = sizeof(param);
	param.nVersion = KOmxVersion;
	param.nPortIndex = aPortIndex;
	
	OMX_ERRORTYPE error = OMX_GetParameter(component, OMX_IndexParamAudioAac, &param);
	if(error)
		{
		FailWithOmxError(_L("OMX_GetParameter(AudioAAC)"), error);
		return EFalse;
		}
	
	// TODO don't overwrite unspecified values
	param.nChannels = aChannels;
	param.nSampleRate = aSamplingRate;
	param.nBitRate = aBitRate;
	param.nAudioBandWidth = aAudioBandwidth;
	param.nFrameLength = aFrameLength;
	param.nAACtools = aAACTools;
	param.nAACERtools = aAACERTools;
	param.eAACProfile = aProfile;
	param.eAACStreamFormat = aStreamFormat;
	param.eChannelMode = aChannelMode;
	
	error = OMX_SetParameter(component, OMX_IndexParamAudioAac, &param);
	if(error)
		{
		FailWithOmxError(_L("OMX_SetParameter(AudioAAC)"), error);
		return EFalse;
		}
	return ETrue;
	}

TBool ROmxScriptTest::MosGetExtensionIndex(const TDesC8& aComp, const TDesC8& aParameterName, OMX_ERRORTYPE aExpectedError)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}
		
	TBuf8<128> extension;
	extension.Copy(aParameterName); 
	
	OMX_INDEXTYPE indexType = OMX_IndexMax;
	OMX_ERRORTYPE error = component->GetExtensionIndex(component, (OMX_STRING) extension.PtrZ(), &indexType);
	if(error != aExpectedError)
		{
		ERR_PRINTF3(_L("MosGetExtensionIndex GetExtensionIndex() error 0x%X, expected 0x%X"), error, aExpectedError);
		return EFalse;
		}

	return ETrue;	
    }
    
TBool ROmxScriptTest::MosCheckTimeClockState(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aExpectedState)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    
	if (!component)
		{
		StopTest(EFail);
		return EFalse;
		}
	
	OMX_TIME_CONFIG_CLOCKSTATETYPE timeClockState;
	timeClockState.nSize = sizeof(timeClockState);
	timeClockState.nVersion = KOmxVersion;
    
	if(component->GetConfig(component, OMX_IndexConfigTimeClockState, &timeClockState) != OMX_ErrorNone)
		{
		StopTest(EFail);
		return EFalse;	
		}
	
	if (timeClockState.eState != aExpectedState)
		{
		ERR_PRINTF3(_L("Clock state %d does not match expected %d"), timeClockState.eState, aExpectedState);
		StopTest(EFail);
		return EFalse;	
		}
	
	return ETrue;
    }

TBool ROmxScriptTest::MosCheckMediaTime(const TDesC8& aComp, TInt aPortIndex, OMX_TICKS aMediaTime, TBool aMoreThan)
	{
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    
	if (!component)
		{
		StopTest(EFail);
		return EFalse;
		}

	OMX_TIME_CONFIG_TIMESTAMPTYPE timeInfo;
	timeInfo.nSize = sizeof(timeInfo);
	timeInfo.nVersion = KOmxVersion;
	timeInfo.nPortIndex = aPortIndex;

	if (component->GetConfig(component, OMX_IndexConfigTimeCurrentMediaTime, &timeInfo) != OMX_ErrorNone)
		{
		StopTest(EFail);
		return EFalse;	
		}

	if (aMoreThan)
	  {
	    INFO_PRINTF3(_L("MosCheckMediaTime() %ld > %ld"), timeInfo.nTimestamp, aMediaTime);

	    if (timeInfo.nTimestamp < aMediaTime)
		{
		INFO_PRINTF3(_L("FAILED! MosCheckMediaTime() Expecting %ld > %ld"), timeInfo.nTimestamp, aMediaTime);
		StopTest(EFail);
		return EFalse;	
		}
	  }
	else
	  {
	    INFO_PRINTF3(_L("MosCheckMediaTime() %ld < %ld"), timeInfo.nTimestamp, aMediaTime);

	    if (timeInfo.nTimestamp > aMediaTime)
		{
		INFO_PRINTF3(_L("FAILED! MosCheckMediaTime() expecting %ld < %ld"), timeInfo.nTimestamp, aMediaTime);
		StopTest(EFail);
		return EFalse;	
		}
	  }

	return ETrue;
	}

TBool ROmxScriptTest::MosBaseSupportPortL(const TDesC8& aComp, TInt aPortIndex)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->AddPortSupportL(aPortIndex);
 
    return ETrue;
    }

TBool ROmxScriptTest::MosBaseSetAutonomous(const TDesC8& aComp, TInt aPortIndex, TBool aEnabled)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->SetAutoMode(aPortIndex, aEnabled);
    return ETrue;   
    }

TBool ROmxScriptTest::MosBaseAllocateBuffersL(const TDesC8& aComp, TInt aPortIndex, TInt aNumberBuffers)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->SetupBuffersL(aPortIndex,aNumberBuffers);
    return ETrue;     
    }
	
TBool ROmxScriptTest::MosBaseFreeAllocatedBuffersL(const TDesC8& aComp)
	{
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
	iComponents[componentIndex]->iBaseHandler->FreeAllocatedBuffersL();
	return ETrue;
	}

TBool ROmxScriptTest::MosBaseSetBufSupplier(const TDesC8& aComp, TInt aPortIndex, TBool aComponentSupplier)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->SetBufferSupplier(aPortIndex, aComponentSupplier);
    return ETrue;     
    }

TBool ROmxScriptTest::MosBaseFillThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->FillThisBuffer(aPortIndex, aPortRelBufIndex);
    return ETrue;  
    }

TBool ROmxScriptTest::MosBaseEmptyThisBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->EmptyThisBuffer(aPortIndex, aPortRelBufIndex);
    return ETrue;  
    }

TBool ROmxScriptTest::MosBaseWaitForBuffer(const TDesC8& aComp, TInt aPortIndex, TInt aPortRelBufIndex)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    iComponents[componentIndex]->iBaseHandler->WaitForBufferCompletion(aPortIndex, aPortRelBufIndex);
    return ETrue;  
    }

TBool ROmxScriptTest::MosBaseTimestampPassClock(const TDesC8& aCompReceiving, const TDesC8& aClockCompToPass)
    {
    TInt receiveCompIndex = iComponents.Find(aCompReceiving, ROmxScriptTest::ShortNameMatchComparison);    
    if (receiveCompIndex == KErrNotFound)
        {
        return EFalse;
        }
    
    TInt clockCompIndex = iComponents.Find(aClockCompToPass, ROmxScriptTest::ShortNameMatchComparison);    
    if (clockCompIndex == KErrNotFound)
        {
        return EFalse;
        }
    
    CBaseProfileTimestampHandling* timestampBaseHandler = static_cast<CBaseProfileTimestampHandling*>(iComponents[receiveCompIndex]->iBaseHandler);
    timestampBaseHandler->SetClockComponent(iComponents[clockCompIndex]->iComponent);
        
    return ETrue;
    }

TBool ROmxScriptTest::MosBaseTimestampCheckTimestampL(const TDesC8& aComp, TInt aPortIndex, TUint aExpectedTime, TUint aTolerance)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    
    CBaseProfileTimestampHandling* timestampBaseHandler = static_cast<CBaseProfileTimestampHandling*>(iComponents[componentIndex]->iBaseHandler);
    timestampBaseHandler->QueueTimestampCheckL(aPortIndex, aExpectedTime, aTolerance);

    return ETrue;
    }

TBool ROmxScriptTest::MosBaseTimestampCompareWithRefClockL(const TDesC8& aComp, TInt aPortIndex, TUint aTolerance)
    {
    TInt componentIndex = iComponents.Find(aComp, ROmxScriptTest::ShortNameMatchComparison);    
    if (componentIndex == KErrNotFound)
        {
        return EFalse;
        }
    
    CBaseProfileTimestampHandling* timestampBaseHandler = static_cast<CBaseProfileTimestampHandling*>(iComponents[componentIndex]->iBaseHandler);
    timestampBaseHandler->QueueCompareWithRefClockL(aPortIndex, aTolerance);

    return ETrue;
    }


TBool ROmxScriptTest::MosStartBuffersforPort( const TDesC8& aComp, TInt aPortIndex)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
     if(!component)
         {
         return EFalse;
         }
     
     if(iPortBufferHandler)
         {
         iPortBufferHandler->Start(component,aPortIndex);
         }
     return ETrue;
    }

void ROmxScriptTest::BeginWait()
	{
	CActiveScheduler::Start();
	}

void ROmxScriptTest::EndWait()
	{
	// use a CAsyncCallBack to call CActiveScheduler::Stop
	// the main reason for doing this is to allow EndWait
	// to be called from any thread but to stop the scheduler
	// of the script parser thread.
	iStopSchedulerCallback->CallBack();
	}



//
TBool ROmxScriptTest::MosDeleteFileL(const TDesC& aFileName, TBool aFileMustExist)
	{
	RFs fs;
	
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = fs.Delete(aFileName);
	if (err != KErrNone)
		{
		if (err != KErrNotFound || aFileMustExist)
			{
			ERR_PRINTF3(_L("Error %d deleting file %S"), err, &aFileName);
			User::Leave(err);
			}
		}

	INFO_PRINTF2(_L("Deleted file %S"), &aFileName);

	CleanupStack::PopAndDestroy(&fs);
	return ETrue;
	}

//
//
//
TBool ROmxScriptTest::MosLogAllEventsL()
	{
	return ETrue;
	}

//
//
//
TBool ROmxScriptTest::MosSetSensorModeTypeL(const TDesC8& aComp, TInt aPort, TInt aFrameRate, TBool aOneShot, TInt aWidth, TInt aHeight)
	{
	OMX_COMPONENTTYPE* comp = ComponentByName(aComp);
	if (!comp)
		{
		return EFalse;
		}
	
	OMX_PARAM_SENSORMODETYPE sensor;
	sensor.nVersion = KOmxVersion;
	sensor.nSize = sizeof(OMX_PARAM_SENSORMODETYPE);
	sensor.nPortIndex = aPort;
	sensor.sFrameSize.nSize = sizeof(OMX_FRAMESIZETYPE);
	sensor.sFrameSize.nVersion = KOmxVersion;
	sensor.sFrameSize.nPortIndex = aPort;

	OMX_ERRORTYPE omxErr = comp->GetParameter(comp, OMX_IndexParamCommonSensorMode, &sensor);
	if (omxErr != OMX_ErrorNone)
		{
		ERR_PRINTF2(_L("Error %08X returned from GetParameter"), omxErr);
		return EFalse;
		}
		
	sensor.bOneShot = (aOneShot ? OMX_TRUE : OMX_FALSE);
	if (aFrameRate != -1) sensor.nFrameRate = aFrameRate;	
	if (aWidth != -1) sensor.sFrameSize.nWidth = aWidth;
	if (aHeight != -1) sensor.sFrameSize.nHeight = aHeight;
	
	omxErr = comp->SetParameter(comp, OMX_IndexParamCommonSensorMode, &sensor);
	if (omxErr != OMX_ErrorNone)
		{
		ERR_PRINTF2(_L("Error %08X returned from SetParameter"), omxErr);
		}
		
	return (omxErr == OMX_ErrorNone);
	}

//
//
//
TBool ROmxScriptTest::MosSetCaptureModeTypeL(const TDesC8& aComp, TInt aPort, TBool aContinuous, TBool aFrameLimited, TInt aFrameLimit)
    {
    OMX_COMPONENTTYPE* comp = ComponentByName(aComp);
    if (!comp)
        {
        return EFalse;
        }
    
    OMX_CONFIG_CAPTUREMODETYPE captureModeType;  
    captureModeType.nSize = sizeof(OMX_CONFIG_CAPTUREMODETYPE);
    captureModeType.nVersion = KOmxVersion;
    captureModeType.nPortIndex = aPort;
    captureModeType.bContinuous = (aContinuous ? OMX_TRUE : OMX_FALSE); 
    captureModeType.bFrameLimited = (aFrameLimited ? OMX_TRUE : OMX_FALSE);
    captureModeType.nFrameLimit = aFrameLimit;
    OMX_ERRORTYPE omxErr = comp->SetConfig(comp,OMX_IndexConfigCaptureMode,&captureModeType); 
    
    if (omxErr != OMX_ErrorNone)
        {
        ERR_PRINTF2(_L("Error %08X returned from SetConfig"), omxErr);
        }
    
    return (omxErr == OMX_ErrorNone);
    }

//

static TInt StopScheduler(TAny* /*unused*/)
	{
	CActiveScheduler::Stop();
	return 0;
	}

void ROmxScriptTest::StopTest(TOmxScriptTestVerdict aVerdict)
	{
	switch(aVerdict)
		{
	case EPass:
		StopTest(KErrNone, EPass);
		break;
	case EFail:
		StopTest(KErrGeneral, EFail);
		break;
	default:
		User::Invariant();
		}
	}

/**
 * Stores the result of the test case. If called multiple times, the first
 * case of EFail will persist.
 * @param aError system-wide error code
 * @param aVerdict either EPass or EFail
 */
void ROmxScriptTest::StopTest(TInt aError, TOmxScriptTestVerdict aVerdict)
	{
	__ASSERT_ALWAYS(aVerdict != EPending, User::Invariant());
	// first negative result persists
	if(iVerdict != EFail)
		{
		iReason = aError;
		iVerdict = aVerdict;
		}
	}

void ROmxScriptTest::FailTest(const TDesC& aErrorMsg)
    {
    ERR_PRINTF1(aErrorMsg);
    StopTest(EFail);
    }

/**
 * @return the error code set via StopTest. Defaults to KErrNone.
 */
TInt ROmxScriptTest::Reason() const
	{
	return iReason;
	}

/**
 * @return the verdict set via StopTest. Defaults to EPending.
 */
ROmxScriptTest::TOmxScriptTestVerdict ROmxScriptTest::Verdict() const
	{
	return iVerdict;
	}

/**
 * Overflow handler to generate a warning message if a log line will not fit
 * in the descriptor.
 */
class TOverflowHandler : public TDes16Overflow
	{
public:
	void Overflow(TDes& aDes)
		{
		_LIT(KWarning, "[truncated]");
		if(aDes.Length() + KWarning().Length() > aDes.MaxLength())
			{
			aDes.SetLength(aDes.Length() - KWarning().Length());
			}
		aDes.Append(KWarning);
		}
	};

/**
 * Target of ERR_PRINTFx, INFO_PRINTFx, WARN_PRINTFx macros.
 * Message is formatted then passed to the MOmxScriptTestLogger.
 */
void ROmxScriptTest::LogExtra(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity,
			TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	TOverflowHandler overflow;
	TBuf<255> msg;
	msg.AppendFormatList(aFmt, aList, &overflow);
	
	iLogger.Log(aFile, aLine, aSeverity, msg);
	
	VA_END(aList);
	}

void ROmxScriptTest::RegisterTunnel(OMX_COMPONENTTYPE* aSourceComp, TInt aSourcePort, OMX_COMPONENTTYPE* aSinkComp, TInt aSinkPort)
	{
	// delete previous registration
	for(TInt index = 0, count = iTunnels.Count(); index < count; index++)
		{
		const TTunnelInfo& info = iTunnels[index];
		if(info.iSourceComponent == aSourceComp && info.iSourcePort == aSourcePort ||
			info.iSinkComponent == aSinkComp && info.iSinkPort == aSinkPort)
			{
			iTunnels.Remove(index);
			index--;
			count--;
			}
		}
	
	TTunnelInfo info;
	info.iSourceComponent = aSourceComp;
	info.iSourcePort = aSourcePort;
	info.iSinkComponent = aSinkComp;
	info.iSinkPort = aSinkPort;
	TInt error = iTunnels.Append(info);
	if(error)
		{
		User::Panic(_L("omxscript"), KErrNoMemory);
		}
	}

void ROmxScriptTest::FindTransitionOrderL(const RPointerArray<CComponentInfo>& aComponents,
						  const RTunnelRelations& aTunnelRelations,
						  RArray<TInt>& aOrder)
	{
	RPointerArray<OMX_COMPONENTTYPE> componentsSimple;
	CleanupClosePushL(componentsSimple);
	componentsSimple.ReserveL(aComponents.Count());
	for(TInt index = 0, count = aComponents.Count(); index < count; index++)
		{
		componentsSimple.AppendL(aComponents[index]->iComponent);
		}
	RSupplierRelations supplierRelations;
	CleanupStack::PushL(TCleanupItem(CloseSupplierRelations, &supplierRelations));
	TInt numComponents = aComponents.Count();
	supplierRelations.ReserveL(numComponents);
	for(TInt index = 0; index < numComponents; index++)
		{
		supplierRelations.Append(RArray<TInt>());
		}
	TInt numTunnels = aTunnelRelations.Count();
	for(TInt index = 0; index < numTunnels; index++)
		{
		const TTunnelInfo& tunnelInfo = aTunnelRelations[index];
		TInt outputCompIndex = componentsSimple.Find(tunnelInfo.iSourceComponent);
		User::LeaveIfError(outputCompIndex);
		TInt inputCompIndex = componentsSimple.Find(tunnelInfo.iSinkComponent);
		User::LeaveIfError(inputCompIndex);
		
		OMX_PARAM_BUFFERSUPPLIERTYPE supplier;
		supplier.nSize = sizeof(supplier);
		supplier.nVersion = KOmxVersion;
		supplier.nPortIndex = tunnelInfo.iSourcePort;
		OMX_ERRORTYPE error = OMX_GetParameter(tunnelInfo.iSourceComponent,
				OMX_IndexParamCompBufferSupplier,
				&supplier);
		if(error != OMX_ErrorNone)
			{
			User::Leave(KErrGeneral);
			}
		if(supplier.eBufferSupplier == OMX_BufferSupplyInput)
			{
			supplierRelations[inputCompIndex].AppendL(outputCompIndex);
			}
		else if(supplier.eBufferSupplier == OMX_BufferSupplyOutput)
			{
			supplierRelations[outputCompIndex].AppendL(inputCompIndex);
			}
		else
			{
			User::Leave(KErrArgument);
			}
		}

	::FindTransitionOrderL(supplierRelations, aOrder);
	
	CleanupStack::PopAndDestroy(2, &componentsSimple);
	}

TBool ROmxScriptTest::MosCheckClockStateL(const TDesC8& aComp, OMX_TIME_CLOCKSTATE aClockState)
	{
	/*
	TODO: Verify that this is a clock component?
	 */
	
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_TIME_CONFIG_CLOCKSTATETYPE conf;
	conf.nVersion = KOmxVersion;
	conf.nSize = sizeof(conf);
	/*
	Completed by GetConfig()

	conf.eState
	conf.nStartTime
	conf.nOffset
	conf.nWaitMask
	*/

	OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigTimeClockState, &conf);
	if(error != OMX_ErrorNone)
		{
		FailWithOmxError(_L("OMX_GetConfig(OMX_IndexConfigTimeClockState)"), error);
		return EFalse;
		}

	if(conf.eState != aClockState)
		{
// TODO: What about the other values?
		ERR_PRINTF3(_L("Clock component expected to be in clock state %S, is actually in %S"), ClockStateDes(aClockState), ClockStateDes(conf.eState));
		StopTest(EFail);
		return EFalse;
		}
	
	TBuf<64> compConverted;
	compConverted.Copy(aComp);
	INFO_PRINTF3(_L("State of %S is %S"), &compConverted, ClockStateDes(aClockState));
	
	return ETrue;
	}

// for debugging...
TBool ROmxScriptTest::MosCheckTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_TIME_CONFIG_TIMESTAMPTYPE conf;
    conf.nVersion = KOmxVersion;
    conf.nSize = sizeof(conf);
    conf.nPortIndex = aPortIndex;
    /*
    To be filled by GetConfig()
    conf.nTimestamp;
    */ 

    OMX_ERRORTYPE error = component->GetConfig(component, OMX_IndexConfigTimePosition, &conf);
    if(error != OMX_ErrorNone)
        {
        FailWithOmxError(_L("OMX_GetConfig(OMX_IndexConfigTimePosition)"), error);
        return EFalse;
        }
	if(conf.nTimestamp != aTimestamp)
		{
		TBuf<64> compConverted;
		compConverted.Copy(aComp);
		ERR_PRINTF5(_L("Port %d of component %S time position expected to be %d, but is actually %d"), aPortIndex, &compConverted, aTimestamp, conf.nTimestamp);
		StopTest(EFail);
		return EFalse;
		}
    
    TBuf<64> compConverted;
    compConverted.Copy(aComp);
    INFO_PRINTF4(_L("Port %d of component %S time position verified to be %d"), aPortIndex, &compConverted, conf.nTimestamp);
    
    return ETrue;
    }

TBool ROmxScriptTest::MosSetTimePositionL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_TIME_CONFIG_TIMESTAMPTYPE conf;
    conf.nVersion = KOmxVersion;
    conf.nSize = sizeof(conf);
    conf.nPortIndex = aPortIndex;
    conf.nTimestamp = aTimestamp;

    OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimePosition, &conf);
    if(error != OMX_ErrorNone)
        {
        FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimePosition)"), error);
        return EFalse;
        }
    
    TBuf<64> compConverted;
    compConverted.Copy(aComp);
    INFO_PRINTF4(_L("Port %d of component %S time position set to %d"), aPortIndex, &compConverted, aTimestamp);
    
    return ETrue;
    }

TBool ROmxScriptTest::MosCheckFrameCountL(const TDesC& aFilename, TInt aCount)
    {
    C3GPParse* parser = C3GPParse::NewL();
    CleanupStack::PushL(parser);
    TInt err = parser->Open(aFilename);
    if(err)
        {
        CleanupStack::PopAndDestroy(parser);
        return EFalse;
        }
    else
        {
        TUint num;
        err = parser->GetNumberOfVideoFrames(num);
        CleanupStack::PopAndDestroy(parser);
        if(err)
            {
            ERR_PRINTF1(_L("aParser->GetNumberOfVideoFrames() failed"));
            return EFalse;
            }
        else
            {
            //assume 20 dropped frames is accepted normally 
            //if the frame count is smaller than 20, we just allow 10 dropped frames
            TInt tmp = (aCount > 20) ? (aCount - 20): (aCount - 10);
            if(num < tmp)
                {
                ERR_PRINTF1(_L("too many dropped frames, ROmxScriptTest->MosCheckFrameCountL() failed"));
                return EFalse;
                }
            }
        }
        return ETrue;
    }

TBool ROmxScriptTest::MosMarkBuffer(const TDesC8& aComp, TInt aPortIndex, const TDesC8& aTargetComp, TInt markData)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	OMX_COMPONENTTYPE* targetComponent = ComponentByName(aTargetComp);
	if(component == NULL || targetComponent == NULL)
		{
		return EFalse;
		}
	
	OMX_MARKTYPE mark;
	mark.hMarkTargetComponent = targetComponent;
	mark.pMarkData = reinterpret_cast<TAny*>(markData);
	
	OMX_ERRORTYPE error = OMX_SendCommand(component, OMX_CommandMarkBuffer, aPortIndex, &mark);
	if(error)
		{
		FailWithOmxError(aComp, _L("SendCommand(MarkBuffer)"), error);
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

void ROmxScriptTest::MosParseError(const TDesC& aError)
    {
    ERR_PRINTF1(aError);
    }

ROmxScriptTest::CComponentInfo* ROmxScriptTest::CComponentInfo::NewL(ROmxScriptTest& aTestController)
    {
    CComponentInfo* self = new (ELeave) CComponentInfo(aTestController);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void ROmxScriptTest::CComponentInfo::ConstructL()
    {
	}

TBool ROmxScriptTest::MosSetClientStartTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_TIME_CONFIG_TIMESTAMPTYPE conf;
    conf.nVersion = KOmxVersion;
    conf.nSize = sizeof(conf);
    conf.nPortIndex = aPortIndex;
    conf.nTimestamp = aTimestamp;

    OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimeClientStartTime, &conf);
    if(error != OMX_ErrorNone)
        {
        FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimeClientStartTime)"), error);
        return EFalse;
        }
    
    TBuf<64> compConverted;
    compConverted.Copy(aComp);
    INFO_PRINTF4(_L("Port %d of component %S time position set to %d"), aPortIndex, &compConverted, aTimestamp);
    
    return ETrue;
    }

TBool ROmxScriptTest::MosSetCurrentAudioReferenceTimeL(const TDesC8& aComp, TInt aPortIndex, TInt aTimestamp)
    {
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);
    if(!component)
        {
        return EFalse;
        }

    OMX_TIME_CONFIG_TIMESTAMPTYPE conf;
    conf.nVersion = KOmxVersion;
    conf.nSize = sizeof(conf);
    conf.nPortIndex = aPortIndex;
    conf.nTimestamp = aTimestamp;

    OMX_ERRORTYPE error = component->SetConfig(component, OMX_IndexConfigTimeCurrentAudioReference, &conf);
    if(error != OMX_ErrorNone)
        {
        FailWithOmxError(_L("OMX_SetConfig(OMX_IndexConfigTimeCurrentAudioReference)"), error);
        return EFalse;
        }
    
    TBuf<64> compConverted;
    compConverted.Copy(aComp);
    INFO_PRINTF4(_L("Port %d of component %S time position set to %d"), aPortIndex, &compConverted, aTimestamp);
    
    return ETrue;
    }
#ifdef OLD_ADPCM_EXTENSION
TBool ROmxScriptTest::MosSetAdPcmAudioPortDefL(const TDesC8& aComp,
											 TInt aPortIndex,
											 TInt aNumChannels,
											 TInt aSamplingRate,
											 TInt aBitsperSample)
	{
	OMX_COMPONENTTYPE* component = ComponentByName(aComp);
	if(!component)
		{
		return EFalse;
		}

	OMX_AUDIO_PARAM_ADPCMTYPE pcmModeType;
	pcmModeType.nSize = sizeof(pcmModeType);
	pcmModeType.nVersion = KOmxVersion;
	pcmModeType.nPortIndex = aPortIndex;
	OMX_ERRORTYPE error = component->GetParameter(component, OMX_IndexParamAudioAdpcm, &pcmModeType);
	if(error)
		{
		FailWithOmxError(_L("GetParameter()"), error);
		return EFalse;
		}
	if(aNumChannels != -1)
		{
		pcmModeType.nChannels = aNumChannels;
		}
	if(aSamplingRate != -1)
		{
		pcmModeType.nSampleRate = aSamplingRate;
		}
	if(aBitsperSample != -1)
		{
		pcmModeType.nBitsPerSample = aBitsperSample;
		}
	error = component->SetParameter(component, OMX_IndexParamAudioAdpcm, &pcmModeType);
	if(error)
		{
		FailWithOmxError(_L("SetParameter()"), error);
		return EFalse;
		}
	return ETrue;
	}	

TBool ROmxScriptTest::MosConfigAdPcmDecoderBlockAlign(const TDesC8& aComp, OMX_S32 aBlockAlign)
    {
    // Create param
    OMX_SYMBIAN_AUDIO_PARAM_ADPCMDECODER_BLOCKALIGN param;
    param.nSize = sizeof(param);
    param.nVersion = KAdPcmDecoderConfigVersion;
    param.nPortIndex = 0;
    param.nBlockAlign = aBlockAlign;
    
    // Show debugging info
    INFO_PRINTF2(_L("Setting the size of the AdPcm block to %d"), aBlockAlign);

    // Get component
    OMX_COMPONENTTYPE* component = ComponentByName(aComp);

    // Check component
    if(!component)
        return EFalse;

    // Set the config
    OMX_ERRORTYPE error = component->SetParameter(component, static_cast<OMX_INDEXTYPE>(OMX_SymbianIndexParamAudioAdPcmDecoderBlockAlign), &param);
    if(error)
        {
        FailWithOmxError(_L("SetParameter(OMX_SymbianIndexParamAudioAdPcmDecoderBlockAlign)"), error);
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }
#endif	

ROmxScriptTest::CComponentInfo::CComponentInfo(ROmxScriptTest& aTestController)
: iTestController(aTestController)    
    {
    }

ROmxScriptTest::CComponentInfo::~CComponentInfo()
    {
    // convert from 8-bit descriptors for logging
    TBuf<64> shortName;
    TBuf<64> compName;
    shortName.Copy(*iShortName);
    compName.Copy(*iComponentName);
    OMX_ERRORTYPE error = OMX_ErrorNone;
    if(iComponent != NULL)
        {
        iTestController.INFO_PRINTF3(_L("Unloading component %S (%S)"), &shortName, &compName);
        if(iThreadRequest != NULL)
            {
            error = iThreadRequest->FreeHandle(iComponent);
            delete iShortName;
            delete iComponentName;
            delete iBaseHandler;
            }
        else
            {
            delete iShortName;
            delete iComponentName;
            delete iBaseHandler;
            error = OMX_FreeHandle(iComponent);
            }
        }
    
    if(error)
        {
        iTestController.FailWithOmxError(_L("OMX_FreeHandle()"), error);
        }
    delete iThreadRequest;
	}
void HashFilter_16bit_EndianSwap(RBuf8& aBuf)
    {
    for (TInt i=0;i<aBuf.Length();i+=2)
        {
        TUint8 tmp = aBuf[i];
        aBuf[i] = aBuf[i+1];
        aBuf[i+1] = tmp;
        }
    }

void HashFilter_16bit_BE_SignednessSwap(RBuf8& aBuf)
    {
    for (TInt i=0;i<aBuf.Length();i+=2)
        {
        aBuf[i] ^= 0x80;
        }
    }

void HashFilter_16bit_LE_SignednessSwap(RBuf8& aBuf)
    {
    for (TInt i=0;i<aBuf.Length();i+=2)
        {
        aBuf[i+1] ^= 0x80;
        }
    }

void HashFilter_16bit_BE_MsbOnly(RBuf8& aBuf)
    {
    TInt src = 0;
    TInt dest = 0;
    
    while (src<aBuf.Length())
        {
        aBuf[dest] = aBuf[src];
        src+=2;
        dest++;
        }
    
    aBuf.SetLength(dest);
    }

void HashFilter_16bit_LE_MsbOnly(RBuf8& aBuf)
    {
    TInt src = 1;
    TInt dest = 0;
    
    while (src<aBuf.Length())
        {
        aBuf[dest] = aBuf[src];
        src+=2;
        dest++;
        }
    
    aBuf.SetLength(dest);
    }

ROmxScriptTest::THashFilter ROmxScriptTest::GetHashFilterByName(const TDesC8& aName)
    {
    if (aName.Length()==0) return NULL;
    else if (aName==_L8("endianswap")) return &HashFilter_16bit_EndianSwap;
    else if (aName==_L8("signswap16be")) return &HashFilter_16bit_BE_SignednessSwap;
    else if (aName==_L8("signswap16le")) return &HashFilter_16bit_LE_SignednessSwap;
    else if (aName==_L8("msbonly16be")) return &HashFilter_16bit_BE_MsbOnly;
    else if (aName==_L8("msbonly16le")) return &HashFilter_16bit_LE_MsbOnly;
    else User::Invariant();
    return NULL; // Inaccessible
    }
