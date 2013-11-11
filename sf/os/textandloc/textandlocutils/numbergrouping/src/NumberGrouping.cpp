/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#include <f32file.h>
#include <barsc.h> 
#include "NumberGrouping.h"
#include "RegularExpression.h"
#include "cleanuputil.h"

#include <barsread.h>
//#include <eikenv.h>
#include <centralrepository.h>
#include <NumberGroupingCRKeys.h>

const TText KNumberGroupingWildcard('n');
const TText KNumberGroupingOneOrMoreCharactersToken('~');

// This constant represents all the valid whitespace that we can handle. Several
// APIs give whitespace special significance as a formatting character, especially
// those methods used to obtain text for formatting into fixed-width UI elements
// where spaces are not to be rendered at the margins.
const TText KNumberGroupingSpace(' ');

const TInt KMinimumLengthToGroup = 1; // No grouping occurs if fewer than this in unformatted buffer

#include <numbergrouping.rsg>


GLDEF_C void Panic(TNumberGroupingPanic aPanic)
    {
    _LIT(KPanicCat,"Number Grouping");
    User::Panic(KPanicCat, aPanic);
    }

// Valid phone number characters apart from IsDigit() characters are listed here
const TText KAdditionalPhoneNumberCharacters[] = {'+','#','*','p','w'};




NONSHARABLE_CLASS(CPNGNumberGroupingExtension): public CBase
	{
public:
	CPNGNumberGroupingExtension();
	~CPNGNumberGroupingExtension();
public:
    TInt        iMaxExtraCharacters; 
    TInt        iNumberGroupingCRValue;
    };

CPNGNumberGroupingExtension::CPNGNumberGroupingExtension()
	{
	}

CPNGNumberGroupingExtension::~CPNGNumberGroupingExtension()
	{
	}



CPNGNumberGrouping::TPNGSeparator::TPNGSeparator()
    : iPosition(-1), iSeparatorCharacter(KNumberGroupingSpace)
    {
    }

CPNGNumberGrouping::TPNGSeparator::TPNGSeparator( TInt aPosition, TText aSeparatorCharacter )
    : iPosition(aPosition), iSeparatorCharacter(aSeparatorCharacter)
    {
    }

CPNGNumberGrouping::TPNGGroupingInfo::TPNGGroupingInfo()
    {
    }

// CPNGNumberGrouping - grouping engine class
CPNGNumberGrouping::CPNGNumberGrouping(TInt aMaxLength /* = 0 */, TBool aReversed /* = EFalse */) :
                                            iForceLanguage(ELangTest),
                                            iUnformattedNumberPtr(KNullDesC),
                                            iFormattedNumberPtr(KNullDesC),
                                            iReverseFormattedNumberPtr(KNullDesC),
                                            iSelectionPtr(KNullDesC),
                                            iLanguage(ELangTest),
                                            iMaxUnformattedLength(aMaxLength),
                                            iReversed(aReversed),
                                            iMatchedPatternIndex(ENoMatchedPattern)
    {
    }

EXPORT_C CPNGNumberGrouping* CPNGNumberGrouping::NewL( TInt aMaxLength, TBool aReserved)
    {
    CPNGNumberGrouping* s = NewLC(aMaxLength, aReserved);
    CleanupStack::Pop();
    return s;
    }

EXPORT_C CPNGNumberGrouping* CPNGNumberGrouping::NewLC( TInt aMaxLength, TBool aReserved)
    {
    CPNGNumberGrouping* s = new(ELeave)CPNGNumberGrouping( aMaxLength, aReserved);
    CleanupStack::PushL(s);
    s->ConstructL();
    return s;
    }

