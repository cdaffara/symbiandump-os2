/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestPicture.cpp test picture.
*
*/


#include "TestPicture.h"

CTestPicture::CTestPicture()
	{
	iLineBreaksAllowed[0] = ETrue;
	iLineBreaksAllowed[1] = ETrue;
	iLineBreaksAllowed[2] = ETrue;
	iLineBreaksAllowed[3] = ETrue;
	iSizeInTwips.iWidth = 100;
	iSizeInTwips.iHeight = 100;
	}

//draw a red square
void CTestPicture::Draw(CGraphicsContext& aGc, const TPoint& aTopLeft,
	const TRect& aClipRect, MGraphicsDeviceMap* aMap) const
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

/**
Does nothing.
@param aStream No effect.
@internalComponent
*/
void CTestPicture::ExternalizeL(RWriteStream& /*aStream*/) const
	{
	}

/**
Returns size of picture.
@param aSize
	Returns size of picture.
@internalComponent
*/
void CTestPicture::GetOriginalSizeInTwips(TSize& aSize) const
	{
	aSize = iSizeInTwips;
	}

/**
Returns information supplied by SetLineBreaksAllowed.
@param aClass
	Not used.
@param aBeforePicture As CPicture.
@param aHaveSpaces As CPicture.
@return TBool if line breaks are allowed.
@internalComponent
*/
TBool CTestPicture::LineBreakPossible(TUint /*aClass*/,
	TBool aBeforePicture, TBool aHaveSpaces) const
	{
	TInt index = aBeforePicture? 0 : 2;
	index += aHaveSpaces? 1 : 0;
	return iLineBreaksAllowed[index];
	}

/**
Set whether LineBreakPossible should return ETrue.
@param aBeforeWithoutSpaces
	Sets the result of LineBreakPossible(*, ETrue, EFalse);
@param aBeforeWithSpaces
	Sets the result of LineBreakPossible(*, ETrue, ETrue);
@param aAfterWithoutSpaces
	Sets the result of LineBreakPossible(*, EFalse, EFalse);
@param aAfterWithSpaces
	Sets the result of LineBreakPossible(*, EFalse, ETrue);
@internalComponent
*/
void CTestPicture::SetLineBreaksAllowed(
	TBool aBeforeWithoutSpaces,
	TBool aBeforeWithSpaces,
	TBool aAfterWithoutSpaces,
	TBool aAfterWithSpaces)
	{
	iLineBreaksAllowed[0] = aBeforeWithoutSpaces;
	iLineBreaksAllowed[1] = aBeforeWithSpaces;
	iLineBreaksAllowed[2] = aAfterWithoutSpaces;
	iLineBreaksAllowed[3] = aAfterWithSpaces;
	}

/**
Sets the picture size.
@param aSize
	The size to set.
@internalComponent
*/
void CTestPicture::SetSizeInTwips(TSize aSize)
	{
	iSizeInTwips = aSize;
	}

/**
Sets the picture size so that the graphics device map aMap will convert to
the size specified.
@param aSize The size to set.
@param aMap The graphics device in which aSize should be the size in pixels.
@internalComponent
*/
void CTestPicture::SetSizeInPixels(TSize aSize, const MGraphicsDeviceMap& aMap)
	{
	iSizeInTwips.iWidth = aMap.HorizontalTwipsToPixels(aSize.iWidth);
	iSizeInTwips.iHeight = aMap.VerticalTwipsToPixels(aSize.iHeight);
	}
