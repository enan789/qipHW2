void blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_blur:
//
// Blur image I1 with a box filter (unweighted averaging).
// The filter has width filterW and height filterH.
// We force the kernel dimensions to be odd.
// Output is in I2.
//
void
HW_blur(ImagePtr I1, int filterW, int filterH, ImagePtr I2)
{
	//copy header and instantiate the width, hieght, total, and the size of x and y
	ImagePtr tempPic;
	IP_copyImageHeader(I1, I2);
	IP_copyImageHeader(I2, tempPic);
	int w = I1->width();
	int h = I1->height();
	int total = w * h;
	//if statements
	ChannelPtr<uchar> p1, p2, temp, endd;

	//If the pixel is both one then the values is output directly
	if (filterW == 1 && filterH == 1){
		int type;
		for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
			IP_getChannel(I2, ch, p2, type);
			for (endd = p1 + total; p1<endd;) *p2++ = *p1++;
		}
		return;
	}
	//qDebug() << filterW << endl;

	//Blur the image row by row as the kernal specifies if it is greater than one and save
	//in the temporary channel pointer
	int type;
	for (int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		if (filterW > 1){
			IP_getChannel(tempPic, ch, temp, type);
			for (int height = 0; height < h; height++){
				blur1D(p1, 1, w, filterW, temp);
				p1 += w;
				temp += w;
			}
		}
		else {
			IP_copyImage(I1, tempPic);
		}
		IP_getChannel(tempPic, ch, temp, type);

		//Blur the image column by column usI1g the tempory pointer and save the changes into the output pointer
		if (filterH > 1){
			IP_getChannel(I2, ch, p2, type);
			for (int x = 0; x < w; x++){
				blur1D(temp, w, h, filterH, p2);
				temp += 1;
				p2 += 1;
			}
		}
		else{
			IP_copyImage(tempPic, I2);
		}
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur1D:
//
// Blur src with 1D box filter of width ww.
//
void
blur1D(ChannelPtr<uchar> src, int len, int stride, int ww, ChannelPtr<uchar> dst)
{
	//declare needed variables including values holding the sum, the size of the orginal image,
	//the value of the current midpoin,t and a record of the start of the current row/column
	int sum = 0;

	//Create Buffer for the column or row by first declareing an array of that size.
	int buffer_size = stride + (ww);
	uchar * Buffer = new uchar[buffer_size];
	uchar * temp = Buffer;

	//Loop through the row or column and place buffer/2 pixels of copied pixels on
	//both side of the line with the actual line in the middle
	for (int i = 0; i < stride; i++){
		for (int i2 = 0; i2 < (ww / 2); i2++){
			*temp = *src;
			//qDebug() << i << " " << ww << " " << *temp << endl;
			temp++;
		}
		for (; i < stride; i++){
			*temp = *src;
			src += len;
			temp++;
		}
		for (int i3 = 0; i3 < ww / 2; i3++){
			*temp = *src;
			temp++;
		}
		//qDebug() << i << " " << stride << endl;
	}

	//Find the initial sum of the first batch of buffer size pixels and save the average
	for (int i2 = 0; i2 < ww; i2++){
		sum += Buffer[i2];
	}
	*dst = sum / ww;

	//Loop over the array the same size of the original image
	for (int i = 0; i < stride; i++){
		//To save time add the next pixel's value in the array to the sum while subtracting the earliest
		sum -= Buffer[i];
		sum += Buffer[i + ww];

		//calculate the next value
		*dst = sum / ww;
		//Increment the by the len to get to the next pixel
		dst += len;
	}
	//clean up the sum and delete the buffer
	sum = 0;
	delete[] Buffer;
}
