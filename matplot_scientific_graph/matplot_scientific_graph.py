import matplotlib.pyplot as plot
from datetime import datetime

now = datetime.now()
formatted_time = now.strftime("%Y-%m-%d %H:%M:%S")
with open("MatplotlibT", "a") as f:
    f.write("\nImported T | " + formatted_time)


def linegraph():
    x = []
    y = []

    # Ask for number of points once
    num_points = int(input("Number of points: "))

    for i in range(num_points):
        # Convert a and b to float, so they are treated as numbers
        b, a = input(f"Point {i+1} (X Y): ").split()
        x.append(float(a))
        y.append(float(b))

    uplabel = input("Y Axis Label: ")
    rightlabel = input("X Axis Label: ")
    name = input("Graph Title: ")

    # Matplotlib will now see these as numbers and scale the axes correctly
    plot.plot(x, y)
    plot.title(name)
    plot.xlabel(rightlabel)
    plot.ylabel(uplabel)

    plot.show()

linegraph()
