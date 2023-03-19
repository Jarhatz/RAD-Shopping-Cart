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

## WiFi Tracking
