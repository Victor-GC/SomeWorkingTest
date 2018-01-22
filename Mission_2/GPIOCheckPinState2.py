try:
        import RPi.GPIO as GPIO
        import time
except RuntimeError:
                print('import some package error!')

GPIO_PIN=24
count=5

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(GPIO_PIN,GPIO.IN)

print('waiting 5 times...')

while count>0:
        GPIO.wait_for_edge(GPIO_PIN,GPIO.RISING)
        count=count-1
        print('input high')


print('exit...')
GPIO.cleanup()
