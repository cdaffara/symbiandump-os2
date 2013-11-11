/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CorruptionTest.h"
#include "wtlscert.h"
#include "tcertutils.h"
#include "t_output.h"
#include "t_input.h"
#include <random.h>

#include <asymmetric.h>

#include <bigint.h>
#include <hash.h>
#include <securityerr.h>

TInt CCorruptionTest::nInstances = 0;
_LIT(KCorruptLogFile, "WTLSCERTCorruptLog.txt");
_LIT(KPathStart, "<path>");
_LIT(KIterationsStart, "<iterations>");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestAction* CCorruptionTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CCorruptionTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CCorruptionTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CCorruptionTest* self = new(ELeave) CCorruptionTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CCorruptionTest::CCorruptionTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	nFileNumber = 0;
	}

CCorruptionTest::~CCorruptionTest(void)
	{
	delete iDirList;
	delete iWriter;
	iLogFile.Close();
	delete iNullOut;
	delete iCorruptOut;
	};

void CCorruptionTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	if(nInstances==0)
		{
		nInstances++;
		TInt pos=0;
		HBufC* body = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
		iNullOut = new(ELeave) NullOutput;
		body->Des().Copy(aTestActionSpec.iActionBody);
		TPtrC chainBuf = Input::ParseElement(*body, KPathStart, pos);
		iPath.Copy(chainBuf);
		TPtrC itertaionBuf = Input::ParseElement(*body, KIterationsStart, pos);
		TLex itLex(itertaionBuf);

		itLex.Val(iIterations);
		CleanupStack::PopAndDestroy();
		User::LeaveIfError(iLogFile.Replace(iFs,KCorruptLogFile,EFileWrite));
		iCorruptOut = new(ELeave) FileOutput(iLogFile);
		iWriter = new(ELeave) WTLSCertWriter(iCorruptOut);
		}
	else
		{
		SetScriptError(ESyntax, _L("Only one corruption test can be run in each script"));
		iFinished = ETrue;
		}
	}


void CCorruptionTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	HBufC *searchPath = HBufC::NewLC(iPath.Size() + 1);
	TPtr searchPathPtr(searchPath->Des());
	searchPathPtr.Copy(iPath);
	searchPathPtr.Append(_L("*"));
    TInt err = iFs.GetDir(searchPathPtr, KEntryAttMaskSupported, ESortByName, iDirList);
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error getting directory "));
		iConsole.Printf(searchPathPtr);
		iConsole.Printf(_L("\n"));
		iOut.writeString(_L("Error getting directory "));
		iOut.writeString(searchPathPtr);
		iOut.writeNewLine();
		iFinished = ETrue;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		SetScriptError(EFileNotFound, searchPathPtr);
		}
	else
		{
		iConsole.Printf(_L("Please view "));
		iConsole.Printf(KCorruptLogFile);
		iConsole.Printf(_L(" for results\n"));
		iOut.writeString(_L("Please view "));
		iOut.writeString(KCorruptLogFile);
		iOut.writeString(_L(" for results"));
		iOut.writeNewLine();
		iActionState = EAction;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		iResult = ETrue;
		}
	CleanupStack::PopAndDestroy();
	}

void CCorruptionTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}



