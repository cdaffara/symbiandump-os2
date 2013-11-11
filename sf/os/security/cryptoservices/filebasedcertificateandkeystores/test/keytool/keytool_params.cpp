/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "keytool_commands.h"

#ifdef KEYTOOL
_LIT(KDefaultKeyFile,"c:\\newkeys.dat");
#endif // KEYTOOL
CKeyToolParameters::CKeyToolParameters()
	{	
	}
	
CKeyToolParameters* CKeyToolParameters::NewLC()
	{
	CKeyToolParameters* self = new (ELeave) CKeyToolParameters;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}
	
CKeyToolParameters::~CKeyToolParameters()
	{	
	delete iDefault;
	delete iLabel;
	delete iPrivate;
	delete iOwnerType;
	delete iIsDeletable;
	iUIDs.Close();
	
	
	
#ifdef KEYTOOL
	delete iOldKeyFile;
	delete iNewKeyFile;
	delete iAuthExpression;
	delete iPassphrase;	
#endif // KEYTOOL
	}
	
void CKeyToolParameters::ConstructL()
	{
	iAccess = CCTKeyInfo::EExtractable;
	iUsage = EPKCS15UsageSign;
	iCertstoreIndex = -1;
	iKeystoreIndex = -1;
	iPageWise = EFalse;
	iRemoveKey = EFalse;
	iPolicy = ENone;
#ifdef KEYTOOL
	iFreshness = -1;
	iNewKeyFile = KDefaultKeyFile().AllocL();
#endif // KEYTOOL
	}

