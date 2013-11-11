/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 * Portions copyright (c) 2006-2009 Nokia Corporation.  All rights reserved.
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* 
 * MT safe
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifdef __SYMBIAN32__
#include <errno.h>
#endif /* __SYMBIAN32__ */

#include "glib.h"
#include "gthreadprivate.h"
#include "galias.h"

#define MEM_PROFILE_TABLE_SIZE 4096
#ifdef __SYMBIAN32__
#include "glib_wsd.h"
#endif /* __SYMBIAN32__ */

#if EMULATOR
#define g_thread_functions_for_glib_use (*_g_thread_functions_for_glib_use())
#define g_thread_use_default_impl (*_g_thread_use_default_impl())
#endif /* EMULATOR */

#ifdef MOBILE_PORT
#include <glowmem.h>
#endif /* MOBILE_PORT */

/* notes on macros:
 * having G_DISABLE_CHECKS defined disables use of glib_mem_profiler_table and
 * g_mem_profile().
 * REALLOC_0_WORKS is defined if g_realloc (NULL, x) works.
 * SANE_MALLOC_PROTOS is defined if the systems malloc() and friends functions
 * match the corresponding GLib prototypes, keep configure.in and gmem.h in sync here.
 * g_mem_gc_friendly is TRUE, freed memory should be 0-wiped.
 */

/* --- prototypes --- */
#if (EMULATOR)

PLS(g_mem_initialized ,gmem,gboolean)
#define g_mem_initialized  (*FUNCTION_NAME(g_mem_initialized ,gmem)())

#else

static gboolean g_mem_initialized = FALSE;

#endif /* EMULATOR */

static void     g_mem_init_nomessage (void);


/* --- malloc wrappers --- */
#ifndef	REALLOC_0_WORKS
static gpointer
standard_realloc (gpointer mem,
		  gsize    n_bytes)
{
  if (!mem)
    return malloc (n_bytes);
  else
    return realloc (mem, n_bytes);
}
#endif	/* !REALLOC_0_WORKS */

#ifdef SANE_MALLOC_PROTOS
#  define standard_malloc	malloc
#  ifdef REALLOC_0_WORKS
#    define standard_realloc	realloc
#  endif /* REALLOC_0_WORKS */
#  define standard_free		free
#  define standard_calloc	calloc
#  define standard_try_malloc	malloc
#  define standard_try_realloc	realloc
#else	/* !SANE_MALLOC_PROTOS */
static gpointer
standard_malloc (gsize n_bytes)
{
  return malloc (n_bytes);
}
#  ifdef REALLOC_0_WORKS
static gpointer
standard_realloc (gpointer mem,
		  gsize    n_bytes)
{
  return realloc (mem, n_bytes);
}
#  endif /* REALLOC_0_WORKS */
static void
standard_free (gpointer mem)
{
  free (mem);
}
static gpointer
standard_calloc (gsize n_blocks,
		 gsize n_bytes)
{
  return calloc (n_blocks, n_bytes);
}
#define	standard_try_malloc	standard_malloc
#define	standard_try_realloc	standard_realloc
#endif	/* !SANE_MALLOC_PROTOS */


/* --- variables --- */
#if (EMULATOR)

PLS(glib_mem_vtable,gmem,GMemVTable)
#define glib_mem_vtable (*FUNCTION_NAME(glib_mem_vtable,gmem)())
#else
static GMemVTable glib_mem_vtable = {
  NULL,//standard_malloc,
  NULL,//standard_realloc,
  NULL,//standard_free,
  NULL,//standard_calloc,
  NULL,//standard_try_malloc,
  NULL,//standard_try_realloc,
};
#endif /* EMULATOR */
#ifdef __SYMBIAN32__
const GMemVTable glib_mem_vtable_temp = {
  standard_malloc,
  standard_realloc,
  standard_free,
  standard_calloc,
  standard_try_malloc,
  standard_try_realloc,
};
#endif//__SYMBIAN32__

