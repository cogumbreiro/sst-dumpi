/*
 *  This file is part of DUMPI: 
 *               The MPI profiling library from the SST suite.
 *  Copyright (c) 2009 Sandia Corporation.
 *  This software is distributed under the BSD License.
 *  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 *  the U.S. Government retains certain rights in this software.
 *  For more information, see the LICENSE file in the top 
 *  SST/macroscale directory.
 */

#ifndef DUMPI_BIN_DUMPI2ASCII_DEFS_H
#define DUMPI_BIN_DUMPI2ASCII_DEFS_H

#include <stdio.h>
#include <dumpi/common/perfctrtags.h>

#ifdef __cplusplus
extern "C" {
#endif /* ! __cplusplus */

  /**
   * \defgroup dumpi2ascii dumpi2ascii:  Convert binary DUMPI trace files
   *     to human-readable ASCII files.
   */

  /**
   * \ingroup dumpi2ascii
   * \defgroup dumpi2ascii_defs dumpi2ascii/defs:  Utility functions and \
   *    preprocessor definitions for reporting the contents of an MPI trace.
   */
  /*@{*/

  /**
   * The filehandle to which dumpi2ascii writes its stuff.
   */
  extern FILE *dumpfh;

  /** "Magic" value to indicate that we're dealing with a
   * NUL-terminated std::string */
#define DUMPI_CSTRING -1
#ifndef CSTRING
#define CSTRING DUMPI_CSTRING
#endif

  /** "Magic" value to indicate that we have a NULL-terminated 
   * array of pointers */
#define DUMPI_NULLTERM -2
#ifndef NULLTERM
#define NULLTERM DUMPI_NULLTERM
#endif

#define DUMPI_INDENT ""
#define DUMPI_EOL ""
#define DUMPI_END_OF_ITEM "\n"

  /**
   * Print an array of statuses.
   * If the input array is NULL or count < 1 the status will be
   * reported as <IGNORED> (corresponding to MPI_IGNORE_STATUSES).
   */
static inline void print_statuses(const dumpi_status *status, int count) {
  if(count < 1 || status == NULL) {
    fprintf(dumpfh, "null");
  }
  else {
    int i;
    fprintf(dumpfh, "[");
    for(i = 0; i < count; ++i) {
      fprintf(dumpfh, DUMPI_EOL DUMPI_INDENT DUMPI_INDENT "{\"bytes\":%d, \"cancelled\":%d, \"source\":%d, \"tag\":%d, \"error\":%d}",
             status[i].bytes, status[i].cancelled, status[i].source,
             status[i].tag, status[i].error);
      if(i < count-1) fprintf(dumpfh, ", ");
    }
    fprintf(dumpfh, "]");
  }
}

  /**
   * Print an array of request handles.  If the request pointer is NULL
   * or count < 1, the requests are reported as IGNORED (this is likely
   * because the dumpi configuration file specified 'statuses disable'
   * or this is a call with 'statuses success' where no match was found.
   */
static inline void print_requests(const dumpi_request *req, int count) {
  if(count < 0 || req == NULL) {
    fprintf(dumpfh, "null");
  }
  else {
    int i;
    fprintf(dumpfh, "[");
    for(i = 0; i < count; ++i) {
      fprintf(dumpfh, "%d", req[i]);
      if(i < count-1) fprintf(dumpfh, ", ");
  }
    fprintf(dumpfh, "]");
  }
}

#define DUMPI_REPORT_PRE_ITEM(K) do {fprintf(dumpfh, DUMPI_INDENT "\"" #K "\": ");} while(0)
#define DUMPI_REPORT_POST_ITEM(K) do {fprintf(dumpfh, "," DUMPI_EOL);} while(0)
#define DUMPI_REPORT_INLINE(K, ...) \
 do { \
    DUMPI_REPORT_PRE_ITEM(K); \
    fprintf(dumpfh, __VA_ARGS__); \
    DUMPI_REPORT_POST_ITEM(K); \
} while(0)
#define DUMPI_REPORT_PAIR_FORMAT(V,L) ("{\"value\":" V ", \"label\": " L "}")
#define DUMPI_REPORT_INLINE_PAIR_EX(K, V) \
    DUMPI_REPORT_INLINE(K, DUMPI_REPORT_PAIR_FORMAT("%d", "\"%s\""), prm->K, V)
#define DUMPI_REPORT_INLINE_PAIR(K, FUNC) \
    DUMPI_REPORT_INLINE_PAIR_EX(K, FUNC(prm->K))
  /** Common preamble for profiling methods */


#define DUMPI_THREAD_STATS(METHOD, THREAD, CPU, WALL, PERF, MSG) \
  do { \
    DUMPI_REPORT_PRE_ITEM(MSG); \
    fprintf(dumpfh,"{\"walltime\": %d.%09d, "        \
           "\"cputime\": %d.%09d, \"thread\": %d}",       \
           (WALL)->start.sec, (WALL)->start.nsec,	    \
           (CPU)->start.sec,  (CPU)->start.nsec,	    \
           (int)(THREAD)); \
    DUMPI_REPORT_POST_ITEM(MSG); \
  } while(0)
#define DUMPI_ENTERING(METHOD, PRM, THREAD, CPU, WALL, PERF) do { \
    (void)uarg;						    \
    fprintf(dumpfh,"{" DUMPI_EOL DUMPI_INDENT "\"event\":\"" #METHOD "\"," DUMPI_EOL); \
    DUMPI_THREAD_STATS(METHOD, THREAD, CPU, WALL, PERF, entering); \
    DUMPI_PUT_PERF_IN(PERF);				    \
} while(0)
#ifndef ENTERING
#define ENTERING DUMPI_ENTERING
#endif

  /** Common postamble for profiling methods */
#define DUMPI_RETURNING(METHOD, PRM, THREAD, CPU, WALL, PERF)	    \
  DUMPI_THREAD_STATS(METHOD, THREAD, CPU, WALL, PERF, returning); \
  fprintf(dumpfh,"}" DUMPI_END_OF_ITEM); \
  DUMPI_PUT_PERF_OUT(PERF);				    \
  return 1
#ifndef RETURNING
#define RETURNING DUMPI_RETURNING
#endif

  /** Print PAPI perfcounter information at start of MPI call if available */
#define DUMPI_PUT_PERF_IN(PERF) do {			            \
  if(PERF != NULL && perf->count > 0) {			    \
    int i;						    \
    fprintf(dumpfh, "Perfcounters: [");				    \
    for(i = 0; i < perf->count; ++i) {			    \
      fprintf(dumpfh, "%s=%lld",					    \
	     perf->counter_tag[i],                          \
	     (long long)perf->invalue[i]);		    \
      if(i < (perf->count-1)) fprintf(dumpfh, ", ");		    \
    }							    \
    fprintf(dumpfh, "]");					    \
  }							    \
} while(0)
#ifndef PUT_PERF_IN
#define PUT_PERF_IN DUMPI_PUT_PERF_IN
#endif

  /** Print PAPI perfcounter information at completion of MPI call */
