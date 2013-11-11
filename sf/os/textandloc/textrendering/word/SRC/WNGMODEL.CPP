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

#include <s32strm.h>

#include <txtfrmat.h>
#include <txtfmlyr.h>
#include <txtstyle.h>
#include <txtrich.h>
#include <txtuids.h>

#include <prnsetup.h>
#include <prnuids.h>

#include "WNGMODEL.H"
#include "WNGPANIC.H"

const TInt KDefaultTextGranularity=256;

EXPORT_C CWordModel* CWordModel::NewL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aFieldNumPagesInfo,const TDesC& aDriverPath)
/** Allocates and creates a new word processor engine, and performs printer initalisation.

@param aFileNameInfo Optional callback interface to get the current document's 
filename. This provides the file name for insertion into a field in the header 
or footer.
@param aFieldNumPagesInfo Optional callback interface to return the number 
of pages in the current document. This provides the total number of pages 
for insertion into a field in the header or footer.
@param aDriverPath Directory that contains printer driver files
@return New word processor engine */
	{
	CWordModel* self=new(ELeave) CWordModel();
	CleanupStack::PushL(self);
	self->ConstructMinimalL(aFileNameInfo,aFieldNumPagesInfo,aDriverPath);
	CleanupStack::Pop();  // self
	return self;
	}


EXPORT_C CWordModel* CWordModel::NewL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aNumPagesInfo)
/** Allocates and creates a new word processor engine.

@param aFileNameInfo Optional callback interface to get the current document's 
filename. This provides the file name for insertion into a field in the header 
or footer.
@param aNumPagesInfo Optional callback interface to return the number 
of pages in the current document. This provides the total number of pages 
for insertion into a field in the header or footer.
@return New word processor engine */
	{
	CWordModel* self=new(ELeave) CWordModel();
	CleanupStack::PushL(self);
	self->ConstructL(aFileNameInfo,aNumPagesInfo);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CWordModel::CWordModel()
/** Default constructor. */
	{}


void CWordModel::ConstructGlobalLayersL()
	{
	//
	// Create a default usable Normal style (the default layers)
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	paraFormat.iSpaceAfterInTwips=100;
	paraFormatMask.SetAttrib(EAttSpaceAfter);
	paraFormat.iHorizontalAlignment=CParaFormat::ELeftAlign;
	paraFormatMask.SetAttrib(EAttAlignment);
	iParaFormatLayer=CParaFormatLayer::NewL(&paraFormat,paraFormatMask);
	//
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	TInt typefaceAttributes=TTypeface::EProportional|TTypeface::ESerif;
	charFormat.iFontSpec.iTypeface.SetAttributes(typefaceAttributes);
	charFormat.iFontSpec.iHeight=200;
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight);
	iCharFormatLayer=CCharFormatLayer::NewL(charFormat,charFormatMask);
	//
	iText=CRichText::NewL(iParaFormatLayer,iCharFormatLayer,
						CEditableText::ESegmentedStorage,
						KDefaultTextGranularity);	
	iPageTable=new(ELeave) CArrayFixFlat<TInt>(5);
	}


EXPORT_C void CWordModel::ConstructMinimalL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aFieldNumPagesInfo,const TDesC& aDriverPath)
// Minimal initialisation, to allow all components to function well.
// Used by winc developers.
// A printer device must be created by the caller.
//
/** Second-phase constructor, with printer initialisation.

@param aFileNameInfo Optional callback interface to get the current document's 
filename. This provides the file name for insertion into a field in the header 
or footer.
@param aFieldNumPagesInfo Optional callback interface to return the number 
of pages in the current document. This provides the total number of pages 
for insertion into a field in the header or footer.
@param aDriverPath Directory that contains printer driver files */
	{
	//
	// Create the uninitialised print setup object
	InitPrintSetupL(aFileNameInfo,aFieldNumPagesInfo,aDriverPath);
	//
	// Create global layers
	//
	ConstructGlobalLayersL();
	//
	// Create the default, empty style list.
	iStyleList=CStyleList::NewL();
	iStyleShortCutList=new(ELeave) CStyleShortCutList(3);
	//
	//
	iText->SetStyleListExternallyOwned(*iStyleList);
	}


