// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : readrequesthandler.cpp
// Part of     : stdio server
// To handle request for read
//



#include "ReadRequestHandler.h"
#include "streambase.h"
#include "StdioServer.h"

//-------------------------------------------------------------------------------
// Function Name : 	TReadRequest::TReadReques()
// Description   : 	Constructor.
//-------------------------------------------------------------------------------	

 
TReadRequest::TReadRequest(const RMessage2& aMessage2):iMessage2(aMessage2)
	{
	
	}

//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::NewL()
// Description   : 	Static Function returns the created CReadRequestHandler 
//					object. This object is creates in StdioSession::ConstructL(),
//					only if the read media in console.
//-------------------------------------------------------------------------------	

 
CReadRequestHandler* CReadRequestHandler::NewL(CStreamBase* aReadStream)
 	{
 	CReadRequestHandler* self = new(ELeave) CReadRequestHandler(aReadStream);
 	return self;
 	}
 	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::CReadRequestHandler()
// Description   : 	Constructor.CReadRequestHandler is an active object to handle 
//					the read request from the client process on console. It adds
//					itself to the active scheduler.
//-------------------------------------------------------------------------------	

	
CReadRequestHandler::CReadRequestHandler(CStreamBase* aReadStream):
		CActive(EPriorityStandard), iQueue(_FOFF(TReadRequest, iLink)), iReadStream(aReadStream),
		iPtr(NULL,0)
 	{
 	CActiveScheduler::Add(this);
 	}
 	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::~CReadRequestHandler()
// Description   :  Destructor. Removes itselfs from the active scheduler. Also
//					deletes the RReadRequest object to avoid memory leak.
//-------------------------------------------------------------------------------	
 
	
CReadRequestHandler::~CReadRequestHandler()
	{
	if ( IsActive() )
		{
		Cancel();
		}
	//empty the queue	
	TReadRequest* pReq = NULL;
	while( !iQueue.IsEmpty() )
		{
		pReq = iQueue.First();
		iQueue.Remove(*pReq);
		delete pReq;
		}	
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::RunL()
// Description   : 	RunL completes the read request by reading the keyed character
//					from the console and returns to the client process. 
//					Checks if there are more pending Read Request in the link list
//					then again issues the read request on the console and sets
//					active.
//-------------------------------------------------------------------------------	

void CReadRequestHandler::RunL()
	{

	TDes8& tempBuf = iReadStream->Buf();
	iPtr.Set( (unsigned char*)tempBuf.Ptr(), tempBuf.Length(),  tempBuf.MaxLength() );

	while ( !iQueue.IsEmpty() && (iPtr.Length() || iStatus.Int() == KErrEof)) 
		{
		TReadRequest* pReq = iQueue.First();
		iQueue.Remove(*pReq);
		Complete(pReq->iMessage2);
		delete pReq;
		}
	
	if ( !iQueue.IsEmpty() )
		{
		iReadStream->Read(iStatus, iQueue.First()->iMessage2.Int1());
		SetActive();	
		}
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::ServiceL()
// Description   : 	For any new read request from the client process,RReadRequest
//					object is created and added to the link list. Checks if the
//					CReadRequestHandler is already active. If not then issues the
//					read request on the console and set itself active.
//-------------------------------------------------------------------------------	

void CReadRequestHandler::ServiceL(const RMessage2& aMessage2)
	{
	TReadRequest* pReq = new(ELeave) TReadRequest(aMessage2);
	iQueue.AddLast(*pReq);

	if ( !IsActive() )
		{
		//check is buffer has data or not
		if ( iPtr.Length() )
			{
			iQueue.Remove(*pReq);
			Complete(pReq->iMessage2);
			delete pReq;
			}
		else 
			{
			iReadStream->Read(iStatus, pReq->iMessage2.Int1());
			SetActive();	
			}
		}
	}


//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::NotifyActivityL()
// Description   : 	registers a notification request. 
//					
//-------------------------------------------------------------------------------	
void CReadRequestHandler::NotifyActivityL(const RMessage2& aMessage2)
	{
	//if this is notification request i.e length is less then zero.
	if ( iNotifyHandle )
		{
		User::Leave(KErrAlreadyExists);
		}
	iNotifyHandle = aMessage2.Handle();
	ServiceL(aMessage2);
	}


//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::CancelNotify()
// Description   : 	Cancel notification request.
//					
//-------------------------------------------------------------------------------	
void CReadRequestHandler::CancelNotify()
	{
	TSglQueIter<TReadRequest> iter(iQueue);
	TReadRequest* pReq = iter++;
	//find and delete pending request.
	while (pReq)
		{
		if ( iNotifyHandle ==  pReq->iMessage2.Handle())
			{
			iQueue.Remove(*pReq);
			pReq->iMessage2.Complete(KErrCancel);
			delete pReq;
			break;
			}
		pReq = iter++;
		}
	iNotifyHandle = 0;
	}
	
	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::DoCancel()
// Description   : 	
//					
//-------------------------------------------------------------------------------	
void CReadRequestHandler::DoCancel()
	{
	iReadStream->ReadCancel();
	}
	
//-------------------------------------------------------------------------------
// Function Name : 	CReadRequestHandler::Complete()
// Description   : 	writes input data to client buf and completes masg.
//					
//-------------------------------------------------------------------------------	

void CReadRequestHandler::Complete(const RMessage2 &aMessage)
    {
    TInt ret = KErrNone;
   
	 // retrieve the max length of the buffer the client has allocated
	 TInt length = aMessage.Int1();
	    
	 //check if this requeste is not a notification request   
	 if ( aMessage.Function() != ENotifyActivity )   
	 	{
	 		
	    TPtr8 tPtr(iPtr);
	    // create a local buffer for placing characters to be written to
	    // client. This is a copy of the the iBuf internal buffer of the stream
	    if ( length < iPtr.Length() )
	        {
	        // then we can't copy all the buffer to the client so we have reduce
	        // the buffers length
	        tPtr.SetLength(length);
	        // delete appropriate part of stream's internal buffer
	        iPtr.Delete(0,length);
	        }
	    else
	        {
	        // we can copy all of the buffer to the client so we set the streams
	        // descriptor to zero length
	        iPtr.Zero();
	        }
	     // copy the local descriptor to the client.
		 TRAP(ret,aMessage.WriteL(0, tPtr));
		 
	     if ( iStatus == KErrEof && ret == KErrNone ) 
	    	{
	    	ret = KErrEof;    	
	    	}
	    // inform the client of success/failure by completing the message
    	aMessage.Complete(ret);	
	 	}
	 else
	 	{
	 	if ( iNotifyHandle == aMessage.Handle() )
	 		{
	 		iNotifyHandle = 0;
	 		// inform the client of success/failure by completing the message
    		if ( KErrNone == ret )
    			{
    			aMessage.Complete(EReadyForReading);
    			}
    		else
    			{
    			aMessage.Complete(ret);
    			}
	 		}
	  	}
 
    
    if ( ( !iQueue.IsEmpty() ) && iPtr.Length() )
		{
		TReadRequest* pReq = iQueue.First();
		iQueue.Remove(*pReq);
		Complete(pReq->iMessage2);
		delete pReq;
		}
		
    return;
    }	
