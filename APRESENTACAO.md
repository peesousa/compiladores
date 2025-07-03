# Roteiro de Apresentação do Projeto de Compilador

## Introdução

Olá a todos. Hoje, vamos mergulhar no fascinante mundo dos compiladores, apresentando um projeto que desenvolvemos. Um compilador é um programa que traduz um código-fonte escrito em uma linguagem de programação (linguagem de alto nível) para uma linguagem de máquina ou outra forma de código que um computador possa entender e executar.

O processo de compilação é tradicionalmente dividido em várias fases, cada uma com um objetivo específico:

1.  **Análise Léxica (Scanning):** Quebra o código-fonte em unidades atômicas chamadas tokens.
2.  **Análise Sintática (Parsing):** Verifica a estrutura gramatical do programa e constrói uma representação hierárquica (Árvore Sintática).
3.  **Análise Semântica:** Verifica o significado do programa, garantindo que ele faça sentido e siga as regras da linguagem (ex: tipos de dados, declaração de variáveis).
4.  **Geração de Código Intermediário:** Traduz a representação do programa para uma forma mais abstrata e independente da máquina.
5.  **Otimização de Código:** Melhora o código intermediário para torná-lo mais eficiente.
6.  **Geração de Código Objeto:** Traduz o código otimizado para a linguagem de máquina específica.

Nosso projeto foca nas três primeiras fases (Análise Léxica, Sintática e Semântica), que compõem o que chamamos de *frontend* do compilador. Vamos explorar a estrutura do nosso compilador, passando por cada arquivo para entender sua função, a lógica por trás de sua implementação e como ele se encaixa no contexto teórico de um compilador real.

## Estrutura do Projeto

O projeto está organizado da seguinte forma:

-   `main.c`: O ponto de entrada do nosso compilador.
-   `Makefile`: Define como o projeto é compilado e construído.
-   `includes/`: Contém os arquivos de cabeçalho (`.h`) que definem as estruturas de dados e as interfaces dos nossos módulos.
-   `src/`: Contém os arquivos de código-fonte (`.c`) que implementam a lógica do compilador.
-   `README.md`: A documentação principal do projeto.

Vamos agora mergulhar em cada um desses componentes.

---

## `Makefile`

**Funcionalidade Geral:**

O `Makefile` é o nosso script de construção. Ele automatiza o processo de compilação do código-fonte em um executável final. A ideia aqui foi criar um sistema de build que fosse eficiente e fácil de usar.

**Raciocínio da Construção:**

-   **Variáveis:** Definimos variáveis como `CC` (o compilador C), `CFLAGS` (as flags de compilação, como `-Wall` para avisos e `-Iincludes` para que o compilador encontre nossos cabeçalhos), e `TARGET` (o nome do nosso executável).
-   **Detecção de Fontes:** Usamos `wildcard` para encontrar automaticamente todos os arquivos `.c` no diretório `src/`, tornando o `Makefile` escalável. Se adicionarmos novos arquivos, ele os compilará sem precisar de modificações.
-   **Compilação Separada:** O projeto compila cada arquivo `.c` em um arquivo-objeto (`.o`) separado. Isso é eficiente porque, se alterarmos apenas um arquivo-fonte, apenas ele precisa ser recompilado, e não o projeto inteiro. Os objetos são armazenados no diretório `obj/`.
-   **Regra `all`:** A regra principal, que constrói o `TARGET` final. Ela depende de todos os arquivos-objeto.
-   **Regra de Ligação:** Esta regra (`$(TARGET): $(OBJECTS)`) liga todos os arquivos-objeto para criar o executável final.
-   **Regra de Limpeza (`clean`):** Uma regra de conveniência para remover os arquivos gerados (o executável e o diretório de objetos), mantendo o projeto limpo.

---

## `includes/token.h` e `src/token.c`

**Teoria da Análise Léxica:**

A Análise Léxica, também conhecida como *scanning*, é a primeira fase de um compilador. Seu principal objetivo é ler o código-fonte caractere por caractere e agrupá-los em sequências significativas chamadas **lexemas**. Cada lexema é então classificado em uma categoria, resultando em um **token**. Pense nos tokens como as "palavras" de uma linguagem de programação.

Os tokens são definidos por **padrões** (geralmente expressos por expressões regulares) que descrevem a forma dos lexemas. Por exemplo, o padrão para um identificador pode ser "uma letra seguida por zero ou mais letras ou dígitos".

