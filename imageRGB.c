/// imageRGB - A simple image module for handling RGB images,
///            pixel color values are represented using a look-up table (LUT)
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
/// 2025

// Student authors (fill in below):
// NMec: 118799
// Name: Daniel Zamurca
// NMec: 119501
// Name: Tomás Coutinho
//
// Date: 07/11/2025
//

#include "imageRGB.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "PixelCoords.h"
#include "PixelCoordsQueue.h"
#include "PixelCoordsStack.h"
#include "instrumentation.h"

// The data structure
//
// A RGB image is stored in a structure containing 5 fields:
// Two integers store the image width and height.
// The next field is a pointer to an array that stores the pointers
// to the image rows.
//
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// FIXED SIZE of LUT for storing RGB triplets
#define FIXED_LUT_SIZE 1000

// Internal structure for storing RGB images
struct image {
  uint32 width;
  uint32 height;
  uint16** image;     // pointer to an array of pointers referencing the image rows
  uint16 num_colors;  // the number of colors (i.e., pixel labels) used
  rgb_t* LUT;         // table storing (R,G,B) triplets
};

// Design by Contract

// This module follows "design-by-contract" principles.
// `assert` is used to check function preconditions, postconditions
// and type invariants.
// This helps to find programmer errors.

/// Defensive Error Handling

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
//
// When one of these functions detects a memory or I/O error,
// it immediately prints an error message and aborts the program.
// This is a Fail-Fast strategy.
//
// You may use the `check` function to check a condition
// and exit the program with an error message if it is false.
// Note that it works similarly to `assert`, but cannot be disabled.
// It should be used to detect "external" uncontrolable errors,
// and not for "internal" programmer errors.
//
// See how it's used in ImageLoadPBM, for example.

// Check a condition and if false, print failmsg and exit.
static void check(int condition, const char* failmsg) {
  if (!condition) {
    perror(failmsg);
    exit(errno || 255);
  }
}

/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of number_labeled_pixelsers.
void ImageInit(void) {  ///
  InstrCalibrate();
  InstrName[0] = "pixmem";  // Instrnumber_labeled_pixels[0] will number_labeled_pixels pixel array acesses
  // Name other number_labeled_pixelsers here...
}

// Macros to simplify accessing instrumentation number_labeled_pixelsers:
#define PIXMEM Instrnumber_labeled_pixels[0]
// Add more macros here...

// TIP: Search for PIXMEM or Instrnumber_labeled_pixels to see where it is incremented!

/// Auxiliary (static) functions

static Image AllocateImageHeader(uint32 width, uint32 height) {
  // Create the header of an image data structure
  // Allocate the array of pointers to rows
  // And the look-up table

  Image newHeader = malloc(sizeof(struct image));
  // Error handling
  check(newHeader != NULL, "malloc");

  newHeader->width = width;
  newHeader->height = height;

  // Allocating the array of pointers to image rows
  newHeader->image = malloc(height * sizeof(uint16*));
  // Error handling
  check(newHeader->image != NULL, "Alloc failed ->image array");

  // Allocating the LUT
  newHeader->LUT = malloc(FIXED_LUT_SIZE * sizeof(rgb_t));
  // Error handling
  check(newHeader->LUT != NULL, "Alloc failed ->LUT array");

  // Initialize LUT with 2 fixed colors
  newHeader->num_colors = 2;
  newHeader->LUT[0] = 0xffffff;  // RGB WHITE
  newHeader->LUT[1] = 0x000000;  // RGB BLACK

  return newHeader;
}

// Allocate row of background (label=0) pixels
static uint16* AllocateRowArray(uint32 size) {
  uint16* newArray = calloc((size_t)size, sizeof(uint16));
  // Error handling
  check(newArray != NULL, "AllocateRowArray");

  return newArray;
}

/// Find color label for given RGB color in img LUT.
/// Return the label or -1 if not found.
static int LUTFindColor(Image img, rgb_t color) {
  for (uint16 index = 0; index < img->num_colors; index++) {
    if (img->LUT[index] == color) return index;
  }
  return -1;
}

/// Return color label for RGB color in img LUT.
/// Finds existing color or allocs new one!
static int LUTAllocColor(Image img, rgb_t color) {
  int index = LUTFindColor(img, color);
  if (index < 0) {
    check(img->num_colors < FIXED_LUT_SIZE, "LUT Overflow");
    index = img->num_colors++;
    img->LUT[index] = color;
  }
  return index;
}

