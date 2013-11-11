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

#ifndef EMANAGED_H
#define EMANAGED_H

#include <e32base.h>

#include <typerel.h>
#include <swap.h>




/**
   @file
   @brief Utility class templates that provide RAII-based automatic
   resource management.

	 @publishedAll
	 @released
*/


  /**
     Implementation function.In order to override the default cleanup
     strategy for a particular type, use the provided
     DEFINE_CLEANUP_FUNCTION utility macro
     @internalComponent
  */
// Not for Client Use , Only to be used Internally.
template<class T>
inline void CallCleanupFunction(T* aObjPtr)
	{
	aObjPtr->Close();
	}


/**
Utility macro that can be used for defining the cleanup member
function for a class (typically a R-class).

This macro can be used in the same namespace in which the R-class is
defined or in a namespace in which the R-class is used.

Example:

class RDestroyableClass
	{
  public:
	// ...
	void Destroy(); // member function used for cleanup and releasing the resources owned by a RDestroyableClass object
	// ...
	};

DEFINE_CLEANUP_FUNCTION(RDestroyableClass, Destroy)

@param AClass the name of the class
@param CleanupMemFun the name of the cleanup member function of the class
 */
#define DEFINE_CLEANUP_FUNCTION(AClass, CleanupMemFun)	\
	inline void CallCleanupFunction(AClass* aObjPtr)	\
		{												\
		aObjPtr->CleanupMemFun();						\
		}

/**
Utility macro that can be used for specializing the default cleanup
strategy class template TResourceCleanupStrategy for a particular
class (typically a R-class).  The default cleanup strategy for a class
specified using DEFINE_CLEANUP_STRATEGY overrides any other cleanup
strategy specified using DEFINE_CLEANUP_FUNCTION for that class.

This macro must be used in the same namespace in which the R-class is
defined.


   Utility macro that can be used for enabling single phase
   construction for CBase-derived classes. This is necessary because
   Symbian OS currently lacks the placement delete operator
   counterparts corresponding to the placement new operators that take
   a TLeave parameter (new(ELeave)), which will result in memory leaks
   if a class constructor leaves.

   This macro must be used within a public section of a class
   definition, if the single phase construction is part of the public
   interface of the class.

   Current Limitation CONSTRUCTORS_MAY_LEAVE is an unfortunate blight on the
   usability of single-phase construction, but we have yet to come up
   with a better alternative in the face of the legacy handling of
   ELeave.
*/
#define CONSTRUCTORS_MAY_LEAVE											\
	static void operator delete(TAny* aPtr) __NO_THROW					\
		{																\
		::operator delete(aPtr);										\
		}																\
																		\
	static void operator delete(TAny*, TAny*) __NO_THROW				\
		{																\
		}																\
																		\
	static void operator delete(TAny* aPtr, TLeave) __NO_THROW			\
		{																\
		::operator delete(aPtr);										\
		}																\
																		\
	static void operator delete(TAny* aPtr, TUint) __NO_THROW			\
		{																\
		::operator delete(aPtr);										\
		}																\
																		\
	static void operator delete(TAny* aPtr, TLeave, TUint) __NO_THROW	\
		{																\
		::operator delete(aPtr);										\
		}																\
																		\
	static void operator delete[](TAny* aPtr) __NO_THROW				\
		{																\
		::operator delete[](aPtr);										\
		}																\
																		\
	static void operator delete[](TAny* aPtr, TLeave) __NO_THROW		\
		{																\
		::operator delete[](aPtr);										\
		}


// Implementation function.
template<typename T>
void ManagedPopCleanupStackItem(T aIsManaged)
	{
// CleanupStack-based cleanup is automatically triggered by a Leave,
// so, in the case when __LEAVE_EQUALS_THROW__,
// CleanupStack::PopAndDestroy must not be called again here
#ifndef __GCCXML__
// for gccxml builds the std::uncaught_exception function is not listed in std name space
// to supress GCCXML error
	if (!std::uncaught_exception())
		{
		if (aIsManaged)
			{
			CleanupStack::PopAndDestroy();
			}
		else
			{
			CleanupStack::Pop();
			}
		}
#endif		
	}

/**
   Strategy (policy) class that defines the default cleanup strategy
   for managed resource class objects.

   The default cleanup strategy is to call the cleanup member function
   of the managed class, which is the Close() member function of the
   managed class, unless explicitly defined otherwise, for example by
   using the provided DEFINE_CLEANUP_FUNCTION macro.
   
   @internalComponent
*/
// Not for Client Use , Only to be used Internally.
class TResourceCleanupStrategy
	{
  public:
	template<typename T>
	static void Cleanup(T* aObjPtr)
		{
		CallCleanupFunction(aObjPtr);
		}
	};

/**
   Strategy (policy) class that defines a cleanup strategy for managed
   resource class objects.  This cleanup strategy calls the Close()
   member function of the managed class.

   @see LCleanedupHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TClose
	{
  public:
	template<class T>
	static void Cleanup(T* aObjPtr)
		{
		aObjPtr->Close();
		}
	};

/**
   Strategy (policy) class that defines a cleanup strategy for managed
   resource class objects.  This cleanup strategy calls the Release()
   member function of the managed class.

   @see LCleanedupHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TRelease
	{
  public:
	template<class T>
	static void Cleanup(T* aObjPtr)
		{
		aObjPtr->Release();
		}
	};

/**
   Strategy (policy) class that defines a cleanup strategy for managed
   resource class objects.  This cleanup strategy calls the Destroy()
   member function of the managed class.

   @see LCleanedupHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TDestroy
	{
  public:
	template<class T>
	static void Cleanup(T* aObjPtr)
		{
		aObjPtr->Destroy();
		}
	};

/**
   Strategy (policy) class that defines a cleanup strategy for managed
   resource class objects.  This cleanup strategy calls the Free()
   member function of the managed class.

   @see LCleanedupHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TFree
	{
  public:
	template<class T>
	static void Cleanup(T* aObjPtr)
		{
		aObjPtr->Free();
		}
	};

/**
   Strategy (policy) class that defines a cleanup strategy for managed
   resource class objects.  This cleanup strategy calls the
   ResetAndDestroy() member function of the managed class.

   @see LCleanedupHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedHandle to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TResetAndDestroy
	{
  public:
	template<class T>
	static void Cleanup(T* aObjPtr)
		{
		aObjPtr->ResetAndDestroy();
		}
	};


/**
   Strategy (policy) class that defines the default cleanup strategy
   for pointer types.  For pointers to CBase-derived types, the
   default cleanup strategy is to call CBase::Delete with the managed
   pointer.  For pointers to types that are not derived from CBase,
   the default cleanup strategy is to delete the managed pointer using
   non-array delete.

   @see LCleanedupPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TPtrCleanupStrategy
	{
  public:
	template<typename T>
	static void Cleanup(T* aPtr)
		{
		delete aPtr;
		}

	static void Cleanup(CBase* aPtr)
		{
		CBase::Delete(aPtr);
		}
	};


/**
   Strategy (policy) class that defines a cleanup strategy for pointer
   types.  This cleanup strategy deletes the managed pointer by using
   non-array delete.

   @see LCleanedupPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TPointerDeleteStrategy
	{
  public:
	template<typename T>
	static void Cleanup(T* aPtr)
		{
		delete aPtr;
		}
	};


/**
   Strategy (policy) class that defines a cleanup strategy for
   pointers to CBase-derived types.  This cleanup strategy calls
   CBase::Delete with the managed pointer.

   @see LCleanedupPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TCBaseDeleteStrategy
	{
  public:
	static void Cleanup(CBase* aPtr)
		{
		CBase::Delete(aPtr);
		}
	};


/**
   Strategy (policy) class that defines a cleanup strategy for pointer
   types.  This cleanup strategy calls User::Free with the managed
   pointer.

   @see LCleanedupPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
   @see LManagedPtr to which this strategy type may be supplied as
   an (optional) second tamplate parameter
*/
class TPointerFree
	{
  public:
	static void Cleanup(TAny* aPtr)
		{
		User::Free(aPtr);
		}
	};


/**
   Strategy (policy) class that defines the default cleanup strategy
   for heap-allocated arrays.  This cleanup strategy deallocates the
   managed array by using array delete.
*/
class TArrayDelete
	{
  public:
	template<typename T>
	static void Cleanup(T* aPtr)
		{
		delete[] aPtr;
		}
	};


// enum type used for identifying the categories of managed pointer types
enum TManagedPtrType
{
	EPtrNonSpecial,
	EPtrCBaseDerived
};


// macro used for determining whether a pointer is special
#define IS_PTR_SPECIAL(T) IS_BASE_OF(CBase, T)


// enum type used for identifying the categories of resource handle types
enum TAutoHandleType
{
	EAutoHandleNonSpecial,
	EAutoRHandleBaseDerived,
	EAutoHandleRBuf
};


// macro used for determining whether a resource handle type is special
#define IS_HANDLE_SPECIAL(T) IS_BASE_OF(RHandleBase, T) ? EAutoRHandleBaseDerived : ( (IS_SAME(RBuf8, T) || IS_SAME(RBuf16, T)) ? EAutoHandleRBuf : EAutoHandleNonSpecial )


