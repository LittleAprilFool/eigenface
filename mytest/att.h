#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <vector>
#include <string.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

class Att
{
	public:
		Att();
		~Att();
		void att_pre(string in, string out, int mode);
	private:
		char* in_src;
		char* out_src;
		void att_pic(char* file, string in, string out, int num);
		void pic_save(Mat img, char* file, string out);
		void pic_rect(Mat img, char* file);
		void pic_show(Mat img, char* file, string in, string out, int num);
		Rect roi;
};
