// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include <tulphonenumberutils.h>
#include "commonphoneparser.h"

EXPORT_C TBool CommonPhoneParser::ParsePhoneNumber( TDes& aNumber, TInt aType )
	{ return TulPhoneNumberUtils::Normalize(aNumber, static_cast<TulPhoneNumberUtils::TPhoneNumberType>(aType)); }
	
EXPORT_C TBool CommonPhoneParser::IsValidPhoneNumber( const TDesC& aNumber, TInt aType )
	{ return TulPhoneNumberUtils::IsValid(aNumber, static_cast<TulPhoneNumberUtils::TPhoneNumberType>(aType)); }


//  End of File  