/// Return a pseudo-random successor of the given color.
static rgb_t GenerateNextColor(rgb_t color) {
  return (color + 7639) & 0xffffff;
}

/// Image management functions

/// Create a new RGB image. All pixels with the background WHITE color.
///   width, height: the dimensions of the new image.
/// Requires: width and height must be non-negative.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreate(uint32 width, uint32 height) {
  assert(width > 0);
  assert(height > 0);

  // Just two possible pixel colors
  Image img = AllocateImageHeader(width, height);

  // Creating the image rows
  for (uint32 i = 0; i < height; i++) {
    img->image[i] = AllocateRowArray(width);  // Alloc all WHITE row
  }

  return img;
}

/// Create a new RGB image, with a color chess pattern.
/// The background is WHITE.
///   width, height: the dimensions of the new image.
///   edge: the width and height of a chess square.
///   color: the foreground color.
/// Requires: width, height and edge must be non-negative.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCreateChess(uint32 width, uint32 height, uint32 edge, rgb_t color) {
  assert(width > 0);
  assert(height > 0);
  assert(edge > 0);

  Image img = ImageCreate(width, height);

  // Alloc color in LUT.
  uint8 label = LUTAllocColor(img, color);

  // Assigning the color to each image pixel

  // Pixel (0, 0) gets the chosen color label
  for (uint32 i = 0; i < height; i++) {
    uint32 I = i / edge;
    for (uint32 j = 0; j < width; j++) {
      uint32 J = j / edge;
      img->image[i][j] = (I + J) % 2 ? 0 : label;
    }
  }
  // Return the created chess image
  return img;
}

/// Create an image with a palete of generated colors.
Image ImageCreatePalete(uint32 width, uint32 height, uint32 edge) {
  assert(width > 0);
  assert(height > 0);
  assert(edge > 0);

  Image img = ImageCreate(width, height);

  // Fill LUT with generated colors
  rgb_t color = 0x000000;
  while (img->num_colors < FIXED_LUT_SIZE) {
    color = GenerateNextColor(color);
    img->LUT[img->num_colors++] = color;
  }

  // number of tiles
  uint32 wtiles = width / edge;

  // Pixel (0, 0) gets the chosen color label
  for (uint32 i = 0; i < height; i++) {
    uint32 I = i / edge;
    for (uint32 j = 0; j < width; j++) {
      uint32 J = j / edge;
      img->image[i][j] = (I * wtiles + J) % FIXED_LUT_SIZE;
    }
  }

  return img;
}

/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
///
/// Ensures: (*imgp)==NULL.
void ImageDestroy(Image* imgp) {
  assert(imgp != NULL);

  Image img = *imgp;

  for (uint32 i = 0; i < img->height; i++) {
    free(img->image[i]);
  }
  free(img->image);
  free(img->LUT);
  free(img);

  *imgp = NULL;
}

/// Create a deep copy of the image pointed to by img.
///   img : address of an Image variable.
///
/// On success, a new copied image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageCopy(const Image img) { //! AUTHOR: DANIEL ZAMURCA
  assert(img != NULL);

  // cria uma nova imagem com a mesma altura e largura
  Image img_copy = ImageCreate(img->width,img->height); 

  // copia a variável num_colors da img para a img_copy
  img_copy->num_colors = img->num_colors;               

  // cópia dos indices da LUT
  for (uint16 i = 0; i < img->num_colors; i++) {
    img_copy->LUT[i] = img->LUT[i];
  } // tambem se poderia usar memcpy
  
  // copia todos os pixeis um a um da img para a img_copy
  for (uint32 i = 0; i < img->height; i++) {      
    for (uint32 j = 0; j < img->width; j++) {
      img_copy->image[i][j] = img->image[i][j];
    }
  } // tambem se poderia usar memcpy
  
  return img_copy;
}

/// Printing on the console

/// These functions do not modify the image and never fail.

