import pygame
from math import sqrt

BLACK = (0, 0, 0)

class Ball(pygame.sprite.Sprite):
    def __init__(self, color, radius, path=[], speed=1, block_size=5, border_size=1):
        super().__init__()

        self.BLOCK_SIZE = block_size
        self.BORDER_SIZE = border_size

        self.image = pygame.Surface([radius * 2, radius * 2])
        self.image.fill(BLACK)
        self.image.set_colorkey(BLACK)

        self.path = path.copy()
        self.nx = 1
        self.speed = speed

        pygame.draw.circle(self.image, color, [radius, radius], radius)

        self.rect = self.image.get_rect()
        self.rect.x = path[0][1] * (self.BLOCK_SIZE + self.BORDER_SIZE)
        self.rect.y = path[0][0] * (self.BLOCK_SIZE + self.BORDER_SIZE)
        self.real_x = self.rect.x
        self.real_y = self.rect.y

    def update(self):
        x = self.real_x
        y = self.real_y
        dist = 0
        while self.nx != len(self.path):
            nx = self.path[self.nx]
            nxc = (nx[1] * (self.BLOCK_SIZE + self.BORDER_SIZE),
                   nx[0] * (self.BLOCK_SIZE + self.BORDER_SIZE))
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