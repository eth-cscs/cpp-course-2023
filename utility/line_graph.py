from matplotlib import pyplot as plt
from typing import Optional
import pathlib


# Your data here
#   Never commit these changes!
xlabel: str = "..."
ylabel: str = "..."
title: Optional[str] = None
xdata = [1, 2]
ydata = [1, 2]
xscale: Optional[str] = None # linear, log
yscale: Optional[str] = None # linear, log


# Display graph
plt.figure()

if not xscale:
    xticks = list(range(0, len(ydata)))
    plt.plot(xticks, ydata, "ro-")
    plt.xticks(ticks=xticks, labels=xdata, rotation=35)
else:
    plt.plot(xdata, ydata, "ro-")
    plt.xscale(xscale)
plt.yscale(yscale or "linear")

plt.xlabel(xlabel)
plt.ylabel(ylabel)
if title is not None:
    plt.title(title)
plt.tight_layout()
path = pathlib.Path(__file__).parent / "line_graph.svg"
plt.savefig(path)