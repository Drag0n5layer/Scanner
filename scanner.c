/* Filename: scanner.c
/* PURPOSE:
 *    SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
 *    as required for CST8152, Assignment #2
 *    scanner_init() must be called before using the scanner.
 *    The file is incomplete;
 *    Provided by: Svillen Ranev
 *    Version: 1.13.02
 *    Date: 25 September 2013
 *******************************************************************
 *    REPLACE THIS HEADER WITH YOUR HEADER
 *******************************************************************
 */

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
 * to suppress the warnings about using "unsafe" functions like fopen()
 * and standard sting library functions defined in string.h.
 * The define does not have any effect in Borland 5.02 projects.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>	/* standard input / output */
#include <ctype.h>	/* conversion functions */
#include <stdlib.h>	/* standard library functions and constants */
#include <string.h>	/* string functions */
#include <limits.h>	/* integer types constants */
#include <float.h>	/* floating-point types constants */
#include <math.h>	/* math functions, like pow() */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

/* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */ 
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atool(char * lexeme); /* converts octal string to decimal value */

 int scanner_init(Buffer * sc_buf) {
  	if(b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
	b_set_getc_offset(sc_buf,0);/* in case the buffer has been read previously  */
   b_reset(str_LTBL);
   line = 1;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
 }

 Token mlwpar_next_token(Buffer * sc_buf)
 {
   Token t; /* token to return after recognition */
   unsigned char c; /* input symbol */
   int state = 0; /* initial state of the FSM */
   short lexstart;  /*start offset of a lexeme in the input buffer */
   short lexend;    /*end   offset of a lexeme in the input buffer */
   int accept = NOAS; /* type of state - initially not accepting */ 
  int i;        /* Loop counter */                                    

/* 
lexstart is the offset from the beginning of the char buffer of the
input buffer (sc_buf) to the first character of the current lexeme,
which is being processed by the scanner.
lexend is the offset from the beginning of the char buffer of the
input buffer (sc_buf) to the last character of the current lexeme,
which is being processed by the scanner.

*/ 




        while (1){ /* endless loop broken by token returns it will generate a warning */


c =  b_getc(sc_buf);



/* special cases or token driven processing */

// WRITE YOUR CODE FOR PROCESSING THE SPECIAL CASES HERE. 
// COMMENTS AND STRING LITERALS ARE ALSO PROCESSED HERE.


// WHAT FOLLOWS IS A PSEUDO CODE. YOU CAN USE switch STATEMENT
// INSTEAD OF if-else TO PROCESS THE SPECIAL CASES
// DO NOT FORGET TO COUNT THE PROGRAM LINES
switch(c){
  /*
  * check for empty space
  */
  case ' ':
  continue;
  /* it will never reach here, but jsut for completenesss*/
  break; 

  /* check for comment token */
  case'!':
  /*  get the next character */
  c = b_getc(sc_buf);
  if (c == '<'){
    /*
    * check if it reaches the end of line
    */
    while(c != '\n'){
      c = b_getc(sc_buf);
    }
    continue;
    /* check for relational operator token */
  }else if (c == '='){
    t.code = REL_OP_T;
    t.attribute.log_op = NE;
  }else{
    /* error what about != */
    /*
    IF (c == '!') TRY TO PROCESS COMMENT
    IF THE FOLLOWING IS NOT CHAR IS NOT < REPORT AN ERROR
  ELSE IN A LOOP SKIP CHARACTERS UNTIL \n THEN continue;
    */
  while(c != '\n'){
    c = b_getc(sc_buf);
  }
  t.code = ERR_T;
  // discard the rest of the wrong comment until \n
}
return t;
break;

// /* check for arithmetic variable identifier token */
// case'i':
// case'o':
// case'd':
// case'n':
// c = b_getc(sc_buf);
// while (c != ' '){
//   c = b_getc(sc_buf);
//   if (c == '#'){
//     t.code = SVID_T;
//     return t;
//   }
// }
// t.code = AVID_T;
// return t;
// break;

// case '#':
// c = b_getc(sc_buf);
// if (c == ' '){
//   b_retract(sc_buf);
//   c = b_getc(sc_buf);
//   while(c != ' ' || c != '\n'){
//     b_retract(sc_buf);
//     c = b_getc(sc_buf);
//   }
//   c = b_getc(sc_buf);
//   t.code = AVID_T;
// }else{
//   t.code = ERR_T;
// }
// return t;
// break;


/* check for string concatenation operator token */
case '<':
c = b_getc(sc_buf);
/* check for relational operator token */
if (c == '>'){
  t.code = SCC_OP_T;
}else{
  b_retract(sc_buf);
  t.code = REL_OP_T;
  t.attribute.log_op = LT;
}
return t;
break;

/* check for relational operator token */
case '>':
t.code = REL_OP_T;
t.attribute.log_op = GT;
return t;

/* check for assignment operator token */
case '=':
c = b_getc(sc_buf);
/* check for relational operator token */
if (c == '='){
  t.code = REL_OP_T;
  t.attribute.log_op = EQ;
}else{
  b_retract(sc_buf);
  t.code = ASS_OP_T;
}
return t;
break;

/* check for arithmetic variable identifier token */
case '+':
t.code = ART_OP_T; 
t.attribute.arr_op = PLUS;
return t;
break;
case '-':
t.code = ART_OP_T; 
t.attribute.arr_op = MINUS;
return t;
break;
case '*':
t.code = ART_OP_T; 
t.attribute.arr_op = MULT;
return t;
break;
case '/':
t.code = ART_OP_T; 
t.attribute.arr_op = DIV;
return t;
break;

/* check for logical operator token */
case '.':
c = b_getc(sc_buf);
switch(c){
  case 'A' :
  c = b_getc(sc_buf);
  if (c == 'N'){
    c = b_getc(sc_buf);
    if(c == 'D'){
      c = b_getc(sc_buf);
      if (c == '.'){
        t.code = LOG_OP_T;
        t.attribute.log_op = AND;
      }else{
        t.code = ERR_T;
      }
    }
    else{
      t.code = ERR_T;
    }
  }else{
    t.code = ERR_T;
  }
  break;
  case 'O':
  c = b_getc(sc_buf);
  if (c == 'R'){
    c = b_getc(sc_buf);
    if (c == '.'){
      t.code = LOG_OP_T;
      t.attribute.log_op = OR;
    }else{
      t.code = ERR_T;
    }
  }else{
    t.code = ERR_T;
  }
  break;
  default:
  t.code = ERR_T;
  break;
}
return t;
break;
/* check for left parenthesis token */
case '(':
  t.code = LPR_T;
  return t;
  break;
  /* check for right parenthesis token */
  case ')':
t.code = RPR_T;
return t;
break;

  /* check for left brace toekn */
case '{':
t.code = LBR_T;
return t;
break;
   /* check for right brace token */
case '}':
t.code = RBR_T;
return t;
break;

  /* #define KW_T     16  /* Keyword token */


case '"':
c = b_getc(sc_buf);
b_setmark(sc_buf, b_get_getc_offset(sc_buf));
while(c!='"'){
  c = b_getc(sc_buf);
  b_addc(str_LTBL, c);
}
b_addc(str_LTBL, "\0");
t.code = STR_T;
t.attribute.kwt_idx = ; 

break;

  /* check for comma token */
case ',':
t.code = COM_T;
return t;

  /* check for End of statement */
case ';':
t.code = EOS_T;
return t;
break;
default:

break;
}

	/* Process state transition table */
if (isdigit(c) || isalpha(c)) {
  lexstart = b_get_getc_offset(sc_buf);
		b_setmark(sc_buf, lexstart); /* Set the mark at the beginning of the lexeme */

		/* CODE YOUR FINATE STATE MACHINE HERE (FSM or DFA)

		FSM0. Begin with state = 0 and the input character c 
		FSM1. Get the next state from the transition table calling                       
			state = get_next_state(state, c, &accept);
		FSM2. Get the next character
		FSM3. If the state is not accepting (accept == NOAS), go to step FSM1
			If the step is accepting, token is found, leave the machine and
			call the accepting function as described below.
		*/

      while (accept == NOAS) {
       state = get_next_state(state, c, &accept);
       c = b_getc(sc_buf);
     }
   }

	lex_buf = b_create(b_getcapacity(sc_buf), sc_buf->inc_factor, b_getmode(sc_buf)); /* Create a temporary buffer */
   lexstart = b_getmark(sc_buf);

	/* Retract getc_offset if the final state is a retracting state */
   if (state == ASWR) {
    b_retract(lex_buf);
  }

	lexend = b_get_getc_offset(lex_buf); /* Set lexend to getc_offset */

	/* Copy the lexeme between lexstart and lexend from the input buffer into lex_buf */
  for (i = lexstart; i < lexend; i++) {
    b_addc(lex_buf, b_getc(sc_buf));
  }

	/* Check if token code is a variable identifier, Keyword, floating point or integer */
  if (t.code == AVID_T || t.code == SVID_T || t.code == KW_T || t.code == FPL_T || t.code == INL_T) {
    aa_table[state](lex_buf->ca_head);
  }

  b_destroy(lex_buf);
  return t;

    /* CHECK OTHER CHARS HERE if NEEDED, SET A TOKEN AND RETURN IT.
     FOR ILLEGAL CHARACTERS SET ERROR TOKEN. 
     THE ILLEGAL CHAR IS THE ATTRIBUTE OF THE ERROR TOKEN 
     IN A CASE OF RUNTIME ERROR, THE FUNCTION MUST STORE 
     A NON-NEGATIVE NUMBER INTO THE GLOBAL VARIABLE scerrnum
     AND RETURN AN ERROR TOKEN. THE ERROR TOKEN ATTRIBUTE MUST
     BE THE STRING "RUN TIME ERROR: "  */              
   }//end while(1)
 }


 int get_next_state(int state, char c, int *accept) {
   int col;
   int next;
   col = char_class(c);
   next = st_table[state][col];

	#ifdef DEBUG
   printf("Input symbol: %c Row: %d Column: %d Next: %d \n",c,state,col,next);
	#endif

	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/

	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem. 
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/ 
	#ifdef DEBUG
	if(next == IS){
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n",c,state,col);
		exit(1);
	}
	#endif

	*accept = as_table[next];
	return next;
}

/*
THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
TABLE st_table FOR THE INPUT CHARACTER c.
SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
FOR EXAMPLE IF COLUMN 1 REPRESENTS [A-Z]
THE FUNCTION RETURNS 1 EVERY TIME c IS ONE
OF THE LETTERS A,B,...,Z.
*/
int char_class(char c) {
	int val;

	if (isalpha(c)) {
		val = 1;
	}
	else if (c == 0) {
		val = 2;
	}
	else if ((int)c >= 1 || (int)c <= 7) {
		val = 3;
	}
	else if ((int)c == 8 || (int)c == 9) {
		val = 4;
	}
	else if (c == '.') {
		val = 5;
	}
	else if (c == '#') {
		val = 6;
	}
	else {
		val = 7;
	}

	return val;
}


/* ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords (VID - AVID/KW)
WHEN CALLED THE FUNCTION MUST
1. CHECK IF THE LEXEME IS A KEYWORD.
   IF YES, IT MUST RETURN A TOKEN WITH THE CORRESPONDING ATTRIBUTE
   FOR THE KEYWORD. THE ATTRIBUTE CODE FOR THE KEYWORD
   IS ITS INDEX IN THE KEYWORD LOOKUP TABLE (kw_table in table.h).
   IF THE LEXEME IS NOT A KEYWORD, GO TO STEP 2.

2. SET a AVID TOKEN.
   IF THE lexeme IS LONGER than VID_LEN (see token.h) CHARACTERS,
   ONLY FIRST VID_LEN CHARACTERS ARE STORED 
   INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[](see token.h) .
   ADD \0 AT THE END TO MAKE A C-type STRING.
*/
   Token aa_func02(char lexeme[]) {
	Token t;				/* The Token to return */
	char temp[VID_LEN + 1];	/* The temporary array for the lexeme, truncated to VID_LEN */
	int i;					/* Loop counter */
	int keyword;			/* To hold the keyword index */

     keyword = isKeyword(lexeme);

	/* Check if the lexeme is a Keyword */
     if (keyword != -1) {
      t.code = KW_T;
      t.attribute.kwt_idx = keyword;
    }
    else {
		t.code = AVID_T; /* Set the token attribute */

		/* Only copy VID_LEN lexeme's into the vid_lex array */
      for (i = 0; i < VID_LEN; i++) {
       temp[i] = lexeme[i];
     }

		temp[i] = '\0'; /* Add \0 at the end to make the c-type string */
     t.attribute.vid_lex = temp;
   }

   return t;
 }


/* ACCEPTING FUNCTION FOR THE string variable identifier (VID - SVID)

WHEN CALLED THE FUNCTION MUST
1. SET a SVID TOKEN.
   IF THE lexeme IS LONGER than VID_LEN characters,
   ONLY FIRST VID_LEN-1 CHARACTERS ARE STORED
   INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[],
   AND THEN THE # CHARACTER IS APPENDED TO THE NAME.
   ADD \0 AT THE END TO MAKE A C-type STRING.
*/
   Token aa_func03(char lexeme[]) {
	Token t;				/* The Token to return */
	char temp[VID_LEN + 1];	/* The temporary array for the lexeme, truncated to VID_LEN */
	int i;					/* Loop counter */

	t.code = SVID_T; /* Set the token attribute */

	/* Only copy VID_LEN - 1 lexeme's into the vid_lex array */
     for (i = 0; i < (VID_LEN - 1); i++) {
      temp[i] = lexeme[i];
    }

	temp[i++] = '#'; /* Append the # character */
	temp[i] = '\0'; /* Add \0 at the end to make the c-type string */
    t.attribute.vid_lex = temp;

    return t;

  }


/* ACCEPTING FUNCTION FOR THE integer literal(IL) - decimal constant (DIL)

  THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING A DECIMAL CONSTANT
  TO A DECIMAL INTEGER VALUE, WHICH IS THE ATTRIBUTE FOR THE TOKEN.
  THE VALUE MUST BE IN THE SAME RANGE AS the value of 2-byte int in C.
  IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
  THE ERROR TOKEN ATTRIBUTE IS  lexeme
  return t;
=======
THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING A DECIMAL CONSTANT
TO A DECIMAL INTEGER VALUE, WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 2-byte int in C.
IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
THE ERROR TOKEN ATTRIBUTE IS  lexeme
*/
Token aa_func05(char lexeme[]) {
	Token t;	/* The Token to return */
	long temp;	/* Temporary variable, larger than an integer */

	t.code = INL_T; /* Set the token attribute */

	temp = atoi(lexeme);
	/* If integer out of range, return error token */
	if (temp > INT_MAX) {
		aa_func12(lexeme);
	}

	t.attribute.int_value = (int)temp;

	return t;
}


/* ACCEPTING FUNCTION FOR THE floating-point literal (FPL)

THE FUNCTION MUST CONVERT THE LEXEME TO A FLOATING POINT VALUE,
WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 4-byte float in C.
IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
THE ERROR TOKEN ATTRIBUTE IS  lexeme
*/
Token aa_func08(char lexeme[]) {
	Token t;		/* The Token to return */
	double temp;	/* Temporary variable, larger than an integer */

	t.code = FPL_T; /* Set the token attribute */

	temp = atoi(lexeme);
	/* If float out of range, return error token */
	if (temp > FLT_MAX || temp < FLT_MIN) {
		aa_func12(lexeme);
	}

	t.attribute.flt_value = (float)temp;

	return t;

}


/* ACCEPTING FUNCTION FOR THE integer literal(IL) - octal constant (OIL)


  THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING AN OCTAL CONSTANT
  TO A DECIMAL INTEGER VALUE WHICH IS THE ATTRIBUTE FOR THE TOKEN.
  THE VALUE MUST BE IN THE SAME RANGE AS the value of 2-byte int in C.
  THIS FUNCTION IS SIMILAR TO THE FUNCTION ABOVE AND THEY CAN BE
  COMBINED INTO ONE FUNCTION
  THE MAIN DIFFERENCE IE THAT THIS FUNCTION CALLS
  THE FUNCTION atool(char * lexeme) WHICH CONVERTS AN ASCII STRING
  REPRESENTING AN OCTAL NUMBER TO INTEGER VALUE
  IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
  THE ERROR TOKEN ATTRIBUTE IS  lexeme
=======
THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING AN OCTAL CONSTANT
TO A DECIMAL INTEGER VALUE WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 2-byte int in C.
THIS FUNCTION IS SIMILAR TO THE FUNCTION ABOVE AND THEY CAN BE
COMBINED INTO ONE FUNCTION
THE MAIN DIFFERENCE IE THAT THIS FUNCTION CALLS
THE FUNCTION atool(char * lexeme) WHICH CONVERTS AN ASCII STRING
REPRESENTING AN OCTAL NUMBER TO INTEGER VALUE
IN CASE OF ERROR (OUT OF RANGE) THE FUNCTION MUST RETURN ERROR TOKEN
THE ERROR TOKEN ATTRIBUTE IS  lexeme
*/
Token aa_func11(char lexeme[]) {
	Token t;	/* The Token to return */
	long temp;	/* Temporary variable, larger than an integer */

	t.code = INL_T; /* Set the token attribute */

	temp = atool(lexeme);
	/* If integer out of range, return error token */
	if (temp > INT_MAX) {
		aa_func12(lexeme);
	}

	t.attribute.int_value = (int)temp;

	return t;
}


/* ACCEPTING FUNCTION FOR THE ERROR TOKEN 

  THE FUNCTION SETS THE ERROR TOKEN. lexeme[] CONTAINS THE ERROR
  THE ATTRIBUTE OF THE ERROR TOKEN IS THE lexeme ITSELF
  AND IT MUST BE STORED in err_lex.  IF THE ERROR LEXEME IS LONGER
  than ERR_LEN caharacters, only the first ERR_LEN character are
  stored in err_lex.
=======
THE FUNCTION SETS THE ERROR TOKEN. lexeme[] CONTAINS THE ERROR
THE ATTRIBUTE OF THE ERROR TOKEN IS THE lexeme ITSELF
AND IT MUST BE STORED in err_lex.  IF THE ERROR LEXEME IS LONGER
than ERR_LEN caharacters, only the first ERR_LEN character are
stored in err_lex.
*/
Token aa_func12(char lexeme[]) {
	Token t;				/* The Token to return */
	char temp[ERR_LEN + 1];	/* The temporary array for the lexeme, truncated to VID_LEN */
	int i;					/* Loop counter */


	t.code = ERR_T; /* Set the token attribute */

	/* Only put ERR_LEN characters into the err_lex */
	for (i = 0; i < ERR_LEN; i++) {
		temp[i] = lexeme[i];
	}

	t.attribute.err_lex = temp;

	return t;
}


/* CONVERSION FUNCTION

  THE FUNCTION CONVERTS AN ASCII STRING
  REPRESENTING AN OCTAL INTEGER CONSTANT TO INTEGER VALUE
=======
THE FUNCTION CONVERTS AN ASCII STRING
REPRESENTING AN OCTAL INTEGER CONSTANT TO INTEGER VALUE
*/
long atool(char * lexeme) {
	long decimal;	/* Decimal value to return */
	long octal;		/* Temporary variable for calculations */
    int i = 0;			/* Loop counter */

	octal = strtol(lexeme);
	while(octal != 0) {
		decimal = decimal + (octal % 10) * pow(8, i++);
		octal = octal / 10;
	}

	return decimal;
}


/*
Purpose: To determine if the given lexeme is a keyword.
Author: Kyle Hinskens
History/Versions: 13.10.22
Called functions: strcmp()
Parameters: char * kw_lexeme
Return value: int The index of the found keyword, or -1 if not found.
Algorithm: Initializes the return variable to -1 and uses strcmp()
			in a loop to determine if the lexeme is a keyword. If
			found, the return value is set to the keyword index.
*/
      int isKeyword(char * kw_lexeme) {
	int keywordIndex = -1; /* To hold the index of the found keyword */

       for (i = 0; i < KWT_SIZE; i++) {

		/* Check to see if the lexeme is a Keyword from the table */
        if (strcmp(kw_lexeme, kw_table[i]) == 0) {
			keywordIndex = i; /* Set the index of the table as our return value */
			break; /* The lexeme was a Keyword, get out of the loop */
        }
      }

      return keywordIndex;
    }
