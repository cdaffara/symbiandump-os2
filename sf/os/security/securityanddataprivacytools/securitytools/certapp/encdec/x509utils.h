#ifndef __X509UTILS_H__
#define __X509UTILS_H__/*
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


/**
 @file
 @internalComponent
*/
enum TCertificateFormat
	{
    EX509Certificate    = 0x00,
    EWTLSCertificate    = 0x01,
	EX968Certificate    = 0x02,
	EUnknownCertificate = 0x0f,
	EX509CertificateUrl = 0x10,
	EWTLSCertificateUrl = 0x11,
	EX968CertificateUrl = 0x12
	};

const TInt KSHA1HashLengthInBytes = 20;
typedef  TBuf8<KSHA1HashLengthInBytes> TSHA1Hash;
typedef TSHA1Hash TKeyIdentifier;

struct KeyIdentifierObject
	{
	bool iAutoKey; // iHash needs generating by us, or matched the value we would generate!
	TKeyIdentifier iHash;
	};

void EncodeHuman(REncodeWriteStream& aStream,const KeyIdentifierObject &aKeyId);
void DecodeHuman(RDecodeReadStream& aStream, KeyIdentifierObject &aKeyId);

RWriteStream& operator<<(RWriteStream& aStream,const KeyIdentifierObject& aKeyId);
RReadStream& operator>>(RReadStream& aStream, KeyIdentifierObject& aKeyId);


enum EUseCertificateExtension
{
	KIgnoreCertificateExtension,
	KUseCertificateExtension
};


/**
   Get the subject key id from the certificate extension or calculate
   it from the public key (as specified in rfc3280 4.2.1.2 method 1).

   The subject name and subject key id are returned.

   WARNING: This function is NOT valid for client certs. For client
   certs, the subject key id in the certificate store MUST match the id
   of the key in the keystore.

   If ok return true.

   If the certificate is invalid then exit the program with an error message!
 */
bool X509SubjectKeyId(EUseCertificateExtension aUseExtension, bool aUseRfc3280Algorithm,
					  bool aIsCa, const std::string &aCert, 
					  std::string &aSubject, TKeyIdentifier &aSubjectKeyId);

/**
   Get the issuer key id from the certificate authority key id extension.

   The issuer name and issuer key id are returned.

   If ok return true.

   If the certificate is invalid then exit the program with an error message!
 */
bool X509IssuerKeyId(EUseCertificateExtension aUseExtension,
					 const TUint8 *aCert, TUint32 aCertLength, 
					 std::string &aIssuer,
					 TKeyIdentifier &aIssuerKeyId);

/**
   Convert the DER certificate into PEM form
*/
void Der2Pem(const std::string &aDerCert, std::string &aPemCert);

/**
   Convert the PEM certificate into DER form
   
   Returns true if conversion succeeds
*/
bool Pem2Der(const std::string &aPemCert, std::string &aDerCert);
#endif
