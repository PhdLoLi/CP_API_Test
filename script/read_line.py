#from pylab import *
import numpy as np
#import pylab as pl
import matplotlib.pyplot as pl
from matplotlib.lines import Line2D
import sys
import os

x = []
#x = [1, 25, 50, 100, 200, 400, 800]
#y_group = [i for i in range(6), j for j in range(8), []]
foo = 0
y_group = [[foo for i in range(5)] for j in range(7)]
buf_dic = {1: 0, 25: 1, 50: 2, 100: 3, 200: 4, 400: 5, 800: 6}
win_dic = {}
res_dic = {}
res_list = [[None] * 7 for tt in range(5)]

tmp = 1;
for i in range(5):
    win_dic[tmp] = i
    res_dic[tmp] = [] 
    tmp *= 2

buf_size = 1;
while buf_size <= 800 : 
    x.append(buf_size)
    if buf_size == 1 :
        buf_size = 25
    else:
        buf_size *= 2

print win_dic
tag = sys.argv[1]

f = open('../results/result_%s/res.csv'%tag, 'r')
for line in f:
    y = line.strip('\n').split(',')
    # [0]:buf_size [1]:win_size [2]:times [3]:duration
#    buf = buf_dic[int(y[0])]
    buf_size = buf_dic[int(y[0])]
    win_size = win_dic[int(y[1])]
    res_list[win_size][buf_size] = float(y[3]) / 1000
    res_dic[int(y[1])].append(float(y[3]) / 1000) 

for i in res_dic :
    res_dic[i].reverse()

markers = []
for m in Line2D.markers:
    try:
        if len(m) == 1 and m != ' ':
            markers.append(m)
    except TypeError:
        pass

linestyles = ['_', '-', '--', ':']

styles = markers + [
    r'$\lambda$',
    r'$\bowtie$',
    r'$\circlearrowleft$',
    r'$\clubsuit$',
    r'$\checkmark$']

print styles

#makerstyles = ['D', 's', '|', 'x', '_', '^', 'd', 'h', '+', '*', ',', 'o', '.', '1', 'p', '3', '2', '4', 'H', 'v', '8', '<', '>', '$\\lambda$', '$\\bowtie$', '$\\circlearrowleft$', '$\\clubsuit$', '$\\checkmark$']

makerstyles = ['D', 'd', '^', 'o', 's', 'h', 'p', ',', '.','+', '|', '*', '_', '1', 's']

colors = ('b', 'g', 'r', 'k', 'm', 'y', 'c')

type = 0
plots = [] 
win_sizes = []
size = 1
for i in range(5) :
    print i
    plot, = pl.plot(x, res_list[i], linestyle='--', marker=makerstyles[type], color=colors[type], markersize=7, linewidth=1.5)
    win_sizes.append('window size %d'%size)
    plots.append(plot)
    type += 1
    size *= 2

#pl.title('Space and Time Relationship')# give plot a title
pl.xlabel('Send buffer size (segments)')# make axis labels
pl.ylabel('Computation time (seconds)')

pl.legend(plots, win_sizes)
#pl.legend([plot1, plot2, plot3, plot4, plot5], ['window size 1', 'window size 2', 'window size 4', 'window size 8', 'window size 16'])

if os.path.isdir('../plots/plot_%s'%tag): 
    pass 
else: 
    os.mkdir('../plots/plot_%s'%tag)
pl.savefig('../plots/plot_%s/fig.pdf'%tag)
pl.show()