/* --- functions --- */
EXPORT_C gpointer
g_malloc (gsize n_bytes)
{
#ifdef MOBILE_PORT

  	return g_try_malloc(n_bytes);

#else

  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (n_bytes))
    {
      gpointer mem;

      mem = glib_mem_vtable.malloc (n_bytes);
      if (mem)
	return mem;

      g_error ("%s: failed to allocate %"G_GSIZE_FORMAT" bytes",
               G_STRLOC, n_bytes);
    }

  return NULL;
  
#endif /* MOBILE_PORT*/
}

EXPORT_C gpointer
g_malloc0 (gsize n_bytes)
{
  #ifdef MOBILE_PORT
  	return g_try_malloc0(n_bytes);
  #else
  
  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (n_bytes))
    {
      gpointer mem;

      mem = glib_mem_vtable.calloc (1, n_bytes);
      if (mem)
	return mem;

      g_error ("%s: failed to allocate %"G_GSIZE_FORMAT" bytes",
               G_STRLOC, n_bytes);
    }

  return NULL;
  
  #endif  /* MOBILE_PORT*/
}

EXPORT_C gpointer
g_realloc (gpointer mem,
	   gsize    n_bytes)
{
  #ifdef MOBILE_PORT
  
  	return g_try_realloc(mem,n_bytes);
  
  #else 
  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (n_bytes))
    {
      mem = glib_mem_vtable.realloc (mem, n_bytes);
      if (mem)
	return mem;

      g_error ("%s: failed to allocate %"G_GSIZE_FORMAT" bytes",
               G_STRLOC, n_bytes);
    }

  if (mem)
    glib_mem_vtable.free (mem);

  return NULL;
  #endif  /* MOBILE_PORT*/
}

EXPORT_C void
g_free (gpointer mem)
{
  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (mem))
  {
#ifdef MOBILE_PORT
  {
  	mem_info * m = _get_thread_specific_data();
  	if(m && m->is_setjmp_called)
  		_findAndDestroy(&(m->stack),mem);
#endif /* MOBILE_PORT */
    glib_mem_vtable.free (mem);
#ifdef MOBILE_PORT
  }
#endif /* MOBILE_PORT */   
	}
}
EXPORT_C gpointer
g_try_malloc (gsize n_bytes)
{
#ifdef MOBILE_PORT
  gpointer ptr;
#endif /* MOBILE_PORT */
  
  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (n_bytes))
#ifdef MOBILE_PORT
  {
	errno = 0;

  		ptr = glib_mem_vtable.try_malloc (n_bytes);
  	if(ENOMEM == errno)
  	{
  		mem_info *m = _get_thread_specific_data();
  		if(!m)
  			return NULL;
  		
  		if(TRUE == m->is_setjmp_called)
  		{
  			longjmp(m->buf,1);
  		}
  	}
  	else
  	{
  		mem_info *m = _get_thread_specific_data();
  		
  		if(m && m->is_setjmp_called)
  		{
  			if(_push(&(m->stack),ptr))
			{	
				free(ptr);
				longjmp(m->buf,1);
			}	
  		}
  	}
  	return ptr;
#else
	{
    return glib_mem_vtable.try_malloc (n_bytes);
  else
	}
#endif /* MOBILE_PORT */
#ifdef MOBILE_PORT
  }
#endif /* MOBILE_PORT */
  else
    return NULL;
}

EXPORT_C gpointer
g_try_malloc0 (gsize n_bytes)
{ 
  gpointer mem;

  mem = g_try_malloc (n_bytes);
  
  if (mem)
    memset (mem, 0, n_bytes);

  return mem;
}

EXPORT_C gpointer
g_try_realloc (gpointer mem,
	       gsize    n_bytes)
{
  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();
  if (G_LIKELY (n_bytes))
    return glib_mem_vtable.try_realloc (mem, n_bytes);

  if (mem)
    glib_mem_vtable.free (mem);

  return NULL;
}

