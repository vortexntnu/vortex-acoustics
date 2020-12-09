/*************************************************************************
ALGLIB 3.16.0 (source code generated 2019-12-19)
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses
>>> END OF LICENSE >>>
*************************************************************************/
#ifndef _ap_h
#define _ap_h

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string>
#include <cstring>
#include <iostream>
#include <math.h>
#include <list>
#include <vector>


#define AE_USE_CPP
/* Definitions */
#define AE_UNKNOWN 0
#define AE_INTEL 1
#define AE_SPARC 2

/* threading models for AE_THREADING */
#define AE_PARALLEL                 100
#define AE_SERIAL                   101
#define AE_SERIAL_UNSAFE            102
#if !defined(AE_THREADING)
#define AE_THREADING AE_PARALLEL
#endif

/* malloc types for AE_MALLOC */
#define AE_STDLIB_MALLOC            200
#define AE_BASIC_STATIC_MALLOC      201
#if !defined(AE_MALLOC)
#define AE_MALLOC AE_STDLIB_MALLOC
#endif

#define AE_LOCK_ALIGNMENT 16

/* automatically determine compiler */
#define AE_MSVC 1
#define AE_GNUC 2
#define AE_SUNC 3
#define AE_COMPILER AE_UNKNOWN

/* state flags */
#define _ALGLIB_FLG_THREADING_MASK          0x7
#define _ALGLIB_FLG_THREADING_SHIFT         0
#define _ALGLIB_FLG_THREADING_USE_GLOBAL    0x0
#define _ALGLIB_FLG_THREADING_SERIAL        0x1
#define _ALGLIB_FLG_THREADING_PARALLEL      0x2


/* now we are ready to include headers */
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////

/**
 * ***************************************************************************
 * WARNING: NOT A PART OF THE OFFICIAL LIBRARY! CHANGED SUCH THAT THE COMPILER 
 * KNOWS THAT WE ARE EXPERIENCED "PROFESSIONALS". TO PREVENT THE COMPILER
 * THROWING ERRORS 
 * 
 * WARNING: Using this, result in the system throwing no errors at all! It is
 * therefore NOT allowed to use 
 * try{
 *      throw(...)
 * }
 * catch(...)
 * 
 * 
 * NOTE: THIS MAKES THREADING UNSAFE, HOWEVER THAT IS NOT REALLY A BIG PROBLEM
 * FOR VORTEX NTNU 2020/2021
 * ***************************************************************************
 */
#define AE_NO_EXCEPTIONS
#define AE_THREADING AE_SERIAL_UNSAFE

//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS DECLARATIONS FOR BASIC FUNCTIONALITY 
// LIKE MEMORY MANAGEMENT FOR VECTORS/MATRICES WHICH IS SHARED 
// BETWEEN C++ AND PURE C LIBRARIES
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{

/* if we work under C++ environment, define several conditions */
#ifdef AE_USE_CPP
#define AE_USE_CPP_BOOL
#define AE_USE_CPP_SERIALIZATION
#include <iostream>
#endif

/*
 * define ae_int32_t, ae_int64_t, ae_int_t, ae_bool, ae_complex, ae_error_type and ae_datatype
 */

#if defined(AE_INT32_T)
typedef AE_INT32_T ae_int32_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_INT32_T)
typedef int32_t ae_int32_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_INT32_T)
#if AE_COMPILER==AE_MSVC
typedef __int32 ae_int32_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef int ae_int32_t;
#endif
#endif

#if defined(AE_INT64_T)
typedef AE_INT64_T ae_int64_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_INT64_T)
typedef int64_t ae_int64_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_INT64_T)
#if AE_COMPILER==AE_MSVC
typedef __int64 ae_int64_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef signed long long ae_int64_t;
#endif
#endif