/// Output the raw RGB image (i.e., print the integer value of pixel).
void ImageRAWPrint(const Image img) {
  printf("width = %d height = %d\n", (int)img->width, (int)img->height);
  printf("num_colors = %d\n", (int)img->num_colors);
  printf("RAW image\n");

  // Print the pixel labels of each image row
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      printf("%2d", img->image[i][j]);
    }
    // At current row end
    printf("\n");
  }

  printf("LUT:\n");
  // Print the LUT (R,G,B) values
  for (int i = 0; i < (int)img->num_colors; i++) {
    rgb_t color = img->LUT[i];
    int r = color >> 16 & 0xff;
    int g = color >> 8 & 0xff;
    int b = color & 0xff;
    printf("%3d -> (%3d,%3d,%3d)\n", i, r, g, b);
  }

  printf("\n");
}

/// PBM file operations --- For BW images

// See PBM format specification: http://netpbm.sourceforge.net/doc/pbm.html

//
static void unpackBits(int nbytes, const uint8 bytes[], uint8 raw_row[]) {
  // bitmask starts at top bit
  int offset = 0;
  uint8 mask = 1 << (7 - offset);
  while (offset < 8) {  // or (mask > 0)
    for (int b = 0; b < nbytes; b++) {
      raw_row[8 * b + offset] = (bytes[b] & mask) != 0;
    }
    mask >>= 1;
    offset++;
  }
}

static void packBits(int nbytes, uint8 bytes[], const uint8 raw_row[]) {
  // bitmask starts at top bit
  int offset = 0;
  uint8 mask = 1 << (7 - offset);
  while (offset < 8) {  // or (mask > 0)
    for (int b = 0; b < nbytes; b++) {
      if (offset == 0) bytes[b] = 0;
      bytes[b] |= raw_row[8 * b + offset] ? mask : 0;
    }
    mask >>= 1;
    offset++;
  }
}

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}

/// Load a raw PBM file.
/// Only binary PBM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageLoadPBM(const char* filename) {  ///
  int w, h;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  check((f = fopen(filename, "rb")) != NULL, "Open failed");
  // Parse PBM header
  check(fscanf(f, "P%c ", &c) == 1 && c == '4', "Invalid file format");
  skipComments(f);
  check(fscanf(f, "%d ", &w) == 1 && w >= 0, "Invalid width");
  skipComments(f);
  check(fscanf(f, "%d", &h) == 1 && h >= 0, "Invalid height");
  check(fscanf(f, "%c", &c) == 1 && isspace(c), "Whitespace expected");

  // Allocate image
  img = AllocateImageHeader((uint32)w, (uint32)h);

  // Read pixels
  int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
  // using VLAs...
  uint8 bytes[nbytes];
  uint8 raw_row[nbytes * 8];
  for (uint32 i = 0; i < img->height; i++) {
    check(fread(bytes, sizeof(uint8), nbytes, f) == (size_t)nbytes,
          "Reading pixels");
    unpackBits(nbytes, bytes, raw_row);
    img->image[i] = AllocateRowArray((uint32)w);
    for (uint32 j = 0; j < (uint32)w; j++) {
      img->image[i][j] = (uint16)raw_row[j];
    }
  }

  fclose(f);
  return img;
}

/// Save image to PBM file.
/// On success, returns nonzero.
/// On failure, a partial and invalid file may be left in the system.
int ImageSavePBM(const Image img, const char* filename) {  ///
  assert(img != NULL);
  assert(img->num_colors == 2);

  int w = (int)img->width;
  int h = (int)img->height;
  FILE* f = NULL;

  check((f = fopen(filename, "wb")) != NULL, "Open failed");
  check(fprintf(f, "P4\n%d %d\n", w, h) > 0, "Writing header failed");

  // Write pixels
  int nbytes = (w + 8 - 1) / 8;  // number of bytes for each row
  // using VLAs...
  uint8 bytes[nbytes];
  uint8 raw_row[nbytes * 8];
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      raw_row[j] = (uint8)img->image[i][j];
    }
    // Fill padding pixels with WHITE
    memset(raw_row + w, WHITE, nbytes * 8 - w);
    packBits(nbytes, bytes, raw_row);
    check(fwrite(bytes, sizeof(uint8), nbytes, f) == (size_t)nbytes,
          "Writing pixels failed");
  }

  // Cleanup
  fclose(f);

  return 0;
}

/// PPM file operations --- For RGB images

