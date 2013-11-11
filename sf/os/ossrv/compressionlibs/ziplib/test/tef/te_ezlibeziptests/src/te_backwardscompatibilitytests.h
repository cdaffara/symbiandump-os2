/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ezlibeziptests.h
* 
*
*/



#ifndef __TE_BACKWARDSCOMPATIBILITYTESTS_H_
#define __TE_BACKWARDSCOMPATIBILITYTESTS_H_

#include "te_ezlibeziptests.h"

/* Backwards Compatibility tests */
_LIT(KTestDefOldInfNewCompatibilityL, "TestDeflateOldInflateNewCompatibitlityL");
_LIT(KTestDefNewInfOldCompatibilityL, "TestDeflateNewInflateOldCompatibitlityL");
_LIT(KTestGZipDefOldInfNewCompatibilityL, "TestGZipDeflateOldInflateNewCompatibitlityL");
_LIT(KTestGZipDefNewInfOldCompatibilityL, "TestGZipDeflateNewInflateOldCompatibitlityL");
_LIT(KTestZipArchiveDecompressCompatibilityL, "TestZipArchiveDecompressCompatibitlityL");
_LIT(KTestOSCompatibilityL, "TestOSCompatibilityL");
_LIT(KTestOSGZipCompatibilityL, "TestOSGZipCompatibilityL");

const unsigned int TheBufferSize = 1024;

class CBackwardsCompatibilityTests : public CEZlibEZipTests
	{
public:
	CBackwardsCompatibilityTests (const TDesC &aStepName);	
	virtual TVerdict doTestStepL();
private:
	void GetCDirLC(const TFileName &aFolderPath, const TFileName &aFolderName, TFileName &aCDirFolderPath, CDir* &aCDir);
	TBool CompareFileDataL(TFileName &aInputFile, TFileName &bOutputFile);
	TBool CompareFolderDataL(const CDir& aEzlibDir, const TFileName& aEzlibDirPath, const CDir& aEzlib2Dir, const TFileName& aEzlib2DirPath);
	TVerdict ZipArchiveDecompressCompatibitlityL();
	TVerdict TestDeflateOldInflateNewCompatibitlityL();
	TVerdict TestDeflateNewInflateOldCompatibitlityL();
	TVerdict TestGZipDeflateOldInflateNewCompatibitlityL();
	TVerdict TestGZipDeflateNewInflateOldCompatibitlityL();
	TVerdict TestZipArchiveDecompressCompatibitlityL();
	TVerdict TestOSCompatibilityL();
	TVerdict TestOSGZipCompatibilityL();
	};

#endif /*TE_BACKWARDSCOMPATIBILITYTESTS_H_*/
