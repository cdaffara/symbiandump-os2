// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
   @file
   @internalComponent
*/


template <typename T>
inline RPriMsgQueue<T>::TFrontQueueElement::TFrontQueueElement(
	const TAny* apInfo)
	:
	ipInfo(apInfo)
	{
	iLink.iPriority = 0;
	}

template <typename T>
inline RPriMsgQueue<T>::TFrontQueueElement::TFrontQueueElement(
	const TAny* apInfo, TInt aPriority)
	:
	ipInfo(apInfo)
	{
	iLink.iPriority = aPriority;
	}

template <typename T>
inline TInt RPriMsgQueue<T>::CreateLocal(TInt aSize, TOwnerType aOwner)
    {
	iFrontQueue.SetOffset(_FOFF(TFrontQueueElement, iLink));
	return iBackQueue.CreateLocal(aSize, sizeof(T), aOwner);
	}

template <typename T>
inline TInt RPriMsgQueue<T>::Handle() const
	{
	return iBackQueue.Handle();
	}

template <typename T>
inline TInt RPriMsgQueue<T>::Send(const T& aMsg)
    {
	ASSERT(aMsg);
	return iBackQueue.Send(&aMsg, sizeof(T));
	}

template <typename T>
inline TInt RPriMsgQueue<T>::Receive(T& aMsg)
    {
	TInt err = DrainBackQueue();
	if (KErrNone == err)
		{
		if (iFrontQueue.IsEmpty())
			{
			return KErrUnderflow;
			}

		TFrontQueueElement* pElement = iFrontQueue.First();
		__ASSERT_DEBUG(pElement != 0,
					   User::Panic(RPriMsgQueuePanicCategory, 1));
		pElement->iLink.Deque();
		aMsg = reinterpret_cast<T>(const_cast<TAny*>(pElement->ipInfo));
		delete pElement;
		}

	return err;

	}

template <typename T>
inline void RPriMsgQueue<T>::Close()
	{

	// Back queue should be empty by now, but just in case...

	// Here we expect a pointer type!...
	T pElement;
	while (iBackQueue.Receive(&pElement, sizeof(T)) == KErrNone)
		{
		delete pElement;
		pElement = 0;
		}
	iBackQueue.Close();

	// Front queue should be empty by now, but just in case...
	TFrontQueueElement* pFQElement = 0;
	while (!iFrontQueue.IsEmpty())
		{
		pFQElement = iFrontQueue.First();
		__ASSERT_DEBUG(pFQElement != 0,
					   User::Panic(RPriMsgQueuePanicCategory, 1));
		pFQElement->iLink.Deque();
		pElement = reinterpret_cast<T>(const_cast<TAny*>(pFQElement->ipInfo));
		delete pElement;
		delete pFQElement;
		}

	}

template <typename T>
inline void RPriMsgQueue<T>::NotifyDataAvailable(TRequestStatus& aStatus)
	{
	iBackQueue.NotifyDataAvailable(aStatus);
	}

template <typename T>
inline void RPriMsgQueue<T>::CancelDataAvailable()
	{
	iBackQueue.CancelDataAvailable();
	}

template <typename T>
TInt RPriMsgQueue<T>::DrainBackQueue()
	{

	// Here we expect a pointer type!...
	T pElement;
	while (iBackQueue.Receive(&pElement, sizeof(T)) == KErrNone)
		{
		TFrontQueueElement* pElem =
			new TFrontQueueElement(pElement, pElement->Priority());

		// This double attempt at allocation is done in order to allow OOM
		// tests to pass, as there is no way to report an error to the client
		// if we lose a RegisterCallbacks message in an OOM situation
		if (!pElem)
			{
			pElem = new TFrontQueueElement(pElement, pElement->Priority());
			}

		if (!pElem)
			{
			// This command will get lost, but at least it won't be leaked
			delete pElement;
			return KErrNoMemory;
			}
		iFrontQueue.Add(*pElem);
		}

	return KErrNone;

	}