**Funcionalidade Geral:**

Estes arquivos definem o que é um "token". Um token é a menor unidade de significado em nosso código-fonte. Por exemplo, uma palavra-chave como `if`, um número como `123`, ou um operador como `+`.

**`includes/token.h`:**

-   **`TokenType` (enum):** Este é o coração do analisador léxico. É uma enumeração que define todos os tipos de tokens que nossa linguagem reconhece. A ideia foi criar uma lista exaustiva de todas as "palavras" e símbolos que o compilador pode entender, desde palavras-chave (`IF`, `WHILE`) até literais (`INTEGER_LITERAL`) e operadores.
-   **`Token` (struct):** Esta estrutura representa um token individual. Ela armazena o `type` (da enumeração), o `lexeme` (o texto real do código-fonte, como "if" ou "123"), e a `line` (a linha onde o token apareceu, o que é crucial para mensagens de erro).

**`src/token.c`:**

-   **`token_type_to_string`:** Uma função auxiliar muito útil para depuração. Ela converte um `TokenType` em uma string legível. A lógica é um simples `switch-case` que mapeia cada valor da enumeração para seu nome em formato de string. Isso nos ajuda a imprimir os tokens de forma clara durante a análise.

---

## `includes/lexer.h` e `src/lexer.c`

**Teoria da Análise Léxica (Continuação):**

O analisador léxico, ou *scanner*, é frequentemente implementado usando **autômatos finitos**. Cada padrão de token (como identificadores, números, palavras-chave) pode ser representado por uma expressão regular, e cada expressão regular pode ser convertida em um autômato finito (determinístico ou não determinístico). O lexer, então, simula o comportamento desse autômato para reconhecer os lexemas no fluxo de entrada.

**Funcionalidade Geral:**

O "lexer" (ou analisador léxico) é o primeiro estágio do nosso compilador. Sua função é ler o código-fonte como uma string de caracteres e dividi-lo em uma sequência de tokens.

**Raciocínio da Construção (`src/lexer.c`):**

-   **`LexerState` (struct):** Para gerenciar o processo de tokenização, criei uma estrutura `LexerState`. Ela mantém o controle de onde estamos no código-fonte (`start` e `current`), a linha atual (`line`), e o array de tokens que estamos construindo.
-   **`tokenize` (função principal):** Esta função percorre o código-fonte caractere por caractere. A lógica principal é um grande `switch-case` que decide o que fazer com base no caractere atual:
    -   **Espaço em branco:** É ignorado.
    -   **Quebra de linha:** Incrementa o contador de linha.
    -   **Símbolos de um caractere:** Como `(`, `)`, `+`, `;`, são imediatamente transformados em tokens.
    -   **Símbolos de múltiplos caracteres:** Para operadores como `==`, `!=`, `<`, `<=`, o código verifica o próximo caractere para decidir qual token criar. Por exemplo, se vemos um `<`, olhamos para frente. Se o próximo for um `=`, criamos um token `TOKEN_LTE`; caso contrário, é um `TOKEN_LT`.
    -   **Números:** Se encontramos um dígito, chamamos a função `number`. Ela consome todos os dígitos subsequentes e também trata de números de ponto flutuante (se houver um `.` seguido por mais dígitos).
    -   **Identificadores e Palavras-chave:** Se encontramos uma letra ou `_`, chamamos a função `identifier`. Ela consome todos os caracteres alfanuméricos subsequentes. Depois, verificamos se o identificador corresponde a alguma palavra-chave (como "if", "while") em nossa lista de `keywords`. Se corresponder, criamos um token de palavra-chave; senão, é um `TOKEN_IDENTIFIER`.
-   **Gerenciamento de Memória:** O lexer aloca dinamicamente a memória para a lista de tokens, redimensionando-a conforme necessário. Isso evita limites fixos e torna o compilador mais robusto.

---

## `includes/symtab.h` e `src/symtab.c`

**Teoria da Tabela de Símbolos:**

A Tabela de Símbolos é uma estrutura de dados fundamental em compiladores, utilizada principalmente nas fases de análise sintática e semântica. Ela atua como um repositório central para armazenar informações sobre todos os identificadores (variáveis, funções, tipos, etc.) encontrados no código-fonte. As informações armazenadas incluem o nome do identificador, seu tipo de dado, seu escopo, sua categoria (variável, função, parâmetro), e, em fases posteriores, seu endereço de memória ou outras propriedades relevantes para a geração de código.

