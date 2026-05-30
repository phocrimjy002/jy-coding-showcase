import sys
sys.stdin = open("crosswords.in", "r")
sys.stdout = open("crosswords.out", "w")

crossword = []
clues = []

rows, columns = map(int, input().split())

for _ in range(rows):
    crossword.append(input().strip())

def update_clues():
    global clues
    starting_points = 0

    for r in range(rows):
        for c in range(columns):

            if crossword[r][c] == ".":

                if (c == 0 or crossword[r][c - 1] == "#") \
                        and c + 2 < columns \
                        and crossword[r][c + 1] == "." \
                        and crossword[r][c + 2] == ".":

                    clues.append((r + 1, c + 1))
                    starting_points += 1

                elif (r == 0 or crossword[r - 1][c] == "#") \
                        and r + 2 < rows \
                        and crossword[r + 1][c] == "." \
                        and crossword[r + 2][c] == ".":

                    clues.append((r + 1, c + 1))
                    starting_points += 1

    return starting_points


print(update_clues())
for r, c in clues:
    print(r, c)
