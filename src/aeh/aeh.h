/* 
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*----------------------------------------------------------------------
 Functions dealing with handling exceptions/assertion failures. Used to
  record and analyze exception/assertion failure info.  For transfer of
  exception/assertion failure info, uses aehlog for file operations and
  dpexcept for network operations.
----------------------------------------------------------------------*/

#ifndef aeh_h
#define aeh_h

#ifdef _WIN32
#include <windows.h>
#else
#include "win4alt.h"
#endif

/* machine-dependent constants */
#define aeh_WIN32_ADDR_LIMIT 0x80000000 /* above this is supervisor space, don't retrace into here */
#define aeh_EXCEPTION_CODE   0xE0000001 /* aeh defined exception code for case where exception is generated by exception code itself */
#ifndef anet_h
#define aeh_ASSERTION_CODE   0xE0000002 /* aeh defined exception code for case where assertion failure occurred */
#endif

#ifdef _WIN32
#define aeh_PATH_DELIMIT '\\'
#elif defined(UNIX)
#define aeh_PATH_DELIMIT '/'
#elif defined(__MWERKS__)
#define aeh_PATH_DELIMIT ':'
#endif

/* private definitions */
#define aeh_BUF_MAXLEN 512 /* max length of single crash info in binary form */

#ifdef MAX_PATH
#define aeh_MAX_PATH MAX_PATH
#else
#define aeh_MAX_PATH 255
#endif

#ifdef DEBUG_PRNT
/* #define aehDPRINT(s) printf s */
#include "logprint.h"
#define aehDPRINT(a) DPRINT(a)
#else
#define aehDPRINT(s)
#endif

/* error codes; correspond to dp error codes */
#define aeh_RES_OK      0
#define aeh_RES_NOMEM   1
#define aeh_RES_EMPTY   2
#define aeh_RES_FULL    3
#define aeh_RES_BAD     5
#define aeh_RES_ALREADY 7
#define aeh_RES_BUG     8 


/***********************************************************************
 ****         mapfile structures and functions                      ****
 ***********************************************************************/

/* single entry in list of module functions gotten from mapfiles */
typedef struct aeh_map_func_s {
	char *name;
	unsigned start_addr;
	struct aeh_map_func_s *next;
} aeh_map_func_t;

/* single entry in list of module mapfiles gotten from mapfile catalog file */
typedef struct {
	unsigned long crc;         /* crc of module corresponding to mapfile */
	unsigned long load_addr;   /* preferred load address for mapfile module */ 
	char *path;                /* path to mapfile */
	char *reldir;              /* relative dir of mapfile; used for printing */
	/* pointer to first entry in linked list of functions for mapfile;
	 * functions are ordered in descending order on function load address */
	aeh_map_func_t *firstfunc;
	/* pointer to last entry in linked list of functions for mapfile */
	aeh_map_func_t *lastfunc;
} aeh_map_t;

/* array of mapfiles gotten from mapfile catalog file */
typedef struct {
	unsigned int nmap;
	aeh_map_t *map;
} aeh_mapcat_t;

/*--------------------------------------------------------------------------
 Get information from the mapfiles in the mapfile catalog file.
 Call aeh_mapcat_Destroy() after finished using the returned value of aehmapcat.
 Input: catpath (path to mapfile catalog file)
 Output: aehmapcat
 Return: aeh_RES_NOMEM if memory couldn't be allocated
         aeh_RES_BUG if catalog file couldn't be opened
         aeh_RES_BAD if couldn't get catalog info
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_mapcat_Create(aeh_mapcat_t *aehmapcat, const char *catpath);

/*--------------------------------------------------------------------------
 Free memory allocated to aeh_mapcat_t during aehmapcat_Create().
 Input: aehmapcat
--------------------------------------------------------------------------*/
void aeh_mapcat_Destroy(aeh_mapcat_t *aehmapcat);



/***********************************************************************
 ****            aeh structures and functions                       ****
 ***********************************************************************/

/* crash application info; see dp_appParam_t */
typedef struct {
	unsigned short sessionType;
	unsigned short platform;
	unsigned short language;
	unsigned short major_version;
	unsigned short minor_version;
} aeh_appParam_t;

/* module (.dll, .exe, etc.) info */
typedef struct {
	unsigned long crc;  /* used to distinguish different versions of modules */
	char *name;
} aeh_modfile_t;

/* call stack at time of crash */
typedef struct {
	unsigned int mod_index;  /* index of module in module array in aeh_t
                                0: unknown/kernel; 1 thru n_mod: array index */
	unsigned long offset_addr; /* address offset by module's base address */
} aeh_stack_entry_t;
#define aeh_MAX_STACK_DEPTH (aeh_BUF_MAXLEN/sizeof(aeh_stack_entry_t))

