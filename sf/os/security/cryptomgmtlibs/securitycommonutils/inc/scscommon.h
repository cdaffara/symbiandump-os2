/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Information shared between SCS client and server implementations,
* and with the subclass implementations, but not with the client API user.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef SCSCOMMON_H
#define SCSCOMMON_H

#include <e32std.h>

#ifndef BULLSEYE_OFF
#ifdef _BullseyeCoverage
#define BULLSEYE_OFF "BullseyeCoverage save off";
#define BULLSEYE_RESTORE "BullseyeCoverage restore";
#else
#define BULLSEYE_OFF 
#define BULLSEYE_RESTORE 
#endif
#endif

namespace ScsImpl
	{
	/** 
		Bit position of SCS code in function code.

		Do not change - Some of the codes based off this definition
		ARE public.
	*/
	const TInt KScsFunctionPos = 24;
	
	/** 
		Mask used to extract SCS commands.

		Do not change - Some of the codes based off this definition
		ARE public.
	*/
	const TInt KScsFunctionMask = 0xFF000000;
	
	enum TScsFunction
	/**
		Bits 31:24 of the function code are reserved for SCS fields.
		The values below should be or'd into the code that is sent to
		the server, on the client side.
		
		The recognized values intentionally exclude zero, to ensure
		that a field is added on the client side.
	 */
		{
		/** 
			Function should be handled by session's DoServiceL.

			This value is effectively PUBLIC because the range used is
			described by defines in scsserver.h (used for configuring
			server security).
		*/
		ECallSessionFunc = 1 << KScsFunctionPos,
		
		/**
			The SCS sends the lower bits of the function code to the subsession's
			(not the session's) ServiceL implementation.

			The session's implementation of [Do]ServiceL is not involved.

			This value is effectively PUBLIC because the range used is
			described by defines in scsserver.h (used for configuring
			server security).
		 */
		ECallSubsessionFunc = 2 << KScsFunctionPos,

		/**
			This message is send with no function identifier or
			IPC arguments just before the session is closed.  Although
			not necessary, it will cancel any outstanding requests on
			the session or its subsessions with KErrCancel, so if the
			client has any outstanding requests they will still get
			completed.
		 */
		EPreCloseSession = 3 << KScsFunctionPos,
		
		/**
			Cancel an asynchronous session-relative function.  The low
			bits of the function code should be the same as the original
			function code.  E.g. if the function was set up with
			ENoScsFunction | X then it would be cancelled with ECancelSessionFunction | X.

			The session's implementation of [Do]ServiceL is not involved.
		 */
		ECancelSessionFunc = 4 << KScsFunctionPos,

		/**
			Tells the server that this function will create a new subsession.
			The low bits can be interpreted by the implementation to indicate
			a type of subsession.
		 */
		ECreateSubsession = 5 << KScsFunctionPos,
		/**
			This SCS code should be used on its own.  Any information in the
			lower bits will be ignored.
		 */
		ECloseSubsession = 6 << KScsFunctionPos,

		/**
			Similar to ECancelSessionFunction, this cancels an asynchronous
			request on a subsession object.  The request will be completed with
			KErrCancel.

			The subsession's implementation of ServiceL is not involved.

			@see ECancelSessionFunction
		 */
		ECancelSubsessionFunc = 7 << KScsFunctionPos,
		
		/**
			Only supported in debug builds, this function starts a server-side
			heap mark with __UHEAP_MARK and sets a deterministic failure rate.
			This function should only be used by RScsClientBase::SetServerHeapFail.

			@see EUHeapResetFail
			@see RScsClientBase::SetServerHeapFail
		 */
		EUHeapSetFail = 8 << KScsFunctionPos,

		/**
			Only supported in debug builds, this function ends the server-side
			heap mark set up with EUHeapSetFail, with __UHEAP_MARKEND, and resets
			the heap failure rate.  This function should only be used by
			RScsClientBase::ResetServerHeapFail.

			@see EUHeapSetFail
			@see RScsClientBase::ResetServerHeapFail
		 */
		EUHeapResetFail = 9 << KScsFunctionPos,

		/**
			Intended for debug use, but also present in production builds.

			Returns the PID of the server. This is a number, not a
			handle, so does not impact security.

		 */
		EGetServerPid = 10 << KScsFunctionPos,

		/**
			Intended for debug use, but also present in production builds.

			This call causes a server which has an inactivity shutdown
			timer to shutdown immediately the next time it is
			idle. This is just adjusts the timing of existing
			functionality, so is not believed to impact security.

			If the server has a shutdown timer, then a flag is set
			which causes the server to immediately exit the next time
			it becomes idle.

			If the server does not have a shutdown timer, then the
			calls fails with KErrNotSupported.
		 */
		EShutdownAsap = 11 << KScsFunctionPos,
		
		/**
			This value is not used by the server implementation.  It is provided
			for test code to confirm the server handles an uncrecognized instruction
			correctly, by failing with KErrNotSupported.
		 */
		EScsUnused = 0x20 << KScsFunctionPos
		};
	
	inline void ExtractScsAndImplFunctions(const RMessage2& aMessage, TScsFunction* aScsFunc, TInt* aImplFunc);
	inline TBool ScsFieldUsed(TInt aFunction);

	/**
		SCS clients are panicked with this category when invalid
		input to the server is detected.

		@see TClientPanic
	 */
	_LIT(KScsClientPanicCat, "SCS-Client");

	enum TScsClientPanic
	/**
		Reasons why the SCS server might panic its clients.

		@see KScsClientPanicCat
	 */
		{
		EScsClBadDesc = 0,				///< Client provided a bad descriptor as an IPC argument.
		EScsClBadHandle = 1,			///< Client passed a bad subsession handle.
		EScsClAsyncAlreadyQueued = 2,	///< Client attempted to requeue an outstanding request.
		
		/** No-arg session-relative function identifier used reserved SCS bits. */
		EScsClNoArgsSessUsedScs = 4,
		
		/** Arg session-relative function identifier used reserved SCS bits. */
		EScsClArgsSessUsedScs = 5,
		
		/** Arg session-relative async function identifier used reserved SCS bits. */
		EScsClArgsSessAsyncUsedScs = 6,
		
		/** Session-relative cancel function identifier used reserved SCS bits. */
		EScsClCancelSessUsedScs = 7,
		
		/** No-arg subsession-relative function identifier used reserved SCS bits. */
		EScsClNoArgsSubsessUsedScs = 16,
		
		/** Arg subsession-relative function identifier used reserved SCS bits. */
		EScsClArgsSubsessUsedScs = 17,
		
		/** Arg subsession-relative async function identifier used reserved SCS bits. */
		EScsClArgsSubsessAsyncUsedScs = 18,
		
		/* Subsesion-relative cancel function identifier used reserved SCS bits. */
		EScsClCancelSubsessUsedScs = 19
		};
	
	// defined only in client-side implementation
	void ClientSidePanic(ScsImpl::TScsClientPanic aReason);
	}	// namespace ScsImpl

#include <scs/scscommon.inl>

#endif	// #ifndef SCSCOMMON_H

