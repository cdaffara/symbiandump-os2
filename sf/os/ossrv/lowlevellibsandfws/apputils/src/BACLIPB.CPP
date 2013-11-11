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
// Started by DS, October 1996
// Clipboard
// 
//

#include <baclipb.h>
#include <s32file.h>
#include <basched.h>
#include <bautils.h>
#include <s32stor.h>
#include <hal.h>
#include <hal_data.h>
#include <baflpan.h>


#ifdef _UNICODE
const TUid KClipboardFileUid16={0x10003A10};
#define KClipboardFileUid KClipboardFileUid16
#else
const TUid KClipboardFileUid8={268435515};
#define KClipboardFileUid KClipboardFileUid8
#endif

_LIT(KClipboardFileName,"\\System\\Data\\Clpboard.cbd");


inline CClipboard::CClipboard(RFs& aFs)
	: iFs(aFs)
	{__DECLARE_NAME(_S("CClipboard"));}

CClipboard* CClipboard::NewLC(RFs& aFs)
	{
	CClipboard* self=new(ELeave) CClipboard(aFs);
    CleanupStack::PushL(self);
	return self;
	}

void CClipboard::ConstructReadL()
	{
	__ASSERT_DEBUG(iStore==NULL&&iStreamDictionary==NULL,User::Invariant());

//	
	TDriveName clipboardDrive = ClipboardFileDrive();
	TFileName clipboardFile(KClipboardFileName);
	clipboardFile.Insert(0,clipboardDrive);

	CFileStore* store=CDirectFileStore::OpenLC(iFs,clipboardFile,EFileRead|EFileShareReadersOnly);
	if (store->Type()!=TUidType(KDirectFileStoreLayoutUid,KClipboardFileUid))
		User::Leave(KErrCorrupt);
	CStreamDictionary* dict=CStreamDictionary::NewLC();
	RStoreReadStream root;
	root.OpenLC(*store,store->Root());
	root >> *dict;
	CleanupStack::PopAndDestroy(); // root stream
//
	CleanupStack::Pop(2);	// store and dictionary
	iStore=store;
	iStreamDictionary=dict;
	}


EXPORT_C TDriveName CClipboard::ClipboardFileDrive()
/**
 Returns the clipboad file drive information stored in the TLS
 
 @return A drive letter and a colon (A:, B:... etc,.)
 @panic BAFL 23 If drive is either not in range 0-24 or is not defined in HAL file(s).
 If this panic occurs, check the value of EClipboardDrive defined in HAL file(s)
 i,e <values.hda> & <config.hcf>. This value should be in range EDriveA-EDriveY.
 */
    {
	TInt drive;
	TInt retVal;
	TBool badDrive = ETrue;

	retVal = HAL::Get(HAL::EClipboardDrive,drive);	
	
	if(retVal == KErrNone)
		{
		//EDriveZ can not be a valid clipboard drive as it is non-writable
		// Check if drive is between range 0-24,
		if(EDriveA <= drive && EDriveY >= drive)
			{
			badDrive = EFalse; // Drive is valid				
			}
		}
			
	__ASSERT_ALWAYS((badDrive == EFalse), ::Panic(EBafPanicBadDrive));

	TDriveUnit driveUnit(drive);
	return driveUnit.Name();
	}

EXPORT_C CClipboard* CClipboard::NewForReadingLC(RFs& aFs)
/** Constructs a clipboard object and prepares the clipboard's store for reading, 
placing a pointer to the object on the cleanup stack. This allows the object 
and allocated resources to be cleaned up if a subsequent leave occurs.

@param aFs A handle to a file server session. 
@return A pointer to the newly constructed clipboard object. This pointer is 
put onto the cleanup stack. */
    {
	CClipboard* self=NewLC(aFs);
	TRAPD(error,self->ConstructReadL());
	switch (error)
		{
	default:
		User::LeaveIfError(error);
		__ASSERT_DEBUG(self->iStore!=NULL&&self->iStreamDictionary!=NULL,User::Invariant());
		break;
	case KErrPathNotFound:	// clipboard does not exist
	case KErrNotFound:		// no root stream
	case KErrInUse:			// someone is writing to the clipboard
	case KErrCorrupt:		// type is invalid, or cardinality failure
	case KErrEof:			// wrong structure
	case KErrNotSupported:	// not a file store
		// return an empty clipboard: will need an empty dictionary
		__ASSERT_DEBUG(self->iStore==NULL&&self->iStreamDictionary==NULL,User::Invariant());
		self->iStreamDictionary=CStreamDictionary::NewL();
		break;
		}
	return(self);
	}

