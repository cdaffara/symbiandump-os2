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


#ifndef _STLP_NEW_H_HEADER
# define _STLP_NEW_H_HEADER

# ifdef _STLP_NO_BAD_ALLOC
# ifndef _STLP_NEW_DONT_THROW
#   define _STLP_NEW_DONT_THROW 1
# endif /* _STLP_NEW_DONT_THROW */

#  include <exception>


_STLP_BEGIN_NAMESPACE

#if defined(__SYMBIAN32__) && ( defined( __WINSCW__) || defined(__GCCXML__)) 

// already defined symcpp.h included from rvct2_2.h
struct nothrow_t {};
#endif



# ifdef _STLP_OWN_IOSTREAMS
#ifdef __ARMCC__
extern _STLP_DECLSPEC const nothrow_t nothrow;
#else
extern IMPORT_C const nothrow_t& GetNoThrowObj();
#define nothrow GetNoThrowObj()
#endif
# else
#  define nothrow nothrow_t()
# endif
#ifndef _STLP_EXCEPTION_BASE
#  define _STLP_EXCEPTION_BASE exception
#endif

class bad_alloc : public _STLP_EXCEPTION_BASE { 
public:
  bad_alloc () _STLP_NOTHROW_INHERENTLY { }
  bad_alloc(const bad_alloc&) _STLP_NOTHROW_INHERENTLY { }
  bad_alloc& operator=(const bad_alloc&) _STLP_NOTHROW_INHERENTLY {return *this;}
  ~bad_alloc () _STLP_NOTHROW_INHERENTLY { }
  const char* what() const _STLP_NOTHROW_INHERENTLY { return "bad alloc"; }
};

_STLP_END_NAMESPACE

#endif /* _STLP_NO_BAD_ALLOC */

#if defined (_STLP_WINCE)
_STLP_BEGIN_NAMESPACE

inline void* _STLP_CALL __stl_new(size_t __n) {
  return ::malloc(__n);
}

inline void _STLP_CALL __stl_delete(void* __p) {
  free(__p);
}

#ifndef __cdecl
# define __cdecl
#endif

_STLP_END_NAMESPACE

#else /* _STLP_WINCE */

#include <new>

# ifndef _STLP_NO_BAD_ALLOC
#  ifdef _STLP_USE_OWN_NAMESPACE

    _STLP_BEGIN_NAMESPACE
    using _STLP_VENDOR_EXCEPT_STD::bad_alloc;
    using _STLP_VENDOR_EXCEPT_STD::nothrow_t;
    using _STLP_VENDOR_EXCEPT_STD::nothrow;

#  if defined (_STLP_GLOBAL_NEW_HANDLER)
    using ::new_handler;
    using ::set_new_handler;
#  else
    using _STLP_VENDOR_EXCEPT_STD::new_handler;
    using _STLP_VENDOR_EXCEPT_STD::set_new_handler;
#  endif
    
    _STLP_END_NAMESPACE

#  endif /* _STLP_OWN_NAMESPACE */

# endif /* _STLP_NO_BAD_ALLOC */

# if defined (_STLP_NO_NEW_NEW_HEADER) || defined (_STLP_NEW_DONT_THROW) || defined (__SYMBIAN32__) \
                    || defined (__WINS__) && ! defined (_STLP_CHECK_NULL_ALLOC)
#  define _STLP_CHECK_NULL_ALLOC(__x) void* __y = __x;if (__y == 0){_STLP_THROW(bad_alloc());}return __y
# if defined (__SYMBIAN32__)
//# define _STLP_NEW operator new
#define _STLP_NEW  ::malloc
#endif
/*
# elif defined (__SYMBIAN32__) || defined (__WINS__)
#  ifndef _STLP_USE_TRAP_LEAVE
#   define _STLP_CHECK_NULL_ALLOC(__x) void* __y = __x;if (__y == 0){abort();}return __y
#  else
#   define _STLP_NEW(x) :: operator new (x, ELeave) 
#   define _STLP_CHECK_NULL_ALLOC(__x) return __x
#  endif
*/
# else
#  define _STLP_CHECK_NULL_ALLOC(__x) return __x
# endif

#ifndef _STLP_NEW
# define _STLP_NEW ::operator new
#endif
# define _STLP_PLACEMENT_NEW ::new

_STLP_BEGIN_NAMESPACE

#ifdef __SYMBIAN32__

typedef void(*new_handler)();

_STLP_DECLSPEC new_handler set_new_handler(new_handler pnew)  __NO_THROW;

#endif

#if (( defined(__IBMCPP__)|| defined(__OS400__) || defined (__xlC__) || defined (qTidyHeap)) && defined(__DEBUG_ALLOC__) )
inline void*  _STLP_CALL __stl_new(size_t __n) {  _STLP_CHECK_NULL_ALLOC(_STLP_NEW(__n, __FILE__, __LINE__)); }
inline void _STLP_CALL __stl_delete(void* __p) { ::operator delete(__p, __FILE__, __LINE__); }
#else
inline void*  _STLP_CALL __stl_new(size_t __n)   { return ::operator new(__n); }
inline void   _STLP_CALL __stl_delete(void* __p) { ::operator delete(__p); }
#endif
_STLP_END_NAMESPACE


# endif /* _STLP_WINCE */

#if defined(__SYMBIAN32__) && !defined(__GCCE__)
IMPORT_C void *operator new(unsigned int aSize);

IMPORT_C void *operator new[](unsigned int aSize);
#endif

IMPORT_C void operator delete(void* aPtr) __NO_THROW;

IMPORT_C void operator delete[](void* aPtr) __NO_THROW;

IMPORT_C void* operator new(unsigned int aSize, const std::nothrow_t& /*aNoThrow*/)  __NO_THROW;

IMPORT_C void* operator new[](unsigned int aSize, const std::nothrow_t& aNoThrow)  __NO_THROW;

IMPORT_C void operator delete(void* aPtr, const std::nothrow_t& /*aNoThrow*/)  __NO_THROW;

IMPORT_C void operator delete[](void* aPtr, const std::nothrow_t& /*aNoThrow*/)  __NO_THROW;


// placement delete
#ifndef __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_VEC_NEW_INLINE
inline void* operator new[](unsigned int /*aSize*/, void* aBase)  __NO_THROW
	{return aBase;}
inline void operator delete[](void* /*aPtr*/, void* /*aBase*/)  __NO_THROW
    {
    
    }
#endif

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void* operator new(unsigned int /*aSize*/, void* aBase)  __NO_THROW
	{return aBase;}

// Global placement operator delete
inline void operator delete(void* /*aPtr*/, void* /*aBase*/)  __NO_THROW
	{}
#endif //__PLACEMENT_NEW_INLINE


#endif /* _STLP_NEW_H_HEADER */
