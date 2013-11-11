

/***************************************************************************
 *
 * 22.locale.num.put.cpp - tests exercising the std::num_put facet
 *
 * $Id: 22.locale.num.put.cpp 290034 2005-09-19 00:48:21Z sebor $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

#ifdef __GNUG__
   // prevent gcc -Wshadow warnings for convenience types
#  define ushort __rw_ushort
#  define uint   __rw_uint
#  define ulong  __rw_ulong
#  include <sys/types.h>
#  undef ushort
#  undef uint
#  undef ulong
#endif   // __GNUG__

#include <ios>
#include <locale>

#include <cerrno>    // for ERANGE, errno
#include <cfloat>    // for floating limits macros
#include <climits>   // for integral limits macros
#include <clocale>   // for LC_NUMERIC, setlocale()
#include <cstdio>    // for sprintf()
#include <cstdlib>   // for mbstowcs()

#include <any.h>         // for TOSTR()
#include <cmdopt.h>      // for rw_enabled
#include <driver.h>      // for rw_test
#include <localedef.h>   // for rw_locales
#include <valcmp.h>      // for rw_equal
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int failures=0;


#ifdef __ARMCC__
#pragma diag_suppress 61
#pragma diag_suppress 236
#endif

/**************************************************************************/
//#if 0
// set by the command line option handler in response to:
static int rw_opt_enable_num_get = 0;   // --enable-num_get
static int rw_opt_no_errno       = 0;   // --no-errno
static int rw_opt_no_grouping    = 0;   // --no-grouping
static int rw_opt_no_widen       = 0;   // --no-widen

/**************************************************************************/

// replacement ctype facet
template <class charT>
struct Ctype: std::ctype<charT>
{
    typedef std::ctype<charT> Base;

    typedef typename Base::char_type char_type;

    static int n_widen_;

    Ctype (): Base (0, 0, 1) { }

    virtual char_type do_widen (char c) const {
        ++n_widen_;

        switch (c) {
        case '0': c = '9'; break;
        case '1': c = '8'; break;
        case '2': c = '7'; break;
        case '3': c = '6'; break;
        case '4': c = '5'; break;
        case '5': c = '4'; break;
        case '6': c = '3'; break;
        case '7': c = '2'; break;
        case '8': c = '1'; break;
        case '9': c = '0'; break;
        default: break;
        }

        return char_type (c);
    }

    virtual const char_type*
    do_widen (const char *lo, const char *hi, char_type *dst) const {
        return Base::do_widen (lo, hi, dst);
    }
};

template <class charT>
int Ctype<charT>::n_widen_;


/**************************************************************************/

// replacement numpunct facet

template <class charT>
struct Punct: std::numpunct<charT>
{
    typedef typename std::numpunct<charT>::char_type   char_type;
    typedef typename std::numpunct<charT>::string_type string_type;

    static char_type        decimal_point_;
    static char_type        thousands_sep_;
    static const char      *grouping_;
    static const char_type *truename_;
    static const char_type *falsename_;

    static int n_objs_;            // number of facet objects in existence
    static int n_thousands_sep_;   // number of calls to do_thousands_sep()

    Punct (std::size_t ref = 0)
        : std::numpunct<charT>(ref) {
        ++n_objs_;
    }

    ~Punct () {
        --n_objs_;
    }

    virtual char_type do_decimal_point () const {
        return decimal_point_;
    }

    virtual std::string do_grouping () const {
        return grouping_;
    }

    virtual char_type do_thousands_sep () const {
        ++n_thousands_sep_;
        return thousands_sep_;
    }

    virtual string_type do_truename () const {
        return truename_ ? string_type (truename_) : string_type ();
    }

    virtual string_type do_falsename () const {
        return falsename_ ? string_type (falsename_) : string_type ();
    }
};

template <class charT>
const char* Punct<charT>::grouping_ = "";

template <class charT>
typename Punct<charT>::char_type Punct<charT>::decimal_point_ = '.';

template <class charT>
typename Punct<charT>::char_type Punct<charT>::thousands_sep_ = ',';

template <class charT>
const typename Punct<charT>::char_type* Punct<charT>::truename_;

template <class charT>
const typename Punct<charT>::char_type* Punct<charT>::falsename_;

template <class charT>
int Punct<charT>::n_thousands_sep_;

template <class charT>
int Punct<charT>::n_objs_;


/**************************************************************************/

template <class charT>
struct Ios: std::basic_ios<charT>
{
    Ios () { this->init (0); }
};


template <class charT>
struct NumGet: std::num_get<charT, const charT*> { NumGet () { } };


template <class charT>
struct NumPut: std::num_put<charT, charT*>
{
    typedef std::num_put<charT, charT*> Base;
    typedef typename Base::char_type    char_type;
    typedef typename Base::iter_type    iter_type;

    enum {
        test_bool, test_short, test_ushort, test_int, test_uint,
        test_long, test_ulong, test_llong, test_ullong,
        test_pvoid, test_float, test_double, test_ldouble
    };
        
    static int ncalls_ [13];

    NumPut (std::size_t ref = 0): Base (ref) { }

#ifndef _RWSTD_NO_BOOL

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill, bool v) const {
        ++ncalls_ [test_bool];
        return Base::do_put (it, f, fill, v);
    }

#endif   // _RWSTD_NO_BOOL

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill, long v) const {
        ++ncalls_ [test_long];
        return Base::do_put (it, f, fill, v);
    }

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill,
            unsigned long v) const {
        ++ncalls_ [test_ulong];
        return Base::do_put (it, f, fill, v);
    }

#ifdef _RWSTD_LONG_LONG

    // provided as an extension unless _RWSTD_NO_LONG_LONG is #defined
    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill,
            unsigned _RWSTD_LONG_LONG v) const {
        ++ncalls_ [test_ullong];
        return Base::do_put (it, f, fill, v);
    }

    // provided as an extension unless _RWSTD_NO_LONG_LONG is #defined
    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill,
            _RWSTD_LONG_LONG v) const {
        ++ncalls_ [test_llong];
        return Base::do_put (it, f, fill, v);
    }

#endif   // _RWSTD_LONG_LONG

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill, double v) const {
        ++ncalls_ [test_double];
        return Base::do_put (it, f, fill, v);
    }

#ifndef __SYMBIAN32__
#ifndef _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill,
            long double v) const {
        ++ncalls_ [test_ldouble];
        return Base::do_put (it, f, fill, v);
    }
#endif 
#endif   // _RWSTD_NO_LONG_DOUBLE

    virtual iter_type
    do_put (iter_type it, std::ios_base &f, char_type fill,
            const void *v) const {
        ++ncalls_ [test_pvoid];
        return Base::do_put (it, f, fill, v);
    }
};


// explicit initialization of the static array below used
// to work around a SunPro 5.4 bug (PR #27293)
template <class charT>
/* static */ int NumPut<charT>::ncalls_ [13] = { 0 };

/**************************************************************************/

template <class charT, class T>
void do_test (charT           /* dummy */,
              const char     *cname,    // charT name
              const char     *tname,    // T name
              int             lineno,   // line number
              T               val,      // value to format
              int             flags,    // ios flags
              std::streamsize prec,     // precision
              std::streamsize width,    // field width
              char            fill,     // fill character
              const char     *grouping, // grouping string
              const char     *str,      // expected output
              int             err_expect = -1,   // expected iostate
              T val_expect = T ()       /* expected num_get result */)
{
    if (!rw_enabled (lineno)) {
        rw_note (0, __FILE__, __LINE__, "test on line %d disabled", lineno);
        return;
    }
    
    // create a distinct punctuation facet for each iteration to make sure
    // any data cached in between successive calls to the facet's public
    // member functions are flushed
    const Punct<charT> pun (1);

    Ios<charT> ios;
    NumPut<charT> np;

    ios.imbue (std::locale (ios.getloc (), (const std::numpunct<charT>*)&pun));

    ios.flags (std::ios_base::fmtflags (flags));
    ios.precision (prec);
    ios.width (width);
    pun.grouping_ = grouping;

//#if defined (_RWSTD_LDBL_MAX_10_EXP)

//    charT buf [_RWSTD_LDBL_MAX_10_EXP + 2] = { 0 };

//#else   // if !defined (_RWSTD_LDBL_MAX_10_EXP)

    //charT buf [4096] = { 0 };
    charT buf [512] = { 0 };

//#endif   // _RWSTD_LDBL_MAX_10_EXP

    const charT* const bufend = np.put (buf, ios, fill, val);

    // verify 22.2.2.2.2, p21
    if (ios.width ()) {

        static int width_fail = 0;

        // fail at most once for each specialization
        #ifndef __SYMBIAN32__
        rw_assert (!width_fail++, 0, lineno,
                   "%line d: num_put<%s>::put (..., %s = %s) "
                   "failed to reset width from %d; width() = %d",
                   __LINE__, cname, tname, TOSTR (val),
                   TOSTR (buf), width, ios.width ());
       #else
        {
          
           if(width_fail++)
           {
             
            failures++;
            std_log(LOG_FILENAME_LINE,"Reason:Failing") ;
             
             }
        }
                    
        #endif           
    }

    /******************************************************************/

    if (str) {

        char cbuf [sizeof buf / sizeof *buf] = { '\0' };

        if ('%' == *str) {
            std::sprintf (cbuf, str, val);
            str = cbuf;

#if defined (_WIN32) || defined (_WIN64)

            std::size_t len = std::strlen (str);

            if (   ('e' == cbuf [len - 5] || 'E' == cbuf [len - 5])
                && ('-' == cbuf [len - 4] || '+' == cbuf [len - 4])
                && ('0' == cbuf [len - 3])) {
                cbuf [len - 3] = cbuf [len - 2];
                cbuf [len - 2] = cbuf [len - 1];
                cbuf [len - 1] = cbuf [len];
            }

#endif   // _WIN{32,64}
        }

        #ifndef __SYMBIAN32__
        // compare output produced by num_put with that produced by printf()
        rw_assert (0 == rw_strncmp (buf, str), 0, lineno,
                   "line %d: num_put<%s>::put (..., %s = %s) "
                   "wrote %{*Ac}, expected %{*Ac}, "
                   "flags = %{If}, precision = %d"
                   "%{?}, grouping = \"%#s\"%{;}",
                   __LINE__, cname, tname, TOSTR (val),
                   sizeof *buf, buf, sizeof *str, str,
                   flags, prec,
                   grouping && *grouping, grouping);
        #else
         {
         if(0 != rw_strncmp (buf, str)){
          failures++;
          std_log(LOG_FILENAME_LINE,"Reason: Failing");}
                     
         } 
         #endif  // __SYMBIAN32__    
    }

    /******************************************************************/

#ifndef NO_NUM_GET

    if (!rw_opt_enable_num_get)
        return;

    // skip negative precision (extension requiring special treatment)
    if (prec < 0)
        return;

    const charT *next = buf;

    // skip leading fill characters (if any)
    for ( ; *next == fill; ++next);

    // find the first non-fill character if it exists
    const charT *last = next;
    for ( ; *last && *last != fill; ++last);
    for ( ; *last && *last == fill; ++last);

    // do not perform extraction if there are any fill characters
    // in the middle of output (they serve as punctuators and break
    // up the input sequence)
    if (next != last && *last)
        return;

    // do not perform extraction if the fill character is the minus
    // sign, the plus sign, the thousands separator, the decimal point,
    // or a digit
    if (   '-' == fill
        || '+' == fill
        || pun.thousands_sep_ == fill
        || pun.decimal_point_ == fill
        || fill >= '0' && fill <= '9')
        return;

    // do not perform extraction if there is no data to extract
    // (unless this is an extraction-only test)
    if (!*next && str)
        return;

    NumGet<charT> ng;

    T x = T ();

    std::ios_base::iostate err = std::ios_base::goodbit;

    if (-1 == err_expect) {

        // lwg issue 17: special treatment for bool:

        // The in iterator is always left pointing one position beyond
        // the last character successfully matched. If val is set, then
        // err is set to str.goodbit; or to str.eofbit if, when seeking
        // another character to match, it is found that (in==end). If
        // val is not set, then err is set to str.failbit; or to
        // (str.failbit|str.eofbit) if the reason for the failure was
        // that (in==end). [Example: for targets true:"a" and false:"abb",
        // the input sequence "a" yields val==true and err==str.eofbit;
        // the input sequence "abc" yields err=str.failbit, with in ending
        // at the 'c' element. For targets true:"1" and false:"0", the
        // input sequence "1" yields val==true and err=str.goodbit. For
        // empty targets (""), any input sequence yields err==str.failbit.
        // --end example]

        err_expect = T (1) == T (2) && flags & std::ios::boolalpha
            ? std::ios::goodbit
            : last == bufend && last [-1] != fill
            ? std::ios::eofbit : std::ios::goodbit;

        val_expect = val;
    }

    last = ng.get (next, bufend, ios, err, x);
#ifndef __SYMBIAN32__
    // verify the state
    rw_assert (err == err_expect, 0, lineno,
               "line %d: num_get<%s>::get (%s, ..., %s&) "
               "flags = %{If}, "
               "%{?}grouping = \"%#s\", %{;}"
               "iostate = %{Is}, expected %{Is}",
               __LINE__, cname,
               TOSTR (next), tname,
               flags,
               grouping && *grouping, grouping,
               err, err_expect);

    // verify the parsed value
    rw_assert (rw_equal (x, val_expect), 0, lineno,
               "line %d: num_get<%s>::get (%s, ..., %s&) got %s, "
               "expected %s, flags = %{If}, "
               "%{?}grouping = \"%#s\", %{;}",
               __LINE__, cname,
               TOSTR (next), tname,
               TOSTR (x), TOSTR (val_expect),
               flags,
               grouping && *grouping, grouping);
  #else
   {
   
   if(err != err_expect)
     {
     failures++;
     std_log(LOG_FILENAME_LINE,"Reason:Failing");
     }
   if(!rw_equal (x, val_expect))
     {
     failures++;
     std_log(LOG_FILENAME_LINE,"Reason:Failing");
     }
   
   
   
   }             
#endif//__SYMBIAN32__
#else

    _RWSTD_UNUSED (bufend);
    _RWSTD_UNUSED (err_expect);
    _RWSTD_UNUSED (val_expect);

#endif   // NO_NUM_GET

}

