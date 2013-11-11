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

inline TInt CApaDataRecognizerType::MimeTypesCount() const
/** Gets the number of data (MIME) types supported by this recognizer.

@return The number of data types.
@see iCountDataTypes */
	{
	return iCountDataTypes;
	}

inline TUid CApaDataRecognizerType::TypeUid() const
/** Gets the UID that identifies the recognizer.

@return The UID. */
	{
	return iTypeUid;
	}

inline TInt CApaDataRecognizerType::Priority() const
/** Gets the recognizer's priority.

The priority defines the likelihood that the recognizer will successfully 
identify data. This is one of the CApaDataRecognizerType::TRecognizerPriority 
enumerators.

@return The priority value.
@see CApaDataRecognizerType::TRecognizerPriority */
	{
	return iPriority;
	}
	
inline TInt CApaDataRecognizerType::Confidence() const
/** Gets the recognizer's confidence rating for the most recently recognized data.

@return The confidence rating. This is one of the TRecognitionConfidence enumeration 
values.
@see CApaDataRecognizerType::TRecognitionConfidence
@see iConfidence */
	{
	return iConfidence;
	}

inline TInt CApaDataRecognizerType::Locked() const
/** Tests whether the recognizer is locked.

@return If zero, the recognizer is not locked; if greater than 0, the recognizer 
is locked.
@see Lock()
@see Unlock() */
	{
	return iLock;
	}

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

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