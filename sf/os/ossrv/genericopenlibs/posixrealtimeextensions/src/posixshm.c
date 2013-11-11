/*
 * Copyright 2000 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * Portions Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 */

#include <sys/cdefs.h>
#ifndef __SYMBIAN32__
__FBSDID("$FreeBSD: src/lib/libc/gen/posixshm.c,v 1.4 2002/02/01 00:57:29 obrien Exp $");
#endif

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#ifdef __SYMBIAN32__
#include <string.h>
#include <sys/syslimits.h>
#endif//__SYMBIAN32__

EXPORT_C int
shm_open(const char *path, int flags, mode_t mode)
	{
#ifndef SYMBIAN_OE_LIBRT
	path = path;
	flags = flags;
	mode = mode;
	errno = ENOSYS;
	return -1;
#else//SYMBIAN_OE_LIBRT
	int fd;
	struct stat stab;
#ifdef __SYMBIAN32__
	if (!path)
		{
		errno = EFAULT;
		return -1;
		}
	if (strlen(path) == 0)
		{
		errno = EINVAL;
		return -1;
		}
	if (strlen(path) >= PATH_MAX)
		{
		errno = ENAMETOOLONG;
		return -1;
		}
	while (*path == '/')
		{
		path++;
		}
#endif//__SYMBIAN32__
	if ((flags & O_ACCMODE) == O_WRONLY)
		{
		errno = EINVAL;
		return -1;
		}		
	fd = open(path, flags|O_SHMFLG, mode);
	if (fd != -1) 
		{
		if (fstat(fd, &stab) != 0 || !S_ISREG(stab.st_mode)) 
			{
			close(fd);
			errno = EINVAL;
			return (-1);
			}

		if (fcntl(fd, F_SETFL, (int)FPOSIXSHM) != 0) 
			{
			close(fd);
			return (-1);
			}
		}
		return (fd);
#endif//SYMBIAN_OE_LIBRT
	}

EXPORT_C int
shm_unlink(const char *path)
	{
#ifndef SYMBIAN_OE_LIBRT
	path = path;
	errno = ENOSYS;
	return -1;
#else//SYMBIAN_OE_LIBRT
#ifdef __SYMBIAN32__
	if (!path)
		{
		errno = EFAULT;
		return -1;
		}
	if (strlen(path) == 0)
		{
		errno = ENOENT;
		return -1;
		}
	if (strlen(path) >= PATH_MAX)
		{
		errno = ENAMETOOLONG;
		return -1;
		}
#endif//__SYMBIAN32__
	return (unlink(path));
#endif//SYMBIAN_OE_LIBRT
	}
