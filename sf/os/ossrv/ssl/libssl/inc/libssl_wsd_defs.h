/*
Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation 
  and/or other materials provided with the distribution.
* Neither the name of Nokia Corporation nor the names of its contributors 
  may be used to endorse or promote products derived from this software 
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Description:
*/

 
#ifndef _LIBSSL_WSD_DEFS_H_
#define _LIBSSL_WSD_DEFS_H_

#if (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#ifdef EMULATOR
#undef EMULATOR
#endif
#ifdef GET_WSD_VAR_NAME
#undef GET_WSD_VAR_NAME
#endif

#define EMULATOR (defined(SYMBIAN) && (defined(__WINSCW__) || defined(__WINS__)))
#define GET_WSD_VAR_NAME(var,filename,prefix) 	_##prefix##_##filename##_##var

#define RETURN_WSD_VAR(var,filename,prefix) (Libssl_ImpurePtr()->GET_WSD_VAR_NAME(var,filename,prefix))

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
datatype GET_WSD_VAR_NAME(var,filename,varprefix);

#define VARIABLE_DECL_ARRAY(name,varprefix,filename,datatype,size) \
datatype GET_WSD_VAR_NAME(name,filename,varprefix)[size];


#define GET_STATIC_VAR_FROM_TLS(var,filename,type) 		GET_WSD_VAR_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_VAR_FROM_TLS(var,filename,type) 	GET_WSD_VAR_FROM_TLS(var,filename,type,g)

#define GET_STATIC_ARRAY_FROM_TLS(var,filename,type) 	GET_WSD_ARRAY_FROM_TLS(var,filename,type,s)

#define GET_GLOBAL_ARRAY_FROM_TLS(var,filename,type) 	GET_WSD_ARRAY_FROM_TLS(var,filename,type,g)



#endif
#endif		//_LIBSSL_WSD_DEFS_H_
