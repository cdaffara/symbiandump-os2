/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Common security functions
*
*/


/**
 @file
*/


#include "securityutils.h"

#include <f32file.h>

// Extracts the next sub-dir name, i.e. "directory1" for "directory1\directory2\...". As a second parameter, returns the remaining path without the leading slash 
// Return ETrue iff a directory was found in the path
 TBool GetNextDirNameL(const TPtrC& aPath, TPtrC& aNextDir, TPtrC& aRemainingPath)
	{
	TInt nextSlashPos = aPath.Locate('\\');
	if (nextSlashPos < 0)
		return EFalse;
	
	aNextDir.Set(aPath.Left(nextSlashPos));
	TPtrC remainingPath = nextSlashPos <  aPath.Length() - 1 ? aPath.Mid(nextSlashPos + 1) : TPtrC();
	aRemainingPath.Set(remainingPath);
	
	return ETrue;
	}

TCapabilitySet PrivateModificationRequiredCapabilitiesL(const TDesC& aPrivateSubPath, TSecureId aClientSid)
	{
	TPtrC privateSubDirName;
	TPtrC remainingSubPath;
	TBool nextDirAvailable = GetNextDirNameL(aPrivateSubPath, privateSubDirName, remainingSubPath);
	// Filter out paths which do not have a /private/<SID> form - require TCB for them
	// First, filter out files directly under the /private directory
	if (!nextDirAvailable)
		return TCapabilitySet(ECapabilityTCB);
	    	
	if (privateSubDirName.Length() != 8)
		return TCapabilitySet(ECapabilityTCB); // Filter out sub-dir names which do not have 8 bytes, i.e. do not represent a SID
	
	TLex hexConverter(privateSubDirName);
	TUint32 foundSecureIdInt;
	if (hexConverter.Val(foundSecureIdInt, EHex) != KErrNone)
		return TCapabilitySet(ECapabilityTCB); // Filter out paths which do not have a <secureId> subdir under private
	
	TSecureId foundSecureId(foundSecureIdInt);
	if (foundSecureId != aClientSid) // Check whether this the client's SID
		{
		TPtrC nextSubPath; // Check for /private/<SID>/import directories
		TPtrC nextSubDir;
		if (!GetNextDirNameL(remainingSubPath, nextSubDir, nextSubPath) || nextSubDir.CompareF(_L("import")) != 0)
			{
			// If not an import directory, require TCB or AllFiles
			TCapabilitySet ret(ECapabilityTCB);
			ret.AddCapability(ECapabilityAllFiles);
			return ret;			
			}
		}
	
	TCapabilitySet emptySet;
	emptySet.SetEmpty();
	return emptySet;
	}
 
EXPORT_C TCapabilitySet SecCommonUtils::FileModificationRequiredCapabilitiesL(const TDesC& aFileName, TSecureId aClientSid)
	{
	TCapabilitySet emptySet;
	emptySet.SetEmpty();	
	
	// TParsePtrC is unusable, since it panics on incorrect paths. We have to use TParse and create a temporary buffer for it (as there's no TParseC)
	RBuf tempbuf;
	tempbuf.CreateL(aFileName);
	tempbuf.CleanupClosePushL();
	
	TParse pathParse;
	if (pathParse.Set(tempbuf, NULL, NULL) != KErrNone)
		{
		CleanupStack::PopAndDestroy(&tempbuf);
		// Path failed to parse - require TCB, as it is the only capability which allows modification anywhere on the FS
		// (and we do not know where on the FS this file is)
		return TCapabilitySet(ECapabilityTCB); 
		}
		
	CleanupStack::PopAndDestroy(&tempbuf);

	// check for wild cards (such as * or ?) in paths 
    if(pathParse.IsWild())
        return TCapabilitySet(ECapabilityTCB);

	// check for relative paths
    if(aFileName.Find(_L("..")) != KErrNotFound)
        return TCapabilitySet(ECapabilityTCB);

    TPtrC pathTmp = pathParse.Path();
    if (pathTmp.Length() <= 1) // The should be at least one directory - otherwise there's nothing to check
    	return emptySet;
    
    // Get the first directory name    
    TPtrC path = pathTmp.Mid(1); // Remove the leading slash
    
    TPtrC firstDirName;
    TPtrC remainingPath;
    TBool nextDirAvailable = GetNextDirNameL(path, firstDirName, remainingPath);
    __ASSERT_ALWAYS(nextDirAvailable, User::Invariant()); // There should be at least one directory if the Path() was not empty
        
    // For 'sys' or 'resource', require TCB
    if (firstDirName.CompareF(_L("sys")) == 0 || firstDirName.CompareF(_L("resource")) == 0)
    	return TCapabilitySet(ECapabilityTCB);
    
    if (firstDirName.CompareF(_L("private")) == 0)
    	return PrivateModificationRequiredCapabilitiesL(remainingPath, aClientSid);
    	
    // If the directory name is not 'private', 'resource' or 'sys', no capabilities are required
	return emptySet;
	}	
