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

#include <gdi.h>
#include <s32mem.h>
#include <conpics.h>

#include "TESTPANI.H"

// ***************************************************************************
// MDEMPICTUREHEADERFACTORY IMPLEMENTATION
// ***************************************************************************

EXPORT_C MDemPictureFactory::MDemPictureFactory()
// Fix to force GCC to export the vtable.
//
	{}


EXPORT_C void MDemPictureFactory::NewPictureL(TPictureHeader& aHeader,const CStreamStore& aDeferredPictureStore)const
// From the picture header, instantiates the correct concrete picture, and
// restores it frorm the specified stream.
//
	{
	if (aHeader.iPictureType==KUidXzePictureType)	
		aHeader.iPicture=CXzePicture::NewL(aDeferredPictureStore,aHeader.iPicture.AsId());
	else if(aHeader.iPictureType==KUidXzeDoorType)
		aHeader.iPicture=CXzeDoor::NewL(aDeferredPictureStore,aHeader.iPicture.AsId());
	else
		Panic(EPictureTypeNotSupportedByHeader);
	}




// ***************************************************************************
// TEST PICTURE IMPLEMENTATION
// ***************************************************************************
EXPORT_C CXzePicture* CXzePicture::NewL(TChar aLabel)
	{return new(ELeave) CXzePicture(aLabel);}


EXPORT_C CXzePicture* CXzePicture::NewL(const CStreamStore& aStore,TStreamId aId)
// Restoring NewL.
//
	{
	CXzePicture* self=new(ELeave) CXzePicture();
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aId);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CXzePicture::CXzePicture(TChar aLabel)
// Sets the startup attributes of this picture
//
	: iLabel(aLabel)
	{ResetToOriginal();}


EXPORT_C CXzePicture::CXzePicture()
// Sets the startup attributes of this picture
//
//	
	{ResetToOriginal();}


EXPORT_C CXzePicture::~CXzePicture()
	{}


EXPORT_C void CXzePicture::ExternalizeL(RWriteStream& aStream)const
// Save this picture.
// Typically called from the base class StoreL();
//
	{aStream.WriteUint32L(iLabel);}


EXPORT_C void CXzePicture::InternalizeL(RReadStream& aStream)
// Load this picture
//
	{iLabel=(TChar)aStream.ReadUint32L();}


EXPORT_C void CXzePicture::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
// Create a read-stream over aStore, and open it over the specified stream ID.
// Internalize picture from this stream.
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	stream>> *this;
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void CXzePicture::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap) const
// Draw this simple picture.
//
 	{
	aGc.Reset();
	aGc.SetClippingRect(aClipRect);
	TSize size;  // Size of graphics device in pixels
	GetSizeInPixels(aMap,size);
	TRect box;  // The rectangle that exactly fits the picture
	box.iTl=aTopLeft;
	box.iBr.iX=aTopLeft.iX+size.iWidth;
	box.iBr.iY=aTopLeft.iY+size.iHeight;
	TRgb white(255,255,255);
// First draw outer box and fill in rest of box.
	aGc.SetBrushColor(white);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(box);
// Now draw label
	CFont* font;
	TFontSpec fontSpec(_L("Arial"),213);
	if (aMap->GetNearestFontInTwips(font,fontSpec)<0)
		{
		return;
		}
	aGc.UseFont(font);
	TBuf<1> label;	label.Append(iLabel);
	TInt baselineOffset=(box.Height()+font->AscentInPixels())/2; 
	aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
	aGc.DrawText(label,box,baselineOffset,CGraphicsContext::ECenter);
	aGc.DiscardFont();
	aMap->ReleaseFont(font);
	}


EXPORT_C void CXzePicture::GetOriginalSizeInTwips(TSize& aSize)const
//
	{aSize=iOriginalSizeInTwips;}


EXPORT_C CXzeDoor* CXzeDoor::NewL(TChar aLabel,TBool aAlwaysFailToDetach)
	{return new(ELeave) CXzeDoor(aLabel,aAlwaysFailToDetach);}


EXPORT_C CXzeDoor* CXzeDoor::NewL(const CStreamStore& aStore,TStreamId aId)
// Restoring NewL.
//
	{
	CXzeDoor* self=new(ELeave) CXzeDoor(EFalse);
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aId);
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CXzeDoor::CXzeDoor(TChar aLabel,TBool aAlwaysFailToDetach)
// Sets the startup attributes of this picture
//
	: CXzePicture(aLabel), iAlwaysFailToDetach(aAlwaysFailToDetach)
	{ResetToOriginal();}


EXPORT_C CXzeDoor::CXzeDoor(TBool aAlwaysFailToDetach)
// Sets the startup attributes of this picture
//
//	
	: iAlwaysFailToDetach(aAlwaysFailToDetach)
	{ResetToOriginal();}


EXPORT_C void CXzeDoor::DetachFromStoreL(TDetach /*aDegree*/)
//
//
	{
	if (iAlwaysFailToDetach)
		User::Leave(KErrNotSupported);
	}

EXPORT_C void CXzeDoor::ExternalizeL(RWriteStream& aStream)const
// Save this picture.
// Typically called from the base class StoreL();
//
	{
	CXzePicture::ExternalizeL(aStream);
	aStream.WriteUint8L((TUint8)iAlwaysFailToDetach!=EFalse);
	}


EXPORT_C void CXzeDoor::InternalizeL(RReadStream& aStream)
// Load this picture
//
	{
	CXzePicture::InternalizeL(aStream);
	iAlwaysFailToDetach=TBool(aStream.ReadUint8L());
	}


EXPORT_C void CXzeDoor::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
// Create a read-stream over aStore, and open it over the specified stream ID.
// Internalize picture from this stream.
//
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	stream>> *this;
	CleanupStack::PopAndDestroy();
	}


EXPORT_C CTestPicture* CTestPicture::NewL()
	{return new(ELeave) CTestPicture();}


EXPORT_C CTestPicture::CTestPicture()
	{ResetToOriginal();}


EXPORT_C void CTestPicture::GetOriginalSizeInTwips(TSize& aSize)const
//
	{aSize=iOriginalSizeInTwips;}


EXPORT_C void CTestPicture::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* /*aMap*/) const
// draw a simple object
 {
	aGc.Reset();
	aGc.SetClippingRect(aClipRect);
	TSize size; // Size in pixels
	TSize sizeInner; // In pixels
	TRect box;
	GetSizeInPixels(aGc.Device(),size);
	box.iTl=aTopLeft;
	box.iBr.iX=aTopLeft.iX+size.iWidth;
	box.iBr.iY=aTopLeft.iY+size.iHeight;
	TRgb black(0,0,0);
	TRgb white(255,255,255);
// First draw outer box and fill in rest of box.
	aGc.SetBrushColor(white);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(box);
// Inner box.
	sizeInner.iWidth=size.iWidth/3;
	sizeInner.iHeight=size.iHeight/3;
	box.iTl.iX+=sizeInner.iWidth;
	box.iTl.iY+=sizeInner.iHeight;
	box.iBr.iX-=sizeInner.iWidth;
	box.iBr.iY-=+sizeInner.iHeight;
	aGc.SetBrushColor(black);
	aGc.SetBrushStyle(CGraphicsContext::EDiamondCrossHatchBrush);
	aGc.DrawRect(box);
	}


EXPORT_C void CTestPicture::ExternalizeL(RWriteStream& /*aStream*/)const
	{}


////////////////////////////////////////////////////////////////////////

EXPORT_C CDummyField::CDummyField()
	{
	}


EXPORT_C TInt CDummyField::Value(TPtr& aValueText)
	{
	if (aValueText.MaxLength() < 3)
		return 3;
	else 
		{
		aValueText = _L("XXX");
		return 0;
		}
	}


EXPORT_C void CDummyField::ExternalizeL(RWriteStream& aStream)const
	{
	aStream.WriteUint8L(0);  // empty streams cause problems
	}


EXPORT_C void CDummyField::InternalizeL(RReadStream& aStream)
	{
	TUint8 dummy=aStream.ReadUint8L();
	dummy=0;
	}


EXPORT_C TUid CDummyField::Type() const
	{
	return KNullUid;
	}
