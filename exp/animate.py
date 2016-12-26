import networkx as nx
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import random
import sys

# Graph initialization
G = nx.Graph()

fig = plt.gcf()

def hierarchy_pos(G, root, width=1., vert_gap = 0.2, vert_loc = 0, xcenter = 0.5, pos = None, parent = None):
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
        dx = width/len(neighbors) 
        nextx = xcenter - width/2 - dx/2
        for neighbor in neighbors:
            nextx += dx
            pos = hierarchy_pos(G,neighbor, width = dx, vert_gap = vert_gap, 
                                vert_loc = vert_loc-vert_gap, xcenter=nextx, pos=pos, 
                                parent = root)
    return pos

# Animation funciton
def animate(i):
    fig.clf()
    G.add_edge(*edges[i])
    nx.draw(G, pos=hierarchy_pos(G,'n0'))
    print(i)

edges = []
for line in sys.stdin:
    tokens = line.split()
    edges.append((tokens[0], tokens[2]))

# Animator call
anim = animation.FuncAnimation(fig, animate, range(len(edges)), interval=50, blit=False, repeat=False)
plt.show()
