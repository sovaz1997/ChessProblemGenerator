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
#include <vector>
#include <string>
#include <fstream>
#include "game.hpp"
#include "bitboard.hpp"

void analyser(std::ifstream&& file_stream);
std::vector<std::string> split(std::string& str);
void testInStockfish(std::string fen);
bool getScore(std::string line, int& score);

std::pair<int, int> stockfish_data_analyser(std::ifstream&& stockfish_data);

const int MAX_SCORE = 1000000;

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

	analyser(std::ifstream("result.uci"));


	//Game* game = new Game();
	//game->startGame();



	//delete game;
}

void analyser(std::ifstream&& file_stream) {
	std::string line;
	Game game;
	while(std::getline(file_stream, line)) {
		std::vector<std::string> moves = split(line);
		game.game_board.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		
		for(int i = 0; i < moves.size(); ++i) {
			game.move(moves[i]);
			testInStockfish(game.game_board.getFen());
		}
	}
}

std::vector<std::string> split(std::string& str) {
	std::vector<std::string> vec;

	std::string buffer;
	for(int i = 0; i < str.size(); ++i) {
		if(str[i] == ' ') {
			if(!buffer.empty()) {
				vec.push_back(buffer);
				buffer.clear();
			}
		} else {
			buffer.push_back(str[i]);
		}
	}

	if(!buffer.empty()) {
		vec.push_back(buffer);
		buffer.clear();
	}

	return vec;
}

void testInStockfish(std::string fen) {
	int max_depth = 3;
	
	for(int i = 1; i <= max_depth; ++i) {
		std::ofstream input_stockfish("input.stockfish");
		input_stockfish << "setoption name Debug Log File value data.stockfish\nposition fen " << fen <<  "\n" << "go depth " << std::to_string(i) << std::endl;

		system("./stockfish_8_x64 < input.stockfish");
		std::pair<int, int> anylyse_stockfish = stockfish_data_analyser(std::ifstream("data.stockfish"));

		std::ofstream result("result.txt");
		if(abs(anylyse_stockfish.first) <= 100 && anylyse_stockfish.second - anylyse_stockfish.first >= 500) {
			if(i == max_depth - 1) {
				//std::cout << fen << std::endl;
				result << fen << std::endl;
				
			}
		}
		//std::cout << anylyse_stockfish.first << " " << anylyse_stockfish.second << std::endl;
	}

	//system("rm data.stockfish");
}

std::pair<int, int> stockfish_data_analyser(std::ifstream&& stockfish_data) {
	std::string line;
	int count = 0;
	int minimum = 0;
	int maximum = 0;

	int score;

	while(std::getline(stockfish_data, line)) {
		if(getScore(line, score)) {
			if(!count) {
				minimum = score;
				maximum = score;
			} else {
				maximum = score;
			}

			++count;
		}
	}

	return std::pair<int, int> (minimum, maximum);
}

bool getScore(std::string line, int& score) {
	std::vector<std::string> split_line = split(line);

	if(split_line.size() > 1) {
		if(split_line[1] == "info") {
			for(int i = 0; i < split_line.size(); ++i) {
				if(split_line[i] == "score") {
					if(split_line[i + 1] == "cp") {
						score = std::stoi(split_line[i + 2]);
						return true;
					} else if(split_line[i + 1] == "mate") {
						int mate = std::stoi(split_line[i + 2]);
						if(mate > 0) {
							score = MAX_SCORE - mate;
						} else if(mate < 0) {
							score = -MAX_SCORE - mate;
						} else {
							return false;
						}
						return true;
					} else {
						return false;
					}
				}
			}
		}
	}

	return false;
}