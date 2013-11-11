

 /***************************************************************************
 *
 * 25.find.cpp - test exercising 25.1.2 [lib.alg.find]
 *
 * $Id: 25.find.cpp 354154 2005-12-05 20:10:23Z sebor $
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

#include <algorithm>    // for find()
#include <cstddef>      // for size_t

#include <alg_test.h>   
#include <driver.h>     // for rw_test()
#include"std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
int failures=0;
/**************************************************************************/

_RWSTD_NAMESPACE (std) { 

#ifndef _RWSTD_NO_EXPLICIT_INSTANTIATION

template
InputIter<eq_comp<base<> > > 
find (InputIter<eq_comp<base<> > >, InputIter<eq_comp<base<> > >, 
      const eq_comp<base<> >&);

template
InputIter<eq_comp<base<> > > 
find_if (InputIter<eq_comp<base<> > >, InputIter<eq_comp<base<> > >, 
         predicate<eq_comp<base<> > >);

#endif // _RWSTD_NO_EXPLICIT_INSTANTIATION

}   // namespace std


/**************************************************************************/

template <class T>
struct Predicate
{
    static std::size_t funcalls_;

    Predicate (const T &val, int /* dummy */)
        : val_ (val) {
        funcalls_ = 0;
    }

    class ConvertibleToBool {
        bool result_;
    public:
        ConvertibleToBool (bool res): result_ (res) { /* empty */ }
        operator bool() const { return result_; }
    };

    // return a type other than bool but one that is implicitly
    // convertible to bool to detect incorrect assumptions
    ConvertibleToBool operator() (T obj) /* not const */ {
        ++funcalls_;
        return ConvertibleToBool (obj == val_);
    }

private:
    void operator= (Predicate&);
    const T &val_;
};

template <class T>
std::size_t Predicate<T>::funcalls_;

/**************************************************************************/

// used to initialize an array of objects of type T
static const char *tinit_begin;

int tinit ()
{
    typedef unsigned char UChar;

    return UChar (*tinit_begin++);
}

/**************************************************************************/

template <class InputIterator, class Predicate, class T>
void test_find (int           line,     // line number of test case
                const char   *src,      // source sequence
                std::size_t   findoff,  // offset of the element to find
                InputIterator dummy_iter,
                T*,
                Predicate    *pred)
{
    static const char* const itname = type_name (dummy_iter, (T*)0);
    static const char* const pname  = pred ? "Predicate" : "operator==";
    #ifndef __SYMBIAN32__
    const std::size_t nsrc = std::strlen (src);
    #else
    const std::size_t nsrc = strlen (src);
    #endif
    // normalize offset
    if (std::size_t (-1) == findoff)
        findoff = nsrc;

    // have the T default ctor initialize objects from `src'
    tinit_begin = src;
    #ifndef __SYMBIAN32__
    T::gen_     = tinit;
    #else
    gen_     = tinit;
    #endif

    T* const tsrc = new T [nsrc];

    T* const src_begin = tsrc;
    T* const src_end   = tsrc + nsrc;

    const InputIterator first =
        make_iter (src_begin, src_begin, src_end, dummy_iter);

    const InputIterator last =
        make_iter (src_end, src_begin, src_end, dummy_iter);

    // get a reference to the object to find
    // or construct a temporary if no such object exists
    const T &to_find = findoff < nsrc ? tsrc [findoff] : T ();

    // construct a predicate object to use with find_if
    const Predicate fun (to_find, 0 /* dummy */);

    // reset the operator==() counter
    #ifndef __SYMBIAN32__
    T::n_total_op_eq_ = 0;
    #else
    n_total_op_eq_ = 0;
    #endif
   

    // invoke find() or find_if(), depending on the predicate flag
    const InputIterator res = pred ?std::find_if (first, last, fun) : std::find (first, last, to_find);

    // silence a bogus EDG eccp remark #550-D:
    // variable "res" was set but never used
  //  _RWSTD_UNUSED (res);
    // verify 25.1.5 p2
   
    if(!(res.cur_ == first.cur_ + findoff))
     {
       failures++;
        std_log(LOG_FILENAME_LINE,"Reason:Failing, Returned iterator is not as expected");   
      }   
 
  
    
    if (!rw_assert (res.cur_ == first.cur_ + findoff, 0, line,
                    "line %d: find%{?}_if%{;} (%s = \"%s\", ..., '%c')"
                    " == (it + %zu), got (it + %td)",
                    __LINE__, pred != 0, itname, src, to_find.val_,
                    findoff, res.cur_ - first.cur_)) {
        delete[] tsrc;
        return;
    }

    // verify 25.1.5 p3
    // Complexity: At most last - first applications
    //             of the corresponding predicate.
    // The complexity when find is successful is actually
    // (res - first) applications of the corresponding predicate.
    
    #ifndef __SYMBIAN32__
    const std::size_t npreds = pred ? fun.funcalls_ : T::n_total_op_eq_;
    #else
    const std::size_t npreds = pred ? fun.funcalls_ : n_total_op_eq_;
    #endif
    
    if(!(npreds <= findoff + 1))
     {
       failures++;
        std_log(LOG_FILENAME_LINE,"Reason: Failing");   
      } 
    
    rw_assert (npreds <= findoff + 1, 0, line,
               "line %d: find%{?}_if%{;} (%s = \"%s\", ..., '%c') "
               "invoked %s %zu times, expected no more than %zu",
               __LINE__, pred != 0, itname, src, to_find.val_,
               pname, npreds, findoff + 1);

    delete[] tsrc;
}

