/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#ifndef MEMSPYDRIVERLOG_H
#define MEMSPYDRIVERLOG_H

// System includes
#include <kernel.h>

// Uncomment this line to enable minimal op code tracing
//#define TRACE_TYPE_OPCODES

// Uncomment this line to enable driver tracing
//#define TRACE_TYPE_GENERAL

// Uncomment this line to dump raw binary data - usually heaps & stacks.
//#define TRACE_TYPE_DATA 

// Uncomment this line to trace heap walking etc
//#define TRACE_TYPE_USERHEAP

// Uncomment this line to trace chunks
//#define TRACE_TYPE_CHUNK

// Uncomment this line to trace event monitor functionality
//#define TRACE_TYPE_EVENTMONITOR

// Uncomment this line to trace kernel heap operations
//#define TRACE_TYPE_KERNELHEAP

// Uncomment this line to trace heap walking
//#define TRACE_TYPE_HEAPWALK

// This block switches on basic tracing in UDEB HW builds
#if (defined(_DEBUG) && !defined(__WINS__))
#   define TRACE_TYPE_OPCODES 
#   define TRACE_TYPE_GENERAL
//#   define TRACE_TYPE_HEAPWALK
#   define TRACE_TYPE_CHUNK
//#   define TRACE_TYPE_EVENTMONITOR
//#   define TRACE_TYPE_DATA
//#   define TRACE_TYPE_USERHEAP
#   define TRACE_TYPE_KERNELHEAP
#endif


#if defined(TRACE_TYPE_OPCODES)
#   define TRACE_OP( x ) x
#else
#   define TRACE_OP( x ) 
#endif



#if defined(TRACE_TYPE_GENERAL)
#   define TRACE( x ) x
#else
#   define TRACE( x ) 
#endif



#if defined(TRACE_TYPE_USERHEAP)
#   define TRACE_HEAP( x ) x
#else
#   define TRACE_HEAP( x ) 
#endif



#if defined(TRACE_TYPE_KERNELHEAP)
#   define TRACE_KH( x ) x
#else
#   define TRACE_KH( x ) 
#endif



#if defined(TRACE_TYPE_HEAPWALK )
#   define TRACE_HEAPWALK( x ) x
#else
#   define TRACE_HEAPWALK( x ) 
#endif



#if defined(TRACE_TYPE_EVENTMONITOR)
#   define TRACE_EM( x ) x
#else
#   define TRACE_EM( x ) 
#endif



#if defined(TRACE_TYPE_DATA)
#   define TRACE_DATA( x ) x
#else
#   define TRACE_DATA( x )
#endif


#if defined(TRACE_TYPE_CHUNK)
#   define TRACE_CHUNK( x ) x
#else
#   define TRACE_CHUNK( x )
#endif

#define LOG(args...) TRACE(Kern::Printf(args))

#endif
