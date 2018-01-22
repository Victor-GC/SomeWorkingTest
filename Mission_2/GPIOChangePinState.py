try:
    import RPi.GPIO as GPIO
    import time
except RuntimeError:
    print("import some package error!")


GPIO_PIN=24
RunCounts=1000

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(GPIO_PIN, GPIO.OUT)

while RunCounts>0:
        GPIO.output(GPIO_PIN,GPIO.HIGH)
        time.sleep(1)
        GPIO.output(GPIO_PIN,GPIO.LOW)
        time.sleep(1)
        --RunCounts
GPIO.cleanup()

