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
* Name          : SdpFmtAttributeField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPFMTATTRIBUTEFIELD_H
#define CSDPFMTATTRIBUTEFIELD_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpAttributeField;
class TSdpRtpmapValue;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the attribute field of the Session Description
 *  Protocol for media format/payload type level attributes. Media and session
 *  level attributes are supported by the CSdpAttributeField class. 
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpFmtAttributeField : public CBase
	{
    public: // Constructors and destructor
        /**
         *	Constructs a new media format level attribute field.
		 * 
		 *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *  @return a new instance.
         */
		IMPORT_C static CSdpFmtAttributeField* DecodeL( const TDesC8& aText );

        /**
         *	Constructs a new media format level attribute field  and adds the 
         *  pointer to the cleanup stack.
		 *			 
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpFmtAttributeField* DecodeLC( const TDesC8& aText);

        /**
         *	Constructs a new media level format attribute field.
		 *
         *	@param aAttribute The attribute name. It can be a pre-defined
         *         name in the SDP string pool or defined by the client.
         *	@param aFormat A valid format/payload identifier 
         *         used in the format list of the media description.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpFmtAttributeField* NewL(
            RStringF aAttribute, const TDesC8& aFormat, const TDesC8& aValue );

        /**
         *	Constructs a new media format level attribute field and adds the 
         *  pointer to the cleanup stack.
		 *
         *	@param aAttribute The attribute name. It can be a pre-defined 
         *         name in the SDP string pool or defined by the client.
         *	@param aFormat A valid format/payload identifier 
         *         used in the format list of the media description.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpFmtAttributeField* NewLC(
            RStringF aAttribute, const TDesC8& aFormat, const TDesC8& aValue );

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpFmtAttributeField();

    public: // New functions

		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream Stream used for output. On return 
         *         the stream includes correctly formatted attribute field.
		 */
		IMPORT_C void EncodeL( RWriteStream& aStream ) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpFmtAttributeField * CloneL() const;

		/**	
		 *	Compares this instance to another for equality. 
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator== ( const CSdpFmtAttributeField& aObj ) const;

		/**
		 *	Gets the attribute.
		 *	
		 *	@return	The attribute name. The name can be pre-defined 
         *          name in the SDP string table or defined by the client.
         */
		IMPORT_C RStringF Attribute() const;

		/**
		 *	Gets the format.
		 *	
		 *	@return	The format.
		 */
		IMPORT_C const TDesC8& Format() const;

		/**
		 *	Gets the attribute value.
		 *	
		 *	@return The value  or an empty descriptor if there is no value part.
		 */
		IMPORT_C const TDesC8& Value() const;

		/**
		 *	Sets the attribute, format/payload type and attribute value.
		 *
         *	@param aAttribute A valid attribute name. It can be a 
         *         pre-defined name in the SDP string pool or defined by 
         *         the client.
         *	@param aFormat A valid format/payload identifier 
         *         used in the formatlist of the media description.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
         *  @leave KErrSdpCodecMediaAttributeField if aAttribute is not rtpmap
         *         or fmtp attribute.
		 */
		IMPORT_C void SetL(
            RStringF aAttribute, const TDesC8& aFormat, const TDesC8& aValue );

    public: // Internal to codec

        /**
         *  Externalizes the object to stream
         *
         *  @param aStream Stream where the object's state will be stored
         */
		void ExternalizeL( RWriteStream& aStream ) const;

        /**
         *  Creates object from the stream data
         *
         *  @param aStream Stream where the object's state will be read        
         *  @return Initialized object
         */
		static CSdpFmtAttributeField* InternalizeL( RReadStream& aStream );

        /**
         *  Returns attribute field
         *
         *  @return Attribute field
         */
        const CSdpAttributeField* AttributeField() const;

    private: // New methods

        /**
         *  Creates object from the stream data.
         *	Does the "2nd phase construction" of internalization.
         *
         *  @param aStream Stream where the object's state will be read
         */
        void DoInternalizeL( RReadStream& aStream );

        /**
         *  Formats value parameters
         *
         *  @param aField Attribute field object
         */
        void FormatValueParamsL( CSdpAttributeField* aField );

    private: // Construction and destruction
        
        /**
         *  Constructor
         */
        CSdpFmtAttributeField();

        /**
         *  Second phase construction
         *         
         *  @param aText A string containing a correctly formatted field value
		 *         terminated by a CRLF.
         */
        void ConstructL( const TDesC8& aText );

        /**
         *  Second phase construction
         *
         *	@param aAttribute The attribute from the pre-defined SDP string 
		 *         table or defined by the user
         *	@param aFormat A valid format/payload identifier used in the format
		 *         list of the media description.
         *	@param aValue A valid attribute value or an empty descriptor
		 *         to omit the value part.         
		 */
		void ConstructL(
            RStringF aAttribute, const TDesC8& aFormat, const TDesC8& aValue );    

        
        /**
         *  Decode format parameter from string
         *
         *	@param aText A text includeing format parameter.
		 */
        void DecodeFormatL(const TDesC8& aText);
        
		void __DbgTestInvariant() const;


    private:

        RStringPool iPool;
        CSdpAttributeField* iAttributeField;       
		/** Pointer to the format part*/
		TPtrC8 iFormat;
        /** Pointer to the format parameters*/
		TPtrC8 iValuePart;
	};

#endif // CSDPFMTATTRIBUTEFIELD_H
