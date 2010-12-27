/*

*/
#include <stdio.h>
#include <cassert>

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <boost/iterator/permutation_iterator.hpp>

struct node
{
	virtual int value() const = 0;
};

struct operation : node
{
	operation(char op, node *left, node *right ) : op_(op), left_(left), right_(right) {}
	virtual int value() const
	{
		int result = 0;
		if (op_ == '+') 
			result = left_->value()+right_->value();
		else if (op_ == '-') 
			result = left_->value()-right_->value();
		else if (op_ == '*') 
			result = left_->value()*right_->value();
		return result;
	}

	char op_;
	node *left_;
	node *right_;
};
struct leaf : node
{
	leaf(int v) : value_(v) {}
	virtual int value() const
	{
		return value_;
	}
	int value_;
};



void parseExpressions(const char *expression, node **root)
{
	
	for (const char *p = expression; *p; ++p)
	{
		while(*p && isspace(*p))
			++p;

		int v = atoi(p);

		while(*p && isdigit(*p))
			++p;
		while(*p && isspace(*p))
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
	}
}

void print(const std::set<int> &results)
{
	std::cout << (int)results.size() << " unique { ";
	std::set<int>::const_iterator end = results.end();
	for (std::set<int>::const_iterator itt = results.begin(); itt != end; ++itt)
	{
		if (itt != results.begin())
			std::cout << ", ";
		std::cout << *itt;

	}
	std::cout << " }"<<std::endl;
}


void parenPermutations(const char *expression)
{
	std::vector<ExpressionPair> terms;
	parseExpressions(expression, terms);
	std::set<int> results;

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < terms.size()-1; ++i)
	{
		indices.push_back(i);
	}

	do
	{
		int r = evaluate(terms, indices);
		results.insert(r);		
	}
	while (std::next_permutation(indices.begin(), indices.end()));

	print(results);
}

int main(int, char **)
{
	std::stringstream ss;
	parenPermutations("1 + 2 - 3");
	parenPermutations("1 - 1 + 1");
	parenPermutations("10");
	parenPermutations("1 + 2 - 3 * 4");
	parenPermutations("1 + 2 + 3 * 4 - 5 * 2");


	parens(2);
	//parens(5);

	return 0;
}