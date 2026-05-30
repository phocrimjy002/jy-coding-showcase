import sys

# Change these to your local file names or comment them out to use the console
sys.stdin = open("learning.in", "r")


sys.stdout = open("learning.out", "w")

def solve():
    line1 = sys.stdin.readline().split()
    if not line1: return
    n, a, b = map(int, line1)

    herd = []
    for _ in range(n):
        s, w = sys.stdin.readline().split()
        herd.append((int(w), s == 'S'))

    # 1. Sort the herd by weight
    herd.sort()

    # 2. Add 'Sentinels' at the far ends (outside range A to B)
    # This ensures every part of [A, B] is covered by an interval
    herd.insert(0, (-10 ** 15, False))
    herd.append((10 ** 15, False))

    ans = 0

    # 3. Process the intervals between cows
    for i in range(len(herd) - 1):
        w1, s1 = herd[i]
        w2, s2 = herd[i + 1]
        mid = (w1 + w2) // 2

        # Interval A: Points closer to w1 (including midpoint if tied)
        if s1:
            # Range is [w1 + 1, mid]
            # (We count the actual cow w1 separately later or earlier)
            start = max(a, w1 + 1)
            end = min(b, mid)
            if start <= end:
                ans += (end - start + 1)

        # Interval B: Points closer to w2
        if s2:
            # Range is [mid + 1, w2]
            # If (w1+w2) is even, mid is equally close to both.
            # If s2 is spotted but s1 isn't, mid was NOT counted above.
            start_point = mid + 1
            if (w1 + w2) % 2 == 0 and not s1:
                start_point = mid

            start = max(a, start_point)
            end = min(b, w2 - 1)  # Stop before w2 to avoid double counting
            if start <= end:
                ans += (end - start + 1)

    # 4. Final step: Count the actual known cows if they are spotted
    for w, s in herd[1:-1]:
        if s and a <= w <= b:
            ans += 1

    print(ans)


solve()
