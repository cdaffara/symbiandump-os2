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
*
*/


#ifndef __TESTPICTURE_H__
#define __TESTPICTURE_H__

#include <e32std.h>
#include <e32base.h>
#include <s32stor.h>
#include <gdi.h>

const TUid KUidTestPictureType={268435511};

//
class CTestPicture : public CPicture
/**
Very simple picture: just draws a red square. 
@internalComponent
*/
    {
public:
	IMPORT_C static CTestPicture* NewLC(TSize aSize);
	IMPORT_C static CTestPicture* NewL(const CStreamStore& aStore,TStreamId aId);
	IMPORT_C ~CTestPicture();
	//
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void RestoreL(const CStreamStore& aStore,TStreamId aId);
	//
	IMPORT_C void GetOriginalSizeInTwips(TSize& aSize) const;
	//
	IMPORT_C virtual void Draw(CGraphicsContext& aGc,const TPoint& aTopLeft,const TRect& aClipRect,MGraphicsDeviceMap* aMap=NULL)const;
private:
	CTestPicture(TSize aSize);
	CTestPicture();
	TSize iOriginalSizeInTwips;
/**
iLabel is a dummy data member. The CPicture framework seems to assume that 
all CPictures will contain data members to externalize. If we do not externalize 
anything to the stream we're given in ExternalizeL, then the stream store 
will abandon that stream and reuse its stream ID for the next picture. 
CStoreMap::BindL will subsequently panic because the map contains two identical 
stream IDs which refer to different pictures.
So we write something to the stream, to ensure that the store generates a 
unique ID for each picture.
*/
	TChar iLabel; 
	};

class TTestPictureFactory : public MPictureFactory
/**
Factory which creates CWordTestPicture objects only. 
Used for restoring pictures when loading documents. 
CWordDocument needs to supply one of these to the rich text object. 
@internalComponent
*/
	{
public:
	virtual void NewPictureL(TPictureHeader& aHeader,const CStreamStore& aDeferredPictureStore) const;
	};

#endif
