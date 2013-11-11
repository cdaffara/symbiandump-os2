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


#include "createx509.h"


X509* CX509_Initializer::CreateX509L(CX509Certificate* X509Cert)
{
	X509* ret = X509_new();
	TBool serail = ETrue;
	
	if(ret != NULL)
	{
	
//validity
		X509_VAL_free(ret->cert_info->validity);
		ret->cert_info->validity = CreateX509_VAL(X509Cert); 

//issuer
		const CX500DistinguishedName& IssName = X509Cert->IssuerName();	
		X509_NAME_free(ret->cert_info->issuer);
		ret->cert_info->issuer = CreateX509_NAMEL(IssName);

//subject
		const CX500DistinguishedName& SubName = X509Cert->SubjectName();	
		X509_NAME_free(ret->cert_info->subject);
		ret->cert_info->subject = CreateX509_NAMEL(SubName);
	//	const HBufC * name = SubName.DisplayNameL();

//signature
		const TPtrC8* sig_alg_ptr = X509Cert->DataElementEncoding(CX509Certificate::EAlgorithmId);
		X509_ALGOR_free(ret->cert_info->signature);
		ret->cert_info->signature = CreateX509_ALGORL(sig_alg_ptr);

//serialnumber
		const TPtrC8 sernum = X509Cert->SerialNumber();
		ASN1_INTEGER_free(ret->cert_info->serialNumber);
		ret->cert_info->serialNumber = CreateASN1_STRING(sernum.Length(),V_ASN1_INTEGER,(unsigned char *)sernum.Ptr(),0);		
		if((sernum.Length()== 1) && sernum[0]==0)
			serail = EFalse;
			
//version

		TInt ver = X509Cert->Version();
		unsigned char verVal = (unsigned char)(ver-1);		
		ASN1_INTEGER_free(ret->cert_info->version);
		if( (verVal) || (!serail))// for X509 V1 certificates, version is null if any serial number present.
			ret->cert_info->version = CreateASN1_STRING(1,V_ASN1_INTEGER,&verVal,0);


//issuerUID
		const TPtrC8* issUID_enc = X509Cert->DataElementEncoding(CX509Certificate::EIssuerUID);
		if(issUID_enc)
			ret->cert_info->issuerUID = CreateASN1_STRING(issUID_enc->Length(),V_ASN1_BIT_STRING,(unsigned char *)issUID_enc->Ptr(),0);
		

//subjectUID
		const TPtrC8* subUID_enc = X509Cert->DataElementEncoding(CX509Certificate::ESubjectUID);
		if(subUID_enc)
			ret->cert_info->subjectUID = CreateASN1_STRING(subUID_enc->Length(),V_ASN1_BIT_STRING,(unsigned char *)subUID_enc->Ptr(),0);

//key
		X509_PUBKEY_free(ret->cert_info->key);
		ret->cert_info->key = CreateX509_PUBKEYL(X509Cert);

	
//extension


		ret->cert_info->extensions = CreateSTACKOF_X509_EXTENSIONL(X509Cert);

		
//name
		ret->name = X509_NAME_oneline(ret->cert_info->subject, NULL, 0);

//sig_alg
		X509_ALGOR_free(ret->sig_alg);
		ret->sig_alg = CreateX509_ALGORL(sig_alg_ptr);
		
//signature
		const TPtrC8 sig = X509Cert->Signature();
		ASN1_STRING_free(ret->signature);
		ret->signature = CreateASN1_STRING(sig.Length(), V_ASN1_BIT_STRING, (unsigned char *)sig.Ptr(), ASN1_STRING_FLAG_BITS_LEFT);
	}
		
	return ret;
}


