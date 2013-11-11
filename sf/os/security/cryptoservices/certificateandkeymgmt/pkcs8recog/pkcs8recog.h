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


/**
 @file
 @internalTechnology 
*/

#ifndef __PKCS8RECOG_H__
#define __PKCS8RECOG_H__

#include <apmrec.h>

/**
 * A recogniser that recognises the following MIME types:
 * application/pkcs8
 * application/pkcs8-encrypted
 */
class CApaPkcs8Recognizer : public CApaDataRecognizerType
	{
public:
	CApaPkcs8Recognizer();

public: // from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;

	static CApaDataRecognizerType* CreateRecognizerL();

private: // from CApaDataRecognizerType
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif

