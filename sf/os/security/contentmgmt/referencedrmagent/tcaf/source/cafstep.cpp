/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cafstep.h"
#include <test/testexecutelog.h>
#include <caf/content.h>
#include <caf/attribute.h>
#include "cafserver.h"
#include "resolver.h"
#include "manager.h"
#include "bitset.h"
#include "CafApaRecognizer.h"

#include <apasvst.h>
#include <apaflrec.h>
#include <fbs.h>
#include <charconv.h>
#include <eikenv.h>

using namespace ContentAccess;

/* 
 * Base class for all CAF test steps
 *
 */

CCAFStep::CCAFStep()
	{
	}

CCAFStep::~CCAFStep()
	{
	}
	
void CCAFStep:: InitialiseFileHandleParametersL()	
	{
	GetStringFromConfig(ConfigSection(),_L("uri"),iUri);
	GetStringFromConfig(ConfigSection(),_L("uniqueid"),iUniqueId);
	GetIntFromConfig(ConfigSection(),_L("result"),iExpectedResult);

	iFs.Connect();
	CleanupClosePushL(iFs);
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iFile.Open(iFs, iUri, EFileShareReadersOnly| EFileRead | EFileStream));
	CleanupClosePushL(iFile);
	}
	
void CCAFStep::ReadBufferL(TDesC16& aFileName, TDes8& aBuffer)
	{
	RFs fs;
	RFile handle;
	TInt result;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	result = handle.Open(fs,aFileName,EFileRead | EFileStream | EFileShareAny);
	User::LeaveIfError(result);
	CleanupClosePushL(handle);
	handle.Read(aBuffer);
	CleanupStack::PopAndDestroy(2, &fs);
	}

HBufC8* CCAFStep::ConvertDes16toHBufC8LC(TDesC& source)
	{
	RFs fs;
	HBufC8 *buf = HBufC8::NewL(source.Length());
	CleanupStack::PushL(buf);
	TPtr8 ptr=buf->Des();

	CCnvCharacterSetConverter *converter = CCnvCharacterSetConverter::NewLC();
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, fs);
	converter->ConvertFromUnicode(ptr, source);
	CleanupStack::PopAndDestroy(2, converter);
	return buf;
	}

HBufC16* CCAFStep::ConvertDes8toHBufC16LC(TDesC8& source)
	{
	RFs fs;
	TInt status=0;
	HBufC16 *buf = HBufC16::NewL(source.Length());
	CleanupStack::PushL(buf);
	TPtr16 ptr=buf->Des();
	CCnvCharacterSetConverter *converter = CCnvCharacterSetConverter::NewLC();
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, fs);
	converter->ConvertToUnicode(ptr, source, status);
	CleanupStack::PopAndDestroy(2, converter);
	return buf;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
         
HBufC8* CCAFStep::CreateWmdrmHeaderLC()     
    {     
    // create dummy header data.     
    const TInt KMaxBufferLen = 64;     
    _LIT8(KHeaderData, "W\0R\0M\0H\0E\0A\0D\0E\0R\0");     
         
    HBufC8* headerData = HBufC8::NewMaxLC(KMaxBufferLen);     
    TPtr8 headerPtr = headerData->Des();     
    headerPtr.Repeat(KHeaderData);     
    return headerData;     
    }     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
  

void CCAFStep::LoadCafRecognizerL()
	{
	static const TUid KImplementationUID= {0x102031E9};

	TAny* ptr = REComSession::CreateImplementationL(KImplementationUID, iDtorIDKey);
	iRecognizer = static_cast <CApaDataRecognizerType*>(ptr);
    }

void CCAFStep::UnLoadCafRecognizer()
	{
	delete iRecognizer;
	iRecognizer=NULL;
	REComSession::DestroyedImplementation(iDtorIDKey);
	REComSession::FinalClose();
	}

TInt CCAFStep::Delete(const TDesC& aFile)
	{
	TInt err = KErrNone;
	RFs fs;
	err = fs.Connect();
	if(err == KErrNone)
		{
		err = fs.Delete(aFile);
		fs.Close();
		}
	return err;
	}



TVerdict CCAFStep::doTestStepPreambleL()
	{
	iActiveScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	return TestStepResult();
	}

TVerdict CCAFStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
	return TestStepResult();
	}


// Dummy class required to create an apparc server (below)
class CT_Serv2DummyShell : public CBase, public MApaAppStarter
     {
 public:
     TThreadId StartAppL(const CApaCommandLine&) { return *(TThreadId*)this; }
     void ConstructL();
 public:
     RFs iFs;
     RFbsSession iFbs;
     };

void CT_Serv2DummyShell::ConstructL()
     {
     User::LeaveIfError(iFs.Connect());
     User::LeaveIfError(iFbs.Connect());
     }


void CCAFStep::StartApparcServerL()
	{
	INFO_PRINTF1(_L("Starting Apparc Server"));
    FbsStartup();
    TInt ret=RFbsSession::Connect();
         TEST(ret==KErrNone);
 
     // set up an fbs
     ret = FbsStartup();
         TEST(ret==KErrNone);
     //
     // set up an app starter
     CT_Serv2DummyShell* dummy=new(ELeave) CT_Serv2DummyShell();
     dummy->ConstructL();
 
 	 ret=StartupApaServerProcess();


     TEST(((ret==KErrNone)||(ret==KErrAlreadyExists)));
     //
 
 
     INFO_PRINTF1(_L("Apparc Startup Complete"));
	}
