// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Interface class describing class that may contains user
// data added to node
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGUSERDATA_H
#define XMLENGUSERDATA_H

#include <e32def.h>

/**
Defines an interface so that user data can be stored in the DOM tree.
Applications that wish to store user data in the DOM tree must wrap the user
data in a class that implements this interface.

There are two common patterns for implementors of this interface:

1)  Instance-based implementation
The implementing class stores the user data.  The object can be duplicated with
CloneL() to form a new object that duplicates the user data.  When Destroy() is
called, the user data attached to the object is destroyed, but other duplicates
may continue to hold the data.

2)  Reference-counted implementation
The implementing class points to the user data.  The object can be duplicated,
but the user data is not duplicated.  When Destroy() is called, the object is
destroyed, but the user data is not destroyed unless this is the last object
referring to this data.
*/
class MXmlEngUserData {
public:
    /**
    Free memory that is allocated and do other case specific cleanup.  Whether
	this frees the user data depends on the implementation.  In a
	reference-counted implementation, the user data may only be destroyed
	when Destroy() is called on the last object referring to that data.
    */
    virtual void Destroy() = 0;
    
    /**
    Make a copy of the the object.  Whether this duplicates the data or
	creates a new object that points to the data is determined by the
	implementor of this class.
    
    @return Pointer to a copy of this object.
	@leave - One of the system-wide error codes
    */
    virtual MXmlEngUserData* CloneL() = 0;

	/**
	Gets the id of the object. It is up to the user data provider determine
	what the result is.  Such a "user data identification" may be useful if
	several types of MXmlEngUserData objects are used.
    
    @return Pointer that somehow identifies the type of MXmlEngUserData (NULL by default)
    */
    virtual void* ID() {return NULL;}
};


#endif /* XMLENGUSERDATA_H*/

