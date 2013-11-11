/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "openssl/pem.h"
#include "encdec.h"
#include "x509utils.h"
#include "logger.h"
//
// TKeyIdentifier
//
void EncodeHuman(REncodeWriteStream& aStream,const KeyIdentifierObject  &aKeyId)
{
	if(aKeyId.iAutoKey)
		{
		aStream.WriteCStr("auto");
		if(aKeyId.iHash.Length() == 0)
			{
			return; // Empty value so no point in including it in a comment...
			}
		if(!aStream.Verbose())
			{
			return; // auto, and not in verbose mode so do not write value in comment
			}
		
		aStream.WriteCStr(" # ");
		}
	aStream.WriteByte('\'');
	const TUint8 *ptr = aKeyId.iHash.Ptr();
	TInt len = aKeyId.iHash.Length();
	while(len--)
		{
		TUint8 byte = *ptr++;
		TUint8 buf[2];

		TUint8 ch = ((byte & 0xf0) >> 4);
		ch = (ch<=9) ? (ch +'0') : (ch - 10 +'A');

		// Write MSB char of byte
		buf[0] = ch;

		ch = (byte & 0x0f);
		ch = (ch<=9) ? (ch +'0') : (ch - 10 +'A');

		// Write LSB char of byte
		buf[1] = ch;

		aStream.WriteBin(buf, sizeof(buf));
		if(len)
			{
			aStream.WriteByte(':');
			}
		
		}
	aStream.WriteByte('\'');
}
void DecodeHuman(RDecodeReadStream& aStream, KeyIdentifierObject &aKeyId)
{
	aStream.ReadNextToken();
	std::string tok = aStream.Token();

	if(tok == "auto")
		{
		aKeyId.iAutoKey = true;
		aKeyId.iHash.SetLength(0);
		return;
		}
	aKeyId.iAutoKey = false;

	if((tok[0] != '\'') || (tok[tok.size()-1] != '\'') || (tok.size() < 2))
		{
		dbg << Log::Indent() << "KeyIdentifier not enclosed in single quotes, or contains spaces - " << tok << Log::Endl();
		FatalError();
		}
	
	tok.erase(0,1);
	tok.erase(tok.size()-1,1);

	if(tok.size() == 0)
		{
		aKeyId.iHash.SetLength(0);
		return;
		}
	
	if(TInt(tok.size()) != (aKeyId.iHash.MaxLength()*2) + (aKeyId.iHash.MaxLength()-1))
		{
		dbg << Log::Indent() << "WARNING: KeyIdentifier length not " << aKeyId.iHash.MaxLength()*2 << " hex digits" << Log::Endl();
		dbg << Log::Indent() << "KeyIdentifier is '" << tok << "'" << Log::Endl();
		}

	bool bad = false;
	TInt bytesRead = 0;
	const char *hexDigit = tok.data();
	TInt charsToRead = tok.size();
	TUint8 *dest = const_cast<TUint8 *>(aKeyId.iHash.Ptr());
	while(charsToRead)
		{
		// Read MSB char
		TUint8 byte = fromHex(*hexDigit++);
		byte <<= 4;
		--charsToRead;
				
		// Read LSB char
		if(charsToRead == 0)
			{
			bad = true;
			break;
			}
		byte |= fromHex(*hexDigit++);
		--charsToRead;

		// Save decoded byte
		*dest++ = byte;
		++bytesRead;

		if(charsToRead != 0)
			{
			// Consume : separator
			if(*hexDigit++ != ':')
				{
				bad = true;
				break;
				}
			--charsToRead;
			}
		}

	if(bytesRead > aKeyId.iHash.MaxLength())
		{
		dbg << Log::Indent() << "Key Identifiier is too long" << Log::Endl();
		bad = true;
		}
	

	if(bad)
		{
		dbg << Log::Indent() << "KeyIdentifier invalid - It should be a single quoted string containing a series of 0 or more 2 digit hex numbers separated by : chars." << Log::Endl();
		dbg << Log::Indent() << "This field should normally be set to auto or omitted" << Log::Endl();
		dbg << Log::Indent() << "KeyIdentifier is '" << tok << "'" << Log::Endl();
		FatalError();
		}

	aKeyId.iHash.SetLength(bytesRead);


	return;
}

