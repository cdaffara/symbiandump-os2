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

#include "T_IsolatedFontStore.h"
#include <graphics/openfontrasterizer.h>
#include <graphics/openfontconstants.h>

CTIsolatedFontStore::CTIsolatedFontStore():iIsHeapOwner(ETrue)
	{
	}

CTIsolatedFontStore::CTIsolatedFontStore(RHeap* aHeap):iIsHeapOwner(EFalse),iHeap(aHeap)
    {
    }

CTIsolatedFontStore::~CTIsolatedFontStore()
	{
	delete iFs;
	if (iIsHeapOwner && iHeap)
	    {
	    iHeap->Close();
	    }
	REComSession::FinalClose();
	}

CTIsolatedFontStore* CTIsolatedFontStore::NewLC()
	{
	CTIsolatedFontStore* self = new (ELeave)CTIsolatedFontStore();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTIsolatedFontStore* CTIsolatedFontStore::NewL()
	{
	CTIsolatedFontStore* self=CTIsolatedFontStore::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

CTIsolatedFontStore* CTIsolatedFontStore::NewLC(RHeap * aHeap)
    {
    CTIsolatedFontStore* self = new (ELeave)CTIsolatedFontStore(aHeap);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CTIsolatedFontStore* CTIsolatedFontStore::NewL(RHeap * aHeap)
    {
    CTIsolatedFontStore* self=CTIsolatedFontStore::NewLC(aHeap);
    CleanupStack::Pop(); // self;
    return self;
    }

void CTIsolatedFontStore::ConstructL()
	{
    if(iIsHeapOwner)
        {
        iHeap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
        }	
	iFs = CFontStore::NewL(iHeap);
	}

/**
Load all available font rasterizers and install to the instance of CFontStore
this class contains.

@pre This function hasn't previously been called on this object
@post All rasterizers located and successfully loaded will be available for use
 */
void CTIsolatedFontStore::LoadRasterizersL()
	{
	RImplInfoPtrArray implementationArray;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	// get implementation list
	ListImplementationsWithRetry(uid, implementationArray, EFalse);

	const TInt availCount = implementationArray.Count();
	for (TInt count=0; count < availCount; ++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		// Create & install a rasterizer
		// ignore Leaves, as any necessary cleanup will have already been done through the cleanup stack
		TRAP_IGNORE(SafeInstallOfRasterizerL(info->ImplementationUid()));
		}
	
	// free memory
	implementationArray.ResetAndDestroy();
	}

/**
Helper function from CFbTop to List all available rasterizers.

@param aInterfaceUid The UID of the ECOM plugin
@param aImplementationArray An array to store the found plugins
@param aRomOnly If ETrue only ROM plugins are returned, otherwise all drives are searched
*/
void CTIsolatedFontStore::ListImplementationsWithRetry(TUid& aInterfaceUid, RImplInfoPtrArray &aImplementationArray, TBool aRomOnly)
	{
	// Making sure that no race situation arises between FBserv and Ecom
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (TInt ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
		TInt ecomError = KErrNone;
		if (aRomOnly)
			{
			TEComResolverParams resParams;
			TRAP(ecomError, REComSession::ListImplementationsL(aInterfaceUid, resParams, KRomOnlyResolverUid, aImplementationArray));
			}
		else
			{ // default resolver
			TRAP(ecomError, REComSession::ListImplementationsL(aInterfaceUid, aImplementationArray));
			}

		if (!ecomError)
			{
			return;
			}
		else
			{
			User::After(0);
			}
		}
	}

/**
Installs the rasterizer with the specified interface UID.

@param aInterfaceImplUid The UID of the rasterizer to be installed.

@see CFbTop::SafeInstallOfRasterizerL
 */
void CTIsolatedFontStore::SafeInstallOfRasterizerL(TUid aInterfaceImplUid)
	{
	COpenFontRasterizer* rasterizer = COpenFontRasterizer::NewL(aInterfaceImplUid);
	CleanupStack::PushL(rasterizer);
	// Install it in the font store.
	iFs->InstallRasterizerL(rasterizer);
	CleanupStack::Pop(rasterizer);
	}
