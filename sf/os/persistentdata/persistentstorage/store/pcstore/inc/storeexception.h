// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__STOREEXCEPTION_H__)
#define __STOREEXCEPTION_H__

namespace PCStore
{
/**
@internalAll

Class for the exceptions which will be thrown by the PCStore library.
*/
class TStoreException
	{
public:
	//Enumerations for the store open mode
	enum EStoreError
		{
		/**
		library wide error code -1: fail to open file.
		*/
		EFileOpenError = -1,

		/**
		library wide error code -2: fail to read file.
		*/
		EFileReadError = -2,

		/**
		library wide error code -3: fail to write file.
		*/
		EFileWriteError = -3,

		/**
		library wide error code -4: fail to seek file.
		*/
		EFileSeekError = -4,

		/**
		library wide error code -5: attemp to create stream while another stream exists.
		*/
		EStreamExclusionError = -5,

		/**
		library wide error code -6: the file is not the supported store type.
		*/
		EStoreNotSupported = -6,

		/**
		library wide error code -7: operation on a mismatching type of store.
		*/
		EStoreModeError = -7,

		/**
		library wide error code -8: the store file is corrupted.
		*/
		EStoreCorrupt = -8,

		/**
		library wide error code -9: the maximum length of the descriptor overfloates
		*/
		EDesOverfloat = -9
		};
	TStoreException(EStoreError aCode);

public:
	EStoreError iErrorCode;
	};
}
#endif // !defined(__STOREEXCEPTION_H__)
