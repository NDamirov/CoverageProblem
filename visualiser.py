from time import sleep
import pygame
import random
from math import sqrt

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
RED = (255, 0, 0)

width, height = map(int, input().split())
field = []
for i in range(width):
    field.append(list(input()))

BLOCK_SIZE = 8
BORDER_SIZE = 1
SCREEN_WIDTH = len(field[0]) * (BLOCK_SIZE + BORDER_SIZE)
SCREEN_HEIGHT = len(field) * (BLOCK_SIZE + BORDER_SIZE)

background = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT))
background.fill(WHITE)

for i in range(len(field)):
    for j in range(len(field[i])):
        color = BLACK
        if field[i][j] == '|': color = RED
        elif field[i][j] == '-': color = BLUE
        pygame.draw.rect(background, color, [j * (BLOCK_SIZE + BORDER_SIZE),
                        i * (BLOCK_SIZE + BORDER_SIZE),
                        BLOCK_SIZE, BLOCK_SIZE])
        if field[i][j] == '|' and i + 1 < len(field) and field[i + 1][j] == '|':
            pygame.draw.rect(background, color, [j * (BLOCK_SIZE + BORDER_SIZE),
                            i * (BLOCK_SIZE + BORDER_SIZE),
                            BLOCK_SIZE, BLOCK_SIZE + BORDER_SIZE])
        if field[i][j] == '-' and j + 1 < len(field[i]) and field[i][j + 1] == '-':
            pygame.draw.rect(background, color, [j * (BLOCK_SIZE + BORDER_SIZE),
                            i * (BLOCK_SIZE + BORDER_SIZE),
                            BLOCK_SIZE + BORDER_SIZE, BLOCK_SIZE])

class Ball(pygame.sprite.Sprite):
    def __init__(self, color, radius, path=[], speed=1):
        super().__init__()

        self.image = pygame.Surface([radius * 2, radius * 2])
        self.image.fill(BLACK)
        self.image.set_colorkey(BLACK)

        self.path = path.copy()
        self.nx = 1
        self.speed = speed

        pygame.draw.circle(self.image, color, [radius, radius], radius)

        self.rect = self.image.get_rect()
        self.rect.x = path[0][1] * (BLOCK_SIZE + BORDER_SIZE)
        self.rect.y = path[0][0] * (BLOCK_SIZE + BORDER_SIZE)
        self.real_x = self.rect.x
        self.real_y = self.rect.y

    def update(self):
        x = self.real_x
        y = self.real_y
        dist = 0
        while self.nx != len(path):
            nx = self.path[self.nx]
            nxc = (nx[1] * (BLOCK_SIZE + BORDER_SIZE),
                   nx[0] * (BLOCK_SIZE + BORDER_SIZE))
            curr = sqrt((x - nxc[0]) ** 2 + (y - nxc[1]) ** 2)
            if dist + curr <= self.speed:
                dist += curr
                self.nx += 1
                x = nxc[0]
                y = nxc[1]
                continue
            to_go = self.speed - dist
            x += (nxc[0] - x) * to_go / curr
            y += (nxc[1] - y) * to_go / curr
            break
        
        self.rect.x = x
        self.rect.y = y
        self.real_x = x
        self.real_y = y

pygame.init()
screen = pygame.display.set_mode([SCREEN_WIDTH, SCREEN_HEIGHT])
pygame.display.set_caption("Coverage visualizer")

path_size = int(input())
path = []
for i in range(path_size):
    x, y = map(int, input().split())
    path.append((x, y))

all_sprites_list = pygame.sprite.Group()
ball = Ball(WHITE, BLOCK_SIZE // 2, path, 10)
all_sprites_list.add(ball)

clock = pygame.time.Clock()
done = False
while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True

    screen.blit(background, (0, 0))
    all_sprites_list.update()
    all_sprites_list.draw(screen)

    pygame.display.flip()    
    clock.tick(30)

pygame.quit()
