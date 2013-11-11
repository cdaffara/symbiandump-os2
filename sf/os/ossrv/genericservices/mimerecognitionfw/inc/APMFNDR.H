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

#if !defined (__APMFNDR_H__)
#define __APMFNDR_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif
#if !defined(__APMREC_H__)
#include <apmrec.h>
#endif

#include <babackup.h>

/**
@internalComponent
*/

// classes defined
class CApaRecognizerDll;

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
/** This is a special priority that overrides any user settings. Applications must
have EWriteDeviceData to use this priority. If they have not then they will be
downgraded to KDataTypePriorityNormal.

@publishedPartner
@released
@see TDataTypeWithPriority */
const TInt32 KDataTypePrioritySystem = 0xFFF9;


class CApaDataRecognizer : public CBase
/**
@internalTechnology
*/
	{
public:
	IMPORT_C TDataRecognitionResult RecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	IMPORT_C TDataRecognitionResult RecognizeL(RFile& aFile, TInt aPreferredBufSize);
	IMPORT_C TBool RecognizeL(const TDesC& aName, const TDesC8& aBuffer, const TDataType& aDataType);
	IMPORT_C TBool RecognizeL(RFile& aFile, TInt aPreferredBufSize, const TDataType& aDataType);
	IMPORT_C ~CApaDataRecognizer();
	IMPORT_C TInt PreferredBufSize() const;
	IMPORT_C void DataTypeL(CDataTypeArray& aArray);
	IMPORT_C void UpdateDataTypesL();
	IMPORT_C TInt AcceptedConfidence() const;
	IMPORT_C void SetAcceptedConfidence(TInt aConfidence);
	inline void SetFilePassedByHandle(RFile* aFilePassedByHandle) {iFilePassedByHandle=aFilePassedByHandle;}
	inline RFile* FilePassedByHandle() {return iFilePassedByHandle;}
protected:
	IMPORT_C CApaDataRecognizer(RFs& aFs);
	IMPORT_C void AddDataRecognizerTypeL(CApaDataRecognizerType* aDataRecognizerType);
	inline void AddDataRecognizerType(CApaDataRecognizerType* aDataRecognizerType);
	IMPORT_C TInt RemoveDataRecognizerType(const CApaDataRecognizerType* aDataRecognizerType);
	IMPORT_C void DestroyRecognizerList();
private:
	void AddDataTypeL(const TDataType& aDataType);
	TDataRecognitionResult RecognizeL(TDataToRecognize& aDataToRecognize);
	TBool RecognizeL(TDataToRecognize& aDataToRecognize, const TDataType& aDataType);
	TBool DoRecognize(CApaDataRecognizerType* aDataRecognizerType, TDataToRecognize& aDataToRecognize, const TDataType& aDataType, TInt& aError);
protected:
	RFs& iFs;
	// iMaxBufferSize is mutable to allow modification by PreferredBufSize() which is const.
	mutable TInt iMaxBufferSize; 
private:
	IMPORT_C virtual void CApaDataRecognizer_Reserved_1(); // shouldn't be necessary as the class is @internalTechnology
private:
	RPointerArray<CApaDataRecognizerType> iDataRecognizerList;
	RArray<TDataType> iDataArray;
	TInt iAcceptedConfidence;
	TDataRecognitionResult iResult;
	RFile* iFilePassedByHandle;
	};

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class CApaScanningDataRecognizer : public CApaDataRecognizer
/**
@publishedPartner 
@released 
*/
	{
public:
	class TRecognizer
		{
	friend class CApaRecognizerDll;
	friend class CT_MimeStep;
	public:
		TRecognizer(HBufC* aName);
		inline const TDesC& Name() const;
	public:
		TUid iUid;
		TInt iDrive;
		TTime iModified;
	private:
		inline TRecognizer();
		TRecognizer(const TRecognizer&);
		TRecognizer operator=(const TRecognizer&);
	private:
		HBufC* iName;
		};
	typedef CArrayFixFlat<TRecognizer> CRecognizerArray;

private:
	class CApaBackupOperationObserver : public CBase, public MBackupOperationObserver
		{
	public:
		CApaBackupOperationObserver(CApaScanningDataRecognizer& aRecognizer);
		~CApaBackupOperationObserver();
		void RegisterObserverL();

		virtual void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);
		TInt UpdateCounter() const;
		void SetUpdateCounter( TInt aValue );
	private:
		CBaBackupSessionWrapper* iSession;
		CApaScanningDataRecognizer& iRecognizer;
		TInt iUpdateCounter;
		};
	friend class CApaScanningDataRecognizer::CApaBackupOperationObserver;

	class CApaScanningDataRecognizerExtra : public CBase
		{
	private:
		CApaBackupOperationObserver* iObserver;
		TBool iRecognizersLoaded;
		friend class CApaScanningDataRecognizer;
		friend class CApaBackupOperationObserver;
		};

	//
