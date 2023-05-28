import sys
from random import randint

h, w, amount = map(int, sys.argv[1:])
arr = [['#' for i in range(w)] for i in range(h)]

def fill_square(x, y, ht, wt, map):
    for i in range(ht):
        for j in range(wt):
            map[x + i][y + j] = '*'

for i in range(amount):
    x = randint(1, h - 4)
    y = randint(1, w - 4)
    ht = randint(1, (h - x - 1) // 2)
    wt = randint(1, (w - y - 1) // 2)
    fill_square(x, y, ht, wt, arr)

with open('out.txt', 'w') as file:
    for i in range(h):
        print(*arr[i], sep='', file=file)