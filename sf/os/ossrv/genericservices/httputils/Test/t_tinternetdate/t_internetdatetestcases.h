// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test cases for t_internetdate test harness
// 
//


#ifndef		__T_INTERNETDATETESTCASES_H__
#define		__T_INTERNETDATETESTCASES_H__

enum TInternetDateTestPanic
	{
	EInternetDateTestFailPanic			=0,
	EInternetDateCorruptTestFailPanic	=1
	};

struct TInternetDateTestCase
	{
	const TText8* iTextFormat;
	TInt iYear;	
	TMonth iMonth;
	TInt iDay;	// specify the day with a -1 offset as they start from 0
	TInt iHour;
	TInt iMin;
	TInt iSecond;
	};


#ifdef __PARSEINTERNETFORMTEST__

const TInt KNumberCorruptTestCases=15;
static const TText8* KCorruptTestCases[]=
	{
		_S8("Sun, 06 Nov 19940 08:49:37 GMT"),	
		_S8("Sun, 06 Nov 01994 08:49:37 GMT"),	
		_S8("Sun, 06 Nov 1994 008:49:37 GMT"),	
		_S8("Sun, 06 Nov 1994 080:49:37 GMT"),	
		_S8("Sun, 06 Nov 1994 010:49:37 GMT"),	
		_S8("Sun, 06 Nov 1994 08:049:37 GMT"),	
		_S8("Sun, 06 Nov 1994 08:010:37 GMT"),	
		_S8("Sun, 06 Nov 1994 08:490:37 GMT"),	
		_S8("Sun, 06 Nov 1994 08:090:37 GMT"),	
		_S8("Sun, 06 Nov 1994 08:49:037 GMT"),	
		_S8("Sun, 06 Nov 1994 08:49:370 GMT"),	
		_S8("Sun, 06 Nov 1994 08:49:030 GMT"),	

		_S8("Sun, 0612343 Nov 1994 08:49:37 GMT"),	
		_S8("Sun, 06 November 1994 08:49:37 GMT"),
		_S8("Sun, 0006 Nov 1994 08:49:37 GMT")	
	};
#endif

