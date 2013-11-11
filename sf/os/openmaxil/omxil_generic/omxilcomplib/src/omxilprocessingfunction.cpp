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

#include "log.h"
#include <openmax/il/common/omxilprocessingfunction.h>


EXPORT_C
COmxILProcessingFunction::COmxILProcessingFunction(
	MOmxILCallbackNotificationIf& aCallbacks)
	:
	iCallbacks(aCallbacks)
	{
    DEBUG_PRINTF(_L8("COmxILProcessingFunction::COmxILProcessingFunction"));

	}

EXPORT_C
COmxILProcessingFunction::~COmxILProcessingFunction()
	{
    DEBUG_PRINTF(_L8("COmxILProcessingFunction::~COmxILProcessingFunction"));

	}

/**
   This is an indication from the framework that the IL Client wants to set the
   default role that a standard component is assuming. Therefore, the role
   defaults need to be reloaded into all ports and the Processing
   Function. This method is the indication to the processing function that the
   role defaults have been set into the component ports. A component not
   claiming support of any of the standard component roles does not need to
   override this method.

   @param aComponentRoleIndex An integer associated to the new component role.

   @return OMX_ERRORTYPE
 */
EXPORT_C OMX_ERRORTYPE
COmxILProcessingFunction::ComponentRoleIndication(TUint /* aComponentRoleIndex */)
	{
	return OMX_ErrorNotImplemented;
	}

EXPORT_C
OMX_ERRORTYPE COmxILProcessingFunction::MediaTimeIndication(const OMX_TIME_MEDIATIMETYPE& )
	{
	return OMX_ErrorNotImplemented;
	}

EXPORT_C
TInt COmxILProcessingFunction::Extension_(TUint aExtensionId, TAny *&a0, TAny *a1)
	{
	return CBase::Extension_(aExtensionId, a0, a1);
	}
