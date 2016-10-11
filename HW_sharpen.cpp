#include "HW_blur.cpp"
extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D (ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

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
	//	int xBuff = xsv / 2;
	//	int yBuff = ysv / 2;
	//if statements
	ChannelPtr<uchar> p1, p2, temp, endd;


	//If the pixel is both one then the values is output directly
	if (size == 1){
		int type;
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (endd = p1 + total; p1 < endd;) *p2++ = *p1++;
		}
		return;
	}

	//Blur the image row by row as the kernal specifies if it is greater than one and save
	//in the temporary channel pointer
	int type;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		if (size > 1){
			IP_getChannel(tempPic, ch, temp, type);
			for (int height = 0; height < h; height++){
				HW_blur1D(p1, size, 1, w, temp);
				p1 += w;
				temp += w;
			}
		}
		else {
			IP_copyImage(I1, tempPic);
		}
		IP_getChannel(tempPic, ch, temp, type);

		//Blur the image column by column using the tempory pointer and save the changes into the output pointer
		if (size > 1){
			IP_getChannel(I2, ch, p2, type);
			for (int x = 0; x < w; x++){
				HW_blur1D(temp, size, w, h, p2);
				temp += 1;
				p2 += 1;
			}
		}
		else{
			IP_copyImage(tempPic, I2);
		}

		//If the factor is greater than 1 subtract the blurred image from the orginal to smooth the 
		//edges of the image and multiply it by a factor. Then add the orginal image back to see the
		//new smoothed image
		if (factor > 1){
			IP_getChannel(I1, ch, p1, type);
			IP_getChannel(I2, ch, p2, type);
			for (endd = p1 + total; p1 < endd; p1++) {
				*p2 = CLIP((((*p1 - *p2)* factor) + *p1), 0, MXGRAY - 1);
				*p2++;
			}
		}
		else{
			IP_copyImage(tempPic, I2);
		}
	}
}