/// Load a raw PPM file.
/// Only ASCII PPM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageLoadPPM(const char* filename) {
  assert(filename != NULL);
  int w, h;
  int levels;
  char c;
  FILE* f = NULL;

  check((f = fopen(filename, "rb")) != NULL, "Open failed");
  // Parse PPM header
  check(fscanf(f, "P%c ", &c) == 1 && c == '3', "Invalid file format");
  skipComments(f);
  check(fscanf(f, "%d ", &w) == 1 && w >= 0, "Invalid width");
  skipComments(f);
  check(fscanf(f, "%d", &h) == 1 && h >= 0, "Invalid height");
  skipComments(f);
  check(fscanf(f, "%d", &levels) == 1 && 0 <= levels && levels <= 255,
        "Invalid depth");
  check(fscanf(f, "%c", &c) == 1 && isspace(c), "Whitespace expected");

  // Allocate image
  Image img = ImageCreate((uint32)w, (uint32)h);

  // Read pixels
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      int r, g, b;
      check(fscanf(f, "%d %d %d", &r, &g, &b) == 3 && 0 <= r && r <= levels &&
                0 <= g && g <= levels && 0 <= b && b <= levels,
            "Invalid pixel color");
      rgb_t color = r << 16 | g << 8 | b;
      uint16 index = LUTAllocColor(img, color);
      img->image[i][j] = index;
      // printf("[%u][%u]: (%d,%d,%d) -> %u (%6x)\n", i, j, r,g,b, index,
      // color);
    }
    fprintf(f, "\n");
  }

  fclose(f);
  return img;
}

/// Save image to PPM file.
/// On success, returns nonzero.
/// On failure, a partial and invalid file may be left in the system.
int ImageSavePPM(const Image img, const char* filename) {
  assert(img != NULL);

  int w = (int)img->width;
  int h = (int)img->height;
  FILE* f = NULL;

  check((f = fopen(filename, "wb")) != NULL, "Open failed");
  check(fprintf(f, "P3\n%d %d\n255\n", w, h) > 0, "Writing header failed");

  // The pixel RGB values
  for (uint32 i = 0; i < img->height; i++) {
    for (uint32 j = 0; j < img->width; j++) {
      uint16 index = img->image[i][j];
      rgb_t color = img->LUT[index];
      int r = color >> 16 & 0xff;
      int g = color >> 8 & 0xff;
      int b = color & 0xff;
      fprintf(f, "  %3d %3d %3d", r, g, b);
    }
    fprintf(f, "\n");
  }

  // Cleanup
  fclose(f);

  return 0;
}

/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
uint32 ImageWidth(const Image img) {
  assert(img != NULL);
  return img->width;
}

/// Get image height
uint32 ImageHeight(const Image img) {
  assert(img != NULL);
  return img->height;
}

/// Get number of image colors
uint16 ImageColors(const Image img) {
  assert(img != NULL);
  return img->num_colors;
}

/// Image comparison

/// These functions do not modify the images and never fail.

/// Check if img1 and img2 represent equal images.
/// NOTE: The same rgb color may correspond to different LUT labels in
/// different images!


int ImageIsEqual(const Image img1, const Image img2) { //! AUTHOR: DANIEL ZAMURCA
  assert(img1 != NULL);
  assert(img2 != NULL);

  // flag de estado: inicializamos a 0 para cada nova cor da img1.
  // assumimos que a cor não existe na img2 até a encontrarmos.
  int found = 0;

  // Ver se heigth, width, num_colors sao iguais nas duas imagens
  if (img1->width != img2->width)  return 0;
  if (img1->height != img2->height) return 0;
  if (img1->num_colors != img2->num_colors) return 0;


  // percorremos todas as cores da LUT na img1
  for (uint32 i = 0; i < img1->num_colors; i++) {
    rgb_t cor_img1 = img1->LUT[i];

    // reinicializar a flag a 0 quando acaba o ciclo 'for' para
    // passar para a verificacão da próxima cor.
    found = 0; 

    // procuramos essa cor específica da img1 percorrendo toda a tabela na img2
    for (uint32 j = 0; j < img2->num_colors; j++) {
      rgb_t cor_img2 = img2->LUT[j];
      if (cor_img1 == cor_img2) { 
        found = 1; // as cores são iguais
        break; // interrompe o loop interno
      }
    }
    // se o loop acabou e a variável 'found' continua a 0, 
    // significa que percorremos a img2 toda e esta cor não existe.
    if (!found) {
      return 0;  // se falta uma cor, ele retorna 0 dizendo qe as imagens não são iguais
    }
  }
    
  // percorre cada pixel da imagem (tanto img1 como img2 pois ambas têm o mesmo tamanho)
  // 
  for (uint32 v = 0; v < img1->height; v++) {
      for (uint32 u = 0; u < img1->width; u++) {
          InstrCount[0]++;  // conta as comparações de pixels

          // obter os índices da LUT das duas imagens
          uint8 index_LUT_img1 = img1->image[v][u];
          uint8 index_LUT_img2 = img2->image[v][u];

          // obter as cores reais da LUT , pois duas imagens podem ser
          // visualmente iguais mas usar índices diferentes.
          // p.ex: na img1 o Branco pode ser o índice 0, e na img2 ser o índice 1
          // por isso, temos de comparar o valor RGB (cor1, cor2) e não os índices
          rgb_t cor1 = img1->LUT[index_LUT_img1];
          rgb_t cor2 = img2->LUT[index_LUT_img2];

          // se as cores forem diferentes
          if (cor1 != cor2) {
              return 0; // retorna 0 dizendo que as imagens não são iguais
          }
      }
  }
  // se a execução chegou até aqui não foram encontradas diferenças
  // assim sendo as dimensões, a LUT e os pixeis são todos iguais
  // conclusão: as imagens são iguais
  return 1;
}

