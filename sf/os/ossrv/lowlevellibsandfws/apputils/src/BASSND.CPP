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
//

#include <bassnd.h>
#include <baflpan.h>
#include <basched.h>
#include <bautils.h>
#include "BaSsndStore.h"

const TInt KSystemSoundArrayGranularity	=4;
const TInt KNullFixedSequence=-1;

// Full path and name of system sound file.
_LIT(KSystemSoundFilePath,"_:\\system\\data\\syssnd.dat");
// Buffer descriptor to hold full path and name of system sound file. 
typedef TBuf<28> TSystemSoundFileName; // a multiple of 4

_LIT(KSystemSoundDefaultSound,"z:\\system\\SystemSounds\\ring.wav");
_LIT(KSystemSoundDefaultRing,"z:\\system\\SystemSounds\\ring.wav");
_LIT(KSystemSoundDefaultAlarm,"z:\\system\\SystemSounds\\alarm.wav");
_LIT(KSystemSoundDefaultMessage,"z:\\system\\SystemSounds\\message.wav");

//
// class TBaSystemSoundType
//

EXPORT_C TBaSystemSoundType::TBaSystemSoundType()
	: iMajor(KNullUid), iMinor(KNullUid)
/** Constructs a TBaSystemSoundType object with a null category and instance. */
	{}


EXPORT_C TBaSystemSoundType::TBaSystemSoundType(TBaSystemSoundUid aMajor,TUid aMinor)
	: iMajor(aMajor), iMinor(aMinor)
/** Constructs a TBaSystemSoundType object with the specified category and, optionally, 
instance.

@param aMajor The category, this can be a file, tone or sequence. 
@param aMinor The instance of the category. This is optional. */
	{}


EXPORT_C TBool TBaSystemSoundType::operator==(const TBaSystemSoundType& aType) const
/** Tests whether the sound's category and instance are the same as that specified.

@param aType The category and instance to test.
@return ETrue if the sound's category and instance is the same as aType, EFalse 
otherwise. */
	{
	return (iMajor==aType.iMajor && iMinor==aType.iMinor);
	}

//
// class TBaSystemSoundInfo
//
TBool TBaSystemSoundInfo::TTone::IsNull() const
/** Tests whether the sound's category and instance are null.
@internalAll
@return    ETrue if the sound's category and  instance are null, EFalse 
otherwise. */	
    {
	return iFrequency==0 && iDuration.Int()==0;
	}


void TBaSystemSoundInfo::TTone::InternalizeL(RReadStream& aStream)
/**
@internalAll 
Gets a tone's frequency and duration.

@param aStream The stream from which the tone's frequency and duration is read. 
*/
	{
	iFrequency=aStream.ReadInt32L();
	iDuration=aStream.ReadInt32L();
	}


void TBaSystemSoundInfo::TTone::ExternalizeL(RWriteStream& aStream) const
/**
@internalAll 
Saves a tone's frequency and duration.

@param aStream The stream to which the tone's frequency and duration is 
written.
*/

	{
	aStream.WriteInt32L(iFrequency);
	aStream.WriteInt32L(iDuration.Int());
	}

EXPORT_C TBaSystemSoundInfo::TBaSystemSoundInfo()
	: iType(KNullUid,KNullUid), iVolume(KSystemSoundDefaultVolume), iPriority(KSystemSoundDefaultPriority),
				iName(KNullDesC), iFixedSequence(KNullFixedSequence), iTone(TTone())
/** Constructs a default TBaSystemSoundInfo object. This object has a null type, 
default volume, default priority, null name, null fixed sequence, and tone 
of TTone. */
	{}

EXPORT_C TBaSystemSoundInfo::TBaSystemSoundInfo(const TBaSystemSoundType& aType,const TBaSystemSoundName& aName,
														TInt aVolume,TInt aPriority)
	: iType(aType), iVolume(aVolume), iPriority(aPriority), iName(aName),
				iFixedSequence(KNullFixedSequence), iTone(TTone())
/** Constructs a TBaSystemSoundInfo object with the specified type and file name. 
The sound has default volume and priority.

@param aType Sound type
@param aName Sound name
@param aVolume Sound volume
@param aPriority Sound priority */
	{}

