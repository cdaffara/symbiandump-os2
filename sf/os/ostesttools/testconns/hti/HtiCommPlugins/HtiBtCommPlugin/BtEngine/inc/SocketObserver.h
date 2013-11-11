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
* Description:  The error notification interface.
*
*/


#ifndef __SOCKETOBSERVER_H__
#define __SOCKETOBSERVER_H__

// CLASS DECLARATION

/**
* MEngineNotifier.
* This class specifies the error notification interface.
*/
class MSocketObserver
    {
    public: // Enumerations

        /**
        * TErrorType.
        * Error types encountered when reading/writing to a sockets.
        *  - EDisconnected.      A disconnected error has been encountered.
        *  - ETimeOutOnWrite.    A write operation has failed to complete
        *                        within a predetermined period.
        *  - EGeneralReadError.  A general error has been encountered
        *                        during a read.
        *  - EGeneralWriteError. A general error has been encountered
        *                        during a write
        */
        enum TErrorType
            {
            EDisconnected,
            ETimeOutOnWrite,
            EGeneralReadError,
            EGeneralWriteError
            };

    public: // New functions

        /**
        * ReportError.
        * Reports a communication error.
        * @param aErrorType Error type.
        * @param aErrorCode Associated error code.
        */
        virtual void ReportError( TErrorType aErrorType, TInt aErrorCode ) = 0;

        /**
        * NewData.
        * Data has been received on the socket and read into a buffer.
        * @param aData The data buffer.
        */
        virtual void NewData(const TDesC8& aData) = 0;

        virtual void AllBufferedDataSent() = 0;
    };

#endif // __SOCKETOBSERVER_H__

// End of File
