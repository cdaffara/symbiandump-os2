// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines IPSec key stream sink test cases
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TIPSEC_H_
#define TIPSEC_H_

#include <es_sock.h>
#include <in_sock.h>
#include <lib_pfkey.h>

#include "tscafstep.h"

class CScafIpSec: public CScafStep
	{
public:
	CScafIpSec(CScafServer& aParent);
	TVerdict doTestStepPreambleL();
	TVerdict doTestL();
	TVerdict doTestStepPostambleL();
protected:
	void SendKeyAssociationToKeySinkL(TInt aSpi, CKeyStreamSink *aKeyStreamSink);
	void AddAndValidateAssociationsL(CKeyStreamSink *aKeyStreamSink, TInt aSpiBase);
	void ValidateNoAssociationsL(TInt aSpiBase);
	CKeyStreamSink *CreateDefaultKeyStreamSinkLC();
	void InitializeAlgorithmsL(CKeyStreamSink *aKeyStreamSink);
	static void CallValidateSadbL(TInt32 aSpi, TInetAddr &aSourceAddr, TInetAddr &aDestAddr, TBool aPositiveTesting);
protected:
	RSocketServ iSocketServ;
	RConnection iConnection;
	RSocket iClientSocket;
	CKeyStreamSink *iKeyStreamSink;
	TInetAddr iServerAddr;
	TInetAddr iClientAddr;
	HBufC8* iEncryptionKey;
	HBufC8* iAuthenticationKey;
	TInt iAssociationsNumber;
	TBool iAuthenticationUsed;
	TInt iEncryptionAlgorithm;
	TInt iAuthenticationAlgorithm;	
	};
	
class CScafIpSecDecoderIntegration : public CScafIpSec
	{
public:
	CScafIpSecDecoderIntegration(CScafServer& aParent);
	TVerdict doTestL();
	};
	
class CScafIpSecSadbVerification : public CScafIpSec
	{
public:
	CScafIpSecSadbVerification(CScafServer& aParent);
	TVerdict doTestL();
	};	
	
class CScafIpSecSadbVerificationConcurrency : public CScafIpSec
	{
public:
	CScafIpSecSadbVerificationConcurrency(CScafServer& aParent);
	TVerdict doTestL();
	void TestThreadFuncL(TInt aBaseSpi);	
	};	
	
class CScafIpSecNegative : public CScafIpSec
	{
public:
	CScafIpSecNegative(CScafServer& aParent);
	TVerdict doTestL();
	};			
	
class CScafIpSecMultipleSinks: public CScafIpSec
	{
public:
	CScafIpSecMultipleSinks(CScafServer& aParent);
	TVerdict doTestL();
	};	
	
class CScafIpSecAlgorithmChange: public CScafIpSec
	{
public:
	CScafIpSecAlgorithmChange(CScafServer& aParent);
	TVerdict doTestL();
	};	
	
class CScafIpSecSARemoval: public CScafIpSec
	{
public:
	CScafIpSecSARemoval(CScafServer& aParent);
	TVerdict doTestL();
	};		
		
// Test case identifier definition
_LIT(KScafIpSec,"SCAFIpSec");
_LIT(KScafIpSecDecoderIntegration,"SCAFIpSecDecoderIntegration");
_LIT(KScafIpSecSadbVerification, "SCAFIpSecSadbVerification");
_LIT(KScafIpSecSadbVerificationConcurrency, "SCAFIpSecSadbVerificationConcurrency");
_LIT(KScafIpSecNegative,"SCAFIpSecNegative");
_LIT(KScafIpSecMultipleSinks,"SCAFIpSecMultipleSinks");
_LIT(KScafIpSecAlgorithmChange,"SCAFIpSecAlgorithmChange");
_LIT(KScafIpSecSARemoval,"SCAFIpSecSARemoval");

#endif /*TIPSEC_H_*/
