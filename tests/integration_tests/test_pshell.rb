require 'ttytest'

@tty = TTYtest.new_terminal(%( PS1='$ '  ./src/bin/pshell), width: 40, height: 24)

BACKSPACE = 127.chr
TAB = 9.chr
CTRLF = 6.chr

puts 'STANDARD TEST'

# Basic IO
sleep 0.2
@tty.send_keys(%(tes))
@tty.assert_cursor_position(13, 1)
@tty.send_keys(%(\n))

@tty.assert_row(1, '/pshell ❱ tes')
@tty.assert_row(2, "couldn't find command tes")

puts "    \u2705 Basic I/O".encode('utf-8')

puts 'ARROW-KEYS AND COMMAND-HISTORY'

# Arrow upwards moves through history
sleep 0.2
@tty.send_keys(%(ls))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZA))
@tty.assert_cursor_position(13, 4)

@tty.assert_row(4, '/pshell ❱ tes')

puts "    \u2705 Arrow up moves up in command history".encode('utf-8')

# Arrow downwards moves back down
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZB))
@tty.assert_cursor_position(10, 4)

@tty.assert_row(4, '/pshell ❱ ')

puts "    \u2705 Arrow downwards moves back to empty line".encode('utf-8')

# Arrow left moves through current line
sleep 0.2
@tty.send_keys(%(test))
@tty.assert_cursor_position(14, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(%(l))
@tty.assert_cursor_position(13, 4)

@tty.assert_row(4, '/pshell ❱ telst')

puts "    \u2705 Arrow left moves backwards through line".encode('utf-8')

# Arrow right moves cursor on pos right
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))
@tty.assert_cursor_position(14, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))

puts "    \u2705 Arrow right moves one pos right".encode('utf-8')

# Backspace removes char and moves cursor
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(BACKSPACE)
@tty.assert_cursor_position(11, 4)
@tty.send_keys(%(\n))

@tty.assert_row(4, '/pshell ❱ tlst')

puts "    \u2705 Backspace removes char and moves cursor".encode('utf-8')

puts 'AUTOCOMPLETE'
# Shows matching autocomplete
sleep 0.2
@tty.send_keys(%(so))
@tty.assert_cursor_position(12, 7)

@tty.assert_row(7, '/pshell ❱ some autocomplete')

puts "    \u2705 Shows matching autocomplete".encode('utf-8')

# Arrow right puts cursor end of line and copies autocomplete
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))

@tty.assert_cursor_position(27, 7)
@tty.assert_row(7, '/pshell ❱ some autocomplete')

@tty.send_keys(%(\n))

puts "    \u2705 Arrow right moves cursor end of line".encode('utf-8')

puts 'BUILTINS'
# builtin-cd changes dir
sleep 0.2
@tty.send_keys(%(cd ..))
@tty.send_keys(%(\n))

@tty.assert_cursor_position(4, 12)
@tty.assert_row(12, '/ ❱')

puts "    \u2705 Builtin-cd changes dir".encode('utf-8')

puts 'MULTI-LINE'
# Cursor jumps down when line longer than term-width
sleep 0.2
@tty.send_keys(%(lllllllllllllllllllllllllllllllllllllllllllllll))

@tty.assert_cursor_position(11, 13)
@tty.assert_row(12, '/ ❱ llllllllllllllllllllllllllllllllllll')
@tty.assert_row(13, 'lllllllllll')

puts "    \u2705 Cursor jumps down one row if line too long".encode('utf-8')

# Output gets shown below line even when cursor on first row
(0..19).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZD))
end
@tty.send_keys(%(\n))
@tty.assert_row(12, '/ ❱ llllllllllllllllllllllllllllllllllll')
@tty.assert_row(13, 'lllllllllll')
@tty.assert_row(14, "couldn't find command llllllllllllllllll")
@tty.assert_row(15, 'lllllllllllllllllllllllllllll')

puts "    \u2705 Output gets shown below line even when cursor on first row".encode('utf-8')

