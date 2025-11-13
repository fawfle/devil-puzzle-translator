import os
import sys
import colorsys
import random
import math
import pygame
import numpy as np;
from typing import List
import datetime

from piece import Piece
from button import Button

pygame.init();

SCREEN_SIZE = [770 * 2, 770 * 2]
GRID_SIZE = 77

BG_COLOR = pygame.Color("#000000")
PIECE_COLOR = pygame.Color("#252525")
HIGHLIGHT_COLOR = pygame.Color("#ffffff")
HIGHLIGHT_COLOR_ALT = pygame.Color("#ff0000")

screen = pygame.display.set_mode(SCREEN_SIZE)

absolute_path = os.path.dirname(os.path.abspath(__file__))
solutionFile = open(absolute_path + '/../../200pc_solution.txt', "r")

fileContent = solutionFile.read().replace('\n', '');

numbersStrings = list(filter(None, fileContent.split(",")));
numbers = list(map(int, numbersStrings))

grid = [];
piece_areas = [0 for _ in range(200)];
pieces: List[Piece] = [];

highlighted_piece = 0;

# create more usable data structure
for i in range(GRID_SIZE):
    row = [];
    for j in range(GRID_SIZE):
        num = numbers[i * GRID_SIZE + j]
        row.append(num);
        if num >= 0:
            piece_areas[num - 1] += 1;
    grid.append(row);

np_grid = np.array(grid);

for i in range(200):
    cutout_array = np.where(np_grid != i + 1, 0, np_grid);
    p = Piece(i + 1, cutout_array);
    pieces.append(p)

# create pieces from input
def find_piece_in_grid(test_piece):
    for p in pieces:
        if (p.is_equal(test_piece)):
            return p;
    return None



def randomColor():
    randColor = colorsys.hsv_to_rgb(random.uniform(0,1), random.uniform(0.5, 1), random.uniform(0.5, 1));
    return pygame.Color((int)(randColor[0] * 255), (int)(randColor[1] * 255), (int)(randColor[2] * 255));

def draw_rect_centered(surface, color, rect):
    pygame.draw.rect(surface, color, rect)

def draw_octagon(surface, color, center, radius):
    points = []
    for i in range(8):
        angle = 2 * math.pi / 8 * i + math.pi / 8
        x = center[0] + radius * math.cos(angle)
        y = center[1] + radius * math.sin(angle)
        points.append((x, y))

    pygame.draw.polygon(surface, color, points)

if len(numbers) != 77*77:
    print("error, incorrect number of numbers");
    sys.exit();

randomColors = [];

for i in range(201):
    randomColors.append(randomColor());

buttons: List[Button] = [];
button_canvas_start = (screen.get_width() * 0.78, screen.get_height() * 0.03)
button_canvas_size = (13, 13)
button_size = 25;

input_piece = np.zeros(button_canvas_size)

def set_input_piece_cell(x, y, pressed):
    input_piece[x][y] = 1 if pressed else 0
    update_highlighted_piece()

def update_highlighted_piece():
    new_found = find_piece_in_grid(Piece(0, input_piece).arr);
    if new_found is None:
        return

    print("new found piece")
    global highlighted_piece;
    highlighted_piece = new_found.index;

for x in range(button_canvas_size[0]):
    for y in range(button_canvas_size[1]):
        isOctagon = (x + y % 2) % 2 != 0
        # https://stackoverflow.com/questions/10452770/python-lambdas-binding-to-local-values
        b = Button((button_canvas_start[0] + x * button_size, button_canvas_start[1] + y * button_size), button_size, lambda p, x=x, y=y: set_input_piece_cell(y, x, p), "O" if isOctagon else "S");
        buttons.append(b)

def reset_buttons():
    for b in buttons:
        b.selected = False;
        global input_piece
        input_piece = np.zeros(button_canvas_size)

def draw_grid():
    screenSize = screen.get_size();
    step = screenSize[0] / 77;
    for i in range(len(numbers)):
        if numbers[i] == 0: continue

        x = i % 77;
        y = i // 77;

        isOctagon = (x + y % 2 ) % 2 != 0
        size = 15 if isOctagon else 10;
        color = randomColors[numbers[i]];
        if numbers[i] == highlighted_piece:
            color = HIGHLIGHT_COLOR
            if datetime.datetime.now().second % 2 == 0:
                color = HIGHLIGHT_COLOR_ALT

        screenX = (x + 0.5) * step
        screenY = (y + 0.5) * step

        if isOctagon: draw_octagon(screen, color, (screenX, screenY), size);
        else: draw_rect_centered(screen, color, pygame.Rect(screenX - size/2, screenY - size/2, size, size))

running = True;

while running:
    for event in pygame.event.get():
        if (event.type == pygame.QUIT):
            running = False;
            break;

    screen.fill(BG_COLOR);

    draw_grid();

    pressed = pygame.key.get_pressed()
    if pressed[pygame.K_r]:
        reset_buttons()

    for b in buttons:
        b.process(screen)

    pygame.display.flip();

pygame.quit();
