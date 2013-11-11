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
// Example CTestStep derived implementation
//



/**
 @file te_perfcsvreader.cpp
 @internalTechnology
*/
#include <e32math.h> //for cvs comparison of elements
#include "te_perfcsvreader.h"
#include "te_ostv2integsuite_defs.h"


CCsvReader::CCsvReader()
	{
	iLineNumber = 0;
	iLineAvailable = ETrue;
	iLineStartPos = 0;
	iLineEndPos = 0;
	iElementNumber = 0;
	iElementPos = 0;	
	}

CCsvReader::~CCsvReader()
	{
	iBuffer.Close();
	iLine.Close();
	}

TInt CCsvReader::Create(TInt aSize)
	{
	TInt error = iLine.Create(aSize);
	if(!error)
		error = iBuffer.Create(aSize);
	return error;
	}

/** Read a line of csv data from the member data buffer and puts
 * it into iLine.
 * 
 * @aBuf Input buffer to return LF delimited text line from iBuffer
 * 
 * @return KErrNone or any other system wide error code
 */
TInt CCsvReader::NextLine()
	{
	//read a new set of data into a fresh iLine
	iLine.Zero();
	
	// Move to the next segment of iBuffer
	iLineStartPos = iLineEndPos+1;
	
	
	// temporary mid object
	TPtrC8 lfBuf = iBuffer.Mid(iLineStartPos, iBuffer.Size() - iLineStartPos);
	TInt nextLfPos = lfBuf.Find(KCsvNewLine);
	if (nextLfPos == KErrNotFound)
		{
		// Not found another element! May have finished parsing the file
		iLineAvailable = EFalse;
		return KErrNotFound;
		}	
	
	// Correct the position of the next LF to be referenced to iBuffer
	iLineEndPos = iLineStartPos + nextLfPos;

	// Now set iLine to be the section between iLineStartPos and iLineEndPos
	TInt size = iLineEndPos - iLineStartPos;
	iLine = iBuffer.Mid(iLineStartPos, size);
	iLineNumber++;
	iElementNumber = EApiName;
	iElementPos=0;
	return KErrNone;
	}

/**
 * Returns the value of the next element from within iLine
 * 
 * @aValue Input value to be set by this function
 * 
 * @return element value (positive) or a system wide error code (negative)
 */
TInt CCsvReader::NextElement(TElement& aValue)
	{
	TInt error = KErrNotSupported;
	// sanity check no LF
	TPtrC8 lfBuf = iLine.Mid(iElementPos, iLine.Size() - iElementPos);

	if(iElementPos < iLine.Size())
		{
		// Find next comma delimiter
		TPtrC8 commaBuf = iLine.Mid(iElementPos, iLine.Size() - iElementPos);
		
		TInt commaPos = commaBuf.Find(KCsvComma);
		if (commaPos == KErrNotFound)
			{
			// Not found another element!
			return KErrNotFound;
			}
		//as mid is relative to iElementPos, we need to add the baseline
		commaPos += iElementPos;
		// Construct a TPtrC8 representing the ascii for this element value in the buffer
		TPtrC8 element = iLine.Mid(iElementPos, commaPos - iElementPos);
			
		// Parse the value if it is not the Api name (a string)
		TInt elementValue = 0;
		if(iElementNumber!=EApiName)
			{
			TLex8 lex(element.Ptr());
			error = lex.Val(elementValue);
			if(error)
				{
				// return now if there is an error
				return error;
				}	
			}
	
		// move to the next element
		iElementPos = commaPos+1;
		
		// return the found value
		aValue.SetValue(elementValue);
		iElementNumber = iElementNumber+1;
		aValue.SetType(iElementNumber);
		error = KErrNone;		
		}
		
	return error;
	}

TInt CCsvReader::CompareFile(CCsvReader& aCompareCsvFile, TBool& aVerdict, RArray<TFailedResults>& aFailedResults)
	{
	TInt error = KErrNotSupported;
	CompareLine(aCompareCsvFile, aVerdict, aFailedResults);
	while(iLineAvailable==1)
		error = CompareLine(aCompareCsvFile, aVerdict, aFailedResults);
//maybe remove this***********************************TEST
	if(error == KErrNotFound)
				error = KErrNone; //we just found the end of file, nothing wrong with that...
	return error;
	}

TInt CCsvReader::CompareLine(CCsvReader& aCompareFile, TBool& aVerdict, RArray<TFailedResults>& aFailedResults)
	{
	TInt error = KErrNotSupported;
	//Get line
	error = this->NextLine();
	if(error)
		return error;
	error = aCompareFile.NextLine();
	if(error)
		return error;

	//Get element
	//Skip first line of the file as this is just the header
	if(iLineNumber>1)
		{
		TElement thisElement;
		TElement compareElement;
		TBool verdict = EFalse;
		TInt variation = 0;
		
		//Skip first elements of the files as this is just the api string name
		//get the first element from the first file
		error = this->NextElement(thisElement);
		//get the first element from the second file
		if(!error)
			error = aCompareFile.NextElement(compareElement);	
		
		
		//loop until we find error
		while(error == KErrNone)
			{
			//get the next element from the first file
			error = this->NextElement(thisElement);
			//get the next element from the second file
			if(!error)
				error = aCompareFile.NextElement(compareElement);
			//compare the two elements
			if(!error)
				error = CompareElement(thisElement, compareElement, verdict, variation);
			//the comparison failed, and we can print out what failed, but we keep looking at the other elements
			if(!verdict) 
				{
				aVerdict = verdict;
				TFailedResults results;
				results.iError = error;
				results.iLineNumber = iLineNumber;
				results.iElementType = thisElement.Type();
				results.iVariation = variation;
				aFailedResults.Append(results);
				}
			//something went wrong (and it wasn't an end of line)
			if((error != KErrNone) && (error != KErrNotFound))
				{
				return error;
				}
			}
		if(error == KErrNotFound)
			error = KErrNone; //we just found the end of line, nothing wrong with that...	
		}
	
	return error;
	}

TInt CCsvReader::CompareColumn(CCsvReader& /*aCompareFile*/, TBool& /*aVerdict*/, TInt& /*aVariation*/)
	{
	return KErrNotSupported;
	}


TInt CCsvReader::CompareElement(TElement aFirstElement, TElement aCompareElement, TBool& aVerdict, TInt& aVariation)
	{
	TInt error = KErrNotSupported;
	if(aFirstElement.Type() == aCompareElement.Type())
		{
		aVariation = aFirstElement.Value() - aCompareElement.Value();
		TInt modVariation = aVariation;
		if(aVariation < 0)
			{
			modVariation = -aVariation;
			}
		TInt compareValue = aFirstElement.AcceptedVariation() * ((TInt)aCompareElement.Value() / (TInt) 100);
		//ensure that the comparevalue returned is positive to be consistent with the modvariation
		//otherwise can get "minus" bugs
		if(compareValue < 0)
			{
			compareValue = -compareValue;
			}
		if(modVariation > compareValue)
			{
			aVerdict = EFalse;
			}
		else
			{
			aVerdict = ETrue;
			}	
		error = KErrNone;
		}
	else //the elements have gone out of synch
		error = KErrGeneral;

	return error;
	}


// eof

