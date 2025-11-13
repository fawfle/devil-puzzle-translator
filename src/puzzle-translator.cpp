#include "pieces.hpp"
#include "doubly-linked-list.hpp"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

#define GRID_SIZE 77

// shuffle pieces to avoid bias caused by importing from the solution
#define SHUFFLE_PIECES false

// check cutout of piece in solution grid
bool testPieceInSolution(int (&grid)[GRID_SIZE][GRID_SIZE], int pieceIndex, PieceOrientation &orientation, int row, int col) {
	for (int i = 0; i < orientation.effectiveHeight(); i++) {
		for (int j = 0; j < orientation.effectiveWidth(); j++) {
			if (orientation.getCellRelative(i, j) != 0 && grid[row + i][col + j] != pieceIndex) return false;
		}
	}
	return true;
}

void findTestCell(PieceOrientation &orientation, int &testI, int &testJ, int &testNum) {
	for (int i = 0; i < orientation.effectiveHeight(); i++) {
		for (int j = 0; j < orientation.effectiveWidth(); j++) {
			if (orientation.getCellRelative(i, j) != 0) {
				testI = i;
				testJ = j;
				testNum = orientation.getCellRelative(i, j);
				return;
			}
		}
	}
}

struct SuperPieceSolution {
	Piece piece;
	vector<Piece> subPieces;
};

bool checkSubPiece(PieceOrientation &superPiece, PieceOrientation &subPiece, int row, int col) {
	for (int i = 0; i < subPiece.effectiveHeight(); i++) {
		for (int j = 0; j < subPiece.effectiveWidth(); j++) {
			// check if the subPiece has something, then make sure the superpiece has that. Don't check empty space, it can overlap
			if (subPiece.getCellRelative(i, j) != 0 && (superPiece.getCellRelative(row + i, col + j) != subPiece.getCellRelative(i, j))) {
				return false;
			}
		}
	}
	return true;
}

PieceOrientation coverPiece(PieceOrientation superPiece, PieceOrientation subPiece, int row, int col) {
	for (int i = 0; i < subPiece.effectiveHeight(); i++) {
		for (int j = 0; j < subPiece.effectiveWidth(); j++) {
			if (subPiece.getCellRelative(i, j) != 0) {
				superPiece.setCellRelative(row + i, col + j, 0);
			}
		}
	}
	return superPiece;
}

// finds subpieces and stores them in the solutions vector. Solution is an empty vector used by the function to store possible solutions.
void findSubPieces(PieceOrientation piece, DLL<Piece>* subPieces, vector<vector<Piece>> &solutions, vector<Piece> &solution) {
	// not really an "airtight" check since the bounding box is calculated weirdly, but negative should always work
	if (piece.effectiveHeight() <= 0 && piece.effectiveWidth() <= 0) {
		solutions.push_back(solution);
		return;
	}

	auto subPiece = subPieces->head->right;
	// store removed nodes for later
	vector<DLLNode<Piece>*> removed;

	// check both orientations and every possible position within the superPiece
	while (subPiece != subPieces->head) {
		for (PieceOrientation orientation : subPiece->data.orientations) {
			for (int row = 0; row <= piece.effectiveHeight() - orientation.effectiveHeight(); row++) {
				for (int col = 0; col <= piece.effectiveWidth() - orientation.effectiveWidth(); col++) {
					if (checkSubPiece(piece, orientation, row, col)) {
						// create a new superPiece to be recursively checked
						auto coveredPiece = coverPiece(piece, orientation, row, col);
						// reduce bounding box for more optimized searching
						coveredPiece.updateBoundingBox();
						subPieces->remove(subPiece);
						solution.push_back(subPiece->data);
						findSubPieces(coveredPiece, subPieces, solutions, solution);
						subPieces->insert(subPiece);
						solution.pop_back();
					}
				}
			}
		}

		// check for when subPiece isn't in
		subPiece = subPiece->right;

		removed.push_back(subPiece->left);
		subPieces->remove(subPiece->left);
	}

	// cleanup, re-add removed nodes. Make sure it's in reverse order
	for (int i = removed.size() - 1; i >= 0; i--) {
		subPieces->insert(removed.at(i));
	}
}

// find the positions of the pieces in solution. Solution is assumed to be correct
bool findSubPiecesPositions(PieceOrientation piece, vector<Piece> &solution, uint index = 0) {
	if (index == solution.size()) return true;

	auto subPiece = &solution.at(index);

	for (uint i = 0; i < subPiece->orientations.size(); i++) {
		auto orientation = subPiece->orientations.at(i);

		for (int row = 0; row <= piece.effectiveHeight() - orientation.effectiveHeight(); row++) {
			for (int col = 0; col <= piece.effectiveWidth() - orientation.effectiveWidth(); col++) {
				if (checkSubPiece(piece, orientation, row, col)) {
					subPiece->rowOffset = row;
					subPiece->colOffset = col;
					subPiece->orientationIndex = i;
					
					auto coveredPiece = coverPiece(piece, orientation, row, col);
					bool finished = findSubPiecesPositions(coveredPiece, solution, index + 1);
					if (finished) return true;
				}
			}
		}
	}
	return false;
}

DLL<Piece>* createPieceData(string filepath) {
	DLL<Piece>* data = new DLL<Piece>();
	ifstream fileStream{filepath};

	if (!fileStream.is_open()) {
		throw std::invalid_argument("Error opening file");
		return data;
	}

	vector<string> lines;
	string line;
	while(getline(fileStream, line)) {
		lines.push_back(line);
	}

	if (SHUFFLE_PIECES) {
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(lines.begin(), lines.end(), g);
	}

	for (string l : lines) {
		stringstream ss(l);

		string heightString;
		string widthString;
		string dataString;
		getline(ss, heightString, ',');
		getline(ss, widthString, ',');
		getline(ss, dataString, ',');

		data->append(Piece(dataString, stoi(heightString), stoi(widthString)));
	}
	
	return data;
}

