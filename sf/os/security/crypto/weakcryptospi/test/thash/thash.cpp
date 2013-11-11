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

#include "hashtestutils.h"
#include <utf.h>

TBool gInOOMTest=EFalse;

RTest test(_L("Hash Tests"));

const TInt KMaxHashSize = 64; // Hash size in bytes

void Hex(HBufC8& aString)
	{
	TPtr8 ptr=aString.Des();
	if (aString.Length()%2)
		{
		ptr.SetLength(0);
		return;
		}
	TInt i;
	for (i=0;i<aString.Length();i+=2)
		{
		TUint8 tmp;
		tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
		tmp*=16;
		tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
		ptr[i/2]=tmp;
		}
	ptr.SetLength(aString.Length()/2);
	}

void Spin()
{//	Pointless function to print a dot
	if (gInOOMTest)
		{
		static TInt count=0;
		if (count++==100)
			{
			test.Printf(_L("o"));
			count=0;
			}
		return;
		}
	test.Printf(_L("."));
}

void FunctionalityTestL(CMessageDigest* aMD)
	{
	if (!gInOOMTest)
		test.Next(_L("Functionality test - original API"));

	const TInt maxbuffersize=1024;
	TInt buffersize;
	TInt increment;
	if (gInOOMTest)
		{
		buffersize=256;
		increment=6;
		}
	else
		{
		buffersize=maxbuffersize;
		increment=1;
		}

	TBool testSuccess = ETrue;
	TInt i = 0;
	TBuf8<maxbuffersize> buf(maxbuffersize);
	for (;i<buffersize;i++)
		{
		buf[i]=(TUint8)i;
		}
	
	for (i=0;i<buffersize;i+=increment)
		{
		TInt j;
		Spin();
		if (!gInOOMTest)
			{
			if (i>128)
				{
				increment=8;
				}
			}
		else
			{
			if (i>24)
				{
				increment=32;
				}
			}

		for (j=0;j<i;j+=16)
			{
			buf[0]=(TUint8)j;
			TPtrC8 ptr=buf.Left(i);
			TPtrC8 ptr2=buf.Left(j);
			TPtrC8 ptr3=buf.Mid(j,i-j);

			CMessageDigest* first=aMD->ReplicateL();
			CleanupStack::PushL(first);
			TPtrC8 firstFinal = first->Hash(ptr);
			
			aMD->Reset();
			aMD->Update(ptr);
			TPtrC8 aMDFinal = aMD->Final();

			CMessageDigest* second=aMD->ReplicateL();
			CleanupStack::PushL(second);
			second->Hash(ptr2);
			
			CMessageDigest* third=second->CopyL();
			CleanupStack::PushL(third);
			
			TPtrC8 secondFinal = second->Hash(ptr3);
			
			if (aMDFinal!=firstFinal)
				{
				testSuccess = EFalse;
				}
			
			if (firstFinal!=secondFinal)
				{
				testSuccess = EFalse;
				}

			TPtrC8 thirdFinal = third->Hash(ptr3);
			if (firstFinal!=thirdFinal)
				{
				testSuccess = EFalse;
				}
			CleanupStack::PopAndDestroy(3);		// first, second, third
			}
		}
	
///////////////////////////////////////////////////////////////
//	Now test the new API - Update/Final functions	
///////////////////////////////////////////////////////////////
	if (!gInOOMTest)
		test.Next(_L("\rFunctionality test - calls added API functions Final & Update"));

	for (i=0;i<buffersize;i++)
		{
		buf[i]=(TUint8)i;
		}
	
	for (i=0;i<buffersize;i+=increment)
		{
		TInt j;
		Spin();
		if (!gInOOMTest)
			{
			if (i>128)
				{
				increment=8;
				}
			}
		else
			{
			if (i>24)
				{
				increment=32;
				}
			}
		for (j=0;j<i;j+=16)
			{
			buf[0]=(TUint8)j;
			TPtrC8 ptr=buf.Left(i);
			TPtrC8 ptr2=buf.Left(j);
			TPtrC8 ptr3=buf.Mid(j,i-j);
			
			CMessageDigest* first=aMD->ReplicateL();
			CleanupStack::PushL(first);
			first->Update(ptr);
			
			aMD->Update(ptr);

			CMessageDigest* second=aMD->ReplicateL();
			CleanupStack::PushL(second);			
			second->Update(ptr2);
			
			CMessageDigest* third=second->CopyL();
			CleanupStack::PushL(third);
			third->Update(ptr3);
			
			second->Update(ptr3);
			
			TPtrC8 aMDFinal = aMD->Final();
			TPtrC8 firstFinal = first->Final();
			TPtrC8 secondFinal = second->Final();
			TPtrC8 thirdFinal = third->Final();
			
			if (aMDFinal!=firstFinal)
				{
				testSuccess = EFalse;
				}

			if (firstFinal!=secondFinal)
				{
				testSuccess = EFalse;
				}
			
			if (firstFinal!=thirdFinal)
				{
				testSuccess = EFalse;
				}

			CleanupStack::PopAndDestroy(3);		// first, second, third
			}
		}
	

	test.Printf(_L("\r\n"));
	
	if (!testSuccess)
		User::Leave(KErrGeneral);
	}

