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


#include "ct.h"
#include "ct/logger.h"

EXPORT_C MCTTokenObject::MCTTokenObject(MCTToken& aToken)
	{
	LOG(_L("MCTTokenObject::MCTTokenObject: token object created")); // can't call Label at this point
	LOG_INC_INDENT();
	
	aToken.ObjectCreated();
	
	LOG_DEC_INDENT();
	}

EXPORT_C void MCTTokenObject::Release()
	{
	LOG1(_L("MCTTokenObject::Release: token object released: %-32S"), &Label());
	LOG_INC_INDENT();

	// Take a copy of the token as we need to release it last
	MCTToken& token = Token();
	// Release the object
	DoRelease();
	// this MAY NOT EXIST ANY MORE!!
	// Release the token.
	token.Release();

	LOG_DEC_INDENT();	
	}

EXPORT_C void MCTTokenObject::DoRelease()
	{
	LOG1(_L("MCTTokenObject::DoRelease: destroying token object: %-32S"), &Label());
	LOG_INC_INDENT();

	delete this;

	LOG_DEC_INDENT();
	}

EXPORT_C void MCTTokenObject::AddTokenRef()
	{
	++(Token().ReferenceCount());
	}
