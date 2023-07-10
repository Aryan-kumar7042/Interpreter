#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define true 1
#define false 0
#define null NULL
#define error -1


#define memalloc(type, number) 			(type*)malloc(sizeof(type)*(number))
#define memrealloc(ptr, type, number) 	(type*)realloc(ptr, sizeof(type)*(number))
#define memfree(ptr) 	free(ptr)




#define LPARAN '('
#define RPARAN ')'
#define PLUS '+'
#define MINUS '-'
#define MULTIPLY '*'
#define DIVIDE '/'
#define POWER '^'

#define INTEGER 0
#define OPERATOR 1

// 80 bytes
#define ARRAY_BUF_NUM 10

#define OPERATOR_LENGTH 7
const char OPERATORS_LIST[OPERATOR_LENGTH] ={ PLUS, MINUS, DIVIDE, MULTIPLY, POWER, LPARAN, RPARAN };


typedef struct  array {
	int length;
	int *arr;
} List;	//the name list

//list implementaion
List* new_list() {
	List *list=memalloc(List, 1);	
	list->arr=null;
	list->length=0;
	return list;

}


int push_list(List *list, int num) {
	int length=list->length;
	//some 
	int last_block_elements=list->length%ARRAY_BUF_NUM;	//3

	if(length==0) {
		list->arr=memalloc(int, ARRAY_BUF_NUM);	//10 int allocate
	} else if(last_block_elements==0) {
		list->arr=memrealloc(list->arr, int, list->length+ARRAY_BUF_NUM);
	}
	list->arr[list->length++]=num;


	return true;
}


int pop_elem(List *list) {
	int last=list->arr[--list->length];
	if(list->length>0)
		list->arr=memrealloc(list->arr, int, list->length);	//stupid
	return last;
}
int at_index(List *list, int index) {
	if(list->length>index) {
		return list->arr[index];
	}
	fprintf(stderr, "No index found\n");
	return 0;
}

void free_list(List *ptr) {
	if (ptr->length>0)
		free(ptr);
	free(ptr);
}
void display_list(List *l) {
	for(int i=0;i<l->length;i++) {
		printf("%d\n", l->arr[i]);
	}
}
void insert_str(List *list, char ch) {
	int four_chars=0;
	int loop=0;

	if(list->length==0) {
		((char*)&four_chars)[0]=ch;
		((char*)&four_chars)[1]=0;
		push_list(list, four_chars);
		return;
	}

	four_chars=at_index(list, list->length-1);
	while(loop<sizeof(int)) {
		if(	((char*)&four_chars)[loop]==0 )
			break;
		loop++;
	}

	if(loop==sizeof(int)-1) {
		((char*)&four_chars)[loop]=ch;
		list->arr[list->length-1]=four_chars;
		
		four_chars=0;
		push_list(list, four_chars);
		return;
	}

	((char*)&four_chars)[loop++]=ch;
	((char*)&four_chars)[loop]=0;

	list->arr[list->length-1]=four_chars;
	return;	
}
char *input(char *label) {
	// This function deals with variable strin ginput
	// it is not perfect as using backspace or delete might corrupts the input
	// fixed 
	
	printf("%s", label);

	char c=true;	//dummy value

	List *string=new_list();

	while((c=getc(stdin))!='\n') {
		
		insert_str(string, c);
	}
	insert_str(string, 0); 	// an extra \0
	char *str=(char*)string->arr;
	free(string);
	return str;
}

int identify_char(char ch) {
	if(ch>='0' && ch<='9')
		return INTEGER;
	for(int i=0;i<OPERATOR_LENGTH;++i) {
		if(ch==OPERATORS_LIST[i])
			return OPERATOR;
	}
	return error;
}

#define token_type(tkn) tkn->token_type

typedef struct tkn {
	char token_type;
	int num;	// if integer then number
} Token;

Token *new_token(char token_type, int num) {
	Token *token=memalloc(Token ,1);
	token->token_type=token_type;
	token->num=num;
	return token;
}
#define free_token(tkn) free(tkn)

