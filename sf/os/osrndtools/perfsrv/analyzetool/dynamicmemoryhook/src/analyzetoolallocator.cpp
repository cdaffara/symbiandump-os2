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
* Description:  Definitions for the class RAnalyzeToolAllocator.
*
*/


#include "analyzetoolallocator.h"
#include "analyzetoolmemoryallocator.h"
#include "atlog.h"
#include "analyzetoolpanics.pan"
#include "analyzetoolfastlog.h"
#include <e32svr.h>

// CONSTANTS

// Length of the callstack address
const TUint32 KAddressLength = 4;

// Thread count
const TInt KThreadCount = 1;

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::RAnalyzeToolAllocator()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RAnalyzeToolAllocator::RAnalyzeToolAllocator( TBool aNotFirst,
                                              RArray<TCodeblock>& aCodeblocks, 
                                              RMutex& aMutex,
                                              TUint aProcessId,
                                              RAnalyzeTool& aAnalyzeTool,
                                              TUint32 aLogOption,
                                              TUint32 aAllocCallStackSize,
                                              TUint32 aFreeCallStackSize,
                                              RATFileLog& aLogFile ) :
    RAnalyzeToolMemoryAllocator( aNotFirst ),
    iCodeblocks( aCodeblocks ), 
    iMutex( aMutex ),
    iProcessId( aProcessId ),
    iThreadArray( KATMaxCallstackLength ),
    iAnalyzeTool( aAnalyzeTool ),
    iLogOption( aLogOption ),
    iAllocMaxCallStack( aAllocCallStackSize ),
    iFreeMaxCallStack( aFreeCallStackSize ),
    iLogFile ( aLogFile )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::RAnalyzeToolAllocator()" );
    
    // Append thread to array of the users of this allocator
    TThreadParamsBuf params;
    params().iThreadId = RThread().Id().operator TUint();
    TInt error = iAnalyzeTool.ThreadStack( params );
    if ( KErrNone == error )
        {
        LOGSTR2( "ATMH Thread stack address: %x", params().iStackAddress );
        LOGSTR2( "ATMH Thread stack size:    %x", params().iStackSize );
        error = iThreadArray.Append( TThreadStack( RThread().Id(), 
                             params().iStackAddress + params().iStackSize ) );

        if ( iLogOption == EATLogToTraceFast )
        	{
            // log thread added
            ATFastLogThreadStarted( aProcessId, RThread().Id().operator TUint() ); 
        	}
        else if ( iLogOption == EATLogToFile )
        	{
            iLogFile.ATFileLogThreadStarted( RThread().Id().operator TUint() );
        	}
        }
    
    __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantAppendToTheArray ) ); 
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::~RAnalyzeToolAllocator()
// Destructor.
// -----------------------------------------------------------------------------
//
RAnalyzeToolAllocator::~RAnalyzeToolAllocator()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::~RAnalyzeToolAllocator()" );
    
    for( TInt i=0; i<iThreadArray.Count(); i++)
    	{
        // log thread removed
        if ( iLogOption == EATLogToTraceFast )
        	{
            ATFastLogThreadEnded( RProcess().Id().operator TUint(), RThread().Id().operator TUint() ); 
            }
        else if ( iLogOption == EATLogToFile )
        	{
            iLogFile.ATFileLogThreadEnded( RThread().Id().operator TUint() );
        	}
         }
    
    
    // Close the thread array 
    iThreadArray.Close();
    }
    
// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Uninstall()
// Uninstalls the current allocator
// -----------------------------------------------------------------------------
//
void RAnalyzeToolAllocator::Uninstall()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Uninstall()" );

    // Switch back to the original allocator
    SwitchOriginalAllocator();
    
    // Check if this is shared allocator between threads
    if ( iThreadArray.Count() > KThreadCount )
        {
        // Close the shared allocator
        Close();
        return;
        }

#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_3 )
    #ifndef __WINS__ 
    // Remove dummy Tls handle
    UserSvr::DllFreeTls( KDummyHandle );
    #endif
#endif
    
    // Since this is the last thread using this allocator it can be deleted
    delete this;
    }