#define DUMPI_PUT_PERF_OUT(PERF) do {			            \
  if(PERF != NULL && perf->count > 0) {			    \
    int i;						    \
    fprintf(dumpfh, "Perfcounters: [");				    \
    for(i = 0; i < perf->count; ++i) {			    \
      fprintf(dumpfh, "%s=%lld",					    \
	     perf->counter_tag[i],                          \
	     (long long)perf->outvalue[i]);		    \
      if(i < (perf->count-1)) fprintf(dumpfh, ", ");		    \
    }							    \
    fprintf(dumpfh, "]");					    \
  }							    \
} while(0)
#ifndef PUT_PERF_OUT
#define PUT_PERF_OUT DUMPI_PUT_PERF_OUT
#endif

  /** Print the name of an integer variable and its value */
#define DUMPI_REPORT_INT(TTAG) DUMPI_REPORT_INLINE(TTAG, "%d", prm->TTAG);
#ifndef REPORT_INT
#define REPORT_INT DUMPI_REPORT_INT
#endif

  /**
   * Print the name of an integer array and all the values.
   * If the array is NULL or length < 1, the array is reported as <IGNORED>
   */
#define DUMPI_REPORT_INT_ARRAY_1(LEN, TTAG) do {                  \
  int i;						    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  if((LEN) < 1 || prm->TTAG == NULL) {			    \
    fprintf(dumpfh, "null");				    \
  } else {						    \
    fprintf(dumpfh, "[");					    \
    for(i = 0; i < (LEN); ++i) {			    \
      fprintf(dumpfh, "%d", prm->TTAG[i]);			    \
      if(i < ((LEN)-1)) fprintf(dumpfh, ", ");			    \
    }							    \
    fprintf(dumpfh, "]");					    \
  }							    \
  DUMPI_REPORT_POST_ITEM(TTAG);\
} while(0)
#ifndef REPORT_INT_ARRAY_1
#define REPORT_INT_ARRAY_1 DUMPI_REPORT_INT_ARRAY_1
#endif

  /** Print the name of an int64_t variable and its value. */
