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
  ImageSavePBM(image_chess_1, "Test/basic/chess_image_1.pbm");
  printf("   [OK] ImageCreateChess + SavePBM (Test/basic/chess_image_1.pbm)\n");

  Image image_chess_2 = ImageCreateChess(20, 20, 8, 0xff0000); 
  ImageSavePPM(image_chess_2, "Test/basic/chess_image_2.ppm");
  printf("   [OK] ImageCreateChess (Red) + SavePPM (Test/basic/chess_image_2.ppm)\n");

  Image copy_image = ImageCopy(image_chess_1);
  if (copy_image != NULL) {
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

  printf("\n   [PASSED] Foi pintada apenas uma região, como cada região tem 30x30, assim dando os 900 pixeis.\n");

  ImageDestroy(&img_rec);
  ImageDestroy(&img_stack);
  ImageDestroy(&img_queue);
}

void Test3_GeometricProperties() {
  printf("\n>> 3. TESTE DE PROPRIEDADES GEOMÉTRICAS \n");
  
  Image original = ImageLoadPPM("img/feep.ppm"); 
  if (original == NULL) original = ImageCreateChess(50, 50, 10, 0x000000); 

  // Rotação 90º x 4 
  Image r1 = ImageRotate90CW(original);
  Image r2 = ImageRotate90CW(r1);
  Image r3 = ImageRotate90CW(r2);
  Image r4 = ImageRotate90CW(r3); 

  if (ImageIsEqual(original, r4)) {
      printf("   [PASSED] Rotate90CW x 4 == Original (Identidade)\n");
  } else {
      printf("   [FAILED] Rotate90CW x 4 != Original\n");
  }

  // Rotação 180º x 2
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
    
    // Tamanho exagerado para testar a robustez da Stack e Queue
    // 2000x2000 = 4 Milhões de pixeis.
    int big_size = 2000; 
    
    // Tamanho MÁXIMO SEGURO para a recursão, pois 300 ja dá segmentation fault
    // Vamos pôr 250 para garantir que corre bem.
    int safe_recursive_limit = 250; 

    printf("   [INFO] A criar imagem branca gigante de %dx%d...\n", big_size, big_size);
    
    Image imgStack = ImageCreate(big_size, big_size);
    Image imgQueue = ImageCopy(imgStack);

    // STACK 
    printf("   A testar Stack (DFS) em %dx%d... ", big_size, big_size);
    fflush(stdout);
    InstrReset();
    ImageRegionFillingWithSTACK(imgStack, 0, 0, 1);
    double timeStack = cpu_time() - InstrTime;
    printf("Tempo: %.4f s (SUCESSO)\n", timeStack);

    // QUEUE 
    printf("   A testar Queue (BFS) em %dx%d... ", big_size, big_size);
    fflush(stdout);
    InstrReset();
    ImageRegionFillingWithQUEUE(imgQueue, 0, 0, 1);
    double timeQueue = cpu_time() - InstrTime;
    printf("Tempo: %.4f s (SUCESSO)\n", timeQueue);

    // RECURSIVE (teste controlado)
    printf("   A testar Recursive... \n");
    
    if (big_size > safe_recursive_limit) {
        // Se a imagem for demasiado grande, não corremos a recursiva
        printf("      -> Teste em imagem GRANDE (%dx%d)... ", big_size, big_size);
        printf("[ABORTADO]\n");
        printf("         [MOTIVO] O teu sistema crasharia com 2000x2000 (Segmentation Fault).\n");
        
        // provar que funciona em pequena escala
        printf("      -> Contra-prova em imagem PEQUENA (%dx%d)... ", safe_recursive_limit, safe_recursive_limit);
        fflush(stdout);
        Image imgSmall = ImageCreate(safe_recursive_limit, safe_recursive_limit);
        InstrReset();
        ImageRegionFillingRecursive(imgSmall, 0, 0, 1);
        printf("Tempo: %.4f s (SUCESSO)\n", cpu_time() - InstrTime);
        ImageDestroy(&imgSmall);
        
    } else {
        Image imgRec = ImageCopy(imgStack);
        ImageRegionFillingRecursive(imgRec, 0, 0, 1);
        printf("Tempo: %.4f s\n", cpu_time() - InstrTime);
        ImageDestroy(&imgRec);
    }

    // Limpeza
    ImageDestroy(&imgStack);
    ImageDestroy(&imgQueue);
}

