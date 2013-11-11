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
*
*/
#include "libutils.h"

//chartotbuf8 related test cases
void test_chartotbuf8_positive();
void test_chartotbuf8_negative1();
void test_chartotbuf8_negative2();

void test_chartotbuf16_positive();

//stringtotbuf8 related test cases
void test_stringtotbuf8_positive();
void test_stringtotbuf8_negative1();
void test_stringtotbuf8_negative2();
void test_stringtotbuf8_boundry();

void test_stringtotbuf8_boundry1();
void test_stringtotbuf8_boundry2();

//wstringtotbuf8 related test cases
void test_wstringtotbuf8_positive();
void test_wstringtotbuf8_negative1();
void test_wstringtotbuf8_negative2();
void test_wstringtotbuf8_boundry();

//wchartotbuf8 related test cases
//void test_wchartotbuf8_positive();
void test_wchartotbuf8_negative1();
void test_wchartotbuf8_negative2();

//wstring to tptr8 related test cases
void test_wstringtotptr8_positive();
void test_wstringtotptr8_negative1();
void test_wstringtotptr8_negative2();
void test_wstringtotptrc8_positive_unicode();

//stringtotbuf16 related cases
void test_stringtotbuf16_positive();
void test_stringtotbuf16_negative1();
void test_stringtotbuf16_negative2();
void test_stringtotbuf16_negative3();

//stringtotptr16 related cases
void test_stringtotptr16_positive();
void test_stringtotptr16_negative1();
void test_stringtotptr16_negative2();
void test_stringtotptr16_negative3();

//stringtorbuf8 related test cases
void test_stringtorbuf8_positive();
void test_stringtorbuf8_negative();

//stringtotptr8 related test cases
void test_stringtotptr8_positive();
void test_stringtotptr8_negative1();
void test_stringtotptr8_negative2();
void test_stringtotptr8_negative3();

//stringtotptrc8 related test cases
void test_stringtotptrc8_positive();
void test_stringtotptrc8_negative1();

//stringtohbufc8 related test cases
void test_stringtohbufc8_positive();
void test_stringtohbufc8_negative1();
void test_stringtohbufc8_negative2();
void test_stringtohbufc8_negative3();

//tbufc8 to string conversions
void test_tbufc8tostring_positive();
void test_tbufc8tostring_negative1();
void test_tbufc8tostring_negative2();

//tbuf8 to string conversions
void test_tbuf8tostring_positive();
void test_tbuf8tostring_negative1();
void test_tbuf8tostring_positive1();

//tptr8 to string conversions
void test_tptr8tostring_positive();
void test_tptr8tostring_positive1();
void test_tptr8tostring_negative1();

//tptrc8 to string conversions
void test_tptrc8tostring_positive();
void test_tptrc8tostring_positive1();
void test_tptrc8tostring_negative1();

//hbufc8 to string conversions
void test_hbufc8tostring_positive();
void test_hbufc8tostring_negative1();
void test_hbufc8tostring_positive1();

//rbuf8 to string conversions
void test_rbuf8tostring_positive();
void test_rbuf8tostring_positive1();
void test_rbuf8tostring_negative1();

//tlit8 to string conversions
void test_tlit8tostring_positive();
void test_tlit8tostring_positive1();
void test_tlit8tostring_negative1();

//descriptor8 to string conversions
void test_tbuf8towchar_positive();
void test_tbuf8towchar_negative1();

//string to rbuf16 coversion test cases 
void test_stringtorbuf16_positive();
void test_stringtorbuf16_negative1();

void test_wstringtotptrc8_positive();
void test_wstringtotptrc8_negative1();


void test_wstringtohbufc8_positive();
void test_wstringtohbufc8_negative1();
void test_wstringtohbufc8_positive_unicode();

//wstring to descriptor16 coversion test cases
 
void test_wstringtotbuf16_positive();
void test_wstringtotbuf16_negative1();
void test_wstringtotbuf16_negative2();
void test_wstringtotbuf16_boundry();
void test_wstringtotbuf16_boundry1();
void test_wstringtotbuf16_boundry2();
void test_wstringtorbuf16_positive();
void test_wstringtorbuf16_negative();
void test_wstringtotptr16_positive();
void test_wstringtotptr16_negative1();
void test_wstringtotptrc16_positive();
void test_wstringtotptrc16_negative1();
void test_wstringtohbufc16_positive();
void test_wstringtohbufc16_negative1();

void test_wchartohbufc8_positive();
void test_wchartotbuf8_positive();

void test_stringtotbuf8_positive();

void test_stringtotbuf8_negative1();
void test_stringtotbuf8_negative2();
void test_stringtotbuf8_boundry();

void test_stringtotbuf8_boundry1();
void test_stringtotbuf8_boundry2();

//wstringtotbuf8 related test cases
void test_wstringtotbuf8_positive();
void test_wstringtotbuf8_negative1();
void test_wstringtotbuf8_negative2();
void test_wstringtotbuf8_boundry();

