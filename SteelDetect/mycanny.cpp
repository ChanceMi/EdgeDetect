#include <math.h>
#include <fstream>
#include "mycanny.h"
#define MAX

//read img
//change gray img
//
    /*1.高斯滤波对图像去噪。
	2.由原始灰度图求出纵横2个梯度图，以及综合梯度图（求梯度的算子很多）。
	3.结合3个 梯度图来进行非极大抑制（此步一过，检测的图像边缘已经很细了）
	4.进行边缘连接（强边缘到 弱边缘的连接，所有可能连接的点都出不了非极大抑制后的范围，在非极大抑制的结果中去除假边缘保留真边缘，同时又能让保留的 边缘尽量连贯真实）
	5.对边缘进行细化
	*/
/* Scale floating point magnitudes and angles to 8 bits */
#define ORI_SCALE 40.0
#define MAG_SCALE 20.0

/* Biggest possible filter mask */
#define MAX_MASK_SIZE 20

/* Fraction of pixels that should be above the HIGH threshold */

float ratio = 0.1;
int WIDTH = 0;
typedef cv::Mat IMAGE;

void readData(){
    cv::Mat srcImg,dstImg;
}



//sqrt x*x+y*y
float norm (float x, float y)
{
	return (float) sqrt ( (double)(x*x + y*y) );
}

void canny (float s, IMAGE& im, IMAGE& mag, IMAGE& ori){
	int width;
	f2D smx, smy, dx, dy;
	int i,j,k,n;
	float gau[MAX_MASK_SIZE], dgau[MAX_MASK_SIZE], z;

/*11111  Create a Gaussian and a derivative of Gaussian filter mask */
	for(i=0; i<MAX_MASK_SIZE; i++){
	  gau[i] = meanGauss ((float)i, s);
	  if (gau[i] < 0.005){
		width = i;
		break;
	  }
	  dgau[i] = dGauss ((float)i, s);
	}

	n = width+width + 1;
	WIDTH = width/2;
	cout<<"Smoothing with a Gaussian ="<<n<<endl;
	smx.Initialize(im.nr, im.nc);
	smy.Initialize(im.nr, im.nc);

/*22222 Convolution of source image with a Gaussian in X and Y directions  */
	seperable_convolution (im, gau, width, smx, smy);

/* Now convolve smoothed data with a derivative */
	cout<<"Convolution with the derivative of a Gaussian...\n";


	dx.Initialize(im.nr, im.nc);
	dxy_seperable_convolution (smx, im.nr, im.nc,
		 dgau, width, dx, 1);
	smx.Finish();

	dy.Initialize(im.nr, im.nc);
	// xy 分别卷积
	dxy_seperable_convolution (smy, im.nr, im.nc,dgau, width, dy, 0);
	smy.Finish();

/* Create an image of the norm of dx,dy */
	for (i=0; i<im.nr; i++)
	  for (j=0; j<im.nc; j++){
	      z = norm (dx.m[i][j], dy.m[i][j]);
	      mag.m[i][j] = (unsigned char)(z*MAG_SCALE);//?????
		  ///why scale twenty 20 bei
	  }

/* 33333 Non-maximum suppression - edge pixels should be a local max */
	nonmax_suppress (dx, dy, (int)im.nr, (int)im.nc, mag, ori);
	dx.Finish();
	dy.Finish();
}

/*      Gaussian        */
float gauss(float x, float sigma)
{
    float xx;
    if (sigma == 0) return 0.0;
    xx = (float)exp((double) ((-x*x)/(2*sigma*sigma)));
    return xx;
}

// meanGauss
float meanGauss (float x, float sigma)
{
	float z;
	z = (gauss(x,sigma)+gauss(x+0.5,sigma)+gauss(x-0.5,sigma))/3.0;
	z = z/(3.14159*2.0*sigma*sigma);
	return z;
}

/*   First derivative of Gaussian    */
float dGauss (float x, float sigma)
{
	return -x/(sigma*sigma) * gauss(x, sigma);
}



int range (IMAGE& im, int i, int j)
{
	if ((i<0) || (i>=im.nr)) return 0;
	if ((j<0) || (j>=im.nc)) return 0;
	return 1;
}
/*      TRACE - recursively trace edge pixels that have a threshold > the low
	edge threshold, continuing from the pixel at (i,j).                     */
