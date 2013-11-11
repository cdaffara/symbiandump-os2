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

#include <e32std.h>
#include <f32file.h>
#include <e32test.h>
#include <euserhl.h>



// Note: Methods are defined inline within classes here simply to make
// the code shorter, keep related code closer together, and hopefully
// make things easier to follow.

RTest test(_L("EuserHl Walkthrough"));

// Some dummy methods and data used in the walkthroughs below
_LIT(KFill, "XXX");
_LIT(KPath, "c:\\a\\b\\c");
_LIT(KOne, "One ");
_LIT(KTwo, "Two ");
_LIT(KTesting, "Testing ");

void MaybeLeaveL()
	{
	// Some code that may leave
	}

HBufC* AllocateNameL(const TDesC& aDes)
	{
	return aDes.AllocL();
	}

void ReadToMax(TDes& aDes)
	{
	aDes.SetMax();
	aDes.Repeat(KFill);
	}

void GetCurrentPath(TDes& aDes)
	{
	aDes = KPath;
	}

void GetCurrentPathStringL(LString& aString)
	{
	aString = L"c:\\a\\b\\c"; // Will auto-grow if necessary, may leave
	}

LString AppendCurrentPathStringL(LString aString)
	{
	return aString+= L"c:\\a\\b\\c";
	}

class CTicker : public CBase
	{
public:
	void Tick() { ++iTicks; }
	void Tock() { ++iTocks; }

	void Zap() { delete this; }

public:
	TInt iTicks;
	TInt iTocks;
	};

// Defines a custom pointer cleanup policy that calls the Zap member
class TTickerZapStrategy
	{
public:
	static void Cleanup(CTicker* aPtr)
		{
		// The general template/class scaffolding remains the same
		// for all custom cleanups, just this cleanup body varies
		aPtr->Zap();
		test.Printf(_L("Zapped CTicker\n"));
		}
	};

void RegisterTicker(CTicker& aTicker)
	{
	(void)aTicker;
	}

void RegisterTickerPtr(CTicker* aTicker)
	{
	(void)aTicker;
	}

void TakeTickerOwnership(CTicker* aTicker)
	{
	delete aTicker;
	}

void RegisterTimer(RTimer& aTimer)
	{
	(void)aTimer;
	}

// Defines a custom handle cleanup policy that calls Cancel then Close
class TCancelClose
	{
public:
	template <class T>
	static void Cleanup(T* aHandle)
		{
		// The general template/class scaffolding remains the same
		// for all custom cleanups, just this cleanup body varies
		aHandle->Cancel();
		aHandle->Close();
		test.Printf(_L("Cancel Closed RTimer\n"));
		}
	};

void BespokeCleanupFunction(TAny* aData)
	{
	(void)aData;
	test.Printf(_L("BespokeCleanupFunction\n"));
	}

// The walkthroughs themselves

// This class demonstrates the use of an embedded LString in the
// conventional Symbian two-phase construction pattern. We've chosen
// to implement the temporary leave protection in NewL in terms of
// LCleanedupPtr instead of the the CleanupStack API in this example.
class CStringUserTwoPhase : public CBase
	{
public:
	static CStringUserTwoPhase* NewL(const TDesC& aName)
		{
		// We can use the resource management utility classes in
		// two-phase if we want to
		LCleanedupPtr<CStringUserTwoPhase> self(new(ELeave) CStringUserTwoPhase);
		self->ConstructL(aName);
		// Calling Unmanage() disables cleanup and yields the
		// previously managed pointer so that it can be safely
		// returned
		return self.Unmanage(); 
		}

	virtual void ConstructL(const TDesC& aName)
		{
		// This assignment may leave if LString fails to allocate a
		// heap buffer large enough to hold the data in aName
		iName = aName; 
		}

	~CStringUserTwoPhase()
		{
		// The iName LString cleans up after itself automatically 
		}

	const TDesC& Name() 
		{
		// We can just return an LString directly as a const TDesC
		return iName; 
		}

protected:
	CStringUserTwoPhase()
		{
		// Everything interesting happens in ConstructL in this
		// version. 

		// Default initialization of the iName LString does not
		// allocate a heap buffer, and so cannot leave. As long as
		// initialization is deferred to ConstructL, LStrings can be
		// used safely with two-phase construction.
		}

protected:
	LString iName;
	};