#if defined(AE_UINT64_T)
typedef AE_UINT64_T ae_uint64_t;
#endif
#if defined(AE_HAVE_STDINT) && !defined(AE_UINT64_T)
typedef uint64_t ae_uint64_t;
#endif
#if !defined(AE_HAVE_STDINT) && !defined(AE_UINT64_T)
#if AE_COMPILER==AE_MSVC
typedef unsigned __int64 ae_uint64_t;
#endif
#if (AE_COMPILER==AE_GNUC) || (AE_COMPILER==AE_SUNC) || (AE_COMPILER==AE_UNKNOWN)
typedef unsigned long long ae_uint64_t;
#endif
#endif

#if !defined(AE_INT_T)
typedef ptrdiff_t ae_int_t;
#endif

#if !defined(AE_USE_CPP_BOOL)
#define ae_bool char
#define ae_true 1
#define ae_false 0
#else
#define ae_bool bool
#define ae_true true
#define ae_false false
#endif

typedef struct { double x, y; } ae_complex;

typedef enum
{
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY = 1,
    ERR_XARRAY_TOO_LARGE = 2,
    ERR_ASSERTION_FAILED = 3
} ae_error_type;

typedef ae_int_t ae_datatype;

/*
 * other definitions
 */
enum { OWN_CALLER=1, OWN_AE=2 };
enum { ACT_UNCHANGED=1, ACT_SAME_LOCATION=2, ACT_NEW_LOCATION=3 };
enum { DT_BOOL=1, DT_BYTE=1, DT_INT=2, DT_REAL=3, DT_COMPLEX=4 };
enum { CPU_SSE2=1 };


/************************************************************************
x-vector:
    cnt         number of elements

    datatype    one of the DT_XXXX values

    owner       OWN_CALLER or OWN_AE. Determines what to do on realloc().
                If vector is owned by caller, X-interface  will  just set
                ptr to NULL before realloc(). If it is  owned  by  X,  it
                will call ae_free/x_free/aligned_free family functions.

    last_action ACT_UNCHANGED, ACT_SAME_LOCATION, ACT_NEW_LOCATION
                contents is either: unchanged, stored at the same location,
                stored at the new location.
                this field is set on return from X interface and may be
                used by caller as hint when deciding what to do with data
                (if it was ACT_UNCHANGED or ACT_SAME_LOCATION, no array
                reallocation or copying is required).

    ptr         pointer to the actual data

Members of this structure are ae_int64_t to avoid alignment problems.
************************************************************************/
typedef struct
{
    ae_int64_t     cnt;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    union
    {
        void *p_ptr;
        ae_int64_t portable_alignment_enforcer;
    } x_ptr;
} x_vector;


/************************************************************************
x-matrix:
    rows        number of rows. may be zero only when cols is zero too.

    cols        number of columns. may be zero only when rows is zero too.

    stride      stride, i.e. distance between first elements of rows (in bytes)

    datatype    one of the DT_XXXX values

    owner       OWN_CALLER or OWN_AE. Determines what to do on realloc().
                If vector is owned by caller, X-interface  will  just set
                ptr to NULL before realloc(). If it is  owned  by  X,  it
                will call ae_free/x_free/aligned_free family functions.

    last_action ACT_UNCHANGED, ACT_SAME_LOCATION, ACT_NEW_LOCATION
                contents is either: unchanged, stored at the same location,
                stored at the new location.
                this field is set on return from X interface and may be
                used by caller as hint when deciding what to do with data
                (if it was ACT_UNCHANGED or ACT_SAME_LOCATION, no array
                reallocation or copying is required).

    ptr         pointer to the actual data, stored rowwise

Members of this structure are ae_int64_t to avoid alignment problems.
************************************************************************/
typedef struct
{
    ae_int64_t     rows;
    ae_int64_t     cols;
    ae_int64_t     stride;
    ae_int64_t     datatype;
    ae_int64_t     owner;
    ae_int64_t     last_action;
    union
    {
        void *p_ptr;
        ae_int64_t portable_alignment_enforcer;
    } x_ptr;
} x_matrix;


