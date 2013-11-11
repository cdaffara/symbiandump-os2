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
* Name          : SdpDocument.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPDOCUMENT_H
#define CSDPDOCUMENT_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "_sdpdefs.h"
#include <stringpool.h>

// FORWARD DECLARATIONS
class SdpUtil;
class RReadStream;
class RWriteStream;
class CUri8;
class CDesC8Array;
class CSdpOriginField;
class CSdpEmailField;
class CSdpPhoneField;
class CSdpConnectionField;
class CSdpBandwidthField;
class CSdpTimeField;
class CSdpAttributeField;
class CSdpMediaField;
class CSdpKeyField;
class CSdpCodecParseUtil;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *    This class encapsulates the Session Description Protocol (SDP) document.
 *  The SDP document is a collection of SDP fields and related values
 *    describing a session.
 *
 *    The normative reference for correct formatting and values is
 *    draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpDocument : public CBase
    {
    public: // Constructors and destructor
        /**
        * Constructs a new SDP document.
        *    
        * @param aText A string containing a correctly formatted SDP document.
        * @return a new instance.
        */
        IMPORT_C static CSdpDocument* DecodeL(const TDesC8& aText);

        /**
        * Constructs a new SDP document and adds the pointer to the cleanup stack.
        *    
        * @param aText A string containing a correctly formatted SDP document.
        * @return a new instance.
        */
        IMPORT_C static CSdpDocument* DecodeLC(const TDesC8& aText);
            
        /**
        * Constructs a new, empty SDP document.
        * The instance will have the following initial values:
        *      v=0
        *      s=-
        *      t=0 0
        *
        * @return a new instance.
        */
        IMPORT_C static CSdpDocument* NewL();

        /**
        * Constructs a new, empty SDP document document and adds the pointer to 
        * the cleanup stack.
        * The instance will have the following initial values:
        *      v=0
        *      s=-
        *      t=0 0
        *
        * @return a new instance.
        */
        IMPORT_C static CSdpDocument* NewLC();

        /**
        * Deletes the resources held by the instance.
        */
        IMPORT_C ~CSdpDocument();

    public: 
        /**
        * Externalizes the object state in an internal, memory optimized format.
        *
        * @param aStream The output stream.
        */
        IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

        /**
        * Internalizes the object state from an internal, memory optimized format.
        *
        * @param aStream The input stream.
        * @return A new SDP document internalized from the input stream.
        */
        IMPORT_C static CSdpDocument* InternalizeL(RReadStream& aStream);

        /**
        * Outputs the SDP document formatted according to SDP syntax.
        * It is possible to output the document even though it is
        * incomplete in the sense that all mandatory SDP fields are
        * not present.When creating an SDP document to be used e.g.
        * as part of SIP signalling, caller should first ensure the
        * validity of the document using the IsValid() function.
        * 
        * @param aStream Stream used for output. On return the 
        *        stream includes correctly formatted SDP description.
        */
        IMPORT_C void EncodeL(RWriteStream& aStream) const;

        /**
        * Creates a new instance that is equal to the target.
        *
        * @return a new instance.
        */
        IMPORT_C CSdpDocument* CloneL() const;

        /**
        * Checks if the instance represents a valid SDP document.
        *
        * @return ETrue if SDP document is valid and EFalse if not.
        */
        IMPORT_C TBool IsValid() const;

        /**
        * Gets the SDP version used in the version field.
        * Note, that the current implementation supports only version number 0.
        *
        * @return The SDP version number.
        */ 
        IMPORT_C TUint SdpVersion() const;

        /**
        * Gets a reference to the session origin field.
        * Note, that the ownership is not transferred and
        * the instance must not be deleted by the caller.
        *
        * @return The origin field or zero if not present.
        */
        IMPORT_C CSdpOriginField* OriginField();

        /**
        * Gets a reference to the session origin field.
        *
        * @return The origin field or zero if not present.
        */
        IMPORT_C const CSdpOriginField* OriginField() const;

        /**
        * Sets or removes the session origin field.
        * Note, that the ownership of the argument is 
        * transferred to the SDP document object.
        *
        * @param aObj New origin field or null to remove the field.
        */
        IMPORT_C void SetOriginField(CSdpOriginField* aObj);

        /**
        * Gets the session name field value.
        *
        * @return The session name or empty descriptor if not present.
        */
        IMPORT_C const TDesC8& SessionName() const;

        /**
        * Sets the session name field value.
        *
        * @param aValue Valid session name value. 
        *        Note that an empty value is not accepted.
        * @leave KErrSdpCodecSessionField if
        *        aValue is not empty or aValue includes invalid byte strings 
        *        (´\0´, ´\n´, ´\r´).
        */
        IMPORT_C void SetSessionNameL(const TDesC8& aValue);

        /**
        * Gets the session information field value.
        *
        * @return Info field value or an empty descriptor if not present.
        */
        IMPORT_C const TDesC8& Info() const;

        /**
        * Sets or removes the session information field value.
        *
        * @param New field value or an empty descriptor to remove the field.
        * @leave KErrSdpCodecInfoField if aValue is not empty or aValue includes 
        *        invalid byte strings (´\0´, ´\n´, ´\r´).
        */
        IMPORT_C void SetInfoL(const TDesC8& aValue);

        /**
        *    Gets the session URI field value.
        *    Note, that the ownership is not transferred and
        *    the instance must not be deleted by the caller.
        *
        *    @return CUri8*: Session URI or zero if the field is not present.
        */
        IMPORT_C CUri8* Uri();

        /**
        * Gets the session URI field value.
        *
        * @return Session URI or zero if the field is not present.
        */
        IMPORT_C const CUri8* Uri() const;

        /**
        * Sets or removes the session level URI field.
        * Note, that the ownership of the argument is 
        * transferred to the SDP document object.
        *
        * @param New value of the field or zero to remove the field.
        */
        IMPORT_C void SetUri(CUri8* aValue);

        /**
        * Gets the set of session level email field values. This array is used to
        * add and remove fields into the document.
        * Note, that only correctly formatted email field values should be placed
        * into the array. Empty values in the array will be ignored.
        *
        * @return Email fields in array or empty array if no email fields.
        */
        IMPORT_C CDesC8Array& EmailFields();

        /**
        * Gets the set of session level phone field values. This array is used to
        * add and remove fields into the document.
        * Note, that only correctly formatted phone field values should be placed
        * into the array. Empty values in the array will be ignored.
        *  
        * @return Phone fields in array or empty array if no phone fields.
        */
        IMPORT_C CDesC8Array& PhoneFields();

        /**
        * Gets the session level connection field.
        * Note, that the ownership is not transferred and the instance must not be
        * deleted by the caller.
        *
        * @return Connection field or zero if not present.
        */
        IMPORT_C CSdpConnectionField* ConnectionField();

        /**
        * Gets the session level connection field.
        *
        * @return Connection field or zero if not present.
        */
        IMPORT_C const CSdpConnectionField* ConnectionField() const;

        /**
        * Sets or removes the session level connection field.
        * Note, that the ownership of the argument is 
        * transferred to the SDP document object.
        *
        * @param New connection field or zero to remove the field.
        */    
        IMPORT_C void SetConnectionField(CSdpConnectionField* aObj);

        /**
        * Gets the possibly empty set of session level bandwidth fields.
        * This array is used directly for element insertion and removal.
        * The array may contain zero references and these are ignored.
        *
        * The objects referenced from the array are owned by the document
        * instance and must not be deleted. An object can be removed from
        * the document by setting the corresponding element to zero. By
        * doing so, the calling party receives ownership of the removed object.
        *
        * @return Set of bandwidth fields.
        */
        IMPORT_C RPointerArray<CSdpBandwidthField>& BandwidthFields();

        /**
        * Gets the set of session level time description fields.
        * This array is used directly for element insertion and removal.
        * There must be at least one time description field in a valid SDP
        * document.The array may contain zero references and these are ignored.
        *
        * The objects referenced from the array are owned by the document 
        * instance and must not be deleted. An object can be removed from the
        * document by setting the corresponding element to zero. By doing so,
        * the calling party receives ownership of the removed object.  
        *
        * @return Set of time fields.
        */
        IMPORT_C RPointerArray<CSdpTimeField>& TimeFields();

        /** 
        * Gets the zone adjustments field value.
        *
        * @return The field value or an empty descriptor if the
        *         field is not present.
        */
        IMPORT_C const TDesC8& ZoneAdjustments() const;

        /** 
        * Sets the zone adjustments field value.
        *
        * @param aValue A valid field value or an empty descriptor
        *        if field is not present.
        */
        IMPORT_C void SetZoneAdjustmentsL(const TDesC8& aValue);

        /**
        * Gets the session level encryption key field.
        * Note, that the ownership is not transferred and
        * the instance must not be deleted by the caller.
        *
        * @return The encryption key or zero if not present.
        */
        IMPORT_C CSdpKeyField* Key();

        /**
        * Gets the session level encryption key field.
        *
        * @return The encryption key or zero if not present.
        */
        IMPORT_C const CSdpKeyField* Key() const;

        /**
        * Sets or removes the encryption key field.
        * Note, that the ownership of the argument is 
        * transferred to the SDP document object.
        *
        * @param aObj New value of the field or zero to remove the field.
        */
        IMPORT_C void SetKey(CSdpKeyField* aObj);

        /**
        * Gets the possibly empty set of session level attribute fields.
        * This array is used directly for element insertion and removal.
        * The array may contain zero references and these are ignored.
        *
        * The objects referenced from the array are owned by the document
        * instance and must not be deleted. An object can be removed from the
        * document by setting the corresponding element to zero. By doing so,
        * the calling party receives ownership of the removed object.
        *
        * @return Set of session level attributes.
        */
        IMPORT_C RPointerArray<CSdpAttributeField>& AttributeFields();

        /**
        * Gets the possibly empty set of media description fields.
        * This array is used directly for element insertion and removal.
        * Note, that media level attributes and fields are managed 
        * through the corresponding media field instance and not through
        * the document instance.
        * The array may contain zero references and these are ignored.
        *
        * The objects referenced from the array are owned by the document
        * instance and must not be deleted. An object can be removed from the
        * document by setting the corresponding element to zero. By doing so,
        * the calling party receives ownership of the removed object.
        *
        * @return Set of media description fields.
        */
        IMPORT_C RPointerArray<CSdpMediaField>& MediaFields();

    private:
        CSdpDocument();
        void ConstructL();
        void ConstructL(const CSdpDocument& aSdpDocument);
        void DoInternalizeL(RReadStream& aStream);

        void ParseL (const TDesC8& aText);
        void ParseSessionVersionL();
        void ParseSessionOwnerL();
        void ParseSessionNameL();
        void ParseSessionInformationL();
        void ParseUriL();
        void ParseEmailL();
        void ParsePhoneL();
        void ParseConnectionL();
        void ParseBandwidthL();
        void ParseTimeFieldL();
        void ParseZoneAdjustmentL();
        void ParseEncryptionKeyL();
        void ParseAttributeFieldL();
        void ParseMediaLevelL ();
        void ExternalizeDesCArrayL(CDesC8ArraySeg& aArray,
                               RWriteStream& aStream) const;
        void EncodeDesCArrayL(CDesC8ArraySeg& aArray,TInt aIndex,
                                            RWriteStream& aStream) const;

        TDesC8& GetTokenFromStreamL(RReadStream& aStream);

        CSdpDocument(const CSdpDocument&); // Hidden.
        CSdpDocument& operator = (const CSdpDocument&); // Hidden

    private: // Data
            
        HBufC8* iInfo;
        HBufC8* iSessionName;
        HBufC8* iZoneAdjustments;
        CSdpKeyField* iSdpKeyField;
        CSdpOriginField* iSdpOriginField;
        CSdpConnectionField* iSdpConnectionField;
        TUint iSdpVersion;
        RPointerArray<CSdpTimeField>* iTimeFields;
        RPointerArray<CSdpBandwidthField>* iBandwidthFields;
        RPointerArray<CSdpAttributeField>* iAttributeFields;
        RPointerArray<CSdpMediaField>* iMediaFields;
        CUri8* iUri;
        CDesC8ArraySeg* iEmailFields;
        CDesC8ArraySeg* iPhoneFields;
        RStringPool iPool;
        HBufC8* iToken;
        CSdpCodecParseUtil* iSdpCodecParseUtil;
    };

#endif // CSDPDOCUMENT_H