const TInt KNumberRfc1123GMTTzTestCases = 22;
const struct TInternetDateTestCase KRfc1123GMTTzTestCases[] = 
	{
		
		{_S8("Sun, 06 Nov 1994 08:49:37 GMT"), 1994,ENovember,5,8,49,37},
		{_S8("Mon, 21 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 20,12,0,0},
		{_S8("Tue, 22 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 21,12,0,0},
		{_S8("Wed, 23 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 22,12,0,0},
		{_S8("Thu, 24 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 23,12,0,0},
		{_S8("Fri, 25 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 24,12,0,0},
		{_S8("Sat, 26 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 25,12,0,0},
		{_S8("Sun, 27 Jan 2002 12:00:00 GMT"), 2002,EJanuary, 26,12,0,0},
		{_S8("Mon, 04 Feb 2002 12:00:00 GMT"), 2002,EFebruary, 3,12,0,0},
		{_S8("Mon, 04 Mar 2002 12:00:00 GMT"), 2002,EMarch, 3,12,0,0},
		{_S8("Tue, 02 Apr 2002 12:00:00 GMT"), 2002,EApril, 1,12,0,0},
		{_S8("Tue, 07 May 2002 12:00:00 GMT"), 2002,EMay, 6,12,0,0},
		{_S8("Wed, 05 Jun 2002 12:00:00 GMT"), 2002,EJune, 4,12,0,0},
		{_S8("Wed, 10 Jul 2002 12:00:00 GMT"), 2002,EJuly, 9,12,0,0},
		{_S8("Thu, 15 Aug 2002 12:00:00 GMT"), 2002,EAugust, 14,12,0,0},
		{_S8("Thu, 12 Sep 2002 12:00:00 GMT"), 2002,ESeptember, 11,12,0,0},
		{_S8("Fri, 11 Oct 2002 12:00:00 GMT"), 2002,EOctober, 10,12,0,0},
		{_S8("Fri, 18 Oct 2002 12:00:00 GMT"), 2002,EOctober, 17,12,0,0},
		{_S8("Sat, 23 Nov 2002 12:00:00 GMT"), 2002,ENovember, 22,12,0,0},
		{_S8("Sat, 30 Nov 2002 12:00:00 GMT"), 2002,ENovember, 29,12,0,0},
		{_S8("Sun, 01 Dec 2002 12:00:00 GMT"), 2002,EDecember, 0,12,0,0},
		{_S8("Sun, 29 Dec 2002 12:00:00 GMT"), 2002,EDecember, 28,12,0,0},
	};


const TInt KNumberINC090613TestCases = 2;
static const TBuf8<50> KINC090613TestCases[]=
	{
		_L8("Sun, 6 Nov 1994 08:49:37 GMT"),
		_L8("Sun, 06 Nov 1994 08:49:37 GMT")
	};

const TInt KNumberINC107382TestCases = 9;
static const TBuf8<50> KINC107382TestCases[]=
	{
		_L8(" Sun, 6 Nov 1994 08:49:37 GMT"),
		_L8("Sun, 06 Nov 1994 08:49:37 GMT "),
		_L8("   Sun, 06 Nov 1994 08:49:37 GMT   "),
		_L8(" Sunday, 06-Nov-94 08:49:37 GMT"),
		_L8("Sunday, 06-Nov-94 08:49:37 GMT "),
		_L8("   Sunday, 06-Nov-94 08:49:37 GMT   "),
		_L8(" Sun Nov 6 08:49:37 1994"),
		_L8("Sun Nov 6 08:49:37 1994 "),
		_L8("   Sun Nov 6 08:49:37 1994   "),
	};

const TInt KNumberRfc850GMTTzTestCases = 22;
const struct TInternetDateTestCase KRfc850GMTTzTestCases[] = 
	{
		
		{_S8("Sun, 06-Nov-94 08:49:37 GMT"), 1994,ENovember,5,8,49,37},
		{_S8("Mon, 21-Jan-02 12:00:00 GMT"), 2002,EJanuary, 20,12,0,0},
		{_S8("Tue, 22-Jan-02 12:00:00 GMT"), 2002,EJanuary, 21,12,0,0},
		{_S8("Wed, 23-Jan-02 12:00:00 GMT"), 2002,EJanuary, 22,12,0,0},
		{_S8("Thu, 24-Jan-02 12:00:00 GMT"), 2002,EJanuary, 23,12,0,0},
		{_S8("Fri, 25-Jan-02 12:00:00 GMT"), 2002,EJanuary, 24,12,0,0},
		{_S8("Sat, 26-Jan-02 12:00:00 GMT"), 2002,EJanuary, 25,12,0,0},
		{_S8("Sun, 27-Jan-02 12:00:00 GMT"), 2002,EJanuary, 26,12,0,0},
		{_S8("Mon, 04-Feb-02 12:00:00 GMT"), 2002,EFebruary, 3,12,0,0},
		{_S8("Mon, 04-Mar-02 12:00:00 GMT"), 2002,EMarch, 3,12,0,0},
		{_S8("Tue, 02-Apr-02 12:00:00 GMT"), 2002,EApril, 1,12,0,0},
		{_S8("Tue, 07-May-02 12:00:00 GMT"), 2002,EMay, 6,12,0,0},
		{_S8("Wed, 05-Jun-02 12:00:00 GMT"), 2002,EJune, 4,12,0,0},
		{_S8("Wed, 10-Jul-02 12:00:00 GMT"), 2002,EJuly, 9,12,0,0},
		{_S8("Thu, 15-Aug-02 12:00:00 GMT"), 2002,EAugust, 14,12,0,0},
		{_S8("Thu, 12-Sep-02 12:00:00 GMT"), 2002,ESeptember, 11,12,0,0},
		{_S8("Fri, 11-Oct-02 12:00:00 GMT"), 2002,EOctober, 10,12,0,0},
		{_S8("Fri, 18-Oct-02 12:00:00 GMT"), 2002,EOctober, 17,12,0,0},
		{_S8("Sat, 23-Nov-02 12:00:00 GMT"), 2002,ENovember, 22,12,0,0},
		{_S8("Sat, 30-Nov-02 12:00:00 GMT"), 2002,ENovember, 29,12,0,0},
		{_S8("Sun, 01-Dec-02 12:00:00 GMT"), 2002,EDecember, 0,12,0,0},
		{_S8("Sun, 29-Dec-02 12:00:00 GMT"), 2002,EDecember, 28,12,0,0},
	};


const TInt KNumberAscTimeTestCases = 22;
const struct TInternetDateTestCase KAscTimeTestCases[] = 
	{
		{_S8("Sun Nov  6 08:49:37 1994"), 1994,ENovember,5,8,49,37},
		{_S8("Mon Jan 21 12:00:00 2002"), 2002,EJanuary, 20,12,0,0},
		{_S8("Tue Jan 22 12:00:00 2002"), 2002,EJanuary, 21,12,0,0},
		{_S8("Wed Jan 23 12:00:00 2002"), 2002,EJanuary, 22,12,0,0},
		{_S8("Thu Jan 24 12:00:00 2002"), 2002,EJanuary, 23,12,0,0},
		{_S8("Fri Jan 25 12:00:00 2002"), 2002,EJanuary, 24,12,0,0},
		{_S8("Sat Jan 26 12:00:00 2002"), 2002,EJanuary, 25,12,0,0},
		{_S8("Sun Jan 27 12:00:00 2002"), 2002,EJanuary, 26,12,0,0},
		{_S8("Mon Feb  4 12:00:00 2002"), 2002,EFebruary, 3,12,0,0},
		{_S8("Mon Mar  4 12:00:00 2002"), 2002,EMarch, 3,12,0,0},
		{_S8("Tue Apr  2 12:00:00 2002"), 2002,EApril, 1,12,0,0},
		{_S8("Tue May  7 12:00:00 2002"), 2002,EMay, 6,12,0,0},
		{_S8("Wed Jun  5 12:00:00 2002"), 2002,EJune, 4,12,0,0},
		{_S8("Wed Jul 10 12:00:00 2002"), 2002,EJuly, 9,12,0,0},
		{_S8("Thu Aug 15 12:00:00 2002"), 2002,EAugust, 14,12,0,0},
		{_S8("Thu Sep 12 12:00:00 2002"), 2002,ESeptember, 11,12,0,0},
		{_S8("Fri Oct 11 12:00:00 2002"), 2002,EOctober, 10,12,0,0},
		{_S8("Fri Oct 18 12:00:00 2002"), 2002,EOctober, 17,12,0,0},
		{_S8("Sat Nov 23 12:00:00 2002"), 2002,ENovember, 22,12,0,0},
		{_S8("Sat Nov 30 12:00:00 2002"), 2002,ENovember, 29,12,0,0},
		{_S8("Sun Dec  1 12:00:00 2002"), 2002,EDecember, 0,12,0,0},
		{_S8("Sun Dec 29 12:00:00 2002"), 2002,EDecember, 28,12,0,0},
	};


const TInt KNumberRfc1123NonGMTTzTestCases = 45;
const struct TInternetDateTestCase KRfc1123NonGMTTzTestCases[] =
	{
	{_S8("Mon, 21 Jan 2002 00:00:00 EST"), 2002,EJanuary, 19,19,0,0}, //  EST -5
	{_S8("Mon, 21 Jan 2002 23:00:00 EST"), 2002,EJanuary, 20,18,0,0}, //  EST -5 	
	{_S8("Mon, 21 Jan 2002 00:00:00 EDT"), 2002,EJanuary, 19,20,0,0}, //  EDT -4
	{_S8("Mon, 21 Jan 2002 23:00:00 EDT"), 2002,EJanuary, 20,19,0,0}, //  EDT -4 

	{_S8("Mon, 21 Jan 2002 00:00:00 CST"), 2002,EJanuary, 19,18,0,0}, //  CST -6
	{_S8("Mon, 21 Jan 2002 23:00:00 CST"), 2002,EJanuary, 20,17,0,0}, //  CST -6 
	{_S8("Mon, 21 Jan 2002 00:00:00 CDT"), 2002,EJanuary, 19,19,0,0}, //  CDT -5
	{_S8("Mon, 21 Jan 2002 23:00:00 CDT"), 2002,EJanuary, 20,18,0,0}, //  CDT -5 

	{_S8("Mon, 21 Jan 2002 00:00:00 MST"), 2002,EJanuary, 19,17,0,0}, //  MST -7
	{_S8("Mon, 21 Jan 2002 23:00:00 MST"), 2002,EJanuary, 20,16,0,0}, //  MST -7 
	{_S8("Mon, 21 Jan 2002 00:00:00 MDT"), 2002,EJanuary, 19,18,0,0}, //  MDT -6
	{_S8("Mon, 21 Jan 2002 23:00:00 MDT"), 2002,EJanuary, 20,17,0,0}, //  MDT -6 
	
	{_S8("Mon, 21 Jan 2002 00:00:00 PST"), 2002,EJanuary, 19,16,0,0}, //  PST -8
	{_S8("Mon, 21 Jan 2002 23:00:00 PST"), 2002,EJanuary, 20,15,0,0}, //  PST -8 
	{_S8("Mon, 21 Jan 2002 00:00:00 PDT"), 2002,EJanuary, 19,17,0,0}, //  PDT -7
	{_S8("Mon, 21 Jan 2002 23:00:00 PDT"), 2002,EJanuary, 20,16,0,0}, //  PDT -7 

	{_S8("Mon, 21 Jan 2002 00:00:00 UT"), 2002,EJanuary, 20,0,0,0},   //  UT = GMT	
	{_S8("Mon, 21 Jan 2002 23:00:00 UT"), 2002,EJanuary, 20,23,0,0},  //  UT = GMT
	{_S8("Mon, 21 Jan 2002 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},  //  GMT	
	{_S8("Mon, 21 Jan 2002 23:00:00 GMT"), 2002,EJanuary, 20,23,0,0}, //  GMT
	
	// Military Time Zones (All these should be considered equivalent to GMT)
	{_S8("Mon, 21 Jan 2002 00:00:00 A"), 2002,EJanuary, 20,0,0,0}, //  A	
	{_S8("Mon, 21 Jan 2002 00:00:00 B"), 2002,EJanuary, 20,0,0,0}, //  B	
	{_S8("Mon, 21 Jan 2002 00:00:00 C"), 2002,EJanuary, 20,0,0,0}, //  C	
	{_S8("Mon, 21 Jan 2002 00:00:00 D"), 2002,EJanuary, 20,0,0,0}, //  D	
	{_S8("Mon, 21 Jan 2002 00:00:00 E"), 2002,EJanuary, 20,0,0,0}, //  E	
	{_S8("Mon, 21 Jan 2002 00:00:00 F"), 2002,EJanuary, 20,0,0,0}, //  F	
	{_S8("Mon, 21 Jan 2002 00:00:00 G"), 2002,EJanuary, 20,0,0,0}, //  G	
	{_S8("Mon, 21 Jan 2002 00:00:00 H"), 2002,EJanuary, 20,0,0,0}, //  H
	{_S8("Mon, 21 Jan 2002 00:00:00 I"), 2002,EJanuary, 20,0,0,0}, //  I	
	{_S8("Mon, 21 Jan 2002 00:00:00 K"), 2002,EJanuary, 20,0,0,0}, //  K
	{_S8("Mon, 21 Jan 2002 00:00:00 L"), 2002,EJanuary, 20,0,0,0}, //  L
	{_S8("Mon, 21 Jan 2002 00:00:00 M"), 2002,EJanuary, 20,0,0,0}, //  M
	{_S8("Mon, 21 Jan 2002 00:00:00 Z"), 2002,EJanuary, 20,0,0,0}, //  Z
	{_S8("Mon, 21 Jan 2002 00:00:00 N"), 2002,EJanuary, 20,0,0,0}, //  N
	{_S8("Mon, 21 Jan 2002 00:00:00 O"), 2002,EJanuary, 20,0,0,0}, //  O
	{_S8("Mon, 21 Jan 2002 00:00:00 P"), 2002,EJanuary, 20,0,0,0}, //  P
	{_S8("Mon, 21 Jan 2002 00:00:00 Q"), 2002,EJanuary, 20,0,0,0}, //  Q
	{_S8("Mon, 21 Jan 2002 00:00:00 R"), 2002,EJanuary, 20,0,0,0}, //  R
	{_S8("Mon, 21 Jan 2002 00:00:00 S"), 2002,EJanuary, 20,0,0,0}, //  S
	{_S8("Mon, 21 Jan 2002 00:00:00 T"), 2002,EJanuary, 20,0,0,0}, //  T
	{_S8("Mon, 21 Jan 2002 00:00:00 U"), 2002,EJanuary, 20,0,0,0}, //  U
	{_S8("Mon, 21 Jan 2002 00:00:00 V"), 2002,EJanuary, 20,0,0,0}, //  V
	{_S8("Mon, 21 Jan 2002 00:00:00 W"), 2002,EJanuary, 20,0,0,0}, //  W
	{_S8("Mon, 21 Jan 2002 00:00:00 X"), 2002,EJanuary, 20,0,0,0}, //  X
	{_S8("Mon, 21 Jan 2002 00:00:00 Y"), 2002,EJanuary, 20,0,0,0}  //  Z
	};

const TInt KNumberRfc850NonGMTTzTestCases = 45;
const struct TInternetDateTestCase KRfc850NonGMTTzTestCases[] =
	{
	{_S8("Mon, 21-Jan-02 00:00:00 EST"), 2002,EJanuary, 19,19,0,0}, //  EST -5
	{_S8("Mon, 21-Jan-02 23:00:00 EST"), 2002,EJanuary, 20,18,0,0}, //  EST -5 	
	{_S8("Mon, 21-Jan-02 00:00:00 EDT"), 2002,EJanuary, 19,20,0,0}, //  EDT -4
	{_S8("Mon, 21-Jan-02 23:00:00 EDT"), 2002,EJanuary, 20,19,0,0}, //  EDT -4 

	{_S8("Mon, 21-Jan-02 00:00:00 CST"), 2002,EJanuary, 19,18,0,0}, //  CST -6
	{_S8("Mon, 21-Jan-02 23:00:00 CST"), 2002,EJanuary, 20,17,0,0}, //  CST -6 
	{_S8("Mon, 21-Jan-02 00:00:00 CDT"), 2002,EJanuary, 19,19,0,0}, //  CDT -5
	{_S8("Mon, 21-Jan-02 23:00:00 CDT"), 2002,EJanuary, 20,18,0,0}, //  CDT -5 

	{_S8("Mon, 21-Jan-02 00:00:00 MST"), 2002,EJanuary, 19,17,0,0}, //  MST -7
	{_S8("Mon, 21-Jan-02 23:00:00 MST"), 2002,EJanuary, 20,16,0,0}, //  MST -7 
	{_S8("Mon, 21-Jan-02 00:00:00 MDT"), 2002,EJanuary, 19,18,0,0}, //  MDT -6
	{_S8("Mon, 21-Jan-02 23:00:00 MDT"), 2002,EJanuary, 20,17,0,0}, //  MDT -6 
	
	{_S8("Mon, 21-Jan-02 00:00:00 PST"), 2002,EJanuary, 19,16,0,0}, //  PST -8
	{_S8("Mon, 21-Jan-02 23:00:00 PST"), 2002,EJanuary, 20,15,0,0}, //  PST -8 
	{_S8("Mon, 21-Jan-02 00:00:00 PDT"), 2002,EJanuary, 19,17,0,0}, //  PDT -7
	{_S8("Mon, 21-Jan-02 23:00:00 PDT"), 2002,EJanuary, 20,16,0,0}, //  PDT -7 

	{_S8("Mon, 21-Jan-02 00:00:00 UT"), 2002,EJanuary, 20,0,0,0},   //  UT = GMT	
	{_S8("Mon, 21-Jan-02 23:00:00 UT"), 2002,EJanuary, 20,23,0,0},  //  UT = GMT
	{_S8("Mon, 21-Jan-02 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},  //  GMT	
	{_S8("Mon, 21-Jan-02 23:00:00 GMT"), 2002,EJanuary, 20,23,0,0}, //  GMT
	
	// Military Time Zones (All these should be considered equivalent to GMT)
	{_S8("Mon, 21-Jan-02 00:00:00 A"), 2002,EJanuary, 20,0,0,0}, //  A	
	{_S8("Mon, 21-Jan-02 00:00:00 B"), 2002,EJanuary, 20,0,0,0}, //  B	
	{_S8("Mon, 21-Jan-02 00:00:00 C"), 2002,EJanuary, 20,0,0,0}, //  C	
	{_S8("Mon, 21-Jan-02 00:00:00 D"), 2002,EJanuary, 20,0,0,0}, //  D	
	{_S8("Mon, 21-Jan-02 00:00:00 E"), 2002,EJanuary, 20,0,0,0}, //  E	
	{_S8("Mon, 21-Jan-02 00:00:00 F"), 2002,EJanuary, 20,0,0,0}, //  F	
	{_S8("Mon, 21-Jan-02 00:00:00 G"), 2002,EJanuary, 20,0,0,0}, //  G	
	{_S8("Mon, 21-Jan-02 00:00:00 H"), 2002,EJanuary, 20,0,0,0}, //  H
	{_S8("Mon, 21-Jan-02 00:00:00 I"), 2002,EJanuary, 20,0,0,0}, //  I	
	{_S8("Mon, 21-Jan-02 00:00:00 K"), 2002,EJanuary, 20,0,0,0}, //  K
	{_S8("Mon, 21-Jan-02 00:00:00 L"), 2002,EJanuary, 20,0,0,0}, //  L
	{_S8("Mon, 21-Jan-02 00:00:00 M"), 2002,EJanuary, 20,0,0,0}, //  M
	{_S8("Mon, 21-Jan-02 00:00:00 Z"), 2002,EJanuary, 20,0,0,0}, //  Z
	{_S8("Mon, 21-Jan-02 00:00:00 N"), 2002,EJanuary, 20,0,0,0}, //  N
	{_S8("Mon, 21-Jan-02 00:00:00 O"), 2002,EJanuary, 20,0,0,0}, //  O
	{_S8("Mon, 21-Jan-02 00:00:00 P"), 2002,EJanuary, 20,0,0,0}, //  P
	{_S8("Mon, 21-Jan-02 00:00:00 Q"), 2002,EJanuary, 20,0,0,0}, //  Q
	{_S8("Mon, 21-Jan-02 00:00:00 R"), 2002,EJanuary, 20,0,0,0}, //  R
	{_S8("Mon, 21-Jan-02 00:00:00 S"), 2002,EJanuary, 20,0,0,0}, //  S
	{_S8("Mon, 21-Jan-02 00:00:00 T"), 2002,EJanuary, 20,0,0,0}, //  T
	{_S8("Mon, 21-Jan-02 00:00:00 U"), 2002,EJanuary, 20,0,0,0}, //  U
	{_S8("Mon, 21-Jan-02 00:00:00 V"), 2002,EJanuary, 20,0,0,0}, //  V
	{_S8("Mon, 21-Jan-02 00:00:00 W"), 2002,EJanuary, 20,0,0,0}, //  W
	{_S8("Mon, 21-Jan-02 00:00:00 X"), 2002,EJanuary, 20,0,0,0}, //  X
	{_S8("Mon, 21-Jan-02 00:00:00 Y"), 2002,EJanuary, 20,0,0,0}  //  Z
	};

const TInt KNumberRfc1123MisformedTestCases = 8;
const struct TInternetDateTestCase KRfc1123MisformedTestCases[] =
	{
	// Test whitespace parsing first, even though these should be in a predefined format
	// Whitespace between 21 Jan 2002 MUST be 1 space or -
	{_S8("Mon, 21 Jan 2002 00:00:00 ZDT"), 2002,EJanuary, 20,0,0,0},	// Invalid Tz Should be 0 offset
	{_S8("   Mon, 21 Jan 2002 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon,    21 Jan 2002 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21 Jan 2002    00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21 Jan 2002 00:00:00    GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21 Jan 2002 00:00:00 GMT   "), 2002,EJanuary, 20,0,0,0},

	{_S8("Mon, 21 Jan 2002 00:00:00 J"), 2002,EJanuary, 20,0,0,0},		// Should ignore military time zone anyway
	{_S8("Mon, 21 Jan 2002 00:00:00 ZST"), 2002,EJanuary, 20,0,0,0} 	// Invalid Tz Should be 0 offset
	};


const TInt KNumberRfc850MisformedTestCases = 8;
const struct TInternetDateTestCase KRfc850MisformedTestCases[] =
	{
	// Test whitespace parsing first, even though these should be in a predefined format
	// Whitespace between 21 Jan 2002 MUST be 1 space or -
	{_S8("   Mon, 21-Jan-02 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon,    21-Jan-02 00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21-Jan-02    00:00:00 GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21-Jan-02 00:00:00    GMT"), 2002,EJanuary, 20,0,0,0},
	{_S8("Mon, 21-Jan-02 00:00:00 GMT   "), 2002,EJanuary, 20,0,0,0},

	{_S8("Mon, 21-Jan-02 00:00:00 J"), 2002,EJanuary, 20,0,0,0},		// Should ignore military time zone anyway
	{_S8("Mon, 21-Jan-02 00:00:00 ZST"), 2002,EJanuary, 20,0,0,0},	// Invalid Tz Should be 0 offset
	{_S8("Mon, 21-Jan-02 00:00:00 ZDT"), 2002,EJanuary, 20,0,0,0}	// Invalid Tz Should be 0 offset
	};

const TInt KNumberRfc1123TimeOffsetTzTestCases = 64;
const struct TInternetDateTestCase KRfc1123TimeOffsetTzTestCases[] =
	{
	{_S8("Mon, 21 Jan 2002 00:00:00 +2359"), 2002,EJanuary, 20,23,59,0},	//  +2359	
	{_S8("Mon, 21 Jan 2002 23:00:00 +2359"), 2002,EJanuary, 21,22,59,0},	//  +2359
	{_S8("Mon, 21 Jan 2002 00:00:00 +2400"), 2002,EJanuary, 20,0,0,0},		//  +2400  Max offset should be +2359
	{_S8("Mon, 21 Jan 2002 23:00:00 +2400"), 2002,EJanuary, 20,23,0,0},		//  +2400  Max offset should be +2359
	{_S8("Mon, 21 Jan 2002 00:00:00 +0060"), 2002,EJanuary, 20,0,0,0},		//  +0060  Max min offset is 59
	{_S8("Mon, 21 Jan 2002 23:00:00 +0060"), 2002,EJanuary, 20,23,0,0},		//  +0060  Max min offset is 59
	{_S8("Mon, 21 Jan 2002 00:00:00 +0000"), 2002,EJanuary, 20,0,0,0},		//  +0000	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0000"), 2002,EJanuary, 20,23,0,0},		//  -0000
	{_S8("Mon, 21 Jan 2002 00:00:00 +0100"), 2002,EJanuary, 20,1,0,0},		//  +0100	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0105"), 2002,EJanuary, 21,0,5,0},		//  +0105

	{_S8("Mon, 21 Jan 2002 00:00:00 +0200"), 2002,EJanuary, 20,2,0,0},		//  +0200	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0210"), 2002,EJanuary, 21,1,10,0},		//  +0210
	{_S8("Mon, 21 Jan 2002 00:00:00 +0300"), 2002,EJanuary, 20,3,0,0},		//  +0300	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0359"), 2002,EJanuary, 21,2,59,0},		//  +0359
	
	{_S8("Mon, 21 Jan 2002 00:00:00 +0400"), 2002,EJanuary, 20,4,0,0},		//  +0400	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0400"), 2002,EJanuary, 21,3,0,0},		//  +0400
	{_S8("Mon, 21 Jan 2002 00:00:00 +0500"), 2002,EJanuary, 20,5,0,0},		//  +0500	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0500"), 2002,EJanuary, 21,4,0,0},		//  +0500
	{_S8("Mon, 21 Jan 2002 00:00:00 +0600"), 2002,EJanuary, 20,6,0,0},		//  +0600	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0600"), 2002,EJanuary, 21,5,0,0},		//  +0600
	{_S8("Mon, 21 Jan 2002 00:00:00 +0700"), 2002,EJanuary, 20,7,0,0},		//  +0700	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0700"), 2002,EJanuary, 21,6,0,0},		//  +0700
	{_S8("Mon, 21 Jan 2002 00:00:00 +0800"), 2002,EJanuary, 20,8,0,0},		//  +0800	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0800"), 2002,EJanuary, 21,7,0,0},		//  +0800

	{_S8("Mon, 21 Jan 2002 00:00:00 +0900"), 2002,EJanuary, 20,9,0,0},		//  +0900	
	{_S8("Mon, 21 Jan 2002 23:00:00 +0900"), 2002,EJanuary, 21,8,0,0},		//  +0900
	{_S8("Mon, 21 Jan 2002 00:00:00 +1000"), 2002,EJanuary, 20,10,0,0},		//  +1000	
	{_S8("Mon, 21 Jan 2002 23:00:00 +1000"), 2002,EJanuary, 21,9,0,0},		//  +1000	
	{_S8("Mon, 21 Jan 2002 00:00:00 +1100"), 2002,EJanuary, 20,11,0,0},		//  +1100	
	{_S8("Mon, 21 Jan 2002 23:00:00 +1100"), 2002,EJanuary, 21,10,0,0},		//  +1100
	{_S8("Mon, 21 Jan 2002 00:00:00 +1200"), 2002,EJanuary, 20,12,0,0},		//  +1200	
	{_S8("Mon, 21 Jan 2002 23:00:00 +1200"), 2002,EJanuary, 21,11,0,0},		//  +1200
	// Now same tests but with negative offsets
	{_S8("Mon, 21 Jan 2002 00:00:00 -2359"), 2002,EJanuary, 19,00,01,0},	//  -2359	
	{_S8("Mon, 21 Jan 2002 23:00:00 -2359"), 2002,EJanuary, 19,23,01,0},	//  -2359
	
	{_S8("Mon, 21 Jan 2002 00:00:00 -2400"), 2002,EJanuary, 20,0,0,0},		//  -2400  Min offset should be -2359
	{_S8("Mon, 21 Jan 2002 23:00:00 -2400"), 2002,EJanuary, 20,23,0,0},		//  -2400  Min offset should be -2359
	{_S8("Mon, 21 Jan 2002 00:00:00 -0060"), 2002,EJanuary, 20,0,0,0},		//  -0060  Min min offset is -59
	{_S8("Mon, 21 Jan 2002 23:00:00 -0060"), 2002,EJanuary, 20,23,0,0},		//  -0060  Min min offset is -59
	{_S8("Mon, 21 Jan 2002 00:00:00 -0000"), 2002,EJanuary, 20,0,0,0},		//  -0000	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0000"), 2002,EJanuary, 20,23,0,0},		//  -0000
	{_S8("Mon, 21 Jan 2002 00:00:00 -0100"), 2002,EJanuary, 19,23,0,0},		//  -0100	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0105"), 2002,EJanuary, 20,21,55,0},	//  -0105
	{_S8("Mon, 21 Jan 2002 00:00:00 -0200"), 2002,EJanuary, 19,22,0,0},		//  -0200	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0210"), 2002,EJanuary, 20,20,50,0},	//  -0210
	
	{_S8("Mon, 21 Jan 2002 00:00:00 -0300"), 2002,EJanuary, 19,21,0,0},		//  -0300	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0359"), 2002,EJanuary, 20,19,01,0},	//  -0359
	{_S8("Mon, 21 Jan 2002 00:00:00 -0400"), 2002,EJanuary, 19,20,0,0},		//  -0400	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0400"), 2002,EJanuary, 20,19,0,0},		//  -0400
	{_S8("Mon, 21 Jan 2002 00:00:00 -0500"), 2002,EJanuary, 19,19,0,0},		//  -0500	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0500"), 2002,EJanuary, 20,18,0,0},		//  -0500
	{_S8("Mon, 21 Jan 2002 00:00:00 -0600"), 2002,EJanuary, 19,18,0,0},		//  -0600	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0600"), 2002,EJanuary, 20,17,0,0},		//  -0600
	{_S8("Mon, 21 Jan 2002 00:00:00 -0700"), 2002,EJanuary, 19,17,0,0},		//  -0700	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0700"), 2002,EJanuary, 20,16,0,0},		//  -0700

	{_S8("Mon, 21 Jan 2002 00:00:00 -0800"), 2002,EJanuary, 19,16,0,0},		//  -0800	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0800"), 2002,EJanuary, 20,15,0,0},		//  -0800
	{_S8("Mon, 21 Jan 2002 00:00:00 -0900"), 2002,EJanuary, 19,15,0,0},		//  -0900	
	{_S8("Mon, 21 Jan 2002 23:00:00 -0900"), 2002,EJanuary, 20,14,0,0},		//  -0900
	{_S8("Mon, 21 Jan 2002 00:00:00 -1000"), 2002,EJanuary, 19,14,0,0},		//  -1000	
	{_S8("Mon, 21 Jan 2002 23:00:00 -1000"), 2002,EJanuary, 20,13,0,0},		//  -1000	
	{_S8("Mon, 21 Jan 2002 00:00:00 -1100"), 2002,EJanuary, 19,13,0,0},		//  -1100	
	{_S8("Mon, 21 Jan 2002 23:00:00 -1100"), 2002,EJanuary, 20,12,0,0},		//  -1100
	{_S8("Mon, 21 Jan 2002 00:00:00 -1200"), 2002,EJanuary, 19,12,0,0},		//  -1200	
	{_S8("Mon, 21 Jan 2002 23:00:00 -1200"), 2002,EJanuary, 20,11,0,0}		//  -1200
	};

const TInt KNumberRfc850TimeOffsetTzTestCases = 64;
const struct TInternetDateTestCase KRfc850TimeOffsetTzTestCases[] =
	{
	{_S8("Mon, 21-Jan-02 00:00:00 +2359"), 2002,EJanuary, 20,23,59,0},	//  +2359	
	{_S8("Mon, 21-Jan-02 23:00:00 +2359"), 2002,EJanuary, 21,22,59,0},	//  +2359
	{_S8("Mon, 21-Jan-02 00:00:00 +2400"), 2002,EJanuary, 20,0,0,0},		//  +2400  Max offset should be +2359
	{_S8("Mon, 21-Jan-02 23:00:00 +2400"), 2002,EJanuary, 20,23,0,0},		//  +2400  Max offset should be +2359
	{_S8("Mon, 21-Jan-02 00:00:00 +0060"), 2002,EJanuary, 20,0,0,0},		//  +0060  Max min offset is 59
	{_S8("Mon, 21-Jan-02 23:00:00 +0060"), 2002,EJanuary, 20,23,0,0},		//  +0060  Max min offset is 59

	{_S8("Mon, 21-Jan-02 00:00:00 +0000"), 2002,EJanuary, 20,0,0,0},		//  +0000	
	{_S8("Mon, 21-Jan-02 23:00:00 -0000"), 2002,EJanuary, 20,23,0,0},		//  -0000
	{_S8("Mon, 21-Jan-02 00:00:00 +0100"), 2002,EJanuary, 20,1,0,0},		//  +0100	
	{_S8("Mon, 21-Jan-02 23:00:00 +0105"), 2002,EJanuary, 21,0,5,0},		//  +0105
	{_S8("Mon, 21-Jan-02 00:00:00 +0200"), 2002,EJanuary, 20,2,0,0},		//  +0200	
	{_S8("Mon, 21-Jan-02 23:00:00 +0210"), 2002,EJanuary, 21,1,10,0},		//  +0210
	{_S8("Mon, 21-Jan-02 00:00:00 +0300"), 2002,EJanuary, 20,3,0,0},		//  +0300	
	{_S8("Mon, 21-Jan-02 23:00:00 +0359"), 2002,EJanuary, 21,2,59,0},		//  +0359
	{_S8("Mon, 21-Jan-02 00:00:00 +0400"), 2002,EJanuary, 20,4,0,0},		//  +0400	
	{_S8("Mon, 21-Jan-02 23:00:00 +0400"), 2002,EJanuary, 21,3,0,0},		//  +0400
	{_S8("Mon, 21-Jan-02 00:00:00 +0500"), 2002,EJanuary, 20,5,0,0},		//  +0500	
	{_S8("Mon, 21-Jan-02 23:00:00 +0500"), 2002,EJanuary, 21,4,0,0},		//  +0500
	{_S8("Mon, 21-Jan-02 00:00:00 +0600"), 2002,EJanuary, 20,6,0,0},		//  +0600	
	{_S8("Mon, 21-Jan-02 23:00:00 +0600"), 2002,EJanuary, 21,5,0,0},		//  +0600
	{_S8("Mon, 21-Jan-02 00:00:00 +0700"), 2002,EJanuary, 20,7,0,0},		//  +0700	
	{_S8("Mon, 21-Jan-02 23:00:00 +0700"), 2002,EJanuary, 21,6,0,0},		//  +0700
	{_S8("Mon, 21-Jan-02 00:00:00 +0800"), 2002,EJanuary, 20,8,0,0},		//  +0800	
	{_S8("Mon, 21-Jan-02 23:00:00 +0800"), 2002,EJanuary, 21,7,0,0},		//  +0800
	{_S8("Mon, 21-Jan-02 00:00:00 +0900"), 2002,EJanuary, 20,9,0,0},		//  +0900	
	{_S8("Mon, 21-Jan-02 23:00:00 +0900"), 2002,EJanuary, 21,8,0,0},		//  +0900
	{_S8("Mon, 21-Jan-02 00:00:00 +1000"), 2002,EJanuary, 20,10,0,0},		//  +1000	
	{_S8("Mon, 21-Jan-02 23:00:00 +1000"), 2002,EJanuary, 21,9,0,0},		//  +1000	
	{_S8("Mon, 21-Jan-02 00:00:00 +1100"), 2002,EJanuary, 20,11,0,0},		//  +1100	
	{_S8("Mon, 21-Jan-02 23:00:00 +1100"), 2002,EJanuary, 21,10,0,0},		//  +1100
	{_S8("Mon, 21-Jan-02 00:00:00 +1200"), 2002,EJanuary, 20,12,0,0},		//  +1200	
	{_S8("Mon, 21-Jan-02 23:00:00 +1200"), 2002,EJanuary, 21,11,0,0},		//  +1200
	
	// Now same tests but with negative offsets
	{_S8("Mon, 21-Jan-02 00:00:00 -2359"), 2002,EJanuary, 19,00,01,0},	//  -2359	
	{_S8("Mon, 21-Jan-02 23:00:00 -2359"), 2002,EJanuary, 19,23,01,0},	//  -2359
	{_S8("Mon, 21-Jan-02 00:00:00 -2400"), 2002,EJanuary, 20,0,0,0},		//  -2400  Min offset should be -2359
	{_S8("Mon, 21-Jan-02 23:00:00 -2400"), 2002,EJanuary, 20,23,0,0},		//  -2400  Min offset should be -2359
	{_S8("Mon, 21-Jan-02 00:00:00 -0060"), 2002,EJanuary, 20,0,0,0},		//  -0060  Min min offset is -59
	{_S8("Mon, 21-Jan-02 23:00:00 -0060"), 2002,EJanuary, 20,23,0,0},		//  -0060  Min min offset is -59

	{_S8("Mon, 21-Jan-02 00:00:00 -0000"), 2002,EJanuary, 20,0,0,0},		//  -0000	
	{_S8("Mon, 21-Jan-02 23:00:00 -0000"), 2002,EJanuary, 20,23,0,0},		//  -0000
	{_S8("Mon, 21-Jan-02 00:00:00 -0100"), 2002,EJanuary, 19,23,0,0},		//  -0100	
	{_S8("Mon, 21-Jan-02 23:00:00 -0105"), 2002,EJanuary, 20,21,55,0},	//  -0105
	{_S8("Mon, 21-Jan-02 00:00:00 -0200"), 2002,EJanuary, 19,22,0,0},		//  -0200	
	{_S8("Mon, 21-Jan-02 23:00:00 -0210"), 2002,EJanuary, 20,20,50,0},	//  -0210
	{_S8("Mon, 21-Jan-02 00:00:00 -0300"), 2002,EJanuary, 19,21,0,0},		//  -0300	
	{_S8("Mon, 21-Jan-02 23:00:00 -0359"), 2002,EJanuary, 20,19,01,0},	//  -0359
	{_S8("Mon, 21-Jan-02 00:00:00 -0400"), 2002,EJanuary, 19,20,0,0},		//  -0400	
	{_S8("Mon, 21-Jan-02 23:00:00 -0400"), 2002,EJanuary, 20,19,0,0},		//  -0400
	{_S8("Mon, 21-Jan-02 00:00:00 -0500"), 2002,EJanuary, 19,19,0,0},		//  -0500	
	{_S8("Mon, 21-Jan-02 23:00:00 -0500"), 2002,EJanuary, 20,18,0,0},		//  -0500
	{_S8("Mon, 21-Jan-02 00:00:00 -0600"), 2002,EJanuary, 19,18,0,0},		//  -0600	
	{_S8("Mon, 21-Jan-02 23:00:00 -0600"), 2002,EJanuary, 20,17,0,0},		//  -0600
	{_S8("Mon, 21-Jan-02 00:00:00 -0700"), 2002,EJanuary, 19,17,0,0},		//  -0700	
	{_S8("Mon, 21-Jan-02 23:00:00 -0700"), 2002,EJanuary, 20,16,0,0},		//  -0700
	{_S8("Mon, 21-Jan-02 00:00:00 -0800"), 2002,EJanuary, 19,16,0,0},		//  -0800	
	{_S8("Mon, 21-Jan-02 23:00:00 -0800"), 2002,EJanuary, 20,15,0,0},		//  -0800
	{_S8("Mon, 21-Jan-02 00:00:00 -0900"), 2002,EJanuary, 19,15,0,0},		//  -0900	
	{_S8("Mon, 21-Jan-02 23:00:00 -0900"), 2002,EJanuary, 20,14,0,0},		//  -0900
	{_S8("Mon, 21-Jan-02 00:00:00 -1000"), 2002,EJanuary, 19,14,0,0},		//  -1000	
	{_S8("Mon, 21-Jan-02 23:00:00 -1000"), 2002,EJanuary, 20,13,0,0},		//  -1000	
	{_S8("Mon, 21-Jan-02 00:00:00 -1100"), 2002,EJanuary, 19,13,0,0},		//  -1100	
	{_S8("Mon, 21-Jan-02 23:00:00 -1100"), 2002,EJanuary, 20,12,0,0},		//  -1100
	{_S8("Mon, 21-Jan-02 00:00:00 -1200"), 2002,EJanuary, 19,12,0,0},		//  -1200	
	{_S8("Mon, 21-Jan-02 23:00:00 -1200"), 2002,EJanuary, 20,11,0,0}		//  -1200
	};


const TInt KInvalidTimeNumTests=46;
static const TBuf8<50> KInvalidTimeTestCases[]=
	{
	_L8("Mon,"),
    _L8("Mon, "),
    _L8("Mon, 14"),
    _L8("Mon, 14 "),
    _L8("Mon, Oct "),
    _L8("Mon, 14 Oct"),
    _L8("Mon, 14 Oct 2002"),
    _L8("Mon, 14 Oct 2002 12"), 
    _L8("Mon, 14 Oct 2002 12:00:00"), 
    _L8("Mon, 14 Oct 2002 12:00:00 GMT"),
    _L8("Mon, 32 Oct 2002 12:00:00 GMT"),
    _L8("Mon, 14 Aaa 2002 12:00:00 GMT"),
    _L8("Mon, 14 Oct 1969 12:00:00 GMT"),
    _L8("Mon, 14 Oct 2002 25:00:00 GMT"),
    _L8("Mon, 14 Oct 2002 12:99:00 GMT"),
    _L8("Mon, 14 Oct 2002 12:00:99 GMT"),
    _L8("Mon, 14 Oct 2002 12:00:00 GMT"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 32 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 128 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 255 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 256 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 1024 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 4096 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 \" ( 66000 x \"a\")"),
    _L8("Mon, 14 Oct 2002 12:00:00 %s%s%s%s%s%s%s%s"),
    _L8("Monday,"),
    _L8("Monday, "),
    _L8("Monday, 14"),
    _L8("Monday, 14-"),
    _L8("Monday, 14-Oct"),
    _L8("Monday, 14-Oct-"),
    _L8("Monday, 14-Oct-02"),
    _L8("Monday, 14-Oct-02 12:00:00 "),
    _L8("Monday, 14-Oct-02 12:00:00 GMT"),
    _L8("Aaaaaa, 14--02 12:00:00 GMT"),
    _L8("Monday, 32-Oct-02 12:00:00 GMT"),
    _L8("Monday, 14-Aaa-02 12:00:00 GMT"),
    _L8("Monday, 14-Oct-50 12:00:00 GMT"),
    _L8("Monday, 14-Oct-02 30:00:00 GMT"),
    _L8("Monday, 14-Oct-02 12:99:00 GMT"),
    _L8("Monday, 14-Oct-02 12:00:99 GMT"),
    _L8("Monday, 14-Oct-02 12:00:00 \" ( 32 x \"a\")"),
    _L8("Monday, 14-Oct-02 12:00:00 \" ( 255 x \"a\")"),
    _L8("Monday, 14-Oct-02 12:00:00 \" ( 256 x \"a\")"),
    _L8("Monday, 14-Oct-02 12:00:00 \" ( 66000 x \"a\")"),
    _L8("Monday, 14-Oct-02 12:00:00 %s%s%s%s"),
	};

struct TInternetDateOffsetTestCase
	{
	const TBuf8<50> iTextInput;
	const TBuf8<50> iExpectedOutput;
	};

const TInt KNumberDEF126191TestCases = 80;

//{ _L8("Input Date time "), _L8("Expected DateTime") },
static struct TInternetDateOffsetTestCase KDEF126191TestCases[]=
	{
	{ _L8("Thu, 17 Jul 2008 08:04:31 EDT"), _L8("Thu, 17 Jul 2008 12:04:31 GMT") },
	{ _L8("Fri, 18 Nov 2005 09:01:17 -0600"), _L8("Fri, 18 Nov 2005 15:01:17 GMT") },
	{ _L8("Mon, 21 Nov 2005 09:05:01 EST"), _L8("Mon, 21 Nov 2005 14:05:01 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +1100"), _L8("Sun, 20 Jan 2002 13:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +2359"), _L8("Sun, 20 Jan 2002 00:01:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0059"), _L8("Sun, 20 Jan 2002 23:01:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0000"), _L8("Mon, 21 Jan 2002 00:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0000"), _L8("Mon, 21 Jan 2002 23:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0100"), _L8("Sun, 20 Jan 2002 23:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0105"), _L8("Mon, 21 Jan 2002 21:55:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0200"), _L8("Sun, 20 Jan 2002 22:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0210"), _L8("Mon, 21 Jan 2002 20:50:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0300"), _L8("Sun, 20 Jan 2002 21:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0359"), _L8("Mon, 21 Jan 2002 19:01:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0400"), _L8("Sun, 20 Jan 2002 20:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0400"), _L8("Mon, 21 Jan 2002 19:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0500"), _L8("Sun, 20 Jan 2002 19:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0500"), _L8("Mon, 21 Jan 2002 18:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0600"), _L8("Sun, 20 Jan 2002 18:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0600"), _L8("Mon, 21 Jan 2002 17:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0700"), _L8("Sun, 20 Jan 2002 17:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0700"), _L8("Mon, 21 Jan 2002 16:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0800"), _L8("Sun, 20 Jan 2002 16:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0800"), _L8("Mon, 21 Jan 2002 15:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +0900"), _L8("Sun, 20 Jan 2002 15:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +0900"), _L8("Mon, 21 Jan 2002 14:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +1000"), _L8("Sun, 20 Jan 2002 14:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +1000"), _L8("Mon, 21 Jan 2002 13:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 +1200"), _L8("Sun, 20 Jan 2002 12:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 +1200"), _L8("Mon, 21 Jan 2002 11:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -2359"), _L8("Mon, 21 Jan 2002 23:59:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -2359"), _L8("Tue, 22 Jan 2002 22:59:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0059"), _L8("Mon, 21 Jan 2002 00:59:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0059"), _L8("Mon, 21 Jan 2002 23:59:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0000"), _L8("Mon, 21 Jan 2002 00:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0000"), _L8("Mon, 21 Jan 2002 23:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0100"), _L8("Mon, 21 Jan 2002 01:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0105"), _L8("Tue, 22 Jan 2002 00:05:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0200"), _L8("Mon, 21 Jan 2002 02:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0210"), _L8("Tue, 22 Jan 2002 01:10:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0300"), _L8("Mon, 21 Jan 2002 03:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0359"), _L8("Tue, 22 Jan 2002 02:59:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0400"), _L8("Mon, 21 Jan 2002 04:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0400"), _L8("Tue, 22 Jan 2002 03:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0500"), _L8("Mon, 21 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0500"), _L8("Tue, 22 Jan 2002 04:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0600"), _L8("Mon, 21 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0600"), _L8("Tue, 22 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0700"), _L8("Mon, 21 Jan 2002 07:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0700"), _L8("Tue, 22 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0800"), _L8("Mon, 21 Jan 2002 08:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0800"), _L8("Tue, 22 Jan 2002 07:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -0900"), _L8("Mon, 21 Jan 2002 09:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -0900"), _L8("Tue, 22 Jan 2002 08:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -1000"), _L8("Mon, 21 Jan 2002 10:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -1000"), _L8("Tue, 22 Jan 2002 09:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -1100"), _L8("Mon, 21 Jan 2002 11:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -1100"), _L8("Tue, 22 Jan 2002 10:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 -1200"), _L8("Mon, 21 Jan 2002 12:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 -1200"), _L8("Tue, 22 Jan 2002 11:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 EST"), _L8("Mon, 21 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 EST"), _L8("Tue, 22 Jan 2002 04:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 EDT"), _L8("Mon, 21 Jan 2002 04:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 EDT"), _L8("Tue, 22 Jan 2002 03:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 CST"), _L8("Mon, 21 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 CST"), _L8("Tue, 22 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 CDT"), _L8("Mon, 21 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 CDT"), _L8("Tue, 22 Jan 2002 04:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 MST"), _L8("Mon, 21 Jan 2002 07:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 MST"), _L8("Tue, 22 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 MDT"), _L8("Mon, 21 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 MDT"), _L8("Tue, 22 Jan 2002 05:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 PST"), _L8("Mon, 21 Jan 2002 08:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 PST"), _L8("Tue, 22 Jan 2002 07:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 PDT"), _L8("Mon, 21 Jan 2002 07:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 PDT"), _L8("Tue, 22 Jan 2002 06:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 00:00:00 UT"), _L8("Mon, 21 Jan 2002 00:00:00 GMT") }, 
	{ _L8("Mon, 21 Jan 2002 23:00:00 UT"), _L8("Mon, 21 Jan 2002 23:00:00 GMT") }, 
	{ _L8("Mon, 21 Jan 2002 00:00:00 GMT"), _L8("Mon, 21 Jan 2002 00:00:00 GMT") },
	{ _L8("Mon, 21 Jan 2002 23:00:00 GMT"), _L8("Mon, 21 Jan 2002 23:00:00 GMT") }
	};
	
const TInt KInvalidDescriptorToIntTests=5;
static const TBuf8<50> KInvalidDesToIntTestCases[]=
	{
	_L8("-"),
    _L8("+"),
    _L8("--"),
    _L8("++"),
    _L8("InvalidString")
	};	

#endif //	__T_INTERNETDATETESTCASES_H__