static gpointer
fallback_calloc (gsize n_blocks,
		 gsize n_block_bytes)
{
  gsize l = n_blocks * n_block_bytes;
  gpointer mem = glib_mem_vtable.malloc (l);

  if (mem)
    memset (mem, 0, l);

  return mem;
}

#if (EMULATOR)

PLS(vtable_set ,gmem,gboolean )
#define vtable_set  (*FUNCTION_NAME(vtable_set ,gmem)())

#else

static gboolean vtable_set = FALSE;

#endif /* EMULATOR */

/**
 * g_mem_is_system_malloc
 * 
 * Checks whether the allocator used by g_malloc() is the system's
 * malloc implementation. If it returns %TRUE memory allocated with
 * malloc() can be used interchangeable with memory allocated using g_malloc(). 
 * This function is useful for avoiding an extra copy of allocated memory returned
 * by a non-GLib-based API.
 *
 * A different allocator can be set using g_mem_set_vtable().
 *
 * Return value: if %TRUE, malloc() and g_malloc() can be mixed.
 **/
EXPORT_C gboolean
g_mem_is_system_malloc (void)
{
  return !vtable_set;
}

EXPORT_C void
g_mem_set_vtable (GMemVTable *vtable)
{
  if (!vtable_set)
    {
      if (vtable->malloc && vtable->realloc && vtable->free)
	{
	  glib_mem_vtable.malloc = vtable->malloc;
	  glib_mem_vtable.realloc = vtable->realloc;
	  glib_mem_vtable.free = vtable->free;
	  glib_mem_vtable.calloc = vtable->calloc ? vtable->calloc : fallback_calloc;
	  glib_mem_vtable.try_malloc = vtable->try_malloc ? vtable->try_malloc : glib_mem_vtable.malloc;
	  glib_mem_vtable.try_realloc = vtable->try_realloc ? vtable->try_realloc : glib_mem_vtable.realloc;
	  vtable_set = TRUE;
	}
      else
	g_warning (G_STRLOC ": memory allocation vtable lacks one of malloc(), realloc() or free()");
    }
  else
    g_warning (G_STRLOC ": memory allocation vtable can only be set once at startup");
}


/* --- memory profiling and checking --- */
#if (EMULATOR)

PLS(glib_mem_profiler_table,gmem,GMemVTable *)
#define glib_mem_profiler_table  (*FUNCTION_NAME(glib_mem_profiler_table,gmem)())

#endif /* EMULATOR */

#ifdef	G_DISABLE_CHECKS
#if !(EMULATOR)
GMemVTable *glib_mem_profiler_table = &glib_mem_vtable;
#endif /* EMULATOR */

EXPORT_C void
g_mem_profile (void)
{
}
#else	/* !G_DISABLE_CHECKS */
typedef enum {
  PROFILER_FREE		= 0,
  PROFILER_ALLOC	= 1,
  PROFILER_RELOC	= 2,
  PROFILER_ZINIT	= 4
} ProfilerJob;

#if (EMULATOR)

PLS(profile_data ,gmem,guint *)
PLS(profile_allocs  ,gmem,gulong)
PLS(profile_zinit ,gmem,gulong)
PLS(profile_frees  ,gmem,gulong)
PLS(gmem_profile_mutex  ,gmem,GMutex *)

#define profile_data (*FUNCTION_NAME(profile_data ,gmem)())
#define profile_allocs (*FUNCTION_NAME(profile_allocs,gmem)())
#define profile_zinit (*FUNCTION_NAME(profile_zinit,gmem)())
#define profile_frees (*FUNCTION_NAME(profile_frees,gmem)())
#define gmem_profile_mutex (*FUNCTION_NAME(gmem_profile_mutex,gmem)())

#else

static guint *profile_data = NULL;
static gsize profile_allocs = 0;
static gsize profile_zinit = 0;
static gsize profile_frees = 0;
static GMutex *gmem_profile_mutex = NULL;

#endif /* EMULATOR */

