// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//




// INCLUDE FILES
#include <ecom/ecom.h>
#include <e32uid.h>
#include <f32file.h>
#include <e32property.h>
#include <sacls.h>
#include "featmgrconfiguration.h"
#include "featmgrfeatureregistry.h"
#include "featmgrserver.h"
#include "featmgrdebug.h"

#define MAXSWIOPS 50
#define SWITIMEOUT 15000000

// CONSTANTS
_LIT( KZFeaturesFileNameMatch, "feature*" );
_LIT( KCRuntimeFeaturesFileName, "features.dat" );
#ifdef EXTENDED_FEATURE_MANAGER_TEST
_LIT( KZFeaturesDir, "C:\\Private\\102836E5\\" );
_LIT( KCFeatMgrPrivatePath, "?:\\Private\\102836E5\\runtime\\" );
#else
_LIT( KZFeaturesDir, "Z:\\Private\\10205054\\" );
_LIT( KCFeatMgrPrivatePath, "?:\\Private\\10205054\\" );
#endif // EXTENDED_FEATURE_MANAGER_TEST

const TUint32 KDefaultData( 0x00000000 );
// Feature file header constants. 
// First 4 bytes of config file: ASCII f-e-a-t followed by file version and flags.
const TUint32 KFileType( 0x74616566 );
const TUint16 KFileVersion( 1 );
const TUint16 KFileFlags( 0 );


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::CFeatMgrFeatureRegistry
// -----------------------------------------------------------------------------
//
CFeatMgrFeatureRegistry::CFeatMgrFeatureRegistry( RFs& aFs,
    MFeatMgrRegistryObserver& aObserver  )
    :
    iObserver( aObserver ),
    iFs( aFs ),
    iSWICacheFeature( EFalse ),
    iSWIStatus( ESWIComplete ),
    iSWIProcessId( 1 ),
    iOomOccured(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeatMgrFeatureRegistry* CFeatMgrFeatureRegistry::NewL( RFs& aFs, 
    MFeatMgrRegistryObserver& aObserver )
    {
    CFeatMgrFeatureRegistry* self = 
        new( ELeave ) CFeatMgrFeatureRegistry( aFs, aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CFeatMgrFeatureRegistry::~CFeatMgrFeatureRegistry()
    {
    FUNC_LOG
    
    iFeatureList.Close();
    iRangeList.Close();
    iFeatureListBackup.Close();    
    iSWICachedOperations.Close();
    if( iSWIListener )
    	{
        delete iSWIListener;    	
    	}
    if( iSWITimer )
    	{
    	delete iSWITimer;
    	}
    }


// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::IsFeatureSupported
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::IsFeatureSupported( TFeatureServerEntry& aFeature )
    {
    TInt err( KErrNotFound );
    const TInt index = SearchFeature( aFeature.FeatureUid() );
    
    if ( index == KErrNotFound )
        {
        // Check whether feature in supported ranges
        TInt count( iRangeList.Count() );
        TUid uid( aFeature.FeatureUid() );
        for( TInt i = 0; i < count; i++ )
            {
            if( (uid.iUid >= iRangeList[i].iLowUid.iUid) && 
                (uid.iUid <= iRangeList[i].iHighUid.iUid) )
                {
                TBitFlags32 flags( 0 );
                flags.Assign( EFeatureSupported, KFeatureSupported );
                TFeatureServerEntry entry( aFeature.FeatureUid(), flags, KDefaultData );
                aFeature = entry;
                err = KFeatureSupported;
                break;
                }
            }
        }
    else if( IsFlagSet( index, EFeatureUninitialized ) )
        {
        // Supported status bit is not taken into account if feature not yet initialized
        err = KErrNotReady;
        }
    else if ( (index < iFeatureList.Count()) && IsFlagSet( index, EFeatureSupported ) )
        {
        TBitFlags32 flags = iFeatureList[index].FeatureFlags();
        flags.Assign( EFeatureSupported, KFeatureSupported );
        TUint32 data = iFeatureList[index].FeatureData();
        TFeatureServerEntry entry( aFeature.FeatureUid(), flags, data );
        aFeature = entry;
        err = KFeatureSupported;
        }
    else
        {
        TBitFlags32 flags = iFeatureList[index].FeatureFlags();
        flags.Assign( EFeatureSupported, KFeatureUnsupported );
        TUint32 data = iFeatureList[index].FeatureData();
        TFeatureServerEntry entry( aFeature.FeatureUid(), flags, data );
        aFeature = entry;
        err = KFeatureUnsupported;
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::AddFeature
// -----------------------------------------------------------------------------
//
TInt CFeatMgrFeatureRegistry::AddFeature( TFeatureServerEntry& aFeature, TUint aPrcId )
    {
    TInt err( KErrAccessDenied );
    
    if( iSWIProcessId == aPrcId && iSWICacheFeature )
		{
		err = SWICacheCommand(ESWIAddFeat, aFeature);
		}
    else
    	{
        const TInt index = SearchFeature( aFeature.FeatureUid() );
	
	    if ( index == KErrNotFound )
	        {
	        TBitFlags32 flags( aFeature.FeatureFlags() );
	        flags.Set( EFeatureRuntime );
	
	        //Check the feature falg is valid
	        TRAP(err,ValidateRuntimeFeatureFlagL(flags));
	        if (err != KErrNone)
	            return err;
	
	        TFeatureServerEntry entry( aFeature.FeatureUid(), flags, aFeature.FeatureData() );	       
			err = iFeatureList.InsertInOrder( entry, FindByUid );
	        if ( err == KErrNone )
	            {
	            TFeatureChangeType changeType( EFeatureFeatureCreated );
	            err = HandleChange( entry, changeType );
	            }
	        }
	    else
	        {
	        err = KErrAlreadyExists;
	        }
	    
	    INFO_LOG("CFeatMgrFeatureRegistry::AddFeature - Features directly stored in registry");
    	}
    
    LOG_IF_ERROR1( err, "CFeatMgrFeatureRegistry::AddFeature - result %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::DeleteFeature
// -----------------------------------------------------------------------------
//
TInt CFeatMgrFeatureRegistry::DeleteFeature( TUid aFeature, TUint aPrcId  )
    {
    TInt err( KErrAccessDenied );
	
    if( iSWIProcessId == aPrcId && iSWICacheFeature )
    	{
    	err = SWICacheCommand(ESWIDeleteFeat, aFeature);
    	}
    else
    	{
	    // Check if the feature is runtime
	    TInt index = SearchFeature( aFeature );
	    if ( index == KErrNotFound )
	        {
	        return KErrNotFound;
	        }
	    if ( !iFeatureList[index].FeatureFlags().IsSet(EFeatureRuntime) )
	        {
	        return KErrAccessDenied;
	        }
	
	    TFeatureServerEntry entry = iFeatureList[index];
	    
    	iFeatureList.Remove( index );
        TFeatureChangeType changeType( EFeatureFeatureDeleted );
        err = HandleChange( entry, changeType );
        
        INFO_LOG("CFeatMgrFeatureRegistry::DeleteFeature - Features deleted directly from registry");
        }
    
    LOG_IF_ERROR1( err, "CFeatMgrFeatureRegistry::DeleteFeature - result %d", err );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SetFeature
//This method cannot set feature flag range in DSR unless it is added to before
//using CFeatMgrFeatureRegistry::AddFeature()
// -----------------------------------------------------------------------------
//	
TInt CFeatMgrFeatureRegistry::SetFeature( TUid aFeature, TInt aEnable, const TUint32 *aData, TUint aPrcId  )
    {
    FUNC_LOG

    TInt err( KErrNone );
    
    if( iSWIProcessId == aPrcId && iSWICacheFeature )
    	{
        TBitFlags32 flags(0);
        flags.Assign( EFeatureSupported, aEnable );
        TUint32 data = 0;
        if( aData )
            {
            data = *aData;
            }

        TFeatureServerEntry entry( aFeature, flags, data );

        // If aData is not null, we want to change the user data too
        //  otherwise only change the feature status.
        if( aData )
            {
            err = SWICacheCommand(ESWISetFeatAndData, entry);
            }
        else
            {
            err = SWICacheCommand(ESWISetFeat, entry);
            }
    	}
    else 
    	{
	    TInt index;
	
	    // Validate feature exists and is modifiable
	    err = ValidateFeature( aFeature, index );
	
	    if ( err != KErrNone )
	        {
			return err;
	        }
	
	    if ( (index >= 0 && index < iFeatureList.Count()) )
	        {
        	TBitFlags32 flags = iFeatureList[index].FeatureFlags();
	        TUint32 data = iFeatureList[index].FeatureData();
	        TFeatureChangeType changeType( EFeatureStatusUpdated );

	        // Update "supported" info according to request
	        if( (aEnable == EFeatureSupportEnable) || (aEnable == EFeatureSupportDisable) )
	            {
	            INFO_LOG1( "CFeatMgrFeatureRegistry::SetFeature() - aEnable %d", aEnable );
	            flags.Assign( EFeatureSupported, aEnable );
	            }
	        // When setting feature, always unset "uninitialized" bit
	        flags.Assign( EFeatureUninitialized, 0 );

	        // Update data whenever applied
	        if( aData )
	            {
	            INFO_LOG1( "CFeatMgrFeatureRegistry::SetFeature() - aData %d", aData );
	            data = *aData;

	            if( aEnable == EFeatureSupportUntouch )
	                {
	                changeType = EFeatureDataUpdated;
	                }
	            else
	                {
	                changeType = EFeatureStatusDataUpdated;
	                }
	            }

	        TFeatureServerEntry entry( aFeature, flags, data );
	        //Check if the feature will actually change
	        if(iFeatureList[index].FeatureFlags() == flags && iFeatureList[index].FeatureData() == data )
	        	{
	        	//No change were made, set change type to EFeatureNoChange
	        	changeType = EFeatureNoChange;
	        	}
	        else
	        	{
	        	// Set the feature entry in list with updated information
	        	iFeatureList[index].Set(entry);
	        	}

	        err = HandleChange( entry, changeType );
	        }
    	}
    
    LOG_IF_ERROR1( err, "CFeatMgrFeatureRegistry::SetFeature - result %d", err );

    return err;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::HandleChange
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::HandleChange( TFeatureServerEntry& aFeature, 
    TFeatureChangeType aType )
    {
    FUNC_LOG
    
    TInt err( KErrNone );
    
    // Update feature file, when feature is specified as persisted.
    if ( aFeature.FeatureFlags().IsSet( EFeaturePersisted ) )
        {
        TRAP( err, UpdateRuntimeFeaturesFileL( aFeature, aType ) );
        LOG_IF_ERROR1( err, "CFeatMgrFeatureRegistry::HandleChange - update error %d", err );
    
        // It is questionnable whether we should remove the feature from iFeatureList.
        // However, feature is usable until device is powered down and features reloaded.
        // if ( err == KErrNone )
        }
    
    // It is also questionnable whether we should suppress notification in case file 
    // update failed.
    // if ( err == KErrNone )
    iObserver.HandleFeatureChange( aFeature, aType );
    
    return err;
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ValidateFeature
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::ValidateFeature( TUid aFeature, TInt &aIndex )
    {
    TInt err( KErrNone );
    
    aIndex = SearchFeature( aFeature );
     
    if ( aIndex == KErrNotFound )
        {
        err = KErrNotFound;
        }
    else if ( !IsFlagSet( aIndex, EFeatureModifiable ) )
        {
        err = KErrAccessDenied;
        }
    
    return err;
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::FindByUid
// Returns Zero if UIDs do match.
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::FindByUid( const TUid *aFeature, 
    const TFeatureServerEntry& aItem )
    {
    if ( aFeature->iUid < aItem.FeatureUid().iUid )
        {
        return -1;
        }
    else if ( aFeature->iUid > aItem.FeatureUid().iUid )
        {
        return 1;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::FindByUid
// Returns Zero if UIDs do match.
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::FindByUid( const TFeatureServerEntry& aFeature, 
    const TFeatureServerEntry& aItem )
    {
    if ( aFeature.FeatureUid().iUid < aItem.FeatureUid().iUid )
        {
        return -1;
        }
    else if ( aFeature.FeatureUid().iUid > aItem.FeatureUid().iUid )
        {
        return 1;
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SearchFeature
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::SearchFeature( TUid aFeature )
    {
    const TUid& uid( aFeature );
    return iFeatureList.FindInOrder( uid, FindByUid );
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::IsFlagSet
// -----------------------------------------------------------------------------
//  
TBool CFeatMgrFeatureRegistry::IsFlagSet( TInt aIndex, TFeatureFlags aFlag )
    {
    TBool isSet( EFalse );
    if( aIndex < iFeatureList.Count() )
        {
        isSet = iFeatureList[aIndex].FeatureFlags().IsSet(aFlag);
        }
        
    return isSet;
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SupportedFeatures
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::SupportedFeaturesL( RFeatureUidArray& aSupportedFeatures )
    {
    FUNC_LOG
    
    TInt count = iFeatureList.Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        if( IsFlagSet( i, EFeatureSupported) )
            {
            aSupportedFeatures.AppendL( iFeatureList[i].FeatureUid() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::NumberOfSupportedFeatures
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::NumberOfSupportedFeatures()
    {
    FUNC_LOG
    
    TInt count = iFeatureList.Count();
    TInt countSupported(0);
    
    for ( TInt i = 0; i < count; i++ )
        {
        if( IsFlagSet( i, EFeatureSupported) )
            {
            countSupported++;
            }
        }
    
    return countSupported;
    }

// -----------------------------------------------------------------------------

void CFeatMgrFeatureRegistry::ResetFeaturesL()
    {
    FUNC_LOG
    
    // backup the feature list before it is destroyed
    iFeatureListBackup.Reset();
    const TInt KCount = iFeatureList.Count();
   	iFeatureListBackup.ReserveL(KCount);
	for(TInt i(0); i < KCount; i++)
		{
		// The main error here would be KErrNoMemory which should not happen as
		// we have already reserved the space. However, we should still check.
		iFeatureListBackup.AppendL(iFeatureList[i]);
		}
	    
    // destroy the feature list
    iFeatureList.Reset();
    iFeatureList.Close();
    
    iRangeList.Reset();
    iRangeList.Close();
    }

/**
 * Get the fully qualified path and filename to the features.dat
 * data file.
 */ 
TFileName CFeatMgrFeatureRegistry::GetFeaturesFilePathAndName( void )
	{
    TFileName path( KCFeatMgrPrivatePath );

    path[0] = iFs.GetSystemDriveChar();
    path.Append( KCRuntimeFeaturesFileName );

    return path;
	}

// CFeatMgrFeatureRegistry::ReadFeatureFilesL
// Reads platform and product feature files. 
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::ReadFeatureFilesL()
    {
    FUNC_LOG    
    
    // Read feature files from Z
    ReadFilesFromDirL( KZFeaturesDir );

    //check that there is at least one DSR 
    if (!iRangeList.Count()) 
    	{
    	_LIT(KPanicCategory, "FeatMgrServer");
    	ERROR_LOG( "CFeatMgrFeatureRegistry::ReadFilesFromDirL() - no DSR found in ROM; this indicates a system integration error  - going to panic" );
    	::FmgrFatalErrorL(KErrNotFound, KPanicCategory, EPanicNoDSR);    
    	}
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ReadFilesFromDirL
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::ReadFilesFromDirL( const TDesC& aDirName )
    {
    _LIT(KPanicCategory, "FEATMGR-READFILE");

    CDir* dir = NULL; 
    TInt err( KErrNone );

    err = iFs.GetDir( aDirName, KEntryAttNormal, ESortByName, dir );
    CleanupStack::PushL( dir );

    if( err == KErrNone )
        {
        err = ReadFiles( aDirName, dir );
        if ( err != KErrNone )
            {            
            ERROR_LOG1( "CFeatMgrFeatureRegistry::ReadFilesFromDirL() - err %d ", err );
            User::Leave( err );    
            }
        }  
    else if( err == KErrPathNotFound )
    	{
    	::FmgrFatalErrorL(err, KPanicCategory, EPanicNoFeatureFiles);
    	}
	else
        {            
        ERROR_LOG1( "CFeatMgrFeatureRegistry::ReadFilesFromDirL() - err %d ", err );
        User::Leave( err );    
        }
    
    CleanupStack::PopAndDestroy( dir );
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ReadRuntimeFeaturesL
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::ReadRuntimeFeaturesL( TBool &aFeaturesReady )
    {
    TFileName path( KCFeatMgrPrivatePath );
    path[0] = iFs.GetSystemDriveChar();
    path.Append( KCRuntimeFeaturesFileName );

    TInt err( KErrNone );
    TRAP( err, ReadFileL( path ) );

    if ((err == KErrCorrupt) || (err == KErrArgument))
    	{
    	User::LeaveIfError(iFs.Delete(path));
    	aFeaturesReady = ETrue;
    	}
    else if ( err != KErrNone && err != KErrNotFound && err != KErrPathNotFound )
        {  
        ERROR_LOG1( "CFeatMgrFeatureRegistry::ReadRuntimeFeatures - ReadFileL returned err %d", err );
        User::Leave( err );
        }
    else
        {
        aFeaturesReady = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ReadFiles
// -----------------------------------------------------------------------------
//  
TInt CFeatMgrFeatureRegistry::ReadFiles( const TDesC& aPath, CDir* aDir )
    {
    TInt fileCount = aDir->Count();
    TFileName fileName;
    TInt err( KErrNotFound );
    
    for ( TInt file = 0; file < fileCount; file++ )
        {
        TInt match = (*aDir)[file].iName.MatchC( KZFeaturesFileNameMatch );
        if( match != KErrNotFound )
            {
            fileName.Copy(aPath);
            fileName.Append((*aDir)[file].iName);

            INFO_LOG1( "CFeatMgrFeatureRegistry::ReadFiles - file: %S", &fileName );
            TRAP( err, ReadFileL( fileName ) );
            LOG_IF_ERROR2( err, "CFeatMgrFeatureRegistry::ReadFiles - file: %S, err %d", 
                &fileName, err );
            
            // Return error if reading of any feature file fails.
            if( err != KErrNone )
                {
                break;
                }
            }
        }

    return( err );
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ReadFileL
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::ReadFileL( const TDesC& aFullPath )
    {
    FUNC_LOG
   
    TUint32 count;
 	RFileReadStream readStream;

 	// Open the file and attach to stream 
    User::LeaveIfError( readStream.Open( iFs, aFullPath, EFileRead ) );
    CleanupClosePushL( readStream );
    TUint32 countDSRs;
    
    //Validate the header
    ValidateHeaderL( readStream, count, countDSRs );

    RArray<TFeatureServerEntry> tempFeatureArray;
    CleanupClosePushL( tempFeatureArray );
    
    //Find the directory that the feature file is contained in. 
    TFileName dirName(aFullPath);
    TChar toFind = '\\';
    dirName.Delete((dirName.LocateReverse(toFind)+1), dirName.Length() );
    TBool runtimeFile = EFalse;
    if (dirName.Compare(KZFeaturesDir) != 0) //Location of the feature file.
    	{
    	runtimeFile = ETrue;
    	}
    
    tempFeatureArray.ReserveL(count);
    
	for(TInt i = 0; i < count; i++)
        {
        TFeatureServerEntry entry;
        entry.InternalizeL( readStream );
        
        //Check for feature flag errors
        TBitFlags32 flags =  entry.FeatureFlags();
        TInt err = KErrNone;
        
        //Validate the flags
        // This validation is done in this read function because the validation functions used
        // are called in other places were this validation is not appropriate. 
        if (runtimeFile)
        	{
        	if (!flags.IsSet(EFeatureRuntime)) //Check to see if the Runtime flag is set if it is not then the feature should have been read in from the rom. 
        		{
        		if (SearchFeature( entry.FeatureUid() ) == KErrNotFound )// Check to see if the feature has been read in previously from the rom.
        			{
        			User::Leave(KErrCorrupt); 
        			}
        		else //The feature has not been read in previously from the rom file and is therefore invalid. The file is deemed to be corrupt
        			{
        			ValidateRuntimeFeatureFlagL(flags);   
            		}
        		}
        	else //Flag set the feature is runtime this is then validated as normal
        		{
        		ValidateRuntimeFeatureFlagL(flags);    	
        		}

        	}
        else //File is not as runtime file.
        	{
        	ValidateFeatureFlagL(flags);
        	}
        
        //If a feature flag defined in system drive (c:) is invalid, it will not be added to Feature Manager 
        if ( (err != KErrNone) && flags.IsSet(EFeatureRuntime) )
        	{
        	continue;
       		}
        
		tempFeatureArray.InsertL( entry, i);
        }

    // Reserve memory if list still empty
    if( !iFeatureList.Count() )
        {
        iFeatureList.ReserveL( tempFeatureArray.Count() );
        }

    // Read features from temp array
    for(TInt i = 0; i < tempFeatureArray.Count(); i++)
        {
        TFeatureServerEntry entry = tempFeatureArray[i];
 
        TInt index = SearchFeature( entry.FeatureUid() );
        
        if( index == KErrNotFound)
            {
            iFeatureList.InsertInOrderL( entry, FindByUid );
            }
        else
            {
            INFO_LOG1( "CFeatMgrFeatureRegistry::ReadFileL - replacing uid 0x%08x",
                iFeatureList[index].FeatureUid().iUid );
            // Set the feature if it is not previously blacklisted
            if ( !IsFlagSet( index, EFeatureBlackListed ) )
                {
                iFeatureList[index].Set(entry);
                }
            }
        }
    
    // Reserve memory if DSR list still empty
    if( !iRangeList.Count() )
        {
        iRangeList.ReserveL( countDSRs );
        }
    
    // Read default supported ranges from file
    for(TInt i = 0; i < countDSRs; i++)
        {
        TDefaultRange range;
        range.iLowUid = TUid::Uid( readStream.ReadUint32L() );
        range.iHighUid = TUid::Uid( readStream.ReadUint32L() );
        iRangeList.AppendL( range );
        if( iRangeList[i].iLowUid.iUid > iRangeList[i].iHighUid.iUid )
            {
            ERROR_LOG( "CFeatMgrFeatureRegistry::ReadFileL - invalid supported range" );
            iRangeList.Remove( i );
            User::Leave( KErrCorrupt );
            }
        }

#if defined(FEATMGR_INFO_LOG_ENABLED)
    count = iFeatureList.Count();
    INFO_LOG1( "CFeatMgrFeatureRegistry::ReadFileL - feature entries: %d", count );
    for(TInt i = 0; i < count; i++)
        {
        INFO_LOG3( "CFeatMgrFeatureRegistry::ReadFileL - uid 0x%08x, flags %d, data %d",
            iFeatureList[i].FeatureUid().iUid, iFeatureList[i].FeatureFlags().iFlags, 
            iFeatureList[i].FeatureData() );
        }

    count = iRangeList.Count();
    INFO_LOG1( "CFeatMgrFeatureRegistry::ReadFileL - supported ranges: %d", count );
    for(TInt i = 0; i < count; i++)
        {
        INFO_LOG2( "CFeatMgrFeatureRegistry::ReadFileL - low 0x%08x, high 0x%08x",
            iRangeList[i].iLowUid, iRangeList[i].iHighUid );
        }
#endif

    CleanupStack::PopAndDestroy( &tempFeatureArray);
    CleanupStack::PopAndDestroy( &readStream );

    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ValidateHeaderL
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::ValidateHeaderL( RFileReadStream &aStream, 
    TUint32& aCount, TUint32& aCountDSRs )
    {
    FUNC_LOG

    TUint32 identifier = aStream.ReadUint32L();
    TUint16 fileVersion = aStream.ReadUint16L();
    TUint16 fileFlags = aStream.ReadUint16L();
    aCount = aStream.ReadUint32L();
    aCountDSRs = aStream.ReadUint32L();
        
    // Carry out simple verification of file content
    if((identifier != KFileType) || fileVersion != KFileVersion || 
        fileFlags != KFileFlags )
        {
        User::Leave( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::UpdateRuntimeFeaturesFileL
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::UpdateRuntimeFeaturesFileL( TFeatureServerEntry& aFeature,
		TFeatureChangeType aType )
    {
    FUNC_LOG

    // Opens a file containing a stream and prepares the stream for writing.
    TInt err( KErrNone );
    RFileWriteStream writeStream;
    TFileName path( KCFeatMgrPrivatePath );
    path[0] = iFs.GetSystemDriveChar();
    path.Append( KCRuntimeFeaturesFileName );

    err = writeStream.Open( iFs, path, EFileWrite  ); 
    CleanupClosePushL( writeStream );
    
    if( err == KErrPathNotFound || err == KErrNotFound )
    	{
        // Create folder and file.
    	if ( err == KErrPathNotFound )
    		{
    		path = KCFeatMgrPrivatePath;
    		path[0] = iFs.GetSystemDriveChar();
		    User::LeaveIfError( iFs.MkDirAll( path ) );
            path.Append( KCRuntimeFeaturesFileName );
    		}
        User::LeaveIfError( writeStream.Create( iFs, path, EFileWrite ) );
        
        // Write header and entry
        RFeatureServerArray temp(1);
        CleanupClosePushL( temp );
        temp.AppendL( aFeature );
        WriteHeaderAndEntriesL( writeStream, temp );
        CleanupStack::PopAndDestroy( &temp );
        CleanupStack::PopAndDestroy( &writeStream );
        }
    else if( err == KErrNone )
	    {
	    // Close write- and open readstream
	    CleanupStack::PopAndDestroy( &writeStream );
        RFileReadStream readStream;
        User::LeaveIfError( readStream.Open( iFs, path, EFileRead ) );
        CleanupClosePushL( readStream );

        // Read entries from file to temporary array
        TUint32 count;
        TUint32 countDSRs;
        ValidateHeaderL( readStream, count, countDSRs );
        TUint32 granularity = 8;
        if (count>granularity) 
        	{
        	granularity=count;
        	}
        RFeatureServerArray temp(granularity);
        CleanupClosePushL( temp );
        for(TInt i = 0; i < count; i++)
            {
            TFeatureServerEntry entry;
            entry.InternalizeL( readStream );
            temp.AppendL( entry );
            }
        // Close read-stream and handle temp array in cleanup stack
        CleanupStack::Pop( &temp );
        CleanupStack::PopAndDestroy( &readStream );
        CleanupClosePushL( temp );

        // Set or insert a new entry in to the array
        const TUid& uid( aFeature.FeatureUid() );
        TInt index = temp.FindInOrder( uid, FindByUid );
        if( index != KErrNotFound )
            {         
            if ( aType != EFeatureFeatureDeleted )
            	{
            	temp[index].Set( aFeature);
            	}
            else
            	{
            	temp.Remove( index );
            	}
            }
        else
            {
            User::LeaveIfError( temp.InsertInOrder( aFeature, FindByUid ) );
            }
		
		//Create a Temporary File
		RFileWriteStream writeStreamTemp;
		const TPtrC KTestFile=_L("TFEATURES.DAT");
        TFileName tempPath( KCFeatMgrPrivatePath );
	    tempPath[0] = iFs.GetSystemDriveChar();
    	tempPath.Append( KTestFile );
    	User::LeaveIfError(writeStreamTemp.Replace( iFs,  tempPath, EFileWrite ));
    	CleanupClosePushL( writeStreamTemp);
    	WriteHeaderAndEntriesL( writeStreamTemp, temp );
    	writeStreamTemp.CommitL();
    	CleanupStack::PopAndDestroy(&writeStreamTemp);
        CleanupStack::PopAndDestroy( &temp );
        User::LeaveIfError(iFs.Replace(tempPath,path));		
       
	    }
    else
        {
        ERROR_LOG1( "CFeatMgrFeatureRegistry::UpdateRuntimeFeatures - err %d", err );
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::WriteHeaderAndEntriesL
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::WriteHeaderAndEntriesL( RFileWriteStream &aStream, 
    RFeatureServerArray& aArray )
    {
    FUNC_LOG

    TInt count( aArray.Count() );
    aStream.WriteUint32L( KFileType );
    aStream.WriteUint16L( KFileVersion );
    aStream.WriteUint16L( KFileFlags );
    aStream.WriteUint32L( count );
    aStream.WriteUint32L( 0 );
    for(TInt i = 0; i < count; i++)
        {
        aArray[i].ExternalizeL( aStream );
        }
    aStream.CommitL();
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::MergePluginFeatures
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::MergePluginFeaturesL( 
        RArray<FeatureInfoCommand::TFeature>& aList )
    {
    FUNC_LOG
    
    TInt count = aList.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        const TUid uid( TUid::Uid( aList[i].iFeatureID ) );
        TInt index = SearchFeature( uid );
        
        if(index != KErrNotFound)
            {
            if ( !IsFlagSet( index, EFeatureBlackListed ) )
                {
                // Update support-status bit
                TBitFlags32 flags( iFeatureList[index].FeatureFlags() );
                flags.Assign( EFeatureSupported, aList[i].iValue);
                
                // Set existing entry in array
                TFeatureServerEntry entry( uid, flags, iFeatureList[index].FeatureData());
                iFeatureList[index].Set(entry);
                }
            else
                {
                INFO_LOG1( "CFeatMgrFeatureRegistry::MergePluginFeatures - 0x%08x blacklisted",
                    iFeatureList[i].FeatureUid().iUid );
                }
            }
        else
            {
            TBitFlags32 flags;
            flags.Assign( EFeatureSupported, aList[i].iValue);
            // Insert new entry in array
            TFeatureServerEntry newFeature( uid, flags, KDefaultData );
            TInt err = iFeatureList.InsertInOrder( newFeature, FindByUid );
            INFO_LOG2( "CFeatMgrFeatureRegistry::MergePluginFeatures - 0x%08x insert result %d",
                newFeature.FeatureUid().iUid, err );
            User::LeaveIfError(err);
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::MergePluginFeatures
// -----------------------------------------------------------------------------
//  
void CFeatMgrFeatureRegistry::MergePluginFeaturesL( RFeatureArray& aList )
    {
    FUNC_LOG
    
    TInt count = aList.Count();

    for ( TInt i = 0; i < count; i++ )
        {
        //Check for feature flag errors
        ValidateFeatureFlagL(aList[i].FeatureFlags()) ; 
        const TUid uid( aList[i].FeatureUid() );
        TInt index = SearchFeature( uid );
        
        if( index != KErrNotFound )
            {
            if ( !IsFlagSet( index, EFeatureBlackListed ) )
                {
                // Set existing entry in array with new info and data
                TFeatureServerEntry entry( uid, aList[i].FeatureFlags(), aList[i].FeatureData() );
                iFeatureList[index].Set(entry);
                }
            else
                {
                INFO_LOG1( "CFeatMgrFeatureRegistry::MergePluginFeatures - 0x%08x blacklisted",
                    iFeatureList[i].FeatureUid().iUid );
                }
            }
        else
            {
            // Insert new entry in array
            TFeatureServerEntry newFeature( uid, aList[i].FeatureFlags(), aList[i].FeatureData() );
            TInt err = iFeatureList.InsertInOrder( newFeature, FindByUid );
            INFO_LOG2( "CFeatMgrFeatureRegistry::MergePluginFeatures - 0x%08x insert result %d",
                newFeature.FeatureUid().iUid, err );
            User::LeaveIfError(err);
            }
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::ValidateFeatureFlag
// Following are the rule to check err in the ROM defined feature flags
// Rule 1) If a feature flag is blacklisted then setting any of modifiable, persisted, Un-initialised bit will be an error
// Rule 2) If a feature flag is non blacklisted, non modifiable  setting any of Un-initialised, Persisted bit will be an error
// -----------------------------------------------------------------------------
//  
   
void CFeatMgrFeatureRegistry::ValidateFeatureFlagL(TBitFlags32 aFlags)
	{
	_LIT(KPanicCategory, "FEATMGR-FLAGS");
	
	if(!aFlags.IsSet(EFeatureRuntime)) //ROM defined feature flag error check
		{
		//Rule 1
		if(aFlags.IsSet(EFeatureBlackListed) )
	    	{
	    	 if(aFlags.IsSet(EFeatureModifiable) || aFlags.IsSet(EFeaturePersisted) || aFlags.IsSet(EFeatureUninitialized) )
	    	 	{
	    	 	//error 
	    	 	::FmgrFatalErrorL(KErrArgument, KPanicCategory, EFmpInvalidFeatureBitFlagsRule1);
	    	  	}
	    	}
	    	
	    //Rule 2    	
	    if (!aFlags.IsSet(EFeatureModifiable))
	       	{
	       	if (aFlags.IsSet(EFeaturePersisted) || aFlags.IsSet(EFeatureUninitialized) )
	       	 	{
	        	//error 
	       	 	::FmgrFatalErrorL(KErrArgument, KPanicCategory, EFmpInvalidFeatureBitFlagsRule2);
	        	}	
	        }
		}
	else // Runtime feature this should not be in the rom
		{		
		::FmgrFatalErrorL(KErrArgument, KPanicCategory, EPanicInvalidFeatureInfo);
		}
	}

/**
 * This function is used to validate feature flags that are read from the features file on the ffs.
 * This function is also used to validate feature flags that are modified or added with the execption of MergePluginFeatures.
 * This validation compares the flags against a set of rules. This ffs file needs to be validate separately from the 
 * rom file. If the rom validation method is used a panic can occur which is appropriate for checking the rom but not
 * for the ffs.  
 * This does not validate the dsr ranges. 
 * The following are the rules to check for errors in the run time defined feature flags
 * Rule 1)Blacklisting of a run-time defined feature flag is an error 
 * Rule 2)Un-initialised feature flag should be modifiable.   
 */
void CFeatMgrFeatureRegistry::ValidateRuntimeFeatureFlagL(TBitFlags32 aFlags)
	{
	
	//Rule 1 (Blacklisting of run-time defined feature aFlags is not allowed)
	if(aFlags.IsSet(EFeatureBlackListed) ) 
    	{
		//error 
    	User::Leave( KErrArgument );
    	}
    	
    //Rule 2 (non modifiable run-time feature aFlags should initialised
	if(!aFlags.IsSet(EFeatureModifiable) && aFlags.IsSet(EFeatureUninitialized) )
	 	{
	 	//error 
	 	User::Leave( KErrArgument );
	 	}
	}


/**
 * After restore, some features might have changed. This function will examine
 * the differences between the old feature set and the newly restored feature set
 * to discover if any changes have taken place: then it will handle the required
 * notifications for new, deleted and changed featuers.
 */
void CFeatMgrFeatureRegistry::HandleRestoredFeatureNotificationsL()
	{
	// All comparisons are between the new list iFeatureList and the old list iFeatureListBackup
	TInt new_count = iFeatureList.Count();
	TInt old_count = iFeatureListBackup.Count();
	
	// Three lists, defining the differences between the two arrays
	RArray<TFeatureServerEntry> added;
	RArray<TFeatureServerEntry> removed;
	RArray<TFeatureServerEntry> changed;

	// Regarding the newer iFeatureList array
	// Get the features according to the "new" iFeatureList array
	for( TInt i=0; i < new_count; i++ )
		{
	    // If not set, the feature flag is a ROM or plug-in
	    if( iFeatureList[i].FeatureFlags().IsSet(EFeatureRuntime) )
	        {
	        TUid uid( iFeatureList[i].FeatureUid() );
		    TInt index = iFeatureListBackup.FindInOrder( uid, FindByUid );
		    
			// KErrNotFound, if no matching object can be found
			if( KErrNotFound == index )
				{
				// Recently added feature
				added.AppendL( iFeatureList[i] );
				}
			else
				{
				// Get the features in iFeatureList that have recently been altered
		        TFeatureServerEntry old_item = iFeatureListBackup[index];
		        TFeatureServerEntry new_item = iFeatureList[i];
		        TUint32 old_flags = old_item.FeatureFlags().Value();
		        TUint32 new_flags = new_item.FeatureFlags().Value();
		        unsigned long int old_data = old_item.FeatureData();
		        unsigned long int new_data = new_item.FeatureData();
	        	// if any thing has changed, then add it to our list.
		        // there is no != overload for TBitFlags32
				if( !( old_flags == new_flags) || !( old_data == new_data) )
					{
					// changed in the "new" iFeatureList array
					changed.AppendL( iFeatureList[i] );
					}
				}
		
	        } // end if ! EFeatureRuntime
		} // end loop


	// Regarding the older iFeatureListBackup array
	// Get the features according to the "old" iFeatureListBackup array
	for( TInt i=0; i < old_count; i++ )
		{
	    // If not set, the feature flag is a ROM or plug-in
	    if( iFeatureListBackup[i].FeatureFlags().IsSet(EFeatureRuntime) )
	        {
	        TUid uid( iFeatureListBackup[i].FeatureUid() );
		    TInt index = iFeatureList.FindInOrder( uid, FindByUid );
		    
			// KErrNotFound, if no matching object can be found
			if( KErrNotFound == index )
				{
				// Recently removed feature
				removed.AppendL( iFeatureListBackup[i] );
				}
			// the else has already been completed in previous loop
		
	        } // end if ! EFeatureRuntime
		} // end loop
	
	TInt size_added 	= added.Count();
	TInt size_changed 	= changed.Count();
	TInt size_removed 	= removed.Count();

	// notify the added features
    for( TInt i = 0; i < size_added; i++ )
        {
        TFeatureServerEntry entry( added[i].FeatureUid(), added[i].FeatureFlags(), added[i].FeatureData() );
        TFeatureChangeType changeType( EFeatureFeatureCreated );
        iObserver.HandleFeatureChange( entry, changeType );
        }

	// notify the changed features
    for( TInt i = 0; i < size_changed; i++ )
        {
        TFeatureServerEntry entry( changed[i].FeatureUid(), changed[i].FeatureFlags(), changed[i].FeatureData() );
        TFeatureChangeType changeType( EFeatureStatusUpdated );
        iObserver.HandleFeatureChange( entry, changeType );        
        }

	// notify the delete features
    for( TInt i = 0; i < size_removed; i++ )
        {
        TFeatureServerEntry entry( removed[i].FeatureUid(), removed[i].FeatureFlags(), removed[i].FeatureData() );
        TFeatureChangeType changeType( EFeatureFeatureDeleted );
        iObserver.HandleFeatureChange( entry, changeType );
        }
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWIStart
// -----------------------------------------------------------------------------
//
TInt CFeatMgrFeatureRegistry::SWIStart(TUint aSWIProcessId)
	{
	FUNC_LOG
	
	// If a previous call to SWIStart was made then return an error indicating that SWI
	// is already running. This assures that no two exes will enable the caching
	// mechanism at the same time.
	if( iSWICacheFeature )
		{
		INFO_LOG( "CFeatMgrFeatureRegistry::SWIStart - Already in use");
		return KErrInUse;
		}
	
	RProperty propertyHndl;
	TInt err =propertyHndl.Attach(KUidSystemCategory, KSAUidSoftwareInstallKeyValue);
	if (KErrNone != err)
		{
		return err;
		}
	TInt val = -1;
	err = propertyHndl.Get(val);
	propertyHndl.Close();

	if( KErrNone == err )
		{
		// If an installation/uninstallation has started and no finishing status has been set for it
		if( ((val&ESASwisInstall) || (val&ESASwisUninstall)) && !(val&ESASwisStatusNone) )
			{
			// Set a flag to tell FeatMgr that features modified from this point onwards
		    // until a call to SWIEnd must be cached.
		    iSWICacheFeature = ETrue;
		    // SWI installation/uninstallation is in progress
		    iSWIStatus = ESWIInstalling;
		    // Set the ID of the process issuing Feature Manager commands to be cached
			iSWIProcessId = aSWIProcessId;
			// Start listening to P&S install property
		    TRAP(err, iSWIListener = CSWIListener::NewL(this));
		    if (KErrNone != err)
		    	{
		    	return err;
		    	}
		    
		    // Start the timer to handle the case of the launched exe hanging or not calling SWIEnd
		    // after SWIStart
		    TRAP(err, iSWITimer = CSWITimer::NewL(TTimeIntervalMicroSeconds32(SWITIMEOUT), this));
		    if (KErrNone != err)
		    	{
		    	return err;
		    	}
		    	
		    INFO_LOG( "CFeatMgrFeatureRegistry::SWIStart - err KErrNone");
		    return KErrNone;
			}
		}
	INFO_LOG( "CFeatMgrFeatureRegistry::SWIStart - err KErrNotReady");

	return KErrNotReady;
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWIEnd
// -----------------------------------------------------------------------------
//
TInt CFeatMgrFeatureRegistry::SWIEnd(TUint aSWIProcessId)
	{
	FUNC_LOG

	// reset the number of operations cached
	iSWIOperations = 0;
	
	// If it is the same process that made a call to SWIStart and caching is in progress
	if( iSWIProcessId == aSWIProcessId && iSWICacheFeature )
		{
		if( iSWIStatus == ESWIAborted )
			{
			SWIReset();
			
			INFO_LOG( "CFeatMgrFeatureRegistry::SWIEnd - SWIStatus = ESWIAborted");
			return KErrNone;
			}
		else if( iSWIStatus == ESWIInstalling )
			{
			// Stop time-out
			if( iSWITimer )
				{
				delete iSWITimer;
				iSWITimer = NULL;
				}
			// Stop caching
			iSWICacheFeature = EFalse;			
			
			TInt err = KErrGeneral;

			if( !iOomOccured )
				{
				if( iAddFeatCount>0 )
					{
					err = iFeatureList.Reserve(iFeatureList.Count() + iAddFeatCount);
                    if (err == KErrNoMemory)
                         {
                         iSWIStatus = ESWIOutOfMemory;
                         }
					}
				}
			else
				{
				err = KErrNoMemory;
				iSWIStatus = ESWIOutOfMemory;
				}
			
			INFO_LOG( "CFeatMgrFeatureRegistry::SWIEnd - SWIStatus = ESWIInstalling");
			return err;
			}
		}

	INFO_LOG( "CFeatMgrFeatureRegistry::SWIEnd - err KErrNotReady");
	return KErrNotReady;
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWICacheCommand
// -----------------------------------------------------------------------------
//
TInt CFeatMgrFeatureRegistry::SWICacheCommand(TSWIOperationCat aOptCat, TFeatureServerEntry aFeature)
	{
	FUNC_LOG
	
	TInt err;

	if (iSWIOperations >= MAXSWIOPS)
		{
		err = KErrArgument;
		}
	else if (iOomOccured)
		{
		err = KErrNoMemory;
		}
	else
		{
		if( aOptCat == ESWIAddFeat )
			{
			++iAddFeatCount;
			}		
		TSWICachedOperation operation;
		operation.iFeatEntry = aFeature;
		operation.iCat = aOptCat;
    	err = iSWICachedOperations.Append(operation);
    	if( err == KErrNoMemory)
    		{
    		iOomOccured = ETrue;
    		}
    	else if( err == KErrNone )
    		{
    		++iSWIOperations;
    		}
		}
	return err;
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::CommitSWIFeatChanges
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::CommitSWIFeatChanges()
	{
	FUNC_LOG
	
	// Commit all cached features.
	if( !iSWICacheFeature )
		{
		TInt count = iSWICachedOperations.Count();

		for( TInt i=0; i<count; ++i )
			{
			TSWIOperationCat optCat = iSWICachedOperations[i].iCat;
	
			switch(optCat)
				{
			case ESWIAddFeat:
				{
				AddFeature( iSWICachedOperations[i].iFeatEntry, 0 );
				}
				break;
			case ESWIDeleteFeat:
				{
				DeleteFeature( iSWICachedOperations[i].iFeatEntry.FeatureUid() );
				}
				break;
			case ESWISetFeatAndData:
				{
				TUint32 data = iSWICachedOperations[i].iFeatEntry.FeatureData();				
				SetFeature( iSWICachedOperations[i].iFeatEntry.FeatureUid(),
							iSWICachedOperations[i].iFeatEntry.FeatureFlags().Value(),
							&data);
		    	}
				break;
			case ESWISetFeatData:
				{
				TUint32 data = iSWICachedOperations[i].iFeatEntry.FeatureData();
				SetFeature(iSWICachedOperations[i].iFeatEntry.FeatureUid(),
							EFeatureSupportUntouch,&data);
				}
				break;
            case ESWISetFeat:
                {
                SetFeature( iSWICachedOperations[i].iFeatEntry.FeatureUid(),
                            iSWICachedOperations[i].iFeatEntry.FeatureFlags().Value(),
                            NULL);
                }
                break;
			default:
				break;
				};
			}
		
		INFO_LOG( "CFeatMgrFeatureRegistry::CommitSWIFeatChanges - Committing completed");
		}
	SWIReset();
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWIReset
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::SWIReset()
	{
	// Clear cached-features array
	iSWICachedOperations.Close();

	// Reset caching flag
	iSWICacheFeature = EFalse;
	
	// Reset SWI process Id
	iSWIProcessId = 1;
	
	// Reset SWI completion status
	iSWIStatus = ESWIComplete;

	// Reset the check for "out of memory" condition
	iOomOccured = EFalse;
	
	// Reset the operations counter
	iSWIOperations = 0;
	
	// No need to listen to P&S insall property any more
	delete iSWIListener;
	iSWIListener = NULL;
	
	// Stop time-out
	if( iSWITimer )
		{
		delete iSWITimer;
		iSWITimer = NULL;
		}
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWIAborted
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::SWIAborted()
	{
	FUNC_LOG
	
	// If abort occured before SWIEnd is called
	if( iSWICacheFeature )
		{
		INFO_LOG( "CFeatMgrFeatureRegistry::SWIAborted - Abort occured before SWIEnd was called");
		iSWIStatus = ESWIAborted;
		}
	else
		{
		INFO_LOG( "CFeatMgrFeatureRegistry::SWIAborted - Abort occured after SWIEnd was called");
		SWIReset();
		}
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWITimedOut
// -----------------------------------------------------------------------------
//
void CFeatMgrFeatureRegistry::SWITimedOut()
	{
	FUNC_LOG
	
	if( iSWICacheFeature )
		{
		INFO_LOG( "CFeatMgrFeatureRegistry::SWITimedOut - Timeout expired");
		SWIReset();
		}
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWICacheStarted
// -----------------------------------------------------------------------------
//
TBool CFeatMgrFeatureRegistry::SWICacheStarted()
	{
	return iSWICacheFeature;
	}

// -----------------------------------------------------------------------------
// CFeatMgrFeatureRegistry::SWICacheStatus
// -----------------------------------------------------------------------------
//
TBool CFeatMgrFeatureRegistry::SWICacheStatusOOM()
	{
	if( iSWIStatus == ESWIOutOfMemory )
		{
		return ETrue;
		}
	return EFalse;
	}
	
//  End of File
