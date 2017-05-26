%token INT_VAL
%token FLOAT_VAL
%token SEMICOLON
%token PRINT_CMD
%token NOME_VAR
%token PLUS
%token MINUS
%token ASSIGN
%token MULT
%token DIV

%type <ident> NOME_VAR
%type <int_val> INT_VAL
%type <float_val> FLOAT_VAL
%type <float_val> Expressao
%type <float_val> Fator
%type <float_val> Termo

%union {

char *ident;
float float_val;
int  int_val;
}

%%

Programa: Comandos {printf("teste");};
	
Comandos: Comando
	|Comandos Comando;

Comando: PRINT_CMD Expressao SEMICOLON {printf("%f\n", $2);}
	|NOME_VAR ASSIGN Expressao SEMICOLON
    ;
	
Expressao: Fator PLUS Expressao {$$=$1+$3;}
   |Fator MINUS Expressao {$$=$1-$3;}
   ;
   
Fator: Termo MULT Fator {$$=$1*$3;}
	|Termo DIV Fator {$$=$1/$3;}
	|Termo {$$=$1;}
	;
Termo:
	NOME_VAR {$$=$1;}
	|INT_VAL {$$= (float) $1;}
	|FLOAT_VAL {$$=$1;}
;