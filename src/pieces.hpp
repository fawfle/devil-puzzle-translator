#define PIECE_SIZE 23

#include <string>
#include <vector>

struct PieceOrientation {
	private:
		int startI = 0;
		int endI = PIECE_SIZE - 1;
		int startJ = 0;
		int endJ = PIECE_SIZE - 1;

	public:
		PieceOrientation(std::string data, int rows, int cols);

		int arr[PIECE_SIZE][PIECE_SIZE] = {0};

		void rotate();
		void updateBoundingBox();

		bool checkZeroRow(int row) const;
		bool checkZeroCol(int col) const;

		void printPiece() const;
		void printFullPiece() const;
		void printBoundingBox() const;

		int effectiveHeight() const { return 1 + endI - startI; };
		int effectiveWidth() const { return 1 + endJ - startJ; };

		int area() const;

		// return piece cells relative to bounding box
		int getCellRelative(int i, int j) const { return arr[i + startI][j + startJ]; };
		void setCellRelative(int i, int j, int value) { arr[i + startI][j + startJ] = value; };

		friend bool operator == (const PieceOrientation p1, const PieceOrientation p2);
		friend bool operator != (const PieceOrientation p1, const PieceOrientation p2);
		
		bool sanityCheck();
};

// wrapper for multiple orientations. Gets unique orientations and stores them in orientations
struct Piece {
	public:
		Piece();
		Piece(std::string data, int rows, int cols);
		std::vector<PieceOrientation> orientations;

		// position and orientation data for subpiece
		int orientationIndex = 0;
		int rowOffset = 0;
		int colOffset = 0;
};
