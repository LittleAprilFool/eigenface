#include "facelibrary.h"

int main(int argc, char* argv[]) {

	if(argc!=4){
		printf("Parameters Invalid");
		return -1;
	}

	int energy = atoi(argv[1]);
	char* folder = argv[2];
	int mode = atoi(argv[3]);

	Att att;
	FaceLibrary library;

	int li = 2;

	mode = 1;

	if(mode == 0 || mode == 2 ) {
		char* a_src = new char[100]; sprintf(a_src, "%s/rawface/att_face", folder);
		char* m_src = new char[100]; sprintf(m_src, "%s/rawface/my_face", folder);
		char* o_src = new char[100]; sprintf(o_src, "%s/facelibrary/face%d", folder, li);
		cout<<a_src<<o_src<<endl;
		att.att_pre(a_src, o_src, 1);
	    att.att_pre(m_src, o_src, 0);
	}
	
	if(mode ==1 || mode == 2) {
		char* l_src = new char[100]; sprintf(l_src, "%s/facelibrary/face%d", folder, li);
		char* m_src = new char[100]; sprintf(m_src, "%s/model", folder);
		library.loadFace(l_src);
		library.getMediaFace();
		library.getEigen();
		library.saveEigen(m_src, energy);
	}
	return 0;
}
