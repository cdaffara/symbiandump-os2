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

#include <pbebase.h>
#include <pbe.h>

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementL(
	const TDesC& aPassword)
	{
	CPBEncryptElementBase* encrypt = NewPBEncryptElementLC(aPassword);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementLC(
	const TDesC& aPassword)
	{
	CPBEncryptElement* encrypt = CPBEncryptElement::NewLC(aPassword);
	return encrypt;
	}

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementL(
	const TDesC& aPassword, const TPBEncryptParms& aEncryptParms)
	{
	CPBEncryptElementBase* encrypt = NewPBEncryptElementLC(aPassword,aEncryptParms);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementLC(
	const TDesC& aPassword, const TPBEncryptParms& aEncryptParms)
	{
	CPBEncryptElement* encrypt = CPBEncryptElement::NewLC(aPassword,
		aEncryptParms);
	return encrypt;
	}

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementL(
	const TPBEncryptionData& aData)
	{
	CPBEncryptElementBase* encrypt = NewPBEncryptElementLC(aData);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptElementBase* TPBEncryptionFactory::NewPBEncryptElementLC(
	const TPBEncryptionData& aData)
	{
	CPBEncryptElement* encrypt = CPBEncryptElement::NewLC(aData);
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetL(
	const TDesC& aPassword)
	{
	CPBEncryptSetBase* encrypt = NewPBEncryptSetLC(aPassword);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetLC(
	const TDesC& aPassword)
	{
	CPBEncryptSet* encrypt = CPBEncryptSet::NewLC(aPassword);
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetL(
	const TDesC& aPassword, const TPBEncryptParms& aEncryptParms)
	{
	CPBEncryptSetBase* encrypt = NewPBEncryptSetLC(aPassword, aEncryptParms);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetLC(
	const TDesC& aPassword, const TPBEncryptParms& aEncryptParms)
	{
	CPBEncryptSet* encrypt = CPBEncryptSet::NewLC(aPassword, aEncryptParms);
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetL(
	const TPBEncryptionData& aData, const TDesC8& aEncryptedMasterKey)
	{
	CPBEncryptSetBase* encrypt = NewPBEncryptSetLC(aData, aEncryptedMasterKey);
	CleanupStack::Pop();
	return encrypt;
	}

EXPORT_C CPBEncryptSetBase* TPBEncryptionFactory::NewPBEncryptSetLC(
	const TPBEncryptionData& aData, const TDesC8& aEncryptedMasterKey)
	{
	CPBEncryptSet* encrypt = CPBEncryptSet::NewLC(aData, aEncryptedMasterKey);
	return encrypt;
	}
