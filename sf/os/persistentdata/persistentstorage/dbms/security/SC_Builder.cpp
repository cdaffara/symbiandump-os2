// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TPolicyDomainBuilder implementation
// 
//

#include "SC_Policy.h"

namespace DBSC
{

/**
The method sets the database policy to the related CPolicyDomain collection.
@param aDbPolicy A pointer to CDbPolicy instance, which has to be set to 
                  the related CPolicyDomain collection. CPolicyDomain collection takes the
				  ownership on the supplied CDbPolicy instance.
@leave KErrAlreadyExists If the database policy was already set.
*/
void TPolicyDomainBuilder::SetDbPolicyL(CDbPolicy* aDbPolicy)
	{
	__ASSERT(aDbPolicy);
	if(iPolicyDomain.iDbPolicy)
		{
		__LEAVE(KErrAlreadyExists);
		}
	iPolicyDomain.iDbPolicy = aDbPolicy;
	}

} //end of - namespace DBSC
