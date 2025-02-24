mingw32-make
if ($?){
	echo "build complete, running.."
	./main
} else {
	echo "something went south, good luck debugging man."
}
