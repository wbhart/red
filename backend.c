#include "backend.h"

/******************************************************************************

   Objects

******************************************************************************/

function_t * current_fn;

gen_t * nil;

void init(void)
{
   sym_tab_init();
   scope_init();
   types_init();
   
   nil = (gen_t *) GC_MALLOC(sizeof(gen_t));
   nil->type = t_nil;

   current_fn = NULL;
}

/******************************************************************************

   Call constructors

******************************************************************************/

fn_t INT(long i)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.i = i; 
      g->type = t_int; 
      return g; 
   };
}

fn_t UINT(unsigned long u)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.u = u; 
      g->type = t_uint; 
      return g; 
   };
}

fn_t DOUBLE(double d)
{
   return [=] () { 
      gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t)); 
      g->val.d = d; 
      g->type = t_double; 
      return g; 
   };
}

fn_t PRINT(fn_t f)
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

fn_t PRINTLN(fn_t f)
{
   fn_t p = PRINT(f);
   return [=] () {
      p();
      printf("\n");
      return nil;
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

BINOP(ADD, +)
BINOP(SUB, -)
BINOP(MUL, *)
BINOP(DIV, /)

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

BINOPEQ(ADDEQ, +=)
BINOPEQ(SUBEQ, -=)
BINOPEQ(MULEQ, *=)
BINOPEQ(DIVEQ, /=)

fn_t MOD(fn_t a, fn_t b)
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

fn_t MODEQ(fn_t a, fn_t b)
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

REL_BIN(LT, <)
REL_BIN(GT, >)
REL_BIN(LE, <=)
REL_BIN(GE, >=)
REL_BIN(EQ, ==)
REL_BIN(NE, !=)

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

OP_UN(INC, ++)
OP_UN(DEC, --)

fn_t IDENT(const char * id)
{
   sym_t * sym = sym_lookup(id);
   bind_t * bind = find_symbol(sym);
   gen_t * g = bind->val;
   
   return [=] () {
      return g;
   };
}

fn_t ASSIGN(fn_t var, fn_t val)
{
   return [=] () {
      gen_t * g1 = var();
      const gen_t * g2 = val();

      g1->type = g2->type;
      g1->val = g2->val;

      return g1;
   };
}

fn_t VAR(const char * id, fn_t val)
{
   gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   bind_symbol(sym_lookup(id), g);

   fn_t var = IDENT(id);

   return ASSIGN(var, val);
}

fn_t BLOCK_FN(const fn_t * arr, int num)
{
   block_t * b = (block_t *) GC_MALLOC(sizeof(block_t));
   gen_t * g = (gen_t *) GC_MALLOC(sizeof(gen_t));
   g->type = t_break;
   g->val.ptr = (void *) b;
   b->scope = current_scope;
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

fn_t WHILE(fn_t cond, fn_t stmt)
{
   return [=] () {
      while (cond()->val.i)
         stmt();

      return nil;
   };
}

void NEW_FUNCTION_FN(const char * id, const char ** params, int num_params)
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

fn_t FUNCTION_FN(const fn_t * arr, int num)
{
   function_t * f = current_fn; 
   int i;

   f->fn = [=] () {
      int i, jval= 0;
      
      if (!(jval = setjmp(f->exc)))
      {
         for (i = 0; i < num; i++)
            arr[i]();
      } 

      return nil;
   };

   return [=] () {
      return nil;
   };
}

fn_t CALL_FN(const char * id, const fn_t * params, int num)
{
   bind_t * bind = find_symbol(sym_lookup(id));
   gen_t * g = bind->val;
   function_t * f = (function_t *) g->val.ptr;
   gen_t ** vals = f->vals;
   fn_t fn = f->fn;
   env_t * save = current_scope;
   current_scope = f->scope;
   bind = find_symbol(sym_lookup("return"));
   g = bind->val;

   return [=] () {
      gen_t * p_save = (gen_t *) alloca(num*sizeof(gen_t));
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

fn_t RETURN_NIL(void)
{
   function_t * f = current_fn;
   bind_t * bind = find_symbol(sym_lookup("return"));
   gen_t * g = bind->val;

   return [=] () {
      g->type = t_nil;
      longjmp(f->exc, 1);
      return nil;
   };
}

fn_t RETURN(fn_t val)
{
   function_t * f = current_fn;
   bind_t * bind = find_symbol(sym_lookup("return"));
   gen_t * g = bind->val;

   return [=] () {
      const gen_t * v = val();
      g->type = v->type;
      g->val = v->val;
      longjmp(f->exc, 1);
      return nil;
   };
}