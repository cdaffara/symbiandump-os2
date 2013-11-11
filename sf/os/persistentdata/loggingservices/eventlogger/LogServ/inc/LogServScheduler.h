// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVSCHEDULER_H__
#define __LOGSERVSCHEDULER_H__

// System includes
#include <e32base.h>

/**
@internalComponent
*/
class CLogActiveScheduler : public CActiveScheduler
	{
public:
	void Error(TInt aError) const;
	};


#endif
