/*
Non portable version took approx 1 minute
Full adder version around 20 minutes
References used http://en.wikipedia.org/wiki/Adder_(electronics) (for the full adder function)
*/
#include <stdio.h>
#include <cassert>

int add1_non_portable(int val)
{
	__asm inc val;
	return val;
}

int add1_reference(int val)
{
	return val+1;
}

// all arguments only use single bit
int full_adder(int a, int b, int &c)
{
	int sum = (a ^ b) ^ c;
	int cout = (a & b) | (b & c) | (a & c);
	c = cout;
	return sum;
}

int add1(int val)
{
	static const unsigned int number_of_bits = sizeof(unsigned int) * 8;
	int carry = 0;
	unsigned int result = 0; // unsigned so it doesn't sign extend when shifting down
	int a = 1;
	int b = val;
	for (unsigned int i = ~0; i != 0; i = i >> 1) // Shift down until all bits have been zero'ed
	{
		int sum = full_adder(1 & a, 1 & b, carry); // Add the least significant bit
		// Get the next bits
		b = b >> 1;
		a = a >> 1;
		// Shift down and set the msb to the sum
		result = (result >> 1) | (sum << (number_of_bits-1));
	}

	return result;
}

int main()
{

	assert(add1_non_portable(0) == add1_reference(0));
	assert(add1_non_portable(1) == add1_reference(1));
	assert(add1_non_portable(132) == add1_reference(132));
	assert(add1_non_portable(277) == add1_reference(277));
	assert(add1_non_portable(9355) == add1_reference(9355));
	assert(add1_non_portable(-5426) == add1_reference(-5426));
	assert(add1_non_portable(0xdadadada) == add1_reference(0xdadadada));
	assert(add1_non_portable(0xfabcdefa) == add1_reference(0xfabcdefa));
	assert(add1_non_portable(0xffffffff) == add1_reference(0xffffffff));

	assert(add1(0) == add1_reference(0));
	assert(add1(1) == add1_reference(1));
	assert(add1(132) == add1_reference(132));
	assert(add1(277) == add1_reference(277));
	assert(add1(9355) == add1_reference(9355));
	assert(add1(-5426) == add1_reference(-5426));
	assert(add1(0xdadadada) == add1_reference(0xdadadada));
	assert(add1(0xfabcdefa) == add1_reference(0xfabcdefa));
	assert(add1(0xffffffff) == add1_reference(0xffffffff));

	return 0;
}