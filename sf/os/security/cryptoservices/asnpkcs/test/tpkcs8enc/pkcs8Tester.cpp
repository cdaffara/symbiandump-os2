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
* Test program for PKCS8 Encoding
*
*/


/**
 @file
*/

#include <e32test.h>
#include <e32std.h>
#include <U32STD.H>
#include <asn1enc.h>
#include <asnpkcs.h>
#include <pbe.h>
#include <pbebase.h>
#include <pbedata.h>
#include <random.h>
#include <asymmetrickeys.h> 
#include <bigint.h>
#include <securityerr.h>
  
#include "pkcs8tester.h"

const TInt KErrEmptyKeyFile = -99990;

/**
 * Extracts the nPos command line argument.
 */
 HBufC* Pkcs8Utils::GetArgumentL(TInt nPos)
        {
		HBufC *argv = HBufC::NewLC(User::CommandLineLength());
		TPtr cmd(argv->Des());
		User::CommandLine(cmd);
        TLex arguments(cmd);

        // finds nth parameter
        while(nPos && !arguments.Eos())
                {
                TPtrC token = arguments.NextToken();
                if(token.Length() > 0)
                        nPos--;
                }

        HBufC* result = NULL;
        if(!arguments.Eos())
                {
                TPtrC testfile(arguments.NextToken());

                if(testfile.Length() > 0)
                        result = testfile.AllocL();
                };

        // no parameter found, but must return something so..
        if(!result)
                result = HBufC::NewL(0);

        CleanupStack::PopAndDestroy(argv);

        return result;
        }
        
 CDecPKCS8Data* Pkcs8Utils::getPkcsDataL(TDesC& fileName)
	{
	// First import a PKCS8 private key from file
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	RFile file;
	CleanupClosePushL(file);	
	TInt r = file.Open(fs, fileName, EFileRead);
	User::LeaveIfError(r);
        
	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	if (fileSize <= 0)
	{
		User::Leave(KErrEmptyKeyFile);
	}

	HBufC8* buf = HBufC8::NewMaxLC(fileSize);	
	TPtr8 data(buf->Des());
	data.FillZ();
	User::LeaveIfError(file.Read(data, fileSize));
        
	CDecPKCS8Data* decoded = TASN1DecPKCS8::DecodeDERL(buf->Des());	

	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fs);
	//CleanupStack::PopAndDestroy(fileName);
	
	return decoded;
	}       
 
 void Pkcs8Utils::WriteSequenceL(const TDesC& fn, CASN1EncSequence& seq)
{
	HBufC8* seqBufc = HBufC8::NewLC(seq.LengthDER());
	TPtr8 seqBuf(seqBufc->Des());
	seqBuf.SetLength(seq.LengthDER());
	TUint pos = 0;
	
	seq.WriteDERL(seqBuf, pos);


	RFs fs2;
	CleanupClosePushL(fs2);
	User::LeaveIfError(fs2.Connect());

	RFile file2;
	CleanupClosePushL(file2);	
	TRAPD(err, file2.Create(fs2, fn, EFileRead));
	if (err==KErrNotFound) 
	{
		User::LeaveIfError(err);
	}
	file2.Close();		
		
	RFileWriteStream writer;
	writer.PushL();
 
    	User::LeaveIfError(writer.Open(fs2, fn, EFileWrite));  
	
	writer.WriteL(seqBuf);
	writer.Close();

	CleanupStack::PopAndDestroy(4, seqBufc);		
}


        
LOCAL_D void DoTestsDSAEncryptedL(CDecPKCS8Data& decoded)
	{
	CPKCS8KeyPairDSA* keypair = static_cast<CPKCS8KeyPairDSA*>(decoded.KeyPairData());

	const CDSAPrivateKey& priv = keypair->PrivateKey();
	
	TPBPassword pass(_L("")); 
	
	CSystemRandom* rand = CSystemRandom::NewLC();
	HBufC8* saltc = HBufC8::NewMaxLC(16);
	TPtr8 salt(saltc->Des());
	salt.FillZ(); 
	TRAPD(err, rand->GenerateBytesL(salt));
	if((err != KErrNone) && (err != KErrNotSecure))
		User::Leave(err);

	HBufC8* ivc = HBufC8::NewMaxLC(8);
	
	TPtr8 iv(ivc->Des());
	iv.FillZ(); 
	//rand->GenerateBytesL(iv);
	
	CPBEncryptParms* params = CPBEncryptParms::NewLC(ECipher3DES_CBC, salt, iv, 2048);
	CPBEncryptElement* encryptElement = CPBEncryptElement::NewLC(pass, *params);
	CPBEncryptor* encryptor = encryptElement->NewEncryptLC();

	CASN1EncSequence* seq =  TASN1EncPKCS8::EncodeEncryptedL(priv, *encryptor, *params);
	
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(encryptElement);
	CleanupStack::PopAndDestroy(params);
	CleanupStack::PopAndDestroy(ivc);
	CleanupStack::PopAndDestroy(saltc);
	CleanupStack::PopAndDestroy(rand);	

	CleanupStack::PushL(seq);
	_LIT(fn, "ExportedDSA_Encrypted.txt");
	Pkcs8Utils::WriteSequenceL(fn, *seq);
	CleanupStack::PopAndDestroy(seq);	
	}


