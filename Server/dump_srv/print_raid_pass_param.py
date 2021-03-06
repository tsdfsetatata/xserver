#!/usr/bin/python
# coding: UTF-8

import sys
from socket import *
import struct
import raid_pb2
import cast_skill_pb2
import move_direct_pb2
import team_pb2
import horse_pb2
import datetime
import get_one_msg

WATCH_PLAYER = {4294968631}

sight_player = {}


HOST='127.0.0.1'
PORT=13697
PORT=get_one_msg.get_dumpsrv_port()
ADDR=(HOST, PORT)
client=socket(AF_INET, SOCK_STREAM)
client.connect(ADDR)

last_data = ""
player_list = {}

while True:
    ret, last_data, player_id, msg_id, pb_data = get_one_msg.get_one_msg(client, last_data)
    if ret == -1:
        break
    if ret == 0:
        continue

#MSG_ID_RAID_PASS_PARAM_CHANGED_NOTIFY
    if msg_id == 10816:
        req = raid_pb2.raid_pass_param_changed_notify()
        req.ParseFromString(pb_data)
        oldtime=datetime.datetime.now()        
        print  oldtime.time(), ": %lu pass_index[%u] pass_value[%u]" % (player_id, req.pass_index, req.pass_value)

