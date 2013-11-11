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
// Name: filetransfer.h


#ifndef __TESTFILETRANSFER_H__
#define __TESTFILETRANSFER_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <e32base.h>

//The return values of the child process filetransfer.exe to parent process is mapped to an enum
enum TChildreturnvals
    {
    EFailNone,			//No error returned by the child process
    EInvalargc,			//argc parameter is not set
    EFailfdopen,		//Failure in fdopen()
    EFailfirstlseek,	//Failure in first lseek() called
    EFailfread,			//Failure in fread()
    EFailstrcmp,		//The buffer contents of written and read are not same
    EFailfirstfwrite,	//Failure in first fwrite()
    EFailsecondlseek,	//Failure in lseek() when called for the second time
    EFailsecondfwrite,	//Failure in fwrite() when called for the second time
    };

#endif
