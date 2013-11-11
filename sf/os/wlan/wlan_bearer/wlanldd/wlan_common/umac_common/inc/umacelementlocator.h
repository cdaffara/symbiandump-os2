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
* Description:   Declaration of the WlanElementLocator class.
*
*/

/*
* %version: 8 %
*/

#ifndef WLAN_ELEMENT_LOCATOR_H
#define WLAN_ELEMENT_LOCATOR_H


/**
 *  Provides methods to locate 802.11 information elements.
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class WlanElementLocator 
    {

public:

    enum TWlanLocateStatus
    {
        EWlanLocateOk,
        EWlanLocateElementNotFound
    };

    /**
     * Constructor. 
     *
     * @since S60 3.1
     * @param aStart   (IN) beginning of the area from where to locate 
     *                 elements
     * @param aLength  (IN) length of the area to search
     */
    WlanElementLocator( const TUint8* aStart, TUint16 aLength );

    /**
    * Return requested information element data.
    * @param aIeId     Id of the requested IE data. See 802dot11.h
    * @param aIeLength (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData   (OUT) Pointer to the beginning of the IE data. 
    *                  NULL if IE not found.
    * @param aValidateLength (IN) If ETrue, the indicated length of the IE 
    *                  data part is checked for validity against the relevant
    *                  specification.
    *                  If EFalse, the validity check is not done.
    * @return          Locate status.
    */
    TWlanLocateStatus InformationElement( 
        TUint8 aIeId, 
        TUint8& aIeLength, 
        const TUint8** aIeData,
        TBool aValidateLength = ETrue );    

    /**
    * Return requested information element data.
    *
    * @since S60 3.2
    * @param aIeId          (IN)  Id of the requested IE data. See 802dot11.h
    * @param aIeOui         (IN)  OUI of the requested IE data. See 802dot11.h
    * @param aIeOuiType     (IN)  OUI Type of the requested IE data. 
    *                             See 802dot11.h
    * @param aIeLength      (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData        (OUT) Pointer to the beginning of the IE data. 
    *                             NULL if IE not found.
    * @return               Locate status.
    */
    TWlanLocateStatus InformationElement( 
        TUint8 aIeId,
        const TIeOui& aIeOui,
        TUint8 aIeOuiType,
        TUint8& aIeLength, 
        const TUint8** aIeData );

    /**
    * Return requested information element data.
    * @param aIeId          (IN)  Id of the requested IE data. See 802dot11.h
    * @param aIeOui         (IN)  OUI of the requested IE data. See 802dot11.h
    * @param aIeOuiType     (IN)  OUI Type of the requested IE data. 
    *                             See 802dot11.h
    * @param aIeOuiSubtype  (IN)  OUI Subtype of the requested IE data. 
    *                             See 802dot11.h
    * @param aIeLength      (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData        (OUT) Pointer to the beginning of the IE data. 
    *                             NULL if IE not found.
    * @return               Locate status.
    */
    TWlanLocateStatus InformationElement( 
        TUint8 aIeId,
        const TIeOui& aIeOui,
        TUint8 aIeOuiType,
        TUint8 aIeOuiSubtype,    
        TUint8& aIeLength, 
        const TUint8** aIeData );
                                                                                    
private:

    /**
    * Return the first information element data.
    * @param aIeId     (OUT) Id of the IE. See 802dot11.h.
    * @param aIeLength (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData   (OUT) Pointer to the beginning of the IE data. 
    *                  NULL if IE not found.
    * @return          Locate status.
    */
    TWlanLocateStatus FirstIE( 
        TUint8& aIeId, 
        TUint8& aIeLength, 
        const TUint8** aIeData );
                                    
    /**
    * Return next information element data.
    * @param aIeId     (OUT) Id of the IE. See 802dot11.h.
    * @param aIeLength (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData   (OUT) Pointer to the beginning of the IE data. 
    *                  NULL if IE not found.
    * @return          Locate status.
    */
    TWlanLocateStatus NextIE( 
        TUint8& aIeId, 
        TUint8& aIeLength, 
        const TUint8** aIeData );
                                   
    /**
    * Return current information element data.
    * @param aIeId     (OUT) Id of the IE. See 802dot11.h.
    * @param aIeLength (OUT) Length of the IE. Zero if IE not found.
    * @param aIeData   (OUT) Pointer to the beginning of the IE data. 
    *                  NULL if IE not found.
    * @return          Locate status.
    */
    TWlanLocateStatus CurrentIE( 
        TUint8& aIeId, 
        TUint8& aIeLength, 
        const TUint8** aIeData ) const;

    /**
    * Validates the indicated length of the IE data part (aIeLength) against
    * the relevant specification. 
    * @param aIeId         (IN) Id of the IE. See 802dot11.h.
    * @param aIeLength     (IN) Length of the IE.
    * @param aIeData       (IN) Pointer to the beginning of the IE data. NULL
    *                      if not relevant for the validity check.
    * @param aIeOuiType    (IN) OUI Type of the IE (if relevant)
    * @param aIeOuiSubtype (IN) OUI Subtype of the IE (if relevant)
    * @return              
    */
    WlanElementLocator::TWlanLocateStatus ValidIE(
        TUint8 aIeId,
        TUint8 aIeLength,
        const TUint8* aIeData = 0,
        TUint8 aIeOuiType = KWmmElemOuiType,
        TUint8 aIeOuiSubtype = KWmmInfoElemOuiSubType ) const;
        
    // Prohibit copy constructor.
    WlanElementLocator( const WlanElementLocator& );
    // Prohibit assigment operator.
    WlanElementLocator& operator= 
        ( const WlanElementLocator& );

private:   // data

    /** start of the search area */    
    const TUint8* iStart;
    /** length of the search area */        
    TUint16 iLength;
    /** Iterator for going through elements */
    const TUint8* iIterator;
    };

#endif // WLAN_ELEMENT_LOCATOR_H
