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

    make

## Como Executar

    ./imageRGBTest

## Descrição Detalhada dos Testes

    Dica de Visualização: Algumas alterações são subtis (ex: mudança de cor num único pixel). Recomenda-se o uso de zoom ou a verificação dos logs no terminal.

    Ao executar, o programa realiza 7 baterias de testes sequenciais:

## 1. Operações Básicas (Test1)

    Objetivo: Validar a gestão de memória básica, manipulação de estruturas e I/O.

    Descrição: Testa as funções de criação (ImageCreate), cópia profunda (ImageCopy) e gravação de ficheiros (SavePBM/SavePPM).

    Verificação: Confirma se os ficheiros são criados corretamente na pasta Test/basic/ e se são legíveis.

## 2. Flood Fill - Visual (Test2)

    Objetivo: Garantir a correção visual dos algoritmos de preenchimento de regiões.

    Descrição: Aplica os três métodos (Recursivo, Stack/DFS, Queue/BFS) num tabuleiro de xadrez e compara os resultados.

    Verificação: As imagens geradas devem mostrar as regiões brancas (ou pretas) perfeitamente preenchidas, respeitando as fronteiras dos quadrados.

## 3. Propriedades Geométricas (Test3)

    Objetivo: Verificar a integridade matemática das operações de matriz.

    Descrição: Testa a identidade das rotações:

    Rodar 90º quatro vezes deve resultar na imagem original.

    Rodar 180º duas vezes deve resultar na imagem original.

    Resultado: O teste passa se ImageIsEqual retornar verdadeiro.

## 4. Segmentação (Test4)

    Objetivo: Validar o algoritmo de contagem de componentes conexos.

    Descrição: Utiliza um tabuleiro de xadrez gerado via código (onde o número de quadrados é conhecido) e aplica a segmentação.

    Verificação: O algoritmo deve retornar exatamente 10 regiões (para o tamanho de tabuleiro configurado).

## 5. Test de Performance (Test5)

    Objetivo: Comparar a eficiência temporal e a robustez de memória (Stack vs Queue vs Recursive).

    Descrição: Executa operações de preenchimento em imagens de alta resolução (2000x2000).

    Cenário: Compara o tempo de CPU entre a implementação com Stack, Queue e Recursiva (com proteções de segurança para evitar Stack Overflow na recursão).

## 6. Casos para Análise de Complexidade (Test6)

    Objetivo: Gerar datasets controlados para a análise empírica da função ImageIsEqual.

    Descrição: Cria pares de imagens nas pastas Test/TestX representando diferentes cenários:

    T1: Imagens iguais (Pior caso - percorre tudo).

    T2: Diferença no primeiro pixel (Melhor caso).

    T3: Diferença no último pixel.

    T4: Diferença no meio.

    T5: Imagens totalmente diferentes.

## 7. Labirinto em Espiral - "Stress Test" Visual (Test7)

    Objetivo: Demonstrar a robustez dos algoritmos em cenários de "pior caso" de caminho (profundidade máxima).

    Descrição: Gera um labirinto complexo em espiral (300x300) que obriga o algoritmo a percorrer um caminho único e longo da periferia até ao centro.

    Visualização: O caminho original (preto) é preenchido a Amarelo (0xFFFF00) nas imagens Test/basic/spiral\_\*.ppm.

    Nota Técnica: Este teste expõe claramente as limitações da recursão simples e a eficácia das abordagens iterativas (Stack/Queue).

    Aviso de Autoria (Test7): A função auxiliar ImageCreateSpiral foi incluída neste ficheiro de testes especificamente para validação extrema e robustez. Não faz parte do módulo nuclear imageRGB.c mas utiliza as suas funções para operar.

## Visualização dos Resultados

Os ficheiros de saída (.ppm e .pbm) localizados na pasta Test/ podem ser visualizados utilizando ferramentas como GIMP, IrfanView ou extensões de visualização de imagem do VS Code (recomendo mais por praticidade).
