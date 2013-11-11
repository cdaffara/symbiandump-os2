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


#ifndef __FLDBLTIN_H__
#define __FLDBLTIN_H__

#include <e32std.h>
#include <fldbase.h>


// Classes declared in this file
class MFieldPageNumInfo;
class MFieldNumPagesInfo;
class MFieldFileNameInfo;
// the built-in field types
class CDateTimeField;
class CPageNumField;
class CNumPagesField;
class CFileNameField;
class TRomanNumeral;
//
// Classes referenced
class RReadStream;
class RWriteStream;

 

/** 
Specifies the mixin protocol for evaluating a current page number field.

You should implement the UpdateFieldPageNum() function in a concrete derived 
class, then pass an object of the class to the page number field (using CPageNumField::SetPageNumInfo()) 
before the field can be evaluated. 
@publishedAll
@released
*/
class MFieldPageNumInfo
	{
public:
	
	 
	/** Implementations of this function should return the current page number.
	
	@return The page number. */
	virtual TInt UpdateFieldPageNum()const=0;
	};


/** 
Specifies the mixin protocol for evaluating a total number of pages field.

You should implement the UpdateFieldNumPages() function in a concrete derived 
class, then pass an object of the class to the number of pages field (using 
CNumPagesField::SetNumPagesInfo()) before the field can be evaluated. 
@publishedAll
@released
*/
class MFieldNumPagesInfo
	{
public:
	
	 
	/** Implementations of this function should return the number of pages in the current 
	document.
	
	@return The total number of pages. */
	virtual TInt UpdateFieldNumPages()const=0;
	};


 
/** 
Specifies the mixin protocol for evaluating a filename field.

You should implement the UpdateFieldFileName() function in a concrete derived 
class, then pass an object of the derived class to the filename field (using 
CFileNameField::SetFileNameInfo()) before the field can be evaluated. 
@publishedAll
@released
*/
class MFieldFileNameInfo
	{
public:
	
	 
	/** Implementations of this function should set aValueText to the current document's 
	filename, if the buffer is large enough. If not, the function should return 
	the length which is required to hold the filename.
	
	@param aValueText Descriptor which on return contains the document's filename. 
	
	@return Zero if aValueText is long enough to hold the filename. Otherwise, 
	the length of the buffer which is required to hold the filename. */
	virtual TInt UpdateFieldFileName(TPtr& aValueText)const=0;
	};


 
/** 
A date/time field. 

This may contain any or all components of the date and time, and can be formatted 
in a variety of ways. It stores a format string, which is used by the Value() 
function to format the current date/time. For information on date/time formatting, 
see TTime::FormatL(). 
@publishedAll
@released
*/
class CDateTimeField : public CTextField
	{
public:
	IMPORT_C CDateTimeField();
	IMPORT_C void SetFormat(const TDesC& aFormat);
	// from TTextField
	IMPORT_C virtual TInt Value(TPtr& aValueText);
	IMPORT_C virtual void InternalizeL(RReadStream& aStream);
	IMPORT_C virtual void ExternalizeL(RWriteStream& aStream)const;
	//
	// Getters
	IMPORT_C const TDesC& FormatString()const;
	IMPORT_C TUid Type()const;
protected:
	TBuf<64> iFormatString;
	};


 

/** 
Stores a style for displaying the value of numeric fields. 

This style is used when converting the integer value of numeric fields into 
a descriptor for display in another format, e.g. Arabic, Roman, alphabetic. 
This is the base class for the numeric fields, CPageNumField and CNumPagesField. 
@publishedAll
@released
*/
class CPageFieldBase : public CTextField
	{
public:
/** Numeric style */
	enum TNumberStyle {
		 /** Arabic numeral, e.g. 1, 2, 3. */
		EArabic,			// 1,2,3
		/** Upper case Roman numeral, e.g. I, II, III. */
		ERomanUpper,		// I,II,III
		/** Lower case Roman numeral, e.g. i, ii, iii. */
		ERomanLower,		// i,ii,iii
		/** Upper case alphabetic. */
		EAlphabeticUpper,	// A,B,C
		/** Lower case alphabetic. */
		EAlphabeticLower	// a,b,c
		};
public:
	
	/** Sets the numeric style.
	
	@param aStyle The numeric style. */
	inline void SetNumberStyle(TNumberStyle aStyle) { iStyle = aStyle; } 
	// from TTextField
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const;
	//
	// Getters
	IMPORT_C TNumberStyle NumberStyle()const;
protected:
	TInt InsertValue(TPtr& aValueText,TInt aValue);
protected:
	TNumberStyle iStyle;
	};



/** 
A field which evaluates to the current page number in the document. 

Before the page number field can be evaluated, it must be passed a pointer 
to an object which implements the UpdateFieldPageNum() function. 
@publishedAll
@released
*/
class CPageNumField : public CPageFieldBase
	{
public:
	/** Sets the object which implements UpdateFieldPageNum(), to get the current page 
	number. SetPageNumInfo() must be called before the page number field can be 
	evaluated.
	
	@param aInfo Pointer to an object which implements UpdateFieldPageNum(). */
	inline void SetPageNumInfo(MFieldPageNumInfo* aInfo) { iPageNumInfo=aInfo; }
	// from TTextField
	IMPORT_C TInt Value(TPtr& aValueText);
	IMPORT_C TUid Type()const;
protected:
	MFieldPageNumInfo* iPageNumInfo;
	};



/** 
A field which evaluates to the number of pages in the document.

Before the number of pages field can be evaluated, it must be passed a pointer 
to an object which implements the UpdateFieldNumPages() function. 
@publishedAll
@released
*/
class CNumPagesField : public CPageFieldBase
	{
public:
	/** Sets the object which implements UpdateFieldNumPages(), to get the number of 
	pages in the document. SetNumPagesInfo() must be called before the number 
	of pages field can be evaluated.
	
	@param aInfo Pointer to an object which implements UpdateFieldNumPages(). */
	inline void SetNumPagesInfo(MFieldNumPagesInfo* aInfo) { iNumPagesInfo=aInfo; }
	// from TTextField
	IMPORT_C TInt Value(TPtr& aValueText);
	IMPORT_C TUid Type()const;
protected:
	MFieldNumPagesInfo* iNumPagesInfo;
	};




/** 
A filename field. 

This is a field which evaluates to the filename of the current document. Before 
the filename field can be evaluated, it must be passed a pointer to an object 
which implements the UpdateFieldFileName() function. 
@publishedAll
@released
*/
class CFileNameField : public CTextField
	{
public:
	/** Sets the object which implements the UpdateFieldFileName() function, to get 
	the current document's filename. SetFileNameInfo() must be called before the 
	filename field can be evaluated.
	
	@param aInfo Pointer to an object which implements the UpdateFieldFileName() 
	function. */
	inline void SetFileNameInfo(MFieldFileNameInfo* aInfo) { iFileNameInfo=aInfo; }
	// from TTextField
	IMPORT_C virtual TStreamId StoreL(CStreamStore& aStore)const;  // returns KNullStreamId
	IMPORT_C virtual void RestoreL(const CStreamStore& aStore,TStreamId aId); // does nothing.
	IMPORT_C virtual TInt Value(TPtr& aValueText);
	

	/** Overrides the base class method to do nothing, because this class has no persistent 
	data. */
	inline virtual void InternalizeL(RReadStream& /*aStream*/) {}; // stream the formatting
	IMPORT_C TUid Type()const;
protected:
	MFieldFileNameInfo* iFileNameInfo;
	};


#endif
