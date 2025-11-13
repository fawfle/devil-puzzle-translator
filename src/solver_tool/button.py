import pygame
import math

class Button():
    def __init__(self, pos, size, on_click, type="O"):
        self.pos = pos
        self.size = size
        self.on_click = on_click

        self.pressed_last = False;
        self.selected = False;
        self.type = type

        self.rect = pygame.Rect(pos[0], pos[1], size, size);

        self.colors = {
            "normal": pygame.Color("#252525"),
            "selected": pygame.Color("#ffffff"),
            "hovered": pygame.Color("#505050")
        }

    def process(self, screen):
        mouse_pos = pygame.mouse.get_pos()
        hovered = self.rect.collidepoint(mouse_pos);
        
        if hovered and pygame.mouse.get_pressed()[0] and not self.pressed_last:
            self.selected = not self.selected;
            self.on_click(self.selected)
            self.pressed_last = True

        if not pygame.mouse.get_pressed()[0]:
            self.pressed_last = False

        current_color = "selected" if self.selected else ("hovered" if hovered else "normal")

        if self.type == "O":
            scale_size = self.size * 0.75
            draw_octagon(screen, self.colors[current_color], (self.pos[0] + scale_size / 2, self.pos[1] + scale_size / 2), scale_size)
        else:
            pygame.draw.rect(screen, self.colors[current_color], self.rect)
            pygame.draw.rect(screen, pygame.Color("#000000"), self.rect, 2)

def draw_octagon(surface, color, center, radius):
    points = []
    for i in range(8):
        angle = 2 * math.pi / 8 * i + math.pi / 8
        x = center[0] + radius * math.cos(angle)
        y = center[1] + radius * math.sin(angle)
        points.append((x, y))

    pygame.draw.polygon(surface, color, points)
    pygame.draw.polygon(surface, pygame.Color("#000000"), points, 2)
