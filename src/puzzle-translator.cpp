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

// shuffle pieces to avoid bias caused by importing from the solution
#define SHUFFLE_PIECES true

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
	superPiece.updateBoundingBox();
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

int main() {
	DLL<Piece>* superPieces = createPieceData("./src/piece_generation/100pc.txt");
	DLL<Piece>* subPieces = createPieceData("./src/piece_generation/200pc.txt");

	chrono::steady_clock::time_point startTime = chrono::steady_clock::now();

	vector<vector<vector<Piece>>> solutionsList;
	for (auto curr = superPieces->head->right; curr != superPieces->head; curr = curr->right) {
		vector<vector<Piece>> solutions;
		vector<Piece> _solution;
		// we only care about 1 orientation
		findSubPieces(curr->data.orientations[0], subPieces, solutions, _solution);
		solutionsList.push_back(solutions);
	}

	int sanityCount = 0;

	// sanity check
	for (auto curr = subPieces->head->right; curr != subPieces->head; curr = curr->right) {
		sanityCount++;
		if (curr->left->right != curr) cout << "LEFT ERROR";
		if (curr->right->left != curr) cout << "RIGHT ERROR";
	}

	int pieceSum = 0;

	for (auto solutions : solutionsList) {
		cout << solutions.size();
		if (solutions.size() > 0) {
			cout << " " << solutions.at(0).size();
			pieceSum += solutions.at(0).size();
		}
		cout << "\n";
	}

	cout << "Piece sum: " << pieceSum << endl;

	chrono::steady_clock::time_point endTime = chrono::steady_clock::now();
	cout << "Searching Elapsed = " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() * 0.001 << " [s]" << std::endl;
}

