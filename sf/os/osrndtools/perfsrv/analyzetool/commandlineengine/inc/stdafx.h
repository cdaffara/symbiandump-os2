/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Include file for standard system include files,
*                or project specific include files that are used frequently, but
*                are changed infrequently.
*  Version     : %version: 1 % 
*
*/


#pragma once

// MemoryLeak checking function.
// Note! Do not comming in svn with this flag enabled.
//#define MEM_LEAK_CHECK

// Addr2line binary used.
//#define ADDR2LINE

#ifdef MEM_LEAK_CHECK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <tchar.h>
#include <fstream>   //ifstream,ofstream
#include <windows.h>
#include <string>    //string
#include <vector>    //string
#include <sstream>   //stringstream
#include <iostream>  // cout
#include <map> // map
#include <time.h>