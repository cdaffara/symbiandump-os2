// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HALSettings main function
// 
//

/**
 @file
 @internalComponent
*/
#include <e32base.h>
#include "halfiles.h"

/**
Implements the HALSettings command line entry point.
HALSettingsManager() is not a leaving function so no cleanup stack
@return KErrNone if successful, otherwise any system wide error code.
*/ 
GLDEF_C TInt E32Main()		   
	{
	__UHEAP_MARK;
	
	TInt result = HALSettingsManager();

	__UHEAP_MARKEND;
	
	return result;
	}
