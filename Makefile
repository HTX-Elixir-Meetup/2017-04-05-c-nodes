
#OTPROOT=$(wildcard /opt/erlang/lib/erl_interface-*)
OTPROOT=/usr/local/lib/erlang/usr/lib/


all:	bin/cnodeserver bin/cnodeclient bin/Elixir.Complex.beam

bin/Elixir.Complex.beam: src/complex.ex
	elixirc -o bin $<

bin/%:	src/%.c
	mkdir -p bin
	gcc -o $@ -I$(OTPROOT)/include -L$(OTPROOT)/lib src/complex.c $< -lerl_interface -lei -lpthread -lnsl

clean:
	rm -rf bin

start_server:
	epmd -daemon
	bin/cnodeserver 3456

start_elixir:
	echo "run ':complex3.foo(4)' via elixir shell"
	iex --sname e1 --cookie secretcookie -pa bin