void CPNGNumberGrouping::ConstructL()
    {
    iExtension = new (ELeave) CPNGNumberGroupingExtension(); 
    CRepository* repository = NULL;
    iExtension->iNumberGroupingCRValue = 0;
    TRAPD(ret, repository = CRepository::NewL(KCRUidNumberGrouping));
    if (ret == KErrNone)
        {
        ret = repository->Get(KNumberGrouping, iExtension->iNumberGroupingCRValue);
        }
    delete repository;
    
    // Read from resource first in order to obtain a value for iExtension->iMaxExtraCharacters
    iLanguage = doReadLanguageFromSharedData();
    doReadFormatInfoFromResourceFileL(); // This sets iExtension->iMaxExtraCharacters

    // Allocation of buffers.  Note that iMaxUnformattedLength must be retained as member
    // data because HBufCs may come back with more storage available than asked for.
    // The uncertainty in the actual length provided by the HBufCs means that although
    // they are asked to be different by iExtension->iMaxExtraCharacters, the difference between the
    // actual MaxLengths may be less than iMaxExtraCharcaters.
    // The approach decided upon is to retain iMaxUnformattedLength as member data and
    // use IT everywhere throughout this class's implementation, NEVER using
    // iUnformattedNumber->Des().MaxLength()
    iUnformattedNumber = HBufC::NewL(iMaxUnformattedLength);
    iFormattedNumber = HBufC::NewL(iMaxUnformattedLength + iExtension->iMaxExtraCharacters);

    // Create revesed buffer only if requested
    if ( iReversed )
        iReverseFormattedNumber = HBufC::NewL(iMaxUnformattedLength + iExtension->iMaxExtraCharacters);        
    }

EXPORT_C CPNGNumberGrouping::~CPNGNumberGrouping()
    {
    doClearGroupingItemsList();

    delete iUnformattedNumber;
    delete iFormattedNumber;
    delete iReverseFormattedNumber;
    delete iRegExp;
    delete iExtension;
    }

EXPORT_C TInt CPNGNumberGrouping::Insert(TInt aIndex, TText aChar)
    {

    if( aIndex >= 0 && aIndex <= iUnformattedNumber->Length())
        {

        if(iUnformattedNumber->Length() >= iMaxUnformattedLength)
            return KErrOverflow;

        TBuf<1> bufChar(1);
        bufChar[0] = aChar;
        TPtr ptrModifyable(iUnformattedNumber->Des());
        ptrModifyable.Insert(aIndex, bufChar);

        doClearFormattedNumbers();

        return KErrNone;
        }

    return KErrIndexOutOfRange;
    }

EXPORT_C TInt CPNGNumberGrouping::Delete(TInt aIndex)
    {
    if(aIndex >= 0 && aIndex < iUnformattedNumber->Length())
        {
        TPtr ptrModifyable(iUnformattedNumber->Des());
        ptrModifyable.Delete(aIndex, KSingleCharacter);

        doClearFormattedNumbers();

        return KErrNone;
        }

    return KErrIndexOutOfRange;
    }

EXPORT_C TInt CPNGNumberGrouping::Append(TText aChar)
    {
    if(iUnformattedNumber->Length() >= iMaxUnformattedLength)
        return KErrOverflow;

    TBuf<1> bufChar(1);
    bufChar[0] = aChar;
    TPtr ptrModifyable(iUnformattedNumber->Des());
    ptrModifyable.Append(bufChar);

    doClearFormattedNumbers();

    return KErrNone;
    }

EXPORT_C TInt CPNGNumberGrouping::Set(const TDesC& aNumber)
    {
    if( aNumber.Length() > iMaxUnformattedLength )
        return KErrOverflow;

    TPtr ptrModifyable( iUnformattedNumber->Des() );
    ptrModifyable.Copy( aNumber );

    doClearFormattedNumbers();

    return KErrNone;
    }

EXPORT_C TInt CPNGNumberGrouping::Length() const
    {
    if(!iFormattedNumber->Length()) // This test is used as the trigger to reformat
        FormattedNumber();

    return iFormattedNumber->Length();
    }

EXPORT_C TInt CPNGNumberGrouping::UnFormattedLength() const
    {
    return iUnformattedNumber->Length();
    }

EXPORT_C TInt CPNGNumberGrouping::MaxDisplayLength() const
    {
    // Despite its name, this method returns the max length of the UNFORMATTED buffer
    // This must not be implemented to return the actual length available in
    // iUnformattedBuffer.
    return iMaxUnformattedLength;
    }

EXPORT_C TBool CPNGNumberGrouping::IsSpace(TInt aPos) const
    {
    // Very tricky semantics for this.  Must be a space inserted by the formatting
    if ( iFormattedNumber->Length() > aPos &&
         iFormattedNumber->operator[](aPos) == KNumberGroupingSpace &&
         // Check also that is is less than the length to group + inserted characters
         aPos < ( LengthToGroup() + (iFormattedNumber->Length() - iUnformattedNumber->Length()) ) )
        return ETrue;
    else
        return EFalse;
    }

