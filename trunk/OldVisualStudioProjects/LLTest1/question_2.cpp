#include <stdio.h>
#include <cassert>

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

struct Expression 
{
	Expression(int v, char o, int p) : 
		value_(v), operation_(o), pos_(p) {}
	int value_;		// The current value of this node
	char operation_;// Character 0, '+', '-' or '*'
	int pos_;		// Original place in the expression
	bool operator== (int i) const { return i == pos_; }
};

void parseExpressions(const char *expression, 
					  std::vector<Expression> &terms)
{
	for (const char *p = expression; *p; ++p)
	{
		while(*p && isspace(*p)) // consume any white space
			++p;

		int v = atoi(p); // get a digit from the string

		while(*p && isdigit(*p)) // skip that digit
			++p;
		while(*p && isspace(*p)) // skip any white space
			++p;        

		char op = 0;
		switch(*p)
		{
		case '+':
		case '*':
		case '-':
			op = *p;
			++p;
		default:
			break;
		}
		terms.push_back(Expression(v,op,(unsigned)terms.size()));
	}
}

// Note deliberate copy of terms. 
int evaluate(std::vector<Expression> terms, 
			 const std::vector<unsigned int> &indices)
{
	for (std::vector<unsigned int>::const_iterator itt = 
		indices.begin(); itt != indices.end(); ++itt)
	{
		// get the term in this index original position
		std::vector<Expression>::iterator term = 
			std::find(terms.begin(), terms.end(), *itt);

		// The term next in the original sequence
		std::vector<Expression>::iterator nextTerm = term+1; 
		
		// Calculate this nodes value and store 
		// it in the next in the sequence
		int left = (*term).value_;
		int right = (*nextTerm).value_;
		char op = (*term).operation_;

		int result = 0;
		if (op == '+') 
			result = left+right;
		else if (op == '-') 
			result = left-right;
		else if (op == '*') 
			result = left*right;

		(*nextTerm).value_ = result; 
		terms.erase(term); 
	}
	return terms[0].value_;
}

void printResults(const std::set<int> &results)
{
	// std::set is ordered so it will probably 
	// not be the same order as the question
	std::cout << (int)results.size() << " unique { ";
	std::set<int>::const_iterator end = results.end();
	for (std::set<int>::const_iterator itt = results.begin(); 
		itt != end; ++itt)
	{
		if (itt != results.begin())
			std::cout << ", ";
		std::cout << *itt;

	}
	std::cout << " }"<<std::endl;
}


void parenPermutations(const char *expression)
{
	std::vector<Expression> terms;
	parseExpressions(expression, terms);

	// Indices of the expression parts appart from the last term.
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < terms.size()-1; ++i)
	{
		indices.push_back(i);
	}

	// Store the results in a set as an easy way to eliminate 
	// duplicates
	std::set<int> results;
	do
	{
		// Evaluate the expression for each permutation of the 
		// indices
		int r = evaluate(terms, indices);
		results.insert(r);		
	}
	while (std::next_permutation(indices.begin(), indices.end()));

	// display the results
	printResults(results);
}

int main(int, char **)
{
	parenPermutations("1 - 1 + 1");
	parenPermutations("10");
	parenPermutations("1 + 2 - 3 * 4");
	parenPermutations("1 + 2 + 3 * 4 - 5 * 2");
	return 0;
}