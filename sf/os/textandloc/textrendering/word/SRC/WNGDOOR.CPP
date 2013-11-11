/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <txtrich.h>
#include <prnsetup.h>
#include <apamdr.h>
#include <apparc.h>
#include "txtmrtsr.h"

#include "WNGDOOR.H"
#include "WNGMODEL.H"

_LIT(KWordApplication, "WORD.APP");


//
// Provides restoration support for objects embedded within embedded word documents
//
class TWordModelStoreResolver : public MRichTextStoreResolver
	{
public:
	TWordModelStoreResolver(const CStreamStore& aStore);
	//
	// from MRichTextStoreResolver
	const CStreamStore& StreamStoreL(TInt aPos)const;
private:
	const CStreamStore& iStore;
	};


TWordModelStoreResolver::TWordModelStoreResolver(const CStreamStore& aStore)
	:iStore(aStore)
	{}


const CStreamStore& TWordModelStoreResolver::StreamStoreL(TInt /*aPos*/)const
	{
	return iStore;
	}

//
//

CWordModelHeaderV2::CWordModelHeaderV2(CWordModel* aModel)
// C'tor
//
	: iModel(aModel)
	{}


void CWordModelHeaderV2::SetFactory(MApaModelHeaderFactory* aFactory)
// takes ownership
	{
	delete iHeaderFactory;
	iHeaderFactory = aFactory;
	}


EXPORT_C CWordModelHeaderV2::~CWordModelHeaderV2()
/** Destructor.

This deletes the wrapped word engine. */
	{
	delete iModel;
	delete iResolver;
	delete iHeaderFactory;
	}


EXPORT_C void CWordModelHeaderV2::StoreL(CStreamStore& aStore,CStreamDictionary& aDict)const
/** Stores the wrapped word engine.

@param aStore Store to write to
@param aDict Stream dictionary to write to */
	{iModel->StoreL(aStore,aDict,NULL);}


EXPORT_C void CWordModelHeaderV2::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict)
/** Restores the wrapped word engine.

@param aStore Store to read from.
@param aDict Stream dictionary to read from. */
	{
	delete iResolver;
	iResolver=NULL;
	iModel->RestoreL(aStore,aDict,NULL);
	iResolver = new(ELeave) TWordModelStoreResolver(aStore);
	}


EXPORT_C TApaAppIdentifier CWordModelHeaderV2::AppId()const
/** Gets an application identifier.

@return Word processor application identifier */
	{return (TApaAppIdentifier(KUidWordApp, KWordApplication()));}


EXPORT_C void CWordModelHeaderV2::DetachFromStoreL(CPicture::TDetach aDegree)
/** Calls DetachFromStoreL() on the engine's components (text, header, and footer).

DetachFromStoreL() sets how pictures are stored.

@param aDegree Picture storage option */
	{
	// Set a picture factory and store resolver if necessary, to support embedded objects
	TBool set=EFalse;
	TApaModelDoorFactory factory(iHeaderFactory);
	if ( !(iModel->Text()->PictureFactory() && iModel->Text()->StoreResolver()) )
		{
		iModel->Text()->SetPictureFactory(&factory,iResolver);
		set=ETrue;
		}
	iModel->Text()->DetachFromStoreL(aDegree);
	CRichText* text=iModel->PrintSetup()->Header()->Text();
	if (text)
		text->DetachFromStoreL(aDegree);
	text=iModel->PrintSetup()->Footer()->Text();
	if (text)
		text->DetachFromStoreL(aDegree);
	// finally, null our reference to the departing store
	if (set)
		iModel->Text()->SetPictureFactory(NULL,NULL);
	delete iResolver;
	iResolver = NULL;
	}


EXPORT_C CWordModel* CWordModelHeaderV2::Model()
/** Gets the wrapped word engine.

@return The wrapped word engine */
	{return iModel;}


EXPORT_C TWordModelHeaderFactoryV2::TWordModelHeaderFactoryV2(const TFileName& aPrintDriverPath)
: iPrintDriverPath(CONST_CAST(TFileName&,aPrintDriverPath))
/** Constructor, specifying printer driver path.

@param aPrintDriverPath Printer driver path */
	{}


EXPORT_C CApaModelHeader* TWordModelHeaderFactoryV2::NewHeaderL(const CStreamStore& aStore,
													const CStreamDictionary& aDict,
													const TApaAppIdentifier& aAppId)const
