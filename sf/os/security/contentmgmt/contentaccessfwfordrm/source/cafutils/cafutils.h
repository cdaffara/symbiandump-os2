/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains constant definitions required by clients accessing the 
* Content Access Framework.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CAFUTILS_H__
#define __CAFUTILS_H__

#include <e32std.h>
#include <charconv.h>

namespace ContentAccess
	{
	/** Provides static utility functions for the Content Access Framework*/
	class TCafUtils
		{
	public:
		/** Utility function to read a descriptor from a stream 
		@param aStream The stream
		@return An HBufC descriptor populated with the data from the stream
		*/
		static HBufC* ReadDescriptor16L(RReadStream& aStream);

		/** Utility function to write a descriptor to a stream 
		@param aStream The stream
		@param aDescriptor The descriptor
		*/
		static void WriteDescriptor16L(RWriteStream& aStream, const TDesC& aDescriptor);

		/** Utility function to read a descriptor from a stream 
		@param aStream The stream
		@return An HBufC descriptor populated with the data from the stream
		*/
		static HBufC8* ReadDescriptor8L(RReadStream& aStream);
		
		/** Utility function to read from a stream into an existing descriptor
		@param aStream The stream
		@param aBuffer Buffer to receive the data
		@leave KErrOverflow If the buffer is not large enough to hold the descriptor
		*/
		static void ReadDescriptor8L(RReadStream& aStream, TDes8& aBuffer);

		/** Utility function to write a descriptor to a stream 
		@param aStream The stream
		@param aDescriptor The descriptor
		*/
		static void WriteDescriptor8L(RWriteStream& aStream, const TDesC8& aDescriptor);
		};
	}
#endif // __CAF_H__
