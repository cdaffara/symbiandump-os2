/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <apmrec.h>
#include <apmstd.h>
#include <asn1cons.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

#include "CertRecog.h"
#include "x509cert.h"
#include "x509certext.h"



const TInt KCertificateRecognizerValue = 0x101F4A71;
const TUid KUidMimeCertRecognizer = { KCertificateRecognizerValue };

// User certs
_LIT8( KDataTypeWAPCertificateResponse, "application/vnd.wap.cert-response" );
_LIT8( KDataTypeX509_USER_Certificate, "application/x-x509-user-cert" );

// CA certs
_LIT8( KDataTypeX509_CA_Certificate, "application/x-x509-ca-cert" );
_LIT8( KDataTypeWAP_WTLS_CA_Certificate, "application/vnd.wap.wtls-ca-certificate" );

const TInt KSupportedDataTypesTotal = 3;

const TInt KX509MinBufferLength = 30;
const TInt KCertRecMaxBufferLength = 80;

const TInt KASN1SequenceTagValue = 0x30;
const TInt KASN1SequenceTwoBytesLength = 0x82;
const TInt KASN1SequenceThreeBytesLength = 0x83;
const TInt KASN1VersionWrapperTagValue = 0xA0;
const TInt KASN1VersionWrapperLengthValue = 0x03;
const TInt KASN1VersionLengthValue = 0x01;
enum { EX509VersionValue1 = 0x00, EX509VersionValue2 = 0x01, EX509VersionValue3 = 0x02 };
const TInt KWTLSCertificateVersionValue = 0x01;



// ----------------------------------------------------------------------------
// CApaCertificateRecognizer
//

CApaCertificateRecognizer::CApaCertificateRecognizer()
	: CApaDataRecognizerType( KUidMimeCertRecognizer, CApaDataRecognizerType::ENormal )
	{
	iCountDataTypes = KSupportedDataTypesTotal;
	}



TUint CApaCertificateRecognizer::PreferredBufSize()
	{
	return KCertRecMaxBufferLength;
	}



TDataType CApaCertificateRecognizer::SupportedDataTypeL( TInt aIndex ) const
	{
	__ASSERT_DEBUG( aIndex >= 0 && aIndex < KSupportedDataTypesTotal,
					User::Panic( _L("RECCERT"), 0 ) );
	switch ( aIndex )
		{
		case 0:
			return TDataType( KDataTypeX509_CA_Certificate );
		case 1:
			return TDataType( KDataTypeWAP_WTLS_CA_Certificate );
		// Used to prevent warning about return paths not all returning a value
		default:
			return TDataType( KDataTypeWAPCertificateResponse );
		}
	}



void CApaCertificateRecognizer::DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer )
	{
 	
 	RFile* filePtr = FilePassedByHandleL();
 	if (filePtr)
 		{
 		ReadFileAndRecognizeL(*filePtr);
 		return;	
 		}
 	
 	//Connect to file server
 	RFs fs;
 	User::LeaveIfError(fs.Connect());	
 	CleanupClosePushL(fs);
 	
 	// See if the data is passed by filename
 	RFile fileToRead;	
 	TInt ret = fileToRead.Open(fs, aName, EFileRead | EFileShareReadersOnly | EFileStream);
 	CleanupClosePushL(fileToRead);
 	if (ret == KErrNone)
 		{
 		//Try the preferred buffer size first
 		ReadFileAndRecognizeL(fileToRead);
 		}
 	else
 		// If not passed by file name, can try to recognize buffer.
 		{
 		DoRecognizeBufferL(aBuffer, EFalse, fileToRead);
 		}
 	
 	CleanupStack::PopAndDestroy(2); //fileToRead, fs		
 	
 	}

void CApaCertificateRecognizer::ReadFileAndRecognizeL(RFile& aFileToRead)
 	{
 	TInt size=PreferredBufSize();
 	HBufC8* memForFile = HBufC8::NewLC(size);
 	TPtr8 preferredBuffer(memForFile->Des());
 	User::LeaveIfError(aFileToRead.Read(preferredBuffer, size));
 	DoRecognizeBufferL(preferredBuffer, ETrue, aFileToRead);		
 	CleanupStack::PopAndDestroy(memForFile); //memForFile	
 	}
 
void CApaCertificateRecognizer::DoRecognizeBufferL(const TDesC8& aBuffer,  TBool aIsFile, RFile& aFile)
	{
	// Ensure length is sufficient for checking type
   	if ( aBuffer.Size() >= 1 )
   		{
   		// First byte of X.509 certificate is an ANS.1 SEQUENCE tag
   		if ( aBuffer[0] == KASN1SequenceTagValue )
   			{
  			RecognizeX509CertL( aBuffer, aIsFile, aFile);
   			}
   		// First byte of WTLS certificate is version == 1
   		else if ( aBuffer[0] == KWTLSCertificateVersionValue )
   			{
  			RecognizeWTLSCertOrCertResponse( aBuffer, aIsFile, aFile);
   			}
  		
 		}						
   	}

