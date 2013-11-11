/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef UTIL_H
#define UTIL_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/policy.h>
#include <test/testexecutelogger.h>

const TDesC& OptionToString(UserPromptService::CPolicy::TOptions aOption);
TInt StringToOptionL(const TDesC& aString);
TBool CheckExpectedError(CTestExecuteLogger& aLogger, TInt aExpected, TInt aActual);

#endif // UTIL_H
