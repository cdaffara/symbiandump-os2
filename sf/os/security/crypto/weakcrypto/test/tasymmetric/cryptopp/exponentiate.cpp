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

#include "modarith.h"
#include "nbtheory.h"
#include <iostream.h>

USING_NAMESPACE(CryptoPP)

void CheckEPOCDecrypt(Integer& e, Integer& p, Integer& q, Integer& cipher, Integer& expectedPlaintext);

void TestRSASigning()
{
	const Integer m("AA36ABCE88ACFDFF55523C7FC4523F90EFA00DF3774A259F2E62B4C5D99CB5ADB300A0285E5301930E0C70FB6876939CE616CE624A11E0086D341EBCACA0A1F5h");
	const Integer d("0A033748626487695F5F30BC38B98B44C2CD2DFF434098CD20D8A138D090BF64797C3FA7A2CDCB3CD1E0BDBA2654B4F9DF8E8AE59D733D9F33B301624AFD1D51h");
	const Integer e(17);

//	Take EPOC padded plain text, sign and verify
	const Integer plain("01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0054859B342C49EA2Ah");
	const Integer expectedSig("786C08484F59280886A19919BECA53711819B2D2DD82D702E2F6DE991DC89CB968C01B1315A742B5BE783A781B5B1286F46DAFE29DE675C64A3BA4781DEF8F19h");
	
//	Private Encrypt (Sign) signedText = (digest ^ d) mod m
	Integer cipher_privateEncrypt = a_exp_b_mod_c(plain, d, m);
	assert(cipher_privateEncrypt==expectedSig);
		
//	Public Decrypt (Decrypt) = (signedText ^ e) mod m
	Integer verified = a_exp_b_mod_c(cipher_privateEncrypt, e, m);
	assert(verified==plain);
		
//	Now reverse operations (as in previous bugged asymmetric) and check results...
//	Do a private decrypt for bad signing
	Integer badSign = a_exp_b_mod_c(plain, d, m);
	assert(badSign==expectedSig);

//	And verify (public encryption)
	Integer badVerify = a_exp_b_mod_c(badSign, e, m);
	assert(badVerify==plain);

}

