/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#include <e32debug.h> // RDebug
#include <analyzetool/analyzetooltraceconstants.h>
#include "analyzetoolfastlog.h"
#include "atlog.h"

// Local time function.
TInt64 CurrentTime()
    {
    LOGSTR1( "ATFL CurrentTime()" );
    TTime time;
    time.UniversalTime();
    return time.Int64() - KMicroSecondsAt1970;
    }

TInt ATFastLogProcessStarted( const TDesC8& aProcessName,
                                 TUint aProcessId,
                                 TUint32 aIsDebug,
                                 const TDesC8& aAtoolVersion,
                                 const TDesC8& aApiVersion )
    {
    LOGSTR1( "ATFL ATFastLogProcessStarted()" );
    
    // PCS <Process name> <Process ID> <Time stamp> <Udeb> <Version>

    //Buffer to trace
    TBuf8<KProcessStartBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex );
    buffer.Append( KSpace );
    
    // PCS
    buffer.Append( KProcessStart );
    // process name
    buffer.Append( aProcessName );
    buffer.Append( KSpace );
    // process id
    buffer.AppendNum( aProcessId, EHex );
    buffer.Append( KSpace );
    // time stamp
    buffer.AppendNum( CurrentTime(), EHex ) ;
    buffer.Append( KSpace );
    // urel/udeb
    buffer.AppendNum( aIsDebug, EHex );
    buffer.Append( KSpace );
    // version
    buffer.AppendNum( KATTraceVersion, EHex );
    buffer.Append( KSpace );    
    // atool version
    buffer.Append( aAtoolVersion );
    buffer.Append( KSpace );
    // ATAPI version
    buffer.Append( aApiVersion );




    RDebug::RawPrint( buffer );
       
    return KErrNone;
    }

TInt ATFastLogProcessEnded( TUint aProcessId, 
                            TUint aHandleLeakCount )
    {
    LOGSTR1( "ATFL ATFastLogProcessEnded()" );
    
    // PCE
    
    if ( aHandleLeakCount > 0 )
    	{
        // HDL <Handle count>
    
        // Buffer to trace.
        TBuf8<KHandleLeakBufLength> buffer2;
        
        // AT indentifier
        buffer2.Append( KATIdentifier );
        // process id
        buffer2.AppendNum( aProcessId, EHex );
        buffer2.Append( KSpace );
        
        // HDL
        buffer2.Append( KHandleLeak );
        // leak count
        buffer2.AppendNum( aHandleLeakCount );

        RDebug::RawPrint( buffer2 );
    	}
    
    // Buffer to trace.
    TBuf8<KProcessEndBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex );
    buffer.Append( KSpace );
    
    // PCE
    buffer.Append( KProcessEnd );
    RDebug::RawPrint( buffer );
    
        
    return KErrNone;
    }

TInt ATFastLogDllLoaded( TUint aProcessId, 
                                        const TDesC8& aDllName,
                                        TUint32 aStartAddress,
                                        TUint32 aEndAddress )
    {
    LOGSTR1( "ATFL ATFastLogDllLoaded()" );
    
    // DLL <DLL name> <Memory start address> <Memory end address>
    
    // Buffer to trace.
    TBuf8<KDllLoadBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // DLL
    buffer.Append( KDllLoad );
    // dll name
    buffer.Append( aDllName );
    buffer.Append( KSpace );
    // start adress
    buffer.AppendNum( aStartAddress, EHex );
    buffer.Append( KSpace );   
    //end adress
    buffer.AppendNum( aEndAddress, EHex );
    
    RDebug::RawPrint( buffer );
    
    return KErrNone;
    }

TInt ATFastLogDllUnloaded( TUint aProcessId, const TDesC8& aDllName, TUint32 aStartAddress,
                                       TUint32 aEndAddress )
    {
    LOGSTR1( "ATFL ATFastLogDllUnloaded()" );
    
    // DLU <DLL name> <Memory start address> <Memory end address>
    
    // Buffer to trace.
    TBuf8<KDllUnloadBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // DLU
    buffer.Append( KDllUnload );
    // dll name
    buffer.Append( aDllName );
    buffer.Append( KSpace );
    // start adress
    buffer.AppendNum( aStartAddress, EHex );
    buffer.Append( KSpace );   
    //end adress
    buffer.AppendNum( aEndAddress, EHex );
    
    RDebug::RawPrint( buffer );

    return KErrNone;
    }

