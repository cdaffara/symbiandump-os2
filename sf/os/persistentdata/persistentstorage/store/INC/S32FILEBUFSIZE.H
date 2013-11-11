// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef S32FILEBUFSIZE_H
#define S32FILEBUFSIZE_H

#ifndef DEFAULT_FILE_BUF_SIZE
    #define DEFAULT_FILE_BUF_SIZE 16384

#else
	#if DEFAULT_FILE_BUF_SIZE < 1536
		#error "DEFAULT_FILE_BUF_SIZE macro value can't be less than 1536"
	#endif
#endif

/** 
The default size of the file buffer. 
@internalComponent
*/
const TInt KDefaultFileBufSize=DEFAULT_FILE_BUF_SIZE;

#endif
