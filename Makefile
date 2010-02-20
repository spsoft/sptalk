
#--------------------------------------------------------------------

ifeq ($(origin version), undefined)
	version = 0.2
endif

#--------------------------------------------------------------------

all: 3rd
	@( cd sptalk; make )

3rd:
	@( cd spxml; make )
	@( cd spdict; make )
	@( cd spserver; make )

dist: clean sptalk-$(version).src.tar.gz

sptalk-$(version).src.tar.gz:
	@find . -type f | grep -v CVS | grep -v .svn | sed s:^./:sptalk-$(version)/: > MANIFEST
	@(cd ..; ln -s sptalk sptalk-$(version))
	(cd ..; tar cvf - `cat sptalk/MANIFEST` | gzip > sptalk/sptalk-$(version).src.tar.gz)
	@(cd ..; rm sptalk-$(version))

clean:
	@( cd spxml; make clean )
	@( cd spdict; make clean )
	@( cd spserver; make clean )
	@( cd sptalk; make clean )

