/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements part of UsbMan USB Class Controller Framework.
* All USB classes to be managed by UsbMan must derive
* from this class.
*
*/

/**
 @file
*/

#include <cusbclasscontrollerplugin.h>
#include <ecom/ecom.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbClassControllerPlugInTraces.h"
#endif

	
/**
 * Constructor.
 *
 */

EXPORT_C CUsbClassControllerPlugIn::CUsbClassControllerPlugIn(
	MUsbClassControllerNotify& aOwner, TInt aStartupPriority)
	: CUsbClassControllerBase(aOwner, aStartupPriority)
	{
	}

/**
 * Constructs a CUsbClassControllerPlugIn object.
 *
 * @return	A new CUsbClassControllerPlugIn object	
 */

EXPORT_C CUsbClassControllerPlugIn* CUsbClassControllerPlugIn::NewL(TUid aImplementationId, 
	MUsbClassControllerNotify& aOwner) 
	{
	OstTraceFunctionEntry0( CUSBCLASSCONTROLLERPLUGIN_NEWL_ENTRY ); 

	return (reinterpret_cast<CUsbClassControllerPlugIn*>(REComSession::CreateImplementationL
		(aImplementationId, _FOFF(CUsbClassControllerPlugIn, iPrivateEComUID),
			(TAny*) &aOwner)));
	}


/**
 * Destructor.
 */
EXPORT_C CUsbClassControllerPlugIn::~CUsbClassControllerPlugIn()
	{
	REComSession::DestroyedImplementation(iPrivateEComUID);
	}

