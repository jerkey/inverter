# if you need to pad the zeros consider this
# http://stackoverflow.com/questions/12638408/decorating-hex-function-to-pad-zeros
import math
STEPS=256
f = open('/tmp/sinewave.txt','w')
f.write('const int sinewave_length='+str(STEPS)+';\nconst unsigned char sinewave_data[] PROGMEM = {')
step = math.pi/STEPS
for i in range(STEPS):
    if i % 16 == 0:
        f.write('\n')
    f.write(hex(int(round(255*math.sin(i*step)))))
    if i < (STEPS-1):
        f.write(',')
f.write('};')
f.close()