//wchartotbuf8 related test cases
void test_wchartotbuf8_positive();
void test_wchartotbuf8_unicode();
void test_wchartotbuf8_positive1();
void test_wchartotbuf8_boundary1();
void test_wchartotbuf8_boundary2();
void test_wchartotbuf8_content_check();
void test_wchartotbuf8_negative1();
void test_wchartotbuf8_negative2();


void test_wchartotbuf16_positive();
void test_wchartotbuf16_positive1();
void test_wchartotbuf16_boundary1();
void test_wchartotbuf16_boundary2();
void test_wchartotbuf16_content_check();
void test_wchartotbuf16_negative1();
void test_wchartotbuf16_negative2();


void test_wchartohbufc8_positive();


void test_wcharptotptr8_positive();
void test_wcharptotptr8_positive1();
void test_wcharptotptr8_boundary1();
void test_wcharptotptr8_boundary2();
void test_wcharptotptr8_content_check();
void test_wcharptotptr8_negative1();
void test_wcharptotptr8_negative2();

void test_wcharptotptr16_positive1();
void test_wcharptotptr16_positive2();
void test_wcharptotptr16_boundary1();
void test_wcharptotptr16_boundary2();
void test_wcharptotptr16_content_check();
void test_wcharptotptr16_negative1();
void test_wcharptotptr16_negative2();

void test_wcharptotptrc8_positive1();
void test_wcharptotptrc8_positive2();
void test_wcharptotptrc8_boundary1();
void test_wcharptotptrc8_boundary2();
void test_wcharptotptrc8_content_check();
void test_wcharptotptrc8_negative1();
void test_wcharptotptrc8_negative2();

void test_wcharptotptrc16_positive1();
void test_wcharptotptrc16_positive2();
void test_wcharptotptrc16_boundary1();
void test_wcharptotptrc16_boundary2();
void test_wcharptotptrc16_content_check();
void test_wcharptotptrc16_negative2();

void test_wchartorbuf8_positive1();
void test_wchartorbuf8_positive2();
void test_wchartorbuf8_boundary1();
void test_wchartorbuf8_boundary2();
void test_wchartorbuf8_content_check();
void test_wchartorbuf8_negative1();
void test_wchartorbuf8_negative2();


void test_wchartorbuf16_positive1();
void test_wchartorbuf16_positive2();
void test_wchartorbuf16_boundary1();
void test_wchartorbuf16_boundary2();
void test_wchartorbuf16_content_check();
void test_wchartorbuf16_negative1();
void test_wchartorbuf16_negative2();
// ------------------------------------


void test_chartotbuf8_positive1();
void test_chartotbuf8_positive2();
void test_chartotbuf8_boundary1();
void test_chartotbuf8_boundary2();
void test_chartotbuf8_content_check();
void test_chartotbuf8_negative1();
void test_chartotbuf8_negative2();

void test_chartotbuf16_positive1();
void test_chartotbuf16_positive2();
void test_chartotbuf16_boundary1();
void test_chartotbuf16_boundary2();
void test_chartotbuf16_content_check();
void test_chartotbuf16_negative1();
void test_chartotbuf16_negative2();


void test_charptotptr8_positive1();
void test_charptotptr8_positive2();
void test_charptotptr8_boundary1();
void test_charptotptr8_boundary2();
void test_charptotptr8_content_check();
void test_charptotptr8_negative1();
void test_charptotptr8_negative2();


void test_charptotptr16_positive1();
void test_charptotptr16_positive2();
void test_charptotptr16_boundary1();
void test_charptotptr16_boundary2();
void test_charptotptr16_content_check();
void test_charptotptr16_negative1();
void test_charptotptr16_negative2();

void test_charptotptrc8_positive1();
void test_charptotptrc8_positive2();
void test_charptotptrc8_boundary1();
void test_charptotptrc8_boundary2();
void test_charptotptrc8_content_check();
//void test_charptotptrc8_negative1();
void test_charptotptrc8_negative2();


void test_charptotptrc16_positive1();
void test_charptotptrc16_positive2();
void test_charptotptrc16_boundary1();
void test_charptotptrc16_boundary2();
void test_charptotptrc16_content_check();
void test_charptotptrc16_negative2();


void test_chartorbuf8_positive1();
void test_chartorbuf8_positive2();
void test_chartorbuf8_boundary1();
void test_chartorbuf8_boundary2();
void test_chartorbuf8_content_check();
void test_chartorbuf8_negative1();
void test_chartorbuf8_negative2();


void test_chartorbuf16_positive1();
void test_chartorbuf16_positive2();
void test_chartorbuf16_boundary1();
void test_chartorbuf16_boundary2();
void test_chartorbuf16_content_check();
void test_chartorbuf16_negative1();
void test_chartorbuf16_negative2();



