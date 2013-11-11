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
#ifndef CRYPTODRIVER_H
#define CRYPTODRIVER_H

#include <e32cmn.h>
#include <e32ver.h>
#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

/**
User interface for crypto hw
*/
class RCryptoDriver : public RBusLogicalChannel
    {
public:
    /**
    Structure for holding driver capabilities information
    */
    class TCaps
        {
    public:
        TVersion iVersion;
        };

    /**
    Structure for holding driver configuration data
    */
    class TConfig
        {
    public:
        TInt iFakeDriverSetting;
        };
    /**
    Typedef used for passing TConfig structure to GetConfig and SetConfig APIs
    */
    typedef TPckgBuf<TConfig> TConfigBuf;

    /**
    Structure for holding h/w version information
    */
    class THwVersions
        {
    public:
		TUint32 iRngHwVersion; 		///< RNG h/w version number
		TUint32 iDes3DesHwVersion; 	///< 3DES h/w version number
		TUint32 iSha1Md5HwVersion; 	///< SHA1 h/w version number
		TUint32 iAesHwVersion; 		///< AES h/w version number
		TUint32 iPkaHwVersion; 		///< PKA h/w version number
        };
    typedef TPckgBuf<THwVersions> THwVersionsBuf;

public:
    IMPORT_C TInt Open();
    IMPORT_C TInt GetHwVersions(THwVersionsBuf& aHwVersionsBuf);

    IMPORT_C TInt GetConfig(TConfigBuf& aConfig);
    IMPORT_C TInt SetConfig(const TConfigBuf& aConfig);

    inline static const TDesC& Name();
    inline static TVersion VersionRequired();

	/**
	   Fill buffer with random data
	   Only one "random" request may be pending at any time.

	   @param aStatus 	The request to be signalled when the data has been received.
	   					The result value will be set to KErrNone on success;
						or set to one of the system wide error codes when an error occurs.

	   @param aData 	Fills the descriptor up to its current length with
	   					random data. Any existing contents are lost.
	*/
	IMPORT_C void Random(TRequestStatus& aStatus, TDes8& aDestination);
	/**
	   Causes the current Random request to cancel synchronously.
	*/
    IMPORT_C void RandomCancel();

	enum TChainingMode {EEcbMode, ECbcMode, ECntrMode};
	/**
	   @param aEncrypt ETrue for encryption
	   @param aMode See TChainingMode
	   @param aKey 	Must be one of the following lengths - 128, 192 or 256 bits (16, 24 or 32 bytes).
	   @param aIV Initialisation Vector, Length must be, 0 for ECB mode, or 16 bytes (all other mdoes)
	*/ 
	IMPORT_C TInt SetAesConfig(TBool aEncrypt, TChainingMode aMode, const TDesC8& aKey, const TDesC8& aIV);

	/**
	   Any length of data may be written, but the h/w will only
	   process the data in multiples of 16 bytes. Any remainder will
	   be buffered pending future writes.

	   Padding is NOT done by this function.
	   
	   Output 
	   
	   @param aStatus
	   @param aBuffer
	*/ 
	IMPORT_C void AesWrite(TRequestStatus& aStatus, TDesC8& aBuffer);

	/**
	   Causes the current "to hw" requests to cancel synchronously.
	*/
    IMPORT_C void AesCancelWrite();

	/**
	   The destination buffer is overwritten. This call will block
	   until the specified number of bytes have been read (the max
	   length of aBuffer).

	   The length is not required to be a multiple of the block size
	   (16 bytes), but note that written data is only processed in
	   multiples of the block size.

	   Data is appended to the supplied buffer.
	   
	   @param aStatus
	   @param aBuffer
	   @param aLength
	*/ 
	IMPORT_C void AesRead(TRequestStatus& aStatus, TDes8& aBuffer, TUint32 aLenth);

	/**
	   Causes the current "from hw" requests to cancel synchronously.
	*/
    IMPORT_C void AesCancelRead();


private:
    /**
    Enumeration of Control messages.
    */
    enum TControl
        {
		EGetHwVersions,
		EAesSetConfig,
        EGetConfig,
        ESetConfig
        };

    /**
    Enumeration of Request messages.
    */
    enum TRequest
        {
		ERandom,
		EAesWrite,
		EAesRead,
        ENumRequests,
        EAllRequests = (1<<ENumRequests)-1
        };

    /**
    Structure for holding driver configuration data
    */
    class TAesConfig
        {
    public:
		TBool iEncrypt;
		TChainingMode iMode;
		const TDesC8 *iKey;
		const TDesC8 *iIV;
        };
    typedef TPckgBuf<TAesConfig> TAesConfigBuf;

    // Kernel side LDD channel is a friend
    friend class DCryptoLddChannel;
    friend class DLddChanAes;
    };


/**
  Returns the driver's name
*/
inline const TDesC& RCryptoDriver::Name()
    {
    _LIT(KDriver1Name,"crypto");
    return KDriver1Name;
    }

/**
  Returns the version number of the driver
*/
inline TVersion RCryptoDriver::VersionRequired()
    {
    const TInt KMajorVersionNumber=1;
    const TInt KMinorVersionNumber=0;
    const TInt KBuildVersionNumber=KE32BuildVersionNumber;
    return TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    }


#endif
