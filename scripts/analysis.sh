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
   echo "Perform static, runtime and performance code analysis."
   echo "Options:"
   echo "  -s, --static        Perform static code analysis only."
   echo "  -r, --runtime       Perform runtime code analysis only."
   echo "  -p, --performance   Perform performance analysis only."
   echo "  -v, --verbose       Show more detailed messages"
   echo "  -h, --help          Print this help and exit"
}

# Check for required executables.
CPPCHECK=$(type -P cppcheck)
if [ ! -x "$CPPCHECK" ]
then
   echo "Error! Could not find the 'cppcheck' executable. Check to make sure it's installed, in the PATH, and executable."
   exit 1
fi

QMAKE=$(type -P qmake)
if [ ! -x "$QMAKE" ]
then
   echo "Error! Could not find the 'qmake' executable. Check to make sure it's installed, in the PATH, and executable."
   exit 1
fi

MAKE=$(type -P make)
if [ ! -x "$MAKE" ]
then
   echo "Error! Could not find the 'make' executable. Check to make sure it's installed, in the PATH, and executable."
   exit 1
fi

# Parse input arguments.
VERBOSE=false
STATIC=false
RUNTIME=false
PERFORMANCE=false

if [[ $# > 0 ]]
then
   while [[ $# > 0 ]]
   do
      PARAM="$1"
      shift
      case $PARAM in
         -s|--static) STATIC=true;;
         -r|--runtime) RUNTIME=true;;
         -p|--performance) PERFORMANCE=true;;
         -v|--verbose) VERBOSE=true;;
         -h|--help) show_usage; exit 0;;
         *) echo "Unknown parameter '$PARAM'"; show_usage; exit 1;;
      esac
   done
fi

# If $STATIC, $RUNTIME and $PERFORMANCE are all false, then their corresponding flags weren't
# passed to the script. This means all three tests are performed by default.
if [[ "$STATIC" = "$RUNTIME" && "$RUNTIME" = "$PERFORMANCE" ]]
then
   STATIC=true
   RUNTIME=true
   PERFORMANCE=true
fi

BASEDIR="$(git rev-parse --show-toplevel)"

if [[ "$STATIC" = true ]]
then
   if [[ "$VERBOSE" = false ]]; then printf "Performing static code analysis... "; fi

   CPPCHECKFLAGS="-q"
   if [[ "$VERBOSE" = true ]]; then CPPCHECKFLAGS=""; fi
   CPPCHECKFLAGS="$CPPCHECKFLAGS warning,style,performance,portability,information --language=c++ --error-exitcode=1"

   # Perform static code analysis.
   printf "$BASEDIR/src $BASEDIR/test $BASEDIR/examples" | xargs $CPPCHECK $CPPCHECKFLAGS --file-list=-

   if [[ "$VERBOSE" = false ]]; then echo "done"; fi
fi

if [[ "$RUNTIME" = true ]]
then
   if [[ "$VERBOSE" = false ]]; then printf "Performing runtime code analysis... "; fi

   MAKEFLAGS="--silent"
   if [[ "$VERBOSE" = true ]]; then MAKEFLAGS=""; fi

   function do_runtime_tests()
   {
      # Make sure the examples can be built without any errors, then run the unit tests.
      EXAMPLESDIR="$BASEDIR/examples"
      UNITTESTSDIR="$BASEDIR/test/unit"
      $QMAKE "$EXAMPLESDIR" -o "$EXAMPLESDIR/Makefile"\
      && $MAKE $MAKEFLAGS -C "$EXAMPLESDIR"\
      && $QMAKE "$UNITTESTSDIR" -o "$UNITTESTSDIR/Makefile"\
      && $MAKE $MAKEFLAGS -C "$UNITTESTSDIR"\
      && "$UNITTESTSDIR/oculusvr_testsuite/build/oculusvr_testsuite"
   }

   if [[ "$VERBOSE" = false ]]; then do_runtime_tests > /dev/null && echo "done"; else do_runtime_tests; fi
fi

if [[ "$PERFORMANCE" = true ]]
then
   # Run benchmarks.
	BENCHMARKDIR="$BASEDIR/test/benchmark"
	$QMAKE "$BENCHMARKDIR" -o "$BENCHMARKDIR/Makefile"\
	&& $MAKE $MAKEFLAGS -C "$BENCHMARKDIR"\
	&& "$BENCHMARKDIR/oculusvr_benchmarks/build/oculusvr_benchmarks"
fi

exit 0
