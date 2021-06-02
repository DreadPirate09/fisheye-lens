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

Point2f findFisheyePanoramic(int Xe, int Ye, double R, double Cfx, double Cfy, double He, double We) {
    Point2f fisheyePoint;
    double theta, r, Xf, Yf; //Polar coordinates

    r = Ye / He * R;
    theta = Xe / We * 2.0 * PI;
    Xf = Cfx + r * sin(theta);
    Yf = Cfy + r * cos(theta);
    fisheyePoint.x = Xf;
    fisheyePoint.y = Yf;

    return fisheyePoint;
}


Point2f findFisheyeProjection(int Xe, int Ye, double R, double Cfx, double Cfy, int Hf, int Wf) {

    Point2f pfish;
    double theta, phi, r;
    Point3f psph;
    //
    float FOV = 3.141592654; // FOV of the fisheye, eg: 180 degrees  
    float width = Wf;
    float height = Hf;
    //
    //   

    theta = 2*3.14159265 * ((double)Xe / (double)width - 0.5f);// -pi to pi  
    phi = 3.14159265 * ((double)Ye /(double)height - 0.5f);  // -pi/2 to pi/2 

    //
    //    // Vector in 3D space  
    psph.x = cos(phi) * sin(theta);
    psph.y = cos(phi) * cos(theta);
    psph.z = sin(phi);
    //
    //    // Calculate fisheye angle and radius  
    theta = atan2(psph.z, psph.x);
    phi = atan2(sqrt(psph.x * psph.x + psph.z * psph.z), psph.y);
    r = width * phi / FOV;
    //
    //    // Pixel in fisheye space  
    pfish.x = 0.5 * width + r * cos(theta);
    pfish.y = 0.5 * width + r * sin(theta);

    return pfish;

}

int main(int argc, char** argv) {

    Mat fisheyeImage, equirectangularImage;

    fisheyeImage = imread(PATH_IMAGE, IMREAD_COLOR);
    namedWindow("Fisheye Image", WINDOW_AUTOSIZE);
    imshow("Fisheye Image", fisheyeImage);


    while (waitKey(0) != ESC) {
        //wait until the key ESC is pressed
    }

    //destroyWindow("Fisheye Image");

    int Hf, Wf, He, We;
    double R, Cfx, Cfy;

    Hf = fisheyeImage.size().height;
    Wf = fisheyeImage.size().width;
    R = Hf / 2; //The fisheye image is a square of 1400x1400 pixels containing a circle so the radius is half of the width or height size
    Cfx = Wf / 2; //The fisheye image is a square so the center in x is located at half the distance of the width
    Cfy = Hf / 2; //The fisheye image is a square so the center in y is located at half the distance of the height

    He = (int)R;
    We = (int)2 * PI * R;

    equirectangularImage.create(Hf, Wf, fisheyeImage.type());

    for (int Xe = 0; Xe < equirectangularImage.size().width; Xe++) {
        for (int Ye = 0; Ye < equirectangularImage.size().height; Ye++) {

            equirectangularImage.at<Vec3b>(Point(Xe, Ye)) = fisheyeImage.at<Vec3b>(findFisheyeProjection(Xe, Ye, R, Cfx, Cfy,Hf,Wf));
        }
    }

    namedWindow("Equirectangular Image", WINDOW_AUTOSIZE);
    imshow("Equirectangular Image", equirectangularImage);

    while (waitKey(0) != ESC) { 
        //wait until the key ESC is pressed
    }

    //destroyWindow("Fisheye Image");

    imwrite("C:/Users/georg/OneDrive/Desktop/equirectangularImage.jpg", equirectangularImage);

    return 0;
}