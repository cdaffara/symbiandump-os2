#ifndef __FILESTREAM_PRIVATE_H__
#define __FILESTREAM_PRIVATE_H__/*
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


#include <s32strm.h>
#include "filestream.h"

/**
 * @file
 * @internalComponent
 */

class FileStreamBuf : public MStreamBuf
	{
public:
	FileStreamBuf(const std::string &aFileName, bool aWritable, bool aSpecialTextHandling = false);
	
	void StripUtf8HeaderIfPresent();

	// Symbian MStreamBuf does not have a virtual destructor and relies on DoRelease virtual...

private:
	virtual TInt DoReadL(TAny *aPtr,TInt aMaxLength);
	virtual void DoWriteL(const TUint8* aPtr,TInt aLength);
	virtual TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
	virtual void DoRelease();
private:
	bool iWritable;
	std::fstream iFile;
	bool iSpecialTextHandling;
	};

#endif
