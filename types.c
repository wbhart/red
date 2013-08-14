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

#include "types.h"

type_t * t_nil;
type_t * t_int;
type_t * t_uint;
type_t * t_bool;
type_t * t_double;
type_t * t_float;
type_t * t_string;
type_t * t_char;
type_t * t_break;

type_t * new_type(const char * name, tag_t tag)
{
   type_t * t = (type_t *) GC_MALLOC(sizeof(type_t));
   t->tag = tag;
   t->sym = sym_lookup(name);
   return t;
}

void types_init(void)
{
   t_nil = new_type("nil", T_NIL);
   t_int = new_type("int", T_INT);
   t_uint = new_type("uint", T_UINT);
   t_bool = new_type("bool", T_BOOL);
   t_double = new_type("double", T_DOUBLE);
   t_float = new_type("float", T_FLOAT);
   t_string = new_type("string", T_STRING);
   t_char = new_type("char", T_CHAR);
   t_break = new_type("break", T_BREAK);
}

type_t * fn_type(int num)
{
   type_t * t = (type_t *) GC_MALLOC(sizeof(type_t));

   t->tag = T_FN;
   t->num_args = num;

   return t;
}

void type_print(type_t * type)
{
   int i;
   
   switch (type->tag)
   {
   case T_NIL:
      printf("nil");
      break;
   case T_INT:
      printf("int");
      break;
   case T_UINT:
      printf("uint");
      break;
   case T_DOUBLE:
      printf("double");
      break;
   case T_FLOAT:
      printf("float");
      break;
   case T_CHAR:
      printf("char");
      break;
   case T_STRING:
      printf("string");
      break;
   case T_BOOL:
      printf("bool");
      break;
   }
}