/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testpicture.h"

EXPORT_C void TTestPictureFactory::NewPictureL(
	TPictureHeader& aHeader, const CStreamStore& aDeferredPictureStore) const
	{
	aHeader.iPicture=CTestPicture::NewL(aDeferredPictureStore,aHeader.iPicture.AsId());		
	}

EXPORT_C CTestPicture* CTestPicture::NewLC(TSize aSize)
	{
	CTestPicture* p = new(ELeave) CTestPicture(aSize);
	CleanupStack::PushL(p);
	return p;
	}

EXPORT_C CTestPicture* CTestPicture::NewL(const CStreamStore& aStore,TStreamId aId)
	{
	CTestPicture* self=new(ELeave) CTestPicture();
	CleanupStack::PushL(self);
	self->RestoreL(aStore,aId);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CTestPicture::CTestPicture()	
	{
	}

EXPORT_C CTestPicture::CTestPicture(TSize aSize)
	:iOriginalSizeInTwips(aSize)	
	{
	ResetToOriginal();
	}

EXPORT_C CTestPicture::~CTestPicture()
	{
	}

EXPORT_C void CTestPicture::ExternalizeL(RWriteStream& aStream)const
	{
	aStream.WriteUint32L(iLabel);
	aStream << iOriginalSizeInTwips;
	}


EXPORT_C void CTestPicture::InternalizeL(RReadStream& aStream)
	{
	iLabel=(TChar)aStream.ReadUint32L();
	aStream >> iOriginalSizeInTwips;
	}

EXPORT_C void CTestPicture::RestoreL(const CStreamStore& aStore,TStreamId aStreamId)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStore,aStreamId);
	stream>> *this;
	CleanupStack::PopAndDestroy();
	ResetToOriginal();
	}


EXPORT_C void CTestPicture::Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap) const
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

EXPORT_C void CTestPicture::GetOriginalSizeInTwips(TSize& aSize)const
//
	{
	aSize=iOriginalSizeInTwips;
	}
