#!/bin/bash

#osascript emulates user-journey and compiles to test mode which writes commands in user_test.txt to test
osascript <<EOF
tell application "System Events"
  keystroke "cd /Users/philipprados/documents/coding/c/pshell"
  delay 1
  keystroke return
  delay 1
  keystroke "make compile_and_test"
  delay 1
  keystroke return
  delay 0.5

  --type "ls" and return
  keystroke "ls"
  delay 0.5
  keystroke return
  delay 0.5

  --press arrow up and add " -a"
  key code 126 
  delay 0.5
  keystroke " -a"
  delay 0.5
  keystroke return
  delay 0.5

  --type uwe and delete last char
  keystroke "uwe"
  delay 0.5
  key code 51
  delay 0.5
  keystroke return
  delay 0.5

  --type in command, press up arrow and add more letters
  keystroke "simone"
  delay 1
  key code 126
  delay 1
  keystroke "s"
  delay 1
  keystroke return

end tell
EOF
