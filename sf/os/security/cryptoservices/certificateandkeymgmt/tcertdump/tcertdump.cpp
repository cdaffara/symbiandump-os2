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


#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <f32file.h>
#include <x509cert.h>
#include <utf.h>
#include <bacline.h>
#include <x509certext.h>

#include "displaytype.h"

_LIT(KAppName, "tcertdump");


void WriteAsUtf8L(const TDesC& aUtf16, RFile& aOutputFile)
	{
	HBufC8* utf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aUtf16);
	CleanupStack::PushL(utf8);
	User::LeaveIfError(aOutputFile.Write(*utf8));
	CleanupStack::PopAndDestroy(utf8);
	}


void PrintDistinguishedNameElementsL(const CX500DistinguishedName& dName, RFile& aOutputFile)
	{
	TInt i;
	for (i=0;i<dName.Count();i++)
		{
		User::LeaveIfError(aOutputFile.Write(_L8("\t\t")));
		//WriteAsUtf8L(dName.Element(i).Type(), aOutputFile);
		
		
		WriteAsUtf8L(GetType(dName.Element(i).Type()), aOutputFile);
		
				
		User::LeaveIfError(aOutputFile.Write(_L8(": ")));
		
		HBufC* value = NULL;
		value=dName.Element(i).ValueL();
		CleanupStack::PushL(value);
		
		
		if (dName.Element(i).Type().Compare(_L("2.5.4.3")))
			{
			WriteAsUtf8L(*value, aOutputFile);
			}
		else 
			{
			HBufC8* temp=NULL;
			temp=HBufC8::NewL(value->Length());
			CleanupStack::PushL(temp);
			
			TPtr8 ptr = temp->Des();
			TPtr valuePtr = value->Des();
			for (TInt j = 0;j < value->Length();++j)
				{
				TChar c = valuePtr[j];
				ptr.Append(c % 0x10000);
				}
			
			User::LeaveIfError(aOutputFile.Write(*temp));
			CleanupStack::PopAndDestroy(temp);
			}
	
		User::LeaveIfError(aOutputFile.Write(_L8("\r\n")));	
		CleanupStack::PopAndDestroy(value);
		}
	}

void DumpCertInfoL(const CX509Certificate& aCert, RFile& aOutputFile)
	{
	// common/display names
	aOutputFile.Write(_L8("Issuer Display Name: "));		
	HBufC* issuer = aCert.IssuerL();
	CleanupStack::PushL(issuer);		
	WriteAsUtf8L(*issuer, aOutputFile);
	User::LeaveIfError(aOutputFile.Write(_L8("\r\n")));
	CleanupStack::PopAndDestroy(issuer);
		
	aOutputFile.Write(_L8("Subject Display Name: "));
	HBufC* subject = aCert.SubjectL();
	CleanupStack::PushL(subject);
	WriteAsUtf8L(*subject, aOutputFile);
	User::LeaveIfError(aOutputFile.Write(_L8("\r\n")));	
	CleanupStack::PopAndDestroy(subject); 
	
	
	// distinguished names
	
	aOutputFile.Write(_L8("Issuer Distinguished Name:\r\n"));
	PrintDistinguishedNameElementsL(aCert.IssuerName(),aOutputFile);
		
	aOutputFile.Write(_L8("Subject Distinguished Name:\r\n"));
	PrintDistinguishedNameElementsL(aCert.SubjectName(),aOutputFile);
			
	// alt names	
	const CX509CertExtension *subjectAltExt=aCert.Extension(KSubjectAltName);
	if (subjectAltExt!=NULL)
		{		
		aOutputFile.Write(_L8("Subject Alt Names:\r\n"));
		CX509AltNameExt* subjectAltNameExt = CX509AltNameExt::NewLC(subjectAltExt->Data());
		const CArrayPtrFlat<CX509GeneralName>&names = subjectAltNameExt->AltName();
		for (TInt i = 0; i < names.Count(); i++)
			{
			CX509GeneralName* name = names.At(i);
			if	( name->Tag() == EX509DirectoryName )
				{
				aOutputFile.Write(_L8("\tDirectory Name:\r\n"));
				CX500DistinguishedName* dirName = CX500DistinguishedName::NewLC(name->Data());
				PrintDistinguishedNameElementsL(*dirName,aOutputFile);
				CleanupStack::PopAndDestroy(dirName);				
				}
			}
		CleanupStack::PopAndDestroy(subjectAltNameExt);
		}
	
	const CX509CertExtension *issuerAltExt=aCert.Extension(KIssuerAltName);
	if (issuerAltExt!=NULL)
		{		
		aOutputFile.Write(_L8("Issuer Alt Names:\r\n"));
		CX509AltNameExt* issuerAltNameExt = CX509AltNameExt::NewLC(issuerAltExt->Data());
		const CArrayPtrFlat<CX509GeneralName>&names = issuerAltNameExt->AltName();
		for (TInt i = 0; i < names.Count(); i++)
			{
			CX509GeneralName* name = names.At(i);
			if	( name->Tag() == EX509DirectoryName )
				{
				aOutputFile.Write(_L8("\tDirectory Name:\r\n"));
				CX500DistinguishedName* dirName = CX500DistinguishedName::NewLC(name->Data());
				PrintDistinguishedNameElementsL(*dirName,aOutputFile);
				CleanupStack::PopAndDestroy(dirName);				
				}
			}
		CleanupStack::PopAndDestroy(issuerAltNameExt);
		}
	}

void DumpCertL()
	{
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TInt count = args->Count();
	if (args->Count() == 3) 
		{
		const TPtrC certFileName = args->Arg(1);
		const TPtrC outputFileName = args->Arg(2);
		
		RFs fs;
		User::LeaveIfError(fs.Connect());
		CleanupClosePushL(fs);
	
		RFile certFile;
		User::LeaveIfError(certFile.Open(fs, certFileName, EFileShareAny | EFileRead | EFileStream));
		CleanupClosePushL(certFile);
		
		TInt fileSize;
		User::LeaveIfError(certFile.Size(fileSize));
		
		HBufC8* certBuffer = HBufC8::NewLC(fileSize);		
		TPtr8 ptr = certBuffer->Des();
		User::LeaveIfError(certFile.Read(ptr, fileSize));
		
		CX509Certificate* cert = CX509Certificate::NewLC(*certBuffer);
		
		RFile outputFile;
		User::LeaveIfError(outputFile.Replace(fs, outputFileName, EFileWrite | EFileShareExclusive));
		
		DumpCertInfoL(*cert, outputFile);
	
		CleanupStack::PopAndDestroy(4, &fs); // outputFile, cert, certBuffer, certFile, fs
		}	
	CleanupStack::PopAndDestroy(args);	
	}

void DoMainL(void)
	{	
	CConsoleBase* console = Console::NewL(KAppName, TSize(KDefaultConsWidth, KDefaultConsHeight));
	CleanupStack::PushL(console);
	__UHEAP_MARK;
	TRAPD(err, DumpCertL());
	if (err)
		{
		console->Printf(_L("Error %d\n"), err);
		console->Getch();
		}
	__UHEAP_MARKEND;	
	CleanupStack::PopAndDestroy(console);
	}

GLDEF_C TInt E32Main()		   // main function called by E32
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	
	TRAP_IGNORE(DoMainL());
	
	delete cleanup; 
	__UHEAP_MARKEND;
	return 0; 
	}

