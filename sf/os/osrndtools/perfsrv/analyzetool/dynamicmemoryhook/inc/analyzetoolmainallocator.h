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
* Description:  Declaration of the class RAnalyzeToolMainAllocator.
*
*/


#ifndef ANALYZETOOLMAINALLOCATOR_H
#define ANALYZETOOLMAINALLOCATOR_H

// INCLUDES
#include <u32std.h>
#include "codeblock.h"
#include <analyzetool/analyzetool.h>
#include <analyzetool/atcommon.h>
#include "analyzetoolmemoryallocator.h"
#include "analyzetooleventhandlernotifier.h"
#include "analyzetoolfilelog.h"

// FORWARD DECLARATIONS
class CLibraryEventHandler;

// CLASS DECLARATION

/**
*  Class which overloads the RAlloctor functions and provides access to 
*  the overloaded functions  
*/
class RAnalyzeToolMainAllocator : public RAnalyzeToolMemoryAllocator, 
								  public MAnalyzeToolEventhandlerNotifier
    {

    public:

        /**
        * C++ default constructor.
        * @param aNotFirst Is this first thread using this heap
        * @param aFileName The name of the log file
        * @param aLogOption The logging option
        * @param aIsDebug Determines whether a binary is UDEB or UREL
        * @param aAllocCallStackSize Max number of stored callstack items when memory allocated
        * @param aFreeCallStackSize Max number of stored callstack items when memory freed
        * @param aAtoolVersion Version of atool
        * @param aApiVersion Version of api
        */
        RAnalyzeToolMainAllocator( TBool aNotFirst,
                const TFileName& aFileName,
                const TPath& aFilePath,
                TUint32 aLogOption, TUint32 aIsDebug,
                TUint32 aAllocCallStackSize,
                TUint32 aFreeCallStackSize,
                const TDesC8& aAtoolVersion,
                const TDesC8& aApiVersion );

        /**
        * Destructor.
        */
        ~RAnalyzeToolMainAllocator();

        /**
        * Allocates a cell of specified size from the heap.
        * @param aSize The size of the cell to be allocated from the heap. 
        * @return TAny* A pointer to the allocated cell.
        */
        TAny* Alloc( TInt aSize );

        /**
        * Frees the specified cell and returns it to the heap.
        * @param aPtr A pointer to a cell to be freed.
        */
        void Free( TAny* aPtr );

        /**
        * Increases or decreases the size of an existing cell.
        * @param aPtr A pointer to the cell to be reallocated.
        * @param aSize The new size of the cell. This may be bigger 
        *              or smaller than the size of the original cell.
        * @param aMode Flags controlling the reallocation.
        * @return TAny* A pointer to the reallocated cell. This may be the 
        *               same as the original pointer supplied through aCell.
        */
        TAny* ReAlloc( TAny* aPtr, TInt aSize, TInt aMode = 0 );
        
        /**
        * Gets the length of the available space in the specified 
        * allocated cell.
        * @param aCell A pointer to the allocated cell.
        * @return TInt The length of the available space in the allocated cell.
        */
        TInt AllocLen(const TAny* aCell) const;

    #ifndef __KERNEL_MODE__

        /**	
        * Opens this heap for shared access. Opening the heap increases 
        * the heap's access count by one.
        */
        TInt Open();
        
        /**
        * Closes this shared heap. Closing the heap decreases the heap's 
        * access count by one.
        */  	
        void Close();

        /**
        * The function frees excess committed space from the top of the heap.
        * The size of the heap is never reduced below the minimum size 
        * specified during creation of the heap.
        * @return TInt The space reclaimed. If no space can be reclaimed, 
                       then this value is zero.
        */
        TInt Compress();

        /**
        * Frees all allocated cells on this heap. 
        */
        void Reset();

        /**
        * Gets the number of cells allocated on this heap, and 
        * the total space allocated to them.
        * @param aTotalAllocSize On return, contains the total 
        *						 space allocated to the cells.
        * @return TInt The number of cells allocated on this heap.
        */
        TInt AllocSize( TInt& aTotalAllocSize ) const;
        
        /**
        * Gets the total free space currently available on the heap and the 
        * space available in the largest free block. The space available 
        * represents the total space which can be allocated. Note that 
        * compressing the heap may reduce the total free space available 
        * and the space available in the largest free block.
        * @param aBiggestBlock On return, contains the space available 
        *                      in the largest free block on the heap.
 
        * @return TInt The total free space currently available on the heap.

        */
        TInt Available( TInt& aBiggestBlock ) const;

	#endif
        
        /**
        * Invocates specified debug funtionality.
        * @param aFunc The debug function
        * @param a1 Debug function specific paramenter.
        * @param a2 Debug function specific paramenter.
        * @return TInt Returns KErrNone, if successful otherwise one 
        *              of the other system-wide error codes.
        */
        TInt DebugFunction( TInt aFunc, TAny* a1 = NULL, TAny* a2 = NULL );
        
        // From MAnalyzeToolEventhandlerNotifier
        
        /**
		* Remove killed thread from threads array.
		* @param aThreadId - Thread Id
		*/
		void RemoveKilledThread( const TUint aThreadId );
		
    protected:

        /**
        * Extension function
        * @param aExtensionId The extension id
        * @param a0 Extension specific paramenter.
        * @param a1 Extension specific paramenter.
        * @return TInt Returns KErrNone, if successful otherwise one 
        *              of the other system-wide error codes. 
        */
        TInt Extension_( TUint aExtensionId, TAny*& a0, TAny* a1 );

    public: // from RAnalyzeToolMemoryAllocator
        
        /**
        * Installs the RTraceAllocator allocator
        */
        void Uninstall();
        
        /**
        * Shares the heap
        */
        void ShareHeap();

    public: // inlines
            
        /**
        * Acquires the codeblocks of the process
        * @return RArray<TCodeblock>& The process codeblocks
        */
        inline RArray<TCodeblock>& Codeblocks();
        
        /**
        * Acquires the mutex used to access shared objects
        * @return RMutex& A reference to open mutex
        */
        inline RMutex& Mutex();
        
        /**
        * Acquires the current process id
        * @return TInt The process id
        */
        inline TInt ProcessId();

        /**
		* Acquires the logical channel handle
		* @return RAnalyzeTool A reference to logical channel
		*/
        inline RAnalyzeTool& AnalyzeTool();
                
        /**
        * Acquires the log option type 
        * @return TUint32 iLogOption
        */        
        inline TUint32 LogOption();
        
        /**
        * Acquires the max size of call stack when memory allocated
        * @return TUint32 iAllocMaxCallStack
        */        
        inline TUint32 AllocMaxCallStack();
        
        /**
         * Acquires the max size of call stack when memory freed
         * @return TUint32 iFreeMaxCallStack
         */
        inline TUint32 FreeMaxCallStack();
        
        /**
         * Acquires handle to lig file
         * @return RATFileLog iLogFile
         */
        inline RATFileLog& LogFile();

    private: // private functions

        /**
        * Log the process initial information
        * @param aFileName The name of the log file
        * @param aLogOption The logging option
        * @param aIsDebug Determines whether a binary is UDEB or UREL
        */
        void LogProcessInformation( const TFileName& aFileName,  const TPath& aFilePath,  TUint32 aLogOption,
        		TUint32 aIsDebug, const TDesC8& aAtoolVersion, const TDesC8& aApiVersion );

        /**
        * Find the current thread which is using the heap
        * @param aStackStart A reference where the stack start is stored
        * @return TBool ETrue if a thread can be found, EFalse otherwise
        */
        TBool FindCurrentThreadStack( TUint32& aStackStart );
        
        /**
        * Installs the eventhandler, if possible
        */
        void InstallEventHandler();
        
        /**
        * Logs version of ATApp, ATApi, S60 version and ROM checksum 
        * at the startup of hooked application
        */
        void LogDeviceInfo();

    private: // member variables

        /* Handle to the RAnalyzeTool */		
        RAnalyzeTool iAnalyzeTool;           
        
        /* A flag for indicating that the RAnalyzeTool is open */
        TBool iAnalyzeToolOpen;

        /* A flag for indicating that the device driver is loaded */
        TBool iDeviceDriverLoaded;
        
        /* The codeblocks of the process */
        RArray<TCodeblock> iCodeblocks;

        /* The handler for kerner events */
        CLibraryEventHandler* iEventHandler;

        /* The mutex for serializing access to the shared resources */
        mutable RMutex iMutex;	
        	
        /* Array for storing the callstack */	
        TFixedArray <TUint32, KATMaxCallstackLength> iCallStack;

        /* Array for storing the reallocation callstack */	
        TFixedArray <TUint32, KATMaxCallstackLength> iReCallStack;

        /* Array for storing the reallocation callstack */	
        TFixedArray<TUint32, KATMaxFreeCallstackLength> iFreeCallStack;
                
        /* Array of threads using this heap */
        RArray<TThreadStack> iThreadArray;
        
        /* Log option */
        TUint32 iLogOption;

        /* The process id */
        TUint iProcessId;	
        
        /* Max items on stored call stack when memory allocated */
        TUint32 iAllocMaxCallStack;
        
        /* Max items on stored call stack when memory freed */
        TUint32 iFreeMaxCallStack;
        
        /* Binary log file */
        RATFileLog iLogFile;
       
        
    };

// INLINES
#include "analyzetoolmainallocator.inl"

#endif // ANALYZETOOLMAINALLOCATOR_H

// End of File
