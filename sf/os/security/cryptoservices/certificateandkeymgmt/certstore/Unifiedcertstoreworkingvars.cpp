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


#include "unifiedcertstoreworkingvars.h"
#include "x500dn.h"

CUnifiedCertStoreWorkingVars::~CUnifiedCertStoreWorkingVars()
	{
	iWritableTokenTypes.Close();
	iReadOnlyTokenTypes.Close();
	delete iCertDesC;
	iIssuerNames.Close();
	iParsedIssuerNames.ResetAndDestroy();
	iHashedIssuerNames.ResetAndDestroy();
	}

CUnifiedCertStoreWorkingVars::CUnifiedCertStoreWorkingVars()
	: iCertDes(NULL,0)
	{
	}
