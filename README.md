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
![](https://user-images.githubusercontent.com/60818062/171273408-cd3f2384-a7ce-4eeb-b359-85b86843b41f.mov)|![](https://user-images.githubusercontent.com/60818062/171273532-5fb9e2a5-6072-45bb-a283-d3584741d28b.mov)|![](https://user-images.githubusercontent.com/60818062/171273620-4ce829ff-f2c4-4b7e-a027-791c0bd648b4.mov)|
Wildcard-matching | Syntax highlighting
![](https://user-images.githubusercontent.com/60818062/171273726-1774d03a-ddbc-4c37-9c18-901f33138fe5.png)|![](https://user-images.githubusercontent.com/60818062/171273732-f4f32544-f140-4f3b-9a96-a0fcc8df77fb.png)|
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
