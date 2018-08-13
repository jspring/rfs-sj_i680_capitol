#!/bin/bash

xterm -T 10.254.27.82  -geometry 46x24+0+0 -e telnet 10.254.27.82 &
xterm -T 10.254.27.81  -geometry 46x24+280+0  -e telnet 10.254.27.81 &
xterm -T 10.254.28.213 -geometry 46x24+560+0 -e telnet 10.254.28.213 &
xterm -T 10.254.28.212 -geometry 46x24+840+0 -e telnet 10.254.28.212  &
xterm -T 10.254.24.156 -geometry 46x24+0+250 -e telnet 10.254.24.156 &
xterm -T 10.254.24.157 -geometry 46x24+280+250 -e telnet 10.254.24.157 &
xterm -T 10.29.248.66 -geometry 46x24+560+250 -e telnet 10.29.248.66 231 &
xterm -T 10.29.248.81 -geometry 46x24+840+250 -e telnet 10.29.248.81 231 &
xterm -T 10.29.248.155:231 -geometry 46x24+0+500 -e telnet 10.29.248.155 231 &
xterm -T 10.29.248.155:232 -geometry 46x24+280+500 -e telnet 10.29.248.155 232 &
xterm -T 10.29.248.67 -geometry 46x24+560+500 -e telnet 10.29.248.67 231 &