// side effect hell
void createInputSolutionMatrix(int (&matrix)[GRID_SIZE][GRID_SIZE]) {
	ifstream inputSolutionStream("./src/100pc_solution.txt");

	if (!inputSolutionStream.is_open()) {
		cerr << "Error opening INPUT solution file.";
		return;
	}

	string line;
	int index = 0;
	while(getline(inputSolutionStream, line)) {
		stringstream ss(line);
		string num;
		while (getline(ss, num, ',')) {
			matrix[index / GRID_SIZE][index % GRID_SIZE] = stoi(num);
			index++;
		}
	}
}

int main() {
	DLL<Piece>* superPieces = createPieceData("./src/piece_generation/100pc.txt");
	DLL<Piece>* subPieces = createPieceData("./src/piece_generation/200pc.txt");

	chrono::steady_clock::time_point startTime = chrono::steady_clock::now();

	cout << "Finding Solutions\n";
	int count = 1;

	vector<SuperPieceSolution> solutionsList;
	for (auto curr = superPieces->head->right; curr != superPieces->head; curr = curr->right) {
		vector<vector<Piece>> solutions;
		vector<Piece> _solution;
		// we only care about 1 orientation for finding the correct subpieces
		findSubPieces(curr->data.orientations[0], subPieces, solutions, _solution);

		if (solutions.size() > 0) solutionsList.push_back(SuperPieceSolution{ curr->data, solutions[0] });

		cout << "for piece: " << count << " found: " << solutions.size() << "\n";
		count++;
	}

	cout << "Finished search\n";

	int sanityCount = 0;

	// sanity check
	for (auto curr = subPieces->head->right; curr != subPieces->head; curr = curr->right) {
		sanityCount++;
		if (curr->left->right != curr) cout << "LEFT ERROR";
		if (curr->right->left != curr) cout << "RIGHT ERROR";
	}

	int pieceSum = 0;

	for (auto solutions : solutionsList) {
			pieceSum += solutions.subPieces.size();
	}

	cout << "Piece sum: " << pieceSum << endl;

	chrono::steady_clock::time_point endTime = chrono::steady_clock::now();
	cout << "Searching Elapsed = " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() * 0.001 << " [s]" << std::endl;

	// create matrix for solution
	int solutionMatrix[GRID_SIZE][GRID_SIZE] = {0};
	int outputMatrix[GRID_SIZE][GRID_SIZE] = {0};

	createInputSolutionMatrix(solutionMatrix);

	// array for size of pieces
	int pieceAreas[200] = {0};

	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			int pieceIndex = solutionMatrix[i][j] - 1;
			if (pieceIndex >= 0) pieceAreas[pieceIndex]++;
		}
	}

	// index of subpiece for solution
	int subPieceIndex = 1;

	for (auto superPieceSolution : solutionsList) {
		// information about solution
		int solutionRow, solutionCol = 0;
		PieceOrientation *solutionOrientation = nullptr;

		// number of 1's and 0's
		int pieceArea = superPieceSolution.piece.orientations[0].area();

		// find out where the superPiece is and what orientation
		for (PieceOrientation orientation : superPieceSolution.piece.orientations) {
			// find non-zero entry. Useful for checking things
			int testI, testJ, testNum;

			findTestCell(orientation, testI, testJ, testNum);

			for (int i = 0; i <= GRID_SIZE - orientation.effectiveHeight(); i++) {
				for (int j = 0; j <= GRID_SIZE - orientation.effectiveWidth(); j++) {
					// test parity (1/2 in the right spot). Top left is a 2
					// int expected = (((i + testI) + (j + testJ) % 2) % 2 == 0) ? 2 : 1;
					// if (expected != testNum) continue;

					int pieceIndex = solutionMatrix[i + testI][j + testJ];
					if (pieceIndex == 0) continue;
					if (pieceAreas[pieceIndex - 1] != pieceArea) continue;

					if (testPieceInSolution(solutionMatrix, pieceIndex, orientation, i, j)) {
						solutionRow = i;
						solutionCol = j;
						solutionOrientation = &orientation;
						// goto?!? Use it like a return statement here
						goto positionFound;
					}
				}
			}
		}
		positionFound:

		if (solutionOrientation == nullptr) {
			cerr << "piece not found in solution";
			return 1;
		}

		// find the way subpieces make superpiece
		bool foundSubPieces = findSubPiecesPositions(*solutionOrientation, superPieceSolution.subPieces);
		
		if (!foundSubPieces) {
			cerr << "subpieces not found in superpiece";
			return 1;
		}

		// update solution in output matrix
		for (auto subPiece : superPieceSolution.subPieces) {
			auto orientation = subPiece.orientations[subPiece.orientationIndex];
			for (int i = 0; i < orientation.effectiveHeight(); i++) {
				for (int j = 0; j < orientation.effectiveWidth(); j++) {
					if (orientation.getCellRelative(i, j) != 0) outputMatrix[solutionRow + subPiece.rowOffset + i][solutionCol + subPiece.colOffset + j] = subPieceIndex;
				}
			}
			subPieceIndex++;
		}
	}

	// write to the output file
	ofstream outputSolutionFile("./200pc_solution.txt");

	if (!outputSolutionFile.is_open()) {
		cerr << "Error opening OUTPUT solution file.";
		return 1;
	}

	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			outputSolutionFile << outputMatrix[i][j] << ',';
		}
		outputSolutionFile << endl;
	}
}

