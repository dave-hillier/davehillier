#include <stdio.h>
#include <string.h>
#include <cassert>

char *gText = 0;
unsigned int gSize = 0;  // the length of gText, in bytes

bool gAllocated = false; // Has this string been allocated by append or elsewhere?

int append(const char* s )
{
	if( s && s[0] )
	{
		if( (! gText) || (gSize == 0) )
		{
			gText = (char *)s;// const cast is bad
			gSize = (unsigned int)strlen(s);// Cast to unsigned to silence warnings
			// This string could be static data, so remember we didnt allocate it
			gAllocated = false;
		}
		else
		{
			unsigned int len = gSize;
			unsigned int s_len = (unsigned int)strlen( s ); // Cast to unsigned to silence warningsv

			gSize += s_len;
			char* temp = new char[ gSize + 1 ];
			gAllocated = true;// Set to true since we've just allocated some memory
			memcpy(temp, gText, len);
			memcpy(temp + len, s, s_len + 1);
			temp[gSize] = '\0';

			gText = temp;
		}
	}
	return gSize;
}

void reset()
{
	// Clean up if we allocated
	if (gAllocated)
		delete [] gText;
	
	// Set the global variables back to their original values
	gText = NULL;
	gSize = 0;
	gAllocated = false;
}

// Test to ensure that a single string can be assigned to gText
void test_initialise()
{
	int ret = append("Hello");
	assert(ret == 5);
	assert(strcmp("Hello", gText) == 0);
	assert(gSize == 5);

	reset();

}

// Ensure that reset has occurred correctly
void test_ensure_empty()
{
	assert(gText == 0);	
	assert(gSize == 0);
}

// Check to see if a string can be appended on to the 
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

// Test to see what happens when we try to append NULL pointer
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

// Test to see what happens when we try to append an empty string
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
	test_ensure_empty();
	test_initialise();
	test_ensure_empty();
	test_append();
	test_ensure_empty();
	test_empty();
	test_ensure_empty();
	test_null();
	test_ensure_empty();

	printf("tests passed\n");
	return 0;
}
