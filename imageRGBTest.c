// imageRGBTest - A program that performs some operations on RGB images.
//
// This program is an example use of the imageRGB module,
// a programming project for the course AED, DETI / UA.PT

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>     // Necessário para random e clock
#include <stdint.h>   // Necessário para uint32_t etc

#include "error.h"
#include "imageRGB.h"
#include "instrumentation.h"

// assim temos acesso direto á estrutura, e podemos mudar pixels
#define FIXED_LUT_SIZE 1000

struct image {
  uint32 width;
  uint32 height;
  uint16** image;     
  uint16 num_colors;  
  uint32* LUT;         
};
// ============================================================================


// --- FUNÇÕES AUXILIARES DE TESTE ---

void Test1_BasicOperations() {
  printf("\n>> 1. TESTES BÁSICOS DE CRIAÇÃO E I/O \n");
  
  Image white_image = ImageCreate(100, 100);
  printf("   [OK] ImageCreate(100, 100)\n");

  Image image_chess_1 = ImageCreateChess(150, 120, 30, 0x000000); 
  // Alterado para salvar na pasta basic
  ImageSavePBM(image_chess_1, "Test/basic/chess_image_1.pbm");
  printf("   [OK] ImageCreateChess + SavePBM (Test/basic/chess_image_1.pbm)\n");

  Image image_chess_2 = ImageCreateChess(20, 20, 8, 0xff0000); 
  // Alterado para salvar na pasta basic
  ImageSavePPM(image_chess_2, "Test/basic/chess_image_2.ppm");
  printf("   [OK] ImageCreateChess (Red) + SavePPM (Test/basic/chess_image_2.ppm)\n");

  Image copy_image = ImageCopy(image_chess_1);
  if (copy_image != NULL) {
    // Alterado para salvar na pasta basic
    ImageSavePBM(copy_image, "Test/basic/copy_image.pbm");
    printf("   [OK] ImageCopy (Test/basic/copy_image.pbm)\n");
  }

  // Limpeza
  ImageDestroy(&white_image);
  ImageDestroy(&image_chess_1);
  ImageDestroy(&image_chess_2);
  ImageDestroy(&copy_image);
}

void Test2_FloodFill_Visual() {
  printf("\n>> 2. TESTES VISUAIS DE FLOOD FILL (Verificar pasta Test/basic/) \n");

  // Recursivo
  Image img_rec = ImageCreateChess(150, 120, 30, 0x000000);
  int pix_rec = ImageRegionFillingRecursive(img_rec, 0, 0, 0); 
  ImageSavePPM(img_rec, "Test/basic/filling_recursive_image.ppm");
  printf("   Recursive: %d pixeis pintados.\n", pix_rec);

  // Stack
  Image img_stack = ImageCreateChess(150, 120, 30, 0x000000);
  int pix_stack = ImageRegionFillingWithSTACK(img_stack, 0, 0, 0);
  ImageSavePPM(img_stack, "Test/basic/filling_stack_image.ppm");
  printf("   Stack:     %d pixeis pintados.\n", pix_stack);

  // Queue
  Image img_queue = ImageCreateChess(150, 120, 30, 0x000000);
  int pix_queue = ImageRegionFillingWithQUEUE(img_queue, 0, 0, 0);
  ImageSavePPM(img_queue, "Test/basic/filling_queue_image.ppm");
  printf("   Queue:     %d pixeis pintados.\n", pix_queue);

  ImageDestroy(&img_rec);
  ImageDestroy(&img_stack);
  ImageDestroy(&img_queue);
}

void Test3_GeometricProperties() {
  printf("\n>> 3. TESTE DE PROPRIEDADES GEOMÉTRICAS \n");
  
  Image original = ImageLoadPPM("img/feep.ppm"); 
  if (original == NULL) original = ImageCreateChess(50, 50, 10, 0x000000); 

  // Teste: Rotação 90º x 4 
  Image r1 = ImageRotate90CW(original);
  Image r2 = ImageRotate90CW(r1);
  Image r3 = ImageRotate90CW(r2);
  Image r4 = ImageRotate90CW(r3); 

  if (ImageIsEqual(original, r4)) {
      printf("   [PASSED] Rotate90CW x 4 == Original (Identidade)\n");
  } else {
      printf("   [FAILED] Rotate90CW x 4 != Original\n");
  }

  // Teste: Rotação 180º x 2
  Image r180_1 = ImageRotate180CW(original);
  Image r180_2 = ImageRotate180CW(r180_1);

  if (ImageIsEqual(original, r180_2)) {
      printf("   [PASSED] Rotate180CW x 2 == Original (Identidade)\n");
  } else {
      printf("   [FAILED] Rotate180CW x 2 != Original\n");
  }
  
  ImageDestroy(&original);
  ImageDestroy(&r1); ImageDestroy(&r2); ImageDestroy(&r3); ImageDestroy(&r4);
  ImageDestroy(&r180_1); ImageDestroy(&r180_2);
}

