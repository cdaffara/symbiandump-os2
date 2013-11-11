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
// Defines CApaDataRecognizerType the base class for concrete mime recognizers that
// perform the actual mime type recognitionand the CApaDataRecognizer that provides
// the interface to the collection of recognizers.
// 
//


#include "APMSTD.H"
#include "APMREC.H"
#include "APMPAN.H"
#include "APMFNDR.H"

#ifdef USING_ECOM_RECOGS
#include <ecom/ecom.h>
#endif

NONSHARABLE_CLASS(CDataRecognizerExtension) : public CBase
	{
public:
	inline static CDataRecognizerExtension* NewL() {return new(ELeave) CDataRecognizerExtension;}
	inline TUid DestructorUid() const {return iDtorKey;}
	inline TUid& DestructorUidReference() {return iDtorKey;}
	inline CApaDataRecognizer& DataRecognizer() {return *iDataRecognizer;}
	inline void SetDataRecognizer(CApaDataRecognizer& aDataRecognizer) {iDataRecognizer=&aDataRecognizer;}
private:
	inline CDataRecognizerExtension() {}
private:
	CApaDataRecognizer* iDataRecognizer; // no ownership
	TUid iDtorKey; // destructor key to track the instance of ECOM implementation class
	};

class TDataToRecognize
	{
public:
	TDataToRecognize(const TDesC& aName, const TDesC8& aBuffer);
	TDataToRecognize(CApaDataRecognizer& aDataRecognizer, RFile& aFile, TInt aPreferredBufSize);
	void RecognizeL(CApaDataRecognizerType& aDataRecognizerType);
	void PrepareForRecognizeLoopLC();
	void PopOffCleanupStackAndResetAfterRecognizeLoopL();
private:
	enum
		{
		EFlagFilePositionStored	=0x00000001,
		EFlagLeaveIfError		=0x00000002
		};
private:
	static void ResetAfterRecognizeLoopL(TAny* aThis);
	void ResetAfterRecognizeLoopL();
private:
	TUint iFlags;
	const TDesC* iName; // owned if iFile!=NULL
	const TDesC8* iBuffer; // owned if iFile!=NULL
	CApaDataRecognizer* iDataRecognizer;
	RFile* iFile;
	TInt iFilePosition;
	TInt iPreferredBufSize;
	};

//
// class CApaDataRecognizer
//

EXPORT_C TDataRecognitionResult CApaDataRecognizer::RecognizeL(const TDesC& aName, const TDesC8& aBuffer)
	{
	TDataToRecognize dataToRecognize(aName, aBuffer);
	return RecognizeL(dataToRecognize);
	}

EXPORT_C TDataRecognitionResult CApaDataRecognizer::RecognizeL(RFile& aFile, TInt aPreferredBufSize)
	{
	TDataToRecognize dataToRecognize(*this, aFile, aPreferredBufSize);
	return RecognizeL(dataToRecognize);
	}

TDataRecognitionResult CApaDataRecognizer::RecognizeL(TDataToRecognize& aDataToRecognize)
// attempt to recognize using all recognizers
// keeps recognizing until either a recognizer's confidence meets or 
// exceeds the currently acceptable confidence, or all recognizers 
// have been used
	{
	iResult.Reset();
	aDataToRecognize.PrepareForRecognizeLoopLC();
	TInt storedErr=KErrNone;
	TInt count=iDataRecognizerList.Count();
	for (TInt i=0; i<count; i++)
		{
		if (DoRecognize(iDataRecognizerList[i], aDataToRecognize, TDataType(), storedErr))
			{
			break;
			}
		}
	aDataToRecognize.PopOffCleanupStackAndResetAfterRecognizeLoopL();
	// if we haven't recognized anything and there was a problem with a recognizer, propagate
	// the error.  n.b. if several recognizers had errors, only propagate the first error we 
	// encountered
	if (iResult.iConfidence==CApaDataRecognizerType::ENotRecognized)
		{
		User::LeaveIfError(storedErr);
		}
	return iResult;
	}

EXPORT_C TBool CApaDataRecognizer::RecognizeL(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType)
	{
	TDataToRecognize dataToRecognize(aName, aBuffer);
	return RecognizeL(dataToRecognize, aDataType);
	}

