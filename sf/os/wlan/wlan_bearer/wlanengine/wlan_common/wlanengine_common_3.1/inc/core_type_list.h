/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A template class implementing a linked-list.
*
*/


#ifndef CORE_TYPE_LIST_H
#define CORE_TYPE_LIST_H

#include "core_types.h"
#include "am_debug.h"

/** Defining this enables further iterator traces. */
//#define WLAN_CORE_DEEP_DEBUG 1

template<class type_t> class core_type_list_iterator_c;

/** Default priority for list entries. */
const u32_t LIST_LOW_PRIORITY       = 0;
const u32_t LIST_DEFAULT_PRIORITY   = 10;
const u32_t LIST_MEDIUM_PRIORITY    = 15;
const u32_t LIST_HIGH_PRIORITY      = 20;
const u32_t LIST_TOP_PRIORITY       = 30; // This priority must be the highest

/**
 * Data structure for storing a single list entry.
 *
 * @since S60 v3.2
 */
template<class type_t> struct core_type_list_entry_s
    {
    /** 
     * Constructor.
     */
    core_type_list_entry_s(
        u32_t priority,
        type_t* entry,
        core_type_list_entry_s* next,
        core_type_list_entry_s* prev ) :
        priority_m( priority ),
        entry_m( entry ),
        next_m( next ),
        prev_m( prev )
        {            
        }

    /** Defines the priority level of the entry. */
    u32_t priority_m;

    /** Pointer to entry data. */
    type_t* entry_m;

    /** Pointer to the next entry. */
    core_type_list_entry_s* next_m;

    /** Pointer to the previous entry. */
    core_type_list_entry_s* prev_m;
    };

/**
 * A template class implementing a linked-list.
 *
 * The list is sorted by priority in either descending
 * or ascending order.
 *
 * The class contains an internal iterator that can
 * be accessed via first() and next() methods. This
 * functionality is deprecated, the external iterator
 * core_type_list_iterator_c should be used instead.
 *
 * @since S60 v3.2
 */
template<class type_t> NONSHARABLE_CLASS( core_type_list_c )
    {
    friend class core_type_list_iterator_c<type_t>;

public:

    /**
     * Constructor.
     *
     * @param is_order_descending Whether the list is sorted in descending or ascending
     *                            priority order.     
     * @param is_owned Whether the list claims ownership of the entries.
     */
    core_type_list_c(
        bool_t is_order_descending = true_t,
        bool_t is_owned = true_t );

    /**
     * Destructor.
     */
    ~core_type_list_c();

    /**
     * Append an entry to the list. 
     *
     * @since S60 v3.2
     * @param entry The entry to be added.
     * @param priority Priority of the entry.
     * @return core_error_ok if success, an error code otherwise.
     * @note The list takes ownership of the passed entry pointer.
     */
    core_error_e append(
        type_t* entry,
        u32_t priority = LIST_DEFAULT_PRIORITY );

    /**
     * Remove an entry from the list.
     *
     * @since S60 v3.2
     * @param entry The entry to be removed.
     * @return core_error_ok if success, an error code otherwise.
     * @note The ownership of the entry is passed to the calling party.
     */    
    core_error_e remove(
        const type_t* entry );

    /**
     * Remove all entries from the list and delete them.
     *
     * @since S60 v3.2
     */
    void clear();
    
    /**
     * Return the first entry in the list.
     *
     * @since S60 v3.2
     * @return NULL if the list empty, pointer to the first entry otherwise.
     * @note This method will reset the internal iterator.
     */
    type_t* first();
    
    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.2
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    type_t* next();

    /**
     * Return the current entry in the list.
     *
     * @since S60 v3.2
     * @return pointer to the current entry.
     */
    type_t* current() const;

    /**
     * Return the amount of entries in the list.
     *
     * @since S60 v3.2
     * @return The amount of entries in the list.
     */    
    u32_t count() const;

protected:

    /**
     * Register an iterator so that it can be notified when an entry is removed.
     * 
     * @param iterator Iterator to register.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e register_iterator(
        core_type_list_iterator_c<type_t>* iterator );

    /**
     * Unregister an iterator.
     *
     * @param iterator Iterator to unregister.
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e unregister_iterator(
        core_type_list_iterator_c<type_t>* iterator );

    /**
     * Called by a core_type_list_iterator_c before an entry is removed.
     *
     * This method will cause the list to notify all registerd iterators
     * about the deletion.
     *
     * @param entry Entry to be removed.
     */
    void on_remove(
        core_type_list_entry_s<type_t>* entry );

private:

    /** Prohibit copy constructor. */
    core_type_list_c(
        const core_type_list_c& );

    /** Prohibit assigment operator. */
    core_type_list_c& operator=(
        const core_type_list_c& );

private: // data

    /** The list head entry. */
    core_type_list_entry_s<type_t> list_head_m;

    /** The first entry in the list of registered iterators. */
    core_type_list_entry_s< core_type_list_iterator_c<type_t> >* iter_head_m;

    /** Iterator for iterating through the list of entries. */
    core_type_list_iterator_c<type_t> list_iter_m;

    /** The amount of entries in the list. */
    u32_t count_m;

    /** Whether the list order is descending or ascending. */
    bool_t is_order_descending_m;

    /** Whether the list claims ownership of the entries. */
    bool_t is_owned_m;

    };

