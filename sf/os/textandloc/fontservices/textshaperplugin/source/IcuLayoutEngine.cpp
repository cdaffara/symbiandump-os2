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
* Implementation of CIcuLayoutEngine
* This is a CShaper using the Icu Layout Engine.
*
*/

// System includes
#include <e32base.h>
#include <gdi.h>
#include <e32math.h>
#include <openfont.h>
#include <fntstore.h>

// ICU includes
#include "math.h"
#include "LETypes.h"
#include "LEScripts.h"
#include "LELanguages.h"
#include "LayoutEngine.h"
#include "SymbianFontInstance.h"

#include "IcuLayoutEngine.h"

// Icu namespace
U_NAMESPACE_USE

// Convert from float to int rounding to the nearest integer
inline TInt FloatToInt(float a)
	{
	return a < 0? static_cast<TInt>(a - 0.5) : static_cast<TInt>(a + 0.5);
	}

/** Array of Script Codes that need to be translated into LEScriptCodes for the 
construction of the LayoutEngine. */
const LETag scriptCodes[] = {
    zyyyScriptTag, /* 'zyyy' (COMMON) */
    qaaiScriptTag, /* 'qaai' (INHERITED) */
    arabScriptTag, /* 'arab' (ARABIC) */
    armnScriptTag, /* 'armn' (ARMENIAN) */
    bengScriptTag, /* 'beng' (BENGALI) */
    bopoScriptTag, /* 'bopo' (BOPOMOFO) */
    cherScriptTag, /* 'cher' (CHEROKEE) */
    qaacScriptTag, /* 'qaac' (COPTIC) */
    cyrlScriptTag, /* 'cyrl' (CYRILLIC) */
    dsrtScriptTag, /* 'dsrt' (DESERET) */
    devaScriptTag, /* 'deva' (DEVANAGARI) */
    ethiScriptTag, /* 'ethi' (ETHIOPIC) */
    georScriptTag, /* 'geor' (GEORGIAN) */
    gothScriptTag, /* 'goth' (GOTHIC) */
    grekScriptTag, /* 'grek' (GREEK) */
    gujrScriptTag, /* 'gujr' (GUJARATI) */
    guruScriptTag, /* 'guru' (GURMUKHI) */
    haniScriptTag, /* 'hani' (HAN) */
    hangScriptTag, /* 'hang' (HANGUL) */
    hebrScriptTag, /* 'hebr' (HEBREW) */
    hiraScriptTag, /* 'hira' (HIRAGANA) */
    kndaScriptTag, /* 'knda' (KANNADA) */
    kanaScriptTag, /* 'kana' (KATAKANA) */
    khmrScriptTag, /* 'khmr' (KHMER) */
    laooScriptTag, /* 'laoo' (LAO) */
    latnScriptTag, /* 'latn' (LATIN) */
    mlymScriptTag, /* 'mlym' (MALAYALAM) */
    mongScriptTag, /* 'mong' (MONGOLIAN) */
    mymrScriptTag, /* 'mymr' (MYANMAR) */
    ogamScriptTag, /* 'ogam' (OGHAM) */
    italScriptTag, /* 'ital' (OLD_ITALIC) */
    oryaScriptTag, /* 'orya' (ORIYA) */
    runrScriptTag, /* 'runr' (RUNIC) */
    sinhScriptTag, /* 'sinh' (SINHALA) */
    syrcScriptTag, /* 'syrc' (SYRIAC) */
    tamlScriptTag, /* 'taml' (TAMIL) */
    teluScriptTag, /* 'telu' (TELUGU) */
    thaaScriptTag, /* 'thaa' (THAANA) */
    thaiScriptTag, /* 'thai' (THAI) */
    tibtScriptTag, /* 'tibt' (TIBETAN) */
    cansScriptTag, /* 'cans' (CANADIAN_ABORIGINAL) */
    yiiiScriptTag, /* 'yiii' (YI) */
    tglgScriptTag, /* 'tglg' (TAGALOG) */
    hanoScriptTag, /* 'hano' (HANUNOO) */
    buhdScriptTag, /* 'buhd' (BUHID) */
    tagbScriptTag, /* 'tagb' (TAGBANWA) */
    braiScriptTag, /* 'brai' (BRAILLE) */
    cprtScriptTag, /* 'cprt' (CYPRIOT) */
    limbScriptTag, /* 'limb' (LIMBU) */
    linbScriptTag, /* 'linb' (LINEAR_B) */
    osmaScriptTag, /* 'osma' (OSMANYA) */
    shawScriptTag, /* 'shaw' (SHAVIAN) */
    taleScriptTag, /* 'tale' (TAI_LE) */
    ugarScriptTag, /* 'ugar' (UGARITIC) */
    hrktScriptTag  /* 'hrkt' (KATAKANA_OR_HIRAGANA) */
};

