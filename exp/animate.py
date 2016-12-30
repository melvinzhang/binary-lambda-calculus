import networkx as nx
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import random
import sys

# Graph initialization
G = nx.Graph()

fig = plt.gcf()

def hierarchy_width(G, root, parent = None, width = None):
    neighbors = G.neighbors(root)
    if parent != None:
        neighbors.remove(parent)
    total = 0.
    for neighbor in neighbors:
        total += hierarchy_width(G, neighbor, parent = root, width = width)
    if total < 1:
        total = 1.
    width[root] = total
    return width[root]


def hierarchy_pos(G, root, width=1., vert_gap = 0.2, vert_loc = 0, xcenter = 0.5, pos = None, parent = None, twidth = None):
    '''If there is a cycle that is reachable from root, then this will see infinite recursion.
       G: the graph
       root: the root node of current branch
       width: horizontal space allocated for this branch - avoids overlap with other branches
       vert_gap: gap between levels of hierarchy
       vert_loc: vertical location of root
       xcenter: horizontal location of root
       pos: a dict saying where all nodes go if they have been assigned
       parent: parent of this branch.'''
    if pos == None:
        pos = {root:(xcenter,vert_loc)}
    else:
        pos[root] = (xcenter, vert_loc)
    neighbors = G.neighbors(root)
    if parent != None:
        neighbors.remove(parent)
    if len(neighbors)!=0:
        nextx = xcenter - width/2
        for neighbor in neighbors:
            dx = (width * twidth[neighbor])/twidth[root]
            nextx += dx/2
            pos = hierarchy_pos(G,neighbor, width = dx, vert_gap = vert_gap,
                                vert_loc = vert_loc-vert_gap, xcenter=nextx, pos=pos,
                                parent = root, twidth = twidth)
            nextx += dx/2
    return pos

def hierarchy_layout(G, root):
    twidth = {}
    hierarchy_width(G, root, parent = None, width = twidth)
    return hierarchy_pos(G, root, twidth = twidth)

# Animation funciton
def animate(i):
    line = sys.stdin.readline()
    while not line == "" and not line.startswith("EA"):
        if line.startswith("P"):
            print line.split()[1]
        line = sys.stdin.readline()
    if line == "":
        sys.exit()
    tokens = line.split()
    n1 = tokens[1]
    n2 = tokens[2]
    fig.clf()
    plt.text(0.5, 0.1, G.number_of_nodes())
    if n1 == "free":
        G.remove_node(n2)
    else:
        G.add_edge(n1,n2)
    nx.draw(G, pos=hierarchy_layout(G,'n0'), node_size=100)

# Animator call
anim = animation.FuncAnimation(fig, animate, frames=None, interval=10, blit=False, repeat=False)
plt.show()
