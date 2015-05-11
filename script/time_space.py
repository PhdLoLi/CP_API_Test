#!/usr/bin/python
# -*- coding:utf8 -*-
# run 6 times send buffer 1 50 100 200 400 8000
# author:Lijing Wang

import os
import random
import time
buf_size = 800 
for i in range(1, 7):
    f = open('pylog', 'w+')
    f.write('i\n')
    f.write(str(i))
    f.write('\nbuf_size\n')
    f.write(str(buf_size))
    time.sleep(i * 60)
    ran = random.randint(1, 1000000) 
    os.system('nohup /Users/lijing/CP_API_Test/build/time_space_pro %s %s > /Users/lijing/CP_API_Test/result/pro%s &'%(buf_size, ran, buf_size))
    os.system('nohup /Users/lijing/CP_API_Test/build/time_space_con %s %s > /Users/lijing/CP_API_Test/result/con%s &'%(buf_size, ran, buf_size))
    if (buf_size == 50):
        buf_size = 1
    else:
        buf_size /= 2

