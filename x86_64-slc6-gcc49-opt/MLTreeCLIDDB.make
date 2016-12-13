#-- start of make_header -----------------

#====================================
#  Document MLTreeCLIDDB
#
#   Generated Mon Dec 12 19:39:22 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_MLTreeCLIDDB_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTreeCLIDDB_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTreeCLIDDB

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeCLIDDB = $(MLTree_tag)_MLTreeCLIDDB.make
cmt_local_tagfile_MLTreeCLIDDB = $(bin)$(MLTree_tag)_MLTreeCLIDDB.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeCLIDDB = $(MLTree_tag).make
cmt_local_tagfile_MLTreeCLIDDB = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTreeCLIDDB)
#-include $(cmt_local_tagfile_MLTreeCLIDDB)

ifdef cmt_MLTreeCLIDDB_has_target_tag

cmt_final_setup_MLTreeCLIDDB = $(bin)setup_MLTreeCLIDDB.make
cmt_dependencies_in_MLTreeCLIDDB = $(bin)dependencies_MLTreeCLIDDB.in
#cmt_final_setup_MLTreeCLIDDB = $(bin)MLTree_MLTreeCLIDDBsetup.make
cmt_local_MLTreeCLIDDB_makefile = $(bin)MLTreeCLIDDB.make

else

cmt_final_setup_MLTreeCLIDDB = $(bin)setup.make
cmt_dependencies_in_MLTreeCLIDDB = $(bin)dependencies.in
#cmt_final_setup_MLTreeCLIDDB = $(bin)MLTreesetup.make
cmt_local_MLTreeCLIDDB_makefile = $(bin)MLTreeCLIDDB.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTreeCLIDDB :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTreeCLIDDB'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTreeCLIDDB/
#MLTreeCLIDDB::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/genCLIDDB_header
# Author: Paolo Calafiura
# derived from genconf_header

# Use genCLIDDB_cmd to create package clid.db files

.PHONY: MLTreeCLIDDB MLTreeCLIDDBclean

outname := clid.db
cliddb  := MLTree_$(outname)
instdir := $(CMTINSTALLAREA)/share
result  := $(instdir)/$(cliddb)
product := $(instdir)/$(outname)
conflib := $(bin)$(library_prefix)MLTree.$(shlibsuffix)

MLTreeCLIDDB :: $(result)

$(instdir) :
	$(mkdir) -p $(instdir)

$(result) : $(conflib) $(product)
	@$(genCLIDDB_cmd) -p MLTree -i$(product) -o $(result)

$(product) : $(instdir)
	touch $(product)

MLTreeCLIDDBclean ::
	$(cleanup_silent) $(uninstall_command) $(product) $(result)
	$(cleanup_silent) $(cmt_uninstallarea_command) $(product) $(result)

#-- start of cleanup_header --------------

clean :: MLTreeCLIDDBclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTreeCLIDDB.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeCLIDDBclean ::
#-- end of cleanup_header ---------------