# When on last line and line too long it shifts term up accordingly
sleep 0.2
@tty.send_keys(%(cd pshell))
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(make)) # arbitrary command to move to last line
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(kajldasjlsdjalsjdaljdjldasjlas))

@tty.assert_cursor_position(0, 23)
@tty.assert_row(22, '/pshell ❱ kajldasjlsdjalsjdaljdjldasjlas')

# @tty.assert_row(23, 'ldslks')

puts "    \u2705 When on last line and line too long it shifts term up accordingly".encode('utf-8')

# When on last line and autocomplete longer than term should shift up but cursor stay
sleep 0.2
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(uu))

@tty.assert_cursor_position(12, 22)
@tty.assert_row(22, '/pshell ❱ uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu')
@tty.assert_row(23, 'uuuuuuuuuuuuuuuu')

@tty.send_keys(%(u))
@tty.assert_cursor_position(13, 22)
@tty.assert_row(22, '/pshell ❱ uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu')
@tty.assert_row(23, 'uuuuuuuuuuuuuuuu')

puts "    \u2705 When on last line and autocomplete longer than term should shift up but cursor stay".encode('utf-8')

# When not on last line and autocomplete longer than term should not shift up
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(clear))
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(uu))

@tty.assert_cursor_position(12, 1)
@tty.assert_row(1, '/pshell ❱ uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu')
@tty.assert_row(2, 'uuuuuuuuuuuuuuuu')

@tty.send_keys(%(u))
@tty.assert_cursor_position(13, 1)
@tty.assert_row(1, '/pshell ❱ uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu')
@tty.assert_row(2, 'uuuuuuuuuuuuuuuu')
@tty.send_keys(%(\n))

puts "    \u2705 When not on last line and autocomplete longer than term should not shift up".encode('utf-8')

puts 'TAB-COMPLETION'
# Multiple Tab-completion matches
sleep 0.2
@tty.send_keys(%(ls))
@tty.send_keys(TAB)
@tty.assert_cursor_position(12, 4)

@tty.assert_row(4, '/pshell ❱ ls')
@tty.assert_row(5, 'lsattr    lsns      lsmem     lslogins')
@tty.assert_row(6, 'lsipc     lslocks   lscpu     lsblk')
@tty.assert_row(7, 'ls')

puts "    \u2705 Multiple Tab-completion matches".encode('utf-8')

# Pressing Tab cycles through all matches
sleep 0.2
@tty.send_keys(TAB)
@tty.send_keys(TAB)
@tty.assert_cursor_position(12, 4)

@tty.assert_row(4, '/pshell ❱ ls')
@tty.assert_row(5, 'lsattr    lsns      lsmem     lslogins')
@tty.assert_row(6, 'lsipc     lslocks   lscpu     lsblk')
@tty.assert_row(7, 'ls')

@tty.send_keys(%(\n))
@tty.assert_row(4, '/pshell ❱ lsmem')
@tty.assert_cursor_position(15, 4)

puts "    \u2705 Pressing Tab cycles through tab-matches".encode('utf-8')

# When only one match copies to line immediately
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(a))
@tty.send_keys(TAB)
@tty.assert_cursor_position(16, 4)

@tty.assert_row(4, '/pshell ❱ lsattr')

puts "    \u2705 When only one tab-comp matches immediately".encode('utf-8')

# When pressing Tab in middle of word looks for matches until cursor-pos
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(TAB)
@tty.assert_cursor_position(12, 4)

@tty.assert_row(4, '/pshell ❱ lsattr')
@tty.assert_row(5, 'lsattr    lsns      lsmem     lslogins')
@tty.assert_row(6, 'lsipc     lslocks   lscpu     lsblk')
@tty.assert_row(7, 'ls')

puts "    \u2705 When pressing Tab in middle of word looks for matches until cursor-pos".encode('utf-8')

# When exiting tab-prompt without selecting comp cursor should be at same pos as before
sleep 0.2
@tty.send_keys(%(a))
@tty.assert_cursor_position(12, 4)
puts "    \u2705 When exiting tab-prompt without selecting comp cursor should be at same pos as before".encode('utf-8')

