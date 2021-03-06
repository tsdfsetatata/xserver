#!/usr/bin/python
# coding: UTF-8

import sys
from socket import *
import struct
import move_pb2
import cast_skill_pb2
import move_direct_pb2
import team_pb2
import datetime
import get_one_msg

#WATCH_PLAYER = {8589935316}
#WATCH_PLAYER = {0x2000002e0}
WATCH_PLAYER = {8589935329}


HOST='127.0.0.1'
PORT=11697
PORT=get_one_msg.get_dumpsrv_port()
ADDR=(HOST, PORT)
client=socket(AF_INET, SOCK_STREAM)
client.connect(ADDR)

last_data = ""
player_list = {}

def get_buff_data(t1):
    retdata = ""
    for buffinfo in t1.buff_info:
        tmp = "(%d) " % buffinfo.id
        retdata = retdata + tmp
    return retdata

while True:
    ret, last_data, player_id, msg_id, pb_data = get_one_msg.get_one_msg(client, last_data)
    if ret == -1:
        break
    if ret == 0:
        continue
#    data_len = data_len - 8 - 16
#    msg_format = "=IHH" + str(data_len) + 'sQIHH' 
#    msg_len, msg_id, seq, pb_data, player_id, t1, t1, t1 = struct.unpack(msg_format, data)

#    print "read msg:", msg_id	

    if not player_id in WATCH_PLAYER:
        continue;

#视野变化    
    if msg_id == 10103:
        req = move_pb2.sight_changed_notify()
        req.ParseFromString(pb_data)
        oldtime=datetime.datetime.now()        
        for t1 in req.add_player:
            buffdata = get_buff_data(t1)
            print oldtime.time(), ": %lu add player %lx buf[%s]" % (player_id, t1.playerid, buffdata)

        for t1 in req.delete_player:
            print oldtime.time(), ": %lu del player %lx" % (player_id, t1)
    
