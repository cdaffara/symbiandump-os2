// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file te_utracekernelcmds.cpp
 @internalTechnology
*/

#include "te_utracekernelcmds.h"


CUKernelTraces::~CUKernelTraces()
{
	/*Destructor*/
}

CUKernelTraces::CUKernelTraces()
{
	/*Constructor*/
}

bool CUKernelTraces::DoTheKernelTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)
{
	bool ret = 0;
	unsigned short ishort = 0x0809;
	switch(KernelTracerID)
	{
		case 0://traces for basic and negative testing
		ret = usbDeviceSession.EndpointZeroRequestError();
		break;
		case 1://traces for multiple testing
		ret = usbDeviceSession.GetStringDescriptorLangId(ishort);
		break;		
		default:
		break;
	}		
	return ret;
}

bool CUKernelTraces::DoTheKernelSetTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)
{
	bool r=0;
	TBuf16<500 / 2> rd_str;
	_LIT16(config, "Relatively Simple Configuration That Is Still Useful");
	rd_str=config;
	TBuf8<0x100> rd_str8;
	_LIT8(config8, "Relatively Simple Configuration That Is Still Useful");
	rd_str8=config8;
	TInt inum =0;
	switch(KernelTracerID)
	{
		case 0:
		r=usbDeviceSession.GetAlternateSetting(inum);
		break;
		case 1:
		r=usbDeviceSession.GetConfigurationStringDescriptor(rd_str);
		break;
		case 2:
		r=usbDeviceSession.SendEp0StatusPacket();
		break;
		case 3:
		r=usbDeviceSession.ReleaseDeviceControl();
		break;
		case 4:
		r=usbDeviceSession.EndpointZeroMaxPacketSizes();
		break;
		case 5:
		r=usbDeviceSession.SetEndpointZeroMaxPacketSize(0);
		break;
		case 6:
		r=usbDeviceSession.GetEndpointZeroMaxPacketSize();
		break;
		case 7:
		r=usbDeviceSession.GetDeviceDescriptor(rd_str8);
		break;
		case 8:
		r=usbDeviceSession.SetDeviceDescriptor(rd_str8);
		break;
		case 9:
		r=usbDeviceSession.GetDeviceDescriptorSize(inum);
		break;
		case 10:
		r=usbDeviceSession.GetConfigurationDescriptor(rd_str8);
		break;
		case 11:
		r=usbDeviceSession.GetConfigurationDescriptorSize(inum);
		break;
		case 12:
		r=usbDeviceSession.SetConfigurationDescriptor(rd_str8);
		break;
		case 13:
		r=usbDeviceSession.GetInterfaceDescriptor(0, rd_str8);
		break;
		case 14:
		r=usbDeviceSession.GetInterfaceDescriptorSize(0, inum);
		break;
		case 15:
		r=usbDeviceSession.SetInterfaceDescriptor(0, rd_str8);
		break;
		case 16:
		r=usbDeviceSession.GetEndpointDescriptor(0, 0, rd_str8);
		break;
		case 17:
		r=usbDeviceSession.GetEndpointDescriptorSize(0, 0, inum);
		break;
		default:
		break;
	}
	return r;	
}
bool CUKernelTraces::DoTheKernelStaticTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)

{
	bool r = 0;
	TBuf16<500 / 2> rd_str;
	_LIT16(config, "Relatively Simple Configuration That Is Still Useful");
	rd_str=config;
	TBuf8<0x100> rd_str8;
	_LIT8(config8, "Relatively Simple Configuration That Is Still Useful");
	rd_str8=config8;
	TInt inum = 0;
	switch(KernelTracerID)
	{
		case 0:
		r=usbDeviceSession.GetCSInterfaceDescriptorBlockSize(0, inum);
		break;
		case 1:
		r=usbDeviceSession.GetCSEndpointDescriptorBlock(0, 0, rd_str8);
		break;
		case 2:
		r=usbDeviceSession.GetCSEndpointDescriptorBlockSize(0, 0, inum);
		break;
		case 3:
		r=usbDeviceSession.SignalRemoteWakeup();
		break;
		case 4:
		r=usbDeviceSession.PowerUpUdc();
		break;
		case 5:
		r=usbDeviceSession.SetDeviceControl();
		break;
		case 6:
		r=usbDeviceSession.CurrentlyUsingHighSpeed();
		break;
		case 7:
		r=usbDeviceSession.ReleaseInterface(0);
		break;
		case 8:
		r=usbDeviceSession.SetCSInterfaceDescriptorBlock(0, rd_str8);
		break;
		case 9:
		r=usbDeviceSession.SetCSEndpointDescriptorBlock(0, 0, rd_str8);
		break;
		case 10:
		r=usbDeviceSession.SetStringDescriptorLangId(inum);
		break;
		case 11:
		r=usbDeviceSession.GetManufacturerStringDescriptor(rd_str);
		break;
		case 12:
		r=usbDeviceSession.SetManufacturerStringDescriptor(rd_str);
		break;
		case 13:
		r=usbDeviceSession.RemoveManufacturerStringDescriptor();
		break;
		case 14:
		r=usbDeviceSession.GetProductStringDescriptor(rd_str);
		break;
		case 15:
		r=usbDeviceSession.SetProductStringDescriptor(rd_str);
		break;
		case 16:
		r=usbDeviceSession.RemoveProductStringDescriptor();
		break;
		case 17:
		r=usbDeviceSession.GetSerialNumberStringDescriptor(rd_str);
		break;
		default:
		break;
	}
	return r;

}


bool CUKernelTraces::DoTheKernelPrintf(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)
{
	/*Note: this covers all the available printf's on the kernel side*/
	bool ret = 0;
	TBuf16<500 / 2> rd_str;
	_LIT16(config, "Relatively Simple Configuration That Is Still Useful");
	rd_str=config;
	switch(KernelTracerID)
	{
		case 0://traces for basic and negative testing
		ret = usbDeviceSession.SetStringDescriptor(0, rd_str);
		break;
		case 1://traces for multiple testing
		ret = usbDeviceSession.RemoveStringDescriptor(0);
		break;		
		default:
		break;
	}		
	return ret;
}


bool CUKernelTraces::UHEAPDoTheKernelTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	__UHEAP_MARK;	
	ret = DoTheKernelTrace(KernelTracerID, usbDeviceSession);
	__UHEAP_MARKEND;
#endif
	return ret;	
}

bool CUKernelTraces::UHEAPDoTheKernelStaticTrace(const int KernelTracerID, RDevUsbcClient& usbDeviceSession)
{
	bool ret = 0;
#ifndef __KERNEL_MODE__
	__UHEAP_MARK;	
	ret = DoTheKernelStaticTrace(KernelTracerID, usbDeviceSession);
	__UHEAP_MARKEND;
#endif
	return ret;	
}

int CUKernelTraces::SizeOfKernelTrace(const int KernelTracerID)
{
	CUTraces TestTracer;
	int TraceSize = TestTracer.SizeOfTrace(KernelTracerID);
	return TraceSize;
}


int CUKernelTraces::SizeOfKernelStaticTrace(const int KernelTracerID)
{
	CUTraces TestTracer;
	int TraceSize = TestTracer.SizeOfTrace(KernelTracerID);
	return TraceSize;
}
