// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



/**
 @file te_logger.h
 @internalTechnology
*/

#ifndef TE_LOGGER_H_
#define TE_LOGGER_H_
#include "te_tracerecord.h"
#include <uloggerclient.h>
//#include <d32btrace.h>

using namespace Ulogger;


	
class TLogger
{
public:
	void SetLogFilename(const TDesC& aLogFilename);
	TInt StartLoggingL(const TTraceConfigs aTraceData);
	TInt StopLogging();
	TInt GetDataL(TUint8*& aData, TUint& aSize);
	TInt DeleteLogfileL();
private:
	//TInt DumpBTracesToFile();
private:
	RULogger iLogger;
	//RBTrace iLogger;
	TBuf<KMaxFileName> iLogFilename;
};
#endif /*TE_LOGGER_H_*/