/************************************************************************
dynamic block which may be automatically deallocated during stack unwinding

p_next          next block in the stack unwinding list.
                NULL means that this block is not in the list
deallocator     deallocator function which should be used to deallocate block.
                NULL for "special" blocks (frame/stack boundaries)
ptr             pointer which should be passed to the deallocator.
                may be null (for zero-size block), DYN_BOTTOM or DYN_FRAME
                for "special" blocks (frame/stack boundaries).

valgrind_hint   is a special field which stores a special hint pointer for
                Valgrind and other similar memory checking tools.  ALGLIB
                manually aligns pointers obtained via malloc, so ptr usually
                points to location past the beginning  of  the  actuallly
                allocated memory. In such cases memory testing tools  may
                report "(possibly) lost" memory.
                
                This "hint" field stores  pointer  actually  returned  by
                malloc (or NULL, if for some reason  we  do  not  support
                this feature). This field is used merely as  a  hint  for
                Valgrind - it should NOT be used for anything else.

************************************************************************/
typedef struct ae_dyn_block
{
    struct ae_dyn_block * volatile p_next;
    /* void *deallocator; */
    void (*deallocator)(void*);
    void * volatile ptr;
    void* valgrind_hint;
} ae_dyn_block;

typedef void(*ae_deallocator)(void*);

/************************************************************************
frame marker
************************************************************************/
typedef struct ae_frame
{
    ae_dyn_block db_marker;
} ae_frame;

/************************************************************************
ALGLIB environment state
************************************************************************/
typedef struct ae_state
{
    /*
     * endianness type: AE_LITTLE_ENDIAN or AE_BIG_ENDIAN
     */
    ae_int_t endianness;
    
    /*
     * double value for NAN
     */
    double v_nan;
    
    /*
     * double value for +INF
     */
    double v_posinf;
    
    /*
     * double value for -INF
     */
    double v_neginf;
    
    /*
     * pointer to the top block in a stack of frames
     * which hold dynamically allocated objects
     */
    ae_dyn_block * volatile p_top_block;
    ae_dyn_block last_block;
    
    /*
     * jmp_buf pointer for internal C-style exception handling
     */
    jmp_buf * volatile break_jump;

    /*
     * ae_error_type of the last error (filled when exception is thrown)
     */
    ae_error_type volatile last_error;
    
    /*
     * human-readable message (filled when exception is thrown)
     */
    const char* volatile error_msg;
    
    /*
     * Flags: call-local settings for ALGLIB
     */
    ae_uint64_t flags;
    
    /*
     * threading information:
     * a) current thread pool
     * b) current worker thread
     * c) parent task (one we are solving right now)
     * d) thread exception handler (function which must be called
     *    by ae_assert before raising exception).
     *
     * NOTE: we use void* to store pointers in order to avoid explicit dependency on smp.h
     */
    void *worker_thread;
    void *parent_task;
    void (*thread_exception_handler)(void*);
    
} ae_state;

