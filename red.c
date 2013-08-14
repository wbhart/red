#include "parser.h"

int main(void)
{
   long i;

   GC_INIT();
   init();

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