# When Tab-completing on last row should shift up and cursor too
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(make))
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(make))
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(lsl))
@tty.send_keys(TAB)

@tty.assert_cursor_position(13, 22)
@tty.assert_row(22, '/pshell ❱ lsl')
@tty.assert_row(23, 'lslogins  lslocks')

@tty.send_keys(TAB)
@tty.assert_row(22, '/pshell ❱ lsl')
@tty.assert_row(23, 'lslogins  lslocks')

puts "    \u2705 When Tab-comp on last row should shift term up and cursor too".encode('utf-8')

puts 'FUZZY-FINDER'
# When pressing Fuzzy-finder on last row should shift prior commands up
sleep 0.2
@tty.send_keys(%(a))
@tty.send_keys(CTRLF)

@tty.assert_row(16, '/pshell ❱ lsl')
@tty.assert_row(17, ' ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━')

@tty.assert_cursor_position(3, 19)
@tty.send_keys(%(te))
@tty.assert_row(19, ' ❱ te                          1/2')
@tty.assert_row(20, '   tlst')
@tty.assert_row(21, '   tes')

puts "    \u2705 When pressing Fuzzy-finder on last row should shift prior commands up".encode('utf-8')

# Fuzzy-finder picked command replaces current line
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZB))

@tty.assert_row(19, ' ❱ te                          2/2')
@tty.send_keys(%(\n))
@tty.assert_row(16, '/pshell ❱ tes')
@tty.assert_cursor_position(13, 16)

puts "    \u2705 Fuzzy-finder picked command replaces current line".encode('utf-8')

# When line longer than term fuzzy-finder should still shift bottom of line up
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(ll))
@tty.send_keys(CTRLF)

@tty.assert_row(15, '/pshell ❱ llllllllllllllllllllllllllllll')
@tty.assert_row(16, 'lllllllllllllllll')
@tty.assert_row(17, ' ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━')
@tty.assert_row(19, ' ❱')

puts "    \u2705 When line longer than term fuzzy-finder should still shift bottom of line up".encode('utf-8')

# When exiting fuzzy-finder should be on upshifted row
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(\033))

@tty.assert_row(15, '/pshell ❱ llllllllllllllllllllllllllllll')
@tty.assert_row(16, 'lllllllllllllllll')
@tty.assert_row(17, '')
@tty.assert_cursor_position(12, 15)

puts "    \u2705 When exiting fuzzy-finder should be on upshifted row".encode('utf-8')

puts 'FILE-COMPLETION'
# File-completes shows all matching files/dirs
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(ls ../))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../')
@tty.assert_row(16, 'bin/     sys/     dev/     lib/')
@tty.assert_row(17, 'home/    run/     lib64/   media/')
@tty.assert_row(18, 'srv/     boot/    proc/    opt/')
@tty.assert_row(19, 'var/     usr/     mnt/     tmp/')
@tty.assert_row(20, 'etc/     sbin/    root/    pshell/')
@tty.assert_cursor_position(16, 15)

puts "    \u2705 File-completes shows all matching files/dirs".encode('utf-8')

# When only one matching file autocompletes immediately
sleep 0.2
@tty.send_keys(%(a)) # random key to exit out of tab-matches
@tty.send_keys(%(ro))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/')
@tty.assert_cursor_position(21, 15)

puts "    \u2705 When only one matching file autocompletes immediately".encode('utf-8')

# Doesnt show dotfiles in file-completion
sleep 0.2
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/')
@tty.assert_row(16, '')
@tty.assert_cursor_position(21, 15)

puts "    \u2705 Doesnt show dotfiles in completion".encode('utf-8')

# Shows dotfiles in file-completion if first char is dot
sleep 0.2
@tty.send_keys(%(.))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/.')
@tty.assert_row(16, '.bashrc       .profile')
@tty.assert_row(17, '../           ./')
@tty.assert_row(18, '.psh_history  .gem/')
@tty.assert_cursor_position(22, 15)

puts "    \u2705 Shows dotfiles if first char is dot".encode('utf-8')

