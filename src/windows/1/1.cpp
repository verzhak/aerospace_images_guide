// 1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cstdio>
#include <string>

#include <gdal_priv.h>
#include <cpl_conv.h>

using namespace std;

#define throw_if(condition) \
if((condition))\
{\
	fprintf(stderr, "[Exception] File %s, line %d\n", __FILE__, __LINE__);\
	throw 1;\
};

#define throw_null(pointer) throw_if((pointer) == NULL)

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char * read_buf = NULL, * write_buf = NULL, ch[3] = {'2', '3', '4'};
	int band, ret = 0;
	unsigned v, u, t, height, width, from_x = 2000, from_y = 2000, x_size = 1000, y_size = 1000;
	double transform_coef[6];
	string src_fname, base_src_fname = "C:/met/gis/LT51760222010175MOR00/L5176022_02220100624_B";
	string dst_fname = "C:/met/gis/res/1.tif";
	GDALDriver * drv = NULL;
	GDALDataset  * src[3] = {NULL, NULL, NULL}, * dst = NULL;

	try
	{
		GDALAllRegister();

		throw_null((drv = GetGDALDriverManager()->GetDriverByName("GTiff")));

		for(v = 0; v < 3; v++)
		{
			src_fname = ch[v];
			src_fname = base_src_fname + src_fname + "0.tif";

			throw_null((src[v] = (GDALDataset *) GDALOpen(src_fname.c_str(), GA_ReadOnly)));
		}

		height = src[0]->GetRasterYSize();
		width = src[0]->GetRasterXSize();

		throw_null((read_buf = new unsigned char[height * width]));
		throw_null((write_buf = new unsigned char[x_size * y_size]));
		throw_null((dst = drv->Create(dst_fname.c_str(), x_size, y_size, 3, GDT_Byte, NULL)));

		for(v = 0; v < 3; v++)
		{
			band = v + 1;

			throw_if((src[v]->RasterIO(GF_Read, 0, 0, width, height, read_buf, width, height, GDT_Byte, 1, NULL, 0, 0, 0) != CE_None));

			for(u = 0; u < y_size; u++)
				for(t = 0; t < x_size; t++)
					write_buf[u * x_size + t] = read_buf[(u + from_y) * width + (t + from_x)];

			throw_if((dst->RasterIO(GF_Write, 0, 0, x_size, y_size, write_buf, x_size, y_size, GDT_Byte, 1, & band, 0, 0, 0) != CE_None));
		}
		
		src[0]->GetGeoTransform(transform_coef);
		
		transform_coef[0] += from_x * transform_coef[1] + from_y * transform_coef[2];
		transform_coef[3] += from_x * transform_coef[4] + from_y * transform_coef[5];
		
		dst->SetProjection(src[0]->GetProjectionRef());
		dst->SetGeoTransform(transform_coef);
	}
	catch(...)
	{
		ret = -1;
	}

	if(read_buf != NULL)
		delete read_buf;

	if(write_buf != NULL)
		delete write_buf;

	for(v = 0; v < 3; v++)
		if(src[v] != NULL)
			GDALClose(src[v]);

	if(dst != NULL)
		GDALClose(dst);

	return ret;
}

