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

#ifndef UTRACEMODUTRACETEST_H_
#define UTRACEMODUTRACETEST_H_

// te_moduetracetest.h
//
// Header file for the ost unit test trace module which defines attributes
// to be used in its trace points.

/**
@internalTechnology
@released
*/

#include <opensystemtrace.h>
#include "te_traceglobals.h"


/**
[Definition of what the scope of the module is.
For instance if the Module has the same scope as a binary file then it may simply be enough to say that the trace from this module comes from the binary <binary file name>. However, if this is not the case then a more detailed explanation will need to be given.]
*/
namespace UTraceModUtraceTest
	{
	enum TraceId
	    {
	    /**
	    [Format 0 description]
	    */
	    EFormatTest1 = KInitialClientFormat,

	    /**
	    [Format 1 description]
	    */
	    EFormatTest2,

	    ETraceIdHighWaterMark,
	    };
    __ASSERT_COMPILE(ETraceIdHighWaterMark <= KMaxTraceId + 1);

} // end of namespace OstModOstTest

#endif /*UTRACEMODUTRACETEST_H_*/

