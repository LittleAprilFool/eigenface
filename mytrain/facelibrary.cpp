#include "facelibrary.h"
FaceLibrary::FaceLibrary() {
}

FaceLibrary::~FaceLibrary() {
}

double* img2double(IplImage* img){
	CvMat* im = cvCreateMat(img->height, img->width, CV_64FC1);
	cvConvert(img, im);
	double* d = im->data.db;
	return d;
}


void FaceLibrary::loadFace(string in){
	DIR* dir = NULL;
	dir = opendir(in.data());

	dirent* ent = NULL;

	if(dir == NULL) return;

	while((ent = readdir(dir))!=NULL) {
		char* suffix = new char[100];
		char* name = new char[100];
		sscanf(ent->d_name,"%*[^.].%s", suffix);
		sscanf(ent->d_name,"%[^.]%*s", name);
		if(strcmp(suffix, "pgm")==0) {
			Face* f= new Face();
			f->name = name;
			char* img_src = new char[100];
			sprintf(img_src,"%s/%s",in.data(),ent->d_name);
			IplImage* d = cvLoadImage(img_src, CV_LOAD_IMAGE_GRAYSCALE);
			CvMat* mat = cvCreateMat(d->height, d->width, CV_64FC1);
			cvConvert(d, mat);
			f->data = mat;
			face.push_back(f);
		}
	}

	row = face[0]->data->rows;
	col = face[0]->data->cols;
	number = face.size();
}

void FaceLibrary::getMediaFace() {
	int k = number;
	int m = row;
	int n = col;
	
	printf("%d %d %d\n", k, m, n);

	image_points[0] = cvCreateMat(k, m*n, CV_64FC1);
	for(int num = 0; num < k; num ++) {
		for(int i = 0; i < m; i ++)
			for(int j = 0; j < n; j ++){
				CV_MAT_ELEM(*image_points[0], double, num, i*n+j) = CV_MAT_ELEM(*face[num]->data, double, i, j);
			}
	}
	
	covar_mat = cvCreateMat(m*n, m*n, CV_64FC1);
	mean_mat = cvCreateMat(1, m*n, CV_64FC1);
	
	cvCalcCovarMatrix((const CvArr**)image_points, k, covar_mat, mean_mat, CV_COVAR_NORMAL|CV_COVAR_ROWS);

	CvMat* Media = cvCreateMat(m, n, CV_64FC1);
	for(int i = 0; i < m; i ++)
		for(int j = 0; j < n; j++){
			CV_MAT_ELEM(*Media, double, i, j) = CV_MAT_ELEM(*mean_mat, double,0, i*n+j);
		}
	
	cvNamedWindow("meanFace");

	IplImage* mm = cvCreateImage(CvSize(Media->cols, Media->rows), 8, 1);
	cvConvert(Media, mm);

	cvShowImage("meanFace", mm);
	cvSaveImage("../Resource/mean.jpg", mm);	
	cvWaitKey(0);
	cvDestroyWindow("meanFace");
}

void FaceLibrary::getEigen() {
	eigen_value = cvCreateMat(row * col, 1, CV_64FC1);
	eigen_vector = cvCreateMat(row * col, row * col, CV_64FC1);
	printf("ready to compute eigen\n");
	cvEigenVV(covar_mat, eigen_vector, eigen_value);
	printf("finish computing eigen\n");
	
	IplImage* dstImg = cvCreateImage(CvSize(col * 5, row * 2),8,1);

	cvZero(dstImg);

	for(int t1 = 0; t1 < 2; t1 ++) {
		for(int t2 = 0; t2 < 5; t2 ++) {
			CvMat * img_mat = cvCreateMat(row, col, CV_64FC1);
			for(int i = 0; i < row; i ++)
				for(int j = 0; j < col; j ++)
					CV_MAT_ELEM(*img_mat, double, i, j) = CV_MAT_ELEM(*eigen_vector, double, t1 * 5 + t2, i * col + j);
			cvNormalize(img_mat, img_mat, 255.0, 0.0, CV_MINMAX);
			IplImage* img = cvCreateImage(CvSize(col, row), 8, 1);
			cvConvert(img_mat, img);
			cvSetImageROI(dstImg, cvRect(t2 * col, t1 * row, col, row));
			cvCopy(img, dstImg);
			cvResetImageROI(dstImg);
		}
	}

	cvNamedWindow("eigenFace");
	cvShowImage("eigenFace", dstImg);
	cvSaveImage("../Resource/dstimg.jpg", dstImg);
	cvWaitKey(-1);
	cvDestroyWindow("eigenFace");
}

