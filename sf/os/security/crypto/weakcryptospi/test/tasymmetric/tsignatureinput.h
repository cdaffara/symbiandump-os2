/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __TSIGNATUREINPUT_H__
#define __TSIGNATUREINPUT_H__

#include <e32base.h>
#include <hash.h>
/** An abstract class capable of representing a message to be signed or verified. */
class CSignatureInput : public CBase
    {
public:
    virtual void Update(const TDesC8& aInput) = 0;
    virtual TPtrC8 Final(void) = 0;
protected:
	CSignatureInput(void) {}
private:
    CSignatureInput(const CSignatureInput&);
    CSignatureInput& operator=(const CSignatureInput&);
    };

class CHashingSignatureInput : public CSignatureInput
    {
public:
    static CHashingSignatureInput* NewL(CMessageDigest::THashId aHashId);
    static CHashingSignatureInput* NewLC(CMessageDigest::THashId aHashId);
    virtual void Update(const TDesC8& aInput);
    virtual TPtrC8 Final(void);
    ~CHashingSignatureInput(void);
protected:
    CHashingSignatureInput(void);
    void ConstructL(CMessageDigest::THashId aHashId);
private:
    CMessageDigest* iHash;
private:
    CHashingSignatureInput(const CHashingSignatureInput&);
    CHashingSignatureInput& operator=(const CHashingSignatureInput&);
    };

#endif