// This class demonstrates the use of an embedded LString in the
// single-phase construction pattern, where a leave-safe constructor
// fully initializes the object. 
//
// Note that where a class's constructor forms part of its exported
// public or protected contract, switching from a non-leaving to a
// potentially leaving constructor would be a BC break. On the other
// hand, if instantiation is entirely encapsulated within factory
// functions like NewL, there is no such BC restriction.
class CStringUserSinglePhase : public CBase
	{
public:
	// This macro is necessary to ensure cleanup is correctly handled
	// in the event that a constructor may leave beneath a call to
	// new(ELeave)
	CONSTRUCTORS_MAY_LEAVE

	static CStringUserSinglePhase* NewL(const TDesC& aName)
		{
		return new(ELeave) CStringUserSinglePhase(aName);
		}

	~CStringUserSinglePhase()
		{
		// The iName LString cleans up after itself automatically
		}

	const TDesC& Name() 
		{
		// We can just return an LString directly as a const TDesC
		return iName;
		}

protected:
	CStringUserSinglePhase(const TDesC& aName)
		// This initialization of iName may leave because LString
		// needs to allocate a heap buffer to copy the aName string
		// data into
		: iName(aName) 
		{
		// If iName initialization is successful but the constructor
		// then goes on to leave later, iName (like all fields fully
		// constructed at the point of a leave in a constructor) will
		// be destructed, and so clean up after itself
		MaybeLeaveL();
		}

protected:
	LString iName;
	};


