/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "ActiveWriter.h"
#include "AcmConstants.h"
#include "AcmPanic.h"
#include "WriteObserver.h"
#include "AcmUtils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ActiveWriterTraces.h"
#endif


CActiveWriter::CActiveWriter(MWriteObserver& aParent, RDevUsbcClient& aLdd, TEndpointNumber aEndpoint)
 :	CActive(KEcacmAOPriority), 
	iParent(aParent),
	iLdd(aLdd),
	iEndpoint(aEndpoint),
	iFirstPortion(NULL, 0),
	iSecondPortion(NULL, 0)
/**
 * Constructor.
 *
 * @param aParent The object that will be notified when write requests 
 * complete.
 * @param aLdd The LDD handle to be used for posting write requests.
 * @param aEndpoint The endpoint to write to.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_CACTIVEWRITER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEWRITER_CACTIVEWRITER_CONS_EXIT );
	}

CActiveWriter::~CActiveWriter()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_CACTIVEWRITER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CACTIVEWRITER_CACTIVEWRITER_DES_EXIT );
	}

CActiveWriter* CActiveWriter::NewL(MWriteObserver& aParent, 
								   RDevUsbcClient& aLdd,
								   TEndpointNumber aEndpoint)
/**
 * Standard two phase constructor.
 *
 * @param aParent The object that will be notified when write requests 
 * complete.
 * @param aLdd The LDD handle to be used for posting write requests.
 * @param aEndpoint The endpoint to write to.
 * @return Ownership of a new CActiveWriter object.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_NEWL_ENTRY );
	CActiveWriter* self = new(ELeave) CActiveWriter(aParent, aLdd, aEndpoint);
	OstTraceFunctionExit0( CACTIVEWRITER_NEWL_EXIT );
	return self;
	}

void CActiveWriter::Write(const TDesC8& aDes, 
					TInt aLen, 
					TBool aZlp)
/**
 * Write the given data to the LDD.
 *
 * @param aDes A descriptor to write.
 * @param aLen The length to write.
 * @param aZlp Whether ZLP termination may be required.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_WRITE_ENTRY );
	
	if ( aZlp )
		{
		// the driver can be relied on to correctly handle appended ZLPs
		// so use them when necessary..
		iLdd.Write(iStatus, iEndpoint, aDes, aLen, ETrue);
		iWritingState = ECompleteMessage;
		}
	else
		{
		// do we need to send this descriptor in two portions to
		// avoid finishing the last packet on a 64 byte boundary ( avoiding
		// expectations of a ZLP by drivers that would handle them ) ?
		
		// If the write request is for zero bytes a 'split' would be erroneous.
		TBool full64BytePacket = ( aLen % KMaxPacketSize ) ? EFalse : ETrue;
		
		if ( full64BytePacket == EFalse || aLen == 0 )
			{
			iLdd.Write(iStatus, iEndpoint, aDes, aLen, EFalse);
			iWritingState = ECompleteMessage;
			OstTrace1( TRACE_NORMAL, CACTIVEWRITER_WRITE, "CActiveWriter::Write;Writing %d bytes", aLen );
			}
		else
			{
			// we do need to split the descriptor, sending aLen-1 bytes now 
			// and sending a second portion with the remaining 1 byte later
			iFirstPortion.Set(aDes.Left(aLen-1));
			
			// Use of Left here ensures that if we've been passed a descriptor
			// longer than aLen (doesn't *currently* happen), we don't corrupt
			// data.
			iSecondPortion.Set(aDes.Left(aLen).Right(1));
			
			iLdd.Write(iStatus, iEndpoint, iFirstPortion, aLen-1, EFalse);
			
			iWritingState = EFirstMessagePart;
			OstTraceExt2( TRACE_NORMAL, CACTIVEWRITER_WRITE_DUP1, 
					"CActiveWriter::Write;Writing %d bytes of the %d", aLen-1, aLen );
			}
		}
	SetActive();
	OstTraceFunctionExit0( CACTIVEWRITER_WRITE_EXIT );
	}

void CActiveWriter::DoCancel()
/**
 * Cancel an outstanding write.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_DOCANCEL_ENTRY );
	iLdd.WriteCancel(iEndpoint);
	OstTraceFunctionExit0( CACTIVEWRITER_DOCANCEL_EXIT );
	}

void CActiveWriter::RunL()
/**
 * This function will be called when the write completes. It notifies the 
 * parent class of the completion.
 */
	{
	OstTraceFunctionEntry0( CACTIVEWRITER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACTIVEWRITER_RUNL, "CActiveWriter::RunL;iStatus=%d", iStatus.Int() );
	
	if ( iWritingState == EFirstMessagePart )
		{
		if ( iStatus.Int() == KErrNone )
			{			
			// now send the second part..
			iLdd.Write(iStatus, iEndpoint, iSecondPortion, iSecondPortion.Length(), EFalse);
			iWritingState = EFinalMessagePart;
			OstTrace0( TRACE_NORMAL, CACTIVEWRITER_RUNL_DUP1, 
					"CActiveWriter::RunL;Writing 1 byte to complete original nx64 byte message" );
			SetActive();
			}
		else
			{
			// the writing of the first part failed
			iParent.WriteCompleted(iStatus.Int());
			}
		}
	else 
		{
		// iWritingState == ECompleteMessage or EFinalMessagePart
		iParent.WriteCompleted(iStatus.Int());
		}
		
	OstTraceFunctionExit0( CACTIVEWRITER_RUNL_EXIT );
	}

//
// End of file