TInt ATFastLogMemoryAllocated( TUint aProcessId, TUint32 aMemAddress,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize, TUint aThreadId )
    {
    LOGSTR1( "ATFL ATFastLogMemoryAllocated()" );
    
    // ALH <Memory address> <Allocation size> <Thread ID> 
    // <Call stack address count> <Call stack address> <Call stack address> ...
        
    // Buffer to trace.
    TBuf8<KMemAllocBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // ALH
    buffer.Append( KMemoryAllocHeader );
    // memory adress
    buffer.AppendNum( aMemAddress, EHex );
    buffer.Append( KSpace );
    // allocation size
    buffer.AppendNum( aSize, EHex );
    buffer.Append( KSpace ); 
    // thread ID
    buffer.AppendNum( aThreadId, EHex );
    buffer.Append( KSpace );

    // Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aCallstack.Count() ; j++ )
		{
		if ( aCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	// Current position in call stack.
	TInt addrPos( 0 );
    
    // address count
    buffer.AppendNum( addrCount, EHex );
    
    TUint packetNumber( 1 );
    
    // Go through all call stack's memory addresses associated with
    // this memory allocation 
    for ( TInt j = 0; j < addrCount; j++ )
        {
        // ALF <Memory address> <Packet number> <Call stack address> <Call stack address> ...
        if ( buffer.Length() <= 0 )
            {
            // AT indentifier
            buffer.Append( KATIdentifier );
            // process id
            buffer.AppendNum( aProcessId, EHex  );
            buffer.Append( KSpace ); 
            
            // Create alloc fragment message header
            buffer.Append( KMemoryAllocFragment );
            buffer.AppendNum( aMemAddress, EHex );
            buffer.Append( KSpace );     
            buffer.AppendNum( packetNumber, EHex );
            // Increase packet number
            packetNumber++;
            }
      
        // Append call stack address.
        buffer.Append( KSpace );
        buffer.AppendNum( aCallstack.At( addrPos ), EHex );
        
        // Move the call stack position.
        addrPos++;
        
        // Check if buffer max length exceed
        if ( KLastItemLength + buffer.Length() >= KMemAllocBufLength )
            {
            // Log through debug channel 
            RDebug::RawPrint( buffer );
            // Empty trace buffer
            buffer.Delete( 0, buffer.MaxLength() );
            }
        }
    // Send last message if exists.
    if ( buffer.Length() > 0 )
        {
        RDebug::RawPrint( buffer );
        }
 
    return KErrNone;
    }


TInt ATFastLogMemoryFreed( TUint aProcessId, TUint32 aMemAddress, 
                              TFixedArray<TUint32, KATMaxFreeCallstackLength>& aFreeCallstack,
                              TUint aThreadId )
    {
    LOGSTR1( "ATFL ATFastLogMemoryFreed()" );

	// FRH <Memory address> <Thread ID> <Call stack address count> 
	// <Call stack address> <Call stack address> ...
	
	// Buffer to trace.
	TBuf8<KMemFreedBufLength> buffer;

	// AT indentifier
	buffer.Append( KATIdentifier );
	// process id
	buffer.AppendNum( aProcessId, EHex  );
	buffer.Append( KSpace );
	
	// FRH
	buffer.Append( KMemoryFreedHeader );
	
	// Append the start address of this allocation in the 32-bit (max 8 characters)
	// hexadecimal text format.
	buffer.AppendNum( aMemAddress, EHex );
	buffer.Append( KSpace );
	
	// thread ID
	buffer.AppendNum( aThreadId, EHex );
	buffer.Append( KSpace );
	
	// Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aFreeCallstack.Count() ; j++ )
		{
		if ( aFreeCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	// Current position in call stack.
	TInt addrPos( 0 );
	
	// address count.
	buffer.AppendNum( addrCount, EHex );
	
	TUint packetNumber( 1 );
	
	// Go through all call stack's memory addresses associated with
	// this memory allocation 
	for ( TInt j = 0; j < addrCount; j++ )
		{
		// FRF <Memory address> <Packet number> 
		// <Call stack address> <Call stack address> ...
		if ( buffer.Length() <= 0 )
			{               
			// Create alloc fragment message header
			buffer.Append( KMemoryFreedFragment );
			buffer.AppendNum( aMemAddress, EHex );
			buffer.Append( KSpace );
			buffer.AppendNum( packetNumber, EHex );
			// Increase packet number
			packetNumber++;
			}
	  
		// Append call stack address.
		buffer.Append( KSpace );
		buffer.AppendNum( aFreeCallstack.At( addrPos ), EHex );
		
		// Move the call stack position.
		addrPos++;
		
		// Check if buffer max length exceed
		if ( KLastItemLength + buffer.Length() >= KMemFreedBufLength )
			{
			// Log through debug channel 
			RDebug::RawPrint( buffer );
			// Empty trace buffer
			buffer.Delete( 0, buffer.MaxLength() );
			}
		}
	// Send last message if exists.
	if ( buffer.Length() > 0 )
		{
		RDebug::RawPrint( buffer );
		}
		
    return KErrNone;   
    }

TInt ATFastLogMemoryReallocated( TUint aProcessId, TUint32 aMemAddressFree,  TUint32 aMemAddressAlloc,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize, TUint aThreadId )
    {
    LOGSTR1( "ATFL ATFastLogMemoryReallocated()" );    
    
    // RAH <Freed memory address> <Allocated memory address> <Allocation size> <Thread ID>
    // <Call stack address count> <Call stack address> <Call stack address> ...
        
    // Buffer to trace.
    TBuf8<KMemReallocBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // RAH
    buffer.Append( KMemoryReallocHeader );
    // memory adress freed
    buffer.AppendNum( aMemAddressFree, EHex );
    buffer.Append( KSpace );
    // memory adress allocated
    buffer.AppendNum( aMemAddressAlloc, EHex );
    buffer.Append( KSpace );
    // allocation size
    buffer.AppendNum( aSize, EHex );
    buffer.Append( KSpace ); 
    // thread ID
    buffer.AppendNum( aThreadId, EHex );
    buffer.Append( KSpace );

    // Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aCallstack.Count() ; j++ )
		{
		if ( aCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	// Current position in call stack.
	TInt addrPos( 0 );
    
    // address count
    buffer.AppendNum( addrCount, EHex );
        
    TUint packetNumber( 1 );
    
    // Go through all call stack's memory addresses associated with
    // this memory allocation 
    for ( TInt j = 0; j < addrCount; j++ )
        {
        // RAF <Freed memory address> <Allocated memory address> <Packet number>
        // <Call stack address> <Call stack address> ...
        if ( buffer.Length() <= 0 )
            {
            // AT indentifier
            buffer.Append( KATIdentifier );
            // process id
            buffer.AppendNum( aProcessId, EHex  );
            buffer.Append( KSpace ); 
            
            // Create alloc fragment message header
            buffer.Append( KMemoryReallocFragment );
            // memory adress freed
            buffer.AppendNum( aMemAddressFree, EHex );
            buffer.Append( KSpace );
            // memory adress allocated
            buffer.AppendNum( aMemAddressAlloc, EHex );
            buffer.Append( KSpace );
            // packet number
            buffer.AppendNum( packetNumber, EHex );
            // Increase packet number
            packetNumber++;
            }
      
        // Append call stack address.
        buffer.Append( KSpace );
        buffer.AppendNum( aCallstack.At( addrPos ), EHex );
        
        // Move the call stack position.
        addrPos++;
        
        // Check if buffer max length exceed
        if ( KLastItemLength + buffer.Length() >= KMemAllocBufLength )
            {
            // Log through debug channel 
            RDebug::RawPrint( buffer );
            // Empty trace buffer
            buffer.Delete( 0, buffer.MaxLength() );
            }
        }
    // Send last message if exists.
    if ( buffer.Length() > 0 )
        {
        RDebug::RawPrint( buffer);
        }
      
    return KErrNone;
    }

TInt ATFastLogThreadStarted( TUint aProcessId, TUint aThreadId )
    {
    LOGSTR1( "ATFL ATFastLogThreadStarted()" );
    
    // TDS <Thread ID>

    //Buffer to trace
    TBuf8<KThreadStartBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // TDS
    buffer.Append( KThreadStart );
    // thread ID
    buffer.AppendNum( aThreadId, EHex );

    RDebug::RawPrint( buffer );
       
    return KErrNone;
    }

TInt ATFastLogThreadEnded( TUint aProcessId, TUint64 aThreadId )
    {
    LOGSTR1( "ATFL ATFastLogThreadEnded()" );
    
    // TDE <Thread ID>

    //Buffer to trace
    TBuf8<KThreadEndBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    // process id
    buffer.AppendNum( aProcessId, EHex  );
    buffer.Append( KSpace );
    
    // TDE
    buffer.Append( KThreadEnd );
    // thread ID
    buffer.AppendNum( aThreadId, EHex );

    RDebug::RawPrint( buffer );
       
    return KErrNone;
    }

TInt ATFastLogDeviceInfo( const TDesC8& aS60Version, const TDesC8& aChecksum)
    {
    LOGSTR1( "ATFL ATFastLogVersionsInfo()" );
    
    //Buffer to trace
    TBuf8<KVersionsInfoBufLength> buffer;
    
    // AT indentifier
    buffer.Append( KATIdentifier );
    
    // VER
    buffer.Append( KVersionsInfo );
    
    // sw version
    buffer.Append(aS60Version);
    buffer.Append(KSpace);
    // rom checksum
    buffer.Append(aChecksum);

    RDebug::RawPrint( buffer );
    
    return KErrNone;
    }
