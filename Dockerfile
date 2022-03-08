FROM ubuntu:18.04

RUN apt update && \
  apt install gcc make ruby tmux -y && \
  gem install ttytest

ENV RUBYOPT="-KU -E utf-8:utf-8"
WORKDIR "/var/lib/gems/2.5.0/gems/ttytest-0.5.0/lib/ttytest"
RUN sed 's/def assert_cursor_position(x:, y:)/def assert_cursor_position(x, y)/' \
  matchers.rb > \
  matchers.rb.changed && \
  mv matchers.rb.changed matchers.rb
RUN touch /home/very_long_filename_to_test_shell_behavior_and_tab_complete
RUN touch /home/short_file

WORKDIR "/pshell"
ENTRYPOINT [ "/bin/bash", "-l", "-c" ]
