<div>
  <p align="center">
    <img src="pictures/logo_transparent.png" width="800">
  </p>
</div>

A C-shell with integrated fuzzy finder, tab completion and more without curses

![](https://img.shields.io/badge/made_for-UNIX-lightgrey)
![](https://img.shields.io/github/languages/code-size/PhilippRados/PShell)
![](https://img.shields.io/github/license/PhilippRados/PShell)
![](https://img.shields.io/badge/Dependencies-Gcc,_Make-brightgreen)
<p>
<img alt="ViewCount" src="https://views.whatilearened.today/views/github/PhilippRados/PShell.svg">
</p>

### Table of contents
* [Features](#features)
* [Setup](#setup)
* [Testing](#testing)
* [Future-Work](#future-work)

## Features
Tab-completion             | Fuzzy-finder              | Autocompletion
:-------------------------:|:-------------------------:|:-------------------------:|
![](https://user-images.githubusercontent.com/60818062/171268726-980055e7-c8fc-43b4-9db6-6d9c37a83927.mov)|![](https://user-images.githubusercontent.com/60818062/171268827-a9d95bc5-e00b-4255-bf2a-da942fff38f9.mov)|![](https://user-images.githubusercontent.com/60818062/171268421-bf8e9eab-589e-4ef3-8959-a96836c57900.mov)|
Syntax Highlighting | Wildcard-matching
![](https://user-images.githubusercontent.com/60818062/171268937-d07bf069-ff0e-4b15-b903-d6a32a1bc373.mov)|![](https://user-images.githubusercontent.com/60818062/171269091-98c727f6-b27b-46b3-b16a-2bbd0b77d7b2.mov)|
## Setup
You only need **make** and **gcc** to build. Also your terminal should be capable of displaying unicode characters.
```
$ git clone https://github.com/PhilippRados/PShell
$ cd PShell
$ make
$ echo /usr/local/bin/psh | sudo tee -a /etc/shells
```
Maybe you have to use `sudo make` because on some platforms `/usr/local/bin` is restricted. When successfully executed the commands above you can change `psh` to your default shell like so:
```
chsh -s /usr/local/bin/psh
```
You can use this same command to change your shell back to your previous one.


## Testing

## Future-Work
