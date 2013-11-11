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
// Unit test implementation for CMatchData class
// 
//

/**
 @file
 @internalComponent
*/

#include <e32test.h>
#include <f32file.h>
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>

#include <xml/matchdata.h>
#include <xml/xmlframeworkerrors.h>

/*
Those tests will make use of a fake array of parsers built for the purpuse of unit test only.
Here is a detailed list of parser used:
*/

const TUid KXmlUtP1Uid = {0x10273870};
const TUid KXmlUtP2Uid = {0x10273871};
const TUid KXmlUtP4Uid = {0x10273873};
const TUid KXmlUtP6Uid = {0x10273875};
const TUid KXmlUtP7Uid = {0x10273876};
const TUid KXmlUtP9Uid = {0x10273878};

//These 2 Uids are added to test the fix of DEF116214.
//const TUid KXmlUtP10Uid = {0x60000000}; //0b01100000000000000000000000000000 Large positive number
const TUid KXmlUtP11Uid = {0x8FFFFFFF}; //0b10001111111111111111111111111111 Very Large negative number


//

_LIT(KTestName,	"CMatchData Unit Tests");
RTest	Test(KTestName);

//
//Global variable systemStatus indicating the plugin system status
RImplInfoArray systemPlugins;
RImplInfoArray systemPluginsBase;


//Test Data
_LIT8(KXmlUtParserXmlMimeType, "text/xml");
_LIT8(KXmlUtParserWbXmlMimeType, "text/wbxml");
_LIT8(KXmlUtSymbianString, "Symbian");
_LIT8(KXmlUtNullString, "");
_LIT8(KXmlUtNonExistantString,"NonExistant");
_LIT8(KXmlUtParserXmlMimeTypeCS, "Text/Xml");
_LIT8(KXmlUtParserWbXmlMimeTypeCS, "Text/WbXml");
_LIT8(KXmlUtSymbianStringCS, "symbian");
_LIT8(KXmlUtExternalString, "External");
_LIT8(KXmlUtSymbianAdvancedString, "Symbian Advanced");
_LIT8(KXmlUtExternalV1String, "External V1");
_LIT8(KXmlUtParserMyXmlMimeTypeCS, "MyText/MyXml");


// 256 character string
TBuf8<256> KXmlUtLongString256;
//255 character string
TBuf8<255> KXmlUtLongString255;

// 33 character string
TBuf8<33> KXmlUtLongString33;
// 32 character string
TBuf8<32> KXmlUtLongString32;

// Interface UID for unit test parsers
TUid KXmlUtParsers = {0x10273867};

// Separator for comand line to run the file copy process
_LIT(KSeparator, "|");

// Name of the file copy process
_LIT(KProcessFileManCopyFile, "t_XMLprocessfilemancopyfile.exe");

// Const value for 1 second in miliseconds
const TInt KOneSecond = 1000000;

static void FileCopyL(const TDesC& anOld,const TDesC& aNew)
	{
	TBuf<120> fileNames(anOld);
	fileNames.Append(KSeparator);
	fileNames.Append(aNew);

	// Launch process with highier capabilities (to copy into c:\sys\bin directory)
	TRequestStatus stat;
	RProcess p;
		User::LeaveIfError(p.Create(KProcessFileManCopyFile(), fileNames));
	// Asynchronous logon: completes when process terminates with process
	// exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	User::LeaveIfError(exitReason);

	}

static void TestPrepareL()
	{
	FileCopyL(_L("c:\\XMLRAMONLY\\tu_XmlRamParserPlugins.dll"), _L("C:\\sys\\bin\\tu_XmlRamParserPlugins.dll"));
	FileCopyL(_L("c:\\XMLRAMONLY\\tu_XmlRamParserPlugins.RSC"), _L("C:\\resource\\plugins\\tu_XmlRamParserPlugins.RSC"));

	//The reason for the folowing delay is:
	//ECOM server could be already started. It means that when we copy some
	//ECOM plugins from Z: to C: drive - ECOM server should look for and
	//find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	//which is an active object. So the discovering service is asynchronous. We have to
	//wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	//requested implementations.
	User::After(KOneSecond * 3);
	}


static void SetUpSystemStatusL()
	{
	REComSession& session = REComSession::OpenL();
	RImplInfoPtrArray array;
	session.ListImplementationsL(KXmlUtParsers, array);
	session.Close();
	for (TInt i = 0; i < array.Count(); i++)
		{
		systemPluginsBase.Append(array[i]);
		}
	}