/** Array of Language Codes that need to be translated into LELanguageCodes for the 
construction of the LayoutEngine. */
const LETag languageCodes[] = {
    nullLanguageTag, /* '' (null) */
    araLanguageTag, /* 'ARA' (Arabic) */
    asmLanguageTag, /* 'ASM' (Assamese) */
    benLanguageTag, /* 'BEN' (Bengali) */
    farLanguageTag, /* 'FAR' (Farsi) */
    gujLanguageTag, /* 'GUJ' (Gujarati) */
    hinLanguageTag, /* 'HIN' (Hindi) */
    iwrLanguageTag, /* 'IWR' (Hebrew) */
    jiiLanguageTag, /* 'JII' (Yiddish) */
    janLanguageTag, /* 'JAN' (Japanese) */
    kanLanguageTag, /* 'KAN' (Kannada) */
    kokLanguageTag, /* 'KOK' (Konkani) */
    korLanguageTag, /* 'KOR' (Korean) */
    kshLanguageTag, /* 'KSH' (Kashmiri) */
    malLanguageTag, /* 'MAL' (Malayalam (Traditional)) */
    marLanguageTag, /* 'MAR' (Marathi) */
    mlrLanguageTag, /* 'MLR' (Malayalam (Reformed)) */
    mniLanguageTag, /* 'MNI' (Manipuri) */
    oriLanguageTag, /* 'ORI' (Oriya) */
    sanLanguageTag, /* 'SAN' (Sanscrit) */
    sndLanguageTag, /* 'SND' (Sindhi) */
    snhLanguageTag, /* 'SNH' (Sinhalese) */
    syrLanguageTag, /* 'SYR' (Syriac) */
    tamLanguageTag, /* 'TAM' (Tamil) */
    telLanguageTag, /* 'TEL' (Telugu) */
    thaLanguageTag, /* 'THA' (Thai) */
    urdLanguageTag, /* 'URD' (Urdu) */
    zhpLanguageTag, /* 'ZHP' (Chinese (Phonetic)) */
    zhsLanguageTag, /* 'ZHS' (Chinese (Simplified)) */
    zhtLanguageTag  /* 'ZHT' (Chinese (Traditional)) */
};

/**
Translate the script code passed in to a LEScriptCode
*/ 
TInt ScriptCode(TUint32 aScript)
	{
	TInt scriptCode = -1;
    for (TInt i= 0; i < scriptCodeCount; i++)
    	{
    	if (scriptCodes[i] == aScript)
    		{
    		scriptCode = i;
    		break;
    		}
    	}
    return scriptCode;
	}
	
/**
Translate the language code passed in to a LELanguageCode
*/
TInt LanguageCode(TUint32 aLanguage)
	{
	TInt languageCode = -1;
    for (TInt i= 0; i < languageCodeCount; i++)
    	{
    	if (languageCodes[i] == aLanguage)
    		{
    		languageCode = i;
    		break;
    		}
    	}
    return languageCode;
	}

