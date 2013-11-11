// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __OP_NEW1_H__
#define __OP_NEW1_H__

#ifdef __WINSCW__   //for armv5, use the local operator new to avoid linking problem
#include <e32cmn.h>
#endif


#ifdef __SYMBIAN_STDCPP_SUPPORT__

namespace std{
	struct nothrow_t{};

	extern const nothrow_t nothrow;
	class bad_alloc{};

} //end of namespace std


IMPORT_C void* operator new    (unsigned int) throw(std::bad_alloc);
IMPORT_C void  operator delete (void*)       throw();

IMPORT_C void* operator new    (unsigned int, const std::nothrow_t&) throw();
IMPORT_C void  operator delete (void*, const std::nothrow_t&)       throw();

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void* operator new(unsigned int, void* p) throw()
	{
	return p;
	}

inline void operator delete(void*, void*) throw()
	{
	}
#endif //__PLACEMENT_NEW_INLINE

// Array form.

IMPORT_C void* operator new[]    (unsigned int) throw(std::bad_alloc);
IMPORT_C void  operator delete[] (void*)       throw();

IMPORT_C void* operator new[]    (unsigned int, const std::nothrow_t&) throw();
IMPORT_C void  operator delete[] (void*, const std::nothrow_t&)       throw();


#ifndef __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_VEC_NEW_INLINE

inline void* operator new[](unsigned int, void* p) throw()
	{
	return p;
	}

inline void operator delete[](void*, void*) throw()
	{
	}
#endif //__PLACEMENT_VEC_NEW_INLINE

#endif //__SYMBIAN_STDCPP_SUPPORT__
#endif //__OP_NEW1_H__
