/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of TWlanUserTxDataCntx inline 
*                methods.
*
*/

/*
* %version: 8 %
*/

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TUint8* TWlanUserTxDataCntx::StartOfFrame()
    {
    return iStartOfFrame;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void TWlanUserTxDataCntx::LengthOfFrame( TUint aFrameLength )
    {
    iLengthOfData = aFrameLength;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline TUint TWlanUserTxDataCntx::LengthOfFrame() const
    {
    return iLengthOfData;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
inline void TWlanUserTxDataCntx::Dot11FrameReady( 
    TUint8* aStartOfFrame, 
    TUint aLengthOfFrame )
    {
    iStartOfFrame = aStartOfFrame;
    iLengthOfData = aLengthOfFrame;
    }