void CCorruptionTest::RunCorruptionTestL(const TDesC &aFilename)
	{
	CSystemRandom *random = CSystemRandom::NewLC();
	iWriter->SetOut(iNullOut);
	for (TInt i = 0; i < iIterations; i++)
		{
		HBufC8* buf = Input::ReadFileLC(aFilename, iPath, iFs);
		TPtr8 pBuf = buf->Des();
		TInt len = pBuf.Size();
		TInt pos = 0;
				//list of positions altered
		HBufC* posOctsBuf = HBufC::NewLC(70);
		TPtr pPosOctsBuf = posOctsBuf->Des();
		pPosOctsBuf.SetLength(0);
		pPosOctsBuf.Append(_L("p:"));
				//list of octets altered
		HBufC8* octsBuf = HBufC8::NewLC(40);
		TPtr8 pOctsBuf = octsBuf->Des();
		pOctsBuf.SetLength(0);

		for (TInt j = 0; j < 10; j++)
			{
				//randomness
			HBufC8* rand = HBufC8::NewLC(5);
			TPtr8 pRand = rand->Des();
			pRand.SetLength(5);
			TRAPD(err, random->GenerateBytesL(pRand));
			if((err != KErrNone) && (err != KErrNotSecure))
				User::Leave(err);
			TUint num = 0;
			for (TInt k = 0; k < 4 ; k++)
				{
				num<<=8;
				num+=pRand[k];
				}
			pos = num % len;
			TUint8 newOctet = pRand[4];
				//update output lines
			pPosOctsBuf.AppendNum(pos);
			pPosOctsBuf.Append(_L(" "));
			pOctsBuf.Append(pBuf.Mid(pos, 1));
			TPtrC8 pNewOct = pRand.Right(1);
			pOctsBuf.Append(pNewOct);
			//switch the selected octet
			pBuf[pos] = newOctet;
			//file out
			iCorruptOut->writeString(pPosOctsBuf);
			iCorruptOut->writeNewLine();
			iCorruptOut->writeString(_L("o:"));
			iCorruptOut->writeOctetString(pOctsBuf);
			iCorruptOut->writeNewLine();
			//try to make corrupt cert
			CWTLSCertificate* cert = NULL;
			TRAP(err, cert = CWTLSCertificate::NewL(pBuf));
			CleanupStack::PushL(cert);
			if (err == KErrNone)
				{
				TRAP_IGNORE(iWriter->WriteCert(*cert));
				TBool res = EFalse;;
				
			//	Don't bother attempting to verify if public key modulus is an even
			//	value since CMontgomery methods will panic
						
				CRSAPublicKey* key = CWTLSRSAPublicKey::NewL(cert->PublicKey().KeyData());
				const TInteger& theN = key->N();
				if (theN.IsOdd())
					{//	Check that the signature is an appropriate value for the key
					//	Otherwise the crypto library will panic (descriptor out of bounds)
					RInteger input = RInteger::NewL(cert->Signature());
					CleanupStack::PushL(input);
					if ( (input < theN) && (input > 0) )
						{
						TRAP_IGNORE(res = cert->VerifySignatureL(cert->PublicKey().KeyData()));
						if (res)
							{
							iCorruptOut->writeString(_L("!!!"));
							iCorruptOut->writeNewLine();
							}
						}
					
					CleanupStack::PopAndDestroy(&input);
					}

					delete key;
				}

			iCorruptOut->writeString(_L("r:"));
			iCorruptOut->writeNum(err);
			iCorruptOut->writeNewLine();
			CleanupStack::PopAndDestroy(2);//cert, randomness
			}
		CleanupStack::PopAndDestroy(3);//
		}
	CleanupStack::PopAndDestroy(1); //random
	iWriter->SetOut(iCorruptOut);
	}

void CCorruptionTest::PerformAction(TRequestStatus& aStatus)
	{
	TBuf<256> filename = (*iDirList)[nFileNumber].iName;
	CSHA1* hash = NULL;
	TRAP_IGNORE(hash = CSHA1::NewL());
	iCorruptOut->writeString(_L("file:"));
	iCorruptOut->writeString(filename);
	iCorruptOut->writeNewLine();
	iConsole.Printf(_L("file:%S\n"), &filename);

	TRAP_IGNORE(RunCorruptionTestL(filename));

	nFileNumber++;
	if(nFileNumber == iDirList->Count())
		{
		iActionState = EPostrequisite;
		};
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	delete hash;
	}

void CCorruptionTest::DoReportAction()
	{
	}

void CCorruptionTest::DoCheckResult(TInt /*aError*/)
	{
	}