# When File in file-completion bigger than term shortens file-name
sleep 0.2
@tty.send_keys(%(  /home/))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/. /home/')
@tty.assert_row(16, 'short_file')
@tty.assert_row(17, 'very_long_filename_to_test_shell_be...')
@tty.assert_cursor_position(29, 15)

puts "    \u2705 When File in file-completion bigger than term shortens file-name".encode('utf-8')

# When selecting shortened completion copies not-shortened version
sleep 0.2
@tty.send_keys(TAB)
@tty.send_keys(%(\n))
@tty.assert_row(15, '/pshell ❱ ls ../root/. /home/very_long_f')
@tty.assert_row(16, 'ilename_to_test_shell_behavior')
@tty.assert_cursor_position(30, 16)

puts "    \u2705 When selecting shortened completion copies not-shortened version".encode('utf-8')

puts 'TAB-COMPLETION MULTI-LINES'

# If tab-completing on second row completes below cursor
sleep 0.2
(0..47).each do |_i|
  @tty.send_keys(BACKSPACE)
end
sleep 0.2
@tty.send_keys(TAB)
@tty.send_keys(%(\n))
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(ls                                             ../m))
@tty.send_keys(TAB)

@tty.assert_row(18, '/pshell ❱ ls')
@tty.assert_row(19, '                 ../m')
@tty.assert_row(20, 'media/  mnt/')
@tty.assert_cursor_position(21, 19)

puts "    \u2705 If tab-completing on second row completes below cursor".encode('utf-8')

# When pressing enter on tab-comp on second row appends to cursor
sleep 0.2
@tty.send_keys(%(\n))
@tty.assert_row(18, '/pshell ❱ ls')
@tty.assert_row(19, '                 ../media/')
@tty.assert_cursor_position(26, 19)

puts "    \u2705 When pressing enter on tab-comp on second row appends to cursor".encode('utf-8')

# When on last line and shifts up when tab-completing on second row
sleep 0.2
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(ls                                             ../))

@tty.assert_row(22, '/pshell ❱ ls')
@tty.assert_row(23, '                 ../media/')
@tty.assert_cursor_position(20, 23)

@tty.send_keys(TAB)
@tty.assert_row(17, '/pshell ❱ ls')
@tty.assert_row(18, '                 ../media/')
@tty.assert_row(19, 'bin/     sys/     dev/     lib/')
@tty.assert_cursor_position(20, 18)

@tty.send_keys(%(\n))
@tty.assert_row(17, '/pshell ❱ ls')
@tty.assert_row(18, '                 ../bin/')
@tty.assert_cursor_position(24, 18)

puts "    \u2705 When on last line and shifts up when tab-completing on second row".encode('utf-8')

puts 'TAB-COMPLETION PROMPTS'
# When user tab-completes and too many matches gets prompted
sleep 0.2
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(m))
@tty.assert_row(23, '/pshell ❱ make')
@tty.assert_cursor_position(11, 23)

@tty.send_keys(TAB)
@tty.assert_row(20, '/pshell ❱ make')
@tty.assert_row(21, 'The list of possible matches is 27 lines')
@tty.assert_row(22, '. Do you want to print all of them? (y/n')
@tty.assert_row(23, ')')
@tty.assert_cursor_position(2, 23)

puts "    \u2705 When user tab-completes and too many matches gets prompted".encode('utf-8')

# When user declines tab-prompt cursor jumps back to original line (even when prompt multiple lines)
sleep 0.2
@tty.send_keys(%(n))
@tty.assert_row(20, '/pshell ❱ make')
@tty.assert_row(21, '')
@tty.assert_cursor_position(11, 20)

puts "    \u2705 When user declines tab-prompt cursor jumps back to original line (even when prompt multiple lines)".encode('utf-8')

# When user declines tab-prompt on shifted multi-line cursor jumps back to original line
sleep 0.2
@tty.send_keys(%(                             s))
@tty.assert_row(20, '/pshell ❱ m')
@tty.assert_row(21, 's')
(0..29).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZD))
end
@tty.send_keys(TAB)
@tty.send_keys(%(n))
@tty.assert_row(19, '/pshell ❱ m')
@tty.assert_row(20, 's')
@tty.assert_cursor_position(11, 19)