EXPORT_C const TDesC& CPNGNumberGrouping::FormattedNumber(TInt aFrom, TInt aTo) const
    {
    if(iUnformattedNumber->Length() != 0 &&
        aFrom >= 0 &&
        aFrom <= aTo )
        {
        FormattedNumber();
        iFormattedNumberPtr.Set(KNullDesC);

        TInt length = iFormattedNumber->Length();
        if(aTo < length + 1)
            {
            TInt length = iFormattedNumber->Length();
            if ( iExtension->iNumberGroupingCRValue )
            	{
				// Advance to the next non-space
				while( (aFrom < length ) && (*iFormattedNumber)[aFrom] == KNumberGroupingSpace )
					{
					++aFrom;
					}
	
				// Retreat to the last non-space
				while( (aTo > 0) && (*iFormattedNumber)[aTo] == KNumberGroupingSpace )
					{
					--aTo;
					}
            	}

            // Does fetching the descriptor still make sense?
            if ( (0 <= aFrom) && (aFrom <= aTo) && (aTo < length) )
                iFormattedNumberPtr.Set( iFormattedNumber->Mid( aFrom, aTo-aFrom+1 ) );
            }
        }
    else
        {
        if(iFormattedNumber->Length())
            {
            CPNGNumberGrouping* pThis = const_cast<CPNGNumberGrouping*>(this);
            pThis->doClearFormattedNumbers();
            }

        iFormattedNumberPtr.Set(KNullDesC);
        }

    return iFormattedNumberPtr;
    }

EXPORT_C const TDesC& CPNGNumberGrouping::FormattedNumber() const
    {
    if( !iFormattedNumber->Length() )
        {
        TInt err = KErrNone;

        CPNGNumberGrouping* pThis = const_cast<CPNGNumberGrouping*>(this);

        if( LengthToGroup() < KMinimumLengthToGroup || !iExtension->iNumberGroupingCRValue )
            {
            // This is now just a short cut, as doNumberGroupingL handles premature truncation of
            // formatting. But this avoids all the language checking
            doNumberSquashing();  // copies the unformatted number straight into the formatted number
            }
        else
            {
            TLanguage eLanguage;
            if(iForceLanguage != ELangTest)
                eLanguage = iForceLanguage;
            else
                eLanguage = doReadLanguageFromSharedData();

            if(eLanguage != iLanguage)
                {
                iLanguage = eLanguage;

                TRAP(err, pThis->doReadFormatInfoFromResourceFileL());
                if(err != KErrNone)
                    {
                    iFormattedNumberPtr.Set(KNullDesC);
                    return iFormattedNumberPtr;
                    }
                }

            TRAP(err, doNumberGroupingL());
            }

        if(err != KErrNone)
            pThis->doClearFormattedNumbers();
        else
            iFormattedNumberPtr.Set(iFormattedNumber->Ptr(), iFormattedNumber->Length());
        }

    return iFormattedNumberPtr;
    }

EXPORT_C const TDesC& CPNGNumberGrouping::ReverseFormattedNumber(TInt aFrom, TInt aTo) const
    {
    if ( iReversed )
        {
        if(iUnformattedNumber->Length() != 0 &&
            aFrom >= 0 &&
            aFrom <= aTo)
            {
            ReverseFormattedNumber();

            iReverseFormattedNumberPtr.Set(KNullDesC);

            TInt length = iReverseFormattedNumber->Length();
            if( aTo < length + 1 )
                {
                // Advance to the next non-space
                if( iExtension->iNumberGroupingCRValue )
                	{
					while( (aFrom < length ) && (*iReverseFormattedNumber)[aFrom] == KNumberGroupingSpace )
						{
						++aFrom;
						}
	
					// Retreat to the last non-space
					while( (aTo > 0) && (*iReverseFormattedNumber)[aTo] == KNumberGroupingSpace )
						{
						--aTo;
						}
                	}

                // Does fetching the descriptor still make sense?
                if ( (0 <= aFrom) && (aFrom <= aTo) && (aTo < length) )
                    iReverseFormattedNumberPtr.Set(
                        iReverseFormattedNumber->Mid( aFrom, aTo-aFrom+1) );
                }
            }
        else
            iReverseFormattedNumberPtr.Set(KNullDesC);
        }

    return iReverseFormattedNumberPtr; // Zero initialized at construction
    }

