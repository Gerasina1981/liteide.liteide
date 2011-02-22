#!/bin/sh

rm ../bin/gopromake*
rm ../bin/goastview*
cd go/promake
gomake
cd ../astview
../../../bin/gopromake -gopro astview.pro

