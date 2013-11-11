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
 @file CTEFClientUT.cpp
*/

#include "ctefclientsuiteut.h"

void CTEFClientSuiteUT::SetupL()
/**
 * SetupL
 */
	{
	CTestFixture::SetupL();
	}
void CTEFClientSuiteUT::ServerConnect() 
	{
	TInt err = iServer.Connect(_L("te_RegStepTestSuite"));
	INFO_PRINTF2( _L("Server(te_RegStepTestSuite) Connect returns = %d"), err );
	}

void CTEFClientSuiteUT::TearDownL()
/**
 * TearDownL
 */
	{
	CTestFixture::TearDownL();
	iServer.Close();
	INFO_PRINTF1( _L("Server Close returns"));
	}

TInt CTEFClientSuiteUT::OpenSession(RTestSession &aSession, const TDesC& aName)
	{
	TInt err; 
	ServerConnect();
	err = aSession.Open(iServer,aName);
	INFO_PRINTF3( _L("%S Session Open returns = %d"),&aName, err );
	return err; 
	}
void CTEFClientSuiteUT::TestOpenLongStepNameL()
	{
	RTestSession session;
	ASSERT_TRUE(OpenSession(session,_L("te_RegStepTestSuite"))==0);
	CleanupClosePushL(session);
	session.Close();
	CleanupStack::Pop();	
	}
void CTEFClientSuiteUT::TestRunLongTestBlockCommandL()
	{
	
	RTestSession session;	
	TRequestStatus status;
	TExitCategoryName panicString;
	TBuf<10> blockArray(_L("BlockArray"));
	ASSERT_TRUE(OpenSession(session,_L("te_RegStepTestSuite"))==0);
	CleanupClosePushL(session);
	HBufC8* buffer = HBufC8::NewLC(256);
	TPtr8 block (buffer->Des());
	block.Zero();
	session.RunTestBlock(_L("123456789012345678901234567890123456789012345678901234567890123456789\
01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\
123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890")
						,panicString
						,block
						,status
						);
	/*	RunTestBlock() should not send the long command to server
	 *	if(aCommandString.Length() > KMaxTestExecuteCommandLength)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrTooBig);
		return;
		}
		SendReceive(....,aStatus);		
	 * 	In current we don't want change TEF beheivour, so we just let 
	 * server send command out.
	 * Because aStatus has be over wirted, KErrTooBig can't be return.
	 * the ASSERT_TRUE can't be execute.
	 */
//	ASSERT_TRUE(status.Int() == KErrTooBig);
	session.AbortTestStep();
	session.Close();
	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::Pop();
	}

void CTEFClientSuiteUT::TestRunLongTestStepCommandL()
	{	
	RTestSession session;
	ASSERT_TRUE(OpenSession(session,_L("te_RegStepTestSuite"))==0);
	CleanupClosePushL(session);
	TRequestStatus status;
	TExitCategoryName panicString;
	session.RunTestStep(_L("12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890")
						,panicString,status);
	/*	RunTestBlock() should not send the long command to server
	 *	if(aCommandString.Length() > KMaxTestExecuteCommandLength)
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status,KErrTooBig);
		return;
		}
		SendReceive(....,aStatus);		
	 * 	In current we don't want change TEF beheivour, so we just let 
	 * server send command out.
	 * Because aStatus has be over wirted, KErrTooBig can't be return.
	 * the ASSERT_TRUE can't be execute.
	 */
//	ASSERT_TRUE(status.Int() == KErrTooBig);
	session.AbortTestStep();
	session.Close();
	CleanupStack::Pop();
	}

void CTEFClientSuiteUT::TestRunTestStepL()
	{
	RTestSession session;
	ASSERT_TRUE(OpenSession(session,_L("te_RegStepTestSuite"))==0);
	CleanupClosePushL(session);
	TRequestStatus status;
	TExitCategoryName panicString;
	session.RunTestStep(_L("OOM=0 !Setup=0 -1 DT-TEF-TestStep-0001 c: C: C: c:\\testdata\\configs\\readwriteconfig.ini Section7"),panicString,status);
	User::WaitForRequest(status);
	panicString.Zero();
	session.RunTestStep(_L("OOM=0 !Setup=0 -1 DT-TEF-TestStep-0001 c: C: C: c:\\testdata\\configs\\readwriteconfig.ini Section7"),panicString,status);
	session.AbortTestStep();
	session.Close();
	CleanupStack::Pop();
	}

void CTEFClientSuiteUT::TestLongServerName()
	{
	TInt err;
	err = iServer.Connect(_L("SYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAuthorSYMScriptAutho"));	
	INFO_PRINTF2( _L("Server Connect returns = %d"), err );	
	ASSERT_TRUE( (KErrTooBig == err) );
	}

CTestSuite* CTEFClientSuiteUT::CreateSuiteL( const TDesC& aName )
/**
 * CreateSuiteL
 *
 * @param aName - Suite name
 * @return - Suite
 */
	{
	SUB_SUITE;
	ADD_TEST_STEP_CLASS( CTEFClientSuiteUT,TestRunTestStepL );
	ADD_TEST_STEP_CLASS( CTEFClientSuiteUT,TestOpenLongStepNameL);
	ADD_TEST_STEP_CLASS( CTEFClientSuiteUT,TestRunLongTestBlockCommandL);
	ADD_TEST_STEP_CLASS( CTEFClientSuiteUT,TestRunLongTestStepCommandL);
	ADD_TEST_STEP_CLASS( CTEFClientSuiteUT,TestLongServerName);
	END_SUITE;
	}