EXPORT_C TBool CApaDataRecognizer::RecognizeL(RFile& aFile, TInt aPreferredBufSize, const TDataType& aDataType)
	{
	TDataToRecognize dataToRecognize(*this, aFile, aPreferredBufSize);
	return RecognizeL(dataToRecognize, aDataType);
	}

TBool CApaDataRecognizer::RecognizeL(TDataToRecognize& aDataToRecognize, const TDataType& aDataType)
// recognize using only recognizers which supply the aDataType
	{
	iResult.Reset();
	TInt count=iDataRecognizerList.Count();
	TInt storedErr=KErrNone;
	aDataToRecognize.PrepareForRecognizeLoopLC();
	for (TInt i=0; i<count; i++)
		{
		CApaDataRecognizerType* rec=iDataRecognizerList[i];
		TInt countMime=rec->MimeTypesCount();
		for (TInt ii=0; ii<countMime; ii++)
			{
			TDataType supportedDataType;
			TRAPD(err, supportedDataType=rec->SupportedDataTypeL(ii));
			if (err == KErrNone)
				{
				if (supportedDataType == aDataType)
					{
					if (DoRecognize(rec, aDataToRecognize, aDataType, storedErr))
						{
						aDataToRecognize.PopOffCleanupStackAndResetAfterRecognizeLoopL();
						return ETrue;
						}
					break;
					}
				}
			else if (storedErr==KErrNone)
				{
				// this is the first time we've hit an error with SupportedDataType,
				// so store it now and propagate later if we don't find a suitable 
				// recognizer
				storedErr=err;
				}
			}
		}
	aDataToRecognize.PopOffCleanupStackAndResetAfterRecognizeLoopL();
	if (iResult.iConfidence==CApaDataRecognizerType::ENotRecognized)
		{
		// a suitable recognizer wasn't found, but we may have encountered an
		// error with one of the recognizers we scanned n.b. only the first error
		// encountered is propagated
		User::LeaveIfError(storedErr);
		}
	return iResult.iConfidence!=CApaDataRecognizerType::ENotRecognized;
	}

TBool CApaDataRecognizer::DoRecognize(CApaDataRecognizerType* aDataRecognizerType, TDataToRecognize& aDataToRecognize, const TDataType& aDataType, TInt& aError)
// do the recognition, if the recognizer is sufficiently confident, return ETrue
	{
	TDataRecognitionResult result;
	TRAPD(err, result=aDataRecognizerType->RecognizeL(aDataToRecognize));
	if (err != KErrNone)
		{
		// We only want to store the first error, it if is not KErrNone, it means
		// we have set this error previously
		if (aError==KErrNone)
			{
			aError=err;
			}
		return EFalse;
		}
 
	if (aDataType!=result.iDataType && aDataType!=TDataType())
		{
		return EFalse;
		}
	if (result.iConfidence>iResult.iConfidence)
		{
		iResult=result;
		return (result.iConfidence>=iAcceptedConfidence);
		}
	return EFalse;
	}

EXPORT_C TInt CApaDataRecognizer::AcceptedConfidence() const
	{
	return iAcceptedConfidence;
	}

EXPORT_C void CApaDataRecognizer::SetAcceptedConfidence(TInt aConfidence)
	{
	iAcceptedConfidence=aConfidence;
	}

EXPORT_C CApaDataRecognizer::~CApaDataRecognizer()
	{
	iDataRecognizerList.ResetAndDestroy();
	iDataArray.Reset();
	}

EXPORT_C CApaDataRecognizer::CApaDataRecognizer(RFs& aFs)
	:iFs(aFs),
	iMaxBufferSize(-1),
	iDataRecognizerList(KDataArrayGranularity),
	iDataArray(KDataArrayGranularity),
	iAcceptedConfidence(CApaDataRecognizerType::ECertain)
	{}

