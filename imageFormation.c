#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "imageFormationUtils.h"

int COLS = 100;
int ROWS = 100;
float FOCAL_LENGTH = 1;

void calculatePinholeProjection(struct point3d *point3d, int N, float focalLength) {
  float zPortion;
  for (int i = 0; i < N; i++) {
    zPortion = (1 + (point3d[i].z / focalLength));
    point3d[i].x = (point3d[i].x / zPortion);
    point3d[i].y = (point3d[i].y / zPortion);
  }
}

void calculateUVParameters(struct point3d *point3d, int N, float *alpha_u, float *alpha_v, float *u_0, float *v_0) {
  float xMin = FLT_MAX;
  float yMin = FLT_MAX;
  float xMax = FLT_MIN;
  float yMax = FLT_MIN;

  for (int i = 0; i < N; i++) {
    if (point3d[i].x < xMin) {
      xMin = point3d[i].x;
    }
    if (point3d[i].x > xMax) {
      xMax = point3d[i].x;
    }
    if (point3d[i].y < yMin) {
      yMin = point3d[i].y;
    }
    if (point3d[i].y > yMax) {
      yMax = point3d[i].y;
    }
  }

  *alpha_u = (xMax - xMin) / COLS;
  *alpha_v = (yMax - yMin) / ROWS;
  *u_0 = xMin;
  *v_0 = yMin;
}

point2d* uvProjection(struct point3d *point3d, int N) {
  point2d *imagePoints = (point2d*) malloc(N * sizeof(point2d));

  float *alpha_u = (float*) malloc(sizeof(float));
  float *alpha_v = (float*) malloc(sizeof(float));
  float *u_0 = (float*) malloc(sizeof(float));
  float *v_0 = (float*) malloc(sizeof(float));

  calculateUVParameters(point3d, N, alpha_u, alpha_v, u_0, v_0);
  printf("UV parameters (alpha_u, alpha_v): (%f,%f)\n", *alpha_u, *alpha_v);
  printf("UV parameters (u_0, v_0): (%f,%f)\n", *u_0, *v_0);
  for (int i = 0; i < N; i++) {
    imagePoints[i].x = (int) (((point3d[i].x - *u_0) / *alpha_u));
    imagePoints[i].y = (int) (((point3d[i].y - *v_0) / *alpha_v));
    if (imagePoints[i].x >= COLS) {
      imagePoints[i].x = COLS - 1;
    }
    if (imagePoints[i].y >= ROWS) {
      imagePoints[i].y = ROWS - 1;
    }
    imagePoints[i].r = point3d[i].r;
    imagePoints[i].g = point3d[i].g;
    imagePoints[i].b = point3d[i].b;
  }
  return imagePoints;
}

int pointSmallerCmp(point2d point, point2d cmp) {
  if (point.y < cmp.y) {
    return 1;
  }
  if (point.y == cmp.y) {
    if (point.x < cmp.x) {
      return 1;
    }
  }
  return 0;
}

void sort(point2d* list, int N) {
    int i, j;
    point2d tmp;
    for (i = 0; i < N; i++) {
      for (j = i + 1; j < N; j++) {
        if (pointSmallerCmp(list[j], list[i])) {
          tmp = list[i];
          list[i] = list[j];
          list[j] = tmp;
        }
      }
    }
}

void printImageFileHeader(FILE *fp, int maxval) {
  fprintf(fp, "P3\n");
  fprintf(fp, "# Created by Manuel, Irman\n");
  fprintf(fp, "%d %d\n", COLS, ROWS);
  fprintf(fp, "%d\n", maxval);
}

void printImage(const char *filename, point2d *imagePoints, int N) {
  FILE *fp;
  fp = fopen(filename, "wb");
  if (!fp) {
       fprintf(stderr, "Unable to open file '%s'\n", filename);
       exit(1);
  }

  printImageFileHeader(fp, 255);
  int imageCounter = 0;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      if (x == imagePoints[imageCounter].x && y == imagePoints[imageCounter].y) {
        fprintf(fp, "%d %d %d ", imagePoints[imageCounter].r, imagePoints[imageCounter].g, imagePoints[imageCounter].b);
        while (x == imagePoints[imageCounter].x && y == imagePoints[imageCounter].y) {
          imageCounter++;
        }
      }
      else {
        fprintf(fp, "0 0 0 ");
      }
    }
  }
  printf("Number of mapped 3d points into 2d: %d\n", imageCounter);
  fclose(fp);
}


int main(int argc, char* argv[]) {
  struct point3d *points3d;
  int N_v = 0;
  char *inputFile = argv[1];
  char *outputFile = argv[2];
  points3d = readOff(inputFile, &N_v);
  centerThePCL(points3d, N_v);
  point2d *imagePoints;

  calculatePinholeProjection(points3d, N_v, FOCAL_LENGTH);
  printf("Pinhole projection done. \n");

  imagePoints = uvProjection(points3d, N_v);
  printf("UV projection done. \n");

  sort(imagePoints, N_v);
  printf("Image points sorted.\n");

  printImage(outputFile, imagePoints, N_v);
  printf("Printing done. \n");

  free(points3d);
  free(imagePoints);
}
