#include "parser.h"

plist_t * parse_start;
plist_t * parse_list;

parse_t * op_le;
parse_t * op_lt;
parse_t * op_ge;
parse_t * op_gt;
parse_t * op_eq;
parse_t * op_neq;
parse_t * op_set;
parse_t * op_plus;
parse_t * op_minus;
parse_t * op_times;
parse_t * op_quo;
parse_t * op_rem;
parse_t * op_rarrow;
parse_t * op_semi;
parse_t * op_colon;
parse_t * op_lparen;
parse_t * op_rparen;
parse_t * op_lbrace;
parse_t * op_rbrace;
parse_t * op_comma;
parse_t * op_inc;
parse_t * op_dec;
parse_t * op_addeq;
parse_t * op_subeq;
parse_t * op_muleq;
parse_t * op_diveq;
parse_t * op_modeq;

sym_t * res_if;
sym_t * res_else;
sym_t * res_while;
sym_t * res_fn;
sym_t * res_let;
sym_t * res_print;
sym_t * res_println;
sym_t * res_var;
sym_t * res_return;

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

parse_t * new_op(sym_t * sym)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.sym = sym;
   p->tag = P_OP;

   return p;
}

void parse_init(void)
{
   parse_list = parse_start = (plist_t *) GC_MALLOC(sizeof(plist_t));

   op_le = new_op(sym_lookup("<="));
   op_lt = new_op(sym_lookup("<"));
   op_ge = new_op(sym_lookup(">="));
   op_gt = new_op(sym_lookup(">"));
   op_eq = new_op(sym_lookup("=="));
   op_neq = new_op(sym_lookup("!="));
   op_set = new_op(sym_lookup("="));
   op_plus = new_op(sym_lookup("+"));
   op_minus = new_op(sym_lookup("-"));
   op_times = new_op(sym_lookup("*"));
   op_quo = new_op(sym_lookup("/"));
   op_rem = new_op(sym_lookup("%"));
   op_rarrow = new_op(sym_lookup("->"));
   op_semi = new_op(sym_lookup(";"));
   op_colon = new_op(sym_lookup(":"));
   op_lparen = new_op(sym_lookup("("));
   op_rparen = new_op(sym_lookup(")"));
   op_lbrace = new_op(sym_lookup("{"));
   op_rbrace = new_op(sym_lookup("}"));
   op_comma = new_op(sym_lookup(","));
   op_inc = new_op(sym_lookup("++"));
   op_dec = new_op(sym_lookup("--"));
   op_addeq = new_op(sym_lookup("+="));
   op_subeq = new_op(sym_lookup("-="));
   op_muleq = new_op(sym_lookup("*="));
   op_diveq = new_op(sym_lookup("/="));
   op_modeq = new_op(sym_lookup("%="));

   res_if = sym_lookup("if");
   res_else = sym_lookup("else");
   res_while = sym_lookup("while");
   res_fn = sym_lookup("fn");
   res_let = sym_lookup("let");
   res_print = sym_lookup("print");
   res_println = sym_lookup("println");
   res_var = sym_lookup("var");
   res_return = sym_lookup("return");
}

void parse_reset(FILE * in)
{
   while (fgetc(in) != '\n')
      ;
   parse_list = parse_start;
   parse_start->next = NULL;
}

void skip_whitespace(FILE * in)
{
   char c;

   while ((c = fgetc(in)) == ' ' || c == '\t' || c == '\r' || c == '\n')
      ;

   ungetc(c, in);
}

parse_t * new_ident(sym_t * sym)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.sym = sym;
   p->tag = P_IDENT;

   return p;
}

parse_t * read_ident(FILE * in)
{
   char buffer[100];
   char c = fgetc(in);
   int i = 0;

   buffer[i++] = c;
   while (i < 100 && isalnum(c = fgetc(in)) || c == '_')
      buffer[i++] = c;

   ungetc(c, in);
   buffer[i] = '\0';

   return new_ident(sym_lookup(buffer));
}

parse_t * new_char(char c)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.c = c;
   p->tag = P_CHAR;

   return p;
}

parse_t * read_char(FILE * in)
{
   char c = fgetc(in);
   fgetc(in);
   return new_char(c);
}

parse_t * new_str(char * str)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.str = str;
   p->tag = P_STRING;

   return p;
}

parse_t * read_string(FILE * in)
{
   char buffer[1000];
   char c = fgetc(in);
   int i = 0;

   do
   {
      while (i < 1000 && (c = fgetc(in)) != '\\' && c != '"')
         buffer[i++] = c;
      if (c == '\\')
      {
         buffer[i++] = c;
         buffer[i++] = fgetc(in);
      }
   } while (c != '"');

   buffer[i] = '\0';
   return new_str(buffer);
}

parse_t * new_int(long i)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.i = i;
   p->tag = P_INT;

   return p;
}

parse_t * new_double(double d)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.d = d;
   p->tag = P_DOUBLE;

   return p;
}

parse_t * read_number(FILE * in)
{
   char buffer[21];
   char c = fgetc(in);
   int i = 0;

   buffer[i++] = c;
   while (i < 21 && isdigit(c = fgetc(in)))
      buffer[i++] = c;

   buffer[i] = '\0';
        
   if (c == '.' || c == 'e' || c == 'E')
   {
      if (c == '.')
      {
         buffer[i++] = c;
         while (i < 21 && isdigit(c = fgetc(in)))
            buffer[i++] = c;
      }
      if (c == 'e' || c == 'E')
      {
         buffer[i++] = c;
         c = fgetc(in);
         if (i < 21 && c == '-')
         {
            buffer[i++] = c;
            c = fgetc(in);
         }
         buffer[i++] = c;
         while (i < 21 && isdigit(c = fgetc(in)))
            buffer[i++] = c;
      } 
      
      ungetc(c, in);
      buffer[i] = '\0';
      return new_double(strtod(buffer, NULL));
   } else
   {
      ungetc(c, in);
      return new_int(atol(buffer));;
   }

}

parse_t * read_op(FILE * in)
{
   char c = getc(in);
   
   switch (c)
   {
   case '<':
      c = getc(in);
      if (c == '=')
         return op_le;
      else 
      {
         ungetc(c, in);
         return op_lt;
      }
   case '>':
      c = getc(in);
      if (c == '=')
         return op_ge;
      else 
      {
         ungetc(c, in);
         return op_gt;
      }
   case '+':
      c = getc(in);
      if (c == '+')
         return op_inc;
      else if (c == '=')
         return op_addeq;
      else 
      {
         ungetc(c, in);
         return op_plus;
      }
   case '=':
      c = getc(in);
      if (c == '=')
         return op_eq;
      else 
      {
         ungetc(c, in);
         return op_set;
      }
   case '!':
      c = getc(in);
      return op_neq;
   case '-':
      c = getc(in);
      if (c == '>')
         return op_rarrow;
      else if (c == '-')
         return op_dec;
      else if (c == '=')
         return op_subeq;
      else if (isdigit(c))
      {
          ungetc(c, in);
          ungetc('-', in);
          return read_number(in);
      } else
      {
         ungetc(c, in);
         return op_minus;
      }
   case ';':
      return op_semi;
   case ':':
      return op_colon;
   case '*':
      c = getc(in);
      if (c == '=')
         return op_muleq;
      else
      {
         ungetc(c, in);
         return op_times;
      }
   case '/':
      c = getc(in);
      if (c == '=')
         return op_diveq;
      else
      {
         ungetc(c, in);
         return op_quo;
      }
   case '%':
      c = getc(in);
      if (c == '=')
         return op_modeq;
      else
      {
         ungetc(c, in);
         return op_rem;
      }
   case '(':
      return op_lparen;
   case ')':
      return op_rparen;
   case '{':
      return op_lbrace;
   case '}':
      return op_rbrace;
   case ',':
      return op_comma;
   default:
      printf("Unknown lexical element!\n");
      exception();
   }
}

parse_t * read_in(FILE * in)
{
   char c;
   
   skip_whitespace(in);
   
   c = getc(in);
   ungetc(c, in);

   if (isalpha(c) || c == '_')
      return read_ident(in);
   else if (c == '"')
      return read_string(in);
   else if (c == '\'')
      return read_char(in);
   else if (isdigit(c))
      return read_number(in);
   else
      return read_op(in);
}

parse_t * read(FILE * in)
{
   if (parse_list->next == NULL)
   {
      plist_t * p = (plist_t *) GC_MALLOC(sizeof(plist_t));
      p->p = read_in(in);
      parse_list->next = p;
   }

   parse_list = parse_list->next;
   return parse_list->p;
}

void rewind(plist_t * l)
{
   parse_list = l;
}

/**************************************************************************************

   Recursive descent parser

**************************************************************************************/

#define TOKEN(l, p, t)    \
   do {                   \
      p  = read(in);      \
                          \
      if (p->tag != t)    \
      {                   \
         rewind(l);       \
         return 0;        \
      }                   \
   } while (0)
   
#define TOKEN_RES(l, p, s)      \
   do {                         \
      p  = read(in);            \
                                \
      if (p->tag != P_IDENT     \
       || p->val.sym != s)      \
      {                         \
         rewind(l);             \
         return 0;              \
      }                         \
   } while (0)
   
#define TOKEN_IDENT(l, p)            \
   do {                              \
      p  = read(in);                 \
                                     \
      if (p->tag != P_IDENT          \
       || p->val.sym == res_if       \
       || p->val.sym == res_else     \
       || p->val.sym == res_while    \
       || p->val.sym == res_fn       \
       || p->val.sym == res_let      \
       || p->val.sym == res_print    \
       || p->val.sym == res_println  \
       || p->val.sym == res_var      \
       || p->val.sym == res_return)  \
      {                              \
         rewind(l);                  \
         return 0;                   \
      }                              \
   } while (0)
   
#define TOKEN_OP(l, p, op) \
   do {                    \
      p  = read(in);       \
                           \
      if (p != op)         \
      {                    \
         rewind(l);        \
         return 0;         \
      }                    \
   } while (0)
   
#define EXIT(l)  \
   do {          \
      rewind(l); \
      return 0;  \
   } while (0)

#define MARK(l)       \
   do {               \
      l = parse_list; \
   } while (0)

int p_int(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   
   MARK(l);
   TOKEN(l, p, P_INT);

   *del = _int(p->val.i);

   return 1;
}

int p_double(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   
   MARK(l);
   TOKEN(l, p, P_DOUBLE);

   *del = _double(p->val.d);

   return 1;
}

int p_print(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   del_t d;

   MARK(l);
   TOKEN_RES(l, p, res_print);
   TOKEN_OP(l, p, op_lparen);
   
   if (!p_expr(&d, in))
      EXIT(l);

   TOKEN_OP(l, p, op_rparen);
   
   *del = _print(d);

   return 1;
}

int p_println(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   del_t d;

   MARK(l);
   TOKEN_RES(l, p, res_println);
   TOKEN_OP(l, p, op_lparen);
   
   if (!p_expr(&d, in))
      EXIT(l);

   TOKEN_OP(l, p, op_rparen);
   
   *del = _println(d);

   return 1;
}

int p_ident(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;

   MARK(l);
   TOKEN_IDENT(l, p);
   *del = _ident(p->val.sym);

   return 1;
}

int p_appl(del_t * del, FILE * in)
{
   plist_t * l, * l2;
   parse_t * p1, * p2;
   del_t * arr = (del_t *) GC_MALLOC(5*sizeof(del_t));
   del_t d;
   int alloc = 5;
   int i = 0;

   MARK(l);
   TOKEN_IDENT(l, p1);
   TOKEN_OP(l, p2, op_lparen);

   if (p_expr(&d, in))
   {
      arr[i++] = d;
      
      MARK(l2);
      while ((p2 = read(in)) == op_comma)
      {
         if (!p_expr(&d, in))
            EXIT(l);
         else
         {
            if (alloc == i)
            {
               arr = (del_t *) GC_REALLOC(arr, (alloc + 5)*sizeof(del_t));
               alloc += 5;
            }
            arr[i++] = d;
         }
         MARK(l2);
      }
      rewind(l2);
   }

   TOKEN_OP(l, p2, op_rparen);
   
   *del = _call_fn(p1->val.sym, arr, i);
   
   return 1;
}

int p_primary(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;

   MARK(l);
   if (!p_int(del, in) && !p_double(del, in))
   {
      p = read(in);

      if (p == op_lparen)
      {
         if (!p_expr(del, in))
            EXIT(l);

         TOKEN_OP(l, p, op_rparen);
      } else
      {
         rewind(l);
         return p_appl(del, in) || p_ident(del, in);
      }
   }

   return 1;
}

