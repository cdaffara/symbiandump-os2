
// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _COMMON_DEF_H_
#define	_COMMON_DEF_H_

#ifdef __SYMBIAN32__
#define _fcntl 	fcntl
#define _open 	open
#define _fstat  	fstat
#define _select select    

#define _write(fd, buf, len) 	write(fd, buf, len)
#define _read(fd, buf, len) 	read(fd, buf, len)
#define _close(fd)			close(fd)


#endif



#endif //_COMMON_DEF_H_

