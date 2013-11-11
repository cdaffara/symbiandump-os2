/*
* Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LINKEDFONTSPRIVATE_H__
#define LINKEDFONTSPRIVATE_H__
#include <linkedfonts.h>
#include "FNTSTD.H"

class CFontStore;
class COpenFontLinkedTypefaceElementSpec;

/**
* A type for a single element in a linked typeface description -
* Internal technology - this is used for client server calls -
* class basically the same as CLinkedTypefaceElement, but a T class.
*/

typedef TBufC<KMaxTypefaceNameLength>  TTypefaceName;

/* Internal technology - data body for CLinkedTypefaceSpecification */
class CLinkedTypefaceSpecificationBody : public CBase
	{
friend class CLinkedTypefaceSpecification;
friend class CFont;
friend class CFontStore;
private:
	CLinkedTypefaceSpecificationBody();
	~CLinkedTypefaceSpecificationBody();
private:
	RPointerArray <CLinkedTypefaceElementSpec> iTypefaces;
	RPointerArray <CLinkedTypefaceGroup> iGroups;
	HBufC* iLinkedTypefaceName;
	};

/* Internal technology - data body for COpenFontLinkedTypefaceSpecification */
class COpenFontLinkedTypefaceSpecificationBody : public CBase
	{
friend class COpenFontLinkedTypefaceSpecification;
friend class CFont;
friend class CFontStore;
private:
	COpenFontLinkedTypefaceSpecificationBody();
	~COpenFontLinkedTypefaceSpecificationBody();
private:
	RPointerArray <COpenFontLinkedTypefaceElementSpec> iTypefaces;
	HBufC* iLinkedTypefaceName;
	TInt iId;
	TInt iSession; //Not owned
	TInt iUsageCounter; //


	const COpenFontGlyph* GlyphLinked(TInt aCode,TInt*& aNode);
	void RasterizeL(TInt aCode,TOpenFontGlyphData* aGlyphData);
	};

#endif //LINKEDFONTSPRIVATE_H__

