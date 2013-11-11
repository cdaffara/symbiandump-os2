/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <fbs.h>
#include "t_testhandler.h"
#include "t_certstoreactionsclient.h"
#include "t_certstoreactionslistcertificates.h"
#include "t_unifiedcertstorewritablecertstorecount.h"
#include "t_unifiedcertstorereadonlycertstorecount.h"
#include "t_unifiedcertstorecertstorecount.h"
#include "t_certstoredefs.h"
#include "t_certstoreactions.h"
#include "t_certstoreactionsfilecertstore.h"
#include "t_unifiedcertstoreadd.h"
#include "t_unifiedcertstoreremove.h"
#include "T_unifiedcertstoreapplications.h"
#include "t_unifiedcertstoreactiongetcert.h"
#include "t_ccttokentypeinfoattributes.h"
#include "t_certstoreactionshandle.h"
#include "t_retrieve.h"
#include "t_concurrentcertstore.h"
#include "t_certstoretests.h"
#include "t_testcertfordeletable.h"
#include "t_filetokens.h"
#include <ecom/ecom.h>

#include "tscriptandhardcoded.h"
#include "t_swicertstoreactions.h"

START_SCRIPT_LIST
SCRIPT_ITEM(CCreateFileCertStore, KCreateFileCertStore),
SCRIPT_ITEM(CImportKey, KImportKey),
SCRIPT_ITEM(CAddCertificate, KAddCert),
SCRIPT_ITEM(CInitialiseCertStore, KInitStore),
SCRIPT_ITEM(COnlyCreateCertStore, KOnlyCreateStore),
SCRIPT_ITEM(CDeleteCertStore, KDeleteStore),
SCRIPT_ITEM(CDeleteCertificate, KDeleteCert),
SCRIPT_ITEM(CDeleteRetrieveCACertificate, KDeleteRetrieveCACert),
SCRIPT_ITEM(CDeleteRetrieveUserCertificate, KDeleteRetrieveUserCert),
SCRIPT_ITEM(CListCertificates, KListCert),
SCRIPT_ITEM(CRetrieveCertificate, KRetrieveCert),
SCRIPT_ITEM(CUnifiedCertStoreApplications, KGetApplications),
SCRIPT_ITEM(CSetApplications, KSetApplications),
SCRIPT_ITEM(CSetTrusters, KSetTruster),
SCRIPT_ITEM(CGetTrusters, KGetTrusters),
SCRIPT_ITEM(CInitCertificateAppInfoManager, KInitAppManager),
SCRIPT_ITEM(CDeleteCertificateAppInfoManager, KDeleteAppManager),
SCRIPT_ITEM(CAddClient, KAddClient),
SCRIPT_ITEM(CGetClients, KGetClients),
SCRIPT_ITEM(CRemoveClient, KRemoveClient),
SCRIPT_ITEM(CUnifiedCertStoreWritableCertStoreCount, KCUnifiedCertStoreWritableCertStoreCount),
SCRIPT_ITEM(CUnifiedCertStoreReadOnlyCertStoreCount, KCUnifiedCertStoreReadOnlyCertStoreCount),
SCRIPT_ITEM(CUnifiedCertStoreCertStoreCount, KCUnifiedCertStoreCertStoreCount),
SCRIPT_ITEM(CUnifiedCertStoreGetCert, KCUnifiedCertStoreGetCert),
SCRIPT_ITEM(CInitSWICertStoreAction, KInitSWICertStore),
SCRIPT_ITEM(CGetCapabilitiesAction, KGetCapabilities),
SCRIPT_ITEM(CGetMandatoryAction, KGetMandatory),
SCRIPT_ITEM(CGetSystemUpgradeAction, KGetSystemUpgrade),
SCRIPT_ITEM(CGetDeletionAction, KGetDeletable),
SCRIPT_ITEM(CCTTokenTypeInfoAttributes, KCCTTokenTypeInfoAttributes),
SCRIPT_ITEM(CCertStoreHandle, KCCTCertInfoHandle),
SCRIPT_ITEM(CTestConcurrentCertStore, KTestConcurrentCertStore),
SCRIPT_ITEM(CTestCertForDeletable, KCTestCertForDeletable),
SCRIPT_ITEM(CServerOOMTestStart, KServerOOMTestStart),
SCRIPT_ITEM(CServerOOMTestStop, KServerOOMTestStop),
SCRIPT_ITEM(CMultipleReadersAction, KMultipleReaders),
SCRIPT_ITEM(CCheckServerHeapError, KCheckServerHeapError)
END_SCRIPT_LIST

void DeleteKeyStoreL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> datFile (sysDrive.Name());
	datFile.Append(_L("\\system\\data\\keys.dat"));
	User::LeaveIfError(fs.Delete(datFile));
	CleanupStack::PopAndDestroy(&fs);
	}

void DoTests()
	{
	// Need to delete the keystore first
	TRAP_IGNORE(DeleteKeyStoreL());
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> scriptFile (sysDrive.Name());
	scriptFile.Append(_L("\\tcertstore\\scripts\\unifiedcertstore1-conf1.txt"));
	
	TBuf<24> logFile (sysDrive.Name());
	logFile.Append(_L("\\tcertstore.log"));
	CScriptSetup::CreateAndRunTestsL(theTestTypes, scriptFile, logFile);
	}

void DoTests(const TDesC& aScriptFile, const TDesC& aLogFile, TBool isConcurrent)
	{
	TRAP_IGNORE(DeleteKeyStoreL());
	CNullConsole console;
	CConsoleBase* consolePtr = (isConcurrent) ? &console : NULL;
	CScriptSetup::CreateAndRunTestsL(theTestTypes, aScriptFile, aLogFile, EFalse, consolePtr);
	}