Um dos conceitos mais importantes gerenciados pela tabela de símbolos é o **escopo**. O escopo define a região do programa onde um identificador é visível e pode ser acessado. Compiladores modernos suportam múltiplos escopos (global, local, de função, de bloco), e a tabela de símbolos precisa ser capaz de gerenciar essa hierarquia, permitindo que identificadores com o mesmo nome existam em escopos diferentes sem conflito.

**Funcionalidade Geral:**

A Tabela de Símbolos (`Symbol Table`) é uma estrutura de dados essencial em um compilador. Ela armazena informações sobre todos os identificadores (nomes de variáveis, funções, etc.) que são declarados no código.

**Raciocínio da Construção:**

-   **Escopo:** A principal complexidade aqui é gerenciar escopos. Uma variável declarada dentro de uma função não deve ser visível fora dela. Para resolver isso, a tabela de símbolos é implementada como uma "pilha" de escopos. Cada escopo (`SymbolTable`) tem um ponteiro para seu escopo pai (`parent`).
    -   `scope_enter`: Cria um novo escopo (por exemplo, ao entrar em uma função) e o empilha.
    -   `scope_lookup`: Procura por um símbolo. A lógica é: primeiro, procure no escopo atual. Se não encontrar, suba para o escopo pai e continue procurando até chegar ao escopo global. Isso modela perfeitamente as regras de escopo da maioria das linguagens.
-   **`SymbolNode` (struct):** Cada entrada na tabela é um `SymbolNode`. Ele armazena o nome do identificador, seu tipo (`TYPE_INTEGER`, `TYPE_FLOAT`), seu tipo de uso (`KIND_VARIABLE`, `KIND_FUNCTION`), e outras informações como a linha onde foi declarado e seu endereço na memória (para geração de código futuro).
-   **Hashing:** Para tornar a busca de símbolos eficiente, usei uma tabela de hash. A função `hash` calcula um índice com base no nome do símbolo, permitindo um acesso rápido. As colisões são tratadas com listas encadeadas.
-   **`scope_insert`:** Antes de inserir um novo símbolo, esta função verifica se um símbolo com o mesmo nome já foi declarado *no mesmo escopo*. Isso é crucial para detectar erros de "redeclaração de variável".

---

## `includes/ast.h` e `src/ast.c`

**Teoria da Árvore Sintática Abstrata (AST):**

A **Árvore Sintática Abstrata (AST)** é uma representação hierárquica da estrutura sintática do código-fonte, gerada pela fase de análise sintática. Diferente de uma *árvore de análise* (parse tree) completa, que inclui todos os detalhes gramaticais (como parênteses e palavras-chave que apenas definem a estrutura), a AST é "abstrata" porque omite esses detalhes sintáticos irrelevantes para o significado do programa, focando apenas nos elementos essenciais que carregam o significado semântico.

A AST serve como uma representação intermediária crucial. Ela é mais compacta e fácil de manipular do que o código-fonte original ou uma parse tree, tornando-a ideal para as fases subsequentes do compilador, como a análise semântica, otimização de código e geração de código. Cada nó na AST representa uma construção na linguagem (como uma declaração, uma expressão, uma instrução de controle de fluxo), e os relacionamentos entre os nós refletem a estrutura lógica do programa.

**Funcionalidade Geral:**

A Árvore Sintática Abstrata (AST - Abstract Syntax Tree) é uma representação em árvore da estrutura do código-fonte. Ela é o resultado do "parsing" (análise sintática) e é muito mais fácil de trabalhar do que a sequência linear de tokens.

**`includes/ast.h`:**

-   **`NodeType` (enum):** Assim como os tokens, cada nó na árvore tem um tipo. Este enum define todos os tipos de nós possíveis, como `NODE_PROGRAM` (a raiz), `NODE_ASSIGNMENT` (uma atribuição), `NODE_ADD` (uma operação de soma), etc.
-   **`ASTNode` (struct):** Define a estrutura de um nó. A ideia foi usar uma estrutura de "filho-esquerdo, irmão-direito". Cada nó tem um ponteiro `child` para seu primeiro filho e um ponteiro `sibling` para seu próximo irmão. Isso nos permite representar qualquer número de filhos para um nó de forma flexível. Por exemplo, um nó de soma (`NODE_ADD`) teria dois filhos (os operandos), enquanto um bloco de código (`NODE_BLOCK`) poderia ter muitos filhos (as declarações dentro do bloco).