EXPORT_C CClipboard* CClipboard::NewForReadingL(RFs& aFs)
/** Constructs a clipboard object and prepares the clipboard's store for reading.

@param aFs A handle to a file server session. 
@return A pointer to the newly constructed clipboard object. */
	{
	CClipboard* self=CClipboard::NewForReadingLC(aFs);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CClipboard* CClipboard::NewForWritingLC(RFs& aFs)
/** Constructs a clipboard object and prepares the clipboard's store for writing.

If the file associated with the clipboard's store does not exist, it is created; 
if it already exists, any existing content is discarded.

@param aFs A handle to a file server session. 
@return A pointer to the newly constructed clipboard object. This pointer is 
put onto the cleanup stack. */
    {
	CClipboard* self=NewLC(aFs);
	self->iStreamDictionary=CStreamDictionary::NewL();
//
	TDriveName clipboardDrive = ClipboardFileDrive();
	TFileName clipboardFile(KClipboardFileName);
	clipboardFile.Insert(0,clipboardDrive);

	RFile file;
	TInt r=file.Open(aFs,clipboardFile,EFileRead|EFileWrite);

	/**
	 * The following branching is desired at this stage:
	 * -> if the path is not present, we need to create it and then create the file. 
	 * -> if the path is present, we only need to create the file 
	 * -> if both path and file exists, we don't need to do anything 
	 * 
	 * Instead of the switch case block we can achieve this by atleast 2 other means.
 	 * Option 1:
	 * RFile file; 
	 * TInt r=file.Open(aFs,clipboardFile,EFileRead|EFileWrite); 
	 * if(r == KErrPathNotFound) { 
	 *       BaflUtils::EnsurePathExistsL(aFs,clipboardFile); 
	 *       r=file.Replace(aFs,clipboardFile,EFileRead|EFileWrite); 
	 * } else if(r == KErrNotFound) { 
	 *       r=file.Replace(aFs,clipboardFile,EFileRead|EFileWrite); 
	 * }
	 * User::LeaveIfError(r);
	 *
	 * => the above code has extra check to be made when the error is KErrNotFound 
	 *    and has increased code size owing to repeated file.Replace().
	 *
	 * Option 2:
	 * BaflUtils::EnsurePathExistsL(aFs,clipboardFile); 
	 * RFile file; 
	 * TInt r=file.Open(aFs,clipboardFile,EFileRead|EFileWrite); 
	 * if(r == KErrNotFound) { 
	 *        r=file.Replace(aFs,clipboardFile,EFileRead|EFileWrite); 
	 * } 
	 * User::LeaveIfError(r);
	 * => Call to BaflUtils::EnsurePathExistsL unconditionally which in turn 
	 * invokes Rfs::MkDirAll() can be expensive and errorneous.
	 * (Check documnentation for RFs::MkDirAll()
	 */
 

	//Maintain the order of the switch case and DO NOT add break between the
	//switch cases. They have been written such that only the required set of 
	//functionality is executed based on the Error code.
	switch (r)
		{
	case KErrPathNotFound:		// path has not been created yet
		BaflUtils::EnsurePathExistsL(aFs,clipboardFile);
		// coverity [fallthrough]
		// Missing break intentional. Read comment before switch case.
	case KErrNotFound:			// it does not exist yet
		r=file.Replace(aFs,clipboardFile,EFileRead|EFileWrite);
		// coverity [fallthrough]
		// Missing break intentional. Read comment before switch case.
	default:
		User::LeaveIfError(r);
		}
//
	CFileStore* store=CDirectFileStore::NewL(file);
	self->iStore=store;
	store->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KClipboardFileUid));
	return self;
	}

