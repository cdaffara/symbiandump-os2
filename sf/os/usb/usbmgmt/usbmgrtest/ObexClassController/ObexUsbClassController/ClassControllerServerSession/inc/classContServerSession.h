/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef CLASSCONTSERVERSESSION_H
#define CLASSCONTSERVERSESSION_H

#include <e32base.h>
#include <obex.h>
#include <obexserver.h>
#include "obexInitiator.h"

class CClassContServer;



/**
 * CClassContServerSession is a CSession2 derived class.
 * This class is implemented on the server side of a Client
 * and Server configuration, it has member functions that exist
 * to process requests from the client. 
 *
 */
NONSHARABLE_CLASS(CClassContServerSession) : public CSession2
	{
	public:
		static CClassContServerSession* NewL(CClassContServer& aServer);
		~CClassContServerSession();

	private:
		CClassContServerSession(CClassContServer& aServer);
		void ConstructL();
		
		// from CSession2
		void ServiceL(const RMessage2& aMessage);
	
		void ForwardMessageL(const RMessage2& aMessage);
		void StartServiceL();
		void StopService();
		
		CClassContServer& iServer;
		CObexInitiator* iObexInitiator;
		TBool iBadMessage;
	};


#endif //CLASSCONTSERVERSESSION_H



	