void TestRSADecryptValues()
{
//	First test a good set
/*	<modulus>BBF82F090682CE9C2338AC2B9DA871F7368D07EED41043A440D6B6F07454F51FB8DFBAAF035C02AB61EA48CEEB6FCD4876ED520D60E1EC4619719D8A5B8B807FAFB8E0A3DFC737723EE6B4B7D93A2584EE6A649D060953748834B2454598394EE0AAB12D7B61A51F527A9A41F6C1687FE2537298CA2A8F5946F8E5FD091DBDCB</modulus>
	<P>EECFAE81B1B9B3C908810B10A1B5600199EB9F44AEF4FDA493B81A9E3D84F632124EF0236E5D1E3B7E28FAE7AA040A2D5B252176459D1F397541BA2A58FB6599</P>
	<Q>C97FB1F027F453F6341233EAAAD1D9353F6C42D08866B1D05A0F2035028B9D869840B41666B42E92EA0DA3B43204B5CFCE3352524D0416A5A441E700AF461503</Q>
	<dP>54494CA63EBA0337E4E24023FCD69A5AEB07DDDC0183A4D0AC9B54B051F2B13ED9490975EAB77414FF59C1F7692E9A2E202B38FC910A474174ADC93C1F67C981</dP>
	<dQ>471E0290FF0AF0750351B7F878864CA961ADBD3A8A7E991C5C0556A94C3146A7F9803F8F6F8AE342E931FD8AE47A220D1B99A495849807FE39F9245A9836DA3D</dQ>
	<qInv>B06C4FDABB6301198D265BDBAE9423B380F271F73453885093077FCD39E2119FC98632154F5883B167A967BF402B4E9E2E0F9656E698EA3666EDFB25798039F7</qInv>
	<plaintext>D436E99569FD32A7C8A05BBC90D32C49</plaintext>
	<ciphertext>4B9C35BC3CA99B3908EF91C91F4D661544B5462CB3079D7B7A610B90039602F080417B049B7F31BAF16A87B59986620EC1BBD791AD30774309C05F0AFA24B0BF1EA1FEAA3A8EFA2C24F5626B8A9C9A157C1018DA54D314E728E2DB75E84FB5E99082561D904139B95C4DA70A5AB6412527B97CEDA04C6FB00BE1E44537706FF2</ciphertext>
*/
	Integer e1 = Integer(17);
	Integer p1 = Integer("EECFAE81B1B9B3C908810B10A1B5600199EB9F44AEF4FDA493B81A9E3D84F632124EF0236E5D1E3B7E28FAE7AA040A2D5B252176459D1F397541BA2A58FB6599h");
	Integer q1 = Integer("C97FB1F027F453F6341233EAAAD1D9353F6C42D08866B1D05A0F2035028B9D869840B41666B42E92EA0DA3B43204B5CFCE3352524D0416A5A441E700AF461503h");
	Integer cipher1 = Integer("4B9C35BC3CA99B3908EF91C91F4D661544B5462CB3079D7B7A610B90039602F080417B049B7F31BAF16A87B59986620EC1BBD791AD30774309C05F0AFA24B0BF1EA1FEAA3A8EFA2C24F5626B8A9C9A157C1018DA54D314E728E2DB75E84FB5E99082561D904139B95C4DA70A5AB6412527B97CEDA04C6FB00BE1E44537706FF2h");
	Integer ePlaintext1 = Integer("D436E99569FD32A7C8A05BBC90D32C49h"); 

	CheckEPOCDecrypt(e1, p1, q1, cipher1, ePlaintext1); 

//	Now test a bad set
/*
	<input>123456789ABCDEF123456789ABCDEF</input>
	<P>ED49CE92ABF0509006E412AC7F28EEA7626107C3F2456FA18E6513153D6F6003</P>
	<Q>ED12B5B118CBA1154DC24AE05AAB1E3A33B5E47A3715861697498824E5BCA331</Q>
	----------- e -----------
01 00 01 
	 ----------- P -----------
ed 49 ce 92 ab f0 50 90 06 e4 12 ac 7f 28 ee a7 62 61 07 c3 f2 45 6f a1 8e 65 13 15 3d 6f 60 03 
	 ----------- Q -----------
ed 12 b5 b1 18 cb a1 15 4d c2 4a e0 5a ab 1e 3a 33 b5 e4 7a 37 15 86 16 97 49 88 24 e5 bc a3 31 
	 ----------- DP -----------
dc e0 33 15 7c 4e f9 ee d7 3b 3d d6 ab 97 22 6f 54 b8 15 4e 64 1a 16 4b 40 0c fd b2 6b b9 5c 99 
	 ----------- DQ -----------
27 65 a3 92 11 a7 cd 56 27 fc f0 ca 85 e6 c7 f8 2e 1d bd 21 5e 44 12 6b 70 aa c4 98 35 21 7b 11 
	 ----------- QInv -----------
76 2f 0a 8c fa 16 d8 a6 df 53 0b 1c fa 5f ff 4c 55 a4 4f 52 e1 b6 28 89 1f c9 c9 10 1c d9 73 2d 
	 ----------- N -----------
db be ab d7 a5 8e c2 a8 83 17 83 86 9a 2e 07 12 03 1a 5c 0f 87 a8 4b d3 fe 3c 21 91 df 49 4e 2b e4 fa 8d a8 65 e5 c8 c7 49 4d fa c5 5c 6a d2 fc e6 96 2e 3b de 1b 38 28 94 93 04 47 dd a7 49 93 
	 ----------- ciphertext -----------
69 3a 72 26 39 cc 70 9c 89 ac dc 24 8c 52 ef 01 dd f7 5a c3 8e bf f9 8d eb ed 25 5e 11 f0 d9 b1 43 de 14 97 d2 34 65 a6 2c 11 13 6e f8 c4 37 81 01 d5 8e 8d ea 0b 33 f2 9f e2 df 68 c4 c7 34 2c 

*/
//	Two primes: p, q
	Integer e2 = Integer(65537);
	Integer p2 = Integer("ED49CE92ABF0509006E412AC7F28EEA7626107C3F2456FA18E6513153D6F6003h");
	Integer q2 = Integer("ED12B5B118CBA1154DC24AE05AAB1E3A33B5E47A3715861697498824E5BCA331h");
	Integer cipher2 = Integer("693A722639CC709C89ACDC248C52EF01DDF75AC38EBFF98DEBED255E11F0D9B143DE1497D23465A62C11136EF8C4378101D58E8DEA0B33F29FE2DF68C4C7342Ch");
	Integer ePlaintext2 = Integer("123456789ABCDEF123456789ABCDEFh"); 
	
	CheckEPOCDecrypt(e2, p2, q2, cipher2, ePlaintext2);

/*
<modulus>EF6419DC54EC49B7D0524BA675727F6D895A66A9940F3C76B6220A5B9073357D70702C9FC2D6ECA41448356CE562F7FFAF1DA64BA947274BA0D372F5866B69CB</modulus>
<P>FB4F79F4E8C816B816A817120901AED45D0FD72F3DC4BA6946F790776E8A5845</P>
<Q>F3DBAF51B232E334964581AE27DE17BFF90A66AE84C2BE95574082880BFF82CF</Q>
<dP>63D8A1C5B22EBD080AC861D2228DEE9E251344155ADF2C88E34F3CB096D49459</dP>
<dQ>F145A87EC23B0B059AB08690132DF07DA61F9E5C894A4D5A610B989A9694658D</dQ>
<qInv>4B8869676360EDC92F0B02F0B93580A570686E7EA3C7D39A5E572AB79314CCBD</qInv>
<ciphertext>2EAEAE1F07AAD1D3A14C2178397DFECD91C92E963511BE5CDDE8BCA79B47ECEF68F8DD2F8240DE2E05E90E2A75FAA6495CE903DE413D332CFDD2DD83BC8244C5</ciphertext>
<plaintext>123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF</plaintext>
*/
	Integer e3 = Integer(65537);
	Integer p3 = Integer("FB4F79F4E8C816B816A817120901AED45D0FD72F3DC4BA6946F790776E8A5845h");
	Integer q3 = Integer("F3DBAF51B232E334964581AE27DE17BFF90A66AE84C2BE95574082880BFF82CFh");
	Integer cipher3 = Integer("2EAEAE1F07AAD1D3A14C2178397DFECD91C92E963511BE5CDDE8BCA79B47ECEF68F8DD2F8240DE2E05E90E2A75FAA6495CE903DE413D332CFDD2DD83BC8244C5h");
	Integer ePlaintext3 = Integer("123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEFh");

	CheckEPOCDecrypt(e3, p3, q3, cipher3, ePlaintext3);
}

