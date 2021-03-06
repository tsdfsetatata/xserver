#!/usr/bin/python
# coding: UTF-8

import sys
from socket import *
import struct
import move_pb2
import cast_skill_pb2
import move_direct_pb2
import datetime
import get_one_msg

HOST='127.0.0.1'
PORT=10697
BUFSIZ=1024
PORT=get_one_msg.get_dumpsrv_port()
ADDR=(HOST, PORT)
client=socket(AF_INET, SOCK_STREAM)
client.connect(ADDR)

last_data = ""
player_list = {}

while True:
    data=client.recv(BUFSIZ)
    data = last_data + data
    data_len = len(data)
    if data_len == 0:
        break
    if data_len < 8:
        last_data = data
        continue
    msg_head = data[0:8]
    cur_pos = 8    
    msg_len, msg_id, seq = struct.unpack('=IHH', msg_head)

    if msg_len > data_len:
        last_data = data
        continue

    pb_len = msg_len - 8 - 16
    pb_data = data[cur_pos:cur_pos+pb_len]
    cur_pos = cur_pos + pb_len
    extern_data = data[cur_pos:cur_pos+16]
    cur_pos = cur_pos + 16
    player_id,t1,t1,t1 = struct.unpack('=QIHH', extern_data)
    if cur_pos > data_len:
        print 'err, cur_pos[%d] > data_len[%d]' %(cur_pos, data_len)
        break
    if cur_pos == data_len:
        last_data = ''
    else:
        last_data = data[cur_pos:]
        
#    data_len = data_len - 8 - 16
#    msg_format = "=IHH" + str(data_len) + 'sQIHH' 
#    msg_len, msg_id, seq, pb_data, player_id, t1, t1, t1 = struct.unpack(msg_format, data)

#    if not player_id in WATCH_PLAYER:
#        continue;

#视野变化    
    if msg_id == 10103:
        req = move_pb2.sight_changed_notify()
        req.ParseFromString(pb_data)
        oldtime=datetime.datetime.now()        
        for t1 in req.add_collect:
            print oldtime.time(), ": %lu add collect[%u:%u] %.2f %.2f %.2f %.2f" % (player_id, t1.collectid, t1.uuid, t1.data.pos_x, t1.data.pos_z, t1.y, t1.yaw)
