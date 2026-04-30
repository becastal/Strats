# Strats: Matching Engine

Implementação em C++ de uma matching engine simples para um único ativo.

A engine suporta ordens limit, market, cancelamento, alteração de ordens, visualização do livro e ordens pegged.

## Como Compilar

```bash
g++ main.cpp -o matching
```

## Como Rodar

```bash
./matching < testes/in1
```

A primeira linha da entrada deve conter o número de comandos.

## Bônus Implementados

| Item | Descrição | Implementação |
|---|---|---|
| 1 | Visualização do livro | Comando `print` |
| 2 | Prioridade por ordem de chegada | Uso do `id` como segundo critério de ordenação |
| 3 | Cancelamento | Comando `cancel <id>` |
| 4 | Alteração de ordem | Comandos `change limit` e `change market` |
| 5 | Ordens pegged | Comandos `peg bid buy`, `peg offer sell`, `peg offer buy`, `peg bid sell` |

## Comandos Suportados

```txt
limit buy <preco> <quantidade>
limit sell <preco> <quantidade>

market buy <quantidade>
market sell <quantidade>

print

cancel <id>

change limit <id> <novo_preco> <nova_quantidade>
change market <id> <nova_quantidade>

peg bid buy <quantidade>
peg offer sell <quantidade>
peg offer buy <quantidade>
peg bid sell <quantidade>
```

## Decisões De Implementação

A prioridade das ordens é definida pelo preço e depois pelo identificador, que representa também a ordem de chegada.
Essa prioridade é feita pelo `map<pair<double, int>, int> ((preço, id), quantidade)` do map `ordensCompras` (ou, inversamente, `ordensVendas`), que ordena pelo menor pair primeiro. Como para compras queremos o maior preço primeiro, basta inverter todos os preços recebidos como compra e o map ordena facilmente. Mais sobre o porquê do map na discussão de complexidade.

Ordens pegged passivas são mantidas separadas do livro principal. Assim, quando o bid ou offer muda, não é necessário atualizar todas as ordens pegged. O preço efetivo é considerado apenas no momento de imprimir ou executar contra o livro. Isso garante a complexidade subquadrática.

`peg offer buy` é tratado como uma `market buy`, e `peg bid sell` é tratado como uma `market sell`, pois ambos cruzam imediatamente contra o melhor preço disponível do lado oposto.

Ordens market não completadas são mantidas em uma fila até que possam ser completadas.

Em cruzamentos entre duas ordens limit, o trade é reportado no preço da ordem de compra. Essa foi uma escolha de implementação mantida de forma consistente.

## Complexidade

Como supracitado, as ordens ficam armazenadas em `map` do C++ (uma árvore red-black), então inserções, remoções e buscas por id têm custo `O(log n)`, enquanto mantém, muito importantemente, a ordem dos inputs.

As funções de matching (como `resolveOrdens` ou `resolveFilaCompra`) percorrem apenas ordens que são efetivamente consumidas. Embora uma única chamada possa consumir várias ordens, cada ordem só é removida do livro uma vez ao longo da execução. Por isso, o custo total do matching é amortizado pelo número de ordens efetivamente executadas. Argumento similar ao de algoritmos como KMP ou Closest Smaller.

De forma geral:

```txt
Inserção de limit order: O(log n)
Cancelamento: O(log n)
Alteração de limit order: O(log n)
Market order: O(k log n), onde k é o número de ordens consumidas (O(log n) amortizado)
Print do livro: O(n + p), onde p é o número de ordens pegged
```
