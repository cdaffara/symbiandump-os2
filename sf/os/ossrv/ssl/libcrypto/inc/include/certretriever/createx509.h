/*
Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Description:
*/


#ifndef CREATE_X509_H
#define CREATE_X509_H

#include "certretriever.h"
#include <openssl/asn1.h>
#include <asn1enc.h>
#include <asn1dec.h>
#include <x500dn.h>
#include <openssl/ossl_typ.h>


class CX509_Initializer : CBase
{
public:
	static X509* CreateX509L(CX509Certificate* X509Cert);
	
	
private:
	CX509_Initializer(){}
	static X509_VAL * 	CreateX509_VAL(CX509Certificate* X509Cert);
	static ASN1_STRING* CreateASN1_STRING(int len, int type, unsigned char* data, long flags);
	static X509_NAME* 	CreateX509_NAMEL(const CX500DistinguishedName& DistName);
	static X509_NAME_ENTRY * CreateX509_NAME_ENTRY(char* type, int typeLen, char * value, int valueLen, int stringType);
	static X509_ALGOR* 	CreateX509_ALGORL(const TPtrC8* ptr);
	static X509_PUBKEY* CreateX509_PUBKEYL(CX509Certificate* X509Cert);
	static STACK_OF(X509_EXTENSION) * CreateSTACKOF_X509_EXTENSIONL(CX509Certificate* X509Cert);
	static TInt Fill_X509_NAME_ENTRYL(X509_NAME* name, const TDesC8& aBinaryData);
};


#endif