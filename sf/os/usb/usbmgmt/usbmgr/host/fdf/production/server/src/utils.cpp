/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "utils.h"
#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "utilsTraces.h"
#endif


#define LOG Log()

//*****************************************************************************
// Code relating to the cleanup stack item which 'Remove's a given TUint from 
// an RArray.

TArrayRemove::TArrayRemove(RArray<TUint>& aDeviceIds, TUint aDeviceId)
 :	iDeviceIds(aDeviceIds), 
	iDeviceId(aDeviceId) 
	{
	}

TArrayRemove::~TArrayRemove()
	{
	}

void Remove(TAny* aArrayRemove)
	{
	OstTraceFunctionEntry0( FDF_UTILS_REMOVE_ENTRY );
	
	TArrayRemove* arrayRemove = reinterpret_cast<TArrayRemove*>(aArrayRemove);

	const TUint count = arrayRemove->iDeviceIds.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( arrayRemove->iDeviceIds[ii] == arrayRemove->iDeviceId )
			{
            OstTrace0( TRACE_NORMAL, FDF_UTILS_REMOVE, "::matching device id" );
			arrayRemove->iDeviceIds.Remove(ii);
			break;
			}
		}
	OstTraceFunctionExit0( FDF_UTILS_REMOVE_EXIT );
	}

void CleanupRemovePushL(TArrayRemove& aArrayRemove)
	{
	TCleanupItem item(Remove, &aArrayRemove);
	CleanupStack::PushL(item);
	}

//*****************************************************************************