/**
Create a instance of CIcuLayoutEngine
@param aBitmapFont The required font.
@param aHeap The heap to be used for storage by the engine.
@return A pointer to the new CIcuLayoutEngine instance.
*/
CShaper * CIcuLayoutEngine::NewL(CBitmapFont* aBitmapFont, TInt aScript, TInt aLanguage, RHeap* aHeap)
	{
	CIcuLayoutEngine* newShaper = new(ELeave)CIcuLayoutEngine(aScript, aLanguage);
	CleanupStack::PushL(newShaper);
	newShaper->ConstructL(aBitmapFont, aScript, aLanguage, aHeap);
	CleanupStack::Pop(); // newShaper
	return newShaper;	
	} 


/**
Construct an instance of CIcuLayoutEngine 	
@param aBitMapFont The required font
@param aHeap The heap to be used for storage by the engine
@return incase of exceptions with an Error Code
@see CShaper
 */
TInt  CIcuLayoutEngine::ConstructL(CBitmapFont* aBitMapFont, TInt aScript, TInt aLanguage, RHeap* aHeap )
	{
	// allocate a block of memory from aHeap for the layout engine 
	// which is accessed via the Umemory class
	iClientHeap = aHeap;

	// this needs to be on the heap
	LEErrorCode fontStatus = LE_NO_ERROR;
	iFontInstance = new SymbianFontInstance(aBitMapFont, fontStatus, aScript == mlymScriptTag);
	if(NULL == iFontInstance)
		{
		User::Leave(KErrGeneral);
		}
	if (fontStatus == LE_MEMORY_ALLOCATION_ERROR)
   		{
   		User::Leave(KErrNoMemory);
   		} 	
   	else if (fontStatus != LE_NO_ERROR)
   		{
   		//note this leave may actually be caused by OOM situations, 
   		//due to the way errors codes are handled in the open source code
   		User::Leave(KErrGeneral);
   	 	}
   	 	

 	// create and initialise a ICU layout Engine
	// Note the engine is created using the Umemory heap 
    LEErrorCode success = LE_NO_ERROR;
    
    // Translate the script code into an LEScriptCode
    TInt scriptCode = ScriptCode(aScript);
    
    // Translate the language code into an LELanguageCode
    TInt languageCode = LanguageCode(aLanguage);
    
    // Finally instantiate the LayoutEngine    
	iEngine = LayoutEngine::layoutEngineFactory(iFontInstance, scriptCode, languageCode, success); 

	// For debugging - check the total memory used by construction
#ifdef SHAPER_MEMORY_DEBUG
	TInt totalAllocSize = 0;
	TInt used = iHeap->AllocSize(totalAllocSize);
	RDebug::Print(_L("shaper construction %d cells %d"), totalAllocSize, used);
#endif

	if (success == LE_MEMORY_ALLOCATION_ERROR)
		{
		User::Leave(KErrNoMemory);	
		}
	else if (success != LE_NO_ERROR)
		{
 		//note this leave may actually be caused by OOM situations, 
 		//due to the way errors codes are handled in the open source code
 		User::Leave(KErrGeneral);
		}
	return KErrNone;
	}

CIcuLayoutEngine::CIcuLayoutEngine(TUint32 aScript, TUint32 aLanguage):
	iScript(aScript),
	iLanguage(aLanguage)
	{
	}

/** 
 Frees all resources owned by ...
 */
 CIcuLayoutEngine::~CIcuLayoutEngine()
	{
	// delete the engine instance
	if ( iEngine )
		{
		delete( iEngine );
		}
		
	// delete the font instance from client heap
	delete( iFontInstance );
	}

/**
Returns the script and the language this shaper has been instansiated with.
*/	
void* CIcuLayoutEngine::ExtendedInterface (TUid aInterfaceId)
	{
	if (aInterfaceId == KUidShaperGetScript)		
		return (TUint32*)iScript;
	else
		if (aInterfaceId == KUidShaperGetLang)
			return (TUint32*)iLanguage;
		else
			return CShaper::ExtendedInterface(aInterfaceId);			
	}
	