/* module function info */
typedef struct {
	char *name;                /* name of the module function */
	unsigned long offset_addr; /* address offset by function's base address */
	unsigned long load_addr;   /* preferred load address of module function */
	char *mappath;             /* path to mapfile for this module function */
	/* int lineno; */   /* module function line number; not yet implemented */
} aeh_modfunc_t;

#define aeh_WIN32afl_ID 0x01  /* refers to unsigned char rwFlag */
#define aeh_WIN32info_ID 0x02 /* refers to aeh_WIN32info_t */
#define aeh_WIN32inst_ID 0x03 /* refers to ninst bytes at crash address */
#define aeh_COMMENT_ID 0x04	  /* refers to an additional comment */
#define aeh_NINSTB 16

/* WIN32 register contents and other info at time of crash */
typedef struct {
	/* integer registers */
	unsigned long eax;
	unsigned long ebx;
	unsigned long ecx;
	unsigned long edx;
	unsigned long esi;
	unsigned long edi;
	/* control registers */
	unsigned long ebp;
	unsigned long eip;
	unsigned long esp;
	unsigned long segCs;
	unsigned long segSs;
	/* segment registers */
	unsigned long segDs;
	unsigned long segEs;
	unsigned long segFs;
	unsigned long segGs;
	/* context flags */
	unsigned long contextFlags;
	/* additional info about access violations */
	unsigned long access_addr;
} aeh_WIN32info_t;

/* additional data of free format */
typedef struct {
	unsigned int id;
	void *data;
} aeh_info_t;

/* crash/assertion failure info */
typedef struct {
	unsigned long retcode;  /* exception code */
	unsigned long retflag;  /* exception flags */
	unsigned long retaddr;  /* address where exception/assertion occurred */
	unsigned int assertln;  /* line number where assertion occurred */
	char *assertfile;       /* file where assertion occurred */
	char *asserttxt;		/* text of line where assertion occurred */
	unsigned int nstk;      /* number of elements in stack trace array */
	aeh_stack_entry_t *stk; /* stack trace array */
	unsigned int nmod;      /* number of elements in module array */
	aeh_modfile_t *mod;     /* array of modules involved when crash/assertion */
	char *systemDesc;	         /* description of video, 3D card */
	aeh_appParam_t app;       /* info about crash/assertion application */
	aeh_modfunc_t *modfunc;   /* array of module function info whose elements
                                    correspond to crash stack elements */
	unsigned int ninfo;       /* number of elements in additional info array */
	aeh_info_t *info;         /* array of additional info */
} aeh_t; 

/* crucial crash info in binary form (limit to keep data size small);
 * used when doing file or network operations; values in buf are in portable
 * byte order */
typedef struct {
	unsigned int buflen;   /* size of useful info in buf */
	unsigned char buf[aeh_BUF_MAXLEN]; /* contain crucial elements of aeh_t */
} aeh_buf_t;

/*--------------------------------------------------------------------------
 Get crucial crash info.  Called by an exception handler during a crash.
 Call aeh_Destroy() after finished using the returned value of aeh.
 Input: ep     (exception pointer; can be NULL)
        aehapp (app info gotten for instance by dpReadAnetInf(); can be NULL)
        systemDesc (info on user's system; can be NULL)
		assertln   (line number of assertion failure; can be NULL);
		assertfile (file where assertion failure occurred; can be NULL);
		asserttxt  (text of assertion failure; can be NULL);
 Output: aeh (preallocated)
 Return: aeh_RES_NOMEM if memory couldn't be allocated
         aeh_RES_ALREADY if exception caused by previous call to aeh_Create
         aeh_RES_BAD if couldn't get call stack info
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
#ifdef _WIN32
int aeh_Create(aeh_t *aeh, const LPEXCEPTION_POINTERS ep, const aeh_appParam_t *aehapp, const char *systemDesc, const int *assertln, const char *assertfile, const char *asserttxt);
#else
int aeh_Create(aeh_t *aeh, const void *ep, const aeh_appParam_t *aehapp, const char *systemDesc, const int *assertln, const char *assertfile, const char *asserttxt);
#endif

/*--------------------------------------------------------------------------
 Free memory allocated to aeh during aeh_Create() or aeh_readInputStream().
 Input: aeh
--------------------------------------------------------------------------*/
void aeh_Destroy(aeh_t *aeh);

/*--------------------------------------------------------------------------
 Get a nearly unique 4 byte identifier for this crash, based on the stack
 trace.
 If a typical game has 2^12 bugs, the chance of an id collision is
 2^(2*12 - 1)/2^32 = 1/2^9.
 On error or empty stack, returns 0.
--------------------------------------------------------------------------*/
unsigned long aeh_getSignature(const aeh_t *aeh);

/*--------------------------------------------------------------------------
 Print out an aeh signature to text string.
 Input: signature (from getSignature)
 Output: 9 byte string plus null termination
--------------------------------------------------------------------------*/
void aeh_signature_toString(unsigned long crc, char *buf);

