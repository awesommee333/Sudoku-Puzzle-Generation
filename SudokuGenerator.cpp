#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void swap(vector<int>& v, int a, int b) {
	int tmp = v[a];
	v[a] = v[b];
	v[b] = tmp;
}

void shuffle(vector<int>& v) {
	for (int i = 0; i < v.size(); i++) {
		swap(v, i, rand() % v.size());
	}
}

void remMovesSquare(set<int>& moves, vector<int>& puzzle, int square, int sizeUnit) {
	int rsSize = sizeUnit * sizeUnit * sizeUnit;
	//rowsquare
	int rs = (square / rsSize) * rsSize;
	int cs = ((square % (sizeUnit*sizeUnit)) / sizeUnit) * sizeUnit;

	//rowplus
	for (int rp = 0; rp < rsSize; rp+=rsSize/sizeUnit) {
		for (int cp = 0; cp < sizeUnit; cp++) {
			moves.erase(puzzle[rs + rp + cs + cp]);
		}
	}
}

void remMovesRow(set<int>& moves, vector<int>& puzzle, int square, int sizeUnit) {
	square = square % (sizeUnit*sizeUnit);
	int sizePuzzle = puzzle.size();
	for (; square < sizePuzzle; square += sizeUnit*sizeUnit) {
		moves.erase(puzzle[square]);
	}
}

void remMovesCol(set<int>& moves, vector<int>& puzzle, int square, int sizeUnit) {
	square = (square / (sizeUnit*sizeUnit))*(sizeUnit*sizeUnit);
	for (int s = square; s - square < sizeUnit*sizeUnit; s++) {
		moves.erase(puzzle[s]);
	}
}

//returns possible moves shuffled
vector<int> getMoves(vector<int>& puzzle, int square, int sizeUnit) {
	if (puzzle[square] != 0)
		return vector<int>(0);

	vector<int> moves ( sizeUnit*sizeUnit );
	for (int i = 0; i < moves.size(); i++)
		moves[i] = i + 1;
	set<int> goodMoves(moves.begin(), moves.end());

	remMovesSquare(goodMoves, puzzle, square, sizeUnit);
	remMovesRow(goodMoves, puzzle, square, sizeUnit);
	remMovesCol(goodMoves, puzzle, square, sizeUnit);

	moves = vector<int>(goodMoves.begin(), goodMoves.end());
	shuffle(moves);
	return moves;
}

//Finds a valid solution to a sudoku puzzle - if multiple solutions then a random one
bool solvePuzzle(vector<int>& puzzle, int i, int sizeUnit, vector<int>& moveOrder) {
	int sizePuzzle = puzzle.size();
	for (; i<sizePuzzle && puzzle[i] != 0; i++);
	if (i == sizePuzzle)
		return true;
	int square = i;
	vector<int> moves = getMoves(puzzle, square, sizeUnit);
	for (int& move : moves) {
		puzzle[square] = move;
		if (solvePuzzle(puzzle, i + 1, sizeUnit, moveOrder))
			return true;
		puzzle[square] = 0;
	}
	return false;
}

bool solvePuzzle(vector<int>& puzzle, int i = 0, int sizeUnit = 3) {
	vector<int> moveOrder(puzzle.size());
	for (int x = 0; x < moveOrder.size(); x++)
		moveOrder[x] = x;
	shuffle(moveOrder);
	return solvePuzzle(puzzle, i, sizeUnit, moveOrder);
}

//returns 1 if unique
int checkUnique(vector<int>& puzzle, int i = 0, int sizeUnit = 3) {
	int sizePuzzle = puzzle.size();
	for (; i < sizePuzzle && puzzle[i] != 0; i++);
	if (i == sizePuzzle)
		return 1;
	int square = i;
	vector<int> moves = getMoves(puzzle, square, sizeUnit);
	int solvedWays = 0;
	for (int& move : moves) {
		puzzle[square] = move;
		solvedWays += checkUnique(puzzle, i + 1, sizeUnit);
		puzzle[square] = 0;
		if (solvedWays > 1)
			return solvedWays;
	}
	return solvedWays;
}

bool remSquares(vector<int>& puzzle, int squaresLeft, vector<int>& remOrder) {
	int i = 0;
	while (true) {
		if (squaresLeft == 0) {
			cout << "good puzzle: " << '\n';
			for (int r = 0; r < 9; r++) {
				for (int c = 0; c < 9; c++)
					cout << puzzle[9 * r + c] << ' ';
				cout << '\n';
			}
			return true;
		}

		i++;
		if (i == 81) {
			cout << "SAD " << squaresLeft << '\n';
			return false;
		}

		int tmp = puzzle[remOrder[i]];
		puzzle[remOrder[i]] = 0;
		while (checkUnique(puzzle) > 1) {
			puzzle[remOrder[i++]] = tmp;

			if (i == puzzle.size())
				return false;

			tmp = puzzle[remOrder[i]];
			puzzle[remOrder[i]] = 0;
		}
		squaresLeft--;
	}

	return false;
}

vector<vector<int>> generatePuzzle(int numSquares) {
	srand(time(NULL));
	vector<int> puzzle(81, 0);
	solvePuzzle(puzzle, 0, 3);

	vector<int> puzzleCopy = puzzle;

	vector<int> remOrder(puzzle.size());
	do {
		puzzle = puzzleCopy;
		for (int x = 0; x < remOrder.size(); x++)
			remOrder[x] = x;
		shuffle(remOrder);
	} while (!remSquares(puzzle, puzzle.size()-numSquares, remOrder));
	
	return { puzzle, puzzleCopy };
}

int main()
{

	while (true) {

		clock_t start_t, end_t;

		srand(time(NULL));

		cout << "Enter difficulty name(type quit to quit): ";
		string s;
		cin >> s;

		if (s.compare("quit") == 0)
			break;

		ofstream fout(s + ".txt");

		cout << "Enter range: ";
		int min, max;
		cin >> min >> max;

		cout << "Enter num puzzles: ";
		int numPuzzles;
		cin >> numPuzzles;

		start_t = clock();

		vector<int> sizes(numPuzzles, 0);
		for (int i = 0; i < numPuzzles; i++)
			sizes[i] = min + rand() % (max + 1 - min);
		sort(sizes.rbegin(), sizes.rend());

		for (int i = 0; i < numPuzzles; i++) {
			vector<vector<int>> puzzle = generatePuzzle(sizes[i]);
			end_t = clock();
			
			fout << "{[\"starting\"]={";
			for (int r = 0; r < 9; r++) {
				for (int c = 0; c < 9; c++) {
					fout << puzzle[0][9 * r + c];
					if (c != 8)
						fout << ',';
				}
				if (r != 8)
					fout << ",";
			}

			fout << "}, [\"solved\"]={";

			for (int r = 0; r < 9; r++) {
				for (int c = 0; c < 9; c++) {
					fout << puzzle[1][9 * r + c];
					if (c != 8)
						fout << ',';
				}
				if (r != 8)
					fout << ",";
			}

			if (i != numPuzzles-1)
				fout << "}}, ";
			else
				fout << "}}";
			cout << i << '\n';
		}

		double total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "Total time taken by CPU: " << total_t << "\n";
	}
}
