# QuadCopter drone using the baremetal drivers

- a quadcopter with 10-dof mpu9255 and bmp280 nrf remote control and both quaternion and euler angles filters

-	An Attitude Estimation algorithm built and implemented based on [Madgwick](https://ahrs.readthedocs.io/en/latest/filters/madgwick.html) paper and Quaternion Algebra using [AML](https://github.com/susano254/STM32L4/tree/master/AML) library built under the supervision of [Complete Guide to Rotations and Transformations](https://www.udemy.com/course/complete-guide-to-rotations-and-transformations/?couponCode=THANKSLEARNER24) course 
-	Accelerometer calibration using Linear regression and least Squares, Gyroscope filtering using moving average and Zero Velocity filter, Magnetometer Calibration using Ellipsoid fitting using [Kriswiner Method](https://github.com/kriswiner/MPU6050/wiki/Simple-and-Effective-Magnetometer-Calibration)
-	An angle PID controller for both Pitch and Roll and rate PID for both Attitude and Yaw for the most easy, stable and comfortable flights based on [Introduction to Control System Design](https://learning.edx.org/course/course-v1:MITx+6.302.0x+2T2016/home)
-	PCB design of both The Flight Controller for Brushed and Brushless motor and PCB design of a custom radio Transmitters and receivers 
