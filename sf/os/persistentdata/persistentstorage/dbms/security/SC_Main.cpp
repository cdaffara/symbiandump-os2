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
// TPolicySpaceFactory class
// 
//

#include "SC_PolicySpace.h"

namespace DBSC
{

/**
A factory method for creating CPolicySpace instances.
@param aFs A reference to a file server session object
@param aPrivatePath A reference to the DBMS server private directory.
@return A MPolicySpace pointer to just created CPolicySpaceinstance.
@leave System-wide error codes, including KErrNoMemory
*/
MPolicySpace* TPolicySpaceFactory::NewPolicySpaceL(RFs& aFs, const TDesC& aPrivatePath)
	{
	return CPolicySpace::NewL(aFs, aPrivatePath);
	}

} //end of - namespace DBSC
