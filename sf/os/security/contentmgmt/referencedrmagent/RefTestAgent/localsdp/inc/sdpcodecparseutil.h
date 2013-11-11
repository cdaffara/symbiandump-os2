/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SdpCodecParseUtil.h
* Part of       : Local SDP Codec
* Interface     : -
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPCODECPARSEUTIL_H
#define CSDPCODECPARSEUTIL_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sdpdefs.h"
#include <e32std.h>
#include "badesca.h"
class CSdpTimeField;
class CSdpBandwidthField;
class CSdpAttributeField;
class CSdpMediaField;	
class CSdpOriginField;
class CUri8;
class CSdpConnectionField;
class CSdpKeyField;
class RReadStream;
class RWriteStream;

// CLASS DECLARATION
/**
 *	Common utilities used in the implementation of the Local SDP codec.
 */
class CSdpCodecParseUtil :public CBase
	{
    public: // Constructors and destructor

		/**
		*	Construct a new, CSdpCodecParseUtil object.
		*	@return The new instance.
		*/
		static CSdpCodecParseUtil* NewL(RStringPool aPool,const TDesC8& aValue,
											  TInt aErrCode);

		/**
		*	Construct a new, CSdpCodecParseUtil object.
		*	@return The new instance.
		*/
		static CSdpCodecParseUtil* NewLC(RStringPool aPool,const TDesC8& aValue,
											  TInt aErrCode);

	    ~CSdpCodecParseUtil();

    public: // New functions

		static TPtrC8 FieldLineValueL(const TDesC8& aValue);
		static RArray<TPtrC8> CheckFieldNameL(RStringPool aPool,
											  TInt aIndex, 
											  const TDesC8& aValue,
											  TInt aErrCode);
		static RArray<TPtrC8> CheckOptionalFieldNameL(RStringPool aPool, 
													TInt aIndex, 
													const TDesC8& aValue,
												    TInt aErrCode);
		TUint ParseSessionVersionL(TInt aErrCode);
		CSdpOriginField* ParseSessionOwnerL(TInt aErrCode);
		const TDesC8& ParseSessionNameL(TInt aErrCode);
		const TDesC8& ParseInformationL(TInt aErrCode);
		CUri8* ParseUriL(TInt aErrCode);
		CDesC8ArraySeg* ParseEmailL(TInt aErrCode);
		CDesC8ArraySeg* ParsePhoneL(TInt aErrCode);
		CSdpConnectionField* ParseConnectionL(TInt aErrCode);
		RPointerArray<CSdpBandwidthField>* ParseBandwidthL(TInt aErrCode);
		RPointerArray<CSdpConnectionField>* ParseConnectionFieldsL(TInt aErrCode);
		RPointerArray<CSdpTimeField>* ParseTimeFieldL(TInt aErrCode);
		const TDesC8& ParseZoneAdjustmentL(TInt aErrCode);
		CSdpKeyField* ParseEncryptionKeyL(TInt aErrCode);
		RPointerArray<CSdpAttributeField>* ParseAttributeFieldL(TInt aErrCode);
		RPointerArray<CSdpMediaField>* ParseMediaLevelL ();
		RArray<TPtrC8> FirstLineArrayElementL(RStringPool aPool,
											  TInt aIndex,
											  TInt aErrCode);

        RArray<TPtrC8>& LineArray();

	private:

		CSdpCodecParseUtil();
		
		void ConstructL(RStringPool aPool,const TDesC8& aValue,TInt aErrCode);

	private: // Data
		RArray<TPtrC8> iLineArray;
		RArray<TPtrC8> iElementArray;
		RStringPool iPool;
		RPointerArray<CSdpTimeField>* iTimeFields;
		RPointerArray<CSdpBandwidthField>* iBandwidthFields;
		RPointerArray<CSdpAttributeField>* iAttributeFields;
		RPointerArray<CSdpMediaField>* iMediaFields;
		RPointerArray<CSdpConnectionField>* iConnectionFields;
		CDesC8ArraySeg* iEmailFields;
		CDesC8ArraySeg* iPhoneFields;
		HBufC8* iToken;
		TPtrC8 iData;
	};

#endif // CSDPCODECPARSEUTIL_H
