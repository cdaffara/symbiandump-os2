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
* Name          : SdpAttributeField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPATTRIBUTEFIELD_H
#define CSDPATTRIBUTEFIELD_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "sdpcodecstringconstants.h"
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpFmtAttributeField;
class CSdpAttributeFieldPtrs;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the attribute field of Session Description Protocol
 *	for media and session level attributes. Media format/payload type level
 *  attributes are supported by the CSdpFmtAttributeField class. 
 * 
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpAttributeField : public CBase
	{
    public:     // Constructors and destructor

        /**
         *	Constructs a new attribute field.
		 *			 
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpAttributeField* DecodeL( const TDesC8& aText );

        /**
         *	Constructs a new attribute field and adds the pointer to the 
         *  cleanup stack.
		 *		 
         *	@param aTest A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpAttributeField* DecodeLC( const TDesC8& aText );

        /**
         *	Constructs a new attribute field. 
		 *
         *	@param aAttribute The attribute name. It can be a pre-
         *         defined name in the SDP string pool or defined by the 
         *         client.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpAttributeField* NewL(
            RStringF aAttribute, const TDesC8& aValue );

        /**
         *	Constructs a new attribute field and adds the pointer to the 
         *  cleanup stack. 
		 *
         *	@param aAttribute The attribute name. It can be a pre-defined 
         *         name in the SDP string pool or defined by the client.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpAttributeField* NewLC(
            RStringF aAttribute, const TDesC8& aValue );

 		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpAttributeField();

    public:     // New functions
	
    	/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream Stream used for output. On return the 
         *         stream includes correctly formatted attribute field.
		 */
		IMPORT_C void EncodeL( RWriteStream& aStream ) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpAttributeField* CloneL() const;

		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param aObj: The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == ( const CSdpAttributeField& aObj ) const;

		/**
		 *	Gets the attribute name.
		 *	
		 *	@return	The attribute name. The name can be pre-defined 
         *          name in the SDP string table or defined by the client.
		 */
		IMPORT_C RStringF Attribute() const;

		/**
		 *	Gets the attribute.
		 *
 		 *	@return	The value or an empty descriptor if there is no value part.
		 */
		IMPORT_C const TDesC8& Value() const;

		/**
		 *	Sets the attribute and attribute value.
		 *
         *	@param aAttribute A valid attribute name. It can be a 
         *         pre-defined name in the SDP string pool or 
         *         defined by the client.
         *	@param aValue A valid attribute value or an empty 
         *         descriptor to omit the value part.
		 *  @leave KErrSdpCodecAttributeField if the attribute part is not 
		 *         pre-defined EAttributeFmtp or EAttributeRtpmap.
		 */
		IMPORT_C void SetL( RStringF aAttribute, const TDesC8& aValue );
		
		/**
 		 *	Assigns attribute to the format level attribute.
 		 *  In the final output the attribute will placed after format level
		 *  attribute.
 		 *
 		 *	@param aFmtAttribute A format level 
 		 *         attribute this attribute is assigned to.
 		 */
 		IMPORT_C void AssignTo(const CSdpFmtAttributeField& aFmtAttribute);
		
		/**
		 *	Test if the attribute belongs to the given format level attribute.
		 *  Note, that the check can be true for the media level paremeter if
		 *  it appears after the last format level attribute.
		 *
		 *	@param  aFmtAttribute: A format level 
		 *          attribute this attribute belongs to.
		 *	@return	ETrue if attribute belongs to the format level attribute, 
		 *          EFalse otherwise.
		 */
		IMPORT_C TBool BelongsTo(const CSdpFmtAttributeField& aFmtAttribute) const;

    public:     // Internal to codec

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
        static CSdpAttributeField* InternalizeL( RReadStream& aStream );
        
        /**
         *  Informs if the attribute belongs to any format level attribute.
         *
         *  @return ETrue if attribute belongs to any format level 
		 *			attribute, EFalse otherwise.
         */
        TBool IsFmtAttribute();
    
	private:    // Constructors

        /**
         *  Constructor
         */
		CSdpAttributeField();

        /**
         *  Second phase constructor
         *
         *  @param aText Text string ending to LF character
         */
        void ConstructL( const TDesC8& aText );

        /**
         *  Second phase constructor
         *
         *  @param aAttribute Attribute identifier
         *  @param aValue Value of the attribute of KNullDesC8
         */
        void ConstructL( RStringF aAttribute, const TDesC8& aValue );
    
    private:    // Internal

        /**
         *  Verifies that attribute is valid property attribute and initializes
         *  the member variables of class accordingly
         *
         *  @param aAttribute Attribute in descriptor
         */
        void FormatProperyAttributeL( const TDesC8& aAttribute );

        /**
         *  Verifies that attribute is valid value attribute and initializes
         *  the member variables of class accordingly
         *
         *  @param aArray Array of the attributes
         */
        void FormatValueAttributeL( const TDesC8& aAttribute,
                                    const TDesC8& aValue );

        /**
         *  Verifies that the value is valid for the attribute
         *
         *  @param aAttribute The attribute
         *  @param aArray Array containing the values (and attribute)
         */
        void VerifyValueAttributeL( SdpCodecStringConstants::TStrings aString,
                                    const TDesC8& aValue );
                                    
        /**
         *  Forms attribute and value strings out of the token array
         *  and pushes both (aAttribute & aValue) into cleanup stack
         *
         *  @param aArray Token array
         *  @param aAttribute Attribute string (in cleanup stack)
         *  @param aValue Value string (in cleanup stack)
         */
        void CreateAttributeAndValueStringsLC( RArray<TPtrC8>& aArray,
                                               HBufC8*& aAttribute,
                                               HBufC8*& aValue );

        /**
         *  Checks if the value is among the valid orient attributes
         *
         *  @param aValue Value string
         */
        void CheckValidOrientL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid language attribute as
         *  specified in RFC 3066
         *
         *  @param aValue Value string
         */
        void CheckValidLangStrL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid framerate attribute
         *
         *  @param aValue Value string
         */
        void CheckValidFrameRateL( const TDesC8& aValue );
        
        /**
         *  Checks if the value is among valid uri value for control attribute
         *
         *  @param aValue Value string
         */       
        void CheckValidControlL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid value for group attribute as
         *  specified in RFC 3388
         *
         *  @param aValue Value string
         */       
        void CheckValidGroupL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid value for des attribute as
         *  specified in RFC 3312
         *
         *  @param aValue Value string
         */       
        void CheckValidDesL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid value for curr and conf attribute as
         *  specified in RFC 3312
         *
         *  @param aValue Value string
         */       
        void CheckValidCurrAndConfL( const TDesC8& aValue );

        /**
         *  Checks if value is included in the list of predefined values
         *
         *  @param aItemsCount Predefined values count
         *  @param aValuesList List of predefined values
         *  @param aValue Value string
         */       
        void ValidValueInListL( TInt aItemsCount, 
                                const TText8* const aValuesList[], 
                                const TDesC8& aValue );

        /**
         *  Checks if the value is valid value for range attribute as
         *  specified in RFC 2326
         *
         *  @param aValue Value string
         */       
        void CheckValidRangeL( const TDesC8& aValue );


        /**
         *  Checks if the value is valid rtpmap string
         *
         *  @param aValue Value string
         */   
        void CheckValidRtpmapL( const TDesC8& aValue );

        /**
         *  Checks if the value is valid fmtp string
         *
         *  @param aValue Value string
         */   
        void CheckValidFmtpL( const TDesC8& aValue );
        
        /**
         *  Set attribute and values. Attribute is not one of predefined ones.
         *
         *  @param aAttribute Attribute string (in cleanup stack)
         *  @param aValue Value string
         */   
        void SetNotPredefinedAttributeL( const TDesC8& aAttribute, 
                                         const TDesC8& aValue );
        
        void CreateAttributeFieldPtrsL();
        
        inline CSdpAttributeFieldPtrs& AttributeFieldPtrs();      
        
        inline const CSdpAttributeFieldPtrs& AttributeFieldPtrs() const;
	   
		void __DbgTestInvariant() const;


    private:    // Data

        // String pool
        RStringPool iPool;

        // Attribute name
        RStringF iAttribute;

        // Flag indicating if the attribute is a property attribute
        TBool iPropertyAttribute;

        // Value field
        HBufC8* iValue;
	};

#endif // CSDPATTRIBUTEFIELD_H
