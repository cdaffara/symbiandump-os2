/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  scan info
*
*/


#ifndef WLANSCANINFOIMPL_H
#define WLANSCANINFOIMPL_H

// INCLUDES
#include "rwlmserver.h"
#include "genscaninfo.h"
#include "genscanlist.h"
#include "wlanscaninfo.h"

class ScanInfo;

// CLASS DECLARATION
/**
* Implementation for MWlanScanInfoBase and MWlanScanInfoIteratorBase interfaces.
*
* This class uses services from ScanInfo and ScanList classes to implement
* most of the required functionality.
* @lib wlanmgmtimpl.dll
* @since S60 3.0
*/
class CWlanScanInfoImpl : public CWlanScanInfo
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
         * Static constructor.
         */
        static CWlanScanInfoImpl* NewL();
        
        /**
         * Destructor.
         */
        virtual ~CWlanScanInfoImpl();

       // New methods

		/**
		 * Return the reference to the internal scan list.
		 * @return Internal scan list.
		 */
        ScanList& GetInternalList();

		/**
		 * Update the internal variables after the internal
		 * scan list has been updated.
		 * @return KErrNone if results updated successfully, an error code otherwise.
		 */
        TInt UpdateResults();

       // Methods from base classes

        /**
         * (From MWlanScanInfoIteratorBase) Return the size of the scan info.
		 * The size includes Status Info, MAC header and Frame Body.
         * @return The size of the scan info in bytes.
         */
        virtual TUint16 Size() const;

        /**
         * (From MWlanScanInfoIteratorBase) Find the data of the first access point.
         * @return Pointer at the beginning of the first access point stored 
         *         in the scan list. NULL if not any.
         */
        virtual const TWlanScanInfoFrame* First();

        /**
         * (From MWlanScanInfoIteratorBase) Find the data of the next access point.
         * @return Pointer at the beginning of the next access point stored
         *         in the scan list. NULL if not any.
         */
        virtual const TWlanScanInfoFrame* Next();

        /**
         * (From MWlanScanInfoIteratorBase) Find the data of the current access point.
         * @return Pointer at the beginning of the current access point stored 
         *         in the scan list. NULL if not any.
         */
        virtual const TWlanScanInfoFrame* Current() const;

        /**
         * (From MWlanScanInfoIteratorBase) Find is there any more unhandled access points.
         * @return EFalse if there is access points in the list left, 
         *         ETrue if not.
         */
        virtual TBool IsDone() const;

		/**
         * (From MWlanScanInfoBase) Return RX level of the BSS.
         * @return RX level.
         */
        virtual TUint8 RXLevel() const;

        /**
         * (From MWlanScanInfoBase) Return BSSID of the BSS.
         * @param  aBssid ID of the access point or IBSS network.
         * @return Pointer to the beginning of the BSSID. Length is always 6 bytes.
         */
		virtual void Bssid( TWlanBssid& aBssid ) const;

        /**
         * (From MWlanScanInfoBase) Get beacon interval of the BSS.
         * @return the beacon interval.
         */
        virtual TUint16 BeaconInterval() const;

        /**
         * (From MWlanScanInfoBase) Get capability of the BSS (see IEEE 802.11 section 7.3.1.4.
         * @return The capability information.
         */
        virtual TUint16 Capability() const;

        /**
         * (From MWlanScanInfoBase) Get security mode of the BSS.
         * @return security mode.
         * @deprecated This method is offered for backward compatibility reasons,
         *             ExtendedSecurityMode() should be used instead.
         */
        virtual TWlanConnectionSecurityMode SecurityMode() const;

        /**
         * (From MWlanScanInfoBase) Return requested information element.
         * @param aIE        Id of the requested IE data.
         * @param aLength    Length of the IE. Zero if IE not found.
         * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
         * @return           General error message.
         */
        virtual TInt InformationElement( TUint8 aIE, 
                                         TUint8& aLength, 
                                         const TUint8** aData );

        /**
         * (From MWlanScanInfoBase) Return WPA information element.
         * @param aLength    Length of the IE. Zero if IE not found.
         * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
         * @return           General error message.
         */
        virtual TInt WpaIE( TUint8& aLength, 
                            const TUint8** aData );

        /**
         * (From MWlanScanInfoBase) Return the first information element.
         * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
         * @param aLength    Length of the IE. Zero if IE not found.
         * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
         * @return           General error message.
         */
        virtual TInt FirstIE( TUint8& aIE, 
                              TUint8& aLength, 
                              const TUint8** aData );

        /**
         * (From MWlanScanInfoBase) Return next information element.
         * @param aIE        Id of the IE. See IEEE 802.11 section 7.3.2.
         * @param aLength    Length of the IE. Zero if IE not found.
         * @param aData      Pointer to the beginning of the IE data. NULL if IE not found.
         * @return           General error message.
         */
        virtual TInt NextIE( TUint8& aIE, 
                             TUint8& aLength, 
                             const TUint8** aData );
        
        /**
         * Find whether Wi-Fi Protected Setup is supported.
         * @return ETrue if AP supports Wi-Fi Protected Setup,
         *         EFalse if not.
         */
        virtual TBool IsProtectedSetupSupported();

        /**
         * Get security mode of the BSS.
         * @return security mode.
         */
        virtual TWlanConnectionExtentedSecurityMode ExtendedSecurityMode() const;
        
    private: // Methods

        /**
         * C++ default constructor.
         */
        CWlanScanInfoImpl();

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();
        
        /**
         * Convert internal error code to Symbian error code.
         * @param aCode      Internal error code.
         * @return           Symbian error code.
         */
        TInt ConvertErrorCode( TInt aCode ) const;
        
    private: // Data

        /** Scan results */
        ScanList iScanList;

        /** Wrapper class for parsing */
        ScanInfo* iScanInfo;
    };

#endif // WLANSCANINFOIMPL_H
            
// End of File
