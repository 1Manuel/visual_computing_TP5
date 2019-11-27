#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "imageFormationUtils.h"

struct point2d* calculatePinholeProjection(struct point3d *point3d, int N, float focalLength) {
  point2d *imagePoints = (point2d*) malloc(N * sizeof(point2d));
  float zPortion;
  for (int i = 0; i < N; i++) {
    zPortion = (1 + (point3d[i].z / focalLength));
    imagePoints[i].x = (point3d[i].x / zPortion);
    imagePoints[i].y = (point3d[i].y / zPortion);
    imagePoints[i].r = point3d[i].r;
    imagePoints[i].g = point3d[i].g;
    imagePoints[i].b = point3d[i].b;
  }
  return imagePoints;
}

void calculateUVParameters(point2d *imagePoints, int N, float *alpha_u, float *alpha_v, Coordinate *imageOrigin) {
  xMin = INT_MAX;
  yMin = INT_MAX;
  xMax = INT_MIN;
  yMax = INT_MIN;

  for (int i = 0; i < N; i++) {
    if (imagePoints[i].x < xMin) {
      xMin = imagePoints[i].x;
    }
    if (imagePoints[i].x > xMax) {
      xMax = imagePoints[i].x;
    }
    if (imagePoints[i].y < yMin) {
      yMin = imagePoints[i].y;
    }
    if (imagePoints[i].y > yMax) {
      yMax = imagePoints[i].y;
    }
  }
  *alpha_u = 1 / (xMax - xMin);
  *alpha_v = 1 / (yMax - yMin);
  imageOrigin->x = abs(xMins);
  imageOrigin->y = abs(yMin);
}

void uvProjection(point2d *imagePoints, int N) {
  float *alpha_u;
  float *alpha_v;
  Coordinate *imageOrigin = (Coordinate*) malloc(sizeof(Coordinate));
  calculateUVParameters(imagePoints, N, alpha_u, alpha_v, imageOrigin);
  for (int i = 0; i < N; i++) {
    imagePoints[i].x = ((imagePoints[i].x / *alpha_u) + imageOrigin->x);
    imagePoints[i].y = ((imagePoints[i].y / *alpha_v) + imageOrigin->y);
  }
}

void printImageFileHeader(FILE *fp, int cols, int rows, int maxval) {
  fprintf(fp, "P3\n");
  fprintf(fp, "# Created by Manuel, Irman\n");
  fprintf(fp, "%d %d\n", cols, rows);
  fprintf(fp, "%d\n", maxval);
}

void printImage(const char *filename, point2d *imagePoints, int N) {
  FILE *fp;
  int cols = ceil(sqrt(N));
  int rows = floor(sqrt(N));

  fp = fopen(filename, "wb");
  if (!fp) {
       fprintf(stderr, "Unable to open file '%s'\n", filename);
       exit(1);
  }

  printImageFileHeader(fp, cols, rows, 255);
  for (int i = 0; i < N; i++) {
    printf("(%f,%f): (%d,%d,%d)\n", imagePoints[i].x, imagePoints[i].y, imagePoints[i].r, imagePoints[i].g, imagePoints[i].b);
    // for (int j = 0; j < N; j++) {
    //   if (i == (imagePoints[j].x + (imagePoints[j].y * cols))) {
    //     fprintf(fp, "%d %d %d ", imagePoints[j].r, imagePoints[j].g, imagePoints[j].b);
    //   }
    // }
  }
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
  imagePoints = calculatePinholeProjection(points3d, N_v, 0.01);

  uvProjection(imagePoints, N_v);

  printImage(outputFile, imagePoints, N_v);

  free(imageOrigin);
  free(points3d);
  free(imagePoints);
}
