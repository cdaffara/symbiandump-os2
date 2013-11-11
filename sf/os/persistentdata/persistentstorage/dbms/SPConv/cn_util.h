// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TSPConvUtil class
// 
//

#ifndef __CN_UTIL_H__
#define __CN_UTIL_H__

#include <e32std.h>

//Forward declarations
class CCommandLineArguments;
struct TCmdLinePrm;
class RFs;

/**
TSPConvUtil is a utility class with a set of static methods, which do things, like:
parsing command line arguments, printing messages, file existence checking, constructing
binary file name, extracting the UID from a binary file name.
@internalComponent
*/
class TSPConvUtil
	{
public:
	static void ParseL(const CCommandLineArguments& aCmdLineParser, TCmdLinePrm& aCmdLinePrm);
	static TBool FileExists(RFs& aFs, const TDesC& aFile);
	static void ConstructBinFileNameL(TDes& aFile);
	static TUid UidFromFileNameL(const TDesC& aFile);
	static void Print(const TDesC& aText);
	static void Print(const TDesC& aFormat, TInt aNumber);
	static void Print(const TDesC& aFormat, const TDesC& aText);
	static void Print(const TDesC& aFormat, const TDesC& aText1, const TDesC& aText2);
private:
	static TBool iPromptOnError;
	};

#endif//__CN_UTIL_H__