#ifdef  G_ENABLE_DEBUG
#if (EMULATOR)
PLS(g_trap_free_size ,gmem,gsize)
PLS(g_trap_realloc_size  ,gmem,gsize)
PLS(g_trap_malloc_size ,gmem,gsize)

#define g_trap_free_size (*FUNCTION_NAME(g_trap_free_size ,gmem)())
#define g_trap_realloc_size (*FUNCTION_NAME(g_trap_realloc_size,gmem)())
#define g_trap_malloc_size (*FUNCTION_NAME(g_trap_malloc_size,gmem)())
#else
static volatile gsize g_trap_free_size = 0;
static volatile gsize g_trap_realloc_size = 0;
static volatile gsize g_trap_malloc_size = 0;
#endif//EMULATOR
#endif  /* G_ENABLE_DEBUG */

#define	PROFILE_TABLE(f1,f2,f3)   ( ( ((f3) << 2) | ((f2) << 1) | (f1) ) * (MEM_PROFILE_TABLE_SIZE + 1))

static void
profiler_log (ProfilerJob job,
	      gsize       n_bytes,
	      gboolean    success)
{
  g_mutex_lock (gmem_profile_mutex);
  if (!profile_data)
    {
      profile_data = standard_calloc ((MEM_PROFILE_TABLE_SIZE + 1) * 8, 
                                      sizeof (profile_data[0]));
      if (!profile_data)	/* memory system kiddin' me, eh? */
	{
	  g_mutex_unlock (gmem_profile_mutex);
	  return;
	}
    }

  if (n_bytes < MEM_PROFILE_TABLE_SIZE)
    profile_data[n_bytes + PROFILE_TABLE ((job & PROFILER_ALLOC) != 0,
                                          (job & PROFILER_RELOC) != 0,
                                          success != 0)] += 1;
  else
    profile_data[MEM_PROFILE_TABLE_SIZE + PROFILE_TABLE ((job & PROFILER_ALLOC) != 0,
                                                         (job & PROFILER_RELOC) != 0,
                                                         success != 0)] += 1;
  if (success)
    {
      if (job & PROFILER_ALLOC)
        {
          profile_allocs += n_bytes;
          if (job & PROFILER_ZINIT)
            profile_zinit += n_bytes;
        }
      else
        profile_frees += n_bytes;
    }
  g_mutex_unlock (gmem_profile_mutex);
}

static void
profile_print_locked (guint   *local_data,
		      gboolean success)
{
  gboolean need_header = TRUE;
  guint i;

  for (i = 0; i <= MEM_PROFILE_TABLE_SIZE; i++)
    {
      glong t_malloc = local_data[i + PROFILE_TABLE (1, 0, success)];
      glong t_realloc = local_data[i + PROFILE_TABLE (1, 1, success)];
      glong t_free = local_data[i + PROFILE_TABLE (0, 0, success)];
      glong t_refree = local_data[i + PROFILE_TABLE (0, 1, success)];
      
      if (!t_malloc && !t_realloc && !t_free && !t_refree)
	continue;
      else if (need_header)
	{
	  need_header = FALSE;
	  g_print (" blocks of | allocated  | freed      | allocated  | freed      | n_bytes   \n");
	  g_print ("  n_bytes  | n_times by | n_times by | n_times by | n_times by | remaining \n");
	  g_print ("           | malloc()   | free()     | realloc()  | realloc()  |           \n");
	  g_print ("===========|============|============|============|============|===========\n");
	}
      if (i < MEM_PROFILE_TABLE_SIZE)
	g_print ("%10u | %10ld | %10ld | %10ld | %10ld |%+11ld\n",
		 i, t_malloc, t_free, t_realloc, t_refree,
		 (t_malloc - t_free + t_realloc - t_refree) * i);
      else if (i >= MEM_PROFILE_TABLE_SIZE)
	g_print ("   >%6u | %10ld | %10ld | %10ld | %10ld |        ***\n",
		 i, t_malloc, t_free, t_realloc, t_refree);
    }
  if (need_header)
    g_print (" --- none ---\n");
}

