let x;
let y;
let basketHeight;
let basketWidth;
let serial; // the Serial object
let serialOptions = { baudRate: 115200 };
let receivedData;
let speed;

function setup() {
  createCanvas(400, 400);
  basketHeight = 20;
  basketWidth = 60;
  x = (width - basketWidth) / 2;
  y = height - basketHeight;
  speed = 0;
  fill(0);
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
  background(220);
  x += speed;
  x = constrain(x, 0, width - basketWidth);
  rect(x, y, basketWidth, basketHeight);
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
  speed = parseFloat(newData);
}

function mouseClicked() {
  if (!serial.isOpen()) {
    serial.connectAndOpen(null, serialOptions);
  }
}