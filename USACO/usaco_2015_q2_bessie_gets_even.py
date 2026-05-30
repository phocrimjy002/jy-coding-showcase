with open("geteven.in", "r") as f:
    lines = f.readlines()

even = {}
odd = {}

for var in "BESIGOM":
    even[var] = 0
    odd[var] = 0

n = int(lines[0])
for i in range(1, n + 1):
    var, val = lines[i].split()
    val = int(val)

    if val % 2 == 0:
        even[var] += 1
    else:
        odd[var] += 1

total = 1
for var in "BESIGOM":
    total *= (even[var] + odd[var])

BI_odd = even["B"] * odd["I"] + odd["B"] * even["I"]

GOES_odd = 0
for g in [0, 1]:
    for o in [0, 1]:
        for e in [0, 1]:
            for s in [0, 1]:
                if (g + o + e + s) % 2 == 1:
                    ways = ((even["G"] if g == 0 else odd["G"]) * (even["O"] if o == 0 else odd["O"]) *
                            (even["E"] if e == 0 else odd["E"]) * (even["S"] if s == 0 else odd["S"]))

                    GOES_odd += ways

M_odd = odd["M"]

bad = BI_odd * GOES_odd * M_odd

with open("geteven.out", "w") as f:
    f.write(str(total - bad) + "\n")
