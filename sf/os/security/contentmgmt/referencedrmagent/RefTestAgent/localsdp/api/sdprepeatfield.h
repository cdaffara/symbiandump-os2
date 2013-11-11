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
* Name          : SdpRepeatField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPREPEATFIELD_H
#define CSDPREPEATFIELD_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sdpdefs.h"
#include "sdptypedtime.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the repeat times field of the 
 *  Session Description Protocol.
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpRepeatField : public CBase
	{
    public: // Constructors and destructor
        /**
         *	Constructs a new repeat times field.
		 *	
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpRepeatField* DecodeL(const TDesC8& aText);

        /**
         *	Constructs a new repeat times field and adds the pointer to the 
         *  cleanup stack.
		 *	
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpRepeatField* DecodeLC(const TDesC8& aText);

        /**
         *	Constructs a new repeat times field.
		 *	
         *	@param aRepeatInterval Repeat interval value 
         *         greater than zero.
         *	@param aActiveDuration Active duration.
         *	@param aOffsets At least one time offset.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpRepeatField* NewL(
                                        const TSdpTypedTime  aRepeatInterval,
										const TSdpTypedTime  aActiveDuration,
										const RArray<TSdpTypedTime>& aOffsets);

        /**
         *	Constructs a new repeat times field and adds the pointer to the 
         *  cleanup stack.
		 *	
         *	@param aRepeatInterval Repeat interval value 
         *         greater than zero.
         *	@param aActiveDuration Active duration.
         *	@param aOffsets At least one time offset.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpRepeatField* NewLC(
                                        const TSdpTypedTime  aRepeatInterval,
										const TSdpTypedTime aActiveDuration,
										const RArray<TSdpTypedTime>& aOffsets);

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpRepeatField();

    public: // New functions
		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream Stream used for output. On return 
         *         the stream includes correctly formatted repeat field.
		 */
		IMPORT_C void EncodeL(RWriteStream& aStream) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpRepeatField * CloneL() const;

		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == (const CSdpRepeatField & aObj) const;

		/**
		 *	Gets the repeat interval.
		 *
		 *	@return The repeat interval.
		 */
		IMPORT_C const TSdpTypedTime RepeatInterval() const;

		/**
		 *	Sets the repeat interval.
		 *
         *	@param aValue Repeat interval value greater than zero.
         *	@leave KErrSdpCodecRepeatField if aValue is not positive integer 
         *         value.
		 */
		IMPORT_C void SetRepeatIntervalL(const TSdpTypedTime  aValue);

		/**
		 *	Gets the active duration..
		 *
		 *	@return The active duration.
		 */
		IMPORT_C const TSdpTypedTime ActiveDuration() const;

		/**
		 *	Sets the active duration.
		 *
         *	@param Active duration.
		 */
		IMPORT_C void SetActiveDuration(const TSdpTypedTime aValue);

		/**
		 *	Gets the array of time offsets.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	@return The offsets.
		 */
		IMPORT_C const RArray<TSdpTypedTime>& TimeOffsets() const;

		/**
		 *	Sets the time offsets.
		 *
         *	@param aOffsets At least one time offset.
		 */
		IMPORT_C void SetTimeOffsetsL(const RArray<TSdpTypedTime>& aValue);

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
		static CSdpRepeatField* InternalizeL(RReadStream& aStream);

	private:
		CSdpRepeatField(const TSdpTypedTime aRepeatInterval,
						const TSdpTypedTime aActiveDuration);
		CSdpRepeatField();
		void ConstructL(const TDesC8& aText);
		void ConstructL(const RArray<TSdpTypedTime>& aOffsets);
        

    private: // Data
        RStringPool iPool;
		TSdpTypedTime iRepeatInterval;
		TSdpTypedTime iActiveDuration;
		RArray<TSdpTypedTime> iTimeOffsets;

		void __DbgTestInvariant() const;
	
	};

#endif // CSDPREPEATFIELD_H
