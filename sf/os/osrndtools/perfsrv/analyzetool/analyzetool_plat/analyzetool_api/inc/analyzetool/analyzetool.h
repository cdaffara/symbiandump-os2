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
* Description:  Declaration of the class RAnalyzeTool.
*
*/


#ifndef __ANALYZETOOL_H__
#define __ANALYZETOOL_H__

// INCLUDES
#include <e32cmn.h>

//Version information for command line engine.
//Tells the version of AT core componenets.
//ANALYZETOOL_CORE_VERSION_FOR_CLE 1.10.1

// CONSTANTS
inline TVersion KAnalyzeToolLddVersion() { return TVersion(1, 0, 1); }

/* The name of the analyze tool device driver*/
_LIT( KAnalyzeToolLddName, "AToolKernelEventHandler" );

/* The priority of AnalyzeTool Dfc */
const TInt KAnalyzeToolThreadPriority = 27;

/* The name of the AnalyzeTool DFC */
_LIT8( KAnalyzeToolThreadName, "AnalyzeToolThreadDfc" );

/* The panic literal */
_LIT( KClientPanic, "AnalyzeTool" );

//const TInt KATMaxCallstackLength = 20;

/* The device handler panic codes */
enum TPanic
	{
	EPanicRequestPending,
	EPanicNoRequestPending,
	EPanicUnsupportedRequest
	};

// Size of following must be multiple of 4 bytes.

class TMainThreadParams
	{
	public:
		RAllocator* iAllocator;
		TBool       iAlone;
		TUint       iProcessId;
	};
typedef TPckgBuf<TMainThreadParams> TMainThreadParamsBuf;

class TLibraryInfo
	{
	public:
		TBuf8<KMaxLibraryName> iLibraryName;
		TLinAddr  iRunAddress;
		TUint32   iSize;
		TInt      iIndex;
		TUint     iProcessId;
	};

typedef TPckgBuf<TLibraryInfo> TLibraryInfoBuf;

class TCodesegInfo
	{
	public:
		TBuf8<KMaxLibraryName> iFullName;
		TLinAddr iRunAddress;
		TUint32  iSize;
		TInt  iIndex;
		TUint iProcessId;
		TInt  iCodesegIndex; 
		TLinAddr iFileEntryPoint;
		TInt     iFuntionCount;
		TLibraryFunction iFirstFunction;
		TModuleMemoryInfo iMemoryInfo;
	};

typedef TPckgBuf<TCodesegInfo> TCodesegInfoBuf;

class TThreadParams
	{		
	public:
	    TLinAddr iStackAddress;
	    TInt iStackSize;
	    TUint iThreadId;
	};

typedef TPckgBuf<TThreadParams> TThreadParamsBuf;

class TProcessIdentityParams
	{		
	public:
		TBuf8<KMaxProcessName> iProcessName;
		TInt iDynamicCount;
		TInt iCodesegCount;
		TUint iProcessId;
		TUint iThreadId;
	    TLinAddr iStackAddress;
	    TInt iStackSize;
	};

typedef TPckgBuf<TProcessIdentityParams> TProcessIdentityParamsBuf;

class TLibraryEventInfo
	{
	public:
		enum TLibraryEvent
			{ 
			ELibraryAdded = 0, 
			ELibraryRemoved,
			EKillThread 
			};
					
	public:
		TUint iProcessId;
		TBuf8<KMaxLibraryName> iLibraryName;
		TLinAddr iRunAddress;
		TUint32 iSize;
		TLibraryEvent iEventType;
		TUint iThreadId;
	};

typedef TPckgBuf<TLibraryEventInfo> TLibraryEventInfoBuf;

class TProcessHandleInfo
	{
	public:
		TInt     iProcessHandleCount;
		TInt     iThreadHandleCount;
		TInt     iIndex;
		TInt     iThreadCount;
		TLinAddr iUserStackRunAddress;
		TInt     iUserStackSize;
		TUint    iProcessId;
	};

typedef TPckgBuf<TProcessHandleInfo> TProcessHandleInfoBuf;

/**
*  A class for particular process's current handle count
*/
class TATProcessHandles
    {
    public:
    
        /** The ID of the process. */
        TUint iProcessId;
        
        /** The number of current handles in the library. */
        TInt iCurrentHandleCount;
    };

typedef TPckgBuf<TATProcessHandles> TATProcessHandlesBuf;

