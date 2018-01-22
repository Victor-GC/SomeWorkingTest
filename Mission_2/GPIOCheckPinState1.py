try:
    import RPi.GPIO as GPIO
    import time
except RuntimeError:
        print('import some package error!')

GPIO_PIN=24
count=100

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(GPIO_PIN,GPIO.IN)

print('check the high state for 5 times...')

while count>0:
    if(GPIO.input(GPIO_PIN)==1):
        count=count-1
        print('input high')
    time.sleep(0.1)

print('exit...')
GPIO.cleanup()
