/*

Copyright 2012 William Hart. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY William Hart ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL William Hart OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <string.h>
#include <stdio.h>
#include <functional>
#include <setjmp.h>

#include "symbol.h"
#include "gc.h"

#ifndef TYPES_H
#define TYPES_H

typedef enum
{
   T_NIL, T_BREAK, T_BOOL, T_INT, T_UINT, T_FLOAT, T_DOUBLE, 
   T_CHAR, T_STRING, T_FN, T_ARRAY, T_TUPLE, T_TYPE 
} tag_t;

typedef struct type_t
{
   tag_t tag; /* kind of type */
   int num_args; /* number of args */
   int num_params; /* number of type parameters */
   struct type_t ** params; /* type parameters */
   struct sym_t * sym; /* name of type */
   struct sym_t ** slots; /* names of type args/slots */
} type_t;

typedef union
{
   long i;
   unsigned long u;
   double d;
   char c;
   char * str;
   void * ptr;
} val_t;

typedef struct
{
   type_t * type;
   val_t val;
} gen_t;

typedef std::function<gen_t * ()> fn_t;

typedef std::function<fn_t ()> del_t;

typedef struct function_t
{
   sym_t ** params;
   gen_t ** vals;
   struct env_t * scope;
   jmp_buf * exc;
   fn_t fn;
   const fn_t * arr;
} function_t;

typedef struct block_t
{
   struct env_t * scope;
   const fn_t * arr;
} block_t;

extern type_t * t_nil;
extern type_t * t_int;
extern type_t * t_uint;
extern type_t * t_bool;
extern type_t * t_double;
extern type_t * t_float;
extern type_t * t_string;
extern type_t * t_char;
extern type_t * t_break;

type_t * new_type(const char * name, tag_t tag);

void types_init(void);

type_t * fn_type(int num);

void type_print(type_t * type);

#endif

