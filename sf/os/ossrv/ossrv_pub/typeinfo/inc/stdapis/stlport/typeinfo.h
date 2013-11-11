/*
 * © Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
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

#ifndef _STLP_OLDSTD_typeinfo
# define _STLP_OLDSTD_typeinfo

# ifndef _STLP_OUTERMOST_HEADER_ID
#  define _STLP_OUTERMOST_HEADER_ID 0x874
#  include <stl/_prolog.h>
# endif

# ifndef _STLP_NO_TYPEINFO
#ifdef __SYMBIAN32__
#  include <exception>
_STLP_BEGIN_NAMESPACE
#ifdef __WINSCW__
class type_info
{
	const char* tname;
public:
	bool operator== (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) == 0);}
	bool operator!= (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) != 0);}
	bool before (const type_info& rhs) const {return static_cast<bool>(strcmp(tname, rhs.tname) < 0);}
	const char* name () const {return tname;}
private:
	type_info (const type_info& rhs);               //  not defined
	type_info& operator= (const type_info& rhs);    //  not defined
};
#else
#pragma define_type_info
class type_info 
{
  public:
    virtual ~type_info();
    IMPORT_C bool operator==(const type_info&) const;
    IMPORT_C bool operator!=(const type_info&) const;
    IMPORT_C bool before(const type_info&) const;
    IMPORT_C const char* name() const;
  private:
    static void operator delete(void*) { /* nothing */ }
    type_info& operator=(const type_info&);  // Not actually defined
    type_info(const type_info&);  // Not actually defined
  private:
    const char *__type_name;
  };
#endif

class bad_cast : public exception 
  {
  public:
    bad_cast() throw() { }
	bad_cast(const bad_cast&) throw() {}
	bad_cast& operator=(const bad_cast&) throw() {return *this;}
    virtual ~bad_cast() throw() {}
	virtual const char* what() const throw() {return "bad_cast";}
  };
  
class bad_typeid : public exception 
  {
  public:
    bad_typeid () throw() { }
	bad_typeid(const bad_typeid&) throw() {}
	bad_typeid& operator=(const bad_typeid&) throw() {return *this;}
    virtual ~bad_typeid() throw() {}
	virtual const char* what() const throw() {return "bad_typeid";}
  };

_STLP_END_NAMESPACE

#else
#if defined (__GNUC__)
# undef _STLP_OLDSTD_typeinfo
#  include <typeinfo>
# define _STLP_OLDSTD_typeinfo
#else
#  include _STLP_NATIVE_CPP_RUNTIME_HEADER(typeinfo.h)
#endif

// if <typeinfo> already included, do not import anything

# if defined  (_STLP_USE_OWN_NAMESPACE) && ! (defined (_STLP_TYPEINFO) && !defined(_STLP_NO_NEW_NEW_HEADER))

_STLP_BEGIN_NAMESPACE

using /*_STLP_VENDOR_EXCEPT_STD */ :: type_info;
# if !(defined(__MRC__) || (defined(__SC__) && !defined(__DMC__)))
using /* _STLP_VENDOR_EXCEPT_STD */ :: bad_typeid;
# endif
using /* _STLP_VENDOR_EXCEPT_STD */ :: bad_cast;

_STLP_END_NAMESPACE

#endif /* _STLP_OWN_NAMESPACE */

#endif // __SYMBIAN32__

# endif /* _STLP_NO_TYPEINFO */

# if (_STLP_OUTERMOST_HEADER_ID == 0x874)
#  include <stl/_epilog.h>
#  undef _STLP_OUTERMOST_HEADER_ID
# endif

#endif /* _STLP_OLDSTD_typeinfo */

