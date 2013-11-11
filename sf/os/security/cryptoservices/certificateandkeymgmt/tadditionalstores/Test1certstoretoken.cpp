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


#include "test1certstoretoken.h"
#include "test1certstore.h"

_LIT(KName1, "Test store 1");
_LIT(KName2, "Test store 2");
#ifdef CERTSTORE_HARDWARE_SIM
	_LIT(KTest1CertStoreFullPath, "\\system\\data\\test1simcertstore.dat");
	_LIT(KTest2CertStoreFullPath, "\\system\\data\\test2simcertstore.dat");
#else
	#ifdef CERTSTORE_HARDWARE_WIM
		_LIT(KTest1CertStoreFullPath, "\\system\\data\\test1wimcertstore.dat");
		_LIT(KTest2CertStoreFullPath, "\\system\\data\\test2wimcertstore.dat");
	#else
		#ifdef CERTSTORE_HARDWARE_UICC
			_LIT(KTest1CertStoreFullPath, "\\system\\data\\test1uicccertstore.dat");
			_LIT(KTest2CertStoreFullPath, "\\system\\data\\test2uicccertstore.dat");
 		#else
			#ifdef CERTSTORE_DEVICE_IMMUTABLE
				_LIT(KTest1CertStoreFullPath, "\\system\\data\\test1deviceimmutablecertstore.dat");
				_LIT(KTest2CertStoreFullPath, "\\system\\data\\test2deviceimmutablecertstore.dat");
			#else
				_LIT(KTest1CertStoreFullPath, "\\system\\data\\test1certstore.dat");
				_LIT(KTest2CertStoreFullPath, "\\system\\data\\test2certstore.dat");
			#endif
 		#endif
	#endif
#endif


CTest1CertStoreToken* CTest1CertStoreToken::New(CTest1CertStoreTokenType::TTokenId aTokenId,
												CTest1CertStoreTokenType& aFileCertStoreTokenType)
	{
	return new CTest1CertStoreToken(aTokenId, aFileCertStoreTokenType);
	}

CTest1CertStoreToken::~CTest1CertStoreToken()
	{
	}

MCTTokenType& CTest1CertStoreToken::TokenType()
	{
	return iTokenType;
	}

const TDesC& CTest1CertStoreToken::Label()
	{
	switch (iTokenId)
		{
	case CTest1CertStoreTokenType::ETest1CertStore:
		return KName1;

	case CTest1CertStoreTokenType::ETest2CertStore:
		return KName2;

	default:
		return KNullDesC;
		}
	}

const TDesC& CTest1CertStoreToken::Information(TTokenInformation)
	{
	_LIT(KAnswer, "I Know Nothing");
	return KAnswer;
	}

TCTTokenHandle CTest1CertStoreToken::Handle()
	{
	return TCTTokenHandle(iTokenType.Type(), iTokenId);
	}

void CTest1CertStoreToken::DoRelease()
	{
	delete this;
	}

TInt& CTest1CertStoreToken::ReferenceCount()
	{
	return iRefCount;
	}

void CTest1CertStoreToken::DoGetInterface(TUid aRequiredInterface,
										  MCTTokenInterface*& aReturnedInterface,
										  TRequestStatus& aStatus)
	{
	RFs& fs = static_cast<CCTTokenType&>(TokenType()).Fs();
	MCTTokenInterface* result = 0;
	TInt error = KErrNone;

	//gets the system drive
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());	
	TBuf<128> fullPath (driveName);
	TInt* refCountInterface = &iRefCountInterface;
	
	if (iTokenId == CTest1CertStoreTokenType::ETest1CertStore)
		{
		fullPath.Append(KTest1CertStoreFullPath);
		refCountInterface = &iRefCountInterface;
		}
		
	// This is mainly used for testing and also as an example on how to manage
	// multiple tokens within one TokenType.
	else if (iTokenId == CTest1CertStoreTokenType::ETest2CertStore)
		{
		fullPath.Append(KTest2CertStoreFullPath);
		refCountInterface = &iRefCountInterface2;
		}
	else
		{
		// If not certstore 1 or 2, then we complete with bad name.
		TRequestStatus* r = &aStatus;
		User::RequestComplete(r, KErrBadName);
		return;
		}

	switch (aRequiredInterface.iUid)
		{
	case KInterfaceCertStore:
		{
		if (!iFileCertStore)
			{
			TRAP(error, iFileCertStore = CTest1CertStore::NewL(fs, *this, fullPath, 
				EFileShareReadersOnly));
			iFileMode = EFileRead;
			}
		MCTCertStore* temp = iFileCertStore;
		result = temp;
		}
		break;

	case KInterfaceWritableCertStore:
		{
		if (!iFileCertStore)
			{
			TRAP(error, iFileCertStore = CTest1CertStore::NewL(fs, *this, fullPath, EFileWrite));
			iFileMode = EFileWrite;
			}
		if (iFileMode == EFileRead)
			{
			error = KErrInUse;
			}
		else
			{
			MCTWritableCertStore* temp = iFileCertStore;
			result = temp;
			}
		}
		break;

	default:
		error = KErrNotSupported;
		break;
		}

	if (error == KErrNotSupported)
		{
		Release();
		}
	else if ((error != KErrNone) || !result)
		{
		Release();
		}
	else
		{
		(*refCountInterface)++;
		aReturnedInterface = result;
		}

	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, error);
	}

TBool CTest1CertStoreToken::DoCancelGetInterface()
	{
	return EFalse;
	}

CTest1CertStoreToken::CTest1CertStoreToken(CTest1CertStoreTokenType::TTokenId aTokenId,
										   CTest1CertStoreTokenType& aFileCertStoreTokenType)
	:  iTokenType(aFileCertStoreTokenType), iTokenId(aTokenId)
	{
	}
