#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "png.h"
using namespace std;

typedef struct _pic_data pic_data;  
struct _pic_data  
{  
	int width, height;  /* �ߴ� */  
	int bit_depth;      /* λ�� */
	int flag;           /* һ����־����ʾ�Ƿ���alphaͨ�� */
	int channels;
	unsigned char   **rgba; /* ͼƬ���� */  
};

#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA  1  
#define NO_ALPHA    0

int detect_png(char *filepath, pic_data *out){
	/* ���ڽ���pngͼƬ */
	/* ��ͼƬһ�ɴ�ΪRGB+alphaͨ��*/
	FILE *pic_fp;  
	pic_fp = fopen(filepath, "rb");  
	if(pic_fp == NULL) /* �ļ���ʧ�� */
		return -1;

	/* ��ʼ�����ֽṹ */
	png_structp png_ptr;  
	png_infop   info_ptr;  
	png_byte        buf[PNG_BYTES_TO_CHECK];  
	int         temp;  

	png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);  
	info_ptr = png_create_info_struct(png_ptr);  

	setjmp(png_jmpbuf(png_ptr)); // ������Ҫ

	temp = fread(buf,1,PNG_BYTES_TO_CHECK,pic_fp);  
	temp = png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK);

	/*����Ƿ�Ϊpng�ļ�*/  
	if (temp!=0) return 1;

	rewind(pic_fp);  
	/*��ʼ���ļ�*/  
	png_init_io(png_ptr, pic_fp);  
	// ���ļ���  
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

	int color_type,channels;  

	/*��ȡ��ȣ��߶ȣ�λ���ɫ����*/  
	out->channels  = png_get_channels(png_ptr, info_ptr);   /*��ȡͨ����*/  
	out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);   /* ��ȡλ�� */  
	color_type     = png_get_color_type(png_ptr, info_ptr); /*��ɫ����*/

	int i,j;  
	int size, pos = 0;
	/* row_pointers��߾���rgba���� */  
	png_bytep* row_pointers;  
	row_pointers = png_get_rows(png_ptr, info_ptr);  
	out->width = png_get_image_width(png_ptr, info_ptr);  
	out->height = png_get_image_height(png_ptr, info_ptr);

	size = out->width * out->height; /* ����ͼƬ�������ص����� */

	if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)  
	{/*�����RGB+alphaͨ��������RGB+�����ֽ�*/   
		size *= (sizeof(unsigned char));  /* ÿ�����ص�ռ4���ֽ��ڴ� */  
		out->flag = HAVE_ALPHA;              /* ��� */
		out->rgba = (unsigned char**) malloc(sizeof(unsigned char*)*4);
		for ( i=0; i<4; i++)
		{
			out->rgba[i] = (unsigned char*) malloc(size);
		}
		if(out->rgba == NULL)  
		{/* ��������ڴ�ʧ�� */  
			fclose(pic_fp);  
			puts("����(png):�޷������㹻���ڴ湩�洢����!");  
			return 1;  
		}  

		temp = (4 * out->width);/* ÿ����4 * out->width���ֽ� */  
		for(i = 0; i < out->height; i++)  
		{  
			for(j = 0; j < temp; j += 4)  
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */  
				out->rgba[0][pos] = row_pointers[i][j]; // red  
				out->rgba[1][pos] = row_pointers[i][j+1]; // green  
				out->rgba[2][pos] = row_pointers[i][j+2];   // blue  
				out->rgba[3][pos] = row_pointers[i][j+3]; // alpha  
				++pos;  
			}  
		}  
	}
	else if(channels == 3 || color_type == PNG_COLOR_TYPE_RGB)  
	{/* �����RGBͨ�� */
     /* �����ΪRGB+ALPHAͨ�� */
		size *= (sizeof(unsigned char));  /* ÿ�����ص�ռ4���ֽ��ڴ� */  
		out->flag = HAVE_ALPHA;              /* ��� */
		out->rgba = (unsigned char**) malloc(sizeof(unsigned char*)*4);
		for ( i=0; i<4; i++)
		{
			out->rgba[i] = (unsigned char*) malloc(size);
		}
		if(out->rgba == NULL)  
		{/* ��������ڴ�ʧ�� */  
			fclose(pic_fp);  
			puts("����(png):�޷������㹻���ڴ湩�洢����!");  
			return 1;  
		}  

		temp = (3 * out->width);/* ÿ����4 * out->width���ֽ� */  
		for(i = 0; i < out->height; i++)  
		{  
			for(j = 0; j < temp; j += 3)  
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */  
				out->rgba[0][pos] = row_pointers[i][j]; // red  
				out->rgba[1][pos] = row_pointers[i][j+1]; // green  
				out->rgba[2][pos] = row_pointers[i][j+2];   // blue
				out->rgba[3][pos] = 255; // alpha  
				++pos;  
			}  
		}  
	}
	else if(channels == 2 || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)  
	{/* �����gray+alphaͨ�� */
		/* �����ΪRGB+ALPHAͨ�� */
		size *= (sizeof(unsigned char));  /* ÿ�����ص�ռ4���ֽ��ڴ� */  
		out->flag = HAVE_ALPHA;              /* ��� */
		out->rgba = (unsigned char**) malloc(sizeof(unsigned char*)*4);
		for ( i=0; i<4; i++)
		{
			out->rgba[i] = (unsigned char*) malloc(size);
		}
		if(out->rgba == NULL)  
		{/* ��������ڴ�ʧ�� */  
			fclose(pic_fp);  
			puts("����(png):�޷������㹻���ڴ湩�洢����!");  
			return 1;  
		}  

		temp = (2 * out->width);/* ÿ����4 * out->width���ֽ� */
		for(i = 0; i < out->height; i++)  
		{  
			for(j = 0; j < temp; j += 2)  
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */  
				out->rgba[0][pos] = row_pointers[i][j]; // red  
				out->rgba[1][pos] = row_pointers[i][j]; // green  
				out->rgba[2][pos] = row_pointers[i][j];   // blue
				out->rgba[3][pos] = row_pointers[i][j+1]; // alpha  
				++pos;  
			}  
		}
	}
	else if(channels == 1 || color_type == PNG_COLOR_TYPE_GRAY)  
	{/* �����grayͨ�� */
		/* �����ΪRGB+ALPHAͨ�� */
		size *= (sizeof(unsigned char));  /* ÿ�����ص�ռ4���ֽ��ڴ� */  
		out->flag = HAVE_ALPHA;              /* ��� */
		out->rgba = (unsigned char**) malloc(sizeof(unsigned char*)*4);
		for ( i=0; i<4; i++)
		{
			out->rgba[i] = (unsigned char*) malloc(size);
		}
		if(out->rgba == NULL)  
		{/* ��������ڴ�ʧ�� */  
			fclose(pic_fp);  
			puts("����(png):�޷������㹻���ڴ湩�洢����!");  
			return 1;  
		}  

		temp = (1 * out->width);/* ÿ����4 * out->width���ֽ� */
		for(i = 0; i < out->height; i++)  
		{  
			for(j = 0; j < temp; j += 1)  
			{/* һ���ֽ�һ���ֽڵĸ�ֵ */  
				out->rgba[0][pos] = row_pointers[i][j]; // red  
				out->rgba[1][pos] = row_pointers[i][j]; // green  
				out->rgba[2][pos] = row_pointers[i][j];   // blue
				out->rgba[3][pos] = 255; // alpha  
				++pos;  
			}  
		}
	}
	else return 1;

	/* ��������ռ�õ��ڴ� */  
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);   
	return 0;  

}

