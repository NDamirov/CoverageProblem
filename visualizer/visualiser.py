from time import sleep
import pygame
from ball import Ball

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)

pygame.init()
screen = pygame.display.set_mode((0, 0))
display_size = screen.get_size()

width, height = map(int, input().split())
field = []
for i in range(width):
    field.append(list(input()))

TOTAL_BLOCK_SIZE = min(display_size[0] // height, display_size[1] // width, 10)
BLOCK_SIZE = TOTAL_BLOCK_SIZE - 1
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

screen = pygame.display.set_mode([SCREEN_WIDTH, SCREEN_HEIGHT])
pygame.display.set_caption("Coverage visualizer")

path_size = int(input())
path = []
for i in range(path_size):
    x, y = map(int, input().split())
    path.append((x, y))

all_sprites_list = pygame.sprite.Group()
ball = Ball(WHITE, BLOCK_SIZE // 2, path, 50, BLOCK_SIZE, BORDER_SIZE)
all_sprites_list.add(ball)

cur_x, cur_y = path[0]
for x, y in path[1:]:
    pygame.draw.line(background, GREEN, (cur_y * (BLOCK_SIZE + BORDER_SIZE) + BLOCK_SIZE / 2,
                                         cur_x * (BLOCK_SIZE + BORDER_SIZE) + BLOCK_SIZE / 2),     
                                        (y * (BLOCK_SIZE + BORDER_SIZE) + BLOCK_SIZE / 2,
                                         x * (BLOCK_SIZE + BORDER_SIZE) + BLOCK_SIZE / 2))
    cur_x, cur_y = x, y

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