EXPORT_C void CWordModel::ConstructL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aNumPagesInfo)
/** Second-phase constructor.

@param aFileNameInfo Callback interface to get the current document's filename.
@param aNumPagesInfo Callback interface to return the number of pages in the current 
document. */
	{
	//
	// Create the uninitialised print setup object
	InitPrintSetupL( aFileNameInfo, aNumPagesInfo, KDefaultPrinterDriverPath );
	//
	// Create a default usable Normal style (the default layers)
	ConstructGlobalLayersL();
/*
	iStyleList=NULL;
	iStyleShortCutList=NULL;
	iStyleList=CStyleList::NewL();
	iStyleShortCutList=new(ELeave) CStyleShortCutList(3);
	for (TInt ii=0;ii<3;ii++)
		{
		CParagraphStyle* style=CParagraphStyle::NewL(*iParaFormatLayer,*iCharFormatLayer);
		RParagraphStyleInfo info(style);
		iStyleList->AppendL(&info);
		}
	iStyleList->At(0).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(0).iStyle->iName=_L("Heading 1");
	iStyleList->At(1).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(1).iStyle->iName=_L("Heading 2");
	iStyleList->At(2).iStyle->SetType(KSystemParagraphStyleUid);
	iStyleList->At(2).iStyle->iName=_L("Heading 3");

	iStyleShortCutList->AppendL('1');
	iStyleShortCutList->AppendL('2');
	iStyleShortCutList->AppendL('3');
	iNormalStyleShortCut='N';
*/
//	iText->SetStyleListExternallyOwned(*iStyleList);
	}


void CWordModel::InitPrintSetupL(const MFieldFileNameInfo* aFileNameInfo,const MFieldNumPagesInfo* aNumPagesInfo,const TDesC& aDriverPath)
// Creates a default printer device and does some initialisation
// A real printer device is not constructed here.
//
    { 
	iPrintSetup=CPrintSetup::NewL();
	iPrintSetup->AddPrinterDriverDirL(aDriverPath);
	iPrintSetup->Header()->SetFileNameInfo(CONST_CAST(MFieldFileNameInfo&,*aFileNameInfo));
	iPrintSetup->Footer()->SetFileNameInfo(CONST_CAST(MFieldFileNameInfo&,*aFileNameInfo));
	iPrintSetup->Header()->SetNumPagesInfo(CONST_CAST(MFieldNumPagesInfo&,*aNumPagesInfo));
	iPrintSetup->Footer()->SetNumPagesInfo(CONST_CAST(MFieldNumPagesInfo&,*aNumPagesInfo));
	iPrintSetup->iNumOfFirstPage=1;
	}


EXPORT_C CWordModel::~CWordModel()
/** Destructor. */
	{
	delete iPrintSetup;
	delete iStyleList;
	delete iStyleShortCutList;
	delete iPageTable;
	delete iText;
	delete iParaFormatLayer;
	delete iCharFormatLayer;
	}

EXPORT_C void CWordModel::StoreL(CStreamStore& aStore,CStreamDictionary& aStreamDic,const TAny* aSecurity)const
/** Stores the engine.

This function writes the engine data to a stream store, and records the streams 
used in a stream dictionary.

@param aStore Store to write to
@param aStreamDic Stream dictionary to write to
@param aSecurity Optional security object. If this is specified, the stored 
text stream is encrypted */
	{
	TStreamId id=StoreStylesL(aStore);
	aStreamDic.AssignL(KUidParagraphStyleStream,id);

	id=iPrintSetup->StoreL(aStore);
	aStreamDic.AssignL(KUidPrintSetupStream,id);

	id=StoreTextDataL(aStore,aSecurity);
	aStreamDic.AssignL(KUidEditableTextStream,id);

	id=StoreFieldDataL(aStore);
	aStreamDic.AssignL(KUidTextFieldStream,id);

	id=StoreMarkupDataL(aStore);
	aStreamDic.AssignL(KUidTextMarkupStream,id);
	}

EXPORT_C void CWordModel::RestoreL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,
								   const TAny* aSecurity,const MFieldFileNameInfo* aFileNameInfo,
								   const MFieldNumPagesInfo* aNumPagesInfo,
									MPictureFactory* aPictureFactory)