void FaceLibrary::saveEigen(string in, int k) {
	A = cvCreateMat(row * col, k, CV_64FC1);
	for(int t = 0; t < k; t ++) {
		CvMat *eigen_face = cvCreateMat(row, col, CV_64FC1);
		for(int i = 0; i < row; i ++)
			for(int j = 0; j < col; j ++) {
				CV_MAT_ELEM(*eigen_face, double,i,j) = CV_MAT_ELEM(*eigen_vector, double, t, i*col+j);
			}
		cvNormalize(eigen_face, eigen_face, 255.0, 0.0, CV_MINMAX);

		for(int i = 0; i < row; i++)
			for(int j = 0; j < col; j++) {
				CV_MAT_ELEM(*A, double, i * col + j, t) = CV_MAT_ELEM(*eigen_face, double, i, j);
			}
		IplImage* eigen_img = cvCreateImage(CvSize(col, row), 8, 1);
		cvConvert(eigen_face, eigen_img);
		char* img_src = new char[100];
		sprintf(img_src, "%s/eigen%d.pgm", in.data(), t);
		cvSaveImage(img_src, eigen_img);
	}

	CvMat* At = cvCreateMat(k, row * col, CV_64FC1);
	cvTranspose(A, At);
	char* src = new char[100];
	sprintf(src, "%s/A.xml", in.data());
	cvSave(src, A);	
	cout<<"finish saving eigen"<<endl;

	CvMat* X = cvCreateMat(row*col, number, CV_64FC1);
	cvTranspose(image_points[0], X);
	eigen_image = cvCreateMat(k, number, CV_64FC1);
	cvMatMul(At, X, eigen_image);
	sprintf(src, "%s/EigenImage.xml", in.data());
	cvSave(src, eigen_image);
	
	for(int n = 0; n < number; n ++) {
		CvMat* yi = cvCreateMat(k, 1, CV_64FC1);
		for(int i = 0; i < k; i ++)
			CV_MAT_ELEM(*yi, double, i, 0) = CV_MAT_ELEM(*eigen_image, double, i, n);
		char* img_src = new char[100];
		sprintf(img_src, "%s/face/%s.xml", in.data(), face[n]->name);
		cvSave(img_src, yi);
	}
	cout<<"finish eigen face"<<endl;
	
}

void FaceLibrary::test() {
	float coord[4] = {1, 1, 1, 1};
	CvMat* test[1];
	test[0] = cvCreateMat(2,2, CV_32FC1);
	for(int i = 0; i < 2; i ++)
		for(int j = 0; j < 2; j ++)
		CV_MAT_ELEM(*test[0], float, i,j) = coord[i * 2 + j];
	
	CvMat* covar = cvCreateMat(2, 2, CV_32FC1);
	CvMat* avg = cvCreateMat(1, 2, CV_32FC1);
	cvCalcCovarMatrix((const CvArr**) test, 2, covar, avg, CV_COVAR_NORMAL + CV_COVAR_ROWS);
	cout<<CV_MAT_ELEM(*covar, float, 0, 0)<<endl;
	cout<<CV_MAT_ELEM(*covar, float, 1, 0)<<endl;
	cout<<CV_MAT_ELEM(*covar, float, 0, 1)<<endl;
	cout<<CV_MAT_ELEM(*covar, float, 1, 1)<<endl;
	cout<<CV_MAT_ELEM(*avg, float, 0, 0) <<endl;
	cout<<CV_MAT_ELEM(*avg, float, 0, 1) <<endl;
}