//将边缘连接起来
int trace (int i, int j, int low, IMAGE& im,IMAGE& mag, IMAGE& ori)
{
	int n,m;
	char flag = 0;

	if (im.m[i][j] == 0){
	  im.m[i][j] = 255;
	  flag=0;
	  for (n= -1; n<=1; n++){
	    for(m= -1; m<=1; m++){
	      if (i==0 && m==0) continue;
	      if (range(mag, i+n, j+m) && mag.m[i+n][j+m] >= low)
		     if (trace(i+n, j+m, low, im, mag, ori)){
		       flag=1;
		       break;
		    }
	    }
	    if (flag) break;
	 }
	  return(1);
   }
	return(0);
}


//
void estimate_thresh (IMAGE& mag, int *hi, int *low)
{
	int i,j,k, hist[256], count;

/* Build a histogram of the magnitude image. */
	for (k=0; k<256; k++) hist[k] = 0;

	for (i=WIDTH; i<mag.nr-WIDTH; i++)
	  for (j=WIDTH; j<mag.nc-WIDTH; j++)
	    hist[mag.m[i][j]]++;

/* The high threshold should be > 80 or 90% of the pixels
	j = (int)(ratio*mag.nr*mag.nc);
*/
	j = mag.nr;
	if (j<mag.nc) j = mag.nc;
	j = (int)(0.9*j);
	k = 255;

	count = hist[255];
	while (count < j){
	  k--;
	  if (k<0) break;
	  count += hist[k];
	}
	*hi = k;

	i=0;
	while (hist[i]==0) i++;
	*low = (*hi+i)/2.0;
}



/*      HYSTERESIS thersholding of edge pixels. Starting at pixels with a
	value greater than the HIGH threshold, trace a connected sequence
	of pixels that have a value greater than the LOW threhsold.        */
//自适应得到阀值
void hysteresis (int high, int low, IMAGE& im, IMAGE& mag, IMAGE& oriim)
{
	int i,j,k;
	cout<<"Beginning hysteresis thresholding...\n";
	for (i=0; i<im.nr; i++)
	  for (j=0; j<im.nc; j++)
	    im.m[i][j] = 0;

	if (high<low){
	  estimate_thresh (mag, &high, &low);
//	  printf ("Hysteresis thresholds (from image): HI %d LOW %D\n",high, low);
	}
/* For each edge with a magnitude above the high threshold, begin
   tracing edge pixels that are above the low threshold.                */

	for (i=0; i<im.nr; i++)
	  for (j=0; j<im.nc; j++)
	    if (mag.m[i][j] >= high)
	      trace (i, j, low, im, mag, oriim);

/* Make the edge black (to be the same as the other methods) */
	for (i=0; i<im.nr; i++)
	  for (j=0; j<im.nc; j++)
	    if (im.m[i][j] == 0) 
			im.m[i][j] = 255;
	    else im.m[i][j] = 0;
}





// 分别卷积
void seperable_convolution (IMAGE& im, float *gau, int width,
		f2D& smx, f2D& smy)
{
	int i,j,k, I1, I2, nr, nc;
	float x, y;

	nr = im.nr;
	nc = im.nc;

	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	  {
	    x = gau[0] * im.m[i][j]; y = gau[0] * im.m[i][j];
	    for (k=1; k<width; k++)
	    {
	      I1 = (i+k)%nr; 
		  I2 = (i-k+nr)%nr;
	      y += gau[k]*im.m[I1][j] + gau[k]*im.m[I2][j];
	      I1 = (j+k)%nc; 
		  I2 = (j-k+nc)%nc;
	      x += gau[k]*im.m[i][I1] + gau[k]*im.m[i][I2];
	    }
	    smx.m[i][j] = x; 
		smy.m[i][j] = y;
	  }
}

void dxy_seperable_convolution (f2D& im, int nr, int nc,  float *gau,
		int width, f2D& sm, int which)
{
	int i,j,k, I1, I2;
	float x;

	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++){
	    x = 0.0;
	    for (k=1; k<width; k++){
	      if (which == 0){
		    I1 = (i+k)%nr; 
			I2 = (i-k+nr)%nr;
		    x += -gau[k]*im.m[I1][j] + gau[k]*im.m[I2][j];
	      }else{
		    I1 = (j+k)%nc; 
			I2 = (j-k+nc)%nc;
		    x += -gau[k]*im.m[i][I1] + gau[k]*im.m[i][I2];
	      }
	    }
	    sm.m[i][j] = x;
	  }
}





