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
* This wraps the ICU layout Engine in a CBase class
* and maps the Symbian structures to ICU structures
*
*/


#ifndef __CICULAYOUTENGINE_H__
#define __CICULAYOUTENGINE_H__

// System includes
#include <e32base.h>

#include <openfont.h>
#include <shapeinfo.h>
#include <graphics/shapeimpl.h>
#include "ScriptAndLanguageTags.h"

class SymbianFontInstance;

// if ICU headers not used define
#ifndef U_NAMESPACE_USE
class LayoutEngine;
#endif

/**
Encapsulates the ICU Layout Engine in a CShaper class
and maps the Symbian objects to ICU objects
@internalTechnology 
*/
class CIcuLayoutEngine : public CShaper
	{
public:

	static CShaper* NewL(CBitmapFont* aBitmapFont, TInt aScript, TInt aLanguage, RHeap* aHeap);
	~CIcuLayoutEngine();
	
	virtual TInt ShapeText(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput);
	virtual void* ExtendedInterface (TUid aInterfaceId);
		
private:
	CIcuLayoutEngine(TUint32 aScript, TUint32 aLanguage);
	TInt  ConstructL(CBitmapFont*, TInt aScript, TInt aLanguage, RHeap* iHeap);

	// leaving version of shaping method
	void  IcuShapeTextL(TShapeHeader*& aOutput, const TInput& aInput, RHeap* aHeapForOutput);
	
	// the client heap pointer passed in during construction
	RHeap* iClientHeap;

	// Open source Icu layout engine
	LayoutEngine* iEngine;	 
	
	// Symbian font data
	SymbianFontInstance* iFontInstance;
	
	// The script which this shaper will shape
	TUint32 iScript;
	
	//The language which this shaper will shape
	TUint32 iLanguage;
	};
#endif
