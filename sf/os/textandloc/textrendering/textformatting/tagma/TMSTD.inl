/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Inline implementations for TMSTD header.
*
*/




/**
 @file
*/

inline CTmTextFontCache::CTmTextFontCache(MGraphicsDeviceMap& aDevice, CFont& aFont)
	: iRefCount(1), iDevice(aDevice), iFont(aFont)
	{}
	
/**
 The constructor is protected by a New here not because there is a 2 stage constructor, 
 but to enforce that this object should be allocated on the heap, so that 'delete this' works
 This reference counting is required because this object can be deleted by either (1) a client of RTmTextCache
 when it is finished using a font, or (2) by the RTmTextCache itself, when the font in the cache is replaced
 */
inline CTmTextFontCache* CTmTextFontCache::New(MGraphicsDeviceMap& aDevice, CFont& aFont)
	{
	return new CTmTextFontCache(aDevice, aFont);
	}

/**
 This should be called before the font is retrieved and used
 Open does not need to be called by the creator of the font (the reference count is initialized to 1)
 */
inline void CTmTextFontCache::Open()
	{
	iRefCount++;
	}

/**
 This should be called when the font is no longer used. The font is released when all references have been closed.
 */
inline void CTmTextFontCache::Close()
	{
	ASSERT(iRefCount > 0);
	iRefCount--;
	if (0 == iRefCount)
		{
		delete this;
		}
	}

inline CTmTextFontCache::~CTmTextFontCache()
	{
	iDevice.ReleaseFont(&iFont);
	}

inline CFont& CTmTextFontCache::Font()
	{
	ASSERT(iRefCount >= 0);
	return iFont;
	}