/**
   Implementation base class - not designed for public inheritance or
   direct use.
   
   @internalComponent
*/
// Not for Client Use , Only to be used Internally.
template<typename T,
		 TInt isHandleSpecial = IS_HANDLE_SPECIAL(T)>
class LAutoHandleBase
	{
  protected:
	LAutoHandleBase()
		: iEnabled(ETrue)
		{
		}

	template<typename Param1>
	explicit LAutoHandleBase(const Param1& aParam1)
		: iHandle(aParam1),
		  iEnabled(ETrue)
		{
		}

	template<typename Param1>
	explicit LAutoHandleBase(Param1& aParam1)
		: iHandle(aParam1),
		  iEnabled(ETrue)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(const Param1& aParam1,
					const Param2& aParam2)
		: iHandle(aParam1,
				  aParam2),
		  iEnabled(ETrue)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(Param1& aParam1,
					const Param2& aParam2)
		: iHandle(aParam1,
				  aParam2),
		  iEnabled(ETrue)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(const Param1& aParam1,
					Param2& aParam2)
		: iHandle(aParam1,
				  aParam2),
		  iEnabled(ETrue)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(Param1& aParam1,
					Param2& aParam2)
		: iHandle(aParam1,
				  aParam2),
		  iEnabled(ETrue)
		{
		}

	template<typename U>
	LAutoHandleBase& operator=(const U& aHandle)
		{
		iHandle = aHandle;
		iEnabled = ETrue;
		return *this;
		}

	T& Get()
		{
		return iHandle;
		}

	const T& Get() const
		{
		return iHandle;
		}

	T& operator*()
		{
		return iHandle;
		}

	const T& operator*() const
		{
		return iHandle;
		}

	T* operator->()
		{
		return &iHandle;
		}

	const T* operator->() const
		{
		return &iHandle;
		}

	T Unmanage()
		{
		iEnabled = EFalse;
		return iHandle;
		}

	TBool IsEnabled() const
		{
		return iEnabled;
		}

	void Disable()
		{
		iEnabled = EFalse;
		}

	void Swap(LAutoHandleBase& aAutoHandle)
		{
		::Swap(iHandle, aAutoHandle.iHandle);
		::Swap(iEnabled, aAutoHandle.iEnabled);
		}

  protected:
	T iHandle;
	TBool iEnabled;

  private:
	LAutoHandleBase(const LAutoHandleBase&);
	LAutoHandleBase& operator=(const LAutoHandleBase&);
	};