/**************************************************************************/

template <class charT>
struct Streambuf: std::basic_streambuf<charT, std::char_traits<charT> > { };
#if 0 //FIX
template <class charT>
void direct_use_test (charT, const char *cname)
{
    _RWSTD_UNUSED (cname);

    // verify that num_put objects can be used directly w/o first
    // having been installed in a locale object; the behavior of
    // such facets is unspecified (but not undefined)

    Ios<charT> ios;

    const std::num_put<charT> np; //can not directly instantiate num_put, it is illegal, same behaviour in linux also 

    Streambuf<charT> sb;

#define DIRECT_USE_TEST(T) \
    np.put (std::ostreambuf_iterator<charT>(&sb), ios, charT (), (T)0)

#ifndef _RWSTD_NO_BOOL

    DIRECT_USE_TEST (bool);

#endif   // _RWSTD_NO_BOOL

    DIRECT_USE_TEST (unsigned long);
    DIRECT_USE_TEST (long);
    DIRECT_USE_TEST (double);

#ifndef _RWSTD_NO_LONG_DOUBLE

    DIRECT_USE_TEST (long double);

#endif

    DIRECT_USE_TEST (void*);
}
#endif 
/**************************************************************************/


// for convenience
#define boolalpha   std::ios_base::boolalpha
#define dec         std::ios_base::dec
#define fixed       std::ios_base::fixed
#define hex         std::ios_base::hex
#define internal    std::ios_base::internal
#define left        std::ios_base::left
#define oct         std::ios_base::oct
#define right       std::ios_base::right
#define scientific  std::ios_base::scientific
#define showbase    std::ios_base::showbase
#define showpoint   std::ios_base::showpoint
#define showpos     std::ios_base::showpos
#define skipws      std::ios_base::skipws
#define unitbuf     std::ios_base::unitbuf
#define uppercase   std::ios_base::uppercase
//#define bin         std::ios_base::bin //it is not bin its is binary
#define bin         std::ios_base::binary
#define adjustfield std::ios_base::adjustfield
#define basefield   std::ios_base::basefield
#define floatfield  std::ios_base::floatfield
#define nolock      std::ios_base::nolock
#define nolockbuf   std::ios_base::nolockbuf

#define Bad         std::ios_base::badbit
#define Eof         std::ios_base::eofbit
#define Fail        std::ios_base::failbit
#define Good        std::ios_base::goodbit


template <class charT>
void bool_test (charT, const char *cname)
{
#ifndef _RWSTD_NO_BOOL

    rw_info (0, 0, __LINE__, "std::num_put<%s>::put (..., bool)", cname);

    const char* const tname = "bool";

    static const charT boolnames[][4] = {
        { 'y', 'e', 's', '\0' }, { 'n', 'o', '.', '\0' },

        // unusual strings to try to trip up the formatting algorithm
        { '+', '1', '\0' },      { '-', '2', '\0' },
        { '-', '+', '1', '\0' }, { '+', '-', '0', '\0' },
        { '\001', '\0' },        { '\0' }
    };

    Punct<charT>::decimal_point_ = '*';
    Punct<charT>::truename_      = boolnames [0];
    Punct<charT>::falsename_     = boolnames [1];

// set line number to __LINE__ and perform test so that
// assertion output will point to the failed TEST line
#define TEST  do_test
#define T     charT (), cname, tname, __LINE__

    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, false, 0, 0, 0, ' ', "", "0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    
    TEST (T, true,  0, 0, 0, ' ', "", "1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, 0, 0, 0, ' ', "", "%d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  0, 0, 0, ' ', "", "%d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, showpos, 0, 0, ' ', "", "%+d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  showpos, 0, 0, ' ', "", "%+d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, oct, 0, 0, ' ', "", "%o");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  oct, 0, 0, ' ', "", "%o");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, dec, 0, 0, ' ', "", "%d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  dec, 0, 0, ' ', "", "%d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, hex, 0, 0, ' ', "", "%x");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  hex, 0, 0, ' ', "", "%x");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::boolalpha");

    TEST (T, false, boolalpha, 0, 0, ' ', "", "no.");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 0, ' ', "", "yes");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // left justified boolalpha
    TEST (T, false, boolalpha | left, 0, 6, ' ', "", "no.   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | left, 0, 6, ' ', "", "yes   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // right justified boolalpha
    TEST (T, false, boolalpha | right, 0, 6, ' ', "", "   no.");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | right, 0, 6, ' ', "", "   yes");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // implied right justified boolalpha (internal bit set)
    TEST (T, false, boolalpha | internal, 0, 4, ' ', "", " no.");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | internal, 0, 4, ' ', "", " yes");
    std_log(LOG_FILENAME_LINE,"Test end");



    Punct<charT>::truename_  = boolnames [2];
    Punct<charT>::falsename_ = boolnames [3];

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha, 0, 1, ' ', "", "-2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 1, ' ', "", "+1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha | internal, 0, 4, ' ', "", "  -2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | internal, 0, 4, ' ', "", "  +1");
    std_log(LOG_FILENAME_LINE,"Test end");



    Punct<charT>::truename_  = boolnames [4];
    Punct<charT>::falsename_ = boolnames [5];

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha, 0, 1, ' ', "", "+-0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 1, ' ', "", "-+1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha | internal, 0, 4, ' ', "", " +-0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | internal, 0, 4, ' ', "", " -+1");
    std_log(LOG_FILENAME_LINE,"Test end");



    Punct<charT>::truename_  = boolnames [6];
    Punct<charT>::falsename_ = boolnames [7];

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha, 0, 0, ' ', "", "");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 0, ' ', "", "\001");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha, 0, 1, ' ', "", " ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 1, ' ', "", "\001");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha, 0, 1, ',', "", ",");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha, 0, 1, ',', "", "\001");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, false, boolalpha | internal, 0,4, ',', "", ",,,,");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, true,  boolalpha | internal, 0,4, ',', "", ",,,\001");
    std_log(LOG_FILENAME_LINE,"Test end");


#endif   // _RWSTD_NO_BOOL

}

/**************************************************************************/

