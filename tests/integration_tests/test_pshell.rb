require 'ttytest'

@tty = TTYtest.new_terminal(%( PS1='$ '  ./src/bin/pshell), width: 40, height: 24)

BACKSPACE = 127.chr
TAB = 9.chr
CTRLF = 6.chr

# Basic IO
sleep 0.2
@tty.send_keys(%(tes))
@tty.assert_cursor_position(13, 1)
@tty.send_keys(%(\n))

@tty.assert_row(1, '/pshell ❱ tes')
@tty.assert_row(2, "couldn't find command tes")

puts "\u2705 Basic I/O".encode('utf-8')

# Arrow upwards moves through history
sleep 0.2
@tty.send_keys(%(ls))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZA))
@tty.assert_cursor_position(13, 4)

@tty.assert_row(4, '/pshell ❱ tes')

puts "\u2705 Arrow up moves up in command history".encode('utf-8')

# Arrow downwards moves back down
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZB))
@tty.assert_cursor_position(10, 4)

@tty.assert_row(4, '/pshell ❱ ')

puts "\u2705 Arrow downwards moves back to empty line".encode('utf-8')

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

puts "\u2705 Arrow left moves backwards through line".encode('utf-8')

# Arrow right moves cursor on pos right
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))
@tty.assert_cursor_position(14, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))

puts "\u2705 Arrow right moves one pos right".encode('utf-8')

# Backspace removes char and moves cursor
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(BACKSPACE)
@tty.assert_cursor_position(11, 4)
@tty.send_keys(%(\n))

@tty.assert_row(4, '/pshell ❱ tlst')

puts "\u2705 Backspace removes char and moves cursor".encode('utf-8')

# Shows matching autocomplete
sleep 0.2
@tty.send_keys(%(so))
@tty.assert_cursor_position(12, 7)

@tty.assert_row(7, '/pshell ❱ some autocomplete')

puts "\u2705 Shows matching autocomplete".encode('utf-8')

# Arrow right puts cursor end of line and copies autocomplete
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))

@tty.assert_cursor_position(27, 7)
@tty.assert_row(7, '/pshell ❱ some autocomplete')

@tty.send_keys(%(\n))

puts "\u2705 Arrow right moves cursor end of line".encode('utf-8')

# builtin-cd changes dir
sleep 0.2
@tty.send_keys(%(cd ..))
@tty.send_keys(%(\n))

@tty.assert_cursor_position(4, 12)
@tty.assert_row(12, '/ ❱')

puts "\u2705 Builtin-cd changes dir".encode('utf-8')

# Cursor jumps down when line longer than term-width
sleep 0.2
@tty.send_keys(%(lllllllllllllllllllllllllllllllllllllllllllllll))

@tty.assert_cursor_position(11, 13)
@tty.assert_row(12, '/ ❱ llllllllllllllllllllllllllllllllllll')
@tty.assert_row(13, 'lllllllllll')
@tty.send_keys(%(\n))

puts "\u2705 Cursor jumps down one row if line too long".encode('utf-8')

# TODO: write test for checking if cursor jumps down too early

# When on last line and line too long it shifts term up accordingly
sleep 0.2
@tty.send_keys(%(cd pshell))
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(make)) # arbitrary command to move to last line
@tty.send_keys(%(\n))
sleep 0.2
@tty.send_keys(%(kajldasjlsdjalsjdaljdjldasjlasldslks))

@tty.assert_cursor_position(6, 23)
@tty.assert_row(22, '/pshell ❱ kajldasjlsdjalsjdaljdjldasjlas')
@tty.assert_row(23, 'ldslks')

puts "\u2705 When on last line and line too long it shifts term up accordingly".encode('utf-8')

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

puts "\u2705 When on last line and autocomplete longer than term should shift up but cursor stay".encode('utf-8')

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

puts "\u2705 When not on last line and autocomplete longer than term should not shift up".encode('utf-8')

# Multiple Tab-completion matches
sleep 0.2
@tty.send_keys(%(ls))
@tty.send_keys(TAB)
@tty.assert_cursor_position(12, 4)

@tty.assert_row(4, '/pshell ❱ ls')
@tty.assert_row(5, 'lsattr    lsns      lsmem     lslogins')
@tty.assert_row(6, 'lsipc     lslocks   lscpu     lsblk')
@tty.assert_row(7, 'ls')