RWriteStream& operator<<(RWriteStream& aStream,const KeyIdentifierObject& aKeyId)
{
	aStream << aKeyId.iHash;
	return aStream;
}

RReadStream& operator>>(RReadStream& aStream, KeyIdentifierObject& aKeyId)
{
	aKeyId.iAutoKey = false;
	aStream >> aKeyId.iHash;
	return aStream;
}

// It is illegal to pass a "X **" ptr to a function taking a "const X
// **" argument. This is because the function could change the callers
// pointer to point at a const object which the caller might then
// accidentally write to!
//
// Unfortunately openssl 0.9.7* defines d2i_X509 to take an "unsigned
// char **" and 0.9.8 takes "const unsigned char **", so neither
// caller choice will compile for both....

#if OPENSSL_VERSION_NUMBER >= 0x00908000L
#define D2I_CONST const
#else
#define D2I_CONST
#endif

bool X509SubjectKeyId(EUseCertificateExtension aUseExtension, bool aUseRfc3280Algorithm,
					  bool aIsCa, const std::string &aCert, 
					  std::string &aSubject, TKeyIdentifier &aSubjectKeyId)
{
	bool done = false;
	prog << Log::Indent() << "X509SubjectKeyId - aUseExtension " << aUseExtension << " aUseRfc3280Algorithm " << aUseRfc3280Algorithm << " :-" << Log::Endl();
	AutoIndent ai(prog); // IncIndent, will DecIndent when it leaves scope

	// decode DER certificate into X509 structure
	D2I_CONST unsigned char *p = (D2I_CONST unsigned char *)aCert.data();
	X509 *x509 = d2i_X509(NULL, &p, aCert.size());
	if(!x509 || ((const char *)p != aCert.data() + aCert.size()))
		{
		dbg << Log::Indent() << "openssl failed to decode certificate" << Log::Endl();
		FatalError();
		}

	// Return the Subject Name
	prog << Log::Indent() << "Cert subject is '" << x509->name << "'" << Log::Endl();
	aSubject = std::string(x509->name);
	TUint32 ver = X509_get_version(x509);
	prog << Log::Indent() << "Cert version is '" << ver << "'" << Log::Endl();
	
	// if the ver is a v1 or v2 type then there is no way of knowing which is a CA, treat all certs as CA as done in the certificate recognizer.
	bool treatAsCa = false;  
	if ( ver < 3 || aIsCa )
		{
		treatAsCa = true;
		}
	
	if(treatAsCa && aUseExtension)
		{
		// Attempt to read Subject Key Id extension
		ASN1_OCTET_STRING *subKeyId = (ASN1_OCTET_STRING *) X509_get_ext_d2i(x509, NID_subject_key_identifier, NULL, NULL);
		if(subKeyId)
			{
			prog << Log::Indent() << "Found SubjectKeyId extension" << Log::Endl();
			if(subKeyId->length <= aSubjectKeyId.MaxLength())
				{
				aSubjectKeyId = TPtrC8(subKeyId->data, subKeyId->length);
				done = true;
				}
			else
				{
				prog << Log::Indent() << "but SubjectKeyId > 160 bits so ignoring it" << Log::Endl();
				}
			ASN1_OCTET_STRING_free(subKeyId);
			}
		}
	
	if(!done)
		{
		// Subject Key Id extension was ignored, missing or too long...
		if(aUseRfc3280Algorithm)
			{
			// We do not need to decode the public key just hash its
			// data as per rfc3280 4.2.1.2 method 1
			prog << Log::Indent() << "Calculating SubjectKeyId using RFC3280 4.2.1.2 method 1" << Log::Endl();
			unsigned char sha1hash[SHA_DIGEST_LENGTH];
			
			SHA1(x509->cert_info->key->public_key->data, x509->cert_info->key->public_key->length,
				 sha1hash);
			aSubjectKeyId = TPtrC8(sha1hash, SHA_DIGEST_LENGTH);
			done = true;
			}
		else
			{
			// Calculate SubjectKeyId via Symbian algorithm
			prog << Log::Indent() << "Calculating SubjectKeyId using Symbian algorithm" << Log::Endl();
			EVP_PKEY *key = X509_PUBKEY_get(x509->cert_info->key);
			if(!key)
				{
				dbg << Log::Indent() << "openssl failed to decode certificate public key" << Log::Endl();
				FatalError();
				}

			switch(key->type)
				{
				case EVP_PKEY_RSA:
					{
					TUint32 len = key->pkey.rsa->n->top*sizeof(BN_ULONG);
					TUint8 *buf = new TUint8[len];
					for(TUint32 i=0; i<len; ++i)
						{
						buf[i] = ((TUint8 *)key->pkey.rsa->n->d)[len-i-1];
						}

					unsigned char sha1hash[SHA_DIGEST_LENGTH];
					SHA1(buf, len, sha1hash);
					delete [] buf;
					aSubjectKeyId = TPtrC8(sha1hash, SHA_DIGEST_LENGTH);
					done = true;
					break;
					}
				case EVP_PKEY_DSA:
					{
					TUint32 len = key->pkey.dsa->pub_key->top*sizeof(BN_ULONG);
					TUint8 *buf = new TUint8[len];
					for(TUint32 i=0; i<len; ++i)
						{
						buf[i] = ((TUint8 *)key->pkey.dsa->pub_key->d)[len-i-1];
						}

					unsigned char sha1hash[SHA_DIGEST_LENGTH];
					SHA1(buf, len, sha1hash);
					delete [] buf;
					aSubjectKeyId = TPtrC8(sha1hash, SHA_DIGEST_LENGTH);
					done = true;
					break;
					}
				default:
					// Unknown public key type.
					prog << Log::Indent() << "Unknown public key type " << key->type << Log::Endl();
					break;
				}
			
			EVP_PKEY_free(key);
			}
		}

	X509_free(x509);
	return done;
}


