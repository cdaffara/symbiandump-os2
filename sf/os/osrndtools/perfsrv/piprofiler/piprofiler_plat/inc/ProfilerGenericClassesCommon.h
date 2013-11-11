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


#ifndef PROFILERGENERICCLASSESCOMMON_H
#define PROFILERGENERICCLASSESCOMMON_H

	#include <e32cmn.h>

/*
 *	
 *	Class TProfilerSampleBufStruct definition
 *
 */


class TProfilerSampleBufStruct
{
public:
	TUint32 iSampleRemainder;
	TUint8	iDataStart;
};

/*
 *  
 *  Class TBapBuf definition
 *
 */

class TBapBuf
{
public:
	TPtr8*		iDes;
	TPtr8*		iBufDes;
	TInt		iBufferSize;
	TInt		iDataSize;
	TUint8*		iBuffer;
	TBapBuf*	iNext;
};

#endif
