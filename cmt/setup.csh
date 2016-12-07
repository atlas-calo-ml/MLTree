# echo "setup MLTree MLTree-00-00-00 in /afs/cern.ch/work/j/jolsson/MLTree"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.7/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtMLTreetempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtMLTreetempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=MLTree -version=MLTree-00-00-00 -path=/afs/cern.ch/work/j/jolsson/MLTree  -no_cleanup $* >${cmtMLTreetempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=MLTree -version=MLTree-00-00-00 -path=/afs/cern.ch/work/j/jolsson/MLTree  -no_cleanup $* >${cmtMLTreetempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtMLTreetempfile}
  unset cmtMLTreetempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtMLTreetempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtMLTreetempfile}
unset cmtMLTreetempfile
exit $cmtsetupstatus

