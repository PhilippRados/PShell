require 'ttytest'

@tty = TTYtest.new_terminal(%( PS1='$ '  ./src/bin/pshell), width: 40, height: 24)

BACKSPACE = 127.chr
TAB = 9.chr

# Basic IO
sleep 0.5
@tty.send_keys(%(t))
@tty.send_keys(%(e))
@tty.send_keys(%(s))
@tty.assert_cursor_position(13, 1)
@tty.send_keys(%(\n))

@tty.assert_row(1, '/pshell ❱ tes')
@tty.assert_row(2, "couldn't find command tes")

puts "\u2705 Basic I/O".encode('utf-8')

# Arrow upwards moves through history
sleep 0.5
@tty.send_keys(%(l))
@tty.send_keys(%(s))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(%(\033))
@tty.send_keys(%(ZA))
@tty.assert_cursor_position(13, 4)

@tty.assert_row(4, '/pshell ❱ tes')

puts "\u2705 Arrow up moves up in command history".encode('utf-8')

# Arrow downwards moves back down
sleep 0.5
@tty.send_keys(%(\033))
@tty.send_keys(%(ZB))
@tty.assert_cursor_position(10, 4)

@tty.assert_row(4, '/pshell ❱ ')

puts "\u2705 Arrow downwards moves back to empty line".encode('utf-8')

# Arrow left moves through current line
sleep 0.5
@tty.send_keys(%(t))
@tty.send_keys(%(e))
@tty.send_keys(%(s))
@tty.send_keys(%(t))
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
sleep 0.5
@tty.send_keys(%(\033))
@tty.send_keys(%(ZD))
@tty.assert_cursor_position(12, 4)
@tty.send_keys(BACKSPACE)
@tty.assert_cursor_position(11, 4)
@tty.send_keys(%(\n))

@tty.assert_row(4, '/pshell ❱ tlst')

puts "\u2705 Backspace removes char and moves cursor".encode('utf-8')

# Shows matching autocomplete
sleep 0.5
@tty.send_keys(%(s))
@tty.send_keys(%(o))
@tty.assert_cursor_position(12, 7)

@tty.assert_row(7, '/pshell ❱ some autocomplete')

puts "\u2705 Shows matching autocomplete".encode('utf-8')

# Arrow right puts cursor end of line and copies autocomplete
sleep 0.5
@tty.send_keys(%(\033))
@tty.send_keys(%(ZC))

@tty.assert_cursor_position(27, 7)
@tty.assert_row(7, '/pshell ❱ some autocomplete')

@tty.send_keys(%(\n))

puts "\u2705 Arrow right moves cursor end of line".encode('utf-8')

# builtin-cd changes dir
sleep 0.5
@tty.send_keys(%(c))
@tty.send_keys(%(d))
@tty.send_keys(%( ))
@tty.send_keys(%(.))
@tty.send_keys(%(.))
@tty.send_keys(%(\n))

@tty.assert_cursor_position(4, 12)
@tty.assert_row(12, '/ ❱')

puts "\u2705 Builtin-cd changes dir".encode('utf-8')

@tty.send_keys(%(q\n))
