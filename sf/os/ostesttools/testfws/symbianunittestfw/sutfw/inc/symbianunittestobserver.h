/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SYMBIANUNITTESTOBSERVER_H
#define SYMBIANUNITTESTOBSERVER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
class MSymbianUnitTestObserver
    {
    public: // New functions
        
        virtual void TestPass(const TDesC& aTestCaseName) = 0;
        virtual void TestFailed(const TDesC& aTestCaseName, const TDesC8& aErrMsg) = 0;
    };

#endif // SYMBIANUNITTESTOBSERVER_H

