#pragma once

namespace Maths
{
	typedef float Real;

	namespace Constant
	{
		const Real RadToDeg = Real(57.2957795130823208767981548141052);
		const Real DegToRad = Real(0.0174532925199432957692369076848861);
		const Real Zero =     Real(0.0);
		const Real One =      Real(1.0);
		const Real Two =      Real(2.0);
		const Real Epsilon =  Real(10e-6);
		const Real Pi =       Real(3.1415926535897932384626433832795);
	}

	template<typename T> 
	bool equivalent(const T &a, const T &b)     
	{ 
		return ((a < b + Constant::Epsilon) && (a > b - Constant::Epsilon)) ? true : false; 
	}

	// TODO deg to rad etc
}