int p_unary(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;

   MARK(l);
   if (p_ident(del, in))
   {
      p = read(in);
      if (p == op_inc)
         *del = _inc(*del);
      else if (p == op_dec)
         *del = _dec(*del);
      else
      {
         rewind(l);
         return p_primary(del, in);
      }
   } else
      return p_primary(del, in);

   return 1;
}

int p_infix10(del_t * del, FILE * in)
{
   plist_t * l1, * l2;
   parse_t * p;
   del_t d;

   MARK(l1);
   if (!p_unary(del, in))
      EXIT(l1);

   MARK(l2);
   while ((p = read(in)) == op_times || p == op_quo || p == op_rem)
   {
      if (!p_unary(&d, in))
         EXIT(l1);

      if (p == op_times)
         *del = _mul(*del, d);
      else if (p == op_quo)
         *del = _div(*del, d);
      else
         *del = _mod(*del, d);

      MARK(l2);
   }
   rewind(l2);

   return 1;
}

int p_infix20(del_t * del, FILE * in)
{
   plist_t * l1, * l2;
   parse_t * p;
   del_t d;

   MARK(l1);
   if (!p_infix10(del, in))
      EXIT(l1);

   MARK(l2);
   while ((p = read(in)) == op_plus || p == op_minus)
   {
      if (!p_infix10(&d, in))
         EXIT(l1);

      if (p == op_plus)
         *del = _add(*del, d);
      else
         *del = _sub(*del, d);

      MARK(l2);
   }
   rewind(l2);

   return 1;
}

int p_infix40(del_t * del, FILE * in)
{
   plist_t * l1, * l2;
   parse_t * p;
   del_t d;

   MARK(l1);
   if (!p_infix20(del, in))
      EXIT(l1);

   MARK(l2);
   while ((p = read(in)) == op_le || p == op_ge || p == op_lt || p == op_gt)
   {
      if (!p_infix20(&d, in))
         EXIT(l1);

      if (p == op_le)
         *del = _le(*del, d);
      else if (p == op_ge)
         *del = _ge(*del, d);
      else if (p == op_gt)
         *del = _gt(*del, d);
      else
         *del = _lt(*del, d);

      MARK(l2);
   }
   rewind(l2);

   return 1;
}

int p_infix50(del_t * del, FILE * in)
{
   plist_t * l1, * l2;
   parse_t * p;
   del_t d;

   MARK(l1);
   if (!p_infix40(del, in))
      EXIT(l1);

   MARK(l2);
   while ((p = read(in)) == op_eq || p == op_neq)
   {
      if (!p_infix40(&d, in))
         EXIT(l1);

      if (p == op_eq)
         *del = _eq(*del, d);
      else
         *del = _ne(*del, d);

      MARK(l2);
   }
   rewind(l2);

   return 1;
}

int p_opeq(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p, * q;
   del_t d;

   MARK(l);
   if (p_ident(&d, in))
   {
      if ((p = read(in)) == op_addeq || p == op_subeq || p == op_muleq || p == op_diveq || p == op_modeq)
      {
         if (!p_expr(del, in))
            rewind(l);
         else
         {
            if (p == op_addeq)
               *del = _addeq(d, *del);
            else if (p == op_subeq)
               *del = _subeq(d, *del);
            else if (p == op_muleq)
               *del = _muleq(d, *del);
            else if (p == op_diveq)
               *del = _diveq(d, *del);
            else if (p == op_modeq)
               *del = _modeq(d, *del);

            return 1;
         }
      } else
         rewind(l);
   } 
   
   return p_infix50(del, in);
}

int p_expr(del_t * del, FILE * in)
{
   return p_opeq(del, in);
}

int p_var(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p, * p2;

   MARK(l);
   TOKEN_RES(l, p, res_var);

   TOKEN(l, p, P_IDENT);

   TOKEN_OP(l, p2, op_set);

   if (!p_expr(del, in))
      EXIT(l);

   *del = _var(p->val.sym, *del);

   return 1;
}

int p_assign(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p, * p2;
   del_t d;

   MARK(l);
   
   TOKEN(l, p, P_IDENT);

   TOKEN_OP(l, p2, op_set);

   if (!p_expr(del, in))
      EXIT(l);

   d = _ident(p->val.sym);

   *del = _assign(d, *del);

   return 1;
}

