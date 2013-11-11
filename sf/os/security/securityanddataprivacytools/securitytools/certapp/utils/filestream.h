#ifndef __FILESTREAM_H__
#define __FILESTREAM_H__/*
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

class FileWriteStream : public RWriteStream
	{
public:
	FileWriteStream(const std::string &aFileName);
	~FileWriteStream();
	};

class FileReadStream : public RReadStream
	{
public:
	FileReadStream(const std::string &aFileName, bool aSpecialTextHandling = false);
	~FileReadStream();
	};

#endif
