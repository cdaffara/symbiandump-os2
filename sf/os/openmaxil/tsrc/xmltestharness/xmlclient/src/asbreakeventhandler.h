/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ASBREAKEVENTHANDLER_H_
#define ASBREAKEVENTHANDLER_H_

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Component.h>

class MASBreakCallback
	{
public:
	virtual void AllComponentsEOS() = 0;
	virtual void AllComponentsTransitioned(OMX_STATETYPE aNewState,
			                               OMX_STATETYPE aOldState) = 0;
	virtual void EventReceived(OMX_COMPONENTTYPE* aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aEventData) = 0;
	virtual void ComponentTransitioned(OMX_STATETYPE aNewState,
				                   OMX_STATETYPE aOldState) = 0;
	virtual void BufferDone(OMX_COMPONENTTYPE* aComp, OMX_BUFFERHEADERTYPE* aBufHdr, TBool aSource) = 0;
	};

class RASBreakEventHandler
	{
public:
	RASBreakEventHandler(MASBreakCallback& aHandler);
	TInt Create();
	void Close();
	
	void InstallIntoL(OMX_COMPONENTTYPE* aComponent, const TDesC& aName);
	void AddComponentL(OMX_COMPONENTTYPE* aComponent, const TDesC& aName);
	
	void AwaitTransition(OMX_STATETYPE aNewState, OMX_STATETYPE aOldState);
	void AwaitEOS();
	void AwaitEOS(OMX_COMPONENTTYPE* aComponent);
	void AwaitSingleTransition(OMX_COMPONENTTYPE* aComponent, OMX_STATETYPE aNewState, OMX_STATETYPE aOldState);
	
	OMX_CALLBACKTYPE& CallbackStruct();
	
	static OMX_ERRORTYPE EventHandler(OMX_HANDLETYPE hComponent,
			                          OMX_PTR pAppData,
			                          OMX_EVENTTYPE eEvent,
			                          OMX_U32 nData1,
			                          OMX_U32 nData2,
			                          OMX_PTR pEventData);
	static OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
			                                OMX_IN OMX_PTR pAppData,
			                                OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
	static OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
			                                 OMX_IN OMX_PTR pAppData,
			                                 OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

private:	
	void StateSet(OMX_COMPONENTTYPE* aComponent, OMX_STATETYPE aState);
	void EndOfStream(OMX_COMPONENTTYPE* aComponent);
	OMX_ERRORTYPE DoBufferDone(OMX_COMPONENTTYPE* aComponent,
	                           OMX_BUFFERHEADERTYPE* aBufHdr,
	                           TBool aSource);
	OMX_ERRORTYPE DoEventHandler(OMX_COMPONENTTYPE* aComponent,
	                          OMX_EVENTTYPE aEvent,
	                          OMX_U32 aData1,
	                          OMX_U32 aData2,
	                          OMX_PTR aEventData);
	const TDesC* ComponentName(OMX_COMPONENTTYPE* aComponent);
	
	OMX_CALLBACKTYPE iOmxCallbackType;

	OMX_STATETYPE iNewState;
	OMX_STATETYPE iOldState;
	
	enum TMode { EIdle, EAwaitingTransition, EAwaitingEOS, EAwaitingSingleTransition };
	MASBreakCallback& iHandler;
	
	class TComponentInfo
		{
	public:
		static TBool CompareHandle(OMX_COMPONENTTYPE* const * a1, const TComponentInfo& a2)
			{
			return (*a1) == a2.iHandle;
			}
		
		OMX_COMPONENTTYPE* iHandle;
		TBool iComplete;
		HBufC* iName;
		};
		
	RArray<TComponentInfo> iComponents;
	TInt iCounter;
	TMode iMode;
	RMutex iMutex;
	
	OMX_COMPONENTTYPE* iEOSComponent;	// from which component we expect EOS. all components if NULL
	};
	
#endif /*ASBREAKEVENTHANDLER_H_*/
