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

#ifndef OMXILPROCESSINGFUNCTION_H
#define OMXILPROCESSINGFUNCTION_H

#include <e32base.h>

#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Types.h>
#include <openmax/il/khronos/v1_x/OMX_Other.h>

//#include "omxilfsm.h"
#include <openmax/il/common/omxilstatedefs.h>

// Forward declarations
class MOmxILCallbackNotificationIf;

/**
   Base class for component-specific implementations of the Processing Function
   abstraction. Component implementations realize within a Processing Function
   object the codec specific logic. Component implementations may choose
   whether to implement the Processing Function functionality in an additional
   independent thread or in-context with the framework. The interface is
   designed to generalize the notification of the OpenMAX IL commands, OpenMAX
   IL state changes and input/output buffer arrivals to the codec
   itself. Implementations of this interface are intended to communicate with
   the entity in charge of the egress component communication, the Callback
   Manager.
 */
class COmxILProcessingFunction : public CBase
	{

public:

	IMPORT_C ~COmxILProcessingFunction();

	/**
	   Framework notification of a state transition in the component.

	   @param aNewState The new state.

	   @return OMX_ERRORTYPE
	 */
	virtual OMX_ERRORTYPE StateTransitionIndication(
		TStateIndex aNewState) = 0;

	/**
	   Framework notification of a buffer flushing command.

	   @param aPortIndex The port that is being flushed.

	   @param aDirection The direction of the port that is being flushed.

	   @return OMX_ERRORTYPE
	 */
	virtual OMX_ERRORTYPE BufferFlushingIndication(
		TUint32 aPortIndex,
		OMX_DIRTYPE aDirection) = 0;

	/**
	   Framework notification of an OpenMAX IL Param structure change.

	   @param aParamIndex The index of the param structure that has changed.

	   @param apComponentParameterStructure The param structure.

	   @return OMX_ERRORTYPE
	 */
	virtual OMX_ERRORTYPE ParamIndication(
		OMX_INDEXTYPE aParamIndex,
		const TAny* apComponentParameterStructure) = 0;

	/**
	   Framework notification of an OpenMAX IL Config structure change.

	   @param aConfigIndex The index of the config structure that has changed.

	   @param apComponentConfigStructure The config structure.

	   @return OMX_ERRORTYPE
	 */
	virtual OMX_ERRORTYPE ConfigIndication(
		OMX_INDEXTYPE aConfigIndex,
		const TAny* apComponentConfigStructure) = 0;

	/**
	   Framework notification of a buffer arrival.

	   @param apBufferHeader The header of the buffer to be processed.

	   @param aDirection The direction of the port that has received the
	   buffer.

	   @return OMX_ERRORTYPE
	 */
	virtual OMX_ERRORTYPE BufferIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection) = 0;

	/**
	   Framework notification of a buffer removal. Here the Processing Function
	   object is being notified that a buffer needs to be removed from its
	   internal queues. At present, a synchronous implementation of this method
	   is assumed. Buffer removal notifications may be originated by the
	   framework in the following scenarios:

	   -# After an OMX_FreeBuffer call has been received from the IL Client or
	   from another component.

	   -# After a port flushing command has been received from the IL Client
	   AND the port is tunnelled and buffer supplier.

	   -# After a transition OMX_StateExecuting/OMX_StatePause -> OMX_StateIdle
            has been requested by the IL Client AND the port associated to the
            buffer is tunnelled and buffer supplier.

	   -# After a port disable command has been received from the IL Client AND
            the port associated to the buffer is tunnelled and buffer supplier.

	   @param apBufferHeader The buffer header to be removed from internal
	   queues.

	   @param aDirection The direction of the port associated to the buffer.

	   @return OMX_BOOL Etrue if the Processing Function has successfully
	    removed the buffer header from its internal queues, EFalse otherwise.
	 */
	virtual OMX_BOOL BufferRemovalIndication(
		OMX_BUFFERHEADERTYPE* apBufferHeader,
		OMX_DIRTYPE aDirection) = 0;

	IMPORT_C virtual OMX_ERRORTYPE ComponentRoleIndication(
		TUint aComponentRoleIndex);

	/**
	   Framework notification of a buffer arrival on a OMX_PortDomainOther;
	   normally the interested component would just extract the buffer and 
	   let the port manager returns the buffer to the buffer supplier
	   component via callback immediately.

	   @param apBufferHeader 	The header of the buffer to be processed.
	   @param aDirection 		The direction of the port that has received
	   							the buffer.
	   @return OMX_ERRORTYPE
	 */
	IMPORT_C virtual OMX_ERRORTYPE MediaTimeIndication(const OMX_TIME_MEDIATIMETYPE& );
	
protected:

	IMPORT_C COmxILProcessingFunction(
		MOmxILCallbackNotificationIf& aCallbacks);
	
	//From CBase
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);

protected:

	MOmxILCallbackNotificationIf& iCallbacks;

	};

#endif // OMXILPROCESSINGFUNCTION_H
