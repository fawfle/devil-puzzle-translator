import numpy as np
from numpy.typing import NDArray

class Piece:
    arr : np.ndarray;

    def __init__(self, index: int, arr: np.ndarray):
        self.arr = arr
        self.width = 0;
        self.height = 0;
        self.index = index;
        self.update_bounding_box()

    # find dimensions of effective piece size
    def update_bounding_box(self):
        self.startI = 0;
        endI = self.arr.shape[0] - 1;
        self.startJ = 0;
        endJ = self.arr.shape[1] - 1;

        for i in range(self.arr.shape[0] - 1):
            if np.all(self.arr[i, :] == 0):
                self.startI += 1;
            else:
                break;

        for i in range(self.arr.shape[0] - 1, 0, -1):
            if np.all(self.arr[i, :] == 0):
                endI -= 1;
            else:
                break;

        for j in range(self.arr.shape[1] - 1):
            if np.all(self.arr[:, j] == 0):
                self.startJ += 1;
            else:
                break;

        for j in range(self.arr.shape[1] - 1, 0, -1):
            if np.all(self.arr[:, j] == 0):
                endJ -= 1;
            else:
                break;

        self.height = 1 + endI - self.startI;
        self.width = 1 + endJ - self.startJ;

        self.arr = self.arr[self.startI:(endI + 1), self.startJ:(endJ + 1)]

    def print(self):
        for i in range(self.height):
            row = "";
            for j in range(self.width):
                row += str(self.arr[i, j]) + " "
            print(row);

    def is_equal(self, p: NDArray) -> bool:
        arr = np.where(p != 0, self.index, p);
        for i in range(4):
            if np.array_equal(self.arr, arr): return True
            arr = np.rot90(arr);

        return False;