#ifdef __WINS__

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Alloc() WINS version
// Allocates a cell of specified size from the heap.
// -----------------------------------------------------------------------------
//
UEXPORT_C TAny* RAnalyzeToolAllocator::Alloc( TInt aSize )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Alloc()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // get thread ID
    TUint threadId = RThread().Id();
    // Alloc memory from the original allocator
    TAny* p = iAllocator->Alloc( aSize );
    
    LOGSTR3( "ATMH RAnalyzeToolAllocator::Alloc() - aSize: %i, address: %x", 
             aSize,  (TUint32) p );
    
	// Reset the callstack
	iCallStack.Reset();

	// Find the current thread callstack start address
	TUint32 stackstart( 0 );
	TBool found( FindCurrentThreadStack( stackstart ) );
	LOGSTR3( "ATMH > stackstart: %x , found = %i", stackstart, found );
	
	TUint32 _sp;
	__asm
		{
		mov [_sp], esp
		}
	
	// Get codeblocks count
	TInt blocksCount( iCodeblocks.Count() );
	TInt error( KErrNone );
	TUint arrayCounter = 0;
	
	for ( TUint32 i = _sp; i < stackstart; i = i + KAddressLength )//lint !e1055 !e526 !e628 !e348
		{
		TUint32 addr = (TUint32) *( (TUint32*) i );
		if ( ! IsAddressLoadedCode( addr ) )
			continue;
		for ( TInt j = 0; j < blocksCount; j++ )
			{
			if ( iCodeblocks[j].CheckAddress( addr ) )
				{
				// To avoid recursive call to ReAlloc specifying granularity
				// Add address to the callstack
				iCallStack[arrayCounter] = ( addr );
				arrayCounter++;
				break;
				}
			}
		if ( arrayCounter == KATMaxCallstackLength ||
			 arrayCounter == iAllocMaxCallStack )
			{
			LOGSTR2( "ATMH > Wanted CallStack items ready( %i )", arrayCounter );
			break;
			}
		}
	// Log the memory allocation information
	if ( iLogOption == EATLogToTraceFast )
		{
		// Using fast mode.
		ATFastLogMemoryAllocated( iProcessId, (TUint32) p, iCallStack, aSize, threadId );
		}
	else if ( iLogOption == EATLogToFile )
		{
		iLogFile.ATFileLogMemoryAllocated( (TUint32) p, iCallStack, aSize, threadId );
		}
   
    // Release the mutex
    iMutex.Signal();
    
    return p;
    }
#else

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Alloc() ARMV5 version
// Allocates a cell of specified size from the heap.
// -----------------------------------------------------------------------------
//
TAny* RAnalyzeToolAllocator::Alloc( TInt aSize )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Alloc()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // get thread ID
    TUint threadId = RThread().Id();
        
    // Alloc memory from the original allocator
    TAny* p = iAllocator->Alloc( aSize );
        

	// Reset the callstack
	iCallStack.Reset(); 
	
	// Find the current thread callstack start address
	TUint32 stackstart( 0 );
	TBool found( FindCurrentThreadStack( stackstart ) );
	LOGSTR3( "ATMH > stackstart: %x , found = %i", stackstart, found );
	
	// Get codeblocks count
	TInt blocksCount( iCodeblocks.Count() );
	TUint arrayCounter = 0;
	
	for ( TUint32 i = __current_sp(); i < stackstart; i = i + KAddressLength )//lint !e1055 !e526 !e628 !e348
		{
		TUint32 addr = (TUint32) *( (TUint32*) i );
		if ( ! IsAddressLoadedCode( addr ) )
			continue;
		for ( TInt j = 0; j < blocksCount; j++ )
			{
			if ( iCodeblocks[j].CheckAddress( addr ) )
				{
				// To avoid recursive call to ReAlloc specifying granularity
				// Add address to the callstack
				iCallStack[arrayCounter] = ( addr );
				arrayCounter++;
				break;
				}
			}
		if ( arrayCounter == KATMaxCallstackLength ||
			 arrayCounter == iAllocMaxCallStack )
			{
			LOGSTR2( "ATMH > Wanted CallStack items ready( %i )", arrayCounter );
			break;
			}
		}
	// Log the memory allocation information
	if ( iLogOption == EATLogToTraceFast )
		{
		// Fast mode.
		ATFastLogMemoryAllocated( iProcessId, (TUint32) p, iCallStack, aSize, threadId );
		} 
	else if ( iLogOption == EATLogToFile )
		{
	    iLogFile.ATFileLogMemoryAllocated( (TUint32) p, iCallStack, aSize, threadId );
		}

 
    // Release the mutex
    iMutex.Signal(); 
    
    // Return the allocatated memory
    return p;
    }
