// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Trace API
// 
//

#ifndef UTRACE_H
#define UTRACE_H

#ifdef __KERNEL_MODE__
#include <kernel/kernel.h>
#else //__KERNEL_MODE__
#include <e32std.h>
#endif //__KERNEL_MODE__
#include <e32btrace.h>


/**
@file
@publishedPartner
@prototype
*/

/**
Primary filter type.
The primary filter type used as classification information.
*/
typedef TUint8 TPrimaryFilter;
/**
Secondary filter type.
The secondary filter type used as classification information.
*/
typedef TUint32 TSecondaryFilter;


/**
The trace contains information used to debug an application
This can be used in both UREL and UDEB builds.
*/
const TPrimaryFilter  KDebug = 253;


/**
Schema type.
Use the schema in conjunction with primary and secondary filters to determine
how a trace shall be interpreted.
The schema must represent a specific, well-documented, and consistent plan;
a way to define the structure and/or its content.
*/
typedef TUint8 TTraceSchema;

/**
The schema is binary, or raw data.
*/
const TTraceSchema KBinary = 0;//Binary format
/**
The schema is ASCII text.
*/
const TTraceSchema KText = 1;//Text only

/**
Maximum length of a formatted string
*/
const TInt KMaxPrintfSize = 256;


/**@internalComponent*/
#define UTRACE_HEADER(aSize,aF1,aSchema,aContext,aPc)																\
	((((aSize)+(4&(TBool)aContext<<2)+(4&(TBool)aPc<<2))<<BTrace::ESizeIndex*8)										\
	+(((BTrace::EContextIdPresent & (aContext<<3))|(BTrace::EPcPresent & (aPc<<4)))<<BTrace::EFlagsIndex*8)			\
	+((aF1)<<BTrace::ECategoryIndex*8)																				\
	+((aSchema)<<BTrace::ESubCategoryIndex*8))
/**@internalComponent*/
#define UTracePf0(aF1,aSchema,aContext,aPc)	\
    BTrace::OutX \
        (UTRACE_HEADER(4,(aF1),(aSchema),(aContext),(aPc)),0,0,0)
