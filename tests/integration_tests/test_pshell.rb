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
@tty.assert_cursor_position(13, 4)
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
@tty.send_keys(BACKSPACE)
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
@tty.assert_row(19, ' ❱                             1/13')

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
@tty.send_keys(BACKSPACE) # random key to exit out of tab-matches
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
@tty.send_keys(%( /home/))
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
@tty.send_keys(%(make\n))
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
(0..40).each do |_i|
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
@tty.assert_row(21, 'The list of possible matches is 86 lines')
puts "    \u2705 When cursor on second row prompts below line".encode('utf-8')

puts 'COMMAND NOT STARTING INDEX 0 - TAB-COMP'
# When command not at beginning of line tab-comps anyway
sleep 0.2
@tty.send_keys(%(n))
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(cd ..))
@tty.send_keys(%(\n))
@tty.send_keys(%(clear))
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(    ls))
@tty.assert_cursor_position(10, 1)

@tty.send_keys(TAB)
@tty.assert_row(1, '/ ❱     ls')
@tty.assert_row(2, 'lsattr    lsns      lsmem     lslogins')
puts "    \u2705 When command not at beginning of line tab-comps anyway".encode('utf-8')

# When cursor on start of command doesnt autocomplete
sleep 0.2
@tty.send_keys(%(n))
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(8, 1)

@tty.send_keys(TAB)
@tty.assert_row(1, '/ ❱     ls')
@tty.assert_row(2, '')
puts "    \u2705 When cursor on start of command doesnt autocomplete".encode('utf-8')

# When Cursor before command doesnt autocomplete
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(7, 1)

@tty.send_keys(TAB)
@tty.assert_row(1, '/ ❱     ls')
@tty.assert_row(2, '')
puts "    \u2705 When Cursor before command doesnt autocomplete".encode('utf-8')

# When Cursor after command autocompletes for files
sleep 0.2
(0..3).each do |_i|
  @tty.send_keys(%(\033))
  @tty.send_keys(%(ZC))
end
@tty.send_keys(%( ))
@tty.assert_cursor_position(11, 1)

@tty.send_keys(TAB)
@tty.assert_row(1, '/ ❱     ls ')
@tty.assert_row(2, 'bin/     sys/     dev/     lib/')
puts "    \u2705 When Cursor after command autocompletes for files".encode('utf-8')

puts 'COMMAND NOT STARTING INDEX 0 - EXECUTION'
# Executes line even if command doesnt start at index 0
sleep 0.2
@tty.send_keys(TAB)
@tty.send_keys(TAB)
@tty.send_keys(%(\n))
@tty.assert_cursor_position(15, 1)
@tty.send_keys(%(\n))

@tty.assert_row(1, '/ ❱     ls dev/')
@tty.assert_row(2, 'console  mqueue  random  stdout')
@tty.assert_row(7, '/ ❱   ')

puts "    \u2705 Executes line even if command doesnt start at index 0".encode('utf-8')

puts 'REPLACING ALIASES'
# Replaces ~ with home-path
sleep 0.2
(0..7).each do |_i|
  @tty.send_keys(BACKSPACE)
end
@tty.send_keys(%(touch /root/some_file /root/another))
@tty.send_keys(%(\n))

sleep 0.2
@tty.send_keys(%(ls ~))
@tty.assert_cursor_position(8, 9)
@tty.send_keys(%(\n))
@tty.assert_row(9, '/ ❱ ls ~')
@tty.assert_row(10, 'another  some_file')

puts "    \u2705 Replaces ~ with home-path".encode('utf-8')

# Tab-completes ~ with home-path
sleep 0.2
@tty.send_keys(%(ls ~/))
@tty.send_keys(TAB)
@tty.assert_row(12, '/ ❱ ls ~/')
@tty.assert_row(13, 'some_file  another')

sleep 0.2
@tty.send_keys(%(\n))
@tty.assert_row(12, '/ ❱ ls ~/some_file')
@tty.assert_cursor_position(18, 12)
@tty.send_keys(%(\n))

puts "    \u2705 Tab-completes ~ with home-path".encode('utf-8')

