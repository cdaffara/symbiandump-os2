/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition for the inline functions of RAnalyzeToolMainAllocator.
*
*/


#include "analyzetoolmemoryallocator.h"
  
// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::Codeblocks()
// Acquires reference to process used codeblocks
// -----------------------------------------------------------------------------
//  
inline RArray<TCodeblock>& RAnalyzeToolMainAllocator::Codeblocks()
    {
    return iCodeblocks;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::Mutex()
// Acquires reference to mutex which is used to share resources
// -----------------------------------------------------------------------------
//  
inline RMutex& RAnalyzeToolMainAllocator::Mutex()
    {
    return iMutex;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::ProcessId()
// Acquires the process id
// -----------------------------------------------------------------------------
//
inline TInt RAnalyzeToolMainAllocator::ProcessId()
    {
    return iProcessId;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::AnalyzeTool()
// Acquires the logical channel handle
// -----------------------------------------------------------------------------
//
inline RAnalyzeTool& RAnalyzeToolMainAllocator::AnalyzeTool()
    {
    return iAnalyzeTool;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::LogOption()
// Acquires the iLogOption variable
// -----------------------------------------------------------------------------
//
inline TUint32 RAnalyzeToolMainAllocator::LogOption()
    {
    return iLogOption;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::AllocMaxCallStack()
// Acquires the iAllocMaxCallStack variable
// -----------------------------------------------------------------------------
//
inline TUint32 RAnalyzeToolMainAllocator::AllocMaxCallStack()
    {
    return iAllocMaxCallStack;
    }
// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::FreeMaxCallStack()
// Acquires the iFreeMaxCallStack variable
// -----------------------------------------------------------------------------
//
inline TUint32 RAnalyzeToolMainAllocator::FreeMaxCallStack()
    {
    return iFreeMaxCallStack;
    }

// -----------------------------------------------------------------------------
// RAnalyzeToolMainAllocator::LogFile()
// Acquires the iLogFile variable
// -----------------------------------------------------------------------------
//
inline RATFileLog& RAnalyzeToolMainAllocator::LogFile()
	{
	return iLogFile;
	}
// End of File