public:
	IMPORT_C static CApaScanningDataRecognizer* NewL(RFs& aFs);
	IMPORT_C static CApaScanningDataRecognizer* NewL(RFs& aFs, TBool aLoadDataRecognizers);
	IMPORT_C ~CApaScanningDataRecognizer();
	IMPORT_C void ScanForRecognizersL();
	IMPORT_C TInt RecognizerCount();
	IMPORT_C CRecognizerArray* RecognizerListLC()const;
	IMPORT_C const TRecognizer& operator[](TInt aIndex)const;
	IMPORT_C TInt UpdateCounter()const;
	IMPORT_C void LoadRecognizersL();
	IMPORT_C void UnloadRecognizers();

protected:
	IMPORT_C CApaScanningDataRecognizer(RFs& aFs);
	IMPORT_C void ConstructL();
private:
	void ConstructL(TBool aLoadDataRecognizers);
	void ScanDriveL(TInt aDriveNum);
	void LoadRecognizerL(const TDesC& aFullName,TUid aUid, const TTime& aTime);
	TInt RemoveRecognizer(CApaRecognizerDll* aDll);
	void LoadEcomRecognizerL(TDriveUnit& aDrive,TUid aUid);
	void ScanForEcomRecognizersL();
public:
	IMPORT_C void SetEcomRecognizersFromListL(const CRecognizerArray& aList);
	IMPORT_C void SetEcomRecognizerL(const TRecognizer& aRecognizer); 
private:
	friend class CApaBackupObserver;
	RPointerArray<CApaRecognizerDll> iRecognizerLib;
	CApaScanningDataRecognizerExtra* iExtra;
	};

//gmahe. This is not moved. Because it is not possible to forward declare 
//the nested class used here - CApaScanningDataRecognizer::TRecognizer.

class CApaRecognizerDll : public CBase
/**
@internalTechnology
*/
	{
public:
	CApaRecognizerDll(HBufC* aName);
	~CApaRecognizerDll(); // closes lib and deletes next
public:
	enum TPresence { ENotPresent, EIsPresent, ESuperseded };
public:
	RLibrary iLibrary;
	CApaRecognizerDll* iNext;
	CApaScanningDataRecognizer::TRecognizer iId; // cached so that this data can be accessed from other threads (RLibrary cant be used in this case)
	CApaDataRecognizerType* iRec; // not ownership
	TPresence iPresent;
	TInt iVersion;
	};

//
// inlines
//

inline const TDesC& CApaScanningDataRecognizer::TRecognizer::Name() const
	{
	return *iName;
	}

inline CApaScanningDataRecognizer::TRecognizer::TRecognizer()
	{
	}

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
// Deprecated - Use CApaDataRecognizer::AddDataRecognizerTypeL instead 
// as adding a concrete recognizer into the list could leave.
/**
@deprecated 7.0s
*/
inline void CApaDataRecognizer::AddDataRecognizerType(CApaDataRecognizerType* aDataRecognizerType)
	{
	AddDataRecognizerTypeL(aDataRecognizerType);
	}
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#endif
