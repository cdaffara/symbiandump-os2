/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides formatting (grouping) for plain phone numbers
*
*
*/


#ifndef C_NUMBER_GROUPING_H
#define C_NUMBER_GROUPING_H

// #define __TEST_AS_EXE__ // put this in if you want all the test code to run and swap the mmps...

#include "NumberGroupingStd.h"

class TResourceReader;
class CRegularExpression;
class CPNGNumberGroupingExtension;

/**
* Engine class to format plain phone numbers by inserting grouping separators.
*  Both unformatted and formatted buffers are maintained by this class.
*  A reversed formatted buffer is also available to assist the client in performing wrapping.
*
* @lib NumberGrouping.lib
*/
NONSHARABLE_CLASS(CPNGNumberGrouping) : public CBase
{

/** 
* Central Repository key values for KNumberGrouping key in NumberGroupingCRKeys.h
*/
public:
    enum TNumberGroupingCRValues
        {
        /** Number grouping disabled */
        ENumberGroupingDisabled = 0,
        /** Number grouping enabled (USA) */
        ENumberGroupingEnabled = 1
        };

public:
	IMPORT_C static CPNGNumberGrouping*	NewL(TInt aMaxLength = 0, TBool aReversed = EFalse);
	IMPORT_C static CPNGNumberGrouping*	NewLC(TInt aMaxLength = 0, TBool aReversed = EFalse);
	IMPORT_C ~CPNGNumberGrouping();

	IMPORT_C TInt	Insert(TInt aIndex, TText aChar);
	IMPORT_C TInt	Delete(TInt aIndex);
	IMPORT_C TInt	Append(TText aChar);

    /**
    * Sets the new unformatted phone number.
    *
    * Formatting does not actually occur until an API is called that accesses in some way the formatted buffer
    * or one of its characteristics
    *
    * @param    aNumber Ungrouped phone number to be copied into the NumberGrouping engine's unformatted buffer
    * @return   KErrOverflow if the number is too long for the length of the unformatted buffer
    */
    IMPORT_C TInt	Set(const TDesC& aNumber);

    /**
    * @return   Length of the currently formatted (grouped) buffer
    */
    IMPORT_C TInt   Length() const;

    /**
    * @return   Length of the currently unformatted buffer
    */
    IMPORT_C TInt   UnFormattedLength() const;

	/**
    * This returns the maximum size of the unformatted buffer. This is the value that was provided during
    * construction.
    *
    * Descriptors provided to Set() must be shorter than this length.
    *
    * @return   maximum length of the unformatted buffer.
    */
    IMPORT_C TInt			MaxDisplayLength() const;

    /**
    * Routine to determine if the character at a given position in the formatted phone number
    * is a space character derived from the number grouping. That is, not part of the supplied phone number
    * proper, but a space character from the number grouping.
    *
    * A client can check the descriptor returned by FormattedNumber() directly to perform a simple test
    * of whether the character is a Space or not (whether or not derived from number grouping formatting)
    *
    * Note also that this returns EFalse if the character is some other formatting character besides
    * space.  To determine that, use IsCharacterInsertedByNumberGrouping().
    *
    * @param    aPos    The index of the character of interest
    * @return   EFalse  iff the characer at aPos in the formatted buffer is a space coming from grouping
    */
	IMPORT_C TBool			IsSpace(TInt aPos) const;

	/**
    * Access to section of the formatted buffer.
    * This routine returns a descriptor for the indicated range in the formatted (grouped) internal
    * buffer.  If there are spaces at either end of the indicated section of the formatted buffer, then
    * the returned descriptor is adjusted to point to the trimmed buffer in order to avoid the spaces.
    *
    * @param    aFrom   Inclusive starting index
    * @param    aTo     Inclusive end index
    * @return   reference to const non-modifiable descriptor for the indicated, trimmed text
    */
    IMPORT_C const TDesC&	FormattedNumber(TInt aFrom, TInt aTo) const;

    IMPORT_C const TDesC&	FormattedNumber() const;

    /**
    * Access to part of the reverse formatted number. If there are spaces at either end of the indicated
    * section of the formatted buffer, then the returned descriptor is adjusted to point to the trimmed
    * buffer in order to avoid the spaces.
    *
    * Returns KNullDesC if the feature has not been enabled by passing ETrue to the
    * parameter aReversed during construction
    *
    * @param    aFrom   lower (inclusive) limit of the text to look at
    * @param    aTo     upper (inclusive) limit of the text to look at
    * @return   Reference to descriptor containing the selected text
    */
	IMPORT_C const TDesC&	ReverseFormattedNumber(TInt aFrom, TInt aTo) const;

    /**
    * Access to the reverse formatted number
    *
    * Returns KNullDesC if the feature has not been enabled by passing ETrue to the
    * parameter aReversed during construction
    *
    * @return   Reference to descriptor containing the reverse formatted text
    */
	IMPORT_C const TDesC&	ReverseFormattedNumber() const;
	IMPORT_C const TDesC&	Selection(TInt aFrom, TInt aTo) const;

	IMPORT_C const TDesC&	UnFormattedNumber(TInt aFrom, TInt aTo) const;
	IMPORT_C const TDesC&	UnFormattedNumber() const;

    /**
    * This method allows the client to determine if the indexed character is a number grouping supplied
    * character. Specifically, this means that this character originates in the number grouping formatting
    * and not from the supplied unformatted phone number.
    *
    * Where the number has not been grouped (e.g. because there is an invalid phone number character in the
    * supplied descriptor), this method returns EFalse, even if the character pointed to may be of the nature
    * of a number grouping character. Use IsChangedByGrouping() to see if the number has been changed by
    * grouping at all.
    *
    * Where a client is interested purely in the nature of the characters rather than whether they come from
    * grouping or not, he may examine the examine the text via the descriptor reference returned by
    * FormattedNumber().
    *
    * @since    Series 60 2.6
    * @param    aPos    The index provided is for the formatted number.
    * @return   EFalse  iff the character at the supplied index is part of the supplied phone number
    */
    IMPORT_C TBool IsCharacterInsertedByNumberGrouping(TInt aPos) const;

    /**
    * Method to determine if the current number has been changed by number grouping.
    * If this returns EFalse, then FormattedNumber() and UnFormattedNumber() refer to descriptors of with identical
    * content.
    * If this method returns ETrue, then the descriptors that would be returned by the two accessor APIs refer
    * to descriptors with different content.
    *
    * @since    Series 60 2.6
    * @return   ETrue iff formatting of the number has made an effective difference.
    */
    IMPORT_C TBool IsChangedByGrouping() const;
    
    /**
    * @return   return iLanguage.
    */
    inline TLanguage  Language() const;


public:

	TLanguage	iForceLanguage;

private: // private classes and enums

    class TPNGSeparator
        {
        public:
        TPNGSeparator();
        TPNGSeparator( TInt aPos, TText aSeparatorCharacter );
        public:
        TInt iPosition;
        TText iSeparatorCharacter;
        };

	class TPNGGroupingInfo
    	{
        public:
		TPNGGroupingInfo();
	    public:
		TInt			        iMinNumberOfDigits;
		TInt			        iMaxNumberOfDigits;
        RArray<TPNGSeparator>   iAfterPositions; // Positions of separators "after" the beginning
		TPNGSeparator			iBeforePosition; // Positions of separators "before" the end
	    };

    // Constant for no pattern in use:
    enum
        {
        ENoMatchedPattern = -1
        };

private: // Constructors
    CPNGNumberGrouping( TInt aMaxLength, TBool aReserved);
	void ConstructL();

private:
	TLanguage doReadLanguageFromSharedData() const;

	void	doReadFormatInfoFromResourceFileL();
	void	doClearGroupingItemsList();
	void	doClearFormattedNumbers();

	void	doNumberGroupingL() const;
	void	doNumberSquashing() const;

    /**
    * Read and process a single NUMBER_GROUPING_ITEM resource structure
    */
    void ReadGroupingSchemeL(
        TResourceReader& aResourceReader,
        RPointerArray<TDesC>& aGroupingPatternsList,
        TInt& aMaxExtraCharacters );

    /**
    * Read and skip a single NUMBER_GROUPING_ITEM resource structure
    */
    void SkipGroupingSchemeL( TResourceReader& aResourceReader ) const;

    /**
    * Process the format pattern for "after positions"
    */
    void ParseForAfterPositions(
        const TDesC& aFormatPattern,
        TPNGGroupingInfo* aGroupingInfo,
        const TDesC& aWildcardedMatchingPattern,
        TInt& aMaxExtraCharacters,
        TBool& trailingPossible ) const;

    /**
    * Process the format pattern for a before positions
    */
    void ParseForBeforePosition(
        const TDesC& aFormatPattern,
        CPNGNumberGrouping::TPNGGroupingInfo* aGroupingInfo,
        TInt& aMaxExtraCharacters
        ) const;

   /**
    * This routine is used to find a wildcarded version of the matching pattern
    * provided in the "initialDigits" element for a grouping scheme read from resource.
    * It uses the services of the CRegularExpression class, an instance of which is constructed
    * with the provided aMatchString pattern only.
    *
    * The client must supply a modifiable descriptor long enough to hold the wildcarded version
    * of the pattern.
    *
    * For each character index, if there is only one possible valid character, this puts in that
    * character.  If there are more than one, then the supplied aWildcardChar is inserted. The
    * initialDigits element uses a full stop as a numeric wildcard; this is replaced with the nominated
    * wildcard.
    *
    * Rules: (where 'n, has been passed is used as the wildcard)
    *   "<a numeric digit>" -> "<a numeric digit>"
    *   ( e.g. "1"         ->  "1" )
    *   "+"         ->  "+"
    *   "."         ->  "n"
    *   "[0-3]"     ->  "n"
    *
    * @param    aMatchString        Regular expression to provide example of
    * @param    aWildcardChar       The character to put in the example pattern if there is no single
    *                               valid character at that point
    * @param    aWildcardMatchString     Descriptor to write the wildcarded match pattern into
    */
	void    GetWildcardVersionOfMatchStringL(
        const TDesC& aMatchString,
        TText aWildcard,
        TDes& aWildcardMatchString ) const;

    /**
    * This method expresses the policy of what characters may form part of a phone number
    * Note that this method is valid even it there is no formatting going on.  It is an intrinsic
    * test on the character itself
    */
    TBool   IsValidPhoneNumberCharacter(TText aCharacter) const;

    /**
    * Examines the unformatted number, counting how many digits are found before non-digit
    * characters or the end is encountered.
    * Returns 0 if Set() has not been called.
    */
    TInt LengthToGroup() const;

    /**
    * Perform number grouping using pattern at given index.  Grouping is only applied to the leading
    * aLengthToGroup characters.
    */
    void doNumberGroupingForPatternL( TInt aMatchedPattern, TInt aLengthToGroup ) const;

private: // private data

	HBufC*							iUnformattedNumber;
	mutable TPtrC					iUnformattedNumberPtr;

	mutable HBufC*					iFormattedNumber;
	mutable TPtrC					iFormattedNumberPtr;

	mutable HBufC*					iReverseFormattedNumber;
	mutable TPtrC					iReverseFormattedNumberPtr;

	mutable TPtrC					iSelectionPtr;

	mutable TLanguage				iLanguage;				// the system language
	TInt							iMaxUnformattedLength;
	TBool							iReversed;

	CRegularExpression*				iRegExp;				// the patterns for matching
	RPointerArray<TPNGGroupingInfo>	iGroupingItemsList;		// the formatting info

    mutable TInt                    iMatchedPatternIndex;   // keep track of what pattern is matched
    CPNGNumberGroupingExtension*    iExtension;
};

inline TLanguage CPNGNumberGrouping::Language() const
    {
    return iLanguage;
    }


#endif // C_NUMBER_GROUPING_H

// End of File
