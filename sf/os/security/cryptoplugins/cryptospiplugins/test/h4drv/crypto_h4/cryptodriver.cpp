/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/
#include "cryptodriver.h"

/*
  NOTE: The following member functions would normally be exported from a separate client DLL
  but are included inline in this header file for convenience.
*/


/**
  Opens a logical channel to the driver

  @return One of the system wide error codes.
*/
EXPORT_C TInt RCryptoDriver::Open()
    {
    return DoCreate(Name(),VersionRequired(),KNullUnit,NULL,NULL,EOwnerThread);
    }


/**
  Gets the HW versions

  @param aHwVersionsBuf A structure which will be filled with the versions

  @return KErrNone
*/
EXPORT_C TInt RCryptoDriver::GetHwVersions(THwVersionsBuf& aHwVersionsBuf)
    {
    return DoControl(EGetHwVersions,(TAny*)&aHwVersionsBuf);
    }

/**
  Gets the current configuration settings.

  @param aConfig A structure which will be filled with the configuration settings.

  @return KErrNone
*/
EXPORT_C TInt RCryptoDriver::GetConfig(TConfigBuf& aConfig)
    {
    return DoControl(EGetConfig,(TAny*)&aConfig);
    }


/**
  Sets the current configuration settings.

  @param aConfig The new configuration settings to be used.

  @return KErrInUse if there are outstanding data transfer requests.
          KErrArgument if any configuration values are invalid.
          KErrNone otherwise
*/
EXPORT_C TInt RCryptoDriver::SetConfig(const TConfigBuf& aConfig)
    {
    return DoControl(ESetConfig,(TAny*)&aConfig);
    }

EXPORT_C void RCryptoDriver::Random(TRequestStatus& aStatus,TDes8& aBuffer)
    {
    DoRequest(ERandom,aStatus,(TAny*)&aBuffer);
    }


EXPORT_C void RCryptoDriver::RandomCancel()
    {
    DoCancel(1<<ERandom);
    }

#include <e32debug.h>
EXPORT_C TInt RCryptoDriver::SetAesConfig(TBool aEncrypt, TChainingMode aMode, 
										  const TDesC8& aKey, const TDesC8& aIV)
    {
	TAesConfigBuf configBuf;
	TAesConfig &config = configBuf();
	config.iEncrypt = aEncrypt;
	config.iMode = aMode;
	config.iKey = &aKey;
	config.iIV = &aIV;

    return DoControl(EAesSetConfig, (TAny*)&configBuf);
    }

EXPORT_C void RCryptoDriver::AesWrite(TRequestStatus& aStatus, TDesC8& aBuffer)
    {
    DoRequest(EAesWrite, aStatus,(TAny*)&aBuffer);
    }
EXPORT_C void RCryptoDriver::AesCancelWrite()
    {
    DoCancel(1<<EAesWrite);
    }

EXPORT_C void RCryptoDriver::AesRead(TRequestStatus& aStatus, TDes8& aBuffer, TUint32 aLength)
    {
    DoRequest(EAesRead, aStatus,(TAny*)&aBuffer, (TAny*)aLength);
    }

EXPORT_C void RCryptoDriver::AesCancelRead()
    {
    DoCancel(1<<EAesRead);
    }

// End of file
