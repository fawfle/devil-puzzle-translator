import random
import colorsys
from PIL import Image

im = Image.open("./Octagonal Puzzle 200pc Process Image.png").copy();

class PixelData:
    def __init__(self, x, y, color) -> None:
        self.x = x;
        self.y = y;
        self.pos: tuple[int, int] = (x, y)
        self.color = color;

def to_pixel_x(x: int) -> int:
    return x * IMAGE_STEP + IMAGE_OFFSET_X;

def to_pixel_y(y: int) -> int:
    return y * IMAGE_STEP +  IMAGE_OFFSET_Y;

# recursively check pixel neighbors to construct a list of positions in the piece
def check_piece(pixel: PixelData, res: list[tuple[int, int]]):
    res.append(pixel.pos)

    # check left, right, top, and bottom neighbors
    offsets = [[-1, 0], [1, 0], [0, 1], [0, -1]];

    for offset in offsets:
        checkX = pixel.x + offset[0];
        checkY = pixel.y + offset[1];

        if (checkX >= matrix_width or checkY >= matrix_height): continue;

        check_pixel: PixelData = pixel_matrix[checkY][checkX];

        same_color = check_pixel.color == pixel.color;

        if (not same_color or check_pixel.pos in res): continue;

        # res.append(check_pixel.pos);

        check_piece(check_pixel, res);

    return res;


print("w:" + str(im.size[0]))
print("h:" + str(im.size[1]))

IMAGE_OFFSET_X = 3;
IMAGE_OFFSET_Y = 2;

IMAGE_STEP = 24;

# size of effective grid
matrix_width = im.size[0] // IMAGE_STEP + 1;
matrix_height = im.size[0] // IMAGE_STEP + 2;

# matrix of pixels skipping by gridsize. [y][x]
pixel_matrix = [[] for _ in range(matrix_height)];

# pixels to be checked for pieces
check_pixels = [];

# pixels that have already been added to a piece (for skipping)
covered_positions = [];

# list of coordinate data for pieces
pieces = [];

for j in range(0, matrix_height):
    for i in range(0, matrix_width):
        x = to_pixel_x(i);
        y = to_pixel_y(j);
 
        pixel_matrix[j].append(PixelData(i, j, im.getpixel((x, y))));
        
        if im.getpixel((x, y)) == (0, 0, 0, 0) or im.getpixel((x, y)) == (255, 255, 255, 255):
            im.putpixel((x, y), (25, 25, 25, 25))
            continue

        check_pixels.append(PixelData(i, j, im.getpixel((x, y))));
        im.putpixel((x, y), (0, 0, 0));
        # print("putting pixel: " + str(i) + " " + str(j));

#check every colored pixel for piece
#for pixel in check_pixels:
#    if (pixel.pos in covered_pixels): continue
#
#    # recursively check neighbors for having colors

# res = [];
# check_piece(check_pixels[0], res);
# print(res);



def randomColor():
    randColor = colorsys.hsv_to_rgb(random.uniform(0, 1), 1, 1);
    return ((int)(randColor[0] * 255), (int)(randColor[1] * 255), (int)(randColor[2] * 255), 255)

for pixel in check_pixels:
    if pixel.pos in covered_positions:
        continue

    piece = [];
    check_piece(pixel, piece);

    pieces.append(piece);

    covered_positions.extend(piece);

    color = randomColor();
    for coord in piece:
        x = to_pixel_x(coord[0]);
        y = to_pixel_y(coord[1]);

        im.putpixel((x, y), color);

print("piece count: " + str(len(pieces)));

outfile = open("200pc.txt", "w")

def is_big_square(x:int, y:int) -> bool:
    return (x + y % 2) % 2 != 0;

for piece in pieces:
    min_x = piece[0][0];
    max_x = piece[0][0];
    min_y = piece[0][1];
    max_y = piece[0][1];
    for coord in piece:
        if coord[0] < min_x:
            min_x = coord[0]
        if coord[0] > max_x:
            max_x = coord[0]
        if coord[1] < min_y:
            min_y = coord[1]
        if coord[1] > max_y:
            max_y = coord[1]

    piece_width = (max_x - min_x) + 1;
    piece_height = (max_y - min_y) + 1;

    piece_matrix = [[0 for _ in range(piece_width)] for _ in range(piece_height)];

    for coord in piece:
        relative_x = coord[0] - min_x;
        relative_y = coord[1] - min_y;
        piece_matrix[relative_y][relative_x] = 1 if is_big_square(coord[0], coord[1]) else 2;


    text = str(piece_height) + "," + str(piece_width) + ",";
    for y in range(piece_height):
        for x in range(piece_width):
            text += str(piece_matrix[y][x])

    text += "\n";

    print(text);

    outfile.write(text);

    # break

im.show();
