# Ophira Lighting Suite
This repository contains all the code used to build and develop the Ophira interior and exterior lighting infrastructure.

## Structure
* `LayerEngine`

  CPP library for creating lighting patterns by layering effects
* `LightMapper`

  CPP library for mapping x/y coordinates to arrangements of LED strips
* `Network`

  CPP library for communicating between lighting panels using BLE
* `ble-test`
  
  Arduino sketch that uses the Network library to test BLE communication
* `exterior`

  Arduino sketch for the exterior panels
* `interior`
  
   Arduino sketch for the interior panels
* `lighting-simulator`
  
   web-based project for simulating LayerEngine-based designs in a web browser. You can design patterns without actually having an LED panel and an Arduino on hand!
* `test-controller`

   NodeJS application for testing communication with Arduinos running the Network peripheral code
