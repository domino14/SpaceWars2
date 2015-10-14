#include "gfx.h"
#include <objbase.h>
#include <iostream.h>
#include <ddraw.h>
#include <dinput.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <fstream.h>
#include <mmsystem.h>

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
 
#include <math.h>
#include <io.h>
#include <fcntl.h>
LPDIRECTDRAW lpdd;

int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char *filename)
{
// this function opens a bitmap file and loads the data into bitmap

int file_handle,  // the file handle
    index;        // looping index


UCHAR   *temp_buffer = NULL; // used to convert 24 bit images to 16 bit
OFSTRUCT file_data;          // the file data information

// open the file if it exists
if ((file_handle = OpenFile(filename,&file_data,OF_READ))==-1)
   return(0);


// now load the bitmap file header
_lread(file_handle, &bitmap->bitmapfileheader,sizeof(BITMAPFILEHEADER));

// test if this is a bitmap file
if (bitmap->bitmapfileheader.bfType!=0x4D42)
   {
   // close the file
   _lclose(file_handle);

   // return error
   return(0);

   } // end if

// now we know this is a bitmap, so read in all the sections

// first the bitmap infoheader

// now load the bitmap file header
_lread(file_handle, &bitmap->bitmapinfoheader,sizeof(BITMAPINFOHEADER));

// now load the color palette if there is one
if (bitmap->bitmapinfoheader.biBitCount == 8)
   {
   _lread(file_handle, &bitmap->palette,256*sizeof(PALETTEENTRY));

   // now set all the flags in the palette correctly and fix the reversed 
   // BGR RGBQUAD data format
   for (index=0; index < 256; index++)
       {
       // reverse the red and green fields
       int temp_color                = bitmap->palette[index].peRed;
       bitmap->palette[index].peRed  = bitmap->palette[index].peBlue;
       bitmap->palette[index].peBlue = temp_color;
       
       // always set the flags word to this
       bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
       } // end for index

    } // end if

// finally the image data itself
_lseek(file_handle,-(int)(bitmap->bitmapinfoheader.biSizeImage),SEEK_END);

// now read in the image, if the image is 8 or 16 bit then simply read it
// but if its 24 bit then read it into a temporary area and then convert
// it to a 16 bit image

if (bitmap->bitmapinfoheader.biBitCount==8 || bitmap->bitmapinfoheader.biBitCount==16 || 
    bitmap->bitmapinfoheader.biBitCount==24)
   {
   // delete the last image if there was one
   if (bitmap->buffer)
       free(bitmap->buffer);

   // allocate the memory for the image
   if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
      {
      // close the file
      _lclose(file_handle);

      // return error
      return(0);
      } // end if

   // now read it in
   _lread(file_handle,bitmap->buffer,bitmap->bitmapinfoheader.biSizeImage);

   } // end if
else
   {
   // serious problem
   return(0);

   } // end else

#if 0
// write the file info out 
printf("\nfilename:%s \nsize=%d \nwidth=%d \nheight=%d \nbitsperpixel=%d \ncolors=%d \nimpcolors=%d",
        filename,
        bitmap->bitmapinfoheader.biSizeImage,
        bitmap->bitmapinfoheader.biWidth,
        bitmap->bitmapinfoheader.biHeight,
		bitmap->bitmapinfoheader.biBitCount,
        bitmap->bitmapinfoheader.biClrUsed,
        bitmap->bitmapinfoheader.biClrImportant);

#endif

// close the file
_lclose(file_handle);

// flip the bitmap
Flip_Bitmap(bitmap->buffer, 
            bitmap->bitmapinfoheader.biWidth*(bitmap->bitmapinfoheader.biBitCount/8), 
            bitmap->bitmapinfoheader.biHeight);

// return success

return(1);

} // end Load_Bitmap_File

///////////////////////////////////////////////////////////

int Unload_Bitmap_File(BITMAP_FILE_PTR bitmap)
{
// this function releases all memory associated with "bitmap"
if (bitmap->buffer)
   {
   // release memory
   free(bitmap->buffer);

   // reset pointer
   bitmap->buffer = NULL;

   } // end if

// return success
return(1);

} // end Unload_Bitmap_File

///////////////////////////////////////////////////////////

int Flip_Bitmap(UCHAR *image, int bytes_per_line, int height)
{
// this function is used to flip bottom-up .BMP images

UCHAR *buffer; // used to perform the image processing
int index;     // looping index

// allocate the temporary buffer
if (!(buffer = (UCHAR *)malloc(bytes_per_line*height)))
   return(0);

// copy image to work area
memcpy(buffer,image,bytes_per_line*height);

// flip vertically
for (index=0; index < height; index++)
    memcpy(&image[((height-1) - index)*bytes_per_line],
           &buffer[index*bytes_per_line], bytes_per_line);

// release the memory
free(buffer);

// return success
return(1);

} // end Flip_Bitmap

int Create_BOB(BITMAP_OBJ_PTR bob, int width, int height, int attr, int flags)
{
	DDSURFACEDESC ddsd;
	bob->state = BOB_STATE_ALIVE;
	bob->attr = attr;
	bob->image = NULL;
	bob->x = bob->y = bob->xv = bob->yv = 0;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = bob->width = width;
	ddsd.dwHeight = bob->height = height;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | flags;
	if (lpdd->CreateSurface(&ddsd, &(bob->image), NULL)!=DD_OK)
		return (0);
	DDCOLORKEY color_key;
	color_key.dwColorSpaceHighValue = 0;
	color_key.dwColorSpaceLowValue = 0;
	(bob->image)->SetColorKey(DDCKEY_SRCBLT, &color_key);

	return (1);
}

int Load_BOB(BITMAP_OBJ_PTR bob, BITMAP_FILE_PTR bitmap,
			 int cx, int cy, int mode)
{
	UCHAR *source_ptr, *dest_ptr;
	DDSURFACEDESC ddsd;
	if (mode == 0)
	{
		cx = cx *(bob->width + 1) + 1;
		cy = cy *(bob->height + 1) + 1;
	} 
	source_ptr = bitmap->buffer + 
		cy*bitmap->bitmapinfoheader.biWidth + cx;
	ddsd.dwSize = sizeof(ddsd);

	(bob->image)->Lock(NULL, &ddsd, 
		DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
		NULL);
	dest_ptr = (UCHAR *)ddsd.lpSurface;
	for (int index_y = 0; index_y< bob->height; index_y++)
	{
		memcpy(dest_ptr, source_ptr, bob->width);
		dest_ptr += bob->width;
		
		source_ptr += bitmap->bitmapinfoheader.biWidth;
	}

	(bob->image)->Unlock(ddsd.lpSurface);
	bob->state |= BOB_STATE_LOADED;
	return 1;
}

int Draw_BOB(BITMAP_OBJ_PTR bob, LPDIRECTDRAWSURFACE dest)
{
	RECT dest_rect, source_rect;
	dest_rect.left = bob->x;
	dest_rect.top = bob->y;
	dest_rect.right = bob->x + bob->width;
	dest_rect.bottom = bob->y + bob->height;
	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = bob->width ;
	source_rect.bottom = bob->height;
	dest->Blt(&dest_rect, bob->image, &source_rect, 
		(DDBLT_WAIT | DDBLT_KEYSRC), NULL);
	return 1;
}

int Destroy_BOB(BITMAP_OBJ_PTR bob)
{
	if (bob->image) (bob->image)->Release();
	else return (0);
	return (1);
}
