%{
#include <stdio.h>
#include <iostream>
#include "gen_framework.h"
extern int yylex();



extern int lineNumber;
int yyerror(const char *msg)
	{
		fprintf(stderr, "Linha %d: %s\n", lineNumber+1, msg);	
		return 1;
		
	}

%}
%token PROJECT
%token TEXT
%token DATE
%token TIME
%token LONG
%token INT
%token FILE_
%token REAL
%token BOOL
%token ENTITY
%token LBRACE
%token RBRACE
%token PK
%token UNIQUE
%token SEMICOLON
%token RELATIONSHIP
%token BILATERAL
%token LPAR
%token RPAR
%token FROM
%token TO
%token UPTO
%token ARITY
%token IDENTIFIER
%token WEAK


%type <type> INT
%type <type> REAL
%type <type> LONG
%type <type> DATE
%type <type> TIME
%type <type> BOOL
%type <type> FILE_
%type <type> TEXT
%type <str> IDENTIFIER
%type <chr> ARITY
%type <fields> Fields
%type <field> Field
%type <entity> Entity
%type <rel> VarRelationship
%type <rel> Relationship
%type <type> Type

%union {
	gen::Field *field;
	gen::Fields *fields;
	char *str;
	char chr;
	gen::Entity *entity;
	gen::Relationship *rel;
	gen::Type type;
}
%%

File: PROJECT IDENTIFIER Parts {
		gen::projectName = $2;
	}
	;

Parts: 
	Part
	| Parts Part
	;

Part: 
	Entity {
		gen::entities.push_back($1);
	}
	| VarRelationship {
		gen::relationship.push_back($1);
	}
	;

Entity: 
	ENTITY IDENTIFIER LBRACE Fields RBRACE {
		$$ = new gen::Entity($2, $4);
	}; 

Fields: 
	Field {
		$$ = new gen::Fields();
		$$->getFields().push_back($1);
	}
	| Fields Field {
		$$ = new gen::Fields();
		for(int i=0;i<$1->getFields().size(); i++) {
			$$->getFields().push_back($1->getFields()[i]);
		}
		$$->getFields().push_back($2);
	}
	;

Field:
	Type IDENTIFIER SEMICOLON {
		$$ = new gen::Field($1, $2, false, false);
	}
	| PK Type IDENTIFIER SEMICOLON {
		$$ = new gen::Field($2, $3, true, true);
	}
	| UNIQUE Type IDENTIFIER SEMICOLON {
		$$ = new gen::Field($2, $3, false, true);
	}
	;
	
Type:
	INT
	| REAL
	| TEXT
	| FILE_
	| DATE
	| TIME
	| LONG
	| BOOL
	;
	
VarRelationship: 
	WEAK Relationship {
		$2->setWeak(true);
		$$ = $2;
	}
	| Relationship {
		$$ = $1;
	}
	;
	
Relationship:
	RELATIONSHIP IDENTIFIER FROM IDENTIFIER TO IDENTIFIER ARITY UPTO ARITY {
		gen::Relationship::Arity arity;
		if($7 == '1' && $9 == 'N') {
			arity = gen::Relationship::ONE_TO_MANY;
		} else if($7 == '1' && $9 == '1') {
			arity = gen::Relationship::ONE_TO_ONE;
		} else if($7 == 'N' && $9 == 'N') {
			arity = gen::Relationship::MANY_TO_MANY;
		} else if($7 == 'N' && $9 == '1') {
			arity = gen::Relationship::MANY_TO_ONE;
		}
		
		gen::Entity *from = NULL;
		for(int i=0; i<gen::entities.size(); i++) {
			if(gen::entities[i]->getName() == $4) {
				from = gen::entities[i];
				
				break;
			}
		}
		if(!from) 
			return yyerror("Entidade no from não existe\n");
		
		gen::Entity *to = NULL;
		for(int i=0; i<gen::entities.size(); i++) {
			if(gen::entities[i]->getName() == $6) {
				to = gen::entities[i];
				break;
			}
		}
		if(!to) 
			return yyerror("Entidade no to não existe\n");
		
		$$ = new gen::Relationship(string($2), from, to, arity);
		from->getRelationship().push_back($$);
	}
	| RELATIONSHIP IDENTIFIER FROM IDENTIFIER TO IDENTIFIER ARITY UPTO ARITY BILATERAL LPAR IDENTIFIER RPAR {
		gen::Relationship::Arity arity;
		if($7 == '1' && $9 == 'N') {
			arity = gen::Relationship::ONE_TO_MANY;
			std::cout << $4 << " 1..N " << $6 << endl;
		} else if($7 == '1' && $9 == '1') {
			arity = gen::Relationship::ONE_TO_ONE;
			std::cout << $4 << " 1..1 " << $6 << endl;
		} else if($7 == 'N' && $9 == 'N') {
			arity = gen::Relationship::MANY_TO_MANY;
			std::cout << $4 << " N..N " << $6 << endl;
		} else if($7 == 'N' && $9 == '1') {
			arity = gen::Relationship::MANY_TO_ONE;
			std::cout << $4 << " N..1 " << $6 << endl;
		}
		
		gen::Entity *from = NULL;
		for(int i=0; i<gen::entities.size(); i++) {
			if(gen::entities[i]->getName() == $4) {
				from = gen::entities[i];
				break;
			}
		}
		if(!from) 
			return yyerror("Entidade no from não existe\n");
		
		gen::Entity *to = NULL;
		for(int i=0; i<gen::entities.size(); i++) {
			if(gen::entities[i]->getName() == $6) {
				to = gen::entities[i];
				break;
			}
		}
		if(!to) 
			return yyerror("Entidade no to não existe\n");
		$$ = new gen::Relationship(string($2), from, to, arity, true, string($12));
		from->getRelationship().push_back($$);
		to->getRelationship().push_back(new gen::Relationship(string($12), to, from, gen::flipArity(arity), true, string($2)));
	}
	;

