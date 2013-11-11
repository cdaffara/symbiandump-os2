/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>
#include <test/testexecutelog.h>
#include <apmstd.h>
#include <caf/caf.h>
#include <caf/data.h>
#include "datastep.h"
#include "rtadata.h"
#include "reftestagentconstants.h"
#include "rtacontent.h"
#include "virtualFile.h"
#include "EntireFile.h"
#include "drmfilesparser.h"

using namespace ContentAccess;
using namespace ReferenceTestAgent;

/*
 * Open a file containing no xml tags.
 */

COpenCorruptedFile::~COpenCorruptedFile()
	{
	}

COpenCorruptedFile::COpenCorruptedFile(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KOpenCorruptedFile);
	}


TVerdict COpenCorruptedFile::doTestStepL()
	{
	SetTestStepResult(EFail);

	__UHEAP_MARK;	
	_LIT(uniqueID, "\\corrupt");	
	INFO_PRINTF1(_L("Open Corrupted File"));
	TPtrC fileName;
	TPtrC outputPath;
	GetStringFromConfig(ConfigSection(),_L("source"),fileName);

	RRtaData dataServer;
	CleanupClosePushL(dataServer);
	TInt err = dataServer.Open(fileName,uniqueID,EFileRead | EFileStream | EFileShareReadersOnly);
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	if(err == KErrCorrupt) 
	{
	SetTestStepResult(EPass);
	}
	
	CleanupStack::PopAndDestroy(&dataServer);

	__UHEAP_MARKEND;

	return TestStepResult();
}	

/*
 * Open container with unique id too large.
 */

CContainerUniqueIdBdrChk::~CContainerUniqueIdBdrChk()
	{
	}

CContainerUniqueIdBdrChk::CContainerUniqueIdBdrChk(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContainerUniqueIdBdrChk);
	}

TVerdict CContainerUniqueIdBdrChk::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCAOutOfRange)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

/*
 * Open content with name too large.
 */

CContentNameBdrChk::~CContentNameBdrChk()
	{
	}

CContentNameBdrChk::CContentNameBdrChk(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContentNameBdrChk);
	}

TVerdict CContentNameBdrChk::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCAOutOfRange)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();

	}

/*
 * Open content with unique id too large.
 */

CContentUniqueIdBdrChk::~ CContentUniqueIdBdrChk()
	{
	}

CContentUniqueIdBdrChk::CContentUniqueIdBdrChk(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContentUniqueIdBdrChk);
	}

TVerdict CContentUniqueIdBdrChk::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCAOutOfRange)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

/*
 * Open container with no unique id.
 */

CContainerNoUniqueId::~CContainerNoUniqueId()
	{
	}

CContainerNoUniqueId::CContainerNoUniqueId(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContainerNoUniqueId);
	}

TVerdict CContainerNoUniqueId::doTestStepL()
	{
	INFO_PRINTF1(_L("hello 00000, reading configuration 1..."));
	SetTestStepResult(EFail);
	TPtrC fileName;
	INFO_PRINTF1(_L("hello, reading configuration 1..."));
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	INFO_PRINTF1(_L("hello, reading configuration 2..."));
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCorrupt)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

/*
 * Open content with no unique id.
 */

CContentNoUniqueId::~CContentNoUniqueId()
	{
	}

CContentNoUniqueId::CContentNoUniqueId(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContentNoUniqueId);
	}

TVerdict CContentNoUniqueId::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCorrupt)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

/*
 * Open container with empty unique id.
 */

CContainerEmptyUniqueId::~CContainerEmptyUniqueId()
	{
	}

CContainerEmptyUniqueId::CContainerEmptyUniqueId(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContainerEmptyUniqueId);
	}

TVerdict CContainerEmptyUniqueId::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCorrupt)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

/*
 * Open content with empty unique id.
 */

CContentEmptyUniqueId::~CContentEmptyUniqueId()
	{
	}

CContentEmptyUniqueId::CContentEmptyUniqueId(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KContentEmptyUniqueId);
	}

TVerdict CContentEmptyUniqueId::doTestStepL()
	{
	SetTestStepResult(EFail);
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("filename"),fileName);
	CEntireFile *entireFile = CEntireFile::NewL(fileName);
	CDrmFilesParser *parser = 0;
	TRAPD(err, parser = CDrmFilesParser::NewL(*entireFile));
	INFO_PRINTF2(_L("Error returned is <%d>"),err);
	delete parser;
	delete entireFile;
	if (err == KErrCorrupt)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}



CBadFileHandleCheck::~CBadFileHandleCheck()
	{
	}

CBadFileHandleCheck::CBadFileHandleCheck(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KBadFileHandleCheck);
	}


TVerdict CBadFileHandleCheck::doTestStepL()
	{
	SetTestStepResult(EFail);
	__UHEAP_MARK;	
		
	TPtrC fileName;
	TPtrC uniqueId;
	GetStringFromConfig(ConfigSection(),_L("source"), fileName);
	GetStringFromConfig(ConfigSection(),_L("uniqueId"), uniqueId);
	
	RFs fs;	
	fs.Connect();
	CleanupClosePushL(fs);
	
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(fs, fileName, EFileShareAny));
	
	//The fileserver session is not marked as shareable. Accessing the file handle in another 
	//process results in KErrBadHandle.
	
	INFO_PRINTF1(_L("RRtaContent Bad File Handle Check"));
	RRtaContent content;
	CleanupClosePushL(content);
	TInt error1 = content.Open(file);

	INFO_PRINTF1(_L("RRtaData Bad File Handle Check"));
	RRtaData data;
	CleanupClosePushL(data);
	TInt error2 = data.Open(file, uniqueId);
	
	if((KErrBadHandle == error2) && (KErrBadHandle == error1))
		{
		SetTestStepResult(EPass);
		}

	CleanupStack::PopAndDestroy(4, &fs);
	__UHEAP_MARKEND;
	return TestStepResult();
	}	