EXPORT_C const TDesC& CPNGNumberGrouping::ReverseFormattedNumber() const
    {
    if( iReverseFormattedNumber && !iReverseFormattedNumber->Length())
        {
        if(!iFormattedNumber->Length())
            FormattedNumber();

        TInt nLength = iFormattedNumber->Length();

        TPtr ptrModifyable(iReverseFormattedNumber->Des());
        TBuf<1> bufChar(1);

        for(TInt i = nLength; i > 0; --i)
            {
            TText cChar = (*iFormattedNumber)[i-1];
            bufChar[0] = cChar;
            ptrModifyable.Insert(nLength - i, bufChar);
            }

        iReverseFormattedNumberPtr.Set(iReverseFormattedNumber->Ptr(), nLength);
        }

    return iReverseFormattedNumberPtr;
    }

EXPORT_C const TDesC& CPNGNumberGrouping::Selection(TInt aFrom, TInt aTo) const
    {
    if(aFrom < iUnformattedNumber->Length())
        {
        TPtr ptrUnformatted = iUnformattedNumber->Des();
        iSelectionPtr.Set(&(ptrUnformatted[aFrom]), aTo - aFrom);
        }
    else
        iSelectionPtr.Set(KNullDesC);

    return iSelectionPtr;
    }

EXPORT_C const TDesC&   CPNGNumberGrouping::UnFormattedNumber(TInt aFrom, TInt aTo) const
    {
    if (iUnformattedNumber && aFrom >= 0 && aFrom <= aTo && aTo < iUnformattedNumber->Length())
        {
        iUnformattedNumberPtr.Set(&((*iUnformattedNumber)[aFrom]), aTo - aFrom + 1);
        }
    else
        {
        iUnformattedNumberPtr.Set(KNullDesC);
        }
    return iUnformattedNumberPtr;
    }

EXPORT_C const TDesC& CPNGNumberGrouping::UnFormattedNumber() const
    {
    return UnFormattedNumber(0, iUnformattedNumber->Length() - 1);
    }

TLanguage CPNGNumberGrouping::doReadLanguageFromSharedData() const
    {    
    if (iExtension->iNumberGroupingCRValue)
        {
        return ELangAmerican;
        }
    else
        {
        return ELangTest;
        }
    }

void CPNGNumberGrouping::doClearFormattedNumbers()
    {
    TPtr ptrModifyable( iUnformattedNumber->Des() );

    for (TInt index = 0; index < ptrModifyable.Length(); index++)
        {
        TChar ch = TChar(ptrModifyable[index]);
        ch.Fold( TChar::EFoldDigits | TChar::EFoldSpaces);
        }
    
    iFormattedNumber->Des().Zero();
    iFormattedNumberPtr.Set(KNullDesC);

    if ( iReverseFormattedNumber )
        iReverseFormattedNumber->Des().Zero();

    iReverseFormattedNumberPtr.Set(KNullDesC);
    iMatchedPatternIndex = ENoMatchedPattern;
    }