void VectorTestL(CMessageDigest* aMD,const TDesC& aFilename)
	{
	test.Next(_L("Test Vector tests - original API"));
	TBool finished=EFalse;
	TBool testSuccess = ETrue;
	//aFilename is a .dat file which contains the message and the corresponding digest
	CTestData* data = CTestData::NewL(aFilename);
	CleanupStack::PushL(data);

	while (!finished)
		{
		switch (data->Type())
			{
		case CTestData::EMessage:
			{
			break;
			}
		//This is added to read large input data from the files.
		case CTestData::EFileName:
			{
			CMessageDigest* md=aMD->ReplicateL();
			CleanupStack::PushL(md);
			
			//get the filename from the .dat file
			HBufC8* filename = (*data)[1];
			User::LeaveIfNull(filename);
			CleanupStack::PushL(filename);
			
			HBufC8* output= (*data)[2];
			User::LeaveIfNull(output);
			CleanupStack::PushL(output);
			Hex(*output);
			
			HBufC16* inputFileName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*filename);
			
			RFs fs;
			RFile file;
			CleanupClosePushL(fs);
			User::LeaveIfError(fs.Connect());
			TDriveUnit sysDrive(fs.GetSystemDrive());
			TBuf<24> filePath (sysDrive.Name());
			filePath.Append(_L("\\thash\\"));
			User::LeaveIfError(fs.SetSessionPath(filePath));
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(fs,*inputFileName,EFileShareAny|EFileRead));
			// read into iFile
			TInt size=0;
			file.Size(size);
						
			HBufC8* fileContents=HBufC8::NewMaxL(size);
			
			TPtr8 ptr=fileContents->Des();
			User::LeaveIfError(file.Read(ptr));
			CleanupStack::PopAndDestroy(2, &fs);
			CleanupStack::PushL(fileContents);
			delete inputFileName;
			
			TPtrC8 digest = md->Hash(*fileContents);
			if (digest!=(*output))	
				{
				testSuccess = EFalse;
				}
		
			md->Reset();

			TPtrC8 digest2 = md->Hash(*fileContents);
			if (digest2!=(*output))	
				{
				testSuccess = EFalse;
				}

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
			
			TInt inputLen = fileContents->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = fileContents->Left(leftHandLen);
				TPtrC8 right = fileContents->Right(inputLen - leftHandLen);

				TPtrC8 halfDigest = md->Hash(left);
				if (halfDigest.Size()==0)	//	Unnecessary test, but removes
					User::Leave(KErrAbort);	//	a warning about not using halfDigest
					
				TPtrC8 wholeDigest = md->Hash(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}			
				}
				
			md->Reset();
			if (md->Final(*fileContents)!=(*output))
				{
				testSuccess = EFalse;
				}

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
			inputLen = fileContents->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = fileContents->Left(leftHandLen);
				TPtrC8 right = fileContents->Right(inputLen - leftHandLen);

				md->Update(left);
				TPtrC8 wholeDigest = md->Final(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}			
				}
			CleanupStack::PopAndDestroy(4, md);//md,filename,output,fileContents
	
			break;
			}
		case CTestData::EData:
			{
			CMessageDigest* md=aMD->ReplicateL();
			CleanupStack::PushL(md);
			
			HBufC8* input = (*data)[0];
			User::LeaveIfNull(input);
		    CleanupStack::PushL(input);
			
			HBufC8* output= (*data)[1];
			User::LeaveIfNull(output);
			CleanupStack::PushL(output);
			Hex(*input);
			Hex(*output);
            
            //call the Hash function of the Algorithm.
			TPtrC8 digest = md->Hash(*input);
			//Compare the digest generated with the output(digest) expected.
			if (digest!=(*output))	
				{
				testSuccess = EFalse;
				}
		
			//check the Reset() function.
			md->Reset();

			TPtrC8 digest2 = md->Hash(*input);
			if (digest2!=(*output))	
				{
				testSuccess = EFalse;
				}

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
			
			TInt inputLen = input->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = input->Left(leftHandLen);
				TPtrC8 right = input->Right(inputLen - leftHandLen);

				TPtrC8 halfDigest = md->Hash(left);
				if (halfDigest.Size()==0)	//	Unnecessary test, but removes
					User::Leave(KErrAbort);	//	a warning about not using halfDigest
					
				TPtrC8 wholeDigest = md->Hash(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}			
				}

