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
* Name          : SdpBandwidthField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPBANDWIDTHFIELD_H
#define CSDPBANDWIDTHFIELD_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sdpdefs.h"

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the bandwidth field of 
 *  the Session Description Protocol.
 * 
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpBandwidthField : public CBase
	{
	public:// Constructors and destructor
       /**
         *	Constructs a new bandwidth field.
		 *	
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpBandwidthField* DecodeL(const TDesC8& aText);

         /**
         *	Constructs a new bandwidth field and adds the pointer to the 
         *  cleanup stack.
		 *	
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpBandwidthField* DecodeLC(const TDesC8& aText);

        /**
         *	Constructs a new bandwidth field.
		 *
         *	@param aModifier Bandwidth modifier.
		 *  @param aValue Bandwidth value.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpBandwidthField* NewL(RStringF aModifier,
												 TUint32 aValue);

        /**
         *	Constructs a new bandwidth field and adds the pointer to the 
         *  cleanup stack.
		 *
         *	@param aModifier Bandwidth modifier.
		 *  @param aValue Bandwidth value.
         *	@return a new instance.
		 */
		IMPORT_C static CSdpBandwidthField* NewLC(RStringF aModifier,
												  TUint32 aValue);

 
        /**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpBandwidthField();

    public: // New functions
		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream Stream used for output. On return 
         *         the stream includes correctly formatted bandwidth field.
		 */
		IMPORT_C void EncodeL(RWriteStream& aStream) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpBandwidthField * CloneL() const;

		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == (const CSdpBandwidthField& aObj) const;

		/**
		 *	Gets the bandwidth modifier.
		 *
		 *	@return The modifier that can be pre-defined in the 
		 *          SDP string table or defined by the user.
		 */
		IMPORT_C RStringF Modifier() const;

		/**
		 *	Sets the bandwidth modifier.
		 *
         *	@param aModifier A valid bandwidth modifier name.
		 */
		IMPORT_C void SetModifier(RStringF aModifier);

		/**
		 *	Gets the bandwidth value.
		 *
		 *	@return The value.
		 */
		IMPORT_C TUint32 Value() const;

		/**
		 *	Sets the bandwidth value.
		 *
		 *	@param aValue The Value.
		 */
		IMPORT_C void SetValue(TUint32 aValue);

    public:
		/**
		 *	Externalize the instance in a memory optimized format.
		 *
		 *	@param aStream The stream used for externalization.
		 */
		void ExternalizeL(RWriteStream& aStream) const;

		/**
		 *	Create a new instance from value created with ExternalizeL().
		 *
		 *	@param aStream The stream used for internalization.
		 *	@return	The new instance.
		 */
        static CSdpBandwidthField* InternalizeL(RReadStream& aStream);

    private:
        CSdpBandwidthField();
		void ConstructL(const TDesC8& aText);
		void ConstructL(RStringF aModifier, TUint32 aValue);
		void ConstructL(const TDesC8& aModifier, TUint32 aValue);

        /**
         *  Copies given modifier to iModifier. If aModifier is none of 
         *  predefined ones, new modifier is added to stringPool.
         *
         *  @param aModifier Given modifier
         */
        void CopyModifierL(const TDesC8& aModifier);

    private: // Data
		RStringF iModifier;
        TUint32 iValue;
        RStringPool iStringPool;
	};

#endif // CSDPBANDWIDTHFIELD_H