#endif // __WINS__

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Free()
// Frees the allocated memory
// -----------------------------------------------------------------------------
//
TAny RAnalyzeToolAllocator::Free( TAny* aPtr )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Free()" );

    // Acquire the mutex
    iMutex.Wait();
    
    // get thread ID
    TUint threadId = RThread().Id();
    
	// Reset the callstack
	iFreeCallStack.Reset();
	
	if ( iFreeMaxCallStack > 0 )
		{
		// Find the current thread callstack start address
		TUint32 stackstart( 0 );
		TBool found( FindCurrentThreadStack( stackstart ) );
		LOGSTR3( "ATMH > stackstart: %x , found = %i", stackstart, found );
		TUint32 _sp;
		
		#ifdef __WINS__
			__asm
				{
				mov [_sp], esp
				}
		#else
			_sp = __current_sp();
		#endif
		
		// Get codeblocks count
		TInt blocksCount( iCodeblocks.Count() );
		TUint arrayCounter = 0;
	
		for ( TUint32 i = _sp; i < stackstart; i = i + KAddressLength )//lint !e1055 !e526 !e628 !e348
			{
			TUint32 addr = (TUint32) *( (TUint32*) i );
			if ( ! IsAddressLoadedCode( addr ) )
				continue;
			for ( TInt j = 0; j < blocksCount; j++ )
				{
				if ( iCodeblocks[j].CheckAddress( addr ) )
					{
					// To avoid recursive call to ReAlloc specifying granularity
					// Add address to the callstack
					iFreeCallStack[arrayCounter] = addr;
					arrayCounter++;
					break;
					}
				}
			if ( arrayCounter == KATMaxFreeCallstackLength ||
				 arrayCounter == iFreeMaxCallStack )
				{
				break;
				}
			}
		LOGSTR2( "ATMH > iFreeCallStack count ( %i )", arrayCounter );
		}
	
	// Log freed memory.
	if ( iLogOption == EATLogToTraceFast )
		{
		// Using fast mode.
		ATFastLogMemoryFreed( iProcessId, (TUint32) aPtr, iFreeCallStack, threadId );
		} 
	else if ( iLogOption == EATLogToFile )
		{
		iLogFile.ATFileLogMemoryFreed( (TUint32) aPtr, iFreeCallStack, threadId );
		}
    
    // Free the memory using original allocator
    iAllocator->Free( aPtr ); 
    
    LOGSTR2( "ATMH RAnalyzeToolAllocator::Free() - aPtr: %x", (TUint32)aPtr );
    
    // Release the mutex
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Open()
// Opens this heap for shared access. Opening the heap increases 
// the heap's access count by one.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::Open()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Open()");
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Share the memory using original allocator
    TInt error = iAllocator->Open();
    
    // If everything is OK add thread to the array which use this allocator
    if ( KErrNone == error )
        {
        TThreadParamsBuf params;
        params().iThreadId = RThread().Id().operator TUint();
        error = iAnalyzeTool.ThreadStack( params );

        __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantAppendToTheArray ) );

        if ( KErrNone == error )
            {
            LOGSTR2( "ATMH Thread stack address: %x", params().iStackAddress );
            LOGSTR2( "ATMH Thread stack size:    %x", params().iStackSize );
            iThreadArray.Append( TThreadStack( RThread().Id(), 
                    params().iStackAddress + params().iStackSize ) );
            if ( iLogOption == EATLogToTraceFast )
            	{
                // log thread added
                ATFastLogThreadStarted( RProcess().Id().operator TUint(), RThread().Id().operator TUint()); 
            	}
            else if ( iLogOption == EATLogToFile )
				{
				iLogFile.ATFileLogThreadStarted( RThread().Id().operator TUint() );
				}
            }
        }
    
    // Release the mutex
    iMutex.Signal();
    
    // Return the error code
    return error;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Close()
