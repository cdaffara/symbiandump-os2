/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Inline fuction used is OMXILAPITraceWrapper class
*
*/



#ifndef  	__OMXILCOMPONENTAPIPERFORMANCETRACE_INL
#define 	__OMXILCOMPONENTAPIPERFORMANCETRACE_INL

inline OMX_COMPONENTTYPE* OMXILAPITraceWrapper::ResolveCallsToComponent(OMX_IN  OMX_HANDLETYPE ahComponent)
        {
        OMX_COMPONENTTYPE* pComp = NULL;
        if(ahComponent)
            {
            COMPONENT_REF* pTRaceRef = reinterpret_cast<COMPONENT_REF*> ((reinterpret_cast<OMX_COMPONENTTYPE*>(ahComponent))->pComponentPrivate );
            pComp = pTRaceRef->pComponentIF;
            }
            return pComp;
        }

inline COMPONENT_REF* OMXILAPITraceWrapper::ResolveCallsFromComponent(OMX_IN  OMX_HANDLETYPE ahComponent)
    {
    ASSERT(ahComponent);
    return reinterpret_cast<COMPONENT_REF*> ((reinterpret_cast<OMX_COMPONENTTYPE*>(ahComponent))->pApplicationPrivate );
    }


#endif  	//__OMXILCOMPONENTAPIPERFORMANCETRACE_INL
