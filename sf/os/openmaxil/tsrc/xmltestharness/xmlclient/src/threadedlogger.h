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
* Description:
*
*/

#ifndef THREADEDLOGGER_H_
#define THREADEDLOGGER_H_

#include "omx_xml_script.h"

/**
 * Adapter class that serializes all Log() calls on the underlying logger into the same thread.
 * That is, a call to Log() from a thread that did not create this AO will block until the AO has
 * completed the log in the AS thread.
 * 
 * This class is provided to allow the use of loggers that would otherwise KERN-EXEC 0 panic if
 * used by multiple threads.
 */
class CThreadedLogger : public CActive, public MOmxScriptTestLogger
	{
public:
	static CThreadedLogger* NewLC(MOmxScriptTestLogger& aRealLogger);
	~CThreadedLogger();
	
	void Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity, const TDes& aMessage);
	
private:
	CThreadedLogger(MOmxScriptTestLogger& aRealLogger);
	void ConstructL();
	
	void RunL();
	void DoCancel();
	
private:
	MOmxScriptTestLogger& iRealLogger;
	RThread iCreatorThread;
	RMutex iMutex;
	RSemaphore iSemaphore;
	TThreadId iCreatorThreadId;
	
	TText8* iFile;
	TInt iLine;
	TOmxScriptSeverity iSeverity;
	TDes* iMessage;
	};

#endif /* THREADEDLOGGER_H_ */
