/*
 * � Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
 *
 * Copyright (c) 1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Copyright (c) 1999 
 * Boris Fomitchev
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted 
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */ 
// WARNING: This is an internal header file, included by other C++
// standard library headers.  You should not attempt to use this header
// file directly.


#ifndef _STLP_INTERNAL_LOCALE_H
#define _STLP_INTERNAL_LOCALE_H

#ifndef _STLP_CSTDLIB
# include <cstdlib>
#endif

#ifndef _STLP_CWCHAR_H
# include <stl/_cwchar.h>
#endif

#ifndef _STLP_INTERNAL_THREADS_H
# include <stl/_threads.h>
#endif

#ifndef _STLP_STRING_FWD_H
# include <stl/_string_fwd.h>
#endif

_STLP_BEGIN_NAMESPACE

class _STLP_CLASS_DECLSPEC _Locale_impl;             // Forward declaration of opaque type.
class _STLP_CLASS_DECLSPEC _Locale;             // Forward declaration of opaque type.
class _STLP_CLASS_DECLSPEC locale;
class _STLP_CLASS_DECLSPEC ios_base;


template <class _CharT, class _Traits, class _Alloc>
bool 
__locale_do_operator_call (const locale* __that, 
                           const basic_string<_CharT, _Traits, _Alloc >& __x,
                           const basic_string<_CharT, _Traits, _Alloc >& __y);

#  define _BaseFacet locale::facet

class _STLP_CLASS_DECLSPEC locale {
public:
  // types:

  class 
#if defined (__SYMBIAN32__) || defined(__GCCE__)
  _STLP_CLASS_DECLSPEC  
#else
    _STLP_DECLSPEC 
#endif
  facet : private _Refcount_Base {
  protected:
    explicit facet(size_t __no_del = 0) : _Refcount_Base(1), _M_delete(__no_del == 0) {}

    _STLP_DECLSPEC virtual ~facet();
    friend class locale;
    friend class _Locale_impl;
    friend class _Locale;
    
  private:                        // Invalidate assignment and copying.
    facet(const facet& __f) : _Refcount_Base(1), _M_delete(__f._M_delete == 0)  {};       
    void operator=(const facet&); 
    
  private:                        // Data members.
    const bool _M_delete;
  };
  
#if defined(__MVS__) || defined(__OS400__)
  struct
#else
  class
#endif
  _STLP_DECLSPEC id {
    friend class locale;
    friend class _Locale_impl;
  public:
    size_t _M_index;
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
   static _STLP_STATIC_MEMBER_DECLSPEC size_t& get_locale_id_S_max();
# else
    static size_t _S_max;
# endif
  };

  typedef int category;
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  enum _Category {
# else
  static const category
# endif
    none      = 0x000,
    collate   = 0x010,
    ctype     = 0x020,
    monetary  = 0x040,
    numeric   = 0x100,
    time      = 0x200,
    messages  = 0x400,
    all       = collate | ctype | monetary | numeric | time | messages
# if defined (_STLP_STATIC_CONST_INIT_BUG)
  }
# endif
  ;

  // construct/copy/destroy:
  _STLP_DECLSPEC locale();
  _STLP_DECLSPEC locale(const locale&) _STLP_NOTHROW;
  _STLP_DECLSPEC explicit locale(const char *);
  locale(const locale&, const char*, category);

  // those are for internal use
  _STLP_DECLSPEC locale(_Locale_impl*);
  _STLP_DECLSPEC locale(_Locale_impl*, bool);

public:

# if defined ( _STLP_MEMBER_TEMPLATES ) /* && defined (_STLP_FUNCTION_TMPL_PARTIAL_ORDER) */
  template <class _Facet> 
  locale(const locale& __loc, _Facet* __f) : _M_impl(0)
    {
      //      _M_impl = this->_S_copy_impl(__loc._M_impl, __f != 0);
      new(this) locale(__loc._M_impl, __f != 0);
      if (__f != 0)
        this->_M_insert((facet*)__f, _Facet::GetFacetLocaleId());
    }
# endif