/**@internalComponent*/
#define UTracePf1(aF1,aSchema,aContext,aPc,a1)	\
    BTrace::OutX(UTRACE_HEADER(8,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(a1),0,0)
/**@internalComponent*/
#define UTracePf2(aF1,aSchema,aContext,aPc,a1,a2) \
    BTrace::OutX(UTRACE_HEADER(12,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(a1),(TUint32)(a2),0)
/**@internalComponent*/
#define UTracePf3(aF1,aSchema,aContext,aPc,a1,a2,a3) \
	BTrace::OutX(UTRACE_HEADER(16,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(a1),(TUint32)(a2),(TUint32)(a3))
/**@internalComponent*/
#define UTracePfAny(aF1,aSchema,aContext,aPc,a1,aData,aDataSize) \
	BTrace::OutBig(UTRACE_HEADER(8,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(a1),aData,(TInt)(aDataSize))
/**@internalComponent*/
#define UTraceSf0(aF1,aF2,aSchema,aContext,aPc)	\
    BTrace::OutFilteredX(UTRACE_HEADER(8,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(aF2),0,0)
/** @internalComponent */
#define UTraceSf1(aF1,aF2,aSchema,aContext,aPc,a1) \
    BTrace::OutFilteredX(UTRACE_HEADER(12,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(aF2),(TUint32)(a1),0)
/** @internalComponent */
#define UTraceSf2(aF1,aF2,aSchema,aContext,aPc,a1,a2) \
	BTrace::OutFilteredX(UTRACE_HEADER(16,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(aF2),(TUint32)(a1),(TUint32)(a2))
/** @internalComponent */
#define UTraceSfAny(aF1,aF2,aSchema,aContext,aPc,aData,aDataSize) \
	BTrace::OutFilteredBig(UTRACE_HEADER(8,(aF1),(aSchema),(aContext),(aPc)),(TUint32)(aF2),aData,(TInt)(aDataSize))


/**
Class for tracing from user and kernel side.

This appends entries to a trace log.
Each trace point require classification information to be passed.
Classification information, i.e. primary and secondary filter,
is meta-data about the trace entry that can be used for
filtering, e.g. the  trace was inserted by a particular component.

In order to append trace points to the trace log they
need to be turned on and off at run-time, based on the classification.
It is also possible to turn trace points on and off
at compile-time using the SYMBIAN_TRACE_ENABLE macro.

The class can be used in two ways, either using the
static methods without any preparatory work, or by setting up
an object which has all necessary information pre-defined which
allows each consecutive call to contain a minimum of information.
The latter set of methods are more intrusive both at run-time and
for foot print.

@code
TBool includeContextID = ETrue;
TBool includePc = EFalse;
TUint32 traceData = aData;
TPrimaryFilter filter1 = 253;

TUTrace tracer(filter1, KBinary, includeContextID, includePc);
tracer.Printf(_L("My trace point."));
tracer.Trace(traceData);

//or

TUTrace::PrintfPrimary(filter1, KBinary, includeContextID, includePc, _L("My trace point with data %i."), traceData);
TUTrace::TracePrimary(filter1, KBinary, includeContextID, includePc, traceData);

@endcode


Note:
Trace does not enforce any security. It is the developer's responsibility
to ensure that trace points do not contain any sensitive information that may 
be used to compromise platform security.

@publishedPartner
@prototype
*/
#if defined (SYMBIAN_TRACE_ENABLE) || defined (__DOXYGEN__)
class TUTrace
{
public:

	/**
	Construct a trace object with classification for
	primary filter only.

	@param aF1 Primary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	TUTrace(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc)
		{
		Set(aF1, aSchema, aIncludeContextID, aIncludePc);
		}

	/**
	Construct a trace object with classification for both
	primary and secondary filters.

	@param aF1 Primary filter as classification information
	@param aF2 Secondary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	TUTrace(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc)
		{
		Set(aF1, aF2, aSchema, aIncludeContextID, aIncludePc);
		}


	/**
	Set the classification and additional information.

	@param aF1 Primary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	void Set(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc)
		{
		iF2 = 0;
		iF1 = aF1;
		iSchema = aSchema;
		iPresent = aIncludeContextID;
		iPresent |= aIncludePc << EPc;
		iReserved1 = 0;
		iReserved2 = 0;
		}


	/**
	Set the classification and additional information.

	@param aF1 Primary filter as classification information
	@param aF2 Secondary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	void Set(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID , TBool aIncludePc)
		{
		Set(aF1, aSchema, aIncludeContextID, aIncludePc);
		iF2 = aF2;
		}

	#ifdef  __KERNEL_MODE__

	/**
	Prints a formatted string in kernel mode only.

	The function uses Kern::AppendFormat() to do the formatting.

	Although it is safe to call this function from an ISR, it polls the output
	serial port and may take a long time to complete, invalidating any
	real-time guarantee.

	If called from an ISR, it is possible for output text to be intermingled
	with other output text if one set of output interrupts or preempts another.

	Some of the formatting options may not work inside an ISR.

	Be careful not to use a string that is too long to fit onto the stack.
	If the specified string is too big to fit into a single	trace
	a multipart trace is generated.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Calling thread can either be in a critical section or not.
	@pre Interrupts must be enabled.
	@pre Kernel must be unlocked
	@pre Call in any context.
	@pre Suitable for use in a device driver
	@pre Classification information must be set up

	@see Kern::AppendFormat()
	@see BTrace::TMultipart
	*/
	TBool Printf(const char* aFmt, ...)
		{
		TBuf8<KMaxPrintfSize> buf;
		VA_LIST list;
		VA_START(list,aFmt);
		Kern::AppendFormat(buf,aFmt,list);
		TUint8 oldFormat = iSchema;
		iSchema = KText;
		TBool logged = EFalse;
		logged = Trace(buf.Ptr(),buf.Size());
		iSchema = oldFormat;
		return logged;
		}

	#else //__KERNEL_MODE__

	/**
	Prints a formatted string.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const char* aFmt, ...)
		{
		TTruncateOverflow8 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TPtrC8 fmt((const TText8*)aFmt);
		TBuf8<KMaxPrintfSize> buf;
		buf.AppendFormatList(fmt,list,&overflow);
		TUint8 oldFormat = iSchema;
		iSchema = KText;
		TBool logged = EFalse;
		logged = Trace(buf.PtrZ(),buf.Size());
		iSchema = oldFormat;
		return logged;
		};

	/**
	Prints a formatted string.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(TRefByValue<const TDesC16> aFmt,...)
	    {
	    TTruncateOverflow16 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TBuf<KMaxPrintfSize> buf;
		buf.AppendFormatList(aFmt,list,&overflow);
		TUint8 oldFormat = iSchema;
		iSchema = KText;
		TBool logged = EFalse;
		#ifdef _UNICODE
		TPtr8 p(buf.Collapse());
		logged = Trace(buf.PtrZ(), p.Size());
		#else
		logged = Trace(buf.PtrZ(), buf.Size());
		#endif //_UNICODE
		iSchema = oldFormat;
		return logged;
	    };

	/**
	Prints a string.

	@param aDes The string. This can be of variable length.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const TDesC8& aDes)
		{
		TUint8 oldFormat = iSchema;
		iSchema = KText;
		TBool logged = EFalse;
		logged = Trace(aDes.Ptr(), aDes.Size());
		iSchema = oldFormat;
		return logged;
		};

	/**
	Prints a string.

	@param aDes The string. This can be of variable length.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const TDesC16& aDes)
	    {
		TUint8 oldFormat = iSchema;
		iSchema = KText;
		TBool logged = EFalse;
		logged = Trace(aDes.Ptr(), aDes.Size());
		iSchema = oldFormat;
		return logged;
		};

	#endif //__KERNEL_MODE__

	/**
	Sends a trace with no payload data.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
    TBool Trace()
		{
		if(iF2 > 0)
			{
			return UTraceSf0(iF1,iF2,iSchema,ContextID(),Pc());
			}
		return UTracePf0(iF1,iSchema,ContextID(),Pc());
		};

	/**
	Sends a trace containing 4 bytes of data.

	@param aData1	4 bytes of data.

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 aData1)
		{
		if(iF2 > 0)
			{
			return UTraceSf1(iF1,iF2,iSchema,ContextID(),Pc(), aData1);
			}
		return UTracePf1(iF1,iSchema,ContextID(),Pc(), aData1);
		};

	/**
	Sends a trace with 8 bytes of data.

	@param aData1	4 bytes of data
	@param aData2	4 bytes of data

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 aData1, TUint32 aData2)
		{
		if(iF2 > 0)
			{
			return UTraceSf2(iF1,iF2,iSchema,ContextID(),Pc(), aData1, aData2);
			}
		return UTracePf2(iF1,iSchema,ContextID(),Pc(),aData1,aData2);
		};

	/**
	Sends a trace with 12 bytes of data.
	Note: This method does not support secondary filtering.

	@param aData1	4 bytes of data
	@param aData2	4 bytes of data
	@param aData3	4 bytes of data

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 aData1, TUint32 aData2,  TUint32 aData3)
		{
		if(iF2 > 0)
			{
			return EFalse;
			}
		return UTracePf3(iF1,iSchema,ContextID(),Pc(),aData1,aData2,aData3);
		};

	/**
	Sends a trace with variable length data.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aData	Address of addition data to add to trace.
					Must be word aligned, i.e. a multiple of 4.
	@param aSize	Number of bytes of additional data.

	@return 		The trace was/was not logged.

	@pre Classification information must be set up

	@see BTrace::TMultipart
	*/
	TBool Trace(const TAny* aData, TInt aSize)
		{
		if(iF2 > 0)
			{
			return UTraceSfAny(iF1,iF2,iSchema,ContextID(),Pc(),aData,aSize);
			}
		return UTracePfAny(iF1,iSchema,ContextID(),Pc(),0,aData,aSize);
		};


	//Static methods
	#ifdef __KERNEL_MODE__

	/**
	Prints a formatted string in kernel mode only.
	The trace is classified by primary filter only.

	The function uses Kern::AppendFormat() to do the formatting.

	Although it is safe to call this function from an ISR, it polls the output
	serial port and may take a long time to complete, invalidating any
	real-time guarantee.

	If called from an ISR, it is possible for output text to be intermingled
	with other output text if one set of output interrupts or preempts another.

	Some of the formatting options may not work inside an ISR.

	Be careful not to use a string that is too long to fit onto the stack.
	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.


	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@pre Calling thread can either be in a critical section or not.
	@pre Interrupts must be enabled.
	@pre Kernel must be unlocked
	@pre Call in any context.
	@pre Suitable for use in a device driver

	@see Kern::AppendFormat()
	@see BTrace::TMultipart
	*/
	static TBool PrintfPrimary(TPrimaryFilter aF1, TBool aIncludeContextID, TBool aIncludePc, const char* aFmt, ...)
		{
		TBuf8<KMaxPrintfSize> buf;
		VA_LIST list;
		VA_START(list,aFmt);
		Kern::AppendFormat(buf,aFmt,list);
		return UTracePfAny(aF1, KText, aIncludeContextID, aIncludePc, 0, buf.Ptr(),buf.Size());
		}

	#else //__KERNEL_MODE__

	/**
	Prints a formatted string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter aF1, TBool aIncludeContextID, TBool aIncludePc, const char* aFmt, ...)
		{
		TTruncateOverflow8 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TPtrC8 fmt((const TText8*)aFmt);
		TBuf8<KMaxPrintfSize> buf;
		buf.AppendFormatList(fmt,list,&overflow);
		return UTracePfAny(aF1, KText, aIncludeContextID, aIncludePc, 0, buf.PtrZ(),buf.Size());
		};

	/**
	Prints a formatted string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter aF1, TBool aIncludeContextID, TBool aIncludePc, TRefByValue<const TDesC16> aFmt,...)
    	{
	    TTruncateOverflow16 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TBuf<KMaxPrintfSize> buf;
		buf.AppendFormatList(aFmt,list,&overflow);
		#ifdef _UNICODE
		TPtr8 p(buf.Collapse());
		return UTracePfAny(aF1, KText, aIncludeContextID, aIncludePc, 0, buf.PtrZ(), p.Size());
		#else
		return UTracePfAny(aF1, KText, aIncludeContextID, aIncludePc, 0, buf.PtrZ(), buf.Size());
		#endif //_UNICODE
    	};

	/**
	Prints a string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter aF1, TBool aIncludeContextID, TBool aIncludePc, const TDesC8& aDes)
    	{
		return UTracePfAny(aF1, KText, aIncludeContextID, aIncludePc,0, aDes.Ptr(), aDes.Size());
		};

	/**
	Prints a string. The trace is classified by primary filter only.


	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter aF1, TBool aIncludeContextID, TBool aIncludePc, const TDesC16& aDes)
    	{
    	TUTrace trace(aF1,KText,aIncludeContextID,aIncludePc);
    	return trace.Printf(aDes);
    	};

	/**
	Prints a formatted string. The trace is classified by primary and secondary filter.


	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter aF1, TSecondaryFilter aF2, TBool aIncludeContextID, TBool aIncludePc, const char* aFmt, ...)
		{
		TTruncateOverflow8 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TPtrC8 fmt((const TText8*)aFmt);
		TBuf8<KMaxPrintfSize> buf;
		buf.AppendFormatList(fmt,list,&overflow);
		return UTraceSfAny(aF1, aF2, KText, aIncludeContextID, aIncludePc, buf.PtrZ(),buf.Size());
		};

	/**
	Prints a formatted string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter aF1, TSecondaryFilter aF2, TBool aIncludeContextID, TBool aIncludePc, TRefByValue<const TDesC16> aFmt,...)
    	{
	    TTruncateOverflow16 overflow;
		VA_LIST list;
		VA_START(list,aFmt);
		TBuf<KMaxPrintfSize> buf;
		buf.AppendFormatList(aFmt,list,&overflow);
		#ifdef _UNICODE
		TPtr8 p(buf.Collapse());
		return UTraceSfAny(aF1, aF2, KText, aIncludeContextID, aIncludePc, buf.PtrZ(), p.Size());
		#else
		return UTraceSfAny(aF1, aF2, KText, aIncludeContextID, aIncludePc, buf.PtrZ(), buf.Size());
		#endif //_UNICODE
    	};

	/**
	Prints a string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter aF1, TSecondaryFilter aF2, TBool aIncludeContextID, TBool aIncludePc, const TDesC8& aDes)
    	{
		return UTraceSfAny(aF1,  aF2, KText,aIncludeContextID, aIncludePc, aDes.Ptr(), aDes.Size());
		};

	/**
	Prints a string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter aF1, TSecondaryFilter aF2, TBool aIncludeContextID, TBool aIncludePc, const TDesC16& aDes)
    	{
    	TUTrace trace(aF1,aF2,KText,aIncludeContextID,aIncludePc);
    	return trace.Printf(aDes);
    	};

	#endif //__KERNEL_MODE_

	/**
	Sends a trace with no payload. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc)
		{
		return UTracePf0(aF1, aSchema, aIncludeContextID, aIncludePc);
		}

	/**
	Sends a trace with 4 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, TUint32 aData1)
		{
		return UTracePf1(aF1, aSchema, aIncludeContextID, aIncludePc, aData1);
		}

	/**
	Sends a trace with 8 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, TUint32 aData1, TUint32 aData2)
		{
		return UTracePf2(aF1, aSchema, aIncludeContextID, aIncludePc,aData1,aData2);
		}

	/**
	Sends a trace with 12 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data
	@param aData3		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, TUint32 aData1, TUint32 aData2, TUint aData3)
		{
		return UTracePf3(aF1, aSchema, aIncludeContextID, aIncludePc,aData1,aData2,aData3);
		}

	/**
	Sends a trace with variable length data. The trace is classified
	by primary filter only.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aF1		Primary filter as classification information
	@param aSchema	The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData	Address of addition data to add to trace.
					Must be word aligned, i.e. a multiple of 4.
	@param aSize	Number of bytes of additional data.

	@return 		The trace was/was not logged.

	@see BTrace::TMultipart
	*/
	static TBool TracePrimary(TPrimaryFilter aF1, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, const TAny* aData, TInt aSize)
		{
		return UTracePfAny(aF1, aSchema, aIncludeContextID, aIncludePc, 0, aData, aSize);
		}

	/**
	Sends a trace with no payload. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc)
		{
		return UTraceSf0(aF1, aF2, aSchema, aIncludeContextID, aIncludePc);
		}

	/**
	Sends a trace with 4 bytes of data. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, TUint32 aData1)
		{
		return UTraceSf1(aF1, aF2, aSchema , aIncludeContextID, aIncludePc, aData1);
		}

	/**
	Sends a trace with 8 bytes of data. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, TUint32 aData1, TUint32 aData2)
		{
		return UTraceSf2(aF1, aF2, aSchema , aIncludeContextID, aIncludePc,aData1,aData2);
		}

	/**
	Sends a trace with variable length data. The trace is classified
	by primary and secondary filter.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData		Address of addition data to add to trace.
						Must be word aligned, i.e. a multiple of 4.
	@param aSize		Number of bytes of additional data.

	@return 			The trace was/was not logged.

	@see BTrace::TMultipart
	*/
	static TBool Trace(TPrimaryFilter aF1, TSecondaryFilter aF2, TTraceSchema aSchema, TBool aIncludeContextID, TBool aIncludePc, const TAny* aData, TInt aSize)
		{
		return UTraceSfAny(aF1, aF2, aSchema, aIncludeContextID, aIncludePc, aData, aSize);
		}

private:

	// See/Update comments below
	TUTrace(){};

	/**
	@internalComponent
	*/
	enum TTraceInfoPresent
		{
		EContext = 0,
		EPc = 1
		};

	/**
	Dummy class to toss away overflow
	@internalComponent
	*/
	#ifndef __KERNEL_MODE__
	class TTruncateOverflow16 : public TDes16Overflow
		{
		public:
		virtual void Overflow(TDes&) {};
		};
	/**
	Dummy class to toss away overflow
	@internalComponent
	*/
	class TTruncateOverflow8 : public TDes8Overflow
		{
		public:
		virtual void Overflow(TDes8&) {};
		};
	#endif  //__KERNEL_MODE__

	/**
	@internalComponent
	*/
	TBool ContextID()
		{
			return iPresent&1;
		};

	/**
	@internalComponent
	*/
	TBool Pc()
		{
			return (iPresent&2)>>EPc;
		};

private:
	TPrimaryFilter iF1;
	TTraceSchema iSchema;
	TSecondaryFilter iF2;
	TInt8 iPresent;

	TUint32 iReserved1;
	TUint32 iReserved2;
};

#else //SYMBIAN_TRACE_ENABLE


class TUTrace
{
public:
	/**
	Construct a trace object with classification for
	primary filter only.

	@param aF1 Primary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	TUTrace(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/){};

	/**
	Construct a trace object with classification for both
	primary and secondary filters.

	@param aF1 Primary filter as classification information
	@param aF2 Secondary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	TUTrace(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/){};

	/**
	Set the classification and additional information.

	@param aF1 Primary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	void Set(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/){};

	/**
	Set the classification and additional information.

	@param aF1 Primary filter as classification information
	@param aF2 Secondary filter as classification information
	@param aSchema The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc Include program counter in the trace
	*/
	void Set(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/){};
	#ifdef  __KERNEL_MODE__

	/**
	Prints a formatted string in kernel mode only.

	The function uses Kern::AppendFormat() to do the formatting.

	Although it is safe to call this function from an ISR, it polls the output
	serial port and may take a long time to complete, invalidating any
	real-time guarantee.

	If called from an ISR, it is possible for output text to be intermingled
	with other output text if one set of output interrupts or preempts another.

	Some of the formatting options may not work inside an ISR.

	Be careful not to use a string that is too long to fit onto the stack.
	If the specified string is too big to fit into a single	trace
	a multipart trace is generated.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Calling thread can either be in a critical section or not.
	@pre Interrupts must be enabled.
	@pre Kernel must be unlocked
	@pre Call in any context.
	@pre Suitable for use in a device driver
	@pre Classification information must be set up

	@see Kern::AppendFormat()
	@See BTrace::TMultipart
	*/
	TBool Printf(const char* /*aFmt*/, ...)
		{
		return EFalse;
		}

	#else //__KERNEL_MODE__

	/**
	Prints a formatted string.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const char* /*aFmt*/, ...)
		{
		return EFalse;
		};

	/**
	Prints a formatted string.

	@param aFmt The format string. This must not be longer than 256 characters.
	@param ...	A variable number of arguments to be converted to text as dictated
				by the format string.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(TRefByValue<const TDesC16> /*aFmt*/,...)
	    {
		return EFalse;
	    };

	/**
	Prints a string.

	@param aDes The string. This can be of variable length.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const TDesC8& /*aDes*/)
		{
		return EFalse;
		};

	/**
	Prints a string.

	@param aDes The string. This can be of variable length.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Printf(const TDesC16& /*aDes*/)
	    {
		return EFalse;
		};

	#endif // __KERNEL_MODE__

	/**
	Sends a trace with no payload data.

	@return The trace was/was not logged.

	@pre Classification information must be set up
	*/
    TBool Trace()
		{
		return EFalse;
		};

	/**
	Sends a trace containing 4 bytes of data.

	@param aData1	4 bytes of data.

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 /*aData1*/)
		{
		return EFalse;
		};

	/**
	Sends a trace with 8 bytes of data.

	@param aData1	4 bytes of data
	@param aData2	4 bytes of data

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 /*aData1*/, TUint32 /*aData2*/)
		{
		return EFalse;
		};

	/**
	Sends a trace with 12 bytes of data.
	Note: This method does not support secondary filtering.

	@param aData1	4 bytes of data
	@param aData2	4 bytes of data
	@param aData3	4 bytes of data

	@return 		The trace was/was not logged.

	@pre Classification information must be set up
	*/
	TBool Trace(TUint32 /*aData1*/, TUint32 /*aData2*/,  TUint32 /*aData3*/)
		{
		return EFalse;
		};

	/**
	Sends a trace with variable length data.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aData	Address of addition data to add to trace.
					Must be word aligned, i.e. a multiple of 4.
	@param aSize	Number of bytes of additional data.

	@return 		The trace was/was not logged.

	@pre Classification information must be set up

	@See BTrace::TMultipart
	*/
	TBool Trace(const TAny* /*aData*/, TInt /*aSize*/)
		{
		return EFalse;
		};

	#ifdef __KERNEL_MODE__

	/**
	Prints a formatted string in kernel mode only.
	The trace is classified by primary filter only.

	The function uses Kern::AppendFormat() to do the formatting.

	Although it is safe to call this function from an ISR, it polls the output
	serial port and may take a long time to complete, invalidating any
	real-time guarantee.

	If called from an ISR, it is possible for output text to be intermingled
	with other output text if one set of output interrupts or preempts another.

	Some of the formatting options may not work inside an ISR.

	Be careful not to use a string that is too long to fit onto the stack.
	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.


	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@pre Calling thread can either be in a critical section or not.
	@pre Interrupts must be enabled.
	@pre Kernel must be unlocked
	@pre Call in any context.
	@pre Suitable for use in a device driver

	@see Kern::AppendFormat()
	@See BTrace::TMultipart
	*/
	static TBool PrintfPrimary(TPrimaryFilter /*aF1*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const char* /*aFmt*/, ...)
		{
		return EFalse;
		}

	#else //__KERNEL_MODE__

	/**
	Prints a formatted string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter /*aF1*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const char* /*aFmt*/, ...)
		{
		return EFalse;
		};

	/**
	Prints a formatted string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter /*aF1*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TRefByValue<const TDesC16> /*aFmt*/,...)
    	{
		return EFalse;
    	};


	/**
	Prints a string. The trace is classified by primary filter only.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter /*aF1*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TDesC8& /*aDes*/)
    	{
		return EFalse;
		};

	/**
	Prints a string. The trace is classified by primary filter only.


	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool PrintfPrimary(TPrimaryFilter /*aF1*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TDesC16& /*aDes*/)
    	{
		return EFalse;
    	};


	/**
	Prints a formatted string. The trace is classified by primary and secondary filter.


	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const char* /*aFmt*/, ...)
		{
		return EFalse;
		};

	/**
	Prints a formatted string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aFmt 		The format string. This must not be longer than 256 characters.
	@param ...			A variable number of arguments to be converted to text as dictated
						by the format string.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TRefByValue<const TDesC16> /*aFmt*/,...)
    	{
		return EFalse;
    	};


	/**
	Prints a string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TDesC8& /*aDes*/)
    	{
		return EFalse;
		};


	/**
	Prints a string. The trace is classified by primary and secondary filter.

	If the specified string is too long to fit into a single trace record
	a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aDes			The string. This can be of variable length.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
    static TBool Printf(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TDesC16& /*aDes*/)
    	{
		return EFalse;
    	};

	#endif //__KERNEL_MODE_

	/**
	Sends a trace with no payload. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with 4 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TUint32 /*aData1*/)

		{
		return EFalse;
		}

	/**
	Sends a trace with 8 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TUint32 /*aData1*/, TUint32 /*aData2*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with 12 bytes of data. The trace is classified
	by primary filter only.

	@param aF1			Primary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data
	@param aData3		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool TracePrimary(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TUint32 /*aData1*/, TUint32 /*aData2*/, TUint /*aData3*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with variable length data. The trace is classified
	by primary filter only.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aF1		Primary filter as classification information
	@param aSchema	The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData	Address of addition data to add to trace.
					Must be word aligned, i.e. a multiple of 4.
	@param aSize	Number of bytes of additional data.

	@return 		The trace was/was not logged.

	@See BTrace::TMultipart
	*/
	static TBool TracePrimary(TPrimaryFilter /*aF1*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TAny* /*aData*/, TInt /*aSize*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with no payload. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with 4 bytes of data. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TUint32 /*aData1*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with 8 bytes of data. The trace is classified
	by primary and secondary filter.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData1		4 bytes of data
	@param aData2		4 bytes of data

	@return 		The trace was/was not logged.
	*/
	static TBool Trace(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, TUint32 /*aData1*/, TUint32 /*aData2*/)
		{
		return EFalse;
		}


	/**
	Sends a trace with variable length data. The trace is classified
	by primary and secondary filter.

	If the specified data is too big to fit into a single
	trace record a multipart trace is generated.

	@param aF1			Primary filter as classification information
	@param aF2			Secondary filter as classification information
	@param aSchema		The schema used to determine the format of the content
	@param aIncludeContextID Include context ID in the trace
	@param aIncludePc	Include program counter in the trace
	@param aData		Address of addition data to add to trace.
						Must be word aligned, i.e. a multiple of 4.
	@param aSize		Number of bytes of additional data.

	@return 			The trace was/was not logged.

	@See BTrace::TMultipart
	*/
	static TBool Trace(TPrimaryFilter /*aF1*/, TSecondaryFilter /*aF2*/, TTraceSchema /*aSchema*/, TBool /*aIncludeContextID*/, TBool /*aIncludePc*/, const TAny* /*aData*/, TInt /*aSize*/)
		{
		return EFalse;
		}

private:
	/**
	Default constructor. This prevents accidental empty initialisation of traces.
	*/
	TUTrace(){};

	/**
	@internalComponent
	*/
	enum TTraceInfoPresent
		{
		EContext = 0,
		EPc = 1
		};

	/**
	Dummy class to toss away overflow
	@internalComponent
	*/
	#ifndef __KERNEL_MODE__
	class TTruncateOverflow16 : public TDes16Overflow
		{
		public:
		virtual void Overflow(TDes&) {};
		};
	/**
	Dummy class to toss away overflow
	@internalComponent
	*/
	class TTruncateOverflow8 : public TDes8Overflow
		{
		public:
		virtual void Overflow(TDes8&) {};
		};
	#endif  //__KERNEL_MODE__

	/**
	@internalComponent
	*/
	TBool ContextID()
		{
			return EFalse;
		};

	/**
	@internalComponent
	*/
	TBool Pc()
		{
			return EFalse;
		};

private:
	TPrimaryFilter iF1;
	TTraceSchema iSchema;
	TSecondaryFilter iF2;
	TInt8 iPresent;

	TUint32 iReserved1;
	TUint32 iReserved2;
};

#endif //SYMBIAN_TRACE_ENABLE


#endif //UTRACE_H
