#include <stdio.h>
#include <string.h>
#include <cassert>

char *gText = 0;
unsigned int gSize = 0;  // the length of gText, in bytes

int append(const char* s )
{
	if( s && s[0] )
	{
		size_t extra_characters = strlen(s);
		gSize += extra_characters;
		char* temp = new char[ gSize + 1 ];
		*temp = 0;
		if (gText)
		{
			strcpy(temp, gText);
			strcat(temp, s);
			delete [] gText;
		}
		else
		{
			strcpy(temp, s);
		}

		gText = temp;
	}
	return gSize;
}

void reset()
{
	delete [] gText;
	gText = NULL;
	gSize = 0;
}

void test_initialise()
{
	int ret = append("Hello");
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);

	reset();

}

void ensure_empty()
{
	assert(gText == 0);
	assert(gSize == 0);
}

void test_append()
{
	int ret;
	ret = append("Hello");
	assert(ret == 5);
	assert(gSize == 5);
	ret = append(", World!");
	assert(ret == 13);
	assert(strcmp("Hello, World!", gText) == 0);
	assert(gSize == 13);
	ret = append(" something else");
	assert(ret == 28);
	assert(strcmp("Hello, World! something else", gText) == 0);
	assert(gSize == 28);
	reset();
}

void test_null()
{
	int ret;

	ret = append(NULL);
	assert(ret == 0);
	assert(gText == 0);
	assert(gSize == 0);

	ret = append("Hello");
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);

	ret = append(NULL);
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);

	reset();
}

void test_empty()
{
	int ret;

	ret = append("");
	assert(ret == 0);
	assert(gText == 0);
	assert(gSize == 0);

	ret = append("Hello");
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);

	ret = append("");
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);
	reset();

}


int main()
{
	ensure_empty();
	test_initialise();
	ensure_empty();
	test_append();
	ensure_empty();
	test_empty();
	ensure_empty();
	test_null();
	ensure_empty();

	printf("tests passed\n");
	return 0;
}
