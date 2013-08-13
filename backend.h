#include <stdarg.h>
#include <stdio.h>
#include <setjmp.h>
#include <alloca.h>
#include <functional>
#include "gc.h"
#include "types.h"
#include "symbol.h"
#include "environment.h"
#include "parser.h"

#ifndef BACKEND_H
#define BACKEND_H

typedef struct
{
   gen_t * (* fn)(void *);
   void * args;
} call_t;

extern gen_t * nil;

extern jmp_buf exc;

extern sym_t ** sym_tab;

void init(void);

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

del_t _ident(const char * id);

del_t _assign(del_t var, del_t val);

del_t _var(const char * id, del_t val);

del_t _block_fn(const del_t * arr, int num);

#define _block(xx) \
      _block_fn(xx, sizeof(xx)/sizeof(del_t))

del_t _function(const char * id, const char ** params, int num_params, 
                                                         const del_t * arr, int num);

#define _fn(id, params, xx) \
   _function(id, params, sizeof(params)/sizeof(char *), xx, sizeof(xx)/sizeof(del_t))

del_t _call_fn(const char * id, const del_t * params, int num);

#define _call(id, params) \
      _call_fn(id, params, sizeof(params)/sizeof(del_t))

del_t _while(del_t cond, del_t stmt);

del_t _if_else(del_t cond, del_t stmt1, del_t stmt2);

del_t _return_nil(void);

del_t _return(del_t val);

#endif
