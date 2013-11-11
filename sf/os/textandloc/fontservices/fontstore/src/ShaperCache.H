/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SHAPERCACHE.CPP
*
*/


#include <fntstore.h>
#include <gdi.h>
#include "FNTSTD.H"
#include <graphics/shapeimpl.h>

const TInt KMaxShaperSesssionCacheMemory = 64*1024;

//class to store the handle the handle-reference count
class THandleCount
	{
public:
	THandleCount(TInt aSessionHandle)
	:iSessionHandle(aSessionHandle),iRefCount(1),iNext(NULL){}	
public:
	TInt iSessionHandle;
	TInt iRefCount;
	THandleCount* iNext;	
	};

/******************** SHAPER CACHE CLASSES*************************************/	
class COpenFontShaperCacheEntry: public CBase
	{
public:
	static COpenFontShaperCacheEntry* New(RHeap* aHeap, CShaper::TInput aInput, TShapeHeader* aShapeHeader);
	static COpenFontShaperCacheEntry* New(RHeap* aHeap);
	static void Delete (RHeap* aHeap, COpenFontShaperCacheEntry* aCache);	
	TBool IsSentinel(){return iShapeHeader==NULL;}
	TInt IncRefCount(TInt aSessionHandle);
	TInt DecRefCount(TInt aSessionHandle,TBool aReset=EFalse);	
private:
	COpenFontShaperCacheEntry(RHeap* aHeap,CShaper::TInput aInput)
	:iStart(aInput.iStart),iEnd(aInput.iEnd),iScript(aInput.iScript), iHeap(aHeap){}
	COpenFontShaperCacheEntry(){}
	TInt Construct(CShaper::TInput aInput,TShapeHeader* aShapeHeader);
public:
	COpenFontShaperCacheEntry* iPrevious;	// pointer to previous node in tree
	COpenFontShaperCacheEntry* iNext;		// pointer to next shaping info node in the list

	/** The shaped information that is cached */
	TShapeHeader* iShapeHeader;
	/** Input information */
	TUint16* iText;
	TInt iStart;
	TInt iEnd;
	TInt iScript;
	/** pointer to a private heap for allocating the handle-refcount array*/
	RHeap* iHeap;
	/** linked list of THandleCount to store the handle-refcount information */
	THandleCount* iHandleRefList;
	TInt iHandleRefCount;
	};
	
/***********************SHAPER CACHE ENDS HERE********************************/
	
