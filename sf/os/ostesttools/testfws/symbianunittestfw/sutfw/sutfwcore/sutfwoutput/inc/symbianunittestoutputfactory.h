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
#ifndef SYMBIANUNITTESTOUTPUTFACTORY_H
#define SYMBIANUNITTESTOUTPUTFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestOutputFormatter;

// CLASS DECLARATION
class SymbianUnitTestOutputFactory
    {
    public: // New functions    
        
        static CSymbianUnitTestOutputFormatter* CreateOutputLC( 
            const TDesC& aFileNameNoExtension,
            const TDesC& aFileExtension );
    };

#endif // SYMBIANUNITTESTOUTPUTFACTORY_H