// Closes this shared heap. Closing the heap decreases the heap's 
// access count by one.
// -----------------------------------------------------------------------------
//
void RAnalyzeToolAllocator::Close()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Close()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Close the memory using original allocator
    iAllocator->Close();
    
    TInt count = iThreadArray.Count();
    
    // Iterate through array of threads to remove current thread
    for ( TInt i = 0; i < count; i++ )
        {
        // Check if this is current thread
        if ( iThreadArray[ i ].Match() )
            {
            // Remove the thread
            iThreadArray.Remove( i );
            if ( iLogOption == EATLogToTraceFast )
            	{
                // log thread removed
                ATFastLogThreadEnded( RProcess().Id().operator TUint(), RThread().Id().operator TUint() ); 
            	}
            else if ( iLogOption == EATLogToFile )
				{
				iLogFile.ATFileLogThreadEnded( RThread().Id().operator TUint() );
				}
            
            break;
            }
        }
    
    // Release the mutex
    iMutex.Signal();
    }

#ifdef __WINS__

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::ReAlloc()
// Increases or decreases the size of an existing cell.
// -----------------------------------------------------------------------------
//
TAny* RAnalyzeToolAllocator::ReAlloc( TAny* aPtr, TInt aSize, TInt aMode )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::ReAlloc()" );

    // Acquire the mutex
    iMutex.Wait();
    
    // get thread ID
    TUint threadId = RThread().Id();

    // Realloc the memory using original allocator
    TAny* ptr = iAllocator->ReAlloc( aPtr, aSize, aMode );
    
    // NULL addresses are not in a process under test
    if ( ptr && !( aMode & ENeverMove ) )
        {
        LOGSTR3( "ATMH RAnalyzeToolAllocator::ReAlloc() - aPtr: %x, ptr: %x", 
                (TUint32)aPtr, (TUint32)ptr );
        LOGSTR3( "ATMH RAnalyzeToolAllocator::ReAlloc() - aSize: %i, aMode: %i", 
                aSize, aMode );


		// Reset the callstack
		iReCallStack.Reset(); 

		// Find the current thread callstack start address
		TUint32 stackstart( 0 ); 
		TBool found( FindCurrentThreadStack( stackstart ) );
		LOGSTR3( "ATMH > stackstart: %x , found = %i", stackstart, found );
		
		// Get current sp
		TUint32 _sp( 0 );
		__asm
			{
			mov [_sp], esp
			}
		
		// Get codeblocks count
		TInt blocksCount( iCodeblocks.Count() );
		TInt error( KErrNone );
		TUint arrayCounter = 0;
		
		for ( TUint32 i = _sp; i < stackstart; i = i + KAddressLength )//lint !e1055 !e526 !e628 !e348
			{
			TUint32 addr = (TUint32) *( (TUint32*) i );
			if ( ! IsAddressLoadedCode( addr ) )
				continue;
			for ( TInt j = 0; j < blocksCount; j++ )
				{
				if ( iCodeblocks[j].CheckAddress( addr ) )
					{
					// To avoid recursive call to ReAlloc specifying granularity
					// Add address to the callstack
					iReCallStack[arrayCounter] = addr;
					arrayCounter++;
					break;
					}
				}
			if ( arrayCounter == KATMaxCallstackLength || 
				 arrayCounter == iAllocMaxCallStack )
				{
				LOGSTR2( "ATMH > Wanted CallStack items ready( %i )", arrayCounter );
				break;
				}
			}

		if ( iLogOption == EATLogToTraceFast )
			{
			// Using fast logging mode.
			ATFastLogMemoryReallocated( iProcessId, (TUint32) aPtr, (TUint32) ptr, iReCallStack, aSize, threadId );
			}
		else if ( iLogOption == EATLogToFile )
			{
			iLogFile.ATFileLogMemoryReallocated( (TUint32) aPtr, (TUint32) ptr, iReCallStack, aSize, threadId );
			}	
		}
    
    // Release the mutex
    iMutex.Signal();

    // Return pointer to the reallocated cell
    return ptr; 
    }

#else

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::ReAlloc()
// Increases or decreases the size of an existing cell.
// -----------------------------------------------------------------------------
//
TAny* RAnalyzeToolAllocator::ReAlloc( TAny* aPtr, TInt aSize, TInt aMode )
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::ReAlloc()" );

    // Acquire the mutex
    iMutex.Wait();
    
    // get thread ID
    TUint threadId = RThread().Id();

    // Realloc the memory using original allocator
    TAny* ptr = iAllocator->ReAlloc( aPtr, aSize, aMode );

    // NULL addresses are not in a process under test
    if ( ptr && !( aMode & ENeverMove ) )
        {
        LOGSTR3( "ATMH RAnalyzeToolAllocator::ReAlloc() - aPtr: %x, ptr: %x", 
                (TUint32)aPtr, (TUint32)ptr );
        LOGSTR3( "ATMH RAnalyzeToolAllocator::ReAlloc() - aSize: %i, aMode: %i", 
                aSize, aMode );

		// Reset the callstack
		iReCallStack.Reset(); 

		// Find the current thread callstack start address
		TUint32 stackstart( 0 ); 
		TBool found( FindCurrentThreadStack( stackstart ) );
		LOGSTR3( "ATMH > stackstart: %x , found = %i", stackstart, found );
		
		// Get codeblocks count
		TInt blocksCount( iCodeblocks.Count() );

		TUint arrayCounter = 0;
		
		for ( TUint32 i = __current_sp(); i < stackstart; i = i + KAddressLength )//lint !e1055 !e526 !e628 !e348
			{
			TUint32 addr = (TUint32) *( (TUint32*) i );
			if ( ! IsAddressLoadedCode( addr ) )
				continue;
			for ( TInt j = 0; j < blocksCount; j++ )
				{
				if ( iCodeblocks[j].CheckAddress( addr ) )
					{
					// To avoid recursive call to ReAlloc specifying granularity
					// Add address to the callstack
					iReCallStack[arrayCounter] = ( addr );
					arrayCounter++;
					break;
					}
				}
			if ( arrayCounter == KATMaxCallstackLength || 
				 arrayCounter == iAllocMaxCallStack )
				{
				LOGSTR2( "ATMH > Wanted CallStack items ready( %i )", arrayCounter );
				break;
				}
			}

		if ( iLogOption == EATLogToTraceFast )
			{
			// Using fast logging mode.
			ATFastLogMemoryReallocated( iProcessId, (TUint32) aPtr, (TUint32) ptr, iReCallStack, aSize, threadId );
			}
		else if ( iLogOption == EATLogToFile )
			{
			iLogFile.ATFileLogMemoryReallocated( (TUint32) aPtr, (TUint32) ptr, iReCallStack, aSize, threadId );
			}
		
		
		}

    // Release the mutex
    iMutex.Signal();

    // Return pointer to the reallocated cell
    return ptr; 
    }

#endif // __WINS__

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Compress()
// The function frees excess committed space from the top of the heap.
// The size of the heap is never reduced below the minimum size 
// specified during creation of the heap.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::Compress()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Compress()" );

    // Acquire the mutex
    iMutex.Wait();

    // Compress the memory using original allocator
    TInt compress = iAllocator->Compress();

    // Release the mutex
    iMutex.Signal();

    // Return the space reclaimed
    return compress;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Reset()
// Frees all allocated cells on this heap. 
// -----------------------------------------------------------------------------
//
void RAnalyzeToolAllocator::Reset()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Reset()" );

    // Acquire the mutex
    iMutex.Wait();

    // Reset the memory using original allocator
    iAllocator->Reset();

    // Release the mutex
    iMutex.Signal();
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::AllocSize()
// Gets the number of cells allocated on this heap, and 
// the total space allocated to them.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::AllocSize( TInt& aTotalAllocSize ) const
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::AllocSize()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Acquire the memory information using original allocator
    TInt size = iAllocator->AllocSize( aTotalAllocSize );
    
    // Release the mutex
    iMutex.Signal();
    
    // Return the number of cells allocated on this heap.
    return size;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Available()