///////////////////////////////////////////////////////////////
//	Now test the new API - Update/Final functions	
///////////////////////////////////////////////////////////////
			md->Reset();
			if (md->Final(*input)!=(*output))
				{
				testSuccess = EFalse;
				}

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
			inputLen = input->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = input->Left(leftHandLen);
				TPtrC8 right = input->Right(inputLen - leftHandLen);

				md->Update(left);
				TPtrC8 wholeDigest = md->Final(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}			
				}
			CleanupStack::PopAndDestroy(3);
			break;
			}
		case CTestData::EFinished:
			finished=ETrue;
			break;
		default:
			test.Printf(_L("Error in data file\r\n"));
			break;
			}
		};

	CleanupStack::PopAndDestroy(data);
	
	if (!testSuccess)
		User::Leave(KErrGeneral);
	}

void OOMTestL(CMessageDigest* aMD)
	{
	test.Next(_L("Out of memory test"));
	TInt err = KErrNoMemory;
	TInt nextFailure=0;
	gInOOMTest=ETrue;
	while (err!=KErrNone)
		{
		__UHEAP_MARK;
		__UHEAP_FAILNEXT(nextFailure);
		TRAP(err,FunctionalityTestL(aMD));		
		__UHEAP_MARKEND;
		nextFailure++;
		}
	__UHEAP_RESET;
	gInOOMTest=EFalse;
	}