/*--------------------------------------------------------------------------
 Add a comment field to an aeh.
 Returns aeh_RES_OK on success.
--------------------------------------------------------------------------*/
int aeh_addComment(aeh_t *aeh, const char *comment);

#if 0
/*--------------------------------------------------------------------------
 Get the nth comment from aeh.
 Usually there is only one comment and an n=1 call will suffice.
 No more than len characters are written to comment.
 Returns aeh_RES_OK on success,
		 aeh_RES_EMPTY if there is no nth comment.
--------------------------------------------------------------------------*/
int aeh_getComment(aeh_t *aeh, char *comment, int len, int n);

/*--------------------------------------------------------------------------
 Get the name of the last (deepest) known module in the stack trace.
 No more than len characters are written to name.
 Returns aeh_RES_OK on success,
		 aeh_RES_EMPTY if the stack trace contains no known modules.
--------------------------------------------------------------------------*/
int aeh_getLastModule(aeh_t *aeh, char *name, int len);
#endif

/*--------------------------------------------------------------------------
 Convert crash info from an object format (aeh_t) to a binary stream
  format (aeh_buf_t). Call this to put crash info into a compact form 
  for doing network or file operations.
 Input: aeh
 Output: aehbuf (preallocated)
 Return: aeh_RES_BAD if null arguments
         aeh_RES_FULL if info is greater than aehbuf buffer size
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_writeOutputStream(const aeh_t *aeh, aeh_buf_t *aehbuf);

/*--------------------------------------------------------------------------
 Convert crash info from a binary stream format (aeh_buf_t) to an object
  format (aeh_t). Call this to put compact format crash info from network or
  file operations into a easily useable form.
 Call aeh_Destroy() after finished using the returned value of aeh.
 Input: aehbuf
 Output: aeh (preallocated)
 Return: aeh_RES_BAD if null arguments
         aeh_RES_FULL if incomplete data read back (due to full aehbuf buffer)
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_readInputStream(aeh_t *aeh, const aeh_buf_t *aehbuf);

/*--------------------------------------------------------------------------
 Evaluate crash data in aeh along with info in corresponding map files to
  get the module function crash stack.
 Map file paths are stored in a catalog file indexed by module crc and
  selected by matching crc to that of the crash modules.
 Input: aeh (not detailed; like when gotten from aeh_readInputStream())
		aehmapcat (gotten by previous call to aeh_map_Create())
 Output: aeh (detailed -- contain module function crash stack)
 Return: aeh_RES_NOMEM if memory couldn't be allocated
         aeh_RES_EMPTY if module array or stack size is zero
         aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_getAllInfo(aeh_t *aeh, aeh_mapcat_t *aehmapcat);

/*--------------------------------------------------------------------------
 Print out aeh info to text string.
 Input: aeh (can be detailed or not detailed)
        len (length of aehDesc allocated)
 Output: aehDesc
         len (actual length of aehDesc)
 Return: length needed to completely write out info
--------------------------------------------------------------------------*/
int aeh_toString(const aeh_t *aeh, char *aehDesc, unsigned int *len);

/*--------------------------------------------------------------------------
 Strip system description from crash info. Takes aeh and returns it with
  null system description string.
 Input: aeh
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripSysDesc(aeh_t *aeh);

/*--------------------------------------------------------------------------
 Strip selected info id's from crash info. Takes aeh and array and size of
  array of info id's to be removed and returns it without them.
 Input: aeh, nids, stripIds
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripInfoId(aeh_t *aeh, unsigned int nids, unsigned int stripIds[]);

/*--------------------------------------------------------------------------
 Strip address info. Takes aeh and return it with its address field set to zero.
 Input: aeh
 Output: aeh
 Return: aeh_RES_BAD if bad arguments
         aeh_RES_OK on success
--------------------------------------------------------------------------*/
int aeh_stripExceptionAddress(aeh_t *aeh);

#ifdef _WIN32
#define DEBUG_AEHEXCP
#endif
#ifdef DEBUG_AEHEXCP
/*--------------------------------------------------------------------------
 Record current line and file.  Used to help debug aeh-caused exceptions.
--------------------------------------------------------------------------*/
void aeh_SetCurrent(int line, char *file);
/*--------------------------------------------------------------------------
 Get most recently recorded line and file.  Used to help debug aeh-caused
 exceptions.
--------------------------------------------------------------------------*/
void aeh_GetCurrent(int *line, char **file);
#else
#define aeh_SetCurrent(s,t)
#define aeh_GetCurrent(s,t)
#endif

#ifdef AEHMAP_PRIVATE     /* shared by aeh.c and aehmap.c only */

#define BUFFER_SIZE 4096
/* Find map entry corresponding the module in aehmodfile */
int aeh_map_Find(aeh_mapcat_t *aehmapcat, aeh_modfile_t *aehmodfile);

#endif

#endif