class TClientCount
    {
    public:
    
        /** The count of clients */
        TInt iClientCount;     
    };

typedef TPckgBuf<TClientCount> TClientCountBuf;

class TATMemoryModel
    {
    public :
        /* Memory model*/
        TUint32 iMemoryModel;
    };
typedef TPckgBuf<TATMemoryModel> TATMemoryModelBuf;

// CLASS DECLARATION

/**
*  The user-side handle to a logical channel which provides functions to 
*  open a channel and to make requests to a analyze tool device driver. 
*/

class RAnalyzeTool : public RBusLogicalChannel
	{
	
	public:
		
	/** Enumeration of supported functions */
	enum TBasicAnalyzerControl
		{
		EGetProcessInfo = 0, /* The process information */
		EGetCodesegInfo,	 /* The codesegment information */
		EGetLibraryInfo,     /* The library information */
		EGetDynamicInfo,	 /* The count of dynamic code in the process */
		ELibraryEvent,       /* Subscribe events from library events */
		ECancelLibraryEvent, /* Cancel subscribetion of library events */
		ECurrentClientCount,  /* The count of clients in device driver */
		EMainThreadAlloctor,
		EThreadStack,
		EGetProcessHandle,	/* Gets process global handles info*/
		EGetCurrentHandles, /* Get a process's current handle count */
		EGetMemoryModel
		};
			
#ifndef __KERNEL_MODE__

        /**
        * Opens a handle to a logical channel.
        * @return TInt Returns KErrNone, if successful or otherwise 
        		  one of the other system-wide error codes
        */
		inline TInt Open();

        /**
        * Acquires process information.
        * @param aProcessIdentityParams The process information which 
        								is filled by the device driver
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline TInt GetProcessInfo( 
				TProcessIdentityParamsBuf& aProcessIdentityParams );

        /**
        * Acquires codeseg information.
        * @param aCodesegInfo The codeseg information which 
        					  is filled by the device driver
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline TInt GetCodesegInfo( TCodesegInfoBuf& aCodesegInfo );
		
        /**
        * Acquires library information.
        * @param aLibraryInfo The library information which 
        					  is filled by the device driver
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline TInt GetLibraryInfo( TLibraryInfoBuf& aLibraryInfo );
				
        /**
        * Subscribes library event.
        * @param aStatus The request status object for this request. 
        * @param aLibraryInfo The library information which 
        					  is filled by the device driver
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline void LibraryEvent( TRequestStatus& aStatus, 
		                          TLibraryEventInfo& aLibraryInfo );
		
        /**
        * Cancels subscribetion of the library event.
        */
		inline void CancelLibraryEvent();

        /**
        * Acquires device driver current client count.
        * @param aClientCount A reference to TInt which is 
        				      updated by the device driver.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline TInt ClientCount( TClientCountBuf& aClientCount );
		
        /**
        * Acquires process main thread RAllocator
        * @param aMainThreadParams The main thread information which 
        					        is filled by the device driver
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */	
		inline TInt MainThreadAlloctor( TMainThreadParamsBuf& aMainThreadParams );
		
		/**
		* Acquires main thread stack address.
		* @param aThreadStack Pointer to the TThreadParams object.
		* @return TInt Returns KErrNone, if successful
					   otherwise one of the other system-wide error codes
		*/
		inline TInt ThreadStack( TThreadParamsBuf& aThreadStack );

        /**
        * Acquires information about process global handles.
        * @param aProcessHandleInfo Pointer to the TProcessHandleInfo object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		inline TInt GetProcessHandleInfo( TProcessHandleInfoBuf& aProcessHandleInfo );

        /**
        * Acquires a process's current handle count.
        * @param aProcessHandles Pointer to the TATProcessHandles object.
        * @return TInt Returns KErrNone, if successful
                       otherwise one of the other system-wide error codes
        */
        inline TInt GetCurrentHandleCount( TATProcessHandlesBuf& aProcessHandles );

        /**
         * Acquires memory model system uses.
         * @param aMemoryModel pointer to the TATMemoryModelBuf object.
         */
        inline TInt GetMemoryModel( TATMemoryModelBuf& aMemoryModel );
        
#endif // #ifndef __KERNEL_MODE__
	};

// INLINES
#include <analyzetool/analyzetool.inl>

#endif // #ifndef __ANALYZETOOL_H__

// End of File