**`src/ast.c`:**

-   **`create_node`:** Uma função de fábrica para criar um novo nó da AST.
-   **`add_child`:** Adiciona um nó como filho de outro. A lógica percorre a lista de irmãos até encontrar o último e anexa o novo filho.
-   **`print_ast`:** Uma função de depuração que imprime a árvore de forma indentada. A recursão é a chave aqui: ela imprime o nó atual e, em seguida, chama a si mesma para os filhos (com um nível de indentação maior) e para os irmãos (com o mesmo nível).

---

## `includes/parser.h` e `src/parser.c`

**Teoria da Análise Sintática e Semântica:**

A **Análise Sintática (Parsing)** é a fase do compilador que verifica se a sequência de tokens gerada pelo analisador léxico está em conformidade com a **gramática** da linguagem de programação. A gramática de uma linguagem é tipicamente definida usando **gramáticas livres de contexto (GLCs)**, que são um conjunto de regras de produção que descrevem a estrutura sintática válida do programa. O resultado da análise sintática é geralmente uma **árvore de análise (parse tree)** ou, mais comumente, uma **Árvore Sintática Abstrata (AST)**, que já discutimos.

Existem diversas técnicas de parsing, que podem ser classificadas em *top-down* (descendente) ou *bottom-up* (ascendente). Nosso compilador utiliza uma abordagem *top-down* específica chamada **Análise Descendente Recursiva**. Nesta técnica, cada regra de produção da gramática é implementada como uma função recursiva no parser. A função tenta "casar" os tokens de entrada com a estrutura esperada pela regra, chamando outras funções de parsing para sub-regras.

A **Análise Semântica** é a fase que segue a análise sintática e se concentra no *significado* do programa. Enquanto o parser garante que o código está bem-formado sintaticamente, o analisador semântico verifica se ele faz sentido logicamente e se adere às regras de significado da linguagem. Isso inclui:

-   **Verificação de Tipos:** Garantir que as operações são aplicadas a tipos de dados compatíveis (ex: não somar um inteiro com uma string).
-   **Verificação de Declaração:** Assegurar que todas as variáveis e funções usadas foram declaradas antes de seu uso.
-   **Verificação de Escopo:** Gerenciar a visibilidade dos identificadores em diferentes partes do programa.

Para realizar essas verificações, o analisador semântico interage intensamente com a **Tabela de Símbolos** (para buscar informações sobre identificadores) e a **AST** (para percorrer a estrutura do programa e aplicar as regras semânticas).

**Funcionalidade Geral:**

O "parser" (ou analisador sintático) é o cérebro do nosso compilador. Ele pega a sequência de tokens do lexer e a transforma em uma AST. Ao mesmo tempo, ele realiza a análise semântica, usando a tabela de símbolos para verificar se o código faz sentido (por exemplo, se as variáveis foram declaradas antes do uso).

**Raciocínio da Construção (`src/parser.c`):**

-   **Análise Descendente Recursiva:** A abordagem que usei é a análise descendente recursiva. Criei uma função de `parse_` para cada regra gramatical da nossa linguagem (por exemplo, `parse_Statement`, `parse_Expression`, `parse_Conditional`).
-   **`ParserState` (struct):** Semelhante ao lexer, o parser precisa de um estado para saber qual token está analisando (`current`), a lista de tokens, e, crucialmente, o `current_scope` (a tabela de símbolos do escopo atual).
-   **Funções Auxiliares:**
    -   `peek`: Olha para o token atual sem consumi-lo.
    -   `advance`: Move para o próximo token.
    -   `consume`: Verifica se o token atual é do tipo esperado. Se for, avança. Se não, lança um erro de sintaxe. Esta é a principal forma de garantir que o código-fonte segue a gramática.
-   **Lógica de Parsing (Exemplo: `parse_Statement`):**
    -   A função `parse_Statement` olha para o token atual (`peek`) para decidir que tipo de declaração está por vir.
    -   Se for `TOKEN_IF`, ela chama `parse_Conditional`.
    -   Se for `TOKEN_WHILE`, ela chama `parse_Loop`.
    -   Se for `TOKEN_IDENTIFIER`, pode ser uma atribuição (`x = 5;`) ou uma chamada de função (`foo();`). O parser olha para o *próximo* token para decidir. Se for um `(`, é uma chamada de função. Se for um `=`, é uma atribuição.
