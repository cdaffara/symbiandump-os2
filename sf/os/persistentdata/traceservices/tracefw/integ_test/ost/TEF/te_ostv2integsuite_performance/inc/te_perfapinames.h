// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TE_UPTAPINAMES_
#define TE_UPTAPINAMES_



#include "te_instrumentation_enum.h"

/**
This is a user-side utility class to return the name strings for the various UTrace calls

@internalTechnology
@prototype
 */

class TApiNames 
	{
	public:
	TApiNames();
	~TApiNames();
	TPtrC8 GetApiIdString(const TInt aApiId);
	
	private:
	};



#endif /*TE_UPTAPINAMES_*/