void WalkthroughStringsL()
	{

		{
		// Trivially exercise the LString using classes defined above

		LCleanedupPtr<CStringUserTwoPhase> one(CStringUserTwoPhase::NewL(KOne));
		test.Printf(_L("Single phase name: %S\n"), &one->Name());

		LCleanedupPtr<CStringUserSinglePhase> two(CStringUserSinglePhase::NewL(KTwo));
		test.Printf(_L("Two phase name: %S\n"), &two->Name());

		// Both instances are automatically deleted as we go out of scope
		}

		{
		// A default constructed LString starts empty, doesn't
		// allocate any memory on the heap, and therefore the
		// following cannot leave
		LString s;

		// But it will grow on demand if you assign to it, so it has
		// enough space to hold the copied string data, and so
		// assignment may leave
		s = L"One ";

		// Similarly if you append to it with the leaving variant of
		// Append, AppendL, if may grow on demand
		s.AppendL(L"Two ");

		// The += operator for LString also maps to AppendL
		s += L"Three ";

		// You can also use new leaving format methods that also grow
		// on demand
		s.AppendFormatL(KTesting);

		// This general style of use of LString may be preferable to
		// typical descriptor use for a number of reasons e.g. it
		// avoids the common temptation to set an artificial maximum
		// buffer size; it avoids massive conservative over-allocation
		// when the average case length of a string is far less than
		// the worst-case maximum; it will not surprise you (compared
		// to the alternative of a large stack-allocated TBuf) by
		// triggering stack overflow.

		// An LString can be printed the same way as any descriptor
		test.Printf(_L("Value: %S\n"), &s);

		// An LString supports all TDesC and TDes methods
		// LString findToken(L"Two ");
		test(s.Find(L"Two ") == 4);
		
		// LString matchPattern(L"*Two* ");
		test(s.Match(L"*Two*") == 4);
		test(s.Match(L"*T?o*") == 4);
		
		// LString compare(L"some string");
		test(s.Compare(L"One Two Three Testing ") == 0);
		test(s.Compare(L"One Two Three Testing! ") < 0);
		test(s.Compare(L"One Two Testing ") > 0);
		
		// also LString ==,!=,>,<,<=,>=(L"some string");
		test(s == L"One Two Three Testing ");
		test(s < L"One Two Three Testing! ");
		test(s > L"One Two Testing ");
		test(s != L"not equal");
		
		// An LString supports all TDesC and TDes operators
		test(s[4] == TChar('T'));

		TInt untrimmed = s.Length();
		s.Trim();
		test(s.Length() == untrimmed - 1);

		s.UpperCase();
		test.Printf(_L("UpperCase: %S\n"), &s);
		s.LowerCase();
		test.Printf(_L("LowerCase: %S\n"), &s);

		// The underlying heap allocated buffer is released
		// automatically when the LString goes out of scope, either
		// normally or through a leave
		}
		{
		// Copy, Append,Insert,Replace,Justify the same way as TDesC and TDes
		
		LString s;
		// Copies data into this 8-bit string descriptor, replacing any existing
		// data, and expanding its heap buffer to accommodate if necessary.
		// leaves on not being able to accomodate the new content
		// both AssignL and += use CopyL internally
		s.CopyL(L"new way of dealing with strings");
		s.CopyUCL(L"new way of dealing with strings");
		test(s == L"NEW WAY OF DEALING WITH STRINGS");
		
		// Insert data into this descriptor.
		// The length of this descriptor is changed to reflect the extra data.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL(L"Some Content Can Be Into This String");
		s.InsertL(20,L"Inserted ");
		test(s == L"Some Content Can Be Inserted Into This String");
		
		// Replace data in this descriptor.
		// The specified length can be different to the length of the replacement data.
		// The length of this descriptor changes to reflect the change of data.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL(L"Some Content Can Be Decalper");
		s.ReplaceL(20,8,L"Replaced");
		test(s == L"Some Content Can Be Replaced");
		
		// Append data onto the end of this descriptor's data.
		// The length of this descriptor is incremented to reflect the new content.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL(L"Try appending ");
		s.AppendL(L"Try appending some more",3);
		test(s == L"Try appending Try");
		
		// Copy data into this descriptor and justifies it, replacing any existing data.
		// The length of this descriptor is set to reflect the new data.
		// The target area is considered to be an area of specified width positioned at
		// the beginning of this descriptor's data area. Source data is copied into, and
		// aligned within this target area according to the specified alignment
		// instruction.
		// If the length of the target area is larger than the length of the source, then
		// spare space within the target area is padded with the fill character.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL(L"Justified");
		s.JustifyL(L"Just",9,ERight,'x');
		test(s == L"xxxxxJust");
		
		// Append data onto the end of this descriptor's data and justifies it.
		// The source of the appended data is a memory location.
		// The target area is considered to be an area of specified width, immediately 
		// following this descriptor's existing data. Source data is copied into, and 
		// aligned within, this target area according to the specified alignment instruction.
		// If the length of the target area is larger than the length of the source, 
		// then spare space within the target area is padded with the fill character.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL(L"One ");
		s.AppendJustifyL(L"Two Three",3,7,ERight,'x');
		test(s == L"One xxxxTwo" );
		
		}
		{
		// You can initialize with a MaxLength value
		LString s(KMaxFileName); // This operation may leave
		test(s.MaxLength() == KMaxFileName);

		// And you can dynamically adjust MaxLength later using 
		// SetMaxLengthL if you want an exact allocated size
		// Setting MaxLength on construction or via SetMaxLengthL is
		// exact; calling MaxLength() immediately afterwards is
		// guaranteed to return exactly the value you specified
		s.SetMaxLengthL(2 * KMaxFileName);
		test(s.MaxLength() == 2 * KMaxFileName);

		// Pre-setting MaxLength is important when passing an LString
		// as a TDes to a library function, because the LString can't
		// be auto-grown via the TDes API

		}

		{
		// You can initialize from any descriptor/literal/[wide]character string and the
		// string data is copied into the LString
		LString s(L"One "); // From a character string
		s += L"Two ";
		LString half(s.Left(s.Length() / 2)); // Left returns a TPtrC
		test.Printf(_L("All: %S, Half: %S\n"), &s, &half);

		// On the other hand, you can initialize from a returned
		// HBufC* and the LString automatically takes ownership
		LString own(AllocateNameL(KTesting));
		test.Printf(_L("What I own: %S\n"), &own);

		// Following that you can re-assign an HBufC to an existing
		// string using the assignment operator 
		// taking ownership of the new content. 
		own = AllocateNameL(KTesting);
		
		// Following that you can re-assign an HBufC to an existing
		// string. The string destroys its original content before
		// taking ownership of the new content. 
		own.Assign(AllocateNameL(KTesting));
		
		// The content of one string can similarly be assigned
		// to another to avoid copying. In this example, the content 
		// is detached from 's' and transfered to 'own'.  
		own.Assign(s);
		
		// The same content transfer can be achieved from an RBuf to a
		// string. You may need to do this if a legacy method returns
		// you an RBuf. The RBuf is emptied of its content.
		RBuf16 buf;
		buf.CreateL(KOne);
		own.Assign(buf);
	
		// You can also assign a simple text array to a string as its
		// new buffer. This method initialises the length to zero.   
		own.Assign((TText*)User::Alloc(24*(TInt)sizeof(TText)), 24);
		
		// If the buffer has already been filled with some characters
		// then you supply the length in this alternative Assign method.   
		own.Assign((TText*)User::Alloc(24*(TInt)sizeof(TText)), 12,24);

		// Each Assign destroys the old content before assuming ownership
		// of the new.
		// As usual the last content of the string is destroyed when the 
		// LString goes out of scope
		}

		{
		// You can reserve extra free space in preparation for an 
		// operation that adds characters to the string. You may
		// need to do this when you cannot use any of the auto-buffer
		// extending LString methods to achieve your objective.
		LString s(L"One ");
		s.ReserveFreeCapacityL(4);
		test(s.Length() == 4);
		test(s.MaxLength() >= 8);

		// Almost all the methods that may extended the string buffer,
		// including the explicit ReserveFreeCapacityL, but excluding
		// SetMaxLengthL, attempt to grow the size exponentially. 
		// The Exponential growth pattern is expected to give better 
		// performance at an amortised complexity of O(n) when adding n characters.
		// If the exponential growth is less than the supplied extra size
		// then the supplied size is used instead to save time.
		// The exponential growth is used in anticipation of further additions
		// to a string. This trades-off speed efficiency for space efficiency.
		// If required you may be able to swap the oversized buffer for 
		// a more compact one using:
		s.Compress();
		test(s.MaxLength() >= 4);	//note indefinite test
		    
		// Resize attempts to re-allocate a smaller buffer to copy
		// the content into. If the new memory cannot be allocated then the
		// original string is left unaffected. 
		
		// When you have finished using the content of a string you can
		// get its buffer released without destroying the string itself. 
		// You may want to do this when using member declared strings.
		// Automatic strings are destroyed when they go out of scope.
		s.Reset();
		test(s.Length() == 0);
		test(s.MaxLength() == 0);
		
		}

		{
		// An LString can be passed directly to any function requiring
		// a const TDesC&
		TInt year = 2009;

		LString s;
		s.FormatL(_L("Happy New Year %d"), year);
		// InfoPrint takes a const TDesC&
		User::InfoPrint(s);

		LString pattern;
		pattern.FormatL(_L("*Year %d"), year);
		// Match takes a const TDesC& as a pattern
		TInt loc = s.Match(pattern);
		test(loc == 10);
		}

		{
		// An LString can be passed directly to any function requiring
		// a TDes& but care must always be taken to pre-set MaxLength
		// since LStrings can't be automatically grown via the TDes
		// interface

		LString s;
		// Calling GetCurrentPath(s) now would panic because LStrings
		// are initialized by default to MaxLength 0.  Although s is
		// an LString GetCurrentPath takes a TDes& and so inside the function
		// s behaves as a TDes and would panic with USER 11 if the resulting 
		// new length of s is greater than its maximum length.
		test(s.MaxLength() == 0);

		// Calling SetMaxLengthL will automatically realloc the
		// underlying buffer if required, and is guaranteed to leave
		// MaxLength() equal to the specified value
		s.SetMaxLengthL(KMaxFileName);
		GetCurrentPath(s);
		//LString pathString(L"c:\\a\\b\\c");
		test.Printf(_L("Path: %S\n"), &s);
		test(s == L"c:\\a\\b\\c");

		// If SetMaxLengthL adjusts MaxLength lower than the current
		// Length, the data is truncated to the new MaxLength and
		// Length set to the new MaxLength
		s.SetMaxLengthL(s.Length() / 2);
		test.Printf(_L("Truncated path: %S\n"), &s);
		test(s.Length() == s.MaxLength());

		// An initial MaxLength can be specified when constructing an
		// LString. Note that unlike the default constructor, this
		// variant allocates and may leave.
		LString s2(KMaxFileName);
		GetCurrentPath(s2);
		test.Printf(_L("Path: %S\n"), &s2);
		test(s2 == L"c:\\a\\b\\c");

		// Your code and APIs can benefit from LString's auto-growth
		// behaviour by accepting an LString to fill in as an output
		// parameter. Using LString rather than TDes parameters means 
		// that the function is able to safely increase the size of the 
		// string as the LString will re-allocate as necessary
		LString s3;
		// GetCurrentPathStringL takes an LString&
		GetCurrentPathStringL(s3);
		test.Printf(_L("Path: %S\n"), &s3);
		test(s3 == L"c:\\a\\b\\c");

		// As a well-defined value class, if you want to, LStrings can
		// be passed and returned by value. This is relatively
		// inefficient however due to the amount of copying and heap
		// reallocation involved. 
		LString s4(AppendCurrentPathStringL(s3));
		test.Printf(_L("Appended path: %S\n"), &s4);
		test(s4.Length() == s3.Length() * 2);
		}

		{
		// LStrings can be allocated on the heap if necessary. 
		// Then it can managed as part of an array of string pointers
		TInt n = 5;
		LCleanedupHandle<RPointerArray<LString>, TResetAndDestroy> sarray;

		for (TInt i = 0; i < n; ++i) 
			{
			LString* s = new(ELeave) LString;
			s->FormatL(_L("String %d"), i);
			sarray->Append(s);
			}

		for (TInt i = 0, n = sarray->Count(); i < n; ++i) 
			{
			LString tmp;
			tmp.FormatL(_L("String %d"), i);
			test(tmp == *(*sarray)[i]);
			test.Printf(_L("String %d = %S\n"), i, (*sarray)[i]);
			}

		}

		{
		// Any allocation failure in new(ELeave)LString throws
		// KErrNoMemory and cleans up after itself fully

		__UHEAP_MARK;
		//coverity[resource_leak]
		//As mentioned in the comment above any allocation failure is taken care of
		TRAPD(status, new(ELeave) LString(100 * 1024 * 1024));
		test(status == KErrNoMemory);
		__UHEAP_MARKEND;
		}

		{
		// Native C arrays (both heap and stack allocated) of LStrings
		// also work, although their use is not recommended

		TInt n = 5;
		LCleanedupArray<LString> sarray(new(ELeave) LString[n]);

		for (TInt i = 0; i < n; ++i) 
			{
			sarray[i].FormatL(_L("String %d"), i);
			}

		for (TInt i = 0; i < n; ++i) 
			{
			LString tmp;
			tmp.FormatL(_L("String %d"), i);
			test(tmp == sarray[i]);
			test.Printf(_L("String %d = %S\n"), i, &sarray[i]);
			}

		}
		{
		// 8-bit wide null terminated character string support
		
		// A default constructed LString8 starts empty, doesn't
		// allocate any memory on the heap, and therefore the
		// following cannot leave
		LString8 s;

		// But it will grow on demand if you assign to it, so it has
		// enough space to hold the copied string data, and so
		// assignment may leave
		s ="One ";

		// Similarly if you append to it with the leaving variant of
		// Append, AppendL, if may grow on demand
		s.AppendL("Two ");

		// The += operator for LString8 also maps to AppendL
		s +="Three ";
		s +="Testing ";

		// An LString8 can be printed the same way as any descriptor
		test.Printf(_L("Value: %S \n"), &s);

		// An LString8 can be compared the same way as any descriptor
		test(s == "One Two Three Testing ");

		// An LString8 supports all TDesC and TDes methods
		// LString findToken("Two ");
		test(s.Find("Two ") == 4);
		
		// LString8 matchPattern("*Two* ");
		test(s.Match("*Two*") == 4);
		test(s.Match("*T?o*") == 4);
		
		// LString8 compare("some string");
		test(s.Compare("One Two Three Testing ") == 0);
		test(s.Compare("One Two Three Testing! ") < 0);
		test(s.Compare("One Two Testing ") > 0);
		
		// also LString8 ==,!=,>,<,<=,>=(L"some string");
		test(s == "One Two Three Testing ");
		test(s < "One Two Three Testing! ");
		test(s > "One Two Testing ");
		test(s != "not equal");
		
		// Copies data into this 8-bit string descriptor, replacing any existing
		// data, and expanding its heap buffer to accommodate if necessary.
		// leaves on not being able to accomodate the new content
		// both AssignL and += use CopyL internally
		s.CopyL("new way of dealing with strings");
		

		// Copy, Append,Insert,Replace,Justify the same way as TDesC8 and TDes8

		// Copies data into this 8-bit string descriptor, replacing any existing
		// data, and expanding its heap buffer to accommodate if necessary.
		// leaves on not being able to accomodate the new content
		// both AssignL and += use CopyL internally
		s.CopyL("new way of dealing with strings");
		s.CopyUCL("new way of dealing with strings");
		test(s == "NEW WAY OF DEALING WITH STRINGS");
		
		// Insert data into this descriptor.
		// The length of this descriptor is changed to reflect the extra data.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL("Some Content Can Be Into This String");
		s.InsertL(20,"Inserted ");
		test(s == "Some Content Can Be Inserted Into This String");
		
		// Replace data in this descriptor.
		// The specified length can be different to the length of the replacement data.
		// The length of this descriptor changes to reflect the change of data.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL("Some Content Can Be Decalper");
		s.ReplaceL(20,8,"Replaced");
		test(s == "Some Content Can Be Replaced");
		
		// Append data onto the end of this descriptor's data.
		// The length of this descriptor is incremented to reflect the new content.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL("Try appending ");
		s.AppendL("Try appending some more",3);
		test(s == "Try appending Try");
		
		// Copy data into this descriptor and justifies it, replacing any existing data.
		// The length of this descriptor is set to reflect the new data.
		// The target area is considered to be an area of specified width positioned at
		// the beginning of this descriptor's data area. Source data is copied into, and
		// aligned within this target area according to the specified alignment
		// instruction.
		// If the length of the target area is larger than the length of the source, then
		// spare space within the target area is padded with the fill character.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL("Justified");
		s.JustifyL("Just",9,ERight,'x');
		test(s == "xxxxxJust");
		
		// Append data onto the end of this descriptor's data and justifies it.
		// The source of the appended data is a memory location.
		// The target area is considered to be an area of specified width, immediately 
		// following this descriptor's existing data. Source data is copied into, and 
		// aligned within, this target area according to the specified alignment instruction.
		// If the length of the target area is larger than the length of the source, 
		// then spare space within the target area is padded with the fill character.
		// This leaving variant of the standard, non-leaving descriptor method
		// differs in that this operation may cause the string descriptor's heap
		// buffer to be reallocated in order to accommodate the new data. As a
		// result, MaxLength() and Ptr() may return different values afterwards,
		// and any existing raw pointers to into the descriptor data may be
		// invalidated.
		s.CopyL("One ");
		s.AppendJustifyL("Two Three",3,7,ERight,'x');
		test(s == "One xxxxTwo" );
		
		}
		
	}