// Gets the total free space currently available on the heap and the 
// space available in the largest free block. The space available 
// represents the total space which can be allocated. Note that 
// compressing the heap may reduce the total free space available 
// and the space available in the largest free block.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::Available( TInt& aBiggestBlock ) const
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Available()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Acquire the memory information using original allocator
    TInt available = iAllocator->Available( aBiggestBlock );
    
    // Release the mutex
    iMutex.Signal();
    
    // Return the total free space currently available on the heap
    return available;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::AllocLen()
// Gets the length of the available space in the specified 
// allocated cell.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::AllocLen( const TAny* aCell ) const
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::AllocLen()" ); 
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Acquire the memory information using original allocator
    TInt len = iAllocator->AllocLen( aCell );
    
    // Release the mutex
    iMutex.Signal();
    
    // Return the length of the available space in the allocated cell.
    return len;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::DebugFunction()
// Invocates specified debug funtionality.
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::DebugFunction( TInt aFunc, TAny* a1, TAny* a2 )
    {
    LOGSTR2( "ATMH RAnalyzeToolAllocator::DebugFunction() %i", aFunc );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Invocate debug funtion using original allocator
    TInt debug = iAllocator->DebugFunction( aFunc, a1, a2 );
    
    switch( aFunc )
		{  
		case EMarkEnd:
			{
			// Disables the __UHEAP_MARKEND macro
			LOGSTR1( "ATMH __UHEAP_MARKEND macro called" );
			if ( debug > 0 )
				{
				LOGSTR2( "ATMH __UHEAP_MARKEND detects leaks: %d", debug );
				// Because there is leaks the alloc panic will occur but
				// lets return a zero to pretend that everything is OK
				debug = 0;
				}
			}
		break;
		
		default:
			{
			}
		break;
		}
    
    // Release the mutex
    iMutex.Signal();
    
    // Return information of the debug function success
    return debug;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::Extension_()
// Extension function
// -----------------------------------------------------------------------------
//
TInt RAnalyzeToolAllocator::Extension_( TUint aExtensionId, TAny*& a0, 
    TAny* a1 ) 
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::Extension_()" );
    
    // Acquire the mutex
    iMutex.Wait();
    
    // Invocate extension funtion using original allocator
    TInt ext = RAllocator::Extension_( aExtensionId, a0, a1 );
    
    // Release the mutex
    iMutex.Signal();
    
    // Return information of the extension function success
    return ext;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::ShareHeap()
// Share heap with other thread
// -----------------------------------------------------------------------------
//
void RAnalyzeToolAllocator::ShareHeap()
    {
    LOGSTR1( "ATMH RAnalyzeToolAllocator::ShareHeap()" );
    
    // Call the overwrited Open function
    Open();
    }
    
// -----------------------------------------------------------------------------
// RAnalyzeToolAllocator::FindCurrentThreadStack()
// Find the current thread which is using the heap
// -----------------------------------------------------------------------------
//
TBool RAnalyzeToolAllocator::FindCurrentThreadStack( TUint32& aStackStart )
    {
    LOGSTR2( "ATMH RAnalyzeToolAllocator::FindCurrentThreadStack(), count( %i )", 
            iThreadArray.Count() );
    
    // Flag for indicating that right thread has been found
    TBool found( EFalse );
    // If threre is only one thread it must be the right thread
    if ( iThreadArray.Count() == KThreadCount )
        {
        if ( !iThreadArray[ 0 ].ThreadStackStart( aStackStart ) )
            {
            // This MUST BE the right thread
            //__ASSERT_ALWAYS( EFalse, AssertPanic( ECantFindRightThread ) );
            }
        else if ( iThreadArray[ 0 ].ThreadStackStart( aStackStart ) )
            {
            found = ETrue;
            }
        }
    else
        {
        // Iterate through array to find right thread
        TInt count( iThreadArray.Count() );
        
        for ( TInt i = 0; i < count; i++ )
            {
            // Check if this is the right thread
            if ( iThreadArray[ i ].ThreadStackStart( aStackStart ) )
                {
                // Right thread found. Mark the flag
                found = ETrue;
                break;
                }
            }
        // If right thread was not found the panic must be raised
        if ( !found )
            {
            //__ASSERT_ALWAYS( EFalse, AssertPanic( ECantFindRightThread ) );
            }
        }
    return found;
    }

// End of File
