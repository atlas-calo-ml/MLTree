#-- start of make_header -----------------

#====================================
#  Document checkreq
#
#   Generated Mon Dec 12 19:38:48 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_checkreq_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_checkreq_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_checkreq

MLTree_tag = $(tag)

#cmt_local_tagfile_checkreq = $(MLTree_tag)_checkreq.make
cmt_local_tagfile_checkreq = $(bin)$(MLTree_tag)_checkreq.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_checkreq = $(MLTree_tag).make
cmt_local_tagfile_checkreq = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_checkreq)
#-include $(cmt_local_tagfile_checkreq)

ifdef cmt_checkreq_has_target_tag

cmt_final_setup_checkreq = $(bin)setup_checkreq.make
cmt_dependencies_in_checkreq = $(bin)dependencies_checkreq.in
#cmt_final_setup_checkreq = $(bin)MLTree_checkreqsetup.make
cmt_local_checkreq_makefile = $(bin)checkreq.make

else

cmt_final_setup_checkreq = $(bin)setup.make
cmt_dependencies_in_checkreq = $(bin)dependencies.in
#cmt_final_setup_checkreq = $(bin)MLTreesetup.make
cmt_local_checkreq_makefile = $(bin)checkreq.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#checkreq :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'checkreq'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = checkreq/
#checkreq::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of cmt_action_runner_header ---------------

ifdef ONCE
checkreq_once = 1
endif

ifdef checkreq_once

checkreqactionstamp = $(bin)checkreq.actionstamp
#checkreqactionstamp = checkreq.actionstamp

checkreq :: $(checkreqactionstamp)
	$(echo) "checkreq ok"
#	@echo checkreq ok

#$(checkreqactionstamp) :: $(checkreq_dependencies)
$(checkreqactionstamp) ::
	$(silent) checkreq.py -i 2 -n
	$(silent) cat /dev/null > $(checkreqactionstamp)
#	@echo ok > $(checkreqactionstamp)

checkreqclean ::
	$(cleanup_silent) /bin/rm -f $(checkreqactionstamp)

else

#checkreq :: $(checkreq_dependencies)
checkreq ::
	$(silent) checkreq.py -i 2 -n

endif

install ::
uninstall ::

#-- end of cmt_action_runner_header -----------------
#-- start of cleanup_header --------------

clean :: checkreqclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(checkreq.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

checkreqclean ::
#-- end of cleanup_header ---------------