EXPORT_C void
g_mem_profile (void)
{
  guint local_data[(MEM_PROFILE_TABLE_SIZE + 1) * 8 * sizeof (profile_data[0])];
  gsize local_allocs;
  gsize local_zinit;
  gsize local_frees;

  if (G_UNLIKELY (!g_mem_initialized))
    g_mem_init_nomessage();

  g_mutex_lock (gmem_profile_mutex);

  local_allocs = profile_allocs;
  local_zinit = profile_zinit;
  local_frees = profile_frees;

  if (!profile_data)
    {
      g_mutex_unlock (gmem_profile_mutex);
      return;
    }

  memcpy (local_data, profile_data, 
	  (MEM_PROFILE_TABLE_SIZE + 1) * 8 * sizeof (profile_data[0]));
  
  g_mutex_unlock (gmem_profile_mutex);

  g_print ("GLib Memory statistics (successful operations):\n");
  profile_print_locked (local_data, TRUE);
  g_print ("GLib Memory statistics (failing operations):\n");
  profile_print_locked (local_data, FALSE);
  g_print ("Total bytes: allocated=%"G_GSIZE_FORMAT", "
           "zero-initialized=%"G_GSIZE_FORMAT" (%.2f%%), "
           "freed=%"G_GSIZE_FORMAT" (%.2f%%), "
           "remaining=%"G_GSIZE_FORMAT"\n",
	   local_allocs,
	   local_zinit,
	   ((gdouble) local_zinit) / local_allocs * 100.0,
	   local_frees,
	   ((gdouble) local_frees) / local_allocs * 100.0,
	   local_allocs - local_frees);
}

static gpointer
profiler_try_malloc (gsize n_bytes)
{
  gsize *p;

#ifdef  G_ENABLE_DEBUG
  if (g_trap_malloc_size == n_bytes)
    G_BREAKPOINT ();
#endif  /* G_ENABLE_DEBUG */

  p = standard_malloc (sizeof (gsize) * 2 + n_bytes);

  if (p)
    {
      p[0] = 0;		/* free count */
      p[1] = n_bytes;	/* length */
      profiler_log (PROFILER_ALLOC, n_bytes, TRUE);
      p += 2;
    }
  else
    profiler_log (PROFILER_ALLOC, n_bytes, FALSE);
  
  return p;
}

static gpointer
profiler_malloc (gsize n_bytes)
{
  gpointer mem = profiler_try_malloc (n_bytes);

  if (!mem)
    g_mem_profile ();

  return mem;
}

static gpointer
profiler_calloc (gsize n_blocks,
		 gsize n_block_bytes)
{
  gsize l = n_blocks * n_block_bytes;
  gsize *p;

#ifdef  G_ENABLE_DEBUG
  if (g_trap_malloc_size == l)
    G_BREAKPOINT ();
#endif  /* G_ENABLE_DEBUG */
  
  p = standard_calloc (1, sizeof (gsize) * 2 + l);

  if (p)
    {
      p[0] = 0;		/* free count */
      p[1] = l;		/* length */
      profiler_log (PROFILER_ALLOC | PROFILER_ZINIT, l, TRUE);
      p += 2;
    }
  else
    {
      profiler_log (PROFILER_ALLOC | PROFILER_ZINIT, l, FALSE);
      g_mem_profile ();
    }

  return p;
}

static void
profiler_free (gpointer mem)
{
  gsize *p = mem;

  p -= 2;
  if (p[0])	/* free count */
    {
      g_warning ("free(%p): memory has been freed %"G_GSIZE_FORMAT" times already",
                 p + 2, p[0]);
      profiler_log (PROFILER_FREE,
		    p[1],	/* length */
		    FALSE);
    }
  else
    {
#ifdef  G_ENABLE_DEBUG
      if (g_trap_free_size == p[1])
	G_BREAKPOINT ();
#endif  /* G_ENABLE_DEBUG */

      profiler_log (PROFILER_FREE,
		    p[1],	/* length */
		    TRUE);
      memset (p + 2, 0xaa, p[1]);

      /* for all those that miss standard_free (p); in this place, yes,
       * we do leak all memory when profiling, and that is intentional
       * to catch double frees. patch submissions are futile.
       */
    }
  p[0] += 1;
}

