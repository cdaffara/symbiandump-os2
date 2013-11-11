#ifndef __F32FILE_H__
#define __F32FILE_H__/*
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


/**
 @file
 @internalComponent
*/
enum TFileMode
{
	EFileShareExclusive,
	EFileShareReadersOnly,
	EFileShareAny,
	EFileShareReadersOrWriters,

	EFileStream=0,
	EFileStreamText=0x100,

	EFileRead=0,
	EFileWrite=0x200,
	EFileReadAsyncAll=0x400,
	EFileWriteBuffered	=0x00000800,
	EFileWriteDirectIO	=0x00001000,
	EFileReadBuffered	=0x00002000,
	EFileReadDirectIO	=0x00004000,
	EFileReadAheadOn	=0x00008000,
	EFileReadAheadOff	=0x00010000

};

class RFs
	{
public:
	TInt Connect(TInt aSlots=-1) { (void)aSlots; return 0; }
	void Close() {}
	};

#endif
