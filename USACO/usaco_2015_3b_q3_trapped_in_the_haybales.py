import sys

# 1. Setup File Input/Output
sys.stdin = open("trapped.in")
sys.stdout = open("trapped.out", "w")

n = int(input())
haybales = []

# 2. Read input and convert to integers immediately
for i in range(n):
    size, position = map(int, input().split())
    # Storing as (position, size) makes sorting by position automatic
    haybales.append((position, size))

# 3. CRUCIAL STEP: Sort by position along the road
haybales.sort()

total_trapped_area = 0

# 4. Check every single starting interval
for i in range(n - 1):
    # Calculate the physical distance of this specific interval
    interval_width = haybales[i + 1][0] - haybales[i][0]

    # Initialize our simulation pointers for this interval
    left = i
    right = i + 1
    is_trapped = True

    # 5. Run the simulation for Bessie trying to break out
    while left >= 0 and right < n:
        # Current total runway Bessie has accumulated
        current_dist = haybales[right][0] - haybales[left][0]

        broke_something = False

        # Can she smash the left boundary bale?
        if current_dist > haybales[left][1]:
            left -= 1  # Bale is "removed", expand her boundary left
            broke_something = True

        # Can she smash the right boundary bale?
        if current_dist > haybales[right][1]:
            right += 1  # Bale is "removed", expand her boundary right
            broke_something = True

        # Optimization: If she can't break either side, she is stuck forever
        if not broke_something:
            break

    # 6. Check the final result of the simulation
    # If the pointers never went out of bounds, she hit a wall she couldn't break.
    if left >= 0 and right < n:
        total_trapped_area += interval_width

# 7. Output the final trapped area
print(total_trapped_area)