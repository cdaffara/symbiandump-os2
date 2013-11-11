/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* An example parser
*
*/


#ifndef __EXPARSER_H__
#define __EXPARSER_H__

#include <mparser.h>
#include <medobsrv.h>


class CTestParser : public CBase, public MParser
	{
public:
	static CTestParser* NewL();

	~CTestParser();

	// Overloads for MParser interface
	TBool ParseThisText(const CRichText& aTextObj, TBool aAllowBack,
						TInt aStartScan, TInt aScanLength,
						TInt& aStartTag, TInt& aTagLength);

	const TDesC& CreateDoItText(const CRichText& aTextObj,
								TInt aStartText, TInt aLength);

	void ActivateThisTextL(const CRichText& aTextObj,
						   TInt aStartText, TInt aLength);

	TBool ReformatOnRecognise() const {return ETrue;}

	TBool ReformatOnRollover() const {return ETrue;}

	void GetRecogniseFormat(TCharFormat& aFormat);

	void GetRolloverFormat(TCharFormat& aFormat);

	void Release();

private:
	CTestParser();

private:
	HBufC* iDoItText;
	};

//-----------------------------------------------

class CTestParser2 : public CBase, public MParser
	{
public:
	static CTestParser2* NewL();

	~CTestParser2();

	// Overloads for MParser interface
	TBool ParseThisText(const CRichText& aTextObj, TBool aAllowBack,
						TInt aStartScan, TInt aScanLength,
						TInt& aStartTag, TInt& aTagLength);

	const TDesC& CreateDoItText(const CRichText& aTextObj,
								TInt aStartText, TInt aLength);

	void ActivateThisTextL(const CRichText& aTextObj,
						   TInt aStartText, TInt aLength);

	TBool ReformatOnRecognise() const {return ETrue;}

	TBool ReformatOnRollover() const {return ETrue;}

	void GetRecogniseFormat(TCharFormat& aFormat);

	void GetRolloverFormat(TCharFormat& aFormat);

	void Release();

private:
	CTestParser2();

private:
	HBufC* iDoItText;
	};


class CEditObserver : public CBase, public MEditObserver
	{
public:
	CEditObserver();
	~CEditObserver();
	void EditObserver(TInt aStart, TInt aExtent);

public:
	TInt iStart;
	TInt iExtent;
	};

#endif
