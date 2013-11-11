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


#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined (__BADESCA_H__)
#include <badesca.h>
#endif

#include <x509certchain.h>

_LIT(KCorrectFile1, "correctchain.txt");

//certs
//thawte root
_LIT(KTWSSRoot, "thawteserverca.cer");
//rsa/verisign root
_LIT(KRSASSCA, "vsignss.cer");
_LIT(KRSASSCA_OLD, "rsassca_old.crt");
	_LIT(KDashnet, "dashnet.crt");
	_LIT(K3Do, "3do.crt");

_LIT(KVSignCA3, "verisignclass3primaryca.cer");
	_LIT(KVSignServer3, "verisignserverclass3.crt");
		_LIT(KLloyds, "lloyds.crt");

_LIT(KRoot, "CA3_root.crt");
	_LIT(KCA2, "CA2.crt");
		_LIT(KCA1, "CA1.crt");
			_LIT(KEE, "EE.crt");
//noise
_LIT(KCA1SameDN, "CA1_H_sameDN.crt");
_LIT(KCA2SameDN, "CA2_H_sameDN.crt");
_LIT(KExtra, "interclear.crt");
//
_LIT(KGSRoot, "globalsignroot.cer");
	_LIT(KGSPC1, "globalsignprimaryclass1ca.cer");
		_LIT(KGSC1, "globalsignclass1ca.cer");
			_LIT(KGSWB, "willsglobalsign.crt");

//fortezza test certs
//1st hierachy
_LIT(KFortezzaPAA1, "x22paa.crt");
	_LIT(KFortezzaPCA1, "x22pca1.crt");
		_LIT(KFortezzaCA1, "x22ca1.crt");
			//user certs off this...
			_LIT(KFortezzaCA1EE1, "rich1.crt");
			_LIT(KFortezzaCA1EE2, "david1.crt");
			_LIT(KFortezzaExpired, "expired.crt");
			_LIT(KFortezzaFuture, "future.crt");
			_LIT(KFortezzaNameMismatch, "nameMismatch.crt");
			_LIT(KFortezzaInvalidSig, "invalidSig.crt");
			//next one just tests CRL-related errors, so should validate OK
			_LIT(KFortezzaKeyCompromise, "KeyCompromise.crt");
			_LIT(KFortezzaBadSubjName1, "BadSubjName1.crt");
			_LIT(KFortezzaBadSubjName2, "BadSubjName2.crt");
			_LIT(KFortezzaInvalidConstraints, "InvalidConstraints.crt");
		//other sub-hierachies
		_LIT(KFortezzaTestCA2, "testCA2.crt");
			_LIT(KFortezzaTestSubCA2, "testSubCA.crt");
				_LIT(KFortezzaPathLen, "pathLen.crt");
		_LIT(KFortezzaBadCA1, "BadCA1.crt");
			_LIT(KFortezzaInvalidCAUser1, "InvalidCAUser1.crt");
		_LIT(KFortezzaBadCA2, "BadCA2.crt");
			_LIT(KFortezzaInvalidCAUser2, "InvalidCAUser2.crt");
//2nd hierachy
_LIT(KFortezzaPAA2, "rn_paa.crt");
	_LIT(KFortezzaPCA2, "rn_pca.crt");
		_LIT(KFortezzaCA2, "rn_ca.crt");
			_LIT(KFortezzaCA2EE1, "rn_user1.crt");		
			_LIT(KFortezzaCA2BadAltName, "rn_BadAltName.crt");		
			_LIT(KFortezzaCA2BadPolicy, "rn_BadPolicy.crt");

//3rd hierachy
_LIT(KMedRootCA, "1A.cer");
	_LIT(KMedCA1, "24.cer");
		_LIT(KMedCA1EE1, "11C5.cer");
		_LIT(KMedCA1EE2, "139F.cer");

//cross certificate
_LIT(KFortezzaX22RNCrossCert, "rn_cross.crt");

//FINEID certificates
_LIT(KFINEIDCitizenCA, "citizen-ca.crt");
	_LIT(KFINEIDLipponenSign, "lipponen-sign.crt");
	_LIT(KFINEIDLipponenEncrypt, "lipponen-encrypt.crt");
_LIT(KFINEIDTestCA3, "finsign-test3-ca.crt");
	_LIT(KFINEIDHelsinkiSign, "helsinki-sign.crt");
	_LIT(KFINEIDHelsinkiEncrypt, "helsinki-encrypt.crt");

