/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of the ScanListIterator class
*
*/


#ifndef GENSCANLISTITERATOR_H
#define GENSCANLISTITERATOR_H

#include "genscanlist.h"

class ScanList;

/**
*  Iterator for TScanList.
*/
NONSHARABLE_CLASS( ScanListIterator )
    {
public:

    /**
     * Return the size of the scan info. The size includes the static
     * control information header and the actual frame data.
     *
     * @return The size of the scan info in bytes.
     */
    inline u16_t Size() const;

    /**
     * Return the first beacon/probe response frame.
     *
     * @return NULL if the container is empty, pointer to the first
     *          frame in the container otherwise.
     */
    const ScanFrame* First();

    /**
     * Return the next beacon/probe response frame.
     *
     * @return NULL if the container is empty or end of the container
     *         has been reached, pointer to the next frame otherwise.
     */
    const ScanFrame* Next();

    /**
     * Return the beacon/probe response frame currently iterated.
     *
     * @return NULL if no frame has been iterated to, pointer to the
     *         frame otherwise.
     */
    const ScanFrame* Current() const;

    /**
     * Check whether the are more beacon/probe response frames to be iterated.
     *
     * @return false_t if end of the container has been reached, true_t otherwise.
     */
    bool_t IsDone() const;
    
protected: // Methods

    /**
     * Private C++ constructor to prevent instantiation.
     *
     * @param scan_list Reference to the scan list container.
     */
    explicit ScanListIterator( const ScanList& scan_list );

private: // Methods

    /**
     * Prohibit copy constructor.
     */
    ScanListIterator(
        const ScanListIterator& );
    /**
     * Prohibit assigment operator.
     */
    ScanListIterator& operator= (
        const ScanListIterator& );

protected: // Data

    /** Pointer to the beginning of the current frame. */
    const ScanFrame* current_m;

private:  // Data
    /** Reference to the scan list container. */
    const ScanList& scan_list_m;
    };

#include "genscanlistiterator.inl"

#endif      // GENSCANLISTITERATOR_H  