/************************************************************************
Serializer:

* ae_stream_writer type is a function pointer for stream  writer  method;
  this pointer is used by X-core for out-of-core serialization  (say,  to
  serialize ALGLIB structure directly to managed C# stream).
  
  This function accepts two parameters: pointer to  ANSI  (7-bit)  string
  and pointer-sized integer passed to serializer  during  initialization.
  String being passed is a part of the data stream; aux paramerer may  be
  arbitrary value intended to be used by actual implementation of  stream
  writer. String parameter may include spaces and  linefeed  symbols,  it
  should be written to stream as is.
  
  Return value must be zero for success or non-zero for failure.
  
* ae_stream_reader type is a function pointer for stream  reader  method;
  this pointer is used by X-core for out-of-core unserialization (say, to
  unserialize ALGLIB structure directly from managed C# stream).
  
  This function accepts three parameters: pointer-sized integer passed to
  serializer  during  initialization; number  of  symbols  to  read  from
  stream; pointer to buffer used to store next  token  read  from  stream
  (ANSI encoding is used, buffer is large enough to store all symbols and
  trailing zero symbol).
  
  Number of symbols to read is always positive.
  
  After being called by X-core, this function must:
  * skip all space and linefeed characters from the current  position  at
    the stream and until first non-space non-linefeed character is found
  * read exactly cnt symbols  from  stream  to  buffer;  check  that  all
    symbols being read are non-space non-linefeed ones
  * append trailing zero symbol to buffer
  * return value must be zero on success, non-zero if  even  one  of  the
    conditions above fails. When reader returns non-zero value,  contents
    of buf is not used.
************************************************************************/
typedef char(*ae_stream_writer)(const char *p_string, ae_int_t aux);
typedef char(*ae_stream_reader)(ae_int_t aux, ae_int_t cnt, char *p_buf);


typedef struct ae_vector
{
    /*
     * Number of elements in array, cnt>=0
     */
    ae_int_t cnt;
    
    /*
     * Either DT_BOOL/DT_BYTE, DT_INT, DT_REAL or DT_COMPLEX
     */
    ae_datatype datatype;
    
    /*
     * If ptr points to memory owned and managed by ae_vector itself,
     * this field is ae_false. If vector was attached to x_vector structure
     * with ae_vector_init_attach_to_x(), this field is ae_true.
     */
    ae_bool is_attached;
    
    /*
     * ae_dyn_block structure which manages data in ptr. This structure
     * is responsible for automatic deletion of object when its frame
     * is destroyed.
     */
    ae_dyn_block data;
    
    /*
     * Pointer to data.
     * User usually works with this field.
     */
    union
    {
        void *p_ptr;
        ae_bool *p_bool;
        unsigned char *p_ubyte;
        ae_int_t *p_int;
        double *p_double;
        ae_complex *p_complex;
    } ptr;
} ae_vector;

typedef struct ae_matrix
{
    ae_int_t rows;
    ae_int_t cols;
    ae_int_t stride;
    ae_datatype datatype;
    
    /*
     * If ptr points to memory owned and managed by ae_vector itself,
     * this field is ae_false. If vector was attached to x_vector structure
     * with ae_vector_init_attach_to_x(), this field is ae_true.
     */
    ae_bool is_attached;
    
    ae_dyn_block data;
    union
    {
        void *p_ptr;
        void **pp_void;
        ae_bool **pp_bool;
        ae_int_t **pp_int;
        double **pp_double;
        ae_complex **pp_complex;
    } ptr;
} ae_matrix;

typedef struct ae_smart_ptr
{
    /* pointer to subscriber; all changes in ptr are translated to subscriber */
    void **subscriber;
    
    /* pointer to object */
    void *ptr;
    
    /* whether smart pointer owns ptr */
    ae_bool is_owner;
    
    /* whether object pointed by ptr is dynamic - clearing such object requires BOTH
       calling destructor function AND calling ae_free for memory occupied by object. */
    ae_bool is_dynamic;
    
    /* destructor function for pointer; clears all dynamically allocated memory */
    void (*destroy)(void*);
    
    /* frame entry; used to ensure automatic deallocation of smart pointer in case of exception/exit */
    ae_dyn_block frame_entry;
} ae_smart_ptr;




}


/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS DECLARATIONS FOR C++ RELATED FUNCTIONALITY
//
/////////////////////////////////////////////////////////////////////////

namespace alglib
{

typedef alglib_impl::ae_int_t ae_int_t;

/********************************************************************
Class forwards
********************************************************************/
class complex;

ae_int_t vlen(ae_int_t n1, ae_int_t n2);

/********************************************************************
Exception class.
********************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
class ap_error
{
public:
    std::string msg;
    
    ap_error();
    ap_error(const char *s);
    static void make_assertion(bool bClause);
    static void make_assertion(bool bClause, const char *p_msg);
private:
};
#endif

/********************************************************************
Complex number with double precision.
********************************************************************/
class complex
{
public:
    complex();
    complex(const double &_x);
    complex(const double &_x, const double &_y);
    complex(const complex &z);

