#include "backend.h"

/******************************************************************************

   Objects

******************************************************************************/

function_t * current_fn;

gen_t * nil;

void init(void)
{
   sym_tab_init();
   parse_init();
   scope_init();
   types_init();
   
   nil = (gen_t *) GC_MALLOC(sizeof(gen_t));
   nil->type = t_nil;

   current_fn = NULL;
}

/******************************************************************************

   Call constructors

******************************************************************************/

fn_t __int(long i)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.i = i; 
      g->type = t_int; 
      return g; 
   };
}

del_t _int(long i)
{
   return [=] () {
      return __int(i);
   };
}

fn_t __uint(unsigned long u)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.u = u; 
      g->type = t_uint; 
      return g; 
   };
}

del_t _uint(unsigned long u)
{
   return [=] () {
      return __uint(u);
   };
}

fn_t __double(double d)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.d = d; 
      g->type = t_double; 
      return g; 
   };
}

del_t _double(double d)
{
   return [=] () {
      return __double(d);
   };
}

fn_t __print(fn_t f)
{
   return [=] () {
      gen_t * g = f();
   
      switch (g->type->tag) {
      case T_INT:
         printf("%ld", g->val.i);
         return nil;
      case T_UINT:
         printf("%lu", g->val.u);
         return nil;
      case T_DOUBLE:
         printf("%g", g->val.d);
         return nil;
      case T_BOOL:
         if (g->val.i == 0)
            printf("false");
         else
            printf("true");
         return nil;
      }
   };
}

del_t _print(del_t v)
{
   return [=] () {
      fn_t f = v();
      
      return __print(f);
   };
}

fn_t __println(fn_t f)
{
   fn_t p = __print(f);
   return [=] () {
      p();
      printf("\n");
      return nil;
   };
}

del_t _println(del_t v)
{   
   return [=] () {
      fn_t f = v();

      return __println(f);
   };
}

#define BINOP(name, op)                                  \
   fn_t name(fn_t a, fn_t b) {                           \
      return [=] () {                                    \
         gen_t * g1 = a();                               \
         const gen_t * g2 = b();                         \
         gen_t * r = (gen_t *) GC_MALLOC(sizeof(gen_t)); \
                                                         \
         if (g1->type == g2->type) {                     \
            switch (g1->type->tag) {                     \
            case T_INT:                                  \
               r->val.i = g1->val.i op g2->val.i;        \
               r->type = t_int;                          \
               return r;                                 \
            case T_UINT:                                 \
               r->val.u = g1->val.u op g2->val.u;        \
               r->type = t_uint;                         \
               return r;                                 \
            case T_DOUBLE:                               \
               r->val.d = g1->val.d op g2->val.d;        \
               r->type = t_double;                       \
               return r;                                 \
            }                                            \
         }                                               \
      };                                                 \
   }

BINOP(__add, +)
BINOP(__sub, -)
BINOP(__mul, *)
BINOP(__div, /)

#define BINOP_DEL(name)           \
   del_t name(del_t a, del_t b) { \
                                  \
      return [=] () {             \
         fn_t f1 = a();           \
         fn_t f2 = b();           \
        return _##name(f1, f2);   \
      };                          \
   }

#define BINOPEQ(name, op)                                \
   fn_t name(fn_t a, fn_t b) {                           \
      return [=] () {                                    \
         gen_t * g1 = a();                               \
         const gen_t * g2 = b();                         \
                                                         \
         if (g1->type == g2->type) {                     \
            switch (g1->type->tag) {                     \
            case T_INT:                                  \
               g1->val.i op g2->val.i;                   \
               return g1;                                \
            case T_UINT:                                 \
               g1->val.u op g2->val.u;                   \
               return g1;                                \
            case T_DOUBLE:                               \
               g1->val.d op g2->val.d;                   \
               return g1;                                \
            }                                            \
         }                                               \
      };                                                 \
   }

BINOPEQ(__addeq, +=)
BINOPEQ(__subeq, -=)
BINOPEQ(__muleq, *=)
BINOPEQ(__diveq, /=)

fn_t __mod(fn_t a, fn_t b)
{
   return [=] () {
      gen_t * g1 = a();
      const gen_t * g2 = b();
      gen_t * r = (gen_t *) GC_MALLOC(sizeof(gen_t));

      if (g1->type == g2->type) {                      
         switch (g1->type->tag) {                     
         case T_INT:                                 
            r->val.i = g1->val.i % g2->val.i;  
            r->type = t_int;
            return r;
         case T_UINT:                                
            r->val.u = g1->val.u % g2->val.u;  
            r->type = t_uint;
            return r;
         }                                           
      }                                              
   };
}

