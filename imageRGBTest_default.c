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



// copiamos a estrutura para podermos manipular 
// os pixels para os testes.
struct image {
  uint32 width;
  uint32 height;
  uint16** image;     // pointer to an array of pointers referencing the image rows
  uint16 num_colors;  // the number of colors (i.e., pixel labels) used
  rgb_t* LUT;         // table storing (R,G,B) triplets
};

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

  printf("12) ImageRegionFillingWithQUEUE \n");
  Image image_chess_queue = ImageCreateChess(150, 120, 30, 0x000000);  // black
  // Chamar a função de preenchimento com queue
  int filled_pixels_queue = ImageRegionFillingWithQUEUE(image_chess_queue, 0, 0, 0);
  // Mostrar resultados
  printf("    Píxeis pintados: %d\n", filled_pixels_queue);
  // Guardar o resultado para comparação
  ImageSavePPM(image_chess_queue, "filling_queue_image.ppm");

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

  printf("13) ImageSegmentation\n");

  // 1. Criar a imagem original "limpa"
  Image image_original = ImageCreateChess(150, 120, 30, 0x000000); 

  // --- TESTE 1: RECURSIVE ---
  printf("    Testing Recursive...\n");
  Image img_rec = ImageCopy(image_original); // Fazemos uma cópia para não estragar a original
  int num_regions_rec = ImageSegmentation(img_rec, ImageRegionFillingRecursive);
  printf("    Regions found: %d\n", num_regions_rec);
  ImageSavePPM(img_rec, "segmented_image_recursive.ppm"); // Gravamos a imagem alterada, não o inteiro
  ImageDestroy(&img_rec); // Limpamos a memória da cópia


  // --- TESTE 2: STACK ---
  printf("    Testing Stack...\n");
  Image img_stack = ImageCopy(image_original); // Nova cópia limpa da original
  int num_regions_stack = ImageSegmentation(img_stack, ImageRegionFillingWithSTACK);
  printf("    Regions found: %d\n", num_regions_stack);
  ImageSavePPM(img_stack, "segmented_image_stack.ppm"); // Gravamos a imagem correta
  ImageDestroy(&img_stack);


  // --- TESTE 3: QUEUE ---
  printf("    Testing Queue...\n");
  Image img_queue = ImageCopy(image_original); // Nova cópia limpa da original
  int num_regions_queue = ImageSegmentation(img_queue, ImageRegionFillingWithQUEUE);
  printf("    Regions found: %d\n", num_regions_queue);
  ImageSavePPM(img_queue, "segmented_image_queue.ppm"); // Gravamos a imagem correta
  ImageDestroy(&img_queue);


  // Limpar a original no fim
  ImageDestroy(&image_original);
  
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
  // T2 – DIFERENTES NO PRIMEIRO PIXEL  (Best-case em pixels)
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

  printf("     ------------------------------------------------------------------\n");
  // ------------------------------------------------------
  // T3 — Diferentes no último pixel (Pior caso: O(N))
  // ------------------------------------------------------
  
  // 1. Preparação do Exemplo Ilustrativo (sizes[0])
  Image E_demo = ImageCreate(sizes[0], sizes[0]); // Cria toda branca
  Image F_demo = ImageCopy(E_demo);
  
  // Modificar APENAS o último pixel (canto inferior direito) para Preto (índice 1)
  F_demo->image[sizes[0]-1][sizes[0]-1] = 1; 

  // Guardar exemplo
  ImageSavePBM(E_demo, "Test/Test3/T3_img1.pbm");
  ImageSavePBM(F_demo, "Test/Test3/T3_img2.pbm");
  
  // Limpar memória do exemplo
  ImageDestroy(&E_demo);
  ImageDestroy(&F_demo);

  // 2. Loop de Complexidade
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image E = ImageCreate(w, h);
    Image F = ImageCopy(E);

    // Mudar o último pixel
    F->image[h-1][w-1] = 1; 

    InstrReset();
    ImageIsEqual(E, F);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test3", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&E);
    ImageDestroy(&F);
  }

  printf("     ------------------------------------------------------------------\n");
  // ------------------------------------------------------
  // T4 — Diferentes num pixel aleatório intermédio (Caso Médio: ~O(N/2))
  // ------------------------------------------------------

  // 1. Preparação do Exemplo Ilustrativo
  Image G_demo = ImageCreate(sizes[0], sizes[0]);
  Image H_demo = ImageCopy(G_demo);

  // Mudar um pixel no meio (evitando o primeiro e o último)
  int r_w_demo = sizes[0] / 2; 
  int r_h_demo = sizes[0] / 2;
  H_demo->image[r_h_demo][r_w_demo] = 1; 

  ImageSavePBM(G_demo, "Test/Test4/T4_img1.pbm");
  ImageSavePBM(H_demo, "Test/Test4/T4_img2.pbm");

  ImageDestroy(&G_demo);
  ImageDestroy(&H_demo);

  // 2. Loop de Complexidade
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image G = ImageCreate(w, h);
    Image H = ImageCopy(G);

    // Mudar pixel aleatório (evita (0,0) e (max,max))
    int random_w = (rand() % (w - 2)) + 1;
    int random_h = (rand() % (h - 2)) + 1;
    H->image[random_h][random_w] = 1; 

    InstrReset();
    ImageIsEqual(G, H);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test4", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&G);
    ImageDestroy(&H);
  }

  printf("     ------------------------------------------------------------------\n");
  // ------------------------------------------------------
  // T5 — Imagens totalmente aleatórias (Melhor Caso: O(1))
  // ------------------------------------------------------

  // 1. Preparação do Exemplo Ilustrativo
  // Criar duas imagens de ruído aleatório
  Image I_demo = ImageCreate(sizes[0], sizes[0]);
  Image J_demo = ImageCreate(sizes[0], sizes[0]);

  // Preencher com ruído (0 ou 1)
  for(uint32 y=0; y < I_demo->height; y++) {
      for(uint32 x=0; x < I_demo->width; x++) {
          I_demo->image[y][x] = rand() % 2;
          J_demo->image[y][x] = rand() % 2;
      }
  }
  // Garantir que o primeiro pixel é diferente para ser O(1)
  I_demo->image[0][0] = 0;
  J_demo->image[0][0] = 1;

  ImageSavePBM(I_demo, "Test/Test5/T5_img1.pbm");
  ImageSavePBM(J_demo, "Test/Test5/T5_img2.pbm");

  ImageDestroy(&I_demo);
  ImageDestroy(&J_demo);

  // 2. Loop de Complexidade
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image I = ImageCreate(w, h);
    Image J = ImageCreate(w, h);

    // Vamos apenas garantir que o primeiro pixel é diferente.
    // Não precisamos de preencher a imagem toda com ruído para o teste de complexidade,
    // pois a função ImageIsEqual vai parar logo no primeiro pixel (0,0).
    // Isto poupa tempo na execução dos testes grandes.
    I->image[0][0] = 0;
    J->image[0][0] = 1;

    InstrReset();
    ImageIsEqual(I, J);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test5", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&I);
    ImageDestroy(&J);
  }

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
