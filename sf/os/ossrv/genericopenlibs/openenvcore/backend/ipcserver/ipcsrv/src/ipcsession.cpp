// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ipcsesion.cpp
// Part of     : ipc server
// To implement the ipc session
// Class definition for Server Side Session
// 
//

   
#include "ipcsession.h"	//CIpcSession
#include "semaphorequeue.h"


//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::NewL()
// Description   : 	Two phase construction. NewL is static function. 
//					CallsConstructL(). Returns the constructed Session object.	  				 
//-------------------------------------------------------------------------------	
 
CSession2* CIpcSession::NewL()
	{
	CIpcSession* self = new (ELeave) CIpcSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::ConstructL()
// Description   : 	Read the .ini file.Uses the helper function ReadIniL function
//					to create the Stream class as mentioned in the .ini file.
//					If .ini file is not found or if there is any other error in 
//					reading the .ini file, then by console stream is created.			 				 
//-------------------------------------------------------------------------------	
 
void CIpcSession::ConstructL()
	{
	}
//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::~CIpcSession()
// Description   : 	Destructor. Deletes the Factory class as created and the 
//					iIniData created in ConstructL function.Also calls the 
//					RemoveSession() to reduce the count of session.	 				 
//-------------------------------------------------------------------------------	

CIpcSession::~CIpcSession()
	{
	static_cast<CSemQueueHandler*>(Server().GetSemHandler())->AdjustSemVal(this);
	Server().RemoveFileSockPathBySession(this);
	Server().RemoveSession();
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::CreateL()
// Description   : 	Increaments the session count. Calls the AddSession method to
//					increament the session count.
//					 				 
//-------------------------------------------------------------------------------	

void CIpcSession::CreateL()
	{
	Server().AddSession();		
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::ServiceL()
// Description   : 	For any request from the client process, the this method
//					checks the request and issues the corresponding request.
//					the iStreamRead and iStreamWrite depends on the media type
//					defined as in the .ini file. 			 				 
//-------------------------------------------------------------------------------	
		
void CIpcSession::ServiceL(const RMessage2& aMessage)
	{
	Server().ServiceL(aMessage);
	return;
	}
	

//-------------------------------------------------------------------------------
// Function Name : 	CIpcSession::Server()
// Description   : 	Returns the handle to the server
//					 				 
//-------------------------------------------------------------------------------	

CIpcServer& CIpcSession::Server()
    {
    return *(static_cast<CIpcServer*>(const_cast<CServer2*>(CSession2::Server())));
    }


