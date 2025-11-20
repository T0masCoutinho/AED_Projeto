// imageRGBTest - A program that performs some operations on RGB images.
//
// This program is an example use of the imageRGB module,
// a programming project for the course AED, DETI / UA.PT
//
// You may freely use and modify this code, NO WARRANTY, blah blah,
// as long as you give proper credit to the original and subsequent authors.
//
// The AED Team <jmadeira@ua.pt, jmr@ua.pt, ...>
// 2025

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "imageRGB.h"
#include "instrumentation.h"

int main(int argc, char* argv[]) {
  program_name = argv[0];
  if (argc != 1) {
    error(1, 0, "Usage: imageRGBTest");
  }

  ImageInit();

  // Creating and displaying some images

  printf("1) ImageCreate\n");
  Image white_image = ImageCreate(100, 100);
  // ImageRAWPrint(white_image);

  printf("2) ImageCreateChess(black)+ ImageSavePBM\n");
  Image image_chess_1 = ImageCreateChess(150, 120, 30, 0x000000);  // black
  // ImageRAWPrint(image_chess_1);
  ImageSavePBM(image_chess_1, "chess_image_1.pbm");

  printf("3) ImageCreateChess(red) + ImageSavePPM\n");
  Image image_chess_2 = ImageCreateChess(20, 20, 8, 0xff0000);  // red
  ImageRAWPrint(image_chess_2);
  ImageSavePPM(image_chess_2, "chess_image_2.ppm");

  printf("4) ImageCreateChess(all black)\n");
  Image black_image = ImageCreateChess(100, 100, 100, 0x000000);  // all black
  // ImageRAWPrint(black_image);
  ImageSavePBM(black_image, "black_image.pbm");

  printf("5) ImageCopy\n");
  Image copy_image = ImageCopy(image_chess_1);
  // ImageRAWPrint(copy_image);
  if (copy_image != NULL) {
    ImageSavePBM(copy_image, "copy_image.pbm");
  }

  printf("6) ImageLoadPBM\n");
  Image image_1 = ImageLoadPBM("img/feep.pbm");
  ImageRAWPrint(image_1);

  printf("7) ImageLoadPPM\n");
  Image image_2 = ImageLoadPPM("img/feep.ppm");
  ImageRAWPrint(image_2);

  printf("8) ImageCreatePalete\n");
  Image image_3 = ImageCreatePalete(4 * 32, 4 * 32, 4);
  ImageSavePPM(image_3, "palete.ppm");

  //!------------------------------------------------------------------------------
  
  printf("9) ImageRegionFillingRecursive \n");

  Image image_chess_recursive = ImageCreateChess(150, 120, 30, 0x000000);  // black
  // Chamar a função de preenchimento
  int filled_pixels = ImageRegionFillingRecursive(image_chess_recursive, 0, 0, 0); 

  printf("   Píxeis pintados: %d\n", filled_pixels);

  // Guardar o resultado para comparação
  ImageSavePPM(image_chess_recursive, "filling_recursive_image.ppm");

  //!------------------------------------------------------------------------------

  printf("10) ImageRegionFillingWithSTACK \n");

  Image image_chess_stack = ImageCreateChess(150, 120, 30, 0x000000);  // black
  // Chamar a função de preenchimento com stack
  int filled_pixels_stack = ImageRegionFillingWithSTACK(image_chess_stack, 0, 0, 0);

  // Mostrar resultados
  printf("    Píxeis pintados: %d\n", filled_pixels_stack);

  // Guardar o resultado para comparação
  ImageSavePPM(image_chess_stack, "filling_stack_image.ppm");

  //!-----------------------------------------------------------------------------

  printf("11) Image Rotate 90 degrees: \n");
  Image rotated_image = ImageRotate90CW(image_2);
  ImageSavePPM(rotated_image, "rotated_image_90.ppm");

  printf("    Rotated image 180 degrees: \n");
  Image rotated_image_180 = ImageRotate180CW(image_2);
  ImageSavePPM(rotated_image_180, "rotated_image_180.ppm");

  //!-----------------------------------------------------------------------------

  printf("12) ImageIsEqual \n");
  int image_is_equal = ImageIsEqual(image_chess_2, copy_image);
  if (image_is_equal) {
    printf("    As imagens são iguais\n");
  } else {
    printf("    As imagens são diferentes\n");
  }

  //!-----------------------------------------------------------------------------

  printf("\n=======================================\n");
  printf(" TESTES DE COMPLEXIDADE: ImageIsEqual    (demora um bocado)\n");
  printf("=======================================\n");

  InstrName[0] = "comparisons";  
  InstrCalibrate();

  int sizes[] = {100, 200, 400, 800, 1600, 3200, 6400};
  int nsizes = sizeof(sizes) / sizeof(sizes[0]);

  printf("\n%10s %18s %12s %12s %15s\n",
        "NºTeste", "Dimensions(w*h)", "Time(s)", "Caltime", "Comparisons");

  // Dimensão padrão das imagens do teste
  //int W = 512, H = 512;

  // ------------------------------------------------------
  // T1 – IMAGENS IGUAIS  (Worst-case linear)
  // ------------------------------------------------------
  Image A = ImageCreate(sizes[0], sizes[0]);
  Image B = ImageCopy(A);

  ImageSavePBM(A, "Test/Test1/T1_img1.pbm");
  ImageSavePBM(B, "Test/Test1/T1_img2.pbm");

  ImageIsEqual(A, B);
  //! EXEMPLO DE PADRAO QUE VAMOS USAR

  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

      // Criar duas imagens iguais
    Image A = ImageCreate(w, h);
    Image B = ImageCopy(A);

    InstrReset();
    ImageIsEqual(A, B);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test1", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&A);
    ImageDestroy(&B);
  }
  printf("     ------------------------------------------------------------------\n");
  // ------------------------------------------------------
  // T2 – DIFERENTES NO PRIMEIRO PIXEL  (Best-case)
  // ------------------------------------------------------
  
  Image C = ImageCreateChess(sizes[0], sizes[0], 30, 0x000000);  // imagem de xadrez
  Image D = ImageCreateChess(sizes[0], sizes[0], 30, 0x000000);  // imagem de xadrez
  ImageRegionFillingRecursive(D, 0, 0, 0); // mudar primeiro quadrado xadrez na imagem D

  ImageSavePBM(C, "Test/Test2/T2_img1.pbm");
  ImageSavePBM(D, "Test/Test2/T2_img2.pbm");

  ImageIsEqual(C, D);
  //! EXEMPLO DE PADRAO QUE VAMOS USAR

  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

      // Criar duas imagens iguais
    Image C = ImageCreateChess(w, h, 30, 0x000000);  // imagem de xadrez
    Image D = ImageCreateChess(w, h, 30, 0x000000);  // imagem de xadrez
    ImageRegionFillingRecursive(D, 0, 0, 0); // mudar primeiro quadrado xadrez na imagem D


    InstrReset();
    ImageIsEqual(C, D);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test2", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&C);
    ImageDestroy(&D);
  } 
  


    /*
    Tipos de comparação importantes:
    Nome do Teste	Caso	O que mede
    T1 — Iguais (Worst-Case Linear)	pior caso	percorre todos os pixels → O(n)
    T2 — Diferentes no primeiro pixel	melhor caso	O(1)
    T3 — Diferentes no último pixel	pior caso	O(n)
    T4 — Diferentes num pixel aleatório	intermédio	O(n)
    T5 — Imagens totalmente aleatórias	realista	distribuição uniforme
    T6 — Imagens enormes (até limite da RAM)	stress test	estabilidade / performance
    */

   //!-----------------------------------------------------------------------------


  // Libertar memória

  ImageDestroy(&white_image);
  ImageDestroy(&black_image);
  if (copy_image != NULL) {
    ImageDestroy(&copy_image);
  }
  ImageDestroy(&image_chess_1);
  ImageDestroy(&image_chess_2);
  ImageDestroy(&image_1);
  ImageDestroy(&image_2);
  ImageDestroy(&image_3);

  return 0;
}
