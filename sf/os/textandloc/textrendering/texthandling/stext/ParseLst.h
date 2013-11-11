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
*
*/


#ifndef __PARSELST_H__
#define __PARSELST_H__

#include <e32std.h>
#include <mparser.h>
#include <txtrich.h>
//++ sort out definitive hash includes


NONSHARABLE_CLASS(CParserList) : public CBase
/**
@internalComponent
*/
	{
	enum TParserTagValues
		{
		EReformatOnRecogniseFlag = (TInt)0x80000000,
		EReformatOnRolloverFlag = 0x40000000,
		EDefaultParserFlag = 0x20000000,
		EParserTagMask = 0x3FFFFFFF,
		EMaxParserIndex = 0x1FFFFFFF
		};

	enum TParserListPanic
		{
		EParserNoneActive,
		EParserNullPtr,
		EParserInstanceNotActive,
		EParserNoSuchTag,
		EParserIgnoringAllowFlag,
		EParserIndexRollover
		};

private:
	class CParserItem : public CBase
		{
	public:
		CParserItem(MParser* aParser, const TUint aTagIndex);
		~CParserItem() {}

		inline MParser* Parser() const {return iParser;}
		inline TUint TagIndex() const {return iTagIndex;}

	private:
		MParser* iParser;
		TUint iTagIndex;
		};

public:
	CParserList();
	~CParserList();

	void ActivateParserL(MParser* aParser);
	void DeactivateParser(MParser* aParser);
	void ActivateDefaultParserL(MParser* aParser);
	void DeactivateParserDefaults();
	TBool ParseThisText(CRichText& aTextObj,TInt aStartScan,TInt aScanLength,TInt& aStartOfTags,TInt& aLength);
	MParser* ParserWithThisTag(const TUint aTagIndex) const;
	TUint TagForThisParser(const MParser *const aParser) const;

	static inline TBool ReformatOnRollover(const TUint aTagIndex) {return (aTagIndex & EReformatOnRolloverFlag);}

private:
	static inline TBool ReformatOnRecognise(const TUint aTagIndex) {return (aTagIndex & static_cast<TUint>( EReformatOnRecogniseFlag ) );}
	static inline TBool DefaultParser(const TUint aTagIndex) {return (aTagIndex & EDefaultParserFlag);}

	static inline TUint MaskedTag(const TUint32 aTagIndex) {return (aTagIndex & EParserTagMask);}

	void ActivateAParserL(MParser* aParser, const TBool aDefaultParser);
	TBool LoopOneParser(MParser* aParser, const TUint aTagIndex, CRichText& aTextObj,
						const TInt aStartScan, const TInt aScanLength,
						TInt& aStartOfTags, TInt& aLength);
	void Panic(TParserListPanic aPanic) const;

private:
	RPointerArray<CParserItem> iParserList;	// List of active parsers (specific and default)
	TUint iHighestIndex;
public:
	TInt iNumberInList;
	TInt iRefCount;
	};

NONSHARABLE_CLASS(CParserData) : public CBase
/**
@internalComponent
*/
	{
public:
	CParserData(TInt aEndParse);
	~CParserData();
	TBool HaveRange() const { return iStartParse != -1; }
	void MergeRange(TInt aStart,TInt aOldLength,TInt aNewLength);
	void KillRange() { iStartParse = -1; }
	TInt StartParse() const { return iStartParse; }
	TInt EndParse() const { return iEndParse; }

	CParserList* iActiveParserList;	// local copy because access to Dll::Tls is slow
	MEditObserver* iEditObserver;
	TInt iLastKnownCursor;

private:
	TInt iStartParse;				// start of text to be parsed; -1 means none
	TInt iEndParse;					// end of text to be parsed
	};

#endif
