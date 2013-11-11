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
// Helper class for ECom test code. Allows various file manipulation operations
// that require higher Capabilities than shoud be given to test harnesses.
// 
//

#include "EcomTestUtils.h"
#include "hal.h"
#include <e32rom.h>

_LIT(KEcomTestUtilsHelperPanic, "EComTestUtilsHelperPanic");
_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames
const TInt KMAxProcessNameLength = 50; // Max process name length.

// Processes with high Capablilities to perform restricted operations.
_LIT(KProcessMakeWriteable, "t_makefilewriteable");
_LIT(KProcessMakeReadOnly, "t_makefilereadonly");
_LIT(KProcessFileManDeleteFile, "t_processfilemandeletefile.exe");
_LIT(KProcessFileManCopyFile, "t_processfilemancopyfile.exe");
_LIT(KProcessFileManRename, "t_processfilemanrename.exe");
_LIT(KProcessRfsDeleteFile, "t_processrfsdeletefile.exe");
_LIT(KProcessRfsReplaceFile, "t_processrfsreplacefile.exe");
_LIT(KProcessKillProcess, "t_processkillprocess.exe");
_LIT(KProcessFileManDeleteDir, "t_processfilemandeletedir.exe");
_LIT(KProcessRLoaderDeleteFile, "t_processrloaderdeletefile.exe");

/** Name format for locale DLLs. */
_LIT(KLocaleDLLNameFormat, "ELOCL.%02d");


// Start the high capability helper process 
void LaunchProcessL(const TDesC& aProcessName, const TDesC& aCmdLine)
{
	TRequestStatus stat;    
	RProcess p;
		User::LeaveIfError(p.Create(aProcessName, aCmdLine));

	// Asynchronous logon: completes when process terminates with process 
	// exit code
	p.Logon(stat);
	p.Resume();
	User::WaitForRequest(stat);

	TExitType exitType = p.ExitType();
	TInt exitReason = p.ExitReason();
	__ASSERT_ALWAYS(exitType == EExitKill, User::Panic(KEcomTestUtilsHelperPanic, exitReason));

	p.Close();
	User::LeaveIfError(exitReason);
}

/**
CFileMan
*/ 
EXPORT_C void EComTestUtils::FileManCopyFileL(const TDesC& anOld,const TDesC& aNew)
	{
	TBuf<KMaxFileName*2> fileNames(anOld);
	fileNames.Append(KSeparator);
	fileNames.Append(aNew);
	TBufC<KMAxProcessNameLength> copyProcess(KProcessFileManCopyFile);	
	LaunchProcessL(copyProcess, fileNames);
	}

EXPORT_C void EComTestUtils::FileManDeleteFileL(const TDesC& aFile)
	{
	TBufC<KMAxProcessNameLength> deleteProcess(KProcessFileManDeleteFile);	
	LaunchProcessL(deleteProcess, aFile);
	}

EXPORT_C void EComTestUtils::FileManRenameL(const TDesC& anOld,const TDesC& aNew)
	{
	TBuf<KMaxFileName*2> fileNames(anOld);
	fileNames.Append(KSeparator);
	fileNames.Append(aNew);
	TBufC<KMAxProcessNameLength> renameProcess(KProcessFileManRename);	
	LaunchProcessL(renameProcess, fileNames);
	}

EXPORT_C void EComTestUtils::FileManDeleteDirL(const TDesC& aPath)
	{
	TBufC<KMAxProcessNameLength> deleteDirProcess(KProcessFileManDeleteDir);	
	LaunchProcessL(deleteDirProcess, aPath);
	}

/**
RFs
*/ 
EXPORT_C void EComTestUtils::RfsDeleteFileL(const TDesC& aFile)
	{
	TBufC<KMAxProcessNameLength> deleteProcess(KProcessRfsDeleteFile);	
	LaunchProcessL(deleteProcess, aFile);
	}
	
EXPORT_C void EComTestUtils::RfsReplaceFileL(const TDesC& anOld,const TDesC& aNew)
	{
	TBuf<KMaxFileName*2> fileNames(anOld);
	fileNames.Append(KSeparator);
	fileNames.Append(aNew);
	TBufC<KMAxProcessNameLength> replaceProcess(KProcessRfsReplaceFile);	
	LaunchProcessL(replaceProcess, fileNames);
	}


EXPORT_C void EComTestUtils::MakeFileWriteableL(const TDesC& aFile)
	{
	TBufC<KMAxProcessNameLength> makeWriteable(KProcessMakeWriteable);	
	LaunchProcessL(makeWriteable, aFile);
	}
    
EXPORT_C void EComTestUtils::MakeFileReadOnlyL(const TDesC& aFile)
	{
	TBufC<KMAxProcessNameLength> makeReadOnly(KProcessMakeReadOnly);   
	LaunchProcessL(makeReadOnly, aFile);
	}    

EXPORT_C void EComTestUtils::KillProcessL(const TDesC& aProcessName)
	{
	TBuf<KMAxProcessNameLength> killProcess(KProcessKillProcess);
	LaunchProcessL(killProcess, aProcessName);
	}