EXPORT_C TBaSystemSoundInfo::TBaSystemSoundInfo(const TBaSystemSoundType& aType,TInt aFixedSequence,
														TInt aVolume,TInt aPriority)
	: iType(aType), iVolume(aVolume), iPriority(aPriority), iName(KNullDesC),
				iFixedSequence(aFixedSequence), iTone(TTone())
/** Constructs a TBaSystemSoundInfo object with the specified type and sequence. 
The sound has default volume and priority.

@param aType Sound type
@param aFixedSequence Sound sequence
@param aVolume Sound volume
@param aPriority Sound priority */
	{}

EXPORT_C TBaSystemSoundInfo::TBaSystemSoundInfo(const TBaSystemSoundType& aType,TTone aTone,
														TInt aVolume,TInt aPriority)
	: iType(aType), iVolume(aVolume), iPriority(aPriority), iName(KNullDesC),
				iFixedSequence(KNullFixedSequence), iTone(aTone)
/** Constructs a TBaSystemSoundInfo object with the specified type and tone. The 
sound has default volume and priority.

@param aType Sound type
@param aTone Sound tone
@param aVolume Sound volume
@param aPriority Sound priority */
	{}


EXPORT_C TBaSystemSoundInfo::TSoundCategory TBaSystemSoundInfo::SoundCategory() const
/** Gets the sound's category.

@return The system sound's category, or NULL if the sound is not a file, sequence, 
or tone. */
	{
	TBaSystemSoundInfo::TSoundCategory cat=TBaSystemSoundInfo::ENull;
	 if (!iTone.IsNull())
		cat=TBaSystemSoundInfo::ETone;
	else if (iName.Length())
		cat=TBaSystemSoundInfo::EFile;
	else if (iFixedSequence!=KNullFixedSequence)
		cat=TBaSystemSoundInfo::ESequence;
	return cat;
	}


EXPORT_C void TBaSystemSoundInfo::SetFixedSequenceNumber(TInt aNumber)
/** Replaces the current sound with the specified sequence number.

@param aNumber The sequence number with which to replace the current sound. */
	{
	iFixedSequence=aNumber;
	iName=KNullDesC();
	iTone=TTone();
	}


EXPORT_C void TBaSystemSoundInfo::SetFileName(const TBaSystemSoundName& aFileName)
/** Replaces the current sound with the specified file.

@param aFileName The file with which to replace the current sound. */
	{
	iFixedSequence=KNullFixedSequence;
	iName=aFileName;
	iTone=TTone();
	}


EXPORT_C void TBaSystemSoundInfo::SetTone(const TTone& aTone)
/** Replaces the current sound with the specified tone.

@param aTone The tone with which to replace the current sound. */
	{
	iFixedSequence=KNullFixedSequence;
	iName=KNullDesC();
	iTone=aTone;
	}
/** Internalises an object of this class from a read stream.
Presence of this function means that the standard templated operator>>()
can be used to internalise objects of this class.

@param aStream The stream from which the object is to be internalised.
@internalAll 
*/
void TBaSystemSoundInfo::InternalizeL(RReadStream& aStream)
	{
	aStream >> iType.iMajor;
	aStream >> iType.iMinor;
	iVolume = aStream.ReadInt32L();
	iPriority = aStream.ReadInt32L();

	TSoundCategory cat = (TSoundCategory)aStream.ReadInt32L();
	switch (cat)
		{
		case EFile:
			{
			TUint16 pathLength = aStream.ReadUint16L();
			if (pathLength > iName.MaxLength())
				{
				User::Leave(KErrCorrupt);
				}
			TUint16* buf = const_cast<TUint16*>(iName.Ptr());
			aStream.ReadL(buf, pathLength);
			iName.SetLength(pathLength);

			iFixedSequence=KNullFixedSequence;
			iTone=TTone();
			}
			break;

		case ESequence:
			iFixedSequence = aStream.ReadInt32L();
			iName=KNullDesC;
			iTone=TTone();
			break;

		case ETone:
			aStream >> iTone;
			iName=KNullDesC;
			iFixedSequence=KNullFixedSequence;
			break;

		default:
			User::Leave(KErrCorrupt);
		}
	}

