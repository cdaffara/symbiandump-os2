// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __EFMDSR_H__)
#define __EFMDSR_H__

class CEFMDSR : public CBase
    {
public:
    CEFMDSR(TUint32, TUint32);
    ~CEFMDSR() {};

	TInt BinaryContent(TDes8 &);
	TUint32 LowUID() const;
	TUint32 HighUID() const;
	TInt Compare(const CEFMDSR &);

private:
	TUint32 ilowuid, ihighuid;

protected:
    };

#endif  //__EFMDSR_H__