puts 'TESTING WITH VERY BIG INPUTS'
# With very big command in command-history still shows up as autocomplete
sleep 0.2
@tty.send_keys(%(jjjjj))
@tty.assert_row(15, '/ ❱ jjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_row(16, 'kkkkkkkkjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_cursor_position(9, 15)

puts "    \u2705 With very big command in command-history still shows up as autocomplete".encode('utf-8')

# When pressing arrow right copies autocomplete in current line
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))
@tty.assert_row(15, '/ ❱ jjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_row(16, 'kkkkkkkkjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_cursor_position(22, 21)

puts "    \u2705 When pressing arrow right copies autocomplete in current line".encode('utf-8')

# Fuzzy-finder still works
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(CTRLF)
@tty.send_keys(%(jjjj))
@tty.assert_row(16, '/ ❱    ')
@tty.assert_row(19, ' ❱ jjjj                        1/1')
@tty.assert_row(20, '   jjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkkk...')
@tty.assert_cursor_position(7, 19)

@tty.send_keys(%(\n))
@tty.assert_row(16, '/ ❱ jjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_row(17, 'kkkkkkkkjjjjjjjjjjkkkkkkkkkkkkkkkkkkkkkk')
@tty.assert_cursor_position(22, 22)

puts "    \u2705 Fuzzy-finder still works".encode('utf-8')

# Tab-completing with multiple big-completions
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(v))
(0..4).each do |_i|
  @tty.send_keys(TAB)
end
@tty.assert_row(18, '/ ❱ v')
@tty.assert_row(19, 'vigr')
@tty.assert_row(23, 'very_very_very_very_very_very_very_...')
@tty.assert_cursor_position(5, 18)
@tty.send_keys(%(\n))

@tty.assert_row(18, '/ ❱ very_very_very_very_very_very_very_v')
@tty.assert_row(21, 'ong_filename_to_test_shell_behavior')
@tty.assert_cursor_position(35, 21)

puts "    \u2705 Tab-completing with multiple big-completions".encode('utf-8')

puts 'REMOVES ALL ERRONEOUS WHITESPACE'
# Lines with too many whitespaces get stripped for execution
sleep 0.2
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(  ls   /root\n))
@tty.assert_row(20, '/ ❱   ls   /root')
@tty.assert_row(21, 'another  some_file')

puts "    \u2705 Lines with too many whitespaces get stripped for execution".encode('utf-8')

puts 'CHECKS IF VALID SHELL SYNTAX'
# Doesnt execute if line starts with pipe
sleep 0.2
@tty.send_keys(%(|  ls\n))
@tty.assert_row(20, '/ ❱ |  ls')
@tty.assert_row(21, 'Syntax Error')

puts "    \u2705 Doesnt execute if line starts with pipe".encode('utf-8')

# Doesnt execute if line ends with pipe
sleep 0.2
@tty.send_keys(%(ls|\n))
@tty.assert_row(20, '/ ❱ ls|')
@tty.assert_row(21, 'Syntax Error')

puts "    \u2705 Doesnt execute if line ends with pipe".encode('utf-8')

# Doesnt execute if multiple pipes consecutively
sleep 0.2
@tty.send_keys(%(ls||   cmd\n))
@tty.assert_row(20, '/ ❱ ls||   cmd')
@tty.assert_row(21, 'Syntax Error')

puts "    \u2705 Doesnt execute if multiple pipes consecutively".encode('utf-8')

puts 'ALLOWS COMPLEX COMMANDS'
# Able to pipe command output to other command
sleep 0.2
@tty.send_keys(%(ls ../root|   cat\n))
@tty.assert_row(19, '/ ❱ ls ../root|   cat')
@tty.assert_row(20, 'another')
@tty.assert_row(21, 'some_file')

puts "    \u2705 Able to pipe command output to other command".encode('utf-8')

# Able to concatenate mutliple commands and execute after another
sleep 0.2
@tty.send_keys(%(ls ../root &&   cd /pshell && make\n))
@tty.assert_row(7, '/ ❱ ls ../root &&   cd /pshell && make')
@tty.assert_row(8, 'another  some_file')
@tty.assert_row(10, 'Usage:')
@tty.assert_row(23, '/pshell ❱')

puts "    \u2705 Able to concatenate mutliple commands and execute after another".encode('utf-8')

# Able to mix concatenation and pipes
sleep 0.2
@tty.send_keys(%(ls ../root |   cat && make | sort\n))
@tty.assert_row(5, '/pshell ❱ ls ../root |   cat && make | s')
@tty.assert_row(6, 'ort')
@tty.assert_row(7, 'another')
@tty.assert_row(8, 'some_file')
@tty.assert_row(10, '  clean            Cleans up all binary')
@tty.assert_row(12, '  help             Display this help')
@tty.assert_row(23, '/pshell ❱')

puts "    \u2705 Able to mix concatenation and pipes".encode('utf-8')

puts 'OUTPUT REDIRECTION'
# If file doesnt exist and > creates it and redirects
sleep 0.2
@tty.send_keys(%(ls / > /in1.txt\n))
@tty.assert_row(21, '/pshell ❱ ls / > /in1.txt')
@tty.assert_row(22, '')
@tty.send_keys(%(cat /in1.txt\n))
@tty.assert_row(1, 'bin')
@tty.assert_row(2, 'boot')
@tty.assert_row(3, 'dev')
puts "    \u2705 If file doesnt exist and > creates it".encode('utf-8')

