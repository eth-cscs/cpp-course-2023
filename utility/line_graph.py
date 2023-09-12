from matplotlib import pyplot as plt
from typing import Optional
import pathlib


# You data here
#   Never commit these changes!
xlabel: str = "Block size (*64 bytes)"
ylabel: str = "Bandwidth (GB/s)"
title: Optional[str] = None
xdata = [1, 2]
ydata = [1, 2]


# Display graph
plt.figure()
xticks = list(range(0, len(ydata)))
plt.plot(xticks, ydata, "ro-")
plt.xticks(ticks=xticks, labels=xdata, rotation=35)
plt.xlabel(xlabel)
plt.ylabel(ylabel)
if title is not None:
    plt.title(title)
plt.tight_layout()
path = pathlib.Path(__file__).parent / "line_graph.svg"
plt.savefig(path)