/** Restores the engine.

@param aStore Store to read from
@param aStreamDic Stream dictionary to read from
@param aSecurity Optional security object. This is required to read text from 
an encrypted stream.
@param aFileNameInfo An optional object implementing the MFieldFileNameInfo 
interface. This provides the file name for insertion into a field in the header 
or footer. 
@param aNumPagesInfo An optional object implementing the MFieldNumPagesInfo 
interface. This provides the total number of pages for insertion into a field 
in the header or footer. 
@param aPictureFactory An optional object implementing the picture factory 
interface. This is required if the header or footer contains pictures which 
should be restored. */
	{
	TStreamId id=aStreamDic.At(KUidParagraphStyleStream);
	__ASSERT_ALWAYS(id!=KNullStreamId,User::Leave(KErrCorrupt));
	RestoreStylesL(aStore,id);

	id=aStreamDic.At(KUidPrintSetupStream);	
	__ASSERT_ALWAYS(id!=KNullStreamId,User::Leave(KErrCorrupt));
	iPrintSetup->RestoreL(aStore,id,CONST_CAST(MFieldFileNameInfo*,aFileNameInfo),CONST_CAST(MFieldNumPagesInfo*,aNumPagesInfo),aPictureFactory);

	id=aStreamDic.At(KUidEditableTextStream);
	__ASSERT_ALWAYS(id!=KNullStreamId,User::Leave(KErrCorrupt));
	RestoreTextDataL(aStore,id,aSecurity);
	
	id=aStreamDic.At(KUidTextFieldStream);
	if (id!=KNullStreamId)
		RestoreFieldDataL(aStore,id);

	iText->SetStyleListExternallyOwned(*iStyleList);  // forces restore with style references
	id=aStreamDic.At(KUidTextMarkupStream);	
	if (id!=KNullStreamId)
		RestoreMarkupDataL(aStore,id);
	}

EXPORT_C void CWordModel::RestoreMinimalL(const CStreamStore& aStore,const CStreamDictionary& aStreamDic,
										  const TAny* aSecurity,const MFieldFileNameInfo* /*aFileNameInfo*/,
										  const MFieldNumPagesInfo* /*aNumPagesInfo*/,
										  MPictureFactory* /*aPictureFactory*/)
/** Restores the engine, without restoring the print setup stream.

This allows restoring without loading a printer driver. Header and footer information 
will not be available.

@param aStore Store to read from
@param aStreamDic Stream dictionary to read from
@param aSecurity Optional security object. This is required to read text from 
an encrypted stream.
@param aFileNameInfo Unused
@param aNumPagesInfo Unused
@param aPictureFactory Unused */
	{
	TStreamId id=aStreamDic.At(KUidParagraphStyleStream);
	__ASSERT_ALWAYS(id!=KNullStreamId,User::Leave(KErrCorrupt));
	RestoreStylesL(aStore,id);

	id=aStreamDic.At(KUidEditableTextStream);
	__ASSERT_ALWAYS(id!=KNullStreamId,User::Leave(KErrCorrupt));
	RestoreTextDataL(aStore,id,aSecurity);
	
	id=aStreamDic.At(KUidTextFieldStream);
	if (id!=KNullStreamId)
		RestoreFieldDataL(aStore,id);

	iText->SetStyleListExternallyOwned(*iStyleList);  // forces restore with style references
	id=aStreamDic.At(KUidTextMarkupStream);	
	if (id!=KNullStreamId)
		RestoreMarkupDataL(aStore,id);
	}



TStreamId CWordModel::StoreFieldDataL(CStreamStore& aStore)const
//
	{
	const TInt fieldCount=iText->FieldCount();
	if (fieldCount<=0)
		return KNullStreamId;
	CStoreMap* map=CStoreMap::NewLC(aStore);
	iText->StoreFieldComponentsL(aStore,*map);
	//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	iText->ExternalizeFieldDataL(stream);
	stream.CommitL();
	//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // stream, map
	return id;
	}

void CWordModel::RestoreFieldDataL(const CStreamStore& aStore,TStreamId aId)
// Assumes a valid stream id
// 	
	{
	__ASSERT_DEBUG(aId!=KNullStreamId,User::Leave(KErrCorrupt));

	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	//
	iText->InternalizeFieldDataL(stream);
	CleanupStack::PopAndDestroy();  // stream
	//
	iText->RestoreFieldComponentsL(aStore);
	}

TStreamId CWordModel::StoreTextDataL(CStreamStore& aStore,const TAny* aSecurity)const
	{
	CStreamStore* store = &aStore;
	//
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(*store);
	iText->ExternalizePlainTextL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy();  // stream
	//
	if (aSecurity)
		CleanupStack::PopAndDestroy();  // secure store
	return id;
	}