    complex& operator= (const double& v);
    complex& operator+=(const double& v);
    complex& operator-=(const double& v);
    complex& operator*=(const double& v);
    complex& operator/=(const double& v);

    complex& operator= (const complex& z);
    complex& operator+=(const complex& z);
    complex& operator-=(const complex& z);
    complex& operator*=(const complex& z);
    complex& operator/=(const complex& z);

    alglib_impl::ae_complex*       c_ptr();
    const alglib_impl::ae_complex* c_ptr() const;
    
#if !defined(AE_NO_EXCEPTIONS)
    std::string tostring(int dps) const;
#endif

    double x, y;
};

const alglib::complex operator/(const alglib::complex& lhs, const alglib::complex& rhs);
bool operator==(const alglib::complex& lhs, const alglib::complex& rhs);
bool operator!=(const alglib::complex& lhs, const alglib::complex& rhs);
const alglib::complex operator+(const alglib::complex& lhs);
const alglib::complex operator-(const alglib::complex& lhs);
const alglib::complex operator+(const alglib::complex& lhs, const alglib::complex& rhs);
const alglib::complex operator+(const alglib::complex& lhs, const double& rhs);
const alglib::complex operator+(const double& lhs, const alglib::complex& rhs);
const alglib::complex operator-(const alglib::complex& lhs, const alglib::complex& rhs);
const alglib::complex operator-(const alglib::complex& lhs, const double& rhs);
const alglib::complex operator-(const double& lhs, const alglib::complex& rhs);
const alglib::complex operator*(const alglib::complex& lhs, const alglib::complex& rhs);
const alglib::complex operator*(const alglib::complex& lhs, const double& rhs);
const alglib::complex operator*(const double& lhs, const alglib::complex& rhs);
const alglib::complex operator/(const alglib::complex& lhs, const alglib::complex& rhs);
const alglib::complex operator/(const double& lhs, const alglib::complex& rhs);
const alglib::complex operator/(const alglib::complex& lhs, const double& rhs);
double abscomplex(const alglib::complex &z);
alglib::complex conj(const alglib::complex &z);
alglib::complex csqr(const alglib::complex &z);

/********************************************************************
Level 1 BLAS functions

NOTES:
* destination and source should NOT overlap
* stride is assumed to be positive, but it is not 
  assert'ed within function
* conj_src parameter specifies whether complex source is conjugated 
  before processing or not. Pass string which starts with 'N' or 'n'
  ("No conj", for example) to use unmodified parameter. All other
  values will result in conjugation of input, but it is recommended
  to use "Conj" in such cases.
********************************************************************/
double vdotproduct(const double *v0, ae_int_t stride0, const double *v1, ae_int_t stride1, ae_int_t n);
double vdotproduct(const double *v1, const double *v2, ae_int_t N);

alglib::complex vdotproduct(const alglib::complex *v0, ae_int_t stride0, const char *conj0, const alglib::complex *v1, ae_int_t stride1, const char *conj1, ae_int_t n);
alglib::complex vdotproduct(const alglib::complex *v1, const alglib::complex *v2, ae_int_t N);

void vmove(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n);
void vmove(double *vdst, const double* vsrc, ae_int_t N);

void vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void vmove(alglib::complex *vdst, const alglib::complex* vsrc, ae_int_t N);

void vmoveneg(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n);
void vmoveneg(double *vdst, const double *vsrc, ae_int_t N);

void vmoveneg(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void vmoveneg(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N);

void vmove(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void vmove(double *vdst, const double *vsrc, ae_int_t N, double alpha);

void vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void vmove(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, double alpha);

void vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha);
void vmove(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, alglib::complex alpha);

void vadd(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n);
void vadd(double *vdst, const double *vsrc, ae_int_t N);

void vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N);

void vadd(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void vadd(double *vdst, const double *vsrc, ae_int_t N, double alpha);

void vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, double alpha);

void vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha);
void vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, alglib::complex alpha);

