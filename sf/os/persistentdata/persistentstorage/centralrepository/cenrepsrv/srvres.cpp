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
//

#include "srvres.h"
#include "cachemgr.h"
#include "obsrvr_noc.h"
#include <bautils.h> // BaflUtils::GetSystemDrive
#ifdef SYMBIAN_BAFL_SYSUTIL
#include <bafl/sysutil.h>
#endif
RFs TServerResources::iFs;

HBufC* TServerResources::iRomDirectory;
HBufC* TServerResources::iDataDirectory;
HBufC* TServerResources::iInstallDirectory;
HBufC* TServerResources::iBURDirectory;


HBufC* TServerResources::iIniExt;
HBufC* TServerResources::iCreExt;
HBufC* TServerResources::iTrnsExt;

TUint8 TServerResources::iPersistsVersion;

RArray<TOwnerIdMapping> TServerResources::iOwnerIdLookUpTable;

CRepositoryCacheManager* TServerResources::iCacheManager;
CObservable* TServerResources::iObserver;


#ifdef __CENTREP_SERVER_PERFTEST__
TCentRepPerfTest TServerResources::iPerfTestMgr;
#endif
#ifdef __CENTREP_SERVER_MEMTEST__
TInt32 TServerResources::iMemTestData[KMemBufMaxEntry];
TInt32 TServerResources::iMemTestDataCount = 0;
#endif //__CENTREP_SERVER_MEMTEST__

TTime TServerResources::CentrepFileTimeStampL(TUid aUid, TCentRepLocation aLocation)
	{
	TEntry entry;
	HBufC* fileName(NULL);
	TServerResources::CreateRepositoryFileNameLC(fileName,aUid,aLocation,ECre);
	TInt err=TServerResources::iFs.Entry(fileName->Des(), entry);
   	CleanupStack::PopAndDestroy(fileName);

	if(err==KErrNone)
		{
		return entry.iModified;
		}
		
	TServerResources::CreateRepositoryFileNameLC(fileName,aUid,aLocation,EIni);
	User::LeaveIfError(TServerResources::iFs.Entry(fileName->Des(), entry));
   	CleanupStack::PopAndDestroy(fileName);
   			   					  
	return entry.iModified;
	}
	
TBool TServerResources::CentrepFileExistsL(TUid aUid, TCentRepLocation aLocation, TCentRepFileType aType)
	{
	HBufC* fileName(NULL);
	TServerResources::CreateRepositoryFileNameLC(fileName,aUid,aLocation,aType);
	TEntry entry;
	TInt err=TServerResources::iFs.Entry(fileName->Des(), entry);
   	CleanupStack::PopAndDestroy(fileName);		   					  
 	
 	TBool r=EFalse;
 	
 	if(err==KErrNone)
 		{
 		r=ETrue;
 		}
 	else if(err==KErrNotFound)
 		{
 		r=EFalse;
 		}
 	// Looking for a file on a composite file system may return KErrPathNotFound when 
 	// the ROM file doesn't exist, so check for this return code as well.
 	else if((aLocation==ERom) && (err== KErrPathNotFound))
  		{
 		r=EFalse;
 		}
 	else
 		{
 		User::Leave(err);
 		}
 #ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
 	//only for ROM we still have to consider for multiple ROFS presence	
	if (aLocation==ERom && r==EFalse)
		{
		//if not still have to check for multi rofs, it might be the case that first file is already mangled
		TMultiRofsList find(aUid);
		TLinearOrder<TMultiRofsList> sort_order(CObservable::CompareUid);		
					
		TInt index=iObserver->GetMultiRofsList().FindInOrder(find,sort_order);		
		if (index!=KErrNotFound)	
			r=ETrue;
		}
#endif		
  	return r;	 
	}

TBool TServerResources::CentrepFileExistsL(TUid aUid, TCentRepLocation aLocation)
	{
	return( CentrepFileExistsL( aUid, aLocation, EIni) || CentrepFileExistsL( aUid, aLocation, ECre));
	}
	
TBool TServerResources::InstallFileExistsL(TUid aUid)
	{
	return CentrepFileExistsL(aUid, EInstall);
	}

TBool TServerResources::RomFileExistsL(TUid aUid)
	{
	return CentrepFileExistsL(aUid, ERom);
	}
	
TBool TServerResources::PersistsFileExistsL(TUid aUid)
	{
	return CentrepFileExistsL(aUid, EPersists);
	}	

#ifdef SYMBIAN_BAFL_SYSUTIL
TInt TServerResources::GetTextFromFile( const TDesC& aFilename, TDes8& aValue)
    {

    TInt err = KErrNone;
    RFile file;
    err = file.Open( iFs, aFilename,
                     EFileRead | EFileStreamText | EFileShareReadersOnly );
    if (err != KErrNone)
        {
        return err;
        }

    err = file.Read(aValue);
    
    file.Close();

    return err;
    }
#endif		
void TServerResources::DeleteCentrepFileL(TUid aUid, TCentRepLocation aLocation, TCentRepFileType aType)
	{
	HBufC* fileName(NULL);
	TServerResources::CreateRepositoryFileNameLC(fileName,aUid,aLocation,aType);
	if(CentrepFileExistsL( aUid, aLocation, aType))	
		{
		User::LeaveIfError(TServerResources::iFs.Delete(fileName->Des()));
		}
	CleanupStack::PopAndDestroy(fileName);	 
	}	

void TServerResources::CreateRepositoryFileNameLC(HBufC*& aFullFileName,
												  TUid aUid,
                                                  TCentRepLocation aLocation,
                                                  TCentRepFileType aFileType)
    {
	TServerResources::CreateRepositoryFileNameL(aFullFileName,aUid,aLocation,aFileType);
	CleanupStack::PushL(aFullFileName);
    }
/**
Generic routine for creating a full repository file name.
aFullFileName is created on the heap and it is caller responsibility
to delete it. 
*/
void TServerResources::CreateRepositoryFileNameL(HBufC*& aFullFileName,
												  TUid aUid,
                                                  TCentRepLocation aLocation,
                                                  TCentRepFileType aFileType)
    {
    const TInt KExtLen = 4;
    const TInt KDirLen = 40;
    const TInt KUidLen = 8;

    TBuf<KDirLen> directory;
    TBuf<KExtLen> ext;

	//path
	switch (aLocation)
	    {
	    case EPersists:
	        {
	        directory.Copy(iDataDirectory->Des());
	        }
	    	break;
	    case EInstall:
	        {
	        directory.Copy(iInstallDirectory->Des());
	        }
	    	break;
	    case ERom:
	        {
	        if (iRomDirectory)
	        	{
	        	directory.Copy(iRomDirectory->Des());	        	
	        	}
			else
				{
				User::Leave(KErrNotFound); //CentRep ROM directory is empty or doesn't exist at all
				}
	        }
	    	break;
	    default:
	        User::Leave(KErrNotFound); //should never get here
	    }
	    
    //file name	    
	TBuf<KUidLen> name;
	name.NumFixedWidth(aUid.iUid, EHex, KUidLen);

	//extension
    switch (aFileType)
        {
        case ECre:
            {
            ext.Copy(iCreExt->Des());
            }
            break;
        case EIni:
            {
            ext.Copy(iIniExt->Des());
            }
            break;
        case ETmp:
	    	{
	    	ext.Copy(iTrnsExt->Des());
	    	}
	    	break;
        default:
	        User::Leave(KErrNotFound); //should never get here
        }

    TBuf<KMaxFileName>  fullFileName;
    fullFileName.Append(directory);
    fullFileName.Append(name);
    fullFileName.Append(ext);
    //allocates memory on the heap. It is caller's resposibility to delete aFullFileName
    aFullFileName = fullFileName.AllocL(); 
    }

