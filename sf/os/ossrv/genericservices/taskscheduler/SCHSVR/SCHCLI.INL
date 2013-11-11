// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SCHCLIENT.INL
// inline functions for CClientProxy
// 
//

inline TInt CClientProxy::Offset()
	{return (_FOFF(CClientProxy, iPriLink));}

inline void CClientProxy::Remove()
	{iPriLink.Deque();}

inline void CClientProxy::DecUsers()
	{iUsers--;}

inline void CClientProxy::IncUsers()
	{iUsers++;}

inline TInt CClientProxy::Users() const
	{return iUsers;}

inline void CClientProxy::ReadyToExecute()
	{iReadyToExecute=ETrue;}

inline TBool CClientProxy::IsReadyToExecute() const
	{return iReadyToExecute;}

inline TInt CClientProxy::Priority() const
	{return iPriLink.iPriority;}
