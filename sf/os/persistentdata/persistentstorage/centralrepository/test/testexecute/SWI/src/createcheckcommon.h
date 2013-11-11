/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/
#if (!defined __REPOSITORYCREATECHECKCOMMON_H__)
#define __REPOSITORYCREATECHECKCOMMON_H__

// Define the lengths of buffers to use for checking or performing functions
// on settings.
#define REPCHECKCREATE_ABSBUFLEN 20
#define REPCHECKCREATE_KEYBUFLEN 20
#define REPCHECKCREATE_VALBUFLEN 20
#define REPCHECKCREATE_MAXSTRINGLEN 30

// This file defines the names for values that can be defined in the
// config file for repository checking / setting creation tests.
_LIT(KRepositoryNumInts, "NumInts");
_LIT(KIntKeyPrefix,      "IntKey" );
_LIT(KIntValPrefix,      "IntVal" );
_LIT(KIntMetaPrefix,      "IntMeta" );
_LIT(KIntFunction,       "Intfn" );
_LIT(KIntExpectedError, "IntExpErr");

_LIT(KRepositoryNumReals,"NumReals");
_LIT(KRealKeyPrefix,     "RealKey" );
_LIT(KRealValPrefix,     "RealVal" );
_LIT(KRealMetaPrefix,      "RealMeta" );
_LIT(KRealFunction,      "Realfn" );
_LIT(KRealExpectedError, "RealExpErr");

_LIT(KRepositoryNumStrings, "NumStrings");
_LIT(KStringKeyPrefix,      "StringKey" );
_LIT(KStringValPrefix,      "StringVal" );
_LIT(KStringFunction,       "Stringfn" );
_LIT(KStringExpectedError,  "StringExpErr");

#endif // __REPOSITORYCREATECHECKCOMMON_H_