void Test4_SegmentationVisual() {
  printf("\n>> 4. TESTES VISUAIS DE SEGMENTAÇÃO (Verificar pasta Test/basic/) \n");

  // Criar uma imagem base de Xadrez
  Image base_chess = ImageCreateChess(150, 120, 30, 0x000000);
  
  // --- VERSÃO RECURSIVA ---
  Image seg_rec = ImageCopy(base_chess);
  int reg_rec = ImageSegmentation(seg_rec, ImageRegionFillingRecursive);
  ImageSavePPM(seg_rec, "Test/basic/segmented_recursive.ppm");
  printf("   Recursive: %d regiões encontradas. (Gravado em segmented_recursive.ppm)\n", reg_rec);
  ImageDestroy(&seg_rec);

  // --- VERSÃO STACK ---
  Image seg_stack = ImageCopy(base_chess);
  int reg_stack = ImageSegmentation(seg_stack, ImageRegionFillingWithSTACK);
  ImageSavePPM(seg_stack, "Test/basic/segmented_stack.ppm");
  printf("   Stack:     %d regiões encontradas. (Gravado em segmented_stack.ppm)\n", reg_stack);
  ImageDestroy(&seg_stack);

  // --- VERSÃO QUEUE ---
  Image seg_queue = ImageCopy(base_chess);
  int reg_queue = ImageSegmentation(seg_queue, ImageRegionFillingWithQUEUE);
  ImageSavePPM(seg_queue, "Test/basic/segmented_queue.ppm");
  printf("   Queue:     %d regiões encontradas. (Gravado em segmented_queue.ppm)\n", reg_queue);
  ImageDestroy(&seg_queue);
  
  // Validação rápida numérica
  // Num tabuleiro 150x120 com quadrados de 30: 
  // (150/30) * (120/30) = 5 * 4 = 20 quadrados totais.
  // Metade são pretos (fundo), metade são brancos (regiões).
  // Esperamos 10 regiões.
  if (reg_queue == 10) printf("   [PASSED] Contagem correta (10 regiões esperadas).\n");
  else printf("   [FAILED] Contagem suspeita (%d vs 10 esperadas).\n", reg_queue);

  ImageDestroy(&base_chess);
}

void Test5_StressTest() {
    printf("\n>> 5. STRESS TEST: Comparação de Estratégias (Imagens Grandes) \n");
    
    // Tamanho "Grande" para Stack/Queue (4 Milhões de pixeis)
    int big_size = 2000; 
    
    // Tamanho "Seguro" para Recursiva (2500 pixeis)
    // 50x50 é seguro. 500x500 já causou o teu crash.
    int safe_recursive_size = 50; 

    printf("   [INFO] A criar imagem branca gigante de %dx%d...\n", big_size, big_size);
    
    Image imgStack = ImageCreate(big_size, big_size);
    Image imgQueue = ImageCopy(imgStack);

    // --- TESTE 1: STACK (DFS Iterativo) ---
    printf("   A testar Stack (DFS) em %dx%d... ", big_size, big_size);
    fflush(stdout);
    InstrReset();
    ImageRegionFillingWithSTACK(imgStack, 0, 0, 1);
    double timeStack = cpu_time() - InstrTime;
    printf("Tempo: %.4f s\n", timeStack);

    // --- TESTE 2: QUEUE (BFS Iterativo) ---
    printf("   A testar Queue (BFS) em %dx%d... ", big_size, big_size);
    fflush(stdout);
    InstrReset();
    ImageRegionFillingWithQUEUE(imgQueue, 0, 0, 1);
    double timeQueue = cpu_time() - InstrTime;
    printf("Tempo: %.4f s\n", timeQueue);

    // --- TESTE 3: RECURSIVE (O Teste Controlado) ---
    printf("   A testar Recursive... \n");
    
    // 3.1 - Teste Seguro (Imagem Pequena)
    printf("      -> Teste em imagem PEQUENA (%dx%d)... ", safe_recursive_size, safe_recursive_size);
    fflush(stdout);
    
    Image imgRecSmall = ImageCreate(safe_recursive_size, safe_recursive_size);
    InstrReset();
    ImageRegionFillingRecursive(imgRecSmall, 0, 0, 1); 
    double timeRec = cpu_time() - InstrTime;
    printf("Sucesso! Tempo: %.4f s\n", timeRec);
    ImageDestroy(&imgRecSmall);

    // 3.2 - Proteção contra Crash
    printf("      -> Teste em imagem GRANDE (%dx%d)... ", big_size, big_size);
    
    if (big_size > 100) { // GUARDA DE SEGURANÇA
        printf("ABORTADO (Safety Check)\n");
        printf("         [ALERTA] Teste cancelado propositadamente.\n");
        printf("         A recursão nesta resolução causaria 'Segmentation Fault'\n");
        printf("         devido ao estouro da Stack do sistema.\n");
    } else {
        // Se alguém diminuir o big_size, isto corre
        ImageRegionFillingRecursive(imgStack, 0, 0, 1); // Cuidado aqui!
        printf("Concluído (com sorte).\n");
    }

    // Limpeza
    ImageDestroy(&imgStack);
    ImageDestroy(&imgQueue);
}