bool X509IssuerKeyId(EUseCertificateExtension aUseExtension,
					 const TUint8 *aCert, TUint32 aCertLength, 
					 std::string &aIssuer, TKeyIdentifier &aIssuerKeyId)
{	
	prog << Log::Indent() << "X509IssuerKeyId :-" << Log::Endl();
	AutoIndent ai(prog); // IncIndent, will DecIndent when it leaves scope
	bool done = false;

	// decode DER certificate into X509 structure
	D2I_CONST unsigned char *p = (D2I_CONST unsigned char *)aCert;
	X509 *x509 = d2i_X509(NULL, &p, aCertLength);
	if(!x509 || (p != aCert+aCertLength))
		{
		dbg << Log::Indent() << "openssl failed to decode certificate" << Log::Endl();
		FatalError();
		}

	// Return the Subject Name
	prog << Log::Indent() << "Cert subject is '" << x509->name << "'" << Log::Endl();
	char *issuerOne = X509_NAME_oneline(X509_get_issuer_name(x509),0,0);
	prog << Log::Indent() << "Cert issuer is '" << issuerOne << "'" << Log::Endl();
	aIssuer = issuerOne;
	OPENSSL_free(issuerOne);

	if(aUseExtension)
		{
		// Attempt to read Subject Key Id extension
		AUTHORITY_KEYID *authKeyId = (AUTHORITY_KEYID *) X509_get_ext_d2i(x509, NID_authority_key_identifier, NULL, NULL);
		if(authKeyId)
			{
			prog << Log::Indent() << "Found AuthorityKeyId extension" << Log::Endl();
			if(authKeyId->keyid)
				{
				if(authKeyId->keyid->length <= aIssuerKeyId.MaxLength())
					{
					aIssuerKeyId = TPtrC8(authKeyId->keyid->data, authKeyId->keyid->length);
					done = true;
					}
				else
					{
					prog << Log::Indent() << "but AuthroityKeyId > 160 bits so ignoring it" << Log::Endl();
					}
				}
			else
				{
				prog << Log::Indent() << "but it does not include a key id, so ignoring it" << Log::Endl();
				}
			
			AUTHORITY_KEYID_free(authKeyId);
			}
		}

	X509_free(x509);
	return done;
}

