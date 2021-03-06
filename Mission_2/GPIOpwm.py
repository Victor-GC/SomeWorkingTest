try:
        import RPi.GPIO as GPIO
        import time
except RuntimeError:
                print('import some package error!')

GPIO_PIN=24
count=5

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(GPIO_PIN,GPIO.OUT)

pwm=GPIO.PWM(GPIO_PIN,50)
pwm.start(25)
input('input any key to stop:')
pwm.stop()
pwm.start(50)
pwm.ChangeFrequency(30)
pwm.ChangeDutyCycle(25)
pwm.stop()

GPIO.cleanup()