puts "    \u2705 When user declines tab-prompt on shifted multi-line cursor jumps back to original cursor".encode('utf-8')

# When user accepts tab-prompt matches get shown below current line
sleep 0.2
(0..29).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZC))
end
sleep 0.2
(0..35).each do |_i|
  @tty.send_keys(BACKSPACE)
end
sleep 0.2
@tty.send_keys(%(l))
@tty.send_keys(TAB)
@tty.send_keys(%(y))
@tty.assert_row(6, '/pshell ❱ ls')
@tty.assert_row(7, '                 ../bin/')
@tty.assert_row(8, 'ldattach       lsattr')
@tty.assert_cursor_position(11, 6)

puts "    \u2705 When user accepts tab-prompt matches get shown below current line".encode('utf-8')

# When user presses enter on prompt completion replaces current line
@tty.send_keys(%(\n))
@tty.assert_row(6, '/pshell ❱ ldattach')
@tty.assert_row(7, '')
@tty.assert_cursor_position(18, 6)
puts "    \u2705 When user presses enter on prompt completion replaces current line".encode('utf-8')

# Tab-prompt always below complete line even when multi-line
sleep 0.2
@tty.send_keys(%(                      s))
(0..29).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZD))
end
@tty.send_keys(TAB)
@tty.assert_row(6, '/pshell ❱ ldattach')
@tty.assert_row(7, 's')
@tty.assert_row(8, 'The list of possible matches is 16 lines')
@tty.assert_cursor_position(2, 10)

# When Tab-completion bigger than terminal-size prints all matches and exits Tab-comp
sleep 0.2
@tty.send_keys(%(nn))
@tty.send_keys(%(\n))
@tty.assert_row(6, '/pshell ❱ lndattach')
@tty.send_keys(%(s))
@tty.send_keys(TAB)
@tty.send_keys(%(y))
@tty.assert_row(23, '/pshell ❱ some autocomplete')
@tty.assert_cursor_position(11, 23)
@tty.send_keys(%(yy))
@tty.assert_row(22, '  ')
@tty.assert_row(23, '/pshell ❱ syy')

puts "    \u2705 When Tab-completion bigger than terminal-size prints all matches and exits Tab-comp".encode('utf-8')

# When Multi-line tab-completion bigger than terminal-size prints all matches and exits Tab-comp
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(m                                  src))
@tty.assert_row(22, '/pshell ❱ m')
@tty.assert_row(23, '     src')
(0..36).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZD))
end
@tty.send_keys(TAB)
@tty.assert_row(19, '/pshell ❱ m')
@tty.assert_row(20, '     src')
@tty.assert_row(21, 'The list of possible matches is 27 lines')

@tty.send_keys(%(y))
@tty.assert_row(22, '/pshell ❱ m')
@tty.assert_row(23, '     src')
@tty.assert_cursor_position(11, 22)
(0..36).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZC))
end
@tty.send_keys(%(yy))
@tty.assert_row(21, '  ')
@tty.assert_row(22, '/pshell ❱ m')
@tty.assert_row(23, '     srcyy')

puts "    \u2705 When Multi-line tab-completion bigger than terminal-size prints all matches and exits Tab-comp".encode('utf-8')

# Leaves one row above free when bigger than term-size
@tty.assert_row(20, 'mv')
@tty.assert_row(21, '')
puts "    \u2705 Leaves one row above free when bigger than term-size".encode('utf-8')

# When on second row prompts below line
@tty.send_keys(%( ../bin/))
@tty.send_keys(TAB)
@tty.assert_row(19, '/pshell ❱ m')
@tty.assert_row(20, '     srcyy ../bin/')
@tty.assert_row(21, 'The list of possible matches is 43 lines')
puts "    \u2705 When cursor on second row prompts below line".encode('utf-8')

@tty.send_keys(%(q\n))
