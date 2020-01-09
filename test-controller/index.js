const noble = require('@abandonware/noble');
const Feather = require('feather-ble');

noble.on('stateChange', state => {
  if (state === 'poweredOn') {
    console.log('Scanning');
    noble.startScanning();
  } else {
    console.log('Stopped scanning');
    noble.stopScanning();
  }
});

noble.on('discover', peripheral => {
  if (!new Feather().isFeather(peripheral)) {
    return;
  }
  console.log('Found a Feather');
  const feather = new Feather({ 
    peripheral,
    verbose: true
  });
  feather.on("ready", err => {
    if (err) {
      console.error('Feather failed to initialize', err);
      return;
    }
    feather.sendMessage("SYN");
  });
  
  feather.on("message", msg => {
    console.log(`Message: ${msg}`);
  });
  
  feather.on("rssi", (err, rssi) => {
    if (err) {
      console.error('Error in rssi', err);
      return;
    }
    console.log(`RSSI ${rssi}`);
  });
  
  feather.on("disconnect", () => {
    console.log('Feather disconnected');
  });
  
  feather.setup();
});