EXPORT_C void CApaDataRecognizer::AddDataRecognizerTypeL(CApaDataRecognizerType* aDataRecognizerType)
// add the concrete recognizer into the list at the appropriate priority
	{
#if defined(_DEBUG)
 	CApaDataRecognizerType* rec=NULL;
	const TInt countDeb=iDataRecognizerList.Count();
	for (TInt ii=0; ii<countDeb; ii++)
		{
		rec=iDataRecognizerList[ii];
 		__ASSERT_ALWAYS(rec->TypeUid()!=aDataRecognizerType->TypeUid(), Panic(EDPanicDuplicateRecognizer));
		}
#endif // _DEBUG

	TInt priority=aDataRecognizerType->Priority();
	TInt count=iDataRecognizerList.Count();
	for (TInt i=0; i<count; i++)
		{
		if (iDataRecognizerList[i]->Priority()<priority)
			{
			iDataRecognizerList.InsertL(aDataRecognizerType, i);
			aDataRecognizerType->DataRecognizerExtension()->SetDataRecognizer(*this);
			return;
			}
		}
	iDataRecognizerList.AppendL(aDataRecognizerType);
	aDataRecognizerType->DataRecognizerExtension()->SetDataRecognizer(*this);
	}

EXPORT_C void CApaDataRecognizer::CApaDataRecognizer_Reserved_1()
	{}

EXPORT_C TInt CApaDataRecognizer::RemoveDataRecognizerType(const CApaDataRecognizerType* aDataRecognizerType)
// remove the concrete recognizer from the list
// if it is not locked, if it fails return an error code
	{
	// find the recognizer in the list
	TInt count=iDataRecognizerList.Count();
	TUid uid=aDataRecognizerType->TypeUid();
	TInt i;
	for (i=0; i<count; i++)
		{
		if (iDataRecognizerList[i]->TypeUid()==uid)
			break;
		}
	// did we find a match
	if (i==count)
		return KErrNotFound;
	// is the matching recognizer locked
	CApaDataRecognizerType* rec=iDataRecognizerList[i];
	if (rec->Locked())
		return KErrLocked;
	// remove the recognizer from the list, then delete it
	delete rec;
	iDataRecognizerList.Remove(i);
	iDataRecognizerList.Compress();
	return KErrNone;
	}

EXPORT_C void CApaDataRecognizer::DataTypeL(CDataTypeArray& aArray)
	{
	TInt count=iDataArray.Count();
	for (TInt i=0; i<count; i++)
		aArray.AppendL(iDataArray[i]);
	}

EXPORT_C void CApaDataRecognizer::UpdateDataTypesL()
// rebuild the list of unique mime types
	{
	iDataArray.Reset();
	TInt count=iDataRecognizerList.Count();
	for (TInt i=0; i<count; i++)
		{
		CApaDataRecognizerType* rec=iDataRecognizerList[i];
		TRAP_IGNORE(rec->UpdateDataTypesL());
		TInt mimeCount=rec->MimeTypesCount();
		for (TInt ii=0; ii<mimeCount; ii++)
			{
			TRAP_IGNORE(AddDataTypeL(rec->SupportedDataTypeL(ii)));
			}
		}
	}

EXPORT_C TInt CApaDataRecognizer::PreferredBufSize() const
// return the maximum preferred buf size
	{
	if (iMaxBufferSize < 0)
		{
		// The recognizers have been (re)loaded, so calulate the maximum buffer size.
		TInt count=iDataRecognizerList.Count();
		for (TInt i=0; i<count; i++)
			iMaxBufferSize=Max((TInt)iDataRecognizerList[i]->PreferredBufSize(), iMaxBufferSize);
		}
	return iMaxBufferSize;
	}

EXPORT_C void CApaDataRecognizer::DestroyRecognizerList()
	{
	iDataRecognizerList.ResetAndDestroy();
	iDataArray.Reset();
	}

void CApaDataRecognizer::AddDataTypeL(const TDataType& aDataType)
// add a mime type to the array if it's unique
	{
	TInt i=iDataArray.Count()-1;
	for (; i>=0; i--)
		{
		if (aDataType==iDataArray[i])
			return;
		}
	iDataArray.AppendL(aDataType);
	}

//
// class CApaDataRecognizerType
//

EXPORT_C CApaDataRecognizerType::CApaDataRecognizerType(TUid aUid, TInt aPriority)
	:iTypeUid(aUid),
	iPriority(aPriority),iDataRecognizerExtn(NULL)
/** Constructs the recognizer with a UID and a priority.

Typically, a derived class constructor calls this constructor through a constructor 
initialization list.

The UID is the way that a recognizer is detected by the framework.

@param aUid A UID that identifies the recognizer.
@param aPriority A value that estimates the probability that the recognizer 
will successfully identify data. This is one of the CApaDataRecognizerType::TRecognizerPriority 
enumerators.
@see CApaDataRecognizerType::TRecognizerPriority */
	{}

