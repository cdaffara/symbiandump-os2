/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef STRINGLOADER_H
#define STRINGLOADER_H

#include <coemain.h>
#include <biditext.h>

class CCoeEnv;

NONSHARABLE_CLASS(StringLoader)
/**
@deprecated
@see TulTextResourceUtils
@note For compatibility with S60 only
*/
    {
public:
    IMPORT_C static void Load(TDes& aDest, TInt aResourceId, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static void Format(TDes& aDest, const TDesC& aSource, TInt aPosition, TInt aSubs);
    IMPORT_C static void Format(TDes& aDest, const TDesC& aSource, TInt aPosition, const TDesC& aSubs);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, TInt aInt, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, const TDesC& aString, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, const TDesC& aString, TInt aInt, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, const MDesCArray& aStrings, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadL(TInt aResourceId, const MDesCArray& aStrings, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, TInt aInt, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, const TDesC& aString, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, const TDesC& aString, TInt aInt, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, const MDesCArray& aStrings, CCoeEnv* aLoaderEnv = NULL);
    IMPORT_C static HBufC* LoadLC(TInt aResourceId, const MDesCArray& aStrings, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv = NULL);
    };

#endif      // STRINGLOADER_H