/**
   Implementation base class - not designed for public inheritance or
   direct use.  Specialization for types derived from RHandleBase.
*/
template<typename T>
class LAutoHandleBase<T, EAutoRHandleBaseDerived>
	{
  protected:
	LAutoHandleBase()
		{
		}

	template<typename Param1>
	explicit LAutoHandleBase(const Param1& aParam1)
		: iHandle(aParam1)
		{
		}

	template<typename Param1>
	explicit LAutoHandleBase(Param1& aParam1)
		: iHandle(aParam1)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(const Param1& aParam1,
					const Param2& aParam2)
		: iHandle(aParam1,
				  aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(Param1& aParam1,
					const Param2& aParam2)
		: iHandle(aParam1,
				  aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(const Param1& aParam1,
					Param2& aParam2)
		: iHandle(aParam1,
				  aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LAutoHandleBase(Param1& aParam1,
					Param2& aParam2)
		: iHandle(aParam1,
				  aParam2)
		{
		}

	template<typename U>
	LAutoHandleBase& operator=(const U& aHandle)
		{
		iHandle = aHandle;
		return *this;
		}

	T& Get()
		{
		return iHandle;
		}

	const T& Get() const
		{
		return iHandle;
		}

	T& operator*()
		{
		return iHandle;
		}

	const T& operator*() const
		{
		return iHandle;
		}

	T* operator->()
		{
		return &iHandle;
		}

	const T* operator->() const
		{
		return &iHandle;
		}

	T Unmanage()
		{
		T handle = iHandle;
		iHandle.SetHandle(KNullHandle);
		return handle;
		}

	TBool IsEnabled() const
		{
		return iHandle.Handle() != KNullHandle;
		}

	void Disable()
		{
		iHandle.SetHandle(KNullHandle);
		}

	void Swap(LAutoHandleBase& aAutoHandle)
		{
		::Swap(iHandle, aAutoHandle.iHandle);
		}

  protected:
	T iHandle;

  private:
	LAutoHandleBase(const LAutoHandleBase&);
	LAutoHandleBase& operator=(const LAutoHandleBase&);
	};


// N.B. RBuf8, RBuf16 and RBuf cannot be used with LManagedHandle and
// LCleanedupHandle.  Use LString or managed references instead.
// The following specialization must not be used.
template<typename T>
class LAutoHandleBase<T, EAutoHandleRBuf>: protected T
	{
  private:
	LAutoHandleBase()
		{
		}

	~LAutoHandleBase()
		{
		}
	};


/**
   A class template for the creation and automatic management of
   resource handles (typically R-class instances) held in the data
   members of objects.

   @note This class should not used to define locals. See below for
   an explanation and links to management classes suitable for use in
   that context.

   This class template can be used to protect a resource handle of
   type T (typically an R-class instance) such that the instance of T
   protected is automatically cleaned up when the management object is
   destroyed; typically when the object containing it is deleted.

   By default, the cleanup action is to call the Close() member
   function of the managed handle. An alternative cleanup strategy may
   be selected by specifying a cleanup strategy template class in the
   optional second template parameter position. The most common
   alternative cleanup strategies are predefined. It is also possible
   to specialize the default cleanup action for a given class using
   the DEFINE_CLEANUP_FUNCTION macro.

   The constructors of this class never leave (unless construction of
   the underlying T instance can leave, which is rare), so data
   members defined with this type may be initialized safely during any
   phase of construction of the owning class.

   Any arguments supplied when initializing an instance of this class
   are automatically passed through to T's constructors.

   As a convenience, the methods of the managed pointer may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
   class CComposite : public CBase
	   {
	 public:
	   CONSTRUCTORS_MAY_LEAVE

	   CComposite()
		   {
		   iFileServ->Connect() OR_LEAVE;
		   iFile->Open(*iFileServ, ...);
		   }

	   ~CComposite()
		   {
		   // the handles are automatically closed
		   }

	 private:

	   LManagedHandle<RFs> iFileServ;
	   LManagedHandle<RFile> iFile;
	   };
   @endcode

   Behind the scenes, this class template simply relies on reliable
   execution of its destructor. If used for a local variable rather
   than a data member, cleanup will occur but out-of-order compared to
   objects protected using the LCleanupXxx variants or the
   CleanupStack directly. Therefore it is not recommended for use in
   that context.

   These management classes may be used as the basis for implementing
   leave-safe single-phase construction, since fully initialized
   data members protected in this way will get destroyed (so reliably
   triggering cleanup) if their containing classes leave during
   execution of their constructors. Note, however, that single-phase
   construction must be explicitly enabled in the containing class
   using the CONSTRUCTORS_MAY_LEAVE macro.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TClose which implements the default Close() calling cleanup strategy
   @see TResetAndDestroy which implements an alternative
   ResetAndDestroy() calling cleanup strategy
   @see TFree which implements an alternative Free() calling cleanup
   strategy
   @see TDestroy which implements an alternative Destroy() calling
   cleanup strategy
   @see TRelease which implements an alternative Release() calling cleanup strategy
   @see LCleanedupHandle which has the same interface, but uses the cleanup
   stack and is suitable for protecting locals
   @see CONSTRUCTORS_MAY_LEAVE
*/
template<typename T,
		 class CleanupStrategyType = TResourceCleanupStrategy>
class LManagedHandle: protected LAutoHandleBase<T, IS_HANDLE_SPECIAL(T)>
	{
	typedef LAutoHandleBase<T, IS_HANDLE_SPECIAL(T)> LAutoHandleBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;

/**
   Default constructor.
*/
	LManagedHandle()
		{
		}

	template<typename Param1>
	explicit LManagedHandle(const Param1& aParam1)
		: LAutoHandleBase(aParam1)
		{
		}

	template<typename Param1>
	explicit LManagedHandle(Param1& aParam1)
		: LAutoHandleBase(aParam1)
		{
		}

	template<typename Param1,
			 typename Param2>
	LManagedHandle(const Param1& aParam1,
				   const Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LManagedHandle(const Param1& aParam1,
				   Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LManagedHandle(Param1& aParam1,
				   const Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		}

	template<typename Param1,
			 typename Param2>
	LManagedHandle(Param1& aParam1,
				   Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		}

/**
   Assigns a new resource to be managed.  If the LManagedHandle object
   already contains a managed resource handle, then the managed
   resource is released using the specified cleanup strategy before
   assigning the new managed resource.

   @param aHandle a reference to a handle object of a type that can be assigned to a handle object of type T
 */
	template<typename U>
	LManagedHandle& operator=(const U& aHandle)
		{
		ReleaseResource();
		LAutoHandleBase::operator=(aHandle);
		return *this;
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor calls the cleanup function defined by the cleanup
   strategy with the contained resource handle object.
 */
	~LManagedHandle()
		{
		if (IsEnabled())
			{
			CleanupStrategy::Cleanup(&Get());
			}
		}

/**
   If automatic resource management is enabled, calls the cleanup
   function defined by the cleanup strategy with the managed resource
   handle object and then disables the automatic resource management
   for this object.	 The cleanup strategy is specified by the
   CleanupStrategy template template parameter.	 The default cleanup
   strategy is to call the cleanup member function on the contained
   resource handle object. which is a member function named Close(),
   unless explicitly defined otherwise for the class of the object,
   for example by using the provided DEFINE_CLEANUP_FUNCTION macro.
*/
	void ReleaseResource()
		{
		if (!IsEnabled())
			return;

		CleanupStrategy::Cleanup(&Get());
		LAutoHandleBase::Disable();
		}

/**
   Disables the automatic resource management for this object and
   returns a copy of the resource handle.

   @return A copy of the resource handle.
*/
	using LAutoHandleBase::Unmanage;

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoHandleBase::IsEnabled;

/**
   Returns a reference to the resource handle.

   @return A reference to the resource handle.
*/
	using LAutoHandleBase::Get;

/**
   Overloaded indirection operator function.

   @return A reference to the resource handle.
*/
	using LAutoHandleBase::operator*;

/**
   Overloaded class member access operator function.

   @return A pointer to the resource handle.
*/
	using LAutoHandleBase::operator->;

	using LAutoHandleBase::Disable;

	void Swap(LManagedHandle& aManagedHandle)
		{
		LAutoHandleBase::Swap(aManagedHandle);
		}
	};


/**
   Implementation base class - not designed for public inheritance or
   direct use.
   
   @internalComponent
*/
// Not for Client Use , Only to be used Internally.
template<typename T>
class LAutoPtrBase
	{
  protected:
	LAutoPtrBase()
		: iPtr(NULL)
		{
		}

	explicit LAutoPtrBase(T* aPtr)
		: iPtr(aPtr)
		{
		}

	LAutoPtrBase& operator=(T* aPtr)
		{
		iPtr = aPtr;
		return *this;
		}

	T* Unmanage()
		{
		T* ptr = iPtr;
		iPtr = NULL;
		return ptr;
		}

	TBool IsEnabled() const
		{
		return iPtr != NULL;
		}

	T* Get() const
		{
		return iPtr;
		}

	T* operator->() const
		{
		return iPtr;
		}

	void Disable()
		{
		iPtr = NULL;
		}

	void Swap(LAutoPtrBase& aAutoPtr)
		{
		::Swap(iPtr, aAutoPtr.iPtr);
		}

  protected:
	T* iPtr;

  private:
	LAutoPtrBase(const LAutoPtrBase&);
	LAutoPtrBase& operator=(const LAutoPtrBase&);
	};


// Cleanup traits class template
template<typename T,
		 class CleanupStrategyType,
		 TInt isPtrSpecial = IS_PTR_SPECIAL(T)>
struct TPtrCleanupTraits
	{
	};


// Cleanup traits class template specialization for pointers to types
// that are not derived from CBase
template<typename T,
		 class CleanupStrategyType>
struct TPtrCleanupTraits<T, CleanupStrategyType, EPtrNonSpecial>
	{
	typedef T ManagedType;
	typedef T BaseManagedType;
	typedef CleanupStrategyType CleanupStrategy;
	};

// Cleanup traits class template specialization for pointers to types
// that are derived from CBase
template<typename T,
		 class CleanupStrategyType>
struct TPtrCleanupTraits<T, CleanupStrategyType, EPtrCBaseDerived>
	{
	typedef T ManagedType;
	typedef CBase BaseManagedType;
	typedef CleanupStrategyType CleanupStrategy;
	};

// Cleanup traits class template specialization for pointers to types
// that are derived from CBase and the default pointer cleanup
// strategy (TPtrCleanupStrategy)
template<typename T>
struct TPtrCleanupTraits<T, TPtrCleanupStrategy, EPtrCBaseDerived>
	{
	typedef CBase ManagedType;
	typedef CBase BaseManagedType;
	typedef TPtrCleanupStrategy CleanupStrategy;
	};


/**
   Implementation base class - not designed for public inheritance or
   direct use.
*/
template<typename T,
		 class CleanupStrategyType>
class LManagedPtrBase: protected LAutoPtrBase<typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType>
	{
	typedef LAutoPtrBase<typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType> LAutoPtrBase;

  protected:
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::ManagedType ManagedType;
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType BaseManagedType;
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::CleanupStrategy CleanupStrategy;

	LManagedPtrBase()
		{
		}

	template<typename U>
	explicit LManagedPtrBase(U* aPtr)
		: LAutoPtrBase(aPtr)
		{
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   pointer.
 */
	~LManagedPtrBase()
		{
		if (IsEnabled())
			{
			CleanupStrategy::Cleanup(static_cast<ManagedType*>(iPtr));
			}
		}

	template<typename U>
	LManagedPtrBase& operator=(U* aPtr)
		{
		ReleaseResource();
		LAutoPtrBase::operator=(aPtr);
		return *this;
		}

/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked for the managed pointer and the automatic
   resource management is then disabled.  The underlying pointer is
   reset to NULL.

   @post Get() == NULL
*/
	void ReleaseResource()
		{
		if (!IsEnabled())
			return;

		CleanupStrategy::Cleanup(static_cast<ManagedType*>(iPtr));
		LAutoPtrBase::Disable();
		}

	using LAutoPtrBase::Unmanage;

	using LAutoPtrBase::IsEnabled;

	using LAutoPtrBase::Get;

	using LAutoPtrBase::operator->;

	using LAutoPtrBase::Disable;

	using LAutoPtrBase::iPtr;

	void Swap(LManagedPtrBase& aManagedPtr)
		{
		LAutoPtrBase::Swap(aManagedPtr);
		}
	};


/**
   A class template that provides automatic management of pointers
   held in the data members of objects.

   @note This class should not used to define locals. See below for
   an explanation and links to management classes suitable for use in
   that context.

   This class template can be used to protect a pointer to type T such
   that the instance of T referred to is automatically cleaned up when
   the management object is destroyed; typically when the object
   containing it is deleted.

   By default, the cleanup action is to delete the managed pointer
   using a (non-array) delete operation. An alternative cleanup
   strategy can be specified using the optional CleanupStrategy class
   template parameter of the LManagedPtr class template. The most
   common alternative cleanup strategies are predefined
   (e.g. TPointerFree).

   The constructors of this class never leave, so data members defined with
   this type may be initialized safely during any phase of
   construction of the owning class.

   As a convenience, the methods of the managed pointer may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
   class CComposite : public CBase
	   {
	 public:
	   CONSTRUCTORS_MAY_LEAVE

	   CComposite()
		   : iComponent(CComponent::NewL())
		   {
		   //...
		   }

	   ~CComposite()
		   {
		   // the pointer to the CComponent object is automatically
		   // deleted
		   }

	 private:
	   LManagedPtr<CComponent> iComponent;
	   };
	@endcode

   Behind the scenes, this class template simply relies on reliable
   execution of its destructor. If used for a local variable rather
   than a data member, cleanup will occur but out-of-order compared to
   objects protected using the LCleanupXxx variants or the
   CleanupStack directly. Therefore it is not recommended for use in
   that context.

   These management classes may be used as the basis for implementing
   leave-safe single-phase construction, since fully initialized
   data members protected in this way will get destroyed (so reliably
   triggering cleanup) if their containing classes leave during
   execution of their constructors. Note, however, that single-phase
   construction must be explicitly enabled in the containing class
   using the CONSTRUCTORS_MAY_LEAVE macro.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TPointerDelete which implements the default deleting cleanup strategy
   @see TPointerFree which implements the alternative User::Free() cleanup strategy
   @see LCleanedupPtr which has the same interface, but uses the cleanup
   stack and is suitable for protecting locals
   @see CONSTRUCTORS_MAY_LEAVE
*/
template<typename T,
		 class CleanupStrategyType = TPtrCleanupStrategy>
class LManagedPtr: protected LManagedPtrBase<T, CleanupStrategyType>
	{
	typedef LManagedPtrBase<T, CleanupStrategyType> LManagedPtrBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;


/**
   Default constructor.	 Constructs an empty LManagedPtr object.

   @post Get() == NULL
 */
	LManagedPtr()
		{
		}

/**
   Explicit constructor template.  Constructs a LManagedPtr object
   that manages the pointer aPtr of a type convertible to T* that can
   be cleaned up using the cleanup strategy of the LManagedPtr class.
   The default cleanup strategy is to delete the pointer to a
   heap-allocated object by using non-array delete.	 Alternative
   cleanup strategies can be specified by using the CleanupStrategy
   template parameter of the LManagedPtr class template.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is of a type convertible to T* and can be cleaned up
   using the cleanup strategy.

   @post Get() == aPtr
 */
	explicit LManagedPtr(T* aPtr)
		: LManagedPtrBase(aPtr)
		{
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   pointer.
 */


/**
   Assigns a new pointer to be managed.	 The new pointer must be of a
   type convertible to T* and it must be possible to use the cleanup
   strategy of the LManagedPtr object for the cleanup of the new
   managed pointer.	 If the LManagedPtr object already contains a
   managed pointer, then the cleanup strategy is invoked with the
   managed pointer before assigning the new managed pointer.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is a pointer of a type that is convertible to T* and can
   be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	LManagedPtr& operator=(T* aPtr)
		{
		LManagedPtrBase::operator=(aPtr);
		return *this;
		}

/**
   Assigns a new pointer to be managed.	 The new pointer must be of a
   type convertible to T* and it must be possible to use the cleanup
   strategy of the LManagedPtr object for the cleanup of the new
   managed pointer.	 If the LManagedPtr object already contains a
   managed pointer, then the cleanup strategy is invoked with the
   managed pointer before assigning the new managed pointer.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is a pointer of a type that is convertible to T* and can
   be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	template<typename U>
	LManagedPtr& operator=(U* aPtr)
		{
		LManagedPtrBase::operator=(aPtr);
		return *this;
		}

	using LManagedPtrBase::ReleaseResource;

/**
   Disables the automatic resource management for this object and
   returns a pointer to the object of type T.

   @return A pointer to the object of type T.
*/
	T* Unmanage()
		{
		return static_cast<T*>(LManagedPtrBase::Unmanage());
		}

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LManagedPtrBase::IsEnabled;

/**
   Returns a pointer to the managed object of type T.

   @return A pointer to the managed object of type T.
*/
	T* Get() const
		{
		return static_cast<T*>(iPtr);
		}

/**
   Overloaded indirection operator function.

   @return A reference to the managed object of type T.
*/
	T& operator*() const
		{
		return *(static_cast<T*>(iPtr));
		}

/**
   Overloaded class member access operator function.

   @return A pointer to the managed object of type T.
*/
	T* operator->() const
		{
		return static_cast<T*>(iPtr);
		}


// Implementation type - do not use
	typedef typename LManagedPtrBase::BaseManagedType* LManagedPtr<T, CleanupStrategy>::*TUnspecifiedBoolType;

/**
   Conversion operator that enables LCleanedupPtr objects to be used
   in boolean contexts.

   @return An unspecified value of an unspecified type convertible to
   boolean, which has a boolean value equal to Get() != NULL
 */
	operator TUnspecifiedBoolType()
		{
		return iPtr ? &LManagedPtr::iPtr : NULL;
		}


	using LManagedPtrBase::Disable;

	void Swap(LManagedPtr& aManagedPtr)
		{
		LManagedPtrBase::Swap(aManagedPtr);
		}

  private:
	using LManagedPtrBase::iPtr;
	};


// function template used for comparing two LManagedPtr-managed
// pointers for equality
template<typename T, typename U>
TBool operator==(const LManagedPtr<T>& aPtr1, const LManagedPtr<U>& aPtr2)
	{
	return aPtr1.Get() == aPtr2.Get();
	}

// function template used for comparing two LManagedPtr-managed
// pointers for inequality
template<typename T, typename U>
TBool operator!=(const LManagedPtr<T>& aPtr1, const LManagedPtr<U>& aPtr2)
	{
	return aPtr1.Get() != aPtr2.Get();
	}

// function template used for testing the ordering of two
// LManagedPtr-managed pointers
template<typename T, typename U>
TBool operator<(const LManagedPtr<T>& aPtr1, const LManagedPtr<U>& aPtr2)
	{
	return aPtr1.Get() < aPtr2.Get();
	}


/**
   A class template that provides automatic management of arrays. Such
   managed arrays can be data members of composite classes.

   @note This class should not used to define locals. See below for
   an explanation and links to management classes suitable for use in
   that context.

   @par

   @note This class can only be used with raw arrays, which are used
   only rarely on Symbian OS.  Instances of Symbian array container
   classes (e.g. RArray, RPointerArray) should be managed using the
   automatic management template classes appropriate for the array's
   type (LManagedHandle template classes for Symbian R arrays or
   LManagedPtr template classes for Symbian C arrays).

   This class template can be used to protect a heap-allocated array
   of objects of type T such that the managed array is automatically
   deallocated when the management object is destroyed.

   The default cleanup strategy is to deallocate the managed array
   using arrray delete (delete[]), assuming that the array is
   heap-allocated.	An alternative cleanup strategy can be selected by
   specifying a cleanup strategy template class as the optional second
   template argument (corresponding to the CleanupStrategy template
   parameter).

   The constructors of this class never leave, so data members defined with
   this type may be initialized safely during any phase of
   construction of the owning class.

   As a convenience, the elements of the managed array may be accessed
   via "[]" notation directly on the management object.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().


   Example:
   @code
   class CComposite : public CBase
	   {
	 public:
	   CONSTRUCTORS_MAY_LEAVE

	   CComposite()
		   : iComponents(new(ELeave) CComponent[KNumComponents])
		   {
		   //...
		   }

	   ~CComposite()
		   {
		   // the array is automatically deleted
		   }

	 private:
	   LManagedArray<CComponent> iComponents;
	   };
   @endcode


   Behind the scenes, this class template simply relies on reliable
   execution of its destructor. If used for a local variable rather
   than a data member, cleanup will occur but out-of-order compared to
   objects protected using the LCleanupXxx variants or the
   CleanupStack directly. Therefore it is not recommended for use in
   that context.

   These management classes may be used as the basis for implementing
   leave-safe single-phase construction, since fully initialized
   data members protected in this way will get destroyed (so reliably
   triggering cleanup) if their containing classes leave during
   execution of their constructors. Note, however, that single-phase
   construction must be explicitly enabled in the containing class
   using the CONSTRUCTORS_MAY_LEAVE macro.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see LCleanedupArray which has the same interface, but uses the cleanup
   stack and is suitable for protecting locals
   @see CONSTRUCTORS_MAY_LEAVE
*/
template<typename T,
		 class CleanupStrategyType = TArrayDelete>
class LManagedArray: protected LAutoPtrBase<T>
	{
	typedef LAutoPtrBase<T> LAutoPtrBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;

/**
   Default constructor.	 Constructs an empty LManagedArray object.

   @post Get() == NULL
 */
	LManagedArray()
		{
		}

/**
   Explicit constructor.  Constructs a LManagedArray object that
   manages an array of objects of type T that can be cleaned up using
   the cleanup strategy of the LManagedArray class.	 The default
   cleanup strategy is to deallocate the managed array by using array
   delete (delete[]), assuming that the array is heap-allocated.
   Alternative cleanup strategies can be specified by using the
   CleanupStrategy template parameter of the LManagedArray class
   template.

   @param aPtr A pointer to the first element of an array of objects
   of type T - array that can be cleaned up using the cleanup strategy
   of the the LManagedArray class.

   @pre The array can be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	explicit LManagedArray(T* aPtr)
		: LAutoPtrBase(aPtr)
		{
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   pointer.
 */
	~LManagedArray()
		{
		if (LAutoPtrBase::IsEnabled())
			{
			CleanupStrategy::Cleanup(iPtr);
			}
		}

/**
   Assigns a new array of objects of type T to be managed.	It needs
   to be possible use the cleanup strategy of the LManagedArray object
   for the cleanup of the new managed array.  The default cleanup
   strategy is to delete the heap-allocated array by using array
   delete (delete[]). If the LManagedArray object already manages an
   array, then the cleanup strategy is invoked with the managed array
   before assigning the new managed array.

   @param aPtr A pointer to the first element of the array of objects
   of type T - array that can be cleaned up using the cleanup
   strategy.

   @pre The new array to be managed can be cleaned up using the
   cleanup strategy.

   @post Get() == aPtr
 */
	LManagedArray& operator=(T* aPtr)
		{
		ReleaseResource();
		LAutoPtrBase::operator=(aPtr);
		return *this;
		}

/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked for the managed pointer and the automatic
   resource management is then disabled.  The underlying pointer is
   reset to NULL.

   @post Get() == NULL
*/
	void ReleaseResource()
		{
		if (!LAutoPtrBase::IsEnabled())
			return;

		CleanupStrategy::Cleanup(iPtr);
		LAutoPtrBase::Disable();
		}

/**
   Disables the automatic resource management for this object and
   returns a pointer to the first element of the array of objects of
   type T.

   @return A pointer to the first element of the array of objects of
   type T.
*/
	T* Unmanage()
		{
		return static_cast<T*>(LAutoPtrBase::Unmanage());
		}

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoPtrBase::IsEnabled;

/**
   Returns a pointer to the first element of the managed array of
   objects of type T.

   @return A pointer to the first element of the managed array of
   objects of type T.
*/
	using LAutoPtrBase::Get;

/**
   Overloaded subscript operator.

   @return A reference to the object of type T at the position aIndex.
 */
	T& operator[](TInt aIndex) const
		{
		return iPtr[aIndex];
		}

	using LAutoPtrBase::Disable;

	void Swap(LManagedArray& aArray)
		{
		LAutoPtrBase::Swap(aArray);
		}

  private:
	using LAutoPtrBase::iPtr;
	};


/**
   Implementation base class - not designed for public inheritance or
   direct use.
   
   @internalComponent
*/
// Not for Client Use , Only to be used Internally.
template<typename T>
class LAutoRefBase
	{
  protected:
	template<typename U>
	explicit LAutoRefBase(U& aRef)
		: iPtr(&aRef)
		{
		}

	template<typename U>
	LAutoRefBase& operator=(U& aRef)
		{
		iPtr = &aRef;
		return *this;
		}

	T& Unmanage()
		{
		T* ptr = iPtr;
		iPtr = NULL;
		return *ptr;
		}

	TBool IsEnabled() const
		{
		return iPtr != NULL;
		}

	T& Get() const
		{
		return *iPtr;
		}

	T& operator*() const
		{
		return *iPtr;
		}

	T* operator->() const
		{
		return iPtr;
		}

	void Disable()
		{
		iPtr = NULL;
		}

	void Swap(LAutoRefBase& aAutoRef)
		{
		::Swap(iPtr, aAutoRef.iPtr);
		}

  protected:
	T* iPtr;

  private:
	LAutoRefBase(const LAutoRefBase&);
	LAutoRefBase& operator=(const LAutoRefBase&);
	};


/**
   A class template that provides automatic management of references
   to resource handles (often R-class instances) held in the data
   members of objects.

   @note This class should not used to define locals. See below for
   an explanation and links to management classes suitable for use in
   that context.

   Unlike LManagedHandle which creates a fresh instance of its managed
   type, this class template can be used to protect an existing
   resource handle of type T (typically an R-class instance). The
   instance of T referred to has a cleanup operation run on it
   automatically when the management object is destroyed; typically
   when the object containing it is deleted.

   By default, the cleanup action is to call the Close() member
   function of the referenced handle. An alternative cleanup strategy may
   be selected by specifying a cleanup strategy template class in the
   optional second template parameter position. The most common
   alternative cleanup strategies are predefined. It is also possible
   to specialize the default cleanup action for a given class using
   the DEFINE_CLEANUP_FUNCTION macro.

   The constructors of this class never leave, so data members defined with
   this type may be initialized safely during any phase of
   construction of the owning class.

   As a convenience, the methods of the managed pointer may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
   class CComposite : public CBase
	   {
	 public:
	   CONSTRUCTORS_MAY_LEAVE

	   // An existing RFs instance is given to us to reuse, but
	   // we are responsible for calling Close() when we're done
	   CComposite(RFs& aFs)
		   : iFileServ(aFs)
		   {
		   iFileServ->Connect() OR_LEAVE;
		   iFile->Open(*iFileServ, ...);
		   }

	   ~CComposite()
		   {
		   // the handles are automatically closed
		   }

	 private:

	   LManagedRef<RFs> iFileServ;
	   LManagedHandle<RFile> iFile;
	   };
   @endcode

   Behind the scenes, this class template simply relies on reliable
   execution of its destructor. If used for a local variable rather
   than a data member, cleanup will occur but out-of-order compared to
   objects protected using the LCleanupXxx variants or the
   CleanupStack directly. Therefore it is not recommended for use in
   that context.

   These management classes may be used as the basis for implementing
   leave-safe single-phase construction, since fully initialized
   data members protected in this way will get destroyed (so reliably
   triggering cleanup) if their containing classes leave during
   execution of their constructors. Note, however, that single-phase
   construction must be explicitly enabled in the containing class
   using the CONSTRUCTORS_MAY_LEAVE macro.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TClose which implements the default Close() calling cleanup strategy
   @see TResetAndDestroy which implements an alternative
   ResetAndDestroy() calling cleanup strategy
   @see TFree which implements an alternative Free() calling cleanup
   strategy
   @see TDestroy which implements an alternative Destroy() calling
   cleanup strategy
   @see TRelease which implements an alternative Release() calling
   cleanup strategy
   @see LCleanedupRef which has the same interface, but uses the cleanup
   stack and is suitable for protecting locals
   @see LManagedHandle which has a similar interface but creates a fresh
   local instance of T
   @see CONSTRUCTORS_MAY_LEAVE
*/
template<typename T,
		 class CleanupStrategyType = TResourceCleanupStrategy>
class LManagedRef: protected LAutoRefBase<T>
	{
	typedef LAutoRefBase<T> LAutoRefBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;

/**
   Explicit constructor.
 */
	template<typename U>
	explicit LManagedRef(U& aRef)
		: LAutoRefBase(aRef)
		{
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   reference.
 */
	~LManagedRef()
		{
		if (LAutoRefBase::IsEnabled())
			{
			CleanupStrategy::Cleanup(iPtr);
			}
		}

/**
   Assigns a new reference to be managed.  If the LManagedRef
   object already contains a managed reference, then the specified
   cleanup strategy is invoked for the managed reference before
   assigning the new managed reference.
 */
	template<typename U>
	LManagedRef& operator=(U& aRef)
		{
		ReleaseResource();
		LAutoRefBase::operator=(aRef);
		return *this;
		}

/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked for the managed reference and the automatic
   resource management is then disabled for this object.
*/
	void ReleaseResource()
		{
		if (!LAutoRefBase::IsEnabled())
			return;

		CleanupStrategy::Cleanup(iPtr);
		LAutoRefBase::Disable();
		}

/**
   Disables the automatic resource management for this object and
   returns a reference to the object of type T.

   @return A reference to the object of type T.
*/
	using LAutoRefBase::Unmanage;

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoRefBase::IsEnabled;

/**
   Returns a reference to the managed object of type T.

   @return A reference to the managed object of type T.
*/
	using LAutoRefBase::Get;

/**
   Overloaded indirection operator function.

   @return A reference to the managed object of type T.
*/
	using LAutoRefBase::operator*;

/**
   Overloaded class member access operator function.

   @return A pointer to the managed object of type T.
*/
	using LAutoRefBase::operator->;

	using LAutoRefBase::Disable;

	void Swap(LManagedRef& aRef)
		{
		LAutoRefBase::Swap(aRef);
		}

  private:
	using LAutoRefBase::iPtr;
	};


/**
   A class template for the creation and CleanupStack-based
   local-scope automatic management of resource handles (typically
   instances of R-classes).

   @note This class can only be used to define locals, never
   data members. See below for an explanation and links to management
   classes suitable for use in different contexts. It should never be
   used in the same function as code that uses the CleanupStack API
   directly.

   This class template can be used to create and protect a resource
   handle of type T (typically a R-class) such that the instance of T
   referred to is automatically cleaned up when either of the
   following occur:

   - The referring local variable goes out of scope normally
   - The referring local variable goes out of scope due to an
	 untrapped leave causing the scope to be exited non-locally

   By default, the cleanup action is to call the Close() member
   function of the managed handle. An alternative cleanup strategy may
   be selected by specifying a cleanup strategy template class in the
   optional second template parameter position. The most common
   alternative cleanup strategies are predefined. It is also possible
   to specialize the default cleanup action for a given class using
   the DEFINE_CLEANUP_FUNCTION macro.

   The constructors of this class may leave.

   Any arguments supplied when initializing an instance of this class
   are automatically passed through to T's constructors.

   As a convenience, the methods of the managed handle may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
	// block scope example
	{
	LCleanedupHandle<RClosable> obj;
	obj->DoSomethingL(); // leave-safe
	if (obj->Finished())
		return; // RClosable::Close is invoked automatically
	obj->DoSomethingElseL(); // leave-safe
	// RClosable::Close is invoked automatically
	}
   @endcode

   Behind the scenes, this class template is implemented in terms of
   the thread-local CleanupStack, restricting its use to locals on the
   stack. This use of the CleanupStack ensures a consistent cleanup
   order between functions that call one another, even if they use
   different cleanup idioms.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TClose which implements the default Close() calling cleanup strategy
   @see TResetAndDestroy which implements an alternative
   ResetAndDestroy() calling cleanup strategy
   @see TFree which implements an alternative Free() calling cleanup
   strategy
   @see TDestroy which implements an alternative Destroy() calling
   cleanup strategy
   @see TRelease which implements an alternative Release() calling cleanup strategy
   @see LManagedHandle which has the same interface, but does not use the cleanup
   stack and is suitable for protecting the data members of classes
*/
template<typename T,
		 class CleanupStrategyType = TResourceCleanupStrategy>
class LCleanedupHandle: protected LAutoHandleBase<T, IS_HANDLE_SPECIAL(T)>
	{
	typedef LAutoHandleBase<T, IS_HANDLE_SPECIAL(T)> LAutoHandleBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;


/**
   Default constructor.
*/
	LCleanedupHandle()
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1>
	explicit LCleanedupHandle(const Param1& aParam1)
		: LAutoHandleBase(aParam1)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1>
	explicit LCleanedupHandle(Param1& aParam1)
		: LAutoHandleBase(aParam1)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1,
			 typename Param2>
	LCleanedupHandle(const Param1& aParam1,
					 const Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1,
			 typename Param2>
	LCleanedupHandle(const Param1& aParam1,
					 Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1,
			 typename Param2>
	LCleanedupHandle(Param1& aParam1,
					 const Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename Param1,
			 typename Param2>
	LCleanedupHandle(Param1& aParam1,
					 Param2& aParam2)
		: LAutoHandleBase(aParam1,
					   aParam2)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}


	~LCleanedupHandle()
		{
		ManagedPopCleanupStackItem(IsEnabled());
		}

/**
   Assigns a new resource to be managed.  If the LCleanedupHandle
   object already contains a managed resource handle, then the managed
   resource is released using the specified cleanup strategy before
   assigning the new managed resource.
 */
	template<typename U>
	LCleanedupHandle& operator=(const U& aHandle)
		{
		ReleaseResource();
		LAutoHandleBase::operator=(aHandle);
		return *this;
		}


/**
   If automatic resource management is enabled, calls the cleanup
   function defined by the cleanup strategy with the managed resource
   handle object and then disables the automatic resource management
   for this object.	 The cleanup strategy is specified by the
   CleanupStrategy template template parameter.	 The default cleanup
   strategy is to call the cleanup member function on the contained
   resource handle object. which is a member function named Close(),
   unless explicitly defined otherwise for the class of the object,
   for example by using the provided DEFINE_CLEANUP_FUNCTION macro.
*/
	void ReleaseResource()
		{
		if (!IsEnabled())
			return;

		CleanupStrategy::Cleanup(&Get());
		LAutoHandleBase::Disable();
		}

/**
   Disables the automatic resource management for this obkect and
   returns a copy of the resource handle.

   @return A copy of the resource handle.
*/
	using LAutoHandleBase::Unmanage;

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoHandleBase::IsEnabled;


/**
   Returns a reference to the resource handle.

   @return A reference to the resource handle.
*/
	using LAutoHandleBase::Get;


/**
   Overloaded indirection operator function.

   @return A reference to the resource handle.
*/
	using LAutoHandleBase::operator*;

/**
   Overloaded class member access operator function.

   @return A pointer to the resource handle.
*/
	using LAutoHandleBase::operator->;

	static void Cleanup(TAny* aPtr)
		{
		LCleanedupHandle* autoh = static_cast<LCleanedupHandle*>(aPtr);

		if (autoh->IsEnabled())
			{
			CleanupStrategy::Cleanup(&autoh->Get());
			}
		}

	using LAutoHandleBase::Disable;

	void Swap(LCleanedupHandle& aCleanedupHandle)
		{
		LAutoHandleBase::Swap(aCleanedupHandle);
		}
	};


/**
   Implementation base class - not designed for public inheritance or
   direct use.
   
   @internalComponent
*/
// Not for Client Use , Only to be used Internally.
template<typename T,
		 class CleanupStrategyType>
class LCleanedupPtrBase: protected LAutoPtrBase<typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType>
	{
	typedef LAutoPtrBase<typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType> LAutoPtrBase;

  protected:
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::ManagedType ManagedType;
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::BaseManagedType BaseManagedType;
	typedef typename TPtrCleanupTraits<T, CleanupStrategyType>::CleanupStrategy CleanupStrategy;

	LCleanedupPtrBase()
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	template<typename U>
	explicit LCleanedupPtrBase(U* aPtr)
		: LAutoPtrBase(aPtr)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

	~LCleanedupPtrBase()
		{
		ManagedPopCleanupStackItem(LAutoPtrBase::IsEnabled());
		}

	template<typename U>
	LCleanedupPtrBase& operator=(U* aPtr)
		{
		ReleaseResource();
		LAutoPtrBase::operator=(aPtr);
		return *this;
		}

	void ReleaseResource()
		{
		if (!LAutoPtrBase::IsEnabled())
			return;

		CleanupStrategy::Cleanup(static_cast<ManagedType*>(iPtr));
		LAutoPtrBase::Disable();
		}

	using LAutoPtrBase::Unmanage;

	using LAutoPtrBase::IsEnabled;

	using LAutoPtrBase::Get;

	using LAutoPtrBase::operator->;

	static void Cleanup(TAny* aPtr)
		{
		LCleanedupPtrBase* cleanupPtr = static_cast<LCleanedupPtrBase*>(aPtr);

		if (cleanupPtr->IsEnabled())
			{
			CleanupStrategy::Cleanup(static_cast<ManagedType*>(cleanupPtr->iPtr));
			}
		}

	using LAutoPtrBase::iPtr;

	void Swap(LCleanedupPtrBase& aCleanedupPtr)
		{
		LAutoPtrBase::Swap(aCleanedupPtr);
		}
	};


/**
   A class template that provides CleanupStack-based local-scope
   automatic management of pointers.

   @note This class can only be used to define locals, never
   data members. See below for an explanation and links to management
   classes suitable for use in different contexts. It should never be
   used in the same function as code that uses the CleanupStack API
   directly

   This class template can be used to protect a pointer to type T such
   that the instance of T referred to is automatically cleaned up
   when either of the following occur:

   - The referring local variable goes out of scope normally
   - The referring local variable goes out of scope due to an
	 untrapped leave causing the scope to be exited non-locally

   By default, the cleanup action is to delete the managed pointer
   using non-array delete. An alternative cleanup strategy may be
   selected by specifying a cleanup strategy template class in the
   optional second template parameter position. The most common
   alternative cleanup strategies are predefined.

   The constructors of this class may leave.

   As a convenience, the methods of the managed pointer may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
	// block scope example
	{
	LCleanedupPtr<CDynamic> autop(new(ELeave) CDynamic);
	autop->DoSomethingL(); // leave-safe
	if (autop->Finished())
		return; //	the pointer is deleted automatically when exiting from scope
	autop->DoSomethingElseL(); // leave-safe
	//	the pointer is deleted automatically when exiting from scope
	}
   @endcode

   Behind the scenes, this class template is implemented in terms of
   the thread-local CleanupStack, restricting its use to locals on the
   stack. This use of the CleanupStack ensures a consistent cleanup
   order between functions that call one another, even if they use
   different cleanup idioms.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TPointerDelete which implements the default deleting cleanup strategy
   @see TPointerFree which implements the alternative User::Free() cleanup strategy
   @see LManagedPtr which has the same interface, but does not use the cleanup
   stack and is suitable for protecting the data members of classes
*/
template<typename T,
		 class CleanupStrategyType = TPtrCleanupStrategy>
class LCleanedupPtr: protected LCleanedupPtrBase<T, CleanupStrategyType>
	{
	typedef LCleanedupPtrBase<T, CleanupStrategyType> LCleanedupPtrBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;


/**
   Default constructor.	 Constructs an empty LCleanedupPtr object.

   @post Get() == NULL
*/
	LCleanedupPtr()
		{
		}

/**
   Explicit constructor template.  Constructs a LCleanedupPtr object
   that manages the pointer aPtr of a type convertible to T* that can
   be cleaned up using the cleanup strategy of the LCleanedupPtr
   class.  The default cleanup strategy is to delete the pointer to a
   heap-allocated object by using non-array delete.	 Alternative
   cleanup strategies can be specified by using the CleanupStrategy
   template parameter of the LCleanedupPtr class template.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is of a type convertible to T* and can be cleaned up
   using the cleanup strategy.

   @post Get() == aPtr
*/
	explicit LCleanedupPtr(T* aPtr)
		: LCleanedupPtrBase(aPtr)
		{
		}

/**
   Assigns a new pointer to be managed.	 The new pointer must be of a
   type convertible to T* and it must be possible to use the cleanup
   strategy of the LCleanedupPtr object for the cleanup of the new
   managed pointer.	 If the LCleanedupPtr object already contains a
   managed pointer, then the cleanup strategy is invoked with the
   managed pointer before assigning the new managed pointer.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is a pointer of a type that is convertible to T* and can
   be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	LCleanedupPtr& operator=(T* aPtr)
		{
		LCleanedupPtrBase::operator=(aPtr);
		return *this;
		}

/**
   Assigns a new pointer to be managed.	 The new pointer must be of a
   type convertible to T* and it must be possible to use the cleanup
   strategy of the LCleanedupPtr object for the cleanup of the new
   managed pointer.	 If the LCleanedupPtr object already contains a
   managed pointer, then the cleanup strategy is invoked with the
   managed pointer before assigning the new managed pointer.

   @param aPtr A pointer of a type that is convertible to T* that can
   be cleaned up using the cleanup strategy.

   @pre aPtr is a pointer of a type that is convertible to T* and can
   be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	template<typename U>
	LCleanedupPtr& operator=(U* aPtr)
		{
		LCleanedupPtrBase::operator=(aPtr);
		return *this;
		}


/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked with the managed pointer and the automatic
   resource management is then disabled.  The underlying pointer is
   reset to NULL.

   @post Get() == NULL
*/
	using LCleanedupPtrBase::ReleaseResource;

/**
   Disables the automatic resource management for this object and
   returns a pointer to the object of type T.

   @return A pointer to the object of type T.
*/
	T* Unmanage()
		{
		return static_cast<T*>(LCleanedupPtrBase::Unmanage());
		}

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LCleanedupPtrBase::IsEnabled;

/**
   Returns a pointer to the managed object of type T.

   @return A pointer to the managed object of type T.
*/
	T* Get() const
		{
		return static_cast<T*>(iPtr);
		}

/**
   Overloaded indirection operator function.

   @return A reference to the managed object of type T.
*/
	T& operator*() const
		{
		return *(static_cast<T*>(iPtr));
		}

/**
   Overloaded class member access operator function.

   @return A pointer to the managed object of type T.
*/
	T* operator->() const
		{
		return static_cast<T*>(iPtr);
		}

// Implementation type - do not use
	typedef typename LCleanedupPtrBase::BaseManagedType* LCleanedupPtr<T, CleanupStrategy>::*TUnspecifiedBoolType;

/**
   Conversion operator that enables LCleanedupPtr objects to be used
   in boolean contexts.

   @return An unspecified value of an unspecified type convertible to
   boolean, which has a boolean value equal to Get() != NULL
 */
	operator TUnspecifiedBoolType()
		{
		return iPtr ? &LCleanedupPtr::iPtr : NULL;
		}

	using LCleanedupPtrBase::Disable;

	void Swap(LCleanedupPtr& aCleanedupPtr)
		{
		LCleanedupPtrBase::Swap(aCleanedupPtr);
		}

  private:
	using LCleanedupPtrBase::iPtr;
	};


// function template used for comparing two LCleanedupPtr-managed
// pointers for equality
template<typename T, typename U>
TBool operator==(const LCleanedupPtr<T>& aPtr1, const LCleanedupPtr<U>& aPtr2)
	{
	return aPtr1.Get() == aPtr2.Get();
	}

// function template used for comparing two LCleanedupPtr-managed
// pointers for inequality
template<typename T, typename U>
TBool operator!=(const LCleanedupPtr<T>& aPtr1, const LCleanedupPtr<U>& aPtr2)
	{
	return aPtr1.Get() != aPtr2.Get();
	}

// function template used for testing the ordering of two
// LCleanedupPtr-managed pointers
template<typename T, typename U>
TBool operator<(const LCleanedupPtr<T>& aPtr1, const LCleanedupPtr<U>& aPtr2)
	{
	return aPtr1.Get() < aPtr2.Get();
	}


/**
   A class template that provides CleanupStack-based local-scope
   automatic management of arrays.

   @note This class can only be used to define locals, never
   data members. See below for an explanation and links to management
   classes suitable for use in different contexts. It should never be
   used in the same function as code that uses the CleanupStack API
   directly

   @par

   @note This class can only be used with raw arrays, which are used
   only rarely on Symbian OS.  Instances of Symbian array container
   classes (e.g. RArray, RPointerArray) should be managed using the
   automatic management template classes appropriate for the array's
   type (LCleanedupHandle template classes for Symbian R arrays or
   LCleanedupPtr template classes for Symbian C arrays).

   This class template can be used to protect a heap-allocated array
   of objects of type T such that the array of T referred to is
   automatically cleaned up when either of the following occur:

   - The referring local variable goes out of scope normally
   - The referring local variable goes out of scope due to an
	 untrapped leave causing the scope to be exited non-locally

   The default cleanup strategy is to deallocate the managed array
   using arrray delete (delete[]), assuming that the array is
   heap-allocated.	An alternative cleanup strategy can be selected by
   specifying a cleanup strategy template class as the optional second
   template argument (corresponding to the CleanupStrategy template
   parameter).

   The constructors of this class may leave.

   As a convenience, the elements of the managed array may be accessed
   via "[]" notation directly on the management object.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   @code
	// block scope example
	{
	LCleanedupArray<TValue> arrayp(new(ELeave) TValue[KArraySize]);
	arrayp[0].DoSomethingL(); // leave-safe
	if (arrayp[0].Finished())
		return; //	the array is deleted automatically when exiting from scope
	arrayp[1].DoSomethingElseL(); // leave-safe
	//	the array is deleted automatically when exiting from scope
	}
   @endcode

   Behind the scenes, this class template is implemented in terms of
   the thread-local CleanupStack, restricting its use to locals on the
   stack. This use of the CleanupStack ensures a consistent cleanup
   order between functions that call one another, even if they use
   different cleanup idioms.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see LManagedArray which has the same interface, but does not use
   the cleanup stack and is suitable for protecting the data members
   of classes
*/
template<typename T,
		 class CleanupStrategyType = TArrayDelete>
class LCleanedupArray: protected LAutoPtrBase<T>
	{
	typedef LAutoPtrBase<T> LAutoPtrBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;

/**
   Default constructor.	 Constructs an empty LCleanedupArray object.

   @post Get() == NULL
 */
	LCleanedupArray()
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

/**
   Explicit constructor.  Constructs a LCleanedupArray object that
   manages an array of objects of type T that can be cleaned up using
   the cleanup strategy of the LCleanedupArray class.  The default
   cleanup strategy is to deallocate the heap-allocated array by using
   array delete.  An alternative cleanup strategy can be selected by
   specifying a cleanup strategy template class as the optional second
   template argument (corresponding to the CleanupStrategy template
   parameter).

   @param aPtr A pointer to the first element of an array of objects
   of type T, array that can be cleaned up using the cleanup strategy
   of the the LCleanedupArray class.

   @pre The array can be cleaned up using the cleanup strategy.

   @post Get() == aPtr
 */
	explicit LCleanedupArray(T* aPtr)
		: LAutoPtrBase(aPtr)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}


/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   pointer.
 */
	~LCleanedupArray()
		{
		ManagedPopCleanupStackItem(LAutoPtrBase::IsEnabled());
		}

/**
   Assigns a new array of objects of type T to be managed.	It needs
   to be be possible to use the cleanup strategy of the
   LCleanedupArray object for the cleanup of the new managed array.
   The default cleanup strategy is to delete the heap-allocated array
   by using array delete (delete[]).  If the LCleanedupArray object
   already manages an array, then the cleanup strategy is invoked with
   the managed array before assigning the new managed array.

   @param aPtr A pointer to the first element of the array of objects
   of type T - array that can be cleaned up using the cleanup
   strategy.

   @pre The new array to be managed can be cleaned up using the
   cleanup strategy.

   @post Get() == aPtr
 */
	LCleanedupArray& operator=(T* aPtr)
		{
		ReleaseResource();
		LAutoPtrBase::operator=(aPtr);
		return *this;
		}

/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked for the managed pointer and the automatic
   resource management is then disabled.  The underlying pointer is
   reset to NULL.

   @post Get() == NULL
*/
	void ReleaseResource()
		{
		if (!LAutoPtrBase::IsEnabled())
			return;

		CleanupStrategy::Cleanup(iPtr);
		iPtr = NULL;
		}


/**
   Disables the automatic resource management for this object and
   returns a pointer to the first element of the array of objects of
   type T.

   @return A pointer to the first element of the array of objects of
   type T.
*/
	using LAutoPtrBase::Unmanage;

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoPtrBase::IsEnabled;

/**
   Returns a pointer to the first element of the managed array of
   objects of type T.

   @return A pointer to the first element of the managed array of
   objects of type T.
*/
	using LAutoPtrBase::Get;

/**
   Overloaded subscript operator.

   @return A reference to the object of type T at the position aIndex.
 */
	T& operator[](TInt aIndex) const
		{
		return iPtr[aIndex];
		}

	static void Cleanup(TAny* aPtr)
		{
		LCleanedupArray* cleanupPtr = static_cast<LCleanedupArray*>(aPtr);

		if (cleanupPtr->IsEnabled())
			{
			CleanupStrategy::Cleanup(cleanupPtr->iPtr);
			}
		}

	using LAutoPtrBase::Disable;

	void Swap(LCleanedupArray& aArray)
		{
		LAutoPtrBase::Swap(aArray);
		}

  private:
	using LAutoPtrBase::iPtr;
	};


/**
   A class template that provides CleanupStack-based local-scope
   automatic management of references to resource handles (often
   instances of R-classes).

   @note This class can only be used to define locals, never
   data members. See below for an explanation and links to management
   classes suitable for use in different contexts. It should never be
   used in the same function as code that uses the CleanupStack API
   directly.

   Unlike LCleanedupHandle which creates a fresh instance of its
   managed type, this class template can be used to reference and
   protect an existing resource handle of type T (typically an
   R-class). The instance of T referred to has a cleanup operation run
   on it automatically when either of the following occur:

   - The referring local variable goes out of scope normally
   - The referring local variable goes out of scope due to an
	 untrapped leave causing the scope to be exited non-locally

   By default, the cleanup action is to call the Close() member
   function of the referenced handle. An alternative cleanup strategy
   may be selected by specifying a cleanup strategy template class in
   the optional second template parameter position. The most common
   alternative cleanup strategies are predefined. It is also possible
   to specialize the default cleanup action for a given class using
   the DEFINE_CLEANUP_FUNCTION macro.

   The constructors of this class may leave.

   As a convenience, the methods of the managed handle may be
   accessed via "->" notation directly on the management object, while
   "." notation is used to access the interface of the management
   object itself. Using "*" to dereference the management object
   yields a T&, and is often useful when passing the managed object as
   an argument.

   Automatic cleanup may be disabled at any time by calling
   Unmanage(), while cleanup may be forced at any time by calling
   ReleaseResource().

   Example:
   @code
	// block scope example
	void DoWithClosable(RClosable& aObj)
	  {
	  LCleanedupRef<RClosable> obj(aObj);
	  obj->DoSomethingL(); // leave-safe
	  if (obj->Finished())
		return; // RClosable::Close is invoked automatically
	  obj->DoSomethingElseL(); // leave-safe
	  // RClosable::Close is invoked automatically
	  }
   @endcode

   Behind the scenes, this class template is implemented in terms of
   the thread-local CleanupStack, restricting its use to locals on the
   stack. This use of the CleanupStack ensures a consistent cleanup
   order between functions that call one another, even if they use
   different cleanup idioms.

   This class template together with the cleanup strategy class
   templates provide a template-based implementation of the Strategy
   design pattern (See also: Policy-based design).

   @see TClose which implements the default Close() calling cleanup strategy
   @see TResetAndDestroy which implements an alternative
   ResetAndDestroy() calling cleanup strategy
   @see TFree which implements an alternative Free() calling cleanup
   strategy
   @see TDestroy which implements an alternative Destroy() calling
   cleanup strategy
   @see TRelease which implements an alternative Release() calling
   cleanup strategy
   @see LManagedRef which has the same interface, but does not use
   the cleanup stack and is suitable for protecting the data members of
   classes
   @see LCleanedupHandle which has a similar interface but creates a
   fresh local instance of T
*/
template<typename T,
		 class CleanupStrategyType = TResourceCleanupStrategy>
class LCleanedupRef: protected LAutoRefBase<T>
	{
	typedef LAutoRefBase<T> LAutoRefBase;

  public:
	typedef T ManagedType;
	typedef CleanupStrategyType CleanupStrategy;

/**
   Explicit constructor.
 */
	template<typename U>
	explicit LCleanedupRef(U& aRef)
		: LAutoRefBase(aRef)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

/**
   Destructor.	When automatic resource management is enabled, the
   destructor invokes the specified cleanup strategy for the managed
   reference.
 */
	~LCleanedupRef()
		{
		ManagedPopCleanupStackItem(LAutoRefBase::IsEnabled());
		}

/**
   Assigns a new reference to be managed.  If the LCleanedupRef
   object already contains a managed reference, then the specified
   cleanup strategy is invoked for the managed reference before
   assigning the new managed reference.
 */
	template<typename U>
	LCleanedupRef& operator=(U& aRef)
		{
		ReleaseResource();
		LAutoRefBase::operator=(aRef);
		return *this;
		}

/**
   If automatic resource management is enabled, the specified cleanup
   strategy is invoked for the managed reference and the automatic
   resource management is then disabled.
*/
	void ReleaseResource()
		{
		if (!LAutoRefBase::IsEnabled())
			return;

		CleanupStrategy::Cleanup(iPtr);
		iPtr = NULL;
		}

/**
   Disables the automatic resource management for this object and
   returns a reference to the object of type T.

   @return A reference to the object of type T.
*/
	using LAutoRefBase::Unmanage;

/**
   Returns ETrue if automatic resource management is enabled; EFalse
   otherwise.

   @return ETrue if automatic resource management is enabled; EFalse
   otherwise.
*/
	using LAutoRefBase::IsEnabled;

/**
   Returns a reference to the managed object of type T.

   @return A reference to the managed object of type T.
*/
	using LAutoRefBase::Get;

/**
   Overloaded indirection operator function.

   @return A reference to the managed object of type T.
*/
	using LAutoRefBase::operator*;

/**
   Overloaded class member access operator function.

   @return A pointer to the managed object of type T.
*/
	using LAutoRefBase::operator->;


	static void Cleanup(TAny* aPtr)
		{
		LCleanedupRef* cleanupRef = static_cast<LCleanedupRef*>(aPtr);

		if (cleanupRef->IsEnabled())
			{
			CleanupStrategy::Cleanup(cleanupRef->iPtr);
			}
		}

	using LAutoRefBase::Disable;

	void Swap(LCleanedupRef& aRef)
		{
		LAutoRefBase::Swap(aRef);
		}

  private:
	using LAutoRefBase::iPtr;
	};


/**
   A class that provides automatic cleanup using a TCleanupOperation
   on the destruction of the LManagedGuard object.

   @note This class can only be used to define object scoped cleanup
   to guard object destruction, never local stack scoped cleanup. See
   below for an explanation and links to management classes suitable
   for use in different contexts.

   This class can be used to manage a TCleanupOperation in such a way
   that the specified cleanup operation is guaranteed to be called
   when the guarding object is destroyed; typically when the object
   containing it is deleted.

   The constructors of this class never leave, so data members defined with
   this type may be initialized safely during any phase of
   construction of the owning class.

   Automatic cleanup may be disabled at any time by calling
   Dismiss(), while cleanup may be forced at any time by calling
   Execute().

   @code
   class CComposite : public CBase
	   {
	 public:
	   CONSTRUCTORS_MAY_LEAVE

	   CComposite(RCleanable* aObj)
		   : iObj(RCleanable::Cleanup, aObj)
		   {
		   }

	   ~CComposite()
		   {
		   // RCleanable::Cleanup(iObj) is automatically invoked
		   }

	 private:
	   LManagedGuard<RCleanable> iObj;
	   };
   @endcode

   Behind the scenes, this class template simply relies on reliable
   execution of its destructor. If used for a local variable rather
   than a data member, cleanup will occur but out-of-order compared to
   objects protected using the LCleanupXxx variants or the
   CleanupStack directly. Therefore it is not recommended for use in
   that context.

   These management classes may be used as the basis for implementing
   leave-safe single-phase construction, since fully initialized
   data members protected in this way will get destroyed (so reliably
   triggering cleanup) if their containing classes leave during
   execution of their constructors. Note, however, that single-phase
   construction must be explicitly enabled in the containing class
   using the CONSTRUCTORS_MAY_LEAVE macro.

   @see LCleanedupGuard which has the same interface, but uses the cleanup
   stack and is suitable for use as a local to guard local scope exit
   @see CONSTRUCTORS_MAY_LEAVE
*/
class LManagedGuard
	{
  public:
/**
   Constructor.	 Creates a LCleanedupGuard object that, when enabled,
   automatically invokes upon destruction a cleanup operation
   specified by the aCleanupOperation parameter with the pointer to
   data specified by the aData parameter.

   @param aCleanupOperation A cleanup operation.
   @param aData Pointer to data to be passed to the cleanup operation
 */
	LManagedGuard(TCleanupOperation aCleanupOperation, TAny* aData = 0)
		: iCleanupOperation(aCleanupOperation),
		  iData(aData)
		{
		}

/**
   Destructor.
 */
	~LManagedGuard()
		{
		Execute();
		}

/**
   Executes the guard cleanup operation.
*/
	void Execute()
		{
		if (iCleanupOperation)
			{
			iCleanupOperation(iData);
			}
		}

/**
   Disables the guard.
*/
	void Dismiss()
		{
		iCleanupOperation = NULL;
		}

  private:
	LManagedGuard(const LManagedGuard&);
	LManagedGuard& operator=(const LManagedGuard&);

	TCleanupOperation iCleanupOperation;
	TAny* iData;
	};


/**
   A class that provides CleanupStack-based local-scope automatic
   cleanup using a TCleanupOperation on the destruction of the
   LManagedGuard object.

   @note This class can only be used to define a local stack scoped
   cleanup, never an object scoped cleanup to guard object
   destruction. See below for an explanation and links to management
   classes suitable for use in different contexts.

   This class can be used to manage a TCleanupOperation in such a way
   that the specified cleanup operation is guaranteed to be called
   when either of the following occur:

   - The guarding local variable goes out of scope normally
   - The guarding local variable goes out of scope due to an
	 untrapped leave causing the scope to be exited non-locally

   The constructors of this class may leave.

   Automatic cleanup may be disabled at any time by calling
   Dismiss(), while cleanup may be forced at any time by calling
   Execute().

   @code
	// block scope example
	{
	RCleanable obj;
	LCleanedupGuard cleanGuard(RCleanable::Cleanup, &obj);

	obj.DoSomethingL(); // leave-safe
	if (Finished())
		return; // RCleanable::Cleanup is invoked automatically when exiting from scope
	obj.DoSomethingElseL(); // leave-safe
	//	RCleanable::Cleanup is invoked automatically when exiting from scope
	}
   @endcode

   Behind the scenes, this class template is implemented in terms of
   the thread-local CleanupStack, restricting its use to local stack
   scope. This use of the CleanupStack ensures a consistent cleanup
   order between functions that call one another, even if they use
   different cleanup idioms.

   @see LManagedGuard which has the same interface, but does not use the cleanup
   stack and is suitable for use as the data member of a class to guard
   object destruction.
*/
class LCleanedupGuard
	{
  public:
/**
   Constructor.	 Creates a LCleanedupGuard object that, when enabled,
   automatically invokes upon destruction a cleanup operation
   specified by the aCleanupOperation parameter with the pointer to
   data specified by the aData parameter.

   @param aCleanupOperation A cleanup operation.
   @param aData Pointer to data to be passed to the cleanup operation
 */
	LCleanedupGuard(TCleanupOperation aCleanupOperation, TAny* aData = 0)
		: iCleanupOperation(aCleanupOperation),
		  iData(aData)
		{
		CleanupStack::PushL(TCleanupItem(Cleanup, this));
		}

/**
   Destructor.
 */
	~LCleanedupGuard()
		{
		ManagedPopCleanupStackItem(iCleanupOperation);
		}

/**
   Executes the guard cleanup operation.
*/
	void Execute()
		{
		if (iCleanupOperation)
			{
			iCleanupOperation(iData);
			}
		}

/**
   Disables the guard.
*/
	void Dismiss()
		{
		iCleanupOperation = NULL;
		}

	static void Cleanup(TAny* aPtr)
		{
		LCleanedupGuard* guard = static_cast<LCleanedupGuard*>(aPtr);
		guard->Execute();
		}

  private:
	LCleanedupGuard(const LCleanedupGuard&);
	LCleanedupGuard& operator=(const LCleanedupGuard&);


	TCleanupOperation iCleanupOperation;
	TAny* iData;
	};

#endif // !EMANAGED_H