#define DUMPI_REPORT_INT64_T(TTAG) \
    DUMPI_REPORT_INLINE(TTAG, "%lld", (long long int)(prm->TTAG))
#ifndef REPORT_INT64_T
#define REPORT_INT64_T DUMPI_REPORT_INT64_T
#endif

  /**
   * Print the name of a 2D integer array and all the values.
   * If the any pointer is NULL or has length < 1, 
   * the array is reported as <IGNORED>
   */
#define DUMPI_REPORT_INT_ARRAY_2(X, Y, TTAG) do {                 \
  int i,j;						    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  if((X) < 1 || prm->TTAG == NULL) {			    \
    fprintf(dumpfh, "null");				    \
  }							    \
  else {						    \
    fprintf(dumpfh, "[");					    \
    for(i = 0; i < (X); ++i) {                              \
      fprintf(dumpfh, "[");                                          \
      for(j = 0; j < (Y); ++j) {                            \
        fprintf(dumpfh, "%d", prm->TTAG[i][j]);			    \
        if(j < ((Y)-1)) fprintf(dumpfh, ", ");			    \
      }							    \
      fprintf(dumpfh, "]");					    \
      if(i < ((X)-1)) fprintf(dumpfh, ", ");                         \
    }                                                       \
    fprintf(dumpfh, "]");                                            \
  }							    \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_INT_ARRAY_2
#define REPORT_INT_ARRAY_2 DUMPI_REPORT_INT_ARRAY_2
#endif

  /** Print the name of a std::string variable and its value */
#define DUMPI_REPORT_CHAR_ARRAY_1(IGNORED, TTAG)              \
    DUMPI_REPORT_INLINE(TTAG, "\"%s\"", prm->TTAG)
#ifndef REPORT_CHAR_ARRAY_1
#define REPORT_CHAR_ARRAY_1 DUMPI_REPORT_CHAR_ARRAY_1
#endif

  /** Print an array of std::strings. */
#define DUMPI_REPORT_CHAR_ARRAY_2(LEN, IGNORED, TTAG) do {        \
  int i;						    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  if((LEN) ==0 || prm->TTAG == NULL) {			    \
    fprintf(dumpfh, "null");				    \
  }							    \
  else {						    \
    fprintf(dumpfh, "[");					    \
    if((LEN) > 0) {                                         \
      for(i = 0; i < (LEN); ++i) {			    \
        fprintf(dumpfh, "\"%s\"", prm->TTAG[i]);			    \
        if(i < ((LEN)-1)) fprintf(dumpfh, ", ");			    \
      }							    \
    }                                                       \
    else {                                                  \
      for(i = 0; prm->TTAG[i] != NULL; ++i) {             \
        fprintf(dumpfh, "\"%s\"", prm->TTAG[i]);                     \
        if(i < ((LEN)-1)) fprintf(dumpfh, ", ");			    \
      }							    \
    }                                                       \
    fprintf(dumpfh, "]");					    \
  }							    \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_CHAR_ARRAY_2
#define REPORT_CHAR_ARRAY_2 DUMPI_REPORT_CHAR_ARRAY_2
#endif

  /** Print a 2D array of std::strings (annoying MPI_Comm_spawn_multiple) */