// This class demonstrates the use of the embeddable management
// classes in a conventional Symbian two-phase construction
// pattern. 
class CManagedUserTwoPhase : public CBase
	{
public:
	static CManagedUserTwoPhase* NewL(CTicker* aTicker)
		{
		// We can use the resource management utility classes in
		// two-phase if we want to
		LCleanedupPtr<CManagedUserTwoPhase> self(new(ELeave) CManagedUserTwoPhase);
		self->ConstructL(aTicker);
		// Calling Unmanage() disables cleanup and yields the
		// previously managed pointer so that it can be safely
		// returned
		return self.Unmanage(); 
		}

	~CManagedUserTwoPhase()
		{
		// The iTicker manager will automatically delete the CTicker
		// The iTimer manager will automatically Close() the RTimer
		}

	CTicker& Ticker()
		{
		// If we dereference the management object we get a CTicker&
		return *iTicker;
		}

	RTimer& Timer()
		{
		// If we dereference the management object we get an RTimer&
		return *iTimer;
		}

private:
	
	virtual void ConstructL(CTicker* aTicker)
		{
		// Take ownership and manage aTicker 
		iTicker = aTicker; 

		// Note use of -> to indirect through the management wrapper
		iTimer->CreateLocal() OR_LEAVE; 
		}
	
	CManagedUserTwoPhase()
		{
		// Everything interesting happens in ConstructL in this
		// version. 

		// Default initialization of the iName LString does not
		// allocate a heap buffer, and so cannot leave. As long as
		// initialization is deferred to ConstructL, LStrings can be
		// used safely with two-phase construction.
		}

private:
	// We have to use LManagedXxx for fields, not LCleanedupXxx
	LManagedPtr<CTicker> iTicker;
	LManagedHandle<RTimer> iTimer;
	};

