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
// SCH_EXEC.H
// 
//

#ifndef __SCHEXEC_H__
#define __SCHEXEC_H__

// System includes
#include <e32base.h>
#include <f32file.h>

// Classes refereced
class CSchLogManager;

/** 
@internalComponent
*/
NONSHARABLE_CLASS(CTaskExecutor) : public CActive
	{
public:
	static CTaskExecutor* NewLC(const TDesC& aErrorMessage, 
								const TDesC& aTaskFileName, 
								const TDesC& aClientFileName,
								CSchLogManager& aSchLogManager);
								
	~CTaskExecutor();

private: // Internal construct
	CTaskExecutor(CSchLogManager& aSchLogManager);
	
	void ConstructL(const TDesC& aTaskFileName, 
					const TDesC& aClientFileName, 
					const TDesC& aErrorMessage);

public: // Kick-off method
	void ExecuteL();

private: // From CActive
	void RunL();
	void DoCancel();

private: // Error handling
	void CheckErrorAndLeaveL(TInt aError);
	void SaveExecutionErrorToLog(TInt aError);

private:
	RFs iFsSession;
	RFile iTaskFile;
	// The log manager in case of errors
	CSchLogManager& iSchLogManager;
	// The Log error message
	HBufC* iLogErrorMessage;
	// Name of executor program
	HBufC* iClientFileName;
	// Name of task file
	HBufC* iTaskFileName;

	RProcess iProcess;
	};
	
/**
Streaming operators for TSecurityInfo
@internalComponent
*/	
RWriteStream& operator<<(RWriteStream& aWriteStream, 
						const TSecurityInfo& aSecurityInfo);

/**
Streaming operators for TSecurityInfo
@internalComponent
*/	
RReadStream& operator>>(RReadStream& aReadStream,
						TSecurityInfo& aSecurityInfo);
	


#endif
