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
* Name          : SdpConnectionField.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef CSDPCONNECTIONFIELD_H
#define CSDPCONNECTIONFIELD_H

//  INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include <stringpool.h>
#include "_sdpdefs.h"

// CONSTANTS
const TUint KDefaultNumOfAddress = 1;

// FORWARD DECLARATIONS
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
 *  @publishedAll
 *  @released
 *
 *	This class encapsulates the connection information field of 
 *  the Session Description Protocol.
 *
 *	The normative reference for correct formatting and values is
 *	draft-ietf-mmusic-sdp-new-14 unless specified otherwise in
 *  member documentation. The implementation supports this normative
 *  reference, but does not enforce it fully. 
 *
 *  @lib localsdpcodec.lib
 */
class CSdpConnectionField : public CBase
	{
    public: // Constructors and destructor

        /**
         *	Constructs a new connection field.
		 * 
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* DecodeL( const TDesC8& aText );

        /**
         *	Constructs a new connection field and adds the pointer to the 
         *  cleanup stack.
		 *	
         *	@param aText A string containing a correctly 
         *         formatted field value terminated by a CRLF.
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* DecodeLC( const TDesC8& aText );

        /**
         *	Constructs a new connection field.
		 *	Also sets the network type to "IN" and address type to "IP4" or
		 *  "IP6" depending on the address family of aAddress.
		 *	
		 *	@param aAddress IP address from either KAfInet 
         *         or KAfInet6 family
         *  @param aTTL Time-To-Live for IP4 multicasts, set it as
         *         KErrNotFound if IPv6 multicast or IPv4 unicast
         *  @param aNumOfAddress Number of addresses in multicast,
         *         if unicast, must be 1
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* NewL(            
            const TInetAddr& aAddress,
            TInt aTTL = KErrNotFound, 
            TUint aNumOfAddress = KDefaultNumOfAddress );

        /**
         *	Constructs a new connection field.
		 *	
         *	@param aNetType A valid network type name from the pre-
         *              defined SDP string table or defined by the user.
		 *  @paramaAddressType A valid address type name from the 
         *              pre-defined SDP string table or defined by the user.
		 *	@param  const TDesC8& aAddress: A valid address of the address type.
         *              Can contain TTL & number of addresses parameter as well.
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* NewL(
			RStringF aNetType, 
			RStringF aAddressType, 
			const TDesC8& aAddress );

		/**
         *	Constructs a new connection field and adds the pointer to the 
         *  cleanup stack. Also sets the network type to "IN" and address type
		 *	to "IP4" or "IP6" depending on the address family of aAddress.
		 *
		 *	@param aAddress IP address from either KAfInet 
         *              or KAfInet6 family
         *  @param aTTL Time-To-Live for IP4 multicasts, set it as
         *              KErrNotFound if IPv6 multicast or IPv4 unicast
         *  @param aNumOfAddress Number of addresses in multicast,
         *              if unicast, must be 1
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* NewLC(       
            const TInetAddr& aAddress, 
            TInt aTTL = KErrNotFound,
            TUint aNumOfAddress = KDefaultNumOfAddress );

        /**
         *	Construct a new connection field  and adds the pointer to the
         *  cleanup stack.
		 *	
         *	@param aNetType A valid network type name from the pre-
         *         defined SDP string table or defined by the user
		 *  @paramaAddressType A valid address type name from the 
         *              pre-defined SDP string table or defined by the user
		 *	@param  const TDesC8& aAddress: A valid address of the address type.
         *              Can contain TTL & number of addresses parameter as well.
         *	@return a new instance.
         */
		IMPORT_C static CSdpConnectionField* NewLC(
            RStringF aNetType, RStringF aAddressType, const TDesC8& aAddress );

		/**
		 *	Deletes the resources held by the instance.
		 */
		IMPORT_C ~CSdpConnectionField();

    public: // New functions

		/**
		 *	Outputs the field formatted according to SDP syntax and including
		 *  the terminating CRLF.
		 * 
		 *	@param aStream: Stream used for output. On return the
         *         stream includes correctly formatted connection field.
		 */
		IMPORT_C void EncodeL( RWriteStream& aStream ) const;

		/**
		 *	Creates a new instance that is equal to the target.
		 *
		 *	@return a new instance.
		 */
		IMPORT_C CSdpConnectionField* CloneL() const;

		/**	
		 *	Compares this instance to another for equality.
		 *
		 *	@param const CSdpConnectionField& aObj: The instance to compare to.
		 *	@return ETrue if equal, EFalse if not.
		 */
		IMPORT_C TBool operator== ( const CSdpConnectionField& aObj ) const;

		/**
		 *	Gets the network type that is from the pre-defined SDP string table
         *  or given by the user.
		 *
		 *	@return The network type.
		 */
		IMPORT_C RStringF NetType() const;

		/**
		 *	Gets the address type that is from the pre-defined SDP string table 
         *  or given by the user.
		 *
		 *	@return The address type.
		 */
		IMPORT_C RStringF AddressType() const;

		/**
		 *	Gets the address.
		 *
		 *	@return Address as an IP address or null if it is
         *          not an IP address. This may be e.g. when the address
		 *          has been specified as a FQDN. In this case, the address
         *          can be accessed using the other getters.
		 */
		IMPORT_C const TInetAddr* InetAddress() const;

		/**
		 *	Gets the address.
		 *
		 *  @return Address as a string.
		 */
		IMPORT_C const TDesC8& Address() const;

		/**
		 *	Sets the address, network and address type. Also sets the network 
		 *	type to "IN" and address type to "IP4" or "IP6" depending on the 
		 *  address family of aAddress.
		 *
		 *	@param aValue The new address.
         *  @param aTTL Time-To-Live for IP4 multicasts, set it as
         *         KErrNotFound if IPv6 multicast or IPv4 unicast
         *  @param aNumOfAddress Number of addresses in multicast,
         *         if unicast, must be 1.
         *  @leave KErrSdpCodecConnectionField ifaddress to be set is wrongly 
         *         formatted.
         */
		IMPORT_C void SetInetAddressL( const TInetAddr& aValue, 
                                   TInt aTTL = KErrNotFound,
                                   TUint aNumOfAddress = KDefaultNumOfAddress);

		/**
		 *	Sets the address, network and address type.
		 *
         *	@param aNetType A valid network type name from the pre-
         *         defined SDP string table or defined by the user
		 *  @param aAddressType A valid address type name from the 
         *         pre-defined SDP string table or defined by the user
		 *	@param aAddress A valid address of the address type.
		 */
		IMPORT_C void SetAddressL( RStringF aNetType, 
								   RStringF aAddressType, 
								   const TDesC8& aAddress );

        /**
         *  Gets TTL attribute.
         *
         *  @return TTL or KErrNotFound, if one is not available for the address
         */
        IMPORT_C TInt TTLValue() const;

        /**
         *  Sets TTL attribute (only valid for IP4 multicasts).
         *  Leaves if trying to set TTL to address that doesn't support it.
         *
         *  @param aTTL Time-To-Live for IP4 multicasts
         *	@leave KErrSdpCodecConnectionField if aTTL is invalid.
         */
        IMPORT_C void SetTTLL( TInt aTTL );

        /**
         *  Gets the number of addresses (can be more than 1 for multicasts).
         *  Multicast addresses are contiguously allocated above the base 
         *  address.
         *
         *  @return Number of addresses
         */
        IMPORT_C TInt NumOfAddress() const;
         
        /**
         *  Sets the number of addreses allocated for multicast. 
         *  
         *	@param aNumOfAddress Number of addresses in multicast
         *  @leave KErrSdpCodecConnectionField if the address is unicast.
         */
        IMPORT_C void SetNumOfAddressL( TUint aNumOfAddress );

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
		static CSdpConnectionField* InternalizeL( RReadStream& aStream );

    private:    // Internal

        /**
         *  Constructor
         */
        CSdpConnectionField();

        /**
         *  2nd phase constructor
         *
         *	@param aText A string containing a correctly formatted field value
		 *         terminated by a CRLF.         
         */         
        void ConstructL( const TDesC8& aText );
        
        /**
         *  2nd phase constructor
         *
         *	@param aAddress IP address from either KAfInet or KAfInet6 family
         *  @param aTTL Time-To-Live for IP4 multicasts
         *  @param aNumOfAddress Nubmer of addresses in multicast
         */
        void ConstructL( const TInetAddr& aAddress,
                         TInt aTTL, TUint aNumOfAddress );

        /**
         *  2nd phase constructor
         *
         *	@param aNetType A valid network type name from the pre-defined
		 *         SDP string table or defined by the user
		 *  @param aAddressType A valid address type name from the pre-defined
		 *         SDP string table or defined by the user
		 *	@param aAddress A valid address of the address type
         */
        void ConstructL( RStringF aNetType, RStringF aAddressType, 
	                     const TDesC8& aAddress );

        /**
         *  Checks if the address is valid against given arguments
         *
         *  @param aAddressTypeIP4 The given type of address (EFalse = IP6)
         *  @param aAddress Address with possibly TTL & number of addresses
         *  @return error code (KErrNone if valid)
         */
        TInt IsValidAddress( TBool aAddressTypeIP4,
                             const TDesC8& aAddress ) const;

        /**
         *  Checks if the address is valid against given arguments
         *         
         *  @param aAddress Address in TInetAddr format
         *  @param aTTL TTL attribute
         *  @param aNumOfAddress Number off addresses
         *  @return error code (KErrNone if valid)
         */
        TInt IsValidAddress( const TInetAddr& aAddress, 
                             TInt aTTL, TUint aNumOfAddress ) const;
        
        /**
         *  Parses address field
         *
         *  @param aAddressTypeIP4 The given type of address (EFalse = IP6)
         *  @param aAddress Address with possibly TTL & number of addresses
         *  @param aTTL TTL value is stored here (or KErrNotFound)
         *  @param aNumberOfAddresses Range of addreses
         *  @return The address
         */
        HBufC8* ParseAddressFieldL( TBool aAddressTypeIP4,
                                    const TDesC8& aAddress,
                                    TInt& aTTL,
                                    TUint& aNumberOfAddresses ) const;

        /**
         *  Parses IP4 address
         *
         *  @param aPos Position of the (first) separation mark         
         *  @param aTTL TTL value is stored here (or KErrNotFound)
         *  @param aAddr Addres in TInetAddr format
         *  @param aAddress The whole address field
         *  @param aNumberOfAddresses Range of addreses
         *  @return The address
         */
        HBufC8* ParseIP4AddressL( TInt aPos, 
                                  TInetAddr& aAddr,
                                  const TDesC8& aAddress,
                                  TInt& aTTL,
                                  TUint& aNumberOfAddresses ) const;

        /**
         *  Parses IP6 address
         *
         *  @param aPos Position of the (first) separation mark         
         *  @param aTTL TTL value is stored here (or KErrNotFound)
         *  @param aAddr Addres in TInetAddr format
         *  @param aAddress The whole address field
         *  @param aNumberOfAddresses Range of addreses
         *  @return The address
         */
        HBufC8* ParseIP6AddressL( TInt aPos, 
                                  TInetAddr& aAddr,
                                  const TDesC8& aAddress,
                                  TInt& aTTL,
                                  TUint& aNumberOfAddresses ) const;

        /**
         *  Copies given network type to iNetType and verifies aNetType to
         *  be valid
         *
         *  @param aNetType Given network type                
         */
        void CopyNetTypeL( const TDesC8& aNetType );

        /**
         *  Copies given address type to iAddressType and verifies aAddrType
         *  to be valid
         * 
         *  @param aAddrType Given address type     
         */
        void CopyAddressTypeL( const TDesC8& aAddrType );

        /**
         *  Copies address to iAddress, and initalizes TTL & number of addresses,
         *  leaves on error
         *  
         *  @param aAddress Address string, which can also contain TTL
         *                  and number of addresses attributes
         */
        void CopyAddressL( const TDesC8& aAddress, RStringPool aPool );

    private:    // Data

        // <network type>
        RStringF iNetType;
        
        // <address type>        
        RStringF iAddressType;

        // mutable TInetAddr for InetAddress()
        mutable TInetAddr iInetAddress;

        // Address in text format
        HBufC8* iAddress;

        // TTL for IP4 multicast addresses
        TInt iTTL;
        // Number of addresses
        TUint iNumOfAddress;

        // String pool
        RStringPool iPool;

		void __DbgTestInvariant() const;


	};

#endif // CSDPCONNECTIONFIELD_H
