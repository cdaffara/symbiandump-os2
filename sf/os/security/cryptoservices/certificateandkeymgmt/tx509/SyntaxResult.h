/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SyntaxTest.H
* CSyntaxResult class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSyntaxResult_H
#define __CSyntaxResult_H

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>

class CSyntaxResult : public CBase
	{
public:
	static CSyntaxResult* NewL();
	static CSyntaxResult* NewLC();
	~CSyntaxResult();

	void SetFilename(const TDesC8 &aFilename);
	void SetResult(const TDesC8 &aResult);

	inline void GetFilename(TPtrC &aFilename) { aFilename.Set(iFilename);};
	inline TInt Result(void) { return(iResult);};

private:
	CSyntaxResult();
	void ConstructL();
	TBuf<128> iFilename;
	TInt iResult;
	};

#endif
