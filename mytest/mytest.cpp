#include "facelibrary.h"
int main(int argc, char* argv[]) {
	Att att;
	FaceLibrary facelibrary;

	if(argc != 4){
		printf("Parameter Invalid\n");
		return -1;
	}

	int facenum = atoi(argv[3]);

	facelibrary.loadLibrary(argv[2]);
	facelibrary.loadRecFace(argv[1]);	
	facelibrary.recFace(facenum);
}
