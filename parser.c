#include "parser.h"

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
parse_t * op_comma;

parse_t * new_op(sym_t * sym)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.sym = sym;
   p->tag = L_OP;

   return p;
}

void parse_init(void)
{
   parse_list = NULL;
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
   op_comma = new_op(sym_lookup(","));
}

void unread(parse_t * p)
{
   plist_t * node = (plist_t *) GC_MALLOC(sizeof(plist_t));

   node->next = parse_list;
   node->p = p;
   parse_list = node;
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
   p->tag = L_IDENT;

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
   p->tag = L_CHAR;

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
   p->tag = L_STRING;

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

parse_t * new_int(long l)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.l = l;
   p->tag = L_INT;

   return p;
}

parse_t * new_double(double d)
{
   parse_t * p = (parse_t *) GC_MALLOC(sizeof(parse_t));

   p->val.l = d;
   p->tag = L_DOUBLE;

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
      return op_plus;
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
      return op_times;
   case '/':
      return op_quo;
   case '%':
      return op_rem;
   case '(':
      return op_lparen;
   case ')':
      return op_rparen;
   case ',':
      return op_comma;
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
   plist_t * l = parse_list;
   
   if (l == NULL)
      return read_in(in);
   else
   {
      parse_list = l->next;
      return l->p;
   }
}