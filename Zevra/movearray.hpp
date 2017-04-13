/*
  Zevra, a UCI chess playing engine
  Copyright (C) 2016-2017 Oleg Smirnov (author)
  Zevra is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.
  Zevra is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOVEARRAY_HPP
#define MOVEARRAY_HPP

#include <vector>
#include "bitmove.hpp"

class BitMove;

class MoveArray {
private:
	const int MAX_SIZE = 288;
public:
	std::vector<BitMove> moveArray;
	MoveArray();
	~MoveArray();
	unsigned int count, num_attacks;
	void clear();
	void addMove(BitMove mv);
};

#endif