// This class demonstrates the use of embedded management classes in
// the single-phase construction pattern, where a leave-safe
// constructor fully initializes the object.
//
// Note that where a class's constructor forms part of its exported
// public or protected contract, switching from a non-leaving to a
// potentially leaving constructor would be a BC break. On the other
// hand, if instantiation is entirely encapsulated within factory
// functions like NewL, there is no such BC restriction.

class CManagedUserSinglePhase : public CBase
	{
public:
	// This macro is necessary to ensure cleanup is correctly handled
	// in the event that a constructor may leave beneath a call to
	// new(ELeave)
	CONSTRUCTORS_MAY_LEAVE

	static CManagedUserSinglePhase* NewL(CTicker* aTicker)
		{
		return new(ELeave) CManagedUserSinglePhase(aTicker);
		}

	~CManagedUserSinglePhase()
		{
		// The iTicker manager destructor will automatically Zap() the CTicker
		// The iTimer manager destructor will automatically Close() the RTimer
		}

	CTicker& Ticker()
		{
		// If we dereference the management object we get a CTicker&
		return *iTicker;
		}

	RTimer& Timer()
		{
		// If we dereference the management object we get an RTimer&
		return *iTimer;
		}

private:
	CManagedUserSinglePhase(CTicker* aTicker)
		// Take ownership and manage aTicker. Note that initialization
		// of the LManagedXxx classes does not actually leave, but
		// initialization of the LCleanedupXxx classes can.
		: iTicker(aTicker)
		{
		// If iTicker initialization is successful but the constructor
		// then goes on to leave later, iTicker (like all fields fully
		// constructed at the point of a leave in a constructor) will
		// be destructed, and the manager will cleanup the CTicker

		// Note use of -> to indirect through the management wrapper
		iTimer->CreateLocal() OR_LEAVE; 

		// Likewise if we leave here, both iTicker and iTimer will
		// undergo managed cleanup
		MaybeLeaveL();
		}

private:
	// We have to use LManagedXxx for fields, not LCleanedupXxx
	LManagedPtr<CTicker, TTickerZapStrategy> iTicker;
	LManagedHandle<RTimer> iTimer;
	};