void vsub(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n);
void vsub(double *vdst, const double *vsrc, ae_int_t N);

void vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n);
void vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N);

void vsub(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha);
void vsub(double *vdst, const double *vsrc, ae_int_t N, double alpha);

void vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha);
void vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, double alpha);

void vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha);
void vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, alglib::complex alpha);

void vmul(double *vdst,  ae_int_t stride_dst, ae_int_t n, double alpha);
void vmul(double *vdst, ae_int_t N, double alpha);

void vmul(alglib::complex *vdst, ae_int_t stride_dst, ae_int_t n, double alpha);
void vmul(alglib::complex *vdst, ae_int_t N, double alpha);

void vmul(alglib::complex *vdst, ae_int_t stride_dst, ae_int_t n, alglib::complex alpha);
void vmul(alglib::complex *vdst, ae_int_t N, alglib::complex alpha);


/********************************************************************
xparams type and several predefined constants
********************************************************************/
struct xparams
{
    alglib_impl::ae_uint64_t flags;
};

extern const xparams &xdefault;
extern const xparams &serial;
extern const xparams &parallel;

/********************************************************************
Threading functions
********************************************************************/
// nworkers can be 1, 2, ... ; or 0 for auto; or -1/-2/... for all except for one/two/...
void setnworkers(alglib::ae_int_t nworkers);

// sets global threading settings to alglib::serial or alglib::parallel
void setglobalthreading(const xparams settings);

// nworkers can be 1, 2, ... ; or 0 for auto; or -1/-2/... for all except for one/two/...
alglib::ae_int_t getnworkers();

/********************************************************************
internal functions used by test_x.cpp, interfaces for functions present
in commercial ALGLIB but lacking in free edition.
********************************************************************/
ae_int_t _ae_cores_count();
void _ae_set_global_threading(alglib_impl::ae_uint64_t flg_value);
alglib_impl::ae_uint64_t _ae_get_global_threading();

/********************************************************************
1- and 2-dimensional arrays
********************************************************************/
class ae_vector_wrapper
{
public:
    //
    // Creates object attached to external ae_vector structure.
    //
    // NOTE: this function also checks that source ae_vector* has
    //       required datatype. An exception is generated otherwise.
    //
    ae_vector_wrapper(alglib_impl::ae_vector *e_ptr, alglib_impl::ae_datatype datatype);
    
    //
    // Creates zero-size vector of specific datatype
    //
    ae_vector_wrapper(alglib_impl::ae_datatype datatype);
    
    //
    // Creates a copy of another vector (can be reference to one of the derived classes)
    //
    // NOTE: this function also checks that source ae_vector* has
    //       required datatype. An exception is generated otherwise.
    //
    ae_vector_wrapper(const ae_vector_wrapper &rhs, alglib_impl::ae_datatype datatype);
    
    //
    // Well, it is destructor...
    //
    virtual ~ae_vector_wrapper();

    //
    // For wrapper object allocated with allocate_own() this function
    // changes length, completely dropping previous contents.
    //
    // It does not work (throws exception) for frozen proxy objects.
    //
    void setlength(ae_int_t iLen);
    
    //
    // Element count
    //
    ae_int_t length() const;
    
    //
    // Access to internal C-structure used by C-core.
    // Not intended for external use.
    //
    const alglib_impl::ae_vector* c_ptr() const;
    alglib_impl::ae_vector* c_ptr();
private:
    ae_vector_wrapper();
    ae_vector_wrapper(const ae_vector_wrapper &rhs);
    const ae_vector_wrapper& operator=(const ae_vector_wrapper &rhs);
protected:
#if !defined(AE_NO_EXCEPTIONS)
    //
    // Copies array given by string into current object. Additional
    // parameter DATATYPE contains information about type of the data
    // in S and type of the array to create.
    //
    // NOTE: this function is not supported in exception-free mode.
    //
    ae_vector_wrapper(const char *s, alglib_impl::ae_datatype datatype);
#endif