static gpointer
profiler_try_realloc (gpointer mem,
		      gsize    n_bytes)
{
  gsize *p = mem;

  p -= 2;

#ifdef  G_ENABLE_DEBUG
  if (g_trap_realloc_size == n_bytes)
    G_BREAKPOINT ();
#endif  /* G_ENABLE_DEBUG */
  
  if (mem && p[0])	/* free count */
    {
      g_warning ("realloc(%p, %"G_GSIZE_FORMAT"): "
                 "memory has been freed %"G_GSIZE_FORMAT" times already",
                 p + 2, (gsize) n_bytes, p[0]);
      profiler_log (PROFILER_ALLOC | PROFILER_RELOC, n_bytes, FALSE);

      return NULL;
    }
  else
    {
      p = standard_realloc (mem ? p : NULL, sizeof (gsize) * 2 + n_bytes);

      if (p)
	{
	  if (mem)
	    profiler_log (PROFILER_FREE | PROFILER_RELOC, p[1], TRUE);
	  p[0] = 0;
	  p[1] = n_bytes;
	  profiler_log (PROFILER_ALLOC | PROFILER_RELOC, p[1], TRUE);
	  p += 2;
	}
      else
	profiler_log (PROFILER_ALLOC | PROFILER_RELOC, n_bytes, FALSE);

      return p;
    }
}

static gpointer
profiler_realloc (gpointer mem,
		  gsize    n_bytes)
{
  mem = profiler_try_realloc (mem, n_bytes);

  if (!mem)
    g_mem_profile ();

  return mem;
}

#if EMULATOR

PLS(profiler_table,gmem,GMemVTable)
#define profiler_table (*FUNCTION_NAME(profiler_table,gmem)())

const GMemVTable temp_profiler_table = {
  profiler_malloc,
  profiler_realloc,
  profiler_free,
  profiler_calloc,
  profiler_try_malloc,
  profiler_try_realloc,
};


#else

static GMemVTable profiler_table = {
  profiler_malloc,
  profiler_realloc,
  profiler_free,
  profiler_calloc,
  profiler_try_malloc,
  profiler_try_realloc,
};

#endif /* EMULATOR */
#if !(EMULATOR)
GMemVTable *glib_mem_profiler_table = &profiler_table;
#endif /* EMULATOR */

#ifdef __SYMBIAN32__
EXPORT_C GMemVTable ** _glib_mem_profiler_table(void)
{
	return &glib_mem_profiler_table;
}
#endif /* __SYMBIAN32__ */

#endif	/* !G_DISABLE_CHECKS */

/* --- MemChunks --- */
#ifndef G_ALLOC_AND_FREE
typedef struct _GAllocator GAllocator;
typedef struct _GMemChunk  GMemChunk;
#define G_ALLOC_ONLY	  1
#define G_ALLOC_AND_FREE  2
#endif

struct _GMemChunk {
  guint alloc_size;           /* the size of an atom */
};

EXPORT_C GMemChunk*
g_mem_chunk_new (const gchar  *name,
		 gint          atom_size,
		 gsize         area_size,
		 gint          type)
{
  GMemChunk *mem_chunk;
  g_return_val_if_fail (atom_size > 0, NULL);

  mem_chunk = g_slice_new (GMemChunk);
  mem_chunk->alloc_size = atom_size;
  return mem_chunk;
}

EXPORT_C void
g_mem_chunk_destroy (GMemChunk *mem_chunk)
{
  g_return_if_fail (mem_chunk != NULL);
  
  g_slice_free (GMemChunk, mem_chunk);
}

