#include <cstring>

const int small_lettered_constant = 9;

int main()
{
	int плохое_название;

	char check_strcpy_src[100] = "abc\xF";
	char check_strcpy_dest[100] = "de";
	strcpy(check_strcpy_dest, check_strcpy_src);

	// return 0;
}