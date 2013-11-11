/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* THindExtensionApi.h
*
*/


/**
 @file 
 @internalComponent - Internal Symbian test code  
*/


#ifndef __THINDIEXTENSIONAPIT_H__
#define __THINDIEXTENSIONAPIT_H__

#include <test/ttmsgraphicsstep.h>
#include "TFreeTypeServer.h"

/**
 Base class for Rasterizer tests.
 Contains the utility functions for creating the test font store
 */
class CRasterizerTest: public CTTMSGraphicsStep
	{
public:
	CTFreeTypeServer* iServer;
	CBitmapFont* iFont;
	CFontStore* iFontStore;

	void LoadFontStore();
	void CloseFontStore();
	};


/**
 Test Rasterizer API: MOpenFontShapingExtension::RasterGlyphL()
 */
class CTestRasterizeGlyph: public CRasterizerTest
	{
public:
	CTestRasterizeGlyph();
protected:	
	TVerdict doTestStepL();
	};
_LIT(KTestRasterizeGlyphStep,"RasterizeGlyph");

/**
 Test Rasterizer API: MOpenFontShapingExtension::GlyphIndex()
 */
class CTestGlyphIndex: public CRasterizerTest
	{
public:
	CTestGlyphIndex();
protected:	
	TVerdict doTestStepL();
	};
_LIT(KTestGlyphIndexStep,"GlyphIndex");
	
/**
 Test Rasterizer API: MOpenFontShapingExtension::GlyphPointInHintedPixels()
 */
class CTestGlyphPointInHintedPixels: public CRasterizerTest
	{
public:
	CTestGlyphPointInHintedPixels();
protected:	
	TVerdict doTestStepL();
	};
_LIT(KTestGlyphPointInHintedPixels,"GlyphPointInHintedPixels");
	
/**
 Test Rasterizer API: MOpenFontShapingExtension::GlyphPointInFontUnits()
 */
class CTestGlyphPointInFontUnits: public CRasterizerTest
	{
public:
	CTestGlyphPointInFontUnits();
protected:	
	TVerdict doTestStepL();
	};
_LIT(KTestGlyphPointInFontUnits,"GlyphPointInFontUnits");

/**
 Test Rasterizer API: MOpenFontShapingExtension::GetExtensionFontMetrics()
 */
class CTestGetExtensionFontMetrics: public CRasterizerTest
	{
public:
	CTestGetExtensionFontMetrics();
protected:	
	TVerdict doTestStepL();
	};
_LIT(KTestGetExtensionFontMetrics,"GetExtensionFontMetrics");

/**
 Test the Rasterizer API: MOpenFontTrueTypeExtension
 */
class CTestTrueTypeTable: public CRasterizerTest
	{
public:
	CTestTrueTypeTable();
protected:
	enum TExpectedResult
		{
		// Does not exist in font
		EUnavailable,
		// Exists, has not been requested before
		EAvailableNew,
		// Exists, old one is being served up again
		EAvailableExisting
		};
	TAny* TestGetTable(TExpectedResult aExpected,
		MOpenFontTrueTypeExtension* aTableStore, TUint32 aTag, TInt* aLength);
	TVerdict doTestStepL();
	};
_LIT(KTestTrueTypeTableStep,"TypeTypeTable");


#endif
