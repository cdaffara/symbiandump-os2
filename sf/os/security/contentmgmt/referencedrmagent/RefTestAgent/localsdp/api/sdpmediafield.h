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
* Name          : SdpMediaField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPMEDIAFIELD_H
#define CSDPMEDIAFIELD_H

//  INCLUDES
#include <e32base.h>
#include "_sdpdefs.h"
#include <stringpool.h>

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpConnectionField;
class CSdpBandwidthField;
class CSdpAttributeField;
class CSdpKeyField;
class CSdpFmtAttributeField;
class CSdpCodecParseUtil;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the media description field and related media
 *  specific subfields of the Session Description Protocol.
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpMediaField : public CBase
	{

	public:


    public: // Constructors and destructor
        /**
         *	Constructs a new media description field.
		 *	
         *	@param aText A correctly formatted media field 
         *         terminated by a CRLF followed by zero or more media 
         *         attribute fields separated by a CRLF.
         *	@param aRecurse If ETrue, attempts to construct also media 
         *         attributes following the CRLF terminating the media field.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpMediaField* DecodeL(const TDesC8& aText,
												TBool aRecurse = ETrue);

        /**
         *	Constructs a new media description field and adds the pointer to 
         *  the cleanup stack.
		 *	
         *	@param aText A correctly formatted media field 
         *         terminated by a CRLF followed by zero or more media 
         *         attribute fields separated by a CRLF.
         *	@param aRecurse If ETrue, attempts to construct also media 
         *         attributes following the CRLF terminating the media field.
		 *  @return a new instance.
         */
		IMPORT_C static	CSdpMediaField* DecodeLC(const TDesC8& aText,
												 TBool aRecurse = ETrue);

        /**
         *	Constructs a new media description field.
		 *  The optional portcount component of the field is initialized to 1.
		 *
		 *	@param aMedia A valid media type name.
         *	@param aPort Port number.
         *	@param aProtocol A valid media protocol name.
         *	@param aFormatList A valid format list.
         *	@return a new instance.
         */
		IMPORT_C static CSdpMediaField* NewL(RStringF aMedia,
											 TUint aPort,
											 RStringF aProtocol,
											 const TDesC8& aFormatList);

		/**
         *	Constructs a new media description field and adds the pointer to 
         *  the cleanup stack.
		 *  The optional portcount component of the field is initialized to 1.
		 *	
		 *	@param aMedia A valid media type name.
         *	@param aPort Port number.
         *	@param aProtocol A valid media protocol name.
         *	@param aFormatList A valid format list.
         *	@return a new instance.
         */
		IMPORT_C static CSdpMediaField* NewLC(RStringF aMedia,
											  TUint aPort,
										      RStringF aProtocol,
										      const TDesC8& aFormatList);

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpMediaField();

    public: // New functions
		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF. Optionally output also the related media 
		 *  level fields. 
		 * 
		 *	@param aStream Stream used for output. On return the
         *         stream includes correctly formatted media field with 
         *         media part fields if aRecurse is defined to ETrue.
		 *	@param aRecurse Flag to specify whether to output media 
		 *         attributes also (ETrue) or only the media field (EFalse).
		 */
		IMPORT_C void 
			EncodeL(RWriteStream& aStream, TBool aRecurse = ETrue) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *  Optionally also related subfields are cloned.
		 *
		 *	@param aRecurse Flag to specify whether to clone subfields
		 *         also (ETrue) or only the media field (EFalse).
		 *	@return a new instance.
		 */
		IMPORT_C CSdpMediaField * CloneL(TBool aRecurse = ETrue) const;

		/**	
		 *	Compares this instance to another for equality. Subfields are
		 *  included in the comparison if present.
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == (const CSdpMediaField& aObj) const;

		/**
		 *	Checks the consistency between the media field and it's subfields.
		 *  In particular, this function checks that each format specific 
		 *  attribute is related to a format in the format list of the
		 *  media field.
		 *
		 *	@return	ETrue if media description is consistent and EFalse if not.
		 */
		IMPORT_C TBool IsValid() const;

		/**
		 *	Gets the media type.
		 *
		 *	@return The media type.
		 */
		IMPORT_C RStringF Media() const;

		/**
		 *	Sets the media type.
		 *
		 *	@param aMedia The media type.
		 *  @leave KErrSdpCodecMediaField if aMedia is not valid character
		 */
		IMPORT_C void SetMediaL(RStringF aMedia);

		/**
		 *	Gets the media protocol.
		 *
		 *	@return The media protocol.
		 */
		IMPORT_C RStringF Protocol() const;

		/**
		 *	Sets the media protocol.
		 *
		 *	@param aProtocol The media protocol.
		 *  @leave KErrSdpCodecMediaField if 
		 *		   aProtocol containing two elements divided by slash are not 
         *         valid tokens.
		 */
		IMPORT_C void SetProtocolL(RStringF aProtocol);

		/**
		 *	Gets the port number.
		 *
		 *	@return The port number.
		 */
		IMPORT_C TUint Port() const;

		/**
		 *	Sets the port number.
		 *
		 *	@param aPort The port number.
		 *  @leave KErrSdpCodecMediaField if port is not valid number as 
         *         defined in draft-ietf-mmusic-sdp-new-14
		 */
		IMPORT_C void SetPortL(TUint aPort);

		/**
		 *	Gets the port count.
		 *
		 *	@return The port count.
		 */
		IMPORT_C TUint PortCount() const;

		/**
		 *	Sets the port count.
		 *
		 *	@param aCount The port count that must be greater than zero.
		 *  @leave KErrSdpCodecMediaField if aCount equals to zero.
		 */
		IMPORT_C void SetPortCountL(TUint aCount);

		/**
		 *	Gets the format list.
		 *
		 *	@return The format list.
		 */
		IMPORT_C const TDesC8& FormatList() const;

		/**
		 *	Sets the format list.
		 *  The format list should contain one or more format tokens separated
		 *  by a single whitespace character.
		 *
		 *	@param aValue A valid format list.
		 *  @leave KErrSdpCodecMediaField if aValue contains invalid tokens
		 */
		IMPORT_C void SetFormatListL(const TDesC8& aValue);

		/**
		 *	Return the media level info field value.
		 *
		 *	@return The value or an empty descriptor if not present.
		 */
		IMPORT_C const TDesC8& Info() const;

		/**
		 *	Sets the media level info field value.
		 *
		 *	@param aValue A valid info field value.
		 *	@leave KErrSdpCodecMediaInfoField if aValue is not KNullDesC8 or 
         *         aValue includes invalid byte strings (´\0´, ´\n´, ´\r´).
		 */
		IMPORT_C void SetInfoL(const TDesC8& aValue);

		/**
		 *	Gets the media level encryption key field.
		 *	Note, that the ownership is not transferred and
		 *	the instance must not be deleted by the caller.
		 *
		 *	@return Encryption key field or null if not present.
		 */
		IMPORT_C CSdpKeyField* Key();

		/**
		 *	Gets the media level encryption key field.
		 *
		 *	@return Encryption key field or null if not present.
		 */
		IMPORT_C const CSdpKeyField* Key() const;

		/**
		 *	Sets or removes the media level encryption key field.
		 *
		 *	@param aObj The new key field or null if field is 
         *         to be removed. Ownership of the referenced object
		 *         is transferred to the media field instance.
		 */
		IMPORT_C void SetKey(CSdpKeyField* aObj);

		/**
		 *	Gets the set of media format level attributes.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	The objects referenced from the array are owned by the media
		 *  field instance and must not be deleted. An object can be
		 *  removed from the media description by setting the corresponding
		 *  element to zero. By doing so, the calling party receives ownership
		 *  of the removed object.
		 *
		 *	@return The set of media format level attributes.
		 */
		IMPORT_C RPointerArray<CSdpFmtAttributeField>& FormatAttributeFields();

		/**
		 *	Gets the set of media level, format independent attributes.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	The objects referenced from the array are owned by the media
		 *  field instance and must not be deleted. An object can be
		 *  removed from the media description by setting the corresponding
		 *  element to zero. By doing so, the calling party receives ownership
		 *  of the removed object.
		 *
		 *	@return The set of media level attributes.
		 */
		IMPORT_C RPointerArray<CSdpAttributeField>& AttributeFields();

		/**
		 *	Gets the set of media level bandwidth fields.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	The objects referenced from the array are owned by the media
		 *  field instance and must not be deleted. An object can be
		 *  removed from the media description by setting the corresponding
		 *  element to zero. By doing so, the calling party receives ownership
		 *  of the removed object.
		 *
		 *	@return The set of media level bandwidth fields.
		 */
		IMPORT_C RPointerArray<CSdpBandwidthField>& BandwidthFields();

		/**
		 *	Gets the set of media level connection fields.
		 *  This array is used directly for element insertion and removal.
		 *
		 *	The objects referenced from the array are owned by the media
		 *  field instance and must not be deleted. An object can be
		 *  removed from the media description by setting the corresponding
		 *  element to zero. By doing so, the calling party receives ownership
		 *  of the removed object.
		 *
		 *	@return The set of media level connection fields.
		 */
		IMPORT_C RPointerArray<CSdpConnectionField>& ConnectionFields();

		/**
		 *	Removes a specific format from the media description.
		 *  The function will remove the named format from the format list if
		 *  found and then delete all format level attributes of type 
		 *  CSdpFmtAttributeField related to the removed format. 
		 *
		 *	@param aFormat The name of the format to remove.
		 */
		IMPORT_C void RemoveFormatL(const TDesC8& aFormat);

		/**
		 *	Removes all formats except one from the media description.
		 *  The function will set the format list to only contain the one
		 *  format to keep and then delete all format level attributes that
		 *  are related to the removed formats. If the format is not found
		 *  from the format list, it is added there. If there are no format
		 *  level attributes for the specified format, the format level
		 *  attribute set will be empty.
		 *
		 *	@param aFormat The name of the format to keep after 
		 *         removing all others.
		 */
		IMPORT_C void KeepFormatL(const TDesC8& aFormat);

		/**
		 *	Sets this media description into rejected state.
		 *  The rejected state is defined by the offer/answer model in
		 *  RFC3264. The function sets the port number to 0 and removes
		 *  all formats except one. The remaining format is determined
		 *  by the implementation.
		 */
		IMPORT_C void RejectMedia();

    public:
		/**
		 *	Shows if contact is present
		 *
		 *	@return ETrue if contact present, otherwise EFalse
		 */
		TBool IsContactPresent() const;
		
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
		static CSdpMediaField* InternalizeL(RReadStream& aStream);

	private:
		CSdpMediaField();
		void ConstructL();
		void ConstructL(TBool aRecurse);
		void ConstructL(RStringF aMedia, TUint aPort, RStringF aProtocol, 
						const TDesC8& aFormatList);

		void ConstructL(const CSdpMediaField& aSdpMediaField);
		void DoInternalizeL(RReadStream& aStream);
		void ParseL (const TDesC8& aText);
		void ParseMediaL();
		void ParseInformationL();
		void ParseConnectionL();
		void ParseBandwithL();
		void ParseEncryptionKeyL();
		void ParseAttributeFieldsL();
		TDesC8& GetTokenFromStreamL(RReadStream& aStream);


		CSdpMediaField(const CSdpMediaField&); // Hidden.
		CSdpMediaField& operator = (const CSdpMediaField&); // Hidden

    private: // Data
		HBufC8* iInfo;
		CSdpKeyField* iSdpKeyField;
		RArray<TPtrC8> iElementArray;
		RStringPool iPool;
		TBool iRecurse;
		RStringF iMedia;
		RStringF iProtocol;
		TUint iPort;
		TUint iPortCount;
		HBufC8* iFormatList;
		RPointerArray<CSdpBandwidthField>* iBandwidthFields;
		RPointerArray<CSdpAttributeField>* iAttributeFields;
		RPointerArray<CSdpConnectionField>* iConnectionFields;
		RPointerArray<CSdpFmtAttributeField>* iFmtAttrFields;
		HBufC8* iToken;
		CSdpCodecParseUtil* iSdpCodecParseUtil;
	};

#endif // CSDPMEDIAFIELD_H
