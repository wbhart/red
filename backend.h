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

typedef struct
{
   gen_t * (* fn)(void *);
   void * args;
} call_t;

extern gen_t * nil;

extern jmp_buf exc;

extern sym_t ** sym_tab;

void init(void);

fn_t INT(long i);
fn_t UINT(unsigned long u);
fn_t DOUBLE(double d);

fn_t ADD(fn_t a, fn_t b);
fn_t SUB(fn_t a, fn_t b);
fn_t MUL(fn_t a, fn_t b);
fn_t DIV(fn_t a, fn_t b);
fn_t MOD(fn_t a, fn_t b);

fn_t ADDEQ(fn_t a, fn_t b);
fn_t SUBEQ(fn_t a, fn_t b);
fn_t MULEQ(fn_t a, fn_t b);
fn_t DIVEQ(fn_t a, fn_t b);
fn_t MODEQ(fn_t a, fn_t b);

fn_t LT(fn_t a, fn_t b);
fn_t GT(fn_t a, fn_t b);
fn_t LE(fn_t a, fn_t b);
fn_t GE(fn_t a, fn_t b);
fn_t EQ(fn_t a, fn_t b);
fn_t NE(fn_t a, fn_t b);

fn_t INC(fn_t a);
fn_t DEC(fn_t a);

fn_t PRINT(fn_t f);

fn_t PRINTLN(fn_t f);

fn_t IDENT(const char * id);

fn_t ASSIGN(fn_t var, fn_t val);

fn_t VAR(const char * id, fn_t val);

fn_t BLOCK_FN(const fn_t * arr, int num);

#define BLOCK(xx) \
      BLOCK_FN(xx, sizeof(xx)/sizeof(fn_t))

void NEW_FUNCTION_FN(const char * id, const char ** params, int num_params);

#define NEW_FUNCTION(id, xx) \
   NEW_FUNCTION_FN(id, xx, sizeof(xx)/sizeof(char *))

fn_t FUNCTION_FN(const fn_t * arr, int num);

#define FUNCTION_BODY(xx) \
   FUNCTION_FN(xx, sizeof(xx)/sizeof(fn_t))

fn_t CALL_FN(const char * id, const fn_t * params, int num);

#define CALL(id, params) \
      CALL_FN(id, params, sizeof(params)/sizeof(fn_t))

fn_t WHILE(fn_t cond, fn_t stmt);

fn_t RETURN_NIL(void);

fn_t RETURN(fn_t val);

#endif
