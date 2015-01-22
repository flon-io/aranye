
NAME=aranye

default: $(NAME).o

.DEFAULT spec clean:
	$(MAKE) -C tmp/ $@ NAME=$(NAME)

# copy up-to-date version of dep libs into src/
#
stamp:
	cd $(REP) && git log -n 1 | sed 's/^/\/\//' >> ../$(NAME)/$(FIL)
upgrade:
	cp -v ../aabro/src/* src/
	cp -v ../flutil/src/flutil.* src/
	find src/aabro.* -exec $(MAKE) --quiet stamp REP=../aabro FIL={} \;
	find src/flutil.* -exec $(MAKE) --quiet stamp REP=../flutil FIL={} \;

cs: clean spec

.PHONY: spec clean upgrade cs

bin: fara fara-svar
	cp tmp/fara bin/
	cp tmp/fara-svar bin/

