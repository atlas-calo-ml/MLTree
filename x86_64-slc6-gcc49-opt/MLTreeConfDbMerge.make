#-- start of make_header -----------------

#====================================
#  Document MLTreeConfDbMerge
#
#   Generated Mon Dec 12 19:39:26 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_MLTreeConfDbMerge_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTreeConfDbMerge_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTreeConfDbMerge

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeConfDbMerge = $(MLTree_tag)_MLTreeConfDbMerge.make
cmt_local_tagfile_MLTreeConfDbMerge = $(bin)$(MLTree_tag)_MLTreeConfDbMerge.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeConfDbMerge = $(MLTree_tag).make
cmt_local_tagfile_MLTreeConfDbMerge = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTreeConfDbMerge)
#-include $(cmt_local_tagfile_MLTreeConfDbMerge)

ifdef cmt_MLTreeConfDbMerge_has_target_tag

cmt_final_setup_MLTreeConfDbMerge = $(bin)setup_MLTreeConfDbMerge.make
cmt_dependencies_in_MLTreeConfDbMerge = $(bin)dependencies_MLTreeConfDbMerge.in
#cmt_final_setup_MLTreeConfDbMerge = $(bin)MLTree_MLTreeConfDbMergesetup.make
cmt_local_MLTreeConfDbMerge_makefile = $(bin)MLTreeConfDbMerge.make

else

cmt_final_setup_MLTreeConfDbMerge = $(bin)setup.make
cmt_dependencies_in_MLTreeConfDbMerge = $(bin)dependencies.in
#cmt_final_setup_MLTreeConfDbMerge = $(bin)MLTreesetup.make
cmt_local_MLTreeConfDbMerge_makefile = $(bin)MLTreeConfDbMerge.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTreeConfDbMerge :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTreeConfDbMerge'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTreeConfDbMerge/
#MLTreeConfDbMerge::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_genconfDb_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.confdb file into a single
# <project>.confdb file in the (lib) install area

.PHONY: MLTreeConfDbMerge MLTreeConfDbMergeclean

# default is already '#'
#genconfDb_comment_char := "'#'"

instdir      := ${CMTINSTALLAREA}/$(tag)
confDbRef    := /share/home/jolsson/workarea/MLTreeAthenaAnalysis/MLTree/genConf/MLTree/MLTree.confdb
stampConfDb  := $(confDbRef).stamp
mergedConfDb := $(instdir)/lib/$(project).confdb

MLTreeConfDbMerge :: $(stampConfDb) $(mergedConfDb)
	@:

.NOTPARALLEL : $(stampConfDb) $(mergedConfDb)

$(stampConfDb) $(mergedConfDb) :: $(confDbRef)
	@echo "Running merge_genconfDb  MLTreeConfDbMerge"
	$(merge_genconfDb_cmd) \
          --do-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)

MLTreeConfDbMergeclean ::
	$(cleanup_silent) $(merge_genconfDb_cmd) \
          --un-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)	;
	$(cleanup_silent) $(remove_command) $(stampConfDb)
libMLTree_so_dependencies = ../x86_64-slc6-gcc49-opt/libMLTree.so
#-- start of cleanup_header --------------

clean :: MLTreeConfDbMergeclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTreeConfDbMerge.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeConfDbMergeclean ::
#-- end of cleanup_header ---------------