void CWordModel::RestoreTextDataL(const CStreamStore& aStore,TStreamId aId,const TAny* aSecurity)
	{
	__ASSERT_DEBUG(aId!=KNullStreamId,User::Leave(KErrCorrupt));
	const CStreamStore* store=&aStore;
	//
	RStoreReadStream stream;
	stream.OpenLC(*store,aId);
	//
	iText->InternalizePlainTextL(stream);
	CleanupStack::PopAndDestroy();  // stream
	//
	if (aSecurity)
		CleanupStack::PopAndDestroy();  // secure store
	}


TStreamId CWordModel::StoreMarkupDataL(CStreamStore& aStore)const
// The markup data must be stored in a distinct stream, and will never be encrypted.
// Only the text content will be encrypted.
// ie, If the container doc is passworded, that password will NOT apply to all embedded objects.
//
	{
	const TBool hasMarkupData=iText->HasMarkupData();
	if (!hasMarkupData)
		return KNullStreamId;
	CStoreMap* map=CStoreMap::NewLC(aStore);
	iText->StoreMarkupComponentsL(aStore,*map);
	//
	RStoreWriteStream stream(*map);
	TStreamId id=stream.CreateLC(aStore);
	iText->ExternalizeMarkupDataL(stream);
	stream.CommitL();
	//
	map->Reset();
	CleanupStack::PopAndDestroy(2);  // stream, map
	return id;
	}
	

void CWordModel::RestoreMarkupDataL(const CStreamStore& aStore,TStreamId aId)
//
//
	{
	__ASSERT_DEBUG(aId!=KNullStreamId,User::Leave(KErrCorrupt));

	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	//
	iText->InternalizeMarkupDataL(stream);
	CleanupStack::PopAndDestroy();  // strean
	}


TStreamId CWordModel::StoreStylesL(CStreamStore& aStore)const
// Store the style list, and the table of short cut keys.
// (The short-cut table is mapped directly on to the style list)
// Also stores the normal (global layers).
//
	{
	RStoreWriteStream stream;
	TStreamId id=stream.CreateLC(aStore);
	//
	stream<< *iText->GlobalParaFormatLayer();  // The "normal" style
	stream<< *iText->GlobalCharFormatLayer();  //
	stream.WriteUint32L(iNormalStyleShortCut);  // The "normal" short cut
	//
	TInt shortCutCount=(iStyleShortCutList) ? iStyleShortCutList->Count() : 0;
	stream.WriteUint8L((TUint8)shortCutCount);
	for (TInt nn=0;nn<shortCutCount;nn++)
		stream.WriteUint32L((*iStyleShortCutList)[nn]);
	stream<< *iStyleList;
	//
	stream.CommitL();
	CleanupStack::PopAndDestroy();  // stream
	return id;
	}
	

void CWordModel::RestoreStylesL(const CStreamStore& aStore,TStreamId aId)
// Restore the style list and style short cut key list from the specified stream.
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aId);
	//	
	delete iParaFormatLayer;
	delete iCharFormatLayer;
	iParaFormatLayer=NULL;
	iCharFormatLayer=NULL;
	iParaFormatLayer=CParaFormatLayer::NewL(stream);
	iCharFormatLayer=CCharFormatLayer::NewL(stream);
	iNormalStyleShortCut=stream.ReadUint32L();
	//
	TUint8 shortCutCount=stream.ReadUint8L();
	if (iStyleList)
		iStyleList->Reset();
	//
	if (iStyleShortCutList)
		iStyleShortCutList->Reset();  // loading new document from within word
	else
		iStyleShortCutList=new(ELeave) CStyleShortCutList(Max(1,(TInt)shortCutCount));  // loading document from shell
	for (TInt mm=0;mm<shortCutCount;mm++)
		{
		TChar shortcut=stream.ReadUint32L();
		iStyleShortCutList->AppendL(shortcut);
		}
	if (iStyleList)
		iStyleList->InternalizeL(stream,iParaFormatLayer,iCharFormatLayer);
	else
		iStyleList=CStyleList::NewL(stream,iParaFormatLayer,iCharFormatLayer);
	//
	iText->SetGlobalParaFormat(iParaFormatLayer);
	iText->SetGlobalCharFormat(iCharFormatLayer);
	//
	CleanupStack::PopAndDestroy();  // stream

	//__ASSERT_DEBUG(iStyleList->Count()==iStyleShortCutList->Count(),Panic(EStyleIntegrityError));
	//remove assertion so word engine supports wp apps with no short cuts but 1 (global) style
	}
