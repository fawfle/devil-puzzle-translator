import sys
import colorsys
import random
from PIL import Image

SCALING_FACTOR = 10;

solutionFile = open("./200pc_solution.txt", "r")

fileContent = solutionFile.read().replace('\n', '');

numbers = list(filter(None, fileContent.split(",")));
numbers = list(map(int, numbers))

def randomColor():
    randColor = colorsys.hsv_to_rgb(random.uniform(0,1), random.uniform(0.5, 1), random.uniform(0.5, 1));
    return ((int)(randColor[0] * 255), (int)(randColor[1] * 255), (int)(randColor[2] * 255), 255)

if len(numbers) != 77*77:
    print("error, incorrect number of numbers");
    sys.exit();

image = Image.new(mode="RGB", size=(77, 77))

randomColors = [];

for i in range(201):
    randomColors.append(randomColor());

for i in range(len(numbers)):
    if numbers[i] == 0: continue

    x = i % 77;
    y = i // 77;
    image.putpixel((x, y), randomColors[numbers[i]])

image = image.resize((77 * SCALING_FACTOR, 77 * SCALING_FACTOR), Image.Resampling.NEAREST)
image.show();
