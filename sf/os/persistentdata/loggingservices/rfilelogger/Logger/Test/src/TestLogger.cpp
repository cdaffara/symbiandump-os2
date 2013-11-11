// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Neil Taylor
// System Includes
// 
//

#include <e32base.h>
#include <e32cons.h>

#include <Test\RFileLogger.h>

/********************************************************************************
 * Externally available test functions
 * TInt CreateTextTest(void)
 * TInt CreateXMLTest(void)
 * TInt AppendXMLTest(void)
 ********************************************************************************/

static TInt WriteTextTest(RFileFlogger& logger);
static TInt LogLevelTest(RFileFlogger& logger);
static TInt LogAdditionalFieldsTest(RFileFlogger& logger);

TInt CreateTextTest(void)
{
	TInt	err =	KErrNone;

	RFileFlogger	logger;

	err =	logger.Connect( );
	err =	logger.CreateLog( _L("C:\\logs\\logger.txt"), RFileFlogger::ELogModeOverWrite );

	WriteTextTest( logger );
	LogLevelTest( logger );
	LogAdditionalFieldsTest( logger );

	logger.Close( );

	return ( err );
}

TInt CreateXMLTest(void)
{
	TInt	err =	KErrNone;

	RFileFlogger	logger;

	err =	logger.Connect( );
	err =	logger.CreateLog( _L("C:\\logger.xml"), RFileFlogger::ELogModeOverWrite );

	WriteTextTest( logger );
	LogLevelTest( logger );
	LogAdditionalFieldsTest( logger );

	logger.Close( );

	return ( err );
}

TInt AppendXMLTest(void)
{
	TInt	err =	KErrNone;

	RFileFlogger	logger;

	err =	logger.Connect( );
	err =	logger.CreateLog( _L("C:\\logger.xml"), RFileFlogger::ELogModeAppend );

	WriteTextTest( logger );

	logger.Close( );

	return ( err );
}

TInt WriteTextTest(RFileFlogger& logger)
{
	TInt	err =	KErrNone;

	logger.Log( ((TText8*)(__FILE__)), __LINE__, RFileFlogger::ESevrWarn, _L("This is a warning.") );

	return ( err );
}

TInt LogLevelTest(RFileFlogger& logger)
{
	TInt	err =	KErrNone;

	logger.SetLogLevel( RFileFlogger::ESevrAll );
	logger.Log( ((TText8*)(__FILE__)), __LINE__, RFileFlogger::ESevrInfo, _L("This is the first info log.") );
	logger.SetLogLevel( RFileFlogger::ESevrErr );
	logger.Log( ((TText8*)(__FILE__)), __LINE__, RFileFlogger::ESevrInfo, _L("This is the second info log.") );
	logger.SetLogLevel( RFileFlogger::ESevrAll );
	logger.Log( ((TText8*)(__FILE__)), __LINE__, RFileFlogger::ESevrInfo, _L("This is the third info log.") );

	return ( err );
}

TInt LogAdditionalFieldsTest(RFileFlogger& logger)
{
	TInt	err =	KErrNone;

	TExtraLogField	fields[2];

	fields[0].iLogFieldName =	_L("Animal");
	fields[0].iLogFieldValue =	_L("Rabbit");
	fields[1].iLogFieldName =	_L("City");
	fields[1].iLogFieldValue =	_L("London");

	logger.Log( ((TText8*)(__FILE__)), __LINE__, RFileFlogger::ESevrInfo, 2, fields, _L("This is %d fields."), 2 );

	return ( err );
}

