#include <iostream>
#include <vector>
#include <algorithm>

#include <ctime>
#include <cassert>
#include <cstdio>
#include <Windows.h>

unsigned compares=0;


float eyex = 1.0f, eyey = 12.433f, eyez = -100.f;
struct SomeSortable {
	SomeSortable() : x_(0.58f), y_(0.58f), z_(0.58f), d_((rand() % 1000)*1.f) {}

	float distanceToEye() const
	{
		float dot = x_ * eyex
			+ y_ * eyey
			+ z_ * eyez;
		return dot + d_;

	}
	bool operator<(SomeSortable const& rhs) const
	{
		++compares;
		return distanceToEye() < rhs.distanceToEye();
	}
	bool operator>(SomeSortable const& rhs) const
	{
		return distanceToEye() > rhs.distanceToEye();
	}

	float x_, y_, z_, d_;
}; 

template<typename T>
bool is_sorted(T start, T end)
{
	for(T itt = start; itt != end-1; ++itt)
	{
		if (*itt > *(itt+1))
			return false;
	}
	return true;
}

struct Sorter
{
	Sorter() {}
	bool operator()(SomeSortable const& lhs, SomeSortable const& rhs) const
	{
		++compares;
		return lhs.d_ < rhs.d_;
	}

};


int main(int argc, char *argv[])
{
	time_t t;
	time(&t);
	srand((unsigned int)t);

	std::vector<SomeSortable> sort1;

	const unsigned num_iterations = 100000;

	DWORD start;
	start = GetTickCount();
	for (unsigned j = 0; j < num_iterations; ++j)
	{
		for (unsigned i = 0; i < 5; ++i)
		{
			sort1.push_back(SomeSortable());
		}
		std::sort(sort1.begin(), sort1.end());

		bool sorted = is_sorted(sort1.begin(), sort1.end());
		assert(sorted);
		sort1.clear();
	}
	DWORD end;
	end = GetTickCount();

	std::cout << "std::sort number of comparisons " << compares/num_iterations << " duration " << end - start << std::endl;

	compares = 0;
	start = GetTickCount();
	for (unsigned j = 0; j < num_iterations; ++j)
	{
		for (unsigned i = 0; i < 5; ++i)
		{
			sort1.push_back(SomeSortable());
		}
		std::stable_sort(sort1.begin(), sort1.end());

		bool sorted = is_sorted(sort1.begin(), sort1.end());
		assert(sorted);
		sort1.clear();
	}

	end = GetTickCount();
	std::cout << "std::stable_sort number of comparisons " << compares/num_iterations << " duration " << end - start << std::endl;

	compares = 0;
	start = GetTickCount();
	for (unsigned j = 0; j < num_iterations; ++j)
	{
		for (unsigned i = 0; i < 5; ++i)
		{
			sort1.push_back(SomeSortable());
		}
		std::make_heap(sort1.begin(), sort1.end()); 
		std::sort_heap(sort1.begin(), sort1.end()); 

		bool sorted = is_sorted(sort1.begin(), sort1.end());
		assert(sorted);
		sort1.clear();
	}

	end = GetTickCount();
	std::cout << "std::sort_heap number of comparisons " << compares/num_iterations << " duration " << end - start << std::endl;



	compares = 0;
	start = GetTickCount();
	for (unsigned j = 0; j < num_iterations; ++j)
	{
		for (unsigned i = 0; i < 5; ++i)
		{
			sort1.push_back(SomeSortable());
		}

		std::sort(sort1.begin(), sort1.end(), Sorter());


		bool sorted = is_sorted(sort1.begin(), sort1.end());
		assert(sorted);
		sort1.clear();
	}

	end = GetTickCount();
	std::cout << "dot sort number of comparisons " << compares/num_iterations << " duration " << end - start << std::endl;


	return 0;
}