int p_block(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   del_t d;
   del_t * arr = (del_t *) GC_MALLOC(10*sizeof(del_t));
   int alloc = 10;
   int i = 0;

   MARK(l);
   TOKEN_OP(l, p, op_lbrace);

   while (p_stmt(&d, in))
   {
      if (i == alloc)
      {
         arr = (del_t *) GC_REALLOC(arr, (alloc + 10)*sizeof(del_t));
         alloc += 10;
      }

      arr[i++] = d;
   }

   TOKEN_OP(l, p, op_rbrace);

   *del = _block_fn(arr, i);

   return 1;
}

int p_while(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   del_t d;

   MARK(l);
   TOKEN_RES(l, p, res_while);
   TOKEN_OP(l, p, op_lparen);

   if (!p_expr(&d, in))
      EXIT(l);
   
   TOKEN_OP(l, p, op_rparen);

   if (!p_stmt(del, in))
      EXIT(l);
   
   *del = _while(d, *del);

   return 1;
}

int p_if_else(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   del_t d, d2;

   MARK(l);
   TOKEN_RES(l, p, res_if);
   TOKEN_OP(l, p, op_lparen);

   if (!p_expr(&d, in))
      EXIT(l);
   
   TOKEN_OP(l, p, op_rparen);

   if (!p_stmt(del, in))
      EXIT(l);
   
   TOKEN_RES(l, p, res_else);

   if (!p_stmt(&d2, in))
      EXIT(l);
   
   *del = _if_else(d, *del, d2);
   
   return 1;
}

int p_fn(del_t * del, FILE * in)
{
   plist_t * l, * l2;
   parse_t * p1, *p2;
   del_t d;
   sym_t ** params = (sym_t **) GC_MALLOC(5*sizeof(sym_t *));
   del_t * stmts = (del_t *) GC_MALLOC(5*sizeof(del_t));
   int alloc = 5;
   int alloc2 = 5;
   int i = 0;
   int j = 0;

   MARK(l);
   TOKEN_RES(l, p1, res_fn);
   TOKEN_IDENT(l, p1);
   TOKEN_OP(l, p2, op_lparen);

   MARK(l2);
   p2 = read(in);
   if (p2->tag != P_IDENT)
      rewind(l2);
   else
      params[i++] = p2->val.sym;

   MARK(l2);
   while ((p2 = read(in)) == op_comma)
   {
      p2 = read(in);
      if (p2->tag != P_IDENT)
         EXIT(l);
      else
      {
         if (alloc == i)
         {
            params = (sym_t **) GC_REALLOC(params, (alloc + 5)*sizeof(sym_t *));
            alloc += 5;
         }
         params[i++] = p2->val.sym;
      }
      MARK(l2);
   }
   rewind(l2);

   TOKEN_OP(l, p2, op_rparen);

   TOKEN_OP(l, p2, op_lbrace);

   while (p_stmt(&d, in))
   {
      if (alloc2 == j)
      {
         stmts = (del_t *) GC_REALLOC(stmts, (alloc2 + 5)*sizeof(del_t));
         alloc2 += 5;
      }
      stmts[j++] = d;
   }
   
   TOKEN_OP(l, p2, op_rbrace);
   
   *del = _function(p1->val.sym, params, i, stmts, j);
   
   return 1;
}

int p_return(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   
   MARK(l);
   TOKEN_RES(l, p, res_return);

   if (!p_stmt(del, in))
   {
      TOKEN_OP(l, p, op_semi);
      *del = _return_nil();
   } else
      *del = _return(*del);

   return 1;
}

int p_stmt(del_t * del, FILE * in)
{
   plist_t * l;
   parse_t * p;
   
   MARK(l);
   if (p_print(del, in) || p_println(del, in) || p_var(del, in) || p_assign(del, in) || p_expr(del, in))
      TOKEN_OP(l, p, op_semi);
   else if (!p_block(del, in) && !p_while(del, in) && !p_if_else(del, in) && !p_fn(del, in) && !p_return(del, in))
      EXIT(l);

   return 1;
}

int parse(del_t * d, FILE * in)
{
   del_t del;
   
   if (!p_stmt(&del, in))
      return 0;

   *d = _println(del);

   return 1;
}