int write_png_file(char *file_name , pic_data *graph)  
/* ��puc_data�ṹ�е�����д����png�ļ� */
/* �Ὣgraph���ڴ������� */
{  
	int j, i, temp, pos;  
	png_byte color_type;   

	png_structp png_ptr;  
	png_infop info_ptr;   
	png_bytep * row_pointers;  
	/* create file */  
	FILE *fp = fopen(file_name, "wb");  
	if (!fp)  
	{  
		printf("[write_png_file] File %s could not be opened for writing", file_name);  
		return -1;  
	}  


	/* initialize stuff */  
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  

	if (!png_ptr)  
	{  
		printf("[write_png_file] png_create_write_struct failed");  
		return -1;  
	}  
	info_ptr = png_create_info_struct(png_ptr);  
	if (!info_ptr)  
	{  
		printf("[write_png_file] png_create_info_struct failed");  
		return -1;  
	}  
	if (setjmp(png_jmpbuf(png_ptr)))  
	{  
		printf("[write_png_file] Error during init_io");  
		return -1;  
	}  
	png_init_io(png_ptr, fp);  


	/* write header */  
	if (setjmp(png_jmpbuf(png_ptr)))  
	{  
		printf("[write_png_file] Error during writing header");  
		return -1;  
	}  
	/* �ж�Ҫд�����ļ���ͼƬ�����Ƿ���͸���ȣ���ѡ��ɫ������ */  
	if(graph->flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;  
	else color_type = PNG_COLOR_TYPE_RGB;  

	png_set_IHDR(png_ptr, info_ptr, graph->width, graph->height,  
		graph->bit_depth, color_type, PNG_INTERLACE_NONE,  
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);  

	png_write_info(png_ptr, info_ptr);  


	/* write bytes */  
	if (setjmp(png_jmpbuf(png_ptr)))  
	{  
		printf("[write_png_file] Error during writing bytes");  
		return -1;  
	}  
	if(graph->flag == HAVE_ALPHA) temp = (4 * graph->width);  
	else temp = (3 * graph->width);  

	pos = 0;  
	row_pointers = (png_bytep*)malloc(graph->height*sizeof(png_bytep));  
	for(i = 0; i < graph->height; i++)  
	{  
		row_pointers[i] = (png_bytep)malloc(sizeof(unsigned char)*temp);  
		for(j = 0; j < temp; j += 4)  
		{  
			row_pointers[i][j]   = graph->rgba[0][pos]; // red  
			row_pointers[i][j+1] = graph->rgba[1][pos]; // green  
			row_pointers[i][j+2] = graph->rgba[2][pos];   // blue  
			if(graph->flag == HAVE_ALPHA)   
				row_pointers[i][j+3] = graph->rgba[3][pos]; // alpha  
			++pos;  
		}  
	}  
	png_write_image(png_ptr, row_pointers);  

	/* end write */  
	if (setjmp(png_jmpbuf(png_ptr)))  
	{  
		printf("[write_png_file] Error during end of write");  
		return -1;  
	}  
	png_write_end(png_ptr, NULL);  

	/* cleanup heap allocation */  
	for (j=0; j<graph->height; j++)  
		free(row_pointers[j]);
	free(row_pointers);
	for (i=0; i<4; i++)
	{
		free(graph->rgba[i]);
	}
	free(graph->rgba);

	fclose(fp);  
	return 0;  
}

int main(){
	cout<<"hello world!"<<endl;
	//myreadpng("F:/jz/sl.png");
	//myreadpng("F:/jz/rotate.png");
	//myreadpng("F:/jz/zr.png");
	pic_data a,b,c;
	detect_png("d:/jz/zr.png", &a);
	write_png_file("d:/jz/3.png", &a);

	detect_png("d:/jz/rotate.png", &c);
	write_png_file("d:/jz/2.png", &c);
	detect_png("d:/jz/sl.png", &b);
	write_png_file("d:/jz/1.png", &b);

	return 0;
}