// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <coeutils.h>
#include <coemain.h>
#include <ConeResLoader.h>

EXPORT_C RConeResourceLoader::RConeResourceLoader(CCoeEnv& aCoeEnv)
	{
	// Uses Placement new. It is used to instruct the constructor of this class to run on "this" memory used as argument for the new operator
	new (this) RCoeResourceLoader(aCoeEnv); 
	}	

EXPORT_C TInt RConeResourceLoader::Open(TFileName& aFileName)
    { return reinterpret_cast<RCoeResourceLoader*>(this)->Open(aFileName); }

EXPORT_C void RConeResourceLoader::OpenL(TFileName& aFileName)
    { reinterpret_cast<RCoeResourceLoader*>(this)->OpenL(aFileName); }

EXPORT_C void RConeResourceLoader::Close()
    { reinterpret_cast<RCoeResourceLoader*>(this)->Close(); }

// End of File
