#!/usr/bin/python
# -*- coding:utf8 -*-
# run 6 times send buffer 1 50 100 200 400 8000
# author:Lijing Wang

import os
import random
from time import gmtime, strftime
import time
buf_size = 100 
for i in range(1, 2):
    f = open('pylog', 'a')
    f.write('\n')
    f.write(strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime()));
    f.write('\nbuf_size\t')
    f.write(str(buf_size))
    ran = random.randint(1, 1000000) 
    os.system('nohup ../build/time_space_pro %s %s > ../result/pro%s &'%(buf_size, ran, buf_size))
    os.system('../build/ts_real_ran_con %s > ../result/real_con%s'%(ran, buf_size))

    if (buf_size == 50):
        buf_size = 1
    else:
        buf_size /= 2

#    time.sleep(60)

