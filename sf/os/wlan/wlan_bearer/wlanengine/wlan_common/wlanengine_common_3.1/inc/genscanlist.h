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
* Description:  Declaration of the ScanList class
*
*/


#ifndef GENSCANLIST_H
#define GENSCANLIST_H

#include "am_platform_libraries.h"

/** The default maximum size of the scan list is 50kB. */
const u32_t SCAN_LIST_DEFAULT_MAX_SIZE = 51200;
/** The default granularity of the scan list is 8kB. */
const u32_t SCAN_LIST_DEFAULT_GRANULARITY = 8192;
/** The return value for failed Append(). */
const u32_t APPEND_FAILED_NO_MEMORY = 0;
/**
 * The definition for beacon/probe response frame data.
 * Always used as pointer to the frame data, i.e. ScanFrame*.
 */
typedef u8_t ScanFrame;

/**
 * Container class for delivering scan results.
 */
NONSHARABLE_CLASS( ScanList )
    {

    friend class ScanListIterator;

public: // Methods

    // Constructors and destructor

    /**
     * Constructor for an empty container with default parameters.
     */
    ScanList();

    /**
     * Constructor for an empty container.
     *
     * @param max_size The maximum size of the container.
     * @param granularity The granularity of the container.
     * @note After the initial allocation, memory is always allocated
     *       in multiples of the granularity parameter.
     */
    ScanList(
        u32_t max_size,
        u32_t granularity = SCAN_LIST_DEFAULT_GRANULARITY );

    /**
     * Destructor.
     */
    ~ScanList();

    /**
     * Returns the number of access points the instance contains.
     *
     * @return the number of access points the instance contains.
     */
    inline u32_t Count() const;

    /**
     * Appends one beacon/probe response frame to container.
     *
     * @param data_length Length of the frame data to be appended.
     * @param data Pointer to the frame data.
     * @return APPEND_FAILED_NO_MEMORY if there is not enough space,
     *         otherwise the new size of the container.
     * @note If the current size of the container is not enough for the
     *       frame, the container will automatically allocate more memory.
     */
    u32_t Append(
        u32_t data_length,
        const ScanFrame* data );

    /**
     * Returns the current size of the container.
     *
     * @return The current size of the container in bytes.
     */
    inline u32_t Size() const;

    /**
     * Returns the maximum size of the container.
     *
     * @return The maximum size of the container in bytes.
     */
    inline u32_t MaxSize() const;

    /**
     * Sets the maximum size of currently allocated container to the given size.
     *
     * @param max_size Maximum size of the container in bytes.
     * @return The current maximum size if size could not be changed,
     *         the new maximum size otherwise.
     * @note If the current container is smaller than the given size,
     *       more memory is automatically allocated.
     * @note The maximum size is still limited by the size parameter given
     *       when the class was instantiated.
     * @note Making the container smaller than the current size (other than
     *       zero) is not supported at the moment.
     */
    u32_t SetCurrentMaxSize(
        u32_t max_size );

    /**
     * Return a pointer to the currently allocated container.
     *
     * @return NULL if the container is empty, pointer to the
     *         currently allocated container otherwise.
     */
    inline u8_t* Data() const;

    /**
     * Updates the amount of frames and the current size of the container.
     *
     * @param count The amount of beacon/probe response frames stored in the container.
     * @param size The current size of the container (in bytes).
     * @note This method does not do any checking on the parameters and it
     *       is supposed to be used only when the container has been
     *       directly modified through Data().
     */
    inline void Update(
        u32_t count,
        u32_t size );

    /**
     * Clears the container.
     */
    inline void ClearAll();

    /**
     * The alignment of one beacon/probe response frame data must be four bytes.
     * Returns the amount of needed padding for a particular frame length.
     *
     * @param data_length Length of the frame data in bytes.
     * @return Total length in bytes with additional padding.
     */
    inline static u32_t AddPadding(
        u32_t data_length );

    /**
     * Compresses the container so it will be occupy the minimum amount of memory.
     *
     * @return The current size of the container in bytes.
     */
    inline u32_t Compress();

private: // Methods

    /**
     * Prohibit copy constructor.
     */
    ScanList(
        const ScanList& );

    /**
     * Check that the given frame is valid, i.e. it has all mandatory IE fields included.
     *
     * @param data Pointer to the frame data.
     * @return true_t if the frame is valid, false_t otherwise.
     * @note The data being checked has to be stored in the container.
     */
    bool_t CheckData(
        const ScanFrame* data ) const;

    /**
     * Copy data from source buffer to target buffer.
     *
     * @param trg Pointer to the target buffer.
     * @param src Pointer to the source buffer.
     * @param len Length of data to copy in bytes.
     * @return Pointer to end of copied data in the target buffer.
     */
    u8_t* Copy(
        void* target,
        const void* source,
        u32_t data_length );

private: // Data

    /** The container for storing beacon/probe response frames. */
    u8_t* data_m;
    /** The granularity of the container (in bytes). */
    u32_t granularity_m;
    /** The amount of beacon/probe response frames stored in the container. */
    u32_t count_m;
    /** The current size of the container (in bytes). */
    u32_t size_m;
    /** The maximum size of the currently allocated container (in bytes). */
    u32_t current_max_size_m;
    /** The maximum size of the container (in bytes). The container will never exceed this limit. */
    u32_t max_size_m;
    };

#include "genscanlist.inl"

#endif // GENSCANLIST_H