EXPORT_C CApaDataRecognizerType::~CApaDataRecognizerType()
// Destructor.
	{
#ifdef USING_ECOM_RECOGS
	//if ecom plugin is used destroy its implementation
	if(iDataRecognizerExtn!=NULL)
		{
		REComSession::DestroyedImplementation(iDataRecognizerExtn->DestructorUid());
		delete iDataRecognizerExtn;
		}
#endif
	}

EXPORT_C void CApaDataRecognizerType::Lock()
/** Adds a lock to the recognizer.

This may be called any number of times, but each call to this function must 
be matched by a corresponding call to Unlock() to completely unlock the recognizer.

This function is used to prevent the recognizer DLL from being unloaded.

@see Unlock()
@see Locked() */
	{
	iLock++;
	}

EXPORT_C void CApaDataRecognizerType::Unlock()
/** Removes a lock from the recognizer.

All calls to Lock() should be matched by a corresponding call to this function. 
The recognizer is not unlocked until all calls to Lock() have been matched 
by corresponding calls to this function.

@see Lock()
@see Locked() */
	{
	if (iLock>0)
		iLock--;
	}

EXPORT_C TUint CApaDataRecognizerType::PreferredBufSize()
/** Gets the size of buffer preferred for the purpose of recognizing the data type.

Regardless of the preferred buffer size returned by an implementation of this 
function, the actual size used is never greater than a maximum value as set 
by the client of the Application Architecture server through a call to RApaLsSession::SetMaxDataBufSize().

@return The preferred data size. The default implementation returns zero.
@see RApaLsSession::SetMaxDataBufSize() */
	{
	return 0;	// default to no buffer
				// name recognition only
	}

EXPORT_C void CApaDataRecognizerType::UpdateDataTypesL()
/** Refreshes the list of data (MIME) types supported by this recognizer. */
	{
	}

EXPORT_C void CApaDataRecognizerType::DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer)
/** Implements the attempt to recognize data.

Recognizers should provide an implementation of this function in a derived 
class. Note that, when the implementation recognizes data, it must put the 
result of the operation in the iDataType and iConfidence data members.

Note that if more data is required than is provided in aBuffer, then 
CApaDataRecognizerType::FilePassedByHandleL should be called and the data 
read from the returned RFile (if not NULL). If this returns NULL, it may be 
possible to retrieve the data by calling RFile::Open() on aName, but only if 
aName is a legal file-name. It may be something else, such as a URL.

The default implementation does not recognize data.

@param aName The name of the data; typically this is a file name containing 
the data to be recognized. It is not necessarily a legal file-name though. It 
may, for example, be a URL/URI.
@param aBuffer A buffer containing data to be recognized; typically, this is 
read from the start of the file containing the data.
@see RecognizeL()
@see FilePassedByHandleL()
*/
	{
	(void)aName;
	(void)aBuffer;
	iDataType=TDataType();
	}

EXPORT_C RFile* CApaDataRecognizerType::FilePassedByHandleL()
/** Returns the RFile (if any) of file to be recognized.

This function returns the file passed by handle from the client-side (i.e. from calls to the RFile-parameter overloads of RApaLsSession's RecognizeData, RecognizeSpecificData, AppForDocument and StartDocument). The function returns NULL if the file to be recognized was not passed by handle.

It may only be called from implementations of DoRecognizeL - indeed the purpose of this function is logically to provide an extra parameter to the virtual DoRecognizeL function. All references/pointers to the 
RFile object returned must be discarded when the implementation of DoRecognizeL returns.

The RFile returned (if any) may be used by implementations of DoRecognizeL to retrieve more data than is provided in DoRecognizeL's aBuffer parameter.

The current-position of the returned RFile is the start of the file.

@return The file, passed by handle, to be recognized. Returns NULL if the data to be recognized was passed from the client-side by name/buffer rather than by file-handle. Ownership of the returned object is NOT transferred to the caller.
@see DoRecognizeL()
*/
	{
	RFile* const filePassedByHandle=iDataRecognizerExtn->DataRecognizer().FilePassedByHandle();
	if (filePassedByHandle!=NULL && (filePassedByHandle->SubSessionHandle()!=KNullHandle))
		{
		// set the current file-position to the start of the file
		TInt filePosition=0;
		User::LeaveIfError(filePassedByHandle->Seek(ESeekStart, filePosition));
		}
	return filePassedByHandle;
	}

