/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include "refcafhelper.h"
#include <caferr.h>


EXPORT_C MCAFHelperInterface* CReferenceCafHelper::NewCafHelperImplL()
    {
    return new (ELeave) CReferenceCafHelper;
    }

CReferenceCafHelper::CReferenceCafHelper()
	{
	}

EXPORT_C TInt CReferenceCafHelper::HandleCAFErrorL(TInt /*aError*/, const TDesC& /*aFileName*/)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CReferenceCafHelper::HandleCAFErrorL(TInt /*aError*/, RFile& /*aFileHandle*/)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CReferenceCafHelper::HandleCAFErrorL(TInt /*aError*/, const TDesC8& /*aHeaderData*/)
	{
	return KErrNotSupported;
	}
	
EXPORT_C void CReferenceCafHelper::Release()
	{
	delete this;
	}

CReferenceCafHelper::~CReferenceCafHelper()
	{
	}
	