LOCAL_D void DoTestsDSAL(CDecPKCS8Data& decoded)
{
	CPKCS8KeyPairDSA* keypair = static_cast<CPKCS8KeyPairDSA*>(decoded.KeyPairData());

	const CDSAPrivateKey& priv = keypair->PrivateKey();
	
	// Now Export it
	CASN1EncSequence* seq =  TASN1EncPKCS8::EncodeL(priv);

	CleanupStack::PushL(seq);
	Pkcs8Utils::WriteSequenceL(_L("ExportedDSA.txt"), *seq);
	CleanupStack::PopAndDestroy(seq);	
}
	
LOCAL_D void DoTestsRSAL(CDecPKCS8Data& decoded)
{
	CPKCS8KeyPairRSA* keypair = static_cast<CPKCS8KeyPairRSA*>(decoded.KeyPairData());

	const CRSAPrivateKeyCRT& priv = static_cast<const CRSAPrivateKeyCRT&>(keypair->PrivateKey());

	const CRSAPublicKey& pub = keypair->PublicKey();
	
	// Now Export it
	CASN1EncSequence* seq =  TASN1EncPKCS8::EncodeL(priv, pub);

	CleanupStack::PushL(seq);
	Pkcs8Utils::WriteSequenceL(_L("ExportedRSA.txt"), *seq);
	CleanupStack::PopAndDestroy(seq);	
}


LOCAL_D void DoTestsRSAEncryptedL(CDecPKCS8Data& decoded)
{
	CPKCS8KeyPairRSA* keypair = static_cast<CPKCS8KeyPairRSA*>(decoded.KeyPairData());

	const CRSAPrivateKeyCRT& priv = static_cast<const CRSAPrivateKeyCRT&>(keypair->PrivateKey());
	const CRSAPublicKey& pub = keypair->PublicKey();
	
	TPBPassword pass(_L("")); 
	
	CSystemRandom* rand = CSystemRandom::NewLC();
	HBufC8* saltc = HBufC8::NewMaxLC(16);
	TPtr8 salt(saltc->Des());
	salt.FillZ(); 
	TRAPD(err, rand->GenerateBytesL(salt));
	if((err != KErrNone) && (err != KErrNotSecure))
		User::Leave(err);

	HBufC8* ivc = HBufC8::NewMaxLC(8);
	
	TPtr8 iv(ivc->Des());
	iv.FillZ(); 
	TRAP(err, rand->GenerateBytesL(iv));
	if((err != KErrNone) && (err != KErrNotSecure))
		User::Leave(err);
	
	CPBEncryptParms* params = CPBEncryptParms::NewLC(ECipher3DES_CBC, salt, iv, 2048);
	CPBEncryptElement* encryptElement = CPBEncryptElement::NewLC(pass, *params);
	CPBEncryptor* encryptor = encryptElement->NewEncryptLC();

	CASN1EncSequence* seq =  TASN1EncPKCS8::EncodeEncryptedL(priv, pub, *encryptor, *params);
	
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(encryptElement);
	CleanupStack::PopAndDestroy(params);
	CleanupStack::PopAndDestroy(ivc);
	CleanupStack::PopAndDestroy(saltc);
	CleanupStack::PopAndDestroy(rand);	

	CleanupStack::PushL(seq);
	_LIT(fn, "ExportedRSA_Encrypted.txt");
	Pkcs8Utils::WriteSequenceL(fn, *seq);
	CleanupStack::PopAndDestroy(seq);	
}


/**
 * 1st parameter dsa cleartext key
 * 2nd parameter dsa encrypted key
 * 3rd parameter rsa cleartext key
 * 4th parameter rsa encrypted key
 */	
LOCAL_D void DoTestsL()
{
	HBufC* fileName = Pkcs8Utils::GetArgumentL(0);
	CleanupStack::PushL(fileName);
	
	CDecPKCS8Data* decoded = Pkcs8Utils::getPkcsDataL(*fileName);
	CleanupStack::PopAndDestroy(fileName);	

	CleanupStack::PushL(decoded);
	DoTestsDSAL(*decoded);

	DoTestsDSAEncryptedL(*decoded);
	CleanupStack::PopAndDestroy(decoded);	

	 fileName = Pkcs8Utils::GetArgumentL(1);
	CleanupStack::PushL(fileName);	

	decoded = Pkcs8Utils::getPkcsDataL(*fileName);
	CleanupStack::PopAndDestroy(fileName);	

	CleanupStack::PushL(decoded);
	DoTestsRSAL(*decoded);

	DoTestsRSAEncryptedL(*decoded);
	CleanupStack::PopAndDestroy(decoded);	
}

LOCAL_C void  ProcessL()
	{
	HBufC* arg = Pkcs8Utils::GetArgumentL(0);
	_LIT(attr, "-attribute");
	if (arg->Find(attr)!=KErrNotFound)
		{
		TRAPD(err, GenerateAndSaveL(*Pkcs8Utils::GetArgumentL(1), *Pkcs8Utils::GetArgumentL(2)));		
		}
	else 
		{
		TRAPD(err, DoTestsL());		
		}
	delete arg;	
	}



/**
* Command line parameters are
* [0] -attribute [1] pkcs8 original file name [2] output file name
* or
* ...
*/
GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	
	TRAPD(err, ProcessL());
	
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }
