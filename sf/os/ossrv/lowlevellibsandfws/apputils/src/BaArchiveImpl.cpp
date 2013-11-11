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

#include <barsc2.h>
#include "BaArchiveImpl.h"
#include <baflpan.h>
#include <bautils.h>

/** TCleanupItem function.
@internalComponent
*/
LOCAL_C void CloseAndDeleteFile(TAny* aFile)
	{
	RFile* const file=static_cast<RFile*>(aFile);
	if (file!=NULL)
		{
		file->Close();
		delete file;
		}
	}

/** TCleanupItem function.
@internalComponent
*/
LOCAL_C void CloseAndDeleteChunk(TAny* aChunk)
	{
	RChunk* const rchunk=static_cast<RChunk*>(aChunk);
	if (rchunk!=NULL)
		{
		rchunk->Close();
		delete rchunk;
		}
	}

/** Default Constructor
@internalComponent
*/
CResourceArchiveImpl::CResourceArchiveImpl() :
	iCurrentIndex(0),
	iSpiFileType(KNullUid)
	{}

/** Class destructor
@internalComponent 
*/
CResourceArchiveImpl::~CResourceArchiveImpl()
	{
	iRscList.Close();
	
	for(TInt i=0; i<iSpiChunkBufferArray.Count(); i++)
		{
		RChunk* const chunk = iSpiChunkBufferArray[i];                     
		chunk->Close();
		delete chunk;
		}
	
	iSpiChunkBufferArray.Reset();
	iSpiChunkBufferArray.Close();

	iSpiBufferArray.ResetAndDestroy();
	iSpiBufferArray.Close();
	iCurrentIndex=0;
	iSpiFileType=KNullUid;
#ifdef __BASPITEST__
	iSpiFileNameArray.Reset();
	iSpiFileNameArray.Close();
#endif	
	}

/** Creates a CResourceArchiveImpl instance to read a single spi file
@internalComponent
*/
CResourceArchiveImpl* CResourceArchiveImpl::NewL(RFs& aFs,const TDesC& aName)
	{
	CResourceArchiveImpl* self=new (ELeave) CResourceArchiveImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aName);
	CleanupStack::Pop();
	return self;
	}

/** Creates a CResourceArchiveImpl instance to read a set of spi files
@internalComponent
*/	
CResourceArchiveImpl* CResourceArchiveImpl::NewL(RFs& aFs,const TDesC& aPath,const TDesC& aPattern)
	{
	CResourceArchiveImpl* self=new (ELeave) CResourceArchiveImpl;
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aPath,aPattern);
	CleanupStack::Pop();
	return self;	
	}

/** ConstructL method of CResourceArchiveImpl
@internalComponent
*/
void CResourceArchiveImpl::ConstructL(RFs& aFs,const TDesC& aName)
	{
	RArray<TPtrC8> hiddenRscList;
	CleanupClosePushL(hiddenRscList);
	//Open the file and initialise the buffer pointer
	TPtr8 bufferPtr(NULL,0,0);
	OpenFileL(aFs,aName,bufferPtr);
	//Now validate the header
	ValidateHeaderL(bufferPtr);
	//Process all the rsc entry
	ProcessEntryL(bufferPtr,hiddenRscList);	
	CleanupStack::PopAndDestroy();
	}
	

/** Function to get the string representation of a language code
assume language code has max N digits currently equal to ELangMaximum
note that the minimum suffix required is NN so TLang(0-9) needs to have a 0
e.g s01...s09, s10, s100 etc
@internalComponent
*/
static void GetLangCodeStringRep(TLanguage lang,TDes& aStringRep)
{
	aStringRep.Zero();
	//special case for 0-9 where you need the 0 appended, langcode min two digit
	if (lang<10)
		aStringRep.AppendNumFixedWidthUC(lang,EDecimal,2);
	else
		aStringRep.AppendFormat(_L("%d"),lang);
}

/**
Open the file and initialise the aBufferPtr to point at the buffer
@internalComponent
*/
void CResourceArchiveImpl::OpenFileL(RFs& aFs,const TDesC& aName,TPtr8& aBufferPtr)
	{
	#ifdef __WINS__
	TUint8* romAddress=NULL;
	#else // ! __WINS__
	TUint8* romAddress=aFs.IsFileInRom(aName);
	#endif // ! __WINS__
	
	RFile* const file=new(ELeave) RFile;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteFile,file));
	User::LeaveIfError(file->Open(aFs, aName, EFileStream | EFileRead | EFileShareReadersOnly));
	TInt fileSize=0;
	User::LeaveIfError(file->Size(fileSize));						
	if (romAddress)
		{	
		aBufferPtr.Set(romAddress,fileSize,fileSize);
		}
	else
		{
		RChunk* rchunk = new(ELeave) RChunk;
		
		CleanupStack::PushL(TCleanupItem(CloseAndDeleteChunk,rchunk));
		
		TInt createRet = rchunk->CreateDisconnectedLocal(0, 0, fileSize);
		TInt commitRet = rchunk->Commit(0, fileSize);
		
		// Create a chunk to store the large file contents of which is closed and destroyed in destructor...
		if(createRet==KErrNone && commitRet==KErrNone)
			{
			TUint8* chunkBase = (TUint8*)rchunk->Base();
			TPtr8 tempBuffer(chunkBase, fileSize);
			User::LeaveIfError(file->Read(0, tempBuffer, fileSize));
			aBufferPtr.Set(tempBuffer);
			iSpiChunkBufferArray.AppendL(rchunk);
			
			CleanupStack::Pop(rchunk);
			}
		else	// Unable to create a chunk so use heap memory...
			{
			HBufC8* fileBuffer=HBufC8::NewMaxLC(fileSize);
			aBufferPtr.Set(fileBuffer->Des());
			User::LeaveIfError(file->Read(0,aBufferPtr,fileSize));
			iSpiBufferArray.AppendL(fileBuffer);
			
			CleanupStack::Pop(fileBuffer);
			CleanupStack::PopAndDestroy(rchunk);
			}
		}
	//can close the file now
	CleanupStack::PopAndDestroy(file);
#ifdef __BASPITEST__
	iSpiFileNameArray.AppendL(aName);
#endif		
	}

/**
Validate the spi header in the file buffer
On return it will update the bufferPtr to point to start of the first
rsc entry
@internalComponent
*/
void CResourceArchiveImpl::ValidateHeaderL(TPtr8& aBufferPtr)
	{
	//Getting the 32 bytes header information. At the moment maybe we should just buffer 16 bytes
	//of the header as the remaining 16 bytes are padding bytes
	TUidType uidType=TCheckedUid(aBufferPtr.Left(16)).UidType();			
	if (uidType[0]!=KSpiFileUid)
		User::Leave(KErrCorrupt);
	//now get the spi file type
	TUid spiFileType=TUid::Uid(uidType[1].iUid);
	if (iSpiFileType==KNullUid)
		iSpiFileType=spiFileType;
	//also check consistency with previous spi files
	__ASSERT_DEBUG(iSpiFileType==spiFileType,::Panic(EBafPanicBadResourceFileFormat));							

	//update the bufferPtr to point to start of first rsc entry
	aBufferPtr.Set(aBufferPtr.MidTPtr(KSpiFirstRscOffset));	
	}

/**
Process all the entry found in the buffer and update the internal rscList
@internalComponent
*/
void CResourceArchiveImpl::ProcessEntryL(TPtr8& aBufferPtr,RArray<TPtrC8>& aHiddenList)
	{
	//now traverse content of the spi file and build up the hidden list and the TRscEntry array
	TRscEntry rscEntry;
	while (aBufferPtr.Length()>0)
		{
		//length(first 4 bytes) and the actual rsc file size(second 4 bytes)
		//Retrieving the rscfilename length
		TUint32 rscFileNameLength=LittleEndianFourByteInteger(aBufferPtr,0);
		TUint32 rscFileSize=LittleEndianFourByteInteger(aBufferPtr,4);
		TUint32 paddingbyte=(4-((rscFileNameLength+rscFileSize)%4))%4;
		__ASSERT_DEBUG((rscFileNameLength+rscFileSize+paddingbyte)%4==0,::Panic(EBafPanicFileSize));
		//construct the TRscEntry				
		rscEntry.iRscName.Set(aBufferPtr.Mid(8,rscFileNameLength));
		rscEntry.iRscData.Set(aBufferPtr.Mid(8+rscFileNameLength,rscFileSize));
#ifdef __BASPITEST__
		rscEntry.iFileNamePtr.Set(iSpiFileNameArray[iSpiFileNameArray.Count()-1].Mid(0));
#endif				
		//update the buffer pointer
		aBufferPtr.Set(aBufferPtr.MidTPtr(8+rscFileNameLength+rscFileSize+paddingbyte));

		//process the TRscEntry
		if (rscEntry.iRscData.Length()==0)
			aHiddenList.AppendL(rscEntry.iRscName);
		else
			{
			//if can find a matching resource name entry in the hidden list ignore this entry
			TIdentityRelation<TPtrC8> identity(MatchDescriptor);
			if (aHiddenList.Find(rscEntry.iRscName,identity)==KErrNotFound)
				{
				//note no duplicate entry, this implies the lastly mounted resource
				//entry is preferred over earlier mounted resource entry.(REPLACING)
				TInt ret=iRscList.InsertInOrder(rscEntry,EntryOrder);
				if (ret!=KErrNone && ret!=KErrAlreadyExists)
					User::Leave(ret);
				}		
			}
		}//end while loop	
	}
	
/** ConstructL method that accepts a spi path and a matching pattern
aPath here must end with \\ as well e.g. z:\\private\\10009d8f\\ 
aPattern should be the spi file withou rom image id and extension
@internalComponent
*/
void CResourceArchiveImpl::ConstructL(RFs& aFs,const TDesC& aPath,const TDesC& aPattern)
	{
	//Get the downgradepath
	RArray<TLanguage> downgradeList;
	BaflUtils::GetDowngradePathL(aFs,User::Language(),downgradeList);
	CleanupClosePushL(downgradeList);
	
	//used to check if there is any matching pattern and leave with KErrNotFound if
	//no matching spi
	TInt spiDiscovered=0;
	
	//sort out the language specific spi first
	for (int i=0;i<=downgradeList.Count();i++)	
		{
		//list all the files in the directory that matches this language
		TFileName matchPattern;
		TBuf<5> langExtension;
		if (i<downgradeList.Count())
			GetLangCodeStringRep(downgradeList[i],langExtension);
		else
			{
			//process the default spi lastly
			langExtension.Append(_L("pi"));			
			}
		matchPattern.AppendFormat(_L("%S%S-*-*.s%S"),&aPath,&aPattern,&langExtension);

		//Get all the spis that match the language pattern
		CDir* spiList=NULL;
		User::LeaveIfError(aFs.GetDir(matchPattern,KEntryAttReadOnly | KEntryAttHidden | KEntryAttSystem | KEntryAttArchive,ESortByName,spiList));
		CleanupStack::PushL(spiList);

		//the hidden list is only a temporary variable for each language
		RArray<TPtrC8> hiddenRscList;
		CleanupClosePushL(hiddenRscList);
		
		//reverse processing
		for (TInt i=spiList->Count()-1;i>=0;i--)
			{
			//reuse the matchPattern buffer to hold the full path of the spi file name
			matchPattern.Zero();
			matchPattern.Append(aPath);
			matchPattern.Append((*spiList)[i].iName);

			//Open the file and initialise the buffer pointer
			TPtr8 bufferPtr(NULL,0,0);
			OpenFileL(aFs,matchPattern,bufferPtr);
	
			//Now validate the header
			ValidateHeaderL(bufferPtr);
	
			//Process all the rsc entry
			ProcessEntryL(bufferPtr,hiddenRscList);
			}//end for loop
		spiDiscovered+=spiList->Count();
		CleanupStack::PopAndDestroy(2);				//hiddenRscList and spiList
		}//end for loop
	//if no spi files are discovered at all leave with KErrNotFound
	if (spiDiscovered==0)
		User::Leave(KErrNotFound);
	CleanupStack::PopAndDestroy(&downgradeList);
	}

/** Create the next CResourceFile that correponds to the next resource file
in the resource archive(SPI) file
@internalComponent
*/
CResourceFile* CResourceArchiveImpl::NextL(HBufC*& aRscFileName)
	{
	//if end of list return nothing
	if (iCurrentIndex==iRscList.Count())
		return NULL;
	//Construct the resource file name buffer
	TRscEntry currentEntry=iRscList[iCurrentIndex];
	HBufC* rscFileName=HBufC::NewLC(currentEntry.iRscName.Length());
	TPtr modifiable(rscFileName->Des());
	modifiable.Copy(currentEntry.iRscName);
	//Construct the CResourceFile object	
	CResourceFile* resourceFile=CResourceFile::NewL(currentEntry.iRscData);
	CleanupStack::Pop(rscFileName);
	//update the index
	iCurrentIndex++;
	aRscFileName=rscFileName;
	return resourceFile;
	}

/** Reset the archive reader to start reading from the first resource
@internalComponent 
*/
void CResourceArchiveImpl::Reset()
	{
	iCurrentIndex=0;
	}

/** Return the type of the resource archive(SPI) file being read
@internalComponent 
*/
TUid CResourceArchiveImpl::Type()
	{
	return iSpiFileType;
	}

/** Function to look ahead whether next resource exists in the resource archive
@internalComponent
*/
TBool CResourceArchiveImpl::NextResourceExist() const
	{
	return (iCurrentIndex<iRscList.Count());
	}

/** Function used to retrieve a 4 bytes signed integer stored in Little Endian Format
@internalComponent
*/
TInt32 CResourceArchiveImpl::LittleEndianFourByteInteger(const TDesC8& aBuffer,TInt aIndexOfFirstByte) const
	{
	__ASSERT_DEBUG((aIndexOfFirstByte + 3) < aBuffer.Length(), ::Panic(EBafPanicBadIndex));
	return aBuffer[aIndexOfFirstByte] | (aBuffer[aIndexOfFirstByte+1]<<8) | (aBuffer[aIndexOfFirstByte+2]<<16) | (aBuffer[aIndexOfFirstByte+3]<<24);
	}





















