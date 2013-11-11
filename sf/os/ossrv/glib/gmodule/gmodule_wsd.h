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



#ifndef __GMODULE_WSD_H__
#define __GMODULE_WSD_H__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))

#include <_ansi.h>
#include <sys/cdefs.h>
#include <gmodule.h>

/* -------------------------- MACRO DEFINATIONS ----------------------*/
#ifndef EMULATOR
#define EMULATOR (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#endif /* EMULATOR */

#define VARIABLE_NAME(var,filename) s_##filename##_##var

#define FUNCTION_NAME(var,filename) _s_##filename##_##var

#define VARIABLE_DECL(var,filename,datatype)\
datatype VARIABLE_NAME(var,filename);

#define RETURN_VAL(var,filename) (Gmodule_ImpurePtr()->VARIABLE_NAME(var,filename))

#define PLS(var,filename,type) \
type *_s_##filename##_##var() \
{ \
return &RETURN_VAL(var,filename);\
}
/* -------------------------- END MACRO DEFINATIONS ----------------------*/


struct global_struct
{
	VARIABLE_DECL(modules,gmodule,GModule *)
	VARIABLE_DECL(main_module,gmodule,GModule *)
	VARIABLE_DECL(module_error_private,gmodule,GStaticPrivate)
	VARIABLE_DECL(g_module_global_lock,gmodule,GStaticRecMutex)
	VARIABLE_DECL(module_debug_initialized,gmodule,gboolean)
	VARIABLE_DECL(module_debug_flags,gmodule,guint)
};

__BEGIN_DECLS
struct global_struct *Gmodule_ImpurePtr();
__END_DECLS
#endif 

#endif /* __GMODULE_WSD_H__ */