int ImageIsDifferent(const Image img1, const Image img2) {
  assert(img1 != NULL);
  assert(img2 != NULL);

  return !ImageIsEqual(img1, img2);
} //TODO CONFIRMAR DEPOIS ESTA FUNÇÃO

/// Geometric transformations

/// These functions apply geometric transformations to an image,
/// returning a new image as a result.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)

/// Rotate 90 degrees clockwise (CW).
/// Returns a rotated version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageRotate90CW(const Image img) { //! AUTHOR: TOMÁS COUTINHO
  assert(img != NULL);

  uint32 oldW = img->width;
  uint32 oldH = img->height;

  // a nova imagem tem width = oldH, height = oldW
  Image out = AllocateImageHeader(oldH, oldW);

  // copia a LUT e o num_colors da img para o out
  out->num_colors = img->num_colors;
  for (uint32 i = 0; i < out->num_colors; i++) {
    out->LUT[i] = img->LUT[i]; 
  } // tambem se poderia usar memcpy


  // Alocar memória para as linhas da nova imagem 
  // (terá 'out->height' linhas, cada uma com comprimento 'out->width')
  for (uint32 i = 0; i < out->height; i++) {
    out->image[i] = AllocateRowArray(out->width);
  } // tambem se poderia usar memcpy

  // Para cada pixel da imagem original, colocamo-lo na nova posição rodada.
  // Lógica da Rotação 90º Horário (Clockwise):
  // A origem (u, v) mapeia para o destino (nova_linha, nova_coluna).
  // - A coluna original (u) passa a ser a nova linha.
  // - A linha original (v) passa a ser a nova coluna (mas invertida).
  // Fórmula: destino[u][oldH - 1 - v] = origem[v][u]
  for (uint32 v = 0; v < oldH; v++) {
      for (uint32 u = 0; u < oldW; u++) {
          out->image[u][oldH - 1 - v] = img->image[v][u];
      }
  }
  return out;
}

/// Rotate 180 degrees clockwise (CW).
/// Returns a rotated version of the image.
/// Ensures: The original img is not modified.
///
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
Image ImageRotate180CW(const Image img) { //! AUTHOR: TOMÁS COUTINHO
  assert(img != NULL);

  // usar a função ImageRotate90CW duas vezes, assim temos os 180º

  Image tmp = ImageRotate90CW(img); // imagem temporária
  if (tmp == NULL) return NULL;
  Image out = ImageRotate90CW(tmp);

  ImageDestroy(&tmp); // libertar a imagem temporária
  return out;
}

/// Check whether pixel coords (u, v) are inside img.
/// ATTENTION
///   u : column index
///   v : row index
int ImageIsValidPixel(const Image img, int u, int v) {
  return 0 <= u && u < (int)img->width && 0 <= v && v < (int)img->height;
}

/// Region Growing

/// The following three *RegionFilling* functions perform region growing
/// using some variation of the 4-neighbors flood-filling algorithm:
///   Given the coordinates (u, v) of a seed pixel,
///   fill all similarly-colored adjacent pixels with a new color label.
///
/// All of these functions receive the same arguments:
///   img: The image to operate on (and modify).
///   u, v: the coordinates of the seed pixel.
///   label: the new color label (LUT index) to fill the region with.
///
/// And return: the number of labeled pixels.

