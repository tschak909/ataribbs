##
## AtariBBS Master Make file.
## Hack-o-matic, for sure, it will get better.
##

.PHONY: all mostlyclean clean install zip

.SUFFIXES:

all mostlyclean clean install zip:
	@$(MAKE) -C bbslib    --no-print-directory $@
	@$(MAKE) -C bbsconf   --no-print-directory $@
	@$(MAKE) -C bbs       --no-print-directory $@
