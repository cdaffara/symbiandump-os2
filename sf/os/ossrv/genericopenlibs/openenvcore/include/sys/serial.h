/*-
 * Copyright (c) 2004 Poul-Henning Kamp
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file contains definitions which pertain to serial ports as such,
 * (both async and sync), but which do not necessarily have anything to
 * do with tty processing.
 *
 * $FreeBSD: src/sys/sys/serial.h,v 1.2 2004/06/25 10:56:43 phk Exp $
 */

#ifndef _SYS_SERIAL_H_
#define	_SYS_SERIAL_H_

#ifndef __SYMBIAN32__

/*
 * Indentification of modem control signals.  These definitions match
 * the TIOCMGET definitions in <sys/ttycom.h> shifted a bit down, and
 * that identity is enforced with CTASSERT at the bottom of kern/tty.c
 * Both the modem bits and delta bits must fit in 16 bit.
 */
#define		SER_DTR	0x0001		/* data terminal ready */
#define		SER_RTS	0x0002		/* request to send */
#define		SER_STX	0x0004		/* secondary transmit */
#define		SER_SRX	0x0008		/* secondary receive */
#define		SER_CTS	0x0010		/* clear to send */
#define		SER_DCD	0x0020		/* data carrier detect */
#define		SER_RI 	0x0040		/* ring indicate */
#define		SER_DSR	0x0080		/* data set ready */

/* Delta bits, used to indicate which signals should/was affected */
#define		SER_DELTA(x)	((x) << 8)

#define		SER_DDTR SER_DELTA(SER_DTR)
#define		SER_DRTS SER_DELTA(SER_RTS)
#define		SER_DSTX SER_DELTA(SER_STX)
#define		SER_DSRX SER_DELTA(SER_SRX)
#define		SER_DCTS SER_DELTA(SER_CTS)
#define		SER_DDCD SER_DELTA(SER_DCD)
#define		SER_DRI  SER_DELTA(SER_RI)
#define		SER_DDSR SER_DELTA(SER_DSR)

#else
/**
notifications
*/
#define	KNotifyBreakInt				0x01
#define	KNotifyCD					0x02
#define	KNotifyCTS					0x04
#define	KNotifyDataAvailable		0x08
#define	KNotifyDSR					0x10
#define	KNotifyFramingError			0x20
#define	KNotifyOutputEmpty			0x40
#define	KNotifyOverrunError			0x80
#define	KNotifyParityError			0x100
#define	KNotifyRI					0x200

/**
enums, #defines and struct for serial port settings
*/
enum DataBits {DBits5,DBits6,DBits7,DBits8};

enum StopBits {Stop1,Stop2};

enum Parity {ParityNone,ParityEven,ParityOdd,ParityMark,ParitySpace};

enum Bps
	{
	Bps50,Bps75,Bps110,Bps134,Bps150,Bps300,Bps600,Bps1200,
	Bps1800,Bps2000,Bps2400,Bps3600,Bps4800,Bps7200,Bps9600,
	Bps19200,Bps38400,Bps57600,Bps115200,Bps230400,Bps460800,
	Bps576000,Bps1152000,Bps4000000,
	BpsSpecial=0x80000000
	};

enum Fifo {FifoEnable,FifoDisable};

enum Sir {SIREnable,SIRDisable};
//
#define ConfigMaxTerminators 4
//
#define ConfigObeyXoff 0x01
#define ConfigSendXoff 0x02
#define ConfigObeyCTS 0x04
#define ConfigFailCTS 0x08
#define ConfigObeyDSR 0x10
#define ConfigFailDSR 0x20
#define ConfigObeyDCD 0x40
#define ConfigFailDCD 0x80
#define ConfigFreeRTS 0x100
#define ConfigFreeDTR 0x200
#define ConfigWriteBufferedComplete 0x80000000
//
#define ConfigParityErrorFail 0
#define ConfigParityErrorIgnore 0x01
#define ConfigParityErrorReplaceChar 0x02
#define ConfigXonXoffDebug 0x80000000
//
#define SignalCTS 0x01
#define SignalDSR 0x02
#define SignalDCD 0x04
#define SignalRNG 0x08
#define SignalRTS 0x10
#define SignalDTR 0x20

#define ConfigSIRPulseWidthMaximum 0x01
#define ConfigSIRPulseWidthMinimum 0x02
//

typedef struct
	{
	enum Bps iRate;
	enum DataBits iDataBits;
	enum StopBits iStopBits;
	enum Parity iParity;
	unsigned int iHandshake;
	unsigned int iParityError;
	unsigned int iFifo;
	int iSpecialRate;
	int iTerminatorCount;
	char iTerminator[ConfigMaxTerminators];
	char iXonChar;
	char iXoffChar;
	char iParityErrorChar;
	enum Sir iSIREnable;
	unsigned int iSIRSettings;
	} SerialConfig;

#endif //__SYMBIAN32__

#endif /* !_SYS_SERIAL_H_ */
