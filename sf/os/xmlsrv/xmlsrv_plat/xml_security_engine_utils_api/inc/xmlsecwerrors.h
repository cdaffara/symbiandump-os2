/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Error codes definitions used in XMLSec Wrapper
*
*/







#ifndef XMLSECW_ERROR_H
#define XMLSECW_ERROR_H

#include <e32base.h>

/**
 * Error during component initialization 
 */
const TInt KErrInit = -32525;
/**
 * Error during initialization of key manager
 */
const TInt KErrKeyManagerInit = -32526;
/**
 * Template is not set or thera are errors in template
 */
const TInt KErrTemplate = -32528;
/**
 * Key is not valid (f.e. size is wrong)
 */
const TInt KErrKey = -32529;
/**
 * Cert is not valid
 */
const TInt KErrCert = -32530;
/**
 * Error during encryption processing 
 * (i.e. template use functions that is not supported)
 */
const TInt KErrEncrypt = -32531;
/**
 * Error during decryption processing 
 * (i.e. encryption was modified, wrong data is passed as input)
 */
const TInt KErrDecrypt = -32532;
/**
 * Error during signing processing 
 * (i.e. template use functions that is not supported or
 *  signing document is not correct)
 */
 const TInt KErrSign = -32535;
/**
 * Error during verify processing 
 * (i.e. template use functions that is not supported or
 *  document that is verified is not correct)
 */
const TInt KErrVerify = -32536;
/**
 * Id namespace used to create multisign template
 * is unknown.
 */
const TInt KErrIdUndefineNS = -32537;
/**
 * Wrong input parameters.
 */
const TInt KErrWrongParameter = -32544;
#endif // XMLSECW_ERROR_H