  locale(const locale&, const locale&, category);
  _STLP_DECLSPEC ~locale() _STLP_NOTHROW;
  _STLP_DECLSPEC const locale& operator=(const locale&) _STLP_NOTHROW;

# if !(defined (_STLP_NO_MEMBER_TEMPLATES) || defined (_STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS))
  template <class _Facet> locale combine(const locale& __loc) const{
    locale __result(__loc._M_impl, true);
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
    if (facet* __f = __loc._M_get_facet(_Facet::GetFacetLocaleId())) {
      __result._M_insert((facet*)__f, _Facet::GetFacetLocaleId());
#else
    if (facet* __f = __loc._M_get_facet(_Facet::id)) {
      __result._M_insert((facet*)__f, _Facet::id);
#endif
      __f->_M_incr();
    }
    else
      _M_throw_runtime_error();    
    return __result;
  }
# endif
  // locale operations:
  _STLP_DECLSPEC string name() const;

  _STLP_DECLSPEC bool operator==(const locale&) const;
  _STLP_DECLSPEC bool operator!=(const locale&) const;

# if ! defined ( _STLP_MEMBER_TEMPLATES ) || defined (_STLP_INLINE_MEMBER_TEMPLATES) || (defined(__MWERKS__) && __MWERKS__ <= 0x2301)
  bool operator()(const string& __x, const string& __y) const;
#  ifndef _STLP_NO_WCHAR_T
  bool operator()(const wstring& __x, const wstring& __y) const;
#  endif
# else
  template <class _CharT, class _Traits, class _Alloc>
  bool operator()(const basic_string<_CharT, _Traits, _Alloc>& __x,
                  const basic_string<_CharT, _Traits, _Alloc>& __y) const  {
    return __locale_do_operator_call(this, __x, __y);
  }              
# endif

  // global locale objects:
  _STLP_DECLSPEC static locale _STLP_CALL global(const locale&);
  _STLP_DECLSPEC static const locale& _STLP_CALL classic();

public:                         // Helper functions for locale globals.
  _STLP_DECLSPEC facet* _M_get_facet(const id&) const;
  // same, but throws
  _STLP_DECLSPEC facet* _M_use_facet(const id&) const;
  _STLP_DECLSPEC static void _STLP_CALL _M_throw_runtime_error(const char* = 0);
  static void _STLP_CALL _S_initialize();
  static void _STLP_CALL _S_uninitialize();

private:                        // More helper functions.
  //  static _Locale_impl* _STLP_CALL _S_copy_impl(_Locale_impl*, bool);
  _STLP_DECLSPEC void _M_insert(facet* __f, id& __id);

  // friends:
  friend class _Locale_impl;
  friend class _Locale;
  friend class ios_base;

private:                        // Data members
  _Locale_impl* _M_impl;
};

//----------------------------------------------------------------------
// locale globals

# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
template <class _Facet>
inline const _Facet& 
_Use_facet<_Facet>::operator *() const
# else
template <class _Facet> inline const _Facet& use_facet(const locale& __loc)
# endif
{
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  return *__STATIC_CAST(const _Facet*,__loc._M_use_facet(_Facet::GetFacetLocaleId()));
#else
  return *__STATIC_CAST(const _Facet*,__loc._M_use_facet(_Facet::id));
#endif
}

 
# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
template <class _Facet> 
struct has_facet {
  const locale& __loc;
  has_facet(const locale& __p_loc) : __loc(__p_loc) {}
  operator bool() const _STLP_NOTHROW
# else
template <class _Facet> inline bool has_facet(const locale& __loc) _STLP_NOTHROW 
# endif
{
#if defined(__LIBSTD_CPP_SYMBIAN32_WSD__) || defined(_STLP_LIBSTD_CPP_NO_STATIC_VAR_)
  return (__loc._M_get_facet(_Facet::GetFacetLocaleId()) != 0);
#else
  return (__loc._M_get_facet(_Facet::id) != 0);
#endif
}

# ifdef _STLP_NO_EXPLICIT_FUNCTION_TMPL_ARGS
  // close class definition
};
# endif

_STLP_END_NAMESPACE

#endif /* _STLP_INTERNAL_LOCALE_H */

// Local Variables:
// mode:C++
// End:

