extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
	//copy header and instantiate the width, hieght, total, and the size of x and y
	ImagePtr tempPic;
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I1, tempPic);
	int w = I1->width();
	int h = I2->height();
	int total = w * h;
	ChannelPtr<uchar> p1, p2, endd;
	HW_blur(I1, size, size, I2);

	//Blur the image row by row as the kernal specifies if it is greater than one and save
	//in the temporary channel pointer
	int type, val;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {

		//If the factor is greater than 1 subtract the blurred image from the orginal to smooth the 
		//edges of the image and multiply it by a factor. Then add the orginal image back to see the
		//new smoothed image
		IP_getChannel(I2, ch, p2, type);
		for (endd = p1 + total; p1 < endd; p1++, p2++) {
			val = ((*p1 - *p2)* factor) + *p1;
			*p2 = CLIP(val, 0, MaxGray);
		}
	}
}
