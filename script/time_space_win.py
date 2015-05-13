#!/usr/bin/python
# -*- coding:utf8 -*-
# run 6 times send buffer 1 50 100 200 400 8000
# author:Lijing Wang

import os
import random
from datetime import datetime, date, time
buf_size = 800 
while buf_size >= 1 :
    win_size = 16
    while win_size >= 1 : 
        ran = random.randint(1, 1000000) 
        os.system('nohup ../build/time_space_pro %s %s > ../result/real_buf%s_win%s_pro &'%(buf_size, ran, buf_size, win_size))
        os.system('../build/ts_real_win_con %s %s > ../result/real_buf%s_win%s_con'%(ran, win_size, buf_size, win_size))

        f = open('pylog', 'a')
        dt = datetime.now()
        f.write(dt.strftime("%A, %d. %B %Y %I:%M%p"));
        f.write('\nbuf_size\t')
        f.write(str(buf_size))
        f.write('\nwin_size\t')
        f.write(str(win_size))
        f.write('\n')

        win_size /= 2 

    if (buf_size == 25):
        buf_size = 1
    else:
        buf_size /= 2

#       time.sleep(60)

