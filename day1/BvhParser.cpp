/*
 *  BvhParser.cpp
 *  Day1
 *
 *  Created by Dave Hillier on 13/07/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "BvhParser.h"

#include <iostream>
#include <fstream>
#include <string>

	// HIERARCHY
	// ROOT name {
	//	OFFSET x y z
	//  CHANNELS 6 name name name name name name
	//  JOINT name {
	//	 OFFSET x y z 
	//   CHANNELS 3 name name name
	//   ...	
	//  }
	//  End Site
	//  {
	//   OFFSET x y z
	//  }
	//}
	// MOTION
	// Frames: n
	// Frame Time: f.f
	// n lines of 6 + 3 * joints in parse order

class BvhParser::Impl
{
public:
	Impl(std::istream &input)
	{
		input >> std::ws >> "HIERARCHY" >> std::ws;
		std::string name;
		input >> "ROOT" >> std::ws >> name >> std::ws >> "{";
		input >> std::ws >>  "}" >> std::ws;
		
		
	}
}

BvhParser::BvhParser()
{
	std::ifstream ifs("walk.bvh");
	BvhParser::Impl tmp(ifs);
	
}
BvhParser::~BvhParser() {}

//BvhParser