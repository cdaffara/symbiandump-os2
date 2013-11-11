// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Main log server engine.
// Process log requests from multiple clients simultaneously.
// 
//

/**
 @file server.cpp
*/
#include "server.h"

CLogServer* CLogServer::NewL()
/**
 * @return - Instance of the log server
 */
	{
	CLogServer * server = new (ELeave) CLogServer();
	CleanupStack::PushL(server);
	server->ConstructL();
	// CServer base class call
	server->StartL(KFileLogrerServerName);
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
		case RFileFlogger::ECreateLog :
			{
			// Sanity check to make sure it's not been called multiple times
			if(iControl)
				{
				aMessage.Complete(KErrInUse);
				break;
				}
			// Get the filepath
			// size policed on the client side
			TBuf<KMaxLoggerFilePath> logFilePath;
			// Read it
			aMessage.ReadL(0,logFilePath);
			// Get the log mode in the second argument
			RFileFlogger::TLogMode logMode;
			logMode = (RFileFlogger::TLogMode)aMessage.Int1();
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
					{  // nancy
					// find out the type of output format and assign to this new created control
					TInt error = logFilePath.Find(_L(".xml"));
					if(error==KErrNotFound) iControl->SetLogType(CLogFileControl::ETxt);
					else iControl->SetLogType(CLogFileControl::EXml);					
					// end nancy
					// Append it to the logfile control class list
					server->LogControl().Append(iControl);					
					}
				}
			if(!err)
				// Increment its session count
				iControl->AddSession();
			aMessage.Complete(err);
			}
			break;
		// One of the API write calls
		case RFileFlogger::EWriteLog :
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
			HBufC8* buffer = HBufC8::NewLC(bufferLength);
			TPtr8 ptr(buffer->Des());
			// read the data
			aMessage.ReadL(0,ptr);			
			// Get a buffer control class contructed with the heap data
			// takes ownership
			CLogBuffer* logBuffer = new (ELeave) CLogBuffer(*buffer);
			CleanupStack::Pop(buffer);
			// Add it to the logfile control class buffer queue
			iControl->AddLogBuffer(*logBuffer);
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