#define DUMPI_REPORT_CHAR_ARRAY_3(X, Y, IGNORED, TTAG) do {       \
  int i,j;						    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  if((X) ==0 || prm->TTAG == NULL) {			    \
    fprintf(dumpfh, "null");				    \
  }							    \
  else {						    \
    fprintf(dumpfh, "[");					    \
    if((X) > 0) {                                           \
      for(i = 0; i < (X); ++i) {			    \
        fprintf(dumpfh, "[");                                        \
        if((Y) >= 0) {                                      \
          for(j = 0; j < (Y); ++j) {                        \
            fprintf(dumpfh, "\"%s\"", prm->TTAG[i][j]);              \
            if(j < ((Y)-1)) fprintf(dumpfh, ", ");                   \
          }                                                 \
        }                                                   \
        else {                                              \
          for(j = 0; prm->TTAG[i][j] != NULL; ++j) {        \
            fprintf(dumpfh, "\"%s\"", prm->TTAG[i][j]);              \
            if(j < ((Y)-1)) fprintf(dumpfh, ", ");                   \
          }                                                 \
        }                                                   \
        fprintf(dumpfh, "]");                                        \
        if(i < ((X)-1)) fprintf(dumpfh, ", ");                       \
      }							    \
    }                                                       \
    else {                                                  \
      for(i = 0; prm->TTAG[i] != NULL; ++i) {             \
        fprintf(dumpfh, "[");                                        \
        if((Y) >= 0) {                                      \
          for(j = 0; j < (Y); ++j) {                        \
            fprintf(dumpfh, "\"%s\"", prm->TTAG[i][j]);              \
            if(j < ((Y)-1)) fprintf(dumpfh, ", ");                   \
          }                                                 \
        }                                                   \
        else {                                              \
          for(j = 0; prm->TTAG[i][j] != NULL; ++j) {        \
            fprintf(dumpfh, "\"%s\"", prm->TTAG[i][j]);              \
            if(j < ((Y)-1)) fprintf(dumpfh, ", ");                   \
          }                                                 \
        }                                                   \
        fprintf(dumpfh, "]");                                        \
        if(i < ((X)-1)) fprintf(dumpfh, ", ");			    \
      }							    \
    }                                                       \
    fprintf(dumpfh, "]");					    \
  }							    \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_CHAR_ARRAY_3
#define REPORT_CHAR_ARRAY_3 DUMPI_REPORT_CHAR_ARRAY_3
#endif

  /** Print the name of an MPI_Combiner variable and its value */
#define DUMPI_REPORT_COMBINER(TTAG) \
    DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_combiner_name);
#ifndef REPORT_DUMPI_COMBINER
#define REPORT_DUMPI_COMBINER DUMPI_REPORT_COMBINER
#endif

  /** 
   * Print the name of a destination variable and its value.
   * Provides special handling for MPI_ROOT. 
   */
#define DUMPI_REPORT_DEST(TTAG) do {                                    \
    DUMPI_REPORT_INT(TTAG);                                                   \
} while(0)
#ifndef REPORT_DUMPI_DEST
#define REPORT_DUMPI_DEST DUMPI_REPORT_DEST
#endif

  /** Print the name of a distribution array variable and all the values */
#define DUMPI_REPORT_DISTRIBUTION_ARRAY_1(LEN, TTAG) do {       \
    int i;                                                      \
    DUMPI_REPORT_PRE_ITEM(TTAG); \
    fprintf(dumpfh, "[");                         \
    for(i = 0; i < (LEN); ++i) {                                \
      fprintf(dumpfh, "%d", (int)(prm->TTAG[i]));                        \
      if(i < ((LEN)-1)) fprintf(dumpfh, ", ");                           \
    }                                                           \
    fprintf(dumpfh, "]");                                              \
    DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_DISTRIBUTION_ARRAY_1
#define REPORT_DUMPI_DISTRIBUTION_ARRAY_1 DUMPI_REPORT_DISTRIBUTION_ARRAY_1
#endif

  /** Print the name of an errorcode array variable and its values */
#define DUMPI_REPORT_ERRCODE_ARRAY_1(LEN, TTAG) DUMPI_REPORT_INT_ARRAY_1(LEN, TTAG)
#ifndef REPORT_DUMPI_ERRCODE_ARRAY_1
#define REPORT_DUMPI_ERRCODE_ARRAY_1 DUMPI_REPORT_ERRCODE_ARRAY_1
#endif

  /** Print the name of a file variable and its dumpi value */
#define DUMPI_REPORT_FILE(TTAG)                        \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_file_name)
#ifndef REPORT_DUMPI_FILE
#define REPORT_DUMPI_FILE DUMPI_REPORT_FILE
#endif

  /** Print the name of an info variable and its value */
#define DUMPI_REPORT_INFO(TTAG)                    \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_info_name)
#ifndef REPORT_DUMPI_INFO
#define REPORT_DUMPI_INFO DUMPI_REPORT_INFO
#endif

  /** Print the name of an array of info variables and their values */
