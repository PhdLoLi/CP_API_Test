#from pylab import *
import numpy as np
import pylab as pl
from matplotlib.lines import Line2D

x = []
#x = [1, 25, 50, 100, 200, 400, 800]
#y_group = [i for i in range(6), j for j in range(8), []]
foo = 0
y_group = [[foo for i in range(5)] for j in range(7)]
buf_dic = {1: 0, 25: 1, 50: 2, 100: 3, 200: 4, 400: 5, 800: 6}
win_dic = {}
res_dic = {}

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

f = open('../script/res.csv', 'r')
for line in f:
    y = line.strip('\n').split(',')
    # [0]:buf_size [1]:win_size [2]:times [3]:duration
#    buf = buf_dic[int(y[0])]
#    res_dic[int(y[1])][int(y[0])] = int(y[3])
    res_dic[int(y[1])].append(int(y[3]) / 1000) 

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

style = styles[0]
plot1, = pl.plot(x, res_dic[1], 'k--')# use pylab to plot x and y : Give your plots names
plot2, = pl.plot(x, res_dic[2], 'k:')
plot3, = pl.plot(x, res_dic[4], 'ko--')
plot4, = pl.plot(x, res_dic[8], 'k^:')
plot5, = pl.plot(x, res_dic[16], 'k-.')

#pl.title('Space and Time Relationship')# give plot a title
pl.xlabel('buffer size (segments)')# make axis labels
pl.ylabel('duration (seconds)')

pl.legend([plot1, plot2, plot3, plot4, plot5], ['window size 1', 'window size 2', 'window size 4', 'window size 8', 'window size 16'])

pl.savefig('../plot/fig_no_fast_signing.pdf')
#pl.show()