X509_ALGOR* CX509_Initializer::CreateX509_ALGORL(const TPtrC8* ptr)
{
	X509_ALGOR* ret = X509_ALGOR_new();
	
	TASN1DecGeneric dec((TDesC8 &)*ptr);	
	dec.InitL();
	
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(dec);
	
	TASN1DecGeneric& AlgorEncSeq = *(seq->At(0));

	if (dec.LengthDERContent() > AlgorEncSeq.LengthDER()) // can also check for (seq->Count() > 1) alternatively
	{
		// parameter part is present in the encoding.
		TASN1DecGeneric& ParameterEncSeq = *(seq->At(1));
		
		// if param = 5, ie. ASN1 type NULL, then create a NULL ASN1 STRING
		ret->parameter = ASN1_TYPE_new();
		ret->parameter->type = (TInt)ParameterEncSeq.Encoding()[0];
		
		if(ret->parameter->type != V_ASN1_NULL)
		{
			// we have some parameter 
			// add code to fill this stuff
		}
	}
	else
	{
		//encoding does not contain parameter at all
		//ret->parameter is anyway NULL when X509_ALGOR is created
		//Not sure if we need to create a NULL ASN1 string or just leave parameter = NULL
	}
	
	
	const TDesC8& algor_data = AlgorEncSeq.GetContentDER();
	char * ch_algor_data = (char *)algor_data.Ptr();
	
	ret->algorithm = ASN1_OBJECT_new();
	ret->algorithm->length = AlgorEncSeq.LengthDERContent();	
	ret->algorithm->data = (unsigned char *)OPENSSL_malloc(ret->algorithm->length);
	if(ret->algorithm->data)
		memcpy(ret->algorithm->data, ch_algor_data, ret->algorithm->length);
	//else log error- cannot malloc
	
	ret->algorithm->flags |= ASN1_OBJECT_FLAG_DYNAMIC_DATA;  // so that X509_ALGOR_free() frees all internally allocated data
	
	CleanupStack::PopAndDestroy();	//seq
	
	return ret;
}



X509_NAME* CX509_Initializer::CreateX509_NAMEL(const CX500DistinguishedName& DistName)
{
	X509_NAME* ret = X509_NAME_new();
	
	CASN1EncSequence * Asn1Seq = DistName.EncodeASN1LC();			

	HBufC8* octetData = HBufC8::NewMaxLC(5000);
	TPtr8 oct(octetData->Des());
	oct.FillZ();
	oct.SetLength(KMaxNameLength);
	TUint writePos = 0;
	Asn1Seq->WriteDERL(oct, writePos);			

  	TInt len = Fill_X509_NAME_ENTRYL(ret, octetData->Des());
	
	char *p = (char *)oct.PtrZ();

    ret->bytes->data = (char *)OPENSSL_malloc(len);        // no need to free this. BUF_MEM_free will free if not NULL
    if(ret->bytes->data)
     memcpy(ret->bytes->data, p, len);
    //else log error- cannot malloc
    
	ret->bytes->length = len;
	ret->bytes->max = len;

	ret->hash = 0; // for now filling zero. Not sure.
	
	CleanupStack::PopAndDestroy(2); // Asn1Seq, octetData
	
	return ret;
} 