template <class charT>
void long_test (charT, const char *cname)
{
    const char* const tname = "long";

    rw_info (0, 0, __LINE__, "std::num_put<%s>::put (..., %s)", cname, tname);

// working around a VAC++/AIX bug where LONG_{MIN,MAX} are
// of type int rather than long as required (see PR #28798)
#ifndef	LONG_MIN 
#undef LONG_MIN
#define LONG_MIN _RWSTD_LONG_MIN
#endif

#ifndef	LONG_MAX
#undef LONG_MAX
#define LONG_MAX _RWSTD_LONG_MAX
#endif

#undef GET_FAIL
#define GET_FAIL (Eof | Fail), LONG_MAX

    //////////////////////////////////////////////////////////////////
    // implicit decimal output
    rw_info (0, 0, __LINE__, "std::ios::fmtflags ()");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       2L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -3L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      12L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     -13L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     345L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    -456L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    6789L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -7890L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   98765L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -98766L,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN,   0, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");


    //////////////////////////////////////////////////////////////////
    // explicit decimal ouptut
    rw_info (0, 0, __LINE__, "std::ios::dec");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       2L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      12L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     345L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    6789L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   98765L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,      ~0L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -1L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -2L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     -12L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    -345L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -6789L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -98765L, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");


    rw_info (0, 0, __LINE__, "std::ios::dec | std::ios::showbase");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, dec | showbase, 0, 0, ' ', "", "%#ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, dec | showbase, 0, 0, ' ', "", "%#ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -1235L, dec | showbase, 0, 0, ' ', "", "%#ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, dec | showbase, 0, 0, ' ', "", "%#ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, dec | showbase, 0, 0, ' ', "", "%#ld");
    std_log(LOG_FILENAME_LINE,"Test end");


    rw_info (0, 0, __LINE__, "std::ios::dec | std::ios::showpos");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, dec | showpos, 0, 0, ' ', "", "%+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1236L, dec | showpos, 0, 0, ' ', "", "%+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -1237L, dec | showpos, 0, 0, ' ', "", "%+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, dec | showpos, 0, 0, ' ', "", "%+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, dec | showpos, 0, 0, ' ', "", "%+ld");
    std_log(LOG_FILENAME_LINE,"Test end");


    rw_info (0, 0, __LINE__,
             "std::ios::dec | std::ios::showbase | std::ios::showpos");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, dec | showbase | showpos, 0, 0, ' ', "", "%#+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1238L, dec | showbase | showpos, 0, 0, ' ', "", "%#+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -1239L, dec | showbase | showpos, 0, 0, ' ', "", "%#+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, dec | showbase | showpos, 0, 0, ' ', "", "%#+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, dec | showbase | showpos, 0, 0, ' ', "", "%#+ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // left justfication
    rw_info (0, 0, __LINE__, "std::ios::dec | std::ios::left");

    TEST (T,  0L,         dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1L,         dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12L,        dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123L,       dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234L,      dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345L,     dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456L,    dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234567L,   dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345678L,  dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | left, 0, 10, ' ', "", "%-10ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // left justfication, signed dec
    TEST (T,  -1L,         dec | left, 0, 10, ' ', "", "-1        ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12L,        dec | left, 0, 10, ' ', "", "-12       ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123L,       dec | left, 0, 10, ' ', "", "-123      ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234L,      dec | left, 0, 10, ' ', "", "-1234     ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345L,     dec | left, 0, 10, ' ', "", "-12345    ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456L,    dec | left, 0, 10, ' ', "", "-123456   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234567L,   dec | left, 0, 10, ' ', "", "-1234567  ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345678L,  dec | left, 0, 10, ' ', "", "-12345678 ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 10, ' ', "", "-123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // left justification with fill char
    TEST (T,  -1L,         dec | left, 0, 10, '#', "", "-1########");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12L,        dec | left, 0, 10, '@', "", "-12@@@@@@@");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123L,       dec | left, 0, 10, '*', "", "-123******");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234L,      dec | left, 0, 10, '=', "", "-1234=====");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345L,     dec | left, 0, 10, '.', "", "-12345....");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456L,    dec | left, 0, 10, ',', "", "-123456,,,");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234567L,   dec | left, 0, 10, '-', "", "-1234567--");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345678L,  dec | left, 0, 10, '+', "", "-12345678+");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 10, ';', "", "-123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // left justfication with grouping
    TEST (T,  1L,         dec | left, 0, 14, ' ', "\2", "1             ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12L,        dec | left, 0, 14, ' ', "\2", "12            ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123L,       dec | left, 0, 14, ' ', "\2", "1,23          ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234L,      dec | left, 0, 14, ' ', "\2", "12,34         ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345L,     dec | left, 0, 14, ' ', "\2", "1,23,45       ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456L,    dec | left, 0, 14, ' ', "\2", "12,34,56      ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234567L,   dec | left, 0, 14, ' ', "\2", "1,23,45,67    ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345678L,  dec | left, 0, 14, ' ', "\2", "12,34,56,78   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | left, 0, 14, ' ', "\2", "1,23,45,67,89 ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | left, 0, 14, ',', "\2", "1,23,45,67,89,");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  1L,         dec | left, 0, 14, ' ', "\2\1\3", "1             ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12L,        dec | left, 0, 14, ' ', "\2\1\3", "12            ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123L,       dec | left, 0, 14, ' ', "\2\1\3", "1,23          ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234L,      dec | left, 0, 14, ' ', "\2\1\3", "1,2,34        ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345L,     dec | left, 0, 14, ' ', "\2\1\3", "12,3,45       ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456L,    dec | left, 0, 14, ' ', "\2\1\3", "123,4,56      ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234567L,   dec | left, 0, 14, ' ', "\2\1\3", "1,234,5,67    ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345678L,  dec | left, 0, 14, ' ', "\2\1\3", "12,345,6,78   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | left, 0, 14, ' ', "\2\1\3", "123,456,7,89  ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | left, 0, 14, '0', "\2\1\3", "123,456,7,8900");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  -1L,         dec | left, 0, 14, ' ', "\1\2\3", "-1            ");
        std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
TEST (T,  -12L,        dec | left, 0, 14, ' ', "\1\2\3", "-1,2          ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123L,       dec | left, 0, 14, ' ', "\1\2\3", "-12,3         ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234L,      dec | left, 0, 14, ' ', "\1\2\3", "-1,23,4       ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345L,     dec | left, 0, 14, ' ', "\1\2\3", "-12,34,5      ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456L,    dec | left, 0, 14, ' ', "\1\2\3", "-123,45,6     ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234567L,   dec | left, 0, 14, ' ', "\1\2\3", "-1,234,56,7   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345678L,  dec | left, 0, 14, ' ', "\1\2\3", "-12,345,67,8  ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 14, ' ', "\1\2\3", "-123,456,78,9 ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 14, ' ', "\3\1\2", "-1,23,45,6,789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  -123456780L, dec | left, 0, 14, ' ', "\x1", "-1,2,3,4,5,6,7,8,0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  -123456781L, dec | left, 0, 14, ' ', "\x2", "-1,23,45,67,81");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456782L, dec | left, 0, 14, ' ', "\x3", "-123,456,782  ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456783L, dec | left, 0, 14, ' ', "\x4", "-1,2345,6783  ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456784L, dec | left, 0, 14, ' ', "\x5", "-1234,56784   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456785L, dec | left, 0, 14, ' ', "\x6", "-123,456785   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456786L, dec | left, 0, 14, ' ', "\x7", "-12,3456786   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456787L, dec | left, 0, 14, ' ', "\x8", "-1,23456787   ");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456788L, dec | left, 0, 14, ' ', "\x9", "-123456788    ");
    std_log(LOG_FILENAME_LINE,"Test end");


#ifndef _RWSTD_NO_EXT_BIN_IO

    rw_info (0, 0, __LINE__, "std::ios::dec | std::ios::bin [extension]");

    TEST (T, 33333333L, bin | dec, 0, 0, ' ', "", "%ld");

#endif   // _RWSTD_NO_EXT_BIN_IO

    //////////////////////////////////////////////////////////////////
    // explicit octal ouptut
    rw_info (0, 0, __LINE__, "std::ios::oct");
    
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, oct, 0, 0, ' ', "", "%lo");
        std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
TEST (T,       1L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       2L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      12L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     345L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    6789L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   98765L, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, LONG_MAX - 1, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // negative values formatted as oct cause overflow on input (lwg issue 23)
    TEST (T,      ~0L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -1L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -2L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     -12L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    -345L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -6789L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -98765L, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, oct, 0, 0, ' ', "", "%lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::oct | std::ios::dec");

    TEST (T, 13579L, oct | dec, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::oct | std::ios::showbase");

    TEST (T,       0L, oct | showbase, 0, 0, ' ', "", "%#lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    2345L, oct | showbase, 0, 0, ' ', "", "%#lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -2346L, oct | showbase, 0, 0, ' ', "", "%#lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, oct | showbase, 0, 0, ' ', "", "%#+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, oct | showbase, 0, 0, ' ', "", "%#+lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::oct | std::ios::showpos");

    TEST (T,       0L, oct | showpos, 0, 0, ' ', "", "%+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    2347L, oct | showpos, 0, 0, ' ', "", "%+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -2348L, oct | showpos, 0, 0, ' ', "", "%+lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, LONG_MAX, oct | showpos, 0, 0, ' ', "", "%+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, oct | showpos, 0, 0, ' ', "", "%+lo", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__,
             "std::ios::oct | std::ios::showbase | std::ios::showpos");

    TEST (T,       0L, oct | showbase | showpos, 0, 0, ' ', "", "%#+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    2349L, oct | showbase | showpos, 0, 0, ' ', "", "%#+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -2350L, oct | showbase | showpos, 0, 0, ' ', "", "%#+lo",
          GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, LONG_MAX, oct | showbase | showpos, 0, 0, ' ', "", "%#+lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, oct | showbase | showpos, 0, 0, ' ', "", "%#+lo",
          GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");


#ifndef _RWSTD_NO_EXT_BIN_IO

    rw_info (0, 0, __LINE__, "std::ios::oct | std::ios::bin [extension]");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, 22222222L, bin | oct, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");


#endif   // _RWSTD_NO_EXT_BIN_IO

    //////////////////////////////////////////////////////////////////
    // explicit hexadecimal ouptut
    rw_info (0, 0, __LINE__, "std::ios::hex");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,       0L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       2L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      12L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     345L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    6789L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   98765L, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX - 1, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MAX - 2, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // negative values formatted as hex cause overflow on input (lwg issue 23)
    TEST (T,      ~0L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -1L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      -2L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     -12L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    -345L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   -6789L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -98765L, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, LONG_MIN, hex, 0, 0, ' ', "", "%lx", GET_FAIL);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::uppercase");

    TEST (T,       0L, hex | uppercase, 0, 0, ' ', "", "%lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, hex | uppercase, 0, 0, ' ', "", "%lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0XABCDL, hex | uppercase, 0, 0, ' ', "", "%lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0XBCDEL, hex | uppercase, 0, 0, ' ', "", "%lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0XCDEFL, hex | uppercase, 0, 0, ' ', "", "%lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::showbase");

    TEST (T,       0L, hex | showbase, 0, 0, ' ', "", "%#lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, hex | showbase, 0, 0, ' ', "", "%#lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0xdef1L, hex | showbase, 0, 0, ' ', "", "%#lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    

    rw_info (0, 0, __LINE__,
             "std::ios::hex | std::ios::uppercase | std::ios::showbase");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       0L, hex | uppercase | showbase, 0, 0, ' ', "", "%#lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, hex | uppercase | showbase, 0, 0, ' ', "", "%#lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0XEF02L, hex | uppercase | showbase, 0, 0, ' ', "", "%#lX");
    std_log(LOG_FILENAME_LINE,"Test end");
    
    
    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::oct");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       0L, oct | hex, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   97531L, oct | hex, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    

    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::dec");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   86420L, dec | hex, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");


    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::dec | std::ios::oct");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, 11111111L, oct | dec | hex, 0, 0, ' ', "", "%ld");
    std_log(LOG_FILENAME_LINE,"Test end");
    

#ifndef _RWSTD_NO_EXT_BIN_IO

    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::bin [extension]");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, 44444444L, bin | hex, 0, 0, ' ', "", "%ld");
std_log(LOG_FILENAME_LINE,"Test end");
    rw_info (0, 0, __LINE__, "std::ios::hex | std::ios::dec | "
                  "std::ios::oct | std::ios::bin [extension]");
std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, 55555555L, bin | oct | dec | hex, 0, 0, ' ', "", "%ld");
std_log(LOG_FILENAME_LINE,"Test end");
#endif   // _RWSTD_NO_EXT_BIN_IO


    //////////////////////////////////////////////////////////////////
    // extension: fixed and negative precision

    rw_info (0, 0, __LINE__,
             "std::ios::fixed with negative precision [extension]");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654321L, 0,      -1, 0, ' ', "", "987654321");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654322L, fixed,   0, 0, ' ', "", "987654322");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654323L, fixed,  -1, 0, ' ', "", "98765432*3");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654323L, fixed,  -1, 0, ' ', "", "-98765432*3");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654324L, fixed,  -2, 0, ' ', "", "9876543*24");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654324L, fixed,  -2, 0, ' ', "", "-9876543*24");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654325L, fixed,  -3, 0, ' ', "", "987654*325");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654325L, fixed,  -3, 0, ' ', "", "-987654*325");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654326L, fixed,  -4, 0, ' ', "", "98765*4326");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654326L, fixed,  -4, 0, ' ', "", "-98765*4326");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654327L, fixed,  -5, 0, ' ', "", "9876*54327");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654327L, fixed,  -5, 0, ' ', "", "-9876*54327");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654328L, fixed,  -6, 0, ' ', "", "987*654328");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654328L, fixed,  -6, 0, ' ', "", "-987*654328");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654329L, fixed,  -7, 0, ' ', "", "98*7654329");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654329L, fixed,  -7, 0, ' ', "", "-98*7654329");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654330L, fixed,  -8, 0, ' ', "", "9*87654330");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654330L, fixed,  -8, 0, ' ', "", "-9*87654330");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654331L, fixed,  -9, 0, ' ', "", "0*987654331");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654331L, fixed,  -9, 0, ' ', "", "-0*987654331");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654332L, fixed, -10, 0, ' ', "", "0*0987654332");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654332L, fixed, -10, 0, ' ', "", "-0*0987654332");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  987654333L, fixed, -11, 0, ' ', "", "0*00987654333");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, -987654333L, fixed, -11, 0, ' ', "", "-0*00987654333");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,          0L, fixed, -12, 0, ' ', "", "0*000000000000");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    //////////////////////////////////////////////////////////////////

