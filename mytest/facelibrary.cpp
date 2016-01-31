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

void FaceLibrary::loadLibrary(string in){
	//load A
	char* A_src = new char[100];
	sprintf(A_src, "%s/A.xml", in.data());
	A = (CvMat*)cvLoad(A_src);

	//load face
	DIR* dir = NULL;
	char* face_src = new char[100];
	sprintf(face_src, "%s/face", in.data());

	dir = opendir(face_src);

	dirent* ent = NULL;
	if(dir == NULL) return;

	while((ent = readdir(dir))!=NULL) {
		char* suffix = new char[100];
		char* name = new char[100];
		sscanf(ent->d_name,"%*[^.].%s", suffix);
		sscanf(ent->d_name,"%[^.]%*s", name);
		if(strcmp(suffix, "xml")==0) {
			Face* f= new Face();
			f->name = name;
			char* img_src = new char[100];
			sprintf(img_src,"%s/face/%s",in.data(),ent->d_name);
			CvMat* img =(CvMat*)cvLoad(img_src);
			f->eigen = img;
			face.push_back(f);
		}
	}

	number = face.size();
	eigen_number = face[0]->eigen->rows;
}

void FaceLibrary::loadRecFace(string in){
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
			rec_face.push_back(f);
		}
	}
	row = rec_face[0]->data->rows;
	col = rec_face[0]->data->cols;
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

void FaceLibrary::recFace(int recn) {

	CvMat* At = cvCreateMat(eigen_number, row * col, CV_64FC1);
	cvTranspose(A, At);

	for(int t = 0; t < rec_face.size(); t ++) {
		//cvNamedWindow(rec_face[t]->name);
		CvMat* rec = cvCreateMat(row * col, 1, CV_64FC1);
		for(int i = 0; i < row; i ++)
			for(int j = 0; j < col; j++)
				CV_MAT_ELEM(*rec, double, i*col+j, 0) = CV_MAT_ELEM(*rec_face[t]->data, double, i, j);
		rec_face[t]->eigen = cvCreateMat(eigen_number, 1, CV_64FC1);
		cvMatMul(At, rec, rec_face[t]->eigen);

		double min[100];
		int min_index[100];

		for(int i = 0; i < recn; i ++) {
			min[i] = cvNorm(face[i]->eigen, rec_face[t]->eigen, CV_L2);
			min_index[i] = i;
		}

		for(int i = recn; i < face.size(); i ++) {
			double dis = cvNorm(face[i]->eigen, rec_face[t]->eigen, CV_L2);
			if(dis < min[recn - 1]) {
				for(int j = 0; j <recn; j ++){
					if(dis < min[j]){
						for(int k = recn - 1; k > j; k --) { min[k] = min[k - 1]; min_index[k] = min_index[k - 1];}
						min[j] = dis;
						min_index[j] = i;
						j = recn;
					}
				}
			}
		}

		cout<<"Test people: "<<rec_face[t]->name<<endl;
		for(int i = 0; i < recn; i ++)
		cout<<"This might be "<<face[min_index[i]]->name<<"; distance is "<<min[i]<<endl;
	}
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
	
	cvWaitKey(0);
	cvDestroyWindow("meanFace");
}

void FaceLibrary::getEigen() {
	eigen_value = cvCreateMat(row * col, 1, CV_64FC1);
	eigen_vector = cvCreateMat(row * col, row * col, CV_64FC1);
	printf("ready to compute eigen\n");
	cvEigenVV(covar_mat, eigen_vector, eigen_value);
	printf("finish computing eigen\n");
}

void FaceLibrary::saveEigen(string in, int k) {
	eigen_number = k;
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
		face[n]->eigen = yi;
	}
	cout<<"finish eigen face"<<endl;
	
}