//Class definition of trivial R-Class
class RSimple
	{
public:
	
	RSimple(){iData = NULL;}
	
	//Open function sets value
	void OpenL(TInt aValue)
		{
		iData = new(ELeave) TInt(aValue);
		}
	
	//Cleanup function – frees resource
	void Close()
		{
		delete iData;
		iData = NULL;
		}

	//Cleanup function – frees resource
	void Free()
		{
		delete iData;
		iData = NULL;
		}

	//Cleanup function – frees resource
	void ReleaseData()
		{
		delete iData;
		iData = NULL;
		}
	
	//static cleanup function – frees aRSimple resources
	static void Cleanup(TAny* aRSimple)
		{
		static_cast<RSimple*>(aRSimple)->Close();
		}


private:
	TInt* iData;

	};


//This sets the default cleanup behaviour for the RSimple class to 
//be RSimple::ReleaseData.
//If this Macro is not used then the default cleanup behaviour
//would be to call RSimple::Close().
DEFINE_CLEANUP_FUNCTION(RSimple, ReleaseData);


void WalkthroughManagedL()
	{
		{
		// Trivially exercise the manager-using classes defined above
		CTicker* ticker1 = new(ELeave) CTicker;
		LCleanedupPtr<CManagedUserTwoPhase> one(CManagedUserTwoPhase::NewL(ticker1));
		test(&one->Ticker() == ticker1);
		one->Timer().Cancel(); // Just to check we can get at it

		CTicker* ticker2 = new(ELeave) CTicker;
		LCleanedupPtr<CManagedUserSinglePhase> two(CManagedUserSinglePhase::NewL(ticker2));
		test(&two->Ticker() == ticker2);
		two->Timer().Cancel(); // Just to check we can get at it

		// Both instances are automatically deleted as we go out of scope
		}

		// Always use LCleanedupXxx for locals, not LManagedXxx

		{
		// Begin the scenes the LCleanedupXxx constructors push a
		// cleanup item onto the cleanup stack and so may leave. If
		// there is a leave during construction, the supplied pointer
		// will still get cleaned up.
		LCleanedupPtr<CTicker> t(new(ELeave) CTicker);

		// We can access CTicker's members via the management object
		// using ->
		t->Tick();
		t->Tock();
		test(t->iTicks == t->iTocks);

		// We can get at a reference to the managed object using *
		// when we need to, e.g. if we need to pass it to a function
		RegisterTicker(*t); // Takes a CTicker&

		// If some unfriendly interface needs a pointer rather than a
		// ref, we have a couple of options
		RegisterTickerPtr(&*t); // Takes a CTicker*
		RegisterTickerPtr(t.Get()); // Takes a CTicker*

		// Note the use of . in t.Get() above; this distinguishes
		// operations on the managing type from operations on the
		// managed object
		
		// When the management object goes out of scope, either
		// normally or as the result of a leave, the managed object is
		// automatically deleted
		}

		{
		// Sometimes you need to protect something temporarily before
		// transferring ownership e.g. by returning the pointer or
		// passing it to a function that takes ownership.

		LCleanedupPtr<CTicker> t(new(ELeave) CTicker);

		// Protected while we do this
		MaybeLeaveL(); 

		// But now we want to hand it off, so we use Unmanage() to
		// both return a pointer and break the management link
		TakeTickerOwnership(t.Unmanage());
		
		// Now when it goes out of scope, no cleanup action is
		// performed
		}

		{
		// If needed, it is possible to reuse a manager by using = to
		// assign it a new managed object.

		// Not managing anything to start with
		LCleanedupPtr<CTicker> t;
		test(t.Get() == NULL);
		test(&*t == NULL);

		for (TInt i = 0; i < 10; ++i)
			{
			// If an object is already being managed, it is cleaned up
			// before taking ownership of the new object
			t = new(ELeave) CTicker;
			}
		// We're left owning the final ticker instance, all prior
		// instances having been automatically deleted
		}

		{
		// If you have stateful code where a pointer can sometimes be
		// NULL, as a convenience you can test the managing object
		// itself as a shortcut test for NULL
		LCleanedupPtr<CTicker> t(new(ELeave) CTicker);

		// Does t refer to NULL?
		if (!t)
			{
			test(EFalse);
			}

		t = NULL; // Also releases the currently managed CTicker 

		// Does t refer to a non-NULL pointer?
		if (t)
			{
			test(EFalse);
			}
		}

		{
		// LCleanedupPtr uses delete to cleanup by default, but
		// alternative cleanups can be specified

		// We just want to free this one and not invoke the destructor
		LCleanedupPtr<CTicker, TPointerFree> t(static_cast<CTicker*>(User::AllocL(sizeof(CTicker))));

		// Now User::Free() is called when t goes out of scope
		}

		{
		// As well as the stock options, custom cleanup policies can
		// also be defined. See above for the definition of
		// TTickerZap.
		LCleanedupPtr<CTicker, TTickerZapStrategy> t(new(ELeave) CTicker);

		// Now Zap() is called on the CTicker instance when t goes out of scope
		}

		{
		// LCleanedupHandle is very similar in behaviour to
		// LCleanedupPtr, the main difference being that it can define
		// and contain its own instance of a handle rather than
		// being supplied one
		LCleanedupHandle<RTimer> t;

		// Again, access to managed handle members is via ->
		t->CreateLocal() OR_LEAVE;
		t->Cancel();

		// We can get a reference to the handle for passing to
		// functions using *
		RegisterTimer(*t);

		// When the management object goes out of scope, either
		// normally or as the result of a leave, the managed object is
		// automatically cleanup by calling Close() on it
		}

		{
		// LCleanedupHandle calls Close() by default, but alternative
		// cleanups can be specified
		
		// We want this RPointerArray cleanup with with
		// ResetAndDestroy instead of Close()
		LCleanedupHandle<RPointerArray<HBufC>, TResetAndDestroy> array;
		for (TInt i = 0; i < 10; ++i) 
			{
			array->AppendL(HBufC::NewL(5));
			}

		// Now when array goes out of scope, ResetAndDestroy is called
		// to clean it up
		}

		{
		// As well as the stock options, custom cleanup policies can
		// also be defined. See above for the definition of
		// TCancelClose.
		LCleanedupHandle<RTimer, TCancelClose> t;
		t->CreateLocal();

		// Now Cancel() followed by Close() are called when t goes out
		// of scope
		}


		{
		// LCleanedupHandleRef calls Close() by default, but alternative
		// cleanups can be specified
		
		// We want this RPointerArray cleanup with with
		// ResetAndDestroy instead of Close()
		RPointerArray<HBufC> rar;
		// calls to functions that cannot leave here
		rar.Append(HBufC::NewL(5));
		rar.Append(HBufC::NewL(5));


		LCleanedupRef<RPointerArray<HBufC>, TResetAndDestroy> array(rar);
		// calls to functions that could leave here
		for (TInt i = 0; i < 10; ++i) 
			{
			array->AppendL(HBufC::NewL(5));
			}

		// Now when array goes out of scope, ResetAndDestroy is called
		// to clean it up
		}

		{
		// Never mix direct cleanup stack API calls with management
		// class use within the same function, because their
		// interaction can be confusing and counter intuitive. Avoid
		// the use of LC methods that leave objects on the cleanup
		// stack, and use L methods instead.

		// If a badly-behaved API were to offer only an LC variant,
		// you would have to use it as follows
		HBufC* raw = HBufC::NewLC(5);
		// Must pop immediately to balance the cleanup stack, before
		// instantiating the manager
		CleanupStack::Pop(); 
		LCleanedupPtr<HBufC> wrapped(raw);

		// Never do this:
		//LCleanedupPtr<HBufC> buf(HBufC::NewLC(5));
		//CleanupStack::Pop();
		// because the manager will be popped (having been pushed
		// last), not the raw buf pointer as you might have hoped

		// A cleaner alternative may be to write your own L function
		// wrapper around the LC function supplied.

		// Luckily this situation (an LC method without a
		// corresponding L method) is rare in practice.
		}

		{
		// Although rarely used on Symbian OS, C++ arrays are
		// supported with a custom management class
		LCleanedupArray<CTicker> array(new CTicker[5]);

		// The array is cleaned up with delete[] on scope exit
		}

		{
		// Although most cases are best covered by applying custom
		// cleanup policies to the management classes already
		// described, there is also a general TCleanupItem style
		// cleanup option
		TAny* data = NULL; // But could be anything
		LCleanedupGuard guard1(BespokeCleanupFunction, data);
		// On scope exit BespokeCleanupFunction is called on data

		LCleanedupGuard guard2(BespokeCleanupFunction, data);
		// But cleanup can also be disabled in this case, as follows
		guard2.Dismiss();
		}
		
		{
		TInt r =KErrNone;	
		LCleanedupHandle<RFs> managedFs;
		r = managedFs->Connect();
		if (r != KErrNone)
		 {
			User::Leave(r);
		 }
		//default cleanup strategy is to call RFs::Close() on scope exit
		}
		
		{
		LCleanedupHandle<RSimple, TFree> simple;
		simple->OpenL(23);
		//Specified cleanup strategy is to call RSimple::Free() on scope exit
		}
	
		//Because the DEFINE_CLEANUP_FUNCTION is defined above, the default
		//cleanup function for RSimple is RSimple::ReleaseData() rather than
		//RSimple::Close()
		{
		LCleanedupHandle<RSimple> simple;
		simple->OpenL(23);
		//Custom cleanup strategy is to call RSimple::ReleaseData() on scope exit
		}
		
		{
		RSimple simple;
		
		//The RSimple class above defines a static cleanup function
		//RSimple::Cleanup.
		LCleanedupGuard guard(RSimple::Cleanup, &simple);

		simple.OpenL(10);
		
		//On scope exit RSimple::Cleanup() is called passing &simple
		}
	}