/** Externalises an object of this class to a write stream.
The presence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class. 

@param aStream Stream to which the object should be externalised.
*/
void TBaSystemSoundInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iType.iMajor;
	aStream << iType.iMinor;
	aStream.WriteInt32L(iVolume);
	aStream.WriteInt32L(iPriority);

	TSoundCategory cat = SoundCategory();
	aStream.WriteInt32L((TInt32)cat);

	switch (cat)
		{
		case TBaSystemSoundInfo::EFile:
			{
			// Do it this way so that in InternalizeL we can check
			// for data corruption.
			TUint16 pathLength = iName.Length();
			aStream.WriteUint16L(pathLength);
			aStream.WriteL(iName.Ptr(), pathLength);
			}
			break;
		case TBaSystemSoundInfo::ESequence:
			aStream.WriteInt32L( iFixedSequence );
			break;
		case TBaSystemSoundInfo::ETone:
			aStream << iTone;
			break;
		default:
			// object not initialized
			User::Leave(KErrCorrupt);
		}
	}

//
// class BaSystemSound
//

EXPORT_C TInt BaSystemSound::GetSound(RFs& /* aFsSession */,const TBaSystemSoundType& aType,TBaSystemSoundInfo& aInfo)
/** Gets sound information from storage.

@param aFsSession unused parameter kept for compatibility reason.
@param aType unique identifier of the sound. aType encapsulates a major
and optionally a minor UID. The major UID specifes the sound category
while the minor UID specifies an instance of the category. 
@param aInfo On return, the sound.
@return KErrNone if successful, or one of the system wide errors if 
unsuccessful. */
	{ // static
	CBaSsndStore* soundStore(NULL);
	TRAPD(err, soundStore = CBaSsndStore::NewL() );

	if (err == KErrNone)
		{
		err = soundStore->GetSound(aType, aInfo);

		if (err == KErrNotFound && aType.iMinor != KNullUid)
			{
			TBaSystemSoundType tmpType(aType.iMajor);
			err = soundStore->GetSound(tmpType, aInfo);
			}

		delete soundStore; // finish with the pointer
		} // CBaSsndStore::NewL is successful

	// err can be from CBaSsndStore::NewL or CBaSsndStore::GetSound
	if (err != KErrNone)
		{
		aInfo.SetFileName( DefaultSound(aType.iMajor) );
		aInfo.iVolume=KSystemSoundDefaultVolume;
		aInfo.iPriority=KSystemSoundDefaultPriority;
		}

	return err;
	}


EXPORT_C void BaSystemSound::SetSoundL(RFs& /* aFsSession */,const TBaSystemSoundInfo& aInfo)
/** Adds the specified sound to the system sound table if it does not 
already exist in the table. If the sound already exists then overwrite
the entry in the sound table.

@param aFsSession unused parameter kept for backward compatibility.
@param aInfo The sound to add to the system sound table. 
@capability WriteDeviceData Note only clients with WriteDeviceData capability can sucessfully 
call this API. 
@leave KErrPermissionDenied caller does not have WriteDeviceData capability.
@leave KErrArgument if the sound has not been initialized.
Other system-wide error codes may also be returned.
*/
	{ // static
	TBaSystemSoundInfo::TSoundCategory cat = aInfo.SoundCategory();
	if (cat == TBaSystemSoundInfo::ENull)
		{
		User::Leave(KErrArgument);
		}

	//check that the name includes a path
#ifdef _DEBUG
	if (cat == TBaSystemSoundInfo::EFile)
		{
		TFileName fullpath = BaflUtils::DriveAndPathFromFullName(
			aInfo.FileName() );
		if (fullpath.Length()==0)
			{
			Panic(EBafPanicSystemSoundNoPath);
			}
		}
#endif
	CBaSsndStore* ssndStore = CBaSsndStore::NewL();
	CleanupStack::PushL(ssndStore);
	ssndStore->SetSoundL(aInfo);
	CleanupStack::PopAndDestroy(ssndStore);
	}

