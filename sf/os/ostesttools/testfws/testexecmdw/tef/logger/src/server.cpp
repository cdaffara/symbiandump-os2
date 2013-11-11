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
* Main log server engine.
* Process log requests from multiple clients simultaneously.
*
*/



/**
 @file server.cpp
*/
#include "server.h"

const TInt KLowMemBufLength = 128;

CLogServer* CLogServer::NewL()
/**
 * @return - Instance of the log server
 */
	{
	CLogServer * server = new (ELeave) CLogServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	// CServer base class call
	server->StartL(KTestExecuteLogServerName);
	CleanupStack::Pop(server);
	return server;
	}

void CLogServer::ConstructL()
/**
 * Second phase construction
 */
	{
	User::LeaveIfError(Fs().Connect());
	}


CLogServer::CLogServer() : CServer2(EPriorityStandard,ESharableSessions)
/**
 * Constructor
 */
	{
	}

CLogServer::~CLogServer()
/**
 * Destructor
 */
	{
	// Close the array of control structures
	LogControl().Close();
	Fs().Close();
	}


CSession2* CLogServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
/**
 * @param RMessage - RMessage for the session open
 */
	{
	// Just create the session
	CLogSession* session = new (ELeave) CLogSession();
	return session;
	}

void CLogServer::ControlComplete(CLogFileControl& aControl)
/**
 * @param aControl - Logfile control class reference
 *
 * Checks to see if this control session can be removed 
 */
	{
	// Check session count and the data buffers on the queue 
	if(aControl.SessionCount() || !aControl.QueueEmpty())
		return;

	// There are no subsessions mapped to the logfile control class and
	// no data buffers on its write queue
	// Loop through the server's control array and remove it then delete it
	TInt i;
	for(i=0;i<LogControl().Count();i++)
		{
		if(&aControl == LogControl()[i])
			{
			// Done
			LogControl().Remove(i);
			delete &aControl;
			break;
			}
		}
	// If it's the last one then exit the server
	if(!LogControl().Count())
		CActiveScheduler::Stop();
	}


///////

CLogSession::CLogSession()
/**
 * Constructor
 */
	{
	}

CLogSession::~CLogSession()
/**
 * Destructor
 */
	{
	// Check for null
	// Session close without a createlog call leaves iControl null
	if(!iControl)
		return;
	// A logfile control structure can have multiple server sessions
	// decrement its server session count
	iControl->RemoveSession();
	CLogServer* p=(CLogServer*) Server();
	// Shuts Down the server if this is the last open session
	p->ControlComplete(*iControl);
	}

void CLogSession::ServiceL(const RMessage2& aMessage)
/**
 * @param aMessage - Function and data for the session
 */
	{
	switch(aMessage.Function())
		{
		// API CreateLog() call
		case RTestExecuteLogServ::ECreateLog :
			{
			// Sanity check to make sure it's not been called multiple times
			if(iControl)
				{
				aMessage.Complete(KErrInUse);
				break;
				}
			// Get the filepath
			// size policed on the client side
			TBuf<KMaxTestExecuteLogFilePath> logFilePath;
			// Read it
			aMessage.ReadL(0,logFilePath);
			// Get the log mode in the second argument
			RTestExecuteLogServ::TLogMode logMode;
			logMode = (RTestExecuteLogServ::TLogMode)aMessage.Int1();
			// Get a pointer to the parent server
			CLogServer* server=(CLogServer*) Server();
			// For compare's convert the whole path to lower case
			logFilePath.LowerCase();
			// Get rid of leading and trailing spaces.
			logFilePath.Trim();
			// Loop the through the server's logfile control class list
			// to see if there's a match.
			TInt i;
			for(i=0;i<server->LogControl().Count();i++)
				{
				if(server->LogControl()[i]->LogFile() == logFilePath)
					// This file's already in open so we don't have to open it
					break;
				}
			TInt err = KErrNone;
			// Check the count
			if(i < server->LogControl().Count())
				// Map this session to an existing logfile control class in the list
				iControl = server->LogControl()[i];
			else
				{
				// Create a new logfile control class
				// creates/opens the logfile
				TRAP(err,iControl = CLogFileControl::NewL(*server,logFilePath,logMode));
				if(!err)
					// Append it to the logfile control class list
					server->LogControl().Append(iControl);
				}
			if(!err)
				// Increment its session count
				iControl->AddSession();
			aMessage.Complete(err);
			}
			break;
		// One of the API write calls
		case RTestExecuteLogServ::EWriteLog :
			{
			// Sanity check
			if(!iControl)
				{
				aMessage.Complete(KErrNotFound);
				break;
				}
			// Data can be any size
			// Get the length from second argument
			TInt bufferLength = aMessage.Int1();
			// Get a heap buffer of the right size
			HBufC8* buffer = NULL;
			TRAPD(err, buffer = HBufC8::NewL(bufferLength));
			if (KErrNoMemory == err)
				{
				// If the buffer length is bigger than 128 and allocation failed, try to allocate
				// 128 bytes and set it the low memory information.
				if (bufferLength > KLowMemBufLength)  
					{
					HBufC8* buf = HBufC8::NewL(KLowMemBufLength);
					CleanupStack::PushL(buf);
					TPtr8 ptr = buf->Des();
					ptr.Copy(KLoggerNotEnoughMemory8);
					CLogBuffer* logBuffer = new (ELeave) CLogBuffer(*buf);
					CleanupStack::Pop(buf);
					iControl->AddLogBuffer(*logBuffer);
					}
				}
			else
				{
				CleanupStack::PushL(buffer);
				TPtr8 ptr(buffer->Des());
				// read the data
				aMessage.ReadL(0,ptr);			
				// Get a buffer control class contructed with the heap data
				// takes ownership
				CLogBuffer* logBuffer = new (ELeave) CLogBuffer(*buffer);
				CleanupStack::Pop(buffer);
				// Add it to the logfile control class buffer queue
				iControl->AddLogBuffer(*logBuffer);
				}
			if(!iControl->IsActive())
				// AO is idle, kick into life
				iControl->Kick();
			aMessage.Complete(KErrNone);
			}
			break;

		default:
			break;
		}
	}

