#include "att.h"

Att::Att() {
	roi.x = 0;
	roi.y = 0;
	roi.height = 100;
	roi.width = 80;
}
Att::~Att() {
}

void Att::att_pre(string in, string out, int mode) {
	DIR* dir = NULL;
	dir = opendir(in.data());

	dirent* ent = NULL;

	if(dir == NULL) return;
	while((ent = readdir(dir)) != NULL) {
		if(mode == 1) {
			if(ent->d_name[0] == 's') {
				att_pic(ent->d_name, in, out, 1);
			}
		}
		if(mode == 0) {
			if(ent->d_name[0] !='.') {
				att_pic(ent->d_name, in, out, 0);
			}
		}
	}
}


//show image with face mask
void Att::pic_rect(Mat img, char* file) {
	Point lefteye = Point(roi.x + roi.width / 3, roi.y + roi.height / 3);
	Point righteye = Point(roi.x + roi.width * 2 / 3, roi.y + roi.height / 3);
	rectangle(img, roi, Scalar(0,0,255));
	line(img, lefteye - Point(5, 0), lefteye + Point(5, 0), Scalar(0, 0, 255));
	line(img, lefteye - Point(0, 5), lefteye + Point(0, 5), Scalar(0, 0, 255));
	line(img, righteye - Point(5, 0), righteye + Point(5, 0), Scalar(0, 0, 255));
	line(img, righteye - Point(0, 5), righteye + Point(0, 5), Scalar(0, 0, 255));
	imshow(file, img);
}

void Att::att_pic(char* file, string in, string out, int num) {
	char* img_src;
	if(num != 0) {
		if(num>10) num = 1;
		img_src = new char[100];
		sprintf(img_src, "%s/%s/%d.pgm", in.data(), file, num);
	}

	if(num == 0) {
		img_src = new char[100];
		sprintf(img_src, "%s/%s/%d.jpg", in.data(), file, num);
	}
	
	cvNamedWindow(file, CV_WINDOW_AUTOSIZE);
	Mat img = imread(img_src, IMREAD_GRAYSCALE);
	roi.x = img.cols / 2 - roi.width / 2;
	roi.y = img.rows / 2 - roi.height / 2;
	pic_show(img, file, in, out, num);

	cvDestroyWindow(file);
}

void Att::pic_show(Mat img, char* file, string in, string out, int num) {
	Mat i;
	img.copyTo(i);
	pic_rect(i, file);

	int t = cvWaitKey(-1);
    
	Mat img2;

	switch (t){
		//key'enter' - save image
		case 10: 
			pic_save(img, file, out);
			break;
		
		//key'N' - next face of this people
		case 110:
			if(num != 0) num++;
			att_pic(file, in, out, num); 
			break;
		
		//key'+' - resize image
		case 61:
			resize(img, img2, Size(img.cols * 1.1, img.rows * 1.1), 0, 0, CV_INTER_LINEAR);
			roi.x = img2.cols / 2 - roi.width / 2;
			roi.y = img2.rows / 2 - roi.height / 2;
			pic_show(img2, file, in, out, num);
			break;
		//key'-' - resize image
		case 45:
			resize(img, img2, Size(img.cols * 0.9, img.rows * 0.9), 0, 0, CV_INTER_LINEAR);
			roi.x = img2.cols / 2 - roi.width / 2;
			roi.y = img2.rows / 2 - roi.height / 2;
			pic_show(img2, file, in, out, num);
			break;
		
		//key'r' - reset image
		case 114:
			att_pic(file, in, out, num);
			break;

		//key'right' - move right
		case 65363:
			roi.x ++;
			pic_show(img, file, in, out, num);
			break;
        //key'left' - move left
		case 65361:
			roi.x --;
			pic_show(img, file, in, out, num);
			break;
	    //key'down' - move down
		case 65362:
			roi.y --;
			pic_show(img, file, in, out, num);
			break;
		//key'up' - move up
		case 65364:
			roi.y ++;
			pic_show(img, file, in, out, num);
			break;

		//key'q' - quit
		case 113:
			exit(0);
			break;
		default:
			pic_show(img, file, in, out, num);
			break;
	}

}

void Att::pic_save(Mat img, char* file, string out) {
	char* img_src = new char[100];
	if((roi.x + roi.width >= img.cols) || (roi.y + roi.height >= img.rows)) { 
		printf("CORP ERROR!!!\n");
		return;
	}
	Mat img_roi = img(roi);
	sprintf(img_src, "%s/%s.pgm", out.data(), file);
	cvNamedWindow("after_corp", CV_WINDOW_AUTOSIZE);
	Mat img_resize;
	resize(img_roi, img_resize, Size(img_roi.cols * 0.5, img_roi.rows * 0.5), 0, 0, CV_INTER_LINEAR);
	imshow("after_corp", img_resize);
	cvWaitKey(0);
	equalizeHist(img_resize, img_resize);
	imshow("after_corp", img_resize);
	cvWaitKey(0);
    //stretch(img_roi);
	//cvWaitKey(0);
	cvDestroyWindow("after_corp");
	imwrite(img_src, img_resize);
}
