// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



// INCLUDE FILES
#include <StringLoader.h>
#include <tultextresourceutils.h>

EXPORT_C void StringLoader::Load(TDes& aDest, TInt aResourceId, CCoeEnv* aLoaderEnv)
	{ TulTextResourceUtils::Load(aDest, aResourceId, aLoaderEnv); }
	
EXPORT_C void StringLoader::Format(TDes& aDest, const TDesC& aSource, TInt aPosition, TInt aSubs)
	{ TulTextResourceUtils::Format(aDest, aSource, aPosition, aSubs); }
	
EXPORT_C void StringLoader::Format(TDes& aDest, const TDesC& aSource, TInt aPosition, const TDesC& aSubs)
	{ TulTextResourceUtils::Format(aDest, aSource, aPosition, aSubs); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, TInt aInt, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aInt, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, const TDesC& aString, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aString, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, const TDesC& aString, TInt aInt, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aString, aInt, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aInts, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, const MDesCArray& aStrings, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aStrings, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadL(TInt aResourceId, const MDesCArray& aStrings, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadL(aResourceId, aStrings, aInts, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, TInt aInt, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aInt, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, const TDesC& aString, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aString, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, const TDesC& aString, TInt aInt, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aString, aInt, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aInts, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, const MDesCArray& aStrings, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aStrings, aLoaderEnv); }
	
EXPORT_C HBufC* StringLoader::LoadLC(TInt aResourceId, const MDesCArray& aStrings, const CArrayFix<TInt>& aInts, CCoeEnv* aLoaderEnv)
	{ return TulTextResourceUtils::LoadLC(aResourceId, aStrings, aInts, aLoaderEnv); }