static void RefreshPluginsStatus()
	{
	systemPlugins.Reset();

	for (TInt i = 0; i < systemPluginsBase.Count(); i++)
		{
		systemPlugins.Append(systemPluginsBase[i]);
		}
	}
static void CleanSystem()
	{
	for (TInt i = 0; i < systemPluginsBase.Count(); i++)
		{
		delete systemPluginsBase[i];
		}
	systemPlugins.Close();
	systemPluginsBase.Close();
	}


/**
@SYMTestCaseID 		 		SYSLIB-XML-UT-3719
@SYMTestCaseDesc		    Checking CMatchData functions
@SYMTestPriority 		    Medium
@SYMTestActions  		    Testing various functions of the CMatchData class
@SYMTestExpectedResults 	The functions should perform as expected.
@SYMREQ 		 		 	REQ0000
*/
static void RunTestsL()
	{
	Xml::CMatchData *mData = Xml::CMatchData::NewL();
	TInt result;
	TUid resultUid;

	//set up systemStatus
	TestPrepareL();
	SetUpSystemStatusL();
	RefreshPluginsStatus();

	//*************************************************
	//Test - Basic API
	Test.Next(_L("Basic Api 1"));
	TRAP(result,mData->SetMimeTypeL(KXmlUtParserXmlMimeType));

	Test(result == KErrNone);
	Test(mData->MimeType().Compare(KXmlUtParserXmlMimeType)==0);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 2"));
	KXmlUtLongString256.SetLength(256);
	TRAP(result,mData->SetMimeTypeL(KXmlUtLongString256));

	Test(result == KErrArgument);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 3"));
	KXmlUtLongString255.SetLength(255);
	TRAP(result,mData->SetMimeTypeL(KXmlUtLongString255));

	Test(result == KErrNone);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 4"));
	TRAP(result,mData->SetMimeTypeL(KXmlUtNullString));

	Test(result == KErrArgument);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 5"));
	TRAP(result,mData->SetVariantL(KXmlUtSymbianString));

	Test(result == KErrNone);
	Test(mData->Variant().Compare(KXmlUtSymbianString)==0);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 6"));
	KXmlUtLongString33.SetLength(33);
	TRAP(result,mData->SetVariantL(KXmlUtLongString33));

	Test(result == KErrArgument);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 7"));
	KXmlUtLongString32.SetLength(32);
	TRAP(result,mData->SetVariantL(KXmlUtLongString32));

	Test(result == KErrNone);

	//*************************************************
	//Test  - Basic API
	Test.Next(_L("Basic Api 8"));
	TRAP(result,mData->SetVariantL(KXmlUtNullString));

	Test(result == KErrNone);
	Test(mData->Variant().Compare(KXmlUtNullString)==0);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 1"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	// Now returns lowest uid Symbian parser
	Test(mData->ResolveL(systemPlugins) == KXmlUtP1Uid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 2"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	// Now returns lowest uid Symbian parser
	Test(KXmlUtP1Uid == mData->ResolveL(systemPlugins));

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 3"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	Test(KXmlUtP6Uid == mData->ResolveL(systemPlugins));

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 4"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	Test(KXmlUtP6Uid == mData->ResolveL(systemPlugins));

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 5"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 6"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue)	;
	mData->SetMimeTypeL(KXmlUtParserXmlMimeTypeCS);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 7"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeTypeCS);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 8"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtNonExistantString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 9"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid Symbian parser
	Test(resultUid == KXmlUtP4Uid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 10"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid parser
	Test(resultUid == KXmlUtP7Uid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 11"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 12"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid parser
	Test(resultUid == KXmlUtP7Uid);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 13"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);

	//*************************************************
	//Test - Mime type tests
	Test.Next(_L("Mime type test 14"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtNonExistantString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);



	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 1"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KXmlUtP6Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 2"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KXmlUtP6Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 3"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(resultUid == KXmlUtP6Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 4"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KXmlUtP7Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 5"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KXmlUtP7Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 6"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 7"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	mData->SetVariantL(KXmlUtExternalString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid parser
	Test(resultUid == KXmlUtP9Uid);

	//*************************************************
	//Test - Mime type and variant tests.
	Test.Next(_L("Mime type and variant test 8"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid parser
	Test(resultUid == KXmlUtP2Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 9"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	mData->SetVariantL(KXmlUtExternalString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 10"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtExternalV1String);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 11"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(ETrue);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KXmlUtP7Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 12"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 13"));
	mData->SetCaseSensitivity(EFalse);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeType);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid parser
	Test(resultUid == KXmlUtP2Uid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 14"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(EFalse);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtSymbianAdvancedString);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);

	//*************************************************
	//Test - Mime type and variant tests
	Test.Next(_L("Mime type and variant test 15"));
	mData->SetCaseSensitivity(ETrue);
	mData->SetRomOnly(EFalse);
	mData->SetLeaveOnMany(ETrue);
	mData->SetMimeTypeL(KXmlUtParserWbXmlMimeTypeCS);
	mData->SetVariantL(KXmlUtExternalV1String);
	TRAP(result, resultUid = mData->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);


	//*************************************************
	//Test - DEF116214 - CMatchData::SortOrder() function behaviour test.
	Test.Next(_L("CMatchData DEF116214 fix test"));
	Xml::CMatchData *mData_2 = Xml::CMatchData::NewL();
	mData_2->SetCaseSensitivity(ETrue);
	mData_2->SetRomOnly(EFalse);
	mData_2->SetLeaveOnMany(EFalse);
	mData_2->SetMimeTypeL(KXmlUtParserMyXmlMimeTypeCS);
	mData_2->SetVariantL(KXmlUtSymbianStringCS);

	//There are 2 elements which could match this mData_2 criteria,
	//ResolveL should return the first one (the smallest one). In the old
	//implementation of CMatchData::SortOrder(), ResolveL() return KXmlUtP10Uid
	//but after the applied fix ResolveL() returns KXmlUtP11Uid.
	//The following test should pass.
	Test(mData_2->ResolveL(systemPlugins) == KXmlUtP11Uid);

	delete mData_2;

	//*************************************************
	//Test - CMatchData default values
	Test.Next(_L("CMatchData default test 1"));
	Xml::CMatchData *newData1 = Xml::CMatchData::NewL();
	newData1->SetMimeTypeL(KXmlUtParserWbXmlMimeType);

	TRAP(result, resultUid = newData1->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Default behaviour became backwards compatible, so Test plugin 4
	// is returned instead of 2 here because Test Plugin 2's variant is
	// not an exact match of "Symbian", but 4's is.
	Test(resultUid == KXmlUtP4Uid);
	delete newData1;

	//*************************************************
	//Test - CMatchData default values
	Test.Next(_L("CMatchData default test 2"));
	Xml::CMatchData *newData2 = Xml::CMatchData::NewL();
	newData2->SetMimeTypeL(KXmlUtParserXmlMimeType);

	TRAP(result, resultUid = newData2->ResolveL(systemPlugins));

	Test(result == KErrNone);
	// Now returns lowest uid Symbian parser
	Test(resultUid == KXmlUtP1Uid);
	delete newData2;

	//*************************************************
	//Test - CMatchData default values
	Test.Next(_L("CMatchData default test 3"));
	Xml::CMatchData *newData3 = Xml::CMatchData::NewL();
	newData3->SetMimeTypeL(KXmlUtParserXmlMimeTypeCS);

	TRAP(result, resultUid = newData3->ResolveL(systemPlugins));

	Test(result == KErrNone);
	Test(resultUid == KNullUid);
	delete newData3;

	//*************************************************
	//Test - CMatchData default values
	Test.Next(_L("CMatchData default test 4"));
	Xml::CMatchData *newData4 = Xml::CMatchData::NewL();
	newData4->SetMimeTypeL(KXmlUtParserXmlMimeType);
	newData4->SetLeaveOnMany(ETrue);

	TRAP(result, resultUid = newData4->ResolveL(systemPlugins));

	Test(result == Xml::KErrXmlMoreThanOneParserMatched);
	delete newData4;

	//*************************************************
	//Test - comparison operator tests
	Test.Next(_L("Comparison operator tests"));
	Xml::CMatchData *newData5 = Xml::CMatchData::NewL();
	Xml::CMatchData *newData6 = Xml::CMatchData::NewL();
	Test(*newData5 == *newData6);

	newData5->SetCaseSensitivity(EFalse);
	newData6->SetCaseSensitivity(EFalse);
	Test(*newData5 == *newData6);

	newData5->SetMimeTypeL(KXmlUtParserXmlMimeType);
	Test(!(*newData5 == *newData6));

	newData6->SetMimeTypeL(KXmlUtParserXmlMimeTypeCS);
	Test(*newData5 == *newData6);

	newData5->SetCaseSensitivity(ETrue);
	Test(!(*newData5 == *newData6));

	newData6->SetCaseSensitivity(ETrue);
	Test(!(*newData5 == *newData6));

	newData6->SetMimeTypeL(KXmlUtParserXmlMimeType);
	Test(*newData5 == *newData6);

	newData6->SetLeaveOnMany(ETrue);
	Test(!(*newData5 == *newData6));

	newData6->SetLeaveOnMany(EFalse);
	newData6->SetRomOnly(ETrue);
	Test(!(*newData5 == *newData6));

	newData6->SetRomOnly(EFalse);
	Test(*newData5 == *newData6);

	newData5->SetVariantL(KXmlUtSymbianString);
	Test(!(*newData5 == *newData6));

	newData6->SetVariantL(KXmlUtSymbianString);
	Test(*newData5 == *newData6);

	newData5->SetCaseSensitivity(ETrue);
	newData6->SetCaseSensitivity(ETrue);
	Test(*newData5 == *newData6);

	newData6->SetVariantL(KXmlUtSymbianStringCS);
	Test(!(*newData5 == *newData6));

	newData5->SetCaseSensitivity(EFalse);
	newData6->SetCaseSensitivity(EFalse);
	Test(*newData5 == *newData6);

	newData6->SetVariantL(_L8(""));
	Test(!(*newData5 == *newData6));

	delete newData5;
	delete newData6;

	//*************************************************
	//Test - Assignement tests
	Test.Next(_L("Comparison operator tests"));
	Xml::CMatchData *newData7 = Xml::CMatchData::NewL();
	Xml::CMatchData *newData8 = Xml::CMatchData::NewL();
	Test(*newData7 == *newData8);

	newData8->SetMimeTypeL(KXmlUtParserXmlMimeType);
	Test(!(*newData7 == *newData8));

	*newData7 = *newData8;
	Test(*newData7 == *newData8);
	Test(newData7->MimeType().Compare(KXmlUtParserXmlMimeType()) == 0);

	newData7->SetVariantL(KXmlUtSymbianString);
	*newData7 = *newData8;
	Test(*newData7 == *newData8);
	Test(newData7->Variant() == KNullDesC8);

	delete newData7;
	delete newData8;

	//*************************************************
	// Out of memory tests
	Test.Next(_L("Out of memory tests"));
	TInt err;
	Xml::CMatchData* parser9 = NULL;

	// Test NewL
	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TRAP(err, parser9 = Xml::CMatchData::NewL());
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			delete parser9;
			__UHEAP_MARKEND;
			RDebug::Print(_L("The OOM test 1 succeeded at heap failure rate=%d.\n"), count);
			__UHEAP_RESET;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			Test(err, KErrNone);
			}
		__UHEAP_RESET;
		}

	TRAP(err, parser9 = Xml::CMatchData::NewL());
	Test(err == KErrNone);

	//Test SetMimeTypeL
	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TRAP(err, parser9->SetMimeTypeL(KXmlUtParserXmlMimeType()));
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			RDebug::Print(_L("The OOM test 1 succeeded at heap failure rate=%d.\n"), count);
			__UHEAP_RESET;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			Test(err, KErrNone);
			}
		__UHEAP_RESET;
		}

	//Test SetVariantL
	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TRAP(err, parser9->SetVariantL(KXmlUtSymbianString()));
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			RDebug::Print(_L("The OOM test 1 succeeded at heap failure rate=%d.\n"), count);
			__UHEAP_RESET;
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			Test(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	delete parser9;

	//*************************************************
	//Test cleanup
	delete mData;
	CleanSystem();
	systemPlugins.Close();
	}



//***********************************************************************************
GLDEF_C TInt E32Main()
 {
	CTrapCleanup* tc = CTrapCleanup::New();
	Test(tc != NULL);


	TInt err;
	Test.Start(_L(" @SYMTestCaseID:SYSLIB-XML-UT-3719 CMatchData Tests "));
	Test.Title();

	__UHEAP_MARK;
	TRAP(err, ::RunTestsL())
	Test(err== KErrNone);
	__UHEAP_MARKEND;

	Test.End();
	Test.Close();



	delete tc;

	return(KErrNone);

	}



