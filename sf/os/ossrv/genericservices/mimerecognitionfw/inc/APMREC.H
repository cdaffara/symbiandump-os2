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

#if !defined(__APMREC_H__)
#define __APMREC_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__APMSTD_H__)
#include <apmstd.h>
#endif

// classes defined
class CApaDataRecognizerType;
// classes referenced
class RFs;
class RFile;
class TDataToRecognize;
class CDataRecognizerExtension;

/** Creates an interface object for a abstract base class for a recognizer.

@publishedAll
@released
*/
typedef CApaDataRecognizerType* (*CreateCApaDataRecognizerType)(); 

class TDataRecognitionResult
/** The result of an attempt to recognize data.

The class contains a data (MIME) type and a confidence rating.

An object of this type is passed to the Application Architecture server when 
making a request to recognize data.

@publishedAll 
@released 
@see RApaLsSession::RecognizeData() */
	{
public:
	IMPORT_C void Reset();
public:
	/** The data type. */
	TDataType iDataType;
	/** The confidence rating associated with the data (EMIME) type. */
	TInt iConfidence;
	};
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
@internalComponent
*/
const TInt KDataArrayGranularity=5;

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
/**
@publishedAll 
@released 
*/
typedef CArrayFixFlat<TDataType> CDataTypeArray;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

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

class CApaDataRecognizerType : public CBase
/** Abstract base class for a recognizer.

Concrete recognizers need to define and implement a derived class; specifically, 
they need to provide an implementation of the DoRecognizeL(), SupportedDataTypeL() 
and PreferredBufSize() functions.

The class is implemented as part of a DLL. The ordinal 1 function of the DLL 
must construct an instance of, and return a pointer to, a CApaDataRecognizerType 
derived class. 

@publishedAll 
@released */
	{
	friend class TDataToRecognize; // so that it can call the overloads of DoRecognizeL
public:	
	/** A set of values used to indicate the probability that the recognizer will successfully 
	identify data. The Application Architecture framework uses this set to determine 
	the order in which recognizers are invoked.

	@see CApaDataRecognizerType()
	@see Priority() */
	enum TRecognizerPriority
		{
		/** The highest recognizer priority. Recognizers with this priority are invoked 
		before those with any other priority. */
		EHigh=100,
		/** The normal recognizer priority. Recognizers with this priority are invoked 
		after those with a EHigh priority but before those with a ELow priority. */
		ENormal=0,
		/** The lowest recognizer priority. */
		ELow=-100
		};		
	/** A set of values describing the probability that the recognizer assigns to its 
	belief that the most recently sampled data is of the type ascribed to it. */
 	enum TRecognitionConfidence
		{
		/** The data is definitely of the data type. */
		ECertain=KMaxTInt,
		/** A level of confidence between ECertain and EPossible. */
		EProbable=100,
		/** The data is possibly of the data type. */
		EPossible=0,
		/** A level of confidence between EPossible and ENotRecognized. */
		EUnlikely=-100,
		/** The data is not recognized. */
		ENotRecognized=KMinTInt
		};
public:
	inline TInt MimeTypesCount() const;
	inline TUid TypeUid() const;
	inline TInt Priority() const;
	inline TInt Confidence() const;
	inline TInt Locked() const;
	IMPORT_C TDataRecognitionResult RecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	TDataRecognitionResult RecognizeL(TDataToRecognize& aDataToRecognize);
	IMPORT_C TDataType MimeType();
	IMPORT_C void Lock();
	IMPORT_C void Unlock();
	IMPORT_C void UpdateDataTypesL();
	IMPORT_C ~CApaDataRecognizerType();
	IMPORT_C virtual TUint PreferredBufSize();
	/** Gets one of the data (MIME) types that the recognizer can recognize.
	
	@param aIndex An index that identifies the data type. Typically, the minimum 
	value is zero and the maximum value is the value of MimeTypesCount() - 1.
	@return The data (MIME) type. */
	IMPORT_C virtual TDataType SupportedDataTypeL(TInt aIndex) const = 0;
protected:
	IMPORT_C CApaDataRecognizerType(TUid aUid, TInt aPriority);
	IMPORT_C RFile* FilePassedByHandleL();
private: 
	IMPORT_C virtual void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	// Reserved virtual functions...
	IMPORT_C virtual void Reserved_1();
public:
	IMPORT_C static CApaDataRecognizerType* CreateDataRecognizerL(TUid aImplUid);
	CDataRecognizerExtension* DataRecognizerExtension();
protected:
	const TUid iTypeUid;
	const TInt iPriority;
	/** The number of data (MIME) types supported by this recognizer. Typically, this 
	is set during construction of an instance of the derived class.
	
	Users of the derived class use MimeTypesCount() to get this value. */
	TInt iCountDataTypes;
	/** The recognizer's confidence rating of the most recently recognized data.
	
	Users of the derived class use Confidence() to get this value. */
	TInt iConfidence;
	/** The data (MIME) type of the most recently recognized data.
	
	Users of the derived class use MimeType() to get this value. */
	TDataType iDataType;
private:
	TInt iLock;
	CDataRecognizerExtension* iDataRecognizerExtn;
	};

#include <apmrec.inl>

#endif
