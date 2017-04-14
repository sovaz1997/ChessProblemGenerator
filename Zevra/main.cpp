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

#include <iostream>
#include <string>
#include <fstream>

#include "game.hpp"
#include "bitboard.hpp"

#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#elif defined(__unix__) || defined(unix) || defined(__unix) || defined(__APPLE__)
	#include <unistd.h>

	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/select.h>
#endif

bool is_input_available() {

#if defined(_WIN32) || defined(_WIN64)

	static bool init = false, is_pipe;
	static HANDLE stdin_h;
	DWORD val, error;
	bool UseDebug = false;

	// val = 0; // needed to make the compiler happy?

	// have a look at the "local" buffer first, *this time before init (no idea if it helps)*

	if (UseDebug && !init) printf("info string init=%d stdin->_cnt=%d\n",int(init),stdin->_cnt);

	if (stdin->_cnt > 0) return true; // HACK: assumes FILE internals

	// input init (only done once)

	if (!init) {

		init = true;

		stdin_h = GetStdHandle(STD_INPUT_HANDLE);

		if (UseDebug && (stdin_h == NULL || stdin_h == INVALID_HANDLE_VALUE)) {
			error = GetLastError();
			printf("info string GetStdHandle() failed, error=%d\n",error);
		}

		is_pipe = !GetConsoleMode(stdin_h,&val); // HACK: assumes pipe on failure

		if (UseDebug) printf("info string init=%d is_pipe=%d\n",int(init),int(is_pipe));

		if (UseDebug && is_pipe) { // GetConsoleMode() failed, everybody assumes pipe then
			error = GetLastError();
			printf("info string GetConsoleMode() failed, error=%d\n",error);
		}

		if (!is_pipe) {
			SetConsoleMode(stdin_h,val&~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(stdin_h); // no idea if we can lose data doing this
		}
	}

	// different polling depending on input type
	// does this code work at all for pipes?

	if (is_pipe) {

		if (!PeekNamedPipe(stdin_h,NULL,0,NULL,&val ,NULL)) {

			if (UseDebug) { // PeekNamedPipe() failed, everybody assumes EOF then
				error = GetLastError();
				printf("info string PeekNamedPipe() failed, error=%d\n",error);
			}

			return true; // HACK: assumes EOF on failure
		}

		if (UseDebug && val < 0) printf("info string PeekNamedPipe(): val=%d\n",val);

		return val > 0; // != 0???

	} else {

		GetNumberOfConsoleInputEvents(stdin_h,&val);
		return val > 1; // no idea why 1
	}

	return false;

#else // assume POSIX

	int val;
	fd_set set[1];
	struct timeval time_val[1];

	FD_ZERO(set);
	FD_SET(STDIN_FILENO,set);

	time_val->tv_sec = 0;
	time_val->tv_usec = 0;

	val = select(STDIN_FILENO+1,set,NULL,NULL,time_val);
	//if (val == -1 && errno != EINTR) {
	//my_fatal("input_available(): select(): %s\n",strerror(errno));
	//}

	return val > 0;

#endif
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "Недостаточно аргументов!" << std::endl;
		return 0;
	}

	std::cout << "Конвертация PGN в PGN-uci формат..." << std::endl;
	std::string cmd = "./pgn-extract -Wuci ";//./pgn-extract  -Wuci lichess_sovaz_2017-04-13.pgn -t test.txt > tmp.uci
	cmd += + argv[1];
	cmd += " -t tmp.info > tmp.uci";
	system("echo \"Variant \\\"Standard\\\"\" > tmp.info");
	system(cmd.c_str());
	system("rm tmp.info");

	std::ifstream input_file("tmp.uci");
	std::string line;

	std::ofstream output_file("result.uci");

	while(std::getline(input_file, line)) {
		if(!line.empty()) {
			if(line[0] != '[') {
				output_file << line << "\n";
			}
		}
	}

	system("rm tmp.uci");


	//Game* game = new Game();
	//game->startGame();



	//delete game;
}
