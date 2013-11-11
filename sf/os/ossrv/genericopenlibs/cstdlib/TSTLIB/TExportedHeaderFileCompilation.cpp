// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This dummy .cpp file only includes all header files exported by stdlib to 
// test the compilation error/warnings on various platforms.
// 
//

// STDLIB exported header files
#include <estlib.h>
#include <estw32.h>
#include <redircli.h>
#include <redirstr.h>

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
// Including order of 'math.h' and 'ieeefp.h' here need to be considered.
//  'math.h' has to be prior to 'ieeefp.h' here to keep the C linkage of a set 
//  of functions which were only specified in 'math.h' but declared in both 
//  files. Otherwise, errors of incompatibility of linkage specification will be
//  generated. Or if C compiler is used, there will be no error at all.
#include <math.h>
#include <ieeefp.h>

#include <netdb.h>
#include <netdb_r.h>
#include <process.h>
#include <pwd.h>
#include <reent.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdarg_e.h>
#include <stddef.h>
#include <stdio.h>
#include <stdio_r.h>
#include <stdlib.h>
#include <stdlib_r.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <_ansi.h>

#include <sys/config.h>
#include <sys/dirent.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/reent.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/stdio_t.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/_types.h>
#include <sys/serial.h>

#include <libc/machine/ieeefp.h>
#include <libc/machine/types.h>

// 'net_types.h' provides missing type definitions required by other header files in
//  'netinet' directory
#include <libc/netinet/net_types.h>
#include <libc/netinet/arp.h>
#include <libc/netinet/in.h>
// A workaround to remove redefinition warning message on TCP_NODELAY when
//  both 'in.h' and 'tcp.h' were included
#undef TCP_NODELAY
#include <libc/netinet/ip.h>
#include <libc/netinet/ip_icmp.h>
#include <libc/netinet/tcp.h>
#include <libc/netinet/tcp_fsm.h>
#include <libc/netinet/tcp_seq.h>
#include <libc/netinet/udp.h>

#include <libc/arpa/ftp.h>
// A workaround to remove redefinition warning message on ERROR when both
//  'ftp.h' and 'tftp.h' were included
#undef ERROR
#include <libc/arpa/inet.h>
#include <libc/arpa/nameser.h>
#include <libc/arpa/telnet.h>
#include <libc/arpa/tftp.h>

