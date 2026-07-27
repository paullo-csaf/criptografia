# O Escriba Arcano

> Um projeto didático que demonstra o funcionamento da criptografia RSA através de uma interface gamificada inspirada em RPGs clássicos.

![Linguagem](https://img.shields.io/badge/C-99-blue)
![Raylib](https://img.shields.io/badge/Raylib-5.x-black)
![Status](https://img.shields.io/badge/status-Beta-orange)

---

# Sobre o projeto

O Escriba Arcano nasceu como um projeto de estudos sobre criptografia RSA.

Ao invés de criar apenas uma aplicação de terminal capaz de criptografar e descriptografar mensagens, a proposta foi desenvolver uma pequena experiência interativa onde o usuário "entrega" suas mensagens a um mago, responsável por ocultar ou revelar seu conteúdo através dos conceitos matemáticos do algoritmo RSA.

Toda a ambientação foi inspirada em jogos 2D clássicos (como Pokémon Fire Red e Undertale), utilizando pixel art, poucos elementos na tela e uma temática medieval.

O foco principal do projeto **não é criar um jogo**, mas sim apresentar os conceitos da criptografia RSA de forma mais visual e acessível.

---

# Objetivos

- Implementar o algoritmo RSA em linguagem C.
- Aplicar conceitos de:
  - exponenciação modular rápida;
  - Teorema de Fermat;
  - Teorema Chinês do Resto (CRT);
  - geração de chaves públicas e privadas;
  - criptografia e descriptografia.
- Desenvolver uma interface gráfica utilizando Raylib.
- Tornar o estudo da criptografia mais intuitivo através de uma experiência gamificada.

---

# Tecnologias utilizadas

- Linguagem C
- Raylib
- GCC
- Makefile
- LibreSprite (pixel art)
- Git
- GitHub
- Ubuntu Linux

---

# Estrutura do projeto

```
criptografia/
│
├── assets/
│   ├── fontes/
│   └── sprites/
│
├── dados/
│   └── parametros.csv
│
├── entrada/
│   └── mensagem.txt
│
├── include/
│
├── saida/
│
├── src/
│
├── Makefile
└── README.md
```

---

# Funcionamento

O programa possui dois modos principais:

## Criptografar

A mensagem pode ser fornecida de duas formas:

### Entrada pelo teclado

O usuário escreve diretamente dentro do editor presente no programa.

Fluxo:

```
Menu

↓

Criptografar

↓

Digitar mensagem

↓

Editor

↓

Ritual

↓

Mensagem criptografada
```

---

### Entrada por arquivo

O usuário pode editar manualmente:

```
entrada/mensagem.txt
```

Depois selecionar:

```
Criptografar

↓

Usar entrada/mensagem.txt
```

O programa irá ler automaticamente o conteúdo do arquivo.

---

## Descriptografar

Também existem duas possibilidades.

### Utilizar a última mensagem criptografada

O programa utiliza automaticamente:

```
saida/criptografado_ascii.txt
```

---

### Digitar uma cifra manualmente

O usuário informa os blocos criptografados manualmente através do editor.

---

# Arquivos gerados

Após uma execução, o programa produz os seguintes arquivos.

```
saida/

├── mensagem_original.txt
├── mensagem_original_ascii.txt
├── criptografado_ascii.txt
├── descriptografado.txt
├── descriptografado_ascii.txt
```

Além disso,

```
dados/

└── parametros.csv
```

contém os parâmetros utilizados pelo RSA.

---

# Parâmetros RSA

Os parâmetros utilizados pelo algoritmo ficam em

```
dados/parametros.csv
```

Exemplo:

```csv
Parametro,Valor
p,17
q,23
e,3
```

Durante a execução, o programa calcula automaticamente:

- φ(n)
- n
- d
- dp
- dq
- q⁻¹

e atualiza o arquivo.

---

# Como executar

## Instalar Raylib

Ubuntu

```bash
sudo apt install libraylib-dev
```

Caso sua distribuição não possua o pacote, consulte:

https://www.raylib.com/

---

## Compilar

```bash
make
```

---

## Executar

```bash
make run
```

ou

```bash
./escriba_arcano
```

---

## Recompilar

```bash
make rebuild
```

---

## Limpar executável

```bash
make clean
```

---

# Limitações atuais (Beta)

Este projeto possui algumas limitações conhecidas.

## Entrada pelo teclado

O editor foi desenvolvido para mensagens relativamente curtas.

Para textos longos recomenda-se utilizar:

```
entrada/mensagem.txt
```

---

## Tamanho máximo da mensagem

O tamanho máximo é definido pela constante

```c
MAX_MENSAGEM
```

presente em

```
include/jogo.h
```

Mensagens maiores que esse limite não são suportadas.

---

## Chaves RSA

Atualmente o programa utiliza chaves pequenas para fins didáticos.

Esse projeto **não deve ser utilizado para segurança real**, pois:

- os primos são pequenos;
- não existe geração automática de primos seguros;
- não há padding (OAEP/PKCS#1);
- não há proteção contra ataques criptográficos modernos.

---

## Interface

A interface foi desenvolvida apenas com fins educacionais.

Algumas animações e elementos visuais ainda encontram-se em desenvolvimento.

---

# Aprendizados

Durante o desenvolvimento deste projeto foram estudados diversos tópicos relacionados à computação:

- Estruturas de dados
- Modularização em C
- Organização de projetos
- Manipulação de arquivos
- Interface gráfica
- Programação orientada a eventos
- Criptografia RSA
- Exponenciação modular rápida
- Teorema Chinês do Resto
- Pixel Art
- Gerenciamento de memória

---

# Possíveis melhorias futuras

- Sons e efeitos sonoros
- Música ambiente
- Mais animações para o mago
- Geração automática de chaves RSA
- Escolha do tamanho das chaves
- Histórico de mensagens criptografadas
- Sistema de configurações
- Exportação dos resultados em PDF
- Melhor editor de texto
- Rolagem para mensagens grandes
- Mais runas e efeitos visuais
- Novos personagens e ambientações

---

# Licença

Este projeto foi desenvolvido exclusivamente para fins acadêmicos e educacionais.

Sinta-se livre para estudar o código, modificá-lo e utilizá-lo como base para seus próprios projetos, mantendo os devidos créditos ao autor.