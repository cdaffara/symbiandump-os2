// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
*/

#ifndef OMXILPORTIMPL_H
#define OMXILPORTIMPL_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <openmax/il/common/omxilspecversion.h>
#include <openmax/il/common/omxilport.h>

/**
   OpenMAX IL component port base implementation class.
*/
class COmxILPortImpl : public CBase
	{
public:
	static const TInt	KMaxBufferMarksQueueSize			= 10;
	static COmxILPortImpl* NewL(const TOmxILCommonPortData& aCommonPortData, COmxILPort& aPort);
	~COmxILPortImpl();

	OMX_ERRORTYPE GetParameter(
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure,
		TBool& aUpdateProcessingFunction);
		
	OMX_ERRORTYPE GetConfig(
		OMX_INDEXTYPE aConfigIndex,
		TAny* apComponentConfigStructure) const;

	OMX_ERRORTYPE SetConfig(
		OMX_INDEXTYPE aConfigIndex,
		const TAny* apComponentConfigStructure,
		TBool& aUpdateProcessingFunction);

	OMX_ERRORTYPE GetExtensionIndex(
		OMX_STRING aParameterName,
		OMX_INDEXTYPE* apIndexType) const;

	OMX_ERRORTYPE PopulateBuffer(
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		const OMX_PTR apAppPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		TBool& portDepopulationCompleted);

	OMX_ERRORTYPE TunnelRequest(
		OMX_HANDLETYPE aTunneledComp,
		OMX_U32 aTunneledPort,
		OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE PopulateTunnel(
		TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeTunnel(
		TBool& portDepopulationCompleted);

	TBool SetBufferSent(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		TBool& aBufferMarkedWithOwnMark);

	TBool SetBufferReturned(
		OMX_BUFFERHEADERTYPE* apBufferHeader);

	void SetTransitionToDisabled();

	void SetTransitionToEnabled();

	void SetTransitionToDisabledCompleted();

	void SetTransitionToEnabledCompleted();

	OMX_ERRORTYPE StoreBufferMark(
		const OMX_MARKTYPE* pMark);

	OMX_ERRORTYPE SetComponentRoleDefaults(
		TUint aComponentRoleIndex);

	OMX_ERRORTYPE DoPortReconfiguration(
		TUint aPortSettingsIndex,
		const TDesC8& aPortSettings,
		OMX_EVENTTYPE& aEventForILClient);

	inline OMX_DIRTYPE Direction() const;

	inline TBool IsEnabled() const;

	inline TBool IsPopulated() const;

	inline TBool IsDePopulated() const;

	inline TBool IsTunnelled() const;

	inline TBool IsTunnelledAndBufferSupplier() const;

	virtual TBool HasAllBuffersAtHome() const;

	TBool IsTransitioningToEnabled() const;

	inline TBool IsTransitioningToDisabled() const;

	virtual TBool IsBufferAtHome(
		OMX_BUFFERHEADERTYPE* apHeaderHeader) const;

	inline OMX_U32 Index() const;

	inline OMX_PORTDOMAINTYPE Domain() const;

	inline OMX_U32 Count() const;

	inline OMX_BUFFERHEADERTYPE* const& operator[](TInt anIndex) const;

	inline OMX_BUFFERHEADERTYPE*& operator[](TInt anIndex);

	inline OMX_U32 BufferMarkPropagationPort() const;

protected:

	class TBufferMarkInfo;

	/**
	   Buffer mark info list
	*/
	class TBufferMarkInfoQue :
		public TSglQue<TBufferMarkInfo>
		{

	public:

		// Convenience typedef....
		typedef TSglQue<TBufferMarkInfo> Tq;

	public:

		inline explicit TBufferMarkInfoQue(TInt aOffset);

		inline TBufferMarkInfo* First() const;

		inline void AddLast(TBufferMarkInfo& aRef);

		inline void Remove(TBufferMarkInfo& aRef);

		inline TInt Elements() const;

		inline void ResetAndDestroy();

	private:

		TBufferMarkInfoQue();
		void AddFirst(TBufferMarkInfo& aRef);
		TBool IsFirst(const TBufferMarkInfo* aPtr) const;
		TBool IsLast(const TBufferMarkInfo* aPtr) const;

		TBufferMarkInfo* Last() const;

	private:

		TInt iNumElements;

		};


	class TBufferInfo;

private:

	enum TPortIntermediateState
		{
		EPortNotTransitioning,
		EPortTransitioningToDisabled,
		EPortTransitioningToEnabled
		};


protected:

	COmxILPortImpl(const TOmxILCommonPortData& aCommonPortData, COmxILPort& aPort);
	
public:
	template<typename T>
	inline OMX_ERRORTYPE GetParamStructureFromTunnel(
		T& aComponentConfigStructure, OMX_INDEXTYPE aParamIndex) const;

	OMX_ERRORTYPE StoreBufferMark(
		OMX_HANDLETYPE& ipMarkTargetComponent,
		OMX_PTR& ipMarkData);

	void CleanUpPort();

	inline const OMX_PARAM_PORTDEFINITIONTYPE& GetParamPortDefinition() const;
	inline const OMX_HANDLETYPE& GetTunnelledComponent() const;
	inline const OMX_U32& GetTunnelledPort() const;
	inline OMX_PARAM_PORTDEFINITIONTYPE& GetParamPortDefinition();
	inline OMX_HANDLETYPE& GetTunnelledComponent();
	inline OMX_U32& GetTunnelledPort();
	
private:

	// Tunnelled component
	OMX_HANDLETYPE iTunnelledComponent;

	// Tunnelled component's port
	OMX_U32 iTunnelledPort;

	// Buffer headers store
	RArray<TBufferInfo> iBufferHeaders;

	// Buffer marks store
	TBufferMarkInfoQue iBufferMarks;

	// Port's intermediate state to enabled or disabled
	TPortIntermediateState iTransitionState;

	// Associated port that will be used to propagate buffer marks
	OMX_U32 iBufferMarkPropagationPortIndex;

	// This is a flag that is set when the port receives the first
	// OMX_UseBuffer (non-supplier ports). This will be used to know when
	// SetParameter will be allowed
	OMX_BOOL aFirstUseBufferHasBeenReceived;

	// OpenMAX IL port definition structure
	OMX_PARAM_PORTDEFINITIONTYPE iParamPortDefinition;

	// OpenMAX IL buffer supplier structure
	OMX_PARAM_BUFFERSUPPLIERTYPE iParamCompBufferSupplier;
	
	COmxILPort& iOmxILPort;
	
	};

/**
   Buffer mark info structure
*/
class COmxILPortImpl::TBufferMarkInfo
	{

public:

	inline explicit TBufferMarkInfo(
		const OMX_MARKTYPE*& apMark,
		TBool aOwnMark = ETrue);

	inline TBufferMarkInfo(
		OMX_HANDLETYPE& apMarkTargetComponent,
		OMX_PTR& apMarkData,
		TBool aOwnMark = ETrue);

public:

	OMX_HANDLETYPE ipMarkTargetComponent;
	OMX_PTR ipMarkData;
	TBool iOwnMark;
	TSglQueLink iLink;

	};

/**
   Buffer info structure
*/
class COmxILPortImpl::TBufferInfo
	{

public:

	enum THeaderLocationProperty
		{
		EBufferAtHome	= 0x01,
		EBufferAway		= 0x00
		};

	enum THeaderOwnershipProperty
		{
		EBufferOwned	= 0x02,
		EBufferNotOwned = 0x00
		};

public:

	inline TBufferInfo(OMX_BUFFERHEADERTYPE* apHeader,
					   THeaderLocationProperty aLocation,
					   THeaderOwnershipProperty aOwnership,
					   OMX_U8* apBuffer,
					   OMX_PTR apApp,
					   OMX_PTR apPlatform,
					   OMX_PTR apPort);

	inline explicit TBufferInfo(OMX_BUFFERHEADERTYPE* apHeader);

	static TBool Compare(const TBufferInfo& aBi1,
						const TBufferInfo& aBi2);

	inline const OMX_BUFFERHEADERTYPE* GetHeader() const;

	inline OMX_BUFFERHEADERTYPE* GetHeader();

	inline operator OMX_BUFFERHEADERTYPE*&();

	inline operator OMX_BUFFERHEADERTYPE* const&() const;

	inline OMX_U8* GetBufferPointer() const;

	inline OMX_U8* GetBufferPointer();

	inline OMX_PTR GetPortPointer() const;

	inline OMX_PTR GetPortPointer();

	inline OMX_PTR GetPlatformPointer() const;

	inline OMX_PTR GetPlatformPointer();

	inline OMX_PTR GetAppPointer() const;

	inline OMX_PTR GetAppPointer();

	inline void SetBufferAtHome();

	inline void SetBufferAway();

	inline void SetBufferOwned();

	inline void SetBufferNotOwned();

	inline TBool IsBufferAtHome() const;

	inline TBool IsBufferOwned() const;

private:

	enum THeaderPropertyMask
		{
		EBufferAwayMask		= 0xFE,
		EBufferNotOwnedMask = 0xFD
		};

private:

	OMX_BUFFERHEADERTYPE* ipHeader;
	TUint8 iBufferProperties;
	OMX_U8* ipBuffer;
	OMX_PTR ipApp;
	OMX_PTR ipPlatform;
	OMX_PTR ipPort;

	};

#include "omxilportimpl.inl"
#endif // OMXILPORTIMPL_H