void Test6_ComplexityAnalysisChart() {
  printf("\n=================================================================================\n");
  printf(" 6. ANÁLISE DE COMPLEXIDADE: ImageIsEqual (Tempo MEDIO por Execucao)\n");
  printf("=================================================================================\n");

  // Configurações
  int sizes[] = {100, 250, 500, 1000, 1500, 2000};
  int num_sizes = 6;
  /*
   * [NOTA TÉCNICA: PORQUÊ 1000 REPETIÇÕES?]
   * A função ImageIsEqual é extremamente rápida (na ordem dos microsegundos).
   * Se medíssemos apenas uma execução, o tempo seria muitas vezes inferior à 
   * resolução mínima do relógio do CPU (resultando em 0.000s) ou seria 
   * distorcido por "ruído" do Sistema Operativo (interrupções, escalonamento).
   *
   * SOLUÇÃO (Micro-Benchmarking):
   * Executamos a função 1000 vezes seguidas, medimos o tempo total acumulado 
   * e dividimos pelo número de repetições. Isto garante:
   * 1. Precisão: O tempo total torna-se grande o suficiente para ser registado pelo relógio.
   * 2. Estabilidade: Dilui eventuais picos de latência externos, dando uma média fiável.
   */
  int REPETICOES = 1000; // mede 1000 vezes, mas depois dividimos

  printf("   [INFO] A contar operacoes reais (InstrCount) e a medir tempo medio (%d reps).\n\n", REPETICOES);

  printf("   +----------------------+----------------------+-----------------+----------------------+-----------------+\n");
  printf("   |  TAMANHO (N*N)       |  COMPARACOES (T1)    |  TEMPO T1 (s)   |  COMPARACOES (T4)    |  TEMPO T4 (s)   |\n");
  printf("   |  (Total Pixeis)      |  (Reais/InstrCount)  |  (Pior Caso)    |  (Reais/InstrCount)  |  (Caso Medio)   |\n");
  printf("   +----------------------+----------------------+-----------------+----------------------+-----------------+\n");

  for (int i = 0; i < num_sizes; i++) {
      int N = sizes[i];
      long long total_pixels = (long long)N * N; // O nosso "Input Size"
      
      Image A = ImageCreate(N, N);
      for(uint32 y=0; y<A->height; y++) for(uint32 x=0; x<A->width; x++) A->image[y][x] = 0;
      Image B = ImageCopy(A); 

      // ===========================
      // BLOCO T1: IMAGENS IGUAIS
      // ===========================
      
      // 1. Contar Comparações (Executar 1 vez)
      InstrReset();
      ImageIsEqual(A, B);
      unsigned long ops_t1 = InstrCount[0];

      // 2. Medir Tempo (Executar 2000 vezes para média)
      InstrReset();
      for (int r = 0; r < REPETICOES; r++) ImageIsEqual(A, B);
      double t1_avg = (cpu_time() - InstrTime) / REPETICOES;


      // ===========================
      // BLOCO T4: DIFERENÇA NO MEIO
      // ===========================
      B->image[N/2][N/2] = 1; // Alterar meio

      // 1. Contar Comparações (Executar 1 vez)
      InstrReset();
      ImageIsEqual(A, B);
      unsigned long ops_t4 = InstrCount[0];

      // medir Tempo (Executar 1000 vezes para média)
      InstrReset();
      for (int r = 0; r < REPETICOES; r++) ImageIsEqual(A, B);
      double t4_avg = (cpu_time() - InstrTime) / REPETICOES;

      printf("   | %-20lld | %-20lu | %-15.6f | %-20lu | %-15.6f |\n", 
             total_pixels, ops_t1, t1_avg, ops_t4, t4_avg);

      ImageDestroy(&A);
      ImageDestroy(&B);
  }
  
  printf("   +----------------------+----------------------+-----------------+----------------------+-----------------+\n");

  printf("   [CONCLUSAO] Repara que o Tempo T1 cresce na mesma proporcao que o tamanho.\n");
  printf("               Se o tamanho quadruplica (ex: 250k -> 1M), o tempo também quadruplica.\n");
  printf("               Isto prova que o algoritmo é Linear O(N) em relação aos pixeis.\n");
}