/// Each function carries out a different version of the algorithm.

/// Region growing using the recursive flood-filling algorithm.
int ImageRegionFillingRecursive(Image img, int u, int v, uint16 color) { //! AUTHOR: Daniel Zamurca
    assert(img != NULL);
    assert(ImageIsValidPixel(img, u, v));
    assert(color < FIXED_LUT_SIZE);

    uint16 background = img->image[v][u];  // cor original do pixel de partida (u,v)

    // Se o pixel já estiver com a cor da região, retorna 0
    if (background == color) return 0;

    // senão, pinta o pixel atual com a nova cor
    img->image[v][u] = color;
    int number_labeled_pixels = 1;  // conta esse pixel

    // Esta função impõe uma pré-condição através de:
    // assert(ImageIsValidPixel(img, u, v));
    //
    // Isto significa que chamar esta função com coordenadas inválidas (fora da imagem)
    // causará a terminação imediata do programa (abort), e não apenas um retorno nulo.
    //
    // Por essa razão, não podemos usar a abordagem recursiva "cega" típica onde se chama
    // a função e se verifica a validade no início da próxima execução.
    // Tivemos de adotar uma abordagem "Look-Ahead" (verificar antes de ir):
    // validamos explicitamente se o vizinho (u+1) está dentro dos limites
    // antes de efetuar a chamada recursiva.
    //
    // (Alternativamente, poderíamos ter criado uma função auxiliar estática sem asserts,
    // mas optámos por manter a estrutura fornecida e gerir a segurança no caller.)
    
    // Explorar vizinhos (4-direções) só se forem válidos e tiverem a cor de background
    if (ImageIsValidPixel(img, u+1, v) && img->image[v][u+1] == background)
       number_labeled_pixels += ImageRegionFillingRecursive(img, u+1, v, color);
    if (ImageIsValidPixel(img, u-1, v) && img->image[v][u-1] == background)
       number_labeled_pixels += ImageRegionFillingRecursive(img, u-1, v, color);
    if (ImageIsValidPixel(img, u, v+1) && img->image[v+1][u] == background)
       number_labeled_pixels += ImageRegionFillingRecursive(img, u, v+1, color);
    if (ImageIsValidPixel(img, u, v-1) && img->image[v-1][u] == background)
       number_labeled_pixels += ImageRegionFillingRecursive(img, u, v-1, color);

    return number_labeled_pixels;  // número total de pixels pintados
}


/// Region growing using a STACK of pixel coordinates to
/// implement the flood-filling algorithm.
int ImageRegionFillingWithSTACK(Image img, int u, int v, uint16 label) { //! AUTHOR: DANIEL ZAMURCA
  assert(img != NULL);
  assert(ImageIsValidPixel(img, u, v));
  assert(label < FIXED_LUT_SIZE);

  // vemos a cor original antes de criar qualquer estrutura de dados
  uint16 background = img->image[v][u];

  // se o pixel já tem a cor alvo, retornamos 0
  // assim evitamos o custo computacional de alocar (malloc) e 
  // libertar  a memória da Stack desnecessariamente.
  if (background == label) {
    return 0;
  }

  //
  Stack* stack = StackCreate(10000); 

  // Cria uma instacia para guardar as coordenadas atuais (u,v)
  PixelCoords p = PixelCoordsCreate(u,v); 
  StackPush(stack, p); // adiciona as cordenadas atuais (u,v) na stack

  int pixels_painted = 0; // variável de contagem dos pixeis que foram pintados

  while (!StackIsEmpty(stack)) {
      // Retira o pixel mais recente do topo da pilha
      PixelCoords p = StackPop(stack);
      // obtemos as coordenadas individuais para usar como índices da matriz
      int x = PixelCoordsGetU(p);
      int y = PixelCoordsGetV(p);

      // Verifica se é válido e se tem a cor de background
      if (ImageIsValidPixel(img, x, y) && img->image[y][x] == background) {
            img->image[y][x] = label;  // pinta o pixel
            pixels_painted++; // soma um à variável de contagem

            // Empilhamos os vizinhos diretamente, sem verificar 
            // se são válidos.
            // Na recursiva, validamos antes de chamar, aqui,
            // permitimos que coordenadas inválidas entrem na stack.
            // A validade será testada apenas quando forem retiradas (pop)
            // no início da próxima iteração do ciclo 'while'
            StackPush(stack, PixelCoordsCreate(x + 1, y));
            StackPush(stack, PixelCoordsCreate(x - 1, y));
            StackPush(stack, PixelCoordsCreate(x, y + 1));
            StackPush(stack, PixelCoordsCreate(x, y - 1));
        }
  }
  StackDestroy(&stack); //destrói a stack que foi algo auxiliar
  return pixels_painted; // dá return ao numero de pixeis pintados
}

