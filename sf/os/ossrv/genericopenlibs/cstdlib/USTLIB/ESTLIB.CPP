// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "POSIXIF.H"

// Category for the panic.
_LIT(KEstlibInit, "ESTLIB-INIT");

// Access for the outside world
extern "C" {

EXPORT_C void CloseSTDLIB()
	{
	struct _reent* p=(struct _reent*)Dll::Tls();
	if (p==0)
		return;
	_reclaim_reent(p);	// Reclaiming calls the atexit processing and generally tries to tidy up
	User::Free(p);		// then we give back the struct _reent itself
	Dll::SetTls(0);		// ... so we don't free it again when the DLL exits
	}

/**
Allocates memory for the library globals struct and returns a pointer to it. If the
memory has been allocated previously then it simply returns a pointer to the memory.
Panics if any error/failure occurs.

@return On Success, a pointer to the memory containing the library globals.
*/
EXPORT_C struct _reent *ImpurePtr(void)
	{
	struct _reent* p=(struct _reent*)Dll::Tls();
	if (p)
		return p; // Memory is already allocated for the library globals.

	// First use, so construct the default struct _reent and the associated SystemInterface
	p=(struct _reent*)User::Alloc(sizeof(struct _reent));
	if(p ==0)
		{
		User::Panic(KEstlibInit,KErrNoMemory);
		}

	TInt err= Dll::SetTls(p);

	if (err != KErrNone)
		{
		delete p;
		p = 0;
		Dll::FreeTls();
		User::Panic(KEstlibInit, err);
		}

	void* sysIf=0;

	CProcessSystemInterface* pSysIf=new CProcessSystemInterface;
	if (pSysIf && pSysIf->Connect()==KErrNone)
		{
		sysIf=pSysIf;
		}
	else
		{
		delete pSysIf;
		pSysIf = 0;

		CTrapCleanup *cleanup = NULL;
		if(User::TrapHandler() == NULL)
			{
			cleanup = CTrapCleanup::New();
			if(cleanup != NULL)
		        {
				//use static_cast to perform the valid conversion from CLocalSystemInterface to base class MSystemInterface
				TRAP(err, sysIf = static_cast<MSystemInterface*> (CLocalSystemInterface::NewL()));
		        }
		    else
		    	{
				err = KErrNoMemory;
				}
	    	delete cleanup;
			}
		else
			{
			//use static_cast to perform the valid conversion from CLocalSystemInterface to base class MSystemInterface
			TRAP(err, sysIf = static_cast<MSystemInterface*> (CLocalSystemInterface::NewL()));
			}
		}

	if (err != KErrNone)
		{
	    delete p;
	    p = 0;
		Dll::FreeTls();
		User::Panic(KEstlibInit, err);
		}

	Mem::FillZ(p,sizeof(struct _reent));
	_init_reent(p,sysIf);

	return p;
	}

/**
This is a panic free version of ImpurePtr. It allocates memory for the library globals
struct and returns a pointer to it. If the memory has been allocated previously then it
simply returns a pointer to the memory.
If there is not enough memory available to set up the library globals or other error occurs,
a NULL pointer will be returned.

@return On Success, a pointer to the memory containing the library globals.
		On Failure, a NULL pointer.
*/
EXPORT_C struct _reent * ImpurePtr2(void)
	{
	struct _reent* p = (struct _reent*)Dll::Tls();
	if (p)
		return p; // Memory is already allocated for the library globals.

	// First use, so construct the default struct _reent and the associated SystemInterface
	p =(struct _reent*)User::Alloc(sizeof(struct _reent));
	if (p== 0)
		return p ; // NULL

	if (Dll::SetTls(p))
		{
		delete p;
		p = 0;
		Dll::FreeTls();
		return p; // NULL
		}

	void* sysIf=0;
	CProcessSystemInterface* pSysIf=new CProcessSystemInterface;
	if (pSysIf && pSysIf->Connect()==KErrNone)
		{
		sysIf=pSysIf;
		}
	else
		{
		TInt err=KErrNone;
		delete pSysIf;
		pSysIf = 0;

		CTrapCleanup *cleanup = NULL;
		if(User::TrapHandler() == NULL)
			{
			cleanup = CTrapCleanup::New();
			if(cleanup != NULL)
		        {
				//use static_cast to perform the valid conversion from CLocalSystemInterface to base class MSystemInterface
				TRAP(err, sysIf = static_cast<MSystemInterface*> (CLocalSystemInterface::NewL()));
		        }
	    	delete cleanup;
			}
		else
			{
			//use static_cast to perform the valid conversion from CLocalSystemInterface to base class MSystemInterface
			TRAP(err, sysIf = static_cast<MSystemInterface*> (CLocalSystemInterface::NewL()));
			}
		}

	if (!sysIf)
		{
		delete p;
		p = 0;
		Dll::FreeTls();
		return p; // NULL
		}

	Mem::FillZ(p,sizeof(struct _reent));
	_init_reent(p,sysIf);

	return p; // Library globals have been set up correctly.
	}


EXPORT_C int *__errno(void)
	{
	return &(ImpurePtr()->_errno);
	}

EXPORT_C void _exit (int status) _ATTRIBUTE((noreturn))
	{
	struct _reent* p=(struct _reent*)Dll::Tls();
	if (p)
		{
		MSystemInterface& sysIf=Interface(p);
		sysIf.TerminateProcess(status);
		}
	RProcess().Terminate(status);	// just in case
	}

} // extern "C"

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
//
// DLL entry point
//
	{

	return KErrNone;
	}
#endif

