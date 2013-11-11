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
//



/**
 @file te_perfcsvreader.h
 @internalTechnology
*/
#if (!defined TE_CSVREADER_H)
#define TE_CSVREADER_H

#include <e32base.h>
#include "te_perfelements.h"


class CCsvReader : public CBase
{
public:
	
	CCsvReader();
	~CCsvReader();
	TInt NextLine(void);
	TInt NextElement(TElement& aValue);
	TInt CompareFile(CCsvReader& aCompareCsvFile, TBool& aVerdict, RArray<TFailedResults>& aFailedResults);
	TInt CompareLine(CCsvReader& aCompareFile, TBool& aVerdict, RArray<TFailedResults>& aFailedResults);
	TInt CompareColumn(CCsvReader& aCompareFile, TBool& aVerdict, TInt& aVariation);
	
	TInt CompareElement(TElement aFirstElement, TElement aCompareElement, TBool& aVerdict, TInt& aVariation);		
	RBuf8& Buffer(){return iBuffer;}
	TInt Create(TInt aSize);
	TInt CurrentElementType(){return iElementNumber;}
	
private:
	RBuf8 iBuffer; 			// Represents the entire csv file.
	RBuf8 iLine;			// Current line data
	
	// Indices into iBuffer for current line
	TInt iLineNumber; 		// Line currently being parsed
	TBool iLineAvailable; 	// Bool on whether any other lines exist in the file
	TInt iLineStartPos;		// Position of start of current line within iBuffer
	TInt iLineEndPos;		// Position of end of current line within iBuffer

	// Indices into iLine for current element
	TInt iElementNumber;	// Element within the current line

	//CCsvReader::TColumnType iElementNumber;	// Element within the current line
	TInt iElementPos;		// Position within the current line, referenced within iLine
};


#endif //TE_OUTPUTSANITYTEST_H
