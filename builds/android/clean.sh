

JNI_DIR="../../src/jni"

if [ -d $JNI_DIR ]; then
	rm -rf $JNI_DIR;
fi

if [ -d $JNI_DIR/obj ]; then
	rm -rf $JNI_DIR/obj
fi

exit 0;
