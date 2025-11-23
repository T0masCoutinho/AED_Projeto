# aed2025-imageRGB

AED 2025 - Trabalho 1: Imagens com cor indexada (pseudocor)

# imageRGBTest - Suite de Testes

Este programa (`imageRGBTest.c`) é o motor de testes principal para o módulo **imageRGB**. O seu objetivo é validar a correção dos algoritmos, garantir a ausência de fugas de memória e gerar dados para a análise de complexidade.

## Estrutura do Projeto

O projeto já inclui a estrutura de pastas necessária para a execução. Os resultados dos testes (imagens geradas) serão guardados automaticamente nos seguintes diretórios:

- **`Test/basic/`**: Resultados dos testes visuais (FloodFill, Segmentação e Operações Básicas).
- **`Test/Test1/` a `Test/Test5/`**: Imagens geradas especificamente para a análise de complexidade (Comparação de algoritmos).

> **Nota:** Não é necessário criar diretórios manualmente; o projeto está pronto a compilar e executar.

---

## Como Compilar

Para compilar o programa, utilize o seguinte comando (garantindo que os ficheiros `imageRGB.c`, `error.c` e `instrumentation.c` estão presentes):

```bash
gcc -Wall -O2 -o imageRGBTest imageRGBTest.c imageRGB.c error.c instrumentation.c

🚀 Como Executar
Bash ./imageRGBTest


📋 Descrição dos Testes
Ao executar, o programa realiza 6 baterias de testes sequenciais:

1. Operações Básicas (Test1)
O que faz: Testa a criação de imagens, cópia (ImageCopy) e operações de I/O (SavePBM/SavePPM).

Verificação: Confirma se as imagens são gravadas corretamente na pasta Test/basic/.

2. Flood Fill - Visual (Test2)
O que faz: Compara visualmente os algoritmos de preenchimento: Recursivo, Stack (DFS) e Queue (BFS).

Verificação: Pode abrir as imagens geradas em Test/basic/ para confirmar que o preenchimento respeita as fronteiras.

3. Propriedades Geométricas (Test3)
O que faz: Valida a lógica matemática das rotações.

Teste de Identidade: Confirma se rodar 90º quatro vezes (ou 180º duas vezes) devolve a imagem ao estado original.

4. Segmentação (Test4)
O que faz: Conta regiões conexas (segmentação) num tabuleiro de xadrez gerado via código.

Validação: O algoritmo deve detetar automaticamente o número exato de regiões brancas isoladas (esperado: 10).

5. Stress Test (Test5)
O que faz: Teste de performance (Tempo de CPU).

Cenário: Executa operações de preenchimento em imagens de alta resolução (2000x2000) para comparar a eficiência da implementação com Stack vs Queue.

6. Casos para Análise de Complexidade (Test6)
Gera datasets (pares de imagens) nas pastas Test/TestX para suportar a análise da função de comparação (ImageIsEqual):

T1: Imagens iguais (Pior caso).

T2: Diferença no primeiro pixel (Melhor caso).

T3: Diferença no último pixel.

T4: Diferença no meio.

T5: Imagens totalmente diferentes.

🔍 Visualização dos Resultados
Os ficheiros de saída (.ppm e .pbm) localizados na pasta Test/ podem ser visualizados utilizando ferramentas como GIMP, IrfanView ou extensões de visualização de imagem do VS Code.
```
