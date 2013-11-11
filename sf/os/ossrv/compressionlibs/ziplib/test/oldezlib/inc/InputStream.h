
// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _INPUT_STREAM_H_
#define _INPUT_STREAM_H_

#include "LocalTypes.h"

class CInputStream: public CBase
	{
public:

	virtual ~CInputStream() = 0;
	
	virtual TInt Read() = 0;
	virtual TInt Read(TByte*, TUint32) = 0;

	};

#endif /* !_INPUT_STREAM_H_ */
