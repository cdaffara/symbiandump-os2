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
* Description:  Declaration of the class DAnalyzeToolEventHandler.
*
*/


#ifndef __ANALYZETOOLEVENTHANDLER_H__
#define __ANALYZETOOLEVENTHANDLER_H__

// INCLUDES
#include <analyzetool/analyzetool.h>
#include "atlog.h"
#include <kernel/kernel.h>

// CLASS DECLARATION

/**
* Callback class for kernel events
*/

class DAnalyzeToolEventHandler : public DKernelEventHandler
	{
	public:
	
        /**
        * C++ default constructor.
        */
		inline DAnalyzeToolEventHandler( TDfcQue* aDfcQ );
		
        /**
        * Destructor.
        */ 
		~DAnalyzeToolEventHandler();
		
        /**
        * Second stage constructor.
        * @param aDevice A pointer to device where the event handler belongs.
        * @param aProcessId Owner process id.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		TInt Create( DLogicalDevice* aDevice, const TUint aProcessId );
		
        /**
        * Subscribes library event.
        * @param aStatus The request status object for this request. 
        * @param aLibraryInfo The library information which 
        					  is filled by the device driver
        * @param aMessage Reference to received thread message.
        * @return TInt Returns KErrNone, if successful
        			   otherwise one of the other system-wide error codes
        */
		void InformLibraryEvent( TRequestStatus* aStatus, 
								 TAny* aLibraryInfo,
								 TThreadMessage& aMessage );
        
        /**
        * Cancels subscribetion of the library event.
        */
		void CancelInformLibraryEvent();

        /**
        * Static function for DFC events.
        * @param aPtr Pointer to DAnalyzeToolEventHandler object. 
        */
	    static void EventDfc( TAny* aPtr );
		
	private:
	
        /**
        * Pointer to  callback function called when an event occurs.
        * @param aEvent Designates what event is dispatched.
        * @param a1 Event-specific paramenter.
        * @param a2 Event-specific paramenter.
        * @param aThis A pointer to the event handler
        * @return TUint Bitmask returned by callback function.
        */
		static TUint EventHandler( TKernelEvent aEvent, 
								   TAny* a1, 
								   TAny* a2, 
								   TAny* aThis );
	
	private:
	
        /**
        * Handles the EEventAddLibrary and EEventRemoveLibrary events
        * @param aLib* A pointer to added/removed library.
        * @param aThread* A pointer to thread where the libary is.
        * @param aInfo* A reference class to be written to the client
        */	
		void HandleLibraryEvent( DLibrary* aLib, 
								 DThread* aThread, 
								 TLibraryEventInfo& aInfo );
		
		/**
		* Handles the EEventKillThread events
		* @param aThread* A pointer to the thread being terminated. 
		* @param aInfo* A reference class
		*/	
		void HandleKillThreadEvent( DThread* aThread, 
									TLibraryEventInfo& aInfo );
		
		/**
		* Informs client about the occured event
		*/	
	    void DoEventComplete();
					
	private:
	
		/* Mutex for serializing access to event handler */
		DMutex* iHandlerMutex;
		
		/* Mutex fof serializing access to event handler variables */
		DMutex* iDataMutex;		
		
		/* If the client is a RAM-loaded LDD (or PDD), it is possible for the DLL to
		be unloaded while the handler is still in use.  This would result in an
		exception.  To avoid this, the handler must open a reference to the
		DLogicalDevice (or DPhysicalDevice) and close it in its d'tor. */
		DLogicalDevice* iDevice;    
				
		/* Owner process ID */
		TUint iProcessId;
		
		/* Event array */
		RArray<TLibraryEventInfo> iEventArray;
				
		/* Pointer to client's TRequestStatus */ 
		TRequestStatus* iClientRequestStatus;
		
		/* Pointer to client's thread */ 
		DThread* iClientThread;

		/* Pointer to client's TLibraryEventInfo */ 
		TAny* iClientInfo;
		
		/* DFC for informing events to the client */ 
		TDfc iEventDfc;
	};
	
// ----------------------------------------------------------------------------
// DAnalyzeToolEventHandler::DAnalyzeToolEventHandler()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
inline DAnalyzeToolEventHandler::DAnalyzeToolEventHandler( TDfcQue* aDfcQ ) :	
	DKernelEventHandler( EventHandler, this ),
	iEventDfc( EventDfc, this, 1 )
	{
	LOGSTR1( "ATDD DAnalyzeToolEventHandler::DAnalyzeToolEventHandler()" );
	iEventDfc.SetDfcQ( aDfcQ );
	}

#endif // __ANALYZETOOLEVENTHANDLER_H__

// End of File
