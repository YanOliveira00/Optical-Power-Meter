#Codigo criado por Yan S de Oliveira
#complementar ao codigo desenvolvido para o Arduino
#feito para TCC em Engenharia da Computacao - UFPa
#Editado e comentado em 17-03-2017 
import sys
import serial
import serial.tools.list_ports
import struct
import time 

#Verifica portas e aciona placa
try:
    portas=list(serial.tools.list_ports.comports())
	for p in portas:
    	print p
    placa=serial.Serial(p[0],250000, timeout=1, dsrdtr=True)
	#Porta, velocidade, timeout e hardware flowcontrol ativado
except serial.SerialException: #Erro, caso haja
	print 'Erro, verificar porta!'
	sys.exit() 

#Le argumentos
limitador=(sys.argv[1]) #limitador
corrente=sys.argv[2] #corrente
amostras=float(sys.argv[3]) #amostras 
if (len(sys.argv)==5):
	funcao=sys.argv[4] #funcao
	nameData='Resultado_'+str(int(time.time()))+'.txt'
elif(len(sys.argv)==6):
	funcao=sys.argv[4] #funcao
	nameData=sys.argv[5]
else:
	funcao='1'
    nameData='Resultado_'+str(int(time.time()))+'.txt' 
placa.write(corrente)
print float(corrente) #confirma o valor

#Argumento limitador 
print limitador #confirma argumento
placa.write('!')#fim do parseFloat e inicio do initLimit em Arduino
time.sleep(.2)
placa.write(limitador)
time.sleep(.5)#valores de tempos arbitrarios
placa.write('!')#fim do parseFloat e inicio do initLimit em Arduino
time.sleep(.5)
placa.write(funcao)
print ("funcao "+funcao)
time.sleep(1) 
#Argumento com do numero de amostras
print amostras
print "Start!" 
call=1 #contador 	

#LEITURA SERIAL
#Busca a leitura do char delimitador de inicio
#apos inicio, le-se 2, representa o int enviado
#ao encontrar a flag de fim, grava e quebra o loop
def ler_serial():
	MyInt=''	
	while(1):
        	rec=placa.read()
        	if(rec=='&'):  
            	data=placa.read(2) #2 bytes de dados
            	MyInt=struct.unpack('H',data)#desempacota o int recebido
                arquivo.write(str(time.time()-inicio)+' ')
            	arquivo.write(str(MyInt)+'\n')           	
            	break        	
	return str(MyInt)


#Inicio do loop principal
#roda-se um time inicial para comparar com o fim do bloco        	
if __name__=="__main__":
	
arquivo=open(nameData,'a')       	
inicio=time.time()
c=''
while (call<=amostras):#contador ate o solicitado em amostra    	
	c=ler_serial()
	if(call==int(amostras/2)):
    	print c	
	call+=1
fim=time.time()
arquivo.close()
print 'done!'
#mostra o tempo que levou
print fim-inicio
#envia a media de tempo por amostra
print (fim-inicio)/amostras
placa.write('9') #flag fim para Arduino
placa.close()
sys.exit()
