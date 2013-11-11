// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definition of the class CDefaultLogOutput
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

/**
 @test
*/

#ifndef __DEFAULTLOGOUTPUT_H__
#define __DEFAULTLOGOUTPUT_H__

#include <e32base.h>
#include <flogger.h>

#include <ecom/test_bed/dataformat.h>

/**
	@internalAll
	Comments : Default implementation of the MLogOutput interface.  This will be used
	if an alternative is not supplied at construction of the CTestController.
 */

NONSHARABLE_CLASS(CDefaultLogOutput) : public CBase, public MLogOutput
	{
public:
/**
	@fn				CDefaultLogOutput(const TDesC& aTitle)
	Intended Usage	:	Standard constructor
	@leave  		KErrNoMemory
	@since			7.0
	@param			aTitle The title to be used in the log
 */
	
	explicit CDefaultLogOutput(const TDesC& aTitle);

/**
	@fn				~CDefaultLogOutput()
	Intended Usage	:	Standard destructor
	@since			7.0
 */
	
	~CDefaultLogOutput();

/**
	@fn				TInt OpenL()
	Intended Usage	:	Opens the RFileLogger ready for logging
	Error Condition	:	Fails to connect to RFileLogger
	@leave  		@see RFileLogger::Connect()
	@since			7.0
	@pre 			This object is fully constructed
	@post			RFileLogger is opened and ready for logging
 */
	
	void OpenL();

/**
	@fn				void Write(const TDesC& aOutput)
	Intended Usage	:	Outputs the specified message to RFileLogger
	@since			7.0
	@param			aOutput The descriptor to be logged
	@pre 			OpenL has previously been called
	@post			The descriptor has been written to RFileLogger
 */
	
	void Write(const TDesC& aOutput);

/**
	@fn				void Close()
	Intended Usage	:	Closes RFileLogger when logging has completed
	@since			7.0
	@pre 			OpenL has previously been called
	@post			RFileLogger has been closed and this object can be deleted
 */
	
	void Close();

protected:
/** The title which is output to the log file */
	
	const TDesC&	iTitle;
/** Handle to the file logging mechanism for outputting messages */
	
	RFileLogger		iFLogger;
	};

#endif	// __DEFAULTLOGOUTPUT_H__
