// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : SdpConnectionField.cpp
// Part of       : Local SDP Codec
// Version       : 1.0
//




#include <in_sock.h>
#include <s32strm.h>
#include <stringpool.h>
#include "sdpconnectionfield.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecconstants.h"
#include "sdpcodecstringpool.h"
#include "sdputil.h"
#include "sdpcodecerr.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"
#include "uriutils.h"

// LOCAL CONSTANTS
const TInt KHeaderIndex = 0;
const TInt KNetworkTypeIndex = 1;
const TInt KAddressTypeIndex = 2;
const TInt KAddressIndex = 3;
const TInt KTokenCount = 4;
const TInt KMaxAddressLength = 256;
const TUint KMaxIPDesLength = 39;
_LIT16(KWildAddr, "0.0.0.0");
_LIT16(KWildAddrIPv6, "::");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSdpConnectionField::CSdpConnectionField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSdpConnectionField::CSdpConnectionField() :
    iTTL( KErrNotFound ),
    iNumOfAddress( KDefaultNumOfAddress )
	{
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::ConstructL(
    const TDesC8& aText )
	{
    // aText should be in format
    // c=<network type> <address type> <connection address>
    iPool = SdpCodecStringPool::StringPoolL();

    RArray<TPtrC8> array =
        SdpUtil::GetElementsFromLineL( aText, KErrSdpCodecConnectionField );
    CleanupClosePushL( array );

    if ( array.Count() != KTokenCount )
        {
        User::Leave( KErrSdpCodecConnectionField );
        }

    RStringF origHeader =  iPool.StringF( SdpCodecStringConstants::EConnection,
                                          SdpCodecStringConstants::Table );
    if ( origHeader.DesC().CompareF( array[KHeaderIndex] ) != 0 )
        {
        // Header didn't match
        User::Leave( KErrSdpCodecConnectionField );
        }

    // <network type>
    CopyNetTypeL( array[KNetworkTypeIndex] );

    // <address type>
    CopyAddressTypeL( array[KAddressTypeIndex] );

    // <address>
    CopyAddressL( array[KAddressIndex], iPool );

    CleanupStack::PopAndDestroy();  // array
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::ConstructL(
    const TInetAddr& aAddress,
    TInt aTTL,
    TUint aNumOfAddress )
	{
    __ASSERT_ALWAYS( IsValidAddress(aAddress, aTTL, aNumOfAddress) == KErrNone,
                     User::Leave( KErrSdpCodecConnectionField ) );

    iPool = SdpCodecStringPool::StringPoolL();
    TBuf<KMaxIPDesLength> addr;
    //If aAddress is IPv4-Mapped IPv6,the result of Output is IPv4
    aAddress.Output( addr );

    // Copy address
    iAddress = HBufC8::NewL( addr.Length() );
    iAddress->Des().Copy( addr );

    // TTL
    iTTL = aTTL;
    // Num of addresses
    iNumOfAddress = aNumOfAddress;
    // Network type
    iNetType = iPool.StringF( SdpCodecStringConstants::ENetType,
                              SdpCodecStringConstants::Table ).Copy();
    // Address type
	if ( UriUtils::HostType( addr ) == UriUtils::EIPv4Host )
		{
		//addresstype for IPv4
			iAddressType = iPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                                      SdpCodecStringConstants::Table ).Copy();
			}
	else if ( UriUtils::HostType( addr ) == UriUtils::EIPv6Host )
			{
			//addresstype IPv6
			iAddressType = iPool.StringF( SdpCodecStringConstants::EAddressType,
                                      SdpCodecStringConstants::Table ).Copy();
			}
	else
		{
		User::Leave(KErrArgument);
		}

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::ConstructL(
    RStringF aNetType,
	RStringF aAddressType,
	const TDesC8& aAddress )
    {
    iPool = SdpCodecStringPool::StringPoolL();

    // Network type
    CopyNetTypeL( aNetType.DesC() );

    // Address type
    CopyAddressTypeL( aAddressType.DesC() );

    // Address + TTL + numOfAddress
    CopyAddressL( aAddress, aNetType.Pool() );
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::DecodeL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::DecodeL(
    const TDesC8& aText )
	{
	CSdpConnectionField* obj = CSdpConnectionField::DecodeLC( aText );
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::DecodeLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::DecodeLC(
    const TDesC8& aText )
	{
	CSdpConnectionField* obj = new ( ELeave ) CSdpConnectionField;
	CleanupStack::PushL( obj );
	obj->ConstructL( aText );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::NewL(
    const TInetAddr& aAddress,
    TInt aTTL,
    TUint aNumOfAddress )
	{
	CSdpConnectionField* obj = CSdpConnectionField::NewLC(
        aAddress, aTTL, aNumOfAddress );
	CleanupStack::Pop();
	return obj;
	}
// -----------------------------------------------------------------------------
// CSdpConnectionField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::NewLC(
    const TInetAddr& aAddress,
    TInt aTTL,
    TUint aNumOfAddress )
	{
	CSdpConnectionField* obj = new ( ELeave ) CSdpConnectionField;
	CleanupStack::PushL( obj );
	obj->ConstructL( aAddress, aTTL, aNumOfAddress );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::NewL(
    RStringF aNetType,
    RStringF aAddressType,
    const TDesC8& aAddress )
    {
    CSdpConnectionField* obj = CSdpConnectionField::NewLC(
        aNetType, aAddressType, aAddress );
	CleanupStack::Pop();
	return obj;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::NewLC(
    RStringF aNetType,
    RStringF aAddressType,
    const TDesC8& aAddress )
    {
    CSdpConnectionField* obj = new ( ELeave ) CSdpConnectionField;
	CleanupStack::PushL( obj );
	obj->ConstructL( aNetType, aAddressType, aAddress );
	return obj;
    }

// Destructor
EXPORT_C CSdpConnectionField::~CSdpConnectionField()
	{
    delete iAddress;
    iNetType.Close();
    iAddressType.Close();
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpConnectionField::EncodeL(
    RWriteStream& aStream ) const
	{
	__TEST_INVARIANT;
    TBuf8<20> number;  // Is enough for addr as well

    // c=<network type> <address type> <connection address>
    RStringF header = iPool.StringF( SdpCodecStringConstants::EConnection,
                                     SdpCodecStringConstants::Table );
    aStream.WriteL( header.DesC() );

    // Network type
    aStream.WriteL( iNetType.DesC() );
    aStream.WriteL( KSPStr );

    // Address type
    aStream.WriteL( iAddressType.DesC() );
    aStream.WriteL( KSPStr );

    // Connection address
    aStream.WriteL( *iAddress );
    if ( iTTL != KErrNotFound )
        {
        number.Zero();
        number.Append( KSlashStr );
        number.AppendFormat( _L8("%d"), iTTL );
        aStream.WriteL( number );
        }

    if ( iNumOfAddress > 1 )
        {
        number.Zero();
        number.Append( KSlashStr );
        number.AppendFormat( _L8("%u"), iNumOfAddress );
        aStream.WriteL( number );
        }

     // End-of-Line mark
	aStream.WriteL( KCRLFStr );
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::CloneL
// Clones an exact copy of this object
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpConnectionField* CSdpConnectionField::CloneL() const
	{
	__TEST_INVARIANT;

	CSdpConnectionField* obj = NULL;
    const TInetAddr* addr = InetAddress();

    if ( addr )
        {
        // Could be anything
        obj = CSdpConnectionField::NewL( *addr, iTTL, iNumOfAddress );
        }
    else
        {
        // Unicast FQDN address
        obj = CSdpConnectionField::NewL( iNetType, iAddressType, *iAddress );
        }

	__ASSERT_DEBUG( *this == *obj, User::Panic(
                                   KSdpCodecPanicCat, KSdpCodecPanicInternal));
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::operator==
// Checks if two origin fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpConnectionField::operator== (
    const CSdpConnectionField& aObj ) const
	{
	__TEST_INVARIANT;

    TBool equal( EFalse );

    if ( (*iAddress).CompareF( aObj.Address() ) == 0 &&
         NetType() == aObj.NetType() &&
         AddressType() == aObj.AddressType() &&
         TTLValue() == aObj.TTLValue() &&
         NumOfAddress() == aObj.NumOfAddress() )
        {
        // All attributes match, two fields are equal
        equal = ETrue;
        }

	return equal;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::NetType
// Returns network type
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpConnectionField::NetType() const
	{
	__TEST_INVARIANT;
	return iNetType;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::AddressType
// Returns address type
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpConnectionField::AddressType() const
	{
	__TEST_INVARIANT;
	return iAddressType;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::InetAddress
// Returns address in TInetAddr format if possible
// -----------------------------------------------------------------------------
//
EXPORT_C const TInetAddr* CSdpConnectionField::InetAddress() const
	{
	__TEST_INVARIANT;

    TInetAddr* addr = NULL;
    TBuf<KMaxAddressLength> addr16;

    addr16.Copy( iAddress->Des() );
    TInt err = iInetAddress.Input( addr16 );

    if ( err == KErrNone )
        {
        addr = &iInetAddress;
        }

	return addr;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::Address
// Returns address in TDesC8 format
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpConnectionField::Address() const
    {
    return *iAddress;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::SetInetAddressL
// Sets address from TInetAddr
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpConnectionField::SetInetAddressL(
    const TInetAddr& aAddress,
    TInt aTTL,
    TUint aNumOfAddress )
	{
	__TEST_INVARIANT;

    if ( IsValidAddress( aAddress, aTTL, aNumOfAddress ) == KErrNone )
        {
        TBuf<KMaxAddressLength> addr16;
       	//If aAddress IPv4-Mapped IPv6, result of Output IPv4
         aAddress.Output( addr16 );

        HBufC8* tempBuf = HBufC8::NewL( addr16.Length() );
        tempBuf->Des().Copy( addr16 );

        SdpUtil::SetDefaultNetTypeAndAddrType(
            iPool, aAddress, iNetType, iAddressType );
        if(aAddress.Address() && aAddress.IsV4Mapped())
			{
			iAddressType.Close();
			iAddressType = iPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                                      SdpCodecStringConstants::Table ).Copy();
			}
        // Set address
        delete iAddress;
        iAddress = tempBuf;
        // Set attributes for address
        iTTL = aTTL;
        iNumOfAddress = aNumOfAddress;
        }
    else
        {
        User::Leave( KErrSdpCodecConnectionField );
        }

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpConnectionField::SetAddressL
// Sets new address and types, leaves on error
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpConnectionField::SetAddressL(
    RStringF aNetType,
    RStringF aAddressType,
    const TDesC8& aAddress )
    {
    __TEST_INVARIANT;
    TInt err( KErrNone );
    RStringF netTypeBackup = iNetType.Copy();
    RStringF addressTypeBackup = iAddressType.Copy();
    CleanupClosePushL( netTypeBackup );
    CleanupClosePushL( addressTypeBackup );

    TRAP( err, CopyNetTypeL( aNetType.DesC() ) );
    if ( !err )
        {
        // Netowrk type changed successfully
        TRAP( err, CopyAddressTypeL( aAddressType.DesC() ) );
        if ( !err )
            {
            // Address type & network type changed successfully
            TRAP( err, CopyAddressL( aAddress, iAddressType.Pool() ) );

            if ( err )
                {
                // Place back old netType & address
                iNetType.Close();
                iNetType = netTypeBackup.Copy();

                iAddressType.Close();
                iAddressType = addressTypeBackup.Copy();

                User::LeaveIfError( err );
                }
            // Everything changed succesfully
            }
        else
            {
            // Place back old netType
            iNetType.Close();
            iNetType = netTypeBackup.Copy();

            User::LeaveIfError( err );
            }
        }
    else
        {
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( 2 );   // netTypeBackup, addressTypeBackup
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::TTLValue
// Returns TTL attribute
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSdpConnectionField::TTLValue() const
    {
    __TEST_INVARIANT;
    return iTTL;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::SetTTLL
// Sets new TTL attribute value, leaves on error
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpConnectionField::SetTTLL(
    TInt aTTL )
    {
    __TEST_INVARIANT;

    const TInetAddr* addr = InetAddress();

    // FQDN; Unicast, can be only KErrNotFound
    // IP4 Multicast: 0 <= TTL <= 255
    // IP4 Unicast, can be only KErrNotFound
    // IP6 can be only KErrNotFound

    if ( ( !addr && aTTL != KErrNotFound ) ||
         ( addr && addr->Address() && addr->IsMulticast() && 
           ( aTTL < 0 || aTTL > 255 ) ) ||
         ( addr && addr->Address() && !addr->IsMulticast() && 
           aTTL != KErrNotFound ) ||
         ( addr && !addr->Address() && aTTL != KErrNotFound ) )
        {
        User::Leave( KErrSdpCodecConnectionField );
        }

    iTTL = aTTL;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::NumOfAddress
// Returns number of continuous addresses for multicast
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSdpConnectionField::NumOfAddress() const
    {
    return iNumOfAddress;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::SetNumOfAddressL
// Sets number of addresses
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpConnectionField::SetNumOfAddressL(
    TUint aNumOfAddress )
    {
    const TInetAddr* addr = InetAddress();
    if ( aNumOfAddress < 1 || ( !addr && aNumOfAddress != 1 )  ||
         ( addr && !addr->IsMulticast() && aNumOfAddress != 1  ) )
        {
        User::Leave( KErrSdpCodecConnectionField );
        }

    iNumOfAddress = aNumOfAddress;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::ExternalizeL
// Externalizes the object to stream
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::ExternalizeL(
    RWriteStream& aStream ) const
    {
    __TEST_INVARIANT;

    // Address
    aStream.WriteUint32L( iAddress->Des().Length() );
    aStream.WriteL( iAddress->Des(), iAddress->Des().Length() );
    // TTL
    aStream.WriteInt32L( iTTL );
    // Number of addresses
    aStream.WriteInt32L( iNumOfAddress );
    // <network type>
    aStream.WriteUint32L( iNetType.DesC().Length() );
    aStream.WriteL( iNetType.DesC() );
    // <address type>
    aStream.WriteUint32L( iAddressType.DesC().Length() );
    aStream.WriteL( iAddressType.DesC() );
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::InternalizeL
// Internalizes the object from stream
// -----------------------------------------------------------------------------
//
CSdpConnectionField* CSdpConnectionField::InternalizeL(
    RReadStream& aStream )
    {
    TUint32 length;
    CSdpConnectionField* obj = NULL;
    RStringPool pool = SdpCodecStringPool::StringPoolL();

    // address
    length = aStream.ReadUint32L();
	HBufC8* address = HBufC8::NewLC( length );
    TPtr8 ptr( address->Des() );
    aStream.ReadL( ptr, length );
    // TTL
    TInt ttl = aStream.ReadInt32L();
    // Number of addresses
    TInt numOfAddr = aStream.ReadInt32L();
    // Network type
    length = aStream.ReadUint32L();
    HBufC8* netType = HBufC8::NewLC( length );
    ptr.Set( netType->Des() );
    aStream.ReadL( ptr, length );
    // Address type
    length = aStream.ReadUint32L();
    HBufC8* addrType = HBufC8::NewLC( length );
    ptr.Set( addrType->Des() );
    aStream.ReadL( ptr, length );

    TInetAddr addr;
    HBufC* address16 = HBufC::NewLC( address->Length() ) ;
    address16->Des().Copy( *address );
    TInt err = addr.Input( *address16 );

    if ( !err )
        {
        // Create connection field based on TInetAddr
        obj = CSdpConnectionField::NewL( addr, ttl, numOfAddr );
        }
    else
        {
        // Create connection field from buffer, ttl & numOfAddr
        // are in this case irrelevant
        RStringF netTypeStr = pool.OpenFStringL( *netType );
        CleanupClosePushL( netTypeStr );
        RStringF addrTypeStr = pool.OpenFStringL( *addrType );
        CleanupClosePushL( addrTypeStr );
        obj = CSdpConnectionField::NewL( netTypeStr, addrTypeStr, *address );
        CleanupStack::PopAndDestroy( 2 );   // netTypeStr, addrTypeStr
        }

    CleanupStack::PopAndDestroy( 4 );  // address, netType, addrType, address16
    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::IsValidAddress
// Checks if the given address is valid
// -----------------------------------------------------------------------------
//
TInt CSdpConnectionField::IsValidAddress(
    TBool aAddressTypeIP4,
    const TDesC8& aAddress ) const
    {
    HBufC8* address = NULL;
    TInt ttl( 0 );
    TUint num( 0 );

    TRAPD( err, address =
                ParseAddressFieldL( aAddressTypeIP4, aAddress, ttl, num ) );
    delete address;

    return err;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::IsValidAddress
// Checks if the given address is valid
// -----------------------------------------------------------------------------
//
TInt CSdpConnectionField::IsValidAddress(
    const TInetAddr& aAddress,
    TInt aTTL,
    TUint aNumOfAddress ) const
    {
    TInt err( KErrSdpCodecConnectionField );
    // 0 <= TTL <= 255 or KErrNotFound if not defined
    // Number of addresses > 0
    if ( ( aTTL == KErrNotFound || ( aTTL >= 0 && aTTL <= 255 ) ) &&
           aNumOfAddress > 0 )
        {
        TBuf16 <KMaxAddressLength> output;
        aAddress.Output(output);

        // Address has to be either of type IP4 or IP6
        // If IP4 and multicast then it must contain TTL attribute
        // If IP4 and unicast then it must NOT contain TTL and must have 1 address
        // If IP6 and multicast then it must NOT contain TTL
        // If IP6 and unicast then it must NOT contain TTL and must have 1 address
        if ( ( aAddress.Address() && 
               ( ( aAddress.IsMulticast() && aTTL != KErrNotFound ) ||
                 ( aAddress.IsUnicast() && aTTL == KErrNotFound &&
                   aNumOfAddress == 1 ) ) ) ||
             ( !aAddress.Address() &&
               ( ( aAddress.IsMulticast() && aTTL == KErrNotFound ) ||
                 ( aAddress.IsUnicast() && aTTL == KErrNotFound &&
                   aNumOfAddress == 1 ) )   ||
               ( aAddress.IsWildAddr() && 
               ( output.Match(KWildAddr) == 0||
               	 output.Match(KWildAddrIPv6) == 0 )) ) )
           {
           err = KErrNone;
           }
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::ParseAddressFieldL
// Format the address field
// -----------------------------------------------------------------------------
//
HBufC8* CSdpConnectionField::ParseAddressFieldL(
    TBool aAddressTypeIP4,
    const TDesC8& aAddress,
    TInt& aTTL,
    TUint& aNumberOfAddresses ) const
    {
    HBufC8* address = NULL;

    if ( aAddress.Length() > 0 && aAddress.Length() <= KMaxAddressLength )
        {
        // IP4 address
        TInetAddr addr;
        TBuf<KMaxAddressLength> buf16;

        // IP address lasts till first separator mark
        TInt pos = aAddress.Locate( KSlashChar );
        if ( pos == KErrNotFound )
            {
            pos = aAddress.Length();
            }
        buf16.Copy( aAddress.Left( pos ) );

        // Try to convert the string to TInetAddr
        TInt err = addr.Input( buf16 );

   		//Type and address check
        TBuf16 <KMaxAddressLength> output;
        addr.Output(output);
        if (err == KErrNone)
	        {
            if (addr.Address() )
                {
                if ( (addr.IsV4Mapped() && aAddressTypeIP4) ||
                     (!addr.IsV4Mapped() && !aAddressTypeIP4) )
                    {
                    User::Leave( KErrSdpCodecConnectionField );
                    }
                else
                    {
                    // IP4 address
                    address = ParseIP4AddressL( pos, addr, aAddress, aTTL,
                                            aNumberOfAddresses );
                    }
                }
            else
                {
                if ( !aAddressTypeIP4)
                    {
                    // IP6 address
                    address = ParseIP6AddressL( pos, addr, aAddress, aTTL,
                                        aNumberOfAddresses );
                    }
                else if ( addr.IsWildAddr() && output.Match(KWildAddr) == 0 )
                    {
                    // 0.0.0.0
                    address = ParseIP4AddressL( pos, addr, aAddress, aTTL,
                                            aNumberOfAddresses );
                    }
                else
                    {
                    User::Leave( KErrSdpCodecConnectionField );
                    }
		        }
	        }
        else
	        {   
            // FQDN
	        // Clarify that the address contains only valid FQDN chars
            // This is always unicast
            for ( TInt i( 0 ); i < aAddress.Length(); i++ )
		        {
                if ( KValidFQDNChars().Locate( aAddress[i] ) ==
			        KErrNotFound )
                    {
                    User::Leave( KErrSdpCodecConnectionField );
                    }
                }
            address = aAddress.AllocL();
            aTTL = KErrNotFound;
            aNumberOfAddresses = 1;
	        }

        }
    else
        {
        User::Leave( KErrSdpCodecConnectionField );
        }

    return address;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::ParseIP4AddressL
// Parses IP4 address and checks that it is valid
// -----------------------------------------------------------------------------
//
HBufC8* CSdpConnectionField::ParseIP4AddressL(
    TInt aPos,
    TInetAddr& aAddr,
    const TDesC8& aAddress,
    TInt& aTTL,
    TUint& aNumberOfAddresses ) const
    {
    HBufC8* address = NULL;

    if ( aAddr.IsMulticast() )
        {
        // Multicast address - MUST contain TTL and possibly
        // number of addresses
        if ( aPos != aAddress.Length() )
            {
            TInt pos2 = aAddress.Right( aAddress.Length() - aPos - 1 ).Locate(
                KSlashChar );

            if ( pos2 != KErrNotFound )
                {
                pos2 += aPos + 1;

                // Validity check for number strings
                TBool digit = SdpUtil::IsDigit(
                    aAddress.Mid( aPos + 1, pos2 - aPos - 1 ) );
                digit = digit && SdpUtil::IsDigit(
                    aAddress.Mid( pos2 + 1, aAddress.Length() - pos2 - 1 ) );
                if ( !digit )
                    {
                    User::Leave( KErrSdpCodecConnectionField );
                    }

                // TTL & number of addresses
                User::LeaveIfError(
                    TLex8( aAddress.Mid( aPos + 1, pos2 - aPos - 1 ) ).Val(
                        aTTL ) );
                User::LeaveIfError(
                    TLex8( aAddress.Mid(
                        pos2 + 1, aAddress.Length() - pos2 - 1 ) ).Val(
                            aNumberOfAddresses, EDecimal ) );

                __ASSERT_ALWAYS( aTTL >= 0
                                && aTTL <= 255
                                && aNumberOfAddresses > 0,
                                User::Leave( KErrSdpCodecConnectionField ) );
                }
            else
                {
                // Validity check for number strings
                TBool digit = SdpUtil::IsDigit(
                   aAddress.Mid( aPos + 1, aAddress.Length() - aPos - 1 ) );
                if ( !digit )
                    {
                    User::Leave( KErrSdpCodecConnectionField );
                    }

                // Plain TTL
                User::LeaveIfError(
                    TLex8( aAddress.Mid(
                        aPos + 1, aAddress.Length() - aPos - 1 ) ).Val(
                            aTTL ) );
                aNumberOfAddresses = 1;

                __ASSERT_ALWAYS( aTTL >= 0 && aTTL <= 255,
                                 User::Leave( KErrSdpCodecConnectionField ) );
                }

            address = aAddress.Left( aPos ).AllocL();
            }
        else
            {
            User::Leave( KErrSdpCodecConnectionField );
            }
        }
    else
        {
        // Unicast address - cannot contain any multicast TTLs etc.
        if ( aPos == aAddress.Length() )
            {
            address = aAddress.AllocL();
            aTTL = KErrNotFound;
            aNumberOfAddresses = 1;
            }
        else
            {
            User::Leave( KErrSdpCodecConnectionField );
            }
        }

    return address;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::ParseIP6AddressL
// Parses IP6 address and checks that it is valid
// -----------------------------------------------------------------------------
//
HBufC8* CSdpConnectionField::ParseIP6AddressL(
    TInt aPos,
    TInetAddr& aAddr,
    const TDesC8& aAddress,
    TInt& aTTL,
    TUint& aNumberOfAddresses ) const
    {
    HBufC8* address = NULL;

    if ( aAddr.IsMulticast() )
        {
        // Multicast address - CAN contain one extra attribute,
        // number of addresses
        if ( aPos != aAddress.Length() )
            {
            // Validity check for number strings

            TBool digit = SdpUtil::IsDigit(
                aAddress.Mid( aPos + 1, aAddress.Length() - aPos - 1 ) );
            if ( !digit )
                {
                User::Leave( KErrSdpCodecConnectionField );
                }

            User::LeaveIfError( TLex8( aAddress.Mid(
                aPos + 1, aAddress.Length() - aPos - 1 ) ).Val(
                    aNumberOfAddresses, EDecimal ) );

            __ASSERT_ALWAYS( aNumberOfAddresses > 0,
                             User::Leave( KErrSdpCodecConnectionField ) );
            }
        else
            {
            aNumberOfAddresses = 1;
            }

        aTTL = KErrNotFound;    // IP6 does not use TTL
        address = aAddress.Left( aPos ).AllocL();
        }
    else
        {
        // Unicast address - cannot contain any multicast TTLs etc.
        if ( aPos == aAddress.Length() )
            {
            address = aAddress.AllocL();
            aTTL = KErrNotFound;
            aNumberOfAddresses = 1;
            }
        else
            {
            User::Leave( KErrSdpCodecConnectionField );
            }
        }

    return address;
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::CopyNetTypeL
// Copies network type to object after checking that it is valid
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::CopyNetTypeL(
    const TDesC8& aNetType )
    {
    if ( !SdpUtil::IsToken( aNetType ) )
        {
        User::Leave( KErrSdpCodecConnectionField );
        }
    else
        {
        RStringF netType = iPool.OpenFStringL( aNetType );
        iNetType.Close();
        iNetType = netType;
        }
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::CopyAddressTypeL
// Copies address type after checking that it is valid
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::CopyAddressTypeL(
    const TDesC8& aAddrType )
    {
    if ( !SdpUtil::IsToken( aAddrType ) )
        {
        User::Leave( KErrSdpCodecConnectionField );
        }
    else
        {
        RStringF addrType = iPool.OpenFStringL( aAddrType );
        iAddressType.Close();
        iAddressType = addrType;
        }
    }

// -----------------------------------------------------------------------------
// CSdpConnectionField::CopyAddressL
// Copies address after checking that it is valid
// -----------------------------------------------------------------------------
//
void CSdpConnectionField::CopyAddressL(
    const TDesC8& aAddress,
    RStringPool aPool )
    {
    RStringF addrTypeIP4 = aPool.StringF(
        SdpCodecStringConstants::EAddressTypeIP4,
        SdpCodecStringConstants::Table );
    RStringF addrTypeIP6 = aPool.StringF(
        SdpCodecStringConstants::EAddressType,
        SdpCodecStringConstants::Table );
    HBufC8* addr = NULL;

    if ( iAddressType == addrTypeIP4 || iAddressType == addrTypeIP6 )
        {
        User::LeaveIfError(
            IsValidAddress( iAddressType == addrTypeIP4, aAddress ) );

        addr = ParseAddressFieldL( iAddressType == addrTypeIP4, aAddress,
                               iTTL, iNumOfAddress );
  		//If address is IPv4-Mapped IPv6 , it is changed to IPv4
		TInetAddr inetaddr;
		TBuf<KMaxAddressLength> address;
		address.Copy(*addr);
		TInt err = inetaddr.Input(address);
        TBuf16 <KMaxAddressLength> output;
        inetaddr.Output(output);
		if(!err && 
            ( ( inetaddr.Address() && inetaddr.IsV4Mapped() ) || 
              ( inetaddr.IsWildAddr() && output.Match(KWildAddr) != KErrNotFound ) 
            ) )
			{//IPv4-Mapped IPv6 is changed to IPv4 address
			TBuf<KMaxIPDesLength> buf;
			inetaddr.Output( buf );
			// Copy new address to safe
			delete addr;
			addr = NULL;
			addr = HBufC8::NewL(buf.Length());
			addr->Des().Copy(buf);
			iAddressType.Close();
			iAddressType = 
				iPool.StringF(SdpCodecStringConstants::EAddressTypeIP4,
                               SdpCodecStringConstants::Table ).Copy();
			}
	
        }
    else
        {
        // Address not IP4 or IP6, must be then non-ws-string
        if ( SdpUtil::IsNonWhitespace( aAddress ) )
            {
            addr = aAddress.AllocL();
            }
        else
            {
            User::Leave( KErrSdpCodecConnectionField );
            }
        }

    delete iAddress;
    iAddress = addr;
    }

// For DEBUG builds

// -----------------------------------------------------------------------------
// CSdpConnectionField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//

void CSdpConnectionField::__DbgTestInvariant() const
	{
    TBool validAddr = EFalse;

    RStringF addrTypeIP4 = iPool.StringF(
        SdpCodecStringConstants::EAddressTypeIP4,
        SdpCodecStringConstants::Table );
    RStringF addrTypeIP6 = iPool.StringF(
        SdpCodecStringConstants::EAddressType,
        SdpCodecStringConstants::Table );

    if ( iAddress &&
        ( iAddressType == addrTypeIP4 || iAddressType == addrTypeIP6 ) )
        {
        TInetAddr addr;
        TBuf<KMaxAddressLength> buf16;
        buf16.Copy( *iAddress );
        TInt err = addr.Input( buf16 );
        if ( !err )
            {
            validAddr = IsValidAddress(addr, iTTL, iNumOfAddress) == KErrNone;
            }
        else
            {
            // FQDN unicast
            if ( iTTL == KErrNotFound && iNumOfAddress == 1 )
                {
                validAddr = ETrue;
                }
            }
        }

    TBool invariant = ( SdpUtil::IsToken( iNetType.DesC() ) &&
                        SdpUtil::IsToken( iAddressType.DesC() ) &&
                        iAddress && SdpUtil::IsNonWhitespace( *iAddress ) &&
                        validAddr );

	if ( !invariant )
        {
		User::Invariant();
        }
	}


