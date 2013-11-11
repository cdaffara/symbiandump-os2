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
* Description:  
*
*/


inline CWriterPluginInterface::CWriterPluginInterface()
    : iOrder( KWriterPluginNotIndexed ) 
    {
        
    }

// -----------------------------------------------------------------------------
// CWriterPluginInterface::~CWriterPluginInterface()
// Destructor.
// -----------------------------------------------------------------------------
//
inline CWriterPluginInterface::~CWriterPluginInterface()
    {
    // We don't unload the plugin object here. The loader
    // has to do this for us. Without this kind of destruction idiom, 
    // the view framework can potentially unload an ECOM plugin dll whilst 
    // there are still child views (Created by the plugin) that are registered 
    // with the view framework. If this occurs, the plugin code segment isn't 
    // loaded anymore and so there is no way to run the subsequent destructor 
    // code => exception.
    
    // If in the NewL some memory is reserved for member data, it must be
    // released here. This interface does not have any instance variables so
    // no need to delete anything.

    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CWriterPluginInterface* CWriterPluginInterface::NewL(const TUid aImplementationUid, TAny* aInitParams)
    {
    // Define options, how the default resolver will find appropriate
    // implementation.
    return REINTERPRET_CAST(CWriterPluginInterface*, 
                            REComSession::CreateImplementationL(aImplementationUid,
                                                                _FOFF( CWriterPluginInterface, iDtor_ID_Key ),
                                                                aInitParams)); 
    }

inline void CWriterPluginInterface::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    REComSession::ListImplementationsL(KWriterPluginInterfaceUid, aImplInfoArray);
    }

inline void CWriterPluginInterface::SetOrder( TInt aOrder )
    {
    iOrder = aOrder;
    }

// end of file