void HMACVectorTestL(CMessageDigest* aMD,const TDesC& aFilename)
	{
	test.Next(_L("HMAC Test Vector tests"));
	CTestData* data = CTestData::NewL(aFilename);
	CleanupStack::PushL(data);

	TBool finished=EFalse;
	TBool testSuccess = ETrue;
	
	while (!finished)
		{
		switch (data->Type())
			{
		case CTestData::EMessage:
			{
			break;
			}
		case CTestData::EFileName:
			{
			//get the filename from the .dat file
			HBufC8* filename = (*data)[1];
			User::LeaveIfNull(filename);
			CleanupStack::PushL(filename);
			
			HBufC8* key= (*data)[2];
			User::LeaveIfNull(key);
			CleanupStack::PushL(key);
						
			HBufC16* inputFileName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*filename);
			
			HBufC8* output = (*data)[3];
			User::LeaveIfNull(output);
			CleanupStack::PushL(output);
			
			Hex(*key);
			Hex(*output);
			
			RFs fs;
			RFile file;
			CleanupClosePushL(fs);
			User::LeaveIfError(fs.Connect());
			TDriveUnit sysDrive(fs.GetSystemDrive());
			TBuf<24> filePath (sysDrive.Name());
			filePath.Append(_L("\\thash\\"));
			User::LeaveIfError(fs.SetSessionPath(filePath));
			CleanupClosePushL(file);
			User::LeaveIfError(file.Open(fs,*inputFileName,EFileShareAny|EFileRead));
			// read into iFile
			TInt size=0;
			file.Size(size);
			HBufC8* fileContents=HBufC8::NewMaxL(size);
			
			TPtr8 ptr=fileContents->Des();
			User::LeaveIfError(file.Read(ptr));
			CleanupStack::PopAndDestroy(2, &fs);
			CleanupStack::PushL(fileContents);
			delete inputFileName;
			
			CMessageDigest* temp = aMD->ReplicateL();
			CleanupStack::PushL(temp);
			CMessageDigest* md = CHMAC::NewL(*key, temp);
			CleanupStack::Pop(temp);	//	Now owned by md
			
			TPtrC8 digest = md->Hash(*fileContents);
			if (digest!=(*output))
				{
				testSuccess = EFalse;
				}
						
		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
		
			TInt inputLen = fileContents->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = fileContents->Left(leftHandLen);
				TPtrC8 right = fileContents->Right(inputLen - leftHandLen);

				TPtrC8 halfDigest = md->Hash(left);
				if (halfDigest.Size()==0)	//	Unnecessary test, but removes
					User::Leave(KErrAbort);	//	a warning about not using halfDigest
				TPtrC8 wholeDigest = md->Hash(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}
				}

			md->Reset();
			TPtrC8 finalDigest = md->Final(*fileContents);
			if (finalDigest!=(*output))
				{
				testSuccess = EFalse;
				}			

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
		
			inputLen = fileContents->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = fileContents->Left(leftHandLen);
				TPtrC8 right = fileContents->Right(inputLen - leftHandLen);

				md->Update(left);
				TPtrC8 wholeDigest = md->Final(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}
				}
			delete md;
			CleanupStack::PopAndDestroy(4, filename);	//	filename, key, output,fileContents			
			break;
			}
		case CTestData::EData:
			{
			HBufC8* input = (*data)[0];
			User::LeaveIfNull(input);
			CleanupStack::PushL(input);
			
			HBufC8* key = (*data)[1];
			User::LeaveIfNull(key);
			CleanupStack::PushL(key);
			
			HBufC8* output = (*data)[2];
			User::LeaveIfNull(output);
			CleanupStack::PushL(output);
			
			Hex(*input);
			Hex(*key);
			Hex(*output);
			
			CMessageDigest* temp = aMD->ReplicateL();
			CleanupStack::PushL(temp);
			CMessageDigest* md = CHMAC::NewL(*key, temp);
			CleanupStack::Pop(temp);	//	Now owned by md
			
			TPtrC8 digest = md->Hash(*input);
			if (digest!=(*output))
				{
				testSuccess = EFalse;
				}
						
		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
		
			TInt inputLen = input->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = input->Left(leftHandLen);
				TPtrC8 right = input->Right(inputLen - leftHandLen);

				TPtrC8 halfDigest = md->Hash(left);
				if (halfDigest.Size()==0)	//	Unnecessary test, but removes
					User::Leave(KErrAbort);	//	a warning about not using halfDigest
				TPtrC8 wholeDigest = md->Hash(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}
				}

