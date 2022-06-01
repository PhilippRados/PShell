<div>
  <p align="center">
    <img src="pictures/logo_transparent.png" width="800">
  </p>
</div>

A C-shell with integrated fuzzy finder, tab completion and more without curses
---
![](https://img.shields.io/badge/made_for-UNIX-lightgrey)
![](https://img.shields.io/github/languages/code-size/PhilippRados/PShell)
![](https://img.shields.io/github/license/PhilippRados/PShell)
![](https://img.shields.io/badge/Dependencies-Gcc,_Make-brightgreen)
<p>
<img alt="ViewCount" src="https://views.whatilearened.today/views/github/PhilippRados/PShell.svg">
</p>

### Table of contents
* [Features](#features)
  + [Syntax](#syntax)
* [Setup](#setup)
* [Testing](#testing)
  + [Unit-tests](#unit)
  + [Integration-tests](#integration)
* [Future Work](#future-work)

## Features
Tab-completion             | Fuzzy-finder              | Autocompletion
:-------------------------:|:-------------------------:|:-------------------------:|
![](https://user-images.githubusercontent.com/60818062/171273408-cd3f2384-a7ce-4eeb-b359-85b86843b41f.mov)|![](https://user-images.githubusercontent.com/60818062/171275441-bfb27356-eb8e-40d2-8142-5f20204e9ea1.mov)|![](https://user-images.githubusercontent.com/60818062/171273620-4ce829ff-f2c4-4b7e-a027-791c0bd648b4.mov)|
Wildcard-matching | Syntax highlighting
![](https://user-images.githubusercontent.com/60818062/171273726-1774d03a-ddbc-4c37-9c18-901f33138fe5.png)|![](https://user-images.githubusercontent.com/60818062/171273732-f4f32544-f140-4f3b-9a96-a0fcc8df77fb.png)|
### Syntax
Besides the basic `&&` to chain multiple commands, `|` to pipe one output to another, `* and ?` for wildcard matching here are some more `psh` syntax elements:
##### Input/Output Redirection
```bash
> or 1>   stdout redirection
>> or 1>> output append
2>        stderr redirection
2>>       stderr append
<         input redirection
&>        merge stdout, stderr into single file
&>>       append merge stdout, stderr to single file

```
##### Fuzzy-finder
```bash
<ctrl-f> to open
<ESC> to close
<Enter> to choose proposal
```
##### Quoted arguments
```bash
echo 'this argument counts as single arg' # currently only single quotes supported
echo you\ can\ also\ escape\ whitespace\ with\ backslash
```

## Setup
You only need **make** and **gcc** to build. Also your terminal should be capable of displaying unicode characters.
```bash
$ git clone https://github.com/PhilippRados/PShell
$ cd PShell
$ make
$ echo /usr/local/bin/psh | sudo tee -a /etc/shells
```
Maybe you have to use `sudo make` because on some platforms `/usr/local/bin` is restricted.<br>
When successfully executed the commands above you can change `psh` to your default shell like so:
```bash
$ chsh -s /usr/local/bin/psh
```
You can use this same command to change your shell back to your original one just swap out the path.
### .pshrc
When first starting the shell you'll be prompted with an option to create a `~/.pshrc` file when you don't have one. This will set basic ENV-variables needed to run the shell.
```bash
# ~/.pshrc
PATH="some/interesting/path/$" # use $ to append to already extistant env-variable
TERM="linux"                   # without $ any existant env-variable gets overwritten
```
## Testing
1. <a name="unit">Unit_tests</a>: This requires the https://criterion.readthedocs.io/en/master/ testing lib for C.
   When you have installed that you can run:
   ```bash
   $ make run_tests              # to run all tests
   $ make <file_name without .c> # to run specific tests
   ```
2. <a name="integration">Integration_tests</a>:
   ```bash
   $ docker build -t testing_container . # have to setup docker container for tests to run in
   $ make integration_tests              # starts running integration tests in testing_container
   ```
## Future Work
- [ ] Implement local session ENV-variables using `export` and `echo $SOME`
- [ ] Have builtin command to search through past commands' output
- [ ] Allow for aliasing in ~/.pshrc
- [ ] Allow for inplace variable execution
- [ ] Improve fuzzy-finding algorithm