// return the length of the encoded sequence
TInt CX509_Initializer::Fill_X509_NAME_ENTRYL(X509_NAME * name, const TDesC8& aBinaryData)
{
	TInt aPos = 0;
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();

	if (dec.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	while (aPos < end)
		{
		TASN1DecGeneric rdn(aBinaryData.Right(aBinaryData.Length() - aPos));
		rdn.InitL();
		if (rdn.Tag() != EASN1Set)
			{
			User::Leave(KErrArgument);
			}
		TInt rdnEnd = rdn.LengthDER();
		TInt rdnPos = rdn.LengthDERHeader();//add on header
		while (rdnPos < rdnEnd)
			{
			const TDesC8& TypeValEnc = rdn.Encoding();
			
		    				TASN1DecGeneric dec(TypeValEnc.Right(TypeValEnc.Length() - rdnPos));
							dec.InitL();
							TInt tvend = rdnPos + dec.LengthDER();
							rdnPos += dec.LengthDERHeader();
						
							//first element must be the id
							TASN1DecObjectIdentifier encOID;
							//iType = encOID.DecodeDERL(TypeValEnc, rdnPos);
							TASN1DecGeneric first(TypeValEnc.Right(TypeValEnc.Length() - rdnPos));
							first.InitL();
							rdnPos += first.LengthDER();
								if (first.Tag() != EASN1ObjectIdentifier)
								{
								User::Leave(KErrArgument);
								}
						
							const TDesC8& type = first.GetContentDER();
							char * ch_type = (char *)type.Ptr();
				
							//second is the data
							TASN1DecGeneric second(TypeValEnc.Right(TypeValEnc.Length() - rdnPos));
							second.InitL();
							rdnPos += second.LengthDER();
							
							const TDesC8& value = second.GetContentDER();
							char * ch_value = (char *)value.Ptr();
							
							X509_NAME_ENTRY* new_entry = CreateX509_NAME_ENTRY(ch_type,first.LengthDERContent(), ch_value, second.LengthDERContent(), second.Tag());
							
							X509_NAME_add_entry(name, new_entry, -1, 0);
							
							//we can free this, since add_entry makes a copy and adds
							X509_NAME_ENTRY_free(new_entry);
							//
							
							if (rdnPos != tvend)
								{
								User::Leave(KErrArgument);
								}
			
			}
		aPos += rdnEnd;
		}
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	return end;
}


X509_NAME_ENTRY * CX509_Initializer::CreateX509_NAME_ENTRY(char* type, int typeLen, char * value, int valueLen, int stringType)
{
	X509_NAME_ENTRY * newEntry = X509_NAME_ENTRY_new();
	
	newEntry->object->length = typeLen;
	newEntry->object->data = (unsigned char *)OPENSSL_malloc(typeLen);
	if(newEntry->object->data)
		memcpy(newEntry->object->data, type, typeLen);
	//else log error- cannot malloc
	
	ASN1_STRING_free(newEntry->value);
	newEntry->value = CreateASN1_STRING(valueLen, stringType, (unsigned char* )value, 0);
	
	newEntry->object->flags |= ASN1_OBJECT_FLAG_DYNAMIC_DATA;  // so that X509_NAME_ENTRY_free() frees all internally allocated data
	
	return newEntry;	
	
}


X509_VAL * CX509_Initializer::CreateX509_VAL(CX509Certificate* X509Cert)
{
	X509_VAL * ret = X509_VAL_new();
	
	const CValidityPeriod& val = X509Cert->ValidityPeriod();
				
		TBuf8<KCertMaxBuffer> numBuffer8;
		char* numPtr = (char*)numBuffer8.PtrZ();
	
	//start date (notBefore)
		TDateTime dt = val.Start().DateTime();
			
		numBuffer8.AppendNum(dt.Year());
		if(numBuffer8.Length() > 2)
			numBuffer8.Delete(0, numBuffer8.Length() - 2);
		
		_LIT8(KCertTimeStampFormat, "%02d%02d%02d%02d%02dZ");
		numBuffer8.AppendFormat(KCertTimeStampFormat,dt.Month()+1,dt.Day()+1,dt.Hour(),dt.Minute(),dt.Second());  //Month and Day - offset from zero, so add 1
		numPtr[13]='\0';
		
		ASN1_STRING_free(ret->notBefore);
		ret->notBefore = CreateASN1_STRING(13,V_ASN1_UTCTIME,(unsigned char *)numPtr,0);

	//finish date (notAfter)
		dt = val.Finish().DateTime();
		
		numBuffer8.Zero();
		numBuffer8.AppendNum(dt.Year());
		if(numBuffer8.Length() > 2)
			numBuffer8.Delete(0, numBuffer8.Length() - 2);
		
		numBuffer8.AppendFormat(KCertTimeStampFormat,dt.Month()+1,dt.Day()+1,dt.Hour(),dt.Minute(),dt.Second());  //Month and Day - offset from zero, so add 1
		numPtr[13]='\0';
		
		ASN1_STRING_free(ret->notAfter);
		ret->notAfter = CreateASN1_STRING(13,V_ASN1_UTCTIME,(unsigned char *)numPtr,0);
		
	return ret;
	
}



ASN1_STRING* CX509_Initializer::CreateASN1_STRING(int len, int type, unsigned char* data, long flags)
{
 ASN1_STRING* ret = ASN1_STRING_new();
 ret->length = len;
 ret->type = type;
 if(data!=NULL)
 {
 	ret->data = (unsigned char *)OPENSSL_malloc(len);
 	if(ret->data)
 	  memcpy(ret->data,data,len);
 	//else log error- cannot malloc
 }
 else
 	ret->data = NULL;
 	
 ret->flags |= flags;
 return ret;
}



X509_PUBKEY* CX509_Initializer::CreateX509_PUBKEYL(CX509Certificate* X509Cert)
{
	X509_PUBKEY* ret = X509_PUBKEY_new();

//algor
	const TPtrC8* ptr = X509Cert->DataElementEncoding(CX509Certificate::ESubjectPublicKeyInfo);
	
	TInt aPos = 0;
	TASN1DecGeneric dec(ptr->Right(ptr->Length() - aPos));
	dec.InitL();
	
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();

	if (dec.Tag() != EASN1Sequence)
		User::Leave(KErrArgument);

	if (aPos < end)
		{
			TASN1DecGeneric rdn(ptr->Right(ptr->Length() - aPos));
			rdn.InitL();
			TPtrC8 newPtr = rdn.Encoding();
			
			X509_ALGOR_free(ret->algor);	// free the one allocated by X509_PUBKEY_new
			ret->algor = CreateX509_ALGORL(&newPtr);
		}
	
//public_key
	const CSubjectPublicKeyInfo& pubkey = X509Cert->PublicKey();
	const TPtrC8 keyDat =	pubkey.KeyData();
	
	ASN1_BIT_STRING_free(ret->public_key);
	ret->public_key = CreateASN1_STRING(keyDat.Size(),V_ASN1_BIT_STRING,(unsigned char*)keyDat.Ptr(),0);
	
//pkey
	ret->pkey = NULL;		// we need not create this. Will be created later. Used to cache the computed data
	
	return ret;
}




STACK_OF(X509_EXTENSION)* CX509_Initializer::CreateSTACKOF_X509_EXTENSIONL(CX509Certificate* X509Cert)
{
	//STACK_OF(X509_EXTENSION) * ret = sk_X509_EXTENSION_new_null();
	STACK_OF(X509_EXTENSION) * ret = NULL;
	
	const TPtrC8* ptr = X509Cert->DataElementEncoding(CX509Certificate::EExtensionList);
	
	const CArrayPtrFlat<CX509CertExtension>& extlist = X509Cert->Extensions();

	TASN1DecSequence encSeq;
	TInt pos = 0;

	if(ptr != NULL)	//There is an extension list
	{
		TASN1DecGeneric dec((TDesC8 &)*ptr);
		ret = sk_X509_EXTENSION_new_null();
		dec.InitL();
		
		CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC( dec.GetContentDER(), pos);
	
		TInt count = seq->Count(); // no of extensions in the ext list
		for (TInt i = 0; i < count; i++)
		{
		
			X509_EXTENSION* ext = X509_EXTENSION_new();
		
			ext->object = ASN1_OBJECT_new();
						
			TASN1DecGeneric* gen = seq->At(i);
			TASN1DecGeneric oid(seq->At(i)->GetContentDER());
			oid.InitL();
			
			const TDesC8& ext_obj_data = oid.GetContentDER();
			char * ch_ext_obj_data = (char *)ext_obj_data.Ptr();
			
			ext->object->length = oid.LengthDERContent();
			ext->object->data = (unsigned char *)OPENSSL_malloc(ext->object->length);
			if(ext->object->data)
				memcpy(ext->object->data, ch_ext_obj_data, ext->object->length);
			//else log error- cannot malloc
		
			ext->object->flags |= ASN1_OBJECT_FLAG_DYNAMIC_DATA;  // so that X509_free() frees all internally allocated data
			
			if(extlist.At(i)->Critical())
				ext->critical = extlist.At(i)->Critical();
			
			const TPtrC8 data = extlist.At(i)->Data();
			TASN1DecGeneric value(data);
			value.InitL();
			
			const TDesC8& ext_value = value.GetContentDER();
			char * ch_ext_value = (char *)ext_value.Ptr();
			ASN1_STRING_free(ext->value);
			ext->value = CreateASN1_STRING(value.LengthDERContent(), V_ASN1_OCTET_STRING, (unsigned char *)ch_ext_value, 0);
			
			sk_X509_EXTENSION_push(ret,ext);		
	
		}
			
		CleanupStack::PopAndDestroy();// seq
	}
	
	return ret;
}

