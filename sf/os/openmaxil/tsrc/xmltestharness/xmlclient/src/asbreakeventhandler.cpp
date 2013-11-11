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


/**
@file
@internalComponent
*/

#include <e32debug.h>
#include "asbreakeventhandler.h"

const TDesC* StateDes(OMX_STATETYPE);

RASBreakEventHandler::RASBreakEventHandler(MASBreakCallback& aHandler):
iHandler(aHandler),
iCounter(0),
iMode(EIdle)
	{
	iOmxCallbackType.EventHandler = EventHandler;
	
	// do nothing function is set for these callbacks, rather than using NULL
	iOmxCallbackType.EmptyBufferDone = EmptyBufferDone;
	iOmxCallbackType.FillBufferDone = FillBufferDone;
	}

TInt RASBreakEventHandler::Create()
	{
	return iMutex.CreateLocal();
	}

void RASBreakEventHandler::Close()
	{
	for(TInt aIndex = 0, aCount = iComponents.Count(); aIndex < aCount; aIndex++)
		{
		delete iComponents[aIndex].iName;
		}
	iComponents.Close();
	iCounter = 0;
	iMode = EIdle;
	iMutex.Close();
	}

void RASBreakEventHandler::AddComponentL(OMX_COMPONENTTYPE* aComponent, const TDesC& aName)
	{
	ASSERT(iMode == EIdle);
	TComponentInfo componentInfo;
	componentInfo.iHandle = aComponent;
	componentInfo.iName = HBufC::NewLC(aName.Length());
	*(componentInfo.iName) = aName;
	componentInfo.iComplete = EFalse;
	User::LeaveIfError(iComponents.Append(componentInfo));
	CleanupStack::Pop(componentInfo.iName);
	}

OMX_ERRORTYPE RASBreakEventHandler::FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                                   OMX_IN OMX_PTR pAppData,
                                                   OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
	{
	return reinterpret_cast<RASBreakEventHandler*>(pAppData)->DoBufferDone(reinterpret_cast<OMX_COMPONENTTYPE*>(hComponent), pBuffer, ETrue);
	}

OMX_ERRORTYPE RASBreakEventHandler::EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                                    OMX_IN OMX_PTR pAppData,
                                                    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
	{
	return reinterpret_cast<RASBreakEventHandler*>(pAppData)->DoBufferDone(reinterpret_cast<OMX_COMPONENTTYPE*>(hComponent), pBuffer, EFalse);
	}

OMX_ERRORTYPE RASBreakEventHandler::DoBufferDone(OMX_COMPONENTTYPE* aComponent, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource)
	{
	iHandler.BufferDone(aComponent, aBufHdr, aSource);
	return OMX_ErrorNone;
	}

OMX_ERRORTYPE RASBreakEventHandler::EventHandler(OMX_HANDLETYPE hComponent,
		OMX_PTR pAppData,
		OMX_EVENTTYPE eEvent,
		OMX_U32 nData1,
		OMX_U32 nData2,
		OMX_PTR pEventData)
	{
	return reinterpret_cast<RASBreakEventHandler*>(pAppData)->DoEventHandler(reinterpret_cast<OMX_COMPONENTTYPE*>(hComponent), eEvent, nData1, nData2, pEventData);
	}

OMX_ERRORTYPE RASBreakEventHandler::DoEventHandler(OMX_COMPONENTTYPE* aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aEventData)
	{
	iMutex.Wait();
	if((iMode == EAwaitingTransition || iMode == EAwaitingSingleTransition) && aEvent == OMX_EventCmdComplete && aData1 == OMX_CommandStateSet)
		{
		StateSet(aComponent, (OMX_STATETYPE) aData2);
		}
	else if(iMode == EAwaitingEOS && aEvent == OMX_EventBufferFlag && aData2 & OMX_BUFFERFLAG_EOS)
		{
		EndOfStream(aComponent);
		}
	else
		{
		iHandler.EventReceived(aComponent, aEvent, aData1, aData2, aEventData);
		}
	iMutex.Signal();
	return OMX_ErrorNone;
	}

