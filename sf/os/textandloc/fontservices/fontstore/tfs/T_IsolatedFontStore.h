/*
* Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TISOLATEDFONTSTORE_H
#define TISOLATEDFONTSTORE_H

#include <e32std.h>
#include <e32base.h>
#include <fntstore.h>

/**
Class to create an isolated version of the font store; upon construction
the font store pointer contains a FontStore in its initial state. Functions
are provided to complete the loading of rasterizers and fonts if required.
 */
class CTIsolatedFontStore : public CBase
	{
public:
	~CTIsolatedFontStore();

	static CTIsolatedFontStore* NewL();
	static CTIsolatedFontStore* NewLC();
	
	static CTIsolatedFontStore* NewL(RHeap*);
	static CTIsolatedFontStore* NewLC(RHeap*);

	void LoadRasterizersL();
	
private:

	CTIsolatedFontStore();
	CTIsolatedFontStore(RHeap* aHeap);
	void ConstructL();
	void ListImplementationsWithRetry(TUid& aInterfaceUid, RImplInfoPtrArray &aImplementationArray, TBool aRomOnly);
	void SafeInstallOfRasterizerL(TUid aInterfaceImplUid);
	
public:
	CFontStore* iFs;
private:
	RHeap* iHeap;
	
	// Indicate the ownership of iHeap.
	// ETrue means iHeap is allocated in ConstructL() and owned by CTIsolatedFontStore in current process; 
    // EFalse means iHeap is a pointer to a heap which is passed in as parameter. Do not take ownership.  
	TBool  iIsHeapOwner; 
	};

#endif // TISOLATEDFONTSTORE_H
