/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SdpTypedTime.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef TSDPTYPEDTIME_H
#define TSDPTYPEDTIME_H

//  INCLUDES
#include <e32std.h>
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class implements the typed time element of the Session
 *	Description Protocol.
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class TSdpTypedTime
	{
    public: // enums

        /** Time units. */
        enum TUnit
            {
            ETimeUnitDays,      /** d */
            ETimeUnitHours,     /** h */
		    ETimeUnitMinutes,   /** m */
		    ETimeUnitSeconds,   /** s */
		    ETimeUnitNone,
            };

    public: // Constructors and destructor
		/**
		 *	Constructs a typed time equal to zero and with no time unit set.
		 */
		IMPORT_C TSdpTypedTime();

		/**
		 *	Constructs a typed time equal to specified value and time unit.
		 *
		 *	@param aValue Time value.
		 *	@param aUnit Time unit.
		 */
		IMPORT_C TSdpTypedTime(TInt64 aValue, TUnit aUnit);

    public: // New functions
		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param aObj Another instance to compare with.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == (const TSdpTypedTime& aObj) const;

		/**
		 *	Sets the typed time value.
		 *
		 *	@param aValue Time value.
		 *	@param aUnit Time unit.
		 */
		IMPORT_C void SetValue(TInt64 aValue, TUnit aUnit);

    public:
		
		/**
         *  Externalizes the object to stream
         *
         *  @param aStream Stream where the object's state will be stored
         */
		void ExternalizeL(RWriteStream& aStream) const;
		
		/**
         *  Creates object from the stream data
         *
         *  @param aStream Stream where the object's state will be read
         *  @return Initialized object
         */
		static TSdpTypedTime InternalizeL(RReadStream& aStream);
        
        /**
		 *	Output the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream	Stream used for output.
		 */
        void EncodeL(RWriteStream& aStream) const;
        
        /**
         *	Construct a new origin field.
		 *		 
         *	@param aText A string containing a correctly formatted field value
		 *               terminated by a CRLF.
         *	@return The new instance.
         */
        static TSdpTypedTime DecodeL(const TDesC8& aText);

    public: // Data
    
        /**
        * @internalComponent
        */
		TInt64 iValue;
		
		/**
		* @internalComponent
		*/
		TInt iUnit;

	};

#endif // TSDPTYPEDTIME_H
