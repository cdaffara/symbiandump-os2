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
* TESTACTIONS.H
*
*/


#ifndef __TESTACTIONS_H
#define __TESTACTIONS_H

#include <e32std.h>

/**
@file
@internalComponent 
*/
class CRichText;
class CBandMaintainer;
class CBandValidator;
class CFbsScreenDevice;

/**
Base class for band test actions
This class is initialised with a CBandMaintainer, CBandValidator and a CRichText object.
These are used to perform the required action and then validate the test and reference view
*/
class TBandTestAction
	{
public:
	// result of a band test action
	enum TTestResult
		{
		ENotExecuted,
		EPassed,
		EFailed
		};
	/**
	Execute a test action.
	@param aDocPos The position in the document where the test action is to be performed
	@param aEditLength The length of the edit to be executed
	@return TTestResult The result of the band test action	
	*/
	IMPORT_C virtual TTestResult ExecuteL(TInt aDocPos, TInt aEditLength) = 0;
protected:
	TBandTestAction(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator);
	CBandMaintainer& iTextViews;
	CRichText& iRichText;
	CBandValidator& iValidator;
	};

/**
Band test action used to carry out a delete
*/	
class TDeleteText : public TBandTestAction
	{
public:
	/**
	Constructor
	@param aTextViews The CBandMaintainer object which maintains the test and reference CTextView objects
	@param aRichText The document which is represented by a CRichText object 	
	@param aValidator The CBandValidator which is used to compare the test and reference views
	*/
	IMPORT_C TDeleteText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator);
	/**
	Execute a delete.
	@param aDocPos The position in the document where the test action is to be performed
	@param aEditLength The length of the edit to be executed
	@return TTestResult The result of the band test action	
	*/
	IMPORT_C virtual TTestResult ExecuteL(TInt aDocPos, TInt aEditLength);
	};

/**
Band test action used to carry out an insert
*/	
class TInsertText : public TBandTestAction
	{
public:
	/**
	Constructor
	@param aTextViews The CBandMaintainer object which maintains the test and reference CTextView objects
	@param aRichText The document which is represented by a CRichText object 	
	@param aValidator The CBandValidator which is used to compare the test and reference views
	@param aTextToInsert The text to be inserted
	*/
	IMPORT_C TInsertText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, const TDesC& aTextToInsert);
	/**
	Execute an insert,
	@param aDocPos The position in the document where the test action is to be performed
	@param aEditLength Not Used
	@return TTestResult The result of the band test action	
	*/
	IMPORT_C virtual TTestResult ExecuteL(TInt aDocPos, TInt aEditLength);
private:
	const TDesC& iTextToInsert;
	};
	
enum TReformatAction
	{
	EMakeBold,
	EIncreaseFontSize,
	EDecreaseFontSize,
	EChangeFont
	};

/**
Band test action used to carry out a reformat
*/
class TReformatText : public TBandTestAction
	{
public:
	/**
	Constructor
	@param aTextViews The CBandMaintainer object which maintains the test and reference CTextView objects
	@param aRichText The document which is represented by a CRichText object 	
	@param aValidator The CBandValidator which is used to compare the test and reference views
	*/
	IMPORT_C TReformatText(CBandMaintainer& aTextViews, CRichText& aRichText, CBandValidator& aValidator, TReformatAction aReformatAction, CFbsScreenDevice& aDevice);
	/**
	Exectute a reformat
	@param aDocPos The position in the document where the test action is to be performed
	@param aEditLength The length of the edit to be executed
	@return TTestResult The result of the band test action	
	*/
	IMPORT_C virtual TTestResult ExecuteL(TInt aDocPos, TInt aEditLength);
private:
	TInt iReformatAction;
	CFbsScreenDevice& iDevice;
	};
	
#endif
