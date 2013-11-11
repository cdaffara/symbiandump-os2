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


/**
   @file
   @internalComponent
*/

#ifndef OMXILSTATE_H
#define OMXILSTATE_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/common/omxilstatedefs.h>
#include "omxilfsm.h"
#include "omxilportmanagerif.h"

// Forward declarations
class TOmxILCommand;

/**
   Base class for concrete OpenMAX IL state objects (State Pattern)
*/
class COmxILFsm::COmxILState : public CBase
	{

public:

	//
	// Events, a.k.a. OpenMAX IL API calls
	//

	virtual OMX_ERRORTYPE GetParameter(
		const COmxILFsm& aFsm,
		OMX_INDEXTYPE aParamIndex,
		TAny* apComponentParameterStructure) const;

	/**
	   State-specific version of the OpenMAX IL SetParameter API.

	   @param [in] aFsm The main FSM context class that delegates the events to the
	   state classes.

	   @param [in] aParamIndex The index of the structure that is to be filled.

	   @param [in] apComponentParameterStructure A pointer to the IL structure.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE SetParameter(
		COmxILFsm& aFsm,
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure) = 0;

	virtual OMX_ERRORTYPE GetConfig(
		const COmxILFsm& aFsm,
		OMX_INDEXTYPE aConfigIndex,
		TAny* apComponentConfigStructure) const;

	virtual OMX_ERRORTYPE SetConfig(
		COmxILFsm& aFsm,
		OMX_INDEXTYPE aConfigIndex,
		const TAny* apComponentConfigStructure);

	virtual OMX_ERRORTYPE GetExtensionIndex(
		const COmxILFsm& aFsm,
		OMX_STRING aParameterName,
		OMX_INDEXTYPE* apIndexType) const;

	/**
	   State-specific version of the OpenMAX IL GetState API.

	   @return OMX_STATETYPE
	*/
	virtual OMX_STATETYPE GetState() const = 0;

	/**
	   State-specific version of the OpenMAX IL ComponentTunnelRequest API.

	   @param [in] aFsm The main FSM context class that delegates the events to
	   the state classes.

	   @param [in] aPort Used to select the port on the component to be used
	   for tunneling.

	   @param [in] aTunneledComp Handle of the component to tunnel with.

	   @param [in] aTunneledPort Indicates the port the component should tunnel
	   with.

	   @param [in] aTunnelSetup Pointer to the tunnel setup structure.

	   @return OMX_ERRORTYPE
	*/
	inline virtual OMX_ERRORTYPE ComponentTunnelRequest(
		COmxILFsm& aFsm,
		OMX_U32 aPort,
		OMX_HANDLETYPE aTunneledComp,
		OMX_U32 aTunneledPort,
		OMX_TUNNELSETUPTYPE* apTunnelSetup) = 0;

	/**
	   State-specific version of the PopulateBuffer method used by the FSM
	   object to implement the OpenMAX IL AllocateBuffer/UseBuffer APIs.

	   @param [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [out] appBufferHdr A pointer to a pointer of an
	   OMX_BUFFERHEADERTYPE structure that receives the pointer to the buffer
	   header.

	   @param [in] aPortIndex The index of the port that will use the
	   specified buffer. This index is relative to the component that owns the
	   port.

	   @param [in] apAppPrivate A pointer that refers to an
	   implementation-specific memory area that is under responsibility of the
	   supplier of the buffer.

	   @param [in] aSizeBytes The buffer size in bytes.

	   @param [in] apBuffer A pointer to the memory buffer area to be used.

	   @param [out] portPopulationCompleted Used to signal the FSM object the
	   the port population has completed.

	   @return OMX_ERRORTYPE
	*/
	inline virtual OMX_ERRORTYPE PopulateBuffer(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aPortIndex,
		OMX_PTR apAppPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		TBool& portPopulationCompleted) = 0;

	/**
	   State-specific version of the OpenMAX IL FreeBuffer API.

	   @param [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [in] aPortIndex The index of the port that is using the
	   specified buffer.

	   @param [in] apBuffer A pointer to an OMX_BUFFERHEADERTYPE structure
	   used to provide or receive the pointer to the buffer header.

	   @param [out] aPortDepopulationCompleted Used to signal the FSM object
	   the the port population has completed.

	   @return OMX_ERRORTYPE
	*/
	inline virtual OMX_ERRORTYPE FreeBuffer(
		COmxILFsm& aFsm,
		OMX_U32 aPortIndex,
		OMX_BUFFERHEADERTYPE* apBuffer,
		TBool& aPortDepopulationCompleted) = 0;

	/**
	   State-specific version of the OpenMAX IL EmptyThisBuffer API.

	   @param  [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [in] apBuffer A pointer to an OMX_BUFFERHEADERTYPE structure that is
	   used to receive the pointer to the buffer header. The buffer header
	   shall specify the index of the input port that receives the buffer.

	   @return OMX_ERRORTYPE
	*/
	inline virtual OMX_ERRORTYPE EmptyThisBuffer(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE* apBuffer) = 0;

	/**
	   State-specific version of the OpenMAX IL FillThisBuffer API.

	   @param [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [in] apBuffer A pointer to an OMX_BUFFERHEADERTYPE structure
	   that is used to receive the pointer to the buffer header. The buffer
	   header shall specify the index of the input port that receives the
	   buffer.

	   @return OMX_ERRORTYPE
	*/
	inline virtual OMX_ERRORTYPE FillThisBuffer(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE* apBuffer) = 0;

	/**
	   State-specific version of the method used by the FSM object to
	   implement the OpenMAX IL OMX_CommandStateSet command.

	   @param [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [in] aCommand A TOmxILCommand object used to carry the command
	   parameters.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand) = 0;

	virtual OMX_ERRORTYPE CommandFlush(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	/**
	   State-specific version of the method used by the FSM object to
	   implement the OpenMAX IL OMX_CommandPortEnable command.

	   @param [in] aFsm The main FSM context class that delegates the events
	   to the state classes.

	   @param [in] aCommand A TOmxILCommand object used to carry the command
	   parameters.

	   @return OMX_ERRORTYPE
	*/
	virtual OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand) = 0;

	virtual OMX_ERRORTYPE CommandPortDisable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	virtual OMX_ERRORTYPE CommandMarkBuffer(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

protected:

	virtual OMX_ERRORTYPE SetParameterV2(
		COmxILFsm& aFsm,
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure);

	inline virtual OMX_ERRORTYPE PopulateBufferV2(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aPortIndex,
		OMX_PTR apAppPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* apBuffer,
		TBool& portPopulationCompleted);

	inline virtual OMX_ERRORTYPE FreeBufferV2(
		COmxILFsm& aFsm,
		OMX_U32 aPortIndex,
		OMX_BUFFERHEADERTYPE* apBuffer,
		TBool& aPortDepopulationCompleted);

	inline virtual OMX_ERRORTYPE ComponentTunnelRequestV2(
		COmxILFsm& aFsm,
		OMX_U32 aPort,
		OMX_HANDLETYPE aTunneledComp,
		OMX_U32 aTunneledPort,
		OMX_TUNNELSETUPTYPE* apTunnelSetup);

	inline virtual OMX_ERRORTYPE EmptyThisBufferV2(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE* apBuffer);

	inline virtual OMX_ERRORTYPE FillThisBufferV2(
		COmxILFsm& aFsm,
		OMX_BUFFERHEADERTYPE* apBuffer);

	};


