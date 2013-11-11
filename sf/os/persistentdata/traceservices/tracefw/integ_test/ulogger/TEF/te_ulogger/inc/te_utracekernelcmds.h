/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file te_utracekernelcmds.h
 @internalTechnology
*/
#if (!defined __TE_UTRACE_KERNEL_CMDS_H__)
#define __TE_UTRACE_KERNEL_CMDS_H__

#include "te_setfilterparameters.h"
#include "te_utracecmds.h"
#include <d32usbc.h>


class CUKernelTraces //: public CBase
{
public:
	virtual ~CUKernelTraces();
	CUKernelTraces();
	virtual bool DoTheKernelTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual bool DoTheKernelSetTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual bool DoTheKernelStaticTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual bool DoTheKernelPrintf(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual bool UHEAPDoTheKernelTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual bool UHEAPDoTheKernelStaticTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession);
	virtual int SizeOfKernelTrace(const int KernelTracerID);
	virtual int SizeOfKernelStaticTrace(const int KernelTracerID);

//private:

};

#endif
