// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides a Symbian version of the main program and Tcl_AppInit
// procedure for Tcl applications (without Tk).
// 
//

#ifndef _TCLSYMBIANGLOBALS
#define _TCLSYMBIANGLOBALS

#include "tcl.h"
#include "tclInt.h"

#ifdef __WINSCW__ 

//Macro to replace the variable with our new method
#define tmpFileName (*(tempFileNameType)get_gFileName(0))
#define fifoFileName (*(tempFileNameType)get_gFileName(1))
#define inFileName (*(tempFileNameType)get_gFileName(2))
#define outFileName (*(tempFileNameType)get_gFileName(3))
#define errFileName (*(tempFileNameType)get_gFileName(4))
#define inFileName1 (*(tempFileNameType)get_gFileName(5))
#define outFileName1 (*(tempFileNameType)get_gFileName(6))
#define errFileName1 (*(tempFileNameType)get_gFileName(7))

#define getdataKey(x) (*(Tcl_ThreadDataKey*)get_dataKey(x))
/*  dataKey[KMaxDataKey]
 *  tclEvent.c[0]
 *  tclAsync.c[1]
 *  tclFileName.c[2]
 *  tclIO.c[3]
 *  tclIOUtil.c[4]
 *  tclNotify.c[5]
 *  tclRegexp.c[6]
 *  tclTimer.c[7]
 *  tclUnixNotify.c[8]
*/

#define tclExecutableName (*get_tclExecutableName())
#define tclNativeExecutableName (*get_tclNativeExecutableName())
#define inFinalize (*(int *)get_inFinalize())
#define subsystemsInitialized (*(int *)get_subsystemsInitialized())
#define defaultEncoding (*(Tcl_Encoding*)get_defaultEncoding())
#define systemEncoding (*(Tcl_Encoding*)get_systemEncoding())
#define encodingTable (*(Tcl_HashTable*)get_encodingTable())
#define keyRecord (*(SyncObjRecord*)get_keyRecord())
#define typeTable (*(Tcl_HashTable*)get_typeTable())
#define typeTableInitialized (*(int *)get_typeTableInitialized())
#define encodingsInitialized (*(int *)get_encodingsInitialized())
#define tclDefaultEncodingDir (*get_tclDefaultEncodingDir())
#define tclLibraryPathStr (*get_tclLibraryPathStr())
#define opTableInitialized (*(int *)get_opTableInitialized())
#define opHashTable (*(Tcl_HashTable*)get_opHashTable())
#define auxDataTypeTableInitialized (*(int *)get_auxDataTypeTableInitialized())
#define auxDataTypeTable (*(Tcl_HashTable*)get_auxDataTypeTable())
#define glcwdPathPtr (*(Tcl_Obj**)get_cwdPathPtr())
#define glcwdPathEpoch (*(int *)get_cwdPathEpoch())
#define tclPlatform (*(TclPlatformType *)get_tclPlatform())

typedef char (*tempFileNameType)[L_tmpnam + 9];

#else
extern char tmpFileName[L_tmpnam + 9];
extern char fifoFileName[L_tmpnam + 9];
extern char inFileName[L_tmpnam + 9];
extern char outFileName[L_tmpnam + 9];
extern char errFileName[L_tmpnam + 9];
extern char inFileName1[L_tmpnam + 9];
extern char outFileName1[L_tmpnam + 9];
extern char errFileName1[L_tmpnam + 9];	
#endif

#endif /* _TCLSYMBIANGLOBALS */
