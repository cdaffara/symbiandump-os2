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
* TestLayout.h test jig for CTmTextLayout
*
*/


#include "TAGMA.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "TAGMA_INTERNAL.H"
#endif

class CRichText;
class CTestGraphicsDevice;

typedef void TransliterationFn(const TDesC&, TDes&);

class CETextSource : public CBase, public MTmSource
	{
public:
	CETextSource(const CRichText& aRichText, MGraphicsDeviceMap* aDeviceMap);
	MGraphicsDeviceMap& FormatDevice() const { return *iDeviceMap; }
	MGraphicsDeviceMap& InterpretDevice() const { return *iDeviceMap; }
	TInt DocumentLength() const;
	void GetText(TInt aPos, TPtrC& aText, TTmCharFormat& aFormat) const;
	void GetParagraphFormatL(TInt aPos, RTmParFormat& aFormat) const;
	TInt ParagraphStart(TInt aPos) const;
	CPicture* PictureL(TInt aPos) const;

private:
	const CRichText* iRichText;
	MGraphicsDeviceMap* iDeviceMap;
	};

class CTestTmTextLayout : public CBase
	{
public:
	static CTestTmTextLayout* NewLC(const CRichText& aText, TInt aWrapWidth);
	static CTestTmTextLayout* NewLC(const TDesC& aText, TInt aWrapWidth,
		TransliterationFn* aTransliterate);
	~CTestTmTextLayout();
	MTmSource& Source() { return *iSource; }
	const MTmSource& Source() const { return *iSource; }
	CTmTextLayout& Layout() { return *iLayout; }
	const CTmTextLayout& Layout() const { return *iLayout; }
	CTestGraphicsDevice& Device() const { return *iDevice; }
	void FormatL(const TTmReformatParam& aIn, TTmReformatResult& aOut);
	// Function to test the API: CTmTextLayout::FindAdjacentChunks()
	void TestAdjacentChunks();
private:
	void ConstructL(const CRichText& aText, TInt aWrapWidth);

private:
	CTmTextLayout* iLayout;
	TTmFormatParam iFormatParam;
	CETextSource* iSource;
	CTestGraphicsDevice* iDevice;
	CRichText* iRichText;
	CParaFormatLayer* iParagraphFormatLayer;
	CCharFormatLayer* iCharacterFormatLayer;
	};
