// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Incremental matcher class
// 
//

#if !defined (__BAMATCH_H__)
#define __BAMATCH_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__BAMDESCA_H__)
#include <bamdesca.h>
#endif


class RTextBuf
/** A simple class that encapsulates a text string.

As with the descriptor classes, the class sets a maximum length for the encapsulated 
string, which cannot be exceeded. The class might be preferred to a descriptor 
in some circumstances as its maximum length can be increased (unlike a TBuf), 
and it can be created on the stack (unlike an HBufC) . 
@publishedAll
@released
*/
	{
public:
	IMPORT_C RTextBuf();
	IMPORT_C ~RTextBuf();
public:
	inline TPtr& Text();
	inline const TPtr& Text() const;
	inline TInt MaxLength() const;
	inline void SetText(const TDesC &aDes);
	IMPORT_C void SetMaxLengthL(TInt aMaxLength);
	IMPORT_C void Close();
private:
	TPtr iPtr;
	TText* iText;
	};



class RIncrMatcherBase
/** Base class for incremental matcher classes.

An incremental matcher compares two text buffers from left-to-right. For example, 
the match between "garage" and "gander" is "ga".

The class provides the interface for getting and setting the text to be matched 
against, and for performing match tests. Each type of match test is available 
in three versions, one using binary comparison, one using locale collation 
rules (comparison of strings to produce a dictionary-like ('lexicographic') 
ordering, e.g. ignoring punctuation), and one using locale folding rules (e.g. 
ignoring case). 

"bafl.lib" 
@since 5.0
@publishedAll
@released
*/
	{
public:
    IMPORT_C virtual ~RIncrMatcherBase();
protected:
	/** Default constructor. */
	inline RIncrMatcherBase() {}
protected:
	/** Gets the match text.
	
	@return Match text */
	virtual TDes& MatchDes()=0;

	/** Gets the match text.
	
	@return Match text */
	virtual const TDes& MatchDes() const=0; 
public:
	inline TInt MaxLength() const;
	inline TInt MatchLength() const;
	inline TPtrC MatchText() const;
//
	inline void Clear();
	IMPORT_C void DeleteLastChar();
	IMPORT_C void AppendChar(TChar aLetter);
	inline void SetMatchText(const TDesC& aText);
//
	IMPORT_C void SetBestMatch(const TDesC& aBuf);
	IMPORT_C void SetBestMatchC(const TDesC& aBuf);
	IMPORT_C void SetBestMatchF(const TDesC& aBuf);
	IMPORT_C TBool IsMatch(const TDesC& aBuf) const;
	IMPORT_C TBool IsMatchC(const TDesC& aBuf) const;
	IMPORT_C TBool IsMatchF(const TDesC& aBuf) const;
	IMPORT_C TInt FirstMatchingIndex(TInt& aResult,const MDesCArray& aDesArray,TInt aStartIndex=0) const;
	IMPORT_C TInt FirstMatchingIndexC(TInt& aResult,const MDesCArray& aDesArray,TInt aStartIndex=0) const;
	IMPORT_C TInt FirstMatchingIndexF(TInt& aResult,const MDesCArray& aDesArray,TInt aStartIndex=0) const;
private:
	typedef TInt (*TCompareFunc)(const TText*,TInt,const TText*,TInt);
	TBool DoIsMatch(const TDesC& aQuery,TCompareFunc aCompare) const;
	TInt DoFirstMatchingIndex(TInt& aIndex,const MDesCArray& aDesArray,TCompareFunc aCompare) const;
	void DoSetBestMatch(const TDesC& aBuf,TCompareFunc aCompare);
	};


class RIncrMatcherPtr : public RIncrMatcherBase
/** Incrementally matches text against a descriptor, accessed via a pointer.

The class does not take a copy of the text to match against, but only stores 
the pointer. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C RIncrMatcherPtr();
	IMPORT_C RIncrMatcherPtr(TDes& aDes);
    IMPORT_C virtual ~RIncrMatcherPtr();
public:
	inline void SetMatcherPtr(TDes& aDes);
protected:
	IMPORT_C TDes& MatchDes();
	IMPORT_C const TDes& MatchDes() const;
private:
	TDes* iDesPtr;
	};


class RIncrMatcherTextBuf : public RIncrMatcherBase
/** Incrementally matches text against a text buffer with variable maximum 
length. 
@publishedAll
@released
*/
	{
public:
    IMPORT_C RIncrMatcherTextBuf();
    IMPORT_C virtual ~RIncrMatcherTextBuf();
	inline void SetMatcherLengthL(TInt aMaxLength);
protected:
	IMPORT_C TDes& MatchDes();
	IMPORT_C const TDes& MatchDes() const;
private:
	RTextBuf iTextBuf;
	};

template <TInt aMaximumSize> 
class RIncrMatcherBuf : public RIncrMatcherBase
/**
Incrementally matches text against a modifiable descriptor buffer.
Set aMaximumSize to be the maximum size of this buffer. 
@publishedAll
@released
*/
	{
public:
	/** Default constructor. */
	RIncrMatcherBuf() {}
protected:

	/**	 Gets the match text.
	 
	  @return   Match text (non const) */
	inline TDes& MatchDes() {return(iMatchBuf);}

	/**	 Gets the match text.
	 
	  @return   Match text (const)*/
	inline const TDes& MatchDes() const {return(iMatchBuf);}

	/** Copy constructor.
	
	@param aMatcher RIncrMatcherBuf to copy */
	inline RIncrMatcherBuf(const RIncrMatcherBuf& aMatcher) {iMatchBuf=aMatcher.iMatchBuf;}
private:
	TBuf<aMaximumSize> iMatchBuf;
    };

inline TPtr& RTextBuf::Text()
/** Gets the text as a descriptor. 

@return   Match text descriptor (non const) */
	{return(iPtr);}

inline const TPtr& RTextBuf::Text() const
/** Gets the text as a descriptor. 

@return   Match text descriptor (const) */
	{return(iPtr);}

inline TInt RTextBuf::MaxLength() const
/** Gets the maximum length of the text. 

@return The maximum length of the text.*/
	{return(iPtr.MaxLength());}

inline void RTextBuf::SetText(const TDesC &aDes)
/** Sets the text. You must have set the maximum length appropriately first with
SetMaxLengthL(). */
	{iPtr.Copy(aDes);}

inline TInt RIncrMatcherBase::MaxLength() const
/** Gets the maximum length of the match text buffer. */
	{return(MatchDes().MaxLength());}
inline TInt RIncrMatcherBase::MatchLength() const
/** Gets the current length of the match text buffer. */
	{return(MatchDes().Length());}
inline TPtrC RIncrMatcherBase::MatchText() const
/** Gets the match text as a TPtrC. */
	{return(TPtrC(MatchDes()));}
inline void RIncrMatcherBase::SetMatchText(const TDesC& aText)
/** Sets the match text.
	
@param aText String to which to set the match text. */
	{MatchDes()=aText;}

inline void RIncrMatcherBase::Clear()
/** Clears the match text. */
	{MatchDes().Zero();}

inline void RIncrMatcherPtr::SetMatcherPtr(TDes& aDes)
/** Sets the text to be matched against.
	
@param aDes Text to be matched against */
	{iDesPtr=&aDes;}


inline void RIncrMatcherTextBuf::SetMatcherLengthL(TInt aMaxLength)
/** Sets the maximum length of text that can be stored through SetMatchText() 
etc.
	
@param aMaxLength Maximum length of text that can be stored */
	{iTextBuf.SetMaxLengthL(aMaxLength);}


#endif
