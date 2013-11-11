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

#ifndef SYMBIANUNITTESTDLLNAMEPARSER_H
#define SYMBIANUNITTESTDLLNAMEPARSER_H

//  INCLUDES
#include <delimitedparser16.h>

// CLASS DECLARATION
class TSymbianUnitTestDllNameParser : public TDelimitedParserBase16
    {
    public: // New functions

        inline TSymbianUnitTestDllNameParser();
        inline void Parse( const TDesC& aValue );
    };

#include "symbianunittestdllnameparser.inl"

#endif // SYMBIANUNITTESTDLLNAMEPARSER_H

// End of File
