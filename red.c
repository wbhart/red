#include "backend.h"

int main(void)
{
   long i;

   GC_INIT();
   init();

   VAR("i", INT(0))();
   VAR("s", INT(0))();

   scope_up();
   
   fn_t arr[] = { INC(IDENT("i")), ADDEQ(IDENT("s"), IDENT("i")) };

   fn_t block = BLOCK(arr);

   scope_down();
   
   WHILE(LT(IDENT("i"), INT(10000000)), block)();

   PRINTLN(IDENT("s"))();

   scope_up();
   
   const char * params[] = { "a" };

   NEW_FUNCTION("bill", params);

   fn_t body[] = { VAR("k", IDENT("a")), INC(IDENT("k")), RETURN(IDENT("k")) };

   FUNCTION_BODY(body)();

   scope_down();
   
   fn_t vals[] = { INT(3) };

   fn_t root = PRINTLN(CALL("bill", vals));

   root();

   return 0;
}