///////////////////////////////////////////////////////////////
//	Now test the new API - Update/Final functions	
///////////////////////////////////////////////////////////////
			md->Reset();
			TPtrC8 finalDigest = md->Final(*input);
			if (finalDigest!=(*output))
				{
				testSuccess = EFalse;
				}			

		//	Now try this in 2 half sections (just a quick check)
			md->Reset();
		
			inputLen = input->Length();
			if (inputLen > 1)
				{
				TInt leftHandLen = inputLen/2;
				TPtrC8 left = input->Left(leftHandLen);
				TPtrC8 right = input->Right(inputLen - leftHandLen);

				md->Update(left);
				TPtrC8 wholeDigest = md->Final(right);
				
				if (wholeDigest!=(*output))	
					{
					testSuccess = EFalse;
					}
				}
			delete md;
			CleanupStack::PopAndDestroy(3);	//	input, key, output			
			break;
			}
		case CTestData::EFinished:
			finished=ETrue;
			break;
		default:
			test.Printf(_L("Error in data file\r\n"));
			break;
			}
		};

	CleanupStack::PopAndDestroy(data);
	
	if (!testSuccess)
		User::Leave(KErrGeneral);
	}

void HMACTestsL(CMessageDigest* aMD,const TDesC& aFilename, const TDesC& aHashType)
	{
	TBuf<0x40> formattable;
	formattable.Format(_L("HMAC Tests for %S"), &aHashType);
	test.Next(formattable);
	CMessageDigest* temp = aMD->ReplicateL();
	CleanupStack::PushL(temp);
	CHMAC* hmac=CHMAC::NewL(_L8("aaaaaaaa"), temp);
	CleanupStack::Pop(temp);
	CleanupStack::PushL(hmac);
	
//	For each of the available digests
	FunctionalityTestL(hmac);		//JCS for now
	HMACVectorTestL(aMD,aFilename);	
	OOMTestL(hmac);
		
	CleanupStack::PopAndDestroy(hmac);
	}

void MD2TestsL()
	{
	CMD2* md2;
	md2=CMD2::NewL();
	CleanupStack::PushL(md2);

	FunctionalityTestL(md2);
	
	VectorTestL(md2,_L("md2.dat"));
	
	OOMTestL(md2);
		
	CleanupStack::PopAndDestroy(md2);

// Problem reported by Jal Panvel, 17-12-1999.
// Report by email, CSHA1::Hash() returning zero length descriptor
// 
// This was caused by failure to set internal hash descriptor length on setup, this
// problem was present in all hashes except MD2 which set it up correctly.
// Fixed 17-12-1999.
	test.Next(_L("Fixed bugs Tests"));
	md2 = CMD2::NewL();
	CleanupStack::PushL(md2);
	TPtrC8 data(_L8("The quick brown fox jumped over the lazy dog"));
	TBuf8<128> hash;
	hash = md2->Hash(data);
	test(hash.Length() == md2->HashSize());
	
	HMACTestsL(md2,_L("hmacmd2.dat"), _L("md2"));
	CleanupStack::PopAndDestroy(md2);
	}

