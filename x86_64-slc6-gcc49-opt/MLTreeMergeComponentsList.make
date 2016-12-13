#-- start of make_header -----------------

#====================================
#  Document MLTreeMergeComponentsList
#
#   Generated Mon Dec 12 19:39:27 2016  by jolsson
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_MLTreeMergeComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTreeMergeComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTreeMergeComponentsList

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeMergeComponentsList = $(MLTree_tag)_MLTreeMergeComponentsList.make
cmt_local_tagfile_MLTreeMergeComponentsList = $(bin)$(MLTree_tag)_MLTreeMergeComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeMergeComponentsList = $(MLTree_tag).make
cmt_local_tagfile_MLTreeMergeComponentsList = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTreeMergeComponentsList)
#-include $(cmt_local_tagfile_MLTreeMergeComponentsList)

ifdef cmt_MLTreeMergeComponentsList_has_target_tag

cmt_final_setup_MLTreeMergeComponentsList = $(bin)setup_MLTreeMergeComponentsList.make
cmt_dependencies_in_MLTreeMergeComponentsList = $(bin)dependencies_MLTreeMergeComponentsList.in
#cmt_final_setup_MLTreeMergeComponentsList = $(bin)MLTree_MLTreeMergeComponentsListsetup.make
cmt_local_MLTreeMergeComponentsList_makefile = $(bin)MLTreeMergeComponentsList.make

else

cmt_final_setup_MLTreeMergeComponentsList = $(bin)setup.make
cmt_dependencies_in_MLTreeMergeComponentsList = $(bin)dependencies.in
#cmt_final_setup_MLTreeMergeComponentsList = $(bin)MLTreesetup.make
cmt_local_MLTreeMergeComponentsList_makefile = $(bin)MLTreeMergeComponentsList.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTreeMergeComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTreeMergeComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTreeMergeComponentsList/
#MLTreeMergeComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_componentslist_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.components file into a single
# <project>.components file in the (lib) install area
# If no InstallArea is present the fragment is dummy


.PHONY: MLTreeMergeComponentsList MLTreeMergeComponentsListclean

# default is already '#'
#genmap_comment_char := "'#'"

componentsListRef    := ../$(tag)/MLTree.components

ifdef CMTINSTALLAREA
componentsListDir    := ${CMTINSTALLAREA}/$(tag)/lib
mergedComponentsList := $(componentsListDir)/$(project).components
stampComponentsList  := $(componentsListRef).stamp
else
componentsListDir    := ../$(tag)
mergedComponentsList :=
stampComponentsList  :=
endif

MLTreeMergeComponentsList :: $(stampComponentsList) $(mergedComponentsList)
	@:

.NOTPARALLEL : $(stampComponentsList) $(mergedComponentsList)

$(stampComponentsList) $(mergedComponentsList) :: $(componentsListRef)
	@echo "Running merge_componentslist  MLTreeMergeComponentsList"
	$(merge_componentslist_cmd) --do-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList)

MLTreeMergeComponentsListclean ::
	$(cleanup_silent) $(merge_componentslist_cmd) --un-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList) ;
	$(cleanup_silent) $(remove_command) $(stampComponentsList)
libMLTree_so_dependencies = ../x86_64-slc6-gcc49-opt/libMLTree.so
#-- start of cleanup_header --------------

clean :: MLTreeMergeComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTreeMergeComponentsList.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeMergeComponentsListclean ::
#-- end of cleanup_header ---------------
