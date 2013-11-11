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
#include "ct/tctfindtokentypesbyinterfaceandattribute.h"
#include "ct/ccttokentypeinfo.h"

EXPORT_C TCTFindTokenTypesByInterfaceAndAttribute::TCTFindTokenTypesByInterfaceAndAttribute(const TArray<TUid> aInterfaces,
																							const TArray<TCTTokenTypeAttribute> aAttributes)
	: iInterfaces(aInterfaces), iAttributes(aAttributes)
	{
	}

TBool TCTFindTokenTypesByInterfaceAndAttribute::Accept(const CCTTokenTypeInfo& aToken) const
	{
	// For every interface in the array, check it's supported. If not, give up.
	TInt count = iInterfaces.Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		if (aToken.Interfaces().Find(iInterfaces[ii])== KErrNotFound)
			{
			return EFalse;
			}
		}
	// If they're all present, check the attributes
	count = iAttributes.Count();
	for (TInt jj = 0; jj < count; jj++)
		{
		TInt found = aToken.Attributes().Find(iAttributes[jj]);
		if (found == KErrNotFound || 
			aToken.Attributes()[found].iVal != iAttributes[jj].iVal)
			{
			return EFalse;
			}
		}
	// If they're all OK, the token type is OK.
	return ETrue;
	}
