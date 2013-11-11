/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkixcertstepbase.h"
#include "cleanuputils.h"
#include <s32file.h>

CPkixCertStepBase::~CPkixCertStepBase()
	{
	delete iConcatenatedChain;	
	iConcatenatedChain = NULL;
	iFileServer.Close();
	iProcessedOids.Reset();
	iOids.Reset();
	iRootCerts.ResetAndDestroy();
	CActiveScheduler::Install(NULL);
	delete iScheduler;
	}

TVerdict CPkixCertStepBase::doTestStepPreambleL()
	{
	//read in stuff from config
	
	GetBoolFromConfig(ConfigSection(), KPerformOom, iPerformOom);

	iFileServer.Connect();	
	TPtrC configString;

	//temporary storage of the DER encoded certs so we can calculate the combined size for iConcatenatedChain
	RPointerArray<HBufC8> rawCerts;
	CleanupResetAndDestroy<RPointerArray<HBufC8> >::PushL(rawCerts);
	TInt chainSize = 0;

	GetStringFromConfig(ConfigSection(), KEndEntity, configString);
	HBufC8* certBuf = ReadFileLC(configString);
	rawCerts.AppendL(certBuf);
	CleanupStack::Pop(certBuf);
	chainSize += certBuf->Size();
	

	RArray<TPtrC> certFileNames;
	CleanupClosePushL(certFileNames);
	
	GetStringArrayFromConfigL(ConfigSection(), KIntermediateCert, certFileNames);

	TInt i;
	for (i = 0; i < certFileNames.Count(); ++i)
		{
		certBuf = ReadFileLC(certFileNames[i]);
		rawCerts.AppendL(certBuf);
		CleanupStack::Pop(certBuf);
		chainSize += certBuf->Size();
		}
	
	CleanupStack::PopAndDestroy(&certFileNames);	
		
	iConcatenatedChain = HBufC8::NewL(chainSize);
	TPtr8 modPtr = iConcatenatedChain->Des();
	for (i = 0; i < rawCerts.Count(); i++)
		{
		modPtr.Append(*rawCerts[i]);
		}	
	
	CleanupStack::PopAndDestroy(&rawCerts);
	
	///////////
	
	//read in oid values.  the usage of these is dependant on the test step
	GetStringArrayFromConfigL(ConfigSection(), KOid, iOids);
	//convert into argument format used by APIs
	for (i=0; i < iOids.Count(); ++i)
		{
		iProcessedOids.AppendL(&(iOids[i]));
		}
		
		
	RArray<TPtrC> certFileNames2;
	CleanupClosePushL(certFileNames2);
	
	GetStringArrayFromConfigL(ConfigSection(), KRootCert, certFileNames2);
	CX509Certificate* cert;
	for (i = 0; i < certFileNames2.Count(); ++i)
		{
		certBuf = ReadFileLC(certFileNames2[i]);
		cert = CX509Certificate::NewLC(*certBuf);
		iRootCerts.AppendL(cert);
		CleanupStack::Pop(cert);
		CleanupStack::PopAndDestroy(certBuf);
		}
	CleanupStack::PopAndDestroy(&certFileNames2);	
	
	TInt uid;
	iUseUidOverload = GetIntFromConfig(ConfigSection(), KUid, uid);
	iUid.iUid = uid;
	
    iScheduler=new(ELeave) CActiveScheduler(); 
	CActiveScheduler::Install(iScheduler);
	
	return EPass;	
	}


//N.B. iCertChain must be popped and destroyed at the end of any deriving class's PerformTestL() method
void CPkixCertStepBase::PerformTestL()
	{
	iPtr.Set(*iConcatenatedChain);
	
	//split on which of the NewLs to use
	if (iUseUidOverload)
		{
		iCertChain = CPKIXCertChain::NewLC(iFileServer, iPtr, iUid);
		}
	else
		{		
		iCertChain = CPKIXCertChain::NewLC(iFileServer, iPtr, iRootCerts);			
		}
	}
	
	
HBufC8* CPkixCertStepBase::ReadFileLC(const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFileServer, aFileName, EFileRead));
	CleanupClosePushL(file);
    TInt size;
    file.Size(size);
    CleanupStack::PopAndDestroy();//fileClose

    HBufC8* result = HBufC8::NewLC(size);
    TPtr8 ptr(result->Des());
    ptr.SetLength(size);

    RFileReadStream stream;
    User::LeaveIfError(stream.Open(iFileServer, aFileName, EFileStream));
    CleanupClosePushL(stream);
    stream.ReadL(ptr, size);
    CleanupStack::PopAndDestroy();//streamClose
    return result;	
	}


void CPkixCertStepBase::GetStringArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TPtrC>& aArray)
	{
    HBufC* buf = HBufC::NewLC(aKeyName.Length() + KKeyFormat().Length());
    TPtr ptr(buf->Des());
    INFO_PRINTF2(_L("Parsing attribute: %S"), &aKeyName);

    TInt i = 0;
    TBool cont = ETrue;
    do
        {
        ++i;
        ptr = aKeyName;
        ptr.AppendFormat(KKeyFormat(), i);
        TPtrC val;

        cont = GetStringFromConfig(aSectName, ptr, val);
        if (cont)
            {
            User::LeaveIfError(aArray.Append(val));
            }
        } while (cont);

    INFO_PRINTF2(_L("Element count: %d"), i-1);
    CleanupStack::PopAndDestroy(buf);
	}
	
	
void CPkixCertStepBase::GetIntArrayFromConfigL(const TDesC& aSectName, const TDesC& aKeyName, RArray<TInt>& aArray)
	{
	HBufC* buf = HBufC::NewLC(aKeyName.Length() + KKeyFormat().Length());
    TPtr ptr(buf->Des());
    INFO_PRINTF2(_L("Parsing attribute: %S"), &aKeyName);

    TInt i = 0;
    TBool cont = ETrue;
    do
        {
        ++i;
        ptr = aKeyName;
        ptr.AppendFormat(KKeyFormat(), i);
        TInt val;

        cont = GetIntFromConfig(aSectName, ptr, val);
        if (cont)
            {
            User::LeaveIfError(aArray.Append(val));
            }
        } while (cont);

    INFO_PRINTF2(_L("Element count: %d"), i-1);
    CleanupStack::PopAndDestroy(buf);
	}

TVerdict CPkixCertStepBase::doTestStepL()
	{
	if (!iPerformOom)	
		{
		TRAPD(err, PerformTestL());
		if (err == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		else{
			SetTestStepResult(EFail);
			}
		}
	else
		{
 		PerformOomTestL();
	    }	

   	return TestStepResult();	
	}
	
		

void CPkixCertStepBase::PerformOomTestL()
	{
 	for (TInt oomCount = 0; ; oomCount++)
 		{
 		__UHEAP_RESET;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
 		TInt countBefore = User::CountAllocCells();
 		TRAPD(error, PerformTestL());// ----> This is the actual test that runs under OOM conditions.
 		TInt countAfter = User::CountAllocCells();
 		
 		if (countBefore != countAfter)
 			{
 			INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
 			SetTestStepResult(EFail);  
 			break;
 			}
 		INFO_PRINTF2(_L("Result: %d"), error); 			
 		if (error == KErrNone)
 			{
			INFO_PRINTF1(_L("Test outcome : Passed"));
			SetTestStepResult(EPass); 
 			break;
 			}
		}	 	
	}
