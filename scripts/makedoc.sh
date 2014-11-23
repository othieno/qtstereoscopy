#!/bin/bash
#
# The MIT License (MIT)
#
# Copyright (c) 2014 Jeremy Othieno.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
function show_usage()
{
   echo "Usage: $0 [options]"
   echo "Validate and/or build QtStereoscopy's API documentation."
   echo "Options:"
   echo "  -c, --check-only   Validate the documentation then exit"
   echo "  -v, --verbose      Show more detailed messages"
   echo "  -h, --help         Print this help and exit"
}

# Check for the qdoc executable.
QDOC=$(type -P qdoc)
if [ ! -x "$QDOC" ]
then
   echo "Error! Could not find the 'qdoc' executable. Check to make sure it's installed, in the PATH, and executable."
   exit 1
fi

# Parse input arguments.
CHECK_ONLY=false
VERBOSE=false

while [[ $# > 0 ]]
do
   PARAM="$1"
   shift
   case $PARAM in
      -c|--check-only) CHECK_ONLY=true;;
      -v|--verbose) VERBOSE=true;;
      -h|--help) show_usage; exit 0;;
      *) echo "Unknown parameter '$PARAM'"; show_usage; exit 1;;
   esac
done

QDOCCONF="$(git rev-parse --show-toplevel)/doc/qtstereoscopy.qdocconf"

# Check to make sure the documentation can be built. Because qdoc returns zero even when warnings are issued,
# we cannot rely on its exit code to determine if it failed. We can, however, check if its stderr output is
# empty (no errors or warnings) or not (warnings or errors building the documentation).
OUTPUT="$($QDOC -redirect-documentation-to-dev-null "$QDOCCONF" 2>&1)"
if [ ! -z $OUTPUT ]
then
   if [[ "$VERBOSE" = false ]]; then OUTPUT="The QtStereoscopy API documentation is either incomplete or obsolete. Please update it."; fi
   echo "$OUTPUT" 1>&2
   exit 1
fi

# Build the documentation, or exit after the check.
if [[ "$CHECK_ONLY" = true ]]; then echo "The QtStereoscopy API documentation is up-to-date."; else $QDOC $QDOCCONF; fi

exit 0