fn_t __modeq(fn_t a, fn_t b)
{
   return [=] () {
      gen_t * g1 = a();
      const gen_t * g2 = b();
      
      if (g1->type == g2->type) {                      
         switch (g1->type->tag) {                     
         case T_INT:                                 
            g1->val.i %= g2->val.i;  
            return g1;
         case T_UINT:                                
            g1->val.u %= g2->val.u;  
            return g1;
         }                                           
      }                                              
   };
}

BINOP_DEL(_add)
BINOP_DEL(_sub)
BINOP_DEL(_mul)
BINOP_DEL(_div)
BINOP_DEL(_mod)

BINOP_DEL(_addeq)
BINOP_DEL(_subeq)
BINOP_DEL(_muleq)
BINOP_DEL(_diveq)
BINOP_DEL(_modeq)

#define REL_BIN(name, rel)                               \
   fn_t name(fn_t a, fn_t b) {                           \
      return [=] () {                                    \
         const gen_t * g1 = a();                         \
         const gen_t * g2 = b();                         \
         gen_t * r = (gen_t *) GC_MALLOC(sizeof(gen_t)); \
                                                         \
         if (g1->type == g2->type) {                     \
            switch (g1->type->tag) {                     \
            case T_INT:                                  \
               r->val.i = g1->val.i rel g2->val.i;       \
               break;                                    \
            case T_UINT:                                 \
               r->val.i = g1->val.u rel g2->val.u;       \
               break;                                    \
            case T_DOUBLE:                               \
               r->val.i = g1->val.d rel g2->val.d;       \
               break;                                    \
            }                                            \
         }                                               \
         r->type = t_bool;                               \
         return r;                                       \
      };                                                 \
   }

REL_BIN(__lt, <)
REL_BIN(__gt, >)
REL_BIN(__le, <=)
REL_BIN(__ge, >=)
REL_BIN(__eq, ==)
REL_BIN(__ne, !=)

BINOP_DEL(_lt)
BINOP_DEL(_gt)
BINOP_DEL(_le)
BINOP_DEL(_ge)
BINOP_DEL(_eq)
BINOP_DEL(_ne)

#define OP_UN(name, op)           \
   fn_t name(fn_t a) {            \
      return [=] () {             \
         gen_t * g1 = a();        \
                                  \
         switch (g1->type->tag) { \
            case T_INT:           \
               op(g1->val.i);     \
               return g1;         \
            case T_UINT:          \
               op(g1->val.u);     \
               return g1;         \
            case T_DOUBLE:        \
               op(g1->val.d);     \
               return g1;         \
         }                        \
      };                          \
   }

OP_UN(__inc, ++)
OP_UN(__dec, --)

#define UNOP_DEL(name)       \
   del_t name(del_t a) {     \
                             \
      return [=] () {        \
         fn_t f1 = a();      \
         return _##name(f1); \
      };                     \
   }

UNOP_DEL(_inc)
UNOP_DEL(_dec)

fn_t __ident(const char * id)
{
   sym_t * sym = sym_lookup(id);
   bind_t * bind = find_symbol(sym);
   
   gen_t * g = bind->val;
   
   return [=] () {
      return g;
   };
}

del_t _ident(const char * id)
{
   return [=] () {
      return __ident(id);
   };
}

fn_t __assign(fn_t var, fn_t val)
{
   return [=] () {
      gen_t * g1 = var();
      const gen_t * g2 = val();

      g1->type = g2->type;
      g1->val = g2->val;

      return g1;
   };
}

del_t _assign(del_t a, del_t b)
{
   return [=] () {
      fn_t f1 = a();
      fn_t f2 = b();
      
      return __assign(f1, f2);
   };
}

fn_t __var(const char * id, fn_t val)
{
   gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   bind_symbol(sym_lookup(id), g);

   fn_t var = __ident(id);

   return __assign(var, val);
}

del_t _var(const char * id, del_t v)
{
   return [=] () {
      fn_t f = v();
   
      return __var(id, f);
   };
}

fn_t __block(const fn_t * arr, int num)
{
   block_t * b = (block_t *) GC_MALLOC(sizeof(block_t));
   gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   g->type = t_break;
   g->val.ptr = (void *) b;
   b->scope = current_scope;
   b->arr = arr;
   env_t * save = current_scope;
   scope_down();
   bind_symbol(sym_lookup("break"), g);
   current_scope = save;

   return [=] () {
      int i;
      gen_t * res;

      for (i = 0; i < num; i++)
         res = arr[i]();

      return res;
   };
}

del_t _block_fn(const del_t * arr, int num)
{
   return [=] () {
      fn_t * farr = (fn_t *) GC_MALLOC(num*sizeof(fn_t));
      int i;

      scope_up();

      for (i = 0; i < num; i++)
         farr[i] = arr[i]();

      fn_t res = __block(farr, num);

      scope_down();

      return res;
   };
}

