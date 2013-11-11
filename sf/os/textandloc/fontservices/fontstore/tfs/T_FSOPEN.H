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
* Tests FNTSTORE with respect to the Open Font system.
* Creates a dummy rasterizer and tests font file loading and unloading and
* font creation. This just exercises the way the Open Font system connects to the
* 'classic' bitmap-font-only font system.
*
*/


#ifndef __T_FSOPEN_H__
#define __T_FSOPEN_H__

#include <fntstore.h>
#include <openfont.h>
#include <e32base.h>
#include "TGraphicsHarness.h"
#include <graphics/openfontrasterizer.h>

class CTFsOpen : public CTGraphicsBase
	{
public:
	CTFsOpen(CTestStep* aStep);
	~CTFsOpen();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void TestFont(const TFontSpec& aSpec);
	void AddAndRemoveFilesL();
	void CreateAndReleaseFontsL();
	void CreateShaperL();
#ifdef _DEBUG
	void TestGetNearestFontHelperL();
#endif //_DEBUG
private:
	RHeap* iHeap;
	CFontStore* iFontStore;
	RFs iFilesys;
	};

class CTFsOpenStep : public CTGraphicsStep
	{
public:
	CTFsOpenStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	};

_LIT(KTFsOpenStep,"TFsOpen");


// make dummy font rasterizer available to t_fntmem

class CDummyRasterizer: public COpenFontRasterizer
	{
public:
	static CDummyRasterizer* NewL();
	// virtual function from COpenFontRasterizer
	COpenFontFile* NewFontFileL(TInt aUid,const TDesC& aFileName,RFs& aFileSession);
	};
	
// dummy shaper factory to create dummy shaper	

class CDummyShaperFactory: public CShaperFactory
	{
public:
	static CDummyShaperFactory* NewL();
	virtual ~CDummyShaperFactory();
	virtual CShaper* NewShaperL(CBitmapFont* aBitmapfont, TInt aScript, TInt aLanguage, RHeap* aHeap);

private:
	CDummyShaperFactory();

	};
	
// dummy shaper

class CDummyShaper : public CShaper
	{
public:

	static CShaper* NewL(CBitmapFont* aBitmapfont, TInt aScript, TInt aLanguage, RHeap* aHeap);
	~CDummyShaper();
	
	virtual TInt ShapeText(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput);
		
private:
	CDummyShaper();
	TInt ConstructL(CBitmapFont* aBitmapfont, TInt aScript, TInt aLanguage, RHeap* iHeap);

	// leaving version of shaping method
	TInt DummyShapeTextL(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput);
	
	// the client heap pointer passed in during construction
	RHeap* iClientHeap;

	// the private heap used by the shaper
	RHeap* iHeap;

	// memory allocated for the private heap	
	TAny* iHeapMemory;

	// buffers used to hold results from the Icu Layout engine 
 	TInt32* iGlyphs;
    TInt32* iIndices;
    TInt32* iPositions;
    
    };

#endif