EXPORT_C gpointer
g_mem_chunk_alloc (GMemChunk *mem_chunk)
{
  g_return_val_if_fail (mem_chunk != NULL, NULL);
  
  return g_slice_alloc (mem_chunk->alloc_size);
}

EXPORT_C gpointer
g_mem_chunk_alloc0 (GMemChunk *mem_chunk)
{
  g_return_val_if_fail (mem_chunk != NULL, NULL);
  
  return g_slice_alloc0 (mem_chunk->alloc_size);
}

EXPORT_C void
g_mem_chunk_free (GMemChunk *mem_chunk,
		  gpointer   mem)
{
  g_return_if_fail (mem_chunk != NULL);
  
  g_slice_free1 (mem_chunk->alloc_size, mem);
}

EXPORT_C void	g_mem_chunk_clean	(GMemChunk *mem_chunk)	{}
EXPORT_C void	g_mem_chunk_reset	(GMemChunk *mem_chunk)	{}
EXPORT_C void	g_mem_chunk_print	(GMemChunk *mem_chunk)	{}
EXPORT_C void	g_mem_chunk_info	(void)			{}
EXPORT_C void	g_blow_chunks		(void)			{}

#if EMULATOR

PLS(dummy ,g_allocator_new,struct _GAllocator)
#define dummy  (*FUNCTION_NAME(dummy ,g_allocator_new)())

#endif /* EMULATOR */

EXPORT_C GAllocator*
g_allocator_new (const gchar *name,
		 guint        n_preallocs)
{
  #if !(EMULATOR)
  static struct _GAllocator {
    gchar      *name;
    guint16     n_preallocs;
    guint       is_unused : 1;
    guint       type : 4;
    GAllocator *last;
    GMemChunk  *mem_chunk;
    gpointer    free_list;
  } dummy = {
    "GAllocator is deprecated", 1, TRUE, 0, NULL, NULL, NULL,
  };
  #endif /* !(EMULATOR)
  /* some (broken) GAllocator uses depend on non-NULL allocators */
  return (void*) &dummy;
}

#if EMULATOR
#undef dummy
#endif /* EMULATOR */

EXPORT_C void
g_allocator_free (GAllocator *allocator)
{
}

#if EMULATOR

PLS(g_mem_gc_friendly ,gmem,gboolean)
#define g_mem_gc_friendly  (*FUNCTION_NAME(g_mem_gc_friendly ,gmem)())

#else

#ifdef ENABLE_GC_FRIENDLY_DEFAULT
gboolean g_mem_gc_friendly = TRUE;
#else
gboolean g_mem_gc_friendly = FALSE;
#endif

#endif /* EMULATOR */

#ifdef __SYMBIAN32__
EXPORT_C gboolean * _g_mem_gc_friendly(void)
{
	return &g_mem_gc_friendly;
}
#endif /* __SYMBIAN32__ */
static void
g_mem_init_nomessage (void)
{
  gchar buffer[1024];
  const gchar *val;
  const GDebugKey keys[] = {
    { "gc-friendly", 1 },
  };
  gint flags;
  #ifdef __SYMBIAN32__
  memcpy(&glib_mem_vtable, &glib_mem_vtable_temp, sizeof(GMemVTable));
  #endif//__SYMBIAN32__
  if (g_mem_initialized)
    return;
  /* don't use g_malloc/g_message here */
  val = _g_getenv_nomalloc ("G_DEBUG", buffer);
  flags = !val ? 0 : g_parse_debug_string (val, keys, G_N_ELEMENTS (keys));
  if (flags & 1)        /* gc-friendly */
    {
      g_mem_gc_friendly = TRUE;
    }
  g_mem_initialized = TRUE;
}

void
_g_mem_thread_init_noprivate_nomessage (void)
{
  /* we may only create mutexes here, locking/
   * unlocking a mutex does not yet work.
   */
  g_mem_init_nomessage();
#ifndef G_DISABLE_CHECKS
  gmem_profile_mutex = g_mutex_new ();
#endif
}

#define __G_MEM_C__
#include "galiasdef.c"
