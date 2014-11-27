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
set -e
set -o pipefail

function show_usage()
{
   echo "Usage: $0 [options]"
   echo "Run software verification tests."
   echo "Options:"
   echo "  -c, --concise       Print minimal feedback."
   echo "  -h, --help          Print this help and exit"
}

# Parse input arguments.
VERBOSE_FLAG="--verbose"

if [[ $# > 0 ]]
then
   while [[ $# > 0 ]]
   do
      PARAM="$1"
      shift
      case $PARAM in
         -c|--concise) VERBOSE_FLAG="";;
         -h|--help) show_usage; exit 0;;
         *) echo "Unknown parameter '$PARAM'"; show_usage; exit 1;;
      esac
   done
fi

SCRIPTDIR="$(git rev-parse --show-toplevel)/scripts"

# Validate documentation.
"$SCRIPTDIR/makedoc.sh" --check-only $VERBOSE_FLAG

# Perform static and runtime code analysis.
"$SCRIPTDIR/analysis.sh" --static --runtime $VERBOSE_FLAG

exit 0
