/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "UniqueInstanceImpl.h"
#include "AssertFileAndLine.h"

using namespace UniqueInstance;

#ifdef _DEBUG
#define USE_INTEGRITY_CHECK 1
#else
#define USE_INTEGRITY_CHECK 0
#endif

// CUniqueInstanceRepositoryBase : user version in front of compiler firewall
void CUniqueInstanceRepositoryBase::ConstructL(TCompareFn* aCompare,
	TDeleteFn* aDelete, TCopyFnL* aCopyL,
	TInt aMaxLinks, TInt aObjectSize)
	{
	CRepositoryImpl* p = new(ELeave) CRepositoryImpl(aCompare,
		aDelete, aCopyL, aObjectSize);
	CleanupStack::PushL(p);
	p->ConstructL(aMaxLinks);
	CleanupStack::Pop(p);
	delete iImpl;
	iImpl = p;
	}

CUniqueInstanceRepositoryBase::~CUniqueInstanceRepositoryBase()
	{
	delete iImpl;
	}

void CUniqueInstanceRepositoryBase::Test() const
	{
	iImpl->Test();
	}

// RInstanceImpl : manager for a unique instance
RInstanceImpl::RInstanceImpl(CUniqueInstanceRepositoryBase& aRepository)
	: iRepository(aRepository.iImpl)
	{
	iPtr = iRepository->NullElement();
	}

RInstanceImpl::~RInstanceImpl()
	{
	ASSERT(iRepository->IsNull(iPtr));
	}

void RInstanceImpl::TakeL(void* a)
	{
	ASSERT(a);
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	UniqueInstance::SElement* r = iRepository->InsertOrIncL(a);
	iRepository->DeleteOrDec(iPtr);
	iPtr = r;
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	}

void RInstanceImpl::TakeCopyL(void* a)
	{
	ASSERT(a);
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	UniqueInstance::SElement* r = iRepository->IncOrCopyL(a);
	iRepository->DeleteOrDec(iPtr);
	iPtr = r;
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	}

void* RInstanceImpl::Peek() const
	{
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	return iPtr->iObject;
	}

void* RInstanceImpl::DropL()
	{
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	void* object = iRepository->DetatchOrCopyL(iPtr);
	iPtr = iRepository->NullElement();
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	return object;
	}

void RInstanceImpl::Close()
	{
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	iRepository->DeleteOrDec(iPtr);
	iPtr = iRepository->NullElement();
#if USE_INTEGRITY_CHECK
	iRepository->Test();
#endif
	}

void RInstanceImpl::CopyTo(RInstanceImpl& aOther) const
	{
#if USE_INTEGRITY_CHECK
	iRepository->Test();
	CRepositoryImpl* otherRep = aOther.iRepository;
	otherRep->Test();
#endif
	++(iPtr->iRefCount);
	SElement* e = iPtr;
	aOther.Close();
	aOther.iRepository = iRepository;
	aOther.iPtr = e;
#if USE_INTEGRITY_CHECK
	iRepository->Test();
	otherRep->Test();
#endif
	}

void RInstanceImpl::MoveTo(RInstanceImpl& aOther)
	{
#if USE_INTEGRITY_CHECK
	iRepository->Test();
	CRepositoryImpl* otherRep = aOther.iRepository;
	otherRep->Test();
#endif
	if (&aOther != this)
		{
		aOther.Close();
		aOther.iRepository = iRepository;
		aOther.iPtr = iPtr;
		iPtr = iRepository->NullElement();
		}
#if USE_INTEGRITY_CHECK
	iRepository->Test();
	otherRep->Test();
#endif
	}

// standard copy and delete functions
void* CUniqueInstanceRepositoryBase::DumbCopyL(void* aBuf, TInt aSize)
	{
	void* newBuf = User::AllocL(aSize);
	Mem::Copy(newBuf, aBuf, aSize);
	return newBuf;
	}

void CUniqueInstanceRepositoryBase::DumbDelete(void* aBuf)
	{
	User::Free(aBuf);
	}

// CUniqueInstanceRepository<TDes>
void* CUniqueInstanceRepository<TDes>::DesCopyL(void* a, TInt)
	{
	TDesC* des = reinterpret_cast<TDesC*>(a);
	return des->AllocL();
	}

TInt CUniqueInstanceRepository<TDes>::DesCompare(void* aL, void* aR)
	{
	TDesC* lh = reinterpret_cast<TDesC*>(aL);
	TDesC* rh = reinterpret_cast<TDesC*>(aR);
	return lh->Compare(*rh);
	}

void CUniqueInstanceRepository<TDes>::DesDelete(void* a)
	{
	operator delete(a);
	}

