comPort = 'COM5';
[s,flag] = setupSerial(comPort);
fread(s,1,'uchar');c
data = zeros(1);
i = 1;
while true
    data(i) = fread(s,1,'uchar');
    disp(data(i))
    i = i + 1;
end
