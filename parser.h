#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "symbol.h"
#include "gc.h"

typedef enum 
{
   L_INT, L_DOUBLE, L_STRING, L_CHAR, L_OP, L_IDENT
} lex_t;

typedef struct {
   lex_t tag;
   union {
      long l;
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

void parse_init(void);

#ifndef PARSER_H
#define PARSER_H

#endif
