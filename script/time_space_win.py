#!/usr/bin/python
# -*- coding:utf8 -*-
# run 6 times send buffer 1 50 100 200 400 8000
# author:Lijing Wang

import os
import random
from datetime import datetime, date, time

tag = datetime.now().strftime("%m-%d-%H:%M")
if os.path.isdir('../results/result_%s'%tag): 
    pass 
else: 
    os.mkdir('../results/result_%s'%tag)
buf_size = 800 

while buf_size >= 1 :
    win_size = 16
    while win_size >= 1 : 
        ran = random.randint(1, 1000000) 

        os.system('nohup ../build/time_space_pro %s %s > ../results/result_%s/real%s_buf%s_win%s_pro &'%(buf_size, ran, tag, ran, buf_size, win_size))
        os.system('../build/ts_real_win_con %s %s > ../results/result_%s/real%s_buf%s_win%s_con'%(ran, win_size, tag, ran, buf_size, win_size))
        
        with open('../results/result_%s/real%s_buf%s_win%s_pro'%(tag, ran, buf_size, win_size), "rb") as f:
            first = f.readline()     # Read the first line.
            f.seek(-2, 2)            # Jump to the second last byte.
            while f.read(1) != "\n": # Until EOL is found...
                f.seek(-2, 1)        # ...jump back the read byte plus one more.
            last = f.readline() 
        f.close()

        res = open('../results/result_%s/res.csv'%tag, 'a+')
        res.write(str(buf_size))
        res.write(',')
        res.write(str(win_size))
        res.write(',')
        res.write(last)
        res.close()
        
        pylog = open('pylog', 'a+')
        dt = datetime.now()
        pylog.write(dt.strftime("%A, %d. %B %Y %I:%M%p"));
        pylog.write('\nbuf_size\t')
        pylog.write(str(buf_size))
        pylog.write('\nwin_size\t')
        pylog.write(str(win_size))
        pylog.write('\n')
        pylog.close()

        win_size /= 2 

    if (buf_size == 25):
        buf_size = 1
    else:
        buf_size /= 2

os.system('python read_line.py %s'%tag)
#       time.sleep(60)