fn_t __while(fn_t cond, fn_t stmt)
{
   return [=] () {
      long i;

      while (cond()->val.i)
         stmt();

      return nil;
   };
}

del_t _while(del_t cond, del_t stmt)
{
   return [=] () {
      fn_t c = cond();
      fn_t s = stmt();
      
      return __while(c, s);
   };
}

fn_t __if_else(fn_t cond, fn_t stmt1, fn_t stmt2)
{
   return [=] () {
      long i;

      if (cond()->val.i)
         stmt1();
      else
         stmt2();

      return nil;
   };
}

del_t _if_else(del_t cond, del_t stmt1, del_t stmt2)
{
   return [=] () {
      fn_t c = cond();
      fn_t s1 = stmt1();
      fn_t s2 = stmt2();
      
      return __if_else(c, s1, s2);
   };
}

void __new_function(const char * id, const char ** params, int num_params)
{
   sym_t * sym = sym_lookup(id);
   sym_t ** p = (sym_t **) GC_MALLOC(num_params*sizeof(sym_t *));
   gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   env_t * save = current_scope;
   function_t * f = (function_t *) GC_MALLOC(sizeof(function_t)); 
   int i;

   current_fn = f;

   for (i = 0; i < num_params; i++)
      p[i] = sym_lookup(params[i]);

   f->params = p;
   
   g->type = fn_type(num_params);
   g->val.ptr = (void *) f;
   scope_down();
   bind_symbol(sym, g);
   current_scope = save;

   f->vals = (gen_t **) GC_MALLOC(num_params*sizeof(gen_t *));
   f->scope = current_scope;

   for (i = 0; i < num_params; i++)
   {
      g = (gen_t *) GC_MALLOC(sizeof(gen_t));
      bind_symbol(sym_lookup(params[i]), g);
      f->vals[i] = g;
   }

   g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   bind_symbol(sym_lookup("return"), g);
}

fn_t __function_body(const fn_t * arr, int num)
{
   function_t * f = current_fn;
   f->arr = arr;
   int i;

   f->fn = [=] () {
      int i, jval = 0;
      
      jmp_buf exc;
      jmp_buf * save = f->exc;
      f->exc = &exc;
         
      if (!(jval = setjmp(exc)))
      {
         for (i = 0; i < num; i++)
            arr[i]();
      } 

      f->exc = save;

      return nil;
   };

   return [=] () {
      return nil;
   };
}

del_t _function(const char * id, const char ** params, int num_params, const del_t * arr, int num)
{
   del_t res = [=] () {
      fn_t * farr = (fn_t *) GC_MALLOC(num*sizeof(fn_t));
      int i;

      scope_up();

      __new_function(id, params, num_params);

      for (i = 0; i < num; i++)
         farr[i] = arr[i]();

      fn_t res = __function_body(farr, num);

      scope_down();

      return res;
   };

   return res;
}

fn_t __call(const char * id, const fn_t * params, int num)
{
   bind_t * bind = find_symbol(sym_lookup(id));
   gen_t * g = bind->val;
   function_t * f = (function_t *) g->val.ptr;
   gen_t ** vals = f->vals;
   env_t * save = current_scope;
   current_scope = f->scope;
   bind = find_symbol(sym_lookup("return"));
   g = bind->val;

   return [=] () {
      gen_t * p_save = (gen_t *) alloca(num*sizeof(gen_t));
      fn_t fn = f->fn;
      int i;

      for (i = 0; i < num; i++)
      {
         gen_t t = *(vals[i]);
         p_save[i] = t;
         *(vals[i]) = *(params[i]());
      }
      
      fn();
      
      for (i = 0; i < num; i++)
         *(vals[i]) = p_save[i];

      return g;
   };
}

del_t _call_fn(const char * id, const del_t * params, int num)
{
   return [=] () {
      fn_t * farr = (fn_t *) GC_MALLOC(num*sizeof(fn_t));
      int i;

      for (i = 0; i < num; i++)
         farr[i] = params[i]();

      return __call(id, farr, num);
   };
}

fn_t __return_nil(void)
{
   function_t * f = current_fn;
   bind_t * bind = find_symbol(sym_lookup("return"));
   gen_t * g = bind->val;

   return [=] () {
      g->type = t_nil;
      longjmp(*(f->exc), 1);
      return nil;
   };
}

del_t _return_nil(void)
{
   return [=] () {
      return __return_nil();
   };
}

fn_t __return(fn_t val)
{
   function_t * f = current_fn;
   bind_t * bind = find_symbol(sym_lookup("return"));
   gen_t * g = bind->val;

   return [=] () {
      const gen_t * v = val();
      g->type = v->type;
      g->val = v->val;
      longjmp(*(f->exc), 1);
      return nil;
   };
}

del_t _return(del_t val)
{
   return [=] () {
      fn_t v = val();
   
      return __return(v);
   };
}