-   **Análise Semântica Integrada:**
    -   **Declaração:** Ao analisar uma declaração de variável (ex: `int x;`), o parser chama `scope_insert` para adicionar `x` à tabela de símbolos do escopo atual.
    -   **Uso:** Ao encontrar um identificador em uma expressão (ex: `y = x + 1;`), o parser chama `scope_lookup` para verificar se `x` foi declarado. Se `scope_lookup` retornar `NULL`, significa que a variável não existe, e o parser lança um erro semântico.

---

## `main.c`

**Funcionalidade Geral:**

O `main.c` orquestra todo o processo de compilação. Ele é o ponto de entrada que amarra todos os módulos.

**Raciocínio da Construção:**

1.  **Leitura do Arquivo:** A primeira etapa é ler o arquivo de código-fonte fornecido como argumento da linha de comando. A função `read_file` faz isso, carregando todo o conteúdo do arquivo em uma única string na memória.
2.  **Análise Léxica:** Em seguida, ele chama `tokenize`, passando o código-fonte. O lexer retorna uma lista de tokens.
3.  **Análise Sintática e Semântica:** O `main` então inicializa o `ParserState` com os tokens e uma tabela de símbolos global. Ele chama `parse`, que é a função de nível superior do parser. O parser, por sua vez, constrói a AST e realiza as verificações semânticas.
4.  **Impressão e Limpeza:** Após o parsing, o `main` imprime a AST resultante (para fins de depuração) e, em seguida, libera toda a memória alocada (o código-fonte, os tokens, a AST e a tabela de símbolos). A limpeza cuidadosa da memória é fundamental para evitar vazamentos.

---

## Demonstração do Funcionamento

Agora, vamos ver o compilador em ação. Para isso, usaremos um arquivo de exemplo chamado `teste.lang`.

```lang
function int main()
begin
    int x;
    x = 10;

    if (x > 5) then
    begin
        x = x + 1;
    end
    endif

    return x;
end
```

Para executar o compilador com este arquivo, usamos o comando:

```bash
./analisador teste.lang
```

A saída do comando será dividida em várias seções, cada uma representando uma fase do processo de compilação.

---

### Saída da Execução:

```
--- Tokens ---
  [1] Type: FUNCTION             Lexeme: 'function'
  [1] Type: INT                  Lexeme: 'int'
  [1] Type: IDENTIFIER           Lexeme: 'main'
  [1] Type: LPAREN               Lexeme: '('
  [1] Type: RPAREN               Lexeme: ')'
  [2] Type: BEGIN                Lexeme: 'begin'
  [3] Type: INT                  Lexeme: 'int'
  [3] Type: IDENTIFIER           Lexeme: 'x'
  [3] Type: SEMICOLON            Lexeme: ';'
  [4] Type: IDENTIFIER           Lexeme: 'x'
  [4] Type: ASSIGN               Lexeme: '='
  [4] Type: INTEGER_LITERAL      Lexeme: '10'
  [4] Type: SEMICOLON            Lexeme: ';'
  [6] Type: IF                   Lexeme: 'if'
  [6] Type: LPAREN               Lexeme: '('
  [6] Type: IDENTIFIER           Lexeme: 'x'
  [6] Type: GT                   Lexeme: '>'
  [6] Type: INTEGER_LITERAL      Lexeme: '5'
  [6] Type: RPAREN               Lexeme: ')'
  [6] Type: THEN                 Lexeme: 'then'
  [7] Type: BEGIN                Lexeme: 'begin'
  [8] Type: IDENTIFIER           Lexeme: 'x'
  [8] Type: ASSIGN               Lexeme: '='
  [8] Type: IDENTIFIER           Lexeme: 'x'
  [8] Type: PLUS                 Lexeme: '+'
  [8] Type: INTEGER_LITERAL      Lexeme: '1'
  [8] Type: SEMICOLON            Lexeme: ';'
  [9] Type: END                  Lexeme: 'end'
  [10] Type: ENDIF                Lexeme: 'endif'
  [12] Type: RETURN               Lexeme: 'return'
  [12] Type: IDENTIFIER           Lexeme: 'x'
  [12] Type: SEMICOLON            Lexeme: ';'
  [13] Type: END                  Lexeme: 'end'
  [14] Type: EOF                  Lexeme: 'EOF'
```

