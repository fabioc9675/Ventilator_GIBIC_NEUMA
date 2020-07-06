%% impresion de los datos de calibracion

fprintf('*******************************************************\r');
fprintf('Copie el siguiente codigo en el microcontrolador\r');
fprintf('*******************************************************\r');
fprintf('\r\n');
fprintf('// Calibracion de los sensores de flujo - coeficientes regresion lineal\r');
fprintf('// Sensor de flujo Inspiratorio\r');
fprintf('#define AMP_FI_1_W      %f         \r',round(a1,4));
fprintf('#define OFFS_FI_1_W     %f         \r',round(b1,4));
fprintf('#define LIM_FI_1_W      %i         \r',round(meanIns(recomend(2)),0));
fprintf('#define AMP_FI_2_W      %f         \r',round(a2,4));
fprintf('#define OFFS_FI_2_W     %f         \r',round(b2,4));
fprintf('#define LIM_FI_2_W      %i         \r',round(meanIns(recomend(3)),0));
fprintf('#define AMP_FI_3_W      %f         \r',round(a3,4));
fprintf('#define OFFS_FI_3_W     %f         \r\n',round(b3,4));

fprintf('// Sensor de flujo Espiratorio\r');
fprintf('#define AMP_FE_1_W      %f         \r',round(a4,4));
fprintf('#define OFFS_FE_1_W     %f         \r',round(b4,4));
fprintf('#define LIM_FE_1_W      %i         \r',round(meanEsp(recomend(2)),0));
fprintf('#define AMP_FE_2_W      %f         \r',round(a5,4));
fprintf('#define OFFS_FE_2_W     %f         \r',round(b5,4));
fprintf('#define LIM_FE_2_W      %i         \r',round(meanEsp(recomend(3)),0));
fprintf('#define AMP_FE_3_W      %f         \r',round(a6,4));
fprintf('#define OFFS_FE_3_W     %f         \r',round(b6,4));