void CPNGNumberGrouping::doReadFormatInfoFromResourceFileL()
    {
    doClearGroupingItemsList();
    delete iRegExp;
    iRegExp = NULL;

    RPointerArray<TDesC> parrGroupingPatternsList;
    CleanupResetAndDestroyPushL(parrGroupingPatternsList);

    TInt maxExtraCharacters(0);

    RFs fs;
    CleanupClosePushL(fs);
    if(fs.Connect() == KErrNone)
        {
        RResourceFile resourceFile;
        CleanupClosePushL(resourceFile);

        resourceFile.OpenL(fs, _L("z:\\resource\\numbergrouping.rsc"));
        HBufC8* bufResource = resourceFile.AllocReadL(R_GROUPING_MAPPING);

        TResourceReader resourceReader;
        resourceReader.SetBuffer(bufResource);

        TInt    nLanguageCount = resourceReader.ReadInt8();
        TBool   bLanguageMatches = EFalse;

        while(nLanguageCount-- || !bLanguageMatches)
            {
            TBool bLanguageMatches = (resourceReader.ReadInt8() == iLanguage);

            if(bLanguageMatches || ((nLanguageCount == -1) && !bLanguageMatches))
                {
                TInt nGroupingSchemeCount = resourceReader.ReadInt8();

                while(nGroupingSchemeCount--)
                    {
                    TInt thisMaxExtraCharacters(0);
                    ReadGroupingSchemeL(
                        resourceReader, parrGroupingPatternsList, thisMaxExtraCharacters );
                    // take this new max extra characters if bigger
                    maxExtraCharacters = Max( maxExtraCharacters, thisMaxExtraCharacters );
                    }

                break; // This breaks out because we take the first language that matches

                } // End of if on language/locale test
            else  // skip other locales
                {
                TInt nGroupingSchemeCount = resourceReader.ReadInt8();
                while(nGroupingSchemeCount--)
                    {
                    SkipGroupingSchemeL( resourceReader );
                    }
                }
            }

        delete bufResource;

        resourceFile.Close();
        CleanupStack::Pop();  // resource file
        }

    fs.Close();
    CleanupStack::Pop();  // file system

    iExtension->iMaxExtraCharacters = maxExtraCharacters; // Latch the high water mark of extra characters

    iRegExp = CRegularExpression::NewL(&parrGroupingPatternsList);

    CleanupStack::PopAndDestroy(&parrGroupingPatternsList);  // patterns list
    }

void CPNGNumberGrouping::doNumberGroupingL() const
    {
    TInt lengthToGroup = LengthToGroup();

    if ( lengthToGroup >= KMinimumLengthToGroup )
        {

        TInt matchedPattern = KErrNotFound;
        TInt newMatchedPattern = KErrNotFound;

        // Search for matches in the RegExp object. It returns the next matching pattern
        // However, even if there is a match, lengthToGroup may not be in the deployment
        // length range between minDigits and MaxDigits, inclusive
        do  {
            // Check for another matching pattern
            newMatchedPattern = iRegExp->SearchFrom( newMatchedPattern+1, *iUnformattedNumber);

            if( newMatchedPattern != KErrNotFound) // Found a match, but it is OK?
                {

                TInt minDigits = iGroupingItemsList[newMatchedPattern]->iMinNumberOfDigits;
                TInt maxDigits = iGroupingItemsList[newMatchedPattern]->iMaxNumberOfDigits;

                // Fill in sensible values for min and max if not present
                if(minDigits == -1)
                    minDigits = 0;
                if(maxDigits == -1)
                    maxDigits = lengthToGroup;

                if ( minDigits <= lengthToGroup && lengthToGroup <= maxDigits )
                    {
                    matchedPattern = newMatchedPattern; // accept this new pattern
                    break;
                    }
                }

            } while ( newMatchedPattern != KErrNotFound  );

        // Actually go and do the grouping
        if ( matchedPattern != KErrNotFound )
            {
            doNumberGroupingForPatternL( matchedPattern, lengthToGroup );
            return;
            }

        }

    // if we get to here, either the string was not matched to any of the patterns or the
    // unformatted string is exactly the display length.  In either case we call
    // doNumberSquashing() which simply leaves the string as it is...
    doNumberSquashing();

    }


