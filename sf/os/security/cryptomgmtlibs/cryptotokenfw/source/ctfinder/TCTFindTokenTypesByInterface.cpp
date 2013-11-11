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
#include "ct/tctfindtokentypesbyinterface.h"
#include "ct/ccttokentypeinfo.h"

EXPORT_C TCTFindTokenTypesByInterface::TCTFindTokenTypesByInterface(const TArray<TUid> aInterfaces)
	: iInterfaces(aInterfaces)
	{
	}

TBool TCTFindTokenTypesByInterface::Accept(const CCTTokenTypeInfo& aToken) const
	{
	// For every interface in the array, check it's supported. If not, give up.
	TInt count = iInterfaces.Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		if (aToken.Interfaces().Find(iInterfaces[ii]) == KErrNotFound)
			{
			return EFalse;
			}
		}
	// If they're all present, the token type is OK
	return ETrue;
	}
