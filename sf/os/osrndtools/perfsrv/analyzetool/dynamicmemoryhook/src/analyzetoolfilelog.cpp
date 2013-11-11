/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of the class TATDriveInfo.
*
*/

#include "analyzetoolfilelog.h"
#include "atdriveinfo.h"
#include "atlog.h"
#include <analyzetool/customuser.h>
#include <analyzetool/analyzetooltraceconstants.h>
#include <f32file.h> //RFs
#include <utf.h>

// New file name start and end index.
const TInt KNameIndexStart = 1;
const TInt KNameIndexEnd = 100;

RATFileLog::RATFileLog()
	{

    // Initialize iMicroSecondsAt1970
    TTime time( KJanuaryFirst1970 );
    iMicroSecondsAt1970 = time.Int64();
    iError = KErrNone;
	}

RATFileLog::~RATFileLog()
	{
	// Auto-generated destructor stub
	}


// Local time function.
TInt64 RATFileLog::CurrentTime()
    {
    LOGSTR1( "ATFL void CATStorageServerSession::GetTime()" );
    
    // Get the current universal time
    TTime time;
    time.UniversalTime();
        
    // Change the time format that tells the number of microseconds from January First,
    // 0 AD nominal Gregorian, into a format that tells the number of microseconds from
    // January First, 1970 AD nominal Gregorian. This is a more generic format and
    // can be directly exploited by the PC code parsing the data file that this
    // server generates.        
    return ( time.Int64() - iMicroSecondsAt1970 );        
    }

TInt RATFileLog::ATFileLogVersion()
{
    LOGSTR1( "ATFL CATFileLog::ATFileLogVersion()" );
    
    TInt err( KErrNone );
    
    iFile << KDataFileVersion;

    TRAP( err, iFile.CommitL() );
    return err;
}

TInt RATFileLog::ATFileLogProcessStarted( const TDesC8& aProcessName,
                                          TUint aProcessId,
                                          TUint32 aIsDebug,
                                          const TDesC8& aAtoolVersion,
                                          const TDesC8& aApiVersion )
{
    LOGSTR1( "ATFL CATFileLog::ATFileLogProcessStarted()" );
 
    TInt err( KErrNone );
    
    // log the current time
    iFile << this->CurrentTime();
    
    // <Time stamp> PCS <Process name> <Process ID> <Udeb> <Version>
    iFile << (TUint8)EProcessStart;
    iFile << aProcessName;
    iFile << (TUint32)aProcessId;
    iFile << aIsDebug;
    iFile << aAtoolVersion;
    iFile << aApiVersion;
    
    // ensure that any buffered data is written to the stream
    TRAP( err, iFile.CommitL() );
    
    return err;
}

// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogProcessEnded
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogProcessEnded( TUint aHandleLeakCount )
{
	LOGSTR1( "ATFL CATFileLog::ATFileLogProcessEnded()" );
	
	TInt err( KErrNone );
	
	if ( aHandleLeakCount > 0 )
	{
		// HDL <Handle count>
		iFile << this->CurrentTime();
        iFile << (TUint8)EHandleLeak;
        iFile << (TUint32)aHandleLeakCount;	
	}

	// log the current time
    iFile << this->CurrentTime();
	
	// PCE
	iFile << (TUint8)EProcessEnd;
	
    // ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );
	
	return err;	
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogDllLoaded
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogDllLoaded( const TDesC8& aDllName, TUint32 aStartAddress,
                                TUint32 aEndAddress )
{
	LOGSTR1( "ATFL CATFileLog::ATFileLogDllLoaded()" );
	   
	TInt err( KErrNone );
	
	// log the current time
	iFile << this->CurrentTime();
	
	// DLL <DLL name> <Memory start address> <Memory end address>

	// DLL
	iFile << (TUint8)EDllLoad;
	// dll name
	iFile << aDllName;
	// start adress
	iFile << aStartAddress;
	//end adress
	iFile << aEndAddress;
	
	// ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );
	
	return err;
}



// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogDllUnloaded
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogDllUnloaded( const TDesC8& aDllName, TUint32 aStartAddress,
                                       TUint32 aEndAddress )
{
    LOGSTR1( "ATFL CATFileLog::ATFileLogDllUnloaded()" );

    TInt err( KErrNone );
    
    // log the current time
    iFile << this->CurrentTime();
    
    // DLU <DLL name> <Memory start address> <Memory end address>
    
    // DLU
    iFile << (TUint8)EDllUnload;
    // dll name
    iFile << aDllName;
    // start adress
    iFile << aStartAddress;
    //end adress
    iFile << aEndAddress;
    
    // ensure that any buffered data is written to the stream
    TRAP( err, iFile.CommitL() );
	
	return err;
		
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogMemoryAllocated
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogMemoryAllocated( TUint32 aMemAddress,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  )
{
	LOGSTR1( "ATFL CATFileLog::ATFileLogMemoryAllocated()" );
	
	TInt err( KErrNone );
	
	// ALH <Memory address> <Allocation size> <Thread ID> 
	// <Call stack address count> <Call stack address> <Call stack address> ...
	
	// log the current time
	iFile << this->CurrentTime();

	// ALH
	iFile << (TUint8)EAllocH;
	// memory adress
	iFile << aMemAddress;
	// allocation size
	iFile << (TUint32)aSize;
	// thread ID
	iFile << (TUint32)aThreadId;

	// Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aCallstack.Count() ; j++ )
		{
		if ( aCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	
	// address count
    iFile << (TUint32)addrCount;
		
	// Go through all call stack's memory addresses associated with
	// this memory allocation 
	for ( TInt addrPos = 0; addrPos < addrCount; addrPos++ )
		{
	    iFile <<  aCallstack.At( addrPos );
		}

	// ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );

	return err;	
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogMemoryFreed
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogMemoryFreed( TUint32 aMemAddress, 
		                                  TFixedArray<TUint32, KATMaxFreeCallstackLength>& aFreeCallstack, 
		                                  TUint aThreadId )
{
    LOGSTR1( "ATFL CATFileLog::ATFileLogMemoryFreed()" );
	
    TInt err( KErrNone );
    
	// FRH <Memory address> <Thread ID> <Call stack address count> 
	// <Call stack address> <Call stack address> ...

	// log the current time
	iFile << this->CurrentTime();

	// FRH
	iFile << (TUint8)EFreeH;
	
	// Append the start address of this allocation in the 32-bit (max 8 characters)
	// hexadecimal text format.
	iFile << aMemAddress;

	// thread ID
	iFile << (TUint32)aThreadId;
	
	// Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aFreeCallstack.Count() ; j++ )
		{
		if ( aFreeCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	
	// address count.
	iFile << (TUint32)addrCount;

	// Go through all call stack's memory addresses associated with
	// this memory allocation 
	for ( TInt addrPos = 0; addrPos < addrCount; addrPos++ )
		{
		iFile << aFreeCallstack.At( addrPos );
		}

	// ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );

	return err;		
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogMemoryReallocated
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogMemoryReallocated( TUint32 aMemAddressFree,  TUint32 aMemAddressAlloc,
                                  TFixedArray<TUint32, KATMaxCallstackLength>& aCallstack,
                                  TInt aSize,  TUint aThreadId  )
{
	LOGSTR1( "ATFL CATFileLog::ATFileLogMemoryReallocated()" );    
	   
	TInt err( KErrNone );
	
	// RAH <Freed memory address> <Allocated memory address> <Allocation size> <Thread ID>
	// <Call stack address count> <Call stack address> <Call stack address> ...
	        
    // log the current time
    iFile << this->CurrentTime();
	
	// RAH
    iFile <<  (TUint8)EReallocH;
	// memory adress freed
    iFile << aMemAddressFree;
	// memory adress allocated
    iFile << aMemAddressAlloc;
	// allocation size
    iFile << (TUint32)aSize;
	// thread ID
    iFile << (TUint32)aThreadId;

	// Search call stack for address count.
	TInt addrCount(0);
	for ( TInt j = 0; j < aCallstack.Count() ; j++ )
		{
		if ( aCallstack.At(j) == 0 )
			break;
		addrCount++;
		}
	
	// address count
	iFile << (TUint32)addrCount;
		
	// Go through all call stack's memory addresses associated with
	// this memory allocation 
	for ( TInt addrPos = 0; addrPos < addrCount; addrPos++ )
		{	  
		// Append call stack address.
		iFile << aCallstack.At( addrPos );
		}

	// ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );

	return err;	
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogThreadStarted
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogThreadStarted( TUint aThreadId )
{
	LOGSTR1( "ATFL CATFileLog::ATFileLogThreadStarted()" );

	TInt err( KErrNone );
	
	// log the current time
	iFile << this->CurrentTime();
	
	// TDS <Thread ID>

	// TDS
	iFile << (TUint8)EThreadStart;
	// thread ID
	iFile << (TUint32)aThreadId;
    
    // ensure that any buffered data is written to the stream
	TRAP( err, iFile.CommitL() );

	return err;
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogThreadEnded
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogThreadEnded( TUint64 aThreadId )
{
    LOGSTR1( "ATFL CATFileLog::ATFileLogThreadEnded()" );
    
    TInt err( KErrNone );
    
    // log the current time
    iFile << this->CurrentTime();
    
    // TDE <Thread ID>
    
    // TDE
    iFile << (TUint8)EThreadEnd;
    // thread ID
    iFile << (TUint32)aThreadId;
    
    // ensure that any buffered data is written to the stream
    TRAP( err, iFile.CommitL() );
    
    return err;
}


// -----------------------------------------------------------------------------
// RATFileLog::ATFileLogDeviceInfo
// 
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::ATFileLogDeviceInfo( const TDesC8& aS60Version, const TDesC8& aChecksum)

{
    LOGSTR1( "ATFL CATFileLog::ATFileLogVersionsInfo()" );

    TInt err( KErrNone );
    
    // log the current time
    iFile << this->CurrentTime();

    // VER
    iFile << (TUint8)EDeviceInfo;
    // sw version
    iFile << aS60Version;
    // rom checksum
    iFile << aChecksum;

    // ensure that any buffered data is written to the stream
    TRAP( err, iFile.CommitL() );

	return err;	
}



// -----------------------------------------------------------------------------
// RATFileLog::OpenFsAndFile
// Internally used for opening a handle to the file server and a file
// -----------------------------------------------------------------------------
// 
TInt RATFileLog::OpenFsAndFile( const TDesC& aFileName, const TDesC& aFilePath, 
    const TDesC8& aProcessName )
    {
    LOGSTR1( "ATFL TInt CATFileLog::OpenFsAndFile()" );
    
    // Connect file server, return if error occured
    iError = iFileServer.Connect();
    if ( iError )
        {
        iFileServer.Close();
        return iError;
        }
    
    iError = iFileServer.ShareAuto();
    if( iError )
    	{
		iFileServer.Close();
		return iError;
    	}

    // Open a file
    TBuf<KMaxFileName> fileNameBuf;  
    
    iError = TATDriveInfo::CreatePath( fileNameBuf, aFileName, aFilePath, iFileServer );
                        
    // Return, if an error occured, and it
    // is not KErrAlreadyExists
    if ( iError && iError != KErrAlreadyExists )
        {
        iFileServer.Close();
        return iError;
        }

    // if file already exist, generat new filename with index
    GenerateNewFileName( fileNameBuf, aProcessName );
    
    // Save the file name for this session
    CnvUtfConverter::ConvertFromUnicodeToUtf8( iLogFile, fileNameBuf );
    
    LOGSTR2( "ATFL > iError(%i)", iError );
        
    // If the file does not exist, create it. Write also the version number of
    // the file at the beginning of the new file
    if ( iError == KErrNotFound )
        {
        iError = iFile.Create( iFileServer, fileNameBuf, EFileWrite | EFileShareAny );
        if ( !iError )
            {
        	// todo
            }
        }
       
    if ( iError )
        {
        iFile.Close();
        iFileServer.Close();
        return iError;
        }

    return iError;
    }

// -----------------------------------------------------------------------------
// RATFileLog::GenerateNewFileName
// Called internally when need generate new file name.
// -----------------------------------------------------------------------------
//                     
void RATFileLog::GenerateNewFileName( TDes& aFileName,
    const TDesC8& aProcessName )
    {    
    LOGSTR1( "ATFL void CATFileLog::GenerateNewFileName()" );
        
    // Extension
    TBuf<KExtensionLength> extension;
        
	// Check if file already exist
	CheckIfFileAlreadyExist( aFileName );
	if ( iError == KErrNotFound )
		{
	    // If not we can use filename without indexing
		return;
		}
	
	// File with aFileName already exist, let's add index
	
	// Parse file extension if exists.
	ParseExtension( aFileName, extension );
	
	// Temp file name
	TBuf<KMaxFileName> tempName;
	
	for ( TInt i = KNameIndexStart; i < KNameIndexEnd; i++ )
		{
		tempName.Delete( 0, tempName.MaxLength() );
		tempName.Format( KFormat, &aFileName, &KLeftBracket, i, &KRightBracket, &extension );
		LOGSTR2( "ATFL > tempName(%S)", &tempName );
		// Try to open file
		CheckIfFileAlreadyExist( tempName );
		// File does not exist, this index can be used
		if ( iError == KErrNotFound )
			{
			aFileName.Copy( tempName );
			break;
			}
		}
    }

// -----------------------------------------------------------------------------
// RATFileLog::ParseExtension
// Method is used to parse file name extension.
// -----------------------------------------------------------------------------
//                     
void RATFileLog::ParseExtension( 
    TDes& aFileName, TDes& aExtension )
    {    
    LOGSTR2( "ATFL void CATFileLog::ParseExtension(%S)", 
            &aFileName );

    // Parse current file name
    TParse parse;
    // Set up the TParse object 
    parse.Set( aFileName, NULL, NULL );

    // Tests whether an extension is present.
    if ( parse.ExtPresent() )
        {
        // Gets the extension
        aExtension.Copy( parse.Ext() );
        // Remove extension from file name
        TInt pos( aFileName.Find( aExtension ) );
        aFileName.Delete( pos, aFileName.Length() );
        LOGSTR3( "ATFL > aFileName(%S), aExtension(%S)", 
                &aFileName, &aExtension );
        }
    }

// -----------------------------------------------------------------------------
// RATFileLog::CheckIfFileAlreadyExist
// Method is used to check that file exists and is valid.
// -----------------------------------------------------------------------------
//                     
void RATFileLog::CheckIfFileAlreadyExist( 
    const TDes& aFileName )
    {    
    LOGSTR2( "ATFL void CATFileLog::CheckIfFileAlreadyExist(%S)", 
            &aFileName );
    
    iError = iFile.Open( iFileServer, aFileName, EFileWrite );
    LOGSTR2( "ATFL > iError(%i)", iError );
    }


// -----------------------------------------------------------------------------
// RATFileLog::CloseFsAndFile
// Internally used for closing a handle to the file server and a file
// -----------------------------------------------------------------------------
// 
void RATFileLog::CloseFsAndFile()
    {
    LOGSTR1( "ATFL void CATFileLog::CloseFsAndFile()" );
    
    // Close the file    
    iFile.Close();
                 
    // Close the server session and return the error code   
    iFileServer.Close();
    }

