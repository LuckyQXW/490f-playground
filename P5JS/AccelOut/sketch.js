let serial; // the Serial object
let serialOptions = { baudRate: 115200 };
let xAccel = 0;
let yAccel = 0;
let zAccel = 0;
const fixedDist = 500;
let receivedData;

function setup() {
  createCanvas(500, 500, WEBGL);

  // Setup Web Serial using serial.js
  serial = new Serial();
  serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
  serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
  serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
  serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);

  // If we have previously approved ports, attempt to connect with them
  serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);
  receivedData = createDiv("Click anywhere to connect to serial port");
}

function draw() {
  background(200);
  let xPos = xAccel * 50;
  let yPos = -yAccel * 50;
  let zPos = Math.sqrt(fixedDist * fixedDist - xPos * xPos - yPos * yPos);
  camera(xPos, yPos, zPos, 0, 0, 0, 0, 1, 0);
  box(100);
}


function onSerialErrorOccurred(eventSender, error) {
  receivedData.html(error);
}

function onSerialConnectionOpened(eventSender) {
  receivedData.html("Serial connection opened successfully");
}

function onSerialConnectionClosed(eventSender) {
  receivedData.html("onSerialConnectionClosed");
}

function onSerialDataReceived(eventSender, newData) {
  receivedData.html("onSerialDataReceived: " + newData);
  let accels = newData.split(",");
  xAccel = parseFloat(accels[0]);
  yAccel = parseFloat(accels[1]);
  zAccel = parseFloat(accels[2]);
}

function mouseClicked() {
  if (!serial.isOpen()) {
    serial.connectAndOpen(null, serialOptions);
  }
}