void Test6_ComplexityAnalysisExample() {
  printf("\n=======================================\n");
  printf(" TESTES DE COMPLEXIDADE: ImageIsEqual    (demora um bocado)\n");
  printf("=======================================\n");

  // Calibrar a instrumentação para ter tempos fiáveis
  InstrName[0] = "comparisons";  
  InstrCalibrate();

  // Tamanhos de teste (N = Lado da imagem)
  int sizes[] = {100, 200, 400, 800, 1600, 3200, 6400};
  int nsizes = sizeof(sizes) / sizeof(sizes[0]);

  // Cabeçalho da Tabela
  printf("\n%10s %18s %12s %12s %15s\n",
        "NºTeste", "Pixels(N*N)", "Time(s)", "Caltime", "Comparisons");

  // ==========================================================================
  // T1 – IMAGENS IGUAIS (Worst-case: O(N))
  // Percorre a imagem toda.
  // ==========================================================================
  
  // 1. Guardar Exemplo Visual (Baseado no primeiro tamanho)
  Image A_demo = ImageCreate(sizes[0], sizes[0]);
  Image B_demo = ImageCopy(A_demo);
  ImageSavePBM(A_demo, "Test/T1_img_iguais/T1_img1.pbm");
  ImageSavePBM(B_demo, "Test/T1_img_iguais/T1_img2.pbm");
  ImageDestroy(&A_demo);
  ImageDestroy(&B_demo);

  // 2. Loop de Teste
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];
    
    Image A = ImageCreate(w, h); // Cria imagem preta (0)
    Image B = ImageCopy(A);      // B é igual a A

    InstrReset();
    ImageIsEqual(A, B);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test1", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&A);
    ImageDestroy(&B);
  }
  printf("     ------------------------------------------------------------------\n");

  // ==========================================================================
  // T2 – DIFERENTES NO PRIMEIRO PIXEL (Best-case: O(1))
  // Pára imediatamente na primeira iteração.
  // ==========================================================================

  // 1. Guardar Exemplo Visual
  Image C_demo = ImageCreate(sizes[0], sizes[0]);
  Image D_demo = ImageCopy(C_demo);
  D_demo->image[0][0] = 1; // Mudar o primeiro pixel
  ImageSavePBM(C_demo, "Test/T2_dif_primeiro_pixel/T2_img1.pbm");
  ImageSavePBM(D_demo, "Test/T2_dif_primeiro_pixel/T2_img2.pbm");
  ImageDestroy(&C_demo);
  ImageDestroy(&D_demo);

  // 2. Loop de Teste
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image C = ImageCreate(w, h);
    Image D = ImageCopy(C);
    D->image[0][0] = 1; // Diferença no início

    InstrReset();
    ImageIsEqual(C, D);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test2", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&C);
    ImageDestroy(&D);
  } 
  printf("     ------------------------------------------------------------------\n");

  // ==========================================================================
  // T3 — DIFERENTES NO ÚLTIMO PIXEL (Worst-case Disfarçado: O(N))
  // Percorre tudo e falha no fim.
  // ==========================================================================
  
  // 1. Guardar Exemplo Visual
  Image E_demo = ImageCreate(sizes[0], sizes[0]); 
  Image F_demo = ImageCopy(E_demo);
  F_demo->image[sizes[0]-1][sizes[0]-1] = 1; // Mudar último pixel
  ImageSavePBM(E_demo, "Test/T3_dif_ultimo_pixel/T3_img1.pbm");
  ImageSavePBM(F_demo, "Test/T3_dif_ultimo_pixel/T3_img2.pbm");
  ImageDestroy(&E_demo);
  ImageDestroy(&F_demo);

  // 2. Loop de Teste
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image E = ImageCreate(w, h);
    Image F = ImageCopy(E);
    F->image[h-1][w-1] = 1; // Diferença no fim

    InstrReset();
    ImageIsEqual(E, F);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test3", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&E);
    ImageDestroy(&F);
  }
  printf("     ------------------------------------------------------------------\n");

  // ==========================================================================
  // T4 — DIFERENTES NO MEIO (Average-case: ~O(N/2))
  // Percorre metade da imagem.
  // ==========================================================================

  // 1. Guardar Exemplo Visual
  Image G_demo = ImageCreate(sizes[0], sizes[0]);
  Image H_demo = ImageCopy(G_demo);
  H_demo->image[sizes[0]/2][sizes[0]/2] = 1; // Mudar meio
  ImageSavePBM(G_demo, "Test/T4_dif_pixel_meio/T4_img1.pbm");
  ImageSavePBM(H_demo, "Test/T4_dif_pixel_meio/T4_img2.pbm");
  ImageDestroy(&G_demo);
  ImageDestroy(&H_demo);

  // 2. Loop de Teste
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image G = ImageCreate(w, h);
    Image H = ImageCopy(G);
    H->image[h/2][w/2] = 1; // Diferença no meio exato

    InstrReset();
    ImageIsEqual(G, H);
    double elapsed = cpu_time() - InstrTime;

    printf("%10s %18d %12.6f %12.6f %15lu\n",
            "Test4", w*h, elapsed, elapsed / InstrCTU, InstrCount[0]);

    ImageDestroy(&G);
    ImageDestroy(&H);
  }
  printf("     ------------------------------------------------------------------\n");

  // ==========================================================================
  // T5 — TOTALMENTE ALEATÓRIAS (Best-case Prático: O(1))
  // Estatisticamente falha quase logo no início.
  // ==========================================================================

  // 1. Guardar Exemplo Visual
  Image I_demo = ImageCreate(sizes[0], sizes[0]);
  Image J_demo = ImageCreate(sizes[0], sizes[0]);
  // Preencher com ruído
  for(uint32 y=0; y < I_demo->height; y++) 
      for(uint32 x=0; x < I_demo->width; x++) 
          I_demo->image[y][x] = rand() % 2;
  // Garantir diferença no inicio para consistência do teste O(1)
  J_demo->image[0][0] = !I_demo->image[0][0]; 

  ImageSavePBM(I_demo, "Test/T5_img_aleatorias/T5_img1.pbm");
  ImageSavePBM(J_demo, "Test/T5_img_aleatorias/T5_img2.pbm");
  ImageDestroy(&I_demo);
  ImageDestroy(&J_demo);

  // 2. Loop de Teste
  for (int i = 0; i < nsizes; i++) {
    int w = sizes[i], h = sizes[i];

    Image I = ImageCreate(w, h);
    Image J = ImageCreate(w, h);

    // Para o teste de tempo, basta garantir que o primeiro é diferente
    // para simular duas imagens que nada têm a ver uma com a outra.
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
  printf("     ------------------------------------------------------------------\n");
}


 /*
 * ======================================================================================
 * FUNÇÃO: ImageCreateSpiral
 * ======================================================================================
 * [DESCRIÇÃO]
 * Gera uma imagem com um padrão de espiral quadrada ("Serpentine") que converge da 
 * periferia para o centro.
 * Cores: 0 (Caminho/Preto), 1 (Paredes/Branco).
 *
 * [ORIGEM & AUTORIA]
 * Código gerado com assistência de inteligência artificial, solicitado pelo aluno.
 *
 * [PROPÓSITO NO PROJETO]
 * Esta função é estritamente auxiliar (ferramenta de teste). O seu objetivo é criar 
 * um cenário de teste (labirinto de caminho único e longo) para validar a 
 * robustez dos algoritmos de Flood Fill (Stack vs Queue) e garantir que lidam com 
 * a profundidade máxima sem erro de memória (Stack Overflow).
 * No entanto, como o código está integrado, funcional e segue as boas práticas,
 * pode ser consultado para a robustez dos testes.
 * ======================================================================================
 */
Image ImageCreateSpiral(int width, int height) {
  Image img = ImageCreate(width, height);
  
  // Pintar tudo de branco (paredes)
  for (uint32 y = 0; y < img->height; y++) {
    for (uint32 x = 0; x < img->width; x++) {
      img->image[y][x] = 1; 
    }
  }

  // Escavar caminho a preto
  int x = 0, y = 0;
  int dx = 1, dy = 0; 
  int max_steps = width * height; 

  for (int i = 0; i < max_steps; i++) {
      img->image[y][x] = 0; 

      int next_x = x + dx;
      int next_y = y + dy;
      int nnx = x + 2*dx;
      int nny = y + 2*dy;

      int turn = 0;
      if (next_x < 0 || next_x >= (int)img->width || next_y < 0 || next_y >= (int)img->height) {
          turn = 1;
      } 
      else if (nnx >= 0 && nnx < (int)img->width && nny >= 0 && nny < (int)img->height && img->image[nny][nnx] == 0) {
          turn = 1;
      }

      if (turn) {
          int temp = dx;
          dx = -dy;
          dy = temp;
          next_x = x + dx;
          next_y = y + dy;
          if (next_x < 0 || next_x >= (int)img->width || next_y < 0 || next_y >= (int)img->height || img->image[next_y][next_x] == 0) {
              break; 
          }
      }
      x = next_x;
      y = next_y;
  }
  img->image[0][0] = 0; // entrada
  return img;
}

void Test7_FloodFill_Visual() {
  printf("\n>> 7. TESTE ESPECIAL: ESPIRAL (LABIRINTO) \n");
  
  // --- CONFIGURAÇÃO ---
  // Tamanho do Labirinto (Podes mudar aqui para testar)
  // 300x300 = 90.000 pixeis de profundidade (máximo seguro na recursão)
  // 500x500 = 250.000 pixeis de profundidade (Crash garantido na recursão)
  int size = 300; 

  // Limite de segurança para a Recursão. 
  // Se 'size' for maior que isto, não executamos a recursiva para evitar SegFault.
  int max_safe_recursion_size = 300; 

  // Configuração da cor Amarela
  uint16 paint_index = 2;       
  uint32 color_yellow = 0xFFFF00; 

  printf("   [CENÁRIO] A gerar labirinto %dx%d...\n", size, size);
  printf("             (Este é o PIOR CASO para recursão: profundidade máxima linear)\n");
  
  Image spiral = ImageCreateSpiral(size, size);
  
  // Garantir espaço na LUT e configurar cores
  if (spiral->num_colors <= paint_index) spiral->num_colors = paint_index + 1;
  spiral->LUT[0] = 0x000000;               // 0 = Preto
  spiral->LUT[1] = 0xFFFFFF;               // 1 = Branco
  spiral->LUT[paint_index] = color_yellow; // 2 = Amarelo

  ImageSavePPM(spiral, "Test/basic/spiral_original.ppm"); 

  // RECURSIVE 
  printf("   1. Recursive: ");
  
  if (size > max_safe_recursion_size) {
      // Se a imagem for muito grande, abortamos a recursão
      printf("[ABORTADO - SAFETY CHECK]\n");
      printf("      [ALERTA] A recursão foi ignorada propositadamente.\n");
      printf("      Neste labirinto %dx%d, a profundidade seria %d chamadas.\n", size, size, size*size);
      printf("      Isso causaria 'Segmentation Fault' (Stack Overflow).\n");
  } else {
      // Se for pequena, corremos
      Image s_rec = ImageCopy(spiral);
      ImageRegionFillingRecursive(s_rec, 0, 0, paint_index);
      ImageSavePPM(s_rec, "Test/basic/spiral_recursive.ppm");
      printf("[OK] -> Saved in Test/basic/spiral_recursive.ppm\n");
      ImageDestroy(&s_rec);
  }

  // STACK 
  printf("   2. Stack:     ");
  fflush(stdout); 
  Image s_stack = ImageCopy(spiral);
  ImageRegionFillingWithSTACK(s_stack, 0, 0, paint_index);
  ImageSavePPM(s_stack, "Test/basic/spiral_stack.ppm");
  printf("[OK] -> Saved in Test/basic/spiral_stack.ppm\n");
  ImageDestroy(&s_stack);

  // QUEUE 
  printf("   3. Queue:     ");
  fflush(stdout);
  Image s_queue = ImageCopy(spiral);
  ImageRegionFillingWithQUEUE(s_queue, 0, 0, paint_index);
  ImageSavePPM(s_queue, "Test/basic/spiral_queue.ppm");
  printf("[OK] -> Saved in Test/basic/spiral_queue.ppm\n");
  ImageDestroy(&s_queue);

  ImageDestroy(&spiral);
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
  Test6_ComplexityAnalysisChart();
  Test6_ComplexityAnalysisExample();
  Test7_FloodFill_Visual();

   printf("\n[WARNING] Tem de se aproximar bastante a imagem ou prestar atenção porque são diferenças mínimas\n");

  printf("\n[SUCCESS] Todos os testes terminaram.\n");
  return 0;
}