/**
Returns the system sound filename.

This function is now deprecated. System sounds are now stored in Central Repository.
The function still returns the file name, but the file is no longer read from nor written to.
There is no guarantee that the file exists.

Client code which uses the filename for backup and restore
purposes should be removed. Central Repository handles backup and
restore of system sound settings.

Client code that uses the filename for change notification
must use KSystemSoundRepositoryUID to connect to Central Repository and
must register for group notification on system sound repository
changes. For example, if you used code such as:
@code
iFs.Connect() ;
TFileName soundFileName = BaSystemSound ::SystemSoundFile();
iFs.NotifyChange(ENotifyAll, iRequestStatus, soundFileName);
@endcode
you would now use:
@code
const Tuint32 KDoNotCareValue = 0 ;
const Tuint32 KAllMask = 0 ;
iRepository = CRepository ::NewL(KBaSsndRepositoryUid);
iRepository.NotifyRequest(KDoNotCareValue, KAllMask, iRequestStatus);
@endcode

@see CRepository::NotifyRequest()
@deprecated
*/
EXPORT_C TFileName BaSystemSound::SystemSoundFile()
	{ // static
	TSystemSoundFileName filename(KSystemSoundFilePath);
	filename[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	return filename;
	}

TBaSystemSoundName BaSystemSound::DefaultSound(TBaSystemSoundUid aSSUid)
	{ // static
	TBaSystemSoundName name;
	if (aSSUid==KSystemSoundRingUID )
		name=KSystemSoundDefaultRing;
	else if (aSSUid==KSystemSoundAlarmUID )
		name=KSystemSoundDefaultAlarm;
	else if (aSSUid==KSystemSoundMessageUID )
		name=KSystemSoundDefaultMessage;
	else
		name=KSystemSoundDefaultSound;
	return name;
	}

//
// class CBaSystemSoundArray
//

/**
	@internalAll*/ 
CBaSystemSoundArray::CBaSystemSoundArray()
	: iSystemSounds(KSystemSoundArrayGranularity)
	
	{}

/**
	@internalAll*/
CBaSystemSoundArray::~CBaSystemSoundArray()
	{	
	iSystemSounds.Reset();
	}

EXPORT_C CBaSystemSoundArray* CBaSystemSoundArray::NewL()
	{
	CBaSystemSoundArray* self=new(ELeave) CBaSystemSoundArray;
	return self;
	}
EXPORT_C CBaSystemSoundArray* CBaSystemSoundArray::NewLC()
	{
	CBaSystemSoundArray* self=CBaSystemSoundArray::NewL();
	CleanupStack::PushL(self);
	return self;
	}

/** Retrieves all instances of a sound category.
@param aFsSession unused parameter kept for backward compatibility
@param aSSuid specifies the sound category to retrieve.
*/
EXPORT_C void CBaSystemSoundArray::RestoreL(RFs& /* aFsSession */,TBaSystemSoundUid aSSUid)
	{
	iUid=aSSUid;
	iSystemSounds.Reset();

	CBaSsndStore* ssndStore = CBaSsndStore::NewL();
	CleanupStack::PushL(ssndStore);
	User::LeaveIfError( ssndStore->GetSoundCategory(aSSUid, iSystemSounds) );
	CleanupStack::PopAndDestroy(ssndStore);
	}

EXPORT_C TBaSystemSoundInfo CBaSystemSoundArray::At(TInt aIndex)
	{
	return iSystemSounds[aIndex];
	}

EXPORT_C TInt CBaSystemSoundArray::Count()
	{
	return iSystemSounds.Count();
	}

//
// class TBaSoundPriorityBase
//

const TInt KPrefEpocMask		=0x0000FF00;
const TInt KPrefDeviceMask		=0xFFFF0000;

EXPORT_C TMdaPriorityPreference TBaSoundPriorityBase::PriorityPreference() const
	{
	TInt pref=0;
	pref=(iPriority&KPrefEpocMask)>>8;
	pref|=(iPriority&KPrefDeviceMask);
	return static_cast<TMdaPriorityPreference>(pref);
	}

/**
	@internalAll*/ 
void TBaSoundPriorityBase::Set(TInt aPriority,TMdaPriorityPreference aPriorityPreference)
	
{
	__ASSERT_ALWAYS(aPriority>=EMdaPriorityMin && aPriority<=EMdaPriorityMax,Panic(EBafPanicInvalidSoundPriority));
	iPriority=0;
	*(TInt8*)&iPriority=(TInt8)aPriority;
	iPriority|=((aPriorityPreference<<8)&KPrefEpocMask);
	iPriority|=(aPriorityPreference&KPrefDeviceMask);
	}

//
// class TBaSoundPriorityEncoder
//

EXPORT_C TBaSoundPriorityEncoder::TBaSoundPriorityEncoder(TInt aPriority,TMdaPriorityPreference aPriorityPreference)
	{
	Set(aPriority,aPriorityPreference);
	}
