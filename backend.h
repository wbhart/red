/*

Copyright 2013 William Hart. All rights reserved.

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

#include <stdarg.h>
#include <stdio.h>
#include <setjmp.h>
#include <alloca.h>
#include <functional>
#include "gc.h"
#include "types.h"
#include "symbol.h"
#include "environment.h"

#ifndef BACKEND_H
#define BACKEND_H

extern jmp_buf exc;

void exception(void);

typedef struct
{
   gen_t * (* fn)(void *);
   void * args;
} call_t;

extern gen_t * nil;

extern jmp_buf exc;

extern sym_t ** sym_tab;

extern function_t * current_fn;

del_t _int(long i);
del_t _uint(unsigned long u);
del_t _double(double d);

del_t _add(del_t a, del_t b);
del_t _sub(del_t a, del_t b);
del_t _mul(del_t a, del_t b);
del_t _div(del_t a, del_t b);
del_t _mod(del_t a, del_t b);

del_t _addeq(del_t a, del_t b);
del_t _subeq(del_t a, del_t b);
del_t _muleq(del_t a, del_t b);
del_t _diveq(del_t a, del_t b);
del_t _modeq(del_t a, del_t b);

del_t _lt(del_t a, del_t b);
del_t _gt(del_t a, del_t b);
del_t _le(del_t a, del_t b);
del_t _ge(del_t a, del_t b);
del_t _eq(del_t a, del_t b);
del_t _ne(del_t a, del_t b);

del_t _inc(del_t a);
del_t _dec(del_t a);

del_t _print(del_t f);

del_t _println(del_t f);

del_t _ident(sym_t * id);

del_t _assign(del_t var, del_t val);

del_t _var(sym_t * id, del_t val);

del_t _block_fn(const del_t * arr, int num);

#define _block(xx) \
      _block_fn(xx, sizeof(xx)/sizeof(del_t))

del_t _function(sym_t * id, sym_t ** params, int num_params, 
                                                         const del_t * arr, int num);

#define _fn(id, params, xx) \
   _function(id, params, sizeof(params)/sizeof(char *), xx, sizeof(xx)/sizeof(del_t))

del_t _call_fn(sym_t * id, const del_t * params, int num);

#define _call(id, params) \
      _call_fn(id, params, sizeof(params)/sizeof(del_t))

del_t _while(del_t cond, del_t stmt);

del_t _if_else(del_t cond, del_t stmt1, del_t stmt2);

del_t _return_nil(void);

del_t _return(del_t val);

#endif