void WalkthroughUsageL()
	{
	RFile file;
	
	test.Printf(_L("Size of RFile = %d"), sizeof(file));
	
	LCleanedupHandle<RFile> cFile;
	
	test.Printf(_L("Size of LCleanedupHandle<RFile> = %d"), sizeof(cFile));
	
	LCleanedupRef<RFile> crFile(file);
	
	test.Printf(_L("Size of LCleanedupRef<RFile> = %d"), sizeof(crFile));
	
	CTicker* tracker = new(ELeave) CTicker;
	//coverity[resource_leak]
	//As mentioned in the comment above any allocation failure is taken care of
	test.Printf(_L("Size of CTracker* = %d"), sizeof(tracker));
	
	LCleanedupPtr<CTicker> cTracker(tracker);
	
	test.Printf(_L("Size of LCleanedupHandle<RFile> = %d"), sizeof(LCleanedupPtr<CTicker>));
	}

TInt TestL()
	{
	WalkthroughStringsL();
	WalkthroughManagedL();
	WalkthroughUsageL();

	return KErrNone;
	}

TInt E32Main()
	{

	test.Start(_L("EUserHl Walkthrough"));
	test.Title();

	CTrapCleanup* trapHandler=CTrapCleanup::New();
	test(trapHandler!=NULL);
	
	__UHEAP_MARK;
	
	TRAPD(status, TestL());
	
	__UHEAP_MARKEND;

	if (status != KErrNone) test.Printf(_L("Error: %d\n"), status);
	
	test.Printf(_L("Test Completed with Error: %d"),status);
	
	return status;
	}


// eof