//apache
_LIT(KApacheRSARoot, "ca-rsa.crt");
	_LIT(KApacheRSAServer, "rsa.crt");
_LIT(KApacheDSARoot, "ca-dsa.crt");
	_LIT(KApacheDSAServer, "dsa.crt");

//policy testing
_LIT(KX22Policy, "2.16.840.1.101.2.1.11");
_LIT(KRNPolicy, "2.16.840.1.101.2.1.11.2");
//_LIT(KExtraBogusPolicy, "2.16.840.1.101.2.1.11.4");
_LIT(KExtraBogusPolicy, "bogus!!");

class CTestChain : public CBase
	{
public:
	~CTestChain();
	CTestChain();
	virtual void ConstructL();
	CDesCArray* iServerCerts;
	CDesCArray* iRootCerts;
	CDesCArray* iExtraCerts;
	};
			
class CTestChainBuild : public CTestChain
	{
public:
	static CTestChainBuild* NewL();
	static CTestChainBuild* NewLC();
	~CTestChainBuild();
	HBufC* iCorrectChain;
	};

class CTestChainValidate : public CTestChain
	{
public:
	static CTestChainValidate* NewL();
	static CTestChainValidate* NewLC();
	void ConstructL();
	~CTestChainValidate();
	TValidationStatus* iError;
	CArrayFixFlat<TValidationStatus>* iWarnings;
	CDesCArray* iPolicies;
	};

class TestSpec
	{
public:
	TestSpec();
//chain build tests
	CTestChainBuild*  TestBuildSpec1();
	CTestChainBuild*  TestBuildSpec2();
	CTestChainBuild*  TestBuildSpec3();
	CTestChainBuild*  TestBuildSpec4();
	CTestChainBuild*  TestBuildSpec5();
	CTestChainBuild*  TestBuildSpec6();

	CTestChainBuild*  TestBuildSpec7();
	CTestChainBuild*  TestBuildSpec8();
	CTestChainBuild*  TestBuildSpec9();
	CTestChainBuild*  TestBuildSpec10();
	CTestChainBuild*  TestBuildSpec11();
	CTestChainBuild*  TestBuildSpec12();

//chain validate tests
//Fortezza tests
	CTestChainValidate*  TestValidateSpec1();
	CTestChainValidate*  TestValidateSpec2();
	CTestChainValidate*  TestValidateSpec3();
	CTestChainValidate*  TestValidateSpec4();
	CTestChainValidate*  TestValidateSpec5();
	CTestChainValidate*  TestValidateSpec6();
	CTestChainValidate*  TestValidateSpec7();
	CTestChainValidate*  TestValidateSpec8();
	CTestChainValidate*  TestValidateSpec9();
	CTestChainValidate*  TestValidateSpec10();
	CTestChainValidate*  TestValidateSpec11();
	CTestChainValidate*  TestValidateSpec12();
	CTestChainValidate*  TestValidateSpec13();
	CTestChainValidate*  TestValidateSpec14();
	CTestChainValidate*  TestValidateSpec15();
	CTestChainValidate*  TestValidateSpec16();
	CTestChainValidate*  TestValidateSpec17();
//lloyds chain
	CTestChainValidate*  TestValidateSpec18();
//test case where only one self signed trusted cert is supplied
	CTestChainValidate*  TestValidateSpec19();
//test case where only one self signed untrusted cert is supplied
	CTestChainValidate*  TestValidateSpec20();
//test case where no certs are supplied (this one should leave)
	CTestChainValidate*  TestValidateSpec21();
//test case for chain using RSA+SHA1	
	CTestChainValidate*  TestValidateSpec22();
	CTestChainValidate*  TestValidateSpec23();
//FINEID test cases
	CTestChainValidate*  TestValidateSpec24();
	CTestChainValidate*  TestValidateSpec25();
	CTestChainValidate*  TestValidateSpec26();
	CTestChainValidate*  TestValidateSpec27();
//test warning generated when root cert not self signed
	CTestChainValidate*  TestValidateSpec28();
//test case with Apache default certs
	CTestChainValidate*  TestValidateSpec29();
	CTestChainValidate*  TestValidateSpec30();
//policy processing tests
	CTestChainValidate*  TestValidateSpec31();
	CTestChainValidate*  TestValidateSpec32();
	CTestChainValidate*  TestValidateSpec33();
	CTestChainValidate*  TestValidateSpec34();
	CTestChainValidate*  TestValidateSpec35();
	};


