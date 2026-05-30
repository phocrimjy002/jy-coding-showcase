with open("marathon.in", "r") as f:
    data = f.read().strip().split()

idx = 0
N = int(data[idx])
idx += 1

points = []
for _ in range(N):
    x = int(data[idx])
    y = int(data[idx + 1])
    points.append((x, y))
    idx += 2


def manhattan(a, b):
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

total = 0
for i in range(N - 1):
    total += manhattan(points[i], points[i + 1])

max_saved = 0
for i in range(1, N - 1):
    saved = (
        manhattan(points[i - 1], points[i]) +
        manhattan(points[i], points[i + 1]) -
        manhattan(points[i - 1], points[i + 1])
    )
    max_saved = max(max_saved, saved)

answer = total - max_saved

with open("marathon.out", "w") as f:
    f.write(str(answer))
