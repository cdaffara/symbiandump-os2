/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* This Test step formats the specified drive
*
*/



/**
 @file TEFFormatDrive.h
*/

#if (!defined __TEF_FORMAT_DRIVE_H__)
#define __TEF_FORMAT_DRIVE_H__


#include <test/testexecutestepbase.h>
#include <f32file.h>

class CTEFFormatDrive : public CTestStep
{
public:
	CTEFFormatDrive();
public:
	virtual	TVerdict	doTestStepL();
	virtual	TVerdict	doTestStepPreambleL();
	virtual	TVerdict	doTestStepPostambleL();

private:
	void ReadDriveNameL(TDriveUnit &aDriveName);
	TInt PreParseFileNameL(const TDesC& aInputFileName, HBufC*& aOutputFileName);	
	TInt FormatDiskL(TPtrC aDrivePath, TPtrC aFormatType, TBool& aFormatFlag);	
private:
	RFs		iFs;

};

_LIT(KFormatDrive,"FormatDrive");

#endif