#define DUMPI_REPORT_INFO_ARRAY_1(LEN, TTAG) do {       \
  int i;                                                \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  for(i = 0; i < LEN; ++i) {                            \
    fprintf(dumpfh, "%d", prm->TTAG[i]);                         \
    if(i < ((LEN)-1)) fprintf(dumpfh, ", ");                     \
  }                                                     \
  fprintf(dumpfh, "]");                                        \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_INFO_ARRAY_1
#define REPORT_DUMPI_INFO_ARRAY_1 DUMPI_REPORT_INFO_ARRAY_1
#endif

  /** Print the name of a keyval variable, its value, and description */
#define DUMPI_REPORT_KEYVAL(TTAG)              \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_keyval_name)
#ifndef REPORT_DUMPI_KEYVAL
#define REPORT_DUMPI_KEYVAL DUMPI_REPORT_KEYVAL
#endif

  /** Print the name of a comm keyval variable, its value, and description */
#define DUMPI_REPORT_COMM_KEYVAL(TTAG) \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_keyval_name)
#ifndef REPORT_DUMPI_COMM_KEYVAL
#define REPORT_DUMPI_COMM_KEYVAL DUMPI_REPORT_COMM_KEYVAL
#endif

  /** Print the name of a type keyval variable, its value, and description */
#define DUMPI_REPORT_TYPE_KEYVAL(TTAG)              \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_keyval_name)
#ifndef REPORT_DUMPI_TYPE_KEYVAL
#define REPORT_DUMPI_TYPE_KEYVAL DUMPI_REPORT_TYPE_KEYVAL
#endif

  /** Print the name of a win keyval variable, its value, and description */
#define DUMPI_REPORT_WIN_KEYVAL(TTAG)              \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_keyval_name)
#ifndef REPORT_DUMPI_WIN_KEYVAL
#define REPORT_DUMPI_WIN_KEYVAL DUMPI_REPORT_WIN_KEYVAL
#endif

  /** Print the name of a locktype variable, its value, and description */
#define DUMPI_REPORT_LOCKTYPE(TTAG)            \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_locktype_name)

#ifndef REPORT_DUMPI_LOCKTYPE
#define REPORT_DUMPI_LOCKTYPE DUMPI_REPORT_LOCKTYPE
#endif

  /** Print the name of an errhandler variable, its value, and description */
#define DUMPI_REPORT_ERRHANDLER(TTAG)          \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_errhandler_name)
#ifndef REPORT_DUMPI_ERRHANDLER
#define REPORT_DUMPI_ERRHANDLER DUMPI_REPORT_ERRHANDLER
#endif

  /** Print the name of a file mode variable, its value, and descriptive name */
#define DUMPI_REPORT_FILEMODE(TTAG)          \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_filemode_name)
#ifndef REPORT_DUMPI_FILEMODE
#define REPORT_DUMPI_FILEMODE DUMPI_REPORT_FILEMODE
#endif

  /** Print the name of an ordering variable, its value, and descriptive name */
#define DUMPI_REPORT_ORDERING(TTAG)            \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_ordering_name)
#ifndef REPORT_DUMPI_ORDERING
#define REPORT_DUMPI_ORDERING DUMPI_REPORT_ORDERING
#endif

  /** 
   * Print the name of a source index.  Special handling for
   * MPI_ANY_SOURCE and MPI_ROOT */
#define DUMPI_REPORT_SOURCE(TTAG) do {				    \
  if(prm->TTAG == DUMPI_ANY_SOURCE) {                                \
    DUMPI_REPORT_INLINE_PAIR_EX(TTAG, "MPI_ANY_SOURCE");       \
  } else if(prm->TTAG == DUMPI_ROOT) {                                 \
    DUMPI_REPORT_INLINE_PAIR_EX(TTAG, "MPI_ANY_MPI_ROOT");       \
  } else {                                                             \
    DUMPI_REPORT_INT(TTAG);                                                   \
  } \
} while(0)
#ifndef REPORT_DUMPI_SOURCE
#define REPORT_DUMPI_SOURCE DUMPI_REPORT_SOURCE
#endif

  /** Print a the name and value of a tag.  Special handling for MPI_ANY_TAG */
#define DUMPI_REPORT_TAG(TTAG) do {                         \
    if(prm->TTAG != DUMPI_ANY_TAG) {			    \
      DUMPI_REPORT_INT(TTAG);					    \
  } else {							    \
    DUMPI_REPORT_INLINE(TTAG,"MPI_ANY_TAG");  \
  } \
} while(0)
#ifndef REPORT_DUMPI_TAG
#define REPORT_DUMPI_TAG DUMPI_REPORT_TAG
#endif

  /** Report the name and value of a thread level variable, and a description */
