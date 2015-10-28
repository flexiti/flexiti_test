

TCP transmision error -8

If you receive the first error, the error remains!!!!! until we make reset


Httcp client stops working, while tcp client seems to be resistant to this error

The physical display is not needed for the test

In my tests error occurs between 15-60 minutes and already then stays until reset

During the test, you should pay attention to the limitations thingspeak
  - sending not more often than every 15 seconds.

 (Nor do I know how some people will simultaneously send the same KEY.
   - Preferably make yourself your own key.

 Here is KEY=7XXUJWCWYTMXKN3L from Siming example

 Normal log:

>CONNECT1 tcp

 >sending..

 >answer ..323 bytes

 >Tcp +++Success+++

 >CONNECT2 http (2)

 >Sending ..

 >Sent..

 >http +++Success+++

 Error log:

> CONNECT1 tcp

 >sending..
 
 >answer ..323 bytes
 
 >Tcp +++Success+++
 
 >CONNECT2 http (14)
 
 >Sending ..
 
 >Sent..
 
 >http ERROR ERROR
 
