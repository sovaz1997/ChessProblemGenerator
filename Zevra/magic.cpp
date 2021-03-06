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

#include "magic.hpp"


Magic::Magic() {}
Magic::Magic(std::vector<uint64_t> _result, uint64_t _magic, uint8_t _count) : result(_result), magic(_magic), count(_count) {}

uint64_t Magic::getPossibleMoves(uint64_t mask) {
	return (result[(mask * magic) >> count]);
}
