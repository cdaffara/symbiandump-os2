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

#ifndef SYMBIANUNITTESTDLLNAMEPARSER_INL
#define SYMBIANUNITTESTDLLNAMEPARSER_INL

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
inline TSymbianUnitTestDllNameParser::TSymbianUnitTestDllNameParser() 
    : TDelimitedParserBase16()
    {
    SetDelimiter(',');
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
inline void TSymbianUnitTestDllNameParser::Parse( const TDesC& aValue )
    {
    TDelimitedParserBase16::Parse(aValue);
    }

#endif // SYMBIANUNITTESTDLLNAMEPARSER_INL

// End of File
