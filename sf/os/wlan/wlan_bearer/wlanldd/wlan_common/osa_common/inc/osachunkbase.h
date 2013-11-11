/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   osachunkbase interface declaration
*
*/

/*
* %version: 4 %
*/

#ifndef M_WLANOSACHUNKBASE_H
#define M_WLANOSACHUNKBASE_H

/**
 *  osachunk interface declaration
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanOsaChunkBase 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanOsaChunkBase() {};

    /**
     * Memory allocation. 
     * Correct alignment guaranteed
     *
     * @since S60 v3.2
     * @param aSize size of the buffer to be allocated in bytes. 
     *              Must be positive otherwise the allocation fails
     * @param aZeroStamp zero stamp memory or not
     * @return begin of the allocated memory, NULL upon failure
     */
    virtual void* Alloc( TInt aSize, TBool aZeroStamp ) = 0;

    /**
     * Releases memory allocated by the Alloc method
     *
     * @since S60 v3.2
     * @param aPtr begin of the buffer to be freed
     */
    virtual void Free( void* aPtr ) = 0;

    /**
     * Returns the maximum free link size
     *
     * @since S60 v3.2
     * @return maximum free link size
     */
    virtual TInt MaxFreeLinkSize() const = 0;

    /**
     * Evaluates is chunk in use or not, 
     * meaning is memory allocated from there or not
     *
     * @since S60 v3.2
     * @return ETrue for unused chunk any other for used chunk
     */
    virtual TBool IsInUse() const = 0;

    /**
     * Returns the size of the memory chunk meta header in bytes
     *
     * @since S60 v3.2
     * @return size of the memory chunk meta header
     */
    virtual TInt HeaderSize() const = 0;
    };

#endif // M_WLANOSACHUNKBASE_H
