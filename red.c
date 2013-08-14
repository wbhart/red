#include "parser.h"

int main(void)
{
   long i;

   GC_INIT();
   init();

   /*_var("i", _int(0))()();
   _var("s", _int(0))()();
   
   del_t arr[] = { _inc(_ident("i")), _addeq(_ident("s"), _ident("i")) };

   _while(_lt(_ident("i"), _int(10)), _block(arr))()();

   _println(_ident("s"))()();

   const char * params[] = { "a" };

   del_t body[] = { _var("k", _ident("a")), _inc(_ident("k")), _return(_ident("k")) };

   _fn("bill", params, body)()();

   del_t vals[] = { _int(3) };

   _println(_call("bill", vals))()();

   const char * params2[] = { "n" };

   del_t vals3[] = { _sub(_ident("n"), _int(1)) };

   del_t body2[] = { _if_else(_le(_ident("n"), _int(1)), _return(_int(1)), _return(_mul(_ident("n"), _call("fac", vals3)))) };

   _fn("fac", params2, body2)()();

   del_t vals2[] = { _int(20) };

   _println(_call("fac", vals2))()();*/

   int jval = 0;
   del_t root;
   
   while (1)
   {
      printf("> ");

      if (!(jval = setjmp(exc)))
      {
         if (parse(&root, stdin))
            root()();
         else
         {
            printf("Syntax error!\n");
            parse_reset(stdin);
         }
      } else
         parse_reset(stdin);
   }

   return 0;
}