void TServerResources::InitialiseL()
	{
#ifndef SYMBIAN_CENTREP_SUPPORT_MULTIROFS
	iPersistsVersion = KPersistFormatVersion;	// Version 0 of persists
#else
	iPersistsVersion = KPersistFormatSupportsIndMetaIndicator;
#endif	
	User::LeaveIfError(iFs.Connect());

	// get system drive
	_LIT(KDriveMask, "_:");
	TDriveName systemDrive(KDriveMask);
	systemDrive[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
		
	_LIT(KRomDrive, "z:");			// This may not always be z:
	_LIT(KPersistsDir, "persists\\");
	_LIT(KBURDir, "bur\\");

	
	// File extensions
	
	_LIT(KIniFileExtension, ".txt");
	_LIT(KExternalizedPersistsFileExt, ".cre");
	_LIT(KTransactFileExt, ".tmp");

	const TInt KMaxExtLength=4;
	
	iIniExt=HBufC::NewL(KMaxExtLength);
	iCreExt=HBufC::NewL(KMaxExtLength);
	iTrnsExt=HBufC::NewL(KMaxExtLength);

	iIniExt->Des().Copy(KIniFileExtension);
	iCreExt->Des().Copy(KExternalizedPersistsFileExt);
	iTrnsExt->Des().Copy(KTransactFileExt);
	
	TBuf<KMaxFileName> path;
	User::LeaveIfError(iFs.PrivatePath(path));

	const TInt pathLen = path.Length();
	
	//
	// ROM-drive cenrep directory
	//
	iRomDirectory = HBufC::NewL(KRomDrive().Length()+pathLen);
	TPtr ptr(iRomDirectory->Des());
	ptr.Append(KRomDrive);
	ptr.Append(path);
	// If the ROM directory does not exist (very unlikely) we set iRomDirectory to zero.
	TEntry fsEntry;
	if(iFs.Entry(*iRomDirectory, fsEntry)!=KErrNone || !fsEntry.IsDir())
		{
		delete iRomDirectory;
		iRomDirectory = NULL;
		}

	//
	// Cenrep install directory
	//
	iInstallDirectory = HBufC::NewL(systemDrive.Length()+pathLen);
	ptr.Set(iInstallDirectory->Des());
	ptr.Append(systemDrive);
	ptr.Append(path);
	TInt r = iFs.MkDirAll(*iInstallDirectory);
	if(r!=KErrNone && r!=KErrAlreadyExists)
		{
		User::Leave(r);		
		}
	//
	// Writeable-drive data directory
	//

	iDataDirectory = HBufC::NewL(systemDrive.Length()+pathLen+KPersistsDir().Length());
	ptr.Set(iDataDirectory->Des());
	ptr.Append(systemDrive);
	ptr.Append(path);
	ptr.Append(KPersistsDir);
	
	r = iFs.MkDirAll(*iDataDirectory);
	if(r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);

	//
	// Writeable-drive backup/restore directory
	//
	iBURDirectory = HBufC::NewL(systemDrive.Length()+pathLen+KBURDir().Length());
	ptr.Set(iBURDirectory->Des());
	ptr.Append(systemDrive);
	ptr.Append(path);
	ptr.Append(KBURDir);
	
	r = iFs.MkDirAll(*iBURDirectory);
	if(r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);

#ifdef SYMBIAN_BAFL_SYSUTIL	
	//
	// romversion directory
	//
	_LIT(KRomCache,"romversion\\");
	HBufC* romVersionDir = HBufC::NewLC(systemDrive.Length()+pathLen+KRomCache().Length());
	
	ptr.Set(romVersionDir->Des());
	ptr.Append(systemDrive);
	ptr.Append(path);
	ptr.Append(KRomCache);
	
	r = iFs.MkDirAll(*romVersionDir);
	if(r!=KErrNone && r!=KErrAlreadyExists)
		User::Leave(r);
	CleanupStack::PopAndDestroy();//romVersionDir
#endif
	//
	// Cache Manager 
	//
	iCacheManager = CRepositoryCacheManager::NewLC(iFs);
	CleanupStack::Pop();
	
	// Observer
	iObserver = CObservable::NewLC();
	CleanupStack::Pop();
	
	iOwnerIdLookUpTable.Reset();

	}

void TServerResources::Close()
	{
	delete iObserver;
	delete iCacheManager;
	delete iInstallDirectory;
	delete iDataDirectory;
	delete iRomDirectory;
	delete iBURDirectory;
	delete iIniExt;
	delete iCreExt;
	delete iTrnsExt;
	iFs.Close();

	iCacheManager=NULL;
	iInstallDirectory=NULL;
	iDataDirectory=NULL;
	iRomDirectory=NULL;
	iBURDirectory=NULL;	
	iIniExt=NULL;	
	iCreExt=NULL;	
	iTrnsExt=NULL;	
	
	iOwnerIdLookUpTable.Close() ;
	}
	
TInt TServerResources::GetUid(TEntry& aEntry, TUid& aUid)
	{
		const TInt KUidLen = 8;
		TPtrC uidPtr = aEntry.iName.Des().LeftTPtr(KUidLen);
		TLex  lex=uidPtr;
		TUint32 uidValue;

		if (lex.Val(uidValue, EHex) == KErrNone)
			{
			aUid = TUid::Uid(uidValue);
			}
		else
			{
			return KErrNotFound;
			}
			
	return KErrNone;
	}

TInt TOwnerIdMapping::CompareUids (const TOwnerIdMapping& aOwnerIdMapping1, const TOwnerIdMapping& aOwnerIdMapping2) 
	{
	if (aOwnerIdMapping1.iRepUid < aOwnerIdMapping2.iRepUid)
		return -1 ;
	else if (aOwnerIdMapping1.iRepUid > aOwnerIdMapping2.iRepUid)
		return 1 ;
	else
		return 0 ;
	}
		

#ifdef __CENTREP_SERVER_MEMTEST__

void TServerResources::StopRecordTimerResult()
	{
	iMemTestDataCount = KMemBufMaxEntry;
	}

//aLocation: location where the memory reading is done (a method specifier)
//aIdentifier: identifier of the memory reading (e.g. repository id, 10th reading etc)
void TServerResources::RecordTimerResult(TMemTestLocationIdentifier aLocation, TInt32 aIdentifier)
	{
	if(iMemTestDataCount+6 > KMemBufMaxEntry)
		{
		if(iMemTestDataCount+3 <= KMemBufMaxEntry)
			{
			iMemTestData[iMemTestDataCount++] = static_cast<TInt32>(KMagicMemTestOutOfBounds);
			iMemTestData[iMemTestDataCount++] = static_cast<TInt32>(KMagicMemTestOutOfBounds);
			iMemTestData[iMemTestDataCount++] = static_cast<TInt32>(KMagicMemTestOutOfBounds);
			}
		}
	else
		{
		RHeap& heap = User::Heap();
		TInt biggestBlock;
		
		iMemTestData[iMemTestDataCount++] = aLocation;
		iMemTestData[iMemTestDataCount++] = aIdentifier;
		iMemTestData[iMemTestDataCount++] = static_cast<TInt32>(heap.Size() - heap.Available(biggestBlock));
		}
	}
	
void TServerResources::StartRecordTimerResult()
	{
	iMemTestDataCount = 0;
	}
	
#endif //__CENTREP_SERVER_MEMTEST__