**Explicação da Seção "Tokens":**

Esta seção é a saída do **Analisador Léxico (Lexer)**. Ele leu o código-fonte (`teste.lang`) caractere por caractere e o dividiu em unidades significativas chamadas "tokens". Cada linha representa um token encontrado:

*   `[Número]`: Indica o número da linha no arquivo de código-fonte onde o token foi encontrado. Isso é crucial para mensagens de erro.
*   `Type`: É a categoria ou tipo do token (definido no `includes/token.h`). Por exemplo, `FUNCTION` é uma palavra-chave, `IDENTIFIER` é um nome de variável ou função, `INTEGER_LITERAL` é um número inteiro.
*   `Lexeme`: É o texto exato (a "string") que o analisador léxico encontrou no código-fonte para aquele token.

**Exemplos:**

*   `[1] Type: FUNCTION Lexeme: 'function'`: O lexer encontrou a palavra-chave `function` na linha 1.
*   `[1] Type: IDENTIFIER Lexeme: 'main'`: Encontrou o identificador `main` na linha 1.
*   `[3] Type: INT Lexeme: 'int'`: Encontrou a palavra-chave `int` na linha 3.
*   `[4] Type: INTEGER_LITERAL Lexeme: '10'`: Encontrou o número `10` na linha 4.
*   `[6] Type: GT Lexeme: '>'`: Encontrou o operador "maior que" (`>`) na linha 6.
*   `[14] Type: EOF Lexeme: 'EOF'`: Indica o "End Of File" (fim do arquivo), um token especial que marca o final do código-fonte.

---

```
--- Análise Sintática e Semântica ---
Análise concluída com sucesso.
```

**Explicação da Seção "Análise Sintática e Semântica":**

Esta é a saída do **Analisador Sintático (Parser)** e do **Analisador Semântico**.

*   **Análise Sintática:** O parser pegou a sequência de tokens e verificou se eles formam uma estrutura gramaticalmente válida de acordo com as regras da linguagem. Se houvesse um erro de sintaxe (por exemplo, um `if` sem um `then`), ele teria reportado aqui.
*   **Análise Semântica:** Simultaneamente, o analisador semântico (usando a Tabela de Símbolos) verificou o "significado" do código. Por exemplo, se todas as variáveis usadas foram declaradas, se os tipos de dados são compatíveis em operações, etc.

A mensagem `Análise concluída com sucesso.` significa que o código `teste.lang` está gramaticalmente correto e semanticamente válido de acordo com as regras implementadas no compilador.

---

```
--- Tabela de Símbolos ---
--- Symbol Table Dump ---
--- Scope Level 0 ---
  -> Name: main           , Kind: 2, Type: 0, Addr: 0
-------------------------
```

**Explicação da Seção "Tabela de Símbolos":**

Esta seção mostra o conteúdo da **Tabela de Símbolos** após a análise. A tabela de símbolos é uma estrutura de dados que armazena informações sobre todos os identificadores (variáveis, funções, etc.) declarados no programa.

*   `--- Symbol Table Dump ---`: Título da seção.
*   `--- Scope Level 0 ---`: Indica o "nível de escopo". `Scope Level 0` geralmente representa o escopo global ou o escopo de nível superior do programa. No nosso caso, a função `main` é declarada neste escopo.
*   `-> Name: main , Kind: 2, Type: 0, Addr: 0`: Esta linha representa uma entrada na tabela de símbolos para a função `main`.
    *   `Name: main`: O nome do identificador.
    *   `Kind: 2`: Refere-se ao `SymbolKind` (definido em `includes/symtab.h`). `KIND_FUNCTION` tem o valor 2. Isso significa que `main` é reconhecida como uma função.
    *   `Type: 0`: Refere-se ao `SymbolDataType` (definido em `includes/symtab.h`). `TYPE_INTEGER` tem o valor 0. Isso indica que a função `main` retorna um valor inteiro.
    *   `Addr: 0`: O endereço ou deslocamento de memória associado a este símbolo. Para funções, pode ser um endereço de entrada ou um identificador interno.

A tabela de símbolos é fundamental para a análise semântica, permitindo que o compilador verifique se os identificadores estão sendo usados corretamente (por exemplo, se uma variável foi declarada antes de ser usada).

---

