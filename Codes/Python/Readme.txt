Código Python, criado por Yan Oliveira. Utilizando a biblioteca PySerial. (https://pythonhosted.org/pyserial/)

Pode-se baixar pelo pip também, mas o seu uso é imprescindível.

O código pode visivelmente ser otimizado, com o intúito de melhorar sua capacidade amostral e ter regras de exceção para os possíveis erros.

Um dos erros mais comuns é o de entrada de argumento inválida. O que trava completamente o programa. 

Formato da entrada: PowerMeter.py <limitador escolhido> <valor corrente> <número de amostras> <porta lida> <nome do arquivo com dados>

Limitadores (Inteiro) = 1 - 220 Ohm; 2 - 680 Ohm; 3 - 2k2 Ohm.
Valor corrente (Real) = valor em mA, deve ser positivo e maior que 0.
Número de amostras (Inteiro) = >0.
Porta lida (Inteiro) = Valores de 1-6.
			1	AMPFEED	A1	Retorna feedback de corrente do interrogador
			2	PDFEED	A0	Feedback de potência do fotodiodo
			3	1X		A5	Canal 1X de amplificação do sinal
			4	10X		A3	Canal 11x de amplificação do sinal
			5	100X	A4	Canal 101x de amplificação do sinal
			6	1000X	A5	Canal 1111x de amplificação do sinal
Nome do arquivo (String) = nome com o final '.txt', contendo o nome do arquivo que os dados brutos serão armazenados. Default: "Resultado<time.time()>.txt"

Formato do dado armazenado: str(Tempo)+' '+str(valor). 

