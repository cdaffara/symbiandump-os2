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
* Defines inline functions used by both SCS client and server implementations,
* but not with the client API user.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef SCSCOMMON_INL
#define SCSCOMMON_INL

#include <scs/scscommon.h>

namespace ScsImpl
	{

	inline void ExtractScsAndImplFunctions(const RMessage2& aMessage, TScsFunction* aScsFunc, TInt* aImplFunc)
	/**
		Takes the function code from the supplied message and breaks it into the SCS
		field and implementation code.

		@param	aMessage		Message contains function code supplied to server
								from client-side implementation.
		@param	aScsFunc		If not NULL then on return this contains the SCS code.
		@param	aImplFunc		If not NULL then on return this contains the implementation custom code.
	 */
		{
		TInt f = aMessage.Function();
		if (aScsFunc != 0)
			*aScsFunc =  static_cast<TScsFunction>(f & KScsFunctionMask);
		
		if (aImplFunc != 0)
			*aImplFunc = f & ~KScsFunctionMask;
		}

#ifdef _DEBUG
	inline TBool ScsFieldUsed(TInt aFunction)
	/**
		This function is used on the client-side in debug builds to
		ensure the (sub)session function identifier which is being
		sent to the server does not yet contain any SCS fields.
	 */
		{
		return aFunction & KScsFunctionMask;
		}
#endif
	}	// namespace UserPromptService

#endif	// #ifndef SCSCOMMON_INL

