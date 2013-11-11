/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



#ifndef CONERESLOADER_H
#define CONERESLOADER_H

#include <e32std.h>

class CCoeEnv;

NONSHARABLE_CLASS(RConeResourceLoader)
/**
@deprecated
@see RCoeResourceLoader
@note For compatibility with S60 only
*/
	{
public:
	IMPORT_C RConeResourceLoader(CCoeEnv& aCoeEnv);
	IMPORT_C TInt Open(TFileName& aFileName);
	IMPORT_C void OpenL(TFileName& aFileName);
	IMPORT_C void Close();
private:
	// Dummy members to make this the same size as a RCoeResourceLoader so
	// in-place construction of the RCoeResourceLoader will work
	void* iDummy1;	// was CCoeEnv&
	TInt iDummy2;	// was TInt
	};

#endif	// CONERESLOADER_H