puts "\u2705 Multiple Tab-completion matches".encode('utf-8')

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

puts "\u2705 Pressing Tab cycles through tab-matches".encode('utf-8')

# When only one match copies to line immediately
sleep 0.2
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(BACKSPACE)
@tty.send_keys(%(a))
@tty.send_keys(TAB)
@tty.assert_cursor_position(16, 4)

@tty.assert_row(4, '/pshell ❱ lsattr')

puts "\u2705 When only one tab-comp matches immediately".encode('utf-8')

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

puts "\u2705 When pressing Tab in middle of word looks for matches until cursor-pos".encode('utf-8')

# When Tab-completing on last row should shift up and cursor too
sleep 0.2
@tty.send_keys(%(a))
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

puts "\u2705 When Tab-comp on last row should shift term up and cursor too".encode('utf-8')

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

puts "\u2705 When pressing Fuzzy-finder on last row should shift prior commands up".encode('utf-8')

# Fuzzy-finder picked command replaces current line
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(ZB))

@tty.assert_row(19, ' ❱ te                          2/2')
@tty.send_keys(%(\n))
@tty.assert_row(16, '/pshell ❱ tes')
@tty.assert_cursor_position(13, 16)

puts "\u2705 Fuzzy-finder picked command replaces current line".encode('utf-8')

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

puts "\u2705 When line longer than term fuzzy-finder should still shift bottom of line up".encode('utf-8')

# When exiting fuzzy-finder should be on upshifted row
sleep 0.2
@tty.send_keys(%(\033))
@tty.send_keys(%(\033))

@tty.assert_row(15, '/pshell ❱ llllllllllllllllllllllllllllll')
@tty.assert_row(16, 'lllllllllllllllll')
@tty.assert_row(17, '')
@tty.assert_cursor_position(12, 15)

puts "\u2705 When exiting fuzzy-finder should be on upshifted row".encode('utf-8')

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

puts "\u2705 File-completes shows all matching files/dirs".encode('utf-8')

# When only one matching file autocompletes immediately
sleep 0.2
@tty.send_keys(%(a)) # random key to exit out of tab-matches
@tty.send_keys(%(ro))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/')
@tty.assert_cursor_position(21, 15)

puts "\u2705 When only one matching file autocompletes immediately".encode('utf-8')

# Doesnt show dotfiles in file-completion
sleep 0.2
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/')
@tty.assert_row(16, '')
@tty.assert_cursor_position(21, 15)

puts "\u2705 Doesnt show dotfiles in completion".encode('utf-8')

# Shows dotfiles in file-completion if first char is dot
sleep 0.2
@tty.send_keys(%(.))
@tty.send_keys(TAB)

@tty.assert_row(15, '/pshell ❱ ls ../root/.')
@tty.assert_row(16, '.bashrc       .profile')
@tty.assert_row(17, '../           ./')
@tty.assert_row(18, '.psh_history  .gem/')
@tty.assert_cursor_position(22, 15)

puts "\u2705 Shows dotfiles if first char is dot".encode('utf-8')

# If tab-completing on second row completes below cursor
sleep 0.2
@tty.send_keys(%(\n))
@tty.send_keys(%(\n))
sleep 0.1
@tty.send_keys(%(ls                                             ../m))
@tty.send_keys(TAB)

@tty.assert_row(18, '/pshell ❱ ls')
@tty.assert_row(19, '                 ../m')
@tty.assert_row(20, 'media/  mnt/')
@tty.assert_cursor_position(21, 19)

puts "\u2705 If tab-completing on second row completes below cursor".encode('utf-8')

# When pressing enter on tab-comp on second row appends to cursor
sleep 0.2
@tty.send_keys(%(\n))
@tty.assert_row(18, '/pshell ❱ ls')
@tty.assert_row(19, '                 ../media/')
@tty.assert_cursor_position(26, 19)

puts "\u2705 When pressing enter on tab-comp on second row appends to cursor".encode('utf-8')

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

puts "\u2705 When on last line and shifts up when tab-completing on second row".encode('utf-8')
@tty.send_keys(%(q\n))
