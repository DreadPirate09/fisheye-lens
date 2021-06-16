#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h> 

using namespace std;
using namespace cv;

const double PI = 3.141592653589793;
const string PATH_IMAGE = "C:/Users/georg/OneDrive/Desktop/img11.png";
const int ESC = 27;

double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);
    s1.convertTo(s1, CV_32F);
    s1 = s1.mul(s1);

    Scalar s = sum(s1);

    double sse = s.val[0] + s.val[1] + s.val[2];

    if (sse <= 1e-10)
        return 0;
    else
    {
        double  mse = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

Point2f findFisheyePanoramic(int Xe, int Ye, double R, double Cfx, double Cfy, double He, double We) {
    Point2f fisheyePoint;
    double theta, r, Xf, Yf;

    r = Ye / He * R;
    theta = Xe / We * 2.0 * PI;
    Xf = Cfx + r * sin(theta);
    Yf = Cfy + r * cos(theta);
    fisheyePoint.x = Xf;
    fisheyePoint.y = Yf;

    return fisheyePoint;
}


Point2f findFisheyeProjection(int Xe, int Ye, double R, int Hf, int Wf) {

    Point2f pfish;
    double theta, phi, r;
    Point3f psph;

    float FOV = 3.141592654; 
    float width = Wf;
    float height = Hf;

    theta = 3.14159265 * ((double)Xe / (double)width - 0.5f);
    phi = 3.14159265 * ((double)Ye /(double)height - 0.5f);  
 
    psph.x = cos(phi) * sin(theta);
    psph.y = cos(phi) * cos(theta);
    psph.z = sin(phi);

    theta = atan2(psph.z, psph.x);
    phi = atan2(sqrt(psph.x * psph.x + psph.z * psph.z), psph.y);
    r = width * phi / FOV;
    
    pfish.x = 0.5 * width + r * cos(theta);
    pfish.y = 0.5 * width + r * sin(theta);

    return pfish;

}

int main(int argc, char** argv) {

    Mat fisheyeImage, equirectangularImage;

    fisheyeImage = imread(PATH_IMAGE, IMREAD_COLOR);
    namedWindow("Fisheye Image", WINDOW_AUTOSIZE);
    imshow("Fisheye Image", fisheyeImage);

    cout << "Press ESC to procede" << endl;

    while (waitKey(0) != ESC) {
        // wait until the key ESC is pressed
    }


    int Hf, Wf, He, We;
    double R, Cfx, Cfy;

    Hf = fisheyeImage.size().height;
    Wf = fisheyeImage.size().width;
    R = Hf / 2;
    Cfx = Wf / 2; 
    Cfy = Hf / 2; 

    He = (int)R;
    We = (int)2 * PI * R;

    bool sw = true;  // false for panoramal images and true for projections

    if (!sw) {
        equirectangularImage.create(He, We, fisheyeImage.type());
        cout << "here" << endl;
    }
    else {
        equirectangularImage.create(Hf, Wf, fisheyeImage.type());
    }

    for (int Xe = 0; Xe < equirectangularImage.size().width; Xe++) {
        for (int Ye = 0; Ye < equirectangularImage.size().height; Ye++) {

            if (!sw) {
                equirectangularImage.at<Vec3b>(Point(Xe, Ye)) = fisheyeImage.at<Vec3b>(findFisheyePanoramic(Xe, Ye, R, Cfx, Cfy, He, We));
            }
            else {
                equirectangularImage.at<Vec3b>(Point(Xe, Ye)) = fisheyeImage.at<Vec3b>(findFisheyeProjection(Xe, Ye, R, Hf, Wf));
            }
        }
    }

    if (sw) {
        cout << "The PSNR " << getPSNR(fisheyeImage, equirectangularImage) << endl; // we do the Peak of the signal ratio just for sqare images
    }

    cout << "here" << endl;

    namedWindow("Equirectangular Image", WINDOW_AUTOSIZE);
    imshow("Equirectangular Image", equirectangularImage);
    imwrite("C:/Users/georg/OneDrive/Desktop/img.jpg", equirectangularImage);

    cout << "Press ESC to end" << endl;
    while (waitKey(0) != ESC) { 
        //wait until the key ESC is pressed
    }

    //destroyWindow("Fisheye Image");


    return 0;
}