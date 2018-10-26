
%option noyywrap

%{
%}

number      [0-9]+
identifier  [a-zA-Z\+\-]+

%%

"="             { return ASSIGN; }
"("             { return PARENOPEN; }
")"             { return PARENCLOSE; }
"{"             { return BRACEOPEN; }
"}"             { return BRACECLOSE; }
"["             { return BRACKOPEN; }
"]"             { return BRACKCLOSE; }
"."             { return DOT; }
","             { return COMMA; }

"if"            { return IF; }
"then"          { return THEN; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"loop"          { return LOOP; }
"do"            { return DO; }
"async"         { return ASYNC; }
"nil"           { return NIL; }
"undefined"     { return UNDEFINED; }
"closure"       { return CLOSURE; }

{number}        { yylval.value = yytext; return NUMBER; }
{identifier}    {
                    yylval.value = (char*) malloc(strlen(yytext) * sizeof(char));
                    strncpy(yylval.value, yytext, yyleng);
                    return IDENTIFIER; 
                }

[ \t\n\r]	    { /* ignore whitespace */ }
<<EOF>>         { return TERMINATOR; }
.               { return UNKNOWN; }

%%









