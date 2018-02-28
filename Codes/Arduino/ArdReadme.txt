Código Arduino (C++), criado por Yan Oliveira.

Código foi totalmente comentado e detalhado, mas talvez algumas coisas não fiquem tão claras apenas lendo.

O uso da função union foi escolhido para obtermos a maior velocidade possível de transmissão entre o Arduino e o código python, pela porta serial. Por isso é melhor enviar dados binários, até por uma questão de diminuição de erros.

Os dados obtidos nas portas são inteiros (0-1024). Evitou-se o uso qualquer operação dentro do arduino, para que não houvessem gargalos de velocidade por parte do SBC.

A função writeTLC5620(), obedeceu a operação de envio de dados mais simples e direta do conversor DAC tlc5620 (http://www.ti.com/lit/ds/slas081e/slas081e.pdf). Representada na figura 2. Sendo que, a variável FREQUENCY representa um valor em uS, ou seja, é necessário fazer conversão de Khz para um valor em tempo, sempre que houver qualquer alteração nesta variável.

/////////////////////////// CUIDADO!!! \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Não foi feito qualquer medida contra erros, devido se tratar apenas de um trabalho acadêmico. Logo garantimos que as entradas não estariam equivocadas. Talvez alguns breakpoints devam ser adicionados para não danificar os LD's que serão utilizados junto ao Power Meter.





