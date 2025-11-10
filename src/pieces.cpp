#include <algorithm>
#include <iostream>
#include <random>

#include "pieces.hpp"

// toggle whether or not to arbitrarily randomize orientations to avoid bias caused by importing from the solution
#define SHUFFLE_ORIENTATIONS false
// toggle whether or not to use wood grain optimization
#define USE_WOOD_GRAIN true

Piece::Piece() {}

Piece::Piece(std::string data, int rows, int cols) {
	PieceOrientation orientation = PieceOrientation(data, rows, cols);

	orientations.push_back(orientation);

	// wood grain reduces orientations to two possibilities. Significantly improves speed
	if (USE_WOOD_GRAIN) {
		orientation.rotate();
		orientation.rotate();

		if (orientations.at(0) != orientation) {
			orientations.push_back(orientation);
		}
	} else {
		// add every possible orientation
		for (int i = 0; i < 3; i++) {
			orientation.rotate();

			if (orientations.at(0) != orientation) {
				orientations.push_back(orientation);
			}
		}
	}

	if (SHUFFLE_ORIENTATIONS) {
		// https://www.geeksforgeeks.org/cpp/how-to-shuffle-a-vector-in-cpp/
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(orientations.begin(), orientations.end(), g);
	}
}

// data is a string with sequental data
PieceOrientation::PieceOrientation(std::string data, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// string returns actual value, convert to int
			this->arr[i][j] = data[i * cols + j] - '0';
		}
	}

	updateBoundingBox();
	sanityCheck();
}

// make sure pieces alternate 1s and 2s in correct places
bool PieceOrientation::sanityCheck() {
	// parity is whatever number is in the odd space
	int parity = -1;
	int oddParity = -1;
	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			bool isOddSquare = (i+ j % 2) % 2;
			if (parity == -1 && arr[i][j] != 0) {
				parity = ((arr[i][j] == 2 && !isOddSquare) || (arr[i][j] == 1 && isOddSquare)) ? 1 : 2;
				oddParity = parity == 1 ? 2 : 1;
				continue;
			}

			if ((!isOddSquare && arr[i][j] == parity) || (isOddSquare && arr[i][j] == oddParity)) {
				std::cout << "FAILED PIECE SANITY CHECK at i: " << i << " j: " << j << " with " << arr[i][j] << std::endl;
				printPiece();
				return false;
			}
		}
	}

	return true;
}

// transposing the matrix and flipping it across the y axis rotates it by 90 degrees counterclockwise
void PieceOrientation::rotate() {
	int tempPieceArr[PIECE_SIZE][PIECE_SIZE];

	// https://stackoverflow.com/questions/18709577/stdcopy-two-dimensional-array
	std::copy(&arr[0][0], &arr[0][0]+PIECE_SIZE*PIECE_SIZE, &tempPieceArr[0][0]);

	// transpose
	// y = x;
	// x = y;
	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			arr[j][i] = tempPieceArr[i][j];
		}
	}

	// flip y
	std::copy(&arr[0][0], &arr[0][0]+PIECE_SIZE*PIECE_SIZE, &tempPieceArr[0][0]);

	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			arr[i][j] = tempPieceArr[PIECE_SIZE - 1 - i][j];
		}
	}

	updateBoundingBox();
}


void PieceOrientation::updateBoundingBox() {
	// get startI
	startI = 0;
	for (int i = 0; i < PIECE_SIZE - 1; i++) {
		if (checkZeroRow(i)) startI = i + 1;
		else break;
	}
	
	// get endI
	endI = PIECE_SIZE - 1;
	for (int i = PIECE_SIZE - 1; i > 0; i--) {
		if (checkZeroRow(i)) endI = i - 1;
		else break;
	}

	startJ = 0;
	for (int j = 0; j < PIECE_SIZE - 1; j++) {
		if (checkZeroCol(j)) startJ = j + 1;
		else break;
	}

	endJ = PIECE_SIZE - 1;
	for (int j = PIECE_SIZE - 1; j > 0; j--) {
		if (checkZeroCol(j)) endJ = j - 1;
		else break;
	}
}

bool PieceOrientation::checkZeroRow(int row) const {
	for (int j = 0; j < PIECE_SIZE; j++) {
		if (arr[row][j] != 0) return false;
	}
	return true;
}

bool PieceOrientation::checkZeroCol(int col) const {
	for (int i = 0; i < PIECE_SIZE; i++) {
		if (arr[i][col] != 0) return false;
	}
	return true;
}

// compare pieces ignoring bounding box
bool operator == (PieceOrientation p1, PieceOrientation p2) {
	// compare bounding box
	if (p1.effectiveWidth() != p2.effectiveWidth() || p1.effectiveHeight() != p2.effectiveHeight()) return false;

	for (int i = 0; i < p1.effectiveHeight(); i++) {
		for (int j = 0; j < p1.effectiveWidth(); j++) {
			if  (p1.getCellRelative(i, j) != p2.getCellRelative(i, j)) return false;
		}
	}

	return true;
}

bool operator != (PieceOrientation p1, PieceOrientation p2) {
	return !(p1 == p2);
}

void PieceOrientation::printFullPiece() const {
	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < PIECE_SIZE; j++) {
			std::cout << arr[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void PieceOrientation::printPiece() const {
	for (int i = 0; i < effectiveHeight(); i++) {
		for (int j = 0; j < effectiveWidth(); j++) {
			std::cout << getCellRelative(i, j) << " ";
		}
		std::cout << std::endl;
	}
}

void PieceOrientation::printBoundingBox() const {
	std::cout << "sI: " << startI << " eI: " << endI << " sJ: " << startJ << " eJ: " << endJ << std::endl;
}
