// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// A set of utilities available for testing / debugging /
// developing components.
// Some macros and a stress test class
// CMemoryLeakTest implements a utility class to assist in 
// stress testing the memory use of a component
// Also intended for use in combination with the TestBed 
// components only.
// 
//

/**
 @internalComponent
 @file
 @test
 @since 7.0
*/

#ifndef  __TESTUTILITIES_H__
#define  __TESTUTILITIES_H__

#include <e32def.h>
#include <e32std.h>
#include <e32base.h>

// __________________________________________________________________________
// Enforce ANSI compliance upon the Microsoft compiler!
#if defined(__VC32__) && _MSC_VER>=1100
#pragma warning(disable : 4127)		// warning C4127 : conditional expression is constant
#define for if(false);else for		// Enforce the definition of a loop variable to local scope
#endif

// __________________________________________________________________________
// A useful macro for TO DO messages
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)

// Use __FILE__LINE__ as a string containing "File.x(line#)" for example:
//     #pragma message( __FILE__LINE__ "Remove this line after testing.")
// The above example will display the file name and line number in the build window
// in such a way as to allow you to double click on it to go to the line in MSDev 6.0.
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#endif __TESTUTILITIES_H__
