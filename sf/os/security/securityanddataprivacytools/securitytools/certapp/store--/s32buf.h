#ifndef __S32BUF_H__
#define __S32BUF_H__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32base.h>
#include <fstream>

/**
 * @file
 * @internalComponent
 */

enum TStreamLocation 
	/** The seek position is calculated relative to the beginning of the 
	stream.*/
	{EStreamBeginning,
	/** The seek position is calculated relative to the end of the stream.*/
	EStreamMark,
	/** The seek position is calculated relative to the existing read or 
	write mark in the stream. */
	EStreamEnd};

typedef std::streampos TStreamPos;

class MStreamBuf
	{
public:
	// Symbian definition does not have a virtual destructor and relies on DoRelease virtual...
	// We define one to avoid compiler warnings
	virtual ~MStreamBuf() {}

	enum TRead {ERead=0x01};
	enum TWrite {EWrite=0x02};
	typedef TInt TMark;

	TInt ReadL(TAny *aPtr,TInt aMaxLength);
	void WriteL(const TUint8* aPtr,TInt aLength);

	void SeekL(TMark aMark,TStreamPos aPos);
	TStreamPos SeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset=0);
	TStreamPos SeekL(TRead,TStreamLocation aLocation,TInt anOffset=0);
	TStreamPos SeekL(TWrite,TStreamLocation aLocation,TInt anOffset=0);
	TStreamPos SeekL(TRead,TInt anOffset);
	TStreamPos SeekL(TWrite,TInt anOffset);

	TStreamPos TellL(TRead) const;
	TStreamPos TellL(TWrite) const;

	void Close();
	void Release();
	void SynchL();

private:
	virtual TInt DoReadL(TAny *aPtr,TInt aMaxLength) = 0;
	virtual void DoWriteL(const TUint8* aPtr,TInt aLength) = 0;
	virtual IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);

	virtual void DoRelease();
	};
#endif
