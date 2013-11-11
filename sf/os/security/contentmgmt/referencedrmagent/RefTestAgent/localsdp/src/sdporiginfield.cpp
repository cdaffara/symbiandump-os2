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
// Name          :  SdpOriginField.cpp
// Part of       :  Local SDP Codec
// Version       :  1.0
//



#include <in_sock.h>
#include <s32strm.h>
#include "sdporiginfield.h"
#include "sdporiginfieldptrs.h"
#include "sdputil.h"
#include "sdpcodecstringconstants.h"
#include "sdpcodecconstants.h"
#include "sdpcodecstringpool.h"
#include "sdpcodecerr.h"
#include "sdpcodec.pan"
#include "_sdpdefs.h"

// LOCAL CONSTANTS AND MACROS
const TUint KMaxIPDesLength = 39;
const TUint KHeaderIndex = 0;
const TUint KUserNameIndex = 1;
const TUint KSessionIdIndex = 2;
const TUint KSessionVersionIndex = 3;
const TUint KNetworkTypeIndex = 4;
const TUint KAddressTypeIndex = 5;
const TUint KAddressIndex = 6;
const TInt KTokenCount = 7;
const TInt64 KDummyValue = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSdpOriginField::CSdpOriginField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSdpOriginField::CSdpOriginField()
    {
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ConstructL(    
    const TDesC8& aText )
	{
    // aText should be of format:
    // o=<username> <session id> <version> <network type> 
    // <address type> <address>     
    iPool = SdpCodecStringPool::StringPoolL();

    RArray<TPtrC8> array;
    array = SdpUtil::GetElementsFromLineL( aText, KErrSdpCodecOriginField );
    
    CleanupClosePushL( array );

    if ( array.Count() != KTokenCount )
        {
        User::Leave( KErrSdpCodecOriginField );
        }

    // Check that the line contains valid header
    RStringF origHeader =  iPool.StringF( SdpCodecStringConstants::EOrigin, 
                                          SdpCodecStringConstants::Table );
    if ( origHeader.DesC().CompareF( array[KHeaderIndex] ) != 0 )
        {
        // Header didn't match
        User::Leave( KErrSdpCodecOriginField );
        }
    
    iUserName =
    	reinterpret_cast< HBufC8* >( CSdpOriginFieldPtrs::NewL( 0, 0 ) );
		
    
    // <username>   
    ParseUserNameL( array );
    
    // <session id> & <version>
    ParseSessionIDAndVersionL( array );
    
    // <network type> & <address type>
    ParseNetTypeAndAddressTypeL( array );
    
    // <address>
    ParseAddressL( array );
    
	//If address is IPv4-Mapped IPv6 , it is changed to IPv4
	TInetAddr addr;
	TBuf<KMaxAddressLength> address;
	address.Copy(iAddress);

	TInt err = addr.Input( address );
	if ( err == KErrNone )
		{
        // Valid IP address
		TBuf<KMaxIPDesLength> buf;
		addr.Output( buf );
		iAddress.Copy(buf); 

        SetIPAddressType( addr );
		}



    CleanupStack::PopAndDestroy();  // array

	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ConstructL(
    const TDesC8& aUserName, 
    TInt64 aSessionId, 
    TInt64 aSessionVersion, 
    TInetAddr& aUnicastAddress )
	{
    iPool = SdpCodecStringPool::StringPoolL();

	__ASSERT_ALWAYS( 
                   IsValidUserName( aUserName )
				&& IsValidAddress( aUnicastAddress )
                && aSessionId >= 0 && aSessionVersion >= 0,
				User::Leave( KErrSdpCodecOriginField ) );

	iUserName = reinterpret_cast< HBufC8* >
		( CSdpOriginFieldPtrs::NewL( aSessionId, aSessionVersion ) );
	OriginFieldPtrs().SetUserNameL( aUserName );
	
    TBuf<KMaxIPDesLength> des;
    aUnicastAddress.Output( des );
	//if aUnicastAddress was IPv4-Mapped IPv6 address,
	// the result of Output is IPv4
	// the address is stored in IPv4 format, so the iAddressType 
	// must also be IPv4.    
    iAddress.Copy( des );
    SetIPAddressType( aUnicastAddress );

    iNetType = iPool.StringF( SdpCodecStringConstants::ENetType, 
                              SdpCodecStringConstants::Table ).Copy();
       
	__TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ConstructL( 
    const TDesC8& aUserName, 
    TInt64 aSessionId, 
    TInt64 aSessionVersion, 
    RStringF aNetType, 
    RStringF aAddressType, 
    const TDesC8& aAddress )
    {        
    iPool = SdpCodecStringPool::StringPoolL();
     
    __ASSERT_ALWAYS(
               IsValidUserName( aUserName )
            && IsValidAddress( aAddress ) &&
            TypeMatchesWithFormat( aAddress, aAddressType.DesC(), iPool )
            && ( SdpUtil::IsToken( aNetType.DesC() ) )
            && ( SdpUtil::IsToken( aAddressType.DesC() ) )
            && aSessionId >= 0 && aSessionVersion >= 0,
            User::Leave( KErrSdpCodecOriginField ) );
    
    iUserName = reinterpret_cast< HBufC8* >
		( CSdpOriginFieldPtrs::NewL( aSessionId, aSessionVersion ) );
	OriginFieldPtrs().SetUserNameL( aUserName );	

    iNetType = aNetType.Copy();
    	TInetAddr addr;
	TBuf<KMaxAddressLength> address;
	address.Copy(aAddress);
	TInt err = addr.Input(address);
	if ( err  == KErrNone )
		{
		// Valid IP address
		TBuf< KMaxIPDesLength > buf;
		addr.Output( buf );
		iAddress.Copy( buf );		
		SetIPAddressType( addr );
		}
	else
		{
		iAddress = aAddress;
    	iAddressType = aAddressType.Copy();	
		}
    __TEST_INVARIANT;    
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::DecodeL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::DecodeL(    
    const TDesC8& aText )
	{
	CSdpOriginField* obj = CSdpOriginField::DecodeLC( aText );
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::DecodeLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::DecodeLC(    
    const TDesC8& aText )
	{
	CSdpOriginField* obj = new ( ELeave ) CSdpOriginField;
	CleanupStack::PushL( obj );
	obj->ConstructL(aText );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::NewL(    
    const TDesC8& aUserName, 
	TInt64 aSessionId, 
	TInt64 aSessionVersion, 
	TInetAddr& aAddress )
	{
	CSdpOriginField* obj = CSdpOriginField::NewLC( 
        aUserName, aSessionId, aSessionVersion, aAddress );
	CleanupStack::Pop();
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::NewLC(    
    const TDesC8& aUserName, 
	TInt64 aSessionId, 
	TInt64 aSessionVersion, 
	TInetAddr& aAddress )
	{
	CSdpOriginField* obj = new ( ELeave ) CSdpOriginField;
	CleanupStack::PushL(obj);
	obj->ConstructL( aUserName, aSessionId, aSessionVersion, aAddress );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::NewL(
    const TDesC8& aUserName, 
	TInt64 aSessionId, 
	TInt64 aSessionVersion, 
    RStringF aNetType,
    RStringF aAddressType, 
	const TDesC8& aAddress )
    {
	CSdpOriginField* obj = CSdpOriginField::NewLC( 
        aUserName, aSessionId, aSessionVersion, aNetType, 
        aAddressType, aAddress );
	CleanupStack::Pop();
	return obj;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::NewLC
// Two-phased constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::NewLC(
    const TDesC8& aUserName, 
	TInt64 aSessionId, 
	TInt64 aSessionVersion, 
	RStringF aNetType,
    RStringF aAddressType, 
	const TDesC8& aAddress )
    {    
    CSdpOriginField* obj = new ( ELeave ) CSdpOriginField;
	CleanupStack::PushL( obj );
	obj->ConstructL( aUserName, aSessionId, aSessionVersion, aNetType, 
                     aAddressType, aAddress );
    return obj;    
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::~CSdpOriginField
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField::~CSdpOriginField()
	{
	CSdpOriginFieldPtrs* tmp =
		reinterpret_cast< CSdpOriginFieldPtrs* >( iUserName );
	delete tmp;

    iNetType.Close();
    iAddressType.Close();
	}
    
// -----------------------------------------------------------------------------
// CSdpOriginField::EncodeL
// Writes attributes in proper format to the stream
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::EncodeL(
    RWriteStream& aStream ) const
	{
    __TEST_INVARIANT;

    // o=<username> <session id> <version> <network type> <address type> 
    //   <address>    

    // "o="
    RStringF header = iPool.StringF( SdpCodecStringConstants::EOrigin, 
                                     SdpCodecStringConstants::Table );
    aStream.WriteL( header.DesC() );

    // <username>
    aStream.WriteL( OriginFieldPtrs().UserName() );
	aStream.WriteL( KSPStr );

    // <session id>    
	aStream.WriteL( OriginFieldPtrs().SessionId() );
	aStream.WriteL( KSPStr );

    // <version>
    aStream.WriteL( OriginFieldPtrs().SessionVersion() );
	aStream.WriteL( KSPStr );

    // <network type>
    aStream.WriteL( iNetType.DesC() );
    aStream.WriteL( KSPStr );    

    // <address type>
    aStream.WriteL( iAddressType.DesC() );
    aStream.WriteL( KSPStr );

    // <address>
    aStream.WriteL( iAddress );

    // End-of-Line mark
	aStream.WriteL( KCRLFStr );
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::CloneL
// Creates an exact copy of the origin field
// -----------------------------------------------------------------------------
//
EXPORT_C CSdpOriginField* CSdpOriginField::CloneL() const
	{
	__TEST_INVARIANT;

    CSdpOriginField* obj = 0;

    if ( InetAddress() )
        {
        // Clones instance with TInetAddr
        TInetAddr addr( *InetAddress() );        
        obj = CSdpOriginField::NewLC( UserName(), KDummyValue,
        							  KDummyValue, addr );
        }
    else
        {
        // Clones instance with Internet address as a standard string               
        obj = CSdpOriginField::NewLC( UserName(), KDummyValue, KDummyValue,
                                      iNetType, iAddressType,  Address() );        
        }

	// Set the real values
	obj->OriginFieldPtrs().SetSessionIdL( OriginFieldPtrs().SessionId() );
	obj->OriginFieldPtrs().SetSessionVersionL(
		OriginFieldPtrs().SessionVersion() );
	CleanupStack::Pop( obj );
		
	__ASSERT_DEBUG( *this == *obj, User::Panic( KSdpCodecPanicCat, 
                                                KSdpCodecPanicInternal ) );
	return obj;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::operator ==
// Checks if two origin fields are equal
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSdpOriginField::operator == (
    const CSdpOriginField& aObj) const
	{
	__TEST_INVARIANT;

    TBool equalFields = EFalse;

    // Check that username, session ID and address type match before
    // going to internet address       
    if ( ( UserName().CompareF( aObj.UserName() ) == 0 ) &&
         ( SessionId() == aObj.SessionId() ) &&
         ( AddressType() == aObj.AddressType() ) &&
         ( Version() == aObj.Version() ) &&
         ( NetType() == aObj.NetType() ) )
        {
        if ( InetAddress() && aObj.InetAddress() )
            {            
            if ( (*InetAddress()).Match( *aObj.InetAddress() ) )
                {
                equalFields = ETrue;
                }
            }
        else if ( !InetAddress() && !aObj.InetAddress() )            
            {
            if ( iAddress.CompareF( aObj.Address() ) == 0 )
                {
                equalFields = ETrue;
                }
            }                
        else
            {
            // These two are not the same
            }
        }   
        
    return equalFields;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::UserName
// Returns username
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpOriginField::UserName() const
    {
    __TEST_INVARIANT;
    return OriginFieldPtrs().UserName();
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::SetUserNameL
// Sets a new username
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::SetUserNameL(
    const TDesC8& aValue )
    {
    __TEST_INVARIANT;
    if ( IsValidUserName( aValue ) )
        {
        OriginFieldPtrs().SetUserNameL( aValue );
        }
    else
        {
        User::Leave( KErrSdpCodecOriginField );
        }
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::SessionId
// Returns current session id
// -----------------------------------------------------------------------------
//
EXPORT_C TInt64 CSdpOriginField::SessionId() const
    {
    __TEST_INVARIANT;

    return Get63Msbs( OriginFieldPtrs().SessionId() );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::SetSessionId
// Sets new session ID
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::SetSessionId(
	TInt64 aValue )
    {
    __TEST_INVARIANT;
    if ( aValue >= 0 )
        {
        TRAP_IGNORE( OriginFieldPtrs().SetSessionIdL( aValue ) )
        }
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::Version
// Returns version number of the announcement
// -----------------------------------------------------------------------------
//
EXPORT_C TInt64 CSdpOriginField::Version() const
    {
    __TEST_INVARIANT;

    return Get63Msbs( OriginFieldPtrs().SessionVersion() );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::SetVersion
// Sets new version for this session's announcement
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::SetVersion(
	TInt64 aValue )
    {
    __TEST_INVARIANT;
    if ( aValue >= 0 )
        {
        TRAP_IGNORE( OriginFieldPtrs().SetSessionVersionL( aValue ) )        
        }
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::NetType
// Returns net type (always Internet)
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpOriginField::NetType() const
	{
	__TEST_INVARIANT;
	return iNetType;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::AddressType
// Returns address type (IP4/IP6)
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSdpOriginField::AddressType() const
	{
	__TEST_INVARIANT;    
    return iAddressType;        
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::InetAddress
// Returns the unicast address
// -----------------------------------------------------------------------------
//
EXPORT_C const TInetAddr* CSdpOriginField::InetAddress() const
	{
	__TEST_INVARIANT;
    
    TBuf<KMaxAddressLength> address16;
    address16.Copy( iAddress );
    TInt err = iUnicastAddress.Input( address16 );
    
    if ( !err )
        {
        return &iUnicastAddress;
        }
    else
        {
        return NULL;
        }
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::Address
// Returns the address
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSdpOriginField::Address() const
    {
    __TEST_INVARIANT;
    return iAddress;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::SetInetAddress
// Sets Internet Address
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::SetInetAddress(
    const TInetAddr& aValue )
	{    
	__TEST_INVARIANT;
    
	if ( IsValidAddress( aValue ) )
        {                       
        TBuf<KMaxIPDesLength> buf;
        aValue.Output( buf );
        // Copy new address to safe
        iAddress.Copy( buf );
    
        // Copy network type and address type        
        SdpUtil::SetDefaultNetTypeAndAddrType( 
            iPool, aValue, iNetType, iAddressType );
        SetIPAddressType( aValue );
        }

    __TEST_INVARIANT;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::SetAddressL
// Sets address from the buffer
// -----------------------------------------------------------------------------
//
EXPORT_C void CSdpOriginField::SetAddressL(
    const TDesC8& aAddress,
    RStringF aNetType, 
    RStringF aAddressType )
    {
    __TEST_INVARIANT;
     
    __ASSERT_ALWAYS( SdpUtil::IsToken( aNetType.DesC() )  &&
                     SdpUtil::IsToken( aAddressType.DesC() ) &&
                     IsValidAddress( aAddress ) &&
                     TypeMatchesWithFormat( aAddress, 
                                            aAddressType.DesC(), iPool ),
                     User::Leave( KErrSdpCodecOriginField ) );

    iAddress = aAddress;
    iAddressType.Close();
    iAddressType = aAddressType.Copy();

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::ExternalizeL
// Externalizes the field back to string
// -----------------------------------------------------------------------------
//	
void CSdpOriginField::ExternalizeL(
    RWriteStream& aStream ) const
    {
    __TEST_INVARIANT;

    // <username>    
    aStream.WriteUint32L( UserName().Length() );
	aStream.WriteL( UserName(), UserName().Length() );
    // <session id>
    aStream.WriteUint32L( OriginFieldPtrs().SessionId().Length() );
    aStream.WriteL( OriginFieldPtrs().SessionId(),
    				OriginFieldPtrs().SessionId().Length() );
    // <version>
    aStream.WriteUint32L( OriginFieldPtrs().SessionVersion().Length() );
    aStream.WriteL( OriginFieldPtrs().SessionVersion(),
    				OriginFieldPtrs().SessionVersion().Length() );
    // <network type>
    aStream.WriteUint32L( iNetType.DesC().Length() );
    aStream.WriteL( iNetType.DesC() );
    // <address type>
    aStream.WriteUint32L( iAddressType.DesC().Length() );
    aStream.WriteL( iAddressType.DesC() );    
    // <address>
    aStream.WriteUint32L( iAddress.Length() );
    aStream.WriteL( iAddress );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::InternalizeL
// Internalizes from stream
// -----------------------------------------------------------------------------
//	
CSdpOriginField* CSdpOriginField::InternalizeL(
    RReadStream& aStream )
    {    
    RStringPool pool = SdpCodecStringPool::StringPoolL();

    // <username>        
    TUint32 length = aStream.ReadUint32L();
	HBufC8* userName = HBufC8::NewLC( length );
    TPtr8 ptr( userName->Des() );  
    aStream.ReadL( ptr, length );    

    // <session id>
    length = aStream.ReadUint32L();
	HBufC8* sessionId = HBufC8::NewLC( length );
    ptr.Set( sessionId->Des() );
    aStream.ReadL( ptr, length );

    // <version>
    length = aStream.ReadUint32L();
	HBufC8* version = HBufC8::NewLC( length );
    ptr.Set( version->Des() );
    aStream.ReadL( ptr, length );

    // <network type>
    length = aStream.ReadUint32L();
    HBufC8* netType = HBufC8::NewLC( length );
    ptr.Set( netType->Des() );
    aStream.ReadL( ptr, length );

    // <address type>
    length = aStream.ReadUint32L();
    HBufC8* addrType = HBufC8::NewLC( length );
    ptr.Set( addrType->Des() );
    aStream.ReadL( ptr, length );

    // <address>        
    length = aStream.ReadUint32L();
    HBufC8* address = HBufC8::NewLC( length );
    TPtr8 ptr2( address->Des() );
    aStream.ReadL( ptr2, length );
            
    RStringF netTypeStr = pool.OpenFStringL( *netType );
    RStringF addrTypeStr = pool.OpenFStringL( *addrType );
    CleanupClosePushL( netTypeStr );
    CleanupClosePushL( addrTypeStr );

	CSdpOriginField* obj = CSdpOriginField::NewLC( *userName, KDummyValue,
												   KDummyValue, netTypeStr,
												   addrTypeStr, *address );
	// Set the real values
	obj->OriginFieldPtrs().SetSessionIdL( *sessionId );
	obj->OriginFieldPtrs().SetSessionVersionL( *version );
	CleanupStack::Pop( obj );
    CleanupStack::Pop( 2 ); // addrTypeStr, netTypeStr
    CleanupStack::PopAndDestroy( address );
    CleanupStack::PopAndDestroy( addrType );
    CleanupStack::PopAndDestroy( netType );
    CleanupStack::PopAndDestroy( version );
    CleanupStack::PopAndDestroy( sessionId );
    CleanupStack::PopAndDestroy( userName );    

    return obj;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::IsValidAddress
// Checks if the address is valid
// -----------------------------------------------------------------------------
//
TBool CSdpOriginField::IsValidAddress( 
    const TDesC8& aAddress ) const
    {
    TInetAddr addr;
    TBool valid = ETrue;

    if ( aAddress.Length() > 0 && aAddress.Length() <= KMaxAddressLength )
        {
        TBuf<KMaxAddressLength> address16;
        address16.Copy( aAddress );
        TInt err = addr.Input( address16 );

        if ( !err )
            {
            valid = ( addr.IsUnicast() || addr.IsUnspecified() );
            }
        else
            {
            RStringF addrTypeIP4 = 
                iPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                               SdpCodecStringConstants::Table );
            RStringF addrTypeIP6 = 
                iPool.StringF( SdpCodecStringConstants::EAddressType,
                               SdpCodecStringConstants::Table );
            
            if ( iAddressType == addrTypeIP4 || iAddressType == addrTypeIP6 )
                {
                // FQDN address, check that it has only valid characters
                // 0..9, a..z, A..Z, '.', '-'
                        
                for ( TInt i( 0 ); i < aAddress.Length() && valid; i++ )
                    {
                    if (KValidFQDNChars().Locate(aAddress[i]) == KErrNotFound)
                        {
                        valid = EFalse;     
                        }
                    }
                }
            else
                {
                valid = SdpUtil::IsNonWhitespace( aAddress );
                }
            }       
        }
    else
    	{
    	valid = EFalse;
    	}

    return valid;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::IsValidAddress
// Checks if the address is valid
// -----------------------------------------------------------------------------
//
TBool CSdpOriginField::IsValidAddress( 
    const TInetAddr& addr ) const
    {
    TBuf<KMaxIPDesLength> buf16;
    TBuf8<KMaxIPDesLength> buf;
	//if addr is IPv4-Mapped IPv6, buf value will be IPv4 after Output
    addr.Output( buf16 );
    buf.Copy( buf16 );

    return IsValidAddress( buf );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::TypeMatchesWithFormat
// Checks if address type is aligned with the address format
// -----------------------------------------------------------------------------
//
TBool CSdpOriginField::TypeMatchesWithFormat( 
    const TDesC8& aAddress, 
    const TDesC8& aType,
    RStringPool aPool ) const
    {
    RStringF addrTypeIP4 = 
        aPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                              SdpCodecStringConstants::Table );
    RStringF addrTypeIP6 = 
        aPool.StringF( SdpCodecStringConstants::EAddressType,
                              SdpCodecStringConstants::Table );
    
    TBool valid( ETrue );
    
    // Check that address type and address matches together
    TInetAddr addr;
    TBuf<KMaxAddressLength> address16;
    address16.Copy( aAddress );
    TInt err = addr.Input( address16 );    
    if ( err == KErrNone && !addr.IsUnspecified())
        {
        TBool ip4Type = ( addrTypeIP4.DesC().CompareF( aType ) == 0 );
        TBool ip6Type = ( addrTypeIP6.DesC().CompareF( aType ) == 0 );
             
        if ( ip4Type || ip6Type )
            {
            if ( ( addr.Address() && !addr.IsV4Mapped() && !ip4Type ) ||
				 (addr.Address() && addr.IsV4Mapped() && !ip6Type) ||
                 ( !addr.Address() && !ip6Type ) )
                {
                valid = EFalse;
                }
            }
        } 
    
    return valid;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::IsValidUserName
// Checks if the given username is valid
// -----------------------------------------------------------------------------
//
TBool CSdpOriginField::IsValidUserName( 
    const TDesC8& aUserName ) const
    {
    TBool valid = EFalse;

    if ( aUserName.Length() > 0 && SdpUtil::IsNonWhitespace( aUserName ) )
        {
        valid = ETrue;
        }

    return valid;
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::ParseUserNameL
// Parses username. Can't use CSdpOriginField::SetUserNameL, as invariant would
// fail.
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ParseUserNameL( 
    RArray<TPtrC8>& aArray )
    {
    __ASSERT_ALWAYS( IsValidUserName( aArray[KUserNameIndex] ), 
                     User::Leave( KErrSdpCodecOriginField ) );
	OriginFieldPtrs().SetUserNameL( aArray[KUserNameIndex] );    
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::ParseSessionIDAndVersionL
// Parses session ID and version
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ParseSessionIDAndVersionL( 
    RArray<TPtrC8>& aArray )
    {
    __ASSERT_ALWAYS( SdpUtil::IsDigit( aArray[KSessionIdIndex] ) &&
    				 SdpUtil::IsDigit( aArray[KSessionVersionIndex] ),
    				 User::Leave( KErrSdpCodecOriginField ) );

    OriginFieldPtrs().SetSessionIdL( aArray[KSessionIdIndex] );
    OriginFieldPtrs().SetSessionVersionL( aArray[KSessionVersionIndex] );    
    }
 
 // -----------------------------------------------------------------------------
// CSdpOriginField::ParseNetTypeAndAddressTypeL
// Parses network type and address type
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ParseNetTypeAndAddressTypeL( 
    RArray<TPtrC8>& aArray )
    {
    if ( !SdpUtil::IsToken( aArray[KNetworkTypeIndex] ) ||
         !SdpUtil::IsToken( aArray[KAddressTypeIndex] ) )
        {        
        User::Leave( KErrSdpCodecOriginField );
        }

    iNetType = iPool.OpenFStringL( aArray[KNetworkTypeIndex] );  
    iAddressType = iPool.OpenFStringL( aArray[KAddressTypeIndex] );    
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::ParseAddressL
// Parses address
// -----------------------------------------------------------------------------
//
void CSdpOriginField::ParseAddressL( 
    RArray<TPtrC8>& aArray )
    {    
    if ( !IsValidAddress( aArray[KAddressIndex] ) )
        {
        User::Leave( KErrSdpCodecOriginField );
        }
    else
        {
        iAddress = aArray[KAddressIndex];
        }

    // Check that address type and address matches together
    if ( !TypeMatchesWithFormat( 
                    aArray[KAddressIndex], aArray[KAddressTypeIndex], iPool ) )
        {
        User::Leave( KErrSdpCodecOriginField );            
        } 
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::OriginFieldPtrs
// -----------------------------------------------------------------------------
//  
inline CSdpOriginFieldPtrs& CSdpOriginField::OriginFieldPtrs()
    {
    return *( reinterpret_cast< CSdpOriginFieldPtrs* >( iUserName ) );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::OriginFieldPtrs
// -----------------------------------------------------------------------------
//  
inline const CSdpOriginFieldPtrs& CSdpOriginField::OriginFieldPtrs() const
    {
    return *( reinterpret_cast< CSdpOriginFieldPtrs* >( iUserName ) );
    }

// -----------------------------------------------------------------------------
// CSdpOriginField::Get63Msbs
// Returns a maximum of 63 bits of information from the descriptor containing a
// decimal number.
// -----------------------------------------------------------------------------
//
TInt64 CSdpOriginField::Get63Msbs( const TDesC8& aDecimalValue ) const
	{	
	// The maximum amount of digits in a decimal number, that is guaranteed to
  	// fit into 63 bits, is 18. Even if all the 18 digits are 9, the decimal
  	// number is 999999999999999999.
  	const TInt64 KMaxAmountOfDecimalDigits = 18;

    TInt64 value( 0 );
    TPtrC8 msbPart = aDecimalValue.Left( KMaxAmountOfDecimalDigits );
	TLex8( msbPart ).Val( value );
    return value;
	}

// -----------------------------------------------------------------------------
// CSdpOriginField::SetIPAddressType
// -----------------------------------------------------------------------------
//
void CSdpOriginField::SetIPAddressType( const TInetAddr& aAddr )
    {
    iAddressType.Close();
    if ( aAddr.Address() || aAddr.IsUnspecified() )
        {
        //IPv4, IPv4-Mapped IPv6 and 0.0.0.0
	    iAddressType =
            iPool.StringF( SdpCodecStringConstants::EAddressTypeIP4,
                           SdpCodecStringConstants::Table ).Copy();
        }
    else
        {
        //IPv6
        iAddressType =
            iPool.StringF( SdpCodecStringConstants::EAddressType,
                           SdpCodecStringConstants::Table ).Copy();
 		}
    }

// For DEBUG builds

// -----------------------------------------------------------------------------
// CSdpOriginField::__DbgTestInvariant
// Test invariant
// -----------------------------------------------------------------------------
//	
void CSdpOriginField::__DbgTestInvariant() const
	{	    
	TBool invariant = 
				iUserName != NULL	
                && SdpUtil::IsToken( iAddressType.DesC() )
                && SdpUtil::IsToken( iNetType.DesC() )
				&& IsValidAddress( iAddress )
				&& IsValidUserName( OriginFieldPtrs().UserName() )
                && TypeMatchesWithFormat(iAddress, iAddressType.DesC(), iPool)
                && SdpUtil::IsNonWhitespace( OriginFieldPtrs().UserName() );
	
    if ( !invariant )
        {
		User::Invariant();
        }
	}

