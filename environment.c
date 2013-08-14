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

#include "environment.h"

env_t * current_scope;

void scope_init(void)
{
   current_scope = (env_t *) GC_MALLOC(sizeof(env_t));
}

bind_t * bind_symbol(sym_t * sym, gen_t * g)
{
   bind_t * scope = current_scope->scope;
   bind_t * b = (bind_t *) GC_MALLOC(sizeof(bind_t));
   b->sym = sym;
   b->val = g;
   b->next = scope;
   current_scope->scope = b;
   return b;
}

bind_t * find_symbol(sym_t * sym)
{
   env_t * s = current_scope;
   bind_t * b;

   while (s != NULL)
   {
      b = s->scope;
 
      while (b != NULL)
      {
         if (b->sym == sym)
            return b;
         b = b->next;
      }
      
      s = s->next;
   }

   return NULL;
}

env_t * scope_up(void)
{
   env_t * env = (env_t *) GC_MALLOC(sizeof(env_t));
   env->next = current_scope;
   current_scope = env;
   return current_scope;
}

env_t * scope_down(void)
{
   current_scope = current_scope->next;
   return NULL;
}

int scope_is_global(bind_t * bind)
{
   env_t * s = current_scope;
   while (s->next != NULL)
      s = s->next;

   bind_t * b = s->scope;
   while (b != NULL)
   {
      if (b == bind)
         return 1;
      b = b->next;
   }
   return 0;
}
