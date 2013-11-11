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
* Abstract base class from which all shapers drive.
*
*/


/**
 @file 
 @internalComponent
*/

// Symbian includes
#include <openfont.h>

// ICU includes 
#include "LETypes.h"
#include "LayoutEngine.h"
#include "SymbianFontInstance.h"
#include "IcuLayoutEngine.h"
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>


class CIcuShaperFactory: public CShaperFactory
	{
public:
	static CIcuShaperFactory* NewL();
	virtual ~CIcuShaperFactory();

   /**
	Factory function to create an instance of shaper object
    @param aBitmapFont The font that requires shaping
    @param aScript Isic standard script code
    @param aLanguage Isic standard language code
    @param aHeap The heap to be used by the shaper
    @return KErrNone if successful else a system wide error code
    */
	virtual CShaper* NewShaperL(CBitmapFont* aBitmapFont, TInt aScript, TInt aLanguage, RHeap* aHeap);

private:
	CIcuShaperFactory();

	};


CIcuShaperFactory* CIcuShaperFactory::NewL()
	{
	CIcuShaperFactory* r = new(ELeave) CIcuShaperFactory;
	return r;
	}

CIcuShaperFactory::CIcuShaperFactory()
	{
	}

CIcuShaperFactory::~CIcuShaperFactory()
	{
	
	}


CShaper* CIcuShaperFactory::NewShaperL(CBitmapFont* aBitmapFont, TInt aScript, TInt aLanguage, RHeap* aHeap)
	{
	return CIcuLayoutEngine::NewL(aBitmapFont, aScript, aLanguage, aHeap);
	} 

void* CShaperFactory::ExtendedInterface(TUid /*aInterfaceId*/)
	{
	return 0;
	}

// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10279727,	CIcuShaperFactory::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