// second group is the last group (i.e., prevent repetition)
#if CHAR_MAX == UCHAR_MAX
#  define GROUPING "\2\1\xff"
#else
#  define GROUPING "\2\1\x7f"
#endif
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   123456789L, dec | left, 0, 0, ' ', GROUPING, "123456,7,89");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 0, ' ', GROUPING, "-123456,7,89");

    std_log(LOG_FILENAME_LINE,"Test end");
    
#undef GROUPING

#if CHAR_MAX == UCHAR_MAX
#  define GROUPING "\2\3\xff"
#else
#  define GROUPING "\2\3\x7f"
#endif
std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   123456789L, dec | left, 0, 0, ' ', GROUPING, "1234,567,89");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, dec | left, 0, 0, ' ', GROUPING, "-1234,567,89");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  0x12345678L, hex | showbase, 0, 0, ' ', GROUPING, "0x123,456,78");
    std_log(LOG_FILENAME_LINE,"Test end");


    //////////////////////////////////////////////////////////////////
    // right justfication
    rw_info (0, 0, __LINE__, "std::ios::right");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1L,         dec | right, 0, 10, ' ', "", "         1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12L,        dec | right, 0, 10, ' ', "", "        12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123L,       dec | right, 0, 10, ' ', "", "       123");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234L,      dec | right, 0, 10, ' ', "", "      1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345L,     dec | right, 0, 10, ' ', "", "     12345");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456L,    dec | right, 0, 10, ' ', "", "    123456");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  1234567L,   dec | right, 0, 10, ' ', "", "   1234567");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  12345678L,  dec | right, 0, 10, ' ', "", "  12345678");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | right, 0, 10, ' ', "", " 123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | right, 0, 10, '0', "", "0123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | right, 0, 10, '+', "", "+123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  123456789L, dec | right, 0, 10, '-', "", "-123456789");
    std_log(LOG_FILENAME_LINE,"Test end");


    // right justification, oct
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0L,         oct | right, 0, 10, ' ', "", "         0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01L,        oct | right, 0, 10, ' ', "", "         1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012L,       oct | right, 0, 10, ' ', "", "        12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0123L,      oct | right, 0, 10, ' ', "", "       123");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01234L,     oct | right, 0, 10, ' ', "", "      1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012345L,    oct | right, 0, 10, ' ', "", "     12345");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0123456L,   oct | right, 0, 10, ' ', "", "    123456");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01234567L,  oct | right, 0, 10, ' ', "", "   1234567");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012345670L, oct | right, 0, 10, ' ', "", "  12345670");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012345670L, oct | right, 0, 10, '1', "", "1112345670");

    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    // right justification with grouping
    TEST (T,  0L,         oct | right, 0, 10, ' ', "\2",  "         0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01L,        oct | right, 0, 10, ' ', "\2",  "         1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012L,       oct | right, 0, 10, ' ', "\2",  "        12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0123L,      oct | right, 0, 10, ' ', "\2",  "      1,23");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01234L,     oct | right, 0, 10, ' ', "\2",  "     12,34");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012345L,    oct | right, 0, 10, ' ', "\2",  "   1,23,45");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0123456L,   oct | right, 0, 10, ' ', "\2",  "  12,34,56");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  01234567L,  oct | right, 0, 10, ' ', "\2",  "1,23,45,67");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  012345670L, oct | right, 0, 10, ' ', "\2", "12,34,56,70");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

#undef FLAGS
#define flags   hex | showbase | internal
#define FLAGS   flags | uppercase

    // internal justfication, hex
    rw_info (0, 0, __LINE__, "std::ios::internal");
    TEST (T,  0X1L,        FLAGS, 0, 10, ' ', "", "0X       1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x12L,       flags, 0, 10, ' ', "", "0x      12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X123L,      FLAGS, 0, 10, ' ', "", "0X     123");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x1234L,     flags, 0, 10, ' ', "", "0x    1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345L,    FLAGS, 0, 10, ' ', "", "0X   12345");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x123abcL,   flags, 0, 10, ' ', "", "0x  123abc");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x123abcL,   flags, 0, 10, '0', "", "0x00123abc");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X1234ABCL,  FLAGS, 0, 10, ' ', "", "0X 1234ABC");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345678L, FLAGS, 0, 10, ' ', "", "0X12345678");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345678L, FLAGS, 0, 10, '0', "", "0X12345678");

    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    // internal justfication, hex, grouping
    TEST (T,  0X1L,        FLAGS, 0, 10, ' ', "\1\2",     "0X       1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x12L,       flags, 0, 10, ' ', "\1\2",     "0x     1,2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X123L,      FLAGS, 0, 10, ' ', "\1\2",     "0X    12,3");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x1234L,     flags, 0, 10, ' ', "\1\2",     "0x  1,23,4");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345L,    FLAGS, 0, 10, ' ', "\1\2",     "0X 12,34,5");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345L,    FLAGS, 0, 10, ',', "\1\2",     "0X,12,34,5");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x123abcL,   flags, 0, 10, ' ', "\1\2",    "0x1,23,ab,c");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X1234ABCL,  FLAGS, 0, 10, ' ', "\1\2",   "0X12,34,AB,C");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345678L, FLAGS, 0, 10, ' ', "\1\2", "0X1,23,45,67,8");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0X12345678L, FLAGS, 0, 10, '0', "\1\2", "0X1,23,45,67,8");
    std_log(LOG_FILENAME_LINE,"Test end");


#undef flags
#undef FLAGS
#define FLAGS   dec | showpos | internal

    // internal justfication, signed dec

    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,   0L,         FLAGS, 0, 10, ' ', "", "+        0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1L,         FLAGS, 0, 10, ' ', "", "-        1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +12L,        FLAGS, 0, 10, ' ', "", "+       12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123L,       FLAGS, 0, 10, ' ', "", "-      123");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +1234L,      FLAGS, 0, 10, ' ', "", "+     1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345L,     FLAGS, 0, 10, ' ', "", "-    12345");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +123456L,    FLAGS, 0, 10, ' ', "", "+   123456");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234567L,   FLAGS, 0, 10, ' ', "", "-  1234567");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +12345678L,  FLAGS, 0, 10, ' ', "", "+ 12345678");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456789L, FLAGS, 0, 10, '0', "", "-123456789");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  +1L,         FLAGS, 0, 10, '-', "", "+--------1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12L,        FLAGS, 0, 10, '-', "", "--------12");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +123L,       FLAGS, 0, 10, '+', "", "+++++++123");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -1234L,      FLAGS, 0, 10, '+', "", "-+++++1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +12345L,     FLAGS, 0, 10, '0', "", "+000012345");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -123456L,    FLAGS, 0, 10, '1', "", "-111123456");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +1234567L,   FLAGS, 0, 10, '2', "", "+221234567");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  -12345678L,  FLAGS, 0, 10, '3', "", "-312345678");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  +123456789L, FLAGS, 0, 10, '4', "", "+123456789");
    std_log(LOG_FILENAME_LINE,"Test end");