void CApaCertificateRecognizer::RecognizeX509CertL( const TDesC8& aBuffer,  TBool aIsFile, RFile& aFile )
		{

	//Basic check if this is a X509 certificate
	TBool isCertV1orV2(EFalse);
	TInt bufferSize = aBuffer.Size();
	if ( bufferSize < KX509MinBufferLength )
		{
		return;
		}
	TInt index = 1;
	// Check first sequence length byte and skip over the length value bytes
	if ( aBuffer[index] == KASN1SequenceTwoBytesLength )
		{
		index += 3;
		}
	else if ( aBuffer[index] == KASN1SequenceThreeBytesLength )
		{
		index += 4;
		}
	else
		{
		return;
		}
	// Check next byte that is another sequence start
	if ( aBuffer[index] != KASN1SequenceTagValue )
		{
		return;
		}
	++index;
	// Check second sequence length byte and skip over the length value bytes
	if ( aBuffer[index] == KASN1SequenceTwoBytesLength )
		{
		index += 3;
		}
	else if ( aBuffer[index] == KASN1SequenceThreeBytesLength )
		{
		index += 4;
		}
	else
		{
		return;
		}
	// Check for VERSION field
	if ( aBuffer[index] == KASN1VersionWrapperTagValue )
		{
		++index;
		if ( aBuffer[index] != KASN1VersionWrapperLengthValue )
			{
			return;
			}
		++index;
		if ( aBuffer[index] != EASN1Integer )
			{
			return;
			}
		++index;
		if ( aBuffer[index] != KASN1VersionLengthValue )
			{
			return;
			}
		++index;

		// The cert is X509 v1 or v2
 		if (aBuffer[index] == EX509VersionValue1 ||
 		    aBuffer[index] == EX509VersionValue2)
		    {
		    isCertV1orV2=ETrue;
		    }
		else
	 	   // The cert is neither X509 v1 nor v2
 		    {
 		   // The cert is not v3 as well 
 		   if (aBuffer[index] != EX509VersionValue3)
 			{
 			return;	
			}
 		    }		

		++index;
		}
	else
		{
		isCertV1orV2=ETrue;	
		}
	// Check for SerialNumber field
	if ( aBuffer[index] != EASN1Integer )
		{
		return;
		}
	++index;
	TInt serialNumberSize = aBuffer[index];
	++index;
	index += serialNumberSize;
	// 1 is added for the next increments of index
	if((index + 1) >= bufferSize)
		{
		return;
		}
	// Check for SIGNATURE field
	if ( aBuffer[index] != KASN1SequenceTagValue )
		{
		return;
		}
	++index;
	TInt signatureSize = aBuffer[index];
	++index;
	index += signatureSize;
	if(index >= bufferSize)
		{
		return;
		}
	 	
 	// if the certificate is passed by the buffer, but not file name 
 	if (!aIsFile)	
 		{
 		// Check only the starting TAG byte of the NAME field
 		if ( aBuffer[index] == KASN1SequenceTagValue )
 			{
 			iDataType = TDataType( KDataTypeX509_CA_Certificate );
 			iConfidence = EProbable;
 			}			
 		}
 	else
 		// if the certificate is passed by file name
		{
		if (!isCertV1orV2) // x509 V3 certificate
 			{ 
 			//Get the file size
 			TInt size;
 			User::LeaveIfError(aFile.Size(size));
 			HBufC8* memForFile = HBufC8::NewLC(size);
 			TPtr8 fileContent(memForFile->Des());
 			TInt pos=0;
 			aFile.Seek(ESeekStart, pos);
 			User::LeaveIfError(aFile.Read(fileContent, size));
 			RecognizeWholeX509CertificateL(fileContent);
 			CleanupStack::PopAndDestroy(memForFile);//memForFile
			}									
 		else  // x509 V1 or V2 certificate
 			{ 
 			iDataType = TDataType( KDataTypeX509_CA_Certificate );
 			iConfidence = EProbable;				
 			}
	
		}
	}

void CApaCertificateRecognizer::RecognizeWholeX509CertificateL(const TDesC8& aBuffer)
 	{
 	CX509Certificate* cert = CX509Certificate::NewLC(aBuffer);
 	if (cert)
 		{
 		const CX509CertExtension* certExt = cert->Extension(KBasicConstraints);
 		if (certExt)
 			{
 			CX509BasicConstraintsExt* basic = CX509BasicConstraintsExt::NewLC(certExt->Data());
 			if (basic->IsCA())
 				{
 				iDataType = TDataType( KDataTypeX509_CA_Certificate );
 				iConfidence = EProbable;					
 				}
 			else
 				{
 				iDataType = TDataType( KDataTypeX509_USER_Certificate );
 				iConfidence = EProbable;										
 				}
 			CleanupStack::PopAndDestroy(basic); //basic				
			}
 		else
 			{
 			iDataType = TDataType( KDataTypeX509_USER_Certificate );
 			iConfidence = EProbable;														
 			}
 		}
 	CleanupStack::PopAndDestroy(cert); //cert
 	}



void CApaCertificateRecognizer::RecognizeWTLSCertOrCertResponse( const TDesC8& aBuffer, TBool /*aIsFile*/, RFile& /*aFile*/ )
	{
	if ( aBuffer.Size() >= 3 )
		{
		// Check next byte is signature algorithm value from 0 - 2
		if ( aBuffer[1] == 0x00 || aBuffer[1] == 0x01 || aBuffer[1] == 0x02 )
			{
			// Check Identifier Type for Issuer Identifier
			if ( aBuffer[2] == 0x01 )
				{
				iDataType = TDataType( KDataTypeWAP_WTLS_CA_Certificate );
				}
			else
				{
				iDataType = TDataType( KDataTypeWAPCertificateResponse );
				}
			iConfidence = EPossible;
			}
		}
	}

CApaDataRecognizerType* CApaCertificateRecognizer::CreateRecognizerL()
	{
	return new (ELeave) CApaCertificateRecognizer();
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x102034A2, CApaCertificateRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}	

