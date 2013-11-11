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
* Description:  Declaration of the class DAnalyzeToolChannel
*
*/


#ifndef __ANALYZETOOLCHANNEL_H__
#define __ANALYZETOOLCHANNEL_H__

// INCLUDES
#include <analyzetool/analyzetool.h>
#include <kernel/kernel.h>
#ifdef __WINSCW__
#include <memmodel/emul/win32/memmodel.h>
#endif // __WINSCW__

// FORWARD DECLARATIONS
class DAnalyzeToolEventHandler;

// CLASS DECLARATION

/**
* The implementation of the abstract base class for a logical channel.
*/
class DAnalyzeToolChannel : public DLogicalChannel
	{
	public:
		
        /**
        * C++ default constructor.
        */
		DAnalyzeToolChannel();
		
        /**
        * Destructor.
        */
		~DAnalyzeToolChannel();
		
	protected: // from DLogicalChannel
		
        /**
        * Creates the logical channel.
        * @param aUnit A unit of the device.
        * @param anInfo A pointer to an explicit 8-bit descriptor containing 
        			    extra information for the physical device
        * @param aVer The required version of the logical device
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		virtual TInt DoCreate( TInt aUnit, 
							   const TDesC8* anInfo, 
							   const TVersion &aVer );
    
        /**
        * Handles a client request.
        * @param aFunction The requested function.
        * @param a1 A 32-bit value passed to the kernel-side. Its meaning 
        			  depends on the device driver requirements
        * @param a2 A 32-bit value passed to the kernel-side. Its meaning
        			  depends on the device driver requirements
        * @param aMessage Reference to received thread message.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		virtual TInt DoControl( TInt aFunction, TAny* a1, TAny* a2, TThreadMessage& aMessage );	
		
		/**
		* Processes a message for this logical channel.
		* This function is called in the context of a DFC thread.
		* @param aMsg  The message to process.
		*/
		virtual void HandleMsg( TMessageBase* aMsg );
		
        /**
        * Handles a client asynchronous request.
        * @param aFunction The requested function.
        * @param aStatus Pointer to client TRequestStatus.
        * @param a1 A 32-bit value passed to the kernel-side. Its meaning 
        			  depends on the device driver requirements
        * @param a2 A 32-bit value passed to the kernel-side. Its meaning
        			  depends on the device driver requirements
        * @param aMessage Reference to received thread message.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		virtual TInt DoRequest( TInt aFunction, 
								TRequestStatus* aStatus, 
								TAny* a1, 
								TAny* a2, 
								TThreadMessage& aMessage );
		
        /**
        * Cancels outstanding asynchronous request.
        */
		virtual void DoCancel();

	private:
		
        /**
        * Acquires current process information
        * @param aProcessInfo Pointer to the TProcessIdentityParams object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt GetProcessInfo( TAny* aProcessInfo, TThreadMessage& aMessage );
		
        /**
        * Acquires codeseg information.
        * @param aCodesegInfo Pointer to the TCodesegInfo object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt GetCodesegInfo( TAny* aCodesegInfo, TThreadMessage& aMessage );
		
        /**
        * Acquires library information.
        * @param aLibraryInfo Pointer to the TLibraryInfo object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt GetLibraryInfo( TAny* aLibraryInfo, TThreadMessage& aMessage );
	
        /**
        * Acquires information about process main thread RAllocator.
        * @param aMainThreadParams Pointer to the TMainThreadParams object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt MainThreadAllocator( TAny* aMainThreadParams, 
								  TThreadMessage& aMessage );
		
		/**
		* Acquires main thread stack address.
		* @param aThreadStack Pointer to the TThreadParams object.
		* @return TInt Returns KErrNone, if successful
					   otherwise one of the other system-wide error codes
		*/
		TInt ThreadStack( TAny* aThreadStack, 
						  TThreadMessage& aMessage );

        /**
        * Acquires information about process global handles.
        * @param aProcessHandleInfo Pointer to the TProcessHandleInfo object.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt GetProcessHandleInfo( TAny* aProcessHandleInfo, 
								   TThreadMessage& aMessage );

        /**
        * Acquires a process's current handle count.
        * @param aProcessHandles Pointer to the TATProcessHandles object.
        * @return TInt Returns KErrNone, if successful
                       otherwise one of the other system-wide error codes
        */
        TInt GetCurrentHandleCount( TAny* aProcessHandles, 
									TThreadMessage& aMessage );
        
        /**
		* Acquires the count of current device driver users.
		* @param aClientCount A reference to TInt variable
		* @return TInt Returns KErrNone, if successful
        *               otherwise one of the other system-wide error codes
		*/			
        TInt ClientCount( TAny* aClientCount, 
						  TThreadMessage& aMessage );
		
        /**
         * Acquires memory model system uses.
         * @return TInt Returns KErrNone, if successful
         *              otherwise one of the other system-wide error codes
         */
        TInt GetMemoryModel( TAny* aMemoryModel,
                            TThreadMessage& aMessage );
        
        #ifdef __WINSCW__
        /**
		* Gets module dependencies
		* @param aModule Module handle
		*/
        void GetModuleDependencies( HMODULE aModule );
        #endif // __WINSCW__
        
	private: //Member variables
		
		/* Handler which receives kernel events */
		DAnalyzeToolEventHandler* iEventHandler;
		
		#ifdef __WINSCW__
		/* Code segment array */
		RArray<TCodesegInfo> iCodeSeg;
		#endif // __WINSCW__
		
		/* A DFC queue for communication */
		TDynamicDfcQue* iOwnDfcQ;

	};

#endif // #ifndef __ANALYZETOOLCHANNEL_H__

// End of File