void Der2Pem(const std::string &aDerCert, std::string &aPemCert)
{
	prog << Log::Indent() << "Converting DER to PEM:-" << Log::Endl();
	AutoIndent ai(prog); // IncIndent, will DecIndent when it leaves scope

	// decode DER certificate into X509 structure
	D2I_CONST unsigned char *p = (D2I_CONST unsigned char *)aDerCert.data();
	X509 *x509 = d2i_X509(NULL, &p, aDerCert.size());
	if(!x509 || ((const char *)p != aDerCert.data()+aDerCert.size()))
		{
		dbg << Log::Indent() << "openssl failed to decode certificate" << Log::Endl();
		FatalError();
		}

	BIO *memBio = BIO_new(BIO_s_mem());
BULLSEYE_OFF
	if(!memBio)
		{
		dbg << Log::Indent() << "openssl failed to create BIO" << Log::Endl();
		FatalError();
		}
	
	if(!PEM_write_bio_X509(memBio, x509))
		{
		dbg << Log::Indent() << "openssl failed to convert to PEM" << Log::Endl();
		FatalError();
		}
BULLSEYE_RESTORE
	
	long pemCertLen = 0;
	char *pemCertData = 0;
	pemCertLen = BIO_get_mem_data(memBio, &pemCertData);

	// Return the PEM cert
	aPemCert.assign(pemCertData, pemCertLen);
	
	BIO_free(memBio);

	X509_free(x509);

	prog << Log::Indent() << "Conversion ok" << Log::Endl();
	return;
}

static const char utf8Header[] = 
	{
		0xef, 0xbb, 0xbf
	};

bool Pem2Der(const std::string &aPemCert, std::string &aDerCert)
{
	prog << Log::Indent() << "Try PEM to DER coversion :-" << Log::Endl();
	AutoIndent ai(prog); // IncIndent, will DecIndent when it leaves scope

	TUint32 pemLength=aPemCert.size();
	const char *pemData=aPemCert.data();
	
	if((pemLength >= 3) && (memcmp(aPemCert.data(), utf8Header, sizeof(utf8Header)) == 0))
		{
		// PEM cert has a UTF8 header, so strip it
		prog << Log::Indent() << "Certificate data file has a UTF-8 header" << Log::Endl();
		pemLength -= sizeof(utf8Header);
		pemData += sizeof(utf8Header);
		}

	//
	// Read PEM to internal
	//
	BIO *memBioIn = BIO_new_mem_buf((void *)pemData, pemLength);
BULLSEYE_OFF
	if(!memBioIn)
		{
		dbg << Log::Indent() << "openssl failed to create BIO for reading PEM" << Log::Endl();
		FatalError();
		}
BULLSEYE_RESTORE
	
	X509 *x509 = PEM_read_bio_X509(memBioIn, NULL, 0, NULL);
	if(!x509)
		{
		prog << Log::Indent() << "Conversion failed - presumably DER" << Log::Endl();
		return false;
		}
	BIO_free(memBioIn);
    memBioIn = 0;

	//
	// Write internal to DER
	//
	unsigned char *derCert = 0;
	int derLen = i2d_X509(x509, &derCert);
	if(derLen <=0 )
		{
		dbg << Log::Indent() << "openssl failed to convert to DER" << Log::Endl();
		FatalError();
		}

	// Return the DER cert
	aDerCert.assign((char *)derCert, derLen);

	X509_free(x509);
	prog << Log::Indent() << "Conversion ok" << Log::Endl();
	return true;
}



// End of file
