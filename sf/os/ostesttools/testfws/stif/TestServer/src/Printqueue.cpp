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
* Description: This module contains implementation of CPrintQueue 
* class and TPrintQueueItem class member functions.
*
*/

// INCLUDE FILES
#include "PrintQueue.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================


// Initialise the queue
const TInt TPrintQueueItem::iOffset = _FOFF( TPrintQueueItem, iSlink );


/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: NewL

    Description: Constructs a print queue.
      
    Parameters: None

    Return Values: CPrintQueue*                     Pointer to new queue

    Errors/Exceptions: Leaves if memory allocation fails.

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CPrintQueue* CPrintQueue::NewL()
    {

    CPrintQueue* self = new( ELeave ) CPrintQueue;
    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: CPrintQueue

    Description: Constructor.
    Initialises the queue and queue iterator.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CPrintQueue::CPrintQueue():
     iQueue( TPrintQueueItem::iOffset ),
     iQueueIterator( iQueue )
    {

    iQueueLen = 0;

    }



/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: ~CPrintQueue

    Description: Destructor.

    Deletes all items in queue. Function requires that it is called from
    the heap to where the items are constructed.
       
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
CPrintQueue::~CPrintQueue()
    {
    
    TPrintQueueItem *item = NULL;

    // Delete items in queue
    while(( item = Pop() ) != NULL )
        {
        delete item;      // TPrintQueueItem destructor does't free the memory
        }

    }


/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: Push

    Description:

    Push item to queue. If queue is full, does not add item to queue.

    Parameters: TPrintQueueItem& anItem       :in:  Item to push to queue

    Return Values: TInt                             KErrNoMemory if queue is full
                                                    KErrNone     otherwise

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CPrintQueue::Push( TPrintQueueItem& anItem )
    {

    if( iQueueLen < KQueueMaxLen  )
        {
        iQueue.AddLast( anItem  );
        iQueueLen++;

        // Added successfully
        return KErrNone;
        }
    else
        {    
        // Do not add to queue
        return KErrNoMemory;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: Pop

    Description: Removes first item from queue and returns that.
    if queue is empty, then return NULL.

    Parameters: None

    Return Values: TPrintQueueItem*                 Queue item or NULL.

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TPrintQueueItem* CPrintQueue::Pop()
    {

    TPrintQueueItem* firstItem = NULL;
    
    if( !iQueue.IsEmpty()  )
        {
        firstItem = iQueue.First();
        iQueue.Remove( *firstItem  );
        iQueueLen--;
        }
    
    return( firstItem  );

    }

/*
-------------------------------------------------------------------------------

    Class: CPrintQueue

    Method: Count

    Description: Returns the queue size.
    
    Parameters: None

    Return Values: TInt                             Count

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TInt CPrintQueue::Count()
    {
    
    return( iQueueLen  );

    }

/*
-------------------------------------------------------------------------------

    Class: TPrintQueueItem

    Method: TPrintQueueItem

    Description: Constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters  const TInt aPriority          :in:  Priority
                const TStifInfoName& aText1       :in:  Description field
                const TName& aText2           :in:  Text field

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
TPrintQueueItem::TPrintQueueItem( const TInt aPriority,
                                  const TStifInfoName& aText1,
                                  const TName& aText2 )
    {

    iPriority = aPriority;
    iData1 = aText1;
    iData2 = aText2;

    }

// End of File
