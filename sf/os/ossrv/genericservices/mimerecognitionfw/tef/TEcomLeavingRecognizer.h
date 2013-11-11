// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalComponent - Internal Symbian test code 
*/

#if !defined(__TECOMLEAVINGRECOGNIZER_H__)
#define __TECOMLEAVINGRECOGNIZER_H__

#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

/*
	Recognizer leaving with KErrNoMemory for reproducing the following defect:
	DEF038928 - App Arc server/ Recogniser needs to be more robust. 
*/
class CEcomLeavingApplicationRec : public CApaDataRecognizerType
	{
public:
    CEcomLeavingApplicationRec();
    TUint PreferredBufSize();
    TDataType SupportedDataTypeL(TInt) const;    
    static CApaDataRecognizerType* CreateRecognizerL();
private:
    void DoRecognizeL(const TDesC&, const TDesC8&);
	};

#endif


