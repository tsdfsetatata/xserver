#!/bin/sh
rm -rf config/server
rm -rf Proto
rm -rf Server
cp /home/jacktang/svnroot/XGame/Server . -a
cp /home/jacktang/svnroot/XGame/Proto . -a
cp /home/jacktang/svnroot/XGame/config/server  config/ -a