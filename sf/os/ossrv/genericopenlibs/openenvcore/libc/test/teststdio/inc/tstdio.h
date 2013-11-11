/*
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



#ifndef __TESTSTDIO_H__
#define __TESTSTDIO_H__

#include <test/testexecutestepbase.h>
#include <sys/fcntl.h>
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <utime.h>

#include <sys/socket.h>
#include <netdb.h>
#include <wchar.h>  //Needed for wopen()
#include <spawn.h> //Needed for posix_spawnattr_destroy()
#include <pwd.h> 

// MACROS
//#define ?macro ?macro_def
#define BUF_SIZE 100

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt bufSize = 200;


//utility functions
void MakeSpace(char *);


_LIT(Kremove1, "remove1");
_LIT(Kremove2, "remove2");
_LIT(Krename1, "rename1");
_LIT(Ktempnam1, "tempnam1");
_LIT(Ktempnam2, "tempnam2");
_LIT(Kfclose1, "fclose1");
_LIT(Kfflush1, "fflush1");
_LIT(Kfopen1, "fopen1");
_LIT(Kfopen2, "fopen2");
_LIT(Kfopen3, "fopen3");
_LIT(Ksetbuf1, "setbuf1");
_LIT(Ksetbuf2, "setbuf2");
_LIT(Ksetvbuf1, "setvbuf1");
_LIT(Ksetvbuf2, "setvbuf2");
_LIT(Ksetvbuf3, "setvbuf3");
_LIT(Kclearerr1, "clearerr1");
_LIT(Kfeof1, "feof1");
_LIT(Kferror1, "ferror1");
_LIT(Kfread1, "fread1");
_LIT(Kfread2, "fread2");
_LIT(Kfread3, "fread3");
_LIT(Kfwrite1, "fwrite1");
_LIT(Kfwrite2, "fwrite2");
_LIT(Kfwrite3, "fwrite3");
_LIT(Kfgetpos1, "fgetpos1");
_LIT(Kfseek1, "fseek1");
_LIT(Kfseek2, "fseek2");
_LIT(Kfsetpos1, "fsetpos1");
_LIT(Kfsetpos2, "fsetpos2");
_LIT(Kftell1, "ftell1");
_LIT(Krewind1, "rewind1");
_LIT(Kfgetc1, "fgetc1");
_LIT(Kfgets1, "fgets1");
_LIT(Kfgets2, "fgets2");
_LIT(Kfgets3, "fgets3");
_LIT(Kfputc1, "fputc1");
_LIT(Kfputc2, "fputc2");
_LIT(Kfputc3, "fputc3");
_LIT(Kfputc4, "fputc4");
_LIT(Kfputs1, "fputs1");
_LIT(Kfputs2, "fputs2");
_LIT(Kfputs3, "fputs3");
_LIT(Kfputs4, "fputs4");
_LIT(Kgetc1, "getc1");
_LIT(Kputc1, "putc1");
_LIT(Kputc2, "putc2");
_LIT(Kputc3, "putc3");
_LIT(Kputc4, "putc4");
_LIT(Kungetc1, "ungetc1");
_LIT(Kfileno1, "fileno1");
_LIT(Kfileno2, "fileno2");
_LIT(Kfileno3, "fileno3");
_LIT(Kgetw1, "getw1");
_LIT(Kputw1, "putw1");
_LIT(Kgetdirentries1, "getdirentries1");
_LIT(Kgetdirentries2, "getdirentries2"); 
_LIT(Kfreopen1, "freopen1");
_LIT(Kbinaryopen,"binaryopen");
_LIT(Kbinaryfopen,"binaryfopen");

//printf manual
_LIT(KprintfTest1L, "printfTest1L");
_LIT(KprintfTest2L, "printfTest2L");
_LIT(KprintfTest3L, "printfTest3L");
_LIT(KprintfTest4L, "printfTest4L");
_LIT(KprintfTest5L, "printfTest5L");
_LIT(KprintfTest6L, "printfTest6L");
_LIT(KprintfTest7L, "printfTest7L");
_LIT(KprintfTest8L, "printfTest8L");
_LIT(KprintfTest9L, "printfTest9L");
_LIT(KprintfTest10L, "printfTest10L");
_LIT(KprintfTest11L, "printfTest11L");
_LIT(KprintfTest12L, "printfTest12L");
_LIT(KprintfTest13L, "printfTest13L");
_LIT(KprintfTest14L, "printfTest14L");
_LIT(KprintfTest15L, "printfTest15L");
_LIT(KprintfTest16L, "printfTest16L");
_LIT(KprintfTest17L, "printfTest17L");
_LIT(KprintfTest18L, "printfTest18L");
_LIT(KprintfTest19L, "printfTest19L");
_LIT(KprintfTest20L, "printfTest20L");
_LIT(KprintfTest21L, "printfTest21L");
_LIT(KprintfTest22L, "printfTest22L");
_LIT(KprintfTest23L, "printfTest23L");
_LIT(KprintfTest24L, "printfTest24L");
_LIT(KprintfTest25L, "printfTest25L");
_LIT(KprintfTest26L, "printfTest26L");
_LIT(KprintfTest27L, "printfTest27L");

//fprintf
_LIT(KfprintfTest1L, "fprintfTest1L");
_LIT(KfprintfTest2L, "fprintfTest2L");
_LIT(KfprintfTest3L, "fprintfTest3L");
_LIT(KfprintfTest4L, "fprintfTest4L");
_LIT(KfprintfTest5L, "fprintfTest5L");
_LIT(KfprintfTest6L, "fprintfTest6L");
_LIT(KfprintfTest7L, "fprintfTest7L");
_LIT(KfprintfTest8L, "fprintfTest8L");
_LIT(KfprintfTest9L, "fprintfTest9L");
_LIT(KfprintfTest10L, "fprintfTest10L");
_LIT(KfprintfTest11L, "fprintfTest11L");
_LIT(KfprintfTest12L, "fprintfTest12L");
_LIT(KfprintfTest13L, "fprintfTest13L");
_LIT(KfprintfTest14L, "fprintfTest14L");
_LIT(KfprintfTest15L, "fprintfTest15L");
_LIT(KfprintfTest16L, "fprintfTest16L");
_LIT(KfprintfTest17L, "fprintfTest17L");
_LIT(KfprintfTest18L, "fprintfTest18L");
_LIT(KfprintfTest19L, "fprintfTest19L");
_LIT(KfprintfTest20L, "fprintfTest20L");
_LIT(KfprintfTest21L, "fprintfTest21L");
_LIT(KfprintfTest22L, "fprintfTest22L");
_LIT(KfprintfTest23L, "fprintfTest23L");
_LIT(KfprintfTest24L, "fprintfTest24L");
_LIT(KfprintfTest25L, "fprintfTest25L");
_LIT(KfprintfTest26L, "fprintfTest26L");
_LIT(KfprintfTest27L, "fprintfTest27L");


_LIT(KsprintfTest1L, "sprintfTest1L");
_LIT(KsprintfTest2L, "sprintfTest2L");
_LIT(KsprintfTest3L, "sprintfTest3L");
_LIT(KsprintfTest4L, "sprintfTest4L");
_LIT(KsprintfTest5L, "sprintfTest5L");
_LIT(KsprintfTest6L, "sprintfTest6L");
_LIT(KsprintfTest7L, "sprintfTest7L");
_LIT(KsprintfTest8L, "sprintfTest8L");
_LIT(KsprintfTest9L, "sprintfTest9L");
_LIT(KsprintfTest10L, "sprintfTest10L");
_LIT(KsprintfTest11L, "sprintfTest11L");
_LIT(KsprintfTest12L, "sprintfTest12L");
_LIT(KsprintfTest13L, "sprintfTest13L");
_LIT(KsprintfTest14L, "sprintfTest14L");
_LIT(KsprintfTest15L, "sprintfTest15L");
_LIT(KsprintfTest16L, "sprintfTest16L");
_LIT(KsprintfTest17L, "sprintfTest17L");
_LIT(KsprintfTest18L, "sprintfTest18L");
_LIT(KsprintfTest19L, "sprintfTest19L");
_LIT(KsprintfTest20L, "sprintfTest20L");
_LIT(KsprintfTest21L, "sprintfTest21L");
_LIT(KsprintfTest22L, "sprintfTest22L");
_LIT(KsprintfTest23L, "sprintfTest23L");
_LIT(KsprintfTest24L, "sprintfTest24L");
_LIT(KsprintfTest25L, "sprintfTest25L");
_LIT(KsprintfTest26L, "sprintfTest26L");
_LIT(KsprintfTest27L, "sprintfTest27L");
_LIT(KsprintfTest28L, "sprintfTest28L");
_LIT(KsprintfTest29L, "sprintfTest29L");
_LIT(KsprintfTestInfL, "sprintfTestInfL");
_LIT(KsprintfTestNanL, "sprintfTestNanL");

//snprintf
_LIT(KsnprintfTest1L, "snprintfTest1L");
_LIT(KsnprintfTest2L, "snprintfTest2L");
_LIT(KsnprintfTest3L, "snprintfTest3L");
_LIT(KsnprintfTest4L, "snprintfTest4L");
_LIT(KsnprintfTest5L, "snprintfTest5L");
_LIT(KsnprintfTest6L, "snprintfTest6L");
_LIT(KsnprintfTest7L, "snprintfTest7L");
_LIT(KsnprintfTest8L, "snprintfTest8L");
_LIT(KsnprintfTest9L, "snprintfTest9L");
_LIT(KsnprintfTest10L, "snprintfTest10L");
_LIT(KsnprintfTest11L, "snprintfTest11L");
_LIT(KsnprintfTest12L, "snprintfTest12L");
_LIT(KsnprintfTest13L, "snprintfTest13L");
_LIT(KsnprintfTest14L, "snprintfTest14L");
_LIT(KsnprintfTest15L, "snprintfTest15L");
_LIT(KsnprintfTest16L, "snprintfTest16L");
_LIT(KsnprintfTest17L, "snprintfTest17L");
_LIT(KsnprintfTest18L, "snprintfTest18L");
_LIT(KsnprintfTest19L, "snprintfTest19L");
_LIT(KsnprintfTest20L, "snprintfTest20L");
_LIT(KsnprintfTest21L, "snprintfTest21L");
_LIT(KsnprintfTest22L, "snprintfTest22L");
_LIT(KsnprintfTest23L, "snprintfTest23L");
_LIT(KsnprintfTest24L, "snprintfTest24L");
_LIT(KsnprintfTest25L, "snprintfTest25L");
_LIT(KsnprintfTest26L, "snprintfTest26L");
_LIT(KsnprintfTest27L, "snprintfTest27L");

_LIT(KvprintfTest1L, "vprintfTest1L");
_LIT(KvprintfTest2L, "vprintfTest2L");
_LIT(KvprintfTest3L, "vprintfTest3L");
_LIT(KvprintfTest4L, "vprintfTest4L");
_LIT(KvprintfTest5L, "vprintfTest5L");
_LIT(KvprintfTest6L, "vprintfTest6L");
_LIT(KvprintfTest7L, "vprintfTest7L");
_LIT(KvprintfTest8L, "vprintfTest8L");
_LIT(KvprintfTest9L, "vprintfTest9L");
_LIT(KvprintfTest10L, "vprintfTest10L");
_LIT(KvprintfTest11L, "vprintfTest11L");
_LIT(KvprintfTest12L, "vprintfTest12L");
_LIT(KvprintfTest13L, "vprintfTest13L");
_LIT(KvprintfTest14L, "vprintfTest14L");
_LIT(KvprintfTest15L, "vprintfTest15L");
_LIT(KvprintfTest16L, "vprintfTest16L");
_LIT(KvprintfTest17L, "vprintfTest17L");
_LIT(KvprintfTest18L, "vprintfTest18L");
_LIT(KvprintfTest19L, "vprintfTest19L");
_LIT(KvprintfTest20L, "vprintfTest20L");
_LIT(KvprintfTest21L, "vprintfTest21L");
_LIT(KvprintfTest22L, "vprintfTest22L");
_LIT(KvprintfTest23L, "vprintfTest23L");
_LIT(KvprintfTest24L, "vprintfTest24L");
_LIT(KvprintfTest25L, "vprintfTest25L");
_LIT(KvprintfTest26L, "vprintfTest26L");
_LIT(KvprintfTest27L, "vprintfTest27L");

_LIT(Kfprintfbasic, "fprintfbasic");

//scanf
_LIT(KscanfTest1L, "scanfTest1L");
_LIT(KscanfTest2L, "scanfTest2L");
_LIT(KscanfTest3L, "scanfTest3L");
_LIT(KscanfTest4L, "scanfTest4L");
_LIT(KscanfTest5L, "scanfTest5L");
_LIT(KscanfTest6L, "scanfTest6L");
_LIT(KscanfTest7L, "scanfTest7L");
_LIT(KscanfTest8L, "scanfTest8L");
_LIT(KscanfTest9L, "scanfTest9L");
_LIT(KscanfTest10L, "scanfTest10L");
_LIT(KscanfTest11L, "scanfTest11L");
_LIT(KscanfTest12L, "scanfTest12L");
_LIT(KscanfTest13L, "scanfTest13L");

//sscanf
_LIT(KsscanfTest1L, "sscanfTest1L");
_LIT(KsscanfTest2L, "sscanfTest2L");
_LIT(KsscanfTest3L, "sscanfTest3L");
_LIT(KsscanfTest4L, "sscanfTest4L");
_LIT(KsscanfTest5L, "sscanfTest5L");
_LIT(KsscanfTest6L, "sscanfTest6L");
_LIT(KsscanfTest7L, "sscanfTest7L");
_LIT(KsscanfTest8L, "sscanfTest8L");
_LIT(KsscanfTest9L, "sscanfTest9L");
_LIT(KsscanfTest10L, "sscanfTest10L");
_LIT(KsscanfTest11L, "sscanfTest11L");
_LIT(KsscanfTest12L, "sscanfTest12L");
_LIT(KsscanfTest13L, "sscanfTest13L");
_LIT(KsscanfTest14L, "sscanfTest14L");

//fscanf
_LIT(KfscanfTest1L, "fscanfTest1L");
_LIT(KfscanfTest2L, "fscanfTest2L");
_LIT(KfscanfTest3L, "fscanfTest3L");
_LIT(KfscanfTest4L, "fscanfTest4L");
_LIT(KfscanfTest5L, "fscanfTest5L");
_LIT(KfscanfTest6L, "fscanfTest6L");
_LIT(KfscanfTest7L, "fscanfTest7L");
_LIT(KfscanfTest8L, "fscanfTest8L");
_LIT(KfscanfTest9L, "fscanfTest9L");
_LIT(KfscanfTest10L, "fscanfTest10L");
_LIT(KfscanfTest11L, "fscanfTest11L");
_LIT(KfscanfTest12L, "fscanfTest12L");
_LIT(KfscanfTest13L, "fscanfTest13L");
_LIT(KfscanfTest14L, "fscanfTest14L");

_LIT(KvscanfTest1L, "vscanfTest1L");
_LIT(KvscanfTest2L, "vscanfTest2L");
_LIT(KvscanfTest3L, "vscanfTest3L");
_LIT(KvscanfTest4L, "vscanfTest4L");
_LIT(KvscanfTest5L, "vscanfTest5L");
_LIT(KvscanfTest6L, "vscanfTest6L");
_LIT(KvscanfTest7L, "vscanfTest7L");
_LIT(KvscanfTest8L, "vscanfTest8L");
_LIT(KvscanfTest9L, "vscanfTest9L");
_LIT(KvscanfTest10L, "vscanfTest10L");
_LIT(KvscanfTest11L, "vscanfTest11L");
_LIT(KvscanfTest12L, "vscanfTest12L");
_LIT(KvscanfTest13L, "vscanfTest13L");

//vsscanf
_LIT(KvsscanfTest1L, "vsscanfTest1L");
_LIT(KvsscanfTest2L, "vsscanfTest2L");
_LIT(KvsscanfTest3L, "vsscanfTest3L");
_LIT(KvsscanfTest4L, "vsscanfTest4L");
_LIT(KvsscanfTest5L, "vsscanfTest5L");
_LIT(KvsscanfTest6L, "vsscanfTest6L");
_LIT(KvsscanfTest7L, "vsscanfTest7L");
_LIT(KvsscanfTest8L, "vsscanfTest8L");
_LIT(KvsscanfTest9L, "vsscanfTest9L");
_LIT(KvsscanfTest10L, "vsscanfTest10L");
_LIT(KvsscanfTest11L, "vsscanfTest11L");
_LIT(KvsscanfTest12L, "vsscanfTest12L");
_LIT(KvsscanfTest13L, "vsscanfTest13L");
_LIT(KvsscanfTest14L, "vsscanfTest14L");

//vfscanf
_LIT(KvfscanfTest1L, "vfscanfTest1L");
_LIT(KvfscanfTest2L, "vfscanfTest2L");
_LIT(KvfscanfTest3L, "vfscanfTest3L");
_LIT(KvfscanfTest4L, "vfscanfTest4L");
_LIT(KvfscanfTest5L, "vfscanfTest5L");
_LIT(KvfscanfTest6L, "vfscanfTest6L");
_LIT(KvfscanfTest7L, "vfscanfTest7L");
_LIT(KvfscanfTest8L, "vfscanfTest8L");
_LIT(KvfscanfTest9L, "vfscanfTest9L");
_LIT(KvfscanfTest10L, "vfscanfTest10L");
_LIT(KvfscanfTest11L, "vfscanfTest11L");
_LIT(KvfscanfTest12L, "vfscanfTest12L");
_LIT(KvfscanfTest13L, "vfscanfTest13L");
_LIT(KvfscanfTest14L, "vfscanfTest14L");

//vsprintf
_LIT(KvsprintfTest1L, "vsprintfTest1L");
_LIT(KvsprintfTest2L, "vsprintfTest2L");
_LIT(KvsprintfTest3L, "vsprintfTest3L");
_LIT(KvsprintfTest4L, "vsprintfTest4L");
_LIT(KvsprintfTest5L, "vsprintfTest5L");
_LIT(KvsprintfTest6L, "vsprintfTest6L");
_LIT(KvsprintfTest7L, "vsprintfTest7L");
_LIT(KvsprintfTest8L, "vsprintfTest8L");
_LIT(KvsprintfTest9L, "vsprintfTest9L");
_LIT(KvsprintfTest10L, "vsprintfTest10L");
_LIT(KvsprintfTest11L, "vsprintfTest11L");
_LIT(KvsprintfTest12L, "vsprintfTest12L");
_LIT(KvsprintfTest13L, "vsprintfTest13L");
_LIT(KvsprintfTest14L, "vsprintfTest14L");
_LIT(KvsprintfTest15L, "vsprintfTest15L");
_LIT(KvsprintfTest16L, "vsprintfTest16L");
_LIT(KvsprintfTest17L, "vsprintfTest17L");
_LIT(KvsprintfTest18L, "vsprintfTest18L");
_LIT(KvsprintfTest19L, "vsprintfTest19L");
_LIT(KvsprintfTest20L, "vsprintfTest20L");
_LIT(KvsprintfTest21L, "vsprintfTest21L");
_LIT(KvsprintfTest22L, "vsprintfTest22L");
_LIT(KvsprintfTest23L, "vsprintfTest23L");
_LIT(KvsprintfTest24L, "vsprintfTest24L");
_LIT(KvsprintfTest25L, "vsprintfTest25L");
_LIT(KvsprintfTest26L, "vsprintfTest26L");
_LIT(KvsprintfTest27L, "vsprintfTest27L");

//vsnprintf
_LIT(KvsnprintfTest1L, "vsnprintfTest1L");
_LIT(KvsnprintfTest2L, "vsnprintfTest2L");
_LIT(KvsnprintfTest3L, "vsnprintfTest3L");
_LIT(KvsnprintfTest4L, "vsnprintfTest4L");
_LIT(KvsnprintfTest5L, "vsnprintfTest5L");
_LIT(KvsnprintfTest6L, "vsnprintfTest6L");
_LIT(KvsnprintfTest7L, "vsnprintfTest7L");
_LIT(KvsnprintfTest8L, "vsnprintfTest8L");
_LIT(KvsnprintfTest9L, "vsnprintfTest9L");
_LIT(KvsnprintfTest10L, "vsnprintfTest10L");
_LIT(KvsnprintfTest11L, "vsnprintfTest11L");
_LIT(KvsnprintfTest12L, "vsnprintfTest12L");
_LIT(KvsnprintfTest13L, "vsnprintfTest13L");
_LIT(KvsnprintfTest14L, "vsnprintfTest14L");
_LIT(KvsnprintfTest15L, "vsnprintfTest15L");
_LIT(KvsnprintfTest16L, "vsnprintfTest16L");
_LIT(KvsnprintfTest17L, "vsnprintfTest17L");
_LIT(KvsnprintfTest18L, "vsnprintfTest18L");
_LIT(KvsnprintfTest19L, "vsnprintfTest19L");
_LIT(KvsnprintfTest20L, "vsnprintfTest20L");
_LIT(KvsnprintfTest21L, "vsnprintfTest21L");
_LIT(KvsnprintfTest22L, "vsnprintfTest22L");
_LIT(KvsnprintfTest23L, "vsnprintfTest23L");
_LIT(KvsnprintfTest24L, "vsnprintfTest24L");
_LIT(KvsnprintfTest25L, "vsnprintfTest25L");
_LIT(KvsnprintfTest26L, "vsnprintfTest26L");
_LIT(KvsnprintfTest27L, "vsnprintfTest27L");

//vfprintf
_LIT(KvfprintfTest1L, "vfprintfTest1L");
_LIT(KvfprintfTest2L, "vfprintfTest2L");
_LIT(KvfprintfTest3L, "vfprintfTest3L");
_LIT(KvfprintfTest4L, "vfprintfTest4L");
_LIT(KvfprintfTest5L, "vfprintfTest5L");
_LIT(KvfprintfTest6L, "vfprintfTest6L");
_LIT(KvfprintfTest7L, "vfprintfTest7L");
_LIT(KvfprintfTest8L, "vfprintfTest8L");
_LIT(KvfprintfTest9L, "vfprintfTest9L");
_LIT(KvfprintfTest10L, "vfprintfTest10L");
_LIT(KvfprintfTest11L, "vfprintfTest11L");
_LIT(KvfprintfTest12L, "vfprintfTest12L");
_LIT(KvfprintfTest13L, "vfprintfTest13L");
_LIT(KvfprintfTest14L, "vfprintfTest14L");
_LIT(KvfprintfTest15L, "vfprintfTest15L");
_LIT(KvfprintfTest16L, "vfprintfTest16L");
_LIT(KvfprintfTest17L, "vfprintfTest17L");
_LIT(KvfprintfTest18L, "vfprintfTest18L");
_LIT(KvfprintfTest19L, "vfprintfTest19L");
_LIT(KvfprintfTest20L, "vfprintfTest20L");
_LIT(KvfprintfTest21L, "vfprintfTest21L");
_LIT(KvfprintfTest22L, "vfprintfTest22L");
_LIT(KvfprintfTest23L, "vfprintfTest23L");
_LIT(KvfprintfTest24L, "vfprintfTest24L");
_LIT(KvfprintfTest25L, "vfprintfTest25L");
_LIT(KvfprintfTest26L, "vfprintfTest26L");
_LIT(KvfprintfTest27L, "vfprintfTest27L");

_LIT(Kfseek_multithreaded, "fseek_multithreaded");
_LIT(Kprintf_multithreaded, "printf_multithreaded");
_LIT(Kfreopen_multithreaded, "freopen_multithreaded");
_LIT(Kputc_multithreaded, "putc_multithreaded");

//
_LIT(KTestRemove3, "TestRemove3");
_LIT(KTestRemove4, "TestRemove4");
_LIT(KTestRemove5, "TestRemove5");
_LIT(KTestRemove6, "TestRemove6");
_LIT(KTestRename2, "TestRename2");
_LIT(KTestRename4, "TestRename4");
_LIT(KTestsprintf1, "Testsprintf1");

////for DEF114259
_LIT(KInvalidDriveTests29, "InvalidDriveTests29");

_LIT(KputcTest, "putcTest");
_LIT(KclrerrTest,"clrerrTest");
_LIT(KgetcTest, "getcTest");
_LIT(KfilenoTest,"filenoTest");
_LIT(KfeofTest, "feofTest");
_LIT(KvasprintfTest1,"vasprintfTest1");
_LIT(KasprintfTest1,"asprintfTest1");
_LIT(Kposix_spawnattr_destroyTest,"posix_spawnattr_destroyTest");
_LIT(KsetgroupsTest,"setgroupsTest");
_LIT(KgetpwentTest,"getpwentTest");
_LIT(KbrkTest,"brkTest");
_LIT(KsigactionTest,"sigactionTest");
_LIT(KwfreopenTest,"wfreopenTest");
_LIT(KwopenTest,"wopenTest");
_LIT(Kfseek_negTest,"fseek_negTest");
_LIT(Klseek_negTest,"lseek_negTest");
_LIT(Kfwrite4, "fwrite4");
_LIT(KftellTest,"ftellTest");
_LIT(Kfreopen_validmode, "freopen_validmode");
_LIT(Kfreopen_invalidmode, "freopen_invalidmode");
// Text-Mode support validation
_LIT(KTestFileModeAccess1, "TestFileModeAccess1");
_LIT(KTestFileModeAccess2, "TestFileModeAccess2");
_LIT(KTestFileModeAccess3, "TestFileModeAccess3");
_LIT(KTestFileModeAccess4, "TestFileModeAccess4");
_LIT(KTestFileModeAccess5, "TestFileModeAccess5");
_LIT(KTestFileModeAccess6, "TestFileModeAccess6");
_LIT(KTestFileModeAccess7, "TestFileModeAccess7");
_LIT(KTestFileGetsPos, "TestFileGetsPos");
_LIT(KTestFileTellReadWrite, "TestFileTellReadWrite");
_LIT(KTestFileTellSeek, "TestFileTellSeek");
_LIT(KTestFileLargeData, "TestFileLargeData");
_LIT(KTestFileReadTell, "TestFileReadTell");
_LIT(KTestFileReadSeekEnd, "TestFileReadSeekEnd");
_LIT(KTestFileWriteSeekEnd, "TestFileWriteSeekEnd");
_LIT(KTestFileFlushWriteSeekEnd, "TestFileFlushWriteSeekEnd");
_LIT(KTestFileFlushWriteSeek, "TestFileFlushWriteSeek");	
_LIT(KTestFileFlushSeekCur, "TestFileFlushSeekCur");
_LIT(KTestFileTellSeekCornerCase1, "TestFileTellSeekCornerCase1");
_LIT(KTestFileTellSeekCornerCase2, "TestFileTellSeekCornerCase2");	
_LIT(KTestFileSeekTellOnRW1, "TestFileSeekTellOnRW1");
_LIT(KTestFileSeekTellOnRW2, "TestFileSeekTellOnRW2");
_LIT(KTestFileSeekTellOnRW3, "TestFileSeekTellOnRW3");	
_LIT(KTestFileSeekTellOnRW4, "TestFileSeekTellOnRW4");
_LIT(KTestFileWriteTextMode, "TestFileWriteTextMode");
_LIT(KTestFileWriteBinaryMode, "TestFileWriteBinaryMode");	

class CTestStdio : public CTestStep
	{
public:
	~CTestStdio(); 
	CTestStdio(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt remove1();
	TInt remove2();
	TInt rename1();
	TInt tempnam1();
	TInt tempnam2();
	TInt fclose1();	
	TInt fflush1();
	TInt fopen1();
	TInt fopen2();
	TInt fopen3();
	TInt setbuf1();
	TInt setbuf2();
	TInt setvbuf1();
	TInt setvbuf2();
	TInt setvbuf3();
	TInt clearerr1();
	TInt feof1();
	TInt ferror1();
	TInt fread1();
	TInt fread2();
	TInt fread3();
	TInt fwrite1();
	TInt fwrite2();
	TInt fwrite3();
	TInt fgetpos1();
	TInt fseek1();
	TInt fseek2();
	TInt fsetpos1();
	TInt fsetpos2();
	TInt ftell1();
	TInt rewind1();
	TInt fgetc1();
	TInt fgets1();
	TInt fgets2();
	TInt fgets3();
	TInt fputc1();	
	TInt fputc2();	
	TInt fputc3();	
	TInt fputc4();
	TInt fputs1();
	TInt fputs2();
	TInt fputs3();
	TInt fputs4();
	TInt getc1();
	TInt putc1();
	TInt putc2();
	TInt putc3();
	TInt putc4();
	TInt ungetc1();	
	TInt fileno1();	
	TInt fileno2();	
	TInt fileno3();		
	TInt getw1();	
	TInt putw1();	
	TInt getdirentries1();
	TInt getdirentries2();
	TInt freopen1();
	TInt printfTest1L();
	TInt printfTest2L();
	TInt printfTest3L();
	TInt printfTest4L();
	TInt printfTest5L();
	TInt printfTest6L();
	TInt printfTest7L();
	TInt printfTest8L();
	TInt printfTest9L();
	TInt printfTest10L();
	TInt printfTest11L();
	TInt printfTest12L();
	TInt printfTest13L();
	TInt printfTest14L();
	TInt printfTest15L();
	TInt printfTest16L();
	TInt printfTest17L();
	TInt printfTest18L();
	TInt printfTest19L();
	TInt printfTest20L();
	TInt printfTest21L();
	TInt printfTest22L();
	TInt printfTest23L();
	TInt printfTest24L();
	TInt printfTest25L();
	TInt printfTest26L();
	TInt printfTest27L();
	
	TInt   fprintfTest1L();
	TInt   fprintfTest2L();
	TInt   fprintfTest3L();
	TInt   fprintfTest4L();
	TInt   fprintfTest5L();
	TInt   fprintfTest6L();
	TInt   fprintfTest7L();
	TInt   fprintfTest8L();
	TInt   fprintfTest9L();
	TInt   fprintfTest10L();
	TInt    fprintfTest11L();
	TInt    fprintfTest12L();
	TInt    fprintfTest13L();
	TInt    fprintfTest14L();
	TInt    fprintfTest15L();
	TInt    fprintfTest16L();
	TInt    fprintfTest17L();
	TInt    fprintfTest18L();
	TInt    fprintfTest19L();
	TInt   fprintfTest20L();
	TInt    fprintfTest21L();
	TInt    fprintfTest22L();
	TInt    fprintfTest23L();
	TInt    fprintfTest24L();
	TInt    fprintfTest25L();
	TInt    fprintfTest26L();
	TInt    fprintfTest27L();

	TInt sprintfTest1L();
	TInt sprintfTest2L();
	TInt sprintfTest3L();
	TInt sprintfTest4L();
	TInt sprintfTest5L();
	TInt sprintfTest6L();
	TInt sprintfTest7L();
	TInt sprintfTest8L();
	TInt sprintfTest9L();
	TInt sprintfTest10L();
	TInt sprintfTest11L();
	TInt sprintfTest12L();
	TInt sprintfTest13L();
	TInt sprintfTest14L();
	TInt sprintfTest15L();
	TInt sprintfTest16L();
	TInt sprintfTest17L();
	TInt sprintfTest18L();
	TInt sprintfTest19L();
	TInt sprintfTest20L();
	TInt sprintfTest21L();
	TInt sprintfTest22L();
	TInt sprintfTest23L();
	TInt sprintfTest24L();
	TInt sprintfTest25L();
	TInt sprintfTest26L();
	TInt sprintfTest27L();
	TInt sprintfTest28L();
	TInt sprintfTest29L();
	TInt sprintfTestInfL();
	TInt sprintfTestNanL();

	TInt snprintfTest1L();
	TInt snprintfTest2L();
	TInt snprintfTest3L();
	TInt snprintfTest4L();
	TInt snprintfTest5L();
	TInt snprintfTest6L();
	TInt snprintfTest7L();
	TInt snprintfTest8L();
	TInt snprintfTest9L();
	TInt snprintfTest10L();
	TInt snprintfTest11L();
	TInt snprintfTest12L();
	TInt snprintfTest13L();
	TInt snprintfTest14L();
	TInt snprintfTest15L();
	TInt snprintfTest16L();
	TInt snprintfTest17L();
	TInt snprintfTest18L();
	TInt snprintfTest19L();
	TInt snprintfTest20L();
	TInt snprintfTest21L();
	TInt snprintfTest22L();
	TInt snprintfTest23L();
	TInt snprintfTest24L();
	TInt snprintfTest25L();
	TInt snprintfTest26L();
	TInt snprintfTest27L();	
	
	TInt vprintfTest1L();
	TInt vprintfTest2L();
	TInt vprintfTest3L();
	TInt vprintfTest4L();
	TInt vprintfTest5L();
	TInt vprintfTest6L();
	TInt vprintfTest7L();
	TInt vprintfTest8L();
	TInt vprintfTest9L();
	TInt vprintfTest10L();
	TInt vprintfTest11L();
	TInt vprintfTest12L();
	TInt vprintfTest13L();
	TInt vprintfTest14L();
	TInt vprintfTest15L();
	TInt vprintfTest16L();
	TInt vprintfTest17L();
	TInt vprintfTest18L();
	TInt vprintfTest19L();
	TInt vprintfTest20L();
	TInt vprintfTest21L();
	TInt vprintfTest22L();
	TInt vprintfTest23L();
	TInt vprintfTest24L();
	TInt vprintfTest25L();
	TInt vprintfTest26L();
	TInt vprintfTest27L();
	
	TInt scanfTest1L();
	TInt scanfTest2L();
	TInt scanfTest3L();
	TInt scanfTest4L();
	TInt scanfTest5L();
	TInt scanfTest6L();
	TInt scanfTest7L();
	TInt scanfTest8L();
	TInt scanfTest9L();
	TInt scanfTest10L();
	TInt scanfTest11L();
	TInt scanfTest12L();
	TInt scanfTest13L();
	
	TInt sscanfTest1L();
	TInt sscanfTest2L();
	TInt sscanfTest3L();
	TInt sscanfTest4L();
	TInt sscanfTest5L();
	TInt sscanfTest6L();
	TInt sscanfTest7L();
	TInt sscanfTest8L();
	TInt sscanfTest9L();
	TInt sscanfTest10L();
	TInt sscanfTest11L();
	TInt sscanfTest12L();
	TInt sscanfTest13L();
	TInt sscanfTest14L();
	
	TInt fscanfTest1L();
	TInt fscanfTest2L();
	TInt fscanfTest3L();
	TInt fscanfTest4L();
	TInt fscanfTest5L();
	TInt fscanfTest6L();
	TInt fscanfTest7L();
	TInt fscanfTest8L();
	TInt fscanfTest9L();
	TInt fscanfTest10L();
	TInt fscanfTest11L();
	TInt fscanfTest12L();
	TInt fscanfTest13L();
	TInt fscanfTest14L();
	
	TInt vscanfTest1L();
	TInt vscanfTest2L();
	TInt vscanfTest3L();
	TInt vscanfTest4L();
	TInt vscanfTest5L();
	TInt vscanfTest6L();
	TInt vscanfTest7L();
	TInt vscanfTest8L();
	TInt vscanfTest9L();
	TInt vscanfTest10L();
	TInt vscanfTest11L();
	TInt vscanfTest12L();
	TInt vscanfTest13L();
	
	TInt vsscanfTest1L();
	TInt vsscanfTest2L();
	TInt vsscanfTest3L();
	TInt vsscanfTest4L();
	TInt vsscanfTest5L();
	TInt vsscanfTest6L();
	TInt vsscanfTest7L();
	TInt vsscanfTest8L();
	TInt vsscanfTest9L();
	TInt vsscanfTest10L();
	TInt vsscanfTest11L();
	TInt vsscanfTest12L();
	TInt vsscanfTest13L();
	TInt vsscanfTest14L();
	
	TInt vfscanfTest1L();
	TInt vfscanfTest2L();
	TInt vfscanfTest3L();
	TInt vfscanfTest4L();
	TInt vfscanfTest5L();
	TInt vfscanfTest6L();
	TInt vfscanfTest7L();
	TInt vfscanfTest8L();
	TInt vfscanfTest9L();
	TInt vfscanfTest10L();
	TInt vfscanfTest11L();
	TInt vfscanfTest12L();
	TInt vfscanfTest13L();
	TInt vfscanfTest14L();
	
	TInt vsprintfTest1L();
	TInt vsprintfTest2L();
	TInt vsprintfTest3L();
	TInt vsprintfTest4L();
	TInt vsprintfTest5L();
	TInt vsprintfTest6L();
	TInt vsprintfTest7L();
	TInt vsprintfTest8L();
	TInt vsprintfTest9L();
	TInt vsprintfTest10L();
	TInt vsprintfTest11L();
	TInt vsprintfTest12L();
	TInt vsprintfTest13L();
	TInt vsprintfTest14L();
	TInt vsprintfTest15L();
	TInt vsprintfTest16L();
	TInt vsprintfTest17L();
	TInt vsprintfTest18L();
	TInt vsprintfTest19L();
	TInt vsprintfTest20L();
	TInt vsprintfTest21L();
	TInt vsprintfTest22L();
	TInt vsprintfTest23L();
	TInt vsprintfTest24L();
	TInt vsprintfTest25L();
	TInt vsprintfTest26L();
	TInt vsprintfTest27L();
	
	TInt vsnprintfTest1L();
	TInt vsnprintfTest2L();
	TInt vsnprintfTest3L();
	TInt vsnprintfTest4L();
	TInt vsnprintfTest5L();
	TInt vsnprintfTest6L();
	TInt vsnprintfTest7L();
	TInt vsnprintfTest8L();
	TInt vsnprintfTest9L();
	TInt vsnprintfTest10L();
	TInt vsnprintfTest11L();
	TInt vsnprintfTest12L();
	TInt vsnprintfTest13L();
	TInt vsnprintfTest14L();
	TInt vsnprintfTest15L();
	TInt vsnprintfTest16L();
	TInt vsnprintfTest17L();
	TInt vsnprintfTest18L();
	TInt vsnprintfTest19L();
	TInt vsnprintfTest20L();
	TInt vsnprintfTest21L();
	TInt vsnprintfTest22L();
	TInt vsnprintfTest23L();
	TInt vsnprintfTest24L();
	TInt vsnprintfTest25L();
	TInt vsnprintfTest26L();
	TInt vsnprintfTest27L();
	
	TInt vfprintfTest1L();
	TInt vfprintfTest2L();
	TInt vfprintfTest3L();
	TInt vfprintfTest4L();
	TInt vfprintfTest5L();
	TInt vfprintfTest6L();
	TInt vfprintfTest7L();
	TInt vfprintfTest8L();
	TInt vfprintfTest9L();
	TInt vfprintfTest10L();
	TInt vfprintfTest11L();
	TInt vfprintfTest12L();
	TInt vfprintfTest13L();
	TInt vfprintfTest14L();
	TInt vfprintfTest15L();
	TInt vfprintfTest16L();
	TInt vfprintfTest17L();
	TInt vfprintfTest18L();
	TInt vfprintfTest19L();
	TInt vfprintfTest20L();
	TInt vfprintfTest21L();
	TInt vfprintfTest22L();
	TInt vfprintfTest23L();
	TInt vfprintfTest24L();
	TInt vfprintfTest25L();
	TInt vfprintfTest26L();
	TInt vfprintfTest27L();
	TInt TestwscanfForStr();
	
	TInt fseek_multithreaded();
	TInt printf_multithreaded();
	TInt freopen_multithreaded();
	TInt putc_multithreaded();
		
	//Locals for vprintf and vscanf
	TInt vprintfTests(const char* fmt,...);
	TInt vfprintfTests(char* filename,const char* fmt,...);
	TInt vsprintfTests(const char* fmt,...);
	TInt vsnprintfTests(const char* fmt,...);
	TInt vscanfTests(const char* fmt,...);
	TInt vsscanfTests(const char* str,const char* fmt,...);
	TInt vfscanfTests (FILE* fp,const char *fmt, ...);
	
	TInt fprintfbasic();
	
	//
	TInt TestRemove3();
	TInt TestRemove4();
	TInt TestRemove5();
	TInt TestRemove6();
	TInt TestRename2();
	TInt TestRename4();
	TInt binaryopen();
	TInt binaryfopen();
	TInt Testsprintf1();
	
	TInt InvalidDriveTests29();
	
	TInt putcTest();
	TInt clrerrTest();
	TInt getcTest();
	TInt filenoTest();
	TInt feofTest();
	TInt vasprintfTest1();
	TInt asprintfTest1();
	TInt vasprintfTests(char **ptr,const char* fmt,...);
	TInt posix_spawnattr_destroyTest();
	TInt setgroupsTest();
	TInt getpwentTest();
	TInt brkTest();
	TInt sigactionTest();
	TInt wfreopenTest();
	TInt wopenTest();
	TInt fseek_negTest();
	TInt lseek_negTest();
	TInt fwrite4();
	TInt ftellTest();
	TVerdict freopen_validmode();	
	TVerdict freopen_invalidmode();	
	
	//File mode operations
	TVerdict TestFileModeAccess1();	
	TVerdict TestFileModeAccess2();	
	TVerdict TestFileModeAccess3();	
	TVerdict TestFileModeAccess4();	
	TVerdict TestFileModeAccess5();	
	TVerdict TestFileModeAccess6();	
	TVerdict TestFileModeAccess7();	
	TVerdict TestFileGetsPos();	
	TVerdict TestFileTellReadWrite();
	TVerdict TestFileTellSeek();	
	TVerdict TestFileLargeData();	
	TVerdict TestFileReadTell();	
	TVerdict TestFileReadSeekEnd();	
	TVerdict TestFileWriteSeekEnd();
	TVerdict TestFileFlushWriteSeekEnd();	
	TVerdict TestFileFlushWriteSeek();	
	TVerdict TestFileFlushSeekCur();
	TVerdict TestFileTellSeekCornerCase1();	
	TVerdict TestFileTellSeekCornerCase2();	
	TVerdict TestFileSeekTellOnRW1();
	TVerdict TestFileSeekTellOnRW2();	
	TVerdict TestFileSeekTellOnRW3();	
	TVerdict TestFileSeekTellOnRW4();
	TVerdict TestFileWriteTextMode();	
	TVerdict TestFileWriteBinaryMode();	
	};
#endif 

