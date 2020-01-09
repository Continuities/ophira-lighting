const noble = require('@abandonware/noble');
const Feather = require('feather-ble');

const stdin = process.stdin;
stdin.setEncoding('utf-8');

const peripherals = [];

function once(fn) {
  let fired = false;
  return (...args) => {
    if (!fired) {
      fired = true;
      fn(...args);
    }
  }
}

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
    // rssi: true,
    // verbose: true
  });
  feather.on("ready", err => {
    if (err) {
      console.error('Feather failed to initialize', err);
      return;
    }
    feather.sendMessage("SYN");
  });
  
  feather.on("message", once(msg => {
    if (msg === 'ACK') {
      // This is one of ours
      peripherals.push(feather);
      console.log('Feather registered');
    }
  }));
  
  feather.on("rssi", (err, rssi) => {
    if (err) {
      console.error('Error in rssi', err);
      return;
    }
    console.log(`RSSI ${rssi}`);
  });
  
  feather.on("disconnect", () => {
    console.log('Feather disconnected');
    noble.stopScanning();
    setTimeout(() => noble.startScanning(), 500);
    const index = peripherals.indexOf(feather);
    if (index >= 0) {
      // Remove from the broadcast list
      peripherals.splice(index, 1);
    }
  });
  
  feather.setup();
});

stdin.on('data', function (data) {
  // User input exit.
  if(data === 'exit\n') {
    process.exit();
  } 
  else {
    console.log(`[TX] (${peripherals.length} devices) ${data}`);
    peripherals.forEach(p => p.sendMessage(data));
  }
});