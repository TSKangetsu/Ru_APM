#! /usr/bin/expect
spawn zip -r SRC.zip SRC/
# interact
spawn zip SRC.zip CMakeLists.txt
# interact

spawn scp /mnt/d/WSL-Project/project/Ru_APM/SRC.zip pi@192.168.88.123:/mnt/d/WSL-Project/project/Ru_APM
expect "*password*"
send "4030\r"
interact

spawn rm SRC.zip
# interact

spawn ssh pi@192.168.88.123 "cd /mnt/d/WSL-Project/project/Ru_APM && unzip SRC.zip && mkdir build"
expect "*password*"
send "4030\r"
expect "replace*"
send "A\r"
# interact

spawn ssh pi@192.168.88.123 "cd /mnt/d/WSL-Project/project/Ru_APM/build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make"
expect "*password*"
send "4030\r"
interact

spawn ssh pi@192.168.88.123 "cd /mnt/d/WSL-Project/project/Ru_APM/build && expect shell.sh"
expect "*password*"
send "4030\r"
# interact

spawn unzip build/build.zip -d build/
expect "replace*"
send "A\r"
# interact

spawn ssh pi@192.168.88.123 "cd /mnt/d/WSL-Project/project/Ru_APM/build && gdbserver :10086 ./RuAPM_Cli -m"
expect "*password*"
send "4030\r"
interact