    //
    // This function attaches wrapper object to external x_vector structure;
    // "frozen proxy" mode is activated (you can read/write, but can not reallocate
    // and do not own memory of the vector).
    //
    // NOTE: initial state of wrapper object is assumed to be initialized;
    //       all previously allocated memory is properly deallocated.
    //
    // NOTE: x_vector structure pointed by new_ptr is used only once; after
    //       we fetch pointer to memory and its size, this structure is ignored
    //       and not referenced anymore. So, you can pass pointers to temporary
    //       x-structures which are deallocated immediately after you call attach_to()
    //
    // NOTE: state structure is used for error reporting purposes (longjmp on errors).
    //
    void attach_to(alglib_impl::x_vector *new_ptr, alglib_impl::ae_state *_state);

    //
    // Assigns RHS to current object. Returns *this.
    //
    // It has several branches depending on target object status:
    // * in case it is proxy object, data are copied into memory pointed by
    //   proxy. Function checks that source has exactly same size as target
    //   (exception is thrown on failure).
    // * in case it is non-proxy object, data allocated by object are cleared
    //   and a copy of RHS is created in target.
    //
    // NOTE: this function correctly handles assignments of the object to itself.
    //
    const ae_vector_wrapper& assign(const ae_vector_wrapper &rhs);
    
    //
    // Pointer to ae_vector structure:
    // * ptr==&inner_vec means that wrapper object owns ae_vector structure and
    //   is responsible for proper deallocation of its memory
    // * ptr!=&inner_vec means that wrapper object works with someone's other
    //   ae_vector record and is not responsible for its memory; in this case
    //   inner_vec is assumed to be uninitialized.
    //
    alglib_impl::ae_vector *ptr;
    
    //
    // Inner ae_vector record.
    // Ignored for ptr!=&inner_rec.
    //
    alglib_impl::ae_vector inner_vec;
    
    //
    // Whether this wrapper object is frozen proxy (you may read array, may
    // modify its value, but can not deallocate its memory or resize it) or not.
    //
    // If is_frozen_proxy==true and if:
    // * ptr==&inner_vec, it means that wrapper works with its own ae_vector
    //   structure, but this structure points to externally allocated memory.
    //   This memory is NOT owned by ae_vector object.
    // * ptr!=&inner_vec, it means that wrapper works with externally allocated
    //   and managed ae_vector structure. Both memory pointed by ae_vector and
    //   ae_vector structure itself are not owned by wrapper object.
    //
    bool                   is_frozen_proxy;
};



class integer_1d_array : public ae_vector_wrapper
{
public:
    integer_1d_array();
    integer_1d_array(const integer_1d_array &rhs);
    integer_1d_array(alglib_impl::ae_vector *p);
    const integer_1d_array& operator=(const integer_1d_array &rhs);
    virtual ~integer_1d_array();

    const ae_int_t& operator()(ae_int_t i) const;
    ae_int_t& operator()(ae_int_t i);

    const ae_int_t& operator[](ae_int_t i) const;
    ae_int_t& operator[](ae_int_t i);

    //
    // This function allocates array[iLen] and copies data
    // pointed by pContent to its memory. Completely independent
    // copy of data is created.
    //
    void setcontent(ae_int_t iLen, const ae_int_t *pContent );
    
    //
    // This function returns pointer to internal memory
    //
    ae_int_t* getcontent();
    const ae_int_t* getcontent() const;

#if !defined(AE_NO_EXCEPTIONS)
    integer_1d_array(const char *s);
    std::string tostring() const;
#endif
};