/**
Switch the current locale to the given language.
@param aLang The language to switch to.
@leave KErrNotFound If locale DLL corresponding to the language cannot be found.
*/
EXPORT_C void EComTestUtils::SwitchToLanguageL(TLanguage aLang)
	{
	RLibrary library;
	TBuf<50> libraryName;
	libraryName.Format(KLocaleDLLNameFormat,aLang);
	User::LeaveIfError(library.Load(libraryName));
	CleanupClosePushL(library);
	User::LeaveIfError(UserSvr::ChangeLocale(libraryName));
	CleanupStack::PopAndDestroy();	// library
	}


/**
Ask the loader to delete an file. This function should be used instead
of RFs::Delete where the supplied file may be a paged executable, although
it can be used for any file.
@param aFile The file to be deleted
@leave KErrBadName If the supplied filename is not fully qualified. Else any of the Symbian OS error code
*/
EXPORT_C void EComTestUtils::RLoaderDeleteFileL(const TDesC& aFile)
	{
	TBufC<KMAxProcessNameLength> deleteTheProcess(KProcessRLoaderDeleteFile);	
	LaunchProcessL(deleteTheProcess, aFile);
	}

/**
Get Rom Build Type i.e. NAND or Default Rom build.
This function is created since in some test one needs to find if its a 
NAND or a Non-Nand build. It returns enum "TRomBuildOption" that states the rom build type.
Enum "TRomBuildOption" is defined so that this function can be enhanced for a new Rom build type
@param aRfs Input Parameter RFs reference is passed so that we don't need to 
connect to the file server again.
*/
EXPORT_C TRomBuildOption EComTestUtils::RomBuildType(const RFs& aRfs)
	{
	
	TRomBuildOption romBuildOption;
	TDriveInfo driveInfo;
	
	aRfs.Drive(driveInfo, EDriveC);
		
	//C drive determines if its a NAND or a Non-NAND ROM build as per the 
	//H4-hrp-UserGuide, Thus CDrive's media type is checked if it is 
	//EMediaNANDFlash, EMediaRam or EMediaFlash...
	//If it is EMediaRam or EMediaFlash it is considered as Default Rom Build. 
	if(driveInfo.iType==EMediaNANDFlash)
		{
		romBuildOption = ENandRomBuild;
		}
	else
		{
		romBuildOption = EDefaultRomBuild;
		}

	return romBuildOption;
	}

/*
Method is used to detect the hardware configuration of the platform running the tests, 
including both machine UID and C-drive type.
*/
EXPORT_C THardwareConfiguration EComTestUtils::GetHardwareConfiguration()
 	{
 	//get the platform type
 	TInt muid;
	if (KErrNone != HAL::Get(HAL::EMachineUid, muid))
		{	
		return EPlatformUnknown;
		}

	//determine the ROM confguration (NAND or non-NAND)
	TInt r = KErrNone;
	RFs fs;
	r = fs.Connect();
	if (r != KErrNone)
		{
			return r;
		}
	TBool nandBuild = (EComTestUtils::RomBuildType(fs)==ENandRomBuild);
	fs.Close();
	
	//determine whether this ROM is Demand-Paging enabled
	TBool DP_rom = false;	
	if (nandBuild)
		{
		TRomHeader* romHeader = (TRomHeader*)UserSvr::RomHeaderAddress();
		DP_rom = romHeader->iPageableRomStart;	
		}

	if (muid == HAL::EMachineUid_OmapH2)
		{
		if (!nandBuild)
			{
			return EPlatformH2RAM;		    		    		
			}
		else
			{
			if (DP_rom)
				{
				return EPlatformH2NANDDP;
				}
			else 
				{
				return EPlatformH2NAND;
				}
			}
		}
	else if (muid == HAL::EMachineUid_OmapH4)
		{
		if (!nandBuild)
			{
			//Incase the ROM is not a nandBuild, 
			//then there could be a chance that it is a WDP enabled system.
			//WDP enabled configuration is checked by checking for Media Type of C drive.
			//Under WDP enabled configuration, the C drive is of type 'Hard Disk'
			//This is done since the function EComTestUtils::RomBuildType, above, used for a similar purpose,
			//is also checking the media type of C drive to determine the same.
			//Currently added this check for WDP enabled in case of H4 'muid == HAL::EMachineUid_OmapH4' only
			TDriveInfo driveInfo;
			TInt rf = KErrNone;
			rf = fs.Connect();
			if (rf != KErrNone)
		  		{
			  return rf;
		  		}
			fs.Drive(driveInfo, EDriveC);
			fs.Close();

			if (EMediaHardDisk == driveInfo.iType)
				{
					return EPlatformH4MMC; //The system is WDP enabled
				}
			
			return EPlatformH4RAM;		    		    		
			}
		else
			{
			if (DP_rom)
				{
				return EPlatformH4NANDDP;
				}
			else 
				{
				return EPlatformH4NAND;
				}	    		    		    		    		
			}
		}
	//For H6 board OMAP3430
	else if (muid == HAL::EMachineUid_OmapH6)
		{
		if (!nandBuild)
			{
			return EPlatformH6RAM;		    		    		
			}
		else
			{
			if (DP_rom)
				{
				return EPlatformH6NANDDP;
				}
			else 
				{
				return EPlatformH6NAND;
				}	    		    		    		    		
			}
		}	// H6 board 3430 ends here
	else if (muid == HAL::EMachineUid_Win32Emulator)
		{		
		return EPlatformWINSCW;
		}
	else
		{		   		
		return EPlatformUnknown;
		}
	}

