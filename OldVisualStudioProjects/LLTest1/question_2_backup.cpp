/*

*/
#include <stdio.h>
#include <cassert>

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

struct Expression 
{
	Expression(int v, char o, int p) : value_(v), operation_(o), pos_(p) {}
	int value_;
	char operation_;
	int pos_;
	bool operator== (int i) const { return i == pos_; }
};

void parseExpressions(const char *expression, std::vector<Expression> &terms)
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
		terms.push_back(Expression(v,op,(unsigned)terms.size()));
	}
}

int evaluate(std::vector<Expression> terms, const std::vector<unsigned int> &indices)
{
	/*for (std::vector<unsigned int>::const_iterator itt = indices.begin(); itt != indices.end(); ++itt)
	{
		for (unsigned int j = 0; j < terms.size(); j++)
		{
			if (*itt == terms[j].pos_)
				i = j;
		}

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

		terms.erase(terms.begin()+i);
		terms[i].value_ = result;
	}
	return terms[0].value_;
*/
	
	for (std::vector<unsigned int>::const_iterator itt = indices.begin(); itt != indices.end(); ++itt)
	{
		std::vector<Expression>::iterator term = std::find(terms.begin(), terms.end(), *itt);
		std::vector<Expression>::iterator nextTerm = term+1;
		
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
	std::vector<Expression> terms;
	parseExpressions(expression, terms);

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < terms.size()-1; ++i)
	{
		indices.push_back(i);
	}

	std::set<int> results;
	do
	{
		int r = evaluate(terms, indices);
		results.insert(r);		
	}
	while (std::next_permutation(indices.begin(), indices.end()));

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