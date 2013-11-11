/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *  Portions Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *	@(#)mman.h	8.2 (Berkeley) 1/9/95
 * $FreeBSD: src/sys/sys/mman.h,v 1.40 2005/04/02 12:33:31 das Exp $
 */

#ifndef _SYS_MMAN_H_
#define _SYS_MMAN_H_
#ifdef __cplusplus
 extern "C" 
 { 
  #endif

#include <sys/cdefs.h>
#include <sys/_types.h>

#if __BSD_VISIBLE
/*
 * Inheritance for minherit()
 */
#define INHERIT_SHARE	0
#define INHERIT_COPY	1
#define INHERIT_NONE	2
#endif

/*
 * Protections are chosen from these bits, or-ed together
 */
#define	PROT_NONE	0x00	/* no permissions */
#define	PROT_READ	0x01	/* pages can be read */
#define	PROT_WRITE	0x02	/* pages can be written */
#define	PROT_EXEC	0x04	/* pages can be executed */

/*
 * Flags contain sharing type and options.
 * Sharing types; choose one.
 */
#define	MAP_SHARED	0x0001		/* share changes */
#define	MAP_PRIVATE	0x0002		/* changes are private */
#if __BSD_VISIBLE
#define	MAP_COPY	MAP_PRIVATE	/* Obsolete */
#endif

/*
 * Other flags
 */
#define	MAP_FIXED	 0x0010	/* map addr must be exactly as requested */

/*
 * Error return from mmap()
 */
#define MAP_FAILED	((void *)-1)

/*
 * msync() flags
 */
#define	MS_SYNC		0x0000	/* msync synchronously */
#define MS_ASYNC	0x0001	/* return immediately */
#define MS_INVALIDATE	0x0002	/* invalidate all cached data */

#ifndef _MODE_T_DECLARED
typedef	__mode_t	mode_t;
#define	_MODE_T_DECLARED
#endif

#ifndef _OFF_T_DECLARED
typedef	__off_t		off_t;
#define	_OFF_T_DECLARED
#endif

#ifndef _SIZE_T_DECLARED
typedef	__size_t	size_t;
#define	_SIZE_T_DECLARED
#endif

#ifndef _KERNEL

__BEGIN_DECLS
/*
 * XXX not yet implemented: posix_mem_offset(), posix_typed_mem_get_info(),
 * posix_typed_mem_open().
 */
#ifndef _MMAP_DECLARED
#define	_MMAP_DECLARED
IMPORT_C void *	mmap(void *, size_t, int, int, int, off_t);

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
#define mmap64	mmap
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */

#endif
IMPORT_C int	mprotect(const void *, size_t, int);
IMPORT_C int	msync(void *, size_t, int);
IMPORT_C int	munmap(void *, size_t);
#if __POSIX_VISIBLE >= 199309
IMPORT_C int	shm_open(const char *, int, mode_t);
IMPORT_C int	shm_unlink(const char *);
#endif
__END_DECLS

#endif /* !_KERNEL */

#ifdef __cplusplus
}
#endif	//__cplusplus

#endif /* !_SYS_MMAN_H_ */