typedef struct lex {
	char *stream;
	int index;
	Token *current_token;
} Lexer;
Lexer *new_lexer(char *stream) {
	Lexer *lx=memalloc(Lexer, 1);
	lx->index=0;
	lx->stream=stream;
	lx->current_token=null;
	return lx;
}
#define inc_lex(l) l->index++
#define curr_char(l) l->stream[l->index]

void skip_shitespace(Lexer *lex) {
	while(lex->stream[lex->index]==' '){inc_lex(lex);}
}
Token * get_next_token(Lexer *lex) {
	skip_shitespace(lex);
	
	char current_char=curr_char(lex );
	int char_type=identify_char(current_char);
	Token *token=null;
	static List *number;
	number=new_list();
	if(char_type==OPERATOR) {
		token=new_token(OPERATOR, current_char);
		inc_lex(lex);
	} else if(char_type==INTEGER) {
		number->length=0;
		
		while(current_char!=' ' && char_type==INTEGER && char_type!='\n') {
			insert_str(number, current_char);
			inc_lex(lex);
			current_char=curr_char(lex);
			char_type=identify_char(current_char);
		}
		token=new_token(INTEGER, atoi((char *)number->arr));
		free(number->arr);
	}
	lex->current_token=token;
	return token;
}


void print_token(Token *tkn) {
	if(tkn->token_type==OPERATOR)
		printf("[OPERATOR %c]\n", tkn->num);
	
	if(tkn->token_type==INTEGER)
		printf("[INTEGER %d]\n", tkn->num);
}
// bad data relationship design
typedef struct nd {
	char node_type;	// OPERATOR or INTEGER
	int value; 	//OPERATOR or INTEGER value
	struct nd *left;
	struct nd *right;
} Node; 

Node *new_node(char node_type, int val, Node *left, Node *right) {
	Node *node=memalloc(Node, 1);
	node->left=left;
	node->right=right;

	node->node_type=node_type;
	node->value=val;
	return node;
}

Node *number(Lexer *);
Node *power(Lexer *);
Node *muldiv(Lexer *);
Node *addsub(Lexer *);

Node *number(Lexer *lex) {
	Token *tkn=get_next_token(lex);
	Node *n;
	print_token(tkn);
	if(tkn->token_type==INTEGER) {
		n= new_node(INTEGER, tkn->num, null, null);
	}
	if(tkn->token_type==OPERATOR && tkn->num==LPARAN) {
		n=power(lex);
		if(lex->current_token->token_type!=OPERATOR || lex->current_token->num!=RPARAN) 
		{
			fprintf(stderr, "Expected a right bracket.\n");
			exit(EXIT_FAILURE);

		}		
	}
	return n;
}
Node *power(Lexer *lex) {
	Node *node=number(lex);
	Token *t=get_next_token(lex);
	while(t!=null && t->token_type==OPERATOR && (t->num==POWER)) {
		node=new_node(OPERATOR, t->num, node, number(lex));
		t=get_next_token(lex);
	}
	return node;
}
/**Other functions for mul div add sub**/

double evalute(Node *node) {
	if(node->node_type==INTEGER)
		return node->value;

	int left=0, right=0;

	if(node->left->node_type==OPERATOR)
		left=evalute(node->left);
	else
		left=node->left->value;
	
	if(node->right->node_type==OPERATOR)
		right=evalute(node->right);
	else
		right=node->right->value;
	
	if(node->value==POWER)
		return pow(left, right);
	else if(node->value==PLUS)
		return left+right;
	else if(node->value==MINUS)
		return left-right;
	else if(node->value==MULTIPLY)
		return left*right;
	else if(node->value==DIVIDE)
		return left/right;
	exit(EXIT_FAILURE);


}

int main() {
	Lexer *lex=new_lexer(input("Enter your command\n"));
	Node *n=muldiv(lex);
	printf("\n%f\n",  evalute(n));
	

	return 0;
}



// By Aryan Kumar
