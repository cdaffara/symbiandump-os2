// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of the CEComServerSession class
// 
//

/**
 @internalComponent
 @file
*/

#ifndef __ECOMSERVERSESSION_H__
#define __ECOMSERVERSESSION_H__

#include <s32mem.h>
#include "EComServer.h"

#include "clientrequest.h"

// __________________________________________________________________________
//
/**
	@internalComponent
	@since 7.0
	Provides the server side session which
	handles the messages from REComSession, unpacking the input
	message parameters, calling the server and packaging the 
	the returned data as output message parameters.
	Warning : The implementation of the ListImplementations
	method for returning an arbitrary sized data set
	uses a 2 phase collection method that
	will not work IF the session is shared and two 
	client calls on this method are interleaved.
	It leads to incorrect results for one of them,
	with potentially fatal results. so...
	DO NOT SHARE SERVER SIDE SESSIONS BETWEEN CLIENTS
 */

class CEComServerSession : public CSession2
	{
public:
	
	CEComServerSession();

	
	void CreateL();
/**
	@fn				void CompleteNotifications(TInt aCompletionCode)
	Intended Usage	: Completes all the outstanding notification requests with the
					supplied error code.
	@since			7.0
	@param			aCompletionCode The code with which to complete the notifications.
	@pre 			REComServerSession is fully constructed.
	@post			The notifications contained on the iNotificationRequests array
					have been completed with aCompletionCode, and the array has 
					been cleared.
 */
	
	void CompleteNotifications(TInt aCompletionCode);

private:
	
	inline CEComServer& Server();

	
	~CEComServerSession();

	
	void ServiceL(const RMessage2& aMessage);

	void ServiceL(const TClientRequest& aMessage);

	
	inline TBool ReceivePending() const;

	
	void DoListImplementationsL(const TClientRequest& aMessage);

	
	TBool DoCollectListL(const TClientRequest& aMessage);
	
	void DoListExtendedInterfacesL(const TClientRequest& aMessage);
	
	void DoGetResolvedImplementationL(const TClientRequest& aMessage);
	void UnpackMatchStrAndExtendedInterfacesFromClientL(const TClientRequest& aMessage,
														RExtendedInterfacesArray& aExtendedInterfaces,
														RBuf8& aMatchStr);
	
	void DoSetGetParametersL(const TClientRequest& aMessage);
/**
	@fn				CleanupInternalList()
	Intended Usage	: Releases the list of CImplementationInformation pointers
					 that are stored within the iList variable after a successful 
					 call to the DoListImplementations service.
	@since			7.0
	@pre 			REComServerSession is fully constructed.
	@post			The resources held by iList are released, and iList is NULL.
 */
	
	void CleanupInternalList();

private:
	/** 
		The externalised data from a ListImplementations service call
		held for return, pending the collection service call.
	 */
	
	CBufFlat* iMemoryStore;
	
	/** The last ListImplementations service request */
	
	TInt iListContext;

	/** 
		A successful ListImplementations service request 
		generates a list of CImplementationInformation pointers
		that are stored within this list.
	 */
	
	RImplInfoArray* iList;

	/** The array of client notification requests outstanding */
	
	struct SEComNotification    
		{
			/** Notification status object */
			TInt iRequestStatusHandle; 
			/** Client identity */
			TClientRequest iMessage;     
		};

	RArray<SEComNotification> iNotificationRequests;

	/** A friend class for testing purposes */
	friend class TEComServer_StateAccessor;
	/** A friend class for testing purposes */
	friend class TEComServerSession_StateAccessor;
	};

/**
@fn				PanicClient(const RMessage& aMessage,TInt aPanicCode)
Intended Usage	: A global function to complete aMessage with the supplied panic code.
@since			7.0
@param			aPanicCode The error code with which to complete the message.
@pre 			None.
@post			aMessage has been completed.
@internalComponent
*/
extern void PanicClient(const TClientRequest& aMessage,TInt aPanicCode);

#include "EComServerSession.inl"

#endif	// __ECOMSERVERSESSION_H__


