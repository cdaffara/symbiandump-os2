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
*/

#ifndef	_DST_DLL_MODULE_
#define _DST_DLL_MODULE_

class CDLLModule
{
public:
	CDLLModule() : m_hDLL(NULL)
	{
	}
	virtual ~CDLLModule()	//destructor, free the library
	{
		if (m_hDLL)
			::FreeLibrary(m_hDLL);
	}
	//////////////////////////////////////////////////////////////////
	// See if dll been loaded, returning true dose not mean that all
	// functions of the dll is valid.
	bool	IsLoaded(void) 
	{
		return m_hDLL != NULL;
	}

	/////////////////////////////////////////////////////////
	//	pure virtual, must implemented in derived class
	//	used macros to generate the implementation
	virtual bool	Init(LPCTSTR szDll)=0;

protected:
	HMODULE m_hDLL;
};

//////////////////////////////////////////////////////////////////////
// macros to implement the Init function
#define DECLARE_DLL_FUNCTION(ret, cc, func, params) \
	ret	(cc* func)params;

#define BEGIN_DLL_INIT() \
	bool Init(LPCTSTR szDll) \
	{ \
		if (m_hDLL) \
			::FreeLibrary(m_hDLL); \
		m_hDLL = ::LoadLibrary(szDll); \
		bool	bOk = true;

#define INIT_DLL_FUNCTION(ret, cc, func, params, origin) \
		if (m_hDLL) \
		{ \
			func = (ret (cc* )params)GetProcAddress(m_hDLL, origin); \
		} \
		else \
			func = NULL; \
		if (!func) \
			bOk = false;

#define END_DLL_INIT() \
		return bOk; \
	} 

#endif