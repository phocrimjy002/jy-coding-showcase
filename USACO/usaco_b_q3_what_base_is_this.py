import math
import sys

sys.stdin = open("whatbase.in", "r")
sys.stdout = open("whatbase.out", "w")

def solve():
    K = int(input().strip())

    for _ in range(K):
        a, b = input().split()

        # digits of first number
        a1, a2, a3 = int(a[0]), int(a[1]), int(a[2])
        # digits of second number
        b1, b2, b3 = int(b[0]), int(b[1]), int(b[2])

        for X in range(10, 15001):
            # value of first number in base X
            N = a1 * X * X + a2 * X + a3

            # quadratic: b1*Y^2 + b2*Y + b3 = N
            A = b1
            B = b2
            C = b3 - N

            D = B * B - 4 * A * C

            if D < 0:
                continue

            sqrtD = int(math.sqrt(D))

            # check perfect square
            if sqrtD * sqrtD != D:
                continue

            # only positive root
            Y_num = -B + sqrtD
            Y_den = 2 * A

            if Y_num % Y_den != 0:
                continue

            Y = Y_num // Y_den

            if 10 <= Y <= 15000:
                print(X, Y)
                break

solve()