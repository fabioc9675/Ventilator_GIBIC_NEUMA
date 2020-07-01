%% Codigo para calibrar los sensores de flujo
clear all, close all, clc


%% Carga del archivo para la calibracion
varName = 'Equipos/Calibracion_00006.mat';
load(varName);
sens = 1;  % esta variable controla en que puntos cercanos al cero se realiza particion de los segmentos de la linealizacion

% evaluacion de la existencia de las variables de referencia y de posicion
% para calibracion
if (~exist('position','var')),  posFlag = 0; else posFlag = 1; end;
if (~exist('reference','var')), refFlag = 0; else refFlag = 1; end;

longitud = 100; % longitud a promediar

if (posFlag == 0)
    fprintf('=======================================================\r');
    fprintf('Inicio de proceso de calibracion\r');
    fprintf('\r');
    fprintf('- De clic en la grafica en las diferentes mesetas para \r');
    fprintf('  capturar la posicion\r');
    fprintf('- Introduzca los valores de flujo medidos en el VTPlus \r');
    fprintf('\r');
    fprintf('cada que ingrese un dato y desee agregar uno adicional, \r');
    fprintf('introduzca cero, en caso de ser el ultimo dato,\r');
    fprintf('introduzca 1 para salir\r');
    fprintf('\r');
    fprintf('Valores recomendados para calibracion\r');
    fprintf('\r');
    fprintf('0, 15, 25, 40, 48, 60, 75 (Lpm)\r');
    fprintf('\r');
    fprintf('Para iniciar presione enter\r');
    fprintf('=======================================================\r');
    input('Iniciar (enter)');
    cont = 0;
    posi = zeros(1, 50);  % matriz vacia para recibir los valores
    refe = zeros(1, 50);  % matriz vacia para recibir los valores
    counter = 0;
    while (cont == 0)
        counter = counter + 1;
        figure(20)
        dcm = datacursormode;  % se crea objeto para detectar eventos de cursor sobre la grafica
        plot(flowSignal(:,1));
        hold on
        plot(flowSignal(:,2));
        w = waitforbuttonpress; % se espera por el clic para capturar la posicion de la variable
        C = getCursorInfo(dcm);
        close(20);
        posi(counter) = C.Position(1);  % se almacena la posicion capturada
        if (refFlag == 0)
            refe(counter) = input('valor de calibracion = ');
        end
        cont = input('continuar = 0, salir = 1, === ');
    end
    
    position = zeros(1,2*counter-1);
    
    for i = 1:counter
        position(i) = posi(counter-i+1);
    end
    for i = 2:counter
        position(counter + i - 1) = posi(i);
    end
        
    if (refFlag == 0)
        reference = zeros(1,2*counter-1);
        for i = 1:counter
            reference(i) = - refe(counter-i+1);
        end
        for i = 2:counter
            reference(counter + i - 1) = refe(i);
        end
    end
    save(varName, 'flowSignal', 'reference', 'position');
end
        

%% procesamiento de los datos para calibracion de sensor de flujo

Fins = flowSignal(:,1);
Fesp = flowSignal(:,2);

x =  flowSignal(:,1);
xx = flowSignal(:,2);


% matriz de señales
flowIns = zeros(length(position), longitud);
flowEsp = zeros(length(position), longitud);

meanIns = zeros(length(position), 1);
meanEsp = zeros(length(position), 1);

for i = 1: length(position)
    flowIns(i,:) = Fins(position(i): position(i) + longitud - 1);
    flowEsp(i,:) = Fesp(position(i): position(i) + longitud - 1);
end


for i = 1: (length(position) - 1)/2
    meanIns(i) = 2*mean(flowIns((length(position) + 1)/2))-mean(flowIns(i));
    meanEsp(i) = 2*mean(flowEsp((length(position) + 1)/2))-mean(flowEsp(i));
end

for i = (length(position) + 1)/2 : length(position)
    meanIns(i) = mean(flowIns(i));
    meanEsp(i) = mean(flowEsp(i));
end



%% Ajuste a tramos de la ecuacion

recomend = [1, ((length(position) + 1)/2) - sens, ((length(position) + 1)/2) + sens, length(position)];

figure(2)
subplot(211)
plot(meanIns, reference)
hold on
plot(meanIns(recomend), reference(recomend),'*')
subplot(212)
plot(meanEsp,reference)
hold on
plot(meanEsp(recomend), reference(recomend),'*')

% Ajuste a tramos
coef_1 = polyfit([meanIns(recomend(1)), meanIns(recomend(2))], [reference(recomend(1)), reference(recomend(2))], 1);
a1 = coef_1 (1);
b1 = coef_1 (2);

coef_2 = polyfit([meanIns(recomend(2)), meanIns(recomend(3))], [reference(recomend(2)), reference(recomend(3))], 1);
a2 = coef_2 (1);
b2 = coef_2 (2);


coef_3 = polyfit([meanIns(recomend(3)), meanIns(recomend(4))], [reference(recomend(3)), reference(recomend(4))], 1);
a3 = coef_3 (1);
b3 = coef_3 (2);



%Ajuste a tramos
coef_4 = polyfit([meanEsp(recomend(1)), meanEsp(recomend(2))], [reference(recomend(1)), reference(recomend(2))], 1);
a4 = coef_4 (1);
b4 = coef_4 (2);

coef_5 = polyfit([meanEsp(recomend(2)), meanEsp(recomend(3))], [reference(recomend(2)), reference(recomend(3))], 1);
a5 = coef_5 (1);
b5 = coef_5 (2);


coef_6 = polyfit([meanEsp(recomend(3)), meanEsp(recomend(4))], [reference(recomend(3)), reference(recomend(4))], 1);
a6 = coef_6 (1);
b6 = coef_6 (2);

%% correccion de flujo

for i =1:length(x)
    if(x(i)<=meanIns(recomend(2)))
        Fcins(i) = a1*x(i) + b1;
    elseif (x(i)<= meanIns(recomend(3)))
        Fcins(i) = a2*x(i) + b2;
    else
        Fcins(i) = a3*x(i) + b3;
    end
    % Fcins(i) = 0.14186*x(i) - 252.79;
end

for i =1:length(xx)
    if(xx(i)<=meanEsp(recomend(2)))
        Fcesp(i) = a4*xx(i) + b4;
    elseif (xx(i)<=meanEsp(recomend(3)))
        Fcesp(i) = a5*xx(i) + b5;
    else
        Fcesp(i) = a6*xx(i) + b6;
    end
    % Fcesp(i) = 0.18536*xx(i) - 304.22;
end
    
% figure
% %Flow insp
% plot(Fcins)
% %Flow esp
% hold on
% plot(Fcesp)    
%    
% t = (0:0.05:(length(Fcins)-1)*0.05);
% figure
% plot(Fcins - Fcesp)
% hold on
% plot(cumtrapz(t,Fcins - Fcesp-29))

%% impresion de datos para el microcontrolador
printCal;    
    
    
    
    

