# PShell
#### A C-shell with integrated fuzzy finder, tab completion and more using no external libs <sub><sup><sup>(aside from a testing-lib :))</sup></sub></sub>

[![asciicast](https://asciinema.org/a/464549.svg)](https://asciinema.org/a/464549)

## Installation
```
git clone https://github.com/PhilippRados/PShell
cd PShell
make
echo /usr/local/bin/psh | sudo tee -a /etc/shells
```
To change psh to default: `chsh -s /usr/local/bin/psh`
