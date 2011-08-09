#! /bin/bash

SRC_DIR="../../src/"
JNI_DIR="../../src/jni"
UUID_DIR="uuid"

NDK_BUILD=`which ndk-build`

if [ -z "$NDK_BUILD" ]; then
	echo "Must have ndk-build in your path"
	exit 1;
fi

if [ ! -d $JNI_DIR ]; then
	mkdir $JNI_DIR;
fi	

if [ -d $UUID_DIR ]; then
	echo "Copying $UUID_DIR $JNI_DIR/$UUID_DIR"
	cp -r $UUID_DIR $JNI_DIR/$UUID_DIR
fi

for i in `ls $SRC_DIR`; do
	if [ -f $SRC_DIR/$i -a ! -f $JNI_DIR/$i ]; then
		echo "copying $SRC_DIR/$i --> $JNI_DIR/$i";
		cp $SRC_DIR/$i $JNI_DIR/$i;
	fi
done;

cp Android.mk $JNI_DIR;
cp Application.mk $JNI_DIR

pushd .
cd $JNI_DIR
$NDK_BUILD
popd

exit 0;
