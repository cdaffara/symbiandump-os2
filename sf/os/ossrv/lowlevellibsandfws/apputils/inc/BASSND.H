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

#if !defined(__EIKSSND_H__)
#define __EIKSSND_H__

#include <e32base.h>
#include <s32file.h>
#include <mmf/common/mmfbase.h>


/**
Specifies a system sound for a ring, alarm, message, error or event.
"bafl.lib"
@since 6.0
@publishedAll
@released
*/
typedef TUid TBaSystemSoundUid;

/**
Specifies a system sound file name.
"bafl.lib"
@since 6.0
@publishedAll
@released
*/
typedef TFileName TBaSystemSoundName;

/**
@publishedAll
@released
*/
#define KSystemSoundDefaultVolume	1
#define KSystemSoundDefaultPriority	0

/**
this dll may use uids 48aa to 48ca
@publishedAll
@released
*/
const TBaSystemSoundUid KSystemSoundRingUID		= {0x100048AB};
const TBaSystemSoundUid KSystemSoundAlarmUID	= {0x100048AC};
const TBaSystemSoundUid KSystemSoundMessageUID	= {0x100048AD};

const TBaSystemSoundUid KUidSystemSoundError={0x1000609E};
const TBaSystemSoundUid KUidSystemSoundEvent={0x1000609F};



class TBaSystemSoundType
/** 
Provides a logical sound wrapper. A sound is specified logically using two 
UIDs. The first UID specifies the sound's category which can be a file, tone 
or sequence; the second, which is optional, specifies an instance of that 
category. The pair of UIDs is encapsulated in a TBaSystemSoundType object.

Once a TBaSystemSoundType object is constructed, the sound player class CoeSoundPlayer 
is used to play the sound. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBaSystemSoundType();
	IMPORT_C TBaSystemSoundType(TBaSystemSoundUid aMajor,TUid aMinor=KNullUid);
public:
	inline TBool IsNull() const;
	IMPORT_C TBool operator==(const TBaSystemSoundType& aType) const;
public:
	/** Specifies a sound's category. */
	TBaSystemSoundUid iMajor;
	/** Specifies an instance of a sound's category. */
	TUid iMinor;
	};


class TBaSystemSoundInfo
/** 
Provides functions that contain the information for a system sound. A sound 
may be a file, a tone or a fixed sequence.

The characteristics of a physical sound that corresponds to a logical sound 
are encapsulated in a TBaSystemSoundInfo object. You can get and set the physical 
sound for a logical sound through the functions provided by the BaSystemSound 
class. 
@publishedAll
@released
*/
	{
public:
	
	class TTone
/** 
Provides functions to define a tone. Once defined, the tone can be encapsulated 
using the TBaSystemSoundInfo class and subsequently played. 
@publishedAll
@released 

*/
		{
	public:
	
		inline TTone();
		inline TTone(TInt aFrequency,TTimeIntervalMicroSeconds32 aDuration);
	public:
	
		TBool IsNull() const;
	public:

		void InternalizeL(RReadStream& aStream);

		void ExternalizeL(RWriteStream& aStream) const;
	public:
	/** Frequency of the tone. */
		TInt iFrequency;
	/** Duration of the tone. */
		TTimeIntervalMicroSeconds32 iDuration;
		};
public:
	/** Specifies the category of the system sound. */
	enum TSoundCategory
		{
		/** Specifies no system sound. */
		ENull,

		/** Specifies a file as the system sound. */
		EFile,

		/** Specifies a sequence as the system sound. */
		ESequence,

		/** Specifies a tone as the system sound. */
		ETone
		};
public:
	IMPORT_C TBaSystemSoundInfo();
	IMPORT_C TBaSystemSoundInfo(const TBaSystemSoundType& aType,const TBaSystemSoundName& aName,
							TInt aVolume=KSystemSoundDefaultVolume,TInt aPriority=KSystemSoundDefaultPriority);
	IMPORT_C TBaSystemSoundInfo(const TBaSystemSoundType& aType,TInt aFixedSequence,
							TInt aVolume=KSystemSoundDefaultVolume,TInt aPriority=KSystemSoundDefaultPriority);
	IMPORT_C TBaSystemSoundInfo(const TBaSystemSoundType& aType,TTone aTone,
							TInt aVolume=KSystemSoundDefaultVolume,TInt aPriority=KSystemSoundDefaultPriority);
public:
	IMPORT_C TSoundCategory SoundCategory() const;
	inline TInt FixedSequenceNumber() const;
	inline TBaSystemSoundName FileName() const;
	inline TTone Tone() const;
	IMPORT_C void SetFixedSequenceNumber(TInt aNumber);
	IMPORT_C void SetFileName(const TBaSystemSoundName& aFileName);
	IMPORT_C void SetTone(const TTone& aTone);

	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
public:
	TBaSystemSoundType iType;
	TInt iVolume;
	TInt iPriority;
private:
	TBaSystemSoundName iName;
	TInt iFixedSequence;
	TTone iTone;
	};


