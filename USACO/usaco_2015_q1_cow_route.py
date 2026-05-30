import sys

sys.stdin = open("cowroute.in")
sys.stdout = open("cowroute.out", "w")

A, B, N = map(int, input().split())

answer = float('inf')

for _ in range(N):
    cost, k = map(int, input().split())
    cities = list(map(int, input().split()))

    posA = -1
    posB = -1

    for i in range(k):
        if cities[i] == A:
            posA = i
        if cities[i] == B:
            posB = i

    if posA != -1 and posB != -1 and posA < posB:
        answer = min(answer, cost)

if answer == float('inf'):
    print(-1)
else:
    print(answer)
