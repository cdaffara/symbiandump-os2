// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// COMSDBGSREXE.CPP
// Implements the Flogger server process entry point
// 
//

/**
 @file
 @internalComponent
*/

#include "comsdbgstd.h"


GLDEF_C TInt E32Main()
/**
 * Flogger server process entry point
 *
 * @note Flogger server is intended to run forever, so it starts automatically.
@internalComponent
 */
	{
   	RProcess().SecureApi(RProcess::ESecureApiOn);
   	RProcess().DataCaging(RProcess::EDataCagingOn);

	return FLogger::Run();
	}



