# RAD-Shopping-Cart
## Abstract
RAD Shopping Cart is an IOT shopping cart, which can follow the user through an aisle, and allow for product lookup through a touchscreen LCD. This project is useful since it simplifies the shopping experience for senior citizens and people with disabilities — both of whom struggle physically pushing around a shopping cart, as well as allowing for an accessible and straightforward item lookup at any point of time. The cart uses a network of three ESP8266 Wi-Fi modules to triangulate a user’s location. In addition, the cart uses HC-SRO4 Ultrasonic Sensors in the front, and the sides to allow for lane correction and object avoidance.

## Introduction
Given the archaic nature of shopping carts throughout the past couple decades, we are motivated to integrate an Internet of Things (IoT) solution for extant shopping carts. These smart features aim to ameliorate a customer’s shopping experience. Many senior citizens and those with physical impairments often have trouble pushing their carts around while shopping. Likewise, busy parents managing their energetic children and trying to multitask while shopping often spend a lot longer in the store than necessary.

Previous renditions of smart carts with autonomous shopping carts exist but are not as prevalent as one would assume. Only a handful of companies such as Caper, Robotnik, and Veeve have successfully launched smart cart lines supporting features to automate customers’ shopping and save customers’ time [1] [2] [3].

The shopping cart will augment performance and convenience for such shoppers. The aim of RAD Shopping Cart will support four main features including user-following, lane-correction, collision-detection, and item-lookup.

For USER-FOLLOWING, the cart follows the shopper around the store by pairing to the user’s mobile Wi-Fi hotspot. It will seek to maintain a close (~3 meter) distance from the user.

For LANE-CORRECTION, the cart uses ultrasonic range sensors to automatically detect walls/shelves and the edges of an aisle. It will seek to maintain a pathing closer to the right side of the aisle.

For COLLISION-DETECTION, the cart detects obstructing objects or people that may walk in front of the cart using ultrasonic sensors mounted on the chassis. It will stop moving forward when an obstacle is noticed.

For ITEM-LOOKUP, the built-in LCD touchscreen will support querying the inventory of the store through an online database which makes shopping for products more convenient and speedier. The LCD touchscreen will also support all other user interface requirements.

RAD Shopping Cart primarily uses a distance estimation algorithm using WiFi RSSI for its cost efficiency and better efficacy for the problem statement. A core value of manufacturing was the cost and simplicity of the project.

The LCD touchscreen ties together the FSM of the shopping cart with a straightforward user interface.

To illustrate the key components within the shopping cart, the paper consists of the following sections in key detail: hardware assembly and interface, Wi-Fi tracking, and finite state machine.

