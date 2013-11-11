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

#ifndef _DEQUE_CTOR_H__
#define _DEQUE_CTOR_H__

namespace gnu_namespace {

 struct counter
  {
   // Specifically and glaringly-obviously marked 'signed' so that when
    // COUNT mistakenly goes negative, we can track the patterns of
    // deletions more easily.
    typedef  signed int     size_type;
    static size_type   count;
    

    counter() { ++count; }
    counter (const counter&) { ++count; }
    ~counter() { --count; }
  };
  
    // A (static) class for counting assignment operator calls and
  // possibly throwing an exception on a desired count.
  class assignment_operator
  {
  public:
    static unsigned int
    count() { return count_; }
    
    static void
    mark_call()
    {
      count_++;
      if (count_ == throw_on_)
	#ifndef __SYMBIAN32__
	__throw_exception_again "assignment operator exception";
	#else
	 throw("assignment operator exception");
	 #endif
    }

    static void
    reset()
    {
      count_ = 0;
      throw_on_ = 0;
    }

    static void
    throw_on(unsigned int count) { throw_on_ = count; }

  private:
    static unsigned int count_;
    static unsigned int throw_on_;
  };
  unsigned int assignment_operator::count_ =0;
  unsigned int assignment_operator::throw_on_ =0;
  
   // A (static) class for tracking calls to an object's destructor.
  class destructor
  {
  public:
    static unsigned int
    count() { return _M_count; }
    
    static void
    mark_call() { _M_count++; }

    static void
    reset() { _M_count = 0; }

  private:
    static unsigned int _M_count;
  };
 unsigned int destructor::_M_count=0;
 // A (static) class for counting copy constructors and possibly throwing an
  // exception on a desired count.
  class copy_constructor
  {
  public:
    static unsigned int
    count() { return count_; }
    
    static void
    mark_call()
    {
      count_++;
      if (count_ == throw_on_)
	#ifndef __SYMBIAN32__
	__throw_exception_again "copy constructor exception";
	#else
	 throw("copy constructor exception");
	 #endif
    }
      
    static void
    reset()
    {
      count_ = 0;
      throw_on_ = 0;
    }
      
    static void
    throw_on(unsigned int count) { throw_on_ = count; }

  private:
    static unsigned int count_;
    static unsigned int throw_on_;
  };
   unsigned int copy_constructor::count_=0;
   unsigned int copy_constructor::throw_on_=0;
 
 
 class copy_tracker
  {
  public:
    // Creates a copy-tracking object with the given ID number.  If
    // "throw_on_copy" is set, an exception will be thrown if an
    // attempt is made to copy this object.
    copy_tracker(int id = next_id_--, bool throw_on_copy = false)
    : id_(id) , throw_on_copy_(throw_on_copy) { }

    // Copy-constructs the object, marking a call to the copy
    // constructor and forcing an exception if indicated.
    copy_tracker(const copy_tracker& rhs)
    : id_(rhs.id()), throw_on_copy_(rhs.throw_on_copy_)
    {
      int kkk = throw_on_copy_;
      if (throw_on_copy_)
	copy_constructor::throw_on(copy_constructor::count() + 1);
      copy_constructor::mark_call();
    }

    // Assigns the value of another object to this one, tracking the
    // number of times this member function has been called and if the
    // other object is supposed to throw an exception when it is
    // copied, well, make it so.
    copy_tracker&
    operator=(const copy_tracker& rhs)
    { 
      id_ = rhs.id();
      if (rhs.throw_on_copy_)
        assignment_operator::throw_on(assignment_operator::count() + 1);
      assignment_operator::mark_call();
      return *this;
    }

    ~copy_tracker()
    { destructor::mark_call(); }

    int
    id() const { return id_; }

  private:
    int   id_;
    const bool  throw_on_copy_;

  public:
    static void
    reset()
    {
      copy_constructor::reset();
      assignment_operator::reset();
      destructor::reset();
    }

    // for backwards-compatibility
    static int
    copyCount() 
    { return copy_constructor::count(); }

    // for backwards-compatibility
    static int
    dtorCount() 
    { return destructor::count(); }

  private:
    static int next_id_;
  };
inline  bool operator==(const copy_tracker& lhs, const copy_tracker& rhs)
    { 
      return (lhs.id() == rhs.id());
    }
} 


  #endif // _DEQUE_CTOR_H__