EXPORT_C TDataRecognitionResult CApaDataRecognizerType::RecognizeL(const TDesC& aName, const TDesC8& aBuffer)
/** Attempts to recognize data.

This function is called by the Application Architecture server as a result 
of client calls to the server through an instance of RApaLsSession.

The function calls DoRecognizeL() which implements recognition behaviour.

@param aName The name of the data; typically this is a file name containing 
the data to be recognized.
@param aBuffer A buffer containing data to be recognized; typically, this is 
read from the start of the file containing the data. Implement PreferredBufSize() 
to define the ideal size for this buffer. Note that failure to implement PreferredBufSize() 
results in a default buffer size of zero.
@return The result of the attempt to recognize the data.
@see PreferredBufSize()
@see RApaLsSession
@see RApaLsSession::SetMaxDataBufSize() */
	{
	TDataToRecognize dataToRecognize(aName, aBuffer);
	return RecognizeL(dataToRecognize);
	}

TDataRecognitionResult CApaDataRecognizerType::RecognizeL(TDataToRecognize& aDataToRecognize)
/**
@internalComponent
*/
	{
	iDataType=TDataType();
	iConfidence=ENotRecognized;
	aDataToRecognize.RecognizeL(*this);
	TDataRecognitionResult rec;
	rec.iDataType=iDataType;
	rec.iConfidence=iConfidence;
	return rec;
	}

EXPORT_C TDataType CApaDataRecognizerType::MimeType()
/** Gets the data (MIME) type of the most recently recognized data.

@return The data (MIME) type.
@see iDataType */
	{
	return iDataType;
	}

EXPORT_C void CApaDataRecognizerType::Reserved_1()
// a spare empty virtual function
	{}

// instantiate the ecom implementation class 
EXPORT_C CApaDataRecognizerType* CApaDataRecognizerType::CreateDataRecognizerL(TUid aImplUid)
	{
#if !defined(USING_ECOM_RECOGS)
	(void)aImplUid;
	return NULL;
#endif
	CDataRecognizerExtension* const dataRecognizerExtn=CDataRecognizerExtension::NewL();
	CleanupStack::PushL(dataRecognizerExtn);

	CApaDataRecognizerType* const dataRecType=static_cast <CApaDataRecognizerType*>(REComSession::CreateImplementationL(aImplUid, dataRecognizerExtn->DestructorUidReference()));

	dataRecType->iDataRecognizerExtn = dataRecognizerExtn;
	CleanupStack::Pop(dataRecognizerExtn);

	return dataRecType;
	}

CDataRecognizerExtension* CApaDataRecognizerType::DataRecognizerExtension()
/** @internalComponent */
	{
	return iDataRecognizerExtn;
	}

//
// class TDataRecognitionResult
//

EXPORT_C void TDataRecognitionResult::Reset()
/** Resets the data type to the default data type and sets the confidence rating 
to CApaDataRecognizerType::ENotRecognized.

@see CApaDataRecognizerType::TRecognitionConfidence
@see TDataType */
	{
	iDataType=TDataType();
	iConfidence=CApaDataRecognizerType::ENotRecognized;
	}

//
// class TDataToRecognize
//

TDataToRecognize::TDataToRecognize(const TDesC& aName, const TDesC8& aBuffer)
	:iFlags(0),
	 iName(&aName),
	 iBuffer(&aBuffer),
	 iDataRecognizer(NULL),
	 iFile(NULL),
	 iFilePosition(0),
	 iPreferredBufSize(0)
	{
	__ASSERT_DEBUG(aName.Length()>0 || aBuffer.Length()>0, Panic(EDPanicInvalidData));
	}

TDataToRecognize::TDataToRecognize(CApaDataRecognizer& aDataRecognizer, RFile& aFile, TInt aPreferredBufSize)
	:iFlags(0),
	 iName(NULL),
	 iBuffer(NULL),
	 iDataRecognizer(&aDataRecognizer),
	 iFile(&aFile),
	 iFilePosition(0),
	 iPreferredBufSize(aPreferredBufSize)
	{
	}