# >> appends to file
sleep 0.2
@tty.send_keys(%(echo some_text>> /in1.txt\n))
@tty.assert_row(21, '/pshell ❱ echo some_text>> /in1.txt')
@tty.assert_row(22, '')
@tty.send_keys(%(cat /in1.txt\n))
@tty.assert_row(0, 'bin')
@tty.assert_row(1, 'boot')
@tty.assert_row(2, 'dev')
@tty.assert_row(21, 'some_text')
puts "    \u2705 >> appends to file".encode('utf-8')

# If multiple output redirections only last one counts
sleep 0.2
@tty.send_keys(%(echo another> /in1.txt >> /in2.txt\n))
sleep 0.2
@tty.send_keys(%(cat /in2.txt && cat /in1.txt | tail -3\n))
@tty.assert_row(18, 'another')
@tty.assert_row(19, 'usr')
@tty.assert_row(20, 'var')
@tty.assert_row(21, 'some_text')
puts "    \u2705 If multiple output redirections only last one counts".encode('utf-8')

# If file exists > truncates existing content
sleep 0.2
@tty.send_keys(%(echo more> /in1.txt\n))
sleep 0.2
@tty.send_keys(%(cat /in1.txt\n))
@tty.assert_row(21, 'more')
puts "    \u2705 If file exists > truncates existing content".encode('utf-8')

puts 'INPUT REDIRECTION'
# When file not found for input throws error
sleep 0.2
@tty.send_keys(%(cat < dsha\n))
@tty.assert_row(20, '/pshell ❱ cat < dsha')
@tty.assert_row(21, 'no such file dsha')
puts "    \u2705 When file not found for input throws error".encode('utf-8')

# Regular input redirection
sleep 0.2
@tty.send_keys(%(ls /root >> /in1.txt\n))
sleep 0.2
@tty.send_keys(%(sort < /in1.txt\n))
@tty.assert_row(19, 'another')
@tty.assert_row(20, 'more')
@tty.assert_row(21, 'some_file')

puts "    \u2705 Regular input redirection".encode('utf-8')

# two input redirections in pipe ignores pipe
sleep 0.2
@tty.send_keys(%(cat < /in1.txt | cat < /in2.txt\n))
@tty.assert_row(21, 'another')
puts "    \u2705 Two input redirections in pipe ignores pipe".encode('utf-8')

# only cares about last redirection
sleep 0.2
@tty.send_keys(%(cat < /in2.txt < /in1.txt\n))
@tty.assert_row(19, 'more')
@tty.assert_row(20, 'another')
@tty.assert_row(21, 'some_file')
puts "    \u2705 Only cares about last redirection".encode('utf-8')

puts 'ERROR REDIRECTION'
# redirects errors to given file
sleep 0.2
@tty.send_keys(%(ls non_existant no_file2> /errors\n))
@tty.assert_row(20, '/pshell ❱ ls non_existant no_file2> /err')
@tty.assert_row(22, '')
sleep 0.2
@tty.send_keys(%(cat /errors\n))
@tty.assert_row(18, "ls: cannot access 'non_existant': No suc")
@tty.assert_row(20, "ls: cannot access 'no_file': No such fil")
puts "    \u2705 Redirects errors to given file".encode('utf-8')

# appends errors when when 2>>
sleep 0.2
@tty.send_keys(%(ls some_error_file 2>>/errors && cat /errors\n))
@tty.assert_row(16, "ls: cannot access 'non_existant': No suc")
@tty.assert_row(18, "ls: cannot access 'no_file': No such fil")
@tty.assert_row(20, "ls: cannot access 'some_error_file': No")
puts "    \u2705 Appends errors when when 2>>".encode('utf-8')

# can split error to error-file and regular output to normal output-redirection with && and |
sleep 0.2
@tty.send_keys(%(ls uwe /bin 2> /errors > /some_file && cat /some_file | grep bz 1>> /bz_bins && cat /bz_bins /errors\n))
@tty.assert_row(18, 'bzless')
@tty.assert_row(19, 'bzmore')
@tty.assert_row(20, "ls: cannot access 'uwe': No such file or")
puts "    \u2705 Can split error to error-file and regular output to normal output-redirection with && and |".encode('utf-8')

puts 'MERGE REDIRECTION'
# when error-output and normal output &> redirects both to same file
sleep 0.2
@tty.send_keys(%(ls not_file /root   &> /merged_ls\n))
sleep 0.2
@tty.send_keys(%(cat /merged_ls\n))
@tty.assert_row(17, "ls: cannot access 'not_file': No such fi")
@tty.assert_row(19, '/root:')
@tty.assert_row(20, 'another')
@tty.assert_row(21, 'some_file')
puts "    \u2705 When error-output and normal output &> redirects both to same file".encode('utf-8')

@tty.send_keys(%(exit\n))
