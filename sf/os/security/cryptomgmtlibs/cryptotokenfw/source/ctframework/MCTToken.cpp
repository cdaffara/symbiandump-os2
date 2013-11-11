/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <ct.h>
#include "ct/logger.h"

// As it's easier for the implementer to initialise the ref
// count to 0 than 1, reference count values are 1 less than you
// might expect. The object should be deleted when the count reaches -1
EXPORT_C void MCTToken::Release()
	{
	--ReferenceCount();

	LOG2(_L("MCTToken::Release: token released, ref count == %d: %-32S"), ReferenceCount(), &Label());
	LOG_INC_INDENT();
	
	if (ReferenceCount() < 0)
		{
		MCTTokenType& tokenType = TokenType();
		DoRelease();
		// FROM THIS POINT ON, THE OBJECT MAY BE DELETED
		tokenType.Release();
		}
	
	LOG_DEC_INDENT();
	}
 
EXPORT_C void MCTToken::DoRelease()
	{
	LOG1(_L("MCTToken::DoRelease: destroying token: %-32S"), &Label());
	LOG_INC_INDENT();

	delete this;

	LOG_DEC_INDENT();
	}

// Dummy removal notification for use on non-removable tokens
EXPORT_C void MCTToken::NotifyOnRemoval(TRequestStatus& /*aStatus*/)
	{
	}

EXPORT_C void MCTToken::CancelNotify()
	{
	}

// Base implementation of GetInterface. This just does the reference counting
EXPORT_C void MCTToken::GetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus)
	{
	++ReferenceCount();
	LOG2(_L("MCTToken::GetInterface: getting interface, ref count == %d: %-32S"), ReferenceCount(), &Label());
	LOG_INC_INDENT();

	DoGetInterface(aRequiredInterface, aReturnedInterface, aStatus);
	
	LOG_DEC_INDENT();
	}

/** Cancel a GetInterface operation */
EXPORT_C void MCTToken::CancelGetInterface()
	{
	if (DoCancelGetInterface())
		{
		Release();
		}
	}

void MCTToken::ObjectCreated()
	{
	++ReferenceCount();
	LOG2(_L("MCTToken::ObjectCreated: token object created, ref count == %d: %-32S"), ReferenceCount(), &Label());
	}