void RASBreakEventHandler::StateSet(OMX_COMPONENTTYPE* aComponent, OMX_STATETYPE aState)
	{
	RDebug::Print(_L("State transition: component %S is in state %S\n"), ComponentName(aComponent), StateDes(aState));
	if(iMode == EAwaitingTransition)
		{
		__ASSERT_ALWAYS(aState == iNewState, User::Panic(_L("RASBreakEventHandler"), 2));
		if(--iCounter == 0)
			{
			iMode = EIdle;
			iHandler.AllComponentsTransitioned(iNewState, iOldState);
			}
		}
	else if (iMode == EAwaitingSingleTransition)
		{
		__ASSERT_ALWAYS(aState == iNewState, User::Panic(_L("RASBreakEventHandler"), 2));
		iMode = EIdle;
		iHandler.ComponentTransitioned(iNewState, iOldState);		
		}
	}

void RASBreakEventHandler::EndOfStream(OMX_COMPONENTTYPE* aComponent)
	{
	RDebug::Print(_L("End of stream from component %S\n"), ComponentName(aComponent));
	if(iMode == EAwaitingEOS)
		{
		TInt index = iComponents.Find(aComponent, TComponentInfo::CompareHandle);
		// panic if component was not found
		__ASSERT_ALWAYS(index >= 0, User::Invariant());
	
		// only respond to EOS changing state, to allow potential duplicates
		if(!iComponents[index].iComplete)
			{
			iComponents[index].iComplete = ETrue;
			iCounter--;
			// if iEOSComponent is NULL, wait for all components, otherwise wait for individual component
			if(iEOSComponent == NULL && iCounter == 0 || iEOSComponent == aComponent)
				{
				iMode = EIdle;
				iHandler.AllComponentsEOS();
				}
			}
		}
	}

void RASBreakEventHandler::AwaitTransition(OMX_STATETYPE aNewState, OMX_STATETYPE aOldState)
	{
	RDebug::Print(_L("Awaiting all components to transition from %S to %S\n"), StateDes(aOldState), StateDes(aNewState));
	ASSERT(iMode == EIdle);
	iMode = EAwaitingTransition;
	iNewState = aNewState;
	iOldState = aOldState;
	iCounter = iComponents.Count();
	}

void RASBreakEventHandler::AwaitSingleTransition(OMX_COMPONENTTYPE* aComponent, OMX_STATETYPE aNewState, OMX_STATETYPE aOldState)
	{
	RDebug::Print(_L("Awaiting %S components to transition from %S to %S\n"), ComponentName(aComponent), StateDes(aOldState), StateDes(aNewState));	
	ASSERT(iMode == EIdle);
	iMode = EAwaitingSingleTransition;
	iNewState = aNewState;
	iOldState = aOldState;
	}

void RASBreakEventHandler::AwaitEOS()
	{
	RDebug::Print(_L("Awaiting End Of Stream flag from all components\n"));
	ASSERT(iMode == EIdle);
	iMode = EAwaitingEOS;
	iEOSComponent = NULL;	// wait for all components, not an individual component
	for(iCounter = 0; iCounter < iComponents.Count(); iCounter++)
		{
		iComponents[iCounter].iComplete = EFalse;
		}
	}

void RASBreakEventHandler::AwaitEOS(OMX_COMPONENTTYPE* aComponent)
	{
	RDebug::Print(_L("Awaiting End Of Stream flag from component %S\n"), ComponentName(aComponent));
	ASSERT(iMode == EIdle);
	iMode = EAwaitingEOS;
	iEOSComponent = aComponent;
	for(iCounter = 0; iCounter < iComponents.Count(); iCounter++)
		{
		iComponents[iCounter].iComplete = EFalse;
		}
	}

const TDesC* RASBreakEventHandler::ComponentName(OMX_COMPONENTTYPE* aComponent)
	{
	TInt index = iComponents.Find(aComponent, TComponentInfo::CompareHandle);
	if(index == KErrNotFound)
		{
		return NULL;
		}
	return iComponents[index].iName;
	}

OMX_CALLBACKTYPE& RASBreakEventHandler::CallbackStruct()
	{
	return iOmxCallbackType;
	}
