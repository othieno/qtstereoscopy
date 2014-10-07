#!/bin/bash
#
# This script performs software testing and verification. It fails if any one
# of the following commands has an exit status that is not zero (failure).
set -e
set -o pipefail

# Echo to stderr.
function perror()
{
   echo $1 1>&2
}

BASEDIR="$(git rev-parse --show-toplevel)"
EXAMPLESDIR="$BASEDIR/examples"
UNITTESTSDIR="$BASEDIR/test/unit"

# Perform static analysis.
printf "\
   \r$BASEDIR/src\n\
   \r$EXAMPLESDIR\n\
   \r$UNITTESTSDIR\n"\
| xargs cppcheck --file-list=- warning,style,performance,portability,information --language=c++ --error-exitcode=1


# Run unit tests.
qmake "$UNITTESTSDIR" -o "$UNITTESTSDIR/Makefile" && make -C "$UNITTESTSDIR" && "$UNITTESTSDIR/build/testqtstereoscopy"


# Make sure the examples can be built without any errors.
qmake "$EXAMPLESDIR" -o "$EXAMPLESDIR/Makefile" && make -C "$EXAMPLESDIR"


# Same goes for the documentation. Note that we can't rely on qdoc's exit code because it returns zero even when
# warnings are issued. Instead, we check whether qdoc's stderr output is an empty string (success) or not (failure).
if [ ! -z "$(qdoc -redirect-documentation-to-dev-null "$BASEDIR/doc/qtstereoscopy.qdocconf" 2>&1)" ]
then
   perror "The QtStereoscopy API documentation is either incomplete or obsolete. Please update it."
   exit 1
fi
