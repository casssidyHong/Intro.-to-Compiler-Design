%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LIST_SOURCE                 appendToCurrentLine(yytext)
#define LIST_TOKEN(name)            do { LIST_SOURCE; if(opt_tok) printf("<%s>\n", name); } while(0)
#define LIST_RESERVED(name)         do { LIST_SOURCE; if(opt_tok) printf("<KW%s>\n", name); } while(0)
#define LIST_LITERAL(name, literal) do { LIST_SOURCE; if(opt_tok) printf("<%s: %s>\n", name, literal); } while(0)
#define MAX_LINE_LENG               512

uint32_t line_num = 1;
static char current_line[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *current_line_ptr = current_line;

static void appendToCurrentLine(const char *yytext_ptr);

%}

    /*Delimiter       (,|;|:|(|)|[|])*/
    /*Operator        (+|-|*|/|mod|:=|<|<=|<>|>=|>|=|and|or|not)*/
Reserved        (var|def|array|of|boolean|integer|real|string|true|false|while|do|if|then|else|for|to|begin|end|print|read|return)
Identifier      [a-zA-Z][a-zA-Z0-9]*
Oct_int         0[0-7]*
Int             0|([1-9][0-9]*)
Float           {Int}\.(0|[0-9]*[1-9])
OneNoneZeroFloat   0\.[0-9]*[1-9]
TwoNoneZeroFloat   [1-9][0-9]*\.(0|[0-9]*[1-9])
Scientific      ([1-9][0-9]*|{OneNoneZeroFloat}|{TwoNoneZeroFloat})[eE][-+]?{Int}

Whitespace      [ \t\n]
%x COMMENT
%x SingleLine
%x STRING

%%

<*>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, current_line);
    }
    ++line_num;
    current_line[0] = '\0';
    current_line_ptr = current_line;
}

"//&S\-"[^\n]* { opt_src = 0; }
"//&S\+"[^\n]* { opt_src = 1; LIST_SOURCE; }
"//&T\-"[^\n]* { opt_tok = 0; LIST_SOURCE; }
"//&T\+"[^\n]* { opt_tok = 1; LIST_SOURCE; }

    /*comments*/
"//"[^\n\&][^\n]* { LIST_SOURCE; }

"/*" { BEGIN(COMMENT); LIST_SOURCE; }
<COMMENT>"*/" {
    LIST_SOURCE;
    BEGIN(INITIAL);
}
<COMMENT>\n { }
<COMMENT>. { LIST_SOURCE; }

    /*{Delimiter} { LIST_TOKEN(delimiter); }*/
"," { LIST_TOKEN(","); }
";" { LIST_TOKEN(";"); }
":" { LIST_TOKEN(":"); }
"(" { LIST_TOKEN("("); }
")" { LIST_TOKEN(")"); }
"[" { LIST_TOKEN("["); }
"]" { LIST_TOKEN("]"); }

    /*Operator { LIST_TOKEN(operator); }*/
"+" { LIST_TOKEN("+"); }
"-" { LIST_TOKEN("-"); }
"*" { LIST_TOKEN("*"); }
"/" { LIST_TOKEN("/"); }
"mod" { LIST_TOKEN("mod"); }

":=" { LIST_TOKEN(":="); }
"<" { LIST_TOKEN("<"); }
"<=" { LIST_TOKEN("<="); }
"<>" { LIST_TOKEN("<>"); }
">=" { LIST_TOKEN(">="); }
">" { LIST_TOKEN(">"); }
"=" { LIST_TOKEN("="); }

"and" { LIST_TOKEN("and"); }
"or" { LIST_TOKEN("or"); }
"not" { LIST_TOKEN("not"); }

{Reserved} { LIST_RESERVED(yytext); }
{Scientific} { LIST_LITERAL("scientific", yytext); }
{Identifier} { LIST_LITERAL("id", yytext); }
{Int}  { LIST_LITERAL("integer", yytext); }
{Oct_int} { LIST_LITERAL("oct_integer", yytext); }
{Float} { LIST_LITERAL("float", yytext); }

"//&"[T|S][+|-].* {
    if (yytext[3] == 'T'){
        if (yytext[4] == '+') opt_tok = 1;
        else opt_tok = 0;
    }
    else if(yytext[3] == 'S'){
        if(yytext[4] == '+') opt_src = 1;
        else opt_tok = 0;
    }
}

    /*discard*/
{Whitespace} { LIST_SOURCE; }

    /*string*/
\"([^\"]|\"{2})*\" { 
    if (opt_tok) {
        char string[MAX_LINE_LENG];
        int j=0;
        int times=1;
        for(int i=1; i<yyleng-1; i++){
            if (yytext[i] == '\"' && yytext[i-1] == '\"'){
                times++;
                if (times%2 == 1){
                    string[j] = yytext[i];
                    j++;
                }
            }
            else{
                times = 1;
                string[j] = yytext[i];
                j++;
            }
        }
        string[j] = '\0';
        LIST_LITERAL("string", string);
    }
}

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void appendToCurrentLine(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *current_line_ptr = *yytext_ptr;
        ++current_line_ptr;
        ++yytext_ptr;
    }
    *current_line_ptr = '\0';
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    fclose(yyin);
    /* Free resources used by the scanner; otherwise, unused data structures
    will persist throughout the successive compiler phases. */
    yylex_destroy();

    return 0;
}