///////

CLogFileControl* CLogFileControl::NewL(CLogServer& aParent, const TDesC& aLogFilePath,RTestExecuteLogServ::TLogMode aMode)
/**
 * @param aParent - Server reference for callback
 * @param aLogFilePath - Full path and filename of the logfile
 * @param aMode - Overwrite or Append
 *
 * First phase construction for logfile control class
 */
	{
	CLogFileControl* self = new (ELeave) CLogFileControl(aParent,aLogFilePath);
	self->ConstructL(aMode);
	return self;
	}

CLogFileControl::CLogFileControl(CLogServer& aParent,const TDesC& aLogFilePath) :
	iParent(aParent),
	iLogFileName(aLogFilePath),
	iTransmitted(EFalse)
/**
 * @param aParent - Server reference for callback
 * @param aLogFilePath - Full path and filename of the logfile
 *
 * Constructor - Safe initialisation
 */
	{
	iQueue.SetOffset(CLogBuffer::LinkOffset());
	}

void CLogFileControl::ConstructL(RTestExecuteLogServ::TLogMode aMode)
/**
 * @param aMode - Overwrite or Append
 *
 * Second phase construction - Create or open the logfile
 */
	{
	if(aMode == RTestExecuteLogServ::ELogModeOverWrite)
		// In overwrite mode replace
		User::LeaveIfError(iLogFile.Replace(iParent.Fs(),iLogFileName,EFileWrite|EFileShareAny));
	else
		{
		// For append try open then replace
		TInt err = iLogFile.Open(iParent.Fs(),iLogFileName,EFileWrite|EFileShareAny);
		if(err != KErrNone)
			// Bomb out if replace fails
			User::LeaveIfError(iLogFile.Replace(iParent.Fs(),iLogFileName,EFileWrite|EFileShareAny));
		else
			{
			// Open worked. Position at EOF
			TInt pos;
			User::LeaveIfError(iLogFile.Seek(ESeekEnd,pos));
			}
		}
	}

CLogFileControl::~CLogFileControl()
/**
 * Destructor
 * The server maintains a list of these classes and will not destruct one if there
 * is data on its queue
 * Destructor just closes the file handle.
 */
	{
	iLogFile.Close();
	}

void CLogFileControl::RunL()
/**
 * Main File writing pump
 * Called on write completion or Kick() by the session that accepts the data
 */
	{
#if (defined _DEBUG)
	_LIT(KPanic,"LogEng RunL()");
#endif
	__ASSERT_DEBUG(iStatus.Int() == KErrNone,User::Panic(KPanic,iStatus.Int()));
	// Check to see if this is the result of write completion
	if(iTransmitted)
		{
		// Write completed
		// Remove the buffer at the head of the queue and free it
		CLogBuffer* buffer = iQueue.First();
		iQueue.Remove(*buffer);
		delete buffer;
		}
	// Check to see if there's more on the queue
	if(!iQueue.IsEmpty())
		{
		// There is so write the head of the queue
		CLogBuffer* buffer = iQueue.First();	
		SetActive();
		// Set the flag to say we've transmitted
		iTransmitted = ETrue;
		iLogFile.Write(buffer->Buf(),iStatus);
		iLogFile.Flush();
		}
	else
		{
		// Nothing on the queue
		iTransmitted = EFalse;
		// Call into the server to check if this resource can be freed
		iParent.ControlComplete(*this);
		}
	}

///////
CLogBuffer::CLogBuffer(HBufC8& aLogBuffer) : iLogBuffer(aLogBuffer)
/**
 * @param aLogBuffer - Heap descriptor. This class takes ownership
 * Constructor
 */
	{
	}

CLogBuffer::~CLogBuffer()
/**
 * Destructor
 * This class owns a heap buffer so just free it
 */
	{
	delete &iLogBuffer;
	}
