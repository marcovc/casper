%{
#include <string>
#include <string.h>
#include <iostream>
#include "driver.h"

using namespace std;
/* Work around an incompatibility in flex (at least versions
   2.5.31 through 2.5.33): it generates code that does
   not conform to C89. See Debian bug 333231
   <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>. */
# undef yywrap
# define yywrap() 1
	 /* By default yylex returns int, we use token_type.
		Unfortunately yyterminate by default returns 0, which is
		not of token_type. */
	 
#define YY_USER_ACTION yylloc->columns(yyleng);
typedef yy::Flatzinc_parser::token token;
	 
#define yyterminate() return token::END  

//static int yy_init = 1;
//istream *yyin;
//ostream *yyout;

%}

%option noyywrap nounput batch debug

ident           [A-Za-z][A-Za-z0-9_]*
string_literal  \"[^"\n]*\"
int_literal     -?[0-9]+|-?0x[0-9A-Fa-f]+|-?0o[0-7]+
float_literal   -?[0-9]+\.[0-9]+|-?[0-9]+\.[0-9]+[Ee][-+]?[0-9]+|-?[0-9]+[Ee][-+]?[0-9]+

%%

"array"         { return token::ARRAY;     }
"bool"          { return token::BOOL;      }
"constraint"    { return token::CONSTRAINT;}
"false"         { return token::FALSE;     }
"float"         { return token::FLOAT;     }
"int"           { return token::INT;       }
"minimize"      { return token::MINIMIZE;  }
"maximize"      { return token::MAXIMIZE;  }
"of"            { return token::OF;        }
"predicate"	 	{ return token::PREDICATE; }
"satisfy"       { return token::SATISFY;   }
"set"           { return token::SET;       }
"solve"         { return token::SOLVE;     }
"true"          { return token::TRUE;      }
"var"           { return token::VAR;       }

"("				{ return token::LPAR;	   }
")"				{ return token::RPAR;	   }
"["				{ return token::LBR;	   }
"]"				{ return token::RBR;	   }
"{"				{ return token::LCUR;	   }
"}"				{ return token::RCUR;	   }
"="				{ return token::EQ;	   	   }
";"				{ return token::SEMICOLON; }
":"				{ return token::COLON;     }
","				{ return token::COMMA; 	   }
\.\.            { return token::DOTDOT;    }
::              { return token::COLONCOLON;}

    /* Attributed tokens */
{ident} { 
        yylval->string_val = new std::string(strdup(yytext));  
        return token::IDENT; 
    }
{string_literal} { 
        yylval->string_val = new std::string(strdup(yytext));  
        return token::STRING_LITERAL; 
    }
{int_literal} {
        /*
	** atoi() doesn't recognise our hex and octal numbers, so we must
	** handle them ourselves.
        **
        ** XXX: this code doesn't detect if the integer literals overflow an
        ** 'int'.  (In particular, 'atoi' doesn't detect overflow errors,
        ** but 'strtol' does, so that could be used for the decimal integer
        ** case.)
        ** 
        ** XXX: I think this code will incorrectly parse negative octal and
        ** hexadecimal numbers -- the two conditions will fail due to the
        ** leading '-', so 'atoi' will be invoked, which will fail, and
        ** yylval->int_val will be set to something bogus (probably zero).
	*/
        if        ('0' == yytext[0] && 'x' == yytext[1])  {
            int i = 2, x = 0;
            while ('\0' != yytext[i]) {
                x *= 16;
                if ('0' <= yytext[i] && yytext[i] <= '9') {
                    x += (yytext[i] - '0');
                } else {
                    x += (yytext[i] - 'a' + 10);
                }
                i++;
            }
            yylval->int_val = x;

        } else if ('0' == yytext[0] && 'o' == yytext[1])  {
            int i = 2, x = 0;
            while ('\0' != yytext[i]) {
                x *= 8;
                x += (yytext[i] - '0');
                i++;
            }
            yylval->int_val = x;

        } else {
            yylval->int_val = atoi(yytext);
        }
        return token::INT_LITERAL; 
    }
{float_literal} {
        yylval->float_val = atof(yytext);
        return token::FLOAT_LITERAL; 
    }

    /* Skip newlines, whitespace and comments, return any non-matched
     	 character. */
\n          ;
[ \t]       ;
%.*         ;
.           { std::cerr << "Unexpected symbol" << std::endl; }

%%
void FlatzincDriver::scan_begin () {
  yy_flex_debug = trace_scanning;
  if (!(yyin = fopen (file.c_str (), "r")))
	error (std::string ("cannot open ") + file);
}

void FlatzincDriver::scan_end () {
  fclose (yyin);
}