/** Creates a new word processor engine application wrapper.

@param aStore Store from which to restore word processor engine
@param aDict Stream dictionary from which to restore word processor engine
@param aAppId Word processor application identifier
@return New word processor engine application wrapper */
	{
	if (aAppId.iAppUid!=KUidWordApp)
		User::Leave(KErrNotSupported);
	CWordModel* model=CWordModel::NewL(NULL,NULL,iPrintDriverPath);
	CleanupStack::PushL(model);
	CWordModelHeaderV2* header = new(ELeave) CWordModelHeaderV2(model);
	CleanupStack::Pop(); // model - the header has taken ownership
	CleanupStack::PushL(header);
	// set the factory
	TWordModelHeaderFactoryV2* factory=new(ELeave) TWordModelHeaderFactoryV2(iPrintDriverPath);
	header->SetFactory(factory); // takes ownership of factory
	// restore the header
	header->RestoreL(aStore,aDict);
	CleanupStack::Pop();  // header
	return (header);
	}






CWordModelHeaderV3::CWordModelHeaderV3(CWordModel* aModel) : iModel(aModel)
	{
	}


// Set header factory. Takes ownership of aFactory
void CWordModelHeaderV3::SetFactory(MApaModelHeaderFactory* aFactory)
	{
	delete iHeaderFactory;
	iHeaderFactory = aFactory;
	}

EXPORT_C CWordModelHeaderV3::~CWordModelHeaderV3()
/** Destructor.

This deletes the wrapped word engine. */
	{
	delete iModel;
	delete iResolver;
	delete iHeaderFactory;
	}

EXPORT_C void CWordModelHeaderV3::StoreL(CStreamStore& aStore,CStreamDictionary& aDict)const
/** Stores the wrapped word engine.

@param aStore Store to write to
@param aDict Stream dictionary to write to */
	{
	iModel->StoreL(aStore,aDict,NULL);
	}

EXPORT_C void CWordModelHeaderV3::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aDict)
/** Restores the wrapped word engine.

@param aStore Store to read from.
@param aDict Stream dictionary to read from. */
	{
	delete iResolver;
	iResolver=NULL;
	iModel->RestoreMinimalL(aStore,aDict,NULL);
	iResolver = new(ELeave) TWordModelStoreResolver(aStore);
	}

EXPORT_C TApaAppIdentifier CWordModelHeaderV3::AppId()const
/** Gets an application identifier.

@return Word processor application identifier */
	{
	return (TApaAppIdentifier(KUidWordApp,KWordApplication()));
	}


/* Propogate the detach from store to all components. 
Header and footer information is not detached.
Set a picture factory and store resolver if necessary, to support embedded objects */
EXPORT_C void CWordModelHeaderV3::DetachFromStoreL(CPicture::TDetach aDegree)
/** Calls DetachFromStoreL() on the engine's components (text, header, and footer).

DetachFromStoreL() sets how pictures are stored.

@param aDegree Picture storage option */
	{
	TBool set=EFalse;
	TApaModelDoorFactory factory(iHeaderFactory);
	if ( !(iModel->Text()->PictureFactory() && iModel->Text()->StoreResolver()) )
		{
		iModel->Text()->SetPictureFactory(&factory,iResolver);
		set=ETrue;
		}
	iModel->Text()->DetachFromStoreL(aDegree);
	// finally, null our reference to the departing store
	if (set)
		iModel->Text()->SetPictureFactory(NULL,NULL);
	delete iResolver;
	iResolver = NULL;
	}

EXPORT_C CWordModel* CWordModelHeaderV3::Model()
/** Gets the wrapped word engine.

@return The wrapped word engine */
	{return iModel;}





/* Construtor
This is exported rather than being inline to allow people to instantiate 
this class without generating a v-table */
EXPORT_C TWordModelHeaderFactoryV3::TWordModelHeaderFactoryV3()
/** Constructor. */
	{
	}


// This is called by ETEXT when detaching the picture from the CRichText object
EXPORT_C CApaModelHeader* TWordModelHeaderFactoryV3::NewHeaderL(const CStreamStore& aStore,
																const CStreamDictionary& aDict,
																const TApaAppIdentifier& aAppId)const
/** Creates a new word processor engine application wrapper.

@param aStore Store from which to restore word processor engine
@param aDict Stream dictionary from which to restore word processor engine
@param aAppId Word processor application identifier
@return New word processor engine application wrapper */
	{
	if (aAppId.iAppUid!=KUidWordApp)
		User::Leave(KErrNotSupported);
	CWordModel* model=CWordModel::NewL( NULL, NULL, KDefaultPrinterDriverPath );
	CleanupStack::PushL(model);
	CWordModelHeaderV3* header = new(ELeave) CWordModelHeaderV3(model);
	CleanupStack::Pop(model); // header takes ownership of model
	CleanupStack::PushL(header);
	// set the factory
	TWordModelHeaderFactoryV3* factory=new(ELeave) TWordModelHeaderFactoryV3();
	header->SetFactory(factory); // header takes ownership of factory
	// restore the header
	header->RestoreL(aStore,aDict);
	CleanupStack::Pop(header);  
	return (header);
	}
