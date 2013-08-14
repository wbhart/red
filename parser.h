#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "symbol.h"
#include "environment.h"
#include "types.h"
#include "backend.h"
#include "gc.h"

typedef enum 
{
   P_INT, P_DOUBLE, P_STRING, P_CHAR, P_OP, P_IDENT
} lex_t;

typedef struct {
   lex_t tag;
   union {
      long i;
      double d;
      char c;
      char * str;
      sym_t * sym;
   } val;
} parse_t;

typedef struct plist_t {
   parse_t * p;
   struct plist_t * next;
} plist_t;

extern parse_t * op_le;
extern parse_t * op_lt;
extern parse_t * op_ge;
extern parse_t * op_gt;
extern parse_t * op_eq;
extern parse_t * op_neq;
extern parse_t * op_set;
extern parse_t * op_plus;
extern parse_t * op_minus;
extern parse_t * op_times;
extern parse_t * op_quo;
extern parse_t * op_rem;
extern parse_t * op_rarrow;
extern parse_t * op_semi;
extern parse_t * op_colon;
extern parse_t * op_lparem;
extern parse_t * op_rparen;
extern parse_t * op_comma;

void init(void);

void parse_init(void);

void parse_reset(FILE * in);

parse_t * read(FILE * in);

void rewind(plist_t * l);

int p_int(del_t * del, FILE * in);

int p_double(del_t * del, FILE * in);

int p_var(del_t * del, FILE * in);

int p_assign(del_t * del, FILE * in);

int p_print(del_t * del, FILE * in);

int p_println(del_t * del, FILE * in);

int p_primary(del_t * del, FILE * in);

int p_infix10(del_t * del, FILE * in);

int p_expr(del_t * del, FILE * in);

int p_while(del_t * del, FILE * in);

int p_if_else(del_t * del, FILE * in);

int p_fn(del_t * del, FILE * in);

int p_return(del_t * del, FILE * in);

int p_appl(del_t * del, FILE * in);

int p_stmt(del_t * del, FILE * in);

int parse(del_t * d, FILE * in);

#ifndef PARSER_H
#define PARSER_H

#endif