CLogFileControl* CLogFileControl::NewL(CLogServer& aParent, const TDesC& aLogFilePath,RFileFlogger::TLogMode aMode)
/**
 * @param aParent - Server reference for callback
 * @param aLogFilePath - Full path and filename of the logfile
 * @param aMode - Overwrite or Append
 *
 * First phase construction for logfile control class
 */
	{
	CLogFileControl* self = new (ELeave) CLogFileControl(aParent,aLogFilePath);
	CleanupStack::PushL(self);
	self->ConstructL(aMode);
	CleanupStack::Pop();
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

void CLogFileControl::ConstructL(RFileFlogger::TLogMode aMode)
/**
 * @param aMode - Overwrite or Append
 *
 * Second phase construction - Create or open the logfile
 */
	{
	if(aMode == RFileFlogger::ELogModeOverWrite)
		// In overwrite mode replace
		User::LeaveIfError(iLogFile.Replace(iParent.Fs(),iLogFileName,EFileWrite));
	else
		{
		// For append try open then replace
		TInt err = iLogFile.Open(iParent.Fs(),iLogFileName,EFileWrite);
		if(err != KErrNone)
			// Bomb out if replace fails
			User::LeaveIfError(iLogFile.Replace(iParent.Fs(),iLogFileName,EFileWrite));
		else
			{
			// Open worked. Position at EOF
			TInt pos;
			iLogFile.Size(pos);
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

//  ------------------------------------
		if(iLogFormat==ETxt)  WriteTxt(buffer->Buf());
		else WriteXml(buffer->Buf());
		} 
	else
		{
		// Nothing on the queue
		iTransmitted = EFalse;
		// Call into the server to check if this resource can be freed
		iParent.ControlComplete(*this);
		}
	}

void CLogFileControl::WriteXml(const TDesC8 &aDes)
/**
 * @param aDes - send a aDes string in xml format to a log file
 */
	{
/*--------- Maintaince Warning:  -----------------------------------
******* the fomat of below is sensible from client original format.  
******* Any change should match actual string formated from client. 
******* Double check the code on client side 

* The current assumtion of format:
* First string values are seperated by sign " - " and extra pair of fields are
* seperated from main log message  by long unusual string "LogFieldsRequiredBeingAddedToAboveLogMessage"
* The \t used to seperate field name and field value and \r\n used to seperate
* each other from those pairs of field
* \t\t\t\t\t\t is used to end of whole string
--------------------------------------------------------------------------------*/
		_LIT8(KxmlHeader,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<LOGFILE>");
		//The order of variables:
		// time 		- aTime
		// Severity 	- aSeverity
		// Thread 		- aThread
		// Filename 	- aFilename
		// Linenumber 	- aLinenumber
		// Text 		- aText
		
		// Start of string retrive/deformating 
		HBufC8* pBuf1 = HBufC8::New(KMaxLoggerLineLength*2); //(aDes.Length()+200);
		if(!pBuf1)
			{
			return; // no memory 
			}
		TPtr8 aPtr(pBuf1->Des());  // used for searching
		TPtr8 alogbuf(pBuf1->Des());  //used for final log
		aPtr.Append(aDes);
		TPtrC8 SearchBuf;
		TInt aCount[8]; 
		aCount[0]=0;
		TInt posI(0);

        // retrive log message:
		// Retrive common part of log message:
		TInt i=0;
		for(i=1; i<6; i++)
			{
			SearchBuf.Set(aPtr.Mid(posI));
			aCount[i]=SearchBuf.Find(KSeperation8)+posI;
			posI=aCount[i]+3;
			if(aCount[i]<aCount[i-1])	
                {
                delete pBuf1;
                return; // wrong format string from client
                }
			}
		// seperating common log message and extra log fields will be easy for future maintaince.
		TLogField8* alogField = new TLogField8[6];  // the common part of log message for both 
								  					// with and without extra log fields
		if(!alogField) 
            {
            delete pBuf1;
            return; // no memory
            }

		TLogField8* extralogField=NULL; // only applied to extra log fields

		TInt alength=0;  // a length of array of extra log fields

		alogField[0].iLogTag8.Copy(_L8("TIME"));
		alogField[1].iLogTag8.Copy(_L8("SEVERITY"));
		alogField[2].iLogTag8.Copy(_L8("THREAD"));
		alogField[3].iLogTag8.Copy(_L8("FILENAME"));
		alogField[4].iLogTag8.Copy(_L8("LINENUMBER"));
		alogField[5].iLogTag8.Copy(_L8("TEXT"));
			
		alogField[0].iLogValue8.Copy(aPtr.Mid(aCount[0],aCount[1]-aCount[0]));
		for(i=1; i<5; i++)
			{
			alogField[i].iLogValue8.Copy(aPtr.Mid(aCount[i]+3,aCount[i+1]-aCount[i]-3));				
			}

		SearchBuf.Set(aPtr.Mid(posI));
		aCount[6]=SearchBuf.Find(_L8("LogFieldsRequiredBeingAddedToAboveLogMessage"))+posI; 
		if(aCount[6]<posI)  // no addtional fields. Find return value is KErrNotFound or >0
			{
			alogField[5].iLogValue8.Copy(aPtr.Mid(aCount[5]+3,aDes.Length()-aCount[5]-5));
			}
		else
            {
			alogField[5].iLogValue8.Copy(aPtr.Mid(aCount[5]+3,aCount[6]-aCount[5]-5));
			posI=aCount[6]+45;  //45 is from the length of long string and a tab
			SearchBuf.Set(aPtr.Mid(posI));
			aCount[7]=SearchBuf.Find(_L8("\r\n"))+posI;
			TLex8 lex(aPtr.Mid(posI,aCount[7]-posI));  // get the length
			TInt err=lex.Val(alength); 
			if (err)
                alength=0; // ignor the extra log fields. Let the log go

			// Retrive the extra log fields
			extralogField = new TLogField8[alength];
			if(!extralogField)
                {
                delete pBuf1; 
                return; // no memory
                }
			for(TInt i=0; i<alength; i++)
				{
				aCount[6]=aCount[7]+2;
				SearchBuf.Set(aPtr.Mid(aCount[6]));
				aCount[7]=SearchBuf.Find(_L8("\t"))+aCount[6];
				extralogField[i].iLogTag8.Copy(aPtr.Mid(aCount[6],aCount[7]-aCount[6]));
				aCount[6]=aCount[7]+1;
				SearchBuf.Set(aPtr.Mid(aCount[6]));
				aCount[7]=SearchBuf.Find(_L8("\r\n"))+aCount[6];
				extralogField[i].iLogValue8.Copy(aPtr.Mid(aCount[6],aCount[7]-aCount[6]));
				}
            }
		// Start to organize an XML format:
		TInt afileSize;
		_LIT(KLogMutex, "LoggingServerMutex");
		RMutex mutex;
		TInt r = mutex.CreateGlobal(KLogMutex);
		if(r==KErrAlreadyExists)
			r = mutex.OpenGlobal(KLogMutex);  
		
		if(!r)
            mutex.Wait(); // If still failed, let logging go without bother the mutex.
		iLogFile.Size(afileSize);
		if(afileSize<12) // 12 is from charters of "\r\n</LOGFILE>" 
					//It shoud happened once at the beginning of the file
					// such as overwrite mode
            {
			afileSize=12; // used for lock position
			alogbuf.Copy(KxmlHeader);
			}
		alogbuf.Append(_L8("\r\n<MESSAGE>\r\n"));
		for(TInt i=0; i<6; i++)
			{
			alogbuf.Append(_L8("  <"));
			alogbuf.Append(alogField[i].iLogTag8);
			alogbuf.Append(_L8(">"));
			alogbuf.Append(alogField[i].iLogValue8);
			alogbuf.Append(_L8("</"));
			alogbuf.Append(alogField[i].iLogTag8);
			alogbuf.Append(_L8(">\r\n"));				
			}
		for(TInt i=0; i<alength; i++)  
			{
			alogbuf.Append(_L8("  <"));
			alogbuf.Append(extralogField[i].iLogTag8);
			alogbuf.Append(_L8(">"));
			alogbuf.Append(extralogField[i].iLogValue8);
			alogbuf.Append(_L8("</"));
			alogbuf.Append(extralogField[i].iLogTag8);
			alogbuf.Append(_L8(">\r\n"));				
			}
		
		alogbuf.Append(_L8("</MESSAGE>"));
		alogbuf.Append(_L8("\r\n</LOGFILE>"));
		
		iLogFile.Write(afileSize-12, alogbuf,iStatus);
													
		if(!r)	
			{
			mutex.Signal();
			mutex.Close();				
			} 

		if(extralogField)
            delete[] extralogField;

		delete[] alogField;	
		delete pBuf1;
	}

void CLogFileControl::WriteTxt(const TDesC8 &aDes)
/**
 * @param aDes - send a aDes string in xml format to a log file
 */
	{
/*--------- Maintaince Warning for aLogBuffer -----------------------------------
******* the fomat of below is sensible from client original format.  
******* Any change should match actual string formated from client. 
******* Double check the code on client side 

* The current assumtion of format:
* First string values are seperated by sign " - " and extra pair of fields are
* seperated from main log message  by long unusual string "LogFieldsRequiredBeingAddedToAboveLogMessage"
* The \t used to seperate field name and field value and \r\n used to seperate
* each other from those pairs of field
--------------------------------------------------------------------------------*/
	iLogFile.Write(aDes,iStatus);
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
