import sys
sys.stdin = open("moocrypt.in")
sys.stdout = open("moocrypt.out", "w")


def solve():

    line1 = input().split()
    N, M = map(int, line1)

    grid = []
    for _ in range(N):
        grid.append(input().strip())

    possible_moos = {}

    directions = [
        (0, 1), (0, -1), (1, 0), (-1, 0),
        (1, 1), (1, -1), (-1, 1), (-1, -1)
    ]

    for r in range(N):
        for c in range(M):
            char1 = grid[r][c]

            if char1 == 'M':
                continue

            for dr, dc in directions:
                r2, c2 = r + dr, c + dc
                r3, c3 = r + 2 * dr, c + 2 * dc

                if 0 <= r3 < N and 0 <= c3 < M:
                    char2 = grid[r2][c2]
                    char3 = grid[r3][c3]

                    if char2 == char3 and char1 != char2:
                        if char2 != 'O':
                            pair = (char1, char2)
                            possible_moos[pair] = possible_moos.get(pair, 0) + 1

    print(max(possible_moos.values()))


solve()
