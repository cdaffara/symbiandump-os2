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
#include "ct.h"
#include "ct/logger.h"

EXPORT_C void MCTTokenInterface::Release()
	{
	LOG(_L("MCTTokenInterface::Release: token interface released"));
	LOG_INC_INDENT();
	
	// Get a copy of the token now before we delete the object
	MCTToken& token = Token();
	// Delete the object
	DoRelease();
	// THE OBJECT MAY NOW BE DELETED
	// Tell the token what's happened.
	token.Release();
	
	LOG_DEC_INDENT();
	}

EXPORT_C void MCTTokenInterface::DoRelease()
	{
	LOG(_L("MCTTokenInterface::DoRelease: destroying token interface"));
	LOG_INC_INDENT();
	
	delete this;
	
	LOG_DEC_INDENT();
	}
