/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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



#ifndef COMMONPHONEPARSER_H
#define COMMONPHONEPARSER_H

#include <coemain.h>

class CommonPhoneParser
/**
@deprecated
@see TulPhoneNumberUtils
@note For compatibility with S60 only
*/
    {
public:
	enum TPhoneNumberType
		{
		EPlainPhoneNumber,
		EContactCardNumber,
		EPhoneClientNumber,
		ESMSNumber
		};

    IMPORT_C static TBool ParsePhoneNumber( TDes& aNumber, TInt aType );
    IMPORT_C static TBool IsValidPhoneNumber( const TDesC& aNumber, TInt aType );
    };

#endif      // COMMONPHONEPARSER_H
            
// End of File