```
--- Árvore Sintática Abstrata ---
Type: 0
  Type: 9, Value: "main"
    Type: 11
    Type: 1
      Type: 2, Value: "x"
      Type: 3, Value: "x"
        Type: 27, Value: "10"
      Type: 5
        Type: 24
          Type: 26, Value: "x"
          Type: 27, Value: "5"
        Type: 1
          Type: 3, Value: "x"
            Type: 15
              Type: 26, Value: "x"
              Type: 27, Value: "1"
      Type: 8
        Type: 26, Value: "x"
```

**Explicação da Seção "Árvore Sintática Abstrata (AST)":**

Esta é a representação hierárquica do código-fonte, construída pelo parser. A AST remove detalhes sintáticos desnecessários (como parênteses em expressões que não alteram a precedência) e foca na estrutura essencial do programa.

*   `Type: [Número]`: Refere-se ao `NodeType` (definido em `includes/ast.h`). Cada número corresponde a um tipo de nó na árvore.
*   `Value: "[String]"`: Alguns nós (como identificadores ou literais) têm um valor associado.
*   A indentação indica a hierarquia: nós filhos são indentados em relação aos seus pais.

Vamos decodificar a AST para `teste.lang`:

```c
function int main()
begin
    int x;
    x = 10;

    if (x > 5) then
    begin
        x = x + 1;
    end
    endif

    return x;
end
```

**Decodificação da AST:**

*   `Type: 0` (`NODE_PROGRAM`): Este é o nó raiz da AST, representando o programa inteiro.
    *   `Type: 9, Value: "main"` (`NODE_FUNC_DECL`): Representa a declaração da função `main`.
        *   `Type: 11` (`NODE_PARAM_LIST`): Lista de parâmetros da função `main`. Está vazia, pois `main()` não tem parâmetros.
        *   `Type: 1` (`NODE_BLOCK`): Representa o bloco de código dentro da função `main` (entre `begin` e `end`).
            *   `Type: 2, Value: "x"` (`NODE_DECLARATION`): Representa a declaração da variável `int x;`.
            *   `Type: 3, Value: "x"` (`NODE_ASSIGNMENT`): Representa a atribuição `x = 10;`.
                *   `Type: 27, Value: "10"` (`NODE_INT_LITERAL`): O valor literal inteiro `10`.
            *   `Type: 5` (`NODE_CONDITIONAL`): Representa a estrutura `if (x > 5) then ... endif`.
                *   `Type: 24` (`NODE_GT`): Representa a condição `x > 5`.
                    *   `Type: 26, Value: "x"` (`NODE_IDENTIFIER`): O identificador `x`.
                    *   `Type: 27, Value: "5"` (`NODE_INT_LITERAL`): O valor literal inteiro `5`.
                *   `Type: 1` (`NODE_BLOCK`): O bloco de código dentro do `if` (entre `begin` e `end`).
                    *   `Type: 3, Value: "x"` (`NODE_ASSIGNMENT`): Representa a atribuição `x = x + 1;`.
                        *   `Type: 15` (`NODE_ADD`): Representa a operação de adição `x + 1`.
                            *   `Type: 26, Value: "x"` (`NODE_IDENTIFIER`): O identificador `x`.
                            *   `Type: 27, Value: "1"` (`NODE_INT_LITERAL`): O valor literal inteiro `1`.
            *   `Type: 8` (`NODE_RETURN_STMT`): Representa a instrução `return x;`.
                *   `Type: 26, Value: "x"` (`NODE_IDENTIFIER`): O identificador `x` que está sendo retornado.

A AST é a representação mais importante do programa para as fases posteriores do compilador (como a geração de código), pois ela captura a estrutura e o significado do código de forma abstrata.

---

```
Limpando memória...
Concluído.
```

**Explicação da Seção "Limpando memória...":**

Estas mensagens indicam que o programa está liberando a memória que foi alocada dinamicamente durante as fases de compilação (para o código-fonte, os tokens, a AST e a tabela de símbolos). Isso é uma boa prática de programação em C para evitar vazamentos de memória.

---

## Conclusão

Este projeto demonstra o fluxo de trabalho completo de um frontend de compilador: da análise léxica à análise sintática e semântica, culminando na construção de uma Árvore Sintática Abstrata. Cada módulo foi projetado para ser coeso e desempenhar uma função específica, e a comunicação entre eles (tokens, AST, tabela de símbolos) foi cuidadosamente definida. O próximo passo lógico seria usar a AST e a tabela de símbolos para gerar código intermediário ou código de máquina.

Obrigado.