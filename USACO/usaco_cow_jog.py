import sys

# Standard USACO file setup
sys.stdin = open("cowjog.in", "r")
sys.stdout = open("cowjog.out", "w")


def solve():
    # Read N and Time
    try:
        line = sys.stdin.readline().split()
        if not line: return
        n, minutes = map(int, line)
    except EOFError:
        return

    end_positions = []
    for _ in range(n):
        pos, speed = map(int, sys.stdin.readline().split())
        # Calculate where the cow WOULD be if no one blocked it
        end_positions.append(pos + (speed * minutes))

    # We start with 1 group (the lead cow)
    groups = 1

    # The 'bottleneck' is the cow furthest ahead
    # In the input, the last cow is the one furthest along the road
    slowest_end_pos = end_positions[-1]

    # Iterate from the second-to-last cow back to the first
    for i in range(n - 2, -1, -1):
        if end_positions[i] < slowest_end_pos:
            # This cow is slower than the group ahead of it.
            # It cannot catch up, so it starts a NEW group.
            groups += 1
            # Now this cow is the new bottleneck for everyone behind it
            slowest_end_pos = end_positions[i]
        else:
            # This cow's end position is >= the lead cow.
            # It will "catch up" and join the group ahead.
            pass

    print(groups)


solve()