#define DUMPI_REPORT_THREADLEVEL(TTAG) \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_threadlevel_name)
#ifndef REPORT_DUMPI_THREADLEVEL
#define REPORT_DUMPI_THREADLEVEL DUMPI_REPORT_THREADLEVEL
#endif

  /** Report the name and value of a topology variable, and a description */
#define DUMPI_REPORT_TOPOLOGY(TTAG)            \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_topology_name)
#ifndef REPORT_DUMPI_TOPOLOGY
#define REPORT_DUMPI_TOPOLOGY DUMPI_REPORT_TOPOLOGY
#endif

  /** Report the name and value of a typeclass variable, and a description */
#define DUMPI_REPORT_TYPECLASS(TTAG)           \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_typeclass_name)
#ifndef REPORT_DUMPI_TYPECLASS
#define REPORT_DUMPI_TYPECLASS DUMPI_REPORT_TYPECLASS
#endif

  /** Report the name and value of a win variable, and a description */
#define DUMPI_REPORT_WIN(TTAG)                 \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_win_name)
#ifndef REPORT_DUMPI_WIN
#define REPORT_DUMPI_WIN DUMPI_REPORT_WIN
#endif

  /** Report the name & value of a win assertion variable, and a description */
#define DUMPI_REPORT_WIN_ASSERT(TTAG)        \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_win_assert_name)
#ifndef REPORT_DUMPI_WIN_ASSERT
#define REPORT_DUMPI_WIN_ASSERT DUMPI_REPORT_WIN_ASSERT
#endif

  /** Report the name and value of a status variable */
#define DUMPI_REPORT_STATUS_PTR(TTAG) do {                    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  print_statuses(prm->TTAG, 1);                               \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_STATUS_PTR
#define REPORT_DUMPI_STATUS_PTR DUMPI_REPORT_STATUS_PTR
#endif

  /** Report the name and values of an array of statuses */
#define DUMPI_REPORT_STATUS_ARRAY_1(COUNT, TTAG) do {         \
    DUMPI_REPORT_PRE_ITEM(TTAG); \
    print_statuses(prm->TTAG, COUNT);                         \
    DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_STATUS_ARRAY_1
#define REPORT_DUMPI_STATUS_ARRAY_1 DUMPI_REPORT_STATUS_ARRAY_1
#endif

  /** Print the name and value of a std::string variable */
#define DUMPI_STRVAL(TTAG) DUMPI_REPORT_INLINE(TTAG, "\"%s\"", prm->TTAG)
#ifndef STRVAL
#define STRVAL DUMPI_STRVAL
#endif

  /** Print the name and values of a std::string array variable */
#define DUMPI_STRARR(TTAG, LEN) do {			            \
  int i;						    \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  if((LEN) < 1 || prm->TTAG == NULL) {			    \
    fprintf(dumpfh, "null");				    \
  }							    \
  else {						    \
    fprintf(dumpfh, "[");					    \
    for(i = 0; i < (LEN); ++i) {			    \
      fprintf(dumpfh, "%s", prm->TTAG[i]);			    \
      if(i < ((LEN)-1)) fprintf(dumpfh, ", ");			    \
    }							    \
    fprintf(dumpfh, "]");					    \
  }							    \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef STRARR
#define STRARR DUMPI_STRARR
#endif

  /** Report the name, value, and descriptor of a communicator variable */
#define DUMPI_REPORT_COMM(TTAG)                         \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_comm_name)
#ifndef REPORT_DUMPI_COMM
#define REPORT_DUMPI_COMM DUMPI_REPORT_COMM
#endif

  /** Report the name, value, and description of a comparison */
#define DUMPI_REPORT_COMPARISON(TTAG)                   \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_comparison_name)
#ifndef REPORT_DUMPI_COMPARISON
#define REPORT_DUMPI_COMPARISON DUMPI_REPORT_COMPARISON
#endif

  /** print the name and values of a data type array variable */
  static inline void print_datatypes(dumpi_datatype *dtarr, int count) {
    int i;
    fprintf(dumpfh, "[");
    for(i = 0; i < count; ++i) {
      fprintf(dumpfh, "%d", (int)dtarr[i]);
      if(i < (count-1)) fprintf(dumpfh, ", ");
    }
    fprintf(dumpfh, "]");
  }

  /** Report the name, value, and description of a datatype variable */
#define DUMPI_REPORT_DATATYPE(TTAG)                     \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_datatype_name)
#ifndef REPORT_DUMPI_DATATYPE
#define REPORT_DUMPI_DATATYPE DUMPI_REPORT_DATATYPE
#endif

  /** Report the name, values, and descriptions of an array of datatypes */
#define DUMPI_REPORT_DATATYPE_ARRAY_1(COUNT, TTAG) do {     \
    DUMPI_REPORT_PRE_ITEM(TTAG); \
    print_datatypes(prm->TTAG, COUNT);                      \
    DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_DATATYPE_ARRAY_1
#define REPORT_DUMPI_DATATYPE_ARRAY_1 DUMPI_REPORT_DATATYPE_ARRAY_1
#endif

  /** Report the name, value, and description of a group variable */
#define DUMPI_REPORT_GROUP(TTAG)                       \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_group_name)
#ifndef REPORT_DUMPI_GROUP
#define REPORT_DUMPI_GROUP DUMPI_REPORT_GROUP
#endif

  /** Report the name, value, and description of an MPI_Op. */
#define DUMPI_REPORT_OP(TTAG)                                  \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_op_name)
#ifndef REPORT_DUMPI_OP
#define REPORT_DUMPI_OP DUMPI_REPORT_OP
#endif

  /** Report the name and value of a request handle */
#define DUMPI_REPORT_REQUEST(TTAG) do {                       \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  print_requests(&(prm->TTAG), 1);                            \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_REQUEST
#define REPORT_DUMPI_REQUEST DUMPI_REPORT_REQUEST
#endif

  /** Report the name and values of an array of request handles */
#define DUMPI_REPORT_REQUEST_ARRAY_1(COUNT, TTAG) do {  \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  print_requests(prm->TTAG, COUNT);                   \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPI_REQUEST_ARRAY_1
#define REPORT_DUMPI_REQUEST_ARRAY_1 DUMPI_REPORT_REQUEST_ARRAY_1
#endif

  /** Report the name and value of an MPI IO request */
#define DUMPIO_REPORT_REQUEST(TTAG) do {                       \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
  print_requests(&(prm->TTAG), 1);                            \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPIO_REQUEST
#define REPORT_DUMPIO_REQUEST DUMPIO_REPORT_REQUEST
#endif

  /** Report the name and values of an array of MPI IO requests */
#define DUMPIO_REPORT_REQUEST_ARRAY_1(COUNT, TTAG) do {       \
  DUMPI_REPORT_PRE_ITEM(TTAG); \
    print_requests(prm->TTAG, COUNT);                         \
  DUMPI_REPORT_POST_ITEM(TTAG); \
} while(0)
#ifndef REPORT_DUMPIO_REQUEST_ARRAY_1
#define REPORT_DUMPIO_REQUEST_ARRAY_1 DUMPIO_REPORT_REQUEST_ARRAY_1
#endif

  /** Report the name, value, and description of an MPI whence variable */
#define DUMPI_REPORT_WHENCE(TTAG)                               \
  DUMPI_REPORT_INLINE_PAIR(TTAG, dumpi_get_whence_name)
#ifndef REPORT_DUMPI_WHENCE
#define REPORT_DUMPI_WHENCE DUMPI_REPORT_WHENCE
#endif

  /** 
   * Report the name, value, and description (if available) 
   * of a profiled function
   */
#define DUMPI_REPORT_FUNC(FN) do {                                      \
    int i;                                                              \
  const char *name = NULL;                                              \
  if(d2a_addr != NULL) {                                                \
    for(i = 0; i < d2a_addr->count; ++i) {                              \
      if(prm->FN == d2a_addr->address[i]) {                             \
        name = d2a_addr->name[i];                                       \
        break;                                                          \
      }                                                                 \
    }                                                                   \
  }                                                                     \
  DUMPI_REPORT_INLINE(K, DUMPI_REPORT_PAIR_FORMAT("%lld", "\"%s\""), (long long)prm->FN, name); \
} while(0)
#ifndef REPORT_DUMPI_FUNC
#define REPORT_DUMPI_FUNC DUMPI_REPORT_FUNC
#endif

  /*@}*/

#ifdef __cplusplus
} /* end of extern "C" block */
#endif /* ! __cplusplus */

#endif /* ! DUMPI_BIN_DUMPI2ASCII_DEFS_H */