void TDataToRecognize::RecognizeL(CApaDataRecognizerType& aDataRecognizerType)
	{
	__ASSERT_DEBUG(iName!=NULL, Panic(EDPanicNullPointer1));
	__ASSERT_DEBUG(iBuffer!=NULL, Panic(EDPanicNullPointer2));
	aDataRecognizerType.DoRecognizeL(*iName, *iBuffer);
	}

void TDataToRecognize::PrepareForRecognizeLoopLC()
	{
	CleanupStack::PushL(TCleanupItem(ResetAfterRecognizeLoopL, this));
	if (iFile!=NULL)
		{
		// set the file-pointer returned by CApaDataRecognizerType::FilePassedByHandleL
		iDataRecognizer->SetFilePassedByHandle(iFile);

		// store current-position in iFilePosition
		__ASSERT_DEBUG((iFlags&EFlagFilePositionStored)==0, Panic(EDPanicBadFlagState1));
		iFlags|=EFlagFilePositionStored;
		iFilePosition=0;
		User::LeaveIfError(iFile->Seek(ESeekCurrent, iFilePosition));

		// read a buffer of the appropriate length from the start of the file
		__ASSERT_DEBUG(iBuffer==NULL, Panic(EDPanicNullPointerExpected1));
		TInt filePosition=0;
		User::LeaveIfError(iFile->Seek(ESeekStart, filePosition));
		HBufC8* const buffer=HBufC8::NewL(iPreferredBufSize);
		iBuffer=buffer; // iBuffer now "owns" this (as long as iFile!=NULL, which is true)
		TPtr8 buffer_asWritable(buffer->Des());
		User::LeaveIfError(iFile->Read(buffer_asWritable));

		// prepend "::" to the file-name to be passed to DoRecognizeL to make it an illegal file-name - this
		// is to prevent any behavioural break since implementers of DoRecognizeL may assume that if aName is
		// *not* an illegal file-name they can pass it to RFile::Open, which will not be possible if we're
		// recognizing by file-handle, both because of data-caging (the file may not live in a directory that
		// the Apparc process has permission to open it from) and because aName does not contain the full path
		__ASSERT_DEBUG(iName==NULL, Panic(EDPanicNullPointerExpected2));
		TDes* const name=new(ELeave) TFileName;
		iName=name; // iName now "owns" this (as long as iFile!=NULL, which is true)
		User::LeaveIfError(iFile->Name(*name));
		_LIT(KLitIllegalifier, "::");
		name->Insert(0, KLitIllegalifier);
		}
	}

void TDataToRecognize::PopOffCleanupStackAndResetAfterRecognizeLoopL()
	{
	iFlags|=EFlagLeaveIfError; // we don't want to leave from ResetAfterRecognizeLoopL if we're being cleaned up from the clean-up stack because of a leave, but if we're leaving because of a CleanupStack::PopAndDestroy then it's okay to leave
	CleanupStack::PopAndDestroy(this); // calls ResetAfterRecognizeLoopL
	}

void TDataToRecognize::ResetAfterRecognizeLoopL(TAny* aThis)
	{ // static
	STATIC_CAST(TDataToRecognize*, aThis)->ResetAfterRecognizeLoopL();
	}

void TDataToRecognize::ResetAfterRecognizeLoopL()
	{
	TInt error=KErrNone;
	if (iFile!=NULL)
		{
		// NULLify the file-pointer returned by CApaDataRecognizerType::FilePassedByHandleL
		iDataRecognizer->SetFilePassedByHandle(NULL);

		// reset the current-position of the file
		__ASSERT_DEBUG(iFlags&EFlagFilePositionStored, Panic(EDPanicBadFlagState2));
		error=iFile->Seek(ESeekStart, iFilePosition);
		iFlags&=~EFlagFilePositionStored;

		// delete the objects that are owned if iFile!=NULL (which is true)
		delete iName;
		delete iBuffer;
		}
	if (iFlags&EFlagLeaveIfError)
		{
		iFlags&=~EFlagLeaveIfError; // probably not necessary, just defensive programming
		User::LeaveIfError(error);
		}
	}

