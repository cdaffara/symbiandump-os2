/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
*/

#include <e32test.h>
#include <e32std.h>
#include "tokentypesenum.h"
#include "fstokencliserv.h"
#include "t_testhandler.h"
#include "tscriptandhardcoded.h"
#include "t_keystore_defs.h"
#include <ecom/ecom.h>
#include "t_keystore_actions.h"
#include "testexecuteinterface.h"
#include "t_filetokens.h"

LOCAL_D void DoTestsL()
	{
	START_SCRIPT_LIST
	SCRIPT_ITEM(CInitialiseKeyStore, KInitStore),			
	SCRIPT_ITEM(CDeleteKeyStore, KDeleteStore),				
	SCRIPT_ITEM(CAddKey, KAddKey),							
	SCRIPT_ITEM(CListKeys, KListKeys),						
	SCRIPT_ITEM(CListAllKeys, KListAllKeys),						
	SCRIPT_ITEM(CDeleteKeys, KDeleteKeys),
	SCRIPT_ITEM(CImportKey, KImportKey),
	SCRIPT_ITEM(CGetKeyInfo, KGetKeyInfo),
	SCRIPT_ITEM(COpenKey, KOpenKey),
	SCRIPT_ITEM(CSign, KSign),
	SCRIPT_ITEM(CDecrypt, KDecrypt),
	SCRIPT_ITEM(CDerive, KDerive),
	SCRIPT_ITEM(CExportKey, KExportKey),
	SCRIPT_ITEM(CDeleteKeyStoreData, KDeleteKeystoreData),
   	SCRIPT_ITEM(CGetPolicy, KGetUsePolicy),
	SCRIPT_ITEM(CSetPolicy, KSetUsePolicy),
	SCRIPT_ITEM(CGetPolicy, KGetManagementPolicy),
	SCRIPT_ITEM(CSetPolicy, KSetManagementPolicy),
   	SCRIPT_ITEM(CSetTimeout, KSetTimeout),
   	SCRIPT_ITEM(CRelockStore, KRelockStore),
	SCRIPT_ITEM(CChangePassphrase, KChangePassphrase),
	SCRIPT_ITEM(CListProtectedObjects, KListProtectedObjects),
	SCRIPT_ITEM(CUnblockPassphrase, KUnblockPassphrase),
	SCRIPT_ITEM(CAuthOpen, KAuthOpen),
	SCRIPT_ITEM(CAuthClose, KAuthClose),
	SCRIPT_ITEM(CGetTimeRemaining, KGetTimeRemaining),
   	SCRIPT_ITEM(CAuthSetTimeout, KAuthSetTimeout),
   	SCRIPT_ITEM(CGetTimeout, KGetTimeout),
   	SCRIPT_ITEM(CGetTimeRemaining, KGetTimeRemaining),
   	SCRIPT_ITEM(CSetTime, KSetTime),
	SCRIPT_ITEM(CExportKey, KExportKey),
	SCRIPT_ITEM(CCompare, KCompare),
	SCRIPT_ITEM(CExportPublicKey, KExportPublicKey),
	SCRIPT_ITEM(CCorrupt, KCorrupt),
	SCRIPT_ITEM(CCheckServerHeapError, KCheckServerHeapError),
	SCRIPT_ITEM(CServerOOMTestStart, KServerOOMTestStart),
	SCRIPT_ITEM(CServerOOMTestStop, KServerOOMTestStop)
#ifdef SYMBIAN_AUTH_SERVER
	,SCRIPT_ITEM(CAuthenticationPolicy, KAuthenticationPolicy)
#endif // SYMBIAN_AUTH_SERVER
	END_SCRIPT_LIST

	// this bit is because of heap checking problems with techview
	CConsoleBase* console = Console::NewL(_L("Test code"), TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	__UHEAP_MARK;
	
	TTime now;
	now.HomeTime();
		
	// Make sure shared keystore is initialised before running the test
	// and destroyed after completion
	CSharedKeyStores::InitialiseKeyStoresL();
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<64> scriptFile (sysDriveName);
	scriptFile.Append(_L("\\tkeystore\\scripts\\tkeystore.txt"));
	
 	TBuf<64> logFile (sysDriveName);
	logFile.Append(_L("\\tkeystore.log"));
 	
  	CScriptSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile, ETrue, console);	
	
	CSharedKeyStores::DestroyKeyStores();
	
	TTime end;
	end.HomeTime();
	
	if (end.MonthsFrom(now).Int() != 0)
		{
		
		// The test has changed the time significantly
		User::SetHomeTime(now);
		
		}
	
	__UHEAP_MARKEND;
	CleanupStack::PopAndDestroy(console);
	}

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

	TRAPD(err, DoTestsL());
	if(err != KErrNone)
		{
		User::Panic(_L("t_keystore.cpp,Main"),err);
		}
	delete cleanup; // destroy clean-up stack
	REComSession::FinalClose();
	__UHEAP_MARKEND;
	return 0; // and return
    }

