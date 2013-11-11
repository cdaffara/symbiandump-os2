/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   inline implementation of list
*
*/

/*
* %version: 6 %
*/

#include <wlanosa.h>
#include "osa_includeme.h"

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline list<T>::~list()
    {
    clear();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::iterator list<T>::begin()
    {
    // for empty ranges begin() == end()
    return ( !(empty()) ? list_iterator<Node, T>( iFirst ) : end() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::const_iterator list<T>::begin() const
    {
    // for empty ranges begin() == end()
    return ( !(empty()) ? list_iterator<Node, T>( iFirst ) : end() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::iterator list<T>::end()
    {
    return list_iterator<Node, T>();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::const_iterator list<T>::end() const
    {
    return list_iterator<Node, T>();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::size_type list<T>::size() const
    {
    return iNumOfElems;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline TBool list<T>::empty() const
    {
    return (!size());
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::reference list<T>::front()
    {
    // front() for empty sequence is undefined so assert
    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__, size() );
    return iFirst->iElem;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::const_reference list<T>::front()  const
    {
    // front() for empty sequence is undefined so assert
    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__, size() );
    return iFirst->iElem;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::reference list<T>::back()
    {
    // back() for empty sequence is undefined so assert
    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__, size() );
    return iLast->iElem;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::const_reference list<T>::back() const
    {
    // back() for empty sequence is undefined so assert
    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__, size() );
    return iLast->iElem;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void list<T>::clear()
    {
    Node* current = iFirst;
    Node* next = current;

    iNumOfElems = 0;
    iFirst = NULL;
    iLast = NULL;

    while ( current )
        {
        next = current->iNext;
        delete current;
        current = next;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void list<T>::push_back( const_reference aElem )
    {
    // allocate a new node for the element
    Node* node = new Node( aElem );

    if ( node )
        {
        // allocation success now append the node to rear
        if ( !empty() )
            {
            // we are not empty just link to rear
            node->iPrev = iLast;
            iLast->iNext = node;
            iLast = node;
            }
        else
            {
            // we are empty, so last and first nodes are the same
            iFirst = node;
            iLast = iFirst;
            }

        ++iNumOfElems;  // allways increment element count upon success
        }
    else
        {
        // allocation failure
        // left intentionally empty
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void list<T>::pop_back()
    {
    Node* prev = iLast->iPrev;

    if ( prev )
        {
        // previous node exists, so make it the new last node
        prev->iNext = NULL;
        iLast = prev;
        }
    else
        {
        // no previous node exist, meaning that we are the only one
        // and as we are gone there exist no one
        iFirst = NULL;
        iLast = NULL;
        }

    --iNumOfElems;  // always decrement upon pop
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void list<T>::push_front( const_reference aElem )
    {
    // allocate a new node for the element
    Node* node = new Node( aElem );

    if ( node )
        {
        // allocation success now append the node to rear
        if ( !empty() )
            {
            // we are not empty just link to front
            node->iNext = iFirst;
            iFirst->iPrev = node;
            iFirst = node;
            }
        else
            {
            // we are empty, so last and first nodes are the same
            iFirst = node;
            iLast = iFirst;
            }

        ++iNumOfElems;  // allways increment element count upon success

        }
    else
        {
        // allocation failure 
        // left intentionally empty
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline void list<T>::pop_front()
    {
    Node* next = iFirst->iNext;

    if ( next )
        {
        // next node exists, so make it the new first node
        next->iPrev = NULL;
        iFirst = next;
        }
    else
        {
        // no next node exist, meaning that we are the only one
        // and as we are gone there exist no one
        iFirst = NULL;
        iLast = NULL;
        }

    --iNumOfElems;  // allways decrement upon pop
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::iterator list<T>::insert( 
    iterator aPos, 
    const T& aElem )
    {
    // allocate a new node for the element
    Node* node = new Node( aElem );

    if ( node )
        {
        // allocation success now insert aElem before aPos
        if ( aPos->iPrev )
            {
            // a previous link exists
            aPos->iPrev->iNext = node; // link old previous node to new node
            }        

        node->iPrev = aPos->iPrev;  // back link new node
        node->iNext = aPos();       // next link new node         
        aPos->iPrev = node;         // back link pos to new node

        // allways increment element count upon success
        ++iNumOfElems;

        const TInt KHeadInsertionCountMark( 2 );
        if ( size() == KHeadInsertionCountMark )
            {
            // set new list head as this insertion was to the front
            iFirst = node;            
            }
        else
            {
            // left intentionally empty
            }
        }
    else
        {
        // allocation failure 
        // left intentionally empty
        TraceDump(ERROR_LEVEL, ("[WLAN] error: allocation"));
        Trace( ERROR_LEVEL, 
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    // return the position of the inserted element (if any as node can be NULL)
    // in case of NULL end() is returned
    return ( (node != NULL) ? list_iterator<Node, T>( node ) : end() );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
template<class T>
inline typename list<T>::iterator list<T>::erase( iterator aPos )
    {
    // extract node to be erased
    Node* node = aPos();

    if ( node->iNext )
        {
        // next node exists
        node->iNext->iPrev = node->iPrev;   // back link next node
        }
    else
        {
        // next node does not exist so we don't have to relink it
        // left intentionally empty
        }

    if ( node->iPrev )
        {
        // previous node exists
        node->iPrev->iNext = node->iNext;   // next link previous node
        }
    else
        {
        // previous node does not exist so we don't have to relink it
        // left intentionally empty
        }
    
    Node* next_node = node->iNext;

    if ( node == iFirst )
        {
        // node to be deleted is the first one set the new head
        iFirst = next_node;
        }
    else
        {
        // left intentionally empty
        }
    if ( node == iLast )
        {
        // node to be deleted is the last one set new tail
        iLast = node->iPrev;
        }
    else
        {
        // left unintentionally empty
        }

    delete node;    // delete the current node
       
    --iNumOfElems;  // allways decrement element count

    if ( empty() )
        {
        // we are now empty 
        iFirst = NULL;
        iLast = NULL;
        }
    else
        {
        // left intentionally empty
        }

    // return the position of the next element 
    // (if any as next_node can be NULL)
    // in case of NULL end() is returned
    return ( (NULL != next_node) 
        ? list_iterator<Node, T>( next_node ) : end() );
    }