void MD5TestsL()
	{
	CMD5* md5;
	md5=CMD5::NewL();
	CleanupStack::PushL(md5);

	FunctionalityTestL(md5);
	
	VectorTestL(md5,_L("md5.dat"));
	
	OOMTestL(md5);
	
	CleanupStack::PopAndDestroy(md5);
	test.Next(_L("Fixed bugs Tests"));
// Problem reported by Jal Panvel, 17-12-1999.
// Report by email, CSHA1::Hash() returning zero length descriptor
// 
// This was caused by failure to set internal hash descriptor length on setup, this
// problem was present in all hashes except MD2 which set it up correctly.
// Fixed 17-12-1999.
	CMD5* md = CMD5::NewL();
	CleanupStack::PushL(md);
	TPtrC8 data(_L8("The quick brown fox jumped over the lazy dog"));
	TBuf8<128> hash;
	hash = md->Hash(data);
	test(hash.Length() == md->HashSize());
		
	HMACTestsL(md,_L("hmacmd5.dat"), _L("md5"));
	CleanupStack::PopAndDestroy(md);

	//	Test for DEF001510 "TLS - Receives Disconnect Indication during hands..."
	CMD5* testHasher = CMD5::NewL();
	CleanupStack::PushL(testHasher);
	TPtrC8 client(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A4"));
	TPtrC8 server(_L8("3E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));

	HBufC8* clientData = client.AllocLC();
	HBufC8* serverData = server.AllocLC();

	Hex(*clientData);
	Hex(*serverData);
	
	testHasher->Hash(*clientData);
	testHasher->Hash(*serverData);
	
	TBuf8<32> md5buf;
	md5buf.Copy(testHasher->Hash(TPtrC8(0,0)));

	testHasher->Reset();

//	Now hash in one chunk
	TPtrC8 all(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A43E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));
	HBufC8* allData = all.AllocLC();
	Hex(*allData);

	TBuf8<32> allbuf;
	allbuf = testHasher->Hash(*allData);
	test(allbuf.Compare(md5buf)==0);
	CleanupStack::PopAndDestroy(4, testHasher);
	}
	
	
void MD4TestsL()
	{
	CMD4* md4;
	md4=CMD4::NewL();
	CleanupStack::PushL(md4);

	FunctionalityTestL(md4);
	
	VectorTestL(md4,_L("md4.dat"));
	
	OOMTestL(md4);
	
	CleanupStack::PopAndDestroy(md4);

    //Copied the tests carried out for other Digest generating algorithms (MD5, SHA1)
	CMD4* md = CMD4::NewL();
	CleanupStack::PushL(md);
	TPtrC8 data(_L8("The quick brown fox jumped over the lazy dog"));
	TBuf8<128> hash;
	hash = md->Hash(data);
	test(hash.Length() == md->HashSize());
		
	HMACTestsL(md,_L("hmacmd4.dat"), _L("md4"));
	CleanupStack::PopAndDestroy(md);

	CMD4* testHasher = CMD4::NewL();
	CleanupStack::PushL(testHasher);
	TPtrC8 client(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A4"));
	TPtrC8 server(_L8("3E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));

	HBufC8* clientData = client.AllocLC();
	HBufC8* serverData = server.AllocLC();

	Hex(*clientData);
	Hex(*serverData);
	
	testHasher->Hash(*clientData);
	testHasher->Hash(*serverData);
	
	TBuf8<32> md4buf;
	md4buf.Copy(testHasher->Hash(TPtrC8(0,0)));

	testHasher->Reset();

//	Now hash in one chunk
	TPtrC8 all(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A43E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));
	HBufC8* allData = all.AllocLC();
	Hex(*allData);

	TBuf8<32> allbuf;
	allbuf = testHasher->Hash(*allData);
	test(allbuf.Compare(md4buf)==0);
	CleanupStack::PopAndDestroy(4, testHasher);
	
	//Tests for the Factory Method CMessageDigestFactory
 	CMessageDigest* messageDigest = CMessageDigestFactory::NewDigestLC(CMessageDigest::EMD4);
 	VectorTestL(messageDigest,_L("md4.dat"));
  	CleanupStack::PopAndDestroy(messageDigest);

	
	}

void SHA1TestsL()
	{
	CSHA1* sha;
	sha=CSHA1::NewL();
	CleanupStack::PushL(sha);

	VectorTestL(sha,_L("sha1.dat"));
	
	FunctionalityTestL(sha);
	
	OOMTestL(sha);
		
	CleanupStack::PopAndDestroy(sha);

	test.Next(_L("Fixed bugs Tests"));	
// Problem reported by Jal Panvel, 17-12-1999.
// Report by email, CSHA1::Hash() returning zero length descriptor
// 
// This was caused by failure to set internal hash descriptor length on setup, this
// problem was present in all hashes except MD2 which set it up correctly.
// Fixed 17-12-1999.

	sha=CSHA1::NewL();
	CleanupStack::PushL(sha);
	TPtrC8 data(_L8("The quick brown fox jumped over the lazy dog"));
	TBuf8<128> hash;
	hash = sha->Hash(data);
	test(hash.Length() == sha->HashSize());
	sha->Reset();
	
	//	Test for DEF001510 "TLS - Receives Disconnect Indication during hands..."
	CSHA1* testHasher = CSHA1::NewL();
	CleanupStack::PushL(testHasher);
	TPtrC8 client(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A4"));
	TPtrC8 server(_L8("3E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));

	HBufC8* clientData = client.AllocLC();
	HBufC8* serverData = server.AllocLC();

	Hex(*clientData);
	Hex(*serverData);

//	Hash in 2 portions
	TBuf8<32> clientbuf;
	TBuf8<32> serverbuf;
	clientbuf = testHasher->Hash(*clientData);
	serverbuf = testHasher->Hash(*serverData);
	
	TBuf8<32> shabuf;
	shabuf.Copy(testHasher->Hash(TPtrC8(0,0)));
	testHasher->Reset();

//	Now hash in one chunk
	TPtrC8 all(_L8("D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A43E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8"));
	HBufC8* allData = all.AllocLC();
	Hex(*allData);

	TBuf8<32> allbuf;
	allbuf = testHasher->Hash(*allData);

	test(allbuf.Compare(shabuf)==0);
	CleanupStack::PopAndDestroy(4, testHasher);
	
	// Test hashing non-word aligned data - used to crash on arm
	TPtrC8 nonAlignedData = data.Mid(1);
	hash = sha->Final(nonAlignedData);
	test(hash.Length() == sha->HashSize());
	sha->Reset();
	
//	Test end
	HMACTestsL(sha,_L("hmacsha1.dat"), _L("sha1"));
	CleanupStack::PopAndDestroy();		// sha
	}

void ExecuteSha2TestsL(CMessageDigest* aMD, const TDesC& aVector, const TDesC& aHMACVector, const TDesC& aHashType)
	{
	VectorTestL(aMD, aVector);
	FunctionalityTestL(aMD);
	OOMTestL(aMD);
	aMD->Reset();

	test.Next(_L("Fixed bugs Tests"));	

	_LIT8(KTest1Data, "The quick brown fox jumped over the lazy dog");
	TBuf8<KMaxHashSize> hash;
	hash = aMD->Hash(KTest1Data());
	test(hash.Length() == aMD->HashSize());
	aMD->Reset();
	
	_LIT8(KClientData, "D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A4");
	_LIT8(KServerData, "3E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8");
	
	HBufC8* clientData = KClientData().AllocLC();
	HBufC8* serverData = KServerData().AllocLC();

	Hex(*clientData);
	Hex(*serverData);
	
//	Hash in 2 portions
	aMD->Hash(*clientData);
	aMD->Hash(*serverData);

	CleanupStack::PopAndDestroy(2, clientData);

	TBuf8<KMaxHashSize> shabuf;
	shabuf.Copy(aMD->Hash(KNullDesC8()));
	aMD->Reset();

//	Now hash in one chunk
	_LIT8(KAllData, "D652CA1A6154D8303C16C055E424A5ACF3EBAB94284CD9B05B85C0D0F0B8E7A43E3E56059EFEE4F8C5B05C76128C4C84916DF9E935510C3C063454856FF29FF8");
	HBufC8* allData = KAllData().AllocLC();
	Hex(*allData);

	TBuf8<KMaxHashSize> allbuf;
	allbuf = aMD->Hash(*allData);
	
	test(allbuf.Compare(shabuf)==0);
	CleanupStack::PopAndDestroy(allData);
	
	// Test hashing non-word aligned data - used to crash on arm
	TPtrC8 nonAlignedData = KTest1Data().Mid(1);
	hash = aMD->Final(nonAlignedData);
	test(hash.Length() == aMD->HashSize());
	aMD->Reset();
	
//	Test end
	HMACTestsL(aMD, aHMACVector, aHashType);
	}

_LIT(K224Algo, "SHA-224.dat");
_LIT(K256Algo, "SHA-256.dat");
_LIT(K384Algo, "SHA-384.dat");
_LIT(K512Algo, "SHA-512.dat");
_LIT(K224Vector, "sha224.dat");
_LIT(K256Vector, "sha256.dat");
_LIT(K384Vector, "sha384.dat");
_LIT(K512Vector, "sha512.dat");
_LIT(K224HmacVector, "hmacsha224.dat");
_LIT(K256HmacVector, "hmacsha256.dat");
_LIT(K384HmacVector, "hmacsha384.dat");
_LIT(K512HmacVector, "hmacsha512.dat");

const TDesC* gNames[] = 
	{
	&K224Vector(),
	&K224HmacVector(),
	&K224Algo(),
	&K256Vector(),
	&K256HmacVector(),
	&K256Algo(),
	&K384Vector(),
	&K384HmacVector(),
	&K384Algo(),
	&K512Vector(),
	&K512HmacVector(),
	&K512Algo(),
	};


void SHA2TestsL(CMessageDigest::THashId aHashId)
	{
	CMessageDigest* md = CMessageDigestFactory::NewDigestLC(aHashId);
	TInt pos = aHashId - CMessageDigest::ESHA224;
	pos *= 3;
	ExecuteSha2TestsL (md, *gNames[pos], *gNames[pos+1], *gNames[pos+2]);
	CleanupStack::PopAndDestroy(md);
	}

void HashTests(void)
	{
	TInt32 testsFailed=0, testCount=0;

	test.Start(_L("MD4 Tests"));
	TRAPD(r, MD4TestsL());
  ++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nMD4 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}	

	test.Start(_L("SHA1 Tests"));
	TRAP(r, SHA1TestsL());
	++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nSHA1 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

	test.Start(_L("SHA-224 Tests"));
	TRAP(r, SHA2TestsL(CMessageDigest::ESHA224));
	++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nSHA-224 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

	test.Start(_L("SHA-256 Tests"));	
	TRAP(r, SHA2TestsL(CMessageDigest::ESHA256));
	++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nSHA-256 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

	test.Start(_L("SHA-384 Tests"));	
	TRAP(r, SHA2TestsL(CMessageDigest::ESHA384));
	++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nSHA-384 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

	test.Start(_L("SHA-512 Tests"));	
	TRAP(r, SHA2TestsL(CMessageDigest::ESHA512));
	++testCount;
	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nSHA-512 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}
		
	test.Start(_L("MD5 Tests"));
	TRAP(r, MD5TestsL());
  ++testCount;
 	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nMD5 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

	test.Start(_L("MD2 Tests"));
	TRAP(r, MD2TestsL());
  ++testCount;
 	if (r!=KErrNone)
		{
		test.Printf(_L("\r\nMD2 Tests failed error code =%d\r\n\r\n"),r);
		++testsFailed;
		}

  test.Printf(_L("\r\n%d tests failed out of %d \r\n"),testsFailed,testCount);
	test(testsFailed==0);
	}


GLDEF_C TInt E32Main(void)

	{
	CTrapCleanup* cleanup;
	cleanup=CTrapCleanup::New();
		
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-HASH-0035 Hash Algorithm Tests "));		 
	CTestConsole* con=NULL;
	TRAPD(ret, con=CTestConsole::NewL(test.Console()));
	if(ret != KErrNone)
		{
		return ret;	
		}
	RFs fs;

	fs.Connect();
	RFile* file;
	file=new (ELeave) RFile;
	
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TBuf<24> hashLogFile (sysDrive.Name());
	hashLogFile.Append(_L("\\HashLog.txt"));
	
	file->Replace(fs,hashLogFile,EFileShareAny|EFileWrite);
	con->SetLogFile(file);
	
	test.SetConsole(con);
	__UHEAP_MARK;
	 HashTests();		
	__UHEAP_MARKEND;

	test.End();
	test.Close();
	delete cleanup;
	return(KErrNone);
	}