/** This is implementation of CShaper::ShapeText for the Icu layout Engine
 The data is taken from TInput and pass to the shaper.
 A memory buffer is allocated on aHeapForOutput starting with TShapeHeader is allocated.
 The results of the shaping are copied into this buffer and passed back via aOutput. 
 @param aOutput On success a new structure containing the results allocated on aHeapForOutput.  
 @param aInput The input text and other parameters.
 @param aHeapForOutput On success, aOutput should be allocated from this and nothing else. 
 		On failure, nothing should be allocated from it.
 @return Error value from one of the system-wide error codes on	failure, KErrNone on success.
 @see CShaper::ShapeText
 */		
TInt CIcuLayoutEngine::ShapeText(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput)
	{
	// For debugging - the heap before shaping
	TInt totalAllocSize = 0;
	TInt used;
#ifdef SHAPER_MEMORY_DEBUG
	used = User::Heap().AllocSize(totalAllocSize);
	RDebug::Print(_L("before shaping %d cells %d bytes used"), used, totalAllocSize);
#endif

	iFontInstance->SetSessionHandle(aInput.iSessionHandle);
	TRAPD( error, IcuShapeTextL(aOutput, aInput, aHeapForOutput));
	if (error == KErrNoMemory)
		{
		used = User::Heap().AllocSize(totalAllocSize);
		RDebug::Print(_L("shaper out of memory %d cells %d"), totalAllocSize, used);
		}

#ifdef SHAPER_MEMORY_DEBUG
	used = User::Heap().AllocSize(totalAllocSize);
	RDebug::Print(_L("Shaped %d characters %d glyphs "), aOutput->iCharacterCount, aOutput->iGlyphCount );
	RDebug::Print(_L("after shaping %d cells %d bytes used"), used, totalAllocSize);
#endif

	// hide the ICU error codes as KErrGeneral
	if ((error == KErrNoMemory) || (error == KErrNone))
		return error;
	else
		return KErrGeneral;		
	}

/** This calls the ICU Layout engine to shape the text.  It allocates memory for the results
and then reads the results.  This memory is freed by the caller.
This function can leave if OOM.
 @param aOutput On success a new structure containing the results allocated on aHeapForOutput.  
 @param aInput The input text and other parameters.
 @param aHeapForOutput On success, aOutput should be allocated from this and nothing else. 
 		On failure, nothing should be allocated from it.
 @return Error value from one of the system-wide error codes on	failure, KErrNone on success.
 @see CIcuLayoutEngine::ShapeText
 */
void  CIcuLayoutEngine::IcuShapeTextL(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput)
	{
	LEErrorCode success = LE_NO_ERROR;
	const LEUnicode * p = (LEUnicode *)aInput.iText->Ptr();
	TInt noChars = aInput.iEnd - aInput.iStart;

	// Call to layout  
	TInt noOfGlyphs = iEngine->layoutChars(
			p, 							// chars - the input character context
			aInput.iStart,				// the offset of the first character to process	
			noChars,					// count - the number of characters to process	// offset
			aInput.iText->Length(), 	// max - the number of characters in the input context	// size of text
			FALSE, 						// rightToLeft - TRUE if the characters are in a right to left directional run
			0, 							// start X
			0, 							// start Y
			success);					// result code

	if (success == LE_MEMORY_ALLOCATION_ERROR)
		User::Leave(KErrNoMemory);
	else if (success != LE_NO_ERROR) 
		{
		User::Leave(KErrGeneral);
		}
	

	// Get some memory to pass into the layout engine for the results
	TInt bufferSize = (sizeof(LEGlyphID) + sizeof(le_int32) + sizeof(float) * 2)
		* noOfGlyphs + sizeof(float) * 2;
	TUint8* buffer = reinterpret_cast<TUint8*>( User::AllocL(bufferSize) );
	CleanupStack::PushL(buffer);
	LEGlyphID* glyphBuffer = reinterpret_cast<LEGlyphID*>(buffer);
	le_int32* indexBuffer = reinterpret_cast<le_int32*>(glyphBuffer + noOfGlyphs);
	float* positionBuffer = reinterpret_cast<float*>(indexBuffer + noOfGlyphs);
	
	
	// now get results glyph codes, positions and indices
	// from the layout engine
	if (success == LE_NO_ERROR)
		iEngine->getGlyphs(glyphBuffer, success);

	if (success == LE_NO_ERROR)
		iEngine->getGlyphPositions(positionBuffer, success);

	if (success == LE_NO_ERROR)
		iEngine->getCharIndices(indexBuffer, aInput.iStart, success);
	if (success == LE_NO_ERROR)
		// Reset the memory used by the IcuLayoutEngine
		iEngine->reset();
	if (success == LE_MEMORY_ALLOCATION_ERROR)
		User::Leave(KErrNoMemory);
	else 
		if (success != LE_NO_ERROR)
 			{
			User::Leave(KErrGeneral);
			}

	// Some of the glyph codes are 0xFFFF and 0x0001. 0xFFFF is a value used by ICU layout
	// engine to indicate no glyph, and 0x0001 is used to indicate a ZWJ or a ZWNJ.
	// These should be stripped out now. A ZWJ or a ZWNJ glyph has no meaning here. Their 
	// effects on the precedig and proceding characters have already been taken into 
	// consideration during shaping, so we don't need them anymore.
	// Also, their presence in the final glyph list was causing GDI to break with GDI:1
	// i.e. more than 8 glyphs in a glyph cluster.
	LEGlyphID gidOfZWJ = iFontInstance->mapCharToGlyph(0x200D); // Added by Symbian: 1922 mlyl
	TInt actualNoOfGlyphs = 0;
	for (LEGlyphID* p = glyphBuffer + noOfGlyphs; p != glyphBuffer;)
		{
		--p;
		if (*p != 0xFFFF && *p != 0x0001 && *p != gidOfZWJ) // Added by Symbian: 1922 mlyl
			++actualNoOfGlyphs;
		}

	// get some memory to pass back the results,  
	// This needs to be big enough for a TShapeHeader
	// plus 10 bytes for every glyph returned (-1 for the 1 byte allocated in TShapeHeader for iBuffer)
	aOutput = reinterpret_cast<TShapeHeader*>( aHeapForOutput->AllocL(
		sizeof(TShapeHeader) + (actualNoOfGlyphs * 10) + 3) );

	// get the results into the shaper structure aOutput 
	aOutput->iGlyphCount = actualNoOfGlyphs;
	aOutput->iCharacterCount = noChars;
 	aOutput->iReserved0 = 0;
	aOutput->iReserved1 = 0;

 	// iBuffer contains 10 bytes for every glyph
 	// the glyph code (4 bytes), position X(2 bytes) Y(2 bytes) and indices(2 bytes)
 
 	// first is glyph count * 4 byte glyph codes
 	TUint32* glyphOut = reinterpret_cast<TUint32*>(aOutput->iBuffer);
 	TInt16* posOut = reinterpret_cast<TInt16*>(aOutput->iBuffer +
 		(4 * actualNoOfGlyphs));
 	TInt16* indicesOut = reinterpret_cast<TInt16*>(aOutput->iBuffer +
 		(8 * actualNoOfGlyphs) + 4);
 	for (TInt i=0; i < noOfGlyphs; i++)
 		{
 		if (*glyphBuffer != 0xFFFF && *glyphBuffer != 0x0001 && *glyphBuffer != gidOfZWJ) // Added by Symbian: 1922 mlyl
 			{
	  		*glyphOut++ = *glyphBuffer;
			*posOut++ = FloatToInt( positionBuffer[0] );
			*posOut++ = FloatToInt( positionBuffer[1] );
			*indicesOut++ = *indexBuffer;
 			}
 		++glyphBuffer;
 		positionBuffer += 2;
 		++indexBuffer;
		}
	// There is an extra pair of positions: this is the total advance
	posOut[0] = FloatToInt( positionBuffer[0] );
	posOut[1] = FloatToInt( positionBuffer[1] );
	
	CleanupStack::PopAndDestroy(buffer);	
	
	}
