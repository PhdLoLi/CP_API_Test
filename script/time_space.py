#!/usr/bin/python
# -*- coding:utf8 -*-
# run 6 times send buffer 1 50 100 200 400 8000
# author:Lijing Wang

import os
import random
from time import gmtime, strftime
import time
buf_size = 100 
for i in range(1, 3):
    f = open('pylog', 'a')
    f.write('\n')
    f.write(strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime()));
    f.write('\nbuf_size\t')
    f.write(str(buf_size))
    ran = random.randint(1, 1000000) 
    os.system('nohup /Users/lijing/CP_API_Test/build/time_space_pro %s %s > /Users/lijing/CP_API_Test/result/pro%s &'%(buf_size, ran, buf_size))
    for j in range(1, 6):
        os.system('nohup /Users/lijing/CP_API_Test/build/ts_random_con %s %s > /Users/lijing/CP_API_Test/result/con%s_%s &'%(buf_size, ran, buf_size, j))

    if (buf_size == 50):
        buf_size = 1
    else:
        buf_size /= 2

    time.sleep(60)

