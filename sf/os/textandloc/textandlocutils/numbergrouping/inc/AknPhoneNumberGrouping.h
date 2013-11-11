/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*    This class gives feature variant - independent access to Series 60 number grouping 
*    and provides digit type conversion.
*
*
*/


#ifndef AKNPHONENUMBERGROUPING_H
#define AKNPHONENUMBERGROUPING_H

// INCLUDES

#include <e32std.h>
#include <gdi.h>

// CONSTANTS


// CLASS DECLARATION

class CPNGNumberGrouping; 


/**
* This class acts as a wrapper for the Number Grouping engine (NumberGrouping.dll). 
* It exposes non-static API to the engine so that it can be used without the client code dealing with the
* issues of
* - build switches for calling numbergrouping API
* - build switches for linking to NumberGrouping.lib
* - checks on Feature Manager.
*
* It deals with these internally.  In the case of __SERIES60_PHONE_NUMBER_GROUPING being #undef-ed, this 
* object acts as if there is a "null" phone number grouping.
* 
* Static methods are included for those who are doing a small number of individual groupings of phone numbers, 
* say, when a view or popup is constructed, or a title pane is refreshed. These construct the object 
* internally within the static call.
* 
* The class also deals with the conversion of numbers to potentially locale or setting-
* specific number symbols, using the Series 60 platform policy.
* 
* For repeated, frequent operations, the non-static API is recommended.  Only a limited number of methods 
* from NumberGrouping.h are exposed, but these suffice in the main use cases.  
*
*  @lib Avkon.dll
*  @since 2.6
*/
NONSHARABLE_CLASS(CAknPhoneNumberGrouping) : public CBase
    {
    public:
        // 2 stage constructors
        IMPORT_C static CAknPhoneNumberGrouping* NewLC(TInt aUnformattedLength);
        IMPORT_C static CAknPhoneNumberGrouping* NewL(TInt aUnformattedLength);

        // Destructor
        ~CAknPhoneNumberGrouping();

    public: // non-static methods for using phone number grouping engine in the simplest cases

        /**
        * Load an ungrouped number into the phone number grouping engine
        * This calls the CPNGNumberGrouping::Set within the engine
        *
        * @since 2.6
        * @param    aUngroupedNumber    input ungrouped phone number
        * @return                       KErrNone if there was no error                   
        */
        IMPORT_C TInt Set( const TDesC& aUngroupedNumber );

        /**
        * Returns a reference to the formatted number stored within the number grouping engine
        *
        * NOTE: This api does not convert to current digit type. This requires a method that takes
        * a modifiable descriptor, so use CopyFormattedNumber( TDes& ) const
        *
        * This method is particularly useful for obtaining the size of the required buffer to copy 
        * the grouped phone number to by calling FormattedNumber.Length()
        *
        * @since 2.6
        * @return   reference to internally held formatted (grouped) phone number
        */
	    IMPORT_C const TDesC& FormattedNumber() const;

        /**
        * Copies the grouped phone number stored in the object to the supplied output buffer, 
        * performing any current digit type conversion required.
        *
        * The length required for the supplied output descriptor must be greater or equal to the text waiting
        * to be copied out of the instance.  This can be determined before the call with no loss of 
        * performance with FormattedNumber().Length();
        *       
        * The application will be Panicked (USER 11) if the output descriptor is too short.
        *
        * @since 2.6
        * @param    aGroupedNumber      Output (grouped) descriptor to be written to.  To avoid a panic, this
        *                               must be at least as big as the formatted number currently stored.
        */
	    IMPORT_C void CopyFormattedNumber( TDes& aGroupedNumber ) const;

    public: // Static utility methods for doing the one-off grouping of phone numbers

        /**
        * Static utility method for doing one-off grouping of a phone number according to the current
        * configuration.
        * 
        * The method Leaves if the number grouping engine could not be allocated, or the output heap memory
        * could not be allocated.
        *
        * @since 2.6
        * @param    aUngroupedNumber    Input descriptor to be grouped
        * @return                       Pointer to heap descriptor containing the grouped number
        *                               Ownership is transferred to the caller
        */
        IMPORT_C static HBufC* CreateGroupedPhoneNumberL( const TDesC& aUngroupedNumber );

        /**
        * Static utility method for doing one-off grouping of a phone number according to the current
        * configuration.
        *
        * The operations are internally trapped so as not to leave. The grouped number must fit into output
        * descriptor, or else no grouping will take place and the input will just be copied to the output.
        * A negative return value will indicate that the output did not fit, and by how much.
        * 
        * The application will be Panicked (USER 11) if the output descriptor is too short even for an exact
        * copy of the input.
        *
        * @since 2.6
        * @param    aUngroupedNumber    Input descriptor to be grouped
        * @param    aGroupedNumber      Output (grouped) descriptor to be written to
        * @return                       If the entire grouped number could be copied to the output, then the
        *                               returned number is zero or positive. This number represents the number
        *                               of further characters in the grouped number that could have been accomodated.
        *                               A negative return value indicates that the grouped number could not be
        *                               copied, and that the ungrouped number had to be copied. The magnitude
        *                               of the return value in this case is how many more characters would have
        *                               been needed.
        */
        IMPORT_C static TInt GroupPhoneNumber( const TDesC& aUngroupedNumber, TDes& aGroupedNumber );

        /**
        * Static utility method for doing one-off grouping of a phone number according to the current
        * configuration.
        * 
        * The operations are internally trapped so as not to leave. The grouped number must fit into the one, 
        * input/output descriptor, or else no grouping will take place. A negative return value indicates that the
        * grouped number did not fit. 
        *
        * @since 2.6
        * @param    aUngroupedNumber    Input descriptor to be grouped
        * @return                       If the entire grouped number could be copied to the output, then the
        *                               returned number is zero or positive. This number represents the number
        *                               of further characters in the grouped number that could have been accomodated.
        *                               A negative return value indicates that the grouped number could not be
        *                               copied, and that the ungrouped number had to be copied. The magnitude
        *                               of the return value in this case is how many more characters would have
        *                               been needed.
        */
        IMPORT_C static TInt GroupPhoneNumber( TDes& aPhoneNumber );

    private:
        /**
        * C++ constructor.
        */
        CAknPhoneNumberGrouping();

        /**
        * 2nd phase constructor is private.
        */
        void ConstructL( TInt aUnformattedLength );

        /**
        * Internal method for creating a phone number grouping engine and copying out the result to an 
        * output buffer. 
        *
        * The creation of the engine may leave, in which case nothing else happens. But if it does not
        * leave, an attempt will be made to copy the grouped phone number to aGroupedPhoneNumber.  
        * If there is enough space, it is copied and the amount of space in characters left over in 
        * the target buffer is returned.  If it does not fit, then there is no copy made and the 
        * aExtraSpace contains a negative value, the additional length required.
        * 
        * @param    aUngroupedNumber    Input descriptor containing a phone number        
        * @param    aGroupedNumber      Output descriptor for the grouped phone number
        * @param    aExtraSpace         How much extra space there was in the output descriptor
        * @return   EFalse iff the grouped phone number was not copied.
        */
        static TBool DoFormatPhoneNumberL( 
            const TDesC& aUngroupedNumber,
            TDes& aGroupedNumber,
            TInt& aExtraSpace);
 
    private:
        // Owned number grouping engine
        CPNGNumberGrouping* iNumberGroupingEngine;
        };

#endif

// End of File
