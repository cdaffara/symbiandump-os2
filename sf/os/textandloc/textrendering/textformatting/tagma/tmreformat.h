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


#ifndef __TMREFORMAT_H__
#define __TMREFORMAT_H__

#include <e32base.h>
#include "TmLayoutImp.h"
#include "TMINTERP.H"
#include "TMSTD.H"

class CTmTextLayout;
class TTmReformatParam;
class TBidirectionalContext;
class TTmFormatParamBase;
class TTmReformatParam;

NONSHARABLE_CLASS(CTmReformat) : public CBase
	{
public:
	static CTmReformat* NewLC(CTmTextLayout& aTextLayout, const TTmFormatParamBase& aFormatParam, 
		const TTmReformatParam& aReformatParam);
	~CTmReformat();
	void ReformatL();
	CTmCode& NewCode();
	const CTmFormatContext::TInfo& FormatInfo() const;
	const TTmByteCodeFinder::TInfo& ReformatRange() const;
	TBool BidirectionalStateChanged();
	const TBidirectionalContext& BidirectionalStateAtEnd() const;
private:
	CTmReformat(CTmTextLayout& aTextLayout,  const TTmFormatParamBase& aFormatParam, 
		const TTmReformatParam& aReformatParam);
	void ConstructL();
//helper functions for ReformatL
	void GetReformatRangeL(TBool aToParagraphStart, TBool aToParagraphEnd);
	void GenerateNewCodeL(TBool aToParagraphStart, TBool aToParagraphEnd);
//query functions for ReformatL
	TBool ReformatRangeExtendsToParagraphStart() const;
	TBool ReformatRangeExtendsToParagraphEnd() const;
	TBool BidirectionalityChangedAtStart();
	TBool BidirectionalityChangedAtEnd();
	TBool LineEndingsChangedAtEnd() const;
//input	
	CTmTextLayout& iTextLayout;
	const TTmFormatParamBase& iFormatParam;
	const TTmReformatParam& iReformatParam;
//temporary state
	TBidirectionalContext* iOldStartBDContext;
	TBidirectionalContext* iOldEndBDContext;
	TBidirectionalContext* iNewEndBDContext;
	TBidirectionalEndOfLineContext iEndOfPreviousLine;
//output
	CTmCode* iNewCode;	
	CTmFormatContext::TInfo iFormatInfo;
	TTmByteCodeFinder::TInfo iReformatRange;
	TBool iBidirectionalStateChanged;
	};
	
#endif
