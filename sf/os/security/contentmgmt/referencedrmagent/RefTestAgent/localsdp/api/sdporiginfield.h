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
* Name          : SdpOriginField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPORIGINFIELD_H
#define CSDPORIGINFIELD_H

//  INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <stringpool.h>
#include "_sdpdefs.h"

//  CONSTANTS
const TInt KMaxAddressLength = 256;

//  FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;
class CSdpOriginFieldPtrs;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the origin field of 
 *  the Session Description Protocol.
 * 
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpOriginField : public CBase
	{
    public:     // Constructors and destructor

        /**
         *	Constructs a new origin field.
		 *		 
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* DecodeL( const TDesC8& aText );

        /**
         *	Constructs a new origin field and adds the pointer to the cleanup 
         *  stack.
		 * 		
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* DecodeLC( const TDesC8& aText );

        /**
         *	Constructs a new origin field.
		 *	         
         *	@param aUserName A valid user name value.
         *	@param aSessionId A valid session id value.
         *	@param aSessionVersion A valid announcement version value.
         *	@param aAddress A unicast IP address from either
		 *         KAfInet or KAfInet6 family.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* NewL( const TDesC8& aUserName, 
											   TInt64 aSessionId, 
											   TInt64 aSessionVersion, 
											   TInetAddr& aAddress );

        /**
         *	Constructs a new origin field and adds the pointer to the cleanup 
         *  stack.
		 *	         
         *	@param aUserName A valid user name value.
         *	@param aSessionId A valid session id value.
         *	@param aSessionVersion A valid announcement version value.
         *	@param aAddress A unicast IP address from either
		 *         KAfInet or KAfInet6 family.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* NewLC( const TDesC8& aUserName, 
											    TInt64 aSessionId, 
											    TInt64 aSessionVersion, 
											    TInetAddr& aAddress );

		/**
         *	Constructs a new origin field.
		 *	
         *	@param aUserName A valid user name value.
         *	@param aSessionId A valid session id value.
         *	@param aSessionVersion A valid announcement version value.
         *	@param aNetType A valid network type value (typically "IN").
		 *  @param aAddressType A valid address type value 
		 *         (typically "IP4" or "IP6").
		 *	@param aAddress A unicast IP Address, Fully 
         *         Qualified Domain Name (FQDN), or an other type of 
         *         address allowed for the address type.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* NewL( const TDesC8& aUserName, 
											TInt64 aSessionId, 
											TInt64 aSessionVersion, 
											RStringF aNetType, 
											RStringF aAddressType, 
											const TDesC8& aAddress );

		/**
         *	Constructs a new origin field and adds the pointer to the cleanup 
         *  stack.
		 *	
         *	@param aUserName A valid user name value.
         *	@param aSessionId A valid session id value.
         *	@param aSessionVersion A valid announcement version value.
         *	@param aNetType A valid network type value (typically "IN").
		 *  @param aAddressType A valid address type value 
		 *         (typically "IP4" or "IP6").
		 *	@param aAddress A unicast IP Address, Fully Qualified Domain
		 *         Name (FQDN), or an other type of address allowed
		 *         for the address type.
         *	@return a new instance.
         */
		IMPORT_C static	CSdpOriginField* NewLC( const TDesC8& aUserName, 
											TInt64 aSessionId, 
											TInt64 aSessionVersion, 
											RStringF aNetType, 
											RStringF aAddressType, 
											const TDesC8& aAddress );

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpOriginField();

    public: // New functions

		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream Stream used for output. On return 
         *         the stream includes correctly formatted origin field.
		 */
		IMPORT_C void EncodeL( RWriteStream& aStream ) const;

		/**
    	 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpOriginField* CloneL() const;

		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param aObj The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator == ( const CSdpOriginField& aObj ) const;

		/**
		 *	Gets the user name.
		 *
		 *	@return User name.
		 */
		IMPORT_C const TDesC8& UserName() const;

		/**
		 *	Sets the user name.
		 *
		 *	@param aValue A valid user name value.
		 *	@leave KErrSdpCodecOriginField if aValue is not a valid user name.
		 */
		IMPORT_C void SetUserNameL( const TDesC8& aValue );

		/**
		 *	Gets the session id of the origin field.
		 *  
		 *	@return Session id.
		 */
		IMPORT_C TInt64 SessionId() const;

		/**
		 *	Sets the session id.
		 *
		 *	@param aValue A valid session id value.
		 */
		IMPORT_C void SetSessionId( TInt64 aValue );
		
		/**
		 *	Gets the announcement version.
		 *	
		 *	@return Version.
		 */
		IMPORT_C TInt64 Version() const;

		/**
		 *	Sets the announcement version..
		 *
		 *	@param aValue A valid version value.
		 */
		IMPORT_C void SetVersion( TInt64 aValue );

		/**
		 *	Gets the network type.
		 *
		 *	@return The network type from pre-defined SDP string 
         *          table or given by the user.
		 */
		IMPORT_C RStringF NetType() const;

		/**
		 *	Gets the address type.
		 *
		 *	@return The address type from pre-defined SDP string 
         *          table or given by the user.
		 */
		IMPORT_C RStringF AddressType() const;
		
		/**
		 *	Gets the address.
		 *
		 *	@return Address as an IP address or null if it is
		 *          not an IP address. This may be e.g. when the address has 
		 *          been specified as a FQDN. In this case, the address can be 
		 *          accessed using the other getters.
		 */
		IMPORT_C const TInetAddr* InetAddress() const;

		/**
		 *	Gets the address.
		 *
		 *  @return Address as a string.
		 */
		IMPORT_C const TDesC8& Address() const;

		/**
		 *	Sets the address, network and address type.
		 *	Also sets the network type to "IN" and address type to "IP4" or 
		 *  "IP6" depending on the address family of aValue.
		 *
         *	@param aValue A unicast IP address from either KAfInet 
         *         or KAfInet6 family.
		 */
		IMPORT_C void SetInetAddress( const TInetAddr& aValue );

		/**
		 *	Sets the address, network and address type.
		 *
		 *	@param aAddress A unicast IP Address, Fully Qualified Domain
		 *         Name (FQDN), or an other type of address allowed for the
		 *         address type.
         *	@param aNetType A valid network type value (typically "IN").
		 *  @param aAddressType A valid address type value 
		 *         (typically "IP4" or "IP6").
		 *	@leave KErrSdpCodecOriginField if aNetType or aAddressType are not
		 *		   valid tokens, or aAddress is invalid, or aAddress type 
		 *		   is not aligned with the address format
		 */
		IMPORT_C void SetAddressL( const TDesC8& aAddress, 
								   RStringF aNetType, 
								   RStringF aAddressType );
        
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
		static CSdpOriginField* InternalizeL( RReadStream& aStream );

	private:    // Constructors

        /**
         *  Constructor
         */
		CSdpOriginField();

        /**
         *  Second phase constructor. Parses the text string and initializes
         *  the member variables
         *         
         *  @param aText Line of text that will be parsed (ends on the newline)
         */
		void ConstructL( const TDesC8& aText );
        
        /**
         *  Second phase constructor. Constructs the object.
         *
         *  @param aUserName  A valid user name value.
         *  @param aSessionId A valid session id value.
         *  @param aSessionVersion A valid announcement version value.
         *  @param aAddress A unicast IP address from either
		 *                  KAfInet or KAfInet6 family.        
         */
		void ConstructL( const TDesC8& aUserName, 
						 TInt64 aSessionId, 
                         TInt64 aSessionVersion, 
						 TInetAddr& aUnicastAddress );
         
        /**                        
         *	Second phase constructor. Constructs the object.
		 *	
         *	@param aUserName  A valid user name value.
         *	@param aSessionId A valid session id value.
         *	@param aSessionVersion A valid announcement version value.
         *	@param aNetType A valid network type value (typically "IN").
		 *  @param aAddressType A valid address type value 
		 *         (typically "IP4" or "IP6").
		 *	@param aAddress A unicast IP Address, Fully Qualified Domain
		 *         Name (FQDN), or an other type of address allowed
		 *         for the address type.
         */
        void ConstructL( const TDesC8& aUserName, 
	                     TInt64 aSessionId, 
	                     TInt64 aSessionVersion, 
	                     RStringF aNetType, 
					     RStringF aAddressType, 
	                     const TDesC8& aAddress );

        // These are unnecessary? Derived from CBase, prevents the use
		CSdpOriginField( const CSdpOriginField& ); // Hidden.
		CSdpOriginField& operator = ( const CSdpOriginField& ); // Hidden

    private:        // New methods

        /**
         *  Checks if the given address is valid
         *
         *  @param aAddress Address to be checked
         *  @return Valid address or not
         */
        TBool IsValidAddress( const TDesC8& aAddress ) const;   
        
        /**
         *  Checks if the given address is valid
         *
         *  @param aAddr Address to be checked
         *  @return Valid address or not
         */             
        TBool IsValidAddress( const TInetAddr& addr ) const;

        /**
         *  Checks that address type matches with address format
         *
         *  @param aAddress Address
         *  @param aType Address type
         *  @param aPool String pool
         *  @return Type Matches with format or not
         */
        TBool TypeMatchesWithFormat( 
            const TDesC8& aAddress, const TDesC8& aType, 
            RStringPool aPool ) const;

        /**
         *  Checks if the given username is valid
         *
         *  @param aUserName Username
         *  @return Validity of the given username
         */
        TBool IsValidUserName( const TDesC8& aUserName ) const;

        /**
         *  Parses username, initializes iUserName
         *
         *  @param aArray Array containing all the elements
         */
        void ParseUserNameL( RArray<TPtrC8>& aArray );

        /**
         *  Parses session ID and version, initializes iSessionId
         *  and iSessionVersion
         *
         *  @param aArray Array containing all the elements
         */
        void ParseSessionIDAndVersionL( RArray<TPtrC8>& aArray );

        /**
         *  Parses network type and address type, initializes
         *  iNetType and iAddressType
         *
         *  @param aArray Array containing all the elements
         */
       void ParseNetTypeAndAddressTypeL( RArray<TPtrC8>& aArray );

        /**
         *  Parses address, checks address against address type,
         *  initializes iAddress
         *
         *  @param aArray Array containing all the elements
         */
        void ParseAddressL( RArray<TPtrC8>& aArray );

		inline CSdpOriginFieldPtrs& OriginFieldPtrs();

		inline const CSdpOriginFieldPtrs& OriginFieldPtrs() const;

		TInt64 Get63Msbs( const TDesC8& aDecimalValue ) const;
		void SetIPAddressType( const TInetAddr& aAddr );

    private:    // Data

        RStringPool iPool;

        // <username>
		HBufC8* iUserName;
        // <session id>
		TInt64 iSessionId;
        // <version>
		TInt64 iSessionVersion;
        // <net type>
        RStringF iNetType;
        // <address type>        
        RStringF iAddressType;
        // <address>
        // Either one is used (this or iAddressType/iAddress combo)
        // Can be defined as mutable because this variable does not affect
        // to the state of the object, but it is needed as const pointer
        // is returned on InetAddr() method
		mutable TInetAddr iUnicastAddress;        
        TBuf8<KMaxAddressLength> iAddress; 
        // For configuring the address into TInetAddr
        TBuf<KMaxAddressLength> iAddress16;          

		void __DbgTestInvariant() const;

	};

#endif // CSDPORIGINFIELD_H
