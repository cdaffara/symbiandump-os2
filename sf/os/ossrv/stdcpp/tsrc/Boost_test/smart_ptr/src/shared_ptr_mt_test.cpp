/*
Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* Description:
*
*/


#include <boost/config.hpp>

#if defined(BOOST_MSVC)
#pragma warning(disable: 4786)  // identifier truncated in debug info
#pragma warning(disable: 4710)  // function not inlined
#pragma warning(disable: 4711)  // function selected for automatic inline expansion
#pragma warning(disable: 4514)  // unreferenced inline removed
#endif

//
//  shared_ptr_mt_test.cpp - tests shared_ptr with multiple threads
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include <vector>
#include <memory>
#include <stdexcept>

#include <cstdio>
#include <ctime>
#include <string>

#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

// 'portable' thread framework

class abstract_thread
{
public:

    virtual ~abstract_thread() {}
    virtual void run() = 0;
};

#if !defined(BOOST_HAS_PTHREADS) && defined(BOOST_HAS_WINTHREADS)

char const * title = "Using Windows threads";

#include <windows.h>
#include <process.h>

typedef HANDLE pthread_t;

unsigned __stdcall common_thread_routine(void * pv)
{
    abstract_thread * pt = static_cast<abstract_thread *>(pv);
    pt->run();
    delete pt;
    return 0;
}

int pthread_create(pthread_t * thread, void const *, unsigned (__stdcall * start_routine) (void*), void* arg)
{
    HANDLE h = (HANDLE)_beginthreadex(0, 0, start_routine, arg, 0, 0);

    if(h != 0)
    {
        *thread = h;
        return 0;
    }
    else
    {
        return 1; // return errno;
    }
}

int pthread_join(pthread_t thread, void ** /*value_ptr*/)
{
    ::WaitForSingleObject(thread, INFINITE);
    ::CloseHandle(thread);
    return 0;
}

#else

char const * title = "Using POSIX threads";

#include <pthread.h>

extern "C" void * common_thread_routine(void * pv)
{
    abstract_thread * pt = static_cast<abstract_thread *>(pv);
    pt->run();
    delete pt;
    return 0;
}

#endif

//

template<class F> class thread: public abstract_thread
{
public:

    explicit thread(F f): f_(f)
    {
    }

    void run()
    {
        f_();
    }

private:

    F f_;
};

template<class F> pthread_t createThread(F f)
{
    std::auto_ptr<abstract_thread> p(new thread<F>(f));

    pthread_t r;

    if(pthread_create(&r, 0, common_thread_routine, p.get()) == 0)
    {
        p.release();
        return r;
    }

	throw std::runtime_error("createThread failed.");
}

//
#ifdef __SYMBIAN32__
int const n = 1024;
#else
int const n = 1024 * 1024;
#endif

void test(boost::shared_ptr<int> const & pi)
{
    std::vector< boost::shared_ptr<int> > v;

    for(int i = 0; i < n; ++i)
    {
        v.push_back(pi);
    }
}

int const m = 16; // threads

int main()
{
	std_log(LOG_FILENAME_LINE,"[Test Case for shared_ptr_mt_test]");
    using namespace std; // printf, clock_t, clock

    printf("%s: %d threads, %d iterations: ", title, m, n);

    boost::shared_ptr<int> pi(new int(42));

    clock_t t = clock();

    pthread_t a[m];

    for(int i = 0; i < m; ++i)
    {
        a[i] = createThread( boost::bind(test, pi) );
    }

    for(int j = 0; j < m; ++j)
    {
        pthread_join(a[j], 0);
    }

    t = clock() - t;

    printf("\n\n%.3f seconds.\n", static_cast<double>(t) / CLK_TCK);

#ifdef __SYMBIAN32__
	std_log(LOG_FILENAME_LINE,"Result : Passed");
	std_log(LOG_FILENAME_LINE,"[End Test Case ]");
#endif
	testResultXml("shared_ptr_mt_test");
	close_log_file();
    return 0;
}