/** 
API class to retrieve and modify the system sounds settings.

Note that SystemSoundFile() is now deprecated and will be removed in a future
release. The filename it returns is no longer valid but is kept for backwards 
compatibility. The system sounds settings are no longer stored in a data file.
 
@see BaSystemSound::SystemSoundFile
@see KSystemSoundRepositoryUID
@publishedAll
@released
*/
class BaSystemSound
	{
public:
	IMPORT_C static TInt GetSound(RFs& aFsSession,const TBaSystemSoundType& aType,TBaSystemSoundInfo& aInfo);
	IMPORT_C static void SetSoundL(RFs& aFsSession,const TBaSystemSoundInfo& aInfo);
    IMPORT_C static TFileName SystemSoundFile(); // Deprecated
private:
	TBaSystemSoundName static DefaultSound(TBaSystemSoundUid aSSUid);
	};


class CBaSystemSoundArray: public CBase
/**
 * An array to contain the set of system sounds for a TBaSystemSoundUid
@publishedAll
@released
 */	
 	{
public:
	CBaSystemSoundArray();
	~CBaSystemSoundArray();
public:
	IMPORT_C static CBaSystemSoundArray* NewL();
	IMPORT_C static CBaSystemSoundArray* NewLC();
	IMPORT_C void RestoreL(RFs& aFsSession,TBaSystemSoundUid aSSUid);
	IMPORT_C TInt Count() ;
	IMPORT_C TBaSystemSoundInfo At(TInt aIndex);

private:
	TBaSystemSoundUid iUid;
	CArrayFixFlat<TBaSystemSoundInfo> iSystemSounds;
	};


class TBaSoundPriorityBase
/**
Utility class to bundle up priority, priority preference and device specific data
Only intended to be used in conjunction with TBaSystemSoundInfo::iPriority

Layout of class is
bits 31 -> 16:	Device specific priority preference data
bits 15 -> 8:	Standard Epoc TMdaPriorityPreference setting
bits 7 -> 0:		Priority value stored as a TInt8.  Maximum range possible is -256 -> +255
					Note that MediaSvr currently defines a narrower range than this
@publishedAll
@released
*/	
	{
public:
	inline TInt Int() const;
	inline TInt Priority() const;
	IMPORT_C TMdaPriorityPreference PriorityPreference() const;
protected:
	inline TBaSoundPriorityBase();
	void Set(TInt aPriority,TMdaPriorityPreference aPriorityPreference=EMdaPriorityPreferenceTimeAndQuality);
protected:
	TInt iPriority;
	};


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bassnduid.h>
#endif

class TBaSoundPriorityEncoder : public TBaSoundPriorityBase
/**
Encode TBaSystemSoundInfo::iPriority before persisting a sound preference
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBaSoundPriorityEncoder(TInt aPriority,TMdaPriorityPreference aPriorityPreference=EMdaPriorityPreferenceTimeAndQuality);
	};



inline TBool TBaSystemSoundType::IsNull() const
	/** Tests whether the sound's category and instance are null.
	
	@return ETrue if the sound's category and instance are null, EFalse otherwise. */
	{return iMajor==KNullUid && iMinor==KNullUid;}


inline TBaSystemSoundInfo::TTone::TTone()
	: iFrequency(0), iDuration(0)
	/** Default constructor. Sets up a TTone object with null frequency and duration. */
	{}
inline TBaSystemSoundInfo::TTone::TTone(TInt aFrequency,TTimeIntervalMicroSeconds32 aDuration)
	: iFrequency(aFrequency), iDuration(aDuration)
	/** Constructor which sets up a TTone object with specified frequency and duration.
	
	@param aFrequency The tone's frequency. 
	@param aDuration The tone's duration. */
	{}

inline TInt TBaSystemSoundInfo::FixedSequenceNumber() const
	/** Gets the current sound's sequence.
	
	@return The current sound's sequence. */
	{return iFixedSequence;}

inline TBaSystemSoundName TBaSystemSoundInfo::FileName() const
	/** Gets the current sound's file.
	
	@return The current sound's file. */
	{return iName;}

inline TBaSystemSoundInfo::TTone TBaSystemSoundInfo::Tone() const
	/** Gets the current system sound's tone.
	
	@return The current sound's tone. */
	{return iTone;}

inline TBaSoundPriorityBase::TBaSoundPriorityBase()
	{}
inline TInt TBaSoundPriorityBase::Int() const
	{return iPriority;}
inline TInt TBaSoundPriorityBase::Priority() const
	{return (TInt8)iPriority;}


#endif