/**
   Concrete class that implements the OpenMAX IL OMX_StateInvalid state
*/
class COmxILFsm::COmxILStateInvalid : public COmxILFsm::COmxILState
	{

public:

	OMX_ERRORTYPE GetParameter(const COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   TAny* apComponentParameterStructure) const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE GetConfig(const COmxILFsm& aFsm,
							OMX_INDEXTYPE aConfigIndex,
							TAny* apComponentConfigStructure) const;

	OMX_ERRORTYPE SetConfig(COmxILFsm& aFsm,
							OMX_INDEXTYPE aConfigIndex,
							const TAny* apComponentConfigStructure);

	OMX_ERRORTYPE GetExtensionIndex(const COmxILFsm& aFsm,
									OMX_STRING aParameterName,
									OMX_INDEXTYPE* apIndexType) const;

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);


	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandFlush(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortDisable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandMarkBuffer(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the OpenMAX IL OMX_StateLoaded state
*/
class COmxILFsm::COmxILStateLoaded : public COmxILFsm::COmxILState
	{

public:

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};


/**
   Concrete class that implements the intermediate state OMX_StateLoaded ->
   OMX_StateIdle.
*/
class COmxILFsm::COmxILStateLoadedToIdle : public COmxILFsm::COmxILStateLoaded
	{

public:


	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the OpenMAX IL OMX_StateWaitForResources
   state

   NOTE: This state is here only to provide the basic functionality of
   transitioning form OMX_StateLoaded to OMX_StateWaitForResources and
   viceversa. Transition from OMX_StateWaitForResources to OMX_StateIdle is not
   implemented at this stage since a Resource Manager is not present
   yet. Whenever a Resource Manager becomes available, a
   COmxILStateWaitForResourcesToIdle substate should be implemented to handle
   the allocation of buffers mandated by the standard in this transition.

*/
class COmxILFsm::COmxILStateWaitForResources : public COmxILFsm::COmxILState
	{

public:

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the OpenMAX IL COmxILStateIdle state
*/
class COmxILFsm::COmxILStateIdle : public COmxILFsm::COmxILState
	{

public:

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandFlush(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the intermediate state OMX_StateIdle ->
   OMX_StateLoaded.
*/
class COmxILFsm::COmxILStateIdleToLoaded : public COmxILFsm::COmxILStateIdle
	{

public:

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the OpenMAX IL COmxILStateExecuting state
*/
class COmxILFsm::COmxILStateExecuting : public COmxILFsm::COmxILState
	{

public:

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Class used to abstract the commonalities of the transitions from
   OMX_StatePause or OMX_StateExecuting to OMX_StateIdle.
*/
class COmxILFsm::MOmxILPauseOrExecutingToIdle
	{

protected:

	OMX_ERRORTYPE ReturnThisBuffer(COmxILFsm& aFsm,
								   OMX_BUFFERHEADERTYPE* apBuffer,
								   OMX_DIRTYPE aDirection);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the intermediate state OMX_StateExecuting ->
   OMX_StateIdle.
*/
class COmxILFsm::COmxILStateExecutingToIdle :
	public COmxILFsm::COmxILStateExecuting,
	private COmxILFsm::MOmxILPauseOrExecutingToIdle

	{

public:

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the OpenMAX IL COmxILStatePause state
*/
class COmxILFsm::COmxILStatePause : public COmxILFsm::COmxILState
	{

public:

	OMX_STATETYPE GetState() const;

	OMX_ERRORTYPE SetParameter(COmxILFsm& aFsm,
							   OMX_INDEXTYPE aParamIndex,
							   const TAny* apComponentParameterStructure);

	OMX_ERRORTYPE PopulateBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE** appBufferHdr,
								 OMX_U32 aPortIndex,
								 OMX_PTR apAppPrivate,
								 OMX_U32 aSizeBytes,
								 OMX_U8* apBuffer,
								 TBool& portPopulationCompleted);

	OMX_ERRORTYPE FreeBuffer(COmxILFsm& aFsm,
							 OMX_U32 aPortIndex,
							 OMX_BUFFERHEADERTYPE* apBuffer,
							 TBool& aPortDepopulationCompleted);

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE ComponentTunnelRequest(COmxILFsm& aFsm,
										 OMX_U32 aPort,
										 OMX_HANDLETYPE aTunneledComp,
										 OMX_U32 aTunneledPort,
										 OMX_TUNNELSETUPTYPE* apTunnelSetup);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	OMX_ERRORTYPE CommandPortEnable(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

/**
   Concrete class that implements the intermediate state OMX_StatePause ->
   OMX_StateIdle.
*/
class COmxILFsm::COmxILStatePauseToIdle
	: public COmxILFsm::COmxILStatePause,
	  private COmxILFsm::MOmxILPauseOrExecutingToIdle
	{

public:

	OMX_ERRORTYPE EmptyThisBuffer(COmxILFsm& aFsm,
								  OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE FillThisBuffer(COmxILFsm& aFsm,
								 OMX_BUFFERHEADERTYPE* apBuffer);

	OMX_ERRORTYPE CommandStateSet(
		COmxILFsm& aFsm,
		const TOmxILCommand& aCommand);

	};

#include "omxilstate.inl"

#endif // OMXILSTATE_H


