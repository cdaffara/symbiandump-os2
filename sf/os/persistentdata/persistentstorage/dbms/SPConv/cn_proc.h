// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCmdProcessor class
// 
//

#ifndef __CN_PROC_H__
#define __CN_PROC_H__

#include <e32base.h>

//Forward declarations
struct TCmdLinePrm;
class RFs;

/**
CCmdProcessor class describes an interface to an object which primary purpose is to
execute requested action, determined by the application's command line arguments.
CCmdProcessor interface implementations have to override pure virtual RunL() method, which
will be called in order command line action to be executed.
@internalComponent
*/
class CCmdProcessor : public CBase
	{
public:
	virtual void RunL() = 0;

protected:
	inline CCmdProcessor(RFs& aFs, const TCmdLinePrm& aCmdLinePrm) :
		iFs(aFs),
		iCmdLinePrm(aCmdLinePrm)
		{
		}

protected:
	RFs&				iFs;
	const TCmdLinePrm&	iCmdLinePrm;

	};

/**
TCmdProcessorFactory class serves itself as a factory for objects which implement
CCmdProcessor interface.
Depending on the requested action (TCmdLinePrm::TAction enum) TCmdProcessorFactory instance
will create an object (which class implements CCmdProcessor interface), capable to perform the
requested operation.
@see CCmdProcessor
@see TCmdLinePrm
@internalComponent
*/
class TCmdProcessorFactory
	{
public:
	static CCmdProcessor* NewLC(RFs& aFs, const TCmdLinePrm& aCmdLinePrm);

	};

#endif//__CN_PROC_H__
