@echo off
@rem
@rem Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

if not "%1"=="" goto %1

call %0 testall unicode \charconv\plugins\test\data\tool\le.uni utf8 \charconv\plugins\test\data\tool\unicode.ut8 -little
call %0 testall unicode \charconv\plugins\test\data\tool\le_bom.uni utf8 \charconv\plugins\test\data\tool\unicode.ut8 -little -byteordermark
call %0 testall unicode \charconv\plugins\test\data\tool\be.uni utf8 \charconv\plugins\test\data\tool\unicode.ut8 -big
call %0 testall unicode \charconv\plugins\test\data\tool\be_bom.uni utf8 \charconv\plugins\test\data\tool\unicode.ut8 -big -byteordermark

call %0 testall shiftjis \charconv\plugins\test\data\tool\SHIFTJIS.TXT utf8 \charconv\plugins\test\data\tool\shiftjis.ut8

call %0 testoneway cp1252 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\cp1252.uni -little
call %0 testoneway iso88591 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88591.uni -little
call %0 testoneway iso88592 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88592.uni -little
call %0 testoneway iso88593 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88593.uni -little
call %0 testoneway iso88594 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88594.uni -little
call %0 testoneway iso88595 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88595.uni -little
call %0 testoneway iso88596 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88596.uni -little
call %0 testoneway iso88597 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88597.uni -little
call %0 testoneway iso88598 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88598.uni -little
call %0 testoneway iso88599 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso88599.uni -little
call %0 testoneway iso885910 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso885910.uni -little
call %0 testoneway iso885913 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso885913.uni -little
call %0 testoneway iso885914 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso885914.uni -little
call %0 testoneway iso885915 \charconv\plugins\test\data\tool\all_bytes.txt unicode \charconv\plugins\test\data\tool\iso885915.uni -little

goto end

:testoneway

call charconv -input=%2 %3 -output=%4 \charconv\plugins\test\data\tool\_._ %6 %7 %8 %9 2> NUL
fc /b %5 \charconv\plugins\test\data\tool\_._
del \charconv\plugins\test\data\tool\_._

call charconv -input=%2 -output=%4 \charconv\plugins\test\data\tool\_._ %6 %7 %8 %9 <%3 2> NUL
fc /b %5 \charconv\plugins\test\data\tool\_._
del \charconv\plugins\test\data\tool\_._

call charconv -input=%2 %3 -output=%4 %6 %7 %8 %9 >\charconv\plugins\test\data\tool\_._ 2> NUL
fc /b %5 \charconv\plugins\test\data\tool\_._
del \charconv\plugins\test\data\tool\_._

call charconv -input=%2 -output=%4 %6 %7 %8 %9 <%3 >\charconv\plugins\test\data\tool\_._ 2> NUL
fc /b %5 \charconv\plugins\test\data\tool\_._
del \charconv\plugins\test\data\tool\_._

goto end

:testall

call %0 testoneway %2 %3 %4 %5 %6 %7 %8 %9
call %0 testoneway %4 %5 %2 %3 %6 %7 %8 %9

goto end

:end