#ifndef _RWSTD_NO_EXT_BIN_IO

    // bin
    rw_info (0, 0, __LINE__, "std::ios::bin [extension]");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,     0L, bin, 0, 16, '.', "\4", "...............0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     1L, bin, 0, 16, '.', "\4", "...............1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     2L, bin, 0, 16, '.', "\4", "..............10");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     3L, bin, 0, 16, '.', "\4", "..............11");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     4L, bin, 0, 16, '.', "\4", ".............100");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     5L, bin, 0, 16, '.', "\4", ".............101");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     6L, bin, 0, 16, '.', "\4", ".............110");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     7L, bin, 0, 16, '.', "\4", ".............111");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     8L, bin, 0, 16, '.', "\4", "............1000");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     9L, bin, 0, 16, '.', "\4", "............1001");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0aL, bin, 0, 16, '.', "\4", "............1010");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0bL, bin, 0, 16, '.', "\4", "............1011");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0cL, bin, 0, 16, '.', "\4", "............1100");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0dL, bin, 0, 16, '.', "\4", "............1101");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0eL, bin, 0, 16, '.', "\4", "............1110");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x0fL, bin, 0, 16, '.', "\4", "............1111");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  0xf0L, bin, 0, 16, '.', "\4", ".......1111,0000");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0xf1L, bin, 0, 16, '.', "\4", ".......1111,0001");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0xf2L, bin, 0, 16, '.', "\4", ".......1111,0010");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0xf3L, bin, 0, 16, '.', "\4", ".......1111,0011");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0xf4L, bin, 0, 16, '.', "\4", ".......1111,0100");
    TEST (T,  0xf5L, bin, 0, 16, '.', "\4", ".......1111,0101");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  0x12345678L, bin, 0, 0, '.', "\4",
             "1,0010,0011,0100,0101,0110,0111,1000");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  0xfedcba98L, bin, 0, 0, '\0', "\010",
             "11111110,11011100,10111010,10011000");
    std_log(LOG_FILENAME_LINE,"Test end");

#endif   // _RWSTD_NO_EXT_BIN_IO

    // locale 3.0 extension

#define BASE(n)   ((n)  << _RWSTD_IOS_BASEOFF)

    // bases 0 and 10 are both base 10
    // base 1 is roman (values 1 through 4999)
    // bases 2 through 36 are what they are
    // anything else is unspecified

    rw_info (0, 0, __LINE__, "base 1 (Roman), and 2 through 36 [extension]");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1234L, BASE ( 0), 0, 0, '\0', "",  "1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 1), 0, 0, '\0', "",  "mccxxxiv");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x1234L, BASE ( 2), 0, 0, '\0', "",  "1001000110100");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,   01234L, oct | BASE ( 8), 0, 0, '\0', "",  "1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, dec | BASE (10), 0, 0, '\0', "",  "1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,  0x1234L, hex | BASE (16), 0, 0, '\0', "",  "1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1234L, BASE ( 2), 0, 0, '\0', "",  "10011010010");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 3), 0, 0, '\0', "",  "1200201");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 4), 0, 0, '\0', "",  "103102");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 5), 0, 0, '\0', "",  "14414");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 6), 0, 0, '\0', "",  "5414");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 7), 0, 0, '\0', "",  "3412");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE ( 9), 0, 0, '\0', "",  "1621");

    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, dec | BASE (10), 0, 0, '\0', "",  "1234");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1234L, BASE (11), 0, 0, '\0', "",  "a22");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (12), 0, 0, '\0', "",  "86a");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (13), 0, 0, '\0', "",  "73c");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (14), 0, 0, '\0', "",  "642");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (15), 0, 0, '\0', "",  "574");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1234L, hex | BASE (16), 0, 0, '\0', "",  "4d2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1234L, BASE (17), 0, 0, '\0', "",  "44a");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (18), 0, 0, '\0', "",  "3ea");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (19), 0, 0, '\0', "",  "37i");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (20), 0, 0, '\0', "",  "31e");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (21), 0, 0, '\0', "",  "2gg");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (22), 0, 0, '\0', "",  "2c2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (23), 0, 0, '\0', "",  "27f");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (24), 0, 0, '\0', "",  "23a");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (25), 0, 0, '\0', "",  "1o9");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (26), 0, 0, '\0', "",  "1lc");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (27), 0, 0, '\0', "",  "1ij");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (28), 0, 0, '\0', "",  "1g2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (29), 0, 0, '\0', "",  "1dg");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (30), 0, 0, '\0', "",  "1b4");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (31), 0, 0, '\0', "",  "18p");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (32), 0, 0, '\0', "",  "16i");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (33), 0, 0, '\0', "",  "14d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (34), 0, 0, '\0', "",  "12a");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (35), 0, 0, '\0', "",  "109");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1234L, BASE (36), 0, 0, '\0', "",  "ya");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // effect of non-empty grouping is unspecified
    TEST (T,       0L, BASE (1), 0, 0, '\0', "",  "0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       1L, BASE (1), 0, 0, '\0', "",  "i");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
TEST (T,       2L, BASE (1), 0, 0, '\0', "",  "ii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       3L, BASE (1), 0, 0, '\0', "",  "iii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       4L, BASE (1), 0, 0, '\0', "",  "iv");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       5L, BASE (1), 0, 0, '\0', "",  "v");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       6L, BASE (1), 0, 0, '\0', "",  "vi");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       7L, BASE (1), 0, 0, '\0', "",  "vii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       8L, BASE (1), 0, 0, '\0', "",  "viii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,       9L, BASE (1), 0, 0, '\0', "",  "ix");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      10L, BASE (1), 0, 0, '\0', "",  "x");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      50L, BASE (1), 0, 0, '\0', "",  "l");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     100L, BASE (1), 0, 0, '\0', "",  "c");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,     500L, BASE (1), 0, 0, '\0', "",  "d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1000L, BASE (1), 0, 0, '\0', "",  "m");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      49L, BASE (1), 0, 0, '\0', "",  "xlix");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      88L, BASE (1), 0, 0, '\0', "",  "lxxxviii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,      99L, BASE (1), 0, 0, '\0', "",  "xcix");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    1999L, BASE (1), 0, 0, '\0', "",  "mcmxcix");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    2000L, BASE (1), 0, 0, '\0', "",  "mm");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    2001L, BASE (1), 0, 0, '\0', "",  "mmi");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    4999L, BASE (1), 0, 0, '\0', "",  "mmmmcmxcix");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,    5000L, BASE (1), 0, 0, '\0', "",  "5000");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,    1492L, BASE (1), 0, 10, '*', "",  "***mcdxcii");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,  1776L, BASE (1) | uppercase, 0, 0, '\0', "", "MDCCLXXVI");
    std_log(LOG_FILENAME_LINE,"Test end");

}


/**************************************************************************/

template <class charT>
void ulong_test (charT, const char *cname)
{
    const char* const tname = "unsigned long";

    rw_info (0, 0, __LINE__, "std::num_put<%s>::put (..., %s)", cname, tname);

    typedef unsigned long ULong;

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, __LINE__, "std::ios::dec");

    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,              0UL,  dec, 0, 0, ' ', "", "%lu");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MAX),  dec, 0, 0, ' ', "", "%lu");

    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    rw_info (0, 0, __LINE__, "std::ios::dec | std::ios::shopos");
    TEST (T,              0UL,  dec | showpos, 0, 0, ' ', "", "%+lu");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,              1UL,  dec | showpos, 0, 0, ' ', "", "%+lu");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MAX),  dec | showpos, 0, 0, ' ', "", "%+lu");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, __LINE__, "std::ios::oct");

    TEST (T, ULong (CHAR_MAX),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (UCHAR_MAX), oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SCHAR_MAX), oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SHRT_MAX),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (USHRT_MAX), oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (INT_MAX),   oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (UINT_MAX),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MAX),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, ULong (CHAR_MIN),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SCHAR_MIN), oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SHRT_MIN),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (INT_MIN),   oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MIN),  oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    // no overflow
    TEST (T,               1UL, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,              ~0UL, oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (ULONG_MAX), oct, 0, 0, ' ', "", "%lo");
    std_log(LOG_FILENAME_LINE,"Test end");
    

    //////////////////////////////////////////////////////////////////
    rw_info (0, 0, __LINE__, "std::ios::hex");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (CHAR_MAX),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (UCHAR_MAX), hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SCHAR_MAX), hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SHRT_MAX),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (USHRT_MAX), hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (INT_MAX),   hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (UINT_MAX),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MAX),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T, ULong (CHAR_MIN),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SCHAR_MIN), hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (SHRT_MIN),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (INT_MIN),   hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (LONG_MIN),  hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");

    TEST (T,               1UL, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,              ~0UL, hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T, ULong (ULONG_MAX), hex, 0, 0, ' ', "", "%lx");
    std_log(LOG_FILENAME_LINE,"Test end");

}


/**************************************************************************/