class real_1d_array : public ae_vector_wrapper
{
public:
    real_1d_array();
    real_1d_array(const real_1d_array &rhs);
    real_1d_array(alglib_impl::ae_vector *p);
    const real_1d_array& operator=(const real_1d_array &rhs);
    virtual ~real_1d_array();

    const double& operator()(ae_int_t i) const;
    double& operator()(ae_int_t i);

    const double& operator[](ae_int_t i) const;
    double& operator[](ae_int_t i);

    //
    // This function allocates array[iLen] and copies data
    // pointed by pContent to its memory. Completely independent
    // copy of data is created.
    //
    void setcontent(ae_int_t iLen, const double *pContent);
    
    //
    // This function attaches array to memory pointed by pContent.
    // No own memory is allocated, no copying of data is performed,
    // so pContent pointer should be valid as long as we work with
    // array.
    //
    // After you attach array object to external memory, it becomes
    // "frozen": it is possible to read/write array elements, but
    // it is not allowed to resize it (no setlength() calls).
    //
    void attach_to_ptr(ae_int_t iLen, double *pContent);
    
    //
    // This function returns pointer to internal memory
    //
    double* getcontent();
    const double* getcontent() const;

#if !defined(AE_NO_EXCEPTIONS)
    real_1d_array(const char *s);
    std::string tostring(int dps) const;
#endif
};

class complex_1d_array : public ae_vector_wrapper
{
public:
    complex_1d_array();
    complex_1d_array(const complex_1d_array &rhs);
    complex_1d_array(alglib_impl::ae_vector *p);
    const complex_1d_array& operator=(const complex_1d_array &rhs);
    virtual ~complex_1d_array();

    const alglib::complex& operator()(ae_int_t i) const;
    alglib::complex& operator()(ae_int_t i);

    const alglib::complex& operator[](ae_int_t i) const;
    alglib::complex& operator[](ae_int_t i);

    //
    // This function allocates array[iLen] and copies data
    // pointed by pContent to its memory. Completely independent
    // copy of data is created.
    //
    void setcontent(ae_int_t iLen, const alglib::complex *pContent );
    alglib::complex* getcontent();
    const alglib::complex* getcontent() const;

#if !defined(AE_NO_EXCEPTIONS)
    complex_1d_array(const char *s);
    std::string tostring(int dps) const;
#endif
};



/********************************************************************
Exception handling macros
********************************************************************/
#if !defined(AE_NO_EXCEPTIONS)
///////////////////////////////////////
// exception-based code
//////////////////////////////
#define _ALGLIB_CPP_EXCEPTION(msg) throw alglib::ap_error(msg)
#define _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN          try{
#define _ALGLIB_CALLBACK_EXCEPTION_GUARD_END            }catch(...){ ae_clean_up_before_breaking(&_alglib_env_state); throw; }

#else
    
///////////////////////////////////////
// Exception-free version
//////////////////////////////
#if AE_OS!=AE_UNKNOWN
#error Exception-free mode can not be combined with AE_OS definition
#endif
#if AE_THREADING!=AE_SERIAL_UNSAFE
#error Exception-free mode is thread-unsafe; define AE_THREADING=AE_SERIAL_UNSAFE to prove that you know it
#endif
#define _ALGLIB_CALLBACK_EXCEPTION_GUARD_BEGIN
#define _ALGLIB_CALLBACK_EXCEPTION_GUARD_END
#define _ALGLIB_SET_ERROR_FLAG(s) set_error_flag(s)

// sets eror flag and (optionally) sets error message
void set_error_flag(const char *s = NULL);

// returns error flag and optionally returns error message (loaded to *p_msg);
// if error flag is not set (or p_msg is NULL) *p_msg is not changed.
bool get_error_flag(const char **p_msg = NULL);

// clears error flag (it is not cleared until explicit call to this function)
void clear_error_flag();
#endif

}//namespace alglib


#endif //_ap_h