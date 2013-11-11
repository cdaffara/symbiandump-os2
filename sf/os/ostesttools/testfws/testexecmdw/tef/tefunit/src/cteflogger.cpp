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
 @file CTEFLogger.cpp
*/

#include "cteflogger.h"
#include "tefunit.h"

void CTEFLogger::LogResult( const TDesC& aTestName, TInt aResult, CTestExecuteLogger& aLogger )
/**
 * LogResult
 *
 * @param aTestName - Name of the test case
 * @param aResult - Result of the test
 * @param aLogger - The logger
 */
	{
	// Log the test case result in the correct colour
	HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
	TPtr ptr(resultBuffer->Des());

	TFullName serverName;
	TFindServer htmServerFinder( KTestExecuteLogServerName );
	TInt errCode = htmServerFinder.Next( serverName );
	
	if ( errCode != KErrNotFound )
		{
		// Set the result text colour
		switch(aResult)
			{
			case KErrTEFUnitPass:
				{
				ptr.Append(KTEFFontGreen);
				}
				break;
			case KErrTEFUnitFail:
				{
				ptr.Append(KTEFFontRed);
				}
				break;
			default:
				{
				ptr.Append(KTEFFontBlue);
				}
				break;
			}

		// Append the test case name
		ptr.AppendFormat( _L("Test Case") );
		ptr.Append(KTEFSpace);
		ptr.Append( aTestName );
		ptr.Append(KTEFSpace);
		ptr.AppendFormat( KTEFResultTag );
		ptr.Append(KTEFSpace);
		ptr.Append( KTEFEquals );
		ptr.Append(KTEFSpace);

		// Append the test result
		switch(aResult)
			{
			case KErrTEFUnitPass:
				{
				ptr.Append(KTEFResultPass);
				}
				break;
			case KErrTEFUnitFail:
				{
				ptr.Append(KTEFResultFail);
				}
				break;
			case KErrTEFUnitInconclusive:
				{
				ptr.Append(KTEFResultInconclusive);
				}
				break;
			case KErrTEFUnitAbort:
				{
				ptr.Append(KTEFResultAbort);
				}
				break;
			default:
				{
				ptr.Append(KTEFResultUnknown);
				}
				break;
			}

		ptr.Append(KTEFSpace);
		ptr.Append(KTEFFontEnd);

		HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
		TPtr8 ptr8(resultBuffer8->Des());
		ptr8.Copy(ptr);

		// Write to the log file
		aLogger.HtmlLogger().Write(ptr8);

		CleanupStack::PopAndDestroy(resultBuffer8);
	}

	TFindServer xmlServerFinder( KFileLogrerServerName );
	errCode = xmlServerFinder.Next( serverName );
	
	if ( errCode != KErrNotFound )
		{
		// Append the test case name
		ptr.AppendFormat( _L("Test Case") );
		ptr.Append(KTEFSpace);
		ptr.Append( aTestName );
		ptr.Append(KTEFSpace);
		ptr.AppendFormat( KTEFResultTag );
		ptr.Append(KTEFSpace);
		ptr.Append( KTEFEquals );
		ptr.Append(KTEFSpace);

		// Append the test result
		switch(aResult)
			{
			case KErrTEFUnitPass:
				{
				ptr.Append(KTEFResultPass);
				}
				break;
			case KErrTEFUnitFail:
				{
				ptr.Append(KTEFResultFail);
				}
				break;
			case KErrTEFUnitInconclusive:
				{
				ptr.Append(KTEFResultInconclusive);
				}
				break;
			case KErrTEFUnitAbort:
				{
				ptr.Append(KTEFResultAbort);
				}
				break;
			default:
				{
				ptr.Append(KTEFResultUnknown);
				}
				break;
			}
		aLogger.XmlLogger().Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrTEFUnit, ptr);
		}

	CleanupStack::PopAndDestroy(resultBuffer);
	}


void CTEFLogger::LogTraverse( const TDesC& aSuiteName, CTestExecuteLogger& aLogger )
/**
 * LogTraverse
 *
 * @param aSuiteName - Name of the test suite
 * @param aLogger - The logger
 */
	{
	// Log the test case result in the correct colour
	HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
	TPtr ptr(resultBuffer->Des());

	TFullName serverName;
	TFindServer htmServerFinder( KTestExecuteLogServerName );
	TInt errCode = htmServerFinder.Next( serverName );
	
	if ( errCode != KErrNotFound )
		{
		// Always blue for suite traversal
		ptr.Append(KTEFFontLightBlue);

		// Append the test case name
		ptr.AppendFormat( _L("Entering Test Suite:") );
		ptr.Append(KTEFSpace);
		ptr.Append( aSuiteName );
		ptr.Append(KTEFSpace);
		ptr.Append(KTEFFontEnd);
	
		HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
		TPtr8 ptr8(resultBuffer8->Des());
		ptr8.Copy(ptr);

		// Write to the log file
		aLogger.HtmlLogger().Write(ptr8);

		CleanupStack::PopAndDestroy(resultBuffer8);
		}

	TFindServer xmlServerFinder( KFileLogrerServerName );
	errCode = xmlServerFinder.Next( serverName );
	
	if ( errCode != KErrNotFound )
		{
		// Append the test case name
		ptr.AppendFormat( _L("Entering Test Suite:") );
		ptr.Append(KTEFSpace);
		ptr.Append( aSuiteName );
		ptr.Append(KTEFSpace);

		aLogger.XmlLogger().Log(((TText8*)__FILE__), __LINE__, RFileFlogger::ESevrTEFUnit, ptr);
		}
	CleanupStack::PopAndDestroy(resultBuffer);
	}
