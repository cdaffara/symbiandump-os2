/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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





#ifndef SERIALPACKETSIZE_H_D7CC307B_845C_4f9e_8079_C9C2C37A1B4D
#define SERIALPACKETSIZE_H_D7CC307B_845C_4f9e_8079_C9C2C37A1B4D

// The reference serial connection (with serial cable) has a
// packet size of 4096 with an 8 bit header so we send 4088
// bytes at a time.
#define KMaxPacketSize								4088

// The USB implementation has a packet size of 64k
#define KMaxUSBPacketSize							0xFFFF

// The Bluetooth implementation is implemented by virtual serial
// port on the PC side but is socket based on the device side.
// As far as the device goes there is no packet limit and messages
// are not broken up in packet chunks.
// Change the value from 1024 * 16 to 1024 * 4, testing proved 1024 * 16
// is too large to communicate.
#define KMaxBluetoothPacketSize					( 1024 * 4 )

// The TCPIP transport is connection oriented and does not
// need to packeted for transport at the application level.
// However we do package it anyway so that the data can be 
// collected in pieces and written to a file rather than a single
// memory buffer.
#define KMaxTCPIPPacketSize						( 1024 * 32 )

#endif // defined SERIALPACKETSIZE_H_D7CC307B_845C_4f9e_8079_C9C2C37A1B4D
