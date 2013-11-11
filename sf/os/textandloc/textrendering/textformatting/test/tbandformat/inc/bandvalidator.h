/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __BANDVALIDATOR_H
#define __BANDVALIDATOR_H

#include <badesca.h>
#include <txtglobl.h>
#include <frmtview.h>
#include <txtfmlyr.h>
#include <txtrich.h>
#include <frmtlay.h>

/**
@file
@internalComponent 
*/	
class CRichText;
class RTest;
class CFileOutput;
class CRichControl;
class CTextView;

/**
Class which compares the contents of two CTextView objects.
It compares the contents of the test view with band formatting enabled
against the reference view with no band formatting enabled
*/
class CBandValidator : public CBase
	{
public:
	/**
	Creates a CBandValidator object
	@param aTestView The text CTextView object
	@param aReferenceView The reference CTextView object
	@param aOutput A CFileOutput object used to log output to a file
	@return CBandValidator An instance of CBandValidator
	*/
	IMPORT_C static CBandValidator* NewLC(const CTextView& aTestView, const CTextView& aReferenceView, CFileOutput& aOutput);
	IMPORT_C ~CBandValidator();
	/** 
	Validates the contents of the views by comparing the visible bands of the test and reference views
	@param aDesc A description of the action being validated
	@return TBool ETrue if the contents are valid
	*/
	IMPORT_C TBool ValidateL(const TDes& aDesc);

public:
	inline TInt FailLineNumberInPara(){return iFailLineNumberInPara;}
	inline TInt FailLineNumber(){return iFailLineNumber;}
	
private:
	CBandValidator(const CTextView& aTestView, const CTextView& aReferenceView, CFileOutput& aOutput);
	void ConstructL();
	// checks if two line infos are equal
	TBool LineInfosAreEqualL( TTmLineInfo& aLineUnderTest, TTmLineInfo& aLineReference);
	// get the information for each visible line
	TBool GetVisibleLineInfosL(const CTextView& aView, RArray<TTmLineInfo>& aVisibleLines);
	// get the line information for the reference view using the visible line info of the the test view
	TBool GetReferenceLineInfosL(const RArray<TTmLineInfo>& aVisibleLines, RArray<TTmLineInfo>& aReferenceLineInfos);
	// get the contents of the lines
	void GetLineContentsL(const CTextView& aView, const RArray<TTmLineInfo>& aLineInfos, CDesCArray& aLineContents);

	CFileOutput& iFileOutput;
	const CTextView& iTestView;
	const CTextView& iReferenceView;	

	//The failed line number of the paragraph, -1 implies no error
	TInt iFailLineNumberInPara;
	//The failed line number, -1 implies no error
	TInt iFailLineNumber;
	};
	
#endif
