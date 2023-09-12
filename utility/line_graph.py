from matplotlib import pyplot as plt
from typing import Optional
import pathlib


# You data here
#   Never commit these changes!
xlabel: str = "..."
ylabel: str = "..."
title: Optional[str] = None
xdata = [
    1,
    2,
]
ydata = [
    3,
    4,
]


# Display graph
plt.figure()
plt.plot(xdata, ydata, "ro-")
plt.xlabel(xlabel)
plt.ylabel(ylabel)
if title is not None:
    plt.title(title)
path = pathlib.Path(__file__).parent / "line_graph.svg"
plt.savefig(path)