void Test6_ComplexityAnalysis() {
  printf("\n===============================================================\n");
  printf(" 6. GERAÇÃO DE EXEMPLOS VISUAIS (Para Relatório)\n");
  printf("===============================================================\n");


  int base = 100; // Tamanho para as imagens de exemplo
  printf("   A gerar imagens de exemplo (%dx%d) nas pastas Test/TestX...\n", base, base);

  // --- T1: IMAGENS IGUAIS ---
  Image A = ImageCreate(base, base); 
  Image B = ImageCopy(A);
  ImageSavePBM(A, "Test/Test1/T1_img1.pbm"); 
  ImageSavePBM(B, "Test/Test1/T1_img2.pbm");
  ImageDestroy(&A); 
  ImageDestroy(&B);
  printf("   [OK] T1 (Iguais) gerado em Test/Test1/\n");

  // --- T2: DIFERENTES NO INÍCIO ---
  Image C = ImageCreateChess(base, base, 10, 0x000000); 
  Image D = ImageCreateChess(base, base, 10, 0x000000);
  // Inverter o primeiro pixel para garantir diferença
  uint16 val_demo = D->image[0][0]; 
  D->image[0][0] = (val_demo == 0) ? 1 : 0;
  ImageSavePBM(C, "Test/Test2/T2_img1.pbm"); 
  ImageSavePBM(D, "Test/Test2/T2_img2.pbm");
  ImageDestroy(&C); 
  ImageDestroy(&D);
  printf("   [OK] T2 (Dif. Inicio) gerado em Test/Test2/\n");

  // --- T3: DIFERENTES NO FIM ---
  Image E = ImageCreate(base, base); 
  Image F = ImageCopy(E);
  // Alterar último pixel
  F->image[base-1][base-1] = 1; 
  ImageSavePBM(E, "Test/Test3/T3_img1.pbm"); 
  ImageSavePBM(F, "Test/Test3/T3_img2.pbm");
  ImageDestroy(&E); 
  ImageDestroy(&F);
  printf("   [OK] T3 (Dif. Fim) gerado em Test/Test3/\n");

  // --- T4: DIFERENTES NO MEIO ---
  Image G = ImageCreate(base, base);
  Image H = ImageCopy(G);
  // Alterar pixel exatamente no meio (para exemplo visual claro)
  int mid = base / 2;
  H->image[mid][mid] = 1; 
  ImageSavePBM(G, "Test/Test4/T4_img1.pbm");
  ImageSavePBM(H, "Test/Test4/T4_img2.pbm");
  ImageDestroy(&G);
  ImageDestroy(&H);
  printf("   [OK] T4 (Dif. Meio) gerado em Test/Test4/\n");

  // --- T5: TOTALMENTE DIFERENTES ---
  Image I = ImageCreate(base, base); // Branca
  Image J = ImageCreate(base, base); // Vamos pôr tudo Preto
  
  // Preencher J com cor 1 (Preto)
  for(uint32 y=0; y < J->height; y++) {
      for(uint32 x=0; x < J->width; x++) {
          J->image[y][x] = 1;
      }
  }
  // Garantir que (0,0) é diferente
  I->image[0][0] = 0;
  J->image[0][0] = 1;

  ImageSavePBM(I, "Test/Test5/T5_img1.pbm");
  ImageSavePBM(J, "Test/Test5/T5_img2.pbm");
  ImageDestroy(&I);
  ImageDestroy(&J);
  printf("   [OK] T5 (Total Dif) gerado em Test/Test5/\n");
}

// --- FUNÇÃO MAIN ---

int main(int argc, char* argv[]) { 
   program_name = argv[0];
  if (argc != 1) {
    error(1, 0, "Usage: imageRGBTest");
  }

  srand(time(NULL));
  ImageInit();
  
  printf("===========================================\n");
  printf("    IMAGE RGB MODULE - TEST SUITE\n");
  printf("===========================================\n");

  // Executar testes
  Test1_BasicOperations();
  Test2_FloodFill_Visual();
  Test3_GeometricProperties();
  Test4_SegmentationVisual();
  Test5_StressTest();
  Test6_ComplexityAnalysis();

   printf("\n[WARNING] Tem de se aproximar bastante a imagem ou prestar atenção porque são diferenças mínimas\n");

  printf("\n[SUCCESS] Todos os testes terminaram.\n");
  return 0;
}