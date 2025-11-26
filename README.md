# aed2025-imageRGB

AED 2025 - Trabalho 1: Imagens com cor indexada (pseudocor)

# imageRGBTest - Suite de Testes

Este programa (`imageRGBTest.c`) é o motor de testes principal para o módulo **imageRGB**. O seu objetivo é validar a correção dos algoritmos, garantir a ausência de fugas de memória e gerar dados para a análise de complexidade.

## Estrutura do Projeto

O projeto já inclui a estrutura de pastas necessária para a execução. Os resultados dos testes (imagens geradas) serão guardados automaticamente nos seguintes diretórios:

- **`Test/1/` a `Test/Test6/`**: Resultados dos testes visuais (FloodFill, Segmentação e Operações Básicas).
- **`Test/7/`**: Imagens geradas especificamente para a análise de complexidade (Comparação de algoritmos).

> **Nota:** Não é necessário criar diretórios manualmente; o projeto está pronto a compilar e executar.

---

## Como Compilar

    make

## Como Executar

    ./imageRGBTest

## Descrição Detalhada dos Testes

    Dica de Visualização: Algumas alterações são subtis (ex: mudança de cor num único pixel). Recomenda-se o uso de zoom ou a verificação dos logs no terminal.

    Ao executar, o programa realiza 7 baterias de testes sequenciais:

## 1. Operações Simples (Test1)

    Objetivo: Validar a gestão de memória básica, manipulação de estruturas.

    Descrição: Testa as funções de criação (ImageCreate), cópia profunda (ImageCopy) e gravação de ficheiros (SavePBM/SavePPM).

    Verificação: Confirma se os ficheiros são criados corretamente na pasta Test/1/ e se são legíveis.

## 2. ImageRotate (Test2) MUDAR

    Objetivo: Verificar a integridade matemática das operações de matriz e coordenadas.

    Descrição: Testa a propriedade de identidade das rotações:

    Rodar 90º (CW) quatro vezes deve resultar na imagem original.

    Rodar 180º duas vezes deve resultar na imagem original.

    Verificação: O teste passa automaticamente se ImageIsEqual retornar verdadeiro (comparação pixel a pixel).

## 3. FillingNoise (Test3)

    Objetivo: Validação visual do Flood Fill em cenários irregulares.

    Descrição: Gera uma imagem com ruído aleatório (obstáculos pretos e espaço branco) e aplica o algoritmo de preenchimento a partir do centro.

    Verificação: Visualizar a pasta Test/3/. A tinta (cor vermelha/amarela) deve ter-se espalhado de forma orgânica pelos espaços brancos conexos, contornando os obstáculos pretos.

## 4. FillingSpiral (Test4)

    Objetivo: Demonstrar a robustez dos algoritmos em cenários de "pior caso" de caminho (profundidade máxima).

    Descrição: Gera um labirinto complexo em espiral ("Serpentina") usando a função auxiliar ImageCreateSpiral. Isto obriga o algoritmo a percorrer um caminho único e longo da periferia até ao centro.

    Nota Técnica: Este teste expõe a limitação da Recursão (que é abortada ou falha por Stack Overflow) e comprova a eficácia das abordagens iterativas (Stack/Queue) que completam a tarefa com sucesso.

    Visualização: Resultados em Test/4/.

## 5. Segmentação (Test5)

    Objetivo: Validar o algoritmo de contagem de componentes conexos (ImageSegmentation).

    Descrição: Utiliza um tabuleiro de xadrez gerado via código onde o número de quadrados brancos é matematicamente conhecido.

    Verificação: O algoritmo deve retornar a contagem exata de regiões (ex: 10 regiões) e gerar imagens onde cada quadrado tem uma cor diferente em Test/5/.

## 6. StressTest (Test6)

    Objetivo: Comparar a eficiência temporal e a robustez de memória em imagens grandes.

    Descrição: Executa operações de preenchimento em imagens de alta resolução (2000x2000 pixeis, total de 4 Milhões).

    Cenário: Mede o tempo de CPU das implementações Iterativas (Stack e Queue). A implementação Recursiva é protegida/limitada neste teste para evitar o crash do programa.

## 7. Análise complexa com tabela (Test7)

    Objetivo: Análise empírica da complexidade temporal O(N).

    Descrição: Executa a função ImageIsEqual repetidamente (micro-benchmarking) para vários tamanhos de imagem (de 100x100 a 2000x2000).

    Resultado: Imprime no terminal uma tabela comparativa entre o número de pixeis e o tempo de execução, demonstrando o crescimento linear.

## 8. Análise complexa dos Casos1 a Casos5 (especificos) (Test8)

    Objetivo: Análise granular dos cenários de Melhor, Pior e Caso Médio.

    Descrição: Mede o número exato de comparações (InstrCount) e o tempo para cenários desenhados especificamente:

        Caso1/Caso3 (Pior Caso): Imagens iguais ou diferentes no fim (O(N)).

        Caso2/Caso5 (Melhor Caso): Diferença no início ou aleatórias (O(1)).

        Caso4 (Caso Médio): Diferença no meio (O(N/2)).

    Visualização: Gera também os exemplos visuais destes casos (ficheiros .pbm) na pasta Test/7/ (ou Test/8/ dependendo da tua config).

## Visualização dos Resultados

Os ficheiros de saída (.ppm e .pbm) localizados na pasta Test/ podem ser visualizados utilizando ferramentas como GIMP, IrfanView ou extensões de visualização de imagem do VS Code (recomendo mais por praticidade).
