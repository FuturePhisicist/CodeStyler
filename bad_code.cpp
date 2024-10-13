#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <stdexcept>

int f1(int a) {return 3;};
int f2(int a) {return 2 * a;};

// Rule 4.5.1
char * copy_str(const char *instr) { // "char *" must be on a separate line, brace must be on a new line
	return NULL;
}

// Rule 4.5.2
int
random1() // void is required
{
	return 1;
}

// Rule 4.5.3
static void
callback_handler( int param1, // must be on a newline, like param2
		int param2)
{
}

// Rule 4.6.11
void
	func(void)
	{
	    for (;;) {
	        for (;;) {
	        }
			exit1:;
	// must be here
	    }
	}

// Rule 5.2
class Worker
{
    static const int VAL = 100; // use constexpr
};

// Rule 5.6
int
test(void)
{
    int a, b;
    scanf("%d%d", &a, &b);
    printf("%d", a + b); // must contain '\n' in the end, except if the analysis is char by char
    return 0;
}

int main(void)
{
	// https://github.com/blackav/cmc-os/blob/master/CodeStyle.md

	// Rule 1.1
	// Will only check string constants
	char s1[] = "aaa\xFA \xFF";

	// Rule 1.2
	// Will only check string constants
	char s2[] = "\t";

	// Rule 2.1
	// Check the indentation length of 4

	// Rule 2.2
	// Column limit: 120
	(1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1 && 1 == 1);

	// Rule 2.3
	if (2 > 1
		&& 1 < 2) { // needs one extra tab
		1 == 1;
	}

	// Rule 3.1
	int some_value; // I don't understand, what it's needed for

	// Rule 3.2
	int плохое_название;
	int vychislenie_kornya;

	// Rule 3.3
	enum {incorrect_case}; // must be UPPER_CASE

	// Rule 3.4
	int SOME_VAR; // must be lower_case

	void Some_Function(void); // must be lower_case

	goto LABEL; // must be lower_case
	LABEL: // must be lower_case

	// Rule 3.5
	float fValue;	// no Hungarian notation
	void *pBox;		// no Hungarian notation

	// Rule 3.6
	class binary_operation { };	// must be UpperCamelCase
	struct node_ { };			// must be UpperCamelCase

	// Rule 3.7
	int l, I; // look like 1

	// Rule 4.1
	struct StructureType { // brace must be on a new line
		int field1;
		int field2;
		char *field3;
	};

	typedef struct Node
	{
	struct Node *next;	// needs one extra tab
	double data;		// needs one extra tab
	} Node;

	struct Foo
	{
		int fudge;
	} foos[10]; // no definition of variable and struct together

	class Complex2
	{
		double m_re, m_im; // no "m_" in variable name
	};

	// Rule 4.2
	enum Colours { // brace must be on a new line
		RED,
		GREEN
	};

	typedef enum Colours1
	{
	RED_1,	// needs one extra tab
	GREEN_1	// needs one extra tab
	} Colours1;

	// Rule 4.3
	int* p;			// must be " *p"
	int* q, r;		// must be " *q", I DON'T UNDERSTAND THIS EXAMPLE
	char * * * s;	// no spaces between asterixes

	// Rule 4.4.1
	int matrix[3][2] =
	{
		4, 5, 10, 2, 1, 1 // must have braces inside
	};

	// Rule 4.4.2
	double summ
	= 0.0; // must be on previous line

	// Rule 4.4.3
	// ?

	// Rule 4.4.4
	typedef struct TimeInterval
	{
		int64_t start;
		int64_t stop;
	} TimeInterval;
	TimeInterval t1 = {0}; // must contain empty braces

	// Rule 4.4.5
	// non-constants initialized variables must be on separate lines
	int var1 = f1(3), var2 = f2(3);

	// Rule 4.5.1
	// SEE IN THE BEGINNING

	// Rule 4.5.2
	// SEE IN THE BEGINNING

	// Rule 4.5.3
	// SEE IN THE BEGINNING

	// Rule 4.6.1
	if (0) 
	{ // must be on previous line
		1 == 1;
	}

	// Rule 4.6.2
	// Look how to do it correctly
	if (0) { 1 == 1;
	}
	else {
		1 == 1;
	}

	// Rule 4.6.3
	// Look how to do it correctly
	if (0) {
		1 == 1;
	}
	else if (1) {
		1 == 1;
	} else {
		1 == 1; }

	// Rule 4.6.4
	// Look how to do it correctly
	while (0) {
    1 == 1;
    }

	// Rule 4.6.5
	// DOESN'T EXIST!!!

	// Rule 4.6.6
	// Look how to do it correctly
	do {
        1 == 1;
    }
    while (0);

	// Rule 4.6.7
	// Look how to do it correctly
	for 
		(int i = 0; i < 1; ++i) {
        1 == 1;
    }

	// Rule 4.6.8
	// Look how to do it correctly
	int expr = 2;
	switch (expr) {
    case 1:
        1 == 1;
        break;
    case 2:
        1 == 1;
        // break; MUST be here
    default:
        1 == 1;
        break;
    }

	// Rule 4.6.9
	if (0) printf("yes\n"); // printf must be in braces    

	// Rule 4.6.10
	int a;
	int b = 1;
	int c = 2;

	a = b, b = c; // each must be on a separate line

	// Rule 4.6.11
	// SEE IN THE BEGINNING

	// Rule 4.6.12
	// Использование оператора goto не рекомендуется. Допускается использование оператора goto при обработке ошибок и при выходе из вложенных циклов. Допускаются только переходы вперед по тексту программы.

	// Rule 4.6.13
	// Look how to do it correctly
	try {
    }
    catch (std::overflow_error e1) {
    } catch (std::invalid_argument e2) {}

	// Rule 4.7.1
    1+ 2, 1 !=0, 2>=1; // spaces must be before and after the binary operation

	// Rule 4.7.2
	typedef struct CheckSpaces
	{
		int field;
	} CheckSpaces;
	CheckSpaces check = {};
	CheckSpaces *check1 = &check;

	int arr[2][3] = {0};

    check. field, check1 ->field, arr [1] [2]; // '.', '->' and '[]'' must have no spaces

	// Rule 4.7.3
	a,b,c = 0;					// after a comma must be a space
    printf("%d\n",1);			// after a comma must be a space
    for (int i = 0;i < 10;++i)	// after a semicolon must be a space

	// Rule 4.7.4
    b = * (&a);	// unary operation requieres no spaces
    a ++;		// unary operation requieres no spaces
    *p ++ = 0;	// unary operation requieres no spaces

	// Rule 4.7.5
	printf ("%d\n", 1);				// no space needed before '('
    while ((c = getchar ()) != EOF) {}	// no space needed before '('

	// Rule 4.7.6
    ( 1 == 1 ); // after '(' and before ')' no space requires

	// Rule 4.7.7
	(unsigned char *)calloc(4, 4); // must be a space after type casting

	// Rule 4.7.8
	*(arr + 1), (*check1).field; // use arr[1] and check1->field instead

	// Rule 4.7.9
	// Incorrect
	if ((a > 0) && (!(b))) {
    }

    // Correct
	if (a > 0 && !b) {
    }

	// Rule 5.1
    for (int i = 0; i < 10; ++i) { // 10 is inappropriate, you need to create a constant for it
    }

	// Rule 5.2
    #define BUFFER_SIZE 1024       // must be enum
	const int BOARD_WIDTH = 8; // must be enum

	// C++ only
	const int BUFSIZE = 1024; // use constexpr
	
	// SEE IN THE BEGINNING

	// Rule 5.3
	// #include "stdio.h"	// use <stdio.h> instead
	// #include <tree.h>	// use "tree.h" instead

	// Rule 5.4
	// Incorrect
	printf("my %s has %d chars\n", "string format", 30);

	// Correct
	fprintf(stderr, "my %s has %d chars\n", "string format", 30);

	// Rule 5.5
	fprintf(stderr, "my %s has %d chars", "string format", 30); // must contain '\n' in the end

	// Rule 5.6
	// SEE IN THE BEGINNING

	// Rule 5.7
	// -Wmain

	// Rule 5.8
	char s3[10];
	char s4[10];

	// gets(s3);
	sprintf(s3, "123");
	strcpy(s3, s4);
	strcat(s3, s4);

	scanf("%s", s3);
	fscanf(stdin, "%s", s4);

	strncpy(s3, s4, 5);
	strncat(s3, s4, 5);

	// Rule 5.9
	// Использование функции malloc не рекомендуется. Допускается использовать malloc при реализации расширяемых массивов, если предполагается выполнять realloc. Во всех остальных случаях используйте функцию calloc. Запрещается использовать malloc при выделении памяти под элементы списков, деревьев и прочих подобных структур.

	// Rule 5.10
	struct Node *p1 = (Node *) calloc(1, sizeof(struct Node)); // must be sizeof(*p1)

	// Rule 5.11
	char *s5 = (char *) calloc(4, sizeof(*s5));
	s5 = (char *) realloc(s5, 3); // must be 3 * sizeof(s5[0])

	// Rule 5.12
	// В программе не должны присутствовать закомментированные строки кода, которые использовались для отладки.

	// Rule 5.13
	// Вспомогательные функции общего назначения (такие, как getline, функции работы со списками и т. д.) не должны ничего выводить на stdout или stderr. В случае возникновения ошибки такие функции должны, как правило, возвращать специальное значение.

	// Rule 5.14
	// Глобальные переменные должны использоваться только при необходимости. Передача параметров предпочтительнее использования глобальных переменных.

	// Rule 5.15
	// PATH_MAX <= 64 * 1024; 
	{
	    char *buf = (char *) calloc(8, sizeof(*buf)); // char buf[PATH_MAX]; instead
	    // ...
	    free(buf);
	}

	// Rule 5.16
	int size;
	scanf("%d", &size);
    double arr1[size]; // use calloc instead

	// Rule 5.17
    // See it yourself

	// Rule 5.18
    // C only
    double *ptr = (double*) calloc(8, sizeof(*ptr)); // no (double *) needed

	// Rule 6.1
	// Rule 6.2
	// Rule 6.3
	// Rule 6.4
	// Rule 6.5
	// Rule 6.6

    return 0;
}