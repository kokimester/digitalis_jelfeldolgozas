clear
clc
close all

fid = fopen('valami.bin', 'r');
samples = fread(fid,inf,'float32');

%%
fs = 14238;
%plot(samples(1:end))
start_index = 165558;
stop_index = 1019830;
bits = samples > 0.04;
offset=2000;
data= ~bits((start_index+offset):fs:(stop_index+offset))

%%
minute=data(22:28);
hour=data(31:35);
day=data(37:42);
wekday=data(43:45);
year = data(51:58);

crc = (data(59)==mod(sum(data(37:58)),2))