void CheckEPOCDecrypt(Integer& e, Integer& p, Integer& q, Integer& cipher, Integer& expectedPlaintext)
{
//////////////////////////////////
//	Key generation
//////////////////////////////////

//	calculate n = p * q 
	Integer n=p*q;

	Integer r1 = p;
    r1-=1;		//	r1 = p-1
    
	Integer r2=q;
    r2-=1;		//	r2 = q-1
    
	Integer r0=r1;
    r0*=r2;	//	r0 = (p-1)(q-1)

//	e * d = 1 mod ((p-1)(q-1)) 
//	d = e^(-1) mod ((p-1)(q-1))
//! calculate multiplicative inverse of *this mod n
//	Integer InverseMod(const Integer &n) const;
	Integer d = e;
	d = d.InverseMod(r0);

//	calculate dP = d mod (p-1) 
	Integer dP=d;
	dP%=r1;

//	calculate dQ = d mod (q-1) 
	Integer dQ=d;
	dQ%=r2;

//	calculate inverse of qInv = q^(-1)mod(p)
    Integer qInv=q;
	qInv = qInv.InverseMod(p);


//////////////////////////////////
//	Decryption
//////////////////////////////////

//	m1 = c^(dP) mod(p)
	Integer m1 = ModularExponentiation((cipher % p), dP, p);
	
//	m2 = c^(dQ) mod(Q)
	Integer m2 = ModularExponentiation((cipher % q), dQ, q);

//	Calculate CRT
//	h = (m1-m2) qInv mod(p)	
	Integer plainRes = m1 - m2;
	plainRes *= qInv;
	plainRes %= p;

//	output = m2 + q * plainRes	
	plainRes *= q;
	plainRes += m2;

//	assert(expectedPlaintext==plainRes);	//	Not true because ciphertext includes
}											//	padding and plaintext doesn't. Though
											//	it's possible to compare the first chunk
											//	of plainRes with expectedPlaintext

int main(int argc, char** argv)
{
//	TestRSASigning();
	TestRSADecryptValues();
	return 0;
}

