#include "att.h"
#include <vector>
struct Face{
	char* name;
	CvMat* data;
	CvMat* eigen;
};

class FaceLibrary{
	public:
		FaceLibrary();
		~FaceLibrary();
		void loadFace(string in);
		void getMediaFace();
		void getEigen();
		void saveEigen(string in, int n);
		void loadLibrary(string in);
		void loadRecFace(string in);
		void recFace(int recn);
	private:
		vector<Face*> face;
		vector<Face*> rec_face;
		CvMat *image_points[1];
		Mat_<double>meanMat;
		Mat_<double>covMat;
		Mat_<double>sample[100];
		CvMat *covar_mat;
		CvMat *mean_mat;
		CvMat *eigen_value;
		CvMat *eigen_vector;
		CvMat *A;
		CvMat *eigen_image;
		int row;
		int col;
		int number;
		int eigen_number;
};
