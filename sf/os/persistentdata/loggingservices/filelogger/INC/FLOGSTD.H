/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __FLOGSTD_H__
#define __FLOGSTD_H__

#include "FLOGSVR.H"

/**
@internalComponent
*/
const TInt KFLogSrvMajorVersionNumber=1;

/**
Log Server Minor Version Number.
*/
const TInt KFLogSrvMinorVersionNumber=0;

/**
Log Server Build Version Number.
*/
const TInt KFLogSrvBuildVersionNumber=20;

/**
Shut down pause time 5secs.
@internalComponent
*/
const TInt KShutdownPause=5000000;

/**
FullStop character.
@internalComponent
*/
const TText KFullStopChar='.';

/**
File logger server name in EKA2.
@internalComponent
*/
_LIT(KFLoggerServerName,"!FLogger server");


/**
@internalComponent
*/
enum TFileLoggerOperations 
	{
	/** const to create a new log file. */
	ECreateLog,	

	/** const to write the log file. */
	EWriteLog,	

	/** const to close the log file. */
	ECloseLog,	

	/** const for create,write and to close the log file. */
	ECreateWriteAndCloseLog	

	};

NONSHARABLE_CLASS(CFLoggerScheduler) : public CActiveScheduler
/**
File Logger Scheduler
@internalComponent
*/
	{
public:
	static void NewL();
	~CFLoggerScheduler();
private:
	inline CFLoggerScheduler() {}

	/** 
	from CActiveScheduler 
	*/
	void Error(TInt aError) const;

private:
	CFileLoggerServer* iServer;
	};

#endif // __FLOGSTD_H__

