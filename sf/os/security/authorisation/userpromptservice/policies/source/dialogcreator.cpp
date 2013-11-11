/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include <ups/dialogcreator.h>
#include <ups/upserr.h>
#include <ecom/ecom.h>
#include "upslog.h"

using namespace UserPromptService;

EXPORT_C CDialogCreator::CDialogCreator()
/**
Constructor
*/
: CActive(EPriorityStandard)
	{
	}

EXPORT_C CDialogCreator::~CDialogCreator()
/**
Destructor
*/
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

EXPORT_C CDialogCreator* CDialogCreator::NewL(const TUid& aDialogCreatorImplementationId)
/**
Creates a new dialog creator.
@param	aDialogCreatorImplementationId	The UID of the dialog creator implemenation
										as specified in the policy file.

@return A pointer to the new dialog creator implementation.
*/
	{
	TAny* plugin(0);
	DEBUG_PRINTF2(_L8("Instantiating dialog creator 0x%08x"), aDialogCreatorImplementationId);
	TRAPD(err, plugin = 
	REComSession::CreateImplementationL(aDialogCreatorImplementationId,
		_FOFF(CDialogCreator,iDtor_ID_Key)));

	if (err != KErrNone)
		{
		DEBUG_PRINTF3(_L8("Failed to instantiate dialog creator 0x%08x, err = %d"),
					  aDialogCreatorImplementationId.iUid, err);
		}

	if (err == KErrNotFound)
		{
		err = KErrUpsMissingDialogCreator;	
		}
	User::LeaveIfError(err);	 
	return reinterpret_cast<CDialogCreator*>(plugin);		
	}

EXPORT_C TInt CDialogCreator::GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1)
/**
Allows extension of this interface. Calls Extension_

@param aExtensionId		The UID of the interface to instantiate.
@param a0				A reference to a pointer that should be set to the newly
						instantiated object.
@param a1				Data specific to the instantiate of the specified interface.

@return KErrNone if the extension is supported or KErrNotSupported if the extension is not
		recognised; otherwise, a system wide error may be returned.
*/
	{
	return Extension_(aExtensionId, a0, a1);
	}

EXPORT_C TInt CDialogCreator::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return CActive::Extension_(aExtensionId, a0, a1);
	}
