#!/bin/bash

   git clone git@github.com:supranove/qtstereoscopy.git -b experimental --single-branch\
&& cd ./qtstereoscopy/doc/\
&& qdoc qtstereoscopy.qdocconf -outputdir ../../api\
&& cd ../../\
&& rm -rf qtstereoscopy
