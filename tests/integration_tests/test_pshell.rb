require 'ttytest'

@tty = TTYtest.new_terminal(%( PS1='$ '  ./src/bin/pshell), width: 40, height: 24)

BACKSPACE = 127.chr
TAB = 9.chr

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

puts "\u2705 When not on last line and autocomplete longer than term should not shift up".encode('utf-8')
@tty.send_keys(%(q\n))
