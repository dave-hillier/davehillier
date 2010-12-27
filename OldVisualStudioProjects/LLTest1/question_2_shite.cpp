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


struct ExpressionPair 
{
	ExpressionPair(int v, char o, int p) : value_(v), operation_(o) {}
	int value_;
	char operation_;
};

struct node
{
	virtual value() const = 0;
};

struct operation : node
{
	operation(char op, node *left, node *right ) : op_(op), left_(left), right_(right) {}
	virtual value() const
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
	virtual value() const
	{
		return value_;
	}
};

int evaluate(std::vector<ExpressionPair> terms, const std::vector<unsigned int> &indices)
{
	/*for (std::vector<unsigned int>::const_iterator itt = indices.begin(); itt != indices.end(); ++itt)
	{

		
		int left = terms[i].value_;
		int right = terms[i+1].value_;
		char op = terms[i].operation_;

		int result = 0;
		if (op == '+') 
			result = left+right;
		else if (op == '-') 
			result = left-right;
		else if (op == '*') 
			result = left*right;

		terms[i].value_ = result;
		terms.erase(terms.begin()+i);
	}
	return terms[0].value_;*/
	return 0;
}

void parseExpressions(const char *expression, std::vector<ExpressionPair> &terms)
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
		terms.push_back(ExpressionPair(v,op, terms.size()));
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