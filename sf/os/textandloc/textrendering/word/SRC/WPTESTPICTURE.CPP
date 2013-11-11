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
* WORDTESTPICTURE.CPP
*
*/


#include "WPTESTPICTURE.H"
#include "WPPANIC.H"

const TInt KWordTestPictureDimensions = 500; //width & height in twips

EXPORT_C void TWordTestPictureFactory::NewPictureL(
	TPictureHeader& aHeader, const CStreamStore& aDeferredPictureStore) const
	{
	if (aHeader.iPictureType==KUidWordTestPictureType)	
		{
		aHeader.iPicture=CWordTestPicture::NewL(aDeferredPictureStore,aHeader.iPicture.AsId());		
		}
	else
		{
		Panic(EPictureTypeNotSupported);			
		}
	}

EXPORT_C CWordTestPicture* CWordTestPicture::NewLC()
	{
	CWordTestPicture* p = new(ELeave) CWordTestPicture();
	CleanupStack::PushL(p);
	return p;
	}

EXPORT_C CWordTestPicture* CWordTestPicture::NewL(const CStreamStore& aStore,TStreamId aId)
	{
	CWordTestPicture* self=new(ELeave) CWordTestPicture();
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aId);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWordTestPicture::CWordTestPicture()
	:iOriginalSizeInTwips(KWordTestPictureDimensions,KWordTestPictureDimensions)	
	{
	ResetToOriginal();
	}

EXPORT_C CWordTestPicture::~CWordTestPicture()
	{
	}

EXPORT_C void CWordTestPicture::ExternalizeL(RWriteStream& aStream)const
	{
	aStream.WriteUint32L(iLabel);
	}


EXPORT_C void CWordTestPicture::InternalizeL(RReadStream& aStream)
	{
	iLabel=(TChar)aStream.ReadUint32L();
	}

EXPORT_C void CWordTestPicture::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	stream>> *this;
	CleanupStack::PopAndDestroy();
	}


EXPORT_C void CWordTestPicture::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap) const
//draw a red square
 	{
	aGc.Reset();
	aGc.SetClippingRect(aClipRect);
	TSize size;  
	GetSizeInPixels(aMap,size);
	TRect box;  
	box.iTl=aTopLeft;
	box.iBr.iX=aTopLeft.iX+size.iWidth;
	box.iBr.iY=aTopLeft.iY+size.iHeight;
	TRgb red(255,0,0);
	aGc.SetBrushColor(red);
	aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	aGc.DrawRect(box);
	}

EXPORT_C void CWordTestPicture::GetOriginalSizeInTwips(TSize& aSize)const
//
	{
	aSize=iOriginalSizeInTwips;
	}