/// Region growing using a QUEUE of pixel coordinates to
/// implement the flood-filling algorithm.
int ImageRegionFillingWithQUEUE(Image img, int u, int v, uint16 label) { //! AUTHOR: TOMÁS COUTINHO
  assert(img != NULL);
  assert(ImageIsValidPixel(img, u, v));
  assert(label < FIXED_LUT_SIZE);

  // vemos a cor original antes de criar qualquer estrutura de dados
  uint16 background = img->image[v][u];

  // se o pixel já tem a cor alvo, retornamos 0
  // assim vitamos o custo computacional de alocar (malloc) e 
  // libertar  a memória da Stack desnecessariamente.
  if (background == label) {
    return 0;
  }

  Queue* queue = QueueCreate(10000);

  // Cria uma instância para guardar as coordenadas atuais (u,v)
  PixelCoords p = PixelCoordsCreate(u, v); // adiciona as coordenadas atuais (u,v) na stack
  QueueEnqueue(queue, p); // adiciona as cordenadas atuais (u,v) na queue

  int pixels_painted = 0;

  while (!QueueIsEmpty(queue)) {
    // Retira o pixel mais antigo do topo da pilha
    PixelCoords p = QueueDequeue(queue);
    // obtemos as coordenadas individuais para usar como índices da matriz
    int x = PixelCoordsGetU(p);
    int y = PixelCoordsGetV(p);

    if (ImageIsValidPixel(img, x, y) && img->image[y][x] == background) {
      img->image[y][x] = label;
      pixels_painted++;
      
      // Empilhamos os vizinhos diretamente, sem verificar 
      // se são válidos.
      // Na recursiva, validamos antes de chamar, aqui,
      // permitimos que coordenadas inválidas entrem na queue.
      // A validade será testada apenas quando forem retiradas (Dequeue)
      // no início da próxima iteração do ciclo 'while'
      QueueEnqueue(queue, PixelCoordsCreate(x + 1, y));
      QueueEnqueue(queue, PixelCoordsCreate(x - 1, y));
      QueueEnqueue(queue, PixelCoordsCreate(x, y + 1));
      QueueEnqueue(queue, PixelCoordsCreate(x, y - 1));
    }
  }

  QueueDestroy(&queue); // destrói a queue que foi algo auxiliar
  return pixels_painted; // dá return ao numero de pixeis pintados
}

/// Image Segmentation

/// Label each WHITE region with a different color.
/// - WHITE (the background color) has label (LUT index) 0.
/// - Use GenerateNextColor to create the RGB color for each new region.
///
/// One of the region filling functions above is passed as the
/// last argument, using a function pointer.
///
/// Returns the number of image regions found.
int ImageSegmentation(Image img, FillingFunction fillFunct) { //! AUTHOR: TOMÁS COUTINHO
  assert(img != NULL);
  assert(fillFunct != NULL);

  int num_regions = 0; // variável de contagem
  // começa com preto, para nao ser da mesma cor que o background
  rgb_t current_color = 0x000000;

  // percorremos todos os pixels da imagem
  for (uint32 v = 0; v < img->height; v++) {
    for (uint32 u = 0; u < img->width; u++) {
      // se encontrarmos um pixel com a cor 0, significa que descobrimos uma região que ainda nao foi visitada
      if (img->image[v][u] == 0) {
        // gerar a próxima cor para esta região
        current_color = GenerateNextColor(current_color);
        uint16 new_label = LUTAllocColor(img, current_color);

        // usamos a função passada por argumento (Recursive, Stack ou Queue)
        // para pintar a região inteira de uma só vez.
        // assim garantindo que o loop principal não volta a contar estes pixels.
        fillFunct(img, u, v, new_label);

        num_regions++;
      }
    }
  }

  return num_regions;
}