![image](https://user-images.githubusercontent.com/69876697/226158215-0f4a6f77-c30a-40e1-b858-ae7bf823476b.png)

## Hardware Assembly and Interface
Before being able to assemble anything we had to create a plan for assembly and interface. We created a circuit schematic detailing how each electronic part would be connected. Then a bill of materials was formed to aid with identifying the parts in the schematic. To ensure that everything was connected properly, hardware testing code was made.

### Wiring Schematic
The wiring schematic acts as a blueprint, when it comes time to assemble the parts. The schematic facilitates the task at looking into each component and ensuring compatibility between devices.

![image](https://user-images.githubusercontent.com/69876697/226158374-461a1bf5-c0a3-4adf-a93e-d651b22c3db2.png)

The above schematic (Fig. 1) is built in KiCad, and most of the part symbols are built from scratch. The symbols show relevant device inputs and outputs. The Mega2560 microcontroller and L298N motor controller were provided each with 11.1V which is within each of their operating voltage ranges. It is important that the L298N is supplied with close to 12V to power the 12V motors. 11.1V is still sufficient voltage for the motors to operate and will only result in a slight performance decrease which will not affect functionality. The Mega2560 provides the ultrasonic sensors with 5 volts. Each ultrasonic sensor draws around 15mA of current. This brings the total current load to around 45mA of current which is well within the Mega2560 voltage regulator output current capability. For the ultrasonic sensor to function, it receives a trigger signal from the microcontroller. Once this trigger signal is received, it releases a sonic pulse. The time it takes to receive an echo of this pulse is measured. The amount of time the echo stays in output high is calculated by the microcontroller which in turn is converted into a distance. The resistive touch LCD takes in 5V and is powered by the 5V output of the L298n motor controller. The LCD draws around 145mA which is within the motor controller's current limit. The display has its RX pin connected to the TX pin of the microcontroller in order to allow for UART serial communication. The microcontroller also sends out signals to control the motor controller. It can enable the motor with the enable pin. Once it's enabled, it can control the direction of the motor depending on the combination of input it gives to the two input pins for each motor. For the user tracking functionality, three ESP8266 wifi modules are used to provide signal strength to the microcontroller. Each of the wifi modules are powered with around 5 volts through a LM7805 linear regulator. The wifi modules each draw around 170mA which makes the total current load on the regulator 510mA. The regulator is rated for 1.5mA which is well within limits.

### Bill of Materials
A bill of materials (Fig. 2) is created to help identify the electronic parts used in the schematic. It also helps with overall organization of the project.

![image](https://user-images.githubusercontent.com/69876697/226158481-194b60aa-c737-4d4b-9ee8-deb9116b42b3.png)

It includes the number of items, component names, quantity, designated part reference and manufacture. These details make it easy to navigate and know what parts are included in the schematic.
### Hardware Testing Code
To verify that the hardware would work, test code was created for each part. In order to test the ultrasonic sensor we gathered three measurements and different distances away and compared the values to measurements made with a measuring tape. For the L298N motor controller, we tested basic motor movement functionality. The code tested different pulse width modulation (PWM) values to ensure the motor controller is powering the motors sufficiently.  The wifi modules are tested for both hotspot connection and serial communication capability. Having the test code is great as a base for building code from and allows us to go back to it to make sure the hardware is functioning properly.

## WiFi Tracking
Since the Arduino Mega2560 does not come with an internal WiFi module, the project uses three ESP8266 modules to set up a triangular network of WiFi modules. This network is responsible for recording and receiving RSSI from the user’s paired hotspot.

The Arduino Mega2560 processes the information from all peripherals and handles the necessary computations. The ESP8266 modules each wait for notifications from the Arduino Mega2560 for login configurations and relay RSSI data back to the main controller.

### WiFi RSSI
Received Signal Strength Indicator (RSSI), as the name says, is a quantitative indication of the connection between the transmitting and receiving Wi-Fi nodes. RSSI values are relative to the device and vary by manufacturer. The RSSI signal strength is a value which generally ranges from 0 to -120. The closer the value is to 0, the more powerful the signal is in the specified range. Collecting RSSI signal strengths at different range intervals is used to determine a range value for the shopping cart [4].

![image](https://user-images.githubusercontent.com/69876697/226158572-50a8d2ee-1ea5-4154-9ac8-97e2b1b0db97.png)

As seen in Fig. 3, the rate of change in distance cannot be represented linearly. Instead, a polynomial regression line is developed with the exact coefficients to determine the distance in meters using the RSSI value. This equation can be modeled using a polynomial regression lambda which will be embedded in the Mega2560 for computation purposes. The distance value will then be assigned for steering and drive instructions for the shopping cart to move forward or backward.

### Serial Communications
Each of the ESP8266 modules communicate with the Arduino Mega2560 through Serial communication. Serial communication requires the receiving and transmitting sides of the two modules to be inverted which allows bitstream data to be sent from different circuits.

![image](https://user-images.githubusercontent.com/69876697/226158822-017db6c6-9c1c-4e67-a46c-c7b2941ab0aa.png)

The above diagram (Fig. 4) depicts the communication wiring schematic between each of the ESP8266 modules and the Mega2560. The ESP8266 modules behave as slaves for the master Mega2560 and wait for login information to be sent through the RX[1-3] channels and attempt connection. Once the connection is successful (user is paired), the ESP slaves intermittently transmit RSSI back to the master through the TX[1-3] channels. The Mega2560 accepts the data and parses the bitstream as a string of RSSI information and calculations take place.

### Tracking the User
The RSSI values transmitted from each of the ESP8266 modules were highly sensitive, due to environmental factors such as noise interference and physical obstructions. This resulted in fluctuating data which made it difficult to ascertain the proper RSSI at a given moment.

![image](https://user-images.githubusercontent.com/69876697/226158849-fbde2875-ec5c-4b16-9616-6c31fad3251a.png)

To circumvent this problem, a queue data structure (Fig. 5) is used for each ESP module. The structure 5 RSSI values sampled at 0.2 second time intervals. The queue allows for new samples to be pushed in while popping the old values out making it ideal for normalizing the instantaneous WiFi RSSI values. Before a new RSSI reading is pushed into the respective queue, the average of the current samples in the specified queue is yielded as the normalized RSSI signal. Each ESP module has a queue which serves as a container for 5 instantaneous RSSI values with 0.2 second time intervals, ultimately resulting in a normalized RSSI value for every 1 second.

![image](https://user-images.githubusercontent.com/69876697/226158856-14198fda-a8eb-4552-bfb2-1dc4a3a0743e.png)

To determine the user’s direction, a simple approach was used using the normalized values for the front, back-left, and back-right ESP8266 modules. The normalized values returned from each of the front, back-left, and back-right RSSI queues is systematically and conditionally checked to get the two-dimensional direction of the user. If the RSSI for the front is less than the back-left and back-right RSSI values, it means that the user is closer to the front, indicating that the shopping cart should move forward, and vice versa. If the front and back RSSI values are below the threshold 40 dB, this indicates that the cart is already at a very close proximity to the user which does not require any motor movement.

# Finite State Machine
