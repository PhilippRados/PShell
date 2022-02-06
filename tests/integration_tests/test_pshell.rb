require 'ttytest'

# @tty = TTYtest.new_terminal(%(PS1='$ '../../src/bin/pshell), width: 80, height: 24)
# # @tty.assert_row(0, '$')
# # @tty.assert_cursor_position(x: 2, y: 0)

# sleep 1
# @tty.send_keys(%(ls\n))

# @tty.assert_contents <<~TTY

# coding/ruby ❱ ls
# log.txt         test_pshell.rb

# coding/ruby ❱#{' '}
# TTY

# puts 'All tests passed!'

# @tty = TTYtest.new_terminal(%{PS1='$ ' ../../src/bin/pshell},width: 80, height: 24)
# # @tty.assert_row(0, '$')
# # @tty.assert_cursor_position(x: 2, y: 0)

# sleep 2
# @tty.send_keys(%{ls ..\n})

# @tty.assert_contents <<TTY

# coding/ruby ❱ ls ..
# integration_tests unit_tests

# coding/ruby ❱
# TTY
# puts "still alive"
# # @tty.assert_cursor_position(x: 2, y: 2)

# # p @tty.rows # => ["$ echo \"Hello, world\"", "Hello, world", "$", "", "", "", ...]

@tty = TTYtest.new_terminal(%(PS1='$ ' ../../src/bin/pshell), width: 80, height: 24)
# @tty.assert_row(0, '$')
# @tty.assert_cursor_position(x: 2, y: 0)

sleep 1
@tty.send_keys(%(test\n))

@tty.assert_contents <<~TTY

  tests/integration_tests ❱ ls
  couldn't find command uwe

  tests/integration_tests ❱
TTY

@tty.send_keys(%(q\n))