/**************************************************************************/

template <class InputIterator, class T, class Predicate>
void test_find (InputIterator dummy_iter, const T*, const Predicate* pred)
{   
    static const char* const itname = type_name (dummy_iter, (T*)0);

    rw_info (0, 0, 0, "std::find%{?}_if%{;} (%s, %1$s, "
             "%{?}Predicate%{:}const X&%{;})",
             0 != pred, itname, 0 != pred);

#define TEST(src, off_find)                            \
    test_find (__LINE__, src, std::size_t (off_find),  \
               dummy_iter, (X*)0, pred)

    //    +------------------ subject sequence
    //    |               +--- offset of the value to find (-1 for none)
    //    |               |    also the offset of the returned iterator
    //    |               |
    //    |               |
    //    v               v
    TEST ("",            -1);
    if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
      
    TEST ("a",            0);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("aa",           0);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("ab",           1);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("aaa",          0);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abb",          1);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abc",          2);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcc",         2);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcd",         3);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcde",        4);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdef",       5);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefg",      6);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefgh",     7);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefghi",    8);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefghij",   9);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefghijk", 10);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
    TEST ("abcdefghijk", -1);
     if(failures)
     {
        std_log(LOG_FILENAME_LINE,"Reason : Failing ");           
     }
}

/**************************************************************************/

/* extern */ int rw_opt_no_input_iter;   // --no-InputIterator
/* extern */ int rw_opt_no_fwd_iter;     // --no-ForwardIterator
/* extern */ int rw_opt_no_bidir_iter;   // --no-BidirectionalIterator
/* extern */ int rw_opt_no_rnd_iter;     // --no-RandomAccessIterator
/* extern */ int rw_opt_no_predicate;    // --no-Predicate

static void
test_find (bool test_predicate)
{
    rw_info (0, 0, 0, 
             "template <class %s, class %s> "
             "%1$s std::find%{?}_if%{;} (%1$s, %1$s, "
             "%{?}%2$s%{:}const %2$s&%{;})",
             "InputIterator", test_predicate ? "Predicate" : "T",
             test_predicate);

    const Predicate<X>* const pred = test_predicate ?
        (Predicate<X>*)1 : (Predicate<X>*)0;

    if (rw_opt_no_input_iter) {
        rw_note (0, __FILE__, __LINE__, "InputIterator test disabled");
    }
    else {
        test_find (InputIter<X>(0, 0, 0), (X*)0, pred);
    }

    if (rw_opt_no_fwd_iter) {
        rw_note (0, __FILE__, __LINE__, "ForwardIterator test disabled");
    }
    else {
        test_find (ConstFwdIter<X>(), (X*)0, pred);
        test_find (FwdIter<X>(), (X*)0, pred);
    }

    if (rw_opt_no_bidir_iter) {
        rw_note (0, __FILE__, __LINE__, "BidirectionalIterator test disabled");
    }
    else {
        test_find (ConstBidirIter<X>(), (X*)0, pred);
        test_find (BidirIter<X>(), (X*)0, pred);
    }

    if (rw_opt_no_rnd_iter) {
        rw_note (0, __FILE__, __LINE__, "RandomAccessIterator test disabled");
    }
    else {
        test_find (ConstRandomAccessIter<X>(), (X*)0, pred);
        test_find (RandomAccessIter<X>(), (X*)0, pred);
    }
}

/**************************************************************************/

static int
run_test (int, char*[])
{
    test_find (false);

    if (rw_opt_no_predicate) {
        rw_note (0, __FILE__, __LINE__, "Predicate test disabled");
    }
    else {
        test_find (true);
    }
                
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
   	  std_log(LOG_FILENAME_LINE,"[Test Case for find]");
    #ifndef __SYMBIAN32__
    return rw_test (argc, argv, __FILE__,
                    "lib.alg.find",
                    0 /* no comment */, run_test,
                    "|-no-InputIterator#"
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate#",
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
   #else
        rw_test (argc, argv, __FILE__,
                    "lib.alg.find",
                    0 /* no comment */, run_test,
                    "|-no-InputIterator#"
                    "|-no-ForwardIterator#"
                    "|-no-BidirectionalIterator#"
                    "|-no-RandomAccessIterator#"
                    "|-no-Predicate#",
                    &rw_opt_no_input_iter,
                    &rw_opt_no_fwd_iter,
                    &rw_opt_no_bidir_iter,
                    &rw_opt_no_rnd_iter,
                    &rw_opt_no_predicate);
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
   
   testResultXml("25_find");
    close_log_file();
 #endif

}
