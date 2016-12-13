#-- start of make_header -----------------

#====================================
#  Document MLTreeComponentsList
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

cmt_MLTreeComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_MLTreeComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_MLTreeComponentsList

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeComponentsList = $(MLTree_tag)_MLTreeComponentsList.make
cmt_local_tagfile_MLTreeComponentsList = $(bin)$(MLTree_tag)_MLTreeComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

MLTree_tag = $(tag)

#cmt_local_tagfile_MLTreeComponentsList = $(MLTree_tag).make
cmt_local_tagfile_MLTreeComponentsList = $(bin)$(MLTree_tag).make

endif

include $(cmt_local_tagfile_MLTreeComponentsList)
#-include $(cmt_local_tagfile_MLTreeComponentsList)

ifdef cmt_MLTreeComponentsList_has_target_tag

cmt_final_setup_MLTreeComponentsList = $(bin)setup_MLTreeComponentsList.make
cmt_dependencies_in_MLTreeComponentsList = $(bin)dependencies_MLTreeComponentsList.in
#cmt_final_setup_MLTreeComponentsList = $(bin)MLTree_MLTreeComponentsListsetup.make
cmt_local_MLTreeComponentsList_makefile = $(bin)MLTreeComponentsList.make

else

cmt_final_setup_MLTreeComponentsList = $(bin)setup.make
cmt_dependencies_in_MLTreeComponentsList = $(bin)dependencies.in
#cmt_final_setup_MLTreeComponentsList = $(bin)MLTreesetup.make
cmt_local_MLTreeComponentsList_makefile = $(bin)MLTreeComponentsList.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)MLTreesetup.make

#MLTreeComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'MLTreeComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = MLTreeComponentsList/
#MLTreeComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
##
componentslistfile = MLTree.components
COMPONENTSLIST_DIR = ../$(tag)
fulllibname = libMLTree.$(shlibsuffix)

MLTreeComponentsList :: ${COMPONENTSLIST_DIR}/$(componentslistfile)
	@:

${COMPONENTSLIST_DIR}/$(componentslistfile) :: $(bin)$(fulllibname)
	@echo 'Generating componentslist file for $(fulllibname)'
	cd ../$(tag);$(listcomponents_cmd) --output ${COMPONENTSLIST_DIR}/$(componentslistfile) $(fulllibname)

install :: MLTreeComponentsListinstall
MLTreeComponentsListinstall :: MLTreeComponentsList

uninstall :: MLTreeComponentsListuninstall
MLTreeComponentsListuninstall :: MLTreeComponentsListclean

MLTreeComponentsListclean ::
	@echo 'Deleting $(componentslistfile)'
	@rm -f ${COMPONENTSLIST_DIR}/$(componentslistfile)

#-- start of cleanup_header --------------

clean :: MLTreeComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(MLTreeComponentsList.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

MLTreeComponentsListclean ::
#-- end of cleanup_header ---------------
