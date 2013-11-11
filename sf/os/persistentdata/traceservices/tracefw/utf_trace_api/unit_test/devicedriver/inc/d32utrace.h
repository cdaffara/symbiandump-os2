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
// d32btrace.h
// 
//

#ifndef D32UTRACE_H
#define D32UTRACE_H

#include <e32cmn.h>
#include <e32ver.h>

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif


/**
Interface to the utrace api.

@internalTechnology
*/
class RUTrace : public RBusLogicalChannel
	{
public:

#ifndef __KERNEL_MODE__
	/**
	Open channel to device driver.
	Must be called before any other methods are used.
	@return KErrNone or standard error code.
	*/
	IMPORT_C TInt Open();

	/**
	Close channel to device driver.
	*/
	IMPORT_C void Close();

	IMPORT_C TInt TestUtrace(TInt& aFailed);

#endif

private:
	inline static const TDesC& Name();

	enum TControl
		{
		ETestUTrace,
		};
	friend class DUTraceChannel;
	friend class DUTraceFactory;
	};


inline const TDesC& RUTrace::Name()
	{
	_LIT(KUTraceName,"utrace");
	return KUTraceName;
	}

#endif