void nonmax_suppress (f2D& dx, f2D& dy, int nr, int nc,
	IMAGE& mag, IMAGE& ori){
	int i,j,k,n,m;
	int top, bottom, left, right;
	float xx, yy, g2, g1, g3, g4, g, xc, yc;

	for (i=1; i<mag.nr-1; i++){
	  for (j=1; j<mag.nc-1; j++){
	    mag.m[i][j] = 0;
/* Treat the x and y derivatives as components of a vector */
	    xc = dx.m[i][j];
	    yc = dy.m[i][j];
	    if (fabs(xc)<0.01 && fabs(yc)<0.01) continue;
	    g  = norm (xc, yc);

/* Follow the gradient direction, as indicated by the direction of
   the vector (xc, yc); retain pixels that are a local maximum. */

	    if (fabs(yc) > fabs(xc)){
/* The Y component is biggest, so gradient direction is basically UP/DOWN */
	      xx = fabs(xc)/fabs(yc);
	      yy = 1.0;

	      g2 = norm (dx.m[i-1][j], dy.m[i-1][j]);
	      g4 = norm (dx.m[i+1][j], dy.m[i+1][j]);
	      if (xc*yc > 0.0){
		    g3 = norm (dx.m[i+1][j+1], dy.m[i+1][j+1]);
		    g1 = norm (dx.m[i-1][j-1], dy.m[i-1][j-1]);
	      }else{
		      g3 = norm (dx.m[i+1][j-1], dy.m[i+1][j-1]);
		      g1 = norm (dx.m[i-1][j+1], dy.m[i-1][j+1]);
	      }

	    }else{
/* The X component is biggest, so gradient direction is basically LEFT/RIGHT */
	      xx = fabs(yc)/fabs(xc);
	      yy = 1.0;

	      g2 = norm (dx.m[i][j+1], dy.m[i][j+1]);
	      g4 = norm (dx.m[i][j-1], dy.m[i][j-1]);
	      if (xc*yc > 0.0){
		    g3 = norm (dx.m[i-1][j-1], dy.m[i-1][j-1]);
		    g1 = norm (dx.m[i+1][j+1], dy.m[i+1][j+1]);
	      }
	      else {
		     g1 = norm (dx.m[i-1][j+1], dy.m[i-1][j+1]);
		     g3 = norm (dx.m[i+1][j-1], dy.m[i+1][j-1]);
	      }
	    }

/* Compute the interpolated value of the gradient magnitude */
	    if ( (g > (xx*g1 + (yy-xx)*g2)) && (g > (xx*g3 + (yy-xx)*g4)) ){
	      if (g*MAG_SCALE <= 255)
		    mag.m[i][j] = (unsigned char)(g*MAG_SCALE);
	      else
		    mag.m[i][j] = 255;
	        ori.m[i][j] = atan2 (yc, xc) * ORI_SCALE;
	    }else{
		   mag.m[i][j] = 0;
		   ori.m[i][j] = 0;
	    }

	  }
	}
}


// Test function for canny edge detection

void CannyEdgeDetection()
{
	int i,j,k,n;
	IMAGE im, magim, oriim;
	cout<<"CANNY: Apply the Canny edge detector to an image.\n";

// 3 adjustable parameters : Lower threshold(low), High threshold(high), Gaussian standard deviation(sigma)
// (1) Defaut value for Lower threshold, High threshold, Gaussian standard deviation
//	float s=1.0;
//	int low= 0,high=-1;
// Test for changing  threshold
	low=50;
	high=-5;
    sigma=1.0;
//  Read the image from file 
	ifstream in1("R_edge.raw",ios::binary);
	im.Initialize(919,1687);
	for(int i=0;i<im.nr;i++)for(int j=0;j<im.nc;j++)im.m[i][j]=in1.get();

// Create local image space 
	magim.Initialize(im.nr, im.nc);
	oriim.Initialize(im.nr, im.nc);

// Apply the filter
	cout<<"canny edge detection...\n";
	canny(sigma, im, magim, oriim);

// Hysteresis thresholding of edge pixels 
	cout<<"Hysteresis thresholding\n";
	hysteresis (high, low, im, magim, oriim);

// Compensation of edge region of image
	for (i=0; i<WIDTH; i++) for (j=0; j<im.nc; j++) im.m[i][j] = 255;
	for (i=im.nr-1; i>im.nr-1-WIDTH; i--) for (j=0; j<im.nc; j++) im.m[i][j] = 255;
	for (i=0; i<im.nr; i++) for (j=0; j<WIDTH; j++) im.m[i][j] = 255;
	for (i=0; i<im.nr; i++) for (j=im.nc-WIDTH-1; j<im.nc; j++) im.m[i][j] = 255;

// output the image to file
	ofstream out("canny_edge.raw");
	for(int i=0;i<im.nr;i++)for(int j=0;j<im.nc;j++)out<<im.m[i][j];
	char key;cin>>key;
}