void CPNGNumberGrouping::doNumberGroupingForPatternL( TInt aMatchingPattern, TInt aLengthToGroup ) const
    {
    iMatchedPatternIndex = aMatchingPattern;

    TInt nLowPos = 0;
    TInt nHighPos = 0;

    TPtr desUnformattedNumber = iUnformattedNumber->Des();
    TInt unformattedLength = iUnformattedNumber->Length();

    __ASSERT_ALWAYS( aLengthToGroup <= unformattedLength , Panic(ENumberGroupingBadLengthToGroup) );

    TPNGGroupingInfo* matchedPattern = iGroupingItemsList[iMatchedPatternIndex];
    TInt nAfterCount = matchedPattern->iAfterPositions.Count();
    TBool bBeforePosition = (matchedPattern->iBeforePosition.iPosition == -1)?0:1;

    // Test to see if the beforePosition can be used with the current text length.
    // The following does not allow the before position to be used if it would result in an
    // insertion right next to one from the AfterPositions.
    // That is, tildas in the formatting string represent 1 or more characters.
    // e.g. if the last afterPosition is 4 and the before position is 3, then a 7 digit
    // number will not be able to have the before position used.
    if( nAfterCount &&
        (unformattedLength - matchedPattern->iBeforePosition.iPosition) <=
        matchedPattern->iAfterPositions[nAfterCount - 1].iPosition)
        {
        bBeforePosition = EFalse;
        }

    TPtr ptrModifyable(iFormattedNumber->Des());

    for(TInt i  = 0; i < nAfterCount && nHighPos < aLengthToGroup ; ++i)
        {
        nHighPos = matchedPattern->iAfterPositions[i].iPosition;
        if ( nHighPos >= aLengthToGroup )
            break;

        if(nHighPos < unformattedLength)
            {
            ptrModifyable.Append( desUnformattedNumber.Mid( nLowPos, nHighPos - nLowPos) );
            ptrModifyable.Append(matchedPattern->iAfterPositions[i].iSeparatorCharacter);
            nLowPos = nHighPos;
            }
        }

    // Do not do "before end" formatting at all if there is any truncation
    if ( aLengthToGroup < unformattedLength )
        {
        TInt nBeforePosition = matchedPattern->iBeforePosition.iPosition;

        if(bBeforePosition && nBeforePosition < unformattedLength)
            {
            nHighPos = unformattedLength - nBeforePosition;
            ptrModifyable.Append( desUnformattedNumber.Mid( nLowPos, nHighPos - nLowPos) );
            ptrModifyable.Append( matchedPattern->iBeforePosition.iSeparatorCharacter );
            nLowPos = nHighPos;
            }
        }

    nHighPos = unformattedLength;
    ptrModifyable.Append( desUnformattedNumber.Mid( nLowPos, nHighPos - nLowPos) );

    }

void CPNGNumberGrouping::doNumberSquashing() const
    {
    __ASSERT_ALWAYS( !iFormattedNumber->Length(), Panic(ENumberGroupingFormattedNumberAlreadyExists) );

    // just copy from one t'other...
    TPtr ptrModifyable(iFormattedNumber->Des());
    ptrModifyable.Copy(*iUnformattedNumber);
    iMatchedPatternIndex = ENoMatchedPattern;
    }

void CPNGNumberGrouping::doClearGroupingItemsList()
    {
    TInt nCount = iGroupingItemsList.Count();

    for(TInt i = 0; i < nCount; ++i)
        {
        iGroupingItemsList[i]->iAfterPositions.Close();
        delete iGroupingItemsList[i];
        iGroupingItemsList[i] = NULL;
        }
    iGroupingItemsList.Close();
    }

void CPNGNumberGrouping::ReadGroupingSchemeL(
    TResourceReader& aResourceReader,
    RPointerArray<TDesC>& aGroupingPatternsList,
    TInt& aMaxExtraCharacters )
    {
    CleanupResetAndDestroyPushL(aGroupingPatternsList);
    TPNGGroupingInfo* groupingInfo = new (ELeave) TPNGGroupingInfo;
    CleanupStack::PushL( groupingInfo );

    // Read in all resource for this grouping scheme, perform checking and then analyze it
    HBufC* initialDigits = aResourceReader.ReadHBufCL();
    __ASSERT_ALWAYS( initialDigits, Panic( ENumberGroupingNoInitialDigitsInResource ) );
    CleanupStack::PushL( initialDigits );

    groupingInfo->iMinNumberOfDigits = aResourceReader.ReadInt8();
    groupingInfo->iMaxNumberOfDigits = aResourceReader.ReadInt8();
    __ASSERT_DEBUG(
        ( groupingInfo->iMaxNumberOfDigits == -1) ||
        ( groupingInfo->iMinNumberOfDigits <= groupingInfo->iMaxNumberOfDigits ),
        Panic( ENumberGroupingBadMinMaxDigitRangeInResource ) );

    // Read in formatting Pattern
    HBufC* formatPattern = aResourceReader.ReadHBufCL();

    if ( formatPattern ) // Does not have to be there
        {
        CleanupStack::PushL( formatPattern );
        TInt formatLength = formatPattern->Length();
        if ( formatLength > 0 )
            {
            // Obtain a wildcard version of the matching pattern in initialDigits.
            // This is used to check the supplied formatPattern for comformance to initialDigits
            HBufC* wildcardedMatchBuf = HBufC::NewLC( formatLength ); // Will not be longer than the search pattern

            TPtr wildcardedMatchPtr( wildcardedMatchBuf->Des() );
            // Get the example number using the latest search pattern only
            GetWildcardVersionOfMatchStringL( *initialDigits, KNumberGroupingWildcard, wildcardedMatchPtr );

            // Now parse the descriptor
            TBool trailingPossible(EFalse);
            ParseForAfterPositions(
                *formatPattern, groupingInfo, wildcardedMatchPtr, aMaxExtraCharacters, trailingPossible );

            // Now parse the descriptor from the end if needed
            if ( trailingPossible )
                ParseForBeforePosition( *formatPattern, groupingInfo, aMaxExtraCharacters );

            CleanupStack::PopAndDestroy( wildcardedMatchBuf );
            }
        CleanupStack::PopAndDestroy( formatPattern );
        } // End of if on formatPattern.Length

    User::LeaveIfError( aGroupingPatternsList.Append( initialDigits ) );
    CleanupStack::Pop( initialDigits );

    // Do not leave if the next one fails, but remove the last from the patterns list and then leave
    // This is done in case someone TRAPs. Otherwise neither of these lists would be used and their
    // mismatch would not be a problem
    if ( TInt err = iGroupingItemsList.Append(groupingInfo) != KErrNone )
        {
        // return value of Count will be at least 1, because we have just successfully gone through an Append
        aGroupingPatternsList.Remove( aGroupingPatternsList.Count() - 1 );
        // ownership is now mine again...
        delete initialDigits;
        // Need to delete groupingInfo, and make sure it is no longer on the cleanupstack
        CleanupStack::PopAndDestroy( groupingInfo );
        User::Leave(err);
        }
    else
        {
        CleanupStack::Pop( groupingInfo ); // Success. This object now not owned by the cleanupstack
        }
    
    CleanupStack::Pop(&aGroupingPatternsList);
    }

void CPNGNumberGrouping::ParseForAfterPositions(
    const TDesC& aFormatPattern,
    TPNGGroupingInfo* aGroupingInfo,
    const TDesC& aWildcardedMatchingPattern,
    TInt& aMaxExtraCharacters,
    TBool& trailingPossible ) const
    {
    TInt pos(0); // Keeps track of the position with which the next separator will be stored
    TInt formatLength = aFormatPattern.Length();
    for (TInt index = 0; index < formatLength; index++ )
        {
        // The format pattern is compared with the matching pattern.  The matching pattern may be
        // shorter than the format pattern, so by default a wildcard character is used.
        TText ch = aFormatPattern[index];
        TText matchingChar(KNumberGroupingWildcard); // default to expect is the wildcard character
        if ( pos < aWildcardedMatchingPattern.Length() ) // if still within the matching pattern
            matchingChar = aWildcardedMatchingPattern[pos];
        if ( ch == matchingChar )
            pos++; // not a separator. index where the next "after" marker goes
        else if ( ch == KNumberGroupingOneOrMoreCharactersToken )
            {
            // finish looking for "afterPositions". But there may be a "before" position in the
            // remainder, so set the flag
            trailingPossible = ETrue;
            break;
            }
        else
            {
            // Explicit prevention of any separator characters being valid phone numbers
#ifdef _DEBUG
            if ( IsValidPhoneNumberCharacter( ch ) || ch == KNumberGroupingWildcard )
                {
                RDebug::Print(
                    _L("NumberGrouping: Illegal character or format mismatch in resource: initialDigits pattern= <%S> formatPattern=<%S>"),
                    &aWildcardedMatchingPattern, &aFormatPattern );
                }
#endif
            __ASSERT_DEBUG( !IsValidPhoneNumberCharacter( ch ), Panic( ENumberGroupingInvalidSeparatorCharacterInFormat ) );
            __ASSERT_DEBUG( ch != KNumberGroupingWildcard, Panic( ENumberGroupingMatchingPatternVersusFormatPatternMismatch ) );
            TPNGSeparator separator( pos, aFormatPattern[index]);
            aGroupingInfo->iAfterPositions.Append(separator);
            aMaxExtraCharacters++;
            }
        }
    }

void CPNGNumberGrouping::ParseForBeforePosition(
    const TDesC& aFormatPattern,
    TPNGGroupingInfo* aGroupingInfo,
    TInt& aMaxExtraCharacters ) const
    {
    TInt pos=0;
    TInt formatLength = aFormatPattern.Length();

    for (TInt index = formatLength-1; index >=0; index-- )
        {
        TText ch = aFormatPattern[index];
        if ( ch == KNumberGroupingWildcard )
            pos++;
        else if ( ch == KNumberGroupingOneOrMoreCharactersToken )
            break;
        else
            {
            // Explicit prevention of any separator characters being valid phone numbers
#ifdef _DEBUG
            if ( IsValidPhoneNumberCharacter( ch ) )
                {
                RDebug::Print(
                    _L("NumberGrouping: Illegal character in trailing part of format string in resource: formatPattern=<%S>"),
                    &aFormatPattern );
                }
#endif
            __ASSERT_DEBUG( !IsValidPhoneNumberCharacter( ch ),
                Panic( ENumberGroupingInvalidSeparatorCharacterInFormat ) );
            TPNGSeparator separator( pos, ch );
            aGroupingInfo->iBeforePosition = separator;
            aMaxExtraCharacters++;
            break;
            }
        }
    }


void CPNGNumberGrouping::SkipGroupingSchemeL( TResourceReader& aResourceReader ) const
    {
    HBufC* tempBuf;
    tempBuf = aResourceReader.ReadHBufCL();
    delete tempBuf;
    aResourceReader.Advance(2); // min and max characters
    tempBuf = aResourceReader.ReadHBufCL();
    delete tempBuf;
    }

void CPNGNumberGrouping::GetWildcardVersionOfMatchStringL(
    const TDesC& aMatchString,
    TText aWildcard,
    TDes& aWildcardMatchString ) const
    {
    RPointerArray<TDesC> patternList;
    CleanupClosePushL(patternList);

    // Make a copy of the input string
    HBufC* matchString = aMatchString.AllocLC();

    User::LeaveIfError( patternList.Append(matchString) );// takes ownership
    CleanupStack::Pop( matchString );

    CRegularExpression* regExp = CRegularExpression::NewLC(&patternList);

    // Only 1 pattern fed in.  Access that pattern at index 0
    regExp->GetWildcardVersionOfPattern( 0 , aWildcard, aWildcardMatchString );

    CleanupStack::PopAndDestroy(regExp);

    // Delete the patterns list
    delete patternList[0];
    CleanupStack::PopAndDestroy();
    }


EXPORT_C TBool CPNGNumberGrouping::IsCharacterInsertedByNumberGrouping(TInt aPos) const
    {
    TInt insertedCharacters = Length() - UnFormattedLength();

    if( insertedCharacters == 0 ) // no formatting was done
        return EFalse;
    else if ( aPos < ( insertedCharacters + LengthToGroup() ) )
        {
        return !IsValidPhoneNumberCharacter( (*iFormattedNumber)[aPos] );
        }
    else // aPos is pointing at or beyond index= LengthToGroup() + <chars inserted>; no formatting there
        return EFalse;
    }


TBool CPNGNumberGrouping::IsValidPhoneNumberCharacter( TText aCharacter ) const
    {
    if ( ((TChar)aCharacter).IsDigit() )
        return ETrue;

    // Check through the list of additional valid phone number characters
    TInt numAdditionalChars = sizeof( KAdditionalPhoneNumberCharacters )/sizeof(TText);

    for (TInt index = 0; index < numAdditionalChars; index++)
        {
        if ( aCharacter == KAdditionalPhoneNumberCharacters[index] )
            return ETrue;
        }

    return EFalse;
    }

EXPORT_C TBool CPNGNumberGrouping::IsChangedByGrouping() const
    {
    // The only way that grouping is effectively different is by making things longer
    return ( Length() > UnFormattedLength() );
    }

TInt CPNGNumberGrouping::LengthToGroup() const
    {

    TPtrC ptr = iUnformattedNumber->Des();
    TInt lengthToGroup = ptr.Length();

    // Find the first non-digit
    for (TInt index = 0; index < ptr.Length(); index++)
        {
        TChar ch = TChar(ptr[index]);
        ch.Fold(TChar::EFoldDigits);
        if ( !( ch.IsDigit() ) )
            {
            lengthToGroup = index; // only characters BEFORE the character at index are grouped
            break;
            }
        }

    return lengthToGroup;
    }

// End of File
