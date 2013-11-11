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
* Description:  Definitions for inline methods of the class RAnalyzeTool.
*
*/


#ifndef __ANALYZETOOL_INL
#define __ANALYZETOOL_INL

#ifndef __KERNEL_MODE__

// ----------------------------------------------------------------------------
// RAnalyzeTool::Open()
// Opens a handle to a analyze tool device driver
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::Open()
	{
	return DoCreate( KAnalyzeToolLddName, 
					 KAnalyzeToolLddVersion(), 
					 KNullUnit, 
					 NULL, 
					 NULL, 
					 EOwnerProcess );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::GetProcessInfo()
// Acquires process information.
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetProcessInfo( 
		TProcessIdentityParamsBuf& aProcessIdentityParams )
	{
	return DoControl( EGetProcessInfo, &aProcessIdentityParams, NULL );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::GetCodesegInfo()
// Acquires codeseg information.
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetCodesegInfo( TCodesegInfoBuf& aCodesegInfo )
	{
	return DoControl( EGetCodesegInfo, &aCodesegInfo, NULL );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::GetLibraryInfo()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetLibraryInfo( TLibraryInfoBuf& aLibraryinfo )
	{
	return DoControl( EGetLibraryInfo, &aLibraryinfo, NULL );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::LibraryEvent()
// Subscribes library event.
// ----------------------------------------------------------------------------
//
inline void RAnalyzeTool::LibraryEvent( TRequestStatus& aStatus,
	TLibraryEventInfo& aLibInfo )
	{
	return DoRequest( ELibraryEvent, aStatus, (TAny*)&aLibInfo );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::CancelLibraryEvent()
// Cancels subscribetion of the library event.
// ----------------------------------------------------------------------------
//
inline void RAnalyzeTool::CancelLibraryEvent()
	{
	DoControl( ECancelLibraryEvent, NULL, NULL);
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::ClientCount()
// Acquires the count of device driver current users
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::ClientCount( TClientCountBuf& aClientCount )
	{
	return DoControl( ECurrentClientCount, &aClientCount, NULL );
	}

// ----------------------------------------------------------------------------
// RAnalyzeTool::MainThreadAlloctor()
// Acquires information about process main thread
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::MainThreadAlloctor( 
	        TMainThreadParamsBuf& aMainThreadParams )
    {
    return DoControl( EMainThreadAlloctor, &aMainThreadParams, NULL );
    }

// ----------------------------------------------------------------------------
// RAnalyzeTool::ThreadStack()
// Acquires main thread stack address.
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::ThreadStack( TThreadParamsBuf& aThreadStack )
    {
    return DoControl( EThreadStack, &aThreadStack, NULL );
    }

// ----------------------------------------------------------------------------
// RAnalyzeTool::GetProcessHandleInfo()
// Acquires information about process handles
// ----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetProcessHandleInfo( 
		    TProcessHandleInfoBuf& aProcessHandleInfo )
    {
    return DoControl( EGetProcessHandle, &aProcessHandleInfo, NULL );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetCurrentHandleCount()
// Acquires a process's current handle count
// -----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetCurrentHandleCount( 
		    TATProcessHandlesBuf& aProcessHandles )
    {
    return DoControl( EGetCurrentHandles, &aProcessHandles, NULL );
    }

// -----------------------------------------------------------------------------
// DAnalyzeToolChannel::GetMemoryModel()
// Acquires memory model system uses.
// -----------------------------------------------------------------------------
//
inline TInt RAnalyzeTool::GetMemoryModel( 
            TATMemoryModelBuf& aMemoryModel )
    {
    return DoControl( EGetMemoryModel, &aMemoryModel, NULL );
    }

#endif // #ifndef __KERNEL_MODE__

#endif // __ANALYZETOOL_INL

// End of file
