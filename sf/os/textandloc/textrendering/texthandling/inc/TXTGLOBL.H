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


#ifndef __TXTGLOBL_H__
#define __TXTGLOBL_H__

#include <e32std.h>
#include <e32base.h>
#include <txtlaydc.h>
#include <txtmfmtx.h>
#include <txtetext.h>


// forward declarations
class CParaFormatLayer;
class CCharFormatLayer;
class TCharFormatX;
class TCharFormatXMask;


// Globally formatted text.
// Uses one instance of each a paragraph format and character format class; Each presents
// the formatting applied to the total document contents.  Both of the format classes used
// may utilise layering of formats through 'based on' links to achieve full format specification.
// These global layers are *never* owned by global text, merely referenced.

/** 
Text with globally applied formatting. 

Global text uses a single set of paragraph format attributes to describe the 
appearance of every paragraph and a single set of character format attributes 
to describe the appearance of every character.

The paragraph formatting is stored in a CParaFormatLayer and the character 
formatting is stored in a CCharFormatLayer. A format layer is a set of character 
or paragraph format attributes, which may own a pointer to another format 
layer. This pointer is called a based-on link. The effective formatting of 
a global text object is the set of attributes specified in its two format 
layers, as well as any attributes inherited from the layers' chain of based-on 
links. To get the effective formatting, the links are read in turn, so that 
each attribute's value is taken from the first layer in the chain in which 
it has been set. Use GetCharFormat() and GetParaFormatL() to get the text 
object's effective formatting.

A global text object references the two format layers; it does not own them. 
This allows more than one global text object to use the same formatting. The 
format layers may be modified (see ApplyParaFormatL() and ApplyCharFormatL()) 
or replaced (see SetGlobalParaFormat() and SetGlobalCharFormat()) during the 
text object's lifetime. 
@publishedAll
@released
*/
class CGlobalText : public CPlainText,public MLayDoc,public MFormatText
	{
public:
//	Special member functions
	// Create new global text component whose sole content is an end-of-document character.	
	IMPORT_C static CGlobalText* NewL(const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									 TDocumentStorage aStorage=ESegmentedStorage,TInt aDefaultTextGranularity=EDefaultTextGranularity);
	// Restore a new global text, that uses the specified global layers.
	IMPORT_C static CGlobalText* NewL(const CStreamStore& aStore,TStreamId aStreamId,
									  const CParaFormatLayer* aGlobalParaLayer,const CCharFormatLayer* aGlobalCharLayer,
									  MTextFieldFactory* aFieldFactory=NULL,
									  TDocumentStorage=ESegmentedStorage);
	IMPORT_C ~CGlobalText();
//	Global Format Layers
	IMPORT_C void SetGlobalParaFormat(const CParaFormatLayer* aParaFormatLayer);
	IMPORT_C void SetGlobalCharFormat(const CCharFormatLayer* aCharFormatLayer);
	inline const CParaFormatLayer* GlobalParaFormatLayer()const;
	inline const CCharFormatLayer* GlobalCharFormatLayer()const;
//	MLayDoc implementation
	IMPORT_C virtual TInt LdDocumentLength()const;
	IMPORT_C virtual TInt LdToParagraphStart(TInt& aCurrentPos)const;
	IMPORT_C virtual void GetParagraphFormatL(CParaFormat* aFormat,TInt aPos)const;
	IMPORT_C virtual void GetChars(TPtrC& aView,TCharFormat& aFormat,TInt aStartPos)const;
	IMPORT_C virtual TInt GetPictureSizeInTwips(TSize& aSize, TInt aPos)const;
	IMPORT_C virtual CPicture* PictureHandleL(TInt aPos,MLayDoc::TForcePictureLoad aForceLoad)const;
	IMPORT_C virtual TBool EnquirePageBreak(TInt aPos,TInt aLength=0)const;
	IMPORT_C virtual TBool SelectParagraphLabel(TInt aPos);
	IMPORT_C virtual void CancelSelectLabel();
//  MFormatText implementation	
	IMPORT_C virtual void GetParaFormatL(CParaFormat* aFormat,TParaFormatMask& aVaries,TInt aPos,TInt aLength,
										CParaFormat::TParaFormatGetMode aMode=CParaFormat::EAllAttributes)const;
	IMPORT_C virtual void ApplyParaFormatL(const CParaFormat* aFormat,const TParaFormatMask& aMask,TInt aPos,TInt aLength);
	IMPORT_C virtual void GetCharFormat(TCharFormat& aFormat,TCharFormatMask& aVaries,TInt aPos,TInt aLength)const;
	IMPORT_C virtual void ApplyCharFormatL(const TCharFormat& aFormat,const TCharFormatMask& aMask,TInt aPos,TInt aLength);
protected:
	IMPORT_C CGlobalText();
	IMPORT_C CGlobalText(const CParaFormatLayer* aGlobalParaLayer,
						 const CCharFormatLayer* aGlobalCharLayer);
private:
	CGlobalText(const CGlobalText& aGlobalText);
	CGlobalText& operator=(const CGlobalText& aGlobalText);
protected:
	const CParaFormatLayer* iGlobalParaFormatLayer;  // the global paragraph format layer; not owned
	const CCharFormatLayer* iGlobalCharFormatLayer;  // the global character format layer; not owned
	__DECLARE_TEST;
private:
	void* iReserved_2;
	};


#include <txtglobl.inl>


#endif
