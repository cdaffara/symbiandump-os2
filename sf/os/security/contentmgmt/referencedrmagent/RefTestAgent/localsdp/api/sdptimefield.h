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
* Name          : SdpTimeField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPTIMESFIELD_H
#define CSDPTIMESFIELD_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpRepeatField;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the time field and the related zero or 
 *  more repeat times fields and an optional time zone adjustment
 *  field of the Session Description Protocol.
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpTimeField : public CBase
	{
    public: // Constructors and destructor
        /**
         *	Constructs a time field.
		 * 
         *	@param aText A correctly formatted time field value
         *         terminated by a CRLF followed by zero or more repeat 
         *         times and zero or one time zone adjustment fields 
         *         separeted by a CRLF.
		 *	@param aRecurse Flag to specify whether to decode subfields 
		 *         also (ETrue) or only the time field (EFalse).
         *	@return a new instance.
         */
		IMPORT_C static CSdpTimeField * DecodeL(const TDesC8& aText, 
                                                TBool aRecurse = ETrue);

        /**
         *	Constructs a time field and adds the pointer to the cleanup stack.
		 *	
         *	@param aText A correctly formatted time field value 
         *         terminated by a CRLF followed by zero or more repeat 
         *         times and zero or one time zone adjustment fields 
         *         separeted by a CRLF.
		 *	@param aRecurse Flag to specify whether to decode subfields 
		 *         also (ETrue) or only the time field (EFalse).
         *	@return a new instance.
         */
		IMPORT_C static CSdpTimeField * DecodeLC(const TDesC8& aText,
                                                TBool aRecurse = ETrue);

        /**
         *	Constructs a time field.
		 *	
         *	@param aStartTime Session start time.
         *	@param aStopTime Session end time that must be 
         *         greater than or equal to the start time.
         *	@return a new instance.
         */
		IMPORT_C static CSdpTimeField * NewL(const TDesC8& aStartTime,
									        const TDesC8& aStopTime);

        /**
         *	Constructs a time field and adds the pointer to the cleanup stack.
		 *	
         *	@param aStartTime Session start time.
         *	@param aStopTime Session end time that must be 
         *         greater than or equal to the start time.
         *	@return a new instance.
         */
		IMPORT_C static CSdpTimeField * NewLC(const TDesC8& aStartTime,
									        const TDesC8& aStopTime);

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpTimeField();

    public: // New functions
		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF. Optionally output also the related repeat
		 *  times.
		 * 
		 *	@param aStream Stream used for output. On return 
         *         the stream includes correctly formatted time field with 
         *         repeat fields if aRecurse is ETrue.
		 *	@param aRecurse Flag to specify whether to output subfields
		 *         also (ETrue) or only the time field (EFalse).
		 */
		IMPORT_C void EncodeL(RWriteStream& aStream, TBool aRecurse = ETrue) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *  Optionally also related repeat times are cloned.
		 *
		 *	@param aRecurse Flag to specify whether to clone subfields 
		 *         also (ETrue) or only the time field (EFalse).
		 *	@return a new instance.
		 */
		IMPORT_C CSdpTimeField * CloneL(TBool aRecurse = ETrue) const;

		/**	
		 *	Compares this instance to another for equality. Subfields are
		 *	included in comparison if present.
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == (const CSdpTimeField & aObj) const;

		/**
		 *	Checks the consistency of the time description and it's subfields.
		 *.
		 *	@return	ETrue if time description is valid and EFalse if not.
		 */
		IMPORT_C TBool IsValid() const;

		/**
		 *	Gets the session start time.
		 *
		 *	@return The session start time.
		 */
		IMPORT_C const TDesC8& StartTime() const;

		/**
		 *	Gets the session stop time.
		 *
		 *	@return The session stop time.
		 */
		IMPORT_C const TDesC8& StopTime() const;

		/**
		 *	Sets the session start and stop times.
		 *
         *	@param aStartTime Session start time..
         *	@param aStopTime Session end time that must be 
         *         greater than or equal to the start time.
		 *	@leave KErrSdpCodecTimeField if start time and stop time are not 
         *         correct as defined draft-ietf-mmusic-sdp-new-14.
		 */
		IMPORT_C void SetTimesL(const TDesC8& aStartTime, const TDesC8& aStopTime);

		/**
		 *	Gets the set of repeat times fields related to this time field.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	The objects referenced from the array are owned by the media field
		 *  instance and must not be deleted. An object can be removed from the 
		 *  media description by setting the corresponding element to zero. By
		 *  doing so, the calling party receives ownership of the removed object.
		 *
		 *	@return The set of repeat fields.
		 */
		IMPORT_C RPointerArray<CSdpRepeatField>& RepeatFields();
	
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
		static CSdpTimeField* InternalizeL(RReadStream& aStream);
        
        /**
         *  Creates object from the stream data.
         *	Does the "2nd phase construction" of internalization.
         *
         *  @param aStream Stream where the object's state will be read
         */
        void DoInternalizeL(RReadStream& aStream);

	private:
   		CSdpTimeField();

		void ConstructL(const TDesC8& aText, TBool aRecurse=ETrue);
		void ConstructL(const TDesC8& aStartTime, const TDesC8& aStopTime);

        TBool RepeatFieldsCompare(const CSdpTimeField& aObj) const;

    private: // Data
		HBufC8* iStartTime;
		HBufC8* iStopTime;
		RPointerArray<CSdpRepeatField> iRFields;
        RStringPool iStringPool;

		void __DbgTestInvariant() const;

	};

#endif // CSDPTIMESFIELD_H
