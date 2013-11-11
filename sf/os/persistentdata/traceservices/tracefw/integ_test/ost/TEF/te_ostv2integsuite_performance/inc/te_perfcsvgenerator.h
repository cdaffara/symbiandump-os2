// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// UTrace Performance Tests CSV output generator
//



/**
 @file te_perfcsvgenerator.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTCSVGENERATOR_H
#define TE_UPTCSVGENERATOR_H 

#include <e32base.h>
#include <f32file.h>
#include "te_perfapicall.h"
#include "te_perfplugin.h"



/**
This is a user-side utility class manipulate csv files and append or overwrite these file with
 performance data metrics .
 
 pre-conditions? assumptions before calling?
 
@internalTechnology
@prototype
 */
class CUptCsvGenerator: public CBase
	{
public:
	CUptCsvGenerator(); 
	~CUptCsvGenerator();

	TInt OpenL(const TDesC& aFileName, const TBool& aAppend); 
	TInt Close();
	void WriteL(const RArray<TInt64>& aPerformanceData);
	void WriteL(const RArray<TPtrC8>& aPerformanceConfig);
	void WriteApiNameL(const TInt aApiEnum);
	TInt WriteNewLineL();		
	TInt WriteHeaderL(const TInt& aTestType);	

	TInt TestL(); 
private:	
	//Private helper method for sanity tests
	TInt FillReferenceBuffer(RBuf8& aBufferName, const TInt& aAppendCount, const RArray<TInt64>& aDataName, const TInt& aData1Count, const RArray<TInt64>& aData2Name, const TInt& aData2Count);

	//data for connecting to file sessions for generating csv												
	RFs iCsvFileSession;
	RFile iCsvFile;
	};

#endif


