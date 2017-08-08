.ONESHELL:
cc = gcc
#cc = clang
copy = rsync --recursive --links --perms --times --group --owner --devices --specials --verbose --human-readable
w = -Wno-int-conversion -Wno-incompatible-pointer-types -Wunused-value
o = -O2
f = -rdynamic
l = -ldl

build:
	./tool/tangle.js &&\
	xxd -i core/0.0.1/core.jo > core/0.0.1/core.h &&\
	$(cc) $(w) $(o) $(f) $(l) jojo.c -o jojo

user-install:
	$(copy) core ~/.jojo/
	$(copy) module ~/.jojo/

clean:
	rm jojo
	rm -f *~ */*~ */*/*~ */*/*/*~ */*/*/*/*~ */*/*/*/*/*~
