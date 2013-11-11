/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 


#ifndef __LIBOIL_WSD_MACROS_H__
#define __LIBOIL_WSD_MACROS_H__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#if EMULATOR
#undef EMULATOR
#endif
#ifdef GET_OIL_WSD_VAR_NAME
#undef GET_OIL_WSD_VAR_NAME
#endif

#define EMULATOR (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#define GET_OIL_WSD_VAR_NAME(var,filename,prefix)     _##prefix##_##filename##_##var

#define RETURN_WSD_VAR(var,filename,prefix) (libOIL_ImpurePtr()->GET_OIL_WSD_VAR_NAME(var,filename,prefix))

#define GET_WSD_VAR_FROM_TLS(var,filename,type,prefix)\
type *_##prefix##_##filename##_##var()\
{\
    return (&RETURN_WSD_VAR(var,filename,prefix));\
}

#define GET_WSD_ARRAY_FROM_TLS(var,filename,type,prefix)\
type *_##prefix##_##filename##_##var()\
{\
    return (RETURN_WSD_VAR(var,filename,prefix));\
}

#define VARIABLE_DECL(var,varprefix,filename,datatype)\
datatype GET_OIL_WSD_VAR_NAME(var,filename,varprefix);

#define VARIABLE_DECL_ARRAY(var,prefix,filename,datatype,size) \
datatype GET_OIL_WSD_VAR_NAME(var,filename,prefix)[size];


#define GET_STATIC_VAR_FROM_TLS(var,filename,type)      GET_WSD_VAR_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_VAR_FROM_TLS(var,filename,type)  GET_WSD_VAR_FROM_TLS(var,filename,type,g)

#define GET_STATIC_ARRAY_FROM_TLS(var,filename,type)    GET_WSD_ARRAY_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_ARRAY_FROM_TLS(var,filename,type)    GET_WSD_ARRAY_FROM_TLS(var,filename,type,g)

#endif
#endif      //__LIBOIL_WSD_MACROS_H__