EXPORT_C TInt CClipboard::Clear(RFs& aFs)
/** Empties the clipboard.

Note that if the file associated with the clipboard's store does not exist, 
this is not regarded as an error and the function completes successfully.

@param aFs A handle to a file server session. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TDriveName clipboardDrive = ClipboardFileDrive();
	TFileName clipboardFile(KClipboardFileName);
	clipboardFile.Insert(0,clipboardDrive);

	RFile file;
	TInt r=file.Open(aFs,clipboardFile,EFileWrite);
	switch (r)
		{
	case KErrNone:			// clear without notification
		r=file.SetSize(0);
		file.Close();
		break;
	case KErrPathNotFound:	// no clipboard to clear
	case KErrNotFound:
		r=KErrNone;
		break;
	default:
		r=aFs.Delete(clipboardFile);	// just delete the fella
		break;
		}
	return r;
	}

EXPORT_C CClipboard::~CClipboard()
/** Destructor. Frees all resources owned by the object, prior to its destruction. 
In particular, the file associated with the clipboard's store is closed.

Note that if the clipboard file store has no root stream, i.e. the CommitL() 
member function of CClipboard has not been called prior to deleting the CClipboard 
object, then the file associated with the clipboard file store is deleted. */
    {
	delete iStreamDictionary;
	CFileStore* store=iStore;
	if (store!=NULL)
		{
		TBool rollback=(store->Root()==KNullStreamId);
   		delete store;
		if (rollback)	// useless clipboard, no root stream
			{
			TDriveName clipboardDrive = ClipboardFileDrive();
			TFileName clipboardFile(KClipboardFileName);
			clipboardFile.Insert(0,clipboardDrive);
			
			iFs.Delete(clipboardFile);
			}
		}
    }

EXPORT_C void CClipboard::CommitL()
/** Commits changes to the clipboard's store.

It externalises the stream dictionary to the clipboard store as the root stream 
and then commits all changes to the store. This function must be called after 
application data has been externalised; failure to do so results in the deletion 
of the file associated with the clipboard store and the loss of data. */
	{
	__ASSERT_DEBUG(iStore!=NULL&&iStreamDictionary!=NULL,User::Invariant());
//
	CFileStore& store=*iStore;
	RStoreWriteStream stream;
	TStreamId root=stream.CreateLC(store);
	stream<< *iStreamDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	store.SetRootL(root);
	store.CommitL();
	}


// Copy and paste member functions for low level data types


const TUid KClipboardRealTypeUid = { 268435705 };


EXPORT_C void  CClipboard::CopyToL(TReal aReal) __SOFTFP
/** Copies a double-precision floating point value to the clipboard.

Note that the function does not automatically commit changes to the clipboard's 
store. This must be done explicitly.

@param aReal The double-precision floating point value to be copied to the 
clipboard.
@see CommitL() */
    {
	__ASSERT_DEBUG(iStore!=NULL&&iStreamDictionary!=NULL,User::Invariant());
//
    RStoreWriteStream stream;
    TStreamId id = stream.CreateLC ( Store() );
    stream << aReal;
	stream.CommitL();
    CleanupStack::PopAndDestroy();
    StreamDictionary().AssignL ( KClipboardRealTypeUid, id );
    }

EXPORT_C TBool CClipboard::PasteFromL  ( TReal& aReal )
/** Pastes a double-precision floating point value from the clipboard.

If a double-precision floating point value exists on the clipboard, then the 
function restores it to the referenced argument and returns a true value.

If there is no double-precision floating point value on the clipboard, then 
the function returns a false value. The referenced argument is not changed.

@param aReal On return, contains the double-precision floating point value 
found on the clipboard.
@return ETrue, if a double-precision floating point value exists on the 
clipboard and has been pasted to the referenced argument; EFalse otherwise. */
    {
	__ASSERT_DEBUG(iStreamDictionary!=NULL,User::Invariant());
    TStreamId id = StreamDictionary().At(KClipboardRealTypeUid);
    if (id == KNullStreamId)
		return (EFalse); // nothing here
    __ASSERT_DEBUG(iStore!=NULL,User::Invariant());
	RStoreReadStream stream;
    stream.OpenLC (Store(), id);
    stream >> aReal;
    CleanupStack::PopAndDestroy();
    return (ETrue);
    }



// end of BACLIPB.CPP
