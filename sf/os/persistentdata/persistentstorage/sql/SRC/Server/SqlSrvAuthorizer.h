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

#ifndef __SQLSRVAUTHORIZER_H__
#define __SQLSRVAUTHORIZER_H__

#include <e32std.h>

//Forward declarations
class TSecurityPolicy;

/**
Intefrace class used for asserting the calling application capabilities aginst the TSecurityPolicy parameter
of the Check() function.

@see TSecurityPolicy

@internalComponent
*/
NONSHARABLE_CLASS(MSqlPolicyInspector)
	{
public:
	virtual TBool Check(const TSecurityPolicy& aPolicy) const = 0;
	
	};
	

#endif//__SQLSRVAUTHORIZER_H__