template <class charT>
void llong_test (charT, const char *cname)
{
#ifdef _RWSTD_LONG_LONG

    const char* const tname = "long long";

    rw_info (0, 0, 0, "std::num_put<%s>::put (..., %s)", cname, tname);

#  define STDIO_FMAT   "%" _RWSTD_LLONG_PRINTF_PREFIX "d"

#  ifndef _MSC_VER
#    define LL(number)   number ## LL
#  else   // if defined (_MSC_VER)
     // MSVC 7.0 doesn't recognize the LL suffix
#    define LL(number)   number ## I64
#  endif   // _MSC_VER
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (T,          LL (0), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,          LL (1), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,         LL (21), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,        LL (321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,       LL (4321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,      LL (54321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,     LL (654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,    LL (7654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,   LL (87654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  LL (987654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T,         ~LL (0), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,         -LL (1), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,        -LL (21), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,       -LL (321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,      -LL (4321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,     -LL (54321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,    -LL (654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,   -LL (7654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  -LL (87654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -LL (987654321), dec, 0, 0, ' ', "", STDIO_FMAT);
    std_log(LOG_FILENAME_LINE,"Test end");


#undef FLAGS
#define FLAGS   hex | showbase | internal

#undef OPTS
#define OPTS   FLAGS, 0, 20, ' ', "\1\2\3\4\1\2\3\4"

    // internal justfication, hex format, grouping
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0),                  OPTS,     "                   0");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x1),                OPTS,     "0x                 1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x12),               OPTS,     "0x               1,2");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123),              OPTS,     "0x              12,3");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x1234),             OPTS,     "0x            1,23,4");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x12345),            OPTS,     "0x           12,34,5");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456),           OPTS,     "0x          123,45,6");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x1234567),          OPTS,     "0x        1,234,56,7");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x12345678),         OPTS,     "0x       12,345,67,8");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789),        OPTS,     "0x      123,456,78,9");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789a),       OPTS,     "0x     1234,567,89,a");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789ab),      OPTS,     "0x   1,2345,678,9a,b");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789abc),     OPTS,     "0x 1,2,3456,789,ab,c");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789abcd),    OPTS,     "0x12,3,4567,89a,bc,d");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789abcde),   OPTS,   "0x1,23,4,5678,9ab,cd,e");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789abcdef),  OPTS,  "0x12,34,5,6789,abc,de,f");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LL (0x123456789abcdef0), OPTS, "0x123,45,6,789a,bcd,ef,0");
    std_log(LOG_FILENAME_LINE,"Test end");


#endif   // _RWSTD_LONG_LONG
}


/**************************************************************************/

template <class charT>
void ullong_test (charT, const char *cname)
{
#ifdef _RWSTD_LONG_LONG

    const char* const tname = "unsigned long long";

    rw_info (0, 0, 0, "std::num_put<%s>::put (..., %s)", cname, tname);

#endif   // _RWSTD_LONG_LONG
}


/**************************************************************************/

template <class charT>
void dbl_test (charT, const char *cname)
{
    const char* const tname = "double";

    rw_info (0, 0, 0, "std::num_put<%s>::put (..., %s)", cname, tname);

    Punct<charT>::decimal_point_ = '.';

    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T,  0.0, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -0.0, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  1.0, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  1.0, 0, 0, 0, ' ', "", "1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -1.0, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -1.0, 0, 0, 0, ' ', "", "-1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  1.1, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,  1.1, 0, 0, 0, ' ', "", "1.1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -1.1, 0, 0, 0, ' ', "", "%g");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, -1.1, 0, 0, 0, ' ', "", "-1.1");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    


    // exercise formatting of very large numbers in a fixed notation
    // using '+' as the fill character to disable num_get tests
    TEST (T, 1.0e+128, fixed, 0, 0, '+', "", "%.0f", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 1.0e+256, fixed, 0, 0, '+', "", "%.0f", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");



#undef CAT
#undef CONCAT

#define CAT(a, b)      CONCAT (a, b)
#define CONCAT(a, b)   a ## b

    const double d = CAT (1.0e+, _RWSTD_DBL_MAX_10_EXP);
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, d, fixed, 0, 0, '+', "", "%.0f", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");    


    {
        char stdiofmt [8];
        const std::streamsize prec = std::streamsize (DBL_DIG + 3);
        std::sprintf (stdiofmt, "%%.%dg", int (prec));
        std_log(LOG_FILENAME_LINE,"Test start");    
        TEST (T, DBL_MIN, 0, prec, 0, ' ', "", stdiofmt);
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
        TEST (T, DBL_MAX, 0, prec, 0, ' ', "", stdiofmt);
    std_log(LOG_FILENAME_LINE,"Test end");

    }

    if (1) {
#if 0    
        // verify that the global LC_NUMERIC setting has no impact on the facet
        for (const char *name = rw_locales (LC_NUMERIC, 0); *name;
             name += std::strlen (name) + 1) {

            // find the first locale whose decimal_point character
            // is different than in the classic C locale (i.e., than '.')
            if (0 == std::setlocale (LC_NUMERIC, name))
                continue;

            const std::lconv* const conv = std::localeconv ();

            if (!conv)
                continue;

            if (conv->decimal_point && '.' != *conv->decimal_point)
                break;
        }
#endif

        Punct<charT>::decimal_point_ = '*';
        Punct<charT>::thousands_sep_ = '/';

    std_log(LOG_FILENAME_LINE,"Test start");    

        TEST (T, 12345.678900, fixed, 6, 0, ' ', "\000", "12345*678900");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
        TEST (T, 123456.78900, fixed, 5, 0, ' ', "\002", "12/34/56*78900");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    
        TEST (T, 1234567.8900, fixed, 4, 0, ' ', "\003", "1/234/567*8900");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");    

        Punct<charT>::decimal_point_ = '.';

        TEST (T, 12345678.900, fixed, 3, 0, ' ', "\004", "1234/5678.900");
    std_log(LOG_FILENAME_LINE,"Test end");


        // reset the global locale
        std::setlocale (LC_NUMERIC, "C");
    }
}


/**************************************************************************/

template <class charT>
void ldbl_test (charT, const char *cname)
{
#ifndef _RWSTD_NO_LONG_DOUBLE

    const char* const tname = "long double";

    rw_info (0, 0, 0, "std::num_put<%s>::put (..., %s) "
             "[sizeof (long double) = %u]",
             cname, tname, sizeof (long double));

    typedef long double LDbl;

    Punct<charT>::decimal_point_ = '.';
    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,     0.0L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,     1.0L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,     2.1L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,    -3.2L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T,    -4.3L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, 1.0e+10L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 2.0e+20L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 4.0e+30L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, 1.0e-10L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 2.0e-20L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 4.0e-30L, 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, LDbl (CHAR_MAX),  0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (UCHAR_MAX), 0, 0, 0, ' ', "", "%Lg");    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, LDbl (SCHAR_MAX), 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (SHRT_MAX),  0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (USHRT_MAX), 0, 0, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    // specify greater precision than the default 6 for large numbers
    TEST (T, LDbl (INT_MAX),   0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (UINT_MAX),  0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, LDbl (LONG_MAX),  0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (ULONG_MAX), 0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, LDbl (FLT_MIN),   0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (FLT_MAX),   0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (DBL_MIN),   0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, LDbl (DBL_MAX),   0, 32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    {
        char stdiofmt [8];
        const std::streamsize prec = std::streamsize (LDBL_DIG + 3);
        std::sprintf (stdiofmt, "%%.%d" _RWSTD_LDBL_PRINTF_PREFIX "g",
                      int (prec));

        TEST (T, LDbl (LDBL_MIN), 0, prec, 0, ' ', "", stdiofmt);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
        TEST (T, LDbl (LDBL_MAX), 0, prec, 0, ' ', "", stdiofmt);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    }

    const long double Pi = 3.14159265358979323846L;

    // some test cases below that use precision in conjuction with
    // scientific in the libc format specifier in order to exercise
    // the proposed resolution of lwg issue 231
    TEST (T, Pi, 0,          32, 0, ' ', "", "%.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed,       0, 0, ' ', "", "%.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific,  0, 0, ' ', "", "%.0Le", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed,       0, 0, ' ', "", "3",     Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific,  0, 0, ' ', "", "3e+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, Pi, uppercase,              32, 0, ' ', "", "%.32LG");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, uppercase | fixed,       0, 0, ' ', "", "%.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, uppercase | scientific,  0, 0, ' ', "", "%.0LE", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, uppercase | scientific,  0, 0, ' ', "", "3E+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define Showpos(f)   (showpos | f)

    TEST (T, Pi, Showpos (0),          32, 0, ' ', "", "%+.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpos (fixed),       0, 0, ' ', "", "%+.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpos (scientific),  0, 0, ' ', "", "%+.0Le", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpos (fixed),       0, 0, ' ', "", "+3",     Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpos (scientific),  0, 0, ' ', "", "+3e+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define SHOWPOS(f)   (showpos | uppercase | f)

    TEST (T, Pi, SHOWPOS (0),          32, 0, ' ', "", "%+.32LG");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOS (fixed),       0, 0, ' ', "", "%+.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOS (scientific),  0, 0, ' ', "", "%+.0LE", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOS (fixed),       0, 0, ' ', "", "+3",     Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOS (scientific),  0, 0, ' ', "", "+3E+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define Showpoint(f)   (showpoint | f)

    TEST (T, Pi, Showpoint (0),          32, 0, ' ', "", "%#.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpoint (fixed),       0, 0, ' ', "", "%#.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpoint (scientific),  0, 0, ' ', "", "%#.0Le", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpoint (fixed),       0, 0, ' ', "", "3.",     Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showpoint (scientific),  0, 0, ' ', "", "3.e+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define SHOWPOINT(f)   (showpoint | uppercase | f)

    TEST (T, Pi, SHOWPOINT (0),          32, 0, ' ', "", "%#.32LG");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOINT (fixed),       0, 0, ' ', "", "%#.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOINT (scientific),  0, 0, ' ', "", "%#.0LE", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWPOINT (scientific),  0, 0, ' ', "", "3.E+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define Showall(f)   (showpoint | showpos | f)

    TEST (T, Pi, Showall (0),          32, 0, ' ', "", "%#+.32Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showall (fixed),       0, 0, ' ', "", "%#+.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showall (scientific),  0, 0, ' ', "", "%#+.0Le", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showall (fixed),       0, 0, ' ', "", "+3.",     Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, Showall (scientific),  0, 0, ' ', "", "+3.e+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

#define SHOWALL(f)   (showpoint | showpos | uppercase | f)

    TEST (T, Pi, SHOWALL (0),          32, 0, ' ', "", "%#+.32LG");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWALL (fixed),       0, 0, ' ', "", "%#+.0Lf", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWALL (scientific),  0, 0, ' ', "", "%#+.0LE", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, SHOWALL (scientific),  0, 0, ' ', "", "+3.E+00", Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    // with {g,G}, precision indicates the number of significant digits
    TEST (T, Pi, 0,  1, 0, ' ', "", "%.1Lg",  Eof, 3.0L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, 0,  2, 0, ' ', "", "%.2Lg",  Eof, 3.1L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, 0,  3, 0, ' ', "", "%.3Lg",  Eof, 3.14L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, 0, 10, 0, ' ', "", "%.10Lg", Eof, 3.141592654L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    // C89 and C99 both specify that (only if specified using the asterisk)
    // negative precision is treated the same as no precision at all; verify
    // that num_put handles negative precision gracefully (i.e., ignores it)
    TEST (T, Pi, 0, -1, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, 0, -9, 0, ' ', "", "%Lg");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    // with {e,E,f,F}, precision indicates the number of fractional digits
    TEST (T, Pi, fixed,  1, 0, ' ', "", "%.1Lf",  Eof, 3.1L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed,  2, 0, ' ', "", "%.2Lf",  Eof, 3.14L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed,  3, 0, ' ', "", "%.3Lf",  Eof, 3.142L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed, 10, 0, ' ', "", "%.10Lf", Eof, 3.1415926536L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    

    // exercise formatting of very large numbers in a fixed notation
    // using '-' as the fill character to disable num_get tests
    TEST (T, 2.0e+128L, fixed, 0, 0, '-', "", "%.0Lf", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 3.0e+256L, fixed, 0, 0, '-', "", "%.0Lf", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 4.0e+300L, fixed, 0, 0, '-', "", "%.0Lf", Eof);

    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    // extension: fixed format and negative precision
    TEST (T, Pi, fixed, -2, 0, ' ', "", "0.03");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, fixed, -8, 0, ' ', "", "0.00000003");
    std_log(LOG_FILENAME_LINE,"Test end");    


#undef CAT
#undef CONCAT

#define CAT(a, b)      CONCAT (a, b)
#define CONCAT(a, b)   a ## b

    const long double ld = CAT (CAT (1.0e+, _RWSTD_LDBL_MAX_10_EXP), L);

    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, ld, fixed, 0, 0, '-', "", "%.0Lf", Eof);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, Pi, scientific,  2, 0, ' ', "", "%.2Le",  Eof, 3.14L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific,  4, 0, ' ', "", "%.4Le",  Eof, 3.1416L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific,  6, 0, ' ', "", "%.6Le",  Eof, 3.141593L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific, 12, 0, ' ', "", "%.12Le", Eof, 3.14159265359L);
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific, -3, 0, ' ', "", "%Le");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, Pi, scientific, -7, 0, ' ', "", "%Le");
    std_log(LOG_FILENAME_LINE,"Test end");    


#endif   // _RWSTD_NO_LONG_DOUBLE
}


/**************************************************************************/

template <class charT>
void pvoid_test (charT, const char *cname)
{
    const char* const tname = "const void*";

    rw_info (0, 0, 0, "std::num_put<%s>::put (..., %s)", cname, tname);

    typedef void* PVoid;

    int foo = 0;
    int bar = 0;

    // use "0" rather than "%p" for null pointers to avoid
    // having to deal with GNU libc's "(nil)" format

    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (     0),  0, 0, 0, ' ', "", "0");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (     1),  0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  &foo),  0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  &bar),  0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (   ~0L),  0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    // 22.2.2.2.2, p16 and lwg issue 282: grouping and thousands_sep
    // is only used in arithmetic types
    TEST (T, PVoid (     0),  0, 0, 0, ' ', "\1",   "0");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (     1),  0, 0, 0, ' ', "\1",   "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  &foo),  0, 0, 0, ' ', "\1\1", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  &bar),  0, 0, 0, ' ', "\1\2", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (   ~0L),  0, 0, 0, ' ', "\1\3", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, PVoid ( SHRT_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid ( SHRT_MIN), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (USHRT_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  INT_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (  INT_MIN), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid ( UINT_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid ( LONG_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid ( LONG_MIN), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (ULONG_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    


#if _RWSTD_LLONG_SIZE <= _RWSTD_PTR_SIZE
#ifndef __SYMBIAN32__

    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (_RWSTD_LLONG_MAX),  0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, PVoid (_RWSTD_ULLONG_MAX), 0, 0, 0, ' ', "", "%p");
    std_log(LOG_FILENAME_LINE,"Test end");    

#endif
#endif   // LLONG_MAX

}


/**************************************************************************/

template <class charT>
void errno_test (charT, const char *cname)
{
    const char* const tname = "long";

    rw_info (0, 0, 0,
             "std::num_put<%s>::put (..., %s) and errno", cname, tname);

    // verify that errno doesn't change after, or adversely affect
    // successful insertion; and that the insertion doesn't change
    // the value of errno

    errno = -1;

    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, 12345L,  0, 0, 0, ' ', "", "12345");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
  #ifndef __SYMBIAN32__
    rw_assert (-1 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", -1);
#else
  if(-1 !=errno)
  {
   failures++;
   std_log(LOG_FILENAME_LINE,"Reason:Failing");
   }
 #endif  
    errno = 0;

    std_log(LOG_FILENAME_LINE,"Test start");    

    TEST (T, 12346L,  0, 0, 0, ' ', "", "12346");
    std_log(LOG_FILENAME_LINE,"Test end");    
    std_log(LOG_FILENAME_LINE,"Test start");    
   #ifndef __SYMBIAN32__
    rw_assert (0 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", -0);
   #else
  if(0!=errno)
  {
   failures++;
   std_log(LOG_FILENAME_LINE,"Reason:Failing");
   }
 #endif  
    
    errno = 1;
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12347L,  0, 0, 0, ' ', "", "12347");
    std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (1 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", 1);
    #else
    if(1!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    errno = 2;
    
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12348L,  0, 0, 0, ' ', "", "12348");
    std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (2 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", 2);
    #else
    if(2!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    
    errno = 3;
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12349L,  0, 0, 0, ' ', "", "12349");
    std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (3 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", 3);
    #else
    if(3!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    
    errno = 4;
    std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12350L,  0, 0, 0, ' ', "", "12350");
        std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (4 == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", 4);
    #else
    if(4!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    
    errno = ERANGE;
    std_log(LOG_FILENAME_LINE,"Test start");     
    TEST (T, 12351L,  0, 0, 0, ' ', "", "12351");
    std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__ 
    rw_assert (ERANGE == errno, 0, __LINE__,
               "errno unexpectedly changed from %{#m} to %{#m} (%m)", ERANGE);
    #else
   if(ERANGE!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif

    errno = Bad;
      std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12352L,  0, 0, 0, ' ', "", "12352");
      std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (Bad == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", Bad);
    #else
    if(Bad != errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    
    errno = Eof;
      std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12353L,  0, 0, 0, ' ', "", "12353");
      std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (Eof == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", Eof);
    #else
    if(Eof!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif
    
    errno = Fail;
      std_log(LOG_FILENAME_LINE,"Test start");    
    TEST (T, 12354L,  0, 0, 0, ' ', "", "12354");
      std_log(LOG_FILENAME_LINE,"Test end");    
    #ifndef __SYMBIAN32__
    rw_assert (Fail == errno, 0, __LINE__,
               "errno unexpectedly changed from %d to %{#m} (%m)", Fail);
    #else
    if(Fail!= errno)
    { 
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #endif

    errno = 0;
}


/**************************************************************************/

// verify that std::ctype<>::widen() is called at least once
// for every distinct narow character (not for fill characters)
// note that an implementation is allowed to cache the results
// of facet virtual functions() called with the same arguments
// (including the implicit this pointer), so the numbers here
// are minimum required values but not exact or maximums
void widen_test ()
{
    rw_info (0, 0, 0,
             "std::num_put<char>::put (..., long) calls ctype<char>::widen()");

    static const struct {
        long            val;
        std::streamsize width;
        int             flags;
        int             n_widen;
        const char     *str;
    } data[] = {
        {         0,  0, 0,                  1, "9"          },
        {         1,  0, 0,                  1, "8"          },
        {        11,  0, 0,                  1, "88"         },
        {        12,  0, 0,                  2, "87"         },
        {       123,  0, 0,                  3, "876"        },
        {       111,  0, 0,                  1, "888"        },
        {     -1234,  0, 0,                  5, "-8765"      },
        {     12121,  0, 0,                  2, "87878"      },
        {     12345,  0, showpos,            6, "+87654"     },
        {   0123456,  0, oct | showbase,     7, "9876543"    },
        {  0x234567,  0, hex | showbase,     8, "9x765432"   },
        { 0x2345678, 10, hex | showbase,     9, " 9x7654321" },
        {         9,  5, showpos | internal, 2, "+   0"  }
    };

    for (unsigned i =  0; i != sizeof data / sizeof *data; ++i) {

        const Ctype<char> ctp;

        // construct and initialize a basic_ios-derived object
        Ios<char> ios;

        // construct a num_put-derived facet to exercise
        const NumPut<char> np;

        // imbue a stream object with a custom locale
        // containing the replacement ctype facet
        ios.imbue (std::locale (ios.getloc (), (const std::ctype<char>*)&ctp));

        ios.flags (std::ios_base::fmtflags (data [i].flags));
        ios.width (data [i].width);

        char buf [40];

        // reset function call counter
        Ctype<char>::n_widen_ = 0;

        *np.put (buf, ios, ' ', data [i].val) = '\0';

        #ifndef __SYMBIAN32__
        rw_assert (data [i].n_widen <= Ctype<char>::n_widen_, 0, __LINE__,
                   "%d. num_put<char>::do_put (..., %d) called "
                   "ctype<char>::do_widen() %d times, %d expected; "
                   "flags = %{If}, width = %d", i, data [i].val,
                   Ctype<char>::n_widen_, data [i].n_widen,
                   data [i].flags, data [i].width);

        rw_assert (0 == rw_strncmp (buf, data [i].str), 0, __LINE__,
                   "%d. num_put<char>::do_put (..., %d) produced %s, "
                   "expected \"%{#*s}\"; "
                   "flags = %{If}, width = %d",
                   i, data [i].val, buf,
                   int (sizeof data [i].str [0]), data [i].str,
                   data [i].flags, data [i].width);
         #else
         {
            if(!(data [i].n_widen <= Ctype<char>::n_widen_))
            {
             failures++;
             std_log(LOG_FILENAME_LINE,"Reason:Failing");
             }
          if(0 != rw_strncmp (buf, data [i].str))
            {
             failures++;
             std_log(LOG_FILENAME_LINE,"Reason:Failing");
             }
            
         }          
         #endif //__SYMBIAN32__
    }
}


/**************************************************************************/

// verify that std::numpunct<>::grouping() is called and used correctly
void grouping_test ()
{
    // construct a "replacement" numpunct-derived facet
    const Punct<char> pun (1);

    // construct and initialize a basic_ios-derived object
    Ios<char> ios;

    // construct a num_put-derived facet to exercise
    const NumPut<char> np;

    // imbue a stream object with a custom locale
    // containing the replacement punctuation facet
    ios.imbue (std::locale (ios.getloc (), (const std::numpunct<char>*)&pun));
  #ifndef __SYMBIAN32__
    rw_assert (1 == Punct<char>::n_objs_, 0, __LINE__,
               "%d facets exist, 1 expected", Punct<char>::n_objs_);
  #else
    if(1 != Punct<char>::n_objs_)
    {
     failures++;
     std_log(LOG_FILENAME_LINE,"Reason:Failing");
     }
  #endif
     
    // decimal output, no special formatting
    ios.setf (std::ios::fmtflags ());

    char buf [40];

    // group after every digit
    Punct<char>::grouping_ = "\1";

    // reset the do_thousands_sep()-call counter
    Punct<char>::n_thousands_sep_ = 0;

    *np.put (buf, ios, '\0', 123456789L) = '\0';

    // verify that the number was formatted correctly
   #ifdef __SYMBIAN32__
   
    if(0 != strcmp (buf, "1,2,3,4,5,6,7,8,9"))
    {
        failures++;
        std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    
    if(!(0 < Punct<char>::n_thousands_sep_))
    {
       failures++;
       std_log(LOG_FILENAME_LINE,"Reason:Failing");
    }
    #else
    rw_assert (0 == std::strcmp (buf, "1,2,3,4,5,6,7,8,9"), 0, __LINE__,
               "num_put<char>::do_put (..., 123456789) produced \"%s\", "
               "expected \"%s\"", buf, "1,2,3,4,5,6,7,8,9");
    rw_assert (0 < Punct<char>::n_thousands_sep_, 0, __LINE__,
               "num_put<char>::do_put (..., 123456789) failed to call "
               "numpunct<char>::do_thousands_sep()");
  #endif
    // verify that do_thousands_sep() was called at least once

    // repeat test to verify that distinct instances
    // of the same facet do not cache each other's data
    {
        // nested scope works around a SunPro bug (PR #27543)
        ios.imbue (std::locale (ios.getloc (),
                       (const std::numpunct<char>*)new Punct<char>(0)));
    }
   #ifndef __SYMBIAN32__
    rw_assert (2 == Punct<char>::n_objs_, 0, __LINE__,
               "%d facets exist, 2 expected", Punct<char>::n_objs_);
   #else
    if(2 != Punct<char>::n_objs_)
    {
     failures++;
     std_log(LOG_FILENAME_LINE,"Reason:Failing");
     }
     
   #endif //__SYMBIAN32__
    // now group after every other digit
    Punct<char>::grouping_ = "\2";

    // reset the do_thousands_sep()-call counter
    Punct<char>::n_thousands_sep_ = 0;

    *np.put (buf, ios, '\0', 123456790L) = '\0';

    // again verify that the number was formatted correctly
  #ifdef __SYMBIAN32__
  if(0 != strcmp (buf, "1,23,45,67,90"))
    { 
      failures++;
      std_log(LOG_FILENAME_LINE,"Reason:Failing");
       
    }           
  
  if(!(0 < Punct<char>::n_thousands_sep_))
  {
   failures++;
   std_log(LOG_FILENAME_LINE,"Reason:Failing")  ;
   }
  #else
    rw_assert (0 == std::strcmp (buf, "1,23,45,67,90"), 0, __LINE__,
               "num_put<char>::do_put (..., 123456790) produced \"%s\", "
               "expected \"%s\"", buf, "1,23,45,67,90");
    rw_assert (0 < Punct<char>::n_thousands_sep_, 0, __LINE__,
               "num_put<char>::do_put (..., 123456790) failed to call "
               "numpunct<char>::do_thousands_sep()");
           
  #endif 
    // verify that do_thousands_sep() was called at least once
    // (i.e., that there is no unwarranted caching going on)
    
    // replace imbued locale with a copy of the classic locale
    // and remove the previously installed locale containing
    // the user-defined facet; this must destroy the dynamically
    // created facet leaving only the local facet on the stack
    {
        // nested scope works around a SunPro bug (PR #27543)
        ios.imbue (std::locale::classic ());
    }
#ifndef __SYMBIAN32__
    rw_assert (1 == Punct<char>::n_objs_, 0, __LINE__,
               "%d facets exist, 1 expected", Punct<char>::n_objs_);
 #else
   if(1 != Punct<char>::n_objs_)
   {
    failures++;
    std_log(LOG_FILENAME_LINE,"Reason:Failing");
    
   }
 #endif              
}

/**************************************************************************/

template <class charT>
void numput_virtuals_test (charT, const char *cname)
{
    const NumPut<charT> put (1);
    const std::num_put<charT, charT*> &np = put;

    Ios<charT> ios;

    ios.flags (std::ios_base::fmtflags ());


#define Abs(i)   ((i) < 0 ? (-i) : (i))

#ifdef __ARMCC__
#pragma diag_suppress 47
#endif

#define ASSERT(T, N, T2, tname, t2name)                              \
    do {                                                             \
        /* number of expected calls to the do_put() member */        \
        const int expect   =  N + put.ncalls_ [put.test_ ## T];      \
        const int expect_2 = !N + put.ncalls_ [put.test_ ## T2];     \
        charT buf [80];                                              \
        const charT* const end  = np.put (buf, ios, charT (), (T)0); \
        rw_assert (end == buf + 1, 0, __LINE__,                      \
                   "line %d: num_put<%s>::put (..., %s = 0) "        \
                   "return value", __LINE__, cname, tname);          \
        const int ncalls   = put.ncalls_ [put.test_ ## T];           \
        const int ncalls_2 = put.ncalls_ [put.test_ ## T2];          \
        rw_assert (expect == ncalls, 0, __LINE__,                    \
                   "line %d: num_put<%s>::put (..., %s) called "     \
                   "do_put(..., %s) %d times, expected %d",          \
                    __LINE__, cname, tname,                          \
                   tname, Abs (ncalls - expect - N), N);             \
        /* make sure extensions, if any, are implemented in */       \
        /* terms of and call the standard virtual functions */       \
        /* bogus addressof operator prevents warnings about */       \
        /* unreachable code when T is the same as T2        */       \
        if (put.test_ ## T != put.test_ ## T2)                       \
            rw_assert (expect_2 == ncalls_2, 0, __LINE__,            \
                       "line %d: num_put<%s>::put (..., %s) called " \
                       "do_put(..., %s) %d times, expected %d",      \
                       __LINE__, cname, tname,                       \
                       t2name, Abs (expect_2 - ncalls_2 - !N), !N);  \
    } while (0)

    typedef unsigned short ushort;
    typedef unsigned int   uint;
    typedef unsigned long  ulong;

    // verify that each public member function calls each corresponding
    // implementation do_xxx() member functions exactly once

    // in the case of extensions, verify that each extended public member
    // function calls the appropriate implementation standard do_xxx()
    // member function

#ifndef _RWSTD_NO_BOOL
    ASSERT (bool, 1, bool, "bool", "bool");
#endif   // _RWSTD_NO_BOOL

    ASSERT (long,  1, long, "long", "long");
    ASSERT (ulong, 1, ulong, "unsigned long", "unsigned long");

#if defined (_RWSTD_LONG_LONG) && !defined (_RWSTD_NO_EXT_NUM_PUT)

    typedef          _RWSTD_LONG_LONG  llong;
    typedef unsigned _RWSTD_LONG_LONG ullong;

    ASSERT (llong,  1, llong, "long long", "long long");
    ASSERT (ullong, 1, ullong, "unsigned long long", "unsigned long long");

#endif   // _RWSTD_LONG_LONG   && !_RWSTD_NO_EXT_NUM_PUT

    ASSERT (double, 1, double, "double", "double");

#ifndef _RWSTD_NO_LONG_DOUBLE

    typedef long double ldouble;
#ifndef __SYMBIAN32__
    ASSERT (ldouble, 1, ldouble, "long double", "long double");
#endif
#endif   // _RWSTD_NO_LONG_DOUBLE

    typedef const void* pvoid;
    ASSERT (pvoid, 1, pvoid, "const void*", "const void*");
}

/**************************************************************************/

template <class charT>
void run_tests (charT, const char *cname)
{
    numput_virtuals_test (charT (), cname);
#if 0 //FIX
    direct_use_test (charT (), cname);
#endif //#if 0

#undef TEST
#define TEST(T, tname)                                          \
    if (rw_enabled (#T))                                        \
        T ## _test (charT (), cname);                           \
    else                                                        \
        rw_note (0, __FILE__, __LINE__, "%s test disabled",     \
                 tname && *tname ? tname : #T)

#ifndef _RWSTD_NO_BOOL

   std_log(LOG_FILENAME_LINE,"Test start");
    TEST (bool, "");
std_log(LOG_FILENAME_LINE,"Test end");
#endif   // _RWSTD_NO_BOOL

    
    // NOTE: there is no num_put::put(..., short)
    // TEST (shrt, "short");
    // TEST (ushrt, "unsigned short");

    // NOTE: there is no num_put::put(..., int)
    // TEST (int, "");
    // TEST (uint, "unsigned int");
std_log(LOG_FILENAME_LINE,"Test start");
    TEST (long, "");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (ulong, "unsigned long");
std_log(LOG_FILENAME_LINE,"Test end");
#ifndef _RWSTD_NO_LONG_LONG
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (llong, "long long");
    std_log(LOG_FILENAME_LINE,"Test end");
    std_log(LOG_FILENAME_LINE,"Test start");
    TEST (ullong, "unsigned long long");
    std_log(LOG_FILENAME_LINE,"Test end");
#endif   // _RWSTD_NO_LONG_LONG

    // NOTE: there is no num_put::put(..., float)
    // TEST (flt, "float");
std_log(LOG_FILENAME_LINE,"Test start");
    TEST (dbl, "dbouble");
std_log(LOG_FILENAME_LINE,"Test end");
#if 0 // no long double support
#ifndef _RWSTD_NO_LONG_DOUBLE

    TEST (ldbl, "long double");

#endif   // _RWSTD_NO_LONG_DOUBLE
#endif //#if 0
std_log(LOG_FILENAME_LINE,"Test start");
    TEST (pvoid, "void*");
std_log(LOG_FILENAME_LINE,"Test end");
    if (rw_opt_no_errno)
        rw_note (0, __FILE__, __LINE__, "errno test disabled");
    else
        errno_test (charT (), cname);
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    if (rw_opt_no_widen) {
        rw_note (0, 0, 0, "widen test disabled");
    }
    else {
       // widen_test ();
    }

    if (rw_opt_no_grouping) {
        rw_note (0, 0, 0, "grouping test disabled");
    }
    else {
        grouping_test ();
    }

    if (rw_enabled ("char"))
        run_tests (char (), "char");
    else
        rw_note (0, __FILE__, __LINE__, "char test disabled");

#ifndef _RWSTD_NO_WCHAR_T

    if (rw_enabled ("wchar_t"))
        run_tests (wchar_t (), "wchar_t");
    else
        rw_note (0, __FILE__, __LINE__, "wchar_t test disabled");

#endif   // _RWSTD_NO_WCHAR_T

    return 0;
}

/**************************************************************************/
//#endif // #if 0
int main (int argc, char *argv[])
{
    #ifndef __SYMBIAN32__
    return rw_test (argc, argv, __FILE__,"lib.locale.num.put",0 , run_test,"|-enable-num_get# ""|-no-errno# "
	    "|-no-grouping# ""|-no-widen# ",&rw_opt_enable_num_get,&rw_opt_no_errno,&rw_opt_no_grouping,&rw_opt_no_widen);
    #else
   rw_test (argc, argv, __FILE__,"lib.locale.num.put",0 , run_test,"|-enable-num_get# ""|-no-errno# "
	    "|-no-grouping# ""|-no-widen# ",&rw_opt_enable_num_get,&rw_opt_no_errno,&rw_opt_no_grouping,&rw_opt_no_widen);
    
    if(failures)  
          {
            assert_failed = true;
       std_log(LOG_FILENAME_LINE,"Result: Failed");
      }
     else
      {

       std_log(LOG_FILENAME_LINE,"Result: Passed");
      }
  
   std_log(LOG_FILENAME_LINE,"[End Test Case]");  
   
   testResultXml("22_locale_num_put");
    close_log_file();

    return 0;
    #endif
}
