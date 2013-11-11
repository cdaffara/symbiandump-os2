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
* Description:  Definitions for the class TATDriveInfo.
*
*/



// INCLUDE FILES
#include <f32file.h>
#include <driveinfo.h>
#include "atdriveinfo.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// TATDriveInfo::TATDriveInfo
// C++ default constructor.
// -----------------------------------------------------------------------------
//
TATDriveInfo::TATDriveInfo()
    {
    LOGSTR1( "TATD TATDriveInfo::TATDriveInfo()" );
    }

// -----------------------------------------------------------------------------
// TATDriveInfo::CreatePath()
// -----------------------------------------------------------------------------
//    
TInt TATDriveInfo::CreatePath( TDes& aPath, 
    const TDesC& aFileName, const TDesC& aFilePath, RFs& aFs )
    {
    LOGSTR1( "TATD TInt TATDriveInfo::CreatePath()" );
    TInt err( KErrNotFound );
    
    // first try to create user defined directory
    if( aFilePath.Length()>0 )
    	{
		err = aFs.MkDir( aFilePath );
		LOGSTR2( "STSE > defined path aFs.MkDir err = %i", err );
		
		if ( !err || err == KErrAlreadyExists )
			{
			//directory was succesfully created or exists
			aPath.Copy( aFilePath );
			if ( aFileName.Length() != 0 && 
				 ( ( aPath.MaxLength() - aPath.Length() ) > aFileName.Length() ) )
				{
				aPath.Append( aFileName );
				}
			return err;
			}
		else if( err == KErrBadName )
			{
			// TODO bad path, log error
			// log default name in the end of function
			}
    	}
	


	//error when trying to create user defined directory, lets use default paths
    
    // Drive letter    
    TChar driveLetter;
    // Drive number
    TInt dNumber( EDriveZ );     
    TBool found( EFalse );
    // Drive type
    TUint driveType( KDriveAttRemovable );    
    
    
    while ( !found )
        {
        // Get drive letter
        if ( GetDrive( driveLetter, dNumber, aFs, driveType ) == KErrNotFound )
        	{
        	if ( driveType == KDriveAttInternal )
        		{
                return KErrNotFound;
        		}       	
            driveType = KDriveAttInternal;
        	dNumber = EDriveZ;
        	}       
        else
        	{
			// Create path
			aPath.Delete( 0, aPath.MaxLength() );
			aPath.Append( driveLetter );
		  
		            
		#ifdef __WINS__
            // For emulator the data file is stored to different location
            aPath.Append( KATDataFilePath );       
		#else
		    TDriveInfo driveInfo;
		    aFs.Drive( driveInfo, dNumber );      
            // The drive is removable( memory card ) so we can log inside of root folder
            if ( driveInfo.iDriveAtt & KDriveAttRemovable )
                {
                aPath.Append( KATDataFilePath );       
                }
            // The drive is internal user can only access data folder so log into that
            else
                {
                aPath.Append( KATDataFilePath2 );       
                }
		#endif
						
			// Make a directory for logging data file
			err = aFs.MkDir( aPath );
			LOGSTR2( "STSE > aFs.MkDir err = %i", err );
			
			if ( !err || err == KErrAlreadyExists )
				{                
                if ( aFileName.Length() != 0 && 
                     ( ( aPath.MaxLength() - aPath.Length() ) > aFileName.Length() ) )
                    {
                    aPath.Append( aFileName );
                    }
				found = ETrue;
				}
        	}
        }        
    return err;        
    }

// -----------------------------------------------------------------------------
// TATDriveInfo::GetDrive()
// -----------------------------------------------------------------------------
//    
TInt TATDriveInfo::GetDrive( TChar& aDrive, TInt& aDriveNumber, RFs& aFs, 
	const TUint aDriveType )
    {
    LOGSTR1( "TATD TInt TATDriveInfo::GetDrive()" );
       
    // Contains drive information.
    TDriveInfo driveInfo; 
    
    for ( TInt i = aDriveNumber; i >= (TInt)EDriveA; i-- )
        {
        // Gets information about a drive and the medium mounted on it.
        // If error occured then skip to next drive.
        if ( aFs.Drive( driveInfo, i ) != KErrNone )
            continue;
                
        // Test whether drive is available. If not, skip to next drive.
        if ( driveInfo.iDriveAtt & KDriveAbsent || i == (TInt)EDriveD ||
             driveInfo.iDriveAtt & KDriveAttRom || 
             driveInfo.iMediaAtt & KMediaAttWriteProtected ||
             driveInfo.iDriveAtt & KDriveFileSysROFS )
            continue;
        
        // Maps a drive number to the corresponding character.
        if ( aFs.DriveToChar( i, aDrive ) != KErrNone )
            continue;                
        
        if ( driveInfo.iDriveAtt & aDriveType &&
             driveInfo.iType != EMediaNotPresent &&
             driveInfo.iType != EMediaCdRom )
            {
            TUint status( 0 );
            
            switch ( aDriveType )
                {
                case KDriveAttRemovable:
                    {
                    // Get the drive status of the default removable mass storage.                    
                    if ( DriveInfo::GetDriveStatus( aFs, i, status ) == KErrNone )
                        {
                        // To indicate that the drive is physically removable.
                        if ( status & DriveInfo::EDriveRemovable &&
                             !( status & DriveInfo::EDriveCorrupt ) )
                            {
                            aDriveNumber = --i;
                            return KErrNone;
                            }
                        }
                    }
                    break;
                case KDriveAttInternal:
                    {
                    // Get the drive status of the default removable mass storage.
                    if ( DriveInfo::GetDriveStatus( aFs, i, status ) == KErrNone )
                        {
                        // To indicate that the drive is internal and 
                        // cannot be physically removed.
                        if ( status & DriveInfo::EDriveInternal &&
                             !( status & DriveInfo::EDriveExternallyMountable ) )
                            {
                            aDriveNumber = --i;
                            return KErrNone;
                            }
                        }
                    }
                    break;
                    
                default:
                    break;
                }
            }
        }
    return KErrNotFound;
    }
 
// End of File
