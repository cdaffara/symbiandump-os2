// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/

#include "CachedCustomResolver.h"

const TInt KMyVersion = 9;
/** Serves as signature of the DLL so that test code 
knows for sure if it gets the correct version.
*/
TInt CExampleResolver::GetSignature() const
	{
	return KMyVersion;
	}
