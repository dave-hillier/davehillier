/*
   Copyright (C) 2002 Nate Miller nathanm@uci.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
#include "MathLib.h"

namespace platformMath {

const float MathLib::PI           = 3.1415933464102f;
const float MathLib::Epsilon      = 1.0e-05f;
const float MathLib::TwoPI        = (MathLib::PI * 2.0f);
const float MathLib::PIOverTwo    = (MathLib::PI / 2.0f);
const float MathLib::DegToRadVal  = (MathLib::PI / 180.0f);

}
