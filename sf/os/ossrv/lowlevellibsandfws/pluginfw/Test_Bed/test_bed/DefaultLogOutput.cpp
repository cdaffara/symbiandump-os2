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
// This file contains the implementation of the class CDefaultLogOutput
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

#include "DefaultLogOutput.h"

CDefaultLogOutput::CDefaultLogOutput(const TDesC& aTitle) :
iTitle(aTitle)
	{
	// Do nothing
	}

CDefaultLogOutput::~CDefaultLogOutput()
	{
	Close();
	}

void CDefaultLogOutput::OpenL()
	{
	// The directory where the log file will go
	_LIT(KTestBedDirectory, "TestBed");

	User::LeaveIfError(iFLogger.Connect());

	iFLogger.CreateLog(KTestBedDirectory, iTitle, EFileLoggingModeOverwrite);
	iFLogger.SetDateAndTime(EFalse, EFalse);
	}

void CDefaultLogOutput::Write(const TDesC& aOutput)
	{
	iFLogger.Write(aOutput);
	}

void CDefaultLogOutput::Close()
	{
	iFLogger.Close();
	}

