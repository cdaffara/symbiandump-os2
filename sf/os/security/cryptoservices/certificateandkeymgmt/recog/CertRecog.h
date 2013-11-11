/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CApaCertificateRecognizer implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#if !defined(__RECTXT_H__)
#define __RECTXT_H__

#include <apmrec.h>



/**
 * A recogniser that recognises the following MIME types:
 * application/vnd.wap.cert-response
 * application/x-x509-ca-cert
 * application/vnd.wap.wtls-ca-certificate
 */
class CApaCertificateRecognizer : public CApaDataRecognizerType
	{
public:
	CApaCertificateRecognizer();

public: // from CApaDataRecognizerType
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL( TInt aIndex ) const;

	static CApaDataRecognizerType* CreateRecognizerL();

private: // from CApaDataRecognizerType
	void DoRecognizeL( const TDesC& aName, const TDesC8& aBuffer );

private:
	void DoRecognizeBufferL(const TDesC8& aBuffer, TBool aIsFile, RFile& aFile);
 	void RecognizeX509CertL( const TDesC8& aBuffer,  TBool aIsFile, RFile& aFile );
 	void RecognizeWTLSCertOrCertResponse( const TDesC8& aBuffer, TBool aIsFile, RFile& aFile);
 	void RecognizeWholeX509CertificateL(const TDesC8& aBuffer);
 	void ReadFileAndRecognizeL(RFile& aFileToRead);

	};



#endif