/**
 * This class implements a robust iterator for core_type_list_c class.
 *
 * @since S60 v3.2
 */
template<class type_t> NONSHARABLE_CLASS( core_type_list_iterator_c )
    {
    friend class core_type_list_c<type_t>;

public:

    /**
     * Constructor.
     *
     * @param list Reference to the list being iterated.
     */
    core_type_list_iterator_c(
        core_type_list_c<type_t>& list );

    /**
     * Destructor.
     */
    ~core_type_list_iterator_c();

    /**
     * Return the first entry in the list.
     *
     * @since S60 v3.2
     * @return NULL if the list empty, pointer to the first entry otherwise.
     */
    type_t* first();

    /**
     * Return the next entry in the list.
     *
     * @since S60 v3.2
     * @return NULL if the current entry is the last one,
     *         pointer to the next entry otherwise.
     */
    type_t* next();

    /**
     * Return the current entry in the list.
     *
     * @since S60 v3.2
     * @return NULL if an empty list or at the end of the list,
     *         pointer to the current entry otherwise.
     */
    type_t* current() const;

    /**
     * Remove the current entry from the list.
     *
     * @since S60 v3.2
     * @return core_error_ok if removed successfully, an error code otherwise.
     */
    core_error_e remove();

protected:

    /**
     * Called by core_type_list_c before an entry is removed.
     *
     * @param entry Entry to be removed.
     */
    void on_remove(
        core_type_list_entry_s<type_t>* entry );

private:

    /** Prohibit copy constructor. */
    core_type_list_iterator_c(
        const core_type_list_iterator_c& );

    /** Prohibit assigment operator. */
    core_type_list_iterator_c& operator=(
        const core_type_list_iterator_c& );

private: // data

    /**
     * Handle to the actual list.
     */
    core_type_list_c<type_t>& list_m;

    /**
     * Pointer to the currently iterated entry. Not owned by this pointer.
     */
    core_type_list_entry_s<type_t>* list_iter_m;

    /**
     * Whether the iterator has an active registration with the list.
     */
    bool_t is_registed_m;

    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_type_list_c<type_t>::core_type_list_c(
    bool_t is_order_descending,
    bool_t is_owned ) :
    list_head_m( 0, NULL, NULL, NULL ),
    iter_head_m( NULL ),
    list_iter_m( *this ),
    count_m( 0 ),
    is_order_descending_m( is_order_descending ),
    is_owned_m( is_owned )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_type_list_c<type_t>::~core_type_list_c()
    {
    clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_error_e core_type_list_c<type_t>::append(
    type_t* entry,
    u32_t priority )
    {
    if( !entry )
        {
        DEBUG( "core_type_list_c::append() - entry is NULL" );

        return core_error_illegal_argument;
        }

    core_type_list_entry_s<type_t>* list_entry = new core_type_list_entry_s<type_t>(
        priority, entry, NULL, NULL );
    if( !list_entry )
        {
    	DEBUG( "core_type_list_c::append() - unable to instantiate core_type_list_entry_s" );

    	return core_error_no_memory;
    	}

    core_type_list_entry_s<type_t>* prev = &list_head_m;
    core_type_list_entry_s<type_t>* current = list_head_m.next_m;
    bool_t is_done( false_t );

    while( current && !is_done )
        {
        if( is_order_descending_m &&
            current->priority_m < list_entry->priority_m )
            {
            is_done = true_t;
            }
        else if( !is_order_descending_m &&
                 current->priority_m > list_entry->priority_m )
            {
            is_done = true_t;
            }
        else
            {
            prev = current;
            current = current->next_m;
            }
        }

    list_entry->next_m = current;
    list_entry->prev_m = prev;
    prev->next_m = list_entry;
    if( current )
        {
        current->prev_m = list_entry;
        }

    ++count_m;

	return core_error_ok;        	 
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_error_e core_type_list_c<type_t>::remove(
    const type_t* entry )
    {
    core_type_list_iterator_c<type_t> iter( *this );
    for( type_t* current = iter.first(); current; current = iter.next() )
        {
        if ( current == entry )
            {
            iter.remove();

            return core_error_ok;
            }
        }

    return core_error_not_found;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> void core_type_list_c<type_t>::clear()
    {
    for( type_t* current = list_iter_m.first(); current; current = list_iter_m.next() )
        {
        list_iter_m.remove();

        if ( is_owned_m )
            {
            delete current;
            current = NULL;           
            }
        }

    ASSERT( !count_m );
    ASSERT( !list_head_m.next_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_c<type_t>::first()
    {
    return list_iter_m.first();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_c<type_t>::next()
    {
    return list_iter_m.next();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_c<type_t>::current() const
    {
    return list_iter_m.current();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> u32_t core_type_list_c<type_t>::count() const
    {
    return count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_error_e core_type_list_c<type_t>::register_iterator(
    core_type_list_iterator_c<type_t>* iterator )
    {
#ifdef WLAN_CORE_DEEP_DEBUG
    DEBUG1( "core_type_list_c::register_iterator() - registering iterator 0x%08X",
        iterator );
#endif // WLAN_CORE_DEEP_DEBUG
    
    if( !iterator )
        {
        DEBUG( "core_type_list_c::register_iterator() - iterator is NULL" );

        return core_error_illegal_argument;
        }

    core_type_list_entry_s< core_type_list_iterator_c<type_t> >* iter_entry =
        new core_type_list_entry_s< core_type_list_iterator_c<type_t> >(
            0,
            iterator,
            iter_head_m,
            NULL );
    if( !iter_entry )
        {
    	DEBUG( "core_type_list_c::register_iterator() - unable to instantiate core_type_list_entry_s" );

    	return core_error_no_memory;
    	}

    if ( iter_head_m )
        {
        iter_head_m->prev_m = iter_entry;
        }
    iter_head_m = iter_entry;

	return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_error_e core_type_list_c<type_t>::unregister_iterator(
    core_type_list_iterator_c<type_t>* iterator )
    {
#ifdef WLAN_CORE_DEEP_DEBUG
    DEBUG1( "core_type_list_c::unregister_iterator() - unregistering iterator 0x%08X",
        iterator );
#endif // WLAN_CORE_DEEP_DEBUG

    if( !iterator )
        {
        DEBUG( "core_type_list_c::unregister_iterator() - iterator is NULL" );

        return core_error_illegal_argument;
        }

    core_type_list_entry_s< core_type_list_iterator_c<type_t> >* iter_entry = iter_head_m;
    while( iter_entry )
        {
        if ( iter_entry->entry_m == iterator )
            {
            if( !iter_entry->prev_m )
                {
                /**
                 * First entry in the list.
                 */
                ASSERT( iter_head_m == iter_entry );

                if ( iter_entry->next_m )
                    {
                    iter_entry->next_m->prev_m = NULL;
                    }
                iter_head_m = iter_entry->next_m;
                }
            else
                {
                /**
                 * Somewhere else in the list.
                 */
                iter_entry->prev_m->next_m = iter_entry->next_m;
                if ( iter_entry->next_m )
                    {
                    iter_entry->next_m->prev_m = iter_entry->prev_m;
                    }
                }

            delete iter_entry;
            iter_entry = NULL;

            return core_error_ok;
            }

        iter_entry = iter_entry->next_m;
        }

    return core_error_not_found;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> void core_type_list_c<type_t>::on_remove(
    core_type_list_entry_s<type_t>* entry )
    {
#ifdef WLAN_CORE_DEEP_DEBUG
    DEBUG1( "core_type_list_c::on_remove() - entry 0x%08X will be removed, notifying iterators",
        entry );
#endif // WLAN_CORE_DEEP_DEBUG

    core_type_list_entry_s< core_type_list_iterator_c<type_t> >* iter_entry = iter_head_m;
    while( iter_entry )
        {
#ifdef WLAN_CORE_DEEP_DEBUG
        DEBUG1( "core_type_list_c::on_remove() - notifying iterator 0x%08X",
            iter_entry->entry_m );
#endif // WLAN_CORE_DEEP_DEBUG

        iter_entry->entry_m->on_remove( entry );

        iter_entry = iter_entry->next_m;
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_type_list_iterator_c<type_t>::core_type_list_iterator_c(
    core_type_list_c<type_t>& list ) :
    list_m( list ),
    list_iter_m( &list_m.list_head_m ),
    is_registed_m( false )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_type_list_iterator_c<type_t>::~core_type_list_iterator_c()
    {
    if ( is_registed_m )
        {
        list_m.unregister_iterator( this );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_iterator_c<type_t>::first()
    {
    if ( !is_registed_m )
        {
        list_m.register_iterator( this );
        is_registed_m = true_t;
        }

    list_iter_m = list_m.list_head_m.next_m;
    if ( list_iter_m )
        {
        return list_iter_m->entry_m;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_iterator_c<type_t>::next()
    {
    if ( !list_iter_m )
        {
        return NULL;
        }

    list_iter_m = list_iter_m->next_m;
    if ( list_iter_m )
        {
        return list_iter_m->entry_m;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> type_t* core_type_list_iterator_c<type_t>::current() const
    {
    if ( !list_iter_m ||
         list_iter_m == &list_m.list_head_m )
        {
        return NULL;        
        }

    return list_iter_m->entry_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> core_error_e core_type_list_iterator_c<type_t>::remove()
    {
    if ( !list_iter_m ||
         list_iter_m == &list_m.list_head_m )
        {
        return core_error_illegal_argument;
        }

    core_type_list_entry_s<type_t>* current = list_iter_m;

    // Notify list about the removal.
    list_m.on_remove( current );

    current->prev_m->next_m = current->next_m;
    if ( current->next_m )
        {
        current->next_m->prev_m = current->prev_m;
        }
    --list_m.count_m;

    delete current;
    current = NULL;

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
template<class type_t> void core_type_list_iterator_c<type_t>::on_remove(
    core_type_list_entry_s<type_t>* entry )
    {
    if ( !list_iter_m ||
         list_iter_m == &list_m.list_head_m )
        {
        return;
        }

    if ( list_iter_m == entry )
        {
#ifdef WLAN_CORE_DEEP_DEBUG
        DEBUG1( "core_type_list_iterator_c::on_remove() - iterator 0x%08X is affected",
            this );
#endif // WLAN_CORE_DEEP_DEBUG

        list_iter_m = list_iter_m->prev_m;
        }
#ifdef WLAN_CORE_DEEP_DEBUG
    else
        {
        DEBUG1( "core_type_list_iterator_c::on_remove() - iterator 0x%08X is not affected",
            this );        
        }
#endif // WLAN_CORE_DEEP_DEBUG
    }

#endif // CORE